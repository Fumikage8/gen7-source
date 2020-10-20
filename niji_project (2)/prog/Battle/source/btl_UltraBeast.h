//=============================================================================
/**
 * @file   btl_UltraBeast.h
 * @date   2016/01/07 15:02:55
 * @author obata_toshihiro
 * @brief  ウルトラビースト
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ULTRABEAST_H_INCLUDED
#define BTL_ULTRABEAST_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_POKEPARAM;


  /**
   * @class UltraBeast
   * @brief ウルトラビーストに関する定義
   */
  class UltraBeast
  {
    GFL_FORBID_COPY_AND_ASSIGN( UltraBeast );

  public:

    /**
     * @brief ウルトラビーストかどうかをチェックする
     * @param monsno  チェック対象のモンスター
     * @retval true   指定したモンスターはウルトラビーストである
     * @retval false  指定したモンスターはウルトラビーストでない
     */
    static bool IsUltraBeast( MonsNo monsno );

    /**
     * @brief ウルトラビーストかどうかをチェックする
     * @param poke  チェック対象のポケモン
     * @retval true   指定したモンスターはウルトラビーストである
     * @retval false  指定したモンスターはウルトラビーストでない
     */
    static bool IsUltraBeast( const BTL_POKEPARAM& poke );

    /**
     * @brief 名前を「？？？？？」と表示すべきポケモンかどうかをチェックする
     * @param monsno  チェック対象のモンスター
     * @retval true   名前を「？？？？？」と表示すべき
     * @retval false  名前を「？？？？？」と表示すべきでない
     */
    static bool IsUnknownNamePokemon( const MainModule& mainModule, MonsNo monsno );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_ULTRABEAST_H_INCLUDED