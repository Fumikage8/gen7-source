/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryManagerr.cpp
 *  @brief  きのみポイントマネージャ
 *  @author Masanori Kanamaru
 *  @date   2015.10.09
 */

#include <util/include/gfl2_std_string.h>

#include "GameSys/include/GameManager.h"

#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

GFL_NAMESPACE_BEGIN( Field )

FieldBerryManager::FieldBerryManager()
{}

FieldBerryManager::~FieldBerryManager()
{}

/**
 *  @brief  なくなっているきのみを復活させる
 *          各種抽選も行う
 */
void FieldBerryManager::BearBerrys()
{
  KANAMARU_PRINT("Start Bearing Berrys\n");

  Savedata::BerrySpot* saveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBerrySpotData(true);  // 復活処理は常にオリジナルのセーブデータを操作
  for (u32 i = 0; i < Savedata::BerrySpot::DataAmount; ++i)
  {   
    if(saveData->GetSpotData(i)->status != EMPTY)
    {
      continue;
    }

    KANAMARU_PRINT("Berry[%d] Beared\n", i);

    // 抽選
    saveData->LotSpotData(i);
  }
}

/**
 *  @brief  配置準備状態にあるきのみを配置可能状態にする
 */
void FieldBerryManager::UpdateBerrysStatusToExist()
{
  KANAMARU_PRINT("Update BerryStatus\n");

  Savedata::BerrySpot* saveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBerrySpotData();

  for (u32 i = 0; i < Savedata::BerrySpot::DataAmount; ++i)
  {
    if(saveData->GetSpotData(i)->status != READY)
    {
      continue;
    }

    KANAMARU_PRINT("Berry[%d] Change To Exist\n", i);

    saveData->ChangeStatus(i, EXIST);
  }
}

/**
 *  @brief  指定したきのみを配置されていない状態にする
 */
void FieldBerryManager::ConsumeBerry(const FieldBerryLottedData* pData)
{
  GFL_ASSERT(pData->berryPointID < Savedata::BerrySpot::DataAmount);

  Savedata::BerrySpot* saveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBerrySpotData();

  GFL_ASSERT(saveData->GetSpotData(pData->berryPointID)->status != EMPTY);

  KANAMARU_PRINT("Consume Berry[%d] \n", pData->berryPointID);

  saveData->ChangeStatus(pData->berryPointID, EMPTY);
}

/**
 *  @brief  指定したきのみが配置されるかどうかを判定
 */
bool FieldBerryManager::ExistBerry(const FieldBerryPointAccessor::BerryPointData* pData) const
{
  GFL_ASSERT(pData->berryPointID < Savedata::BerrySpot::DataAmount);

  Savedata::BerrySpot* saveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBerrySpotData();

  return saveData->GetSpotData(pData->berryPointID)->status == EXIST;
}

/**
 *  @brief  抽選済みのデータを取得する
 */
bool FieldBerryManager::GetLottedData(const FieldBerryPointAccessor::BerryPointData* pData, FieldBerryLottedData* pOut) const
{
  GFL_ASSERT(pData->berryPointID < Savedata::BerrySpot::DataAmount);

  Savedata::BerrySpot* saveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBerrySpotData();

  const Savedata::BerrySpot::SpotData* pFactorData = saveData->GetSpotData(pData->berryPointID);

  pOut->berryPointID      = pData->berryPointID;
  pOut->encountTableID    = pData->encountTableID;

  bool isWealth = pFactorData->wealthFactor < pData->wealthPercentage;

  // 「大量」かつ「設定されているエンカウント確率が0%でない」場合は必ずエンカウント
  if ((isWealth && pData->encountPercentage > 0))
  {
    pOut->existMonster = true;
  }
  else
  {
    pOut->existMonster = pFactorData->encountFactor < pData->encountPercentage;
  }

  pOut->monsterLevel  = (isWealth) ? pData->bossMonsterLevel : pData->monsterLevel;
    
  u8 min;
  u8 max;
  if (isWealth)
  {
    min = pData->berryAmountWealthMin;
    max = pData->berryAmountWealthMax;
  }
  else
  {
    min = pData->berryAmountMin;
    max = pData->berryAmountMax;
  }
  pOut->berryAmount = min + pFactorData->amountFactor * (max - min + 1) / 100;

  gfl2::std::MemCopy(pData->berryItemIDSlots, &(pOut->berryItemIDSlots), sizeof(u16) * FieldBerryManager::BERRY_SLOT_AMOUNT);
  pOut->rareBerryItemID   = (isWealth) ? pData->rareberryItemID : 0;

  return isWealth;
}

/**
  *  @brief  見た目やコリジョンのデータを取得する
  */
void FieldBerryManager::GetVisualDesc(const bool isWealth, VisualDesc* pOut) const
{
  ResidentData* pData = reinterpret_cast<ResidentData*>(GameSys::GameManager::GetInstance()->GetFieldResident()->GetResource( BL_IDX_FIELD_RESIDENT__BERRY_RESIDENT_DATA_BIN));

  ModelData* pD = &(pData->data[isWealth ? 1 : 0]);

  pOut->modelID           = pD->modelID;
  pOut->focusID           = pD->focusID;
  pOut->pTalkColData      = FieldBerryManager::ExtractCollisionData(pData, pD->talkColOffset);
  pOut->pForbiddenColData = FieldBerryManager::ExtractCollisionData(pData, pD->forbiddenColOffset);
}

/**
 *  @brief  コリジョンデータを常駐データから取得する
 */
void* FieldBerryManager::ExtractCollisionData(const ResidentData* pData, u32 offset)
{
  uptr baseAddr = reinterpret_cast<uptr>(pData);  
  return reinterpret_cast<void*>(baseAddr + offset);
}

GFL_NAMESPACE_END( Field )
