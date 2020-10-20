#ifndef GFLIB2_SYSTEM_TIMER_CTR_PERFORMANCECOUNTER_H_INCLUDED
#define GFLIB2_SYSTEM_TIMER_CTR_PERFORMANCECOUNTER_H_INCLUDED

#include <nn.h>
#include <types/include/gfl2_Typedef.h>

namespace gfl2 { namespace system { namespace Timer { namespace Ctr {

  class CtrPerformanceCounter
  {
  public:
    //機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。

  protected:

    static inline u64 getTick()
    {
      return (u64) nn::os::Tick::GetSystemCurrent();
    }

    static inline u64 getFrequency()
    {
      return (u64) nn::os::Tick::TICKS_PER_SECOND;
    }

  public:
    /**
     * @brief 現在の Tick 値をミリ秒単位で返す
     */
    static inline u64 GetMilliSecond()
    {
      return nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    }

    /**
     * @brief 渡した Tick 値をミリ秒単位で返す
     */
    static inline u64 GetMilliSecond( u64 tick)
    {
      return nn::os::Tick(tick).ToTimeSpan().GetMilliSeconds();
    }

    /**
     * @brief 現在の Tick 値をマイクロ秒単位で返す
     */
    static inline u64 GetMicroSecond()
    {
      return nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
    }


    /**
     * @brief 渡した Tick 値をマイクロ秒単位で返す
     */
    static inline u64 GetMicroSecond( u64 tick)
    {
      return nn::os::Tick(tick).ToTimeSpan().GetMicroSeconds();
    }

    CtrPerformanceCounter();
    inline virtual ~CtrPerformanceCounter();
  };

}}}}


#endif
