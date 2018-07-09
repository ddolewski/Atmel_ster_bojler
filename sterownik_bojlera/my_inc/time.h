#ifndef TIME_H_
#define TIME_H_

#include "global.h"

#define TIME_DST_BASE_YEAR 	2016
#define TIME_DST_MAX_YEAR	2036

typedef struct
{
	uint16_t 	year;
	uint8_t 	month;
	uint8_t 	mday;
	uint8_t 	wday;
	uint8_t 	hour;
	uint8_t 	min;
	uint8_t 	sec;

}time_complex_t;

typedef struct
{
	uint8_t 	hour;
	uint8_t 	min;
}time_simple_t;

typedef enum
{
	TIME_COMP_A_GREATER,
	TIME_COMP_B_GREATER,
	TIME_COMP_EQUAL
}time_comp_t;

typedef enum
{
	LOCALIZATION_FIRST,
	EUROPE = LOCALIZATION_FIRST,
	AUSTRALIA,
	UNITED_STATES,
	CANADA,
	LOCALIZATION_LAST = CANADA
}localization_t;

typedef struct
{
	time_comp_t	result;
	uint32_t secAbsDiff;
}time_compare_result_t;

void timeHandler(void);
uint8_t timeLeapYearCheck(uint16_t year_to_test);
void timeTimestampToTime(uint32_t xTimestamp, time_complex_t * xTime);
void timeTimeToTimestamp(const time_complex_t * xTime, uint32_t * xTimestamp);
uint8_t timeCalcDayWeek(const time_complex_t * xTime);
uint8_t timeMaxDayOfMonthGet(uint8_t xMonth, uint16_t  xYear);
uint8_t timeUtcToLocalConv(const time_complex_t * xInUtcTime, time_complex_t * xOutLocalTime);
void timeComplexCopy(const time_complex_t * xSrcTime, time_complex_t * xDstTime);
time_compare_result_t timeComplexCompare(const time_complex_t * xTimeA, const time_complex_t * xTimeB);
int8_t timeSimpleCompare(const time_simple_t * xTimeA, const time_simple_t * xTimeB);
uint8_t timeLocalToUtcConv(const time_complex_t * xInLocalTime, time_complex_t * xOutUtcTime);

void testTimeDstCorrection(const time_complex_t* xInTime, time_complex_t* xOutTime, int8_t xLocaltimeZoneHours, int8_t xLocaltimeZoneMinutes, uint8_t xInDstActive);
#endif /* TIME_H_ */
