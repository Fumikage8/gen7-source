//============================================================================================
/**
 * @file		BerrySpot.h
 * @brief	  フィールド上のきのみ状態データ
 * @author	Masanori Kanamaru
 * @date		15.10.30
 */
//============================================================================================

#if !defined(__SAVEDATA_BERRY_SPOT_H__)
#define __SAVEDATA_BERRY_SPOT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <Savedata/include/SaveDataInterface.h>

GFL_NAMESPACE_BEGIN(Savedata)

class BerrySpot : public SaveDataInterface
{
public:
  enum
  {
    DataAmount = 64,
  };

  struct SpotData
  {
    u8 status;
    u8 amountFactor;  // 0 - 99
    u8 wealthFactor;  // 0 - 99
    u8 encountFactor; // 0 - 99  
  };

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
   *  @brief    指定したスポットの抽選値を取得
   */
  const SpotData* GetSpotData(u32 id) const;

  /**
   *  @brief   指定したスポットのデータを再抽選
   */
  void LotSpotData(u32 id);

  /**
   *  @brief    指定したスポットの抽選値を変更
   */
  void SetSpotData(u32 id, const SpotData* pData);

  void ChangeStatus(u32 id, u8 newStatus);

private :
  struct Data
  {
    SpotData spots[DataAmount];
  };

  Data data;

  /**
   *  @brief      データ初期化
   */
  void InitData(gfl2::heap::HeapBase* pHeap);
};

GFL_NAMESPACE_END(Savedata)

#endif // __SAVEDATA_BERRY_SPOT_H__