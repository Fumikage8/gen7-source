//============================================================================================
/**
 * @file		FishingSpot.h
 * @brief	  フィールド上の釣りスポット状態データ
 * @author	Masanori Kanamaru
 * @date		15.10.30
 */
//============================================================================================

#if !defined(__SAVEDATA_FISHING_SPOT_H__)
#define __SAVEDATA_FISHING_SPOT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <Savedata/include/SaveDataInterface.h>

GFL_NAMESPACE_BEGIN(Savedata)

class FishingSpot : public SaveDataInterface
{
public:
  enum
  {
    DataAmount = 8,
  };

#if PM_DEBUG
  static bool debugForceSpawnRare;
#endif

  // --- SaveDataInterface実装 Start -------------------------

  //-----------------------------------------------------------------------------
  /**
   *  @brief    読み込んだデータをセットする関数
    * @param    pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  void SetData(void* pData);

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  void* GetData();
  //-----------------------------------------------------------------------------
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  //-----------------------------------------------------------------------------
  size_t GetDataSize();

  //-----------------------------------------------------------------------------
  /**
   *  @brief    セーブデータのクリア
   *  @param    heap
   */
  //-----------------------------------------------------------------------------
  void Clear( gfl2::heap::HeapBase* pHeap );

  // --- SaveDataInterface実装 End -------------------------

  /**
   *  @brief レア出現状況のリセット
   */
  void ResetFishingSpot();

  /**
   *  @brief 特定のスポットにレアが発生しているか
   *  @param スポットを特定するindex
   */
  bool ExistRare(u32 index) const;

  /**
   *  @brief  特定スポットのレア出現状態をオフにする
   *  @param スポットを特定するindex
   */
  void EscapeRare(u32 index);

  /**
   *  @brief レア抽選を行う必要があるか
   */
  bool NeedLottery() const;

  /**
   *  @brief レア抽選
   *  @param rarePercentage   スポット毎のレア出現率(配列)
   *  @oaram amount           スポットの数(= rarePercentageの長さ)
   */
  void LotRare(u32 rarePercentages[], u32 amount);

private :
  struct Data
  {
    u32 existRareBitField;             // スポット毎のレアの出現状況
    s32 isLotted;                      // レアの出現抽選を行ったか
    u8  dummyData[248];                // 今後の仕様変更に備えたへそくり(最終的には不要な部分)
  };

  Data data;

  /**
   *  @brief      データ初期化
   */
  void InitData();
};

GFL_NAMESPACE_END(Savedata)

#endif // __SAVEDATA_FISHING_SPOT_H__