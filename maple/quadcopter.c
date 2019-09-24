#include "quadcopter.h"

#include <delay.h>
#include <timer.h>
#include <string.h>
#include <gpio.h>

#include "sensors.h"
#include "angle.h"
#include "debug.h"
#include "vector.h"
#include "pwm.h"
#include "command.h"


static uint16 test_count;
static struct sensor_data sd;
static vector3_t angles;
static int mag_counter = 0;

#define STAB_TIMER TIMER2

static void timer_intr(void)
{
	timer_set_count(TIMER6, 0);

	/* Grab sensor data, only get magnetometer data every 8th read (50Hz) */
	sensors_get_data(&sd, !(mag_counter & 4));

	mag_counter++;

#ifdef IMU_PASSTHROUGH
	usb_cdcacm_tx((uint8_t*)&sd, sizeof(sd));
#else

	angle_update(&sd.gyro, &sd.accel, &sd.mag);

	angles = angle_get();
#endif

	test_count = timer_get_count(TIMER6);
}

void motor_off(const uint8_t *data, uint8_t len)
{
	pwm_set(TIMER_CH2, 0);
}

void motor_on(const uint8_t *data, uint8_t len)
{
	pwm_set(TIMER_CH2, 10000);
}



void quadcopter_run(void)
{
	struct sensor_data local_sd;
	vector3_t l_angles;

	pwm_init();


	sensors_power_on();

	/* give time to connect serial */
#ifdef SERIAL_DEBUG
//	debug_wait_connect();
#endif

	debug_printf("Quadcopter starting in 2 seconds.\n");
	delay_us(2000000);
	debug_printf("Starting...\n");

//	command_btconfig();
	command_init();

	command_register('p', motor_on);
	command_register('o', motor_off);

	angle_init(1/400.0, 0, 0);

	sensors_init();

	/* need to wait enough time before first sensor read */
	delay_us(20000);

	gpio_set_mode(ERROR_LED_PORT, ERROR_LED_PIN, GPIO_OUTPUT_PP);

	/* setup profiling timer with tick period 1 us*/
	timer_init(TIMER6);
	timer_set_prescaler(TIMER6, 74);
	timer_resume(TIMER6);

	/* setup timer for stabilization task with
	 * interrupt period 2.5 ms (400 Hz) */
	timer_init(STAB_TIMER);
	timer_pause(STAB_TIMER);
	timer_set_mode(STAB_TIMER, TIMER_CH1, TIMER_OUTPUT_COMPARE);
	timer_set_prescaler(STAB_TIMER, 2);
	timer_set_reload(STAB_TIMER, 60000);
	timer_set_compare(STAB_TIMER, TIMER_CH1, 1);
	timer_attach_interrupt(STAB_TIMER, TIMER_CH1, timer_intr);
	timer_generate_update(STAB_TIMER);
	timer_resume(STAB_TIMER);

	debug_printf("Done with initialization.\n");

	while (1) {
		command_process();

		if (command_connected()) {
			nvic_globalirq_disable();
			memcpy(&local_sd, &sd, sizeof(local_sd));
			memcpy(&l_angles, &angles, sizeof(l_angles));
			nvic_globalirq_enable();

			command_send('0', (uint8_t*)&local_sd, sizeof(local_sd));
			command_send('1', (uint8_t*)&l_angles, sizeof(l_angles));
		}
	}
}
