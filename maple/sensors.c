#include "sensors.h"

#include "debug.h"
#include "vector.h"

#include <i2c.h>
#include <string.h>

#define SENSOR_STICK_VCC_PIN 13
#define SENSOR_STICK_VCC_DEV GPIOB
#define SENSOR_STICK_GND_PIN 12
#define SENSOR_STICK_GND_DEV GPIOB

#define BAROMETER_VCC_PIN 8
#define BAROMETER_VCC_DEV GPIOG
#define BAROMETER_GND_PIN 7
#define BAROMETER_GND_DEV GPIOG

#define ACCEL_ADDR 0x53
#define MAG_ADDR   0x1E
#define GYRO_ADDR  0x68

vector3_t gyro_offset;
vector3_t accel_offset;

#define SENSORS_GYRO_SAMPLES 50
#define SENSORS_ACCEL_SAMPLES 50

static int read_reg(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *buf, int length)
{
	i2c_msg msg[2];
	int32_t status;

	bzero(&msg, sizeof(msg));

	msg[0].addr = addr;
	msg[0].length = 1;
	msg[0].data = &reg;

	msg[1].addr = addr;
	msg[1].flags = I2C_MSG_READ;
	msg[1].length = length;
	msg[1].data = buf;

	status = i2c_master_xfer(dev, msg, 2, 1);

	if (status != 0) {
		DEBUG("I2C Error: %d\n", status);
		return -1;
	}

	return 0;
}

static int write_reg(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t data)
{
	i2c_msg msg;
	int32_t status;
	uint8_t buf[2];

	bzero(&msg, sizeof(msg));

	buf[0] = reg;
	buf[1] = data;

	msg.addr = addr;
	msg.length = 2;
	msg.data = buf;

	status = i2c_master_xfer(dev, &msg, 1, 1);

	if (status != 0) {
		DEBUG("I2C Error: %d\n", status);
		return -1;
	}

	return 0;
}

static int get_gyro_data(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t buf[6];

	if (read_reg(I2C2, GYRO_ADDR, 0x1D, buf, 6)) {
		DEBUG("Error reading gyro data\n");
		return -1;
	}

	*x = (int16_t)((buf[0] << 8) | buf[1]);
	*y = -(int16_t)((buf[2] << 8) | buf[3]);
	*z = -(int16_t)((buf[4] << 8) | buf[5]);

	return 0;
}

static int get_accel_data(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t buf[6];

	if (read_reg(I2C2, ACCEL_ADDR, 0x32, buf, 6)) {
		DEBUG("Error reading accel data\n");
		return -1;
	}

	/* Note: X is marked wrong on the sensor stick 
	 * so we correct it here for convenience */
	*x = -(int16_t)((buf[1] << 8) | buf[0]);
	*y = (int16_t)((buf[3] << 8) | buf[2]);
	*z = (int16_t)((buf[5] << 8) | buf[4]);

	return 0;
}

static int get_mag_data(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t buf[6];

	if (read_reg(I2C2, MAG_ADDR, 0x03, buf, 6)) {
		DEBUG("Error reading magnetometer data\n");
		return -1;
	}

	*x = (int16_t)((buf[2] << 8) | buf[3]);
	*y = (int16_t)((buf[0] << 8) | buf[1]);
	*z = (int16_t)((buf[4] << 8) | buf[5]);

	return 0;
}

/* calculate zero for gyro */
static void calibrate_gyro(void)
{
	int i;
	int16_t x, y, z;

	gyro_offset.x = 0;
	gyro_offset.y = 0;
	gyro_offset.z = 0;

	/* this is sort of a hack but it should work */
	for (i = 0; i < SENSORS_GYRO_SAMPLES; i++) {
		delay_us(2500);
		get_gyro_data(&x, &y, &z);
		
		gyro_offset.x += x;
		gyro_offset.y += y;
		gyro_offset.z += z;
	}

	gyro_offset.x /= (float)SENSORS_GYRO_SAMPLES;
	gyro_offset.y /= (float)SENSORS_GYRO_SAMPLES;
	gyro_offset.z /= (float)SENSORS_GYRO_SAMPLES;
}

static void calibrate_accel(void)
{
	int i;
	int16_t x, y, z;

	accel_offset.x = 0;
	accel_offset.y = 0;
	accel_offset.z = 0;

	/* this is sort of a hack but it should work */
	for (i = 0; i < SENSORS_ACCEL_SAMPLES; i++) {
		delay_us(2500);
		get_accel_data(&x, &y, &z);
		
		accel_offset.x += x;
		accel_offset.y += y;
	}

	accel_offset.x /= (float)SENSORS_ACCEL_SAMPLES;
	accel_offset.y /= (float)SENSORS_ACCEL_SAMPLES;
}



