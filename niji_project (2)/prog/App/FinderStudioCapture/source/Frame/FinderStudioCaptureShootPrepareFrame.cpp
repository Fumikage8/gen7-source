//======================================================================
/**
 * @file    FinderStudioCaptureShootPrepareFrame.cpp
 * @date    2016/11/09 20:33:26
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影準備フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureShootPrepareFrame.h"

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureBGListView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePokeListView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureDressupListView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureCompositionView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureSelectButtonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePrepareSelectView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePrepareCommonView.h>

#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>

#include <niji_conv_header/message/msg_fs_photo.h>


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
ShootPrepareFrame::ShootPrepareFrame( void )
  : m_pUpperView( NULL )
  , m_pPokeListView( NULL )
  , m_pBGListView( NULL )
  , m_pDressUpView( NULL )
  , m_pSelectButtonView( NULL )
  , m_pCompositionView( NULL )
  , m_pPrepareSelectView( NULL )
  , m_pPrepareCommonView( NULL )
  , m_CurtainSeq( CURTAIN_SEQ_IDLE )
  , m_PrevSeq( SEQ_NUM )
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
applib::frame::Result ShootPrepareFrame::InitFunc( void )
{
  GFL_PRINT( "ShootPrepareFrame Start\n" );

  Static::ViewList* viewList = m_pMainWork->viewList;

  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView( VIEW_ID_UPPER ) );
  /// ポケモン選択View
  m_pPokeListView = reinterpret_cast<PokeListView*>( viewList->GetView( VIEW_ID_POKE_LIST ) );
  /// 背景リストView
  m_pBGListView = reinterpret_cast<BGListView*>( viewList->GetView( VIEW_ID_BG_LIST ) );
  /// 衣装選択View
  m_pDressUpView = reinterpret_cast<DressupListView*>( viewList->GetView( VIEW_ID_DRESSUP_LIST ) );
  /// 選択ボタンView
  m_pSelectButtonView = reinterpret_cast<SelectButtonView*>( viewList->GetView( VIEW_ID_SELECT_BUTTON ) );
  /// 構図選択View
  m_pCompositionView = reinterpret_cast<CompositionView*>( viewList->GetView( VIEW_ID_COMPOSITION ) );
  /// 準備選択View
  m_pPrepareSelectView = reinterpret_cast<PrepareSelectView*>( viewList->GetView( VIEW_ID_PREPARE_SELECT ) );
  /// 共通View
  m_pPrepareCommonView = reinterpret_cast<PrepareCommonView*>( viewList->GetView( VIEW_ID_PREPARE_COMMON ) );
  m_pPrepareCommonView->Show();
  /// 汎用メッセージウィンドウView
  m_pMsgMenuView = viewList->GetMessageMenuView();

  Work* work = m_pMainWork->work;

  // 通常起動時
  if( work->app.start_frame_id != Frame::FRAME_ID_SELECT_DIFFICULTY )
  {
    m_MainSeq = SEQ_SELECT_PREPARE;
  }
  // 着せ替えアプリからの復帰時
  else {
    work->app.start_frame_id = Frame::FRAME_ID_SDCARD_LOAD;
    m_MainSeq = SEQ_SELECT_DRESSUP;
  }

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result ShootPrepareFrame::EndFunc( void )
{
  GFL_PRINT( "ShootPrepareFrame Finish\n" );

  m_pMsgMenuView->RemoveListener();

  // 画面非表示
  m_pPokeListView->Hide();
  m_pBGListView->Hide();
  m_pDressUpView->Hide();
  m_pSelectButtonView->Hide();
  m_pCompositionView->Hide();
  m_pPrepareSelectView->Hide();

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
applib::frame::Result ShootPrepareFrame::UpdateFunc( void )
{
#if PM_DEBUG
  if( DebugClass::IsAnyDebugMode() )
  {
    return applib::frame::RES_FINISH;
  }
#endif

  Result( ShootPrepareFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &ShootPrepareFrame::_UpdateSelectPrepare,
    &ShootPrepareFrame::_UpdateSelectCaptureMode,
    &ShootPrepareFrame::_UpdateSelectPokemon,
    &ShootPrepareFrame::_UpdateSelectBackGround,
    &ShootPrepareFrame::_UpdateSelectDressup,
    &ShootPrepareFrame::_UpdateSelectComposition,
    &ShootPrepareFrame::_UpdateSelectFinishConfirm,
    &ShootPrepareFrame::_UpdateCurtainClose,
  };

  // 各準備画面の更新
  if( ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( ) == RESULT_FINISH )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：カーテン演出
 *
 * @note    着せ替えアプリからの復帰でのみここの更新をする
 */
