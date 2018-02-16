#ifndef I2CMGR_H_
#define I2CMGR_H_

/*
 * In "i2cmgr" ist eine Verwaltung f�r den I2C-Bus implementiert.
 *
 * Diese Verwaltung �bernimmt die Initialisierung der Busse und die �bertragung
 * von Nachrichten (Messages) zu und von Ger�ten (Devices), die an die Busse
 * angeschlossen sind.
 * Unter Nachricht ist einfach die �bertragung einer bestimmten Anzahl von Bytes
 * zu einem Ger�t (TX) oder von einem Ger�t (RX) zu verstehen.
 * Die Nachrichten werden dabei grunds�tzlich asynchron �bermittelt, d.h. dass
 * mit den unten genannten Funktionen die Nachrichten zun�chst nur an die
 * Verwaltung �bergeben und die Funktionen dann gleich wieder beendet werden, ohne
 * dass auf die �bertragung der Nachrichten gewartet wird.
 * Somit muss man auch bei mehreren Ger�ten an einem Bus nicht darauf achten, dass
 * diese sich den Bus gegenseitig "gerecht" teilen, da dies von i2cmgr �bernommen
 * wird.
 *
 * Fehlerbehandlung:
 * - Wenn das angesprochene Ger�t kein Acknowledge sendet, so wird ein Stop-Bit
 * gesendet.
 * - In allen anderen F�llen wird der Bus resetet. D.h. es wird "manuell" ein
 * Null-Byte (inklusive Start- und Stop-Bit) auf den Bus geschrieben.
 * In beiden F�llen wird die �bertragung aller Nachrichten zu dem betroffenden
 * Ger�t abgebrochen und der Status auf I2CMSGSTATE_ERROR gesetzt. Dann wird mit
 * ggfs. vorhandenen Nachrichten anderer Ger�te weitergemacht.
 *
 *
 * Die verwalteten Busse sowie deren Pin-Konfigurationen werden in i2cmgr.c
 * gew�hlt. D.h. es m�ssen nicht alle Busse des uC �ber i2cmgr verwaltet werden.
 *
 * Die Verwaltung muss mit dem Aufruf von
 *		i2cmgr_init
 * initialisiert werden.
 * Die Ger�te werden �ber
 *		i2cmgr_addDevice(...)
 * hinzugef�gt, wobei angegeben wird, an welchen Bus das Ger�t angeschlossen ist,
 * welche Bus-Konfiguration ben�tigt ist und welche Adresse das Ger�t hat. Dabei
 * sind nur 7bit-Adressen zul�ssig, wobei diese schon um eins nach links geshiftet
 * anzugeben sind.
 * Bei der (erfolgreichen) Anmeldung erh�lt jedes Ger�t eine eindeutige ID, durch
 * die es im Weiteren angesprochen wird.
 *
 * Die Nachrichten, die ein Ger�t verschicken und empfangen soll, werden �ber
 *		i2cmgr_enqueueAsynchMsgs(...)
 * in die Liste der abzuarbeitenden Nachrichten eingereiht. Der genannten Funktion
 * wird ein Zeiger auf ein Array von I2CMGR_Msg_t-Elementen �bergeben. Diese
 * Struktur hat einen f�r die sp�tere Verwendung hin ausgelegten und weniger einen
 * intuitiven Aufbau. Daher wird auch die Funktion
 *		i2cmgr_setupMsgStruct(...)
 * zur Verf�gung gestellt, die aus den Angaben Senden oder Empfangen, Beginn des
 * Buffers und Anzahl der zu �bertragenden Bytes eine I2CMGR_Msg_t-Struktur f�llt.
 * F�r die Sonderf�lle "Nur Senden", "Nur Empfangen" und "Einmal Senden, dann
 * Empfangen" werden der Einfachheit halber die Funktionen
 *		i2cmgr_enqueueAsynchWrite(...)
 *		i2cmgr_enqueueAsynchRead(...)
 *		i2cmgr_enqueueAsynchWriteRead(...)
 * zur Verf�ung gestellt. Damit spart man sich den Aufbau der I2CMGR_Msg_t-
 * Strukturen.
 *
 * Wenn eine oder mehrere Nachrichten �ber EINEN Aufruf einer der
 * i2cmgr_enqueueAsynch... Funktionen f�r ein Ger�t hinzugef�gt wurden, dann
 * k�nnen diesem Ger�t keine weiteren Nachrichten mehr hinzugef�gt werden, solange
 * diese nicht abgearbeitet sind. (Das ist eine Einschr�nkung der einfachen
 * Implementierung der "Liste" der abzuarbeitenden Nachrichten.)
 * Der Status der Nachrichten eines Ger�tes kann �ber
 *		i2cmgr_getMsgState(...)
 * abgefragt werden.
 * Sollte das Ger�t, f�r welches der Status der Nachrichten abgefragt wird, nicht
 * exisitieren, so wird
 *		I2CMSGSTATE_INVALIDDEVICE
 * zur�ckgegeben. Wenn keine Nachricht vorliegen, dann ist der R�ckgabewert
 *		I2CMSGSTATE_IDLE
 * Direkt nach dem Hinzuf�gen von neuen Nachrichten erhalten diese den Zustand
 *		I2CMSGSTATE_WAITING
 * den sie so lange behalten, bis die �bertragung dieser Nachrichten beginnt.
 * Damit erhalten diese den Status
 *		I2CMSGSTATE_PROCESSING
 * Auch wenn der Status "nur" I2CMSGSTATE_WAITING ist, k�nnen die Nachrichten nicht
 * mehr abgebrochen werden.
 * Wenn die �bertragung der Nachrichten abgeschlossen ist, so ist der Status
 *		I2CMSGSTATE_COMPLETED
 * Sollte ein Fehler aufgetreten sein, so wird der Status
 *		I2CMSGSTATE_ERROR
 * zur�ckgegebem.
 * Praktisch bedeutet das, dass nach dem Hinzuf�gen mit i2cmgr_enqueueAsynch...
 * gepollt werden muss, bis der Status I2CMSGSTATE_COMPLETED oder I2CMSGSTATE_ERROR
 * ist.
 * WICHTIG: Auch dann k�nnen dem Ger�t keine neuen Nachrichten zur �bertragung
 * gegebem werden. Dazu muss erst mit
 *		i2cmgr_resetMsg(...)
 * die Nachrichten des Ger�tes zur�ckgesetzt werden, wodurch der Status wieder auf
 * I2CMSGSTATE_IDLE gesetzt wird. Dann k�nnen wieder Nachrichten �bermittelt werden.
 * Sollte ein Fehler aufgetreten sein, so kann mit
 *		i2cmgr_getMsgRes(...)
 * die Ursache des Fehlers abgefragt werden. (Dies muss vor i2cmgr_resetMsg(...)
 * erfolgen.)
 *
 */


