#ifndef ANGLE_H_
#define ANGLE_H_

#include "vector.h"

extern void angle_init(float new_dt, float new_gyro_bias_gain, float new_error_gain);

extern void angle_update(vector3_t *gyro, vector3_t *accel, vector3_t *comp);

extern vector3_t angle_get(void);

#endif /* ANGLE_H_ */
