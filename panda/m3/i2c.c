#include "i2c.h"

#include "debug.h"

#include <util.h>

/* Base addresses */
//#define I2C1_BASEADDR       0x48070000
//#define I2C2_BASEADDR       0x48072000
//#define I2C3_BASEADDR       0x48060000
#define I2C4_BASEADDR       0x48350000

/* Register offsets from base address */
#define I2C_REVNB_LO        (I2C4_BASEADDR + 0x00)
#define I2C_REVNB_HI        (I2C4_BASEADDR + 0x04)
#define I2C_SYSC            (I2C4_BASEADDR + 0x10)
#define I2C_IRQSTATUS_RAW   (I2C4_BASEADDR + 0x24)
#define I2C_IRQSTATUS       (I2C4_BASEADDR + 0x28)
#define I2C_IRQENABLE_SET   (I2C4_BASEADDR + 0x2C)
#define I2C_IRQENABLE_CLR   (I2C4_BASEADDR + 0x30)
#define I2C_WE              (I2C4_BASEADDR + 0x34)
#define I2C_DMARXENABLE_SET (I2C4_BASEADDR + 0x38)
#define I2C_DMATXENABLE_SET (I2C4_BASEADDR + 0x3C)
#define I2C_DMARXENABLE_CLR (I2C4_BASEADDR + 0x40)
#define I2C_DMATXENABLE_CLR (I2C4_BASEADDR + 0x44)
#define I2C_DMARXWAKE_EN    (I2C4_BASEADDR + 0x48)
#define I2C_DMATXWAKE_EN    (I2C4_BASEADDR + 0x4C)
#define I2C_IE              (I2C4_BASEADDR + 0x84)
#define I2C_STAT            (I2C4_BASEADDR + 0x88)
#define I2C_SYSS            (I2C4_BASEADDR + 0x90)
#define I2C_BUF             (I2C4_BASEADDR + 0x94)
#define I2C_CNT             (I2C4_BASEADDR + 0x98)
#define I2C_DATA            (I2C4_BASEADDR + 0x9C)
#define I2C_CON             (I2C4_BASEADDR + 0xA4)
#define I2C_OA              (I2C4_BASEADDR + 0xA8)
#define I2C_SA              (I2C4_BASEADDR + 0xAC)
#define I2C_PSC             (I2C4_BASEADDR + 0xB0)
#define I2C_SCLL            (I2C4_BASEADDR + 0xB4)
#define I2C_SCLH            (I2C4_BASEADDR + 0xB8)
#define I2C_SYSTEST         (I2C4_BASEADDR + 0xBC)
#define I2C_BUFSTAT         (I2C4_BASEADDR + 0xC0)
#define I2C_OA1             (I2C4_BASEADDR + 0xC4)
#define I2C_OA2             (I2C4_BASEADDR + 0xC8)
#define I2C_OA3             (I2C4_BASEADDR + 0xCC)
#define I2C_ACTOA           (I2C4_BASEADDR + 0xD0)
#define I2C_SBLOCK          (I2C4_BASEADDR + 0xD4)

/* I2C_CON bits */
#define CON_I2C_EN (1 << 15)
#define CON_MST (1 << 10)
#define CON_TRX (1 << 9)
#define CON_STP (1 << 1)
#define CON_STT (1 << 0)

/* I2C_SYSC bits */
#define SYSC_SRST (1 << 1)

/* I2C_SYSS bits */
#define SYSS_RDONE (1 << 0)

/* I2C_STAT bits */
#define STAT_BB (1 << 12)
#define STAT_ROVR (1 << 11)
#define STAT_XUDF (1 << 10)
#define STAT_XRDY (1 << 4)
#define STAT_RRDY (1 << 3)
#define STAT_ARDY (1 << 2)
#define STAT_NACK (1 << 1)
#define STAT_XDR (1 << 14)
#define STAT_RDR (1 << 13)
#define STAT_AL (1 << 0)

/* I2C_BUF bits */
#define BUF_TXTRSH_MASK 0x003F
#define BUF_RXTRSH_MASK 0x3F00
#define BUF_RXTRSH_SHIFT 8
#define BUF_RXFIFO_CLR (1 << 14)
#define BUF_TXFIFO_CLR (1 << 6)

/* I2C_BUFSTAT bits */
#define BUFSTAT_TXSTAT_MASK 0x003F
#define BUFSTAT_RXSTAT_MASK 0x3F00
#define BUFSTAT_RXSTAT_SHIFT 8

#define I2C_FIFO_SIZE 64

#define I2C_DEBUG(...) //DEBUG(__VA_ARGS__)

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

	/* set fifo sizes and clear fifos */
	REG16(I2C_BUF) = BUF_RXFIFO_CLR | ((I2C_FIFO_SIZE - 1) << BUF_RXTRSH_SHIFT) |
	                 BUF_TXFIFO_CLR | (I2C_FIFO_SIZE - 1);
}

