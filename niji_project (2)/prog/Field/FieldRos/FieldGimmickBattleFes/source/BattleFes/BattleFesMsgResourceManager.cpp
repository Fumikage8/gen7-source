//==============================================================================
/**
* @file	BattleFesMsgResourceManager.cpp
* @brief	バトルフェスメッセージリソースマネージャー
* @author	munakata_kai
* @date	2016/12/01
*/
// =============================================================================
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesMsgResourceManager.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
/*  messages  */
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_jf_phrase.h>

GFL_NAMESPACE_BEGIN(BattleFes)

BattleFesMsgResourceManager::BattleFesMsgResourceManager(app::util::Heap* pHeap)
  : m_pHeap(pHeap)
  , m_MessageUtil(pHeap->GetDeviceHeap(), 1)
  , m_State(STATE_Initial)
{
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
BattleFesMsgResourceManager::~BattleFesMsgResourceManager()
{
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
bool BattleFesMsgResourceManager::RequestLoad()
{

  if (!ReleaseData())  {
    return false;
  }

  const gfl2::fs::ArcFile::ARCID  messageArcID = print::GetMessageArcId();


  /*  arcのopen  */
  {
    gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();
    /*  message  */
    app::util::FileAccessor::FileOpen(messageArcID, pHeapBase, true);
  }


  /*  読み込みリソースを追加する際はここへ  */
  m_ResourceLoader.RequestLoad(m_pHeap, messageArcID, GARC_message_jf_phrase_DAT, false, true);
  m_State = STATE_Loading;

  return true;
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
bool  BattleFesMsgResourceManager::ObserveLoad()
{
  switch (m_State)
  {
  case  STATE_Initial:
    break;

  case  STATE_Loading:
  {
    bool  isLoaded = true;
    if (!m_ResourceLoader.ObserveLoad())
    {
      isLoaded = false;
    }

    if (isLoaded)
    {
      /*  arcのclose  */
      gfl2::heap::HeapBase*   pHeapBase = m_pHeap->GetDeviceHeap();
      /*  message  */
      app::util::FileAccessor::FileClose(print::GetMessageArcId(), pHeapBase);
      m_State = STATE_Closing;
    }
  }
  break;

  case  STATE_Closing:
  {
    bool  bAllClosed = true;

    /*  message  */
    bAllClosed = app::util::FileAccessor::IsFileClose(print::GetMessageArcId());

    if (bAllClosed)
    {
      m_State = STATE_Loaded;
    }
  }
  break;

  case  STATE_Loaded:
    break;

  default:
    break;
  }

  return (m_State == STATE_Loaded);
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
void* BattleFesMsgResourceManager::GetData()
{
  return (m_ResourceLoader.IsLoadComplete() ? m_ResourceLoader.GetData() : NULL);
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*/
//------------------------------------------------------------------
bool BattleFesMsgResourceManager::ReleaseData()
{
  if (!m_ResourceLoader.ReleaseData())
  {
    return false;
  }

  return true;
}


//------------------------------------------------------------------
/**
*  @func
*  @brief
*  @param
*  @return
*/
//------------------------------------------------------------------
gfl2::str::StrBuf& BattleFesMsgResourceManager::GetString(const u32 messageID, const bool isExpand)
{
  if (!m_MessageUtil.GetData(GARC_message_jf_phrase_DAT))
  {
    m_MessageUtil.LoadFromMemory(GARC_message_jf_phrase_DAT, m_ResourceLoader.GetData());
  }

  if (isExpand)
  {
    return m_MessageUtil.GetExpandedString(GARC_message_jf_phrase_DAT, messageID);
  }
  else
  {
    return m_MessageUtil.GetString(GARC_message_jf_phrase_DAT, messageID);
  }
}

//------------------------------------------------------------------
/**
*  @func     GetMsgData
*  @brief    メッセージデータ取得
*  @param    resourceID     リソースID
*  @return   MsgData
*/
//------------------------------------------------------------------
gfl2::str::MsgData* BattleFesMsgResourceManager::GetMsgData()
{
  if (!m_MessageUtil.GetData(GARC_message_jf_phrase_DAT))
  {
    m_MessageUtil.LoadFromMemory(GARC_message_jf_phrase_DAT, m_ResourceLoader.GetData());
  }

  return m_MessageUtil.GetData(GARC_message_jf_phrase_DAT);
}

GFL_NAMESPACE_END( BattleFes )
