#if PM_DEBUG

//======================================================================
/**
* @file FieldDebugRotomPower.cpp
* @date 2017/01/30
* @author munakata_kai
* @brief デバッグ：ロトムパワー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugRotomPower.h"

// gfl2
#include <fs/include/gfl2_FsAsyncFileManager.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

// msg
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// sound
#include "Sound/include/Sound.h"

#include <debug/include/gfl2_DebugWinItem.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

gfl2::ui::Button* DebugRotomPower::GetButton()
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  return pButton;
}

s32 DebugRotomPower::UpdateKey(gfl2::ui::Button* pButton, s32 input, s32 value, s32 valueHigh, s32 min, s32 max)
{
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    value = valueHigh;
  }

  s32 ret = input;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    ret = input + value;
    if( ret > max)
    {
      ret = min;
    }
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    ret = input - value;
    if( ret < min)
    {
      ret = max;
    }
  }

  return ret;
}

// 更新
void DebugRotomPower::UpdateFunc_RotomPower(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = DebugRotomPower::GetButton();
  DebugRotomPower* pThis = (DebugRotomPower*)userWork;

  s32 rotomPowerId = DebugRotomPower::UpdateKey(pButton, pThis->m_rotomPowerID, 1, 1, Field::RotomPower::ROTOM_POWER_ID_NULL, Field::RotomPower::ROTOM_POWER_ID_SOUGUU_DEC);
  if (pThis->m_rotomPowerID != rotomPowerId)
  {
    pThis->m_rotomPowerID = rotomPowerId;
    pThis->m_isChangeRotomPowerID = true;
  }

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    if (pThis->m_rotomPowerID == Field::RotomPower::ROTOM_POWER_ID_NULL)
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->ClearRotomPower();
    }
    else
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->SetRotomPower(static_cast <Field::RotomPower::ROTOM_POWER_ID>(pThis->m_rotomPowerID));
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->Start();
    }
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

// 更新
void DebugRotomPower::UpdateFunc_RotomPowerTime(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = DebugRotomPower::GetButton();
  DebugRotomPower* pThis = (DebugRotomPower*)userWork;

  pThis->m_rotomPowerTime = DebugRotomPower::UpdateKey(pButton, pThis->m_rotomPowerTime, 1, 1, 0, 59);

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->SetDebugRotomPowerTime(pThis->m_rotomPowerTime);
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

// 表示
wchar_t* DebugRotomPower::DrawFunc_RotomPower(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  const u32 rotomPowerID[] = {
    msg_dmenu_rotom_power_02,
    msg_dmenu_rotom_power_03,
    msg_dmenu_rotom_power_04,
    msg_dmenu_rotom_power_05,
    msg_dmenu_rotom_power_06,
    msg_dmenu_rotom_power_07,
    msg_dmenu_rotom_power_08,
    msg_dmenu_rotom_power_09,
  };
  DebugRotomPower *pDebug = reinterpret_cast<DebugRotomPower*>(userWork);
  if (pDebug->m_isChangeRotomPowerID)
  {
    pDebug->m_pMsgData->GetString(rotomPowerID[pDebug->m_rotomPowerID], *pDebug->m_pRotomPowerIDSTR);
    pDebug->m_isChangeRotomPowerID = false;
  }
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), pDebug->m_pRotomPowerIDSTR->GetPtr());
  return workStr;
}

// 表示
wchar_t* DebugRotomPower::DrawFunc_NowRotomPower(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  const u32 rotomPowerID[] = {
    msg_dmenu_rotom_power_02,
    msg_dmenu_rotom_power_03,
    msg_dmenu_rotom_power_04,
    msg_dmenu_rotom_power_05,
    msg_dmenu_rotom_power_06,
    msg_dmenu_rotom_power_07,
    msg_dmenu_rotom_power_08,
    msg_dmenu_rotom_power_09,
  };
  DebugRotomPower *pDebug = reinterpret_cast<DebugRotomPower*>(userWork);
  if (pDebug->m_nowRotomPowerID != GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->GetRotomPowerID())
  {
    pDebug->m_nowRotomPowerID = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->GetRotomPowerID();
    pDebug->m_pMsgData->GetString(rotomPowerID[pDebug->m_nowRotomPowerID], *pDebug->m_pNowRotomPowerIDSTR);
  }
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls→%ls", item->GetName(), pDebug->m_pNowRotomPowerIDSTR->GetPtr());
  return workStr;
}

// 表示
wchar_t* DebugRotomPower::DrawFunc_RotomPowerTime(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugRotomPower *pDebug = reinterpret_cast<DebugRotomPower*>(userWork);
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_rotomPowerTime);
  return workStr;
}

/**
 *  @brief  Group create
 */
