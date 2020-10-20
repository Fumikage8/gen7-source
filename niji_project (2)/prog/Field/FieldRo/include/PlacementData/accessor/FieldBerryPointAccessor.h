/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryPointAccessor.h
 *  @brief  きのみ配置データアクセサ
 *  @author Masanori Kanamaru
 *  @date   2015.10.07
 */

#if !defined(__FIELD_BERRY_POINT_ACCESSOR_H__)
#define __FIELD_BERRY_POINT_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field );

class FieldBerryPointAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldBerryPointAccessor );

public:
  struct BerryPointData
  {
    u32 type;                       // 識別子
    f32 position[3];                // 座標
    f32 quaternion[4];              // 姿勢
    u32 eventID;                    // ゾーンにおける全イベント内でユニークなID
    u32 berryPointID;               // 全ゾーン通してのきのみ配置場所のID
    u32 encountTableID;             // エンカウントテーブルのID
    u16 encountPercentage;          // バトルエンカウント率
    u8  monsterLevel;               // 通常エンカウント時のモンスターレベル
    u8  bossMonsterLevel;           // ボスエンカウント時のモンスターレベル
    u16 wealthPercentage;           // きのみが「大量」となる確率
    u8  berryAmountMin;             // 通常時のきのみ獲得数下限
    u8  berryAmountMax;             // 通常時のきのみ獲得数上限
    u8  berryAmountWealthMin;       // 大量時のきのみ獲得数下限
    u8  berryAmountWealthMax;       // 大量時のきのみ獲得数上限
    u16 berryItemIDSlots[6];        // 獲得可能なきのみのアイテムID
    u16 rareberryItemID;            // レアきのみのアイテムID
  };

  /**
   *  @brief  コンストラクタ
   */
  FieldBerryPointAccessor()
    :m_pData( NULL )
  {
  }

  /**
   *  @brief  デストラクタ
   */
  ~FieldBerryPointAccessor(){}

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  最大数取得
   */
  u32 GetDataMax() const;

  /**
   *  @brief  配置データを1つ取得 
   */
  const BerryPointData* GetData(const u32 idx) const;

private:
  GFL_WARNING_WIN32_PUSH
  GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
  struct BerryPointDataHeader
  {
    u32 dataAmount;
    BerryPointData data[];
  };
  GFL_WARNING_WIN32_POP

  const BerryPointDataHeader *m_pData;
};

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_BERRY_POINT_ACCESSOR_H__)