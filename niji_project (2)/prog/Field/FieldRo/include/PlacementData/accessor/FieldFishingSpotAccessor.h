/**
 *  GAME FREAK inc.
 *
 *  @file   FieldFishingSpotAccessor.h
 *  @brief  釣りスポット配置データアクセサ
 *  @author Masanori Kanamaru
 *  @date   2015.10.22
 */

#if !defined(__FIELD_FISHING_SPOT_ACCESSOR_H__)
#define __FIELD_FISHING_SPOT_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field );

class FieldFishingSpotAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldFishingSpotAccessor );
public:
  struct LotData
  {
    u32 encountTableID;             // エンカウントテーブルのID
    u32 dropItemTableID;            // ドロップアイテムテーブルID
    u32 encountPercentage;          // バトルエンカウント率
  };

  struct Data
  {
    u32 type;                       // 識別子
    f32 position[3];                // 座標
    f32 quaternion[4];              // 姿勢
    u32 flagwork;                   //
    u32 flagWorkValue;              //
    u32 eventID;                    // ゾーンにおける全イベント内でユニークなID
    u32 focusID;                    // 注目ID
    u16 staticModelID;              // 靄モデルID
    u16 rarePercentage;             // レア発生確率
    LotData normalTable;            // 通常抽選データ
    LotData rareTable;              // レア抽選データ
    f32 invalidAreaRadius;          // 無効(逃走)範囲の半径
    u32 watchingAreaOffset;         // 警戒範囲データのオフセット
    u32 actionAreaOffset;           // 釣り可能範囲データのオフセット
  };

  enum CollisionType
  {
    Watching,
    Action,
  };

  /**
   *  @brief  コンストラクタ
   */
  FieldFishingSpotAccessor();

  /**
   *  @brief  デストラクタ
   */
  ~FieldFishingSpotAccessor();

  void Initialize( const void *data);

  u32 GetDataMax() const;

  const Data* GetData(const u32 idx) const;

  /**
   *  @brief  コリジョン情報取得
   */
  const void* GetCollisionData( const Data *pData, CollisionType type) const;

private:
  GFL_WARNING_WIN32_PUSH
  GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
  struct DataHeader
  {
    u32 dataAmount;
    Data data[];
  };
  GFL_WARNING_WIN32_POP

  const DataHeader *pData;
};

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_FISHING_SPOT_ACCESSOR_H__)