void sensors_power_on(void)
{
	gpio_set_mode(SENSOR_STICK_VCC_DEV, SENSOR_STICK_VCC_PIN, GPIO_OUTPUT_PP);
	gpio_set_mode(SENSOR_STICK_GND_DEV, SENSOR_STICK_GND_PIN, GPIO_OUTPUT_PP);
	gpio_set_mode(BAROMETER_VCC_DEV, BAROMETER_VCC_PIN, GPIO_OUTPUT_PP);
	gpio_set_mode(BAROMETER_GND_DEV, BAROMETER_GND_PIN, GPIO_OUTPUT_PP);

	gpio_write_bit(SENSOR_STICK_VCC_DEV, SENSOR_STICK_VCC_PIN, 1);
	gpio_write_bit(SENSOR_STICK_GND_DEV, SENSOR_STICK_GND_PIN, 0);
	gpio_write_bit(BAROMETER_VCC_DEV, BAROMETER_VCC_PIN, 1);
	gpio_write_bit(BAROMETER_GND_DEV, BAROMETER_GND_PIN, 0);
}

int sensors_init(void)
{
	uint8_t buf[3];

	i2c_init(I2C2);

	i2c_master_enable(I2C2, I2C_FAST_MODE);

	if (read_reg(I2C2, ACCEL_ADDR, 0x00, buf, 1)) {
		DEBUG("Failed to get accelerometer deviceid register\n");
	}
	
	DEBUG("Accelerometer id = %hhx\n", buf[0]);
	
	/* BW_RATE: Set rate to 400Hz */
	write_reg(I2C2, ACCEL_ADDR, 0x2C, 0x0C);
	/* DATA_FORMAT: Set full resolution and +/-16 g range */
	write_reg(I2C2, ACCEL_ADDR, 0x31, 0x0B);
	/* POWER_CTL: Turn on measurement */
	write_reg(I2C2, ACCEL_ADDR, 0x2D, 0x08);

	if (read_reg(I2C2, MAG_ADDR, 0x0A, buf, 3)) {
		DEBUG("Failed to get magnetometer Id Reg\n");
	}

	DEBUG("Magnetometer id = %c%c%c\n", buf[0], buf[1], buf[2]);

	/* Configuration Register A: Set rate to 50Hz */
	write_reg(I2C2, MAG_ADDR, 0x00, 0x18);
	/* Mode Register: Turn on continuous conversion */
	write_reg(I2C2, MAG_ADDR, 0x02, 0x00);

	if (read_reg(I2C2, GYRO_ADDR, 0x00, buf, 1)) {
		DEBUG("Failed to get gyroscope WHO_AM_I register\n");
	}

	DEBUG("Gyroscope id = %hhx\n", buf[0]);

	/* SMPLRT_DIV: Set sample rate divider to get 400Hz */
	write_reg(I2C2, GYRO_ADDR, 0x15, 0x13);
	/* DLPF_FS: Set bits according to datasheet */
	write_reg(I2C2, GYRO_ADDR, 0x16, 0x18);
	/* PWR_MGM: Set clock source to "PLL with X Gyro reference."
	 * NOTE - datasheet recommends setting clock source to a
	 * gyro reference. I guess which one is arbitrary? */
	write_reg(I2C2, GYRO_ADDR, 0x3E, 0x01);

	calibrate_gyro();
	calibrate_accel();

	return 0;
}

int sensors_get_data(struct sensor_data *data, int update_mag)
{
	/* all coordinates are in terms of the accelerometer markings on the
	 * sensor stick. */

	int16_t gyro_x, gyro_y, gyro_z;
	int16_t accel_x, accel_y, accel_z;
	int16_t mag_x, mag_y, mag_z;

	if (get_gyro_data(&gyro_x, &gyro_y, &gyro_z) < 0)
		return -1;

	data->gyro.x = -gyro_x;
	data->gyro.y = -gyro_y;
	data->gyro.z = -gyro_z;
	vector3_sub_eq(&data->gyro, &gyro_offset);
	vector3_mult(&data->gyro, 0.00121414209f); /* scale to rad/s */

	if (get_accel_data(&accel_x, &accel_y, &accel_z) < 0)
		return -1;

	data->accel.x = accel_x;
	data->accel.y = accel_y;
	data->accel.z = -accel_z;
	vector3_sub_eq(&data->accel, &accel_offset);
	vector3_norm(&data->accel);

	if (update_mag) {
		if (get_mag_data(&mag_x, &mag_y, &mag_z) < 0)
			return -1;

		data->mag.x = mag_x;
		data->mag.y = mag_y;
		data->mag.z = -mag_z;
		vector3_norm(&data->mag);
	}

	return 0;
}
