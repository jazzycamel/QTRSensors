#include <sys/time.h>
#include <cstddef>
#include <ctime>
#include "RPiTime.h"

RPiTime::RPiTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	epochMilli=(uint64_t)tv.tv_sec*(uint64_t)1000+(uint64_t)(tv.tv_usec/1000);
    epochMicro=(uint64_t)tv.tv_sec*(uint64_t)1000000+(uint64_t)(tv.tv_usec);
}

RPiTime::~RPiTime(){

}

unsigned int RPiTime::millis(void){
	struct timeval tv;
	uint64_t now;

	gettimeofday(&tv,NULL);
	now=(uint64_t)tv.tv_sec*(uint64_t)1000+(uint64_t)(tv.tv_usec/1000);
	return (uint32_t)(now-epochMilli);
}

unsigned int RPiTime::micros(void){
	struct timeval tv;
	uint64_t now;

	gettimeofday(&tv,NULL);
	now=(uint64_t)tv.tv_sec*(uint64_t)1000000+(uint64_t)tv.tv_usec;
	return (uint32_t)(now-epochMicro);
}

void RPiTime::delay(unsigned int length){
	struct timespec sleeper, dummy;
	sleeper.tv_sec=(time_t)(length/1000);
	sleeper.tv_nsec=(long)(length%1000)*1000000;
	nanosleep(&sleeper,&dummy);
}

void RPiTime::delayMicroseconds(unsigned int length){
	struct timespec sleeper;
	unsigned int uSecs=length%1000000;
	unsigned int wSecs=length/1000000;

	if(length==0) return;
	else if(length<100) delayMicrosecondsHard(length);
	else {
		sleeper.tv_sec=wSecs;
		sleeper.tv_nsec=(long)(uSecs*1000L);
		nanosleep(&sleeper,NULL);
	}
}

void RPiTime::delayMicrosecondsHard(unsigned int length){
	struct timeval tNow, tLong, tEnd;

	gettimeofday(&tNow,NULL);
	tLong.tv_sec=length/1000000;
	tLong.tv_usec=length%1000000;
	timeradd(&tNow, &tLong, &tEnd);

	while(timercmp(&tNow, &tEnd, <)) gettimeofday(&tNow,NULL);
}
