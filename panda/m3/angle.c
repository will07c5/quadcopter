#include "angle.h"
#include "command.h"
#include "debug.h"
#include "intr.h"

static quaternion_t q;
static float gyro_bias_gain;
static float error_gain;
#ifdef DRIFT_CORRECTION
static quaternion_t gyro_bias;
#endif
static float dt;
#ifdef MARG
static vector3_t b;
#endif

void update_gains(const uint8_t* buf, uint8_t length)
{
	(void)length;

	float error_gain = *(const float*)buf;
	float gyro_bias_gain = *(const float*)&buf[4];

	DEBUG("update gains: %d %d\n", (int)error_gain, (int)gyro_bias_gain);

	angle_init(dt, gyro_bias_gain, error_gain);
}

void angle_init(float new_dt, float new_gyro_bias_gain, float new_error_gain)
{
	/* XXX due to issues masking the systick interrupt we don't disable interrupts here
	 * this means that this function MUST ONLY be called from within the systick interrupt
	 */
#if 0
	/* we don't want to be running the algorithm while we update gains */
	intr_disable_systick();
#endif

	gyro_bias_gain = new_gyro_bias_gain;
	error_gain = new_error_gain;

	dt = new_dt;
	
	q.q1 = 1;
	q.q2 = 0;
	q.q3 = 0;
	q.q4 = 0;

#ifdef MARG
	b.x = 1;
	b.y = 0;
	b.z = 0;
#endif

#ifdef DRIFT_CORRECTION
	gyro_bias.q1 = 0;
	gyro_bias.q2 = 0;
	gyro_bias.q3 = 0;
	gyro_bias.q4 = 0;
#endif

#if 0
	intr_enable_systick();
#endif
	command_register('u', update_gains);
}

void angle_update(vector3_t *gyro, vector3_t *accel, vector3_t *comp)
{
	(void)comp;

	vector3_t fg;
#ifdef MARG
	vector3_t fb;
	quaternion_t Jbfb;
	vector3_t h;
#endif
	quaternion_t Jgfg;
#ifdef DRIFT_CORRECTION
	quaternion_t q_star;
	quaternion_t gyro_bias_int;
	quaternion_t gyro_bias_scaled;
#endif
	quaternion_t gyro_in;
	quaternion_t grad_f;
	quaternion_t q_int;

//	float q1q1 = q.q1*q.q1;
	float q1q2 = q.q1*q.q2;
	float q1q3 = q.q1*q.q3;
//	float q1q4 = q.q1*q.q4;
	float q2q2 = q.q2*q.q2;
//	float q2q3 = q.q2*q.q3;
	float q2q4 = q.q2*q.q4;
	float q3q3 = q.q3*q.q3;
	float q3q4 = q.q3*q.q4;
//	float q4q4 = q.q4*q.q4;

	/* calculate objective function fg */
	fg.x = 2.0f*(q2q4 - q1q3)       - accel->x;
	fg.y = 2.0f*(q1q2 + q3q4)       - accel->y;
	fg.z = 2.0f*(0.5f - q2q2 - q3q3) - accel->z;

	/* multiply by jacobian Jg to get quaternion */
	Jgfg.q1 = 2.0f*(-fg.x*q.q3 + fg.y*q.q2);
	Jgfg.q2 = 2.0f*( fg.x*q.q4 + fg.y*q.q1 - 2.0f*fg.z*q.q2);
	Jgfg.q3 = 2.0f*(-fg.x*q.q1 + fg.y*q.q4 - 2.0f*fg.z*q.q3);
	Jgfg.q4 = 2.0f*( fg.x*q.q2 + fg.y*q.q3);

#ifdef MARG
	/* calculate objective function fb */
	fb.x = 2.0f*b.x*(0.5f - q3q3 - q4q4) + 2.0f*b.z*(q2q4 - q1q3)       - comp->x;
	fb.y = 2.0f*b.x*(q2q3 - q1q4)       + 2.0f*b.z*(q1q2 + q3q4)       - comp->y;
	fb.z = 2.0f*b.x*(q1q3 + q2q4)       + 2.0f*b.z*(0.5f - q2q2 - q3q3) - comp->z;

	/* multiply by jacobian Jb to get second quaternion */
	Jbfb.q1 = 2.0f*(-fb.x*b.z*q.q3                 + fb.y*(-b.x*q.q4 + b.z*q.q2) + fb.z*b.z*q.q3);
	Jbfb.q2 = 2.0f*( fb.x*b.z*q.q4                 + fb.y*(b.x*q.q3 + b.z*q.q1)  + fb.z*(b.x*q.q4 - b.z*q.q2));
	Jbfb.q3 = 2.0f*(-fb.x*(2.0f*b.x*q.q3 + b.z*q.q1)  + fb.y*(b.x*q.q2 + b.z*q.q4)  + fb.z*(b.x*q.q1 - b.z*q.q3));
	Jbfb.q4 = 2.0f*( fb.x*(-2.0f*b.x*q.q4 + b.z*q.q2) + fb.y*(-b.x*q.q1 + b.z*q.q3) + fb.z*b.x*q.q2);
#endif

	/* finally add them together and normalize to get the normalized gradient of f */
#ifdef MARG
	grad_f = quaternion_add(&Jbfb, &Jgfg);
#else
	grad_f = Jgfg;
#endif
	quaternion_norm(&grad_f);

	/* gyro drift correction */
#ifdef DRIFT_CORRECTION
	q_star = quaternion_conj(&q);
	gyro_bias_int = quaternion_mult(&q_star, &grad_f);
	gyro_bias_int = quaternion_multf(&gyro_bias_int, 2.0f*dt);
	gyro_bias = quaternion_add(&gyro_bias, &gyro_bias_int);
	gyro_bias_scaled = quaternion_multf(&gyro_bias, gyro_bias_gain);
#endif

	gyro_in.q1 = 0;
	gyro_in.q2 = gyro->x;
	gyro_in.q3 = gyro->y;
	gyro_in.q4 = gyro->z;

#ifdef DRIFT_CORRECTION
	gyro_in = quaternion_sub(&gyro_in, &gyro_bias_scaled);
#endif

	gyro_in = quaternion_mult(&q, &gyro_in);
	gyro_in = quaternion_multf(&gyro_in, 0.5f);

	grad_f = quaternion_multf(&grad_f, error_gain);

	q_int = quaternion_sub(&gyro_in, &grad_f);
	q_int = quaternion_multf(&q_int, dt);
	q = quaternion_add(&q, &q_int);
	quaternion_norm(&q);

#ifdef MARG
	/* 3.4 Magnetic distortion compensation */
	h.x = 2*(comp->x*(q1q1 - 0.5f + q2q2) + comp->y*(q2q3 + q1q4)       + comp->z*(q2q4 - q1q3)      );
	h.y = 2*(comp->x*(q2q3 - q1q4)       + comp->y*(q1q1 - 0.5f + q3q3) + comp->z*(q3q4 - q1q2)      );
	h.z = 2*(comp->x*(q2q4 + q1q3)       + comp->y*(q3q4 + q1q2)       + comp->z*(q1q1 - 0.5f + q4q4));
	
	b.x = sqrt(h.x*h.x + h.y*h.y);
	b.z = h.z;
#endif
}

vector3_t angle_get(void)
{
	return quaternion_to_angles(&q);
}
