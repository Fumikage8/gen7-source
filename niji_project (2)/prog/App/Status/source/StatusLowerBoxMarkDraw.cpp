//======================================================================
/**
 * @file    StatusLowerBoxMarkDraw.cpp
 * @date    2015/11/27 19:10:19
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ボックスマーク変更画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusLowerBoxMarkDraw.h"
#include "StatusDrawListener.h"
#include "StatusDrawerBase.h"

#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/field_status/field_status.h>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  シーケンスID
  enum {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IDLE,
    SEQ_OPEN, // @fix NMCat[285]
    SEQ_MAIN,
    SEQ_END,
  };

  //! @brief  ボタンID
  enum {
    BTN_ID_CANCEL,
    BTN_ID_MARK_00,
    BTN_ID_MARK_01,
    BTN_ID_MARK_02,
    BTN_ID_MARK_03,
    BTN_ID_MARK_04,
    BTN_ID_MARK_05,
    BTN_ID_NUM,
  };

  //! ボックスマークカラー
  static const u32 c_MarkColorAnim[pml::pokepara::BOX_MARK_NUM][pml::pokepara::BOX_MARK_COLOR_NUM] = {
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_COLOR_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_COLOR_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_COLOR_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_COLOR_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_COLOR_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_COLOR_00,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_COLOR_01,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_COLOR_02,
    },
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
LowerBoxMarkDraw::LowerBoxMarkDraw( CreateParam& param )
  : app::ui::UIView( param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator() )
  , m_pHeap( param.heap )
  , m_pAppRenderingManager( param.renderMan )
  , m_uiListener( NULL )
  , m_pPokeParam( NULL )
  , m_pDrawerBase( NULL )
  , m_Seq( SEQ_INIT )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::SetTitleViewUIListener( StatusDrawListener* listener )
{
  m_uiListener = listener;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Init( InitParam& param )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( param );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LowerBoxMarkDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_OPEN:
    {
      m_Seq = SEQ_MAIN; // @fix NMCat[285]
    } break;

  case SEQ_READY:
    {
      m_Seq = SEQ_IDLE;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_Seq == SEQ_MAIN )
  {
    app::util::G2DUtil* pG2D = this->GetG2DUtil();

    pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, gfl2::lyt::ORDER_NORMAL, 1 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool LowerBoxMarkDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool LowerBoxMarkDraw::IsReady( void ) const
{
  return m_Seq == SEQ_IDLE;
}


//------------------------------------------------------------------------------
/**
 * @brief   オープン
 *
 * @param   updateProhibition   更新禁止フラグ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Open( bool updateProhibition )
{
  // セットアップ
  SetupBoxMark( updateProhibition );

  // 入力を有効
  this->SetInputEnabled( true );
  // 表示
  this->SetVisible( true );

  m_Seq = SEQ_OPEN; // @fix NMCat[285]
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Close( void )
{
  // 入力を無効
  this->SetInputEnabled( false );
  // 非表示
  this->SetVisible( false );

  m_Seq = SEQ_IDLE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::SetPokemonParam( pml::pokepara::PokemonParam* pp )
{
  m_pPokeParam = pp;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.appLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MRK_LOW
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000___NUM,
      param.appLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
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
    param.baseMsgData,
    param.wordSet );

  this->SetInputEnabled( false );
  this->SetVisible( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  {
    // 描画共通処理の生成
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, param.baseMsgData, param.wordSet );
    GFL_ASSERT(m_pDrawerBase);

    // ボタンマネージャの生成
    CreateButtonManager();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::InitializeText( void )
{
  //gfl2::lyt::LytTextBox* textbox = m_pDrawerBase->GetTextBox( PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_TEXTBOX_00 );
  //m_pDrawerBase->GetUIView()->SetTextboxPaneMessage( textbox, PARAM_TEXT_03 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークのセットアップ
 *
 * @param   updateProhibition   更新禁止フラグ
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::SetupBoxMark( bool updateProhibition )
{
  for( u32 i=0; i<pml::pokepara::BOX_MARK_NUM; ++i )
  {
    // ボックスマークの色を取得
    pml::pokepara::BoxMarkColor color = GetBoxMarkColor( static_cast<pml::pokepara::BoxMark>( i ) );

    // アニメの再生
    m_pDrawerBase->StartAnim( c_MarkColorAnim[i][color] );
  }

  // テキストの変更
  gfl2::lyt::LytTextBox* textbox = m_pDrawerBase->GetTextBox( PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_TEXTBOX_00 );
  {
    u32 msgID = (!updateProhibition) ? PARAM_TEXT_03 : PARAM_TEXT_01;

    this->SetTextboxPaneMessage( textbox, msgID );
  }

  // ボタンの有効設定
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    static const u32 c_SetupButtonTable[] = {
      BTN_ID_MARK_00,
      BTN_ID_MARK_01,
      BTN_ID_MARK_02,
      BTN_ID_MARK_03,
      BTN_ID_MARK_04,
      BTN_ID_MARK_05,
    };

    for( u32 i=0; i<GFL_NELEMS(c_SetupButtonTable); ++i )
    {
      if( !updateProhibition )
      {
        btnMan->SetButtonActive( c_SetupButtonTable[i] );
      }
      else {
        btnMan->SetButtonPassive( c_SetupButtonTable[i] );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークの更新
 * 
 * @param   markID    ボックスマークID
 *
 * @note    無色 -> 青 -> 赤 -> 無色
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::UpdateBoxMark( const u32 markID )
{
  pml::pokepara::BoxMark mark = static_cast<pml::pokepara::BoxMark>( markID );

  // ボックスマークの色を取得
  pml::pokepara::BoxMarkColor color = GetBoxMarkColor( mark );

  // 色の変更
  color = static_cast<pml::pokepara::BoxMarkColor>( (color + 1) % pml::pokepara::BOX_MARK_COLOR_NUM );

  // 変更した色に反映
  SetBoxMarkColor( mark, color );

  // アニメの再生
  m_pDrawerBase->StartAnim( c_MarkColorAnim[markID][color] );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークの色の取得
 * 
 * @param   markID    ボックスマークID
 * @param   color     ボックスマークカラー
 */
//------------------------------------------------------------------------------
void LowerBoxMarkDraw::SetBoxMarkColor( pml::pokepara::BoxMark mark, pml::pokepara::BoxMarkColor color )
{
  if( m_pPokeParam == NULL ) return;

  bool fastmode_flag = m_pPokeParam->StartFastMode();
  {
    m_pPokeParam->SetBoxMark( mark, color );
  }
  m_pPokeParam->EndFastMode( fastmode_flag );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークの色の取得
 * 
 * @param   markID    ボックスマークID
 *
 * @return  ボックスマークの色
 */
//------------------------------------------------------------------------------
pml::pokepara::BoxMarkColor LowerBoxMarkDraw::GetBoxMarkColor( pml::pokepara::BoxMark mark )
{
  pml::pokepara::BoxMarkColor color = pml::pokepara::BOX_MARK_COLOR_NONE;

  if( m_pPokeParam )
  {
    bool fastmode_flag = m_pPokeParam->StartFastMode();
    {
      color = m_pPokeParam->GetBoxMark( mark );
    }
    m_pPokeParam->EndFastMode( fastmode_flag );
  }

  return color;
}





//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerBoxMarkDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      // ポケモンパラメータの反映
      m_uiListener->UpdatePokeParam();
      // 閉じる
      m_uiListener->CloseBoxMark();
    } break;

  default:
    {
      u32 markID = paneId - BTN_ID_MARK_00;

      UpdateBoxMark( markID );
    }
  }

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
void LowerBoxMarkDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__BUTTON_BACK_PASSIVE,
    },
    {
      BTN_ID_MARK_00, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_00_PASSIVE,
    },
    {
      BTN_ID_MARK_01, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_01_PASSIVE,
    },
    {
      BTN_ID_MARK_02, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_02_PASSIVE,
    },
    {
      BTN_ID_MARK_03, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_03_PASSIVE,
    },
    {
      BTN_ID_MARK_04, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_04_PASSIVE,
    },
    {
      BTN_ID_MARK_05, NULL, NULL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_TOUCH,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_CANSEL,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_TOUCH_RELEASE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_ACTIVE,
      LA_FIELD_STATUS_FIELDSTATUS_MRK_LOW_000__PARTSBTN_05_PASSIVE,
    },
  };
  const gfl2::lyt::LytPaneIndex c_BtnPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_BUTTON_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_00, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_01, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_02, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_03, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_04, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
    { PANENAME_FIELDSTATUS_MRK_LOW_000_PANE_PARTSBTN_05, PANENAME_COMMON_BTN_LOW_007_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_BtnPaneIndex[i][0] );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = m_pDrawerBase->GetPane( c_BtnPaneIndex[i][1], parts );
  }

  gfl2::lyt::LytWk* lytwk = m_pDrawerBase->GetLytWk();
  m_pDrawerBase->GetUIView()->CreateButtonManager( m_pHeap, lytwk, info, BTN_ID_NUM );
  {
    app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

    btnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B );

    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      u32 seid = (i == BTN_ID_CANCEL) ? SEQ_SE_CANCEL1 : SEQ_SE_DECIDE1;

      btnMan->SetButtonSelectSE( i, seid );
    }
  }

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
