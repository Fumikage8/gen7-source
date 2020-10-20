
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   NetAppHIOUtil.cpp
 * @date   2016/10/04 Tue. 15:35:11
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
#include  "../../include/debug/NetAppHIOUtil.h"


namespace
{
  enum
  {
    STATE_Initial,
    STATE_WaitHost,
    STATE_WaitData,
    STATE_ReadingData,
    STATE_ReadEnd,
  };
}


namespace NetAppLib  {
namespace Debug {
// =============================================================================
/**/
//==============================================================================

HIOUtil::HIOUtil(void)
  : m_pHeapBase(NULL)
  , m_State(m_State)
  , m_pWorkMemory(NULL)
  , m_HIOSCh()
  , m_Ch(-1)
  , m_pDataBuffer(NULL)
  , m_DataBufferBytes(0)
  , m_DataBytes(0)
  , m_ReadableBytes(0)
{
}

HIOUtil::~HIOUtil()
{
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::Initialize(gfl2::heap::HeapBase* pHeapBase, const s32 ch)
{
  m_pHeapBase   = pHeapBase;
  m_pWorkMemory = GflHeapAllocMemoryAlign(m_pHeapBase, nn::hio::WORKMEMORY_SIZE, 128);
  m_Ch          = ch;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::Finalize(void)
{
  Close();
  ReleaseBuffer();
  GflHeapSafeFreeMemory(m_pWorkMemory);
  m_pHeapBase   = NULL;
  
  m_State = STATE_Initial;
}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
int  HIOUtil::Update(void)
{
  switch(m_State)
  {
    case  STATE_Initial:
    {
      Open();
      m_State = STATE_WaitHost;
    }
      break;

    case  STATE_WaitHost:
    {
      if(m_HIOSCh.WaitHost(nn::hio::SerialChannel::ATTRIBUTE_NO_WAIT) == nn::hio::ResultConnected())
      {
        GFL_PRINT("host found\n");
        m_State = STATE_WaitData;
      }
    }
      break;

    case  STATE_WaitData:
    {
      DataInfo  dataInfo;
      const u32 infoBytes = sizeof(dataInfo);
      
      
      if(m_HIOSCh.Read(&dataInfo, infoBytes, nn::hio::SerialChannel::ATTRIBUTE_NO_WAIT) == infoBytes)
      {
        if(dataInfo.word == DataInfo::constWord)  /*  "head"  */
        {
          m_DataInfo      = dataInfo;   /*  copy  */
          m_ReadableBytes = m_DataInfo.bytes;

          GFL_PRINT("data found: %u\n", m_ReadableBytes);

          CreateBuffer(m_ReadableBytes);
          m_State = STATE_ReadingData;
        }
      }
    }
      break;

    case  STATE_ReadingData:
    {
      m_DataBytes = m_HIOSCh.Read(m_pDataBuffer, m_ReadableBytes);
      
      GFL_PRINT("read: %u / %u\n", m_DataBytes, m_ReadableBytes);
//      GFL_PRINT("data: %s\n", m_pDataBuffer);
      
      m_State = STATE_ReadEnd;
    }
      break;

    case  STATE_ReadEnd:
      Event(IEventHandler::EVENT_ReadEnd);
      break;


    default:
      break;
  }

  return m_State;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::Open(void)
{
  GFL_PRINT("open HIO sch:%d\n", m_Ch);
  m_HIOSCh.Open(m_Ch, m_pWorkMemory);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::Close(void)
{
  if(STATE_WaitData <= m_State) m_HIOSCh.Disconnect();
  if(STATE_WaitHost <= m_State) m_HIOSCh.Close();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::Event(const HIOUtil::IEventHandler::EventID eventID)
{
  if(m_pHandler)
  {
    m_pHandler->OnEvent(this, eventID);
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
void  HIOUtil::CreateBuffer(const u32 bytes)
{
  m_DataBufferBytes = bytes;
  m_pDataBuffer     = static_cast<u8*>(GflHeapAllocMemoryAlign(m_pHeapBase, m_DataBufferBytes, 128));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOUtil::ReleaseBuffer(void)
{
  GflHeapFreeMemory(m_pDataBuffer);
  m_pDataBuffer     = NULL;
  m_DataBufferBytes = 0;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void* HIOUtil::GetData(u32* pDataBytes)
{
  void*   pData = m_pDataBuffer;

  if(m_State != STATE_ReadEnd)  return NULL;
  if(pDataBytes)                *pDataBytes = m_DataBytes;
  
  m_State           = STATE_WaitData;
  m_pDataBuffer     = NULL;
  m_DataBytes       = 0;
  m_DataBufferBytes = 0;
  return pData;
}


} /*  namespace Debug */
} /*  namespace NetApp  */
#endif  /*  #if NN_SWITCH_ENABLE_HOST_IO  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
