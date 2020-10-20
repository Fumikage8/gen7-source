//=============================================================================
/**
 * @file   btl_DiarySave.h
 * @date   2016/02/17 17:46:14
 * @author obata_toshihiro
 * @brief  ポケモンダイアリー
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_DIARYSAVE_H_INCLUDED
#define BTL_DIARYSAVE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( btl )

  class BTL_POKEPARAM;


  /**
   * @class DiarySave
   * @brief ポケモンダイアリー
   */
  class DiarySave
  {
    GFL_FORBID_COPY_AND_ASSIGN( DiarySave );

  public:

    /**
     * @brief ダイアリー( 努力値が最大になった )をセットする
     * @param poke  努力値を取得したポケモン
     */
    static void SetEffortValueFull( const BTL_POKEPARAM& poke );

    /**
     * @brief ダイアリー( レベルアップした )をセットする
     * @param poke  レベルアップしたポケモン
     */
    static void SetLevelUp( const BTL_POKEPARAM& poke );

  };



GFL_NAMESPACE_END( btl )

#endif // BTL_DIARYSAVE_H_INCLUDED