void DebugRotomPower::CreateDebugGroupFunc(void *userWork, gfl2::debug::DebugWinItem *item)
{
  DebugRotomPower *pDebug = reinterpret_cast<DebugRotomPower*>(userWork);
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->m_pHeap;
    gfl2::str::MsgData* pMsgData = pDebug->m_pMsgData;

    pDebug->m_rotomPowerTime = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->GetDebugRotomPowerTime();

    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_rotom_power_01, pDebug, DebugRotomPower::UpdateFunc_RotomPower, DebugRotomPower::DrawFunc_RotomPower);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_rotom_power_10, pDebug, NULL, DebugRotomPower::DrawFunc_NowRotomPower);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_rotom_power_11, pDebug, DebugRotomPower::UpdateFunc_RotomPowerTime, DebugRotomPower::DrawFunc_RotomPowerTime);
  }
}

/**
 *  @brief  Group destroy
 */
void DebugRotomPower::DestroyDebugGroupFunc(void *userWork, gfl2::debug::DebugWinItem *item)
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );

  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

//------------------------------------------------------------------------------
// DebugRotomPower
//------------------------------------------------------------------------------

/**
 * @class DebugRotomPower
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugRotomPower::DebugRotomPower(void)
  : m_pHeap( NULL)
  , m_pMsgData( NULL)
  , m_rotomPowerID(Field::RotomPower::ROTOM_POWER_ID_NULL)
  , m_pRotomPowerIDSTR(NULL)
  , m_isChangeRotomPowerID(true)
  , m_nowRotomPowerID(Field::RotomPower::ROTOM_POWER_ID_NULL)
  , m_pNowRotomPowerIDSTR(NULL)
  , m_rotomPowerTime(0)
{
}

/**
 *  @brief  デストラクタ
 */
DebugRotomPower::~DebugRotomPower(void)
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugRotomPower::Initialize(
  gfl2::heap::HeapBase *pHeap,
  gfl2::str::MsgData* pMsgData
  )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
  m_rotomPowerID = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->GetRotomPowerID();
  m_pRotomPowerIDSTR = GFL_NEW(m_pHeap) gfl2::str::StrBuf(gfl2::debug::DEBUGWIN_NAME_LEN, m_pHeap);
  m_isChangeRotomPowerID = true;
  m_nowRotomPowerID = -1;
  m_pNowRotomPowerIDSTR = GFL_NEW(m_pHeap) gfl2::str::StrBuf(gfl2::debug::DEBUGWIN_NAME_LEN, m_pHeap);
}

/**
 *  @brief  破棄
 */
void DebugRotomPower::Terminate(void)
{
  GFL_SAFE_DELETE(m_pRotomPowerIDSTR);
  GFL_SAFE_DELETE(m_pNowRotomPowerIDSTR);
}

/**
 *  @brief  メニューの作成
 */
void DebugRotomPower::CreateDebugMenu(gfl2::debug::DebugWinGroup *root)
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, m_pMsgData, msg_dmenu_rotom_power_00, root);
  pGroup->SetCreateDestroyGroupFunc(this, DebugRotomPower::CreateDebugGroupFunc, DebugRotomPower::DestroyDebugGroupFunc);
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
