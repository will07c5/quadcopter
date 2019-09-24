#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

typedef struct {
	float x;
	float y;
	float z;
} vector3_t;

typedef struct {
	float q1;
	float q2;
	float q3;
	float q4;
} quaternion_t;


extern float vector3_mag(vector3_t *v);
extern float vector3_dot(vector3_t *a, vector3_t *b);
extern vector3_t vector3_cross(vector3_t *a, vector3_t *b);
extern void vector3_div(vector3_t *v, float f);
extern void vector3_mult(vector3_t *v, float f);
extern void vector3_norm(vector3_t *v);
extern vector3_t vector3_add(vector3_t *a, vector3_t *b);
extern vector3_t vector3_sub(vector3_t *a, vector3_t *b);
extern void vector3_add_eq(vector3_t *sum, vector3_t *b);
extern void vector3_sub_eq(vector3_t *diff, vector3_t *b);

extern quaternion_t quaternion_mult( quaternion_t *a, quaternion_t *b);
extern quaternion_t quaternion_multf(quaternion_t *q, float f);
extern quaternion_t quaternion_add(quaternion_t *a, quaternion_t *b);
extern quaternion_t quaternion_sub(quaternion_t *a, quaternion_t *b);
extern void quaternion_norm(quaternion_t *q);
extern quaternion_t quaternion_conj(quaternion_t *q);
extern vector3_t quaternion_to_angles(quaternion_t *q);

#if 0
quaternion_t quaternion_vrot(quaternion_t *q, vector3_t *v)
{
	quaternion_t res;

	float q_xx = q->x*q->x;
	float q_yy = q->y*q->y;
	float q_zz = q->z*q->z;
	float q_ww = q->w*q->w;
	float q_yz = q->y*q->z;
	float q_xw = q->x*q->w;
	float q_yw = q->y*q->w;
	float q_xz = q->x*q->z;
	float q_zw = q->z*q->w;
	float q_xy = q->x*q->y;

	res.x = v->x*2*(q_xx - 0.5 + q_yy) +
	        v->y*2*(q_yz + q_xw) +
	        v->z*2*(q_yw - q_xz);

	res.y = v->x*2*(q_yz - q_xw) +
	        v->y*2*(q_xx - 0.5 + q_zz) +
	        v->z*2*(q_zw + q_xy);

	res.z = v->x*2*(q_yw + q_xz) +
	        v->y*2*(q_zw + q_xy) +
	        v->z*2*(q_xx - 0.5 + q_ww);

	return res;
}
#endif

#endif

