//==============================================================================
/**
 * @file   app_util_AppDemoUtilityHelper.cpp
 * @date   2015/08/27 Thu. 12:15:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <ro/include/gfl2_RoManager.h>
#include  <fs/include/gfl2_Fs.h>

#include  "GameSys/include/GameManager.h"

#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"


namespace
{
  enum  ModuleID
  {
    MODULE_AppDemo,
    
    MODULE_MAX
  };
  static const char* const moduleTable[] =
  {
    "AppDemo.cro",      /*  MODULE_AppDemo, */
  };
  static const u32         moduleTableItems  = GFL_NELEMS(moduleTable);
}



namespace app   {
namespace util  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
AppDemoUtilityHelper::AppDemoUtilityHelper(void)
  : m_pHeapBase(NULL)
  , m_ppROModules(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
AppDemoUtilityHelper::~AppDemoUtilityHelper()
{
  GFL_ASSERT(!m_ppROModules);   /*  開放忘れ検知  */
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtilityHelper::Begin(gfl2::heap::HeapBase* pHeapBase)
{
  GFL_ASSERT(pHeapBase);
  GFL_ASSERT(!m_ppROModules);

  m_pHeapBase = pHeapBase;

  /*  modules */
  {
    gfl2::ro::RoManager*          pROManager    = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 
    gfl2::fs::AsyncFileManager*   pFileManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

    m_ppROModules = GFL_NEW(m_pHeapBase) gfl2::ro::Module*[moduleTableItems];

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      m_ppROModules[index] = pROManager->LoadModule(pFileManager, moduleTable[index]);
      pROManager->StartModule(m_ppROModules[index]);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  AppDemoUtilityHelper::End(void)
{
  GFL_ASSERT(m_ppROModules);

  /*  modules */
  {
    gfl2::ro::RoManager*  pROManager  = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager); 

    for(u32 index = 0; index < moduleTableItems; ++index)
    {
      pROManager->DisposeModule(m_ppROModules[index]);
    }
    GFL_SAFE_DELETE_ARRAY(m_ppROModules);
  }

  m_pHeapBase = NULL;
}








} /*  namespace util  */
} /*  namespace app   */
