#ifndef __RT_POX_ITS_H

volatile void rtpox_its_setAllInterruptsAsDumb();
void rtpox_its_dummy();


void rtpox_interruptDispatcher_SystickNS();

void rtpox_initITS();


void rtpox_its_endContext();

uint32_t rtpox_it_getResume();

#endif // !__RT_POX_ITS_H