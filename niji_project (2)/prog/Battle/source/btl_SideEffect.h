//-----------------------------------------------------------------------------
/**
 * @file   btl_SideEffect.h
 * @brief  サイドエフェクト
 * @author obata_toshihiro
 * @date   2015.06.16
 */
//-----------------------------------------------------------------------------
#ifndef BTL_SIDEEFFECT_H_INCLUDED
#define BTL_SIDEEFFECT_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include "./btl_common.h"
#include "./btl_SideEff.h"


namespace btl {


  /**
   * @class SideEffect
   * @brief サイドエフェクトに関する静的メソッド群
   */
  class SideEffect
  {
  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 重ねがけの最大数を取得する
     * @param sideEffect  取得したいサイドエフェクト
     */
    //-------------------------------------------------------------------------
    static u32 GetMaxAddCount( BtlSideEffect sideEffect );



  private:

    // インスタンスは生成できない
    SideEffect( void );
    ~SideEffect();

  };


} // namespace btl

#endif // BTL_SIDEEFFECT_H_INCLUDED