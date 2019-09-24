#include "pid.h"

#include <string.h>

#include "debug.h"
#include "command.h"

void pid_init(struct pid_control *pid, float dt)
{
	memset(pid, 0, sizeof(*pid));
	pid->dt = dt;
	pid->Kp = 0;
	pid->Ki = 0;
	pid->Kd = 0;

	pid->old_val = 0;
	pid->integral = 0;
}

void pid_update_gains(struct pid_control *pid, float p, float i, float d)
{
	pid->Kp = p;
	pid->Ki = i;
	pid->Kd = d;
}

/* TODO put a cap on integral term */
float pid_update(struct pid_control *pid, float target, float cur)
{
	float error;
	float pterm, iterm, dterm;

	error = target - cur;

	pid->integral += error * pid->dt;

	pterm = pid->Kp * cur;
	iterm = pid->Ki * pid->integral;
	dterm = pid->Kd * (cur - pid->old_val) / pid->dt;

	pid->old_val = cur;
	
	return (pterm + iterm + dterm);
}
