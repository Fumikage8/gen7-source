//=============================================================================
/**
 * @file   btl_WazaTargetSelector.h
 * @date   2015/07/23 19:21:39
 * @author obata_toshihiro
 * @brief  技ターゲットの選出
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_WAZATARGETSELECTOR_H_INCLUDED
#define BTL_WAZATARGETSELECTOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./btl_server_const.h"
#include "./btl_PokemonPosition.h"


GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_POKEPARAM;
  class POKECON;
  class PokeSet;


  /**
   * @class WazaTargetSelector
   * @brief 技ターゲットの選出を行うクラス
   */
  class WazaTargetSelector
  {
    GFL_FORBID_COPY_AND_ASSIGN( WazaTargetSelector );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 技ターゲットを選出する
     * 
     * @param      mainModule          参照するメインモジュール
     * @param      pokecon             ターゲット選出対象のポケモンコンテナ
     * @param      attacker            攻撃するポケモン
     * @param      targetPos           技ターゲットの位置
     * @param      wazaParam           使用する技のパラメータ
     * @param      intrPokeID         「カウンター」「ミラーコート」等により、
     *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
     * @param[out] rec                 選出したポケモンの格納先
     * @param[out] isTargetTemptEnable 選出後、技「このゆびとまれ」特性「よびみず」等による、技ターゲットの変更を許可するか？
     */
    //-------------------------------------------------------------------------
    static void SelectWazaTargets(
      const MainModule*     mainModule, 
      POKECON*              pokecon, 
      BTL_POKEPARAM*        attacker, 
      BtlPokePos            targetPos,
      const SVFL_WAZAPARAM* wazaParam, 
      u8                    intrPokeID, 
      PokeSet*              rec,
      bool*                 isTargetTemptEnable );



  private:

    static void SelectWazaTargets_Single( const MainModule* mainModule, POKECON* pokecon, BTL_POKEPARAM* attacker, BtlPokePos targetPos, const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, PokeSet* rec ); 
    static void SelectWazaTargets_Double( const MainModule* mainModule, POKECON* pokecon, BTL_POKEPARAM* attacker, BtlPokePos targetPos, const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, PokeSet* rec, bool* isTargetTemptEnable ); 
    static void SelectWazaTargets_Royal( const MainModule* mainModule, POKECON* pokecon, BTL_POKEPARAM* attacker, BtlPokePos targetPos, const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, PokeSet* rec, bool* isTargetTemptEnable ); 
    static void SelectWazaTargets_Intrude( const MainModule* mainModule, POKECON* pokecon, BTL_POKEPARAM* attacker, BtlPokePos targetPos, const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, PokeSet* rec, bool* isTargetTemptEnable ); 
    static BTL_POKEPARAM* GetOpponentPokemon( const MainModule* mainModule, POKECON* pokecon, BtlPokePos pos, u8 pokeSideIdx );
    static BTL_POKEPARAM* GetNextPokemon( const MainModule* mainModule, POKECON* pokecon, BtlPokePos pos ); 
    static BtlPokePos CorrectNoroiTarget( const MainModule* mainModule, const POKECON* pokecon, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BtlPokePos targetPos );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_WAZATARGETSELECTOR_H_INCLUDED
