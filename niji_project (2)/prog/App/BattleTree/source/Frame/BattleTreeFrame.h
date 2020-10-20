// ============================================================================
/*
 * @file BattleTreeFrame.h
 * @brief バトルツリーアプリのプロセスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_FRAME_H_INCLUDE )
#define BATTLE_TREE_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "App/BattleTree/source/System/BattleTreeWork.h"

//  viewのインクルード
#include "App/BattleTree/source/View/BattleTreeLowerNPCListView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(Frame)

  class BattleTreeFrame : public NetAppLib::System::NetAppFrameBase, App::BattleTree::View::LowerNPCTrainerListView_Delegate
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleTreeFrame );
  public:
    BattleTreeFrame(
      App::BattleTree::System::BattleTreeWork* pBattleTreeWork );
    virtual~BattleTreeFrame();

  protected:
    //! フレームを終了します(自身の処理を終わらせます)
    virtual void exitFrame( NetAppLib::System::FrameResult result );

  private:

    // NetAppFrameBase
    virtual bool startup();
    virtual bool cleanup();
    virtual void setListener();
    virtual void removeListener();
    virtual void updateSequence();

    //  リストカーソル時に呼び出す
    virtual void LowerNPCTrainerListView_OnCursor( const u32 dataID );

  private:
    enum
    {
      UPDATE_SEQ_NONE = 0,
      UPDATE_SEQ_LIST,
      UPDATE_SEQ_CONFIRM,
    };

    void _Clear()
    {
      m_pBattleTreeWork = NULL;
      m_nowDataID       = 0xffffffff;
    }

    u32 _StartSeq( u32 newSeq );

    u32 m_nowDataID;
    App::BattleTree::System::BattleTreeWork*                    m_pBattleTreeWork;

  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_FRAME_H_INCLUDE
