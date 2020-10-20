#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestUpperDraw.cpp
 * @date    2016/10/21 16:36:23
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ImageDBTestUpperDraw.h"
#include "ImageDBTestDrawListener.h"

#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
UpperDraw::UpperDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pListener(NULL)
  , m_pPicPortrait( NULL )
  , m_pPicSidewise( NULL )
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
bool UpperDraw::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  return true;
}

void UpperDraw::Initialize2D( void* lytDatBuf )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuf, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 上
      0,
      LYTRES_FINDERSTUDIOUITEST_FINDER_STUDIO_UP_000_BFLYT,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_UP_000___NUM,
      lytDatBuf,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    }
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS( resTbl ),
    setupData,
    GFL_NELEMS( setupData ),
    this->UNUSE_MESSAGE_DATA );
}

void UpperDraw::InitializePane( void )
{
  gfl2::lyt::LytWk* lytwk = GetG2DUtil()->GetLayoutWork(0);

  m_pPicPortrait = lytwk->GetPicturePane( PANENAME_FINDER_STUDIO_UP_000_PANE_PHOTO_DUMMY );
  m_pPicSidewise = lytwk->GetPicturePane( PANENAME_FINDER_STUDIO_UP_000_PANE_PHOTO_DUMMY_01 );
}

bool UpperDraw::Terminate( void )
{
  Delete2D();
  DeleteGraphicsSystem();

  return true;
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void UpperDraw::Update( void )
{
  app::ui::UIView::Update();
}

void UpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}


GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
