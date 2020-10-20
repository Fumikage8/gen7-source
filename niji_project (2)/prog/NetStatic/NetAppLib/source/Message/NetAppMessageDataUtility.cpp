//==============================================================================
/**
  *  @file   NetAppMessageDataUtility.cpp
  *  @brief  
  *  @date   2015.06.03 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"


namespace NetAppLib {
namespace Message {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageUtility::MessageUtility(gfl2::heap::HeapBase* pHeapBase, u32 dataNum)
  : m_pHeapBase(pHeapBase)
  , m_DataNum(dataNum)
  , m_MsgDataMap(pHeapBase, dataNum)
  , m_StrBuf(STR_MAX, pHeapBase)
  , m_WordSet(pHeapBase)
{
}


MessageUtility::MessageUtility(gfl2::heap::HeapBase* pHeapBase, u32 dataNum, u32 wordMax, u32 bufLen)
  : m_pHeapBase(pHeapBase)
  , m_DataNum(dataNum)
  , m_MsgDataMap(pHeapBase, dataNum)
  , m_StrBuf(STR_MAX, pHeapBase)
  , m_WordSet(wordMax,bufLen,pHeapBase)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageUtility::~MessageUtility()
{
  /*  release message resources  */
  {
    gfl2::util::Map<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>::Iterator       itr     = m_MsgDataMap.Begin();
    const gfl2::util::Map<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>::Iterator itrEnd  = m_MsgDataMap.End();


    while(itr != itrEnd)
    {
      GFL_DELETE(itr->second);
      ++itr;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageUtility::Load(const gfl2::fs::ArcFile::ARCDATID garcID)
{
  gfl2::str::MsgData*   pMsgData  = GFL_NEW(m_pHeapBase) gfl2::str::MsgData( print::GetMessageArcId(), garcID, m_pHeapBase, gfl2::str::MsgData::LOAD_FULL );

  const gfl2::util::Pair<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>  resourcePair(garcID, pMsgData);

  m_MsgDataMap.Insert(resourcePair);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageUtility::LoadFromMemory(const gfl2::fs::ArcFile::ARCDATID garcID, const void* pMessageData)
{
  gfl2::str::MsgData*   pMsgData  = GFL_NEW(m_pHeapBase) gfl2::str::MsgData( pMessageData, m_pHeapBase );

  const gfl2::util::Pair<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>  resourcePair(garcID, pMsgData);

  m_MsgDataMap.Insert(resourcePair);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::str::MsgData*   MessageUtility::GetData(const gfl2::fs::ArcFile::ARCDATID garcID)
{
  gfl2::util::Map<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>::Iterator       itrFind = m_MsgDataMap.Find(garcID);
  const gfl2::util::Map<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>::Iterator itrEnd  = m_MsgDataMap.End();
  
  return ((itrFind != itrEnd) ? itrFind->second : NULL);    /*  iteratorのoperatorに==がない  */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::str::StrBuf&  MessageUtility::GetString(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID)
{
  gfl2::str::MsgData*   pMessageData  = GetData(garcID);

  m_StrBuf.Clear();
  if(pMessageData)
  {
    pMessageData->GetString(messageID, m_StrBuf);
  }

  return m_StrBuf;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::str::StrBuf&    MessageUtility::GetExpandedString(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID)
{
  gfl2::str::StrBuf     srcStrBuf(GetString(garcID, messageID), m_pHeapBase);

  GetWordSet()->Expand(&m_StrBuf, &srcStrBuf);

  return m_StrBuf;
}



} /*  namespace Message   */
} /*  namespace NetAppLib */
