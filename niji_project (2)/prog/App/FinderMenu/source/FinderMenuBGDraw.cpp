//======================================================================
/**
 * @file    FinderMenuBGDraw.cpp
 * @date    2016/01/14 17:56:30
 * @author  fukushima_yuya
 * @brief   ポケファインダーメニュー：背景
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuBGDraw.h"
#include "FinderMenuDrawerBase.h"

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuBGDraw::FinderMenuBGDraw( CreateParam& param )
  : app::ui::UIView( param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator() )
  , m_pHeap( param.heap )
  , m_pAppRenderingManager( param.renderMan )
  , m_pDrawerBase( NULL )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppLytBuff   レイアウトバイナリ
 */
//------------------------------------------------------------------------------
void FinderMenuBGDraw::Init( void* pAppLytBuff )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderMenuBGDraw::Initialize2D( void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_BG_LOW_000_BFLYT,
      LA_FINDER_MENU_FINDER_BG_LOW_000___NUM,
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
    this->UNUSE_MESSAGE_DATA );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, NULL, NULL );
  {
    GFL_ASSERT( m_pDrawerBase );

    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_BG_LOW_000_LOOPANIM );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuBGDraw::End( void )
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
void FinderMenuBGDraw::Update( void )
{
  app::ui::UIView::Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FinderMenuBGDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();
  {
    g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, 120 );
  }
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
