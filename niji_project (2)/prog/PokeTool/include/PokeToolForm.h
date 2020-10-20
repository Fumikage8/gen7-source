//==============================================================================
/**
 * @file poke_tool_Form.h
 * @brief フォルム更新処理
 * @date  2012.06.15
 * @author  tamada@gamefreak.co.jp
 */
//==============================================================================

#if !defined( __POKE_TOOL_FORM_H__)
#define __POKE_TOOL_FORM_H__

//#include <gflib.h>
#include "pml/include/pmlib.h"

#include "GameSys/include/GameData.h"


//======================================================================
//
//======================================================================

namespace PokeTool {

  /**
   * @namespace PokeTool::form
   * @brief フォルムの更新処理、ユーティリティなど
   */
  namespace form {
  //======================================================================
  //  定数など
  //======================================================================

  //------------------------------------------------------------------------
  /// プードルポケモンのトリミング状態が維持する日数
  //------------------------------------------------------------------------
  extern const u32 TRIMING_KEEP_DAYS;

  //======================================================================
  //
  //  公開関数
  //
  //======================================================================
  //------------------------------------------------------------------------
  /**
   * @brief ケルディオのフォルムを戻す処理
   * @param gamedata
   */
  //------------------------------------------------------------------------
  extern void UpdateKerudhioForm( GameSys::GameData * gamedata );

  //------------------------------------------------------------------------
  /**
   * @brief  ケルディオフォルムチェンジ PokemonParam版
   * @param  PokemonParam*
   */
  //------------------------------------------------------------------------
  extern void UpdateKerudhioFormPP( GameSys::GameData * gamedata, pml::pokepara::PokemonParam * pp );
    
  //------------------------------------------------------------------------
  /**
   * @brief 時間情報でシェイミのフォルム情報を更新する
   * @param my_party
   * @param now
   * @param passed_minutes
   */
  //------------------------------------------------------------------------
  extern void UpdateSheimiForm( pml::PokeParty * my_party, const gfl2::system::Date *now, u32 passed_minutes );

  //------------------------------------------------------------------------
  /**
   * @brief シェイミのフォルムを元に戻す
   */
  //------------------------------------------------------------------------
  extern void ChangeSheimiFormNormal( pml::PokeParty * my_party );

  //------------------------------------------------------------------------
  /**
   * @brief 時間でプードルポケモンの情報を更新する
   * @param my_party
   * @param passed_days
   */
  //------------------------------------------------------------------------
  extern void UpdatePoodleForm( pml::PokeParty * my_party, u32 passed_days );

  //------------------------------------------------------------------------
  /**
   * @brief プードルポケモンのフォルムをセット
   * @param pp
   * @param formno
   */
  //------------------------------------------------------------------------
  extern void SetPoodleForm( pml::pokepara::CoreParam * pp, pml::FormNo formno );
  extern void SetPoodleForm( pml::pokepara::PokemonParam * pp, pml::FormNo formno );

  //------------------------------------------------------------------------
  /**
   * @brief フーパのフォルムをセット
   * @param pp
   * @param formno
   */
  //------------------------------------------------------------------------
  extern void SetHuupaForm( pml::pokepara::CoreParam * pp, pml::FormNo formno );
  extern void SetHuupaForm( pml::pokepara::PokemonParam * pp, pml::FormNo formno );

  //------------------------------------------------------------------------
  /**
   * @brief ミノムッチのフォルムを地形で更新する
   * @param[out]  p_gamedata    ゲーム進行データ
   * @param[in]   fightIndex    バトルに参加したかどうか？のフラグ列
   * @param[in]   form_no       地形から導出したフォルムナンバー
   *
   * @note  2013.05.18 BTS6803【シーケンス：ミノムッチのフォルム違いが登録されるタイミングが前作と相違している】対応のため、引数を変更した
   */
  //------------------------------------------------------------------------
  extern void UpdateMinomuttiForm( GameSys::GameData * p_gamedata,
    const u8 * fightIndex, u8 form_no );

  //------------------------------------------------------------------------
  /**
   * @brief     ボックスでのフォルム解除処理
   * @param     pp  CoreParamへのポインタ
   * @return     true = フォルムを強制セットした
   *
   * @note    シェイミ、トリミアンをボックスに入れた時のフォルムクリア処理
   */
  //------------------------------------------------------------------------
  extern bool UpdateFormForBox( pml::pokepara::CoreParam * pp );

  //------------------------------------------------------------------------
  /**
   * @brief     セルからのジガルデ生成処理
   * 
   * PokemonParamをnewするので、呼び出し元で開放してください
   * ジガルデ生成で消費されるセル数のチェックや処理は外部でやる(zigarude_reborn.p)
   */
  //------------------------------------------------------------------------
  extern pml::pokepara::PokemonParam * ZigarudeMakeFromCell( GameSys::GameData* p_gdata, FormNo formNo, u32 level, gfl2::heap::HeapBase* p_heap );

  //------------------------------------------------------------------------
  /**
   * @brief バトル専用フォルムをノーマルに戻す
   * @param my_party  手持ちポケモンパーティー
   * @note  改造対策用
   */
  //------------------------------------------------------------------------
  extern void NormalizeBattleForm( pml::PokeParty * my_party );

  };  //namespace form

}; //namespace PokeTool

#endif //!defined( __POKE_TOOL_FORM_H__)
