//======================================================================
/**
* @file BattleFesManager.cpp
* @date 2016/11/23
* @author munakata_kai
* @brief バトルフェス常駐マネージャー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesMsgResourceManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"
#include "Savedata/include/RandomGroup.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#include "Field/FieldRo/include/Fieldmap.h"
// data
#include <niji_conv_header/field/zone/zone_id.h>
#endif

GFL_NAMESPACE_BEGIN( BattleFes )

BattleFesManager::BattleFesManager(gfl2::heap::HeapBase* pHeap)
  : m_pBattleFes(NULL)
  , m_pMsgResourceManager(NULL)
  , m_pSetupParam(NULL)
  , m_pBattleFesPokeLotterySystem(NULL)
  , m_pBtlRegulation(NULL)
  , m_pBattleFesSave(NULL)
  , m_monitorRandSeed(0)
{
  m_pBattleFesSave = GFL_NEW(pHeap) Savedata::BattleFesSave();
  m_pBattleFesSave->Clear(pHeap);
}

BattleFesManager::~BattleFesManager()
{
  GFL_SAFE_DELETE(m_pBattleFesSave);
}

void BattleFesManager::Create( GameSys::GameManager *p_gman )
{
  if( m_pBattleFes == NULL )
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_DEVICE );
    m_pBattleFes = GFL_NEW(p_heap) BattleFes(p_gman, p_heap);

#if PM_DEBUG
    if (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetZoneID() == Z_F0601JF0102)
    {
      BattleFesDebug::InitializeBattleFesDebug();
    }
#endif
  }
}

void BattleFesManager::Delete()
{
  if( m_pBattleFes != NULL )
  {
    GFL_SAFE_DELETE( m_pBattleFes );

#if PM_DEBUG
    BattleFesDebug::TerminateBattleFesDebug();
#endif
  }
}

gfl2::str::MsgData* BattleFesManager::GetMsgData()
{
  GFL_ASSERT(m_pMsgResourceManager != NULL);
  GFL_ASSERT(m_pMsgResourceManager->ObserveLoad());
  return m_pMsgResourceManager->GetMsgData();
}

BATTLE_SETUP_PARAM* BattleFesManager::GetSetupParam() const
{
  GFL_ASSERT(m_pSetupParam != NULL);
  return m_pSetupParam;
}

void BattleFesManager::CreateBattleFesPokeLotterySystem(gfl2::heap::HeapBase* pHeap)
{
  if (m_pBattleFesPokeLotterySystem == NULL)
  {
    m_pBattleFesPokeLotterySystem = GFL_NEW(pHeap) BattleFesPokeLotterySystem(pHeap);
    // ポケモン抽選＆作成
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    GameSys::GameData*    pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    Field::EventWork*     ev = pGameData->GetEventWork();
    bool                  isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
    Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
    m_pBattleFesPokeLotterySystem->PokemonLottery(pGameData->GetSaveData()->GetRandomGroup()->GetTodayRandSeed(), p_heap, pBattleFesSave->IsUseShop());
  }  
}
void BattleFesManager::DeleteBattleFesPokeLotterySystem()
{
  // ＮＵＬＬで呼ばれる設計ではない
  GFL_ASSERT(m_pBattleFesPokeLotterySystem != NULL);
  if (m_pBattleFesPokeLotterySystem != NULL)
  {
    GFL_SAFE_DELETE(m_pBattleFesPokeLotterySystem);
  }
}

Regulation* BattleFesManager::GetRegulation() const
{ 
  // NULLの場合有り
  return m_pBtlRegulation;
}

GFL_NAMESPACE_END( BattleFes )

