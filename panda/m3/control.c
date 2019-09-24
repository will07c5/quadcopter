#include "control.h"

#include "debug.h"
#include "command.h"
#include "angle.h"
#include "vector.h"
#include "sensors.h"
#include "pid.h"
#include "pwm.h"

#include <util.h>
static int mag_counter = 0;

static struct pid_control pid_pitch;
static struct pid_control pid_roll;
static struct pid_control pid_yaw;

static int calibrating = 1;

#define DT (1/100.0f)

static uint16_t throttle = 0;
static float pitch = 0;
static float roll = 0;
static float yaw = 0;
static int armed = 0;

static int count = 0;

#define THROTTLE_MAX 40000
#define THROTTLE_MIN 5000
/* bounds check values before sending them to pwm. return 1 if max value exceeded */
static int set_motor_safe(struct pwm_device *pwm, int val)
{
	int warn_max = 0;

	if (val < THROTTLE_MIN) {
		pwm_set(pwm, 0);
	} else if (val > THROTTLE_MAX) {
		pwm_set(pwm, THROTTLE_MAX);
		warn_max = 1;
	} else {
		pwm_set(pwm, (uint16_t)val);
	}

	return warn_max;
}

static void control_cmd(const uint8_t *buf, uint8_t length)
{
	if (length != 3*sizeof(float) + sizeof(uint16_t)) {
		DEBUG("invalid length for control command\n");
		return;
	}

	pitch = *(const float*)&buf[0];
	roll = *(const float*)&buf[4];
	yaw = *(const float*)&buf[8];

	throttle = *(const uint16_t*)&buf[12];
}

static void tune_cmd(const uint8_t *buf, uint8_t length)
{
	float pitch_p, pitch_i, pitch_d;
	float roll_p, roll_i, roll_d;
	float yaw_p, yaw_i, yaw_d;

	if (length != 9*sizeof(float)) {
		DEBUG("invalid length for tune command\n");
	}

	DEBUG("tuning\n");

	pitch_p = BUF2FLOAT(buf, 0);
	pitch_i = BUF2FLOAT(buf, 1);
	pitch_d = BUF2FLOAT(buf, 2);
	roll_p = BUF2FLOAT(buf, 3);
	roll_i = BUF2FLOAT(buf, 4);
	roll_d = BUF2FLOAT(buf, 5);
	yaw_p = BUF2FLOAT(buf, 6);
	yaw_i = BUF2FLOAT(buf, 7);
	yaw_d = BUF2FLOAT(buf, 8);

	pid_update_gains(&pid_pitch, pitch_p, pitch_i, pitch_d);
	pid_update_gains(&pid_roll, roll_p, roll_i, roll_d);
	pid_update_gains(&pid_yaw, yaw_p, yaw_i, yaw_d);
}

static void arm_cmd(const uint8_t *buf, uint8_t length)
{
	(void)buf;
	(void)length;

	DEBUG("arming\n");

	armed = 1;
}

static void disarm_cmd(const uint8_t *buf, uint8_t length)
{
	(void)buf;
	(void)length;

	DEBUG("disarming\n");
	
	armed = 0;

	set_motor_safe(&PWM1, 0);
	set_motor_safe(&PWM2, 0);
	set_motor_safe(&PWM3, 0);
	set_motor_safe(&PWM4, 0);
}

void control_init(void)
{
	angle_init(DT, 0, 0);

	pid_init(&pid_pitch, DT);
	pid_init(&pid_roll, DT);
	pid_init(&pid_yaw, DT);

	command_register('o', control_cmd);
	command_register('t', tune_cmd);
	command_register('a', arm_cmd);
	command_register('s', disarm_cmd);
}

#if 0
uint32_t bench_read(void);
void bench_reset(void);
#endif

/* X axis/pitch -> motor2,4
 * Y axis/roll -> motor1,3
 */
void control_update(void)
{
	struct sensor_data sd;
	vector3_t angles;
	(void)angles;

	int pitch_out, roll_out, yaw_out;
	int motor1, motor2, motor3, motor4;
	uint16_t avg_motor = throttle;
	int warn_max = 0;
//	uint32_t bench = 0;


	if (calibrating)
		calibrating = sensors_calibrate();

	/* Grab sensor data, only get magnetometer data every 8th read (50Hz) */
//	bench_reset();
	sensors_get_data(&sd, 0 /*!(mag_counter & 4)*/);

	mag_counter++;

	angle_update(&sd.gyro, &sd.accel, &sd.mag);

	angles = angle_get();
//	bench = bench_read();

	pitch_out = (int)pid_update(&pid_pitch, pitch, angles.x);
	roll_out = (int)pid_update(&pid_roll, roll, angles.y);
	yaw_out = 0;//= pid_update(&pid_yaw, angles.z);

	motor2 = avg_motor - pitch_out - yaw_out;
	motor4 = avg_motor + pitch_out - yaw_out;
	motor3 = avg_motor - roll_out + yaw_out;
	motor1 = avg_motor + roll_out + yaw_out;

	if (armed) {
		warn_max |= set_motor_safe(&PWM1, motor1);
		warn_max |= set_motor_safe(&PWM2, motor2);
		warn_max |= set_motor_safe(&PWM3, motor3);
		warn_max |= set_motor_safe(&PWM4, motor4);
	}

	if (warn_max) {
		DEBUG("control: motor max exceeded\n");
	}

	command_process();

	if (command_connected() && (count % 25) == 0) {

//		command_send('0', (uint8_t*)&sd, sizeof(sd));
//		command_send('1', (uint8_t*)&angles, sizeof(angles));
		command_send('2', (uint8_t*)&motor1, sizeof(int));
		command_send('3', (uint8_t*)&motor2, sizeof(int));
		command_send('4', (uint8_t*)&motor3, sizeof(int));
		command_send('5', (uint8_t*)&motor4, sizeof(int));

//		DEBUG("1:\t%hu\t2:\t%hu\t3:\t%hu\t4:%hu\n", motor1, motor2, motor3, motor4);
//	DEBUG("%hd\n", count);
	}


//	DEBUG("%u\n", bench);

	count++;
}
