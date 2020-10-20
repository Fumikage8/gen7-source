//==============================================================================
/**
  *  @file   NetAppCursorView.cpp
  *  @brief  
  *  @date   2015.06.16 Tue.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "Sound/include/Sound.h"
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const u32  SEID_MOVEUP   = SEQ_SE_SELECT1;
  static const u32  SEID_MOVEDOWN = SEQ_SE_SELECT1;
}


namespace NetAppLib  {
namespace UI {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppCursorView::NetAppCursorView(NetAppLib::System::ApplicationWorkBase* pWorkBase)
  : NetAppLib::System::NetApplicationViewBase(pWorkBase->GetAppHeap())   /*  super  */
  , m_pWorkBase(pWorkBase)
  /*  cursor  */
  , m_DefaultCursorController(pWorkBase->GetAppHeap(), pWorkBase->GetAppRenderingManager())
  , m_pCursorController(NULL)
  , m_CursorPosLayoutWorkID(0)
  , m_CursorPosLytMultiResID()
  , m_pMappedUIButtonIDTable(NULL)
  , m_MappedUIButtonIDTableElems(0)
  , m_bEnableCursor(true)
  /*  shortcut  */
//  , m_ShortcutButton_A(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_B(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_X(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_Y(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_L(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_R(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_RIGHT(app::tool::ButtonManager::BUTTON_ID_NULL)
  , m_ShortcutButton_LEFT(app::tool::ButtonManager::BUTTON_ID_NULL)
{
  SetCursorController();    /*  set to default  */
  SetInputListener(this);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppCursorView::~NetAppCursorView()
{
  /*  
    派生先で作成されたG2DUtilが渡されて初期化された際、
    先に派生側dtorでDelete2D()などされることになるため
    事実上ここではReleaseCursorできない
  */
}


//------------------------------------------------------------------
/**
  *  @brief   CursorControllerを指定する
  *  @note    CursorControllerはCreateCursorでSetupされるため、未SetupのものをCreateCursor前に限り設定可能
  */
//------------------------------------------------------------------
void  NetAppCursorView::SetCursorController(app::tool::CursorController* pCursorController)
{
  m_pCursorController = pCursorController ? pCursorController : &m_DefaultCursorController;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief   カーソル(NetAppMenuCursor)を生成する
  *  @param   menuCursorResourceID    [in]  カーソルのResourceID
  *  @param   layoutResourceIndex     [in]  「カーソルを乗せるためのボタンが含まれるLayout」のResourceを取得するためのIndex
  *  @param   layoutWorkID            [in]  「カーソルを乗せるためのボタンが含まれるLayoutWork」のID
  *  @note    本関数と対にReleaseCursorを明示的にコールする必要がある
  */
//------------------------------------------------------------------
void  NetAppCursorView::CreateCursor(
        NetAppLib::System::ResourceID               menuCursorResourceID,
        u32                                         layoutResourceIndex,
        u32                                         layoutWorkID
)
{
//  gfl2::heap::HeapBase*       pHeapBase       = m_pWorkBase->GetDeviceHeap();
  app::util::G2DUtil*         pG2DUtil        = GetG2DUtil();


  ReleaseCursor();
  m_pCursorController->Setup(
    m_pWorkBase->GetResourceManager()->GetResourceBuffer(menuCursorResourceID),
    pG2DUtil->GetLayoutSystem(),
    pG2DUtil->GetLayoutWorkSetup( /*(m_DisplayType == app::util::G2DUtil::SETUP_UPPER) ? gfl2::lyt::DISPLAY_UPPER :*/ gfl2::lyt::DISPLAY_LOWER ),
    pG2DUtil->GetAppLytAccessor(),
    false /*(m_DisplayType == app::util::G2DUtil::SETUP_UPPER)*/
  );
  m_pCursorController->SetEventHandler(this);

  m_CursorPosLayoutWorkID   = layoutWorkID;
  m_CursorPosLytMultiResID  = pG2DUtil->GetLayoutResourceID( layoutResourceIndex );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param   pButtonPartsPaneIDTable [in]  カーソル位置に対応するボタン部品のID配列
  *  @param   pCursorPaneIDTable      [in]  ボタン部品内カーソル位置定義NULLPaneのID配列、pButtonPartsPaneIDTableと一対一で対応すること
  *  @param   cursorPositionMax       [in]  カーソル位置の数（pButtonPartsPaneIDTable及びpCursorPaneIDTableの要素数）
  */
//------------------------------------------------------------------
void  NetAppCursorView::CreateCursor(
  NetAppLib::System::ResourceID                       menuCursorResourceID,
  u32                                                 layoutResourceIndex,
  u32                                                 layoutWorkID,
  const u32*                                          pButtonPartsPaneIDTable,
  const u32*                                          pCursorPaneIDTable,
  const app::tool::ButtonManager::ButtonId*           pMappedUIButtonIDTable,
  s32                                                 cursorPositionMax
)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWorkBase->GetDeviceHeap();


  ReleaseCursor();
  CreateCursor(menuCursorResourceID, layoutResourceIndex, layoutWorkID);
  SetCusorPosPanes(pButtonPartsPaneIDTable, pCursorPaneIDTable, pMappedUIButtonIDTable, cursorPositionMax);

  /*  maptable  */
  SetMappedUIButtonIDTable(pMappedUIButtonIDTable, cursorPositionMax);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppCursorView::CreateCursor(
  NetAppLib::System::ResourceID               menuCursorResourceID,
  u32                                         layoutResourceIndex,
  u32                                         layoutWorkID,
  gfl2::lyt::LytPane*                         pCursorPosPanes[],
  const app::tool::ButtonManager::ButtonId*   pMappedUIButtonIDTable,
  s32                                         cursorPositionMax
)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWorkBase->GetDeviceHeap();


  ReleaseCursor();
  CreateCursor(menuCursorResourceID, layoutResourceIndex, layoutWorkID);
  SetCusorPosPanes(pCursorPosPanes, pMappedUIButtonIDTable, cursorPositionMax);

  /*  maptable  */
  SetMappedUIButtonIDTable(pMappedUIButtonIDTable, cursorPositionMax);
}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppCursorView::SetCusorPosPanes(
        const u32*                                pButtonPartsPaneIDTable,
        const u32*                                pCursorPaneIDTable,
        const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable,
        const s32                                 cursorPositionMax
      )
{
  RegisterCursorLocations(pButtonPartsPaneIDTable, pCursorPaneIDTable, cursorPositionMax);
  SetMappedUIButtonIDTable(pMappedUIButtonIDTable, cursorPositionMax);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppCursorView::SetCusorPosPanes(
        gfl2::lyt::LytPane*                       pCursorPosPanes[],
        const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable,
        const s32                                 cursorPositionMax
      )
{
  RegisterCursorLocations(pCursorPosPanes, cursorPositionMax);
  SetMappedUIButtonIDTable(pMappedUIButtonIDTable, cursorPositionMax);
}





//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppCursorView::CursorMoveToButton(const app::tool::ButtonManager::ButtonId buttonID)
{
  for(u32 tableIndex = 0; tableIndex < m_MappedUIButtonIDTableElems; ++tableIndex)
  {
    if(m_pMappedUIButtonIDTable[tableIndex] == buttonID)
    {
      CursorMoveToIndex(tableIndex);
      break;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppCursorView::CursorMoveToIndex(const u32 mappedUIButtonIDTableIndex)
{
  if(mappedUIButtonIDTableIndex < m_MappedUIButtonIDTableElems)
  {
    m_pCursorController->MoveTo(mappedUIButtonIDTableIndex);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppCursorView::ReleaseCursor(void)
{
  GFL_SAFE_DELETE(m_pMappedUIButtonIDTable);

  /*  unregister shortcuts  */
  RegisterShortcut_B();
  RegisterShortcut_X();
  RegisterShortcut_Y();
  RegisterShortcut_L();
  RegisterShortcut_R();

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppCursorView::OnLayoutPaneTouchTriggerEvent( const u32 buttonID )
{
  CursorMoveToButton(buttonID);
}


//------------------------------------------------------------------
/**
  *  @func     OnKeyAction
  *  @brief    
  *  @note     buttonManagerにもBindKeyというショートカットキー機能がある
  *            しかしAボタンでUIを起動するにはOnKeyActionでStartSelectedActをコールする必要があり、
  *            その他のUIをBindKey実装としても管理箇所が分散されるだけでメリットがない（キーの排他も担保しにくくなる）ため、
  *            協議の結果敢えて自前で対応する方針を採ることとした
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  NetAppCursorView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;
  
  
  if( IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }

  /*  cursor  */
  if(m_bEnableCursor)
  {
    result  = m_pCursorController->OnKeyAction(pButton, pKey, pStick);
    if(result != ENABLE_ACCESS) return result;
  }

  /*
    button
    ショートカットキー機能だけはCreateCursor未コール時でも動作するようにしておく
  */
  {
    app::tool::ButtonManager*   pUIButtonManager  = GetButtonManager();

    /*  その他shortcut  */
    {
      const struct  {
        u32                                 triggerButtonID;
        app::tool::ButtonManager::ButtonId  shotrcutUIButtonID;
      }shortcutTable[]  = {
        {gfl2::ui::BUTTON_B,      m_ShortcutButton_B},
        {gfl2::ui::BUTTON_X,      m_ShortcutButton_X},
        {gfl2::ui::BUTTON_Y,      m_ShortcutButton_Y},
        {gfl2::ui::BUTTON_L,      m_ShortcutButton_L},
        {gfl2::ui::BUTTON_R,      m_ShortcutButton_R},
        {gfl2::ui::BUTTON_RIGHT,  m_ShortcutButton_RIGHT},
        {gfl2::ui::BUTTON_LEFT,   m_ShortcutButton_LEFT},
      };

      for(int num = 0; num < GFL_NELEMS(shortcutTable); ++num)
      {
        if((shortcutTable[num].shotrcutUIButtonID != app::tool::ButtonManager::BUTTON_ID_NULL) && pButton->IsTrigger(shortcutTable[num].triggerButtonID))
        {
          /*  入力に対応したUIボタンを動作させる  */
          pUIButtonManager->StartSelectedAct( shortcutTable[num].shotrcutUIButtonID );
          return DISABLE_ACCESS;
        }
      }
    }
  }

  return result;
}



/*  implement UIView I/F  */
void  NetAppCursorView::Update(void)
{
  NetAppLib::System::NetApplicationViewBase::Update();    /*  super  */

  if(m_bEnableCursor)
    m_pCursorController->OnUpdate();
}


void  NetAppCursorView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_bEnableCursor)
    m_pCursorController->OnDraw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  NetAppCursorView::IsDrawing(void) const
{
  return  (m_pCursorController->Cleanup() && this->app::ui::UIView::IsDrawing());
}
 /*  endimplement UIView I/F  */


/**
 * @brief カーソル表示・非表示設定
 * @param bVisible  trueで表示
 */
void NetAppCursorView::SetCursorVisible( bool bVisible )
{
  m_bEnableCursor = bVisible;
  if(bVisible)  m_pCursorController->Show();
  else          m_pCursorController->Hide();
}


/**
 * @brief   カーソルが表示されているか
 * @retval  true  = 表示されている
 * @retval  false = それ以外
 */
bool NetAppCursorView::IsCursorVisible() const
{
  return  m_pCursorController->IsVisible();

}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement CursorController::IEventHandler  */
void  NetAppCursorView::CursorController_OnEvent(app::tool::CursorController* pSender, const app::tool::CursorController::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  app::tool::CursorController::IEventHandler::EVENT_OnDecided:   /**<  カーソルで決定された  */
    {
      const u32 selectedIndex  = pSender->GetSelectedIndex();

      
      if(m_MappedUIButtonIDTableElems && (selectedIndex < m_MappedUIButtonIDTableElems))
      {
        GetButtonManager()->StartSelectedAct( m_pMappedUIButtonIDTable[selectedIndex] );
      }
    }
      break;


    case  app::tool::CursorController::IEventHandler::EVENT_OnMove:      /**<  カーソルが移動した    */
    default:
      break;
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppCursorView::SetMappedUIButtonIDTable(const app::tool::ButtonManager::ButtonId* pMappedUIButtonIDTable, const s32 cursorPositionMax)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pWorkBase->GetDeviceHeap();

  /*  delete old  */
  GFL_SAFE_DELETE(m_pMappedUIButtonIDTable);

  /*  create new  */
  m_MappedUIButtonIDTableElems  = cursorPositionMax;
  m_pMappedUIButtonIDTable      = GFL_NEW(pHeapBase)  app::tool::ButtonManager::ButtonId[m_MappedUIButtonIDTableElems];
  for(int num = 0; num < m_MappedUIButtonIDTableElems; ++num)
  {
    m_pMappedUIButtonIDTable[num] = pMappedUIButtonIDTable[num];
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
void  NetAppCursorView::RegisterCursorLocations(const u32* pButtonPartsPaneIDTable, const u32* pCursorPaneIDTable, const s32 cursorPositionMax)
{
  app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(m_CursorPosLayoutWorkID), &m_CursorPosLytMultiResID);

  m_pCursorController->SetLocationNum(cursorPositionMax);
  for(u32 index = 0; index < cursorPositionMax; ++index)
  {
    helper.Clear().Push(pButtonPartsPaneIDTable[index]);
    m_pCursorController->RegisterLocation(index, GetG2DUtil()->GetLayoutWork(m_CursorPosLayoutWorkID), helper.GetPane(pCursorPaneIDTable[index]));
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
void  NetAppCursorView::RegisterCursorLocations(gfl2::lyt::LytPane*  pCursorPosPanes[], const s32 cursorPositionMax)
{
  m_pCursorController->SetLocationNum(cursorPositionMax);
  for(u32 index = 0; index < cursorPositionMax; ++index)
  {
    m_pCursorController->RegisterLocation(index, GetG2DUtil()->GetLayoutWork(m_CursorPosLayoutWorkID), pCursorPosPanes[index]);
  }
}








} /*  namespace UI          */
} /*  namespace NetAppLib   */
