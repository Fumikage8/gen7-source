//======================================================================
/**
 * @file    FinderRotomFadeDraw.cpp
 * @date    2016/01/12 17:38:04
 * @author  fukushima_yuya
 * @brief   ポケファインダー：ロトムフェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderRotomFadeDraw.h"
#include "FinderDrawerBase.h"

#include <AppLib/include/Ui/UIView.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/rotom_fade/rotom_fade.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_pane.h>
#include <niji_conv_header/app/rotom_fade/rotom_fade_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_IDLE,
    SEQ_FADE,
    SEQ_END
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderRotomFade::FinderRotomFade( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pAppParam( NULL )
  , m_Seq( SEQ_INIT )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 *
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void FinderRotomFade::Init( APP_PARAM* pAppParam, void* pAppLytBuff )
{
  {
    m_pAppParam = pAppParam;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );

  m_Seq = SEQ_IDLE;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderRotomFade::Initialize2D( void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_ROTOM_FADE_ROTOM_FADE_UPP_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_ROTOM_FADE_ROTOM_FADE_LOW_000_BFLYT,
      LA_ROTOM_FADE_ROTOM_FADE_LOW_000___NUM,
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
    GFL_NELEMS(setupData),
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    this->UNUSE_MESSAGE_DATA );

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  {
    for( u32 i=0; i<DRAW_NUM; ++i )
    {
      m_pDrawerBase[i] = GFL_NEW(heap) DrawerBase( this, m_pHeap, i, NULL, NULL );
      GFL_ASSERT( m_pDrawerBase[i] );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderRotomFade::End( void )
{
  for( u32 i=0; i<DRAW_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pDrawerBase[i] );
  }

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderRotomFade::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_FADE:
    {
      if( IsEndRotomFade() )
      {
        this->SetVisible( false );

        m_Seq = SEQ_END;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FinderRotomFade::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, DRAW_UPP, 130 );
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, DRAW_LOW, 130 );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   アニメの開始
 */
//------------------------------------------------------------------------------
void FinderRotomFade::StartRotomFade( void )
{
  // 上画面
  if( m_pDrawerBase[DRAW_UPP] )
  {
    DrawerBase* base = m_pDrawerBase[DRAW_UPP];
    {
      base->StartAnim( LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 );
    }
  }

  // 下画面
  if( m_pDrawerBase[DRAW_LOW] )
  {
    DrawerBase* base = m_pDrawerBase[DRAW_LOW];
    {
      base->StartAnim( LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00 );
    }
  }

  Sound::PlaySE( SEQ_SE_SYS_NJ_028 );
  Sound::PlaySE( SEQ_SE_SYS_NJ_029 );

  m_Seq = SEQ_FADE;
}

//------------------------------------------------------------------------------
/**
 * @brief   アニメの終了
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool FinderRotomFade::IsEndRotomFade( void )
{
  if( m_pDrawerBase[DRAW_UPP] )
  {
    if( !m_pDrawerBase[DRAW_UPP]->IsAnimEnd( LA_ROTOM_FADE_ROTOM_FADE_UPP_000_FADE_00 ) )
    {
      return false;
    }
  }

  if( m_pDrawerBase[DRAW_LOW] )
  {
    if( !m_pDrawerBase[DRAW_LOW]->IsAnimEnd( LA_ROTOM_FADE_ROTOM_FADE_LOW_000_FADE_00 ) )
    {
      return false;
    }
  }

  return true;
}

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )
