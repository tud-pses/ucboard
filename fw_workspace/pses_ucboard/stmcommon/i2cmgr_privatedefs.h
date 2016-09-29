#ifndef I2CMGR_PRIVATEDEFS_H_
#define I2CMGR_PRIVATEDEFS_H_

#include "i2cmgr.h"
#include "stm32f3xx.h"
#include "stm32f3xx_hal_i2c.h"
#include "stdtypes.h"

// ISR register flags
//#define I2C_ISR_BUSY			((uint16_t)0x8000)
//#define I2C_ISR_RESERVED		((uint16_t)0x4000)
//#define I2C_ISR_ALERT			((uint16_t)0x2000)
//#define I2C_ISR_TIMEOUT			((uint16_t)0x1000)
//#define I2C_ISR_PECERR			((uint16_t)0x0800)
//#define I2C_ISR_OVR				((uint16_t)0x0400)
//#define I2C_ISR_ARLO			((uint16_t)0x0200)
//#define I2C_ISR_BERR			((uint16_t)0x0100)
//#define I2C_ISR_TCR				((uint16_t)0x0080)
//#define I2C_ISR_TC				((uint16_t)0x0040)
//#define I2C_ISR1_STOPF			((uint16_t)0x0020)
//#define I2C_ISR_NACKF			((uint16_t)0x0010)
//#define I2C_ISR_ADDR			((uint16_t)0x0008)
//#define I2C_ISR_RXNE			((uint16_t)0x0004)
//#define I2C_ISR_TXIS			((uint16_t)0x0002)
//#define I2C_ISR_TXE				((uint16_t)0x0001)

// CR1 register
//#define I2C_CR1_SWRST			((uint16_t)0x8000)
////#define I2C_CR1_RESERVED2		((uint16_t)0x4000)
////#define I2C_CR1_ALERT			((uint16_t)0x2000)
//#define I2C_CR1_PEC				((uint16_t)0x1000)
//#define I2C_CR1_POS				((uint16_t)0x0800)
//#define I2C_CR1_ACK				((uint16_t)0x0400)
//#define I2C_CR1_STOPGEN			((uint16_t)0x0200)
//#define I2C_CR1_STARTGEN		((uint16_t)0x0100)
////#define I2C_CR1_NOSTRTECH		((uint16_t)0x0080)
////#define I2C_CR1_ENGC			((uint16_t)0x0040)
////#define I2C_CR1_ENPEC			((uint16_t)0x0020)
////#define I2C_CR1_ENARP			((uint16_t)0x0010)
////#define I2C_CR1_SMBTYPE			((uint16_t)0x0008)
////#define I2C_CR1_RESERVED1		((uint16_t)0x0004)
////#define I2C_CR1_SMBUS			((uint16_t)0x0002)
//#define I2C_CR1_PE				((uint16_t)0x0001)

#define I2C_sendStartRestart(b)		((b)->CR1 |= I2C_CR1_STARTGEN)
#define I2C_sendStop(b)				((b)->CR1 |= I2C_CR1_STOPGEN)
#define I2C_sendNoAck(b)			((b)->CR1 &= ~I2C_CR1_ACK)
#define I2C_sendAck(b)				((b)->CR1 |= I2C_CR1_ACK)

#define I2C_isInSlaveMode(b)		( ((b)->SR2 & 0x1) == 0 )

// Aufzählung mit I2C_Events
// (Im wesentlichen entspricht das der Aufzählung der mit ISR
// beschriebenen Events, nur in den beiden "Leerstellen" die
// DMA-Events hineingelegt sind. Damit lassen sich alle für die
// Datenübertragung relevanten Events mit einer Variablen abdecken.)
typedef enum EnI2CEvent
{
	I2CEVENT_NOEVENT = 0,
	I2CEVENT_TXE = I2C_ISR_TXE,
	I2CEVENT_TXIS = I2C_ISR_TXIS,
	I2CEVENT_RXNE = I2C_ISR_RXNE,
	I2CEVENT_ADDR = I2C_ISR_ADDR,
	I2CEVENT_NACKF = I2C_ISR_NACKF,
	I2CEVENT_STOPF = I2C_ISR_STOPF,
	I2CEVENT_TC = I2C_ISR_TC,
	I2CEVENT_TCR = I2C_ISR_TCR,
	I2CEVENT_BERR = I2C_ISR_BERR,
	I2CEVENT_ARLO = I2C_ISR_ARLO,
	I2CEVENT_OVR = I2C_ISR_OVR,
	I2CEVENT_PECERR = I2C_ISR_PECERR,
	I2CEVENT_TIMEOUT = I2C_ISR_TIMEOUT,
	//I2CEVENT_DMAEOT = I2C_SR1_RESERVED1,
	//I2CEVENT_DMAEOT_1 = I2C_SR1_RESERVED2
} EnI2CEvent_t;

#define isI2CEventImpossible(e) (e & ( I2CEVENT_STOPF | I2CEVENT_PECERR ))
#define isI2CEventError(e) (e & (I2CEVENT_BERR | I2CEVENT_ARLO \
												| I2CEVENT_OVR | I2CEVENT_TIMEOUT))

// SR2
//#define I2C_SR2_DUALF                  ((uint32_t)0x00800000)
//#define I2C_SR2_SMBHOST                ((uint32_t)0x00400000)
//#define I2C_SR2_SMBDEFAULT             ((uint32_t)0x00200000)
//#define I2C_SR2_GENCALL                ((uint32_t)0x00100000)
//#define I2C_SR2_TRA                    ((uint32_t)0x00040000)
//#define I2C_SR2_BUSY                   ((uint32_t)0x00020000)
//#define I2C_SR2_MSL                    ((uint32_t)0x00010000)




#define NOCOMM 0xFF


// 
typedef struct I2CMGR_I2C_Pins
{
	GPIO_TypeDef* portSCL;
	uint16_t pinSCL;
	GPIO_TypeDef* portSDL;
	uint16_t pinSDL;
} I2CMGR_I2C_Pins_t;


typedef enum EnI2CommMode
{
	I2CCOMM_IDLE,
	I2CCOMM_ERROR,
	I2CCOMM_TX_HIGHPRIORITYEVENTS,
	I2CCOMM_RX_HIGHPRIORITYEVENTS,
	I2CCOMM_TX_DMA,
	I2CCOMM_RX_DMA
} EnI2CommMode_t;


typedef enum EnI2CState
{
	STATE_IDLE,
	STATE_ERROR,
	STATE_COMMREQ,
	STATE_SBREQ,
	STATE_SBSENT,
	STATE_ADDRSENT,
	STATE_DATATRANSFER,
	STATE_STOPREQ,
	STATE_RESETBUSINIT,
	STATE_RESETBUSERROR
} EnI2CState_t;


typedef struct I2CMGR_Device
{
	bool bInit;
	EnI2C_PORT_t eI2CPort;
	uint8_t uAddress;
	EnI2C_MsgState_t eMsgState; 
	EnI2CMgrRes_t eMsgRes;
	uint8_t nMsgs;
	uint8_t uCurMsg;
	I2CMGR_Msg_t* pMsgs;
} I2CMGR_Device_t;


typedef enum EnBusResetState
{
	BUSRESETSTATE_NORESET,
	BUSRESETSTATE_INIT,
	BUSRESETSTATE_START1,
	BUSRESETSTATE_START2,
	BUSRESETSTATE_TOGGLE,
	BUSRESETSTATE_STOP
} EnBusResetState_t;


typedef struct BusResetData
{
	EnBusResetState_t eState;
	uint32_t uTic;
	uint8_t uToggleCount;
} BusResetData_t;


typedef struct I2CMGR_I2C
{
	I2C_TypeDef* pI2C;
	bool bInit;
	EnI2CState_t state;
	EnI2CMgrRes_t eCurMsgRes;
	I2C_InitTypeDef stConfig;
	I2CMGR_Msg_t* pCurMsg;
	uint8_t uCurMsgDevice;
	void (*stateMachine) (EnI2C_PORT_t, EnI2CEvent_t);
	BusResetData_t stBusResetData;
} I2CMGR_I2C_t;



#endif /* I2CMGR_PRIVATEDEFS_H_ */
