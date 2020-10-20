//==============================================================================
/**
 * @file	BattleFesMsgResourceManager.h
 * @brief	バトルフェスメッセージリソースマネージャー
 * @author	munakata_kai
 * @date	2016/12/01
 */
// =============================================================================

#ifndef __BATTLEFESMSGRESOURCEMANAGER_H__
#define __BATTLEFESMSGRESOURCEMANAGER_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>

#include  "AppLib/include/Util/app_util_heap.h"
#include  "AppLib/source/Tool/GeneralView/Module/ResourceLoader.h"

/*  message  */
#include  "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"

GFL_NAMESPACE_BEGIN( BattleFes )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class BattleFesMsgResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleFesMsgResourceManager);

public:
  // コンストラクタ
  BattleFesMsgResourceManager(app::util::Heap* pHeap);
  // デストラクタ
  virtual ~BattleFesMsgResourceManager();

  bool  RequestLoad();
  bool  ObserveLoad();
  void* GetData();
  bool  ReleaseData();

  /*  message  */
  print::WordSet*           GetWordSet()  { return m_MessageUtil.GetWordSet(); }
  gfl2::str::StrBuf&        GetString(const u32 messageID, const bool isExpand = false);
  gfl2::str::MsgData*       GetMsgData();

private:
  enum
  {
    STATE_Initial,
    STATE_Loading,
    STATE_Closing,
    STATE_Loaded,
  };

  app::util::Heap*                    m_pHeap;
  app::tool::ResourceLoader           m_ResourceLoader;
  NetAppLib::Message::MessageUtility  m_MessageUtil;
  u32                                 m_State;
};

GFL_NAMESPACE_END( BattleFes )

#endif // __BATTLEFESMSGRESOURCEMANAGER_H__
