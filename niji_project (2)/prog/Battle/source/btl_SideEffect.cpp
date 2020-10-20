//-----------------------------------------------------------------------------
/**
 * @file   btl_SideEffect.cpp
 * @brief  サイドエフェクト
 * @author obata_toshihiro
 * @date   2015.06.16
 */
//-----------------------------------------------------------------------------
#include "./btl_SideEffect.h"


namespace btl {


  namespace {


    static const struct 
    {
      BtlSideEffect  sideEffect;   // サイドエフェクトの種類
      u32            maxAddCount;  // 重ねがけ最大数
    }
    SIDE_EFFECT_DESC[] = 
    {
      { BTL_SIDEEFF_REFRECTOR,      1 },
      { BTL_SIDEEFF_HIKARINOKABE,   1 },
      { BTL_SIDEEFF_AURORAVEIL,     1 },
      { BTL_SIDEEFF_SINPINOMAMORI,  1 },
      { BTL_SIDEEFF_SIROIKIRI,      1 },
      { BTL_SIDEEFF_OIKAZE,         1 },
      { BTL_SIDEEFF_OMAJINAI,       1 },
      { BTL_SIDEEFF_MAKIBISI,       3 },
      { BTL_SIDEEFF_DOKUBISI,       2 },
      { BTL_SIDEEFF_STEALTHROCK,    1 },
      { BTL_SIDEEFF_WIDEGUARD,      1 },
      { BTL_SIDEEFF_FASTGUARD,      1 },
      { BTL_SIDEEFF_RAINBOW,        1 },
      { BTL_SIDEEFF_BURNING,        1 },
      { BTL_SIDEEFF_MOOR,           1 },
      { BTL_SIDEEFF_NEBANEBANET,    1 },
      { BTL_SIDEEFF_TATAMIGAESHI,   3 },
      { BTL_SIDEEFF_TRICKGUARD,     1 },
      { BTL_SIDEEFF_SPOTLIGHT,      1 },
    };


  } // namespace



  //-------------------------------------------------------------------------
  /**
   * @brief 重ねがけの最大数を取得する
   * @param sideEffect  取得したいサイドエフェクト
   */
  //-------------------------------------------------------------------------
  u32 SideEffect::GetMaxAddCount( BtlSideEffect sideEffect )
  {
    for( u32 i=0; i<GFL_NELEMS(SIDE_EFFECT_DESC); ++i )
    {
      if( SIDE_EFFECT_DESC[i].sideEffect == sideEffect ) {
        return SIDE_EFFECT_DESC[i].maxAddCount;
      }
    }

    GFL_ASSERT(0);
    return 0;
  }



} // namespace btl