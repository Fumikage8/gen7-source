// ============================================================================
/*
 * @file BattleRoyalResultEventFrame.cpp
 * @brief バトルロイヤルの結果アプリのプロセスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/Frame/BattleRoyalResultEventFrame.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultFrameResult.h"

//  viewのインクルード
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewObjects.h"

#include <Fade/include/gfl2_FadeManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(Frame)


  //  画面終了の秒数定義
  const u64 BattleRoyalResultEventFrame::m_sScneeEndTimeSec = 8;

  BattleRoyalResultEventFrame::BattleRoyalResultEventFrame(
    App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork ) :
    NetAppLib::System::NetAppFrameBase( pBattleRoyalResultWork )
  {
    _Clear();

    m_pWork = pBattleRoyalResultWork;
  }


  BattleRoyalResultEventFrame::~BattleRoyalResultEventFrame()
  {
  }


  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool BattleRoyalResultEventFrame::startup()
  {
    App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    if( m_initSeq == 0 )
    {
      App::BattleRoyalResult::System::BattleRoyalResultViewList*     pViewList     = pViewSysAccessor->GetViewList();
      GFL_ASSERT( pViewList );

      //  view構築
      pViewList->RemoveAllView();

      pViewObjects->InputDisableAll();

      //  上Viewの構築
      {
        App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*  pRankingView = pViewObjects->GetUpperRankingView();
        GFL_ASSERT( pRankingView );

        pViewList->AddUpperView( pRankingView );

      }

      //  下Viewの構築
      {
        App::BattleRoyalResult::View::BattleRoyalResultLowerBGView*  pBGView = pViewObjects->GetLowerBGView();
        GFL_ASSERT( pBGView );

        pViewList->AddLowerView( pBGView );
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      //  事前開始
      App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*  pRankingView = pViewObjects->GetUpperRankingView();
      GFL_ASSERT( pRankingView );

      bool  bNextSeq  = true;
      if( pRankingView->PreStart() == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }


  bool BattleRoyalResultEventFrame::cleanup()
  {
    App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );
  
    if( m_endSeq == 0 )
    {
      bool  bNextSeq  = true;

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }


  void BattleRoyalResultEventFrame::setListener()
  {
    //  フレーム開始
    App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*  pRankingView = pViewObjects->GetUpperRankingView();
    GFL_ASSERT( pRankingView );

    pRankingView->Start();
  }


  void BattleRoyalResultEventFrame::removeListener()
  {
  }


  void BattleRoyalResultEventFrame::updateSequence()
  {
    App::BattleRoyalResult::System::BattleRoyalResultViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleRoyalResult::System::BattleRoyalResultViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    u32 seq = GetSubSeq();

    if( seq == 0 )
    {
      App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView*  pRankingView = pViewObjects->GetUpperRankingView();
      GFL_ASSERT( pRankingView );

      App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView::State  nowState  = pRankingView->GetState();
      if(nowState == App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView::STATE_IDLE )
      {
        App::BattleRoyalResult::View::BattleRoyalResultLowerBGView* pBGView = pViewObjects->GetLowerBGView();
        GFL_ASSERT( pBGView );

        // @fix GFNMCat[1670] 通信の切断対処のためにロイヤルは指定時間経過での自動終了に仕様変更になりましたので「つぎへ」ボタンを出さないようにしました。
//        pBGView->InEventBackButton();

        // @fix GFNMCat[1670] 通信の切断対処のために指定時間が経過したらＡボタンを押さなくても画面を終了するようにする
        m_sceneEndTimeLimit.Setup( m_sScneeEndTimeSec );

        ++seq;
      }
    }
    else
    if( seq == 1 )
    {
      if( m_sceneEndTimeLimit.Update() == true )
      {
        ++seq;
      }
      // @fix GFNMCat[1670] 通信の切断対処のためにロイヤルは指定時間経過での自動終了に仕様変更になりました
      /*
      else
      {
        App::BattleRoyalResult::View::BattleRoyalResultLowerBGView* pBGView = pViewObjects->GetLowerBGView();
        GFL_ASSERT( pBGView );

        if( pBGView->GetPushButtonId() == App::BattleRoyalResult::View::BattleRoyalResultLowerBGView::BUTTON_ID_NEXT )
        {
          ++seq;
        }
      }
      */
    }
    else
    {
      //  終了
      exitFrame( App::BattleRoyalResult::RESULT_BACK );
    }

    SetSubSeq( seq );
  }


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
