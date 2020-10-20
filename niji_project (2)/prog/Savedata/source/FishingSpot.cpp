//============================================================================================
/**
 * @file		FishingSpot.cpp
 * @brief	  フィールド上の釣りスポット状態データ
 * @author	Masanori Kanamaru
 * @date		15.10.30
 */
//============================================================================================

#include "Savedata/include/FishingSpot.h"

#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include "System/include/GflUse.h"

GFL_NAMESPACE_BEGIN(Savedata)

#if PM_DEBUG
bool FishingSpot::debugForceSpawnRare = false;
#endif

//-----------------------------------------------------------------------------
/**
  *  @brief    読み込んだデータをセットする関数
  * @param    pData   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void FishingSpot::SetData(void* pData)
{
  gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&(this->data)), sizeof(Data) ); 
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void* FishingSpot::GetData()
{
  return reinterpret_cast<void*>(&(this->data));
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データサイズ
  *  @retval   バイト
  */
//-----------------------------------------------------------------------------
size_t FishingSpot::GetDataSize()
{
  return sizeof(Data);
}

//-----------------------------------------------------------------------------
/**
  *  @brief    セーブデータのクリア
  *  @param    heap
  */
//-----------------------------------------------------------------------------
void FishingSpot::Clear(gfl2::heap::HeapBase* pHeap)
{
  this->InitData();
}

/**
 *  @brief レア出現状況のリセット
 */
void FishingSpot::ResetFishingSpot()
{
  this->InitData();
}

/**
 *  @brief 特定のスポットにレアが発生しているか
 *  @param スポットを特定するindex
 */
bool FishingSpot::ExistRare(u32 index) const
{
  u32 bit = (1 << index);

  return (this->data.existRareBitField & bit) == bit;
}

/**
 *  @brief  特定スポットのレア出現状態をオフにする
 *  @param スポットを特定するindex
 */
void FishingSpot::EscapeRare(u32 index)
{
  u32 bit = (1 << index);

  this->data.existRareBitField = this->data.existRareBitField & (~bit);
}

/**
 *  @brief レア抽選を行う必要があるか
 */
bool FishingSpot::NeedLottery() const
{
  return this->data.isLotted == false;
}

/** 
 *  @brief レア抽選
 *  @param rarePercentage   スポット毎のレア出現率(配列)
 *  @oaram amount           スポットの数(= rarePercentageの長さ)
 */
void FishingSpot::LotRare(u32 rarePercentages[], u32 amount)
{
  for(int i = 0; i < amount; ++i)
  {
    bool isRare = (System::GflUse::GetPublicRand(100) < rarePercentages[i]);

#if PM_DEBUG
    isRare |= (FishingSpot::debugForceSpawnRare && rarePercentages[i] > 0);
#endif

    u32 bit = isRare ? (1 << i) : 0;

    this->data.existRareBitField = this->data.existRareBitField | bit;
  }

  this->data.isLotted = true;
}

/**
  * @brief      データ初期化
  */
void FishingSpot::InitData()
{
  gfl2::std::MemClear( &(this->data), sizeof(Data) );

  this->data.existRareBitField = 0;

  this->data.isLotted = false;
}

GFL_NAMESPACE_END(Savedata)