void ShootPrepareFrame::_UpdateCurtainOpen( void )
{
  if( m_CurtainSeq == CURTAIN_SEQ_OPEN )
  {
    // カーテンを開く
    m_pUpperView->CurtainOpen();
    m_CurtainSeq = CURTAIN_SEQ_OPEN_WAIT;
  }
  if( m_CurtainSeq == CURTAIN_SEQ_OPEN_WAIT )
  {
    if( m_pUpperView->IsCurtainOpen() )
    {
      m_CurtainSeq = CURTAIN_SEQ_IDLE;
    }
  }
}

/**
 * @brief   更新：準備選択画面
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectPrepare( void )
{
  enum {
    PRE_SEQ_SHOW,
    PRE_SEQ_MAIN,
  };

  if( m_SubSeq == PRE_SEQ_SHOW )
  {
    m_pPrepareSelectView->SetDifficulty( m_pMainWork->work->app.diff_mode );
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_PREPARE );
    // 準備選択の表示
    _ShowPrepareSelect();
    this->SetSubSeq( PRE_SEQ_MAIN );
  }
  if( m_SubSeq == PRE_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 cmnResult = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( cmnResult != PrepareCommonView::RES_CONTINUE )
    {
      if( cmnResult == PrepareCommonView::RES_BACK )
      {
        // 準備選択を非表示
        _HidePrepareSelect();

        this->SetMainSeq( SEQ_SELECT_FINISH_CONFIRM );    //!< 準備終了選択へ
      }
      else if( cmnResult == PrepareCommonView::RES_NEXT )
      {
        // 準備選択を非表示
        _HidePrepareSelect();
        m_pPrepareCommonView->Hide(); //!< 撮影メインの時は非表示にする

        this->GetFrameManager()->SetResult( Frame::RES_NEXT );
        return RESULT_FINISH;
      }
      else if( cmnResult == PrepareCommonView::RES_INFO_CLOSE )
      {
        m_pPrepareSelectView->SetInputEnabled( true );
      }

      return RESULT_CONTINUE;
    }

    // リザルトを取得
    u32 result = m_pPrepareSelectView->GetResult();
    m_pPrepareSelectView->SetResult( PREPARE_RES_CONTINUE );

    if( result != PrepareSelectView::RES_CONTINUE )
    {
      // ボタンが選択された  @fix MMCat[108]
      if( result == PrepareSelectView::RES_SELECTED_ITEM )
      {
        // 入力を無効にしておく
        m_pPrepareCommonView->SetInputEnabled( false );
        return RESULT_CONTINUE;
      }

      // 準備選択を非表示
      _HidePrepareSelect();

      // ボタンを選択
      if( result == PrepareSelectView::RES_CAPTURE_MODE )
      {
        this->SetMainSeq( SEQ_SELECT_CAPTURE_MODE );      //!< 撮影モード選択へ
      }
      // ボタンを選択
      else if( result == PrepareSelectView::RES_POKEMON )
      {
        this->SetMainSeq( SEQ_SELECT_POKEMON );      //!< ポケモン選択へ
      }
      // ボタンを選択
      else if( result == PrepareSelectView::RES_BACKGROUND )
      {
        this->SetMainSeq( SEQ_SELECT_BACKGROUND );      //!< 背景選択へ
      }
      // ボタンを選択
      else if( result == PrepareSelectView::RES_DRESSUP )
      {
        this->SetMainSeq( SEQ_SELECT_DRESSUP );      //!< 衣装選択へ
      }
      // ボタンを選択
      else if( result == PrepareSelectView::RES_COMPOSITION )
      {
        this->SetMainSeq( SEQ_SELECT_COMPOSITION );      //!< 構図選択へ
      }
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：撮影モード選択
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectCaptureMode( void )
{
  enum {
    CAP_SEQ_INIT,
    CAP_SEQ_SHOW,
    CAP_SEQ_MAIN,
  };

  if( m_SubSeq == CAP_SEQ_INIT )
  {
    m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_CAPTURE_MODE );
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_CAPTURE_MODE );
    this->SetSubSeq( CAP_SEQ_SHOW );
    return RESULT_CONTINUE;
  }
  if( m_SubSeq == CAP_SEQ_SHOW )
  {
    // 撮影モード選択の表示
    _ShowSelectButton();
    this->SetSubSeq( CAP_SEQ_MAIN );
  }
  if( m_SubSeq == CAP_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 cmnResult = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( cmnResult != PrepareCommonView::RES_CONTINUE )
    {
      if( cmnResult == PrepareCommonView::RES_BACK )
      {
        // 撮影モード選択を非表示
        _HideSelectButton();

        this->SetMainSeq( SEQ_SELECT_PREPARE );    //!< 準備終了選択へ
      }

      return RESULT_CONTINUE;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：ポケモン選択
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectPokemon( void )
{
  enum {
    POKE_SEQ_SHOW,
    POKE_SEQ_MAIN,
  };

  if( m_SubSeq == POKE_SEQ_SHOW )
  {
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_POKEMON );
    // ポケモン選択の表示
    _ShowPokeList();
    this->SetSubSeq( POKE_SEQ_MAIN );
  }
  if( m_SubSeq == POKE_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 cmnResult = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( cmnResult != PrepareCommonView::RES_CONTINUE )
    {
      if( cmnResult == PrepareCommonView::RES_BACK )
      {
        // ポケモン選択を非表示
        _HidePokeList();

        this->SetMainSeq( SEQ_SELECT_PREPARE );    //!< 準備終了選択へ
      }

      return RESULT_CONTINUE;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：背景選択
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectBackGround( void )
{
  enum {
    BG_SEQ_SHOW,
    BG_SEQ_MAIN,
  };

  if( m_SubSeq == BG_SEQ_SHOW )
  {
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_BACKGROUND );
    // 背景選択の表示
    _ShowBGList();
    this->SetSubSeq( BG_SEQ_MAIN );
  }
  if( m_SubSeq == BG_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 cmnResult = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( cmnResult != PrepareCommonView::RES_CONTINUE )
    {
      if( cmnResult == PrepareCommonView::RES_BACK )
      {
        // 背景選択を非表示
        _HideBGList();

        this->SetMainSeq( SEQ_SELECT_PREPARE );    //!< 準備終了選択へ
      }
      else if( cmnResult == PrepareCommonView::RES_INFO_CLOSE )
      {
        m_pBGListView->SetInputEnabled( true );
      }

      return RESULT_CONTINUE;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：衣装選択
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectDressup( void )
{
  enum {
    DRESSUP_SEQ_SHOW,
    DRESSUP_SEQ_MAIN,
  };

  if( m_SubSeq == DRESSUP_SEQ_SHOW )
  {
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_DRESSUP );
    // 衣装選択の表示
    _ShowDressupList();
    this->SetSubSeq( DRESSUP_SEQ_MAIN );
  }
  if( m_SubSeq == DRESSUP_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    u32 result = m_pPrepareCommonView->GetResult();
    m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

    if( result != PrepareCommonView::RES_CONTINUE )
    {
      if( result == PrepareCommonView::RES_BACK )
      {
        // 衣装選択を非表示
        _HideDressupList();

        this->SetMainSeq( SEQ_SELECT_PREPARE );    //!< 準備終了選択へ
      }
      // 着せ替えアプリを起動
      if( result == PrepareCommonView::RES_APP_KISEKAE )
      {
        m_pMainWork->work->app.call_kisekae = true;
        m_pMainWork->work->app.start_frame_id = Frame::FRAME_ID_SELECT_DIFFICULTY;
        m_pMainWork->work->prepare.dressup_id = m_pMainWork->work->app.dressup_save_index + 1;

        this->GetFrameManager()->SetResult( Frame::RES_FINISH );

        return RESULT_FINISH;
      }
      else if( result == PrepareCommonView::RES_INFO_CLOSE )
      {
        m_pDressUpView->SetInputEnabled( true );
      }

      return RESULT_CONTINUE;
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：構図選択
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectComposition( void )
{
  enum {
    COMP_SEQ_SHOW,
    COMP_SEQ_MAIN,
  };

  if( m_SubSeq == COMP_SEQ_SHOW )
  {
    // 構図選択の表示
    _ShowCompositionSelect();
    this->SetSubSeq( COMP_SEQ_MAIN );
  }
  if( m_SubSeq == COMP_SEQ_MAIN )
  {
    // 共通ビューのリザルト
    {
      u32 cmnResult = m_pPrepareCommonView->GetResult();
      m_pPrepareCommonView->SetResult( PREPARE_RES_CONTINUE );

      if( cmnResult != PrepareCommonView::RES_CONTINUE )
      {
        if( cmnResult == PrepareCommonView::RES_BACK )
        {
          // 構図選択を非表示
          _HideCompositionSelect();

          this->SetMainSeq( SEQ_SELECT_PREPARE );    //!< 準備終了選択へ
        }

        return RESULT_CONTINUE;
      }
    }

    // 構図選択Viewのリザルト
    {
      u32 result = m_pCompositionView->GetResult();
      m_pCompositionView->SetResult( CompositionView::RES_CONTINUE );

      if( result == CompositionView::RES_ENABLE )
      {
        m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_COMPOSITION );
      }
      else if( result == CompositionView::RES_DISABLE )
      {
        m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_COMPOSITION_DISABLE );
      }
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：終了確認
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateSelectFinishConfirm( void )
{
  enum {
    FIN_SEQ_INIT,
    FIN_SEQ_SHOW,
    FIN_SEQ_MAIN,
  };

  if( m_SubSeq == FIN_SEQ_INIT )
  {
    if( _IsEventFlag( SYS_FLAG_FS_TUTORIAL_END ) )
    {
      m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_FINISH_PREPARE );
    }
    else {
      m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_FINISH_PREPARE_LOCK );
    }
    m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_FINISH_PREPARE );

    this->SetSubSeq( FIN_SEQ_SHOW );
    return RESULT_CONTINUE;
  }
  if( m_SubSeq == FIN_SEQ_SHOW )
  {
    // 終了確認の表示
    _ShowSelectButton();
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
      _HideSelectButton();

      // つづける
      if( result == SelectButtonView::RES_SELECT_BTN_00 )
      {
        this->SetMainSeq( SEQ_SELECT_PREPARE );
      }
      // 難易度選択へ：カーテンを閉じる
      else if( result == SelectButtonView::RES_SELECT_BTN_01 )
      {
        this->SetMainSeq( SEQ_CURTAIN_CLOSE );
      }
      // 終わる
      else if( result == SelectButtonView::RES_SELECT_BTN_02 )
      {
        this->GetFrameManager()->SetResult( Frame::RES_FINISH );

        return RESULT_FINISH;
      }
    }
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   更新：カーテンを閉じる
 */
