/*
 * us_srf08.h
 *
 *  Created on: 16.09.2016
 *      Author: elenz
 */

#ifndef US_SRF08_H_
#define US_SRF08_H_



#include "stdtypes.h"
#include "i2cmgr.h"

typedef enum EnUSonicDataState_ {
	USONICDATA_IDLE,
	USONICDATA_INITMEASUREMENT,
	USONICDATA_CHECKIFCOMPLETED,
	USONICDATA_QUERYDATA
} EnUSonicDataState_t;


typedef enum EnUSDataQueryResult_ {
	USDATAQUERYRES_INPROGRESS,
	USDATAQUERYRES_OK,
	USDATAQUERYRES_ERROR
} EnUSDataQueryResult_t;

typedef enum EnUSDataAvailableQueryResult_ {
	USDATAAVAILABLEQUERYRES_INPROGRESS,
	USDATAAVAILABLEQUERYRES_DATAAVAILABLE,
	USDATAAVAILABLEQUERYRES_DATANOTAVAILABLE,
	USDATAAVAILABLEQUERYRES_ERROR
} EnUSDataAvailableQueryResult_t;

typedef enum EnUSConfigResult_ {
	USCONFIGRES_INPROGRESS,
	USCONFIGRES_OK,
	USCONFIGRES_ERROR
} EnUSConfigResult_t;


typedef struct USdevice_
{
	uint8_t uI2CDeviceID;
	EnI2C_PORT_t eI2CPort;
	uint8_t uI2CAddress;
	uint8_t acRxBuffer[10];
	uint8_t acTxBuffer[10];
	I2CMGR_Msg_t aMsgs[4];

	bool bNewData;
	bool bStartNewMeasurement;

	uint16_t uCurData;
	uint16_t uCommErrorCount;
	EnUSonicDataState_t eDataState;
} USdevice_t;


typedef struct USbroadcaster_
{
	uint8_t uI2CDeviceID;
	EnI2C_PORT_t eI2CPort;
	uint8_t uI2CAddress;
	uint8_t acRxBuffer[10];
	uint8_t acTxBuffer[10];
	I2CMGR_Msg_t aMsgs[4];
} USbroadcaster_t;


bool usonicbc_init(USbroadcaster_t* this, EnI2C_PORT_t ePort);
bool usonicbc_trigger(USbroadcaster_t* this);

bool usonic_init(USdevice_t* this, EnI2C_PORT_t ePort, uint8_t address);
bool usonic_ping(USdevice_t* this);

bool usonic_startConfig(USdevice_t* this);
EnUSConfigResult_t usonic_getConfigResult(USdevice_t* this);

bool usonic_startDataAvailableQuery(USdevice_t* this);
EnUSDataAvailableQueryResult_t usonic_getDataAvailableQueryResult(USdevice_t* this);

bool usonic_startDataQuery(USdevice_t* this);
EnUSDataQueryResult_t usonic_getDataQueryResult(USdevice_t* this, uint16_t* pVal);

inline bool usonic_hasNewData(USdevice_t* this);
inline bool usonic_getData(USdevice_t* this, uint16_t* pDistance);
void usonic_copyData(USdevice_t* this, uint16_t* pData);
void usonic_do(USdevice_t* this);
void usonic_trigger(USdevice_t* this);

inline bool usonic_hasNewData(USdevice_t* this)
{
	return this->bNewData;
}

inline bool usonic_getData(USdevice_t* this, uint16_t* pDistance)
{
	if (!this->bNewData)
	{
		return false;
	}
	else
	{
		usonic_copyData(this, pDistance);
		this->bNewData = false;
	}

	return true;
}


#define US_SLAVE_ADDR_OLD	0xE0
#define US_SLAVE_ADDR_NEW	0xE2	//!!!!!!!!

#define US_SLAVE_ADDR		US_SLAVE_ADDR_OLD
#define US_SLAVE_ADDR_READ	(US_SLAVE_ADDR+1)

