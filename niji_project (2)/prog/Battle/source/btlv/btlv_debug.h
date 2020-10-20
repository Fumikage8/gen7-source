//======================================================================
/**
 * @file	btlv_debug.h
 * @brief	バトル描画デバッグ
 * @author	ariizumi
 * @data	15/03/22
 */
//======================================================================

#if !defined(__BTLV_DEBUG_H__)
#define __BTLV_DEBUG_H__
#pragma once

#include <Macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;

#if PM_DEBUG

void InitBtlvDebug(BattleViewSystem *btlvCore );
void ExitBtlvDebug(BattleViewSystem *btlvCore );

void UpdateBtlvDebug(BattleViewSystem *btlvCore );

#endif //PM_DEBUG

#if PM_DEBUG && defined(GF_PLATFORM_CTR)
#define BTLV_TICK_CALC (1)
#else
#define BTLV_TICK_CALC (0)
#endif

#if BTLV_TICK_CALC

class BtlvTickCalc
{
public:
  BtlvTickCalc()
  {
    mTickStart = 0;
    mTick = 0;
    mName[0] = '\0';
  }

  s64 mTickStart;
  s64 mTick;
  char mName[32];
  
  void Start(char *name)
  {
    nw::ut::strcpy( mName,32,name );
    mTickStart = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
  }
  void End(void)
  {
    s64 end = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMicroSeconds();
    mTick = end-mTickStart;
  }
};

void _BtlvTickStart(int idx , char *name );
void _BtlvTickEnd(int idx , char *name);
void _BtlvTickPrint(void);

#define BtlvTickStart(...) _BtlvTickStart(__VA_ARGS__)
#define BtlvTickEnd(...) _BtlvTickEnd(__VA_ARGS__)
#define BtlvTickPrint(...) _BtlvTickPrint(__VA_ARGS__)

#else

#define BtlvTickStart(...) //None
#define BtlvTickEnd(...) //None
#define BtlvTickPrint(...) //None

#endif

GFL_NAMESPACE_END(btl)

#endif //__BTLV_DEBUG_H__
