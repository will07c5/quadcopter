#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>

#include "vector.h"


struct sensor_data
{
	vector3_t gyro;
	vector3_t accel;
	vector3_t mag;
};

extern int sensors_get_data(struct sensor_data *data, int update_mag);
extern int sensors_init(void);
extern int sensors_calibrate(void);

#endif
