//============================================================================================
/**
 * @file		WeatherSave.h
 * @brief	  天候セーブデータ
 * @author	Saito
 * @date		15.12.21
 */
//============================================================================================

#if !defined(__WEATHER_SAVE_H__)
#define __WEATHER_SAVE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <Savedata/include/SaveDataInterface.h>
#include "niji_conv_header/field/WeatherKindDef.h"

GFL_NAMESPACE_BEGIN(Savedata)

class WeatherSave : public SaveDataInterface
{
public:
  enum
  {
    DataCount = 8,
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
  * @brief      データ取得
  * @param      idx データインデックス
  * @return     WeatherKind 天候種類
  */
  Field::weather::WeatherKind GetWeather(const int idx);

  /**
  * @brief      データセット
  * @param      idx 　　　　 データインデックス
  * @param      weather_kind 天候種類
  */
  void SetWeather(const int idx, Field::weather::WeatherKind weather_kind);

  /**
  * @brief      データリセット
  * @param      idx データインデックス
  */
  void ResetWeather(const int idx);

private :
  struct Data
  {
    s32 weather[DataCount];
    u8  dummyData[256];                // 今後の仕様変更に備えたへそくり(最終的には不要な部分)
  };

  Data data;

  /**
   *  @brief      データ初期化
   */
  void InitData();

  
};

GFL_NAMESPACE_END(Savedata)

#endif // __WEATHER_SAVE_H__