#define US_ADDR_FIRMWARE	0
#define US_ADDR_DISTANCE_H	2
#define US_ADDR_DISTANCE_L	3

#define US_ADDR_CMD			0
#define US_ADDR_GAIN		1
#define US_ADDR_RANGE		2

#define	US_CMD_MEASURE_INCH		80
#define US_CMD_MEASURE_CM		81
#define US_CMD_MEASURE_US		82
#define US_CMD_MEASUREMENT		US_CMD_MEASURE_US


#define US_CMD_SET_SLAVEID_1BYTE	160
#define US_CMD_SET_SLAVEID_3BYTE	165
#define US_CMD_SET_SLAVEID_2BYTE	170

//Werte für US_ADDR_GAIN
#define US_GAIN_40		1
#define US_GAIN_50		2
#define US_GAIN_60		3
#define US_GAIN_70		4
#define US_GAIN_80		5
#define US_GAIN_100		6
#define US_GAIN_120		7
#define US_GAIN_140		8
#define US_GAIN_200		9
#define US_GAIN_250		10
#define US_GAIN_300		11
#define US_GAIN_350		12
#define US_GAIN_400		13
#define US_GAIN_500		14
#define US_GAIN_600		15
#define US_GAIN_700		16
//#define US_GAIN			US_GAIN_700
#define US_GAIN			US_GAIN_700

#define US_NO_RESPONSE	0xFF




