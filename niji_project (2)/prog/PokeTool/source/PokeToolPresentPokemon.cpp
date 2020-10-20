//============================================================================================
/**
 * @file    poe_tool_PresentPokemon.cpp
 * @brief   配布ポケモンチェック関連（主に旧作）
 * @author  Hiroyuki Nakamura
 * @date    13.02.18
 *
 * namespace  PokeTool::
 */
//============================================================================================
#include "pml/include/pmlib.h"

#include "PokeTool/include/PokeToolRibbon.h"
#include "PokeTool/include/PokeToolPresentPokemon.h"
#include "System/include/PokemonVersion.h"


namespace PokeTool {

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
bool PresentPokemon::IsTradeEntry( pml::pokepara::CoreParam * cp )
{
  // 配布用リボンを持っているか
  if( IsHavePresentRibbon( cp ) == true ){
    return false;
  }

  // 旧作の配布ポケモンか
  if( IsOldVersionPokemon( cp ) == true ){
    return false;
  }

  return true;
}

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
bool PresentPokemon::IsHavePresentRibbon( pml::pokepara::CoreParam * cp )
{
  static const u32 tbl[] = {
    RIBBON_NO_COUNTRY,          ///< 22カントリーリボン
    RIBBON_NO_NATIONAL,         ///< 23ナショナルリボン
    RIBBON_NO_EARTH,            ///< 24アースリボン
    RIBBON_NO_WORLD,            ///< 25ワールドリボン
    RIBBON_NO_CLASSIC,          ///< 26クラシックリボン
    RIBBON_NO_PREMIERE,         ///< 27プレミアリボン
    RIBBON_NO_EVENT,            ///< 28イベントリボン（旧ヒストリーリボン）
    RIBBON_NO_BIRTHDAY,         ///< 29バースデーリボン（旧グリーンリボン）
    RIBBON_NO_SPECIAL,          ///< 30スペシャルリボン（旧ブルーリボン）
    RIBBON_NO_MEMORIAL,         ///< 31メモリアルリボン（旧フェスティバルリボン）
    RIBBON_NO_WISH,             ///< 32ウィッシュリボン（旧カーニバルリボン）
    RIBBON_NO_BATTLE_CHAMP,     ///< 33バトルチャンプリボン（旧マリンリボン）
    RIBBON_NO_AREA_CHAMP,       ///< 34エリアチャンプリボン（旧ランドリボン）
    RIBBON_NO_NATIONAL_CHAMP,   ///< 35ナショナルチャンプリボン（旧スカイリボン）
    RIBBON_NO_WORLD_CHAMP,      ///< 36ワールドチャンプリボン（旧レッドリボン）
  };

  bool ret  = false;

  bool fast = cp->StartFastMode();

  for( int i=0; i<GFL_NELEMS(tbl); i++ ){
    if( cp->HaveRibbon(tbl[i]) == true ){
      ret = true;
      break;
    }
  }

  cp->EndFastMode( fast );

  return ret;
}

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
bool PresentPokemon::IsOldVersionPokemon( pml::pokepara::CoreParam * cp )
{
  bool fast = cp->StartFastMode();

  MonsNo mons = cp->GetMonsNo();
  u32 rom = cp->GetCassetteVersion();

  cp->EndFastMode( fast );

  if( CheckRomVersion( rom ) == true &&
      CheckPresentPokemon( mons ) == true ){
    return true;
  }
  return false;
}

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
bool PresentPokemon::CheckRomVersion( u32 rom )
{
  static const u32 tbl[] = {
    VERSION_SAPPHIRE,   ///< AGBサファイア
    VERSION_RUBY,       ///< AGBルビー
    VERSION_EMERALD,    ///< AGBエメラルド
    VERSION_RED,        ///< AGBファイアレッド
    VERSION_GREEN,      ///< AGBリーフグリーン
    VERSION_GOLD,       ///< DSハートゴールド
    VERSION_SILVER,     ///< DSソウルシルバー
    VERSION_DIAMOND,    ///< DSダイヤモンド
    VERSION_PEARL,      ///< DSパール
    VERSION_PLATINUM,   ///< DSプラチナ
    VERSION_COLOSSEUM,  ///< GCコロシアム
    VERSION_WHITE,      ///< DSホワイト
    VERSION_BLACK,      ///< DSブラック
    VERSION_W2,         ///< DSホワイト２
    VERSION_B2,         ///< DSブラック２
  };

  for( int i=0; i<GFL_NELEMS(tbl); i++ ){
    if( rom == tbl[i] ){
      return true;
    }
  }
  return false;
}

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
bool PresentPokemon::CheckPresentPokemon( MonsNo mons )
{
  static const MonsNo tbl[] = {
    MONSNO_MYUU,        // ミュウ
    MONSNO_SEREBHI,     // セレビィ
    MONSNO_ZIRAATI,     // ジラーチ
    MONSNO_DEOKISISU,   // デオキシス
    MONSNO_MANAFI,      // マナフィ
    MONSNO_DAAKURAI,    // ダークライ
    MONSNO_SHEIMI,      // シェイミ
    MONSNO_ARUSEUSU,    // アルセウス
    MONSNO_BIKUTHINI,   // ビクティニ
    MONSNO_KERUDHIO,    // ケルディオ
    MONSNO_MEROETTA,    // メロエッタ
    MONSNO_GENOSEKUTO,  // ゲノセクト
  };

  for( int i=0; i<GFL_NELEMS(tbl); i++ ){
    if( mons == tbl[i] ){
      return true;
    }
  }
  return false;
}





} // PokeTool
