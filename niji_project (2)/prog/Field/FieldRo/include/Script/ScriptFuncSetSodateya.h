//======================================================================
/**
 * @file ScriptFuncSetSodateya.h
 * @date 2015/12/15 22:54:15
 * @author onoue_masayuki
 * @brief フィールド用コマンド
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __SCRIPTFUNCSETSODATEYA_H_INCLUDED__
#define __SCRIPTFUNCSETSODATEYA_H_INCLUDED__
#pragma once

#include <pawn\include\gfl2_Pawn.h>

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )  ///< ScriptObject の定義に合わせて修正

/**
* @class FuncSetSodateya
* @brief 育て屋関コマンド
*/
class FuncSetSodateya
{
  GFL_FORBID_COPY_AND_ASSIGN(FuncSetSodateya);
public:
  static const GFL_PAWN_FUNC_TABLE* GetTable( void );

private:
  static GameSys::GameManager* GetGameManager(AMX * amx);

  static cell AMX_NATIVE_CALL _SodateyaPokeNumGet( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaWordsetPokeName( AMX*, const cell* );
#if 0
  static cell AMX_NATIVE_CALL _SodateyaWordsetPokeLevelUp( AMX*, const cell* );
#endif
  static cell AMX_NATIVE_CALL _SodateyaWordsetPokeTakeBackMenu( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaPokeLeave( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaPokeTakeBack( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaEggAdopt( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaEggDelete( AMX*, const cell* );
  static cell AMX_NATIVE_CALL _SodateyaLoveCheck( AMX*, const cell* );
};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCRIPTFUNCSETSODATEYA_H_INCLUDED__