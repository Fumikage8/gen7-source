//==============================================================================
/**
 @file    ZukanDistributionZoneDataAccessor.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.17
 @brief   図鑑の分布図のゾーンリストのアクセッサー
 @note    zukan_distribution_zone_data_bin_maker.rbが生成したzukan_distribution_zone_data.binの中身にアクセスする
 */
//==============================================================================

// gfl2のインクルード
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// 図鑑のインクルード
#include "ZukanDistributionZoneDataAccessor.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


const f32 ZukanDistributionZoneDataAccessor::POS_SCALL_VAL = 100.0f;

void ZukanDistributionZoneDataAccessor::ZoneDataBitFlagSetOn(u32 zoneDataBitFlag[], u32 zoneDataNo)
{
  if( zoneDataNo < ZukanDistributionZoneDataAccessor::ZONE_DATA_NUM_MAX )
  {
    zoneDataBitFlag[zoneDataNo/32] = zoneDataBitFlag[zoneDataNo/32] | (1 << zoneDataNo%32);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT。ゾーンデータ番号が大き過ぎる。
  }
}
bool ZukanDistributionZoneDataAccessor::ZoneDataBitFlagIsOn(const u32 zoneDataBitFlag[], u32 zoneDataNo)
{
  return ( ( zoneDataBitFlag[zoneDataNo/32] & (1 << zoneDataNo%32) ) != 0 );
}

void ZukanDistributionZoneDataAccessor::SetupData(const void* buf)
{
  m_buf = buf;

  // アクセスしやすくしておく
  m_zoneDataNum = *(reinterpret_cast<const u16*>(&((static_cast<const u8*>(m_buf))[2])));
  m_zoneDataBinArray = reinterpret_cast<const ZoneDataBin*>(&((static_cast<const u8*>(m_buf))[4]));

  // データ数チェック
  if(m_zoneDataNum > ZONE_DATA_NUM_MAX)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データ数が想定外の多さ。
  }
}

void ZukanDistributionZoneDataAccessor::GetZoneData(u32 zoneDataNo, ZoneData* zoneData) const
{
  if( m_buf != NULL )
  {
    if( zoneDataNo < m_zoneDataNum )
    {
      const ZoneDataBin* zoneDataBin = &(m_zoneDataBinArray[zoneDataNo]);

      zoneData->islandNo   = zoneDataBin->islandNo;
      zoneData->intX       = zoneDataBin->intX;
      zoneData->intZ       = zoneDataBin->intZ;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT 
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

