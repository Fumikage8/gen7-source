// ============================================================================
/*
 * @file BattleRoyalResultEventFrame.h
 * @brief バトルロイヤルの結果アプリのプロセスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULTSAMPLE_FRAME_H_INCLUDE )
#define BATTLE_ROYAL_RESULTSAMPLE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"
#include "AppLib/include/Tool/AppToolTimeLimit.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(Frame)

  class BattleRoyalResultEventFrame : public NetAppLib::System::NetAppFrameBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultEventFrame );
  public:
    BattleRoyalResultEventFrame(
      App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork );
    virtual~BattleRoyalResultEventFrame();

  private:

    // NetAppFrameBase
    virtual bool startup();
    virtual bool cleanup();
    virtual void setListener();
    virtual void removeListener();
    virtual void updateSequence();

  private:

    void _Clear()
    {
      m_initSeq = m_endSeq  = 0;
      m_pWork = NULL;
    }

    App::BattleRoyalResult::System::BattleRoyalResultWork*                    m_pWork;
    App::Tool::TimeLimit                                                      m_sceneEndTimeLimit;

    u32 m_initSeq, m_endSeq;

    //  画面終了の秒数定義
    static const u64 m_sScneeEndTimeSec;
  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULTSAMPLE_FRAME_H_INCLUDE
