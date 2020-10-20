
//==============================================================================
/**
 * @file   GiftViewContainer.cpp
 * @date   2015/07/31 Fri. 16:23:58
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/GiftWork.h"

#include  "./GiftViewContainer.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
GiftViewContainer::GiftViewContainer(GiftWork* pWork)
  : m_pUpperView_BG(NULL)
  , m_pUpperView_Card(NULL)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_BG(NULL)
  , m_pLowerView_Album(NULL)
  , m_pLowerView_Back(NULL)
  , m_pLowerView_Message(NULL)
  , m_pLowerView_Selection(NULL)
  , m_pLowerView_SelectData(NULL)
  , m_pLowerView_SelectInstruction(NULL)
{
  gfl2::heap::HeapBase* pHeapBase = pWork->GetAppHeap()->GetDeviceHeap();

  m_pMessageMenu                  = GFL_NEW(pHeapBase)  GiftMessageMenu(pWork);

  m_pUpperView_BG                 = GFL_NEW(pHeapBase)  UpperView_BG(pWork);
  m_pUpperView_Card               = GFL_NEW(pHeapBase)  UpperView_Card(pWork);
  m_pUpperView_Message            = GFL_NEW(pHeapBase)  UpperView_Message(pWork);
  m_pLowerView_BG                 = GFL_NEW(pHeapBase)  LowerView_BG(pWork);
  m_pLowerView_Album              = GFL_NEW(pHeapBase)  LowerView_Album(pWork);
  m_pLowerView_Back               = GFL_NEW(pHeapBase)  LowerView_Back(pWork);
  m_pLowerView_InfoWindow         = GFL_NEW(pHeapBase)  LowerView_InfoWindow(pWork);
  m_pLowerView_Message            = GFL_NEW(pHeapBase)  LowerView_Message(pWork);
  m_pLowerView_Selection          = GFL_NEW(pHeapBase)  LowerView_Selection(pWork);
  m_pLowerView_SelectData         = GFL_NEW(pHeapBase)  LowerView_SelectData(pWork);
  m_pLowerView_SelectInstruction  = GFL_NEW(pHeapBase)  LowerView_SelectInstruction(pWork);

  SetScene(SCENE_Void);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GiftViewContainer::~GiftViewContainer()
{
  GFL_SAFE_DELETE(m_pMessageMenu);

  GFL_SAFE_DELETE(m_pUpperView_BG);
  GFL_SAFE_DELETE(m_pUpperView_Card);
  GFL_SAFE_DELETE(m_pUpperView_Message);
  GFL_SAFE_DELETE(m_pLowerView_BG);
  GFL_SAFE_DELETE(m_pLowerView_Album);
  GFL_SAFE_DELETE(m_pLowerView_Back);
  GFL_SAFE_DELETE(m_pLowerView_InfoWindow);
  GFL_SAFE_DELETE(m_pLowerView_Message);
  GFL_SAFE_DELETE(m_pLowerView_Selection);
  GFL_SAFE_DELETE(m_pLowerView_SelectData);
  GFL_SAFE_DELETE(m_pLowerView_SelectInstruction);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftViewContainer::SetAllInputEnable(const bool isEnable)
{

  m_pLowerView_BG                 ->SetInputEnabled(isEnable);
  m_pLowerView_Album              ->SetInputEnabled(isEnable);
  m_pLowerView_Back               ->SetInputEnabled(isEnable);
  m_pLowerView_InfoWindow         ->SetInputEnabled(isEnable);
  m_pLowerView_Message            ->SetInputEnabled(isEnable);
  m_pLowerView_Selection          ->SetInputEnabled(isEnable);
  m_pLowerView_SelectData         ->SetInputEnabled(isEnable);
  m_pLowerView_SelectInstruction  ->SetInputEnabled(isEnable);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  GiftViewContainer::TryFinalize(void)
{
  bool  isComplete  = true;

  if(m_pUpperView_Card)
  {
    isComplete  = false;
    if(m_pUpperView_Card->FinallizePokeModel())
    {
      isComplete  = true;
    }
  }


  return isComplete;
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  GiftViewContainer::IsDrawingAny(void) const
{
  if(!m_pLowerView_InfoWindow->ExitQuery()) return false;

  if(
    m_pMessageMenu->IsDrawing()
    || m_pUpperView_BG->IsDrawing()
    || m_pUpperView_Card->IsDrawing()
    || m_pUpperView_Message->IsDrawing()
    || m_pLowerView_BG->IsDrawing()
    || m_pLowerView_Album->IsDrawing()
    || m_pLowerView_Back->IsDrawing()
    || m_pLowerView_InfoWindow->IsDrawing()
    || m_pLowerView_Message->IsDrawing()
    || m_pLowerView_Selection->IsDrawing()
    || m_pLowerView_SelectData->IsDrawing()
    || m_pLowerView_SelectInstruction->IsDrawing()
  )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftViewContainer::SetScene(const SceneID sceneID)
{
  u32 viewIndex = 0;

  ClearViewArray();


#define  ADDVIEW(__pView)    do{                                                  \
                                GFL_ASSERT(viewIndex < GFL_NELEMS(m_pViewArray)); \
                                m_pViewArray[viewIndex++] = (__pView);            \
                             }while(0)

  ADDVIEW(m_pUpperView_BG);
  ADDVIEW(m_pLowerView_BG);
  switch(sceneID)
  {
    /*
      mainmenu
    */
    case  SCENE_MainMenu:
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_Selection);
      break;

    /*
      instruction
    */
    case  SCENE_Instruction_Menu:           /**<  項目選択  */
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_SelectInstruction);
      break;

    case  SCENE_Instruction_Instruction:    /**<  説明表示  */
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_InfoWindow);
      break;

    /*
      album
    */
    case  SCENE_Album:
      ADDVIEW(m_pUpperView_Card);
      ADDVIEW(m_pLowerView_Album);
      break;

    /*
      receive
    */
    case  SCENE_Receive_Information:
      ADDVIEW(m_pMessageMenu);
      break;

    case  SCENE_Receive_Selection:
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_Selection);
      break;

    case  SCENE_Receive_SearchingViaUDS:
      ADDVIEW(m_pLowerView_Back);
      break;

    /*
      receiveIr
    */
    case  SCENE_Receive_Ir:
      ADDVIEW(m_pLowerView_Back);
      break;

    /*
      receiveInternet
    */
    case  SCENE_Receive_Internet:
      ADDVIEW(m_pLowerView_Back);
      break;

    /*
      receiveSerial
    */
    case  SCENE_Receive_Serial:
      ADDVIEW(m_pLowerView_Back);
      break;

    case  SCENE_Receive_Serial_Auth:
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_Back);
      break;

    /*
      selectData
    */
    case  SCENE_SelectData:
      ADDVIEW(m_pUpperView_Message);
      ADDVIEW(m_pLowerView_SelectData);
      break;

    /*
      receiveResult
    */
    case  SCENE_ReceiveResult:
      ADDVIEW(m_pUpperView_Card);
      ADDVIEW(m_pLowerView_Message);
      break;


    case  SCENE_BGOnly:
      /*  nop  */
      break;


    case  SCENE_Void: /*  through  */
    default:
      ClearViewArray();
      break;
  }

#undef   ADDVIEW
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftViewContainer::Update(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->UpdateTree();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  GiftViewContainer::Draw(const gfl2::gfx::CtrDisplayNo dispNo)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(!m_pViewArray[index])  continue;
    m_pViewArray[index]->DrawTree(dispNo);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  GiftViewContainer::DebugDraw(const gfl2::gfx::CtrDisplayNo dispNo)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    if(m_pViewArray[index] == m_pUpperView_Card)  m_pUpperView_Card->DebugDraw(dispNo);
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  GiftViewContainer::ClearViewArray(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pViewArray); ++index)
  {
    m_pViewArray[index] = NULL;
  }

  m_pLowerView_Selection->HideDeliveryEventInformation();
}




} /*  namespace Gift    */
} /*  namespace NetApp  */
