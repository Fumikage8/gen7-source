//-----------------------------------------------------------------------------
/**
 * @file   btl_SideEffectManager.h
 * @brief  全陣営のサイドエフェクトの管理
 * @author obata_toshihiro
 * @date   2015.06.16
 */
//-----------------------------------------------------------------------------
#ifndef BTL_SIDEEFFECTMANAGER_H_INCLUDED
#define BTL_SIDEEFFECTMANAGER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include "./btl_common.h"
#include "./btl_SideEff.h"


namespace btl {

  class SideEffectStatus;


  /**
   * @class SideEffectManager
   * @brief 全陣営のサイドエフェクトを管理する
   */
  class SideEffectManager
  {
    GFL_FORBID_COPY_AND_ASSIGN( SideEffectManager );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap  使用するヒープ
     */
    //-------------------------------------------------------------------------
    SideEffectManager( HeapHandle heap );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~SideEffectManager();

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのサイドエフェクトを解除する
     */
    //-------------------------------------------------------------------------
    void RemoveAllSideEffect( void );
    

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief サイドエフェクトの状態を取得する
     * @param side    取得したい陣営
     * @param effect  取得したいサイドエフェクト
     * @retval NULL  指定したサイドエフェクトが存在しない場合
     */
    //-------------------------------------------------------------------------
    SideEffectStatus* GetSideEffectStatus( BtlSide side, BtlSideEffect effect );

    //-------------------------------------------------------------------------
    /**
     * @brief サイドエフェクトの状態を取得する
     * @param side    取得したい陣営
     * @param effect  取得したいサイドエフェクト
     * @retval NULL  指定したサイドエフェクトが存在しない場合
     */
    //-------------------------------------------------------------------------
    const SideEffectStatus* GetSideEffectStatusConst( BtlSide side, BtlSideEffect effect ) const;





  private:

    void CreateEffectStatusArray( HeapHandle heap );
    void DeleteEffectStatusArray( void );
    SideEffectStatus* GetSideEffectStatusCommon( BtlSide side, BtlSideEffect effect ) const;


  private:

    SideEffectStatus* m_sideEffects[ BTL_SIDE_NUM ][ BTL_SIDEEFF_MAX ];

  };


} // namespace btl

#endif // BTL_SIDEEFFECTMANAGER_H_INCLUDED