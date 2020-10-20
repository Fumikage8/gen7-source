//======================================================================
/**
 * @file  DebugWinSystem.h
 * @brief 15/02/19
 * @author  ariizumi
 * @data  niji用デバッグウィンドウ周り
 */
//======================================================================
#ifndef __DEBUG_WIN_SYSTEM__
#define __DEBUG_WIN_SYSTEM__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <macro/include/gfl2_Macros.h>
#include <ro/include/gfl2_RoManager.h>


GFL_NAMESPACE_BEGIN(Debug)
GFL_NAMESPACE_BEGIN(DebugWin)

#if GFL_DEBUG
#define USE_DEBUG_WIN (1)
#else
#define USE_DEBUG_WIN (0)
#endif

//Tick計測の種類
typedef enum
{
  DEBUGWIN_TICK_UPDATE,
  DEBUGWIN_TICK_UPNEXT,
  DEBUGWIN_TICK_DRAW,
  DEBUGWIN_TICK_GPU,

  DEBUGWIN_TICK_MAX,
}DEBUGWIN_TICK_TYPE;


//デバッグシステムの初期化
void InitializeSystem( gfl2::heap::HeapBase *parentHeap , gfl2::heap::HeapBase *parentDevHeap );
//デバッグシステムの終了
void FinalizeSystem();

//Tick計測用
void TickStart( int tickType );
void TickEnd( int tickType );
void TickCalc( int tickType , s64 microSec );



#if USE_DEBUG_WIN
//デバッグシステム用ワーク
class DebugWinWork:public gfl2::base::Singleton<DebugWinWork>,public gfl2::debug::DebugWinSystemCallBack
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugWinWork );

public:
  DebugWinWork();

  void TickReset(void);
  void TickReset( int tickType );
  void TickResetRequest( int tickType );  //MainFuncの中からTickResetを呼ぶと

  void TickStart( int tickType );
  void TickEnd( int tickType );

  void TickCalc( int tickType , s64 microSec );

  s64 GetTickNow( int tickType ){return mTickNow[tickType];}
  s64 GetTickAve( int tickType , int cnt ){return mTickAve[tickType][cnt];}
  s64 GetTickCalcAve( int tickType );
  s64 GetTickMax( int tickType ){return mTickMax[tickType];}


  bool IsMode60(void){return mIs60Mode;}
  void SetMode60(bool flg){mIs60Mode = flg;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  デバッグウィンドウ用サウンド再生
   *          gflibにサウンドを持たせるわけにもいかないので、アプリにお任せ。
   *
   *  @param  const DEBUGWIN_SOUND_TYPE サウンド種類
   */
  //-----------------------------------------------------------------------------
  virtual void SoundCallBack( const gfl2::debug::DEBUGWIN_SOUND_TYPE type );

  int mResetRequest;
private:
  static const int TICK_AVE_NUM = 20;

  s64 mTickStart[DEBUGWIN_TICK_MAX];
  s64 mTickNow[DEBUGWIN_TICK_MAX];
  s64 mTickMax[DEBUGWIN_TICK_MAX];
  s64 mTickAve[DEBUGWIN_TICK_MAX][TICK_AVE_NUM];
  int mTickAveCnt[DEBUGWIN_TICK_MAX];

  bool mIs60Mode;
};

#endif //USE_DEBUG_WIN

GFL_NAMESPACE_END(DebugWin)
GFL_NAMESPACE_END(Debug)


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
#if USE_DEBUG_WIN
SINGLETON_INSTANCE_DECLARATION( Debug::DebugWin::DebugWinWork );
#endif

#endif __DEBUG_WIN_SYSTEM__
