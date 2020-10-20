
//==============================================================================
/**
 * @file   AppDemoUtility.cpp
 * @date   2015/08/27 Thu. 12:09:12
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameManager.h"

#include  "App/AppDemo/include/AppDemoutility.h"

#include  <System/include/Timer/gfl2_PerformanceCounter.h>

// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  ModuleID
  {
//    MODULE_SeqEditor,
    MODULE_SeqPlayer,
    MODULE_Object,
    
    MODULE_MAX
  };
  static const char* const moduleTable[] =
  {
    //"SeqEditor.cro",      /*  MODULE_SeqEditor, *///Object,Background,SeqEditorはmomijiで統合されました。
    "SequencePlayer.cro", /*  MODULE_SeqPlayer, */
    "Object.cro",         /*  MODULE_Object,    */
  };
  static const u32         moduleTableItems  = GFL_NELEMS(moduleTable);

  enum
  {
    /*  init  */
    SUBSEQ_Init,

    /*  update  */
    SUBSEQ_Idle,
    SUBSEQ_Running,

    /*  end  */
    SUBSEQ_Cleanup,
    SUBSEQ_End,
    
    SUBSEQ_
  };

  static const struct
  {
    u32   systemHeapSize;
    u32   deviceHeapSize;
    u32   marginSize;
    
    u32   totalSize(void) const {return (systemHeapSize + deviceHeapSize + marginSize);}
  }HeapSizeModels[App::AppDemo::AppDemoUtility::HEAPMODEL_MAX] = {
    {0x00300000,  0x01B00000, 0x00100000}, /*  HEAPMODEL_Full       */
    {0x00240000,  0x01440000, 0x00100000}, /*  HEAPMODEL_75_Percent */
    {0x00180000,  0x00D80000, 0x00100000}, /*  HEAPMODEL_Half       */
    {0x00300000,  0x01F00000, 0x00100000}, /*  HEAPMODEL_Dendou       */
  };


}



namespace App       {
namespace AppDemo   {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
AppDemoUtility::AppDemoUtility(gfl2::heap::HeapBase* pParentHeapBase, const HeapModel heapModel)
  : m_HeapModel(heapModel)
  , m_pParentHeapBase(pParentHeapBase)
  , m_pLocalHeapBase(NULL)
  , m_ppROModules(NULL)
  , m_pSequenceViewSystem(NULL)
  , m_pHandler(NULL)
  , m_Subseq(SUBSEQ_Init)
  , m_IsExitRequest(false)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
AppDemoUtility::~AppDemoUtility()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::SendCommand(const Command command)
{
  if(!m_pSequenceViewSystem) return;

  switch(command)
  {
    case  COMMAND_SystemSync:           /**<  待機ループの解除：        キー入力待ちなど    */
//      m_pSequenceViewSystem->SetSystemSync(true);
      m_pSequenceViewSystem->SystemSync();
      break;

    case  COMMAND_SetSystemCancel:      /**<  デモ分岐フラグのセット：  進化キャンセルなど  */
      m_pSequenceViewSystem->SetSystemCancel(true);
      break;

    case  COMMAND_ResetSystemCancel:    /**<  デモ分岐フラグの解除：    進化キャンセルなど  */
      m_pSequenceViewSystem->SetSystemCancel(false);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::Update(void)
{
  switch(m_Subseq)
  {
    case  SUBSEQ_Init:
      OnInitFirst();
      m_Subseq  = SUBSEQ_Idle;
      break;

    /*  update  */
    case  SUBSEQ_Idle:
      if(m_IsExitRequest)
      {
        m_pSequenceViewSystem->ProcessToFinalize();
        m_Subseq  = SUBSEQ_Cleanup;
      }
      else if(m_pHandler)
      {
        const u32 garcID  = m_pHandler->AppDemoUtility_StartQuery(m_pSequenceViewSystem);

        if(IsValidGARCID(garcID))
        {
          m_pSequenceViewSystem->SeqComFunc_PlaySequenceFile(garcID);
          m_Subseq  = SUBSEQ_Running;
        }
      }
      break;

    case  SUBSEQ_Running:
      m_pSequenceViewSystem->Update();
      /*  deltatime */
#if 0
      {
          u64 tickStart;
          m_pSequenceViewSystem->GetSeqStartTick(tickStart);
          u64 tickEnd = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
          u64 tickDif = tickEnd - tickStart;
          double frame = tickDif*0.000030;               
          m_pSequenceViewSystem->SetTargetFrame(static_cast<int>(frame));
      }
#endif

      if(m_pSequenceViewSystem->SeqComFunc_IsFinishSequenceFile())
      {
        if(m_pHandler)
        {
          m_pHandler->AppDemoUtility_OnSequenceEnd(m_pSequenceViewSystem);
        }
        m_Subseq  = SUBSEQ_Idle;
      }
      break;

    /*  end  */
    case  SUBSEQ_Cleanup:
      if(m_pSequenceViewSystem->Finalize())
      {
        OnEndLast();
        m_Subseq  = SUBSEQ_End;
      }
      break;

    case  SUBSEQ_End:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  AppDemoUtility::IsStable(void) const
{
  return (m_Subseq == SUBSEQ_End);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::PreDraw(void)
{
  if(m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->PreDraw();
 }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::Draw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_pSequenceViewSystem)
  {
    m_pSequenceViewSystem->Draw(displayNo);
 }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::OnInitFirst(void)
{
  /*  heap  */
  {
    m_pLocalHeapBase  = GFL_CREATE_LOCAL_HEAP(m_pParentHeapBase, HeapSizeModels[m_HeapModel].totalSize(), gfl2::heap::HEAP_TYPE_EXP, false );
  }

  /*  modules */
  {
    gfl2::ro::RoManager*          pROManager    = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 
    gfl2::fs::AsyncFileManager*   pFileManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

    m_ppROModules = GFL_NEW(m_pLocalHeapBase) gfl2::ro::Module*[moduleTableItems];

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      m_ppROModules[index] = pROManager->LoadModule(pFileManager, moduleTable[index]);
      pROManager->StartModule(m_ppROModules[index]);
    }
  }

  /*  seqView  */
  {
    m_pSequenceViewSystem = GFL_NEW(m_pLocalHeapBase) DemoLib::Sequence::SequenceViewSystem();
    m_pSequenceViewSystem->Initialize(m_pLocalHeapBase, m_pLocalHeapBase, HeapSizeModels[m_HeapModel].systemHeapSize, HeapSizeModels[m_HeapModel].deviceHeapSize);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtility::OnEndLast(void)
{

  /*  seqView  */
  {
    GFL_SAFE_DELETE(m_pSequenceViewSystem);
  }

  /*  modules */
  {
    gfl2::ro::RoManager*  pROManager  = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      pROManager->DisposeModule(m_ppROModules[index]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppROModules);
  }

  /*  heap  */
  {
    gfl2::heap::Manager::DeleteHeap( m_pLocalHeapBase );
    m_pLocalHeapBase  = NULL;
  }

}



} /*  namespace AppDemo   */
} /*  namespace App       */
