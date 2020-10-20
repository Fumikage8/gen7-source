#ifndef GFLIB2_SYSTEM_TIMER_PERFORMANCECOUNTER_H_INCLUDED
#define GFLIB2_SYSTEM_TIMER_PERFORMANCECOUNTER_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#if defined(GF_PLATFORM_WIN32)

#include "Win/gfl2_WinPerformanceCounter.h"

#elif defined(GF_PLATFORM_CTR)

#include "Ctr/gfl2_CtrPerformanceCounter.h"

#endif

namespace gfl2 { namespace system { namespace Timer {

  template <class Implement_>
  class PerformanceCounterBase : public Implement_
  {
  public:
    /**
     * @brief 現在の Tick 値を取得する
     */
    static inline u64 GetTick(){
      return Implement_::getTick();
    }

    /**
     * @brief 1 秒あたりの Tick 数を取得する
     */
    static inline u64 GetFrequency(){
      static u64 s_frequencyCache = Implement_::getFrequency(); // 再取得を行わない
      return s_frequencyCache;
    }

  private:

    PerformanceCounterBase():Implement_(){}
    inline virtual ~PerformanceCounterBase(){}

  };

#if defined(GF_PLATFORM_WIN32)

  typedef PerformanceCounterBase<Win::WinPerformanceCounter> PerformanceCounter;

#elif defined(GF_PLATFORM_CTR)

  typedef PerformanceCounterBase<Ctr::CtrPerformanceCounter> PerformanceCounter;

#endif


}}}

#endif
