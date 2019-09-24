#include "i2c.h"

#include <xdc/runtime/System.h>

/* Base addresses */
#define I2C1_BASEADDR       0xA8070000
#define I2C2_BASEADDR       0xA8072000
#define I2C3_BASEADDR       0xA8060000
#define I2C4_BASEADDR       0xA8350000

/* Register offsets from base address */
#define I2C_REVNB_LO        0x00
#define I2C_REVNB_HI        0x04
#define I2C_SYSC            0x10
#define I2C_IRQSTATUS_RAW   0x24
#define I2C_IRQSTATUS       0x28
#define I2C_IRQENABLE_SET   0x2C
#define I2C_IRQENABLE_CLR   0x30
#define I2C_WE              0x34
#define I2C_DMARXENABLE_SET 0x38
#define I2C_DMATXENABLE_SET 0x3C
#define I2C_DMARXENABLE_CLR 0x40
#define I2C_DMATXENABLE_CLR 0x44
#define I2C_DMARXWAKE_EN    0x48
#define I2C_DMATXWAKE_EN    0x4C
#define I2C_IE              0x84
#define I2C_STAT            0x88
#define I2C_SYSS            0x90
#define I2C_BUF             0x94
#define I2C_CNT             0x98
#define I2C_DATA            0x9C
#define I2C_CON             0xA4
#define I2C_OA              0xA8
#define I2C_SA              0xAC
#define I2C_PSC             0xB0
#define I2C_SCLL            0xB4
#define I2C_SCLH            0xB8
#define I2C_SYSTEST         0xBC
#define I2C_BUFSTAT         0xC0
#define I2C_OA1             0xC4
#define I2C_OA2             0xC8
#define I2C_OA3             0xCC
#define I2C_ACTOA           0xD0
#define I2C_SBLOCK          0xD4

// I2C_CON bits
#define I2C_EN (1 << 15)
#define MST (1 << 10)
#define TRX (1 << 9)

// I2C_SYSC bits
#define SRST (1 << 1)

// I2C_SYSS bits
#define RDONE (1 << 0)

static uintptr_t baseaddr = 0;

#define REG16(offset) *(volatile uint16_t*)(baseaddr + (offset))
#define REG8(offset) *(volatile uint8_t*)(baseaddr + (offset))

static void set_baseaddr(i2c_bus bus)
{
	switch (bus) {
	case I2C1: baseaddr = I2C1_BASEADDR; break;
	case I2C2: baseaddr = I2C2_BASEADDR; break;
	case I2C3: baseaddr = I2C3_BASEADDR; break;
	case I2C4: baseaddr = I2C4_BASEADDR; break;
	default:
	// TODO panic or something
		break;
	}
}

static void set_speed(i2c_speed speed)
{
	uint16_t scll = 0;
	uint16_t sclh = 0;
	uint16_t psc = 0;

	/* These values are from Table 23-9 of the OMAP44xx TRM vO */
	switch (speed) {
	case I2C_SPEED_STANDARD:
		psc = 23;
		scll = 13;
		sclh = 15;
		break;
	case I2C_SPEED_FAST:
		psc = 9;
		scll = 5;
		sclh = 7;
		break;
	case I2C_SPEED_FAST_PLUS:
		psc = 5;
		scll = 3;
		sclh = 4;
		break;
	default:
	// TODO panic or something
		break;
	}

	REG16(I2C_PSC) = psc;
	REG16(I2C_SCLL) = scll;
	REG16(I2C_SCLH) = sclh;
}

static void do_reset(void)
{
	REG16(I2C_CON) = 0; // disable i2c
	REG16(I2C_SYSC) = SRST; // set reset
	REG16(I2C_CON) = I2C_EN; // enable i2c
	
	while (!(REG16(I2C_SYSS) & RDONE));

	REG16(I2C_CON) = 0;
}

void i2c_init(i2c_bus bus, i2c_speed speed)
{
	System_printf("i2c: Initializing (bus: %d, speed: %d)\n", bus, speed);

	set_baseaddr(bus);

	System_printf("i2c: Doing reset\n");

	do_reset();

	System_printf("i2c: Configuring\n");

	set_speed(speed);

	// Enable i2c and set as master and transmitter
	REG16(I2C_CON) = I2C_EN | MST | TRX;

	System_printf("i2c: Done\n");
}
