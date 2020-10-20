//==============================================================================
/**
 * @file   SurfUI.cpp
 * @date   2016/11/14 Mon. 19:10:38
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <thread/include/gfl2_ThreadStatic.h>

#include  "./SurfUIResource.h"
#include  "./SurfUI.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
/*  ※momijiで unnamed namespace が禁止となったためnamedにしている  */
#define FILELOCAL namespace_SurfUI
namespace FILELOCAL {
  enum  {
    STATE_Initial,
    STATE_Loading,
    STATE_Setup,
    STATE_Running,

    STATE_
  };

  
  
  static const struct
  {
    u32 SystemHeapBytes;
    u32 DeviceHeapBytes;
  }HeapInfo = {
    0x00100000,
    0x00300000,
  };
  
  static const u32 InfoListCapacity = 1024;
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  SurfUI::GetRequiredHeapBytes(void)
{
  return (FILELOCAL::HeapInfo.SystemHeapBytes + FILELOCAL::HeapInfo.DeviceHeapBytes);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI::SurfUI(
  gfl2::heap::HeapBase*           pHeapBase,
  app::util::AppRenderingManager* pRenderingManager
)
  : m_UtilHeap()
  , m_pRenderingManager(pRenderingManager)
  , m_pResourceProvider(NULL)
  , m_pDefaultResourceProvider(NULL)
  , m_pEventHandler(NULL)
  , m_pUpper_Result(NULL)
  , m_pUpper_Trick(NULL)
  , m_pUpper_Tutorial(NULL)
  , m_pLower_Tutorial(NULL)
  , m_pLower_BG(NULL)
  , m_State(FILELOCAL::STATE_Initial)
  , m_ScoreInfoList()
{
  m_UtilHeap.LocalHeapCreate( pHeapBase, pHeapBase, FILELOCAL::HeapInfo.SystemHeapBytes, FILELOCAL::HeapInfo.DeviceHeapBytes);
  m_ScoreInfoList.CreateBuffer(m_UtilHeap.GetDeviceHeap(), FILELOCAL::InfoListCapacity);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
SurfUI::~SurfUI()
{
  m_pResourceProvider->CleanupMessage();

  GFL_SAFE_DELETE(m_pUpper_Result);
  GFL_SAFE_DELETE(m_pUpper_Trick);
  GFL_SAFE_DELETE(m_pUpper_Tutorial);
  GFL_SAFE_DELETE(m_pLower_Tutorial);
  GFL_SAFE_DELETE(m_pLower_BG);

  if(m_pDefaultResourceProvider)
  {
    m_pDefaultResourceProvider->ReleaseData();
    GFL_SAFE_DELETE(m_pDefaultResourceProvider);
  }

  m_ScoreInfoList.Clear();
  m_UtilHeap.Delete();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  SurfUI::ObserveSetup(void)
{
  gfl2::heap::HeapBase* pHeapBase = m_UtilHeap.GetDeviceHeap();

  switch(m_State)
  {
    case FILELOCAL::STATE_Initial:
    {
      if(!m_pResourceProvider)
      {
        /**   */
        const app::tool::ResourceProvider::ArcTable  arcTable[]  =
        {
          {ARCID_SURF_UI,             GARC_SurfUI_SurfUI_applyt_COMP,  true},
          {print::GetMessageArcId(),  SURFUI_MESSAGE_ARCDAT,          false},
        };
        const u32  arcTableElems = GFL_NELEMS(arcTable);

        m_pDefaultResourceProvider  = GFL_NEW(pHeapBase) SurfUIResourceProvider();
        m_pDefaultResourceProvider->SetHeap(&m_UtilHeap);
        m_pResourceProvider = m_pDefaultResourceProvider;
        m_pResourceProvider->LoadRequest(arcTable, arcTableElems);
        m_State = FILELOCAL::STATE_Loading;
      }
      else
      {
        m_State = FILELOCAL::STATE_Setup;
      }
    }
      /*  fall through  */

    case  FILELOCAL::STATE_Loading:
    {
      if(m_pResourceProvider->ObserveLoad())
      {
        m_State = FILELOCAL::STATE_Setup;
      }
    }
      break;


    case FILELOCAL::STATE_Setup:
    {
      /*  message  */
      m_pResourceProvider->SetupMessage(1);
      m_pResourceProvider->GetMessageUtil()->LoadFromMemory(SURFUI_MESSAGE_ARCDAT, m_pResourceProvider->IResourceProvider_OnResourceRequest(print::GetMessageArcId(), SURFUI_MESSAGE_ARCDAT));
      
      m_pUpper_Result   = GFL_NEW(pHeapBase) SurfUI_Upper_Result(&m_UtilHeap, m_pRenderingManager);
      m_pUpper_Trick    = GFL_NEW(pHeapBase) SurfUI_Upper_Trick(&m_UtilHeap, m_pRenderingManager);
      m_pUpper_Tutorial = GFL_NEW(pHeapBase) SurfUI_Upper_Tutorial(&m_UtilHeap, m_pRenderingManager);
      m_pLower_Tutorial = GFL_NEW(pHeapBase) SurfUI_Lower_Tutorial(&m_UtilHeap, m_pRenderingManager);
      m_pLower_BG       = GFL_NEW(pHeapBase) SurfUI_Lower_BG(&m_UtilHeap, m_pRenderingManager);

      m_pUpper_Result->Setup(m_pResourceProvider);
      m_pUpper_Trick->Setup(m_pResourceProvider);
      m_pUpper_Tutorial->Setup(m_pResourceProvider);
      m_pLower_Tutorial->Setup(m_pResourceProvider);
      m_pLower_BG->Setup(m_pResourceProvider);

      m_pUpper_Result->SetListDataProvider(this);
      m_pUpper_Result->SetEventHandler(this);
      m_pUpper_Tutorial->SetVisible(false);
      m_pLower_Tutorial->SetEventHandler(this);

      m_State = FILELOCAL::STATE_Running;
    }
      break;

    case FILELOCAL::STATE_Running:
    default:
      break;
  }

  return (m_State == FILELOCAL::STATE_Running);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI::Update(void)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Running:
    {
      const bool  bTutorial = m_pUpper_Tutorial ? m_pUpper_Tutorial->IsVisible() : false;
      
      if(bTutorial)
      {
        if(m_pUpper_Tutorial) m_pUpper_Tutorial->UpdateTree();
        if(m_pLower_Tutorial) m_pLower_Tutorial->UpdateTree();
      }
      else
      {
        if(m_pUpper_Result)   m_pUpper_Result->UpdateTree();
        if(m_pUpper_Trick)    m_pUpper_Trick->UpdateTree();
      }
      if(m_pLower_BG)       m_pLower_BG->UpdateTree();
    }
      break;

    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Loading:
    case FILELOCAL::STATE_Setup:
    default:
      break;
  }


}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch(m_State)
  {
    case FILELOCAL::STATE_Running:
    {
      if(m_pUpper_Result)   m_pUpper_Result->DrawTree(displayNo);
      if(m_pUpper_Trick)    m_pUpper_Trick->DrawTree(displayNo);
      if(m_pUpper_Tutorial) m_pUpper_Tutorial->DrawTree(displayNo);
      /*  lower  */
      if(m_pLower_BG)       m_pLower_BG->DrawTree(displayNo);   /**<  1st  */
      if(m_pLower_Tutorial) m_pLower_Tutorial->DrawTree(displayNo);
    }
      break;

    case FILELOCAL::STATE_Initial:
    case FILELOCAL::STATE_Loading:
    case FILELOCAL::STATE_Setup:
    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  SurfUI::IsDrawing(void) const
{
  bool  isDrawing = false;

  isDrawing |= (m_pUpper_Result   ? m_pUpper_Result->IsDrawing()    : false);
  isDrawing |= (m_pUpper_Trick    ? m_pUpper_Trick->IsDrawing()     : false);
  isDrawing |= (m_pUpper_Tutorial ? m_pUpper_Tutorial->IsDrawing()  : false);
  isDrawing |= (m_pLower_Tutorial ? m_pLower_Tutorial->IsDrawing()  : false);
  isDrawing |= (m_pLower_BG       ? m_pLower_BG->IsDrawing()        : false);

  return isDrawing;
}


//------------------------------------------------------------------
/**
  *  @brief    jump開始通知：次回のRegisterTrickResultで上画面ジャンプスコアを表示する
  */
//------------------------------------------------------------------
void  SurfUI::OnJumpStart(void)
{
  if(m_pUpper_Trick)
  {
    m_pUpper_Trick->Notify(SurfUI_Upper_TotalScore::NOTIFY_JumpStart);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    jump終了通知：一定時間後に上画面ジャンプスコアを非表示にする
  */
//------------------------------------------------------------------
void  SurfUI::OnJumpEnd(void)
{
  if(m_pUpper_Trick)
  {
    m_pUpper_Trick->Notify(SurfUI_Upper_TotalScore::NOTIFY_JumpEnd);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    名前のみの表示、結果は登録しない
  */
//------------------------------------------------------------------
#if 0
void  SurfUI::ShowTrickName(const App::MantainSurf::MantainSurfAppData::SkillId trickID)
{
  GFL_PRINT("%s: id:%u\n", __FUNCTION__, static_cast<u32>(trickID));
  m_pUpper_Trick->ShowTrick(trickID);
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    技とスコアを表示し、結果をResult用に登録
  */
//------------------------------------------------------------------
void  SurfUI::RegisterTrickResult(const App::MantainSurf::MantainSurfAppData::SkillId trickID, const u32 score)
{
  const bool  isFull = (m_ScoreInfoList.MaxSize() <= m_ScoreInfoList.Size());

  GFL_PRINT("%s: id:%u, score:%u\n", __FUNCTION__, static_cast<u32>(trickID), score);
  
  GFL_ASSERT(!isFull);
  if(isFull)  return;

  m_pUpper_Trick->ShowTrick(trickID, score);
  m_ScoreInfoList.PushBack(sScoreInfo(trickID, score));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI::ShowResult(void)
{
  m_pUpper_Result->SetupList(m_ScoreInfoList.Size());
  m_pUpper_Result->ShowResult();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  SurfUI::StartTutorial(const MantainSurfAppData::TutorialID tutorialID)
{
  m_pUpper_Tutorial->SetVisible(true);
  m_pLower_Tutorial->Start(tutorialID);
}


//------------------------------------------------------------------
/**
  *  @brief    リストデータの取得
  *  @return   データの有無
  */
//------------------------------------------------------------------
bool  SurfUI::OnRequireListData(const u32 listIndex, App::MantainSurf::MantainSurfAppData::SkillId& rTrickID, u32& rScore)
{
  bool  isValidData = false;

  if(listIndex < m_ScoreInfoList.Size())
  {
    gfl2::util::List<sScoreInfo>::Iterator itr = m_ScoreInfoList.Begin();
    
    for(u32 index = 0; index < listIndex; ++index)
    {
      ++itr;
    }

    rTrickID  = itr->trickID;
    rScore    = itr->score;

    isValidData = true;
  }

  return isValidData;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI::SurfUI_Upper_Result_OnEvent(SurfUI_Upper_Result* pSender, const SurfUI_Upper_Result::IEventHandler::EventCode eventCode)
{
  Event(SurfUI::IEventHandler::EVENT_ResultEnd);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI::SurfUI_Lower_Tutorial_OnEvent(SurfUI_Lower_Tutorial* pSender, const SurfUI_Lower_Tutorial::IEventHandler::EventCode eventCode)
{
  m_pUpper_Tutorial->SetVisible(false);
  Event(SurfUI::IEventHandler::EVENT_TutorialEnd);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  SurfUI::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pEventHandler)
  {
    m_pEventHandler->SurfUI_OnEvent(this, eventCode);
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
#if PM_DEBUG
void  SurfUI::Debug_EnableSE(const bool bEnableSE)
{
  m_pUpper_Trick->Debug_EnableSE(bEnableSE);
  m_pUpper_Result->Debug_EnableSE(bEnableSE);
}
#endif


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
