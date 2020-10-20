//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffect.h
 * @brief  位置エフェクト
 * @author obata_toshihiro
 * @date   2015.06.22
 */
//-----------------------------------------------------------------------------
#ifndef BTL_POSEFFECT_H_INCLUDED
#define BTL_POSEFFECT_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include "./btl_common.h"
#include "./btl_PosEff.h"


namespace btl {


  /**
   * @class PosEffect
   * @brief 位置エフェクトの静的な設定を定義するクラス
   */
  class PosEffect
  {
    GFL_FORBID_COPY_AND_ASSIGN( PosEffect );

  public:

    /**
     * @brief パラメータの型
     */
    enum EffectParamType
    {
      PARAM_TYPE_NONE,          // パラメータなし
      PARAM_TYPE_DELAY_ATTACK,  // 遅延攻撃
      PARAM_TYPE_DELAY_RECOVER, // 遅延回復
    };


    /**
     * @brief 位置エフェクトのパラメータ
     */
    union EffectParam
    {
      struct
      {
        u32 param1;  // パラメータの値( パラメータの型によって解釈が異なります )
      } Raw;

      // パラメータの型が「遅延攻撃( PARAM_TYPE_DELAY_ATTACK )」の場合
      struct
      {
        u32 wazaNo        : 16;  // 発動する技の技No.( WAZANO_xxxx )
        u32 execTurnMax   :  4;  // 発動に要するターン数
        u32 execTurnCount :  4;  // 経過したターン数
        u32 padding       :  8;
      } DelayAttack;
    };


    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトに対応するパラメータの型を取得する
     * @param posEffect  位置エフェクト
     */
    //-------------------------------------------------------------------------
    static EffectParamType GetEffectParamType( BtlPosEffect posEffect );





  private:

    PosEffect( void ) {}
    virtual ~PosEffect() {}

  };


} // namespace btl

#endif // BTL_POSEFFECT_H_INCLUDED