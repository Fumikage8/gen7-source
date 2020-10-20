//==============================================================================
/**
 @file    ZukanDistributionPokeDataAccessor.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の分布図のポケモンごとの分布データのアクセッサー
 @note    zukan_distribution_encount_data_converter.rbが生成したzukan_distribution_poke_data.binの中身にアクセスする
 */
//==============================================================================

// gfl2のインクルード
#include <debug/include/gfl2_Assert.h>

// 図鑑のインクルード
#include "ZukanDistributionPokeDataAccessor.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


void ZukanDistributionPokeDataAccessor::SetupData(const void* buf)
{
  m_buf = buf;

  // アクセスしやすくしておく
  m_monsNo_end = *(reinterpret_cast<const u16*>(&((static_cast<const u8*>(m_buf))[2])));
  m_startByte  = static_cast<const u32*>(m_buf);
}

void ZukanDistributionPokeDataAccessor::GetPokeDistributionData(MonsNo monsNo, PokeDistributionData* pokeDistributionData) const
{
  if( m_buf != NULL )
  {
    if( 1<=monsNo && monsNo<=static_cast<s32>(m_monsNo_end) )
    {
      const void* addr = &( (static_cast<const u8*>(m_buf))[ m_startByte[monsNo] ] );
      const PokeDistributionDataHeader* pokdeDistributionDataHeader = static_cast<const PokeDistributionDataHeader*>(addr);
     
      pokeDistributionData->placeIdDataNum = pokdeDistributionDataHeader->placeIdDataNum;
      pokeDistributionData->bitFlag        = pokdeDistributionDataHeader->bitFlag;
      pokeDistributionData->placeIdData    = (pokdeDistributionDataHeader->placeIdDataNum > 0) ?
                                                 //static_cast<const PlaceIdData*>(static_cast<const void*>(&(pokdeDistributionDataHeader->placeIdDataTopPlaceId))) : NULL ;  // 上下行どちらの書き方でもコンパイルエラーにならない
                                                 reinterpret_cast<const PlaceIdData*>(&(pokdeDistributionDataHeader->placeIdDataTopPlaceId)) : NULL ;
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

// 昼通常
bool ZukanDistributionPokeDataAccessor::IsAppearanceDay(const PlaceIdData* placeIdData)
{
  return ( ( placeIdData->appearanceBitFlag & (1<<APPEARANCE_DAY_REGULAR) ) != 0 );
}

// 夜通常
bool ZukanDistributionPokeDataAccessor::IsAppearanceNight(const PlaceIdData* placeIdData)
{
  return ( ( placeIdData->appearanceBitFlag & (1<<APPEARANCE_NIGHT_REGULAR) ) != 0 );
}

// 昼乱入
bool ZukanDistributionPokeDataAccessor::IsAppearanceDayEnter(const PlaceIdData* placeIdData)
{
  static const u16 APPEARANCE_DAY_BITS =  
      ( 1 << APPEARANCE_DAY_ENTER      )
    | ( 1 << APPEARANCE_DAY_RAIN_ENTER )
    | ( 1 << APPEARANCE_DAY_SNOW_ENTER )
    | ( 1 << APPEARANCE_DAY_SAND_ENTER );

  return ( ( placeIdData->appearanceBitFlag & APPEARANCE_DAY_BITS ) != 0 );
}

// 夜乱入
bool ZukanDistributionPokeDataAccessor::IsAppearanceNightEnter(const PlaceIdData* placeIdData)
{
  static const u16 APPEARANCE_NIGHT_BITS =  
      ( 1 << APPEARANCE_NIGHT_ENTER      )
    | ( 1 << APPEARANCE_NIGHT_RAIN_ENTER )
    | ( 1 << APPEARANCE_NIGHT_SNOW_ENTER )
    | ( 1 << APPEARANCE_NIGHT_SAND_ENTER );

  return ( ( placeIdData->appearanceBitFlag & APPEARANCE_NIGHT_BITS ) != 0 );
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

