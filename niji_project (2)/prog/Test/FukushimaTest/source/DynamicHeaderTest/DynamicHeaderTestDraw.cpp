#if PM_DEBUG

//======================================================================
/**
 * @file    DynamicHeaderTestDraw.cpp
 * @date    2015/10/06 13:23:26
 * @author  fukushima_yuya
 * @brief   DynamicHeaderテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/DynamicHeaderTest/DynamicHeaderTestDraw.h>

#include <App/LangSelect/source/LangSelectDrawListener.h>
#include <App/LangSelect/source/LangSelectDrawerBase.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// arc
#include <niji_conv_header/app/lang_select/lang_select.h>
#include <niji_conv_header/app/lang_select/lang_select_pane.h>
#include <niji_conv_header/app/lang_select/lang_select_anim_list.h>
#include <niji_conv_header/message/msg_language_select.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

DynamicHeaderTestDraw::DynamicHeaderTestDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRanderingManager )
{

}

void DynamicHeaderTestDraw::Init( void* arcBuf, void* msgBuf )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( arcBuf, msgBuf );
  InitializeText();

  SetInputListener( this );
}

bool DynamicHeaderTestDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  GFL_SAFE_DELETE( m_pWordSet );

  GFL_SAFE_DELETE( m_pMsgData );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

void DynamicHeaderTestDraw::Update( void )
{
  app::ui::UIView::Update();
}

void DynamicHeaderTestDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

bool DynamicHeaderTestDraw::IsExit( void )
{
  return true;
}

void DynamicHeaderTestDraw::SetTitleViewUIListener( App::LangSelect::LangSelectDrawListener* pListener )
{
  m_uiListener = pListener;
}

void DynamicHeaderTestDraw::Initialize2D( void* arcBuf, void* msgBuf )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { arcBuf, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_LANG_SELECT_LANGSELECT_MIN_UPP_BFLYT,
      LA_LANG_SELECT_LANGSELECT_MIN_UPP___NUM,
      arcBuf,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  m_pMsgData = GFL_NEW(devHeap) gfl2::str::MsgData( msgBuf, devHeap );
  GFL_ASSERT( m_pMsgData );

  m_pWordSet = GFL_NEW(devHeap) print::WordSet( devHeap );
  GFL_ASSERT( m_pWordSet );

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

  //m_pDrawerBase = GFL_NEW(devHeap) App::LangSelect::DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  //GFL_ASSERT( m_pDrawerBase );
}

void DynamicHeaderTestDraw::InitializeText( void )
{

}

app::ui::UIInputListener::ListenerResult DynamicHeaderTestDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_uiListener->EndLangSelect();
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG