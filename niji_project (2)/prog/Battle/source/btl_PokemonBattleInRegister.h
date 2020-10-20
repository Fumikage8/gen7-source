//=============================================================================
/**
 * @file   btl_PokemonBattleInRegister.h
 * @date   2015/08/21 19:10:44
 * @author obata_toshihiro
 * @brief  １ターンの期間内において、各ポケモンが場に出たかどうかの記録
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined POKEMON_BATTLE_IN_REGISTER_H_INCLUDED
#define POKEMON_BATTLE_IN_REGISTER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_const.h"

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class PokemonBattleInRegister
   * @brief １ターンの期間内において、各ポケモンが場に出たかどうかを記録する
   */
  class PokemonBattleInRegister
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokemonBattleInRegister );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    PokemonBattleInRegister( void );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~PokemonBattleInRegister();

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンが場に出た( 出ていた )ことを記録する
     * @param pokeId  記録対象ポケモンのID
     */
    //-------------------------------------------------------------------------
    void Register( u8 pokeId );

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンが場に出た( 出ていた )かどうか取得する
     * @param pokeId  チェック対象ポケモンのID
     * @retval true   場に出た( 出ていた )
     * @retval false  場に出ていない
     */
    //-------------------------------------------------------------------------
    bool Check( u8 pokeId ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 記録をクリアする
     */
    //-------------------------------------------------------------------------
    void Clear( void );



  private:

    bool m_isPokemonBattleIn[ BTL_POKEID_MAX ];

  };


GFL_NAMESPACE_END( btl )

#endif // POKEMON_BATTLE_IN_REGISTER_H_INCLUDED