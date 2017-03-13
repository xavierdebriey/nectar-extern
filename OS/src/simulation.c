#include "config.h"
#include "filesystem.h"
#include "take_measures.h"

uint32_t measure_flag;

void take_measures2(void)
{
	Data data;
	data.timestamp = timestamp;
	data.humidity = 32.54;
	data.temperature = 22.42;
	data.weight = 0.0002;
	data.volume = 32.2234;
	data.freq[0] = 8.0239812;
	data.freq[1] = 8.2012342;
	data.freq[2] = 7.9056181;
	pushData(data);
	measure_flag = 0;
}
