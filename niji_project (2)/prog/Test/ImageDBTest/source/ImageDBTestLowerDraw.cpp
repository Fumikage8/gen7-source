#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestLowerDraw.cpp
 * @date    2016/10/24 19:29:13
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ImageDBTestLowerDraw.h"
#include "ImageDBTestDrawListener.h"
#include "search/ImageDBTestSearch.h"

#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

namespace {
  enum BtnID {
    BTN_ID_SHOOT,
    BTN_ID_SHOOT_SAVE,
    BTN_ID_LOAD,
    BTN_ID_SEARCH,
    BTN_ID_THUMB,
    BTN_ID_NUM,
  };
}

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
LowerDraw::LowerDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pListener( NULL )
  , m_PhotoNum( 0 )
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
bool LowerDraw::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  SetInputListener( this );

  return true;
}

bool LowerDraw::Terminate( void )
{
  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void LowerDraw::Initialize2D( void* lytDatBuf )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuf, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 下
      0,
      LYTRES_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000_BFLYT,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000___NUM,
      lytDatBuf,
      true,
      app::util::G2DUtil::SETUP_LOWER,
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

void LowerDraw::InitializeButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_SHOOT, NULL, NULL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_TOUCH,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_CANSEL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_TOUCH_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_ACTIVE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_04_PASSIVE,
    },
    {
      BTN_ID_SHOOT_SAVE, NULL, NULL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_TOUCH,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_CANSEL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_ACTIVE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_00_PASSIVE,
    },
    {
      BTN_ID_LOAD, NULL, NULL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_TOUCH,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_CANSEL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_ACTIVE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_01_PASSIVE,
    },
    {
      BTN_ID_SEARCH, NULL, NULL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_TOUCH,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_CANSEL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_TOUCH_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_ACTIVE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_02_PASSIVE,
    },
    {
      BTN_ID_THUMB, NULL, NULL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_TOUCH,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_CANSEL,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_TOUCH_RELEASE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_ACTIVE,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_000__BUTTON_03_PASSIVE,
    },
  };

  const u32 c_PartsPaneIndex[] = {
    PANENAME_FINDER_STUDIO_LOW_000_PANE_BUTTON_04,
    PANENAME_FINDER_STUDIO_LOW_000_PANE_BUTTON_00,
    PANENAME_FINDER_STUDIO_LOW_000_PANE_BUTTON_01,
    PANENAME_FINDER_STUDIO_LOW_000_PANE_BUTTON_02,
    PANENAME_FINDER_STUDIO_LOW_000_PANE_BUTTON_03,
  };

  app::ui::ButtonInfoEx* info =
    GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  gfl2::lyt::LytWk* lytwk = GetG2DUtil()->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = GetG2DUtil()->GetLayoutResourceID( 0 );

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    info[i] = c_DefInfo[i];

    gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( c_PartsPaneIndex[i] );
    info[i].picture_pane = parts;

    gfl2::lyt::LytPane* bound = lytwk->GetPane( parts, PANENAME_COMMON_BTN_LOW_005_PANE_BOUND_00, &resID );
    info[i].bound_pane = bound;
  }

  app::ui::UIView* view = this;
  {
    view->CreateButtonManager( m_pHeap, lytwk, info, BTN_ID_NUM );
  }

  GFL_DELETE( info );

  GetButtonManager()->SetButtonPassive( BTN_ID_LOAD );
  GetButtonManager()->SetButtonPassive( BTN_ID_THUMB );
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void LowerDraw::Update( void )
{
  app::ui::UIView::Update();
}

void LowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerDraw::OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  if( btn->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_pListener->OnEndImageDBTest();
  }

#if defined(GF_PLATFORM_CTR)
  if( btn->IsTrigger( gfl2::ui::BUTTON_UP ) )
  {
    if( m_PhotoNum > 0 )
    {
      m_PhotoNum--;
      GFL_PRINT( "Photo Index[%d]\n", m_PhotoNum );
    }
  }
  else if( btn->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
  {
    if( m_PhotoNum < ( ImageDBSearch::m_PhotoNum - 1 ) )
    {
      m_PhotoNum++;
      GFL_PRINT( "Photo Index[%d]\n", m_PhotoNum );
    }
  }
#endif // defined(GF_PLATFORM_CTR)

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerDraw::OnLayoutPaneEvent( const u32 paneID )
{
#if defined(GF_PLATFORM_CTR)
  if( paneID == BTN_ID_SHOOT )
  {
    m_pListener->OnRequestPhotoShoot();
  }
  if( paneID == BTN_ID_SHOOT_SAVE )
  {
    m_pListener->OnRequestImgdbSave();
  }
  if( paneID == BTN_ID_LOAD )
  {
    m_pListener->OnRequestImgdbLoad_Snap( m_PhotoNum );
  }
  if( paneID == BTN_ID_SEARCH )
  {
    m_pListener->OnRequestImgdbSearch();

    if( ImageDBSearch::m_PhotoNum > 0 )
    {
      GetButtonManager()->SetButtonActive( BTN_ID_LOAD );
      GetButtonManager()->SetButtonActive( BTN_ID_THUMB );
    }
  }
  if( paneID == BTN_ID_THUMB )
  {
    m_pListener->OnRequestOpenAlbum();
  }
#endif // defined(GF_PLATFORM_CTR)

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
