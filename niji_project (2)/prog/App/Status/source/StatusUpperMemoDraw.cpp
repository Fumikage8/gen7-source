//======================================================================
/**
 * @file    StatusUpperMemoDraw.cpp
 * @date    2015/11/30 19:20:31
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メモ画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusUpperMemoDraw.h"
#include "StatusDrawListener.h"
#include "StatusDrawerBase.h"

#include <App/Status/source/upperParts/StatusTrainerMemo.h>
#include <App/Status/source/upperParts/StatusRibbonExplain.h>

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
    SEQ_MAIN,
    SEQ_END,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
UpperMemoDraw::UpperMemoDraw( CreateParam& param )
  : app::ui::UIView(param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator())
  , m_pHeap(param.heap)
  , m_pAppRenderingManager(param.renderMan)
  , m_uiListener(NULL)
  , m_pDrawerBase(NULL)
  , m_pTrainerMemo(NULL)
  , m_pRibbonExplain(NULL)
  , m_Seq(SEQ_INIT)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::SetTitleViewUIListener( StatusDrawListener* listener )
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
void UpperMemoDraw::Init( InitParam& param )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( param );
  InitializeText();

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool UpperMemoDraw::End( void )
{
  GFL_SAFE_DELETE( m_pRibbonExplain );

  GFL_SAFE_DELETE( m_pTrainerMemo );

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
void UpperMemoDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
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
void UpperMemoDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_Seq > SEQ_IDLE )
  {
    app::util::G2DUtil* pG2D = this->GetG2DUtil();

    pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool UpperMemoDraw::IsExit( void )
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
bool UpperMemoDraw::IsReady( void ) const
{
  return m_Seq == SEQ_IDLE;
}


//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::Open( void )
{
  // 表示
  this->SetVisible( true );

  m_Seq = SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::Close( void )
{
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
void UpperMemoDraw::SetPokemonParam( const pml::pokepara::PokemonParam* pp )
{
  // トレーナーメモ
  if( m_pTrainerMemo )
  {
    m_pTrainerMemo->Setup( pp );
  }
  // リボン
  if( m_pRibbonExplain )
  {
    m_pRibbonExplain->Setup( pp );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.appLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // MRK_LOW
      0,
      LYTRES_FIELD_STATUS_FIELDSTATUS_SUB_UPP_000_BFLYT,
      LA_FIELD_STATUS_FIELDSTATUS_SUB_UPP_000___NUM,
      param.appLytBuff,
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
    param.baseMsgData,
    param.wordSet );

  this->SetVisible( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  {
    // 描画共通処理の生成
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, param.baseMsgData, param.wordSet );
    {
      GFL_ASSERT(m_pDrawerBase);

      m_pDrawerBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_SUB_UPP_000__BG_WAVE_GREEN_UPP_00_BG_LOOP_00 );
    }

    // トレーナーメモ
    m_pTrainerMemo = GFL_NEW(devHeap) TrainerMemo( m_pDrawerBase, param.memoMsgData );
    {
      GFL_ASSERT(m_pTrainerMemo);

      // 初期化
      m_pTrainerMemo->Initialize();
    }

    // リボンの説明
    m_pRibbonExplain = GFL_NEW(devHeap) RibbonExplain( m_pDrawerBase, param.ribbonMsgData );
    {
      GFL_ASSERT( m_pRibbonExplain );

      // 初期化
      m_pRibbonExplain->Initialize( m_uiListener );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::InitializeText( void )
{
  gfl2::lyt::LytParts*   helpParts = m_pDrawerBase->GetParts( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_PARTS_BTNHELP );
  gfl2::lyt::LytTextBox* helpText  = m_pDrawerBase->GetTextBox( PANENAME_COMMON_PLT_000_PANE_TEXTBOX_00, helpParts );

  this->SetTextboxPaneMessage( helpText, PARAM_S_TEXT_02 );
}


//------------------------------------------------------------------------------
/**
 * @brief   リボンの説明を開く
 *
 * @param   ribbonID    表示するリボンのID
 *
 * @param   "true  = 開くことに成功"
 * @param   "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UpperMemoDraw::OpenRibbonExplain( u32 ribbonID )
{
  if( m_pRibbonExplain )
  {
    // リボンの説明を開く
    return m_pRibbonExplain->Open( ribbonID );
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   リボンの説明を閉じる
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::CloseRibbonExplain( void )
{
  if( m_pRibbonExplain )
  {
    // リボンの説明を閉じる
    m_pRibbonExplain->Close();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   表示するリボンの設定
 *
 * @param   ribbonID    表示するリボンのID
 */
//------------------------------------------------------------------------------
void UpperMemoDraw::SetRibbonExplain( u32 ribbonID )
{
  if( m_pRibbonExplain )
  {
    // 表示するリボンのセットアップ
    m_pRibbonExplain->SetupRibbon( ribbonID );
  }
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
