/*
 * ARingbuffer_t.h
 *
 *  Created on: 15.02.2010
 *      Author: elenz
 */



/*
 * Die "Klasse" ARingbuffer_t implementiert einen als Ringpuffer angelegten
 * FIFO-Puffer mit folgenden Eigenschaften:
 * - FIFO
 * - nicht �berschreibend (wenn voll, dann werden die neuen Daten verworfen)
 * - Atomares Gruppieren von Schreiboperationen (entweder werden alle Daten
 *      einer Folge von Schreiboperationen hinzugef�gt, oder keine)
 * - Optionales Schreiben eines "Failed-Marks", wenn Schreiboperation mangels
 *   Platz nicht ausgef�hrt werden konnte
 * - Optionales Schreiben der Gesamtl�nge der hinzugef�gten Daten einer Folge
 *   von Schreiboperationen an den Begin dieser Daten
 *
 *
 * Alle "public" Funktionen beginnen mit "ARingbuffer_" und erwarten als
 * erstes Argument einen Pointer auf eine ARingbuffer_t-Struktur:
 *         ARingbuffer_xyz(ARingbuffer_t*, ...)
 *
 *
 * Initialisierung
 *     Der Speicher muss au�erhalb des Rinpuffers allokiert werden.
 *     (Damit kann auf die Funktion "malloc" verzichtet werden.)
 *     Die Startadresse und L�nge des allokierten Speicherbereichs wird
 *     beim Initialisieren �bergeben
 *         ARingbuffer_init(...)
 *
 *
 * Schreiboperationen
 *  (Alle Schreiboperationen geben einen bool-Wert zur�ck. Dieser dient nur
 *  zur Information der aufrufenden Funktion und kann ignoriert werden)
 *
 *  Starten einer atomaren Gruppe von Schreiboperationen
 *         ARingbuffer_atomicput_start(...)
 *  Schreiben von Daten
 *         ARingbuffer_atomicputX(...)
 *  Schreiben eines \0-terminierten Strings
 *         ARingbuffer_atomicputX(...)
 *  Abschlie�en einer atomaren Gruppe von Schreiboperationen
 *         ARingbuffer_atomicput_end(...)
 *
 * Leseoperationen
 *     Lesen und Freigeben des Speichers
 *         ARingbuffer_getX(...)
 *     Daten betrachten (Lesen OHNE Freigeben des Speichers)
 *         ARingbuffer_glanceX(...)
 *     Freigeben des Speichers (ohne Lesen)
 *         ARingbuffer_dropX(...)
 *
 * "Zustand" Puffer
 *  Freien Speicherplatz (ggfs. aktive Schreibgruppe ber�cksichtigt)
 *        ARingbuffer_getFreeSpace(...)
 *    Anzahl Daten in Puffer (ohne Daten einer ggfs. aktive Schreibgruppe)
 *        ARingbuffer_getCount(...)
 *    Abk�rzung f�r (ARingbuffer_getCount(...) == 0)
 *        ARingbuffer_isempty(...)
 *  (Durch ggfs. aktive Schreibgruppen ergibt getFreeSpace + getCount nicht
 *  unbedingt die effektive Pufferl�nge (= L�nge allokierter Speicher - 1))
 *
 *    Anzahl fehlgeschlagener atomarer Schreibgruppen
 *        ARingbuffer_getFailedCount(...)
 *    Zur�cksetzen des Z�hlers f�r fehlgeschlagene atomare Schreibgruppen
 *        ARingbuffer_getFailedCount(...)
 *
 *
 *    Anzahl physikalisch hintereinanderliegender Daten in Puffer
 *        ARingbuffer_getContiguousCount(...)
 *    Kann benutzt werden, um Daten vom Puffer an andere Funktionen weiter-
 *    zugeben, die die Daten hintereinanderliegend erwarten. Dazu kann mit
 *        ARingbuffer_getReadPtr(...)
 *    der Lesezeiger geholt werden. (Dann den Speicher mit ARingbuffer_dropX(...)
 *    freigeben.)
 *
 * L�schen
 *    Puffer zur�cksetzen
 *         ARingbuffer_clear(...)
 *
 *
 * Threadsicherheit
 *  - Die Gruppe der Schreiboperationen ist gegen�ber der Gruppe der Lese-
 *    operationen strukturell threadsicher, d.h. es kann aus verschiedenen
 *    Threads geschrieben und gelesen werden, ohne das die Datenstruktur
 *    besch�digt wird.
 *  - Die Schreib- und Leseoperationen jeweils untereinander sind NICHT
 *    threadsicher, d.h. es ist bei der Verwendung eines ARingbuffers darauf
 *    zu achten, dass sich mehrere Schreib- bzw. Leseoperationen nicht �ber-
 *    lappen k�nnen.
 *
 * Gesamtl�nge der hinzugef�gten Daten
 *  Beim Initialisieren kann angeben werden, in wievielen Bytes zu Begin
 *  der neuen Datengruppe die Anzahl der Bytes der Datengruppe gespeichert
 *  werden soll. (Wenn die Wunschl�nge gleich Null ist, dann wird die Gesamt-
 *  l�nge der hinzugef�gten Daten nicht geschrieben.)
 *  Damit ergibt sich bspw. beim Hinzuf�gen der Datengruppen
 *  (0xA1 0xA2 0xA3) und (0xB1 0xB2) folgender Pufferinhalt (L�ngenbytes = 1):
 *      ... 0x03 0xA1 0xA2 0xA3 0x02 0xB1 0xB2
 *
 * Failed-Marks
 *  Bei Beginnen einer neuen atomaren Gruppe von Schreiboperationen kann
 *  festgelegt werden, ob und welche Daten eingef�gt werden sollen, wenn
 *  das Schreiben der atomaren Gruppe nicht erfolgreich war.
 *  Beim Initialisieren des Puffers kann angegeben werden, ob direkt auf-
 *  einanderfolgende Failed-Marks ignoriert werden sollen. (Aufeinander-
 *  folgende Failed-Marks werden zwangsweise ignoriert, wenn das vorherige
 *  Failed-Mark mangels Platz nicht in den Puffer geschrieben werden kann.)
 *
 *  Die Interpretation der Gesamtl�nge der Daten und der Failed-Marks muss
 *  von den aufrufenden Funktionen �bernommen werden. Es exitieren keine
 *  besonderen Leseoperationen.
 *
 */

/*
 * Allgemeine Bezeichnung der Pointer:
 *     pBegin    Zeigt auf den Begin des allokierten Bereichs
 *  pEnd    Zeigt auf die erste Adresse NACH dem allokierten Bereich
 *  pPut    Zeigt auf die Adresse, die als n�chstes beschrieben wird
 *  pGet    Zeigt auf die Adresse, die als n�chstes gelesen wird
 *          Wenn (pPut == pGet), dann ist der Puffer leer
 *          Die effektive Pufferl�nge ist damit ein Byte k�rzer als der
 *          allokierte Bereich
 */

/*
 * ToDos:
 * - Falls die L�nge der hinzugef�gten Daten eingef�gt werden soll, dann
 *      muss noch sichergestellt werden, dass die Anzahl der Datenbytes nicht
 *   den Wertebereich des Speicherplatzes f�r die Anzahl �berschreitet.
 * - Es k�nnten noch nicht-atomare Schreiboperationen und Byte-weise Schreib-
 *   und Leseoperationen implementiert werden, um den Puffer allgemeiner
 *   verwenden zu k�nnen.
 */

#ifndef ARINGBUFFER_H_
#define ARINGBUFFER_H_


#include "stdtypes.h"


#define MAXFAILEDMARKLEN    10

typedef struct ARingbuffer_Atomicput {
    uint8_t* pPut;
    uint8_t* pBegin;
    bool bFailed;
    //bool bCurPutFailed;
    bool bPrevPutFailed;
    uint8_t uWriteDataCountBytes;
    bool bWriteDataCountIfFailed;
    uint8_t uFailedMarkLen;
    uint8_t acFailedMark[MAXFAILEDMARKLEN];
    bool bSuppressConsectutiveFailedMarks;
} ARingbuffer_Atomicput_t;

