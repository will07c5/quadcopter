#include "vector.h"

float vector3_mag(vector3_t *v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

float vector3_dot(vector3_t *a, vector3_t *b)
{
	return a->x*b->x + a->y*b->y + a->z*b->z;
}

vector3_t vector3_cross(vector3_t *a, vector3_t *b)
{
	vector3_t prod;

	prod.x = a->y*b->z - a->z*b->y;
	prod.y = a->z*b->x - a->x*b->z;
	prod.z = a->z*b->y - a->y*b->z;

	return prod;
}

void vector3_div(vector3_t *v, float f)
{
	v->x /= f;
	v->y /= f;
	v->z /= f;
}

void vector3_mult(vector3_t *v, float f)
{
	v->x *= f;
	v->y *= f;
	v->z *= f;
}

void vector3_norm(vector3_t *v)
{
	float norm = vector3_mag(v);
	vector3_div(v, norm);
}

vector3_t vector3_add(vector3_t *a, vector3_t *b)
{
	vector3_t sum;
	sum.x = a->x + b->x;
	sum.y = a->y + b->y;
	sum.z = a->z + b->z;
	
	return sum;
}

vector3_t vector3_sub(vector3_t *a, vector3_t *b)
{
	vector3_t diff;
	diff.x = a->x - b->x;
	diff.y = a->y - b->y;
	diff.z = a->z - b->z;
	
	return diff;
}

void vector3_add_eq(vector3_t *sum, vector3_t *b)
{
	sum->x += b->x;
	sum->y += b->y;
	sum->z += b->z;
}

void vector3_sub_eq(vector3_t *diff, vector3_t *b)
{
	diff->x -= b->x;
	diff->y -= b->y;
	diff->z -= b->z;
}

quaternion_t quaternion_mult( quaternion_t *a, quaternion_t *b)
{
	quaternion_t prod;

	prod.q1 = a->q1*b->q1 - a->q2*b->q2 - a->q3*b->q3 - a->q4*b->q4;
	prod.q2 = a->q1*b->q2 + a->q2*b->q1 + a->q3*b->q4 - a->q4*b->q3;
	prod.q3 = a->q1*b->q3 - a->q2*b->q4 + a->q3*b->q1 + a->q4*b->q2;
	prod.q4 = a->q1*b->q4 + a->q2*b->q3 - a->q3*b->q2 + a->q4*b->q1;

	return prod;
}

quaternion_t quaternion_multf(quaternion_t *q, float f)
{
	quaternion_t prod;

	prod.q1 = q->q1*f;
	prod.q2 = q->q2*f;
	prod.q3 = q->q3*f;
	prod.q4 = q->q4*f;

	return prod;
}

quaternion_t quaternion_add(quaternion_t *a, quaternion_t *b)
{
	quaternion_t sum;

	sum.q1 = a->q1 + b->q1;
	sum.q2 = a->q2 + b->q2;
	sum.q3 = a->q3 + b->q3;
	sum.q4 = a->q4 + b->q4;

	return sum;
}

quaternion_t quaternion_sub(quaternion_t *a, quaternion_t *b)
{
	quaternion_t diff;

	diff.q1 = a->q1 - b->q1;
	diff.q2 = a->q2 - b->q2;
	diff.q3 = a->q3 - b->q3;
	diff.q4 = a->q4 - b->q4;

	return diff;
}

void quaternion_norm(quaternion_t *q)
{
	float norm = sqrt(q->q1*q->q1 + q->q2*q->q2 + q->q3*q->q3 + q->q4*q->q4);

	q->q1 /= norm;
	q->q2 /= norm;
	q->q3 /= norm;
	q->q4 /= norm;
}

quaternion_t quaternion_conj(quaternion_t *q)
{
	quaternion_t conj;

	conj.q1 = q->q1;
	conj.q2 = -q->q2;
	conj.q3 = -q->q3;
	conj.q4 = -q->q4;

	return conj;
}

vector3_t quaternion_to_angles(quaternion_t *q)
{
	vector3_t v;

	v.x = atan2(2*(q->q1*q->q2 + q->q3*q->q4), 1 - 2*(q->q2*q->q2 + q->q3*q->q3));
	v.y = asin(2*(q->q1*q->q3 - q->q4*q->q2));
	v.z = atan2(2*(q->q1*q->q4 + q->q2*q->q3), 1 - 2*(q->q3*q->q3 + q->q4*q->q4));

	return v;
}

