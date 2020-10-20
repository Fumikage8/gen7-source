//-----------------------------------------------------------------------------
/**
 * @file   btl_PosEffectManager.h
 * @brief  全ての位置の位置エフェクトの管理
 * @author obata_toshihiro
 * @date   2015.06.18
 */
//-----------------------------------------------------------------------------
#ifndef BTL_POSEFFECTMANAGER_H_INCLUDED
#define BTL_POSEFFECTMANAGER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include "./btl_common.h"
#include "./btl_PosEff.h"


namespace btl {

  class PosEffectStatus;


  /**
   * @class PosEffectManager
   * @brief 全ての位置の位置エフェクトを管理する
   */
  class PosEffectManager
  {
    GFL_FORBID_COPY_AND_ASSIGN( PosEffectManager );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap  使用するヒープ
     */
    //-------------------------------------------------------------------------
    PosEffectManager( HeapHandle heap );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~PosEffectManager();

    //-------------------------------------------------------------------------
    /**
     * @brief 全ての位置エフェクトを解除する
     */
    //-------------------------------------------------------------------------
    void RemoveAllPosEffect( void );
    

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトの状態を取得する
     * @param pos        取得したい位置
     * @param posEffect  取得したい位置エフェクト
     * @retval NULL  指定した位置エフェクトが存在しない場合
     */
    //-------------------------------------------------------------------------
    PosEffectStatus* GetPosEffectStatus( BtlPokePos pos, BtlPosEffect posEffect );

    //-------------------------------------------------------------------------
    /**
     * @brief 位置エフェクトの状態を取得する
     * @param pos        取得したい位置
     * @param posEffect  取得したい位置エフェクト
     * @retval NULL  指定した位置エフェクトが存在しない場合
     */
    //-------------------------------------------------------------------------
    const PosEffectStatus* GetPosEffectStatusConst( BtlPokePos pos, BtlPosEffect posEffect ) const;





  private:

    void CreateEffectStatusArray( HeapHandle heap );
    void DeleteEffectStatusArray( void );
    PosEffectStatus* GetPosEffectStatusCommon( BtlPokePos pos, BtlPosEffect posEffect ) const;


  private:

    PosEffectStatus* m_posEffects[ BTL_EXIST_POS_MAX ][ BTL_POSEFF_MAX ];

  };


} // namespace btl

#endif // BTL_POSEFFECTMANAGER_H_INCLUDED