 *	## Overview
 * 	-----------
 * 	The accel_led application shows how to interface off-shelf components using SDK drivers for I2C.
 * 	In this example the LPC845 Breakout board is interfaced with a MMA8652 accelerometer breakout board.
 * 	The accelerometer acts as an I2C slave device to the LPC845 master device.
 * 	Colors on the on-board RGB LED should change when accelerometer position is changed.
 *
 * 	By default, the I2C timeout is disabled, so the code will continuously attempt to read I2C. A timeout
 * 	can be enabled by defining I2C_WAIT_TIMEOUT in fsl_i2c.h with a (fairly large) non-zero value.
 *
 *	## Toolchain Supported
 *	---------------------
 *	- MCUXpresso IDE v10.3
 *
 *	## Hardware Requirements
 *	------------------------
 *	- Micro USB cable
 *	- LPC845 Breakout board
 *	- MMA8652 break-out board (BRKT-STBC-AGM04) from NXP and its distributors
 *	- External 2.2Kohm pull-up resistors (on SCL and SDA lines)
 *	- Personal Computer
 *
 *	## Board Settings
 *	------------------------
 *	Connect pins of I2C master and slave as below:
 *	MASTER_BOARD        CONNECTS TO         SLAVE_BOARD
 *	Pin Name   Board Location     Pin Name   Board Location
 *	SCL        CN1-23              SCL        J2-3
 *	SDA        CN1-22              SDA        J2-4
 *	VCC		   CN1-40              VDD/VDDIO  J1-3/4
 *	GND        CN1-20              GND        J1-5
