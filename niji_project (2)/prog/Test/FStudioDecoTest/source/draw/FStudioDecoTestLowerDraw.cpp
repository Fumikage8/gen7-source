#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestLowerDraw.cpp
 * @date    2016/10/27 13:05:13
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestLowerDraw.h"

#include <Test/FStudioDecoTest/source/FStudioDecoTestDrawListener.h>
#include <Test/FStudioDecoTest/source/deco/FStudioDecoTestItem.h>
#include <Test/FStudioDecoTest/source/deco/FStudioDecoTestItemList.h>
#include <Test/FStudioDecoTest/source/util/FStudioDecoTestUIUtility.h>
#include <Test/ImageDBTest/source/util/ImageDBTestUtility.h>
#include <Sound/include/Sound.h>

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/DecoItemData.cdat>

//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
LowerDraw::LowerDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pUtil(NULL)
  , m_pListener(NULL)
  , m_pActiveDeco(NULL)
  , m_Wait(0)
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
  GFL_SAFE_DELETE( m_pItemList );

  m_pUtil->ResetInputExclusiveKeyByBtnAnim();
  GFL_SAFE_DELETE( m_pUtil );

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
      LYTRES_FSTUDIODECOTEST_DECOTEST_LOWCANVAS_BFLYT,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS___NUM,
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

void LowerDraw::InitializeMember( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetSystemHeap();

  // utility
  m_pUtil = GFL_NEW( heap ) UIUtility( this );
  m_pUtil->SetPrevInputExclusiveKeyByBtnAnim();
  m_pUtil->SetInputExclusiveKeyByBtnAnim( true );

  m_pNullDecoParent = m_pUtil->GetPane( PANENAME_DECOTEST_LOWCANVAS_PANE_NULL_DECO );

  // ItemList
  m_pItemList = GFL_NEW( heap ) ItemList( m_pNullDecoParent );
}

void LowerDraw::InitializeButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_TOUCH,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_RELEASE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_CANSEL,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_TOUCH_RELEASE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_ACTIVE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__BACK_PASSIVE,
    },
    {
      BTN_ID_SHOOT, NULL, NULL,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_TOUCH,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_RELEASE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_CANSEL,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_TOUCH_RELEASE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_ACTIVE,
      LA_FSTUDIODECOTEST_DECOTEST_LOWCANVAS__DECIDE_PASSIVE,
    },
  };

  const u32 c_PartsPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_DECOTEST_LOWCANVAS_PANE_BACK, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { PANENAME_DECOTEST_LOWCANVAS_PANE_DECIDE, PANENAME_COMMON_BTN_LOW_022_PANE_BOUND_00 },
  };

  app::ui::ButtonInfoEx* info =
    GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    info[i] = c_DefInfo[i];

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_PartsPaneIndex[i][0] );
    info[i].picture_pane = parts;

    gfl2::lyt::LytPane* bound = m_pUtil->GetPane( c_PartsPaneIndex[i][1], parts );
    info[i].bound_pane = bound;
  }

  app::ui::UIView* view = this;
  view->CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  btnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B );
  btnMan->SetButtonSelectSE( BTN_ID_CANCEL, SEQ_SE_CANCEL1 );
  btnMan->SetButtonSelectSE( BTN_ID_SHOOT, SEQ_SE_DECIDE1 );

  GFL_DELETE( info );
}

void LowerDraw::InitializePhoto( void* photoBuf )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::lyt::LytPicture* picture = m_pUtil->GetPicture( PANENAME_DECOTEST_LOWCANVAS_PANE_CANVAS );
  ImageDB::ImageDBUtil::ReplaceTexture_LoadSnap( photoBuf, picture );
#endif
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void LowerDraw::Update( void )
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

void LowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}



void LowerDraw::AddDecoItem( void )
{
  DecoItemKind kind = DecoItem::GetDecoItemKind();
  m_pItemList->AddItem( DecoItem::CreateDecoItem_Lower( m_pHeap->GetDeviceHeap(), m_pUtil ), kind );
  if( kind != DECO_ITEM_KIND_FRAME ) m_pActiveDeco = m_pItemList->GetActiveItem();
  m_Wait = 0;
}

void LowerDraw::RemoveDecoItem( void )
{
  m_pUtil->SetVisiblePane( m_pItemList->GetActiveItem(), false );  
  m_pItemList->RemoveItem();
}

void LowerDraw::SetDecoItem( u32 index )
{
  m_pActiveDeco = m_pItemList->GetItem( index );
  m_pItemList->SetActiveItem( m_pActiveDeco );
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( isTouch )
  {
    if( m_pActiveDeco )
    {
      // ドラッグ移動
      gfl2::math::VEC3 pos = DecoItem::TransferLayoutCoordinate( pTouchPanel );
      m_pUtil->SetPanePos( m_pActiveDeco, pos );
      m_MoveParam.pos = pos;
      m_pListener->OnUpdateTouchPoint( pos );
    }
    else {
      for( s32 i = ItemList::ITEM_MAX - 1; i >= 0; --i )
      {
        gfl2::lyt::LytPicture* picture = m_pItemList->GetItem( i, true );

        if( picture )
        {
          if( GetG2DUtil()->IsTouchPane( pTouchPanel, m_pUtil->GetLytID(), picture ) )
          {
            m_pListener->OnSetDecoItem( m_pItemList->GetItemIndex(picture) );
            m_MoveParam.move = true;
            m_MoveParam.decoIdx = static_cast<u32>( i );
            break;
          }
        }
      }
    }
  }
  else {
    GFL_PRINT( "Prev x[%d] y[%d]\n", pTouchPanel->GetPrevX(), pTouchPanel->GetPrevY() );
    GFL_PRINT( "Pos  x[%d] y[%d]\n", m_MoveParam.pos.x, m_MoveParam.pos.y );

    //if( m_pUtil->GetLytWk()->GetHitPane( m_MoveParam.pos.x, m_MoveParam.pos.y, PANENAME_DECOTEST_LOWCANVAS_PANE_DELETE ) )
    if( m_pUtil->GetLytWk()->GetHitPane( pTouchPanel->GetPrevX(), pTouchPanel->GetPrevY(), PANENAME_DECOTEST_LOWCANVAS_PANE_DELETE ) )
    {
      GFL_PRINT( "RemoveDecoItem\n" );
      m_pListener->OnRemoveDecoItem();
    }

    m_MoveParam.move = false;
    m_pActiveDeco = NULL;
  }

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerDraw::OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( paneID == BTN_ID_CANCEL )
  {
    m_pListener->OnEndDecoTest();
  }
#if defined(GF_PLATFORM_CTR)
  if( paneID == BTN_ID_SHOOT )
  {
    m_pListener->OnRequestShootAndSave();
  }
#endif // defined(GF_PLATFORM_CTR)

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
