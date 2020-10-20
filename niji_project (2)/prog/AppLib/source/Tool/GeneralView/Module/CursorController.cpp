//==============================================================================
/**
 * @file   CursorController.cpp
 * @date   2015/09/30 Wed. 15:33:26
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "Sound/include/Sound.h"

#include  "AppLib/include/Tool/app_tool_MenuCursor.h"
#include  "AppLib/include/Util/AppRenderingManager.h"
#include  "./CursorController.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const app::tool::MenuCursor::CursorSize  CURSOR_SIZE = app::tool::MenuCursor::SIZE_SMALL;

  static const u32  SEID_MOVEUP   = SEQ_SE_SELECT1;
  static const u32  SEID_MOVEDOWN = SEQ_SE_SELECT1;
  static const u32  SEID_DECIDE   = SEQ_SE_DECIDE1;
}



namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
CursorController::CursorController(app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager)
  : m_pHeap(pHeap)
  , m_pRenderingManager(pRenderingManager)
  , m_SelectedIndexMax(0)
  , m_SelectedIndex(0)
  , m_pMenuCursor(NULL)
  , m_pEventHandler(NULL)
  , m_bEnable(true)
  , m_bForUpper(false)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
CursorController::~CursorController()
{
  GFL_SAFE_DELETE(m_pMenuCursor);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::Setup(
  void*                         pResource,
  gfl2::lyt::LytSys*            pLytSys,
  const gfl2::lyt::LytWkSetUp*  pLytWkSetUp,
  app::util::AppLytAccessor*    pAppLytAccessor,
  const bool                    bForUpper

)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();

  /*
    cursor
  */
  m_pMenuCursor = GFL_NEW(pHeapBase) MenuCursor(m_pHeap, &pResource);

  m_pMenuCursor->CreateLayoutWork(
    pLytSys,
    pLytWkSetUp,
    pAppLytAccessor,
    CURSOR_SIZE,
    false
  );
//  m_pMenuCursor->SetVisible( true );
  m_pMenuCursor->StartAnime();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  CursorController::Cleanup(void) const
{
  if(m_pMenuCursor)
    return  m_pMenuCursor->DeleteLayoutWork();
  

  return  true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::OnUpdate(void)
{
  if(m_bEnable)
  {
    if(m_pMenuCursor)
    {
      m_pMenuCursor->Update();
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  CursorController::OnDraw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_bEnable)
  {
    if(m_pMenuCursor)
    {
      m_pMenuCursor->Draw(m_pRenderingManager, displayNo, m_bForUpper ? gfl2::lyt::DISPLAY_UPPER : gfl2::lyt::DISPLAY_LOWER);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  CursorController::OnKeyAction(gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick)
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;


  /*
    以下カーソル未作成時には動作しない
  */
  if( !IsVisible() )
  {
    return app::ui::UIInputListener::ENABLE_ACCESS;
  }


  /*
    keys
  */
  {
    if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      /*  決定  */
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      Event(IEventHandler::EVENT_OnDecided);
    }
    else if(pKey->IsRepeat(gfl2::ui::BUTTON_UP))     result  = OnUp(   pKey->IsTrigger(gfl2::ui::BUTTON_UP));
    else if(pKey->IsRepeat(gfl2::ui::BUTTON_DOWN))   result  = OnDown( pKey->IsTrigger(gfl2::ui::BUTTON_DOWN));
    else if(pKey->IsRepeat(gfl2::ui::BUTTON_LEFT))   result  = OnLeft( pKey->IsTrigger(gfl2::ui::BUTTON_LEFT));
    else if(pKey->IsRepeat(gfl2::ui::BUTTON_RIGHT))  result  = OnRight(pKey->IsTrigger(gfl2::ui::BUTTON_RIGHT));
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::SetLocationNum(const u32 locationNum)
{
  GFL_ASSERT(locationNum <= LOCATION_MAX);

  if(locationNum <= LOCATION_MAX)
  {
    m_SelectedIndexMax  = locationNum;

    if(m_SelectedIndex >= m_SelectedIndexMax)
    {
      if(m_SelectedIndexMax)
      {
        m_SelectedIndex = m_SelectedIndexMax-1;
      }
      else
      {
        m_SelectedIndex = 0;
      }
    }
    
    if(m_SelectedIndexMax)  Show();
    else                    Hide();

    MoveTo(m_SelectedIndex);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  CursorController::RegisterLocation(const u32 locationIndex, gfl2::lyt::LytWk* pTargetLytWk, gfl2::lyt::LytPane* pTargetPane)
{
  GFL_ASSERT(locationIndex < LOCATION_MAX);

  if(locationIndex < LOCATION_MAX)
  {
    m_LocationInfo[locationIndex].pTargetLytWk  = pTargetLytWk;
    m_LocationInfo[locationIndex].pTargetPane   = pTargetPane;

    if(locationIndex == m_SelectedIndex)  MoveTo(m_SelectedIndex);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  CursorController::Show(void)
{
  m_bEnable = true;
  if(m_pMenuCursor) m_pMenuCursor->SetVisible(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::Hide(void)
{
  m_bEnable = false;
  if(m_pMenuCursor) m_pMenuCursor->SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  CursorController::IsVisible(void) const
{
  return (m_bEnable && (m_pMenuCursor ? m_pMenuCursor->IsVisible() : false));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->CursorController_OnEvent(this, eventCode);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CursorController::MoveTo(const u32 locationIndex)
{
  if(m_SelectedIndexMax)
  {
    m_SelectedIndex = (locationIndex % m_SelectedIndexMax);

    if(m_pMenuCursor && m_pMenuCursor->GetLayoutWork() && m_LocationInfo[m_SelectedIndex].IsValid())
    {
      /*  Pane::Get/SetGlobalMtx()を使用できればLytWkが不要 */
      m_pMenuCursor->PutNonVisible(m_LocationInfo[m_SelectedIndex].pTargetLytWk, m_LocationInfo[m_SelectedIndex].pTargetPane);
      Event(IEventHandler::EVENT_OnMove);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  CursorController::OnUp(const bool isTrigger)
{

  if(2 <= m_SelectedIndexMax)
  {
    if(isTrigger || (1 <= m_SelectedIndex))    /*  triggerEventのときのみループする  */
    {
      Sound::PlaySE(SEID_MOVEUP);
      MoveTo((m_SelectedIndex + (m_SelectedIndexMax-1)));
    }
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  CursorController::OnDown(const bool isTrigger)
{
  if(2 <= m_SelectedIndexMax)
  {
    if(isTrigger || (m_SelectedIndex < (m_SelectedIndexMax-1)))    /*  triggerEventのときのみループする  */
    {
      Sound::PlaySE(SEID_MOVEDOWN);
      MoveTo(m_SelectedIndex + 1);
    }
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}



} /*  namespace tool  */
} /*  namespace app   */