ShootPrepareFrame::Result ShootPrepareFrame::_UpdateCurtainClose( void )
{
  enum {
    CURTAIN_SEQ_CLOSE,
    CURTAIN_SEQ_CLOSE_WAIT,
  };

  if( m_SubSeq == CURTAIN_SEQ_CLOSE )
  {
    m_pUpperView->CurtainClose( false );
    this->SetSubSeq( CURTAIN_SEQ_CLOSE_WAIT );
  }
  if( m_SubSeq == CURTAIN_SEQ_CLOSE_WAIT )
  {
    if( m_pUpperView->IsCurtainClose() )
    {
      this->GetFrameManager()->SetResult( Frame::RES_PREV );
      return RESULT_FINISH;
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
 * @brief   準備選択画面の表示
 */
void ShootPrepareFrame::_ShowPrepareSelect( void )
{
  /// 入力を有効に
  m_pPrepareCommonView->SetInputEnabled( true );
  /// チュートリアル
  m_pPrepareCommonView->ShowTutorial();

  /// 準備選択画面の表示
  m_pPrepareSelectView->Show();
}

/**
 * @brief   準備選択画面の非表示
 */
void ShootPrepareFrame::_HidePrepareSelect( void )
{
  m_pPrepareSelectView->Hide();
}

/**
 * @brief   ポケモンリストの表示
 */
void ShootPrepareFrame::_ShowPokeList( void )
{
  m_pPrepareCommonView->SetInputEnabled( true );
  m_pPokeListView->Show();
}

/**
 * @brief   ポケモンリストの非表示
 */
void ShootPrepareFrame::_HidePokeList( void )
{
  m_pPokeListView->Hide();
}

/**
 * @brief   背景リストの表示
 */
void ShootPrepareFrame::_ShowBGList( void )
{
  /// 入力を有効に
  m_pPrepareCommonView->SetInputEnabled( true );
  /// チュートリアル
  m_pPrepareCommonView->ShowTutorial();

  m_pBGListView->Show();
}

/**
* @brief   背景リストの非表示
*/
void ShootPrepareFrame::_HideBGList( void )
{
  m_pBGListView->Hide();
}

/**
* @brief   衣装選択の表示
*/
void ShootPrepareFrame::_ShowDressupList( void )
{
  /// 入力を有効に
  m_pPrepareCommonView->SetInputEnabled( true );
  /// チュートリアル
  m_pPrepareCommonView->ShowTutorial();

  m_pDressUpView->Show();
}

/**
* @brief   衣装選択の非表示
*/
void ShootPrepareFrame::_HideDressupList( void )
{
  m_pDressUpView->Hide();
}

/**
* @brief   構図選択の表示
*/
void ShootPrepareFrame::_ShowCompositionSelect( void )
{
  m_pPrepareCommonView->SetInputEnabled( true );
  m_pCompositionView->Show();
}

/**
* @brief   構図選択の非表示
*/
void ShootPrepareFrame::_HideCompositionSelect( void )
{
  m_pCompositionView->Hide();
}

/**
 * @brief   ボタン選択画面の表示
 *
 * @note    難易度選択
 *          撮影モード選択
 *          終了確認
 */
void ShootPrepareFrame::_ShowSelectButton( void )
{
  /// 入力を有効に
  m_pPrepareCommonView->SetInputEnabled( true );
  /// チュートリアル
  m_pPrepareCommonView->ShowTutorial();

  m_pSelectButtonView->Show();
}

/**
 * @brief   ボタン選択画面の非表示
 *
 * @note    難易度選択
 *          撮影モード選択
 *          終了確認
 */
void ShootPrepareFrame::_HideSelectButton( void )
{
  m_pSelectButtonView->Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   チェック：イベントフラグ
 *
 * @retval  "true"ならフラグ開放済み
 * @retval  "false"ならそれ以外
 */
bool ShootPrepareFrame::_IsEventFlag( u32 flg )
{
  // イベントワークの取得
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Field::EventWork* eventWork = gameData->GetEventWork();

  // イベントワークのチェック
  return eventWork->CheckEventFlag( flg );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
