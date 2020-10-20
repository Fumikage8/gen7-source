//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffectStatus.h
 * @brief  １種類の位置エフェクトの状態
 * @author obata_toshihiro
 * @date   2015.06.18
 */
//-----------------------------------------------------------------------------
#ifndef BTL_POSEFFECTSTATUS_H_INCLUDED
#define BTL_POSEFFECTSTATUS_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include "./btl_common.h"
#include "./btl_PosEff.h"
#include "./btl_PosEffect.h"


namespace btl {


  /**
   * @class PosEffectStatus
   * @brief １種類の位置エフェクトの状態を管理する
   */
  class PosEffectStatus
  {
    GFL_FORBID_COPY_AND_ASSIGN( PosEffectStatus );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param pos        管理対象の位置
     * @param posEffect  管理対象の位置エフェクト
     */
    //-------------------------------------------------------------------------
    PosEffectStatus( BtlPokePos pos, BtlPosEffect posEffect );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~PosEffectStatus();

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトを登録する
     * @param effectParam  位置エフェクトのパラメータ
     * @retval true   位置エフェクトが有効になった
     * @retval false  位置エフェクトが有効にならなかった
     */
    //-------------------------------------------------------------------------
    bool SetEffect( const PosEffect::EffectParam& effectParam );

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトを解除する
     */
    //-------------------------------------------------------------------------
    void RemoveEffect( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトが有効か？
     * @retval true  有効
     * @retval false 有効でない
     */
    //-------------------------------------------------------------------------
    bool IsEffective( void ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトのパラメータを取得する
     */
    //-------------------------------------------------------------------------
    const PosEffect::EffectParam& GetEffectParam( void ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトのパラメータを設定する
     * @param effectParam  設定するパラメータ
     */
    //-------------------------------------------------------------------------
    void SetEffectParam( const PosEffect::EffectParam& effectParam );




  private:

    BtlPokePos              m_pos;          // 管理対象の位置
    BtlPosEffect            m_posEffect;    // 管理対象の位置エフェクト
    bool                    m_isEffective;  // サイドエフェクトが有効か？
    PosEffect::EffectParam  m_effectParam;  // 位置エフェクトのパラメータ

  };


} // namespace btl

#endif // BTL_POSEFFECTSTATUS_H_INCLUDED