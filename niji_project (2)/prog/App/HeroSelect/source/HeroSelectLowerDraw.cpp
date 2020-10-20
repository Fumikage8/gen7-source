//======================================================================
/**
 * @file HeroSelectLowerDraw.cpp
 * @date 2015/08/26 12:29:06
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "HeroSelectLowerDraw.h"
#include "HeroSelectDrawerBase.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <str/include/gfl2_Str.h>

// arc
#include <niji_conv_header/app/hero_select/hero_select.h>
#include <niji_conv_header/app/hero_select/hero_select_pane.h>
#include <niji_conv_header/app/hero_select/hero_select_anim_list.h>
#include <niji_conv_header/message/msg_introdemo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
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
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
HeroSelectLowerDraw::HeroSelectLowerDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pFadeManager( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pDrawerBase( NULL )
  , m_Seq( SEQ_INIT )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   msgData         メッセージデータ
 * @param   wordSet         ワードセット
 */
//------------------------------------------------------------------------------
void HeroSelectLowerDraw::Init( void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgData = msgData;
  m_pWordSet = wordSet;
  m_pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );
  InitializeText();

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool HeroSelectLowerDraw::End( void )
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
void HeroSelectLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void HeroSelectLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  if( m_Seq > SEQ_READY )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool HeroSelectLowerDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   画面のオープン
 */
//-----------------------------------------------------------------------------
void HeroSelectLowerDraw::Open( void )
{
  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   画面のクローズ
 */
//-----------------------------------------------------------------------------
void HeroSelectLowerDraw::Close( void )
{
  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool HeroSelectLowerDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER );
  }

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void HeroSelectLowerDraw::Initialize2D( void* pAppLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_HERO_SELECT_HEROSELECT_MIN_LOW_BFLYT,
      LA_HERO_SELECT_HEROSELECT_MIN_LOW___NUM,
      pAppLytBuff,
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
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );

  // 背景アニメの再生
  m_pDrawerBase->StartAnim( LA_HERO_SELECT_HEROSELECT_MIN_LOW__BG_00_BG_LOOP_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void HeroSelectLowerDraw::InitializeText( void )
{
  ;
}

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )
