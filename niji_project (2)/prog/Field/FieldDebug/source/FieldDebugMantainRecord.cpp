//======================================================================
/**
* @file FieldDebugMantainRecord.cpp
* @date 2015/10/26 16:49:59
* @author saito_nozomu
* @brief マンタインサーフレコード デバッグ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldDebug/include/FieldDebugMantainRecord.h"

// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

#include <Sound/include/Sound.h>

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Local Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//

static void Execute(void *userWork, u32 id)
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if (!pButton->IsTrigger(gfl2::ui::BUTTON_A)) return;

  DebugMantain *pDebug = reinterpret_cast<DebugMantain*>(userWork);
  pDebug->Execute(id);
}

static void Execute01(void *userWork, gfl2::debug::DebugWinItem *item)
{
  Execute(userWork, 0);
}

static void Execute02(void *userWork, gfl2::debug::DebugWinItem *item)
{
  Execute(userWork, 1);
}

static void Execute03(void *userWork, gfl2::debug::DebugWinItem *item)
{
  Execute(userWork, 2);
}

static void Execute04(void *userWork, gfl2::debug::DebugWinItem *item)
{
  Execute(userWork, 3);
}

static void ExecuteAll(void *userWork, gfl2::debug::DebugWinItem *item)
{
  Execute(userWork, 4);
}

/**
*  @brief  Group ctor
*/
static void CreateDebugGroupFunc(void *userWork, gfl2::debug::DebugWinItem *item)
{
  DebugMantain *pDebug = reinterpret_cast<DebugMantain*>(userWork);
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>(item);
  if (pDebug && pGroup)
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData *pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_mantain_score_reset_0, pDebug, Execute01, NULL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_mantain_score_reset_1, pDebug, Execute02, NULL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_mantain_score_reset_2, pDebug, Execute03, NULL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_mantain_score_reset_3, pDebug, Execute04, NULL);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_mantain_score_reset_all, pDebug, ExecuteAll, NULL);
  }
}
/**
*  @brief  Group dtor
*/
static void DestroyDebugGroupFunc(void *userWork, gfl2::debug::DebugWinItem *item)
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>(item);
  if (pGroup)
  {
    pGroup->RemoveAllItem();
  }
}

/**
*  @brief  初期化
*/
void DebugMantain::Initialize(gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData)
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
}

/**
*  @brief  メニューの作成
*/
void DebugMantain::CreateDebugMenu(gfl2::debug::DebugWinGroup *root)
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, m_pMsgData, msg_dmenu_field_mantain_score, root);
  pGroup->SetCreateDestroyGroupFunc(this, CreateDebugGroupFunc, DestroyDebugGroupFunc);
}


/**
*/
void DebugMantain::Execute(u32 id)
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  //コースIDに対応する保持レコードと今回のレコードを比較して、更新していたら、上書きをする
  Savedata::Misc* p_misc = pGameManager->GetGameData()->GetMisc();
  //セーブデータ更新リセット
  if (id < 4)
  {
    p_misc->SetSurfRecord(id, 0);
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      p_misc->SetSurfRecord(i, 0);
    }
  }
  
  Sound::PlaySE(SEQ_SE_SAVE);
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
#endif //PM_DEBUG