#include "stdtypes.h"

#include "stm32f3xx.h"
#include "stm32f3xx_hal_i2c.h"

#include "common_fcts.h"


typedef enum EnI2CMgrRes
{
	I2CMGRRES_OK = 0,
	I2CMGRRES_BUSBLOCKED,
	I2CMGRRES_UNMANAGEDPORT,
	I2CMGRRES_INVALIDPORT,
	I2CMGRRES_MAXDEVICESREACHED,
	I2CMGRRES_UNMATCHINGCONFIG,
	I2CMGRRES_INVALIDPARAMS,
	I2CMGRRES_INVALIDDEVICE,
	I2CMGRRES_DEVICEQUEUENOTEMPTY,
	I2CMGRRES_NOADDRACK,
	I2CMGRRES_TIMEOUT,
	I2CMGRRES_BUSERR,
	I2CMGRRES_STATEMACHINEERR
} EnI2CMgrRes_t;


typedef enum EnI2C_PORT
{
	I2CPORT_1 = 0,
	I2CPORT_2 = 1,
	I2CPORT_3 = 2
} EnI2C_PORT_t;


typedef enum EnI2C_MsgState
{
	I2CMSGSTATE_IDLE = 0x0,
	I2CMSGSTATE_WAITING = 0x01,
	I2CMSGSTATE_PROCESSING = 0x02,
	I2CMSGSTATE_COMPLETED = 0x04,
	I2CMSGSTATE_ERROR = 0x08,
	I2CMSGSTATE_INVALIDDEVICE = 0x10
} EnI2C_MsgState_t;


typedef enum EnI2CMgr_MsgDir
{
	I2CMGRMSG_RX,
	I2CMGRMSG_TX
} EnI2CMgr_MsgDir_t;

typedef struct I2CMGR_Msg
{
	EnI2CMgr_MsgDir_t eDir;
	uint8_t* pBuffer;
	uint8_t* pBufferEnd;
	uint8_t* pBufferCur;
} I2CMGR_Msg_t;



// =========================================================================
// Deklarationen der "public"-Funktionen
// =========================================================================

void i2cmgr_init();

EnI2CMgrRes_t i2cmgr_addDevice(const EnI2C_PORT_t eI2CPort,
									const I2C_InitTypeDef* pstConfig,
									const uint8_t uAddress,
									uint8_t* uDeviceID);

EnI2CMgrRes_t i2cmgr_removeDevice(uint8_t uDeviceID);

EnI2CMgrRes_t i2cmgr_enqueueAsynchRead(uint8_t uDeviceID,
											uint16_t nRxCount,
											uint8_t* paRxdata);

EnI2CMgrRes_t i2cmgr_enqueueAsynchWrite(uint8_t uDeviceID,
											uint16_t nTxCount,
											const uint8_t* paTxdata);

EnI2CMgrRes_t i2cmgr_enqueueAsynchWriteRead(uint8_t uDeviceID,
												uint16_t nTxCount,
												const uint8_t* paTxdata,
												uint16_t nRxCount,
												uint8_t* paRxdata);

EnI2CMgrRes_t i2cmgr_enqueueAsynchMsgs(uint8_t uDeviceID,
												uint16_t nMsgs,
												I2CMGR_Msg_t* pMsgs);


static inline void i2cmgr_setupMsgStruct(I2CMGR_Msg_t* pMsg, EnI2CMgr_MsgDir_t eDir,
									uint16_t nCount, uint8_t* pBuffer);

EnI2C_MsgState_t i2cmgr_getMsgState(uint8_t uDeviceID);

EnI2CMgrRes_t i2cmgr_resetMsg(uint8_t uDeviceID);

EnI2CMgrRes_t i2cmgr_getMsgRes(uint8_t uDeviceID);


// =========================================================================
// Definitionen der inline-Funktionen
// =========================================================================

static inline void i2cmgr_setupMsgStruct(I2CMGR_Msg_t* pMsg, EnI2CMgr_MsgDir_t eDir,
									uint16_t nCount, uint8_t* pBuffer)
{
	pMsg->eDir = eDir;
	pMsg->pBuffer = pBuffer;
	pMsg->pBufferEnd = pBuffer + nCount;
	pMsg->pBufferCur = pBuffer;
	return;
}

#endif /* I2CMGR_H_ */
