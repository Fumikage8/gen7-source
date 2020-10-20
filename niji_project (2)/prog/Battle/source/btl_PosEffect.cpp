//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffect.cpp
 * @brief  位置エフェクト
 * @author obata_toshihiro
 * @date   2015.06.22
 */
//-----------------------------------------------------------------------------
#include "./btl_PosEffect.h"


namespace btl {



  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトに対応するパラメータの型を取得する
   * @param posEffect  位置エフェクト
   */
  //-------------------------------------------------------------------------
  PosEffect::EffectParamType PosEffect::GetEffectParamType( BtlPosEffect posEffect )
  {
    static const EffectParamType ParamTypeTable[ BTL_POSEFF_MAX ] =
    {
      PARAM_TYPE_DELAY_RECOVER,// BTL_POSEFF_NEGAIGOTO
      PARAM_TYPE_NONE,         // BTL_POSEFF_MIKADUKINOMAI
      PARAM_TYPE_NONE,         // BTL_POSEFF_IYASINONEGAI
      PARAM_TYPE_DELAY_ATTACK, // BTL_POSEFF_DELAY_ATTACK
      PARAM_TYPE_NONE,         // BTL_POSEFF_BATONTOUCH
      PARAM_TYPE_NONE,         // BTL_POSEFF_ZENRYOKU_EFFECT_REPAIR_HP
    };

    if( BTL_POSEFF_MAX <= posEffect ) 
    {
      GFL_ASSERT(0);
      return PARAM_TYPE_NONE;
    }

    return ParamTypeTable[ posEffect ];
  }
  


} // namespace btl