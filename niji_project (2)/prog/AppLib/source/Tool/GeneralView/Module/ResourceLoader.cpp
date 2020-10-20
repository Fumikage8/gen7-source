//==============================================================================
/**
 * @file   ResourceLoader.cpp
 * @date   2015/09/30 Wed. 11:46:45
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_DebugPrint.h>

#include  "AppLib/include/Util/app_util_Heap.h"
#include  "AppLib/include/Util/app_util_FileAccessor.h"

#include  "./ResourceLoader.h"


namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================
namespace {

  enum  State
  {
    STATE_Initial,
    STATE_Open,
    STATE_Opening,
    STATE_Loading,
    STATE_Closing,
    STATE_Loaded,
    STATE_
  };
}


// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
ResourceLoader::ResourceLoader(void)
  : m_pHeap(NULL)
  , m_pData(NULL)
  , m_ReadBytes(0)
  , m_ArcID(gfl2::fs::ArcFile::ARCID_NULL)
  , m_ArcDatID(gfl2::fs::ArcFile::ARCDATID_NULL)
  , m_State(STATE_Initial)
  , m_IsCompressedData(false)
  , m_IsSkipOpenClose(false)
{}


//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
ResourceLoader::ResourceLoader(app::util::Heap* pHeap)
  : m_pHeap(pHeap)
  , m_pData(NULL)
  , m_ArcID(gfl2::fs::ArcFile::ARCID_NULL)
  , m_ArcDatID(gfl2::fs::ArcFile::ARCDATID_NULL)
  , m_State(STATE_Initial)
  , m_IsCompressedData(false)
  , m_IsSkipOpenClose(false)
{}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ResourceLoader::~ResourceLoader()
{
  GflHeapFreeMemory(m_pData);
}
  
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  ResourceLoader::RequestLoad(app::util::Heap* pHeap, const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID, const bool isCompressedData, const bool isSkipOpenClose)
{
  m_pHeap = pHeap;
  return RequestLoad(arcID, arcDatID, isCompressedData, isSkipOpenClose);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  ResourceLoader::RequestLoad(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID, const bool isCompressedData, const bool isSkipOpenClose)
{
  if(m_State != STATE_Initial) return false;

  m_IsSkipOpenClose   = isSkipOpenClose;
  m_ArcID             = arcID;
  m_ArcDatID          = arcDatID;
  m_State             = STATE_Open;
  m_IsCompressedData  = isCompressedData;

  return true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  ResourceLoader::ObserveLoad(void)
{
  gfl2::heap::HeapBase*   pSysLowerHeapBase = m_pHeap ? m_pHeap->GetSystemHeap()->GetLowerHandle() : NULL;
  gfl2::heap::HeapBase*   pHeapBase         = m_pHeap ? m_pHeap->GetDeviceHeap() : NULL;

  switch(m_State)
  {
    case  STATE_Initial:
      break;

    case  STATE_Open:
      if(!m_IsSkipOpenClose)
      {
        app::util::FileAccessor::FileOpen(m_ArcID, pSysLowerHeapBase);
      }
      m_State = STATE_Opening;
      /*  fallthrough  */

    case  STATE_Opening:
      if(app::util::FileAccessor::IsFileOpen(m_ArcID))
      {
        app::util::FileAccessor::FileLoad(
          m_ArcID,
          m_ArcDatID,
          &m_pData,
          pHeapBase,
          m_IsCompressedData,
          128,
          gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
          NULL,
          &m_ReadBytes
        );
        
        m_State  = STATE_Loading;
      }
      break;

    case  STATE_Loading:
      if(app::util::FileAccessor::IsFileLoad(&m_pData))
      {
        if(!m_IsSkipOpenClose)
        {
          app::util::FileAccessor::FileClose(m_ArcID, pSysLowerHeapBase );
        }
        m_State  = STATE_Closing;
      }
      break;

    case  STATE_Closing:
      if(m_IsSkipOpenClose || app::util::FileAccessor::IsFileClose( m_ArcID ))
      {
        m_State  = STATE_Loaded;
      }
      break;

    case  STATE_Loaded:
      break;
  }

  return IsLoadComplete();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool    ResourceLoader::IsLoadComplete(void) const
{
  return (m_State == STATE_Loaded);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool    ResourceLoader::ReleaseData(void)
{
  bool  isSuccess = false;

  if(IsLoadComplete())
  {
    GflHeapFreeMemory(m_pData);
    m_pData     = NULL;
    m_ReadBytes = 0;

    m_ArcID             = gfl2::fs::ArcFile::ARCID_NULL;
    m_ArcDatID          = gfl2::fs::ArcFile::ARCDATID_NULL;
    m_State             = STATE_Initial;
    m_IsCompressedData  = false;
    
    isSuccess = true;
  }
  else if(m_State == STATE_Initial)
  {
    isSuccess = true;
  }
  else
  {
    GFL_WARNING("%s(%d):%s:failed (state == %d)\n", __FILE__, __LINE__, __FUNCTION__, static_cast<u32>(m_State));
  }

  return isSuccess;
}


} /*  namespace tool  */
} /*  namespace app   */
