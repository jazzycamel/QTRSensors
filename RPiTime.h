#ifndef RPITIME_H
#define RPITIME_H

#include <stdint.h>

class RPiTime {
public:
	RPiTime();
	~RPiTime();

	unsigned int millis(void);
	unsigned int micros(void);
	void delay(unsigned int length);
	void delayMicroseconds(unsigned int length);

private:
	void delayMicrosecondsHard(unsigned int length);

	uint64_t epochMilli, epochMicro;
};

#endif