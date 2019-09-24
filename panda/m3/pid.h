#ifndef PID_H_
#define PID_H_

struct pid_control {
	float Kp;
	float Ki;
	float Kd;

	float old_val;
	float integral;
	float target_val;
	float dt;
};
void pid_init(struct pid_control *pid, float dt);

void pid_update_gains(struct pid_control *pid, float p, float i, float d);

float pid_update(struct pid_control *pid, float target, float cur);

#endif
