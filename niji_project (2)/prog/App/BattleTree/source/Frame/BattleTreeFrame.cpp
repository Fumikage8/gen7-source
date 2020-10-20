// ============================================================================
/*
 * @file BattleTreeFrame.cpp
 * @brief バトルツリーアプリのプロセスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/Frame/BattleTreeFrame.h"
#include "App/BattleTree/source/BattleTreeFrameResult.h"

//  Viewのインクルード
#include "App/BattleTree/source/System/BattleTreeViewSystemAccessor.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(Frame)


  BattleTreeFrame::BattleTreeFrame(
    App::BattleTree::System::BattleTreeWork* pBattleTreeWork ) :
    NetAppLib::System::NetAppFrameBase( pBattleTreeWork )
  {
    _Clear();

    m_pBattleTreeWork = pBattleTreeWork;
  }


  BattleTreeFrame::~BattleTreeFrame()
  {
  }


  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool BattleTreeFrame::startup()
  {
    App::BattleTree::System::ViewSystemAccessor* pViewSysAccessor  = m_pBattleTreeWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleTree::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );
    pViewObjects->InputDisableAll();

    App::BattleTree::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );
    pViewList->RemoveAllView();

    //  上Viewの構築
    {
      App::BattleTree::View::UpperBGView*  pBGView = pViewObjects->GetUpperBGView();
      GFL_ASSERT( pBGView );

      pViewList->AddUpperView( pBGView );
    }

    //  下Viewの構築
    {
      App::BattleTree::View::LowerBGView*  pBGView = pViewObjects->GetLowerBGView();
      GFL_ASSERT( pBGView );

      App::BattleTree::View::LowerNPCTrainerListView* pNPCTrainerListView = pViewObjects->GetLowerNPCTrainerListView();
      GFL_ASSERT( pNPCTrainerListView );
      pNPCTrainerListView->Start();

      pViewList->AddLowerView( pBGView );
      pViewList->AddLowerView( pNPCTrainerListView );

      //  初回カーソル位置を指定（これをしないと上画面の情報画面が数フレーム反映しない状態になる）
      LowerNPCTrainerListView_OnCursor( 0 );
    }

    return true;
  }


  bool BattleTreeFrame::cleanup()
  {
    return true;
  }


  void BattleTreeFrame::setListener()
  {
  }


  void BattleTreeFrame::removeListener()
  {
  }


  void BattleTreeFrame::updateSequence()
  {
    App::BattleTree::System::ViewSystemAccessor* pViewSysAccessor  = m_pBattleTreeWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleTree::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::BattleTree::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    u32 seq = GetSubSeq();

    if( seq == UPDATE_SEQ_NONE )
    {
      seq = _StartSeq( UPDATE_SEQ_LIST );
    }

    if( seq == UPDATE_SEQ_LIST )
    {
      App::BattleTree::View::LowerNPCTrainerListView* pNPCTrainerListView = pViewObjects->GetLowerNPCTrainerListView();
      if( pNPCTrainerListView->GetPushId() == App::BattleTree::View::LowerNPCTrainerListView::BUTTON_ID_CANCEL )
      {
        exitFrame( FRAME_RESULT_BACK );
      }
      else
      if( pNPCTrainerListView->GetPushId() == App::BattleTree::View::LowerNPCTrainerListView::BUTTON_ID_SELECT )
      {
        seq = _StartSeq( UPDATE_SEQ_CONFIRM );
      }
    }
    else
    if( seq == UPDATE_SEQ_CONFIRM )
    {
      App::BattleTree::View::LowerSelectConfirmView* pSelectConfirmListView = pViewObjects->GetLowerSelectConfirmView();
      if( pSelectConfirmListView->GetPushId() == App::BattleTree::View::LowerSelectConfirmView::BUTTON_ID_CANCEL )
      {
        seq = _StartSeq( UPDATE_SEQ_LIST );
      }
      else
      if( pSelectConfirmListView->GetPushId() == App::BattleTree::View::LowerSelectConfirmView::BUTTON_ID_NO )
      {
        seq = _StartSeq( UPDATE_SEQ_LIST );
      }
      else
      if( pSelectConfirmListView->GetPushId() == App::BattleTree::View::LowerSelectConfirmView::BUTTON_ID_YES )
      {
        //  選択した
        exitFrame( FRAME_RESULT_SELECT );
      }
    }

    SetSubSeq( seq );
  }

  //! フレームを終了します(自身の処理を終わらせます)
  void BattleTreeFrame::exitFrame( NetAppLib::System::FrameResult result )
  {
    App::BattleTree::System::ViewSystemAccessor* pViewSysAccessor  = m_pBattleTreeWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleTree::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::BattleTree::View::LowerNPCTrainerListView* pNPCTrainerListView = pViewObjects->GetLowerNPCTrainerListView();

    App::BattleTree::APP_PARAM* pAppParam = m_pBattleTreeWork->GetAppParam();
    if( result == FRAME_RESULT_SELECT )
    {
      //  選択して終了
      pAppParam->out.npcSelectIdx = pNPCTrainerListView->GetSelectDataID();
    }
    else
    {
      //  選択しないで終了
      pAppParam->out.npcSelectIdx = (u32)App::BattleTree::NPC_SELECT_IDX_ERROR;
    }

    NetAppFrameBase::exitFrame( result );
  }

  //  リストカーソル時に呼び出す
  void BattleTreeFrame::LowerNPCTrainerListView_OnCursor( const u32 dataID )
  {
    if( m_nowDataID == dataID )
    {
      return;
    }
    else
    if( m_pBattleTreeWork->GetAppParam()->in.npcSelectListNum <= dataID )
    {
      return;
    }

    m_nowDataID = dataID;

    App::BattleTree::System::ViewSystemAccessor* pViewSysAccessor  = m_pBattleTreeWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleTree::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::BattleTree::NPC_SELECT_ITEM* pSelectItem = &m_pBattleTreeWork->GetAppParam()->in.aNpcSelectItem[ dataID ];

    //  上画面の情報を更新
    {
      App::BattleTree::View::UpperBGView* pUpperBGView  = pViewObjects->GetUpperBGView();
      pUpperBGView->SetInfoText( pSelectItem->aName );
      pUpperBGView->SetPokeInfo( pSelectItem->aPokeSimpleParam[ 0 ], App::BattleTree::View::UpperBGView::POKE_INFO_POS_LEFT );
      pUpperBGView->SetPokeInfo( pSelectItem->aPokeSimpleParam[ 1 ], App::BattleTree::View::UpperBGView::POKE_INFO_POS_RIGHT );
    }
  }

  u32 BattleTreeFrame::_StartSeq( u32 newSeq )
  {
    App::BattleTree::System::ViewSystemAccessor* pViewSysAccessor  = m_pBattleTreeWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::BattleTree::System::ViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::BattleTree::System::ViewList*     pViewList     = pViewSysAccessor->GetViewList();
    GFL_ASSERT( pViewList );

    App::BattleTree::View::LowerNPCTrainerListView* pNPCTrainerListView = pViewObjects->GetLowerNPCTrainerListView();

    u32 seq = newSeq;

    if( seq == UPDATE_SEQ_LIST )
    {
      //  確認画面ははずす
      App::BattleTree::View::LowerSelectConfirmView* pSelectConfirmListView = pViewObjects->GetLowerSelectConfirmView();
      pSelectConfirmListView->RemoveFromSuperView();

      //  選択リストを復帰
      pNPCTrainerListView->SetDelegate( this );
      pNPCTrainerListView->Start();
    }
    else
    if( seq == UPDATE_SEQ_CONFIRM )
    {
      //  確認画面を出す
      App::BattleTree::View::LowerSelectConfirmView* pSelectConfirmListView = pViewObjects->GetLowerSelectConfirmView();

      if( m_pBattleTreeWork->GetAppParam()->in.npcSelectListNum <= pNPCTrainerListView->GetSelectDataID() )
      {
        GFL_ASSERT( 0 );
        exitFrame( FRAME_RESULT_BACK );
      }
      else
      {
        pSelectConfirmListView->Start( &m_pBattleTreeWork->GetAppParam()->in.aNpcSelectItem[ pNPCTrainerListView->GetSelectDataID() ] );

        pViewList->AddLowerView( pSelectConfirmListView );
      }
    }

    return  seq;
  }

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
