//======================================================================
/**
 * @file    FinderStudioCaptureZMotionView.cpp
 * @date    2016/11/28 14:58:54
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：Zモーション選択View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureZMotionView.h"
#include "FinderStudioCaptureViewListener.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyItemSave.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>
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
ZMotionView::ZMotionView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_pCursorManager( NULL )
  , m_pCursorMoveData( NULL )
  , m_MotionType( Viewer::Z_MOTION_TYPE_MAX )
  , m_ReqMotionType( Viewer::Z_MOTION_TYPE_MAX )
  , m_MainSeq( SEQ_IDLE )
  , m_SubSeq( 0 )
  , m_ButtonNum( 0 )
{
  for( u32 i = 0; i < MOTION_NUM; ++i )
  {
    m_pCursorPane[i] = NULL;
    m_ButtonList[i] = PlayerMotionData::GetDefaultMotionButton();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化：公開
 */
void ZMotionView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // モーションデータの初期化
  _InitMotionData();
  // カーソル移動データの初期化
  _InitCursorMoveData();
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ボタンの初期化
  InitButton();
  // テキストの初期化
  InitText();
  // ペインの初期化
  InitPane();
  // アニメの初期化
  InitAnim();
}

/**
 * @brief   2Dの初期化：非公開
 */
void ZMotionView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   初期化：ペイン
 */
void ZMotionView::InitPane( void )
{
  const u32 c_TypeIconParts[] = {
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_00,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_01,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_02,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_03,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_04,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_05,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_06,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_07,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_08,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_09,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_10,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_11,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_12,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_13,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_14,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_15,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_16,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_17,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_18,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_19,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_TypeIconParts ); ++i )
  {
    /// ボタン：部品ペイン
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_TypeIconParts[i] );
    /// ボタン：タイプアイコン用ペイン
    gfl2::lyt::LytParts* typeIcon = m_pUtil->GetParts( PANENAME_STUDIOPARTS_BUTTON_06_PANE_TYPEICON, parts );
    /// ボタン：アローラアイコン用ペイン
    gfl2::lyt::LytPane*  aloraIcon = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_06_PANE_ALORAICON, parts );
    /// ボタン：ピカチュウZアイコン用ペイン
    gfl2::lyt::LytPane*  pikaIcon = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_06_PANE_PIKAICON, parts );
    /// ボタン：カーソル用ペイン
    m_pCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_06_PANE_ICON_WAKU, parts );

    PlayerMotionData::MotionType type = m_ButtonList[i].motion_type;

    if( type == PlayerMotionData::MOTION_TYPE_ZMOTION )
    {
      gfl2::lyt::LytPicture* pic = m_pUtil->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, typeIcon );
      m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( m_ButtonList[i].poke_type, pic );
    }

    m_pUtil->SetVisiblePane( typeIcon, ( type == PlayerMotionData::MOTION_TYPE_ZMOTION ) );
    m_pUtil->SetVisiblePane( aloraIcon, ( type == PlayerMotionData::MOTION_TYPE_ALORA ) );
    m_pUtil->SetVisiblePane( pikaIcon, ( type == PlayerMotionData::MOTION_TYPE_PIKA_Z ) );
  }
}

/**
 * @brief   初期化：テキスト
 */
void ZMotionView::InitText( void )
{
  // ガイドテキスト
  gfl2::lyt::LytParts* guideParts = m_pUtil->GetParts( PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_MESSAGE_L1 );
  gfl2::lyt::LytTextBox* guideText = m_pUtil->GetTextBox( PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, guideParts );

  SetTextboxPaneMessage( guideText, fs_photo_cap_13 );
  m_pUtil->SetVisiblePane( guideText, true );

  // 決定ボタンテキスト
  gfl2::lyt::LytParts* okayParts = m_pUtil->GetParts( PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_OKAYBUTTON );
  gfl2::lyt::LytTextBox* okayText = m_pUtil->GetTextBox( PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, okayParts );

  SetTextboxPaneMessage( okayText, fs_photo_cap_14 );
}

