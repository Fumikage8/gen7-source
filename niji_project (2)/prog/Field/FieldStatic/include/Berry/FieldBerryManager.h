/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryManagerr.h
 *  @brief  きのみポイントマネージャ
 *  @author Masanori Kanamaru
 *  @date   2015.10.09
 */
#if !defined(__FIELD_BERRY_MANAGER_H__)
#define __FIELD_BERRY_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldBerryPointAccessor.h"

GFL_NAMESPACE_BEGIN( Field )

// 前方参照
struct FieldBerryLottedData;

class FieldBerryManager
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldBerryManager );

public:
  enum BerryStatus
  {
    EMPTY,            //  何も無い(0時に復活する)
    READY,            //  復活直後の状態(抽選済み,ゾーンチェンジが起きていないので配置しては駄目)
    EXIST,            //  配置可能
  };

  struct VisualDesc
  {
    u32 modelID;
    u32 focusID;
    const void* pTalkColData;
    const void* pForbiddenColData;
  };

  const static u16 BERRY_SLOT_AMOUNT = 6;

  FieldBerryManager();

  ~FieldBerryManager();

  /**
   *  @brief  なくなっているきのみを復活させる
   *          各種抽選も行う
   */
  void BearBerrys();

  /**
   *  @brief  配置準備状態にあるきのみを配置可能状態にする
   */
  void UpdateBerrysStatusToExist();

  /**
   *  @brief  指定したきのみを配置されていない状態にする
   */
  void ConsumeBerry(const FieldBerryLottedData* pData);

  /**
   *  @brief  指定したきのみが配置されるかどうかを判定
   */
  bool ExistBerry(const FieldBerryPointAccessor::BerryPointData* pData) const;

  /**
   *  @brief  抽選済みのデータを取得する
   *  @return 「大量」かどうか
   */
  bool GetLottedData(const FieldBerryPointAccessor::BerryPointData* pData, FieldBerryLottedData* pOut) const;

  /**
   *  @brief  見た目やコリジョンのデータを取得する
   */
  void GetVisualDesc(const bool isWealth, VisualDesc* pOut) const;

private:
  // 常駐データのフォーマット
  struct ModelData
  {
    u16 modelID;
    u16 padding;
    u32 focusID;
    u32 talkColOffset;
    u32 forbiddenColOffset;
  };
  struct ResidentData
  {
    u32       amount;
    ModelData data[2];
  };

  /**
   *  @brief  コリジョンデータを常駐データから取得する
   */
  static void* ExtractCollisionData(const ResidentData* pData, u32 offset);
};

// 抽選後のデータ
struct FieldBerryLottedData 
{
  u32 berryPointID;               // 全ゾーン通してのきのみ配置場所のID
  u32 encountTableID;             // エンカウントテーブルのID
  b8  existMonster;              // バトルが発生するか
  u8  monsterLevel;               // エンカウント時のモンスターレベル
  u8  berryAmount;                // きのみ獲得数
  u8  padding;
  u16 berryItemIDSlots[FieldBerryManager::BERRY_SLOT_AMOUNT];    // 獲得可能なきのみのアイテムID
  u16 rareBerryItemID;            // レアきのみのアイテムID(大量でないときは0)
};

GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_BERRY_POINT_MANAGER_H__)