typedef struct ARingbuffer {
    uint8_t* pPut;
    uint8_t* pGet;
    uint8_t* pBegin;    // Zeigt auf das erste Feld IM allokierten Bereich
    uint8_t* pEnd;        // Zeigt auf das erste Feld NACH allokierten Bereich
    ARingbuffer_Atomicput_t atomicput;
    uint32_t uFailed;
} ARingbuffer_t;


#define RINGBUFFER_COUNT(pPut, pGet, pBegin, pEnd)    \
            (pPut >= pGet) ? (pPut - pGet) : ( (pEnd - pGet) + (pPut - pBegin) )

#define RINGBUFFER_FREE(pPut, pGet, pBegin, pEnd)    \
            (pGet > pPut) ? (pGet - pPut - 1) : ( (pEnd - pPut) + (pGet - pBegin) - 1 )


void ARingbuffer_init(ARingbuffer_t* this,
                            uint8_t* pBufferLoc, uint32_t nBufferSize,
                            uint8_t uWriteDataCountBytes,
                            bool bSuppressConsectutiveFailedMarks);

bool ARingbuffer_atomicput_start(ARingbuffer_t* this,
                                uint8_t uFailedDataLen, uint8_t* pFailedData,
                                bool bWriteDataCountIfFailed);
bool ARingbuffer_atomicputX(ARingbuffer_t* this, uint8_t* pData, uint8_t uDataLen);
bool ARingbuffer_atomicput_putS(ARingbuffer_t* this, char* S, bool bWriteNull);

bool ARingbuffer_atomicput_end(ARingbuffer_t* this);

bool ARingbuffer_glanceX(ARingbuffer_t* this, uint8_t* pData,
                                            uint32_t uOffset, uint32_t uCount);

bool ARingbuffer_dropX(ARingbuffer_t* this, uint32_t uCount);

bool ARingbuffer_getX(ARingbuffer_t* this, uint8_t* pTarget, uint16_t uCount);

void ARingbuffer_clear(ARingbuffer_t* this);

uint16_t ARingbuffer_getFreeSpace(ARingbuffer_t* this);


static inline bool ARingbuffer_isempty(ARingbuffer_t* this);
static inline uint16_t ARingbuffer_getCount(ARingbuffer_t* this);

static inline uint16_t ARingbuffer_getContiguousCount(ARingbuffer_t* this);
static inline uint8_t* ARingbuffer_getReadPtr(ARingbuffer_t* this);


static inline bool ARingbuffer_isempty(ARingbuffer_t* this)
{
    return (ARingbuffer_getCount(this) == 0);
}


static inline uint16_t ARingbuffer_getCount(ARingbuffer_t* this)
{
    return RINGBUFFER_COUNT(this->pPut, this->pGet, this->pBegin, this->pEnd);
}


static inline uint16_t ARingbuffer_getContiguousCount(ARingbuffer_t* this)
{
    return (this->pPut >= this->pGet) ? (this->pPut - this->pGet) : (this->pEnd - this->pGet);
}


static inline uint8_t* ARingbuffer_getReadPtr(ARingbuffer_t* this)
{
    return this->pGet;
}


static inline uint32_t ARingbuffer_getFailedCount(ARingbuffer_t* this)
{
    return this->uFailed;
}

static inline uint32_t ARingbuffer_resetFailedCount(ARingbuffer_t* this)
{
    uint32_t uFailed;

    uFailed = this->uFailed;
    this->uFailed = 0;

    return uFailed;
}

#endif /* ARINGBUFFER_H_ */
