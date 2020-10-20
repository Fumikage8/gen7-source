//============================================================================================
/**
 * @file		FishingSpot.cpp
 * @brief	  天候セーブデータ
 * @author	Saito
 * @date		15.12.21
 */
//============================================================================================

#include "Savedata/include/WeatherSave.h"

#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include "System/include/GflUse.h"

#include "niji_conv_header/field/WeatherKindDef.h"

GFL_NAMESPACE_BEGIN(Savedata)
//-----------------------------------------------------------------------------
/**
  *  @brief    読み込んだデータをセットする関数
  * @param    pData   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void WeatherSave::SetData(void* pData)
{
  gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&(this->data)), sizeof(Data) ); 
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void* WeatherSave::GetData()
{
  return reinterpret_cast<void*>(&(this->data));
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データサイズ
  *  @retval   バイト
  */
//-----------------------------------------------------------------------------
size_t WeatherSave::GetDataSize()
{
  return sizeof(Data);
}

//-----------------------------------------------------------------------------
/**
  *  @brief    セーブデータのクリア
  *  @param    heap
  */
//-----------------------------------------------------------------------------
void WeatherSave::Clear(gfl2::heap::HeapBase* pHeap)
{
  this->InitData();
}




/**
  * @brief      データ初期化
  */
void WeatherSave::InitData()
{
  for(u32 i = 0; i < DataCount; ++i)
  {
    ResetWeather(i);
  }
  NOZOMU_PRINT( "dummydata_size = %d\n", sizeof(this->data.dummyData) );
  ::std::memset( this->data.dummyData, 0 , sizeof(this->data.dummyData) );
}

/**
  * @brief      データ取得
  * @param      idx データインデックス
  * @return     WeatherKind 天候種類
  */
Field::weather::WeatherKind WeatherSave::GetWeather(const int idx)
{
  if (idx >= DataCount)
  {
    GFL_ASSERT(0);
    return Field::weather::FORCE_WEATHER_NONE;
  }

  return (Field::weather::WeatherKind)(this->data.weather[idx]);
}

/**
 * @brief      データセット
 * @param      idx 　　　　 データインデックス
 * @param      weather_kind 天候種類
 */
void WeatherSave::SetWeather(const int idx, Field::weather::WeatherKind weather_kind)
{
  if (idx >= DataCount)
  {
    GFL_ASSERT(0);
    return;
  }

  this->data.weather[idx] = (s32)weather_kind;
}

 /**
  * @brief      データリセット
  * @param      idx データインデックス
  */
void WeatherSave::ResetWeather(const int idx)
{
  if (idx >= DataCount)
  {
    GFL_ASSERT(0);
    return;
  }
  this->data.weather[idx] = (s32)(Field::weather::FORCE_WEATHER_NONE);

}

GFL_NAMESPACE_END(Savedata)