static void do_reset(void)
{
	REG16(I2C_CON) = 0; // disable i2c
	REG16(I2C_SYSC) = SYSC_SRST; // set reset
	REG16(I2C_CON) = CON_I2C_EN; // enable i2c
	
	while (!(REG16(I2C_SYSS) & SYSS_RDONE));

	REG16(I2C_CON) = 0;
}

void i2c_init(i2c_speed speed)
{
	debug_printf("i2c: Initializing (baseaddr: %x, speed: %d)\n", I2C4_BASEADDR, speed);


	debug_printf("i2c: Doing reset\n");

	do_reset();

	/*if (REG16(I2C_STAT) & STAT_BB) {
		DEBUG("i2c: bus is busy when it shouldn't be, attempt to fix\n");
		w = REG16(I2C_SYSTEST);
		REG16(I2C_CON) = CON_I2C_EN;
		REG16(I2C_SYSTEST) = (OMAP_I2C_SYSTEST_ST_EN | OMAP_I2C_SYSTEST_TMODE_TEST);
		msleep(1); // ?
		REG16(I2C_SYSTEST) = w;
		do_reset();
	}*/

	debug_printf("i2c: Configuring\n");

	set_speed(speed);

	// Enable i2c module
	REG16(I2C_CON) = CON_I2C_EN;

	debug_printf("i2c: Done\n");
}

int i2c_tx_rx(uint8_t addr, uint8_t *buf, int count, int tx)
{
	uint16_t stat, amt;
	unsigned int i;
	unsigned int pos = 0;

	I2C_DEBUG("i2c: beginning i2c tx/rx %hhx %d %d\n", addr, count, tx);

	/* wait until bus is free */
	while (REG16(I2C_STAT) & STAT_BB);

	REG16(I2C_SA) = addr;
	REG16(I2C_CNT) = count;
	REG16(I2C_BUF) |= BUF_RXFIFO_CLR | BUF_TXFIFO_CLR;
	
	if (tx)
		REG16(I2C_CON) = CON_I2C_EN | CON_MST | CON_TRX | CON_STP | CON_STT;
	else
		REG16(I2C_CON) = CON_I2C_EN | CON_MST | CON_STP | CON_STT;

	while (1) {
		stat = REG16(I2C_STAT);

		if (stat & STAT_NACK) {
			DEBUG("i2c: got NACK :(\n");
			break;
		} else if (stat & STAT_AL) {
			DEBUG("i2c: arbitration lost\n");
			break;

		} else if (stat & STAT_XDR) {
			amt = REG16(I2C_BUFSTAT) & BUFSTAT_TXSTAT_MASK;
			
			for (i = 0; i < amt; i++) {
				REG16(I2C_DATA) = buf[pos++];
			}

			REG16(I2C_STAT) = STAT_XDR;
			
			I2C_DEBUG("i2c: transmitted (drain) %d bytes\n", count);
		} else if (stat & STAT_RDR) {
			amt = (REG16(I2C_BUFSTAT) & BUFSTAT_RXSTAT_MASK) >> BUFSTAT_RXSTAT_SHIFT;
			
			for (i = 0; i < amt; i++) {
				buf[pos++] = REG16(I2C_DATA);
			}

			REG16(I2C_STAT) = STAT_RDR;

			I2C_DEBUG("i2c: received (drain) %d bytes\n", count);
		} else if (stat & STAT_ARDY) {
			I2C_DEBUG("i2c: done\n");
			break;
		} else if (stat & STAT_XRDY) {
			for (i = 0; i < I2C_FIFO_SIZE; i++) {
				REG16(I2C_DATA) = buf[pos++];
			}
			
			REG16(I2C_STAT) = STAT_XRDY;
			
			I2C_DEBUG("i2c: transmitted %d bytes\n", count);
		} else if (stat & STAT_RRDY) {
			for (i = 0; i < I2C_FIFO_SIZE; i++) {
				buf[pos++] = REG16(I2C_DATA);
			}
			
			REG16(I2C_STAT) = STAT_RRDY;
			
			I2C_DEBUG("i2c: received %d bytes\n", count);
		} else if (stat & STAT_ROVR) {
			// receive overflow
			DEBUG("i2c: receive overflow\n");
			break;
		} else if (stat & STAT_XUDF) {
			// transmit underflow
			DEBUG("i2c: trasnmit underflow\n");
			break;
		}
	}

	I2C_DEBUG("i2c: final status %x\n", stat);

	/* clear status bits */
	REG16(I2C_STAT) = stat;

	if (stat & (STAT_NACK | STAT_AL | STAT_ROVR | STAT_XUDF))
		return -1;
	else
		return 0;
}

int i2c_read_regs(uint8_t addr, uint8_t reg, uint8_t *buf, int count)
{
	int status;

	status = i2c_tx_rx(addr, &reg, 1, 1);

	if (status < 0)
		return status;

	return i2c_tx_rx(addr, buf, count, 0);
}

int i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val)
{
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = val;

	return i2c_tx_rx(addr, buf, 2, 1);
}
