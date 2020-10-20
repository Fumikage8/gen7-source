//=============================================================================
/**
 * @file   btl_BossBattle.h
 * @date   2016/01/24 14:31:37
 * @author obata_toshihiro
 * @brief  代表ビースト戦
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_BOSSBATTLE_H_INCLUDED
#define BTL_BOSSBATTLE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h>

GFL_NAMESPACE_BEGIN( btl )

 

  /**
   * @class BossBattle
   * @brief 代表ビースト戦だけの例外処理を定義するクラス
   */
  class BossBattle
  {
    GFL_FORBID_COPY_AND_ASSIGN( BossBattle );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief ボス戦用のデフォルト能力アップを適用する
     * @param[out] setupParam  適用対象のセットアップパラメータ
     */
    //-------------------------------------------------------------------------
    static void ApplyDefaultPowerUp( BATTLE_SETUP_PARAM* setupParam );

  };



GFL_NAMESPACE_END( btl )

#endif // BTL_BOSSBATTLE_H_INCLUDED