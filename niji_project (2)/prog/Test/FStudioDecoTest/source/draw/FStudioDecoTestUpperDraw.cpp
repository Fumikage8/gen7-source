#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestUpperDraw.cpp
 * @date    2016/10/27 13:00:25
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestUpperDraw.h"

#include <Test/FStudioDecoTest/source/deco/FStudioDecoTestItem.h>
#include <Test/FStudioDecoTest/source/deco/FStudioDecoTestItemList.h>
#include <Test/FStudioDecoTest/source/util/FStudioDecoTestUIUtility.h>
#include <Test/ImageDBTest/source/util/ImageDBTestUtility.h>

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/DecoItemData.cdat>

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
UpperDraw::UpperDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pUtil( NULL )
  , m_pListener( NULL )
  , m_pItemList(NULL)
  , m_pActiveDeco(NULL)
  , m_Wait(0)
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
      LYTRES_FSTUDIODECOTEST_DECOTEST_UPPCANVAS_BFLYT,
      LA_FSTUDIODECOTEST_DECOTEST_UPPCANVAS___NUM,
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

void UpperDraw::InitializeMember( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();

  // utility
  m_pUtil = GFL_NEW( heap ) UIUtility( this );

  // ペインの初期化
  m_pNullDecoParent = m_pUtil->GetPane( PANENAME_DECOTEST_UPPCANVAS_PANE_NULL_DECO );

  // itemList
  m_pItemList = GFL_NEW( heap ) ItemList( m_pNullDecoParent );
}

bool UpperDraw::Terminate( void )
{
  GFL_SAFE_DELETE( m_pItemList );
  GFL_SAFE_DELETE( m_pUtil );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void UpperDraw::InitializePhoto( void* photoBuf )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::lyt::LytPicture* picture = m_pUtil->GetPicture( PANENAME_DECOTEST_UPPCANVAS_PANE_PHOTODUMMY00 );
  ImageDB::ImageDBUtil::ReplaceTexture_LoadSnap( photoBuf, picture );
#endif
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void UpperDraw::Update( void )
{
  app::ui::UIView::Update();

  if( m_Wait == 0 )
  {
    m_Wait++;
  }
  else if( m_Wait == 1 )
  {
    m_pItemList->SetAllItemVisible( true );
    m_Wait++;
  }
}

void UpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

void UpperDraw::UpdateTouchPoint( gfl2::math::VEC3 vec )
{
  if( m_pActiveDeco )
  {
    vec.x /= LOW_CANVAS_SCALE_X;
    vec.y /= LOW_CANVAS_SCALE_Y;

    m_pUtil->SetPanePos( m_pActiveDeco, vec );
  }
}



void UpperDraw::AddDecoItem( void )
{
  DecoItemKind kind = DecoItem::GetDecoItemKind();
  // デコアイテムの追加
  m_pItemList->AddItem( DecoItem::CreateDecoItem_Upper( m_pHeap->GetDeviceHeap(), m_pUtil ), kind );
  // 
  if( kind != DECO_ITEM_KIND_FRAME ) m_pActiveDeco = m_pItemList->GetActiveItem();
  m_Wait = 0;
}

void UpperDraw::RemoveDecoItem( void )
{
  m_pUtil->SetVisiblePane( m_pItemList->GetActiveItem(), false );
  // デコアイテムの削除
  m_pItemList->RemoveItem();
}

void UpperDraw::SetDecoItem( u32 index )
{
  m_pActiveDeco = m_pItemList->GetItem( index );
  m_pItemList->SetActiveItem( m_pActiveDeco );
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
