/*
 * mag_AK8963_privatedefs.h
 *
 *  Created on: 07.11.2016
 *      Author: elenz
 */

#ifndef MAG_AK8963_PRIVATEDEFS_H_
#define MAG_AK8963_PRIVATEDEFS_H_

#define AK_I2C_ADDR	0x0C

typedef enum EnAKRegister_
{
	AKREG_WIA = 0x00,
	AKREG_INFO = 0x01,
	AKREG_ST1 = 0x02,
	AKREG_HXL = 0x03,
	AKREG_HXH = 0x04,
	AKREG_HYL = 0x05,
	AKREG_HYH = 0x06,
	AKREG_HZL = 0x07,
	AKREG_HZH = 0x08,
	AKREG_ST2 = 0x09,
	AKREG_CNTL = 0x0A,
	AKREG_CNTL2 = 0x0B,
	AKREG_ASTC = 0x0C,
	AKREG_TS1 = 0x0D,
	AKREG_TS2 = 0x0E,
	AKREG_I2CDIS = 0x0F,
	AKREG_ASAX = 0x10,
	AKREG_ASAY = 0x11,
	AKREG_ASAZ = 0x12,
} EnAKRegister_t;


#define AK_WIA_VALUE	0x48

// CNTL
#define AK_CNTL_MODE_MASK			(15 << 0)
#define AK_CNTL_MODE_PWRDWN			(0 << 0)
#define AK_CNTL_MODE_SINGLE			(1 << 0)
#define AK_CNTL_MODE_CONT_8HZ		(2 << 0)
#define AK_CNTL_MODE_CONT_100HZ		(6 << 0)
#define AK_CNTL_MODE_EXTTRIG		(4 << 0)
#define AK_CNTL_MODE_SELFTEST		(8 << 0)
#define AK_CNTL_MODE_FUSEROMACCESS	(15 << 0)

#define AK_CNTL_OUTBIT_MASK			(1 << 4)
#define AK_CNTL_OUTBIT_14			(0 << 4)
#define AK_CNTL_OUTBIT_16			(1 << 4)


// CNTL2
#define AK_CNTL2_SRST_MASK			(1 << 0)
#define AK_CNTL2_SRST_RESET			(1 << 0)


// ST2
#define AK_ST2_BITM_MASK			(1 << 4)
#define AK_ST2_HOFL_MASK			(1 << 3)
#define AK_ST2_HOFL_OVERFLOWOCCURED (1 << 3)


#endif /* MAG_AK8963_PRIVATEDEFS_H_ */
