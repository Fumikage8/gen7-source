//============================================================================================
/**
 * @file    poe_tool_PresentPokemon.h
 * @brief   配布ポケモンチェック関連（主に旧作）
 * @author  Hiroyuki Nakamura
 * @date    13.02.18
 *
 * namespace  PokeTool::
 */
//============================================================================================
#if !defined( __POKE_TOOL_PRESENTPOKEMON_H__ )
#define __POKE_TOOL_PRESENTPOKEMON_H__
#pragma once


namespace PokeTool {

class PresentPokemon {
public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   交換に出せるか
   *
   * @param   cp  CoreParam
   *
   * @retval  "true = 出せる"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  static bool IsTradeEntry( pml::pokepara::CoreParam * cp );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   配布リボンを持っているか
   *
   * @param   cp  CoreParam
   *
   * @retval  "true = 持っている"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  static bool IsHavePresentRibbon( pml::pokepara::CoreParam * cp );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   旧作の配布ポケモンか
   *
   * @param   cp  CoreParam
   *
   * @retval  "true = 旧作の配布ポケモン"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  static bool IsOldVersionPokemon( pml::pokepara::CoreParam * cp );


private:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   旧作のROMバージョンか
   *
   * @param   rom   ROMバージョン
   *
   * @retval  "true = 旧作"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  static bool CheckRomVersion( u32 rom );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   旧作で配布したポケモンか
   *
   * @param   mons  ポケモン番号
   *
   * @retval  "true = 配布した"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  static bool CheckPresentPokemon( MonsNo mons );


};


} // PokeTool


#endif  // __POKE_TOOL_PRESENTPOKEMON_H__
