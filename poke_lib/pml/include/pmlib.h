//=======================================================================================
/**
 * @brief  ポケモンライブラリ・インクルードヘッダ
 * @file   pmlib.h
 * @author obata_toshihiro
 * @author tamada
 * @date   2010.12.21
 *
 */
//=======================================================================================

#if !defined( __PMLIB_H__ )
#define __PMLIB_H__
#pragma once


#ifdef __cplusplus
#include <pml/include/pml_Type.h>
#include <pml/include/pml_Const.h>
#include <pml/include/pml_Personal.h>
#include <pml/include/pml_PokePara.h>
#include <pml/include/pml_Waza.h>
#include <pml/include/pml_System.h>
#include <pml/include/pml_PokeParty.h>
#include <pml/include/pml_Item.h>
#include <pml/include/pml_Battle.h>
#endif // __cplusplus


//---------------------------------------------------------------------
/**
 * @namespace pml
 * @brief   ポケモンライブラリ全体の名前空間
 *
 * \par 関連クラス
 * pml::Library
 * pml::PokeParty
 */
//---------------------------------------------------------------------
namespace pml {

  //---------------------------------------------------------------------
  /**
   * @namespace pml::personal
   * @brief   ポケモン種族ごとのパラメータに関する名前空間
   *
   * \par 関連クラス
   * personal::PersonalSystem
   *
   * \par 関連ヘッダ
   * include/personal/pml_PersonalSystem.h
   * include/personal/pml_PersonalType.h 
   *
   * \par 使い方概要
   * -# pml::System::PersonalSystem() 関数で, pml::personal::PersonalSystem クラスのインスタンスを取得する.
   * -# 取得した pml::personal::PersonalSystem クラスのインスタンスを介してデータを取得する.
   * 
   */
  //---------------------------------------------------------------------
  namespace personal {
  }

  //---------------------------------------------------------------------
  /**
   * @namespace pml::pokepara
   * @brief ポケモン個体データを扱う名前空間
   *
   * \par 関連クラス
   * pml::pokepara::Factory
   * pml::pokepara::PokemonParam
   * pml::pokepara::EvolutionSystem
   *
   * \par 関連ヘッダ
   * include/pokepara/pml_PokemonParamType.h
   * include/pokepara/pml_PokemonParamSpec.h
   * include/pokepara/pml_PokemonParam.h
   * include/pokepara/pml_PokemonParamFactory.h
   * include/pokepara/pml_PokemonParamEvolutionSystem.h
   *
   * \par 使い方概要
   * ポケパラを作成する
   * -# pml::System::PokeParaFactory() 関数で, pml::pokepara::PokeParaFactory クラスのインスタンスを取得する.
   * -# 取得した pml::pokepara::PokeParaFactory クラスを使用して pml::pokepara::PokemonParam インスタンスを生成する.  
   * ポケパラを破棄する
   * -# pml::pokepara::Factory クラスのメソッドを使用する.
   * ポケパラを操作する
   * -# pml::pokepara::PokemonParam クラスのメソッドを使用する.
   * ポケモンを進化させる
   * -# pml::System::PokeParaEvolutionSystem() 関数で, pml::pokepara::EvolutionSystem クラスのインスタンスを取得する.
   * -# 取得した pml::pokepara::EvolutionSystem クラスを使用して進化できるかどうかをチェックする.
   * -# pml::pokepara::PokemonParam::Evolve() メソッドで進化させる.
   * 
   */
  //---------------------------------------------------------------------
  namespace pokepara {
  }


  //---------------------------------------------------------------------
  /**
   * @namespace pml::wazadata
   * @brief   ポケモンが持つわざのパラメータに関する名前空間
   *
   * \par 関連クラス
   * pml::wazadata::System
   *
   * \par 関連ヘッダ
   * include/waza/pml_WazaDataSystem.h
   * 
   * \par 使い方
   * -# pml::System::WazaDataSystem() 関数で, pml::wazadata::System クラスのインスタンスを取得する.
   * -# 取得した pml::wazadata::System クラスのインスタンスを介してデータを取得する.
   */
  //---------------------------------------------------------------------
  namespace wazadata {
  }

  //---------------------------------------------------------------------
  /**
   * @namespace pml::local
   * @brief   ライブラリの内部のみで使用する名前空間
   */
  //---------------------------------------------------------------------
  namespace local {
  }

} //end of namespace pml

//---------------------------------------------------------------------
/**
 * @namespace item
 * @brief   どうぐ関連の名前空間
 */
//---------------------------------------------------------------------
namespace item {
}





#endif  // __PMLIB_H__

