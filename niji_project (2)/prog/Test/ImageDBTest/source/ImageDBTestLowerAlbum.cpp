#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestLowerAlbum.cpp
 * @date    2016/10/25 17:16:33
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ImageDBTestLowerAlbum.h"
#include "ImageDBTestDrawListener.h"
#include "debug/ImageDBTestTick.h"
#include "search/ImageDBTestSearch.h"

#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/finder_studio_test/FinderStudioUITest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

namespace {
  enum BtnID {
    BTN_ID_THUMB_00,
    BTN_ID_THUMB_01,
    BTN_ID_THUMB_02,
    BTN_ID_THUMB_03,
    BTN_ID_THUMB_04,
    BTN_ID_THUMB_05,
    BTN_ID_THUMB_06,
    BTN_ID_THUMB_07,
    BTN_ID_THUMB_08,
    BTN_ID_THUMB_09,
    BTN_ID_THUMB_10,
    BTN_ID_THUMB_11,
    BTN_ID_THUMB_12,
    BTN_ID_THUMB_13,
    BTN_ID_THUMB_14,
    BTN_ID_THUMB_15,
    BTN_ID_CANCEL,
    BTN_ID_NUM,
  };
}

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
LowerAlbumDraw::LowerAlbumDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pListener( NULL )
  , m_Seq(0)
  , m_LoadIndex( 0 )
  , m_LoadCount(0)
  , m_PageNum( 0 )
  , m_PageMax(0)
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
bool LowerAlbumDraw::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  SetInputListener( this );

  return true;
}

bool LowerAlbumDraw::Terminate( void )
{
  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void LowerAlbumDraw::Initialize2D( void* lytDatBuf )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuf, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 下
      0,
      LYTRES_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000_BFLYT,
      LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000___NUM,
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

  SetVisible( false );
  SetInputEnabled( false );
}

void LowerAlbumDraw::InitializeButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo = {
    BTN_ID_THUMB_00, NULL, NULL,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_TOUCH,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_RELEASE,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_CANSEL,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_TOUCH_RELEASE,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_ACTIVE,
    LA_FINDERSTUDIOUITEST_FINDER_STUDIO_LOW_THUMB_000__PHOTOBUTTON_00_PASSIVE,
  };

  app::ui::ButtonInfoEx* info =
    GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  gfl2::lyt::LytWk* lytwk = GetG2DUtil()->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = GetG2DUtil()->GetLayoutResourceID( 0 );

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    u32 add_anim = i * 6;
    info[i] = c_DefInfo;
    info[i].button_id += i;
    info[i].touch_anime_index += add_anim;
    info[i].release_anime_index += add_anim;
    info[i].cancel_anime_index += add_anim;
    info[i].key_select_anime_index += add_anim;
    info[i].active_anime_index += add_anim;
    info[i].passive_anime_index += add_anim;

    if( i < BTN_ID_CANCEL )
    {
      gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( PANENAME_FINDER_STUDIO_LOW_THUMB_000_PANE_PHOTOBUTTON_00 + i );
      info[i].picture_pane = parts;

      gfl2::lyt::LytPane* bound = lytwk->GetPane( parts, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00, &resID );
      info[i].bound_pane = bound;

      m_pThumbPicture[i] = lytwk->GetPicturePane( parts, PANENAME_FINDER_BUTTON_LOW_004_PANE_PHOTODUMMY, &resID );
    }
    else {
      gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( PANENAME_FINDER_STUDIO_LOW_THUMB_000_PANE_CANCELBTN );
      info[i].picture_pane = parts;

      gfl2::lyt::LytPane* bound = lytwk->GetPane( parts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resID );
      info[i].bound_pane = bound;
    }
  }

  app::ui::UIView* view = this;
  {
    view->CreateButtonManager( m_pHeap, lytwk, info, BTN_ID_NUM );
  }

  GFL_DELETE( info );

  GetButtonManager()->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B );
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void LowerAlbumDraw::Update( void )
{
  app::ui::UIView::Update();

#if defined(GF_PLATFORM_CTR)
  if( m_Seq == 1 )
  {
    // ロードリクエスト
    m_pListener->OnRequestImgdbLoad_Thumb( m_LoadIndex, m_LoadCount, m_pThumbPicture[m_LoadCount] );
    m_Seq++;
  }
  if( m_Seq == 3 )
  {
    m_LoadIndex++;
    m_LoadCount++;

    if( m_LoadIndex == ImageDBSearch::m_PhotoNum || m_LoadCount == THUMB_MAX )
    {
      m_Seq++;
    }
    else {
      m_Seq = 1;
    }
  }
  if( m_Seq == 4 )
  {
    for( u32 i = 0; i < THUMB_MAX; ++i )
    {
      if( i < m_LoadCount ) GetButtonManager()->SetButtonVisible( BTN_ID_THUMB_00 + i );
      else GetButtonManager()->SetButtonInvisible( BTN_ID_THUMB_00 + i );
    }

    this->SetVisible( true );
    this->SetInputEnabled( true );
    m_Seq = 0;

    DebugTick::EndTick( TICK_ID_IMGDB_LOAD_THUMB_TOTAL );
    DebugTick::DumpTick( TICK_ID_IMGDB_LOAD_THUMB_TOTAL, "ImgdbLoadThumbTotal" );
  }
#endif // defined(GF_PLATFORM_CTR)
}

void LowerAlbumDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}



//----------------------------------------------------------------------
// @brief   オープン・クローズ
//----------------------------------------------------------------------
void LowerAlbumDraw::Open( void )
{
  this->SetModal( true );
#if defined(GF_PLATFORM_CTR)
  UpdateThumbnail();
#else
  this->SetVisible( true );
  this->SetInputEnabled( true );
#endif // defined(GF_PLATFORM_CTR)
}

void LowerAlbumDraw::Close( void )
{
  this->SetInputEnabled( false );
  this->SetVisible( false );
  this->SetModal( false );
}



//----------------------------------------------------------------------
// @brief   サムネイル更新
//----------------------------------------------------------------------
void LowerAlbumDraw::UpdateThumbnail( void )
{
#if defined(GF_PLATFORM_CTR)
  DebugTick::StartTick( TICK_ID_IMGDB_LOAD_THUMB_TOTAL );

  this->SetInputEnabled( false );

  m_PageMax = ImageDBSearch::m_PhotoNum / THUMB_MAX;
  if( ( ImageDBSearch::m_PhotoNum % THUMB_MAX ) != 0 ) m_PageMax++;
  GFL_PRINT( "Album PageMax=[%d]\n", m_PageMax );

  m_LoadIndex = m_PageNum * THUMB_MAX;
  m_LoadCount = 0;
  m_Seq = 1;
#endif // defined(GF_PLATFORM_CTR)
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerAlbumDraw::OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
#if defined(GF_PLATFORM_CTR)
  if( btn->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    if( m_PageNum > 0 )
    {
      m_PageNum--;
      UpdateThumbnail();
    }
  }
  if( btn->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    if( m_PageNum < (m_PageMax - 1) )
    {
      m_PageNum++;
      UpdateThumbnail();
    }
  }
#endif // defined(GF_PLATFORM_CTR)

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerAlbumDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( paneID == BTN_ID_CANCEL )
  {
    m_pListener->OnRequestCloseAlbum();
  }
#if defined(GF_PLATFORM_CTR)
  if( paneID >= BTN_ID_THUMB_00 && paneID <= BTN_ID_THUMB_15 )
  {
    u32 photoIndex = (paneID - BTN_ID_THUMB_00) + (m_PageNum * THUMB_MAX);
    m_pListener->OnRequestImgdbLoad_Snap( photoIndex );
  }
#endif // defined(GF_PLATFORM_CTR)

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
