//======================================================================
/**
 * @file    HeroSelectUpperDraw.cpp
 * @date    2015/08/26 12:12:31
 * @author  fukushima_yuya
 * @brief   容姿性別選択上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "HeroSelectUpperDraw.h"
#include "HeroSelectDrawListener.h"
#include "HeroSelectDrawerBase.h"
#include "HeroSelectConst.h"
#include "HeroSelectYesNoWindow.h"

#include <App/HeroSelect/include/HeroSelectAppParam.h>

#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Tool/AppToolTrainerIconRendering.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/FashionSave.h>
#include "Savedata/include/JoinFestaDataSave.h"
#include <sound/include/Sound.h>
#include <system/include/HeapDefine.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <model/include/gfl2_DressUpParam.h>
#include <str/include/gfl2_Str.h>

// arc
#include <niji_conv_header/app/hero_select/hero_select.h>
#include <niji_conv_header/app/hero_select/hero_select_pane.h>
#include <niji_conv_header/app/hero_select/hero_select_anim_list.h>
#include <niji_conv_header/message/msg_introdemo.h>
#include <System/include/DressUp.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
#define DEBUG_BACK    0   //!< デバッグ用フラグ(Bボタンバック)

#define BODY_PARTS_MAX      poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT
#define ITEM_PARTS_MAX      poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT

  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

  enum SubSequence
  {
    SUB_SEQ_INIT = 0,
    SUB_SEQ_MAIN,
    SUB_SEQ_TERM,
    SUB_SEQ_NUM,
  };

  enum {
    //DISP_MODE_TALK_1,
    DISP_MODE_SELECT,
    DISP_MODE_MOVE,
    DISP_MODE_TALK_2,
    DISP_MODE_CONFIRM_1,
    DISP_MODE_TALK_3,
    // 名前入力
    DISP_MODE_TALK_4,
    DISP_MODE_CONFIRM_2,
    //DISP_MODE_TALK_5,
    DISP_MODE_NUM,

    //DISP_MODE_FIRST_BEGIN  = DISP_MODE_TALK_1,
    DISP_MODE_FIRST_BEGIN  = DISP_MODE_SELECT,
    DISP_MODE_SECOND_BEGIN = DISP_MODE_TALK_4,
  };

  enum {
    BTN_ID_CHARA_1,
    BTN_ID_CHARA_2,
    BTN_ID_CHARA_3,
    BTN_ID_CHARA_4,
    BTN_ID_CHARA_5,
    BTN_ID_CHARA_6,
    BTN_ID_CHARA_7,
    BTN_ID_CHARA_8,
    BTN_ID_MAX,
  };

  // ボディカテゴリ
  static const s16 c_BodyCategory[CHARA_MAX][BODY_PARTS_MAX] =
  {
    { // 男１
      poke_3d::model::DressUpParam::SEX_MALE,
      DRESSUP_COLOR_ID_SKIN_SWH,
      DRESSUP_COLOR_ID_EYE_EBL,
      DRESSUP_COLOR_ID_HAIR_HLB,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 男２
      poke_3d::model::DressUpParam::SEX_MALE,
      DRESSUP_COLOR_ID_SKIN_SYE,
      DRESSUP_COLOR_ID_EYE_EBK,
      DRESSUP_COLOR_ID_HAIR_HBK,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 男３
      poke_3d::model::DressUpParam::SEX_MALE,
      DRESSUP_COLOR_ID_SKIN_SBR,
      DRESSUP_COLOR_ID_EYE_EBR,
      DRESSUP_COLOR_ID_HAIR_HDB,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 男４
      poke_3d::model::DressUpParam::SEX_MALE,
      DRESSUP_COLOR_ID_SKIN_SDB,
      DRESSUP_COLOR_ID_EYE_EBR,
      DRESSUP_COLOR_ID_HAIR_HBK,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 女１
      poke_3d::model::DressUpParam::SEX_FEMALE,
      DRESSUP_COLOR_ID_SKIN_SWH,
      DRESSUP_COLOR_ID_EYE_EBL,
      DRESSUP_COLOR_ID_HAIR_HLB,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 女２
      poke_3d::model::DressUpParam::SEX_FEMALE,
      DRESSUP_COLOR_ID_SKIN_SYE,
      DRESSUP_COLOR_ID_EYE_EBK,
      DRESSUP_COLOR_ID_HAIR_HBK,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 女３
      poke_3d::model::DressUpParam::SEX_FEMALE,
      DRESSUP_COLOR_ID_SKIN_SBR,
      DRESSUP_COLOR_ID_EYE_EBR,
      DRESSUP_COLOR_ID_HAIR_HDB,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
    { // 女４
      poke_3d::model::DressUpParam::SEX_FEMALE,
      DRESSUP_COLOR_ID_SKIN_SDB,
      DRESSUP_COLOR_ID_EYE_EBR,
      DRESSUP_COLOR_ID_HAIR_HBK,
      DRESSUP_COLOR_ID_LIP_LOF,
    },
  };

  ////! 選択アニメ
  //static const u32 c_SelectAnim[] = {
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_00_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_01_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_02_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_03_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_04_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_05_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_06_ON,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_07_ON,
  //};

  ////! 非選択アニメ
  //static const u32 c_UnselectAnim[] = {
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_00_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_01_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_02_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_03_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_04_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_05_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_06_OFF,
  //  LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_07_OFF,
  //};

  //! 
  static const u32 c_SelectHeroAnim[] = {
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_00,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_01,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_02,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_03,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_04,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_05,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_06,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_DECISION_00_HERO_07,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
HeroSelectUpperDraw::HeroSelectUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager, MenuCursor* pCursor )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pDrawerBase( NULL )
  , m_pYesNoWindow( NULL )
  , m_pCursor( pCursor )
  , m_pTalkWindow( NULL )
  , m_SubSeq( 0 )
  , m_DispMode( 0 )
  , m_CursorPos( 0 )
  , m_DecideTrigger( false )
  , m_pHeroAll( NULL )
  , m_pSelectHero( NULL )
  , m_pWindowPane( NULL )
  , m_pKeyRepeat( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam       外部設定パラメータ
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   pAppWndLytBuff  レイアウトバイナリ
 * @param   msgData         メッセージデータ
 * @param   wordSet         ワードセット
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pAppWndLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgData = msgData;
  m_pWordSet = wordSet;
  m_pAppParam = pAppParam;
  m_pMyStatus = pAppParam->my_status;
  m_pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff, pAppWndLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool HeroSelectUpperDraw::End( void )
{
  if( m_pTalkWindow )
  {
    m_pTalkWindow->RemoveFromSuperView();
    m_pTalkWindow->End();
    GFL_SAFE_DELETE( m_pTalkWindow );
  }

  if( m_pCursor )
  {
    m_pCursor->UnsetupCursor();
  }

  GFL_SAFE_DELETE( m_pKeyRepeat );

  GFL_SAFE_DELETE( m_pYesNoWindow );

  GFL_SAFE_DELETE( m_pDrawerBase);

  Delete2D();
  DeleteGraphicsSystem();
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( !MainSequenceReady() ) break;

      // 画面を開く
      Open();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_SubSeq = SUB_SEQ_INIT;
      m_DispMode = (!m_pAppParam->name_input) ? DISP_MODE_FIRST_BEGIN : DISP_MODE_SECOND_BEGIN;
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pCursor ) m_pCursor->Update();
      if( m_pYesNoWindow ) m_pYesNoWindow->Update();

      if( m_SubSeq == SUB_SEQ_INIT )      { SubSequenceInit(); }
      else if( m_SubSeq == SUB_SEQ_MAIN ) { SubSequenceMain(); }
      else                                { SubSequenceTerm(); }
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->EndHeroSelect();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }

  if( m_pCursor ) m_pCursor->Draw( no );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool HeroSelectUpperDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   画面のオープン
 */