/**
 * @brief   初期化：アニメ
 */
void ZMotionView::InitAnim( void )
{
  // 背景アニメの再生
  m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__BG_00_BG_LOOP_01 );
}

/**
 * @brief   初期化：ボタン
 */
void ZMotionView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_05_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_06_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_07_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_08_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_09_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_10_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_11_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_12_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_13_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_14_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_15_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_16_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_17_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_18_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__TYPEBUTTON_19_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__BACKBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PLAYERMOTIONSELECT_LOW_00__OKAYBUTTON_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_00,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_01,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_02,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_03,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_04,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_05,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_06,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_07,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_08,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_09,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_10,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_11,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_12,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_13,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_14,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_15,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_16,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_17,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_18,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_TYPEBUTTON_19,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_BACKBUTTON,
    PANENAME_STUDIO_PLAYERMOTIONSELECT_LOW_00_PANE_OKAYBUTTON,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_06_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
    gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );

    info[i].picture_pane = parts;
    info[i].bound_pane = bound;
  }

  // ボタンマネージャの生成
  CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  {
    // Bind
    btnMan->SetButtonBindKey( BTN_ID_BACK, gfl2::ui::BUTTON_B );
    btnMan->SetButtonBindKey( BTN_ID_DECIDE, gfl2::ui::BUTTON_Y );

    // SE
    for( u32 i = 0; i < BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
    btnMan->SetButtonSelectSE( BTN_ID_BACK, SEQ_SE_CANCEL1 );

    // Visible
    for( u32 i = 0; i < MOTION_NUM; ++i )
    {
      if( m_ButtonList[i].motion_type != PlayerMotionData::MOTION_TYPE_NULL )
      {
        btnMan->SetButtonVisible( BTN_ID_MOTION_00 + i );
      }
      else {
        btnMan->SetButtonInvisible( BTN_ID_MOTION_00 + i );
      }
    }
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   破棄：公開
 */
bool ZMotionView::Terminate( void )
{
  GFL_SAFE_DELETE( m_pCursorManager );
  GFL_SAFE_DELETE_ARRAY( m_pCursorMoveData );

  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：公開
 */
void ZMotionView::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス更新
  _UpdateSequence();

  // モーション更新
  if( m_MainSeq == SEQ_MAIN )
  {
    _UpdateTrainerMotion();
  }
}

/**
 * @brief   更新：シーケンス
 */
void ZMotionView::_UpdateSequence( void )
{
  switch( m_MainSeq )
  {
    case SEQ_SHOW_MOTION_RESET:
    case SEQ_HIDE_MOTION_RESET:
    {
      // モーションフレームのリセット
      _ResetMotion();
      m_MainSeq++;
    } break;

    case SEQ_SHOW_MOTION_RESET_END:
    case SEQ_HIDE_MOTION_RESET_END:
    {
      // Viewer更新の完了チェック
      if( !m_pListener->IsUpdateViewer() )
      {
        break;
      }

      if( m_MainSeq == SEQ_SHOW_MOTION_RESET_END )
      {
        this->SetInputEnabled( true );
        m_MainSeq = SEQ_MAIN;
      }
      if( m_MainSeq == SEQ_HIDE_MOTION_RESET_END )
      {
        this->SetResult( RES_CLOSE );
        m_MainSeq = SEQ_IDLE;
      }
    } // no break;
  }
}

/**
 * @brief   更新：トレーナーモーション
 */
void ZMotionView::_UpdateTrainerMotion( void )
{
  enum {
    SEQ_MOTION_IDLE,
    SEQ_MOTION_CHECK,
    SEQ_MOTION_RESET,
    SEQ_MOTION_RESET_END,
    SEQ_MOTION_PLAY,
  };

  if( m_SubSeq == SEQ_MOTION_IDLE )
  {
    if( m_ReqMotionType != Viewer::Z_MOTION_TYPE_MAX )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == SEQ_MOTION_CHECK )
  {
    /// 更新中ではなくモーション再生中だったならリセット
    if( !_IsUpdateViewer() && _IsPlayingMotion() )
    {
      m_SubSeq = SEQ_MOTION_RESET;
    }
    else if( _IsUpdateViewer() )
    {
      m_SubSeq = SEQ_MOTION_RESET_END;
    }
    /// そうでなければモーション再生
    else {
      m_SubSeq = SEQ_MOTION_PLAY;
    }
  }
  if( m_SubSeq == SEQ_MOTION_RESET )
  {
    // モーションリセット
    _ResetMotion();
    m_SubSeq = SEQ_MOTION_RESET_END;
  }
  if( m_SubSeq == SEQ_MOTION_RESET_END )
  {
    /// 更新が完了したか？
    if( !_IsUpdateViewer() )
    {
      m_SubSeq = SEQ_MOTION_PLAY;
    }
  }
  if( m_SubSeq == SEQ_MOTION_PLAY )
  {
    /// モーション再生
    _PlayMotion( m_ReqMotionType );

    m_ReqMotionType = Viewer::Z_MOTION_TYPE_MAX;
    m_SubSeq = SEQ_MOTION_IDLE;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void ZMotionView::Show( void )
{
  // カーソルの初期化
  _InitCursor();

  // 表示
  this->_Show();

  m_MainSeq = SEQ_SHOW_MOTION_RESET;
}

/**
 * @brief   非表示
 */
void ZMotionView::Hide( void )
{
  this->SetModal( false );
  this->SetInputEnabled( false );
  this->SetVisible( false );
}

/**
 * @brief   View非表示の開始
 */
void ZMotionView::StartHideView( void )
{
  m_MainSeq = SEQ_HIDE_MOTION_RESET;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    モーション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   モーション：データの初期化
 */
void ZMotionView::_InitMotionData( void )
{
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyItem* itemSave = gameData->GetMyItem();

  u32 localIndex = 0;

  for( u32 i = 0; i < MOTION_NUM; ++i )
  {
    PlayerMotionData::MotionButton data = PlayerMotionData::GetMotionButton( i );

    if( data.itemNo != ITEM_DUMMY_DATA )
    {
      // 道具の有無をチェック
      if( itemSave->Check( data.itemNo, 1 ) )
      {
        m_ButtonList[localIndex] = data;
        localIndex++;
      }
    }
    // ITEM_DUMMY_DATAの場合は道具の有無に関わらず追加
    else {
      m_ButtonList[localIndex] = data;
      localIndex++;
    }
  }

  m_ButtonNum = localIndex;
}

/**
 * @brief   モーション：再生
 *
 * @param[in]   type    モーションタイプ
 */
void ZMotionView::_PlayMotion( Viewer::ZMotionType type )
{
  GFL_ASSERT( m_pListener );

  m_pListener->TrainerMotionStart( type );
}

/**
 * @brief   モーション：リセット
 */
void ZMotionView::_ResetMotion( void )
{
  GFL_ASSERT( m_pListener );

  m_pListener->TrainerMotionReset();
}

/**
 * @brief   モーション：再生中か？
 *
 * @retval  "true"なら再生中
 * @retval  "false"ならそれ以外
 */
bool ZMotionView::_IsPlayingMotion( void )
{
  GFL_ASSERT( m_pListener );

  return m_pListener->IsPlayingTrainerMotion();
}

/**
 * @brief   Viewer更新中か？
 *
 * @retval  "true"なら更新中
 * @retval  "false"ならそれ以外
 */
bool ZMotionView::_IsUpdateViewer( void )
{
  GFL_ASSERT( m_pListener );

  return (m_pListener->IsRequestUpdateViewer() || !m_pListener->IsFinishUpdateViewer());
}

/**
 * @brief   モーション：指定位置のモーションタイプを取得
 *
 * @param[in]   index   指定位置
 *
 * @retval  モーションタイプ
 */
Viewer::ZMotionType ZMotionView::_GetMotionType_Index( u32 index )
{
  GFL_ASSERT( index < m_ButtonNum );

  return m_ButtonList[index].z_motion;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動データの初期化
 */
void ZMotionView::_InitCursorMoveData( void )
{
  const Dir c_DirTable[] = {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
  };

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  // カーソル移動データの生成
  m_pCursorMoveData = GFL_NEW( heap ) app::tool::CursorManager::MOVE_DATA[m_ButtonNum];

  for( u32 i = 0; i < m_ButtonNum; ++i )
  {
    app::tool::CursorManager::MOVE_DATA* data = &m_pCursorMoveData[i];
    u32 nowIndex = i;

    data->id = nowIndex;

    for( u32 j = 0; j < DIR_NUM; )
    {
      Dir dir = c_DirTable[j];

      // 次のボタンインデックスを取得
      u32 nextIndex = _GetNextButtonIndex( dir, nowIndex );

      FUKUSHIMA_PRINT( "now=[%d] next=[%d]\n", nowIndex, nextIndex );

      // 移動データの設定
      if( nextIndex < m_ButtonNum )
      {
        _SetMoveData( dir, data, nextIndex );
        nowIndex = i;
        ++j;
      }
      else {
        nowIndex = nextIndex;
      }
    }

    FUKUSHIMA_PRINT( "MoveKey[%d] : id=[%d] up=[%d] dw=[%d] lt=[%d] rt=[%d]\n",
      i, data->id, data->key.up, data->key.down, data->key.left, data->key.right );
  }

  // カーソルマネージャの生成
  m_pCursorManager = GFL_NEW( heap ) app::tool::CursorManager( m_pCursorMoveData, m_ButtonNum );
}

/**
 * @brief   次のボタンインデックスの取得
 *
 * @param[in]   dir       方向
 * @param[in]   nowIndex  現在のインデックス
 *
 * @retval  次のボタンインデックス
 */
u32 ZMotionView::_GetNextButtonIndex( Dir dir, u32 nowIndex )
{
  // 上方向
  if( dir == DIR_UP )
  {
    return ( nowIndex >= BTN_MTX_COL ) ? nowIndex - BTN_MTX_COL : nowIndex + MOTION_NUM - BTN_MTX_COL;
  }
  // 下方向
  if( dir == DIR_DOWN )
  {
    return ( nowIndex + BTN_MTX_COL ) % MOTION_NUM;
  }
  // 左方向
  if( dir == DIR_LEFT )
  {
    return ( ( nowIndex % BTN_MTX_COL ) == 0 ) ? nowIndex + BTN_MTX_COL - 1 : nowIndex - 1;
  }
  // 右方向
  if( dir == DIR_RIGHT )
  {
    return ( ( nowIndex % BTN_MTX_COL ) == 4 ) ? nowIndex - ( BTN_MTX_COL - 1 ) : nowIndex + 1;
  }

  GFL_ASSERT(0);
  return 0;
}

/**
 * @brief   移動データの設定
 *
 * @param[in]   dir       方向
 * @param[out]  dstData   データ格納先
 * @param[in]   nextIndex 移動先のインデックス
 */
void ZMotionView::_SetMoveData( Dir dir, app::tool::CursorManager::MOVE_DATA* dstData, u32 nextIndex )
{
  // 上方向
  if( dir == DIR_UP )
  {
    dstData->key.up = nextIndex;
  }
  // 下方向
  if( dir == DIR_DOWN )
  {
    dstData->key.down = nextIndex;
  }
  // 左方向
  if( dir == DIR_LEFT )
  {
    dstData->key.left = nextIndex;
  }
  // 右方向
  if( dir == DIR_RIGHT )
  {
    dstData->key.right = nextIndex;
  }
}

/**
 * @brief   カーソルの初期化
 */
void ZMotionView::_InitCursor( void )
{
  u32 cursorPos = 0;

  // カーソル位置の初期化
  _InitCursorPos( &cursorPos );
  // カーソルのセット
  _SetCursor( cursorPos );

  m_pCursorManager->SetPos( cursorPos );
}

/**
 * @brief   カーソル位置の初期化
 *
 * @param[out]  dstCursorPos    カーソル位置の格納先
 *
 * @note    決定したモーションタイプにカーソル位置を合わせる
 */
void ZMotionView::_InitCursorPos( u32* dstCursorPos )
{
  // 現在決定しているモーションタイプを取得
  Viewer::ZMotionType nowType = m_pListener->GetTrainerMotionType();

  for( u32 i = 0; i < m_ButtonNum; ++i )
  {
    // 現在のモーションタイプとデータテーブルを比較
    if( _GetMotionType_Index(i) == nowType )
    {
      *dstCursorPos = i;
      break;
    }
  }

  m_MotionType = nowType;

  FUKUSHIMA_PRINT( "_InitCursorPos : nowType=[%d] pos=[%d]\n", nowType, *dstCursorPos );
}

/**
 * @brief   カーソル
 *
 * @param[in]   cursorPos   カーソル位置
 */
void ZMotionView::_SetCursor( u32 cursorPos )
{
  for( u32 i = 0; i < m_ButtonNum; ++i )
  {
    bool visible = ( i == cursorPos );

    // カーソル表示
    _SetVisibleCursor( i, visible );
  }

  m_CursorPos = cursorPos;
}

/**
 * @brief   カーソルの表示切替
 *
 * @param[in]   cursorPos   カーソル位置
 * @param[in]   visible     表示フラグ
 */
void ZMotionView::_SetVisibleCursor( u32 cursorPos, bool visible )
{
  GFL_ASSERT( cursorPos < m_ButtonNum );

  gfl2::lyt::LytPane* cursorPane = m_pCursorPane[cursorPos];
  GFL_ASSERT( cursorPane );

  m_pUtil->SetVisiblePane( cursorPane, visible );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    UIInputListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインイベント：非公開
 */
app::ui::UIInputListener::ListenerResult ZMotionView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_BACK )
  {
    this->SetInputEnabled( false );
    m_MainSeq = SEQ_HIDE_MOTION_RESET;
  }
  /// 決定ボタン
  else if( paneId == BTN_ID_DECIDE )
  {
    /// 選択中のタイプを取得
    Viewer::ZMotionType selectType = _GetMotionType_Index( m_CursorPos );

    if( m_MotionType != selectType )
    {
      /// タイプ変更
      m_pListener->SetTrainerMotionType( selectType );
      /// タイプ変更メッセージの表示
      this->SetResult( RES_CALL_MSGWND );
    }
    else {
      /// 前に選択していたタイプと同じなので戻ると同様の処理をする
      this->SetInputEnabled( false );
      m_MainSeq = SEQ_HIDE_MOTION_RESET;
    }
  }
  else {
    //// モーションボタン
    u32 btnIndex = (paneId - BTN_ID_MOTION_00);

    /// カーソルのセット
    _SetCursor( btnIndex );
    /// カーソルマネージャにも反映   // @fix MMCat[806]
    m_pCursorManager->SetPos( btnIndex );

    /// モーション再生リクエスト
    m_ReqMotionType = _GetMotionType_Index( m_CursorPos );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   キーアクション：非公開
 */
app::ui::UIInputListener::ListenerResult ZMotionView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

    // ボタン選択
    btnMan->StartSelectedAct( BTN_ID_MOTION_00 + m_CursorPos );
  }
  else {
    // カーソル移動：リピート入力
    app::tool::CursorManager::MOVE_RET ret = m_pCursorManager->MainRepeat();

    // 移動
    if( ret == app::tool::CursorManager::RET_MOVE )
    {
      // 最新のカーソル位置を取得
      u32 cursorPos = m_pCursorManager->GetPos();

      // カーソルのセット
      _SetCursor( cursorPos );
    }
  }

  return ENABLE_ACCESS;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    MessageMenuViewListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   イベント取得：非公開
 */
void ZMotionView::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    /// 決定後のメッセージウィンドウを閉じる
    this->SetResult( RES_MSGWND_CLOSE );

    this->SetInputEnabled( false );
  }
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
