#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestLowerTabDraw.cpp
 * @date    2016/10/28 11:40:23
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestLowerTabDraw.h"

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>
#include <Test/FStudioDecoTest/source/FStudioDecoTestDrawListener.h>
#include <Test/FStudioDecoTest/source/util/FStudioDecoTestUIUtility.h>

#include <Sound/include/Sound.h>

#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_pane.h>
#include <niji_conv_header/app/test/fukushima_test/fstudio_deco_test/FStudioDecoTest_anim_list.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

LowerTabDraw::LowerTabDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pUtil( NULL )
  , m_pListener(NULL)
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
bool LowerTabDraw::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  SetInputListener( this );

  return true;
}

bool LowerTabDraw::Terminate( void )
{
  m_pUtil->ResetInputExclusiveKeyByBtnAnim();
  GFL_SAFE_DELETE( m_pUtil );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void LowerTabDraw::Initialize2D( void* lytDatBuf )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuf, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 下
      0,
      LYTRES_FSTUDIODECOTEST_DECOTEST_LOWTAB_BFLYT,
      LA_FSTUDIODECOTEST_DECOTEST_LOWTAB___NUM,
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

  m_pUtil = GFL_NEW( m_pHeap->GetSystemHeap() ) UIUtility( this );
  m_pUtil->SetPrevInputExclusiveKeyByBtnAnim();
  m_pUtil->SetInputExclusiveKeyByBtnAnim( true );
}

void LowerTabDraw::InitializeButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_DECO_00, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTN_ID_DECO_01, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTN_ID_DECO_02, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTN_ID_DECO_03, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
  };

  const u32 c_PartsPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_DECOTEST_LOWTAB_PANE_DECOTAB_00, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
    { PANENAME_DECOTEST_LOWTAB_PANE_DECOTAB_01, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
    { PANENAME_DECOTEST_LOWTAB_PANE_DECOTAB_02, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
    { PANENAME_DECOTEST_LOWTAB_PANE_DECOTAB_03, PANENAME_STATUS_BTN_LOW_001_PANE_BOUND_00 },
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

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE( info );
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
void LowerTabDraw::Update( void )
{
  app::ui::UIView::Update();
}

void LowerTabDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  this->GetG2DUtil()->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerTabDraw::OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{

  return ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult LowerTabDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( paneID >= BTN_ID_DECO_00 && paneID <= BTN_ID_DECO_03 )
  {
    u32 type = paneID - BTN_ID_DECO_00;

    m_pListener->OnOpenDecoWindow( CAST_DECO_TYPE(type) );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