//-----------------------------------------------------------------------------
void HeroSelectUpperDraw::Open( void )
{
  // フェードイン
  if( m_pFadeManager ) m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );

  // 下画面の表示
  if( m_uiListener ) m_uiListener->OpenHeroSelect();

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   画面のクローズ
 */
//-----------------------------------------------------------------------------
void HeroSelectUpperDraw::Close( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // 下画面のクローズ
  if( m_uiListener ) m_uiListener->CloseHeroSelect();

  // フェードアウト
  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  if( m_pFadeManager ) m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool HeroSelectUpperDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//-----------------------------------------------------------------------------
void HeroSelectUpperDraw::SetTitleViewUIListener( HeroSelectDrawListener* pListener )
{
  m_uiListener = pListener;
}



//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pAppLytBuff     レイアウトバイナリデータ
 * @param   pAppWndLytBuff  レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::Initialize2D( void* pAppLytBuff, void* pAppWndLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_HERO_SELECT_HEROSELECT_MIN_UPP_BFLYT,
      LA_HERO_SELECT_HEROSELECT_MIN_UPP___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  // 
  //for( u8 i=0; i<CHARA_MAX; ++i )
  //{
  //  m_pDrawerBase->StartAnim( c_UnselectAnim[i] );    //!< 非選択
  //}

  // キー入力
  m_pKeyRepeat = GFL_NEW(devHeap) app::util::KeyRepeatController( COL_MAX - 1, CHARA_MAX - 1 );
  GFL_ASSERT( m_pKeyRepeat );

  // カーソル
  {
    static const u32 c_CursorPosIndex[CHARA_MAX][2] = {
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_00, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_01, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_02, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_03, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_04, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_05, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_06, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
      { PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_07, PANENAME_HEROSELECT_ICN_000_PANE_NULL_CUR },
    };

    gfl2::lyt::LytPane* curPosPane[CHARA_MAX];

    for( u8 i=0; i<CHARA_MAX; ++i )
    {
      u32 partsIndex = c_CursorPosIndex[i][0];
      u32 paneIndex  = c_CursorPosIndex[i][1];

      DrawerBase* base = m_pDrawerBase;

      gfl2::lyt::LytParts* parts = base->GetParts( partsIndex );
      curPosPane[i] = base->GetPane( paneIndex, parts );
    }
    // セットアップ
    m_pCursor->SetupCursor( m_pDrawerBase->GetG2D(), curPosPane, CHARA_MAX, gfl2::lyt::DISPLAY_UPPER );
  }

  // トークウィンドウ
  m_pTalkWindow = GFL_NEW(devHeap) App::Tool::TalkWindow( m_pDrawerBase->GetHeap(), m_pAppRenderingManager );
  //m_pTalkWindow->Setup( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
  m_pTalkWindow->SetWindowType( App::Tool::WIN_TYPE_SYS_DOWN );
  m_pTalkWindow->Setup( pAppWndLytBuff );
  m_pTalkWindow->InitMsgData( m_pMsgData );
  this->AddSubView( m_pTalkWindow );

  // 
  m_pHeroAll = m_pDrawerBase->GetPane( PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_ALL );
  m_pDrawerBase->SetVisiblePane( m_pHeroAll, false );

  m_pSelectHero = m_pDrawerBase->GetPane( PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_DECISION_00 );
  m_pDrawerBase->SetVisiblePane( m_pSelectHero, false );

  m_pWindowPane = m_pDrawerBase->GetPane( PANENAME_HEROSELECT_MIN_UPP_PANE_HERO_BG );
  m_pDrawerBase->SetVisiblePane( m_pWindowPane, false );

  // 顔の変更
  static const u32 c_CharaTypeAnim[] = {
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_00_HERO_00,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_01_HERO_01,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_02_HERO_02,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_03_HERO_03,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_04_HERO_04,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_05_HERO_05,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_06_HERO_06,
    LA_HERO_SELECT_HEROSELECT_MIN_UPP__HERO_07_HERO_07,
  };

  for( u32 i=0; i<GFL_NELEMS(c_CharaTypeAnim); ++i )
  {
    m_pDrawerBase->StartAnim( c_CharaTypeAnim[i] );
  }

  // 背景アニメの再生
  m_pDrawerBase->StartAnim( LA_HERO_SELECT_HEROSELECT_MIN_UPP__BG_01_BG_LOOP_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::InitializeText( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：準備
 */
//------------------------------------------------------------------------------
bool HeroSelectUpperDraw::MainSequenceReady( void )
{
  if( m_pYesNoWindow )
  {
    m_pYesNoWindow->Update();

    if( !m_pYesNoWindow->IsReady() ) return false;
  }
  else {
    // YesNoウィンドウ
    m_pYesNoWindow = GFL_NEW(m_pHeap->GetDeviceHeap()) YesNoWindow( m_pDrawerBase, m_pCursor, m_pAppRenderingManager );

    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   サブシーケンス：初期化
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::SubSequenceInit( void )
{
  switch( m_DispMode )
  {
  //case DISP_MODE_TALK_1:
  //  {
  //    // トークウィンドウ
  //    m_pTalkWindow->SetMessage( static_cast<u32>(doctor_06) );
  //    m_pTalkWindow->CallMsgWin();
  //  } break;

  case DISP_MODE_SELECT:
    {
      //m_pDrawerBase->StartAnim( c_SelectAnim[m_CursorPos] );
      m_pDrawerBase->StartAnim( c_SelectHeroAnim[m_CursorPos] );

      if( m_pCursor )   // @fix cov_ctr[11052]
      {
        m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), m_CursorPos );
        m_pCursor->SetVisible( true );
      }

      m_pDrawerBase->SetVisiblePane( m_pHeroAll, true );
      m_pDrawerBase->SetVisiblePane( m_pWindowPane, true );
    } break;

  case DISP_MODE_MOVE:
    {
      ;
    } break;

  case DISP_MODE_TALK_2:
    {
      m_pDrawerBase->SetVisiblePane( m_pSelectHero, true );

      // トークウィンドウ
      m_pTalkWindow->SetMessage( static_cast<u32>(doctor_07) );
      m_pTalkWindow->CallMsgWin();
    } break;

  case DISP_MODE_CONFIRM_1:
    {
      // YesNoウィンドウ
      m_pYesNoWindow->OpenWindow();
    } break;

  case DISP_MODE_TALK_3:
    {
      // トークウィンドウ
      m_pTalkWindow->SetMessage( static_cast<u32>(doctor_08) );
      m_pTalkWindow->CallMsgWin();
    } break;

  case DISP_MODE_TALK_4:
    {
      app::util::G2DUtil* g2d = m_pDrawerBase->GetG2D();
      gfl2::str::StrBuf* str = g2d->GetTempStrBuf( 1 );

      // 主人公名のセット
      g2d->SetRegisterPlayerName( 0, m_pMyStatus );
      g2d->GetStringExpand( str, doctor_09 );

      // トークウィンドウ
      m_pTalkWindow->SetMessage( str );
      m_pTalkWindow->CallMsgWin();
    } break;

  case DISP_MODE_CONFIRM_2:
    {
      // YesNoウィンドウ
      m_pYesNoWindow->OpenWindow();
    } break;

  //case DISP_MODE_TALK_5:
  //  {
  //    // トークウィンドウ
  //    m_pTalkWindow->SetMessage( static_cast<u32>(doctor_10) );
  //    m_pTalkWindow->CallMsgWin();
  //  } break;
  }

  m_SubSeq = SUB_SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   サブシーケンス：処理
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::SubSequenceMain( void )
{
  switch( m_DispMode )
  {
  //case DISP_MODE_TALK_1:
  case DISP_MODE_TALK_3:
  //case DISP_MODE_TALK_5:
    {
      gfl2::str::StrWin::Result res = m_pTalkWindow->GetStrWinResult();

      if( m_DecideTrigger && res == gfl2::str::StrWin::RES_DONE )
      {
        m_pTalkWindow->CloseMsgWin();

        // 決定音
        Sound::PlaySE( SEQ_SE_DECIDE1 );

        m_SubSeq = SUB_SEQ_TERM;
      }
    } break;

  case DISP_MODE_TALK_2:
  case DISP_MODE_TALK_4:
    {
      gfl2::str::StrWin::Result res = m_pTalkWindow->GetStrWinResult();

      if( res == gfl2::str::StrWin::RES_DONE )
      {
        m_SubSeq = SUB_SEQ_TERM;
      }
    } break;

  case DISP_MODE_MOVE:
    {
      m_SubSeq = SUB_SEQ_TERM;
    } break;

  case DISP_MODE_SELECT:
    {
      if( m_DecideTrigger )
      {
        // 決定音
        Sound::PlaySE( SEQ_SE_DECIDE1 );

        m_SubSeq = SUB_SEQ_TERM;
      }
    } break;

  case DISP_MODE_CONFIRM_1:
  case DISP_MODE_CONFIRM_2:
    {
      if( m_pYesNoWindow )
      {
        u8 res = m_pYesNoWindow->GetResult();

        if( res != WND_RES_NONE )
        {
          m_pYesNoWindow->CloseWindow();
          m_pTalkWindow->CloseMsgWin();

          m_SubSeq = SUB_SEQ_TERM;
        }
      }
    } break;
  }

  m_DecideTrigger = false;
}

//------------------------------------------------------------------------------
/**
 * @brief   サブシーケンス：解放
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::SubSequenceTerm( void )
{
  switch( m_DispMode )
  {
  //case DISP_MODE_TALK_1:
  //  {
  //    m_DispMode = DISP_MODE_SELECT;  //!< 容姿選択へ
  //  } break;

  case DISP_MODE_SELECT:
    {
      m_pCursor->SetVisible( false );
      m_pDrawerBase->SetVisiblePane( m_pHeroAll, false );
      m_pDrawerBase->SetVisiblePane( m_pWindowPane, false );
      m_DispMode = DISP_MODE_MOVE;
    } break;

  case DISP_MODE_MOVE:
    {
      m_DispMode = DISP_MODE_TALK_2;
    } break;

  case DISP_MODE_TALK_2:
    {
      m_DispMode = DISP_MODE_CONFIRM_1;
    } break;

  case DISP_MODE_CONFIRM_1:
    {
      m_pDrawerBase->SetVisiblePane( m_pSelectHero, false );

      if( m_pYesNoWindow->GetResult() == WND_RES_YES )
      {
        // 容姿をセーブデータに反映
        SetDressUpParam( m_CursorPos );

        m_DispMode = DISP_MODE_TALK_3;  //!< 名前入力
      }
      else {
        m_DispMode = DISP_MODE_SELECT;  //!< 容姿選択へ
      }
    } break;

  case DISP_MODE_TALK_3:
    {
      m_pAppParam->endMode = END_MODE_STRINPUT;
      Close();
    } break;

  case DISP_MODE_TALK_4:
    {
      m_DispMode = DISP_MODE_CONFIRM_2;
    } break;

  case DISP_MODE_CONFIRM_2:
    {
      if( m_pYesNoWindow->GetResult() == WND_RES_YES )
      {
        //m_DispMode = DISP_MODE_TALK_5;
        m_pAppParam->endMode = END_MODE_NEXT;
        Close();
      }
      else {
        m_pAppParam->endMode = END_MODE_STRINPUT;
        Close();
      }
    } break;

  //case DISP_MODE_TALK_5:
  //  {
  //    m_pAppParam->endMode = END_MODE_NEXT;
  //    Close();
  //  } break;
  }

  m_SubSeq = SUB_SEQ_INIT;
}

//------------------------------------------------------------------------------
/**
 * @brief   着せ替えデータのセット
 * @param   index   容姿選択のインデックス
 */
//------------------------------------------------------------------------------
void HeroSelectUpperDraw::SetDressUpParam( u32 index )
{
  poke_3d::model::DressUpParam param;

  u8 sex = (index < ROW_MAX) ? PM_MALE : PM_FEMALE;
  m_pMyStatus->SetSex( sex );

  Savedata::MyStatus::ICON_DATA  iconData;

  // 各性別のデフォルト着せ替えパラメータの設定
  if( sex == PM_MALE )
  {
    System::DressUp::GetDressUpParamDefaultHero( &param );
  }
  else if( sex == PM_FEMALE )
  {
    System::DressUp::GetDressUpParamDefaultHeroine( &param );
  }

  //  性別によって初期トレーナーアイコンパラメータ取得
  app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( sex, &iconData );
  iconData.bDressDefault  = false;

  // ボディカテゴリ
  const s16* body = c_BodyCategory[index];

  for( u32 i=0; i<poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT; ++i )
  {
    param.bodyParams[i] = body[i];
  }

  //// アイテムカテゴリ
  //const s16* item = c_ItemCategory[index];

  //for( u32 i=0; i<poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i )
  //{
  //  param.itemParams[i] = item[i];
  //}

  //  トレーナーアイコンの設定
  m_pMyStatus->SetIcon( iconData );

  // MyStatusへ登録
  m_pMyStatus->SetDressup( param );

  // 着せ替えの初回所持をセーブ  内田さん依頼
  {
    Savedata::Fashion* pFashionSaveData =
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFashion();
    GFL_ASSERT( pFashionSaveData);

    {
      pFashionSaveData->Clear( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
    }

    pFashionSaveData->InitializeDefaultFashionItem( ( m_pMyStatus->GetSex() == poke_3d::model::DressUpParam::SEX_FEMALE ) );
  }

  // ジョインフェスタのセーブデータ初期値
  {
    GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData()->InitializeDefaultPattern( sex );
  }

  // ファインダースタジオセーブデータ初期値
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFinderStudioSave()->InitDressupParam( param );

  // 選択されたインデックスを外側へ渡す
  m_pAppParam->select_index = index;
}





//==============================================================================
/**
 *      ↓  以下がリスナー処理  ↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult HeroSelectUpperDraw::OnLayoutPaneEvent( const u32 paneId )
{
  GFL_UNUSED( paneId );

  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  return ENABLE_ACCESS;
}


//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult HeroSelectUpperDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

#if DEBUG_BACK
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    //!< 閉じる
    Close();
  }
#endif

  if( m_DecideTrigger )
  {
    return ENABLE_ACCESS;   //! @fix NMCat[3512]
  }

  if( m_SubSeq == SUB_SEQ_MAIN && m_DispMode == DISP_MODE_SELECT )   //! @fix NMCat[3512]
  {
    // カーソル移動
    u32 curPosX = static_cast<u32>(m_CursorPos % ROW_MAX);
    u32 curPosY = static_cast<u32>(m_CursorPos / ROW_MAX);

    m_pKeyRepeat->UpdateInfo( pKey, curPosY, m_CursorPos );

    if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      curPosY = (curPosY + COL_MAX - 1) % COL_MAX;
    }
    else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      curPosY = (curPosY + 1) % COL_MAX;
    }
    else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_LEFT ) )
    {
      if( curPosX == 0 )
      {
        curPosX = ROW_MAX - 1;
        curPosY = (curPosY + COL_MAX - 1) % COL_MAX;
      }
      else {
        curPosX--;
      }
    }
    else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_RIGHT ) )
    {
      if( curPosX == (ROW_MAX - 1) )
      {
        curPosX = 0;
        curPosY = (curPosY + 1) % COL_MAX;
      }
      else {
        curPosX++;
      }
    }

    u32 curPos = (curPosY * ROW_MAX) + curPosX;

    if( curPos != m_CursorPos )
    {
      m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), curPos );

      //m_pDrawerBase->StartAnim( c_UnselectAnim[m_CursorPos] );
      //m_pDrawerBase->StartAnim( c_SelectAnim[curPos] );
      m_pDrawerBase->StartAnim( c_SelectHeroAnim[curPos] );

      Sound::PlaySE( SEQ_SE_SELECT1 );

      m_CursorPos = curPos;
    }
  }

  // 決定
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_DecideTrigger = true;
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )
