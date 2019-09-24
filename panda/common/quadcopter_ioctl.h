#ifndef QUADCOPTER_IOCTL_H_
#define QUADCOPTER_IOCTL_H_

#include <linux/ioctl.h>

#define QUADCOPTER_START _IO(10, 0)
#define QUADCOPTER_STOP _IO(10, 1)
#define QUADCOPTER_WRITE_CONFIG _IO(10, 3)

#endif
