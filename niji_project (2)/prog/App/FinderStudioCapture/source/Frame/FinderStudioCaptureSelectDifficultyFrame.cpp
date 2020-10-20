//======================================================================
/**
 * @file    FinderStudioCaptureSelectDifficultyFrame.cpp
 * @date    2017/01/20 21:12:43
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：難易度選択フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureSelectDifficultyFrame.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureViewerManager.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureSelectButtonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePrepareCommonView.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>

#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyStatus.h>
#include <System/include/gflUse.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
SelectDifficultyFrame::SelectDifficultyFrame( void )
  : m_pUpperView( NULL )
  , m_pSelectButtonView( NULL )
  , m_pPrepareCommonView( NULL )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result SelectDifficultyFrame::InitFunc( void )
{
  GFL_PRINT( "SelectDifficultyFrame Start\n" );

  Static::ViewList* viewList = m_pMainWork->viewList;

  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView( VIEW_ID_UPPER ) );
  /// 選択ボタンView
  m_pSelectButtonView = reinterpret_cast<SelectButtonView*>( viewList->GetView( VIEW_ID_SELECT_BUTTON ) );
  /// 共通View
  m_pPrepareCommonView = reinterpret_cast<PrepareCommonView*>( viewList->GetView( VIEW_ID_PREPARE_COMMON ) );

  // 通常起動時
  if( !_IsKisekaeReversion() )
  {
    // 本格モードが開放済みか？
    if( _IsEventFlag(SYS_FLAG_FS_TUTORIAL_END) )
    {
      m_pPrepareCommonView->Show();
      m_MainSeq = SEQ_SELECT_DIFFICULTY;
    }
    else {
      // 「おてがる撮影」でViewer更新開始
      _StartViewerUpdate_Easy();
    }
  }
  // 着せ替えアプリからの復帰時
  else {
    // 「着せ替えアプリ復帰」でViewer更新開始
    _StartViewerUpdate_KisekaeReversion();
  }

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result SelectDifficultyFrame::EndFunc( void )
{
  GFL_PRINT( "SelectDifficultyFrame Finish\n" );

  // 画面非表示
  m_pSelectButtonView->Hide();

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result SelectDifficultyFrame::UpdateFunc( void )
{
#if PM_DEBUG
  if( DebugClass::IsAnyDebugMode() )
  {
    return applib::frame::RES_FINISH;
  }
#endif

  Result( SelectDifficultyFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &SelectDifficultyFrame::_UpdateSelectDifficulty,
    &SelectDifficultyFrame::_UpdateViewer,
    &SelectDifficultyFrame::_UpdateCurtainOpen,
    &SelectDifficultyFrame::_UpdateFinishConfirm,
  };

  // 各準備画面の更新
  if( ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( ) == RESULT_FINISH )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：
 */
