
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Model.cpp
 * @date   2016/09/14 Wed. 18:15:31
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  "./HIOViewModule_Model.h"


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOViewModule_Model::OnInitialize(gfl2::heap::HeapBase* pHeapBase)
{
  m_pModelPool  = GFL_NEW(pHeapBase) EasyModelPool(pHeapBase, m_ModelCapacity);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  HIOViewModule_Model::OnFinalize(void)
{
  GFL_SAFE_DELETE(m_pModelPool);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOViewModule_Model::OnUpdate(void)
{
  if(m_pModelPool)
  {
    m_pModelPool->Update();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  HIOViewModule_Model::OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID)
{
  if(!m_pModelPool) return;
  if(!m_pUtilHeap)  return;

  switch(eventID)
  {
    case NetAppLib::Debug::HIOUtil::IEventHandler::EVENT_ReadEnd:
    {
      DataHandling(pSender);
    }
      break;
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
void  HIOViewModule_Model::DisposeAll(void)
{
  if(m_pModelPool)  m_pModelPool->DisposeAll();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOViewModule_Model::DataHandling(NetAppLib::Debug::HIOUtil* pHIOUtil)
{
  const NetAppLib::Debug::HIOUtil::DataInfo&  rDataInfo = pHIOUtil->GetDataInfo();

  switch(rDataInfo.userdata[0])
  {
    /*
      addModel
      [0]:0
      [1]:modelHash
    */
    case  0:
    {
      void*       pData   = pHIOUtil->GetData();
      EasyModel*  pModel  = m_pModelPool->Get();
      const u32   hash    = rDataInfo.userdata[1];

      if(pModel)
      {
        GFL_PRINT("create model\n");
        pModel->Create(pData, m_pUtilHeap, true);
        pModel->SetHash(hash);
        
        if(m_pHandler)
        {
          m_pHandler->OnEvent_AddModel(this, *pModel);
        }
        else
        {
          pModel->Release();
          m_pModelPool->Dispose(pModel);
          GflHeapFreeMemory(pData);
        }
      }
      else
      {
        GflHeapFreeMemory(pData);
      }
    }
      break;
      
    /*
      deleteModel
      [0]:1
      [1]:modelHash
    */
    case  1:
    {
      {
        void* pData = pHIOUtil->GetData();
        GflHeapFreeMemory(pData);
      }

      if(m_pHandler)
      {
        EasyModel*  pFoundModel = NULL;
        const u32   hash        = rDataInfo.userdata[1];
        
        pFoundModel = m_pModelPool->FindBusyModel(hash);
        
        if(pFoundModel) m_pHandler->OnEvent_RemoveModel(this, *pFoundModel);
      }
    }
      break;
  }

}


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
