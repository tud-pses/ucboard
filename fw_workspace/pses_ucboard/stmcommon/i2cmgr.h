#ifndef I2CMGR_H_
#define I2CMGR_H_

/*
 * In "i2cmgr" ist eine Verwaltung für den I2C-Bus implementiert.
 *
 * Diese Verwaltung übernimmt die Initialisierung der Busse und die Übertragung 
 * von Nachrichten (Messages) zu und von Geräten (Devices), die an die Busse
 * angeschlossen sind.
 * Unter Nachricht ist einfach die Übertragung einer bestimmten Anzahl von Bytes
 * zu einem Gerät (TX) oder von einem Gerät (RX) zu verstehen.
 * Die Nachrichten werden dabei grundsätzlich asynchron übermittelt, d.h. dass
 * mit den unten genannten Funktionen die Nachrichten zunächst nur an die 
 * Verwaltung übergeben und die Funktionen dann gleich wieder beendet werden, ohne
 * dass auf die Übertragung der Nachrichten gewartet wird.
 * Somit muss man auch bei mehreren Geräten an einem Bus nicht darauf achten, dass
 * diese sich den Bus gegenseitig "gerecht" teilen, da dies von i2cmgr übernommen
 * wird.
 * 
 * Fehlerbehandlung:
 * - Wenn das angesprochene Gerät kein Acknowledge sendet, so wird ein Stop-Bit
 * gesendet.
 * - In allen anderen Fällen wird der Bus resetet. D.h. es wird "manuell" ein
 * Null-Byte (inklusive Start- und Stop-Bit) auf den Bus geschrieben.
 * In beiden Fällen wird die Übertragung aller Nachrichten zu dem betroffenden 
 * Gerät abgebrochen und der Status auf I2CMSGSTATE_ERROR gesetzt. Dann wird mit 
 * ggfs. vorhandenen Nachrichten anderer Geräte weitergemacht.
 * 
 *
 * Die verwalteten Busse sowie deren Pin-Konfigurationen werden in i2cmgr.c
 * gewählt. D.h. es müssen nicht alle Busse des uC über i2cmgr verwaltet werden.
 *
 * Die Verwaltung muss mit dem Aufruf von
 *		i2cmgr_init
 * initialisiert werden.
 * Die Geräte werden über
 *		i2cmgr_addDevice(...)
 * hinzugefügt, wobei angegeben wird, an welchen Bus das Gerät angeschlossen ist,
 * welche Bus-Konfiguration benötigt ist und welche Adresse das Gerät hat. Dabei
 * sind nur 7bit-Adressen zulässig, wobei diese schon um eins nach links geshiftet
 * anzugeben sind.
 * Bei der (erfolgreichen) Anmeldung erhält jedes Gerät eine eindeutige ID, durch
 * die es im Weiteren angesprochen wird.
 *
 * Die Nachrichten, die ein Gerät verschicken und empfangen soll, werden über
 *		i2cmgr_enqueueAsynchMsgs(...)
 * in die Liste der abzuarbeitenden Nachrichten eingereiht. Der genannten Funktion
 * wird ein Zeiger auf ein Array von I2CMGR_Msg_t-Elementen übergeben. Diese
 * Struktur hat einen für die spätere Verwendung hin ausgelegten und weniger einen
 * intuitiven Aufbau. Daher wird auch die Funktion
 *		i2cmgr_setupMsgStruct(...)
 * zur Verfügung gestellt, die aus den Angaben Senden oder Empfangen, Beginn des
 * Buffers und Anzahl der zu übertragenden Bytes eine I2CMGR_Msg_t-Struktur füllt.
 * Für die Sonderfälle "Nur Senden", "Nur Empfangen" und "Einmal Senden, dann
 * Empfangen" werden der Einfachheit halber die Funktionen
 *		i2cmgr_enqueueAsynchWrite(...)
 *		i2cmgr_enqueueAsynchRead(...)
 *		i2cmgr_enqueueAsynchWriteRead(...)
 * zur Verfüung gestellt. Damit spart man sich den Aufbau der I2CMGR_Msg_t-
 * Strukturen.
 *
 * Wenn eine oder mehrere Nachrichten über EINEN Aufruf einer der
 * i2cmgr_enqueueAsynch... Funktionen für ein Gerät hinzugefügt wurden, dann 
 * können diesem Gerät keine weiteren Nachrichten mehr hinzugefügt werden, solange
 * diese nicht abgearbeitet sind. (Das ist eine Einschränkung der einfachen
 * Implementierung der "Liste" der abzuarbeitenden Nachrichten.)
 * Der Status der Nachrichten eines Gerätes kann über
 *		i2cmgr_getMsgState(...)
 * abgefragt werden.
 * Sollte das Gerät, für welches der Status der Nachrichten abgefragt wird, nicht
 * exisitieren, so wird
 *		I2CMSGSTATE_INVALIDDEVICE
 * zurückgegeben. Wenn keine Nachricht vorliegen, dann ist der Rückgabewert
 *		I2CMSGSTATE_IDLE
 * Direkt nach dem Hinzufügen von neuen Nachrichten erhalten diese den Zustand
 *		I2CMSGSTATE_WAITING
 * den sie so lange behalten, bis die Übertragung dieser Nachrichten beginnt.
 * Damit erhalten diese den Status
 *		I2CMSGSTATE_PROCESSING
 * Auch wenn der Status "nur" I2CMSGSTATE_WAITING ist, können die Nachrichten nicht
 * mehr abgebrochen werden.
 * Wenn die Übertragung der Nachrichten abgeschlossen ist, so ist der Status
 *		I2CMSGSTATE_COMPLETED
 * Sollte ein Fehler aufgetreten sein, so wird der Status
 *		I2CMSGSTATE_ERROR
 * zurückgegebem.
 * Praktisch bedeutet das, dass nach dem Hinzufügen mit i2cmgr_enqueueAsynch...
 * gepollt werden muss, bis der Status I2CMSGSTATE_COMPLETED oder I2CMSGSTATE_ERROR
 * ist.
 * WICHTIG: Auch dann können dem Gerät keine neuen Nachrichten zur Übertragung
 * gegebem werden. Dazu muss erst mit
 *		i2cmgr_resetMsg(...)
 * die Nachrichten des Gerätes zurückgesetzt werden, wodurch der Status wieder auf
 * I2CMSGSTATE_IDLE gesetzt wird. Dann können wieder Nachrichten übermittelt werden.
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
	uint8_t* pBufferEnd1;
	uint8_t* pBufferCur;
} I2CMGR_Msg_t;



// =========================================================================
// Deklarationen der "public"-Funktionen
// =========================================================================

void i2cmgr_init();

EnI2CMgrRes_t i2cmgr_addDevice(EnI2C_PORT_t eI2CPort, I2C_InitTypeDef* pstConfig, 
									uint8_t uAddress,
									uint8_t* uDeviceID);

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
	pMsg->pBufferEnd1 = pBuffer + nCount;
	pMsg->pBufferCur = pBuffer;
	return;
}

#endif /* I2CMGR_H_ */
