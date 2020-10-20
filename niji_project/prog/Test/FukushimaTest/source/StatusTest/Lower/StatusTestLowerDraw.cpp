#if PM_DEBUG

//==============================================================================
/**
 * @file    StatusTestLowerDraw.h
 * @author  fukushima_yuya
 * @date    2015.06.26
 * @brief   ステータス下画面描画
 */
//==============================================================================

// niji
#include "Test/FukushimaTest/source/StatusTest/Lower/StatusTestLowerDraw.h"

#include <App/Status/source/StatusDrawListener.h>
#include <App/Status/source/StatusDrawerBase.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/SystemFont.h>
#include <Sound/include/Sound.h>

// gflib
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

namespace {
  enum {
    TAB_ID_NULL,
    TAB_ID_WAZA,
    TAB_ID_MEMO,
    TAB_ID_NUM,
  };

  const u32 ANIM_TAB_WAZA_SELECT = LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_UNSELECT;
  const u32 ANIM_TAB_WAZA_UNSELECT = LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_SELECT;
  const u32 ANIM_TAB_MEMO_SELECT = LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_UNSELECT;
  const u32 ANIM_TAB_MEMO_UNSELECT = LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_SELECT;
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
TestStatusLowerDraw::TestStatusLowerDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_uiListener( NULL )
  , m_pAppParam( NULL )
  , m_pAppRenderingManager( renderingManager )
  , m_pFadeManager( NULL )
  , m_LayoutID( 0 )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_TabID( TAB_ID_NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------------------
TestStatusLowerDraw::~TestStatusLowerDraw( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief	  このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::SetTitleViewUIListener( App::Status::StatusDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::Init( App::Status::APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  m_pAppParam = pAppParam;
  //m_pParty = pAppParam->mpParty;
  //m_PokeIndex = pAppParam->select_poke_index;
  //m_pFadeManager = pAppParam->mpFadeManager;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff, pMsgData, pWordSet );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool TestStatusLowerDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ステータス画面を開く
      StatusOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      if( m_uiListener ) m_uiListener->EndStatus();

      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool TestStatusLowerDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::StatusOpen( void )
{
  // フェードイン
  m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 3 );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::StatusClose( void )
{
  this->SetInputEnabled( false );

  gfl2::math::Vector4 color( 0, 0, 0, 255 );
  m_pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool TestStatusLowerDraw::IsEndInOutAnim( bool in )
{
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
    && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MIN_LOW
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  //Create2D(
  //  pHeap,
  //  NULL,
  //  App::Status::RES_LOW_NUM,
  //  resTbl,
  //  GFL_NELEMS(resTbl),
  //  setupData,
  //  GFL_NELEMS(setupData),
  //  this->UNUSE_MESSAGE_DATA );

  this->SetInputEnabled( false );

  {
    gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

    // 描画共通処理クラスの生成
    m_pDrawerBase = GFL_NEW(devHeap) App::Status::DrawerBase( this, m_pHeap, 0, pMsgData, pWordSet );
    GFL_ASSERT( m_pDrawerBase );

    // ボタンマネージャの生成
    CreateButtonManager();

    // タブの変更
    ChangeTab( TAB_ID_WAZA, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::InitializeText( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   タブの変更
 * @param   tabID   変更後のタブID
 * @param   swFlg   切り替えフラグ
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::ChangeTab( u32 tabID, bool swFlg )
{
  if( swFlg && (m_TabID > TAB_ID_NULL && m_TabID < TAB_ID_NUM) )
  {
    u32 stopAnim  = (m_TabID == TAB_ID_WAZA) ? ANIM_TAB_WAZA_SELECT : ANIM_TAB_MEMO_SELECT;
    u32 startAnim = (m_TabID == TAB_ID_WAZA) ? ANIM_TAB_WAZA_UNSELECT : ANIM_TAB_MEMO_UNSELECT;

    m_pDrawerBase->StopAnim( stopAnim );
    m_pDrawerBase->StartAnim( startAnim );
  }

  if( tabID > TAB_ID_NULL && tabID < TAB_ID_NUM )
  {
    u32 stopAnim = (tabID == TAB_ID_WAZA) ? ANIM_TAB_WAZA_UNSELECT : ANIM_TAB_MEMO_UNSELECT;
    u32 startAnim  = (tabID == TAB_ID_WAZA) ? ANIM_TAB_WAZA_SELECT : ANIM_TAB_MEMO_SELECT;

    m_pDrawerBase->StopAnim( stopAnim );
    m_pDrawerBase->StartAnim( startAnim );

    m_TabID = tabID;
  }
}





//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::OnPreLoadResource( void )
{
  print::SystemFont_CreateNumFont( m_pHeap->GetDeviceHeap(), m_pHeap->GetDeviceHeap() );
  print::SystemFont_SetLytSystemNumFont();
}

//------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneID
 */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TestStatusLowerDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  //switch( paneID )
  //{
  //case App::Status::BTN_ID_CANCEL:
  //  {
  //    StatusClose();
  //  } break;

  //case App::Status::BTN_ID_TAB_WAZA:
  //  {
  //    ChangeTab( TAB_ID_WAZA );
  //  } break;

  //case App::Status::BTN_ID_TAB_MEMO:
  //  {
  //    ChangeTab( TAB_ID_MEMO );
  //  } break;
  //}

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult TestStatusLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  app::tool::ButtonManager * bm = m_pDrawerBase->GetButtonManager();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    StatusClose();
  }

  //if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  //{
  //  if( m_TabID == TAB_ID_MEMO )
  //  {
  //    bm->StartSelectedAct( App::Status::BTN_ID_TAB_WAZA );
  //  }
  //}
  //else   if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) || pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  //{
  //  if( m_TabID == TAB_ID_WAZA )
  //  {
  //    bm->StartSelectedAct( App::Status::BTN_ID_TAB_MEMO );
  //  }
  //}

  return ENABLE_ACCESS;
}





//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void TestStatusLowerDraw::CreateButtonManager( void )
{
  //const u32 BTN_NUM = 10;

  //const app::ui::ButtonInfoLytWk c_DefInfo[] = {
  //  { // CANCEL
  //    App::Status::BTN_ID_CANCEL, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BACK_PASSIVE,
  //  },
  //  { // TAB_WAZA
  //    App::Status::BTN_ID_TAB_WAZA, NULL, NULL, NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_00_PASSIVE,
  //  },
  //  { // TAB_MEMO
  //    App::Status::BTN_ID_TAB_MEMO, NULL, NULL, NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    app::tool::ButtonManager::ANIMATION_NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSPAGEBTN_01_PASSIVE,
  //  },
  //  { // WAZA_0
  //    App::Status::BTN_ID_WAZA_0, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_00_PASSIVE,
  //  },
  //  { // WAZA_1
  //    App::Status::BTN_ID_WAZA_1, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_01_PASSIVE,
  //  },
  //  { // WAZA_2
  //    App::Status::BTN_ID_WAZA_2, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_02_PASSIVE,
  //  },
  //  { // WAZA_3
  //    App::Status::BTN_ID_WAZA_3, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTSSKILLPLT_03_PASSIVE,
  //  },
  //  { // INFO_TOKUSEI
  //    App::Status::BTN_ID_INFO_TOKUSEI, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_00_PASSIVE,
  //  },
  //  { // INFO_ITEM
  //    App::Status::BTN_ID_INFO_TOKUSEI, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__INFO_01_PASSIVE,
  //  },
  //  { // MARK_PLATE
  //    App::Status::BTN_ID_MARK_PLATE, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MARKPLT_PASSIVE,
  //  },
  //  { // SANTI_ICON
  //    App::Status::BTN_ID_SANTI_ICON, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_ACTIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_SANCHI_PASSIVE,
  //  },
  //  { // BOX_MARK_0
  //    App::Status::BTN_ID_BOX_MARK_0, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_ACTIVE,
  //  },
  //  { // BOX_MARK_1
  //    App::Status::BTN_ID_BOX_MARK_1, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_ACTIVE,
  //  },
  //  { // BOX_MARK_2
  //    App::Status::BTN_ID_BOX_MARK_2, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_ACTIVE,
  //  },
  //  { // BOX_MARK_3
  //    App::Status::BTN_ID_BOX_MARK_3, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_ACTIVE,
  //  },
  //  { // BOX_MARK_4
  //    App::Status::BTN_ID_BOX_MARK_4, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_ACTIVE,
  //  },
  //  { // BOX_MARK_5
  //    App::Status::BTN_ID_BOX_MARK_5, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_ACTIVE,
  //  },
  //  { // WND_CANCEL
  //    App::Status::BTN_ID_WND_CANCEL, NULL, NULL, NULL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_TOUCH,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_CANSEL,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_TOUCH_RELEASE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_PASSIVE,
  //    LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_ACTIVE,
  //  },
  //};

  //const gfl2::lyt::LytPaneIndex c_MinLowBtnPaneIndex[BTN_NUM][2] = {
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSPAGEBTN_00, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSPAGEBTN_01, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_01, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_02, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_03, PANENAME_STATUS_PLT_LOW_000_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_00, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_01, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_MARKPLT, PANENAME_COMMON_PLT_LOW_003_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_SANCHI, PANENAME_COMMON_BTN_LOW_006_PANE_BOUND_00 },
  //};

  //const gfl2::lyt::LytPaneIndex c_MrkLowBtnPaneIndex[App::Status::BTN_ID_MRK_LOW_NUM][2] = {
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_00, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_01, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_02, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_03, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_04, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_05, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  //  { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_BUTTON_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
  //};

  //app::ui::ButtonInfoLytWk* info = GFL_NEW_LOW_ARRAY(m_pHeap->GetDeviceHeap()) app::ui::ButtonInfoLytWk[BTN_NUM];

  //for( u32 i=0; i<BTN_NUM; ++i )
  //{
  //  gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( c_MinLowBtnPaneIndex[i][0] );

  //  info[i] = c_DefInfo[i];
  //  info[i].picture_pane = pParts;
  //  info[i].bound_pane = m_pDrawerBase->GetPane( c_MinLowBtnPaneIndex[i][1], pParts );
  //  info[i].lyt_wk = m_pDrawerBase->GetLytWk();
  //}
  //for( u32 i=0; i<App::Status::BTN_ID_MRK_LOW_NUM; ++i )
  //{
  //  gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( c_MrkLowBtnPaneIndex[i][0] );
  //  u32 idx = i + BTN_ID_MRK_LOW_BEGIN;

  //  info[idx] = c_DefInfo[idx];
  //  info[idx].picture_pane = pParts;
  //  info[idx].bound_pane = m_pDrawerBase->GetPane( c_MrkLowBtnPaneIndex[i][1], pParts );
  //  info[idx].lyt_wk = m_pDrawerBase->GetLytWk();
  //}

  //app::ui::UIResponder::CreateButtonManager( m_pHeap, info, BTN_NUM );

  //app::tool::ButtonManager* pBtnMan = app::ui::UIResponder::GetButtonManager();
  //{
  //  pBtnMan->SetButtonBindKey( App::Status::BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  //  pBtnMan->SetButtonBindKey( App::Status::BTN_ID_WND_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  //  pBtnMan->StartSelectedAct( App::Status::BTN_ID_TAB_WAZA );
  //  
  //  for( u8 i=0; i<BTN_NUM; ++i )
  //  {
  //    if( i == App::Status::BTN_ID_CANCEL || i == App::Status::BTN_ID_WND_CANCEL )
  //    {
  //      pBtnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
  //    }
  //    else {
  //      pBtnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  //    }
  //  }
  //}

  //GFL_DELETE_ARRAY info;
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG