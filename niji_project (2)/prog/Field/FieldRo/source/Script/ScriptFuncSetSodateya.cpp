//======================================================================
/**
 * @file ScriptFuncSetSodateya.cpp
 * @date 2015/12/15 22:57:19
 * @author onoue_masayuki
 * @brief フィールド用コマンド
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Script/ScriptFuncSetSodateya.h"
#include "Field/FieldStatic/include/Sodateya/FieldSodateya.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"

#include "Field/FieldStatic/include/Event/EventAddPokemon.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

//-----------------------------------------------------------------------------
/**
 *  @brief  命令テーブルセットを取得
 */
//-----------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE* FuncSetSodateya::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
  {
    GFL_PAWN_FUNC(_SodateyaPokeNumGet)
    GFL_PAWN_FUNC(_SodateyaWordsetPokeName)
#if 0
    GFL_PAWN_FUNC(_SodateyaWordsetPokeLevelUp)
#endif
    GFL_PAWN_FUNC(_SodateyaWordsetPokeTakeBackMenu)
    GFL_PAWN_FUNC(_SodateyaPokeLeave)
    GFL_PAWN_FUNC(_SodateyaPokeTakeBack)
    GFL_PAWN_FUNC(_SodateyaEggAdopt)
    GFL_PAWN_FUNC(_SodateyaEggDelete)
    GFL_PAWN_FUNC(_SodateyaLoveCheck)
    GFL_PAWN_FUNC_END,
  };

  return s_FuncTable;
}

GameSys::GameManager* FuncSetSodateya::GetGameManager(AMX * amx)
{
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  return p_obj->GetGameManager();
}

//-----------------------------------------------------------------------------
/**
 *  @brief 育て屋　預けているポケモンの数を返す
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaPokeNumGet(AMX * amx, const cell * ptr)
{
  GameSys::GameManager *p_gman = GetGameManager(amx);
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);

  return p_gman->GetGameData()->GetSodateyaSave()->PokeNumGet( id );
}


//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 預けているポケモンのニックネームワードセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaWordsetPokeName(AMX * amx, const cell * ptr)
{
  u32 buf_id   = ptr[1];
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[2]);
  u32 poke_idx = ptr[3];
  
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();

  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  p_sodateya->PokeNameWordset( p_this->GetSystemWork()->GetWordSet(), buf_id, id, poke_idx );
  return NULL;
}

#if 0
//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 預けているポケモンのレベルアップ値ワードセット&取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaWordsetPokeLevelUp(AMX * amx, const cell * ptr)
{
  u32 buf_id   = ptr[1];
   Savedata::SodateyaID id = static_cast< Savedata::SodateyaID>(ptr[2]);
  u32 poke_idx = ptr[3];
  
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();

  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  return p_sodateya->PokeLevelUpWordset( p_this->GetSystemWork()->GetWordSet(), buf_id, id, poke_idx );
}
#endif

//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 育て屋からポケモンを引き取る時のメニュー生成用ワードセット
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaWordsetPokeTakeBackMenu(AMX * amx, const cell * ptr)
{
  u32 buf_id   = ptr[1];
   Savedata::SodateyaID id = static_cast< Savedata::SodateyaID>(ptr[2]);
  u32 poke_idx = ptr[3];
   
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();
  FieldScriptSystem* p_this = FieldScriptSystem::GetInstance();
  return p_sodateya->PokeTakeBackMenuWordset( p_this->GetSystemWork()->GetWordSet(), buf_id, id, poke_idx );
}

//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 手持ちから育て屋にデータ移動
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaPokeLeave(AMX * amx, const cell * ptr)
{
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);
  u32 party_idx = ptr[2];
  
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();

  p_sodateya->PokeLeave( id, party_idx );
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 育て屋から手持ちにデータ移動
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaPokeTakeBack(AMX * amx, const cell * ptr)
{
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);
  u32 poke_idx = ptr[2];
  
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();

  p_sodateya->PokeTakeBack( id, poke_idx );
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 タマゴを手持ちに追加
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaEggAdopt(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);

  enum{
    FADE_DEFAULT_SYNC = 4, //Fadeは演出フェードの基本値4frame
  };

  pml::pokepara::PokemonParam* p_pp = p_sodateya->EggAdopt(id);
  if( p_pp == NULL ) return false;

  //パラメータが返ってきたらイベントを呼び出す
  gfl2::math::Vector4 col_s(0.0f,0.0f,0.0f,0.0f);
  gfl2::math::Vector4 col_e(0.0f,0.0f,0.0f,255.0f);

  EventAddPokemon* p_event = EventAddPokemon::CallAddPokemonFromPP( p_gman, *p_pp, ADD_POKEMON_MODE_DEFAULT );
  p_event->SetFadeParam( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, col_s, col_e, FADE_DEFAULT_SYNC, false );

  GFL_SAFE_DELETE(p_pp);

  p_sys->Suspend();
  return true;
}


//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 発生したタマゴを削除
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaEggDelete(AMX * amx, const cell * ptr)
{
  GameSys::GameManager *p_gman = GetGameManager(amx);
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::Sodateya* p_sodateya_sv = p_gdata->GetSodateyaSave();
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);

  p_sodateya_sv->EggClear( id );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 *	@brief 育て屋 相性チェック
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetSodateya::_SodateyaLoveCheck(AMX * amx, const cell * ptr)
{
  GameSys::GameManager *p_gman = GetGameManager(amx);
  FieldSodateya* p_sodateya = p_gman->GetFieldmap()->GetSodateya();
  Savedata::SodateyaID id = static_cast<Savedata::SodateyaID>(ptr[1]);

  return p_sodateya->LoveCheck( id );
}

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );


