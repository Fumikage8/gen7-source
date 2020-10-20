//======================================================================
/**
 * @file    ResortUpperHomeMenuDraw.cpp
 * @date    2015/09/24 16:31:23
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人メニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperHomeMenuDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortUtility.h>
#include <App/Resort/source/parts/ResortCharaModel.h>

#include <AppLib/include/Ui/UIResponder.h>
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/BoxPokemonSave.h>

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

namespace
{
  enum {
    SEQ_IDLE,
    SEQ_OPEN_MENU,
    SEQ_OPEN_CHARA,
    SEQ_MAIN,
    SEQ_CLOSE_CHARA,
    SEQ_CLOSE_MENU,
    SEQ_END,
  };
};

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
UpperHomeMenuDraw::UpperHomeMenuDraw( CreateParam& param )
  : UpperBaseDraw( param )
  , m_pDrawerBaseBG(NULL)
  , m_DispType(DISP_TYPE_TALK)
  , m_UpdateFrame(0)
  , m_MainSeq(SEQ_IDLE)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_IKD_UPP_000_BFLYT,
      LA_RESORT_RESORT_IKD_UPP_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_RESORT_RESORT_BGR_UPP_001_BFLYT,
      LA_RESORT_RESORT_BGR_UPP_001___NUM,
      param.lytBuff,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    LYTID_MAX,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, LYTID_FRAME, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // 背景レイアウト用描画共通クラス
    m_pDrawerBaseBG = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, LYTID_BG, param.wordSet, param.msgSys );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::InitializePane( void )
{
  m_pDispVisiblePane = m_pDrawerBase->GetPane( PANENAME_RESORT_IKD_UPP_000_PANE_NULL_VISIBLE );
  {
    m_pDrawerBase->SetVisiblePane( m_pDispVisiblePane, false );
  }

  gfl2::lyt::LytParts* msgParts = m_pDrawerBase->GetParts( PANENAME_RESORT_IKD_UPP_000_PANE_MESSAGE_L2 );
  {
    m_pDrawerBase->SetVisiblePane( msgParts, false );
  }

  static const u32 c_NumTextPaneID[] = {
    PANENAME_RESORT_IKD_UPP_000_PANE_ITEMNUMB_00,
    PANENAME_RESORT_IKD_UPP_000_PANE_ITEMNUMB_01,
    PANENAME_RESORT_IKD_UPP_000_PANE_ITEMNUMB_02,
    PANENAME_RESORT_IKD_UPP_000_PANE_ITEMNUMB_03,
  };

  for( u8 i=0; i<NUM_TEXT_NUM; ++i )
  {
    m_pNumText[i] = m_pDrawerBase->GetTextBox( c_NumTextPaneID[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::InitializeText( void )
{
  gfl2::lyt::LytTextBox* titleText =
    m_pDrawerBase->GetTextBox( PANENAME_RESORT_IKD_UPP_000_PANE_TEXT_TITLE );
  {
    this->SetTextboxPaneMessage( titleText, msg_prsys_up_00_01 );
  }

  // ポケモンの数
  u32 pokeCount = util::GetBoxPokeAndResortPokeNum( m_pResortSave, m_pBoxPokeSave, m_pLytHeap );
  {
    this->SetTextboxPaneNumber( m_pNumText[NUM_TEXT_POKE_COUNT],
      msg_prsys_common_15, pokeCount, 3, 0, print::NUM_DISP_SPACE );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool UpperHomeMenuDraw::Terminate( void )
{
  GFL_SAFE_DELETE( m_pDrawerBaseBG );

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_MainSeq )
  {
  case SEQ_OPEN_MENU:
    {
      this->SetVisible( true );
      m_MainSeq = SEQ_OPEN_CHARA;
    } break;

  case SEQ_OPEN_CHARA:
    {
      if( m_pCharaModel )
      {
        m_pCharaModel->SetVisible( true );
      }
      m_MainSeq = SEQ_MAIN;
    } break;

  case SEQ_CLOSE_CHARA:
    {
      if( m_pCharaModel )
      {
        m_pCharaModel->SetVisible( false );
      }
      m_MainSeq = SEQ_CLOSE_MENU;
    } break;

  case SEQ_CLOSE_MENU:
    {
      this->SetVisible( false );
      m_MainSeq = SEQ_IDLE;
    } break;
  }

  {
    m_UpdateFrame++;

    if( m_UpdateFrame >= UPDATE_FRAME )
    {
      UpdateCount();

      m_UpdateFrame = 0;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = UIView::GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LYTID_FRAME, 2, 1 );
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LYTID_BG, gfl2::lyt::ORDER_NORMAL, 0 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::Open( void )
{
  m_MainSeq = SEQ_OPEN_MENU;
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::Close( void )
{
  m_MainSeq = SEQ_CLOSE_CHARA;
}


//------------------------------------------------------------------------------
/**
 * @brief   開くタイプの設定
 *
 * @param   type    画面タイプ
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::SetDispType( DispType type )
{
  m_DispType = type;

  // 画面モードの設定
  SetDispMode();
}

//------------------------------------------------------------------------------
/**
 * @brief   画面モードの設定
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::SetDispMode( void )
{
  if( m_DispType == DISP_TYPE_HOME_MENU )
  {
    // 数値の更新
    UpdateCount();

    // 管理人メニューの時のみ表示
    m_pDrawerBase->SetVisiblePane( m_pDispVisiblePane, true );
  }
  else {
    // 管理人メニューの時のみ表示
    m_pDrawerBase->SetVisiblePane( m_pDispVisiblePane, false );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   数値の更新
 */
//------------------------------------------------------------------------------
void UpperHomeMenuDraw::UpdateCount( void )
{
  // 豆の数
  const u32 BEANS_KIND_NUM = 3;

  u32 beansCount[BEANS_KIND_NUM];

  // ランクごとのポケマメ数を取得
  util::GetRankBeansCount( m_pResortSave, beansCount );

  for( u32 i=0; i<BEANS_KIND_NUM; ++i )
  {
    this->SetTextboxPaneNumber( m_pNumText[i], msg_prsys_common_15, beansCount[i], 4 );
  }
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
