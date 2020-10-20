#ifndef GFLIB2_SYSTEM_TIMER_PERFORMANCECOUNTER_CLR_H_INCLUDED
#define GFLIB2_SYSTEM_TIMER_PERFORMANCECOUNTER_CLR_H_INCLUDED

#include <system/include/Timer/gfl2_PerformanceCounter.h>

namespace gfl2 { namespace clr { namespace system { namespace timer {

public ref class PerformanceCounter
{
public:
		
	/**
		* @brief	現在の Tick 値をマイクロ秒単位で返す
		*/
	static inline u64 GetMicroSecond(){
    return (u64)gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
	}
};

}}}}

#endif
