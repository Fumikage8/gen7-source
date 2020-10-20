//==============================================================================
/**
 @file    ZukanDistributionPositionDataAccessor.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の分布図の配置IDのリストのアクセッサー
 @note    zukan_distribution_position_data_converter.rbが生成したzukan_distribution_position_data.binの中身にアクセスする
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// 図鑑のインクルード
#include "ZukanDefine.h"

#include "ZukanDistributionPositionDataAccessor.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


const f32 ZukanDistributionPositionDataAccessor::POS_SCALL_VAL = 100.0f;

void ZukanDistributionPositionDataAccessor::SetupData(const void* buf)
{
  m_buf = buf;

  // アクセスしやすくしておく
  m_placeIdDataNum = *(reinterpret_cast<const u16*>(&((static_cast<const u8*>(m_buf))[2])));
  m_startByte = reinterpret_cast<const u32*>(&((static_cast<const u8*>(m_buf))[4]));
}

void ZukanDistributionPositionDataAccessor::GetPlaceIdData(u32 placeId, PlaceIdData* placeIdData) const
{
  if( m_buf != NULL )
  {
    if( placeId < m_placeIdDataNum )
    {
      const PlaceIdDataHeader* placeIdDataHeader = reinterpret_cast<const PlaceIdDataHeader*>(&((static_cast<const u8*>(m_buf))[m_startByte[placeId]]));

      placeIdData->islandNo    = placeIdDataHeader->islandNo;
      placeIdData->zoneDataNo  = placeIdDataHeader->zoneDataNo;
      placeIdData->encountType = placeIdDataHeader->encountType;
      placeIdData->posDataNum  = placeIdDataHeader->posDataNum;
      placeIdData->posData     = (placeIdDataHeader->posDataNum > 0) ?
                                    reinterpret_cast<const PosData*>(&(placeIdDataHeader->posDataTopIntX)) : NULL ;
    }
    else
    {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
      GFL_PRINT("WARNING(ZukanDistributionPositionDataAccessor.cpp): placeId=%d\n", placeId);
#else
      GFL_RELEASE_PRINT("########################################\n");
      GFL_RELEASE_PRINT("WARNING: Id Over: placeId=%d: dataNum=%d\n", placeId, m_placeIdDataNum);
      GFL_RELEASE_PRINT("########################################\n");
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT  // 開発中はpoke_dataとplace_dataが一致していないことがあるのでdefine分岐。
#endif
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

