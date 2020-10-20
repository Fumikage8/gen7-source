//============================================================================================
/**
 * @file		BerrySpot.cpp
 * @brief	  フィールド上のきのみ状態データ
 * @author	Masanori Kanamaru
 * @date		15.10.30
 */
//============================================================================================

#include "Savedata/include/BerrySpot.h"

#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include "System/include/GflUse.h"

#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

GFL_NAMESPACE_BEGIN(Savedata)

//-----------------------------------------------------------------------------
/**
  *  @brief    読み込んだデータをセットする関数
  * @param    pData   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void BerrySpot::SetData(void* pData)
{
  gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&(this->data)), sizeof(Data) ); 
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
  */
//-----------------------------------------------------------------------------
void* BerrySpot::GetData()
{
  return reinterpret_cast<void*>(&(this->data));
}

//-----------------------------------------------------------------------------
/**
  *  @brief    データサイズ
  *  @retval   バイト
  */
//-----------------------------------------------------------------------------
size_t BerrySpot::GetDataSize()
{
  return sizeof(Data);
}

//-----------------------------------------------------------------------------
/**
  *  @brief    セーブデータのクリア
  *  @param    heap
  */
//-----------------------------------------------------------------------------
void BerrySpot::Clear(gfl2::heap::HeapBase* pHeap)
{
  this->InitData(pHeap);
}

/**
  * @brief      データ初期化
  */
void BerrySpot::InitData(gfl2::heap::HeapBase* pHeap)
{
	gfl2::std::MemClear( &(this->data), sizeof(BerrySpot::Data) );

  for(int i = 0; i < BerrySpot::DataAmount; ++i)
  {
    this->LotSpotData(i);
  }
}

/**
  *  @brief    指定したスポットの抽選値を取得
  */
const BerrySpot::SpotData* BerrySpot::GetSpotData(u32 id) const
{
  GFL_ASSERT_MSG(id < BerrySpot::DataAmount, "Invalid BerrySpot ID!! [%d] \n", id);
  return &(this->data.spots[id]);
}

/**
 *  @brief   指定したスポットのデータを再抽選
 */
void BerrySpot::LotSpotData(u32 id)
{
  GFL_ASSERT_MSG(id < BerrySpot::DataAmount, "Invalid BerrySpot ID!! [%d] \n", id);

  // 抽選
  this->data.spots[id].status        = Field::FieldBerryManager::READY;
  this->data.spots[id].wealthFactor  = System::GflUse::GetPublicRand(100);
  this->data.spots[id].amountFactor  = System::GflUse::GetPublicRand(100);
  this->data.spots[id].encountFactor = System::GflUse::GetPublicRand(100);
}

void BerrySpot::ChangeStatus(u32 id, u8 newStatus)
{
  GFL_ASSERT_MSG(id < BerrySpot::DataAmount, "Invalid BerrySpot ID!! [%d] \n", id);

  this->data.spots[id].status = newStatus;
}


GFL_NAMESPACE_END(Savedata)