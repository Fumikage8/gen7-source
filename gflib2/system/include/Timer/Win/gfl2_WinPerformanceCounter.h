#ifndef GFLIB2_SYSTEM_TIMER_WIN_PERFORMANCECOUNTER_H_INCLUDED
#define GFLIB2_SYSTEM_TIMER_WIN_PERFORMANCECOUNTER_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <windows.h>
#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

namespace gfl2 { namespace system { namespace Timer { namespace Win {

  class WinPerformanceCounter
  {
  public:
    //機種ごとの拡張コードでユーザ−に与えたい物のみ、ここに出す。

  protected:

    static inline u64 getTick()
    {
      LARGE_INTEGER longInteger;
      return (::QueryPerformanceCounter  (&longInteger) != FALSE) ? (u64) longInteger.QuadPart : 0ULL;
    }

    static inline u64 getFrequency()
    {
      LARGE_INTEGER longInteger;
      return (::QueryPerformanceFrequency(&longInteger) != FALSE) ? (u64) longInteger.QuadPart : 0ULL;
    }

  public:
    /**
     * @brief 現在の Tick 値をミリ秒単位で返す
     */
    static inline u64 GetMilliSecond()
    {
      return 1000ULL * getTick() / getFrequency();
    }

    /**
     * @brief 渡した Tick 値をミリ秒単位で返す
     */
    static inline u64 GetMilliSecond( u64 tick)
    {
      return 1000ULL * tick / getFrequency();
    }

    /**
     * @brief 現在の Tick 値をマイクロ秒単位で返す
     */
    static inline u64 GetMicroSecond()
    {
      return 1000000ULL * getTick() / getFrequency();
    }


    /**
     * @brief 渡した Tick 値をマイクロ秒単位で返す
     */
    static inline u64 GetMicroSecond( u64 tick)
    {
      return 1000000ULL * tick / getFrequency();
    }

    WinPerformanceCounter();
    inline virtual ~WinPerformanceCounter();
  };

}}}}


#endif