SelectDifficultyFrame::Result SelectDifficultyFrame::_UpdateSelectDifficulty( void )
{
  enum {
    DIFF_SEQ_SHOW,
    DIFF_SEQ_MAIN,
  };

  if( m_SubSeq == DIFF_SEQ_SHOW )
  {
    // 難易度選択の表示
    _ShowSelectDifficulty();
    this->SetSubSeq( DIFF_SEQ_MAIN );
  }
  if( m_SubSeq == DIFF_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 cmnResult = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( cmnResult != PrepareCommonView::RES_CONTINUE )
    {
      if( cmnResult == PrepareCommonView::RES_BACK )
      {
        // 難易度選択を非表示
        _HideSelectDifficulty();

        this->SetMainSeq( SEQ_FINISH_CONFIRM );    //!< 準備終了選択へ
      }
      else if( cmnResult == PrepareCommonView::RES_INFO_CLOSE )
      {
        m_pSelectButtonView->SetInputEnabled( true );
      }

      return RESULT_CONTINUE;
    }

    // リザルトを取得
    u32 result = m_pSelectButtonView->GetResult();
    m_pSelectButtonView->SetResult( PREPARE_RES_CONTINUE );

    if( result != SelectButtonView::RES_CONTINUE )
    {
      // 難易度選択を非表示
      _HideSelectDifficulty();

      // ボタンを選択
      if( result == SelectButtonView::RES_SELECT_BTN_00 )
      {
        // 「おてがる撮影」でViewer更新開始
        _StartViewerUpdate_Easy();
      }
      // ボタンを選択
      else if( result == SelectButtonView::RES_SELECT_BTN_01 )
      {
        // 「本格撮影」でViewer更新開始
        _StartViewerUpdate_Authentic();
      }
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：ビューワーセットアップ
 */
SelectDifficultyFrame::Result SelectDifficultyFrame::_UpdateViewer( void )
{
  enum {
    SETUP_SEQ_START,
    SETUP_SEQ_FINISH,
    SETUP_SEQ_END,
  };

  if( m_SubSeq == SETUP_SEQ_START )
  {
    // Viewerセットアップ
    if( _SetupViewer() )
    {
      this->SetSubSeq( SETUP_SEQ_FINISH );
    }
    else {
      this->SetSubSeq( SETUP_SEQ_END );
    }
  }
  if( m_SubSeq == SETUP_SEQ_FINISH )
  {
    // Viewer更新完了待ち
    if( m_pMainWork->viewer->IsFinishUpdadteViewer() )
    {
      this->SetSubSeq( SETUP_SEQ_END );
    }
  }
  if( m_SubSeq == SETUP_SEQ_END )
  {
    this->SetMainSeq( SEQ_CURTAIN_OPEN );
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：カーテンを開く
 */
SelectDifficultyFrame::Result SelectDifficultyFrame::_UpdateCurtainOpen( void )
{
  enum {
    CURTAIN_SEQ_OPEN,
    CURTAIN_SEQ_OPEN_WAIT,
  };

  if( m_SubSeq == CURTAIN_SEQ_OPEN )
  {
    // カーテンを開く
    m_pUpperView->CurtainOpen();
    m_SubSeq = CURTAIN_SEQ_OPEN_WAIT;
  }
  if( m_SubSeq == CURTAIN_SEQ_OPEN_WAIT )
  {
    if( m_pUpperView->IsCurtainOpen() )
    {
      this->GetFrameManager()->SetResult( Frame::RES_NEXT );
      return RESULT_FINISH;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：
 */
SelectDifficultyFrame::Result SelectDifficultyFrame::_UpdateFinishConfirm( void )
{
  enum {
    FIN_SEQ_SHOW,
    FIN_SEQ_MAIN,
  };

  if( m_SubSeq == FIN_SEQ_SHOW )
  {
    // 終了確認の表示
    _ShowFinishConfirm();
    this->SetSubSeq( FIN_SEQ_MAIN );
  }
  if( m_SubSeq == FIN_SEQ_MAIN )
  {
    // リザルトを取得
    u32 result = m_pSelectButtonView->GetResult();
    m_pSelectButtonView->SetResult( PREPARE_RES_CONTINUE );

    if( result != SelectButtonView::RES_CONTINUE )
    {
      // 終了確認の表示
      _HideFinishConfirm();

      // はい
      if( result == SelectButtonView::RES_SELECT_BTN_00 )
      {
        this->GetFrameManager()->SetResult( Frame::RES_FINISH );

        return RESULT_FINISH;
      }
      // いいえ
      else if( result == SelectButtonView::RES_SELECT_BTN_01 )
      {
        this->SetMainSeq( SEQ_SELECT_DIFFICULTY );
      }
    }
  }

  return RESULT_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    画面 表示・非表示：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   難易度選択画面の表示
 */
void SelectDifficultyFrame::_ShowSelectDifficulty( void )
{
  m_pPrepareCommonView->SetInputEnabled( true );
  m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_DIFFICULTY );
  m_pPrepareCommonView->ShowTutorial();

  m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_DIFFICULTY );
  m_pSelectButtonView->Show();
}

/**
 * @brief   難易度選択画面の非表示
 */
void SelectDifficultyFrame::_HideSelectDifficulty( void )
{
  m_pPrepareCommonView->SetInputEnabled( false );
  m_pSelectButtonView->Hide();
}

/**
 * @brief   終了確認の表示
 */
void SelectDifficultyFrame::_ShowFinishConfirm( void )
{
  m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_FINISH_DIFFICULTY );

  m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_FINISH_YESNO );
  m_pSelectButtonView->Show();
}

/**
 * @brief   終了確認の非表示
 */
void SelectDifficultyFrame::_HideFinishConfirm( void )
{
  m_pSelectButtonView->Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   設定：Viewer
 *
 * @retval  "true"なら設定を行う
 * @retval  "false"なら設定を飛ばす
 */
bool SelectDifficultyFrame::_SetupViewer( void )
{
  const AppWork* appWork = &m_pMainWork->work->app;

  if( _IsKisekaeReversion() )
  {
    // 着せ替えアプリ復帰設定
    _SetupViewer_KisekaeReversion();
  }
  else if( appWork->diff_mode == Static::DIFF_MODE_EASY )
  {
    // お手軽設定
    _SetupViewer_EasyShoot();
  }
  else if( appWork->evolution_mode )
  {
    // 本格：進化モード
    _SetupViewer_EvolutionMode();
  }
  else {
    return false;
  }

  return true;
}

/**
 * @brief   設定：お手軽撮影モード
 */
void SelectDifficultyFrame::_SetupViewer_EasyShoot( void )
{
  // 共通データ
  Work* work = m_pMainWork->work;

  // Viewer更新パラメータの設定
  ViewerManager::RequestViewerParam param;
  {
    // 撮影モードの設定
    param.captureMode = Viewer::CAPTURE_MODE_BOTH;
    // 背景の設定
    if( work->app.evolution_mode )
    {
      param.bgID = Static::EVOLUTION_BACKGROUND_INDEX;
    }
    else {
      // ランダム
      _SetRandomBackGround( &param.bgID );
    }
    // 時間帯の設定
    param.timeZone = BGDataAccessor::GetDefaultTimeZone( param.bgID );
    // 着せ替えデータ
    _SetDefaultFieldDressupParam( &param.dressupParam );
    // リセット
    param.bViewerReset = true;
  }

  ViewerManager* viewer = m_pMainWork->viewer;
  {
    // 最新データの反映
    work->prepare.capture_mode  = param.captureMode;
    work->prepare.background_id = param.bgID;
    work->prepare.dressup_id    = 0;

    work->viewer.capture_mode   = param.captureMode;
    work->viewer.background_id  = param.bgID;
    work->viewer.dressup_param  = param.dressupParam;
    work->viewer.time_zone      = param.timeZone;

    // お手軽撮影モード設定の開始
    viewer->RequestViewerUpdate_All( param );
    viewer->StartUpdateViewer();
  }
}

/**
 * @brief   設定：進化撮影モード
 */
void SelectDifficultyFrame::_SetupViewer_EvolutionMode( void )
{
  // 共通データ
  Work* work = m_pMainWork->work;

  // Viewer更新パラメータの設定
  ViewerManager::RequestViewerParam param;
  {
    // 背景の設定
    param.bgID = Static::EVOLUTION_BACKGROUND_INDEX;
    // 時間帯の設定
    param.timeZone = BGDataAccessor::GetDefaultTimeZone( param.bgID );
  }

  // 最新データの反映
  ViewerManager* viewer = m_pMainWork->viewer;
  {
    // 最新データの反映
    work->prepare.background_id = param.bgID;

    work->viewer.background_id = param.bgID;
    work->viewer.time_zone     = param.timeZone;

    // 進化撮影モード設定の開始
    viewer->RequestViewerUpdate_All( param );
    viewer->StartUpdateViewer();
  }
}

/**
 * @brief   設定：着せ替えアプリ復帰
 */
void SelectDifficultyFrame::_SetupViewer_KisekaeReversion( void )
{
  // 共通データ
  Work* work = m_pMainWork->work;

  // Viewer更新パラメータの設定
  ViewerManager::RequestViewerParam param;
  {
    /// 衣装データ
    param.dressupParam = work->viewer.dressup_param;
  }

  // 最新データの反映
  ViewerManager* viewer = m_pMainWork->viewer;
  {
    // 着せ替えアプリ復帰モード設定の開始
    viewer->RequestViewerUpdate_All( param );
    viewer->StartUpdateViewer();
  }
}

/**
 * @brief   設定：ランダム背景
 *
 * @param[out]  dstBGIndex    格納先
 */
void SelectDifficultyFrame::_SetRandomBackGround( u32* dstBGIndex )
{
#if PM_DEBUG
  // 背景開放チェック
  _CheckBackGroundUnlock();
#endif

  u32 backGroundID = BG_ID_NUM;

  while( backGroundID == BG_ID_NUM )
  {
    // ランダムで背景インデックスを取得
    backGroundID = System::GflUse::GetPublicRand( BG_ID_NUM );

    FUKUSHIMA_PRINT( "BackGroundID=[%d]\n", backGroundID );

    // 使用可能かを判定
    if( !BGDataAccessor::IsUnlock( backGroundID ) )
    {
      backGroundID = BG_ID_NUM;
    }
    // 世代跨ぎ背景で尚且つ選択中のポケモンの世代と違う   // @fix MMCat[723]
    else if( _IsGenerationBackGround( backGroundID ) && !_IsCompareGenerationBackGround( backGroundID ) )
    {
      backGroundID = BG_ID_NUM;
    }
  }

  // 選ばれた背景インデックスを格納
  *dstBGIndex = backGroundID;

  FUKUSHIMA_PRINT( "RandomSelect BackGroundID=[%d]\n", backGroundID );
}

/**
 * @brief   設定：フィールド衣装
 *
 * @param[out]  dstDressupParam   格納先
 */
void SelectDifficultyFrame::_SetDefaultFieldDressupParam( poke_3d::model::DressUpParam* dstDressupParam )
{
  GameSys::GameData* gd = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus* myStatus = gd->GetPlayerStatus();

  // フィールド上での着せ替えデータを設定
  *dstDressupParam = myStatus->GetDressup();
}


/**
 * @brief   「おてがる撮影」でViewer更新を開始
 */
void SelectDifficultyFrame::_StartViewerUpdate_Easy( void )
{
  // 「おてがる撮影」
  m_pMainWork->work->app.diff_mode = Static::DIFF_MODE_EASY;
  // Viewerセットアップシーケンスへ
  this->SetMainSeq( SEQ_SETUP_VIEWER );
}

/**
 * @brief   「本格撮影」でViewer更新を開始
 */
void SelectDifficultyFrame::_StartViewerUpdate_Authentic( void )
{
  // 「本格撮影」
  m_pMainWork->work->app.diff_mode = Static::DIFF_MODE_AUTHENTIC;
  // Viewerセットアップシーケンスへ
  this->SetMainSeq( SEQ_SETUP_VIEWER );
}

/**
 * @brief   「着せ替えアプリ復帰」でViewer更新を開始
 */
void SelectDifficultyFrame::_StartViewerUpdate_KisekaeReversion( void )
{
  // Viewerセットアップシーケンスへ
  this->SetMainSeq( SEQ_SETUP_VIEWER );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   チェック：イベントフラグ
 *
 * @retval  "true"ならフラグ開放済み
 * @retval  "false"ならそれ以外
 */
bool SelectDifficultyFrame::_IsEventFlag( u32 flg )
{
  // イベントワークの取得
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Field::EventWork* eventWork = gameData->GetEventWork();

  // イベントワークのチェック
  return eventWork->CheckEventFlag( flg );
}

/**
 * @brief   チェック：着せ替えアプリ復帰か
 *
 * @retval  "true"ならアプリ復帰
 * @retval  "false"なら通常起動
 */
bool SelectDifficultyFrame::_IsKisekaeReversion( void )
{
  return ( m_pMainWork->work->app.start_frame_id == Frame::FRAME_ID_SELECT_DIFFICULTY );
}

/**
 * @brief   チェック：世代跨ぎ背景かどうか
 *
 * @param[in]   bgID    チェック対象背景ID
 *
 * @retval  "true"なら世代跨ぎ背景
 * @retval  "false"ならそれ以外
 *
 * @fix MMCat[723]
 */
bool SelectDifficultyFrame::_IsGenerationBackGround( u32 bgID )
{
  return BGDataAccessor::IsGenerationBackGround( bgID );
}

/**
 * @brief   チェック：選択中のポケモンの世代跨ぎ背景と一致しているか？
 *
 * @param[in]   bgID    チェック対象背景ID
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool SelectDifficultyFrame::_IsCompareGenerationBackGround( u32 bgID )
{
  pml::pokepara::PokemonParam* pp = m_pMainWork->work->prepare.pokemon_param;

  /// ポケモンのカセットバージョンを取得
  u32 version = BGDataAccessor::GetPokemonVersion( pp );

  /// 世代跨ぎ背景と対象の背景IDを比較
  return BGDataAccessor::IsCompareGenrationBackGround( version, bgID );
}

#if PM_DEBUG
/**
 * @brief   デバッグ：背景が１つでも開放されているかをチェック
 *
 * @note    通常プレイで背景が１つも開放されていないということは無いが、
 *          デバッグプレイ中は１つも開放されていないことがあるためここでチェック
 */
void SelectDifficultyFrame::_CheckBackGroundUnlock( void )
{
  u32 unlockCount = 0;

  for( u32 i = 0; i < BG_ID_NUM; ++i )
  {
    // 背景のアンロックをチェック
    if( BGDataAccessor::IsUnlock( i ) )
    {
      unlockCount++;
    }
  }

  GFL_PRINT( "UnlockBackGroundCount=[%d]\n", unlockCount );
  GFL_ASSERT( unlockCount > 0 );
}
#endif

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