#define US_RANGE_43MM			0
#define US_RANGE_86MM			1
#define US_RANGE_129MM			2
#define US_RANGE_172MM			3
#define US_RANGE_215MM			4
#define US_RANGE_258MM			5
#define US_RANGE_301MM			6
#define US_RANGE_344MM			7
#define US_RANGE_387MM			8
#define US_RANGE_430MM			9
#define US_RANGE_473MM			10
#define US_RANGE_516MM			11
#define US_RANGE_559MM			12
#define US_RANGE_602MM			13
#define US_RANGE_645MM			14
#define US_RANGE_688MM			15
#define US_RANGE_731MM			16
#define US_RANGE_774MM			17
#define US_RANGE_817MM			18
#define US_RANGE_860MM			19
#define US_RANGE_903MM			20
#define US_RANGE_946MM			21
#define US_RANGE_989MM			22
#define US_RANGE_1032MM			23
#define US_RANGE_1075MM			24
#define US_RANGE_1118MM			25
#define US_RANGE_1161MM			26
#define US_RANGE_1204MM			27
#define US_RANGE_1247MM			28
#define US_RANGE_1290MM			29
#define US_RANGE_1333MM			30
#define US_RANGE_1376MM			31
#define US_RANGE_1419MM			32
#define US_RANGE_1462MM			33
#define US_RANGE_1505MM			34
#define US_RANGE_1548MM			35
#define US_RANGE_1591MM			36
#define US_RANGE_1634MM			37
#define US_RANGE_1677MM			38
#define US_RANGE_1720MM			39
#define US_RANGE_1763MM			40
#define US_RANGE_1806MM			41
#define US_RANGE_1849MM			42
#define US_RANGE_1892MM			43
#define US_RANGE_1935MM			44
#define US_RANGE_1978MM			45
#define US_RANGE_2021MM			46
#define US_RANGE_2064MM			47
#define US_RANGE_2107MM			48
#define US_RANGE_2150MM			49
#define US_RANGE_2193MM			50
#define US_RANGE_2236MM			51
#define US_RANGE_2279MM			52
#define US_RANGE_2322MM			53
#define US_RANGE_2365MM			54
#define US_RANGE_2408MM			55
#define US_RANGE_2451MM			56
#define US_RANGE_2494MM			57
#define US_RANGE_2537MM			58
#define US_RANGE_2580MM			59
#define US_RANGE_2623MM			60
#define US_RANGE_2666MM			61
#define US_RANGE_2709MM			62
#define US_RANGE_2752MM			63
#define US_RANGE_2795MM			64
#define US_RANGE_2838MM			65
#define US_RANGE_2881MM			66
#define US_RANGE_2924MM			67
#define US_RANGE_2967MM			68
#define US_RANGE_3010MM			69
#define US_RANGE_3053MM			70
#define US_RANGE_3096MM			71
#define US_RANGE_3139MM			72
#define US_RANGE_3182MM			73
#define US_RANGE_3225MM			74
#define US_RANGE_3268MM			75
#define US_RANGE_3311MM			76
#define US_RANGE_3354MM			77
#define US_RANGE_3397MM			78
#define US_RANGE_3440MM			79
#define US_RANGE_3483MM			80
#define US_RANGE_3526MM			81
#define US_RANGE_3569MM			82
#define US_RANGE_3612MM			83
#define US_RANGE_3655MM			84
#define US_RANGE_3698MM			85
#define US_RANGE_3741MM			86
#define US_RANGE_3784MM			87
#define US_RANGE_3827MM			88
#define US_RANGE_3870MM			89
#define US_RANGE_3913MM			90
#define US_RANGE_3956MM			91
#define US_RANGE_3999MM			92
#define US_RANGE_4042MM			93
#define US_RANGE_4085MM			94
#define US_RANGE_4128MM			95
#define US_RANGE_4171MM			96
#define US_RANGE_4214MM			97
#define US_RANGE_4257MM			98
#define US_RANGE_4300MM			99
#define US_RANGE_4343MM			100
#define US_RANGE_4386MM			101
#define US_RANGE_4429MM			102
#define US_RANGE_4472MM			103
#define US_RANGE_4515MM			104
#define US_RANGE_4558MM			105
#define US_RANGE_4601MM			106
#define US_RANGE_4644MM			107
#define US_RANGE_4687MM			108
#define US_RANGE_4730MM			109
#define US_RANGE_4773MM			110
#define US_RANGE_4816MM			111
#define US_RANGE_4859MM			112
#define US_RANGE_4902MM			113
#define US_RANGE_4945MM			114
#define US_RANGE_4988MM			115
#define US_RANGE_5031MM			116
#define US_RANGE_5074MM			117
#define US_RANGE_5117MM			118
#define US_RANGE_5160MM			119
#define US_RANGE_5203MM			120
#define US_RANGE_5246MM			121
#define US_RANGE_5289MM			122
#define US_RANGE_5332MM			123
#define US_RANGE_5375MM			124
#define US_RANGE_5418MM			125
#define US_RANGE_5461MM			126
#define US_RANGE_5504MM			127
#define US_RANGE_5547MM			128
#define US_RANGE_5590MM			129
#define US_RANGE_5633MM			130
#define US_RANGE_5676MM			131
#define US_RANGE_5719MM			132
#define US_RANGE_5762MM			133
#define US_RANGE_5805MM			134
#define US_RANGE_5848MM			135
#define US_RANGE_5891MM			136
#define US_RANGE_5934MM			137
#define US_RANGE_5977MM			138
#define US_RANGE_6020MM			139
#define US_RANGE_6063MM			140
#define US_RANGE_6106MM			141
#define US_RANGE_6149MM			142
#define US_RANGE_6192MM			143
#define US_RANGE_6235MM			144
#define US_RANGE_6278MM			145
#define US_RANGE_6321MM			146
#define US_RANGE_6364MM			147
#define US_RANGE_6407MM			148
#define US_RANGE_6450MM			149
#define US_RANGE_6493MM			150
#define US_RANGE_6536MM			151
#define US_RANGE_6579MM			152
#define US_RANGE_6622MM			153
#define US_RANGE_6665MM			154
#define US_RANGE_6708MM			155
#define US_RANGE_6751MM			156
#define US_RANGE_6794MM			157
#define US_RANGE_6837MM			158
#define US_RANGE_6880MM			159
#define US_RANGE_6923MM			160
#define US_RANGE_6966MM			161
#define US_RANGE_7009MM			162
#define US_RANGE_7052MM			163
#define US_RANGE_7095MM			164
#define US_RANGE_7138MM			165
#define US_RANGE_7181MM			166
#define US_RANGE_7224MM			167
#define US_RANGE_7267MM			168
#define US_RANGE_7310MM			169
#define US_RANGE_7353MM			170
#define US_RANGE_7396MM			171
#define US_RANGE_7439MM			172
#define US_RANGE_7482MM			173
#define US_RANGE_7525MM			174
#define US_RANGE_7568MM			175
#define US_RANGE_7611MM			176
#define US_RANGE_7654MM			177
#define US_RANGE_7697MM			178
#define US_RANGE_7740MM			179
#define US_RANGE_7783MM			180
#define US_RANGE_7826MM			181
#define US_RANGE_7869MM			182
#define US_RANGE_7912MM			183
#define US_RANGE_7955MM			184
#define US_RANGE_7998MM			185
#define US_RANGE_8041MM			186
#define US_RANGE_8084MM			187
#define US_RANGE_8127MM			188
#define US_RANGE_8170MM			189
#define US_RANGE_8213MM			190
#define US_RANGE_8256MM			191
#define US_RANGE_8299MM			192
#define US_RANGE_8342MM			193
#define US_RANGE_8385MM			194
#define US_RANGE_8428MM			195
#define US_RANGE_8471MM			196
#define US_RANGE_8514MM			197
#define US_RANGE_8557MM			198
#define US_RANGE_8600MM			199
#define US_RANGE_8643MM			200
#define US_RANGE_8686MM			201
#define US_RANGE_8729MM			202
#define US_RANGE_8772MM			203
#define US_RANGE_8815MM			204
#define US_RANGE_8858MM			205
#define US_RANGE_8901MM			206
#define US_RANGE_8944MM			207
#define US_RANGE_8987MM			208
#define US_RANGE_9030MM			209
#define US_RANGE_9073MM			210
#define US_RANGE_9116MM			211
#define US_RANGE_9159MM			212
#define US_RANGE_9202MM			213
#define US_RANGE_9245MM			214
#define US_RANGE_9288MM			215
#define US_RANGE_9331MM			216
#define US_RANGE_9374MM			217
#define US_RANGE_9417MM			218
#define US_RANGE_9460MM			219
#define US_RANGE_9503MM			220
#define US_RANGE_9546MM			221
#define US_RANGE_9589MM			222
#define US_RANGE_9632MM			223
#define US_RANGE_9675MM			224
#define US_RANGE_9718MM			225
#define US_RANGE_9761MM			226
#define US_RANGE_9804MM			227
#define US_RANGE_9847MM			228
#define US_RANGE_9890MM			229
#define US_RANGE_9933MM			230
#define US_RANGE_9976MM			231
#define US_RANGE_10019MM			232
#define US_RANGE_10062MM			233
#define US_RANGE_10105MM			234
#define US_RANGE_10148MM			235
#define US_RANGE_10191MM			236
#define US_RANGE_10234MM			237
#define US_RANGE_10277MM			238
#define US_RANGE_10320MM			239
#define US_RANGE_10363MM			240
#define US_RANGE_10406MM			241
#define US_RANGE_10449MM			242
#define US_RANGE_10492MM			243
#define US_RANGE_10535MM			244
#define US_RANGE_10578MM			245
#define US_RANGE_10621MM			246
#define US_RANGE_10664MM			247
#define US_RANGE_10707MM			248
#define US_RANGE_10750MM			249
#define US_RANGE_10793MM			250
#define US_RANGE_10836MM			251
#define US_RANGE_10879MM			252
#define US_RANGE_10922MM			253
#define US_RANGE_10965MM			254
#define US_RANGE_11008MM			255

//#define US_RANGE				US_RANGE_11008MM
#define US_RANGE				US_RANGE_6020MM



#endif /* US_SRF08_H_ */
