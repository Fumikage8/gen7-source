//=============================================================================
/**
 * @file   btl_Oteire.h
 * @date   2016/01/20 20:27:30
 * @author obata_toshihiro
 * @brief  お手入れ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_OTEIRE_H_INCLUDED
#define BTL_OTEIRE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( PokeTool )
class KawaigariParamCareCoreManager;
GFL_NAMESPACE_END( PokeTool )


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class POKECON;
class BTL_POKEPARAM;
class BTL_PARTY;


/**
 * @class Oteire
 * @brief お手入れに関する仕様を定義するクラス
 */
class Oteire
{
public:

  //---------------------------------------------------------------------------
  /**
   * @brief お手入れ対象となるポケモンのIDを取得する
   * @param[out] isFound          お手入れ対象が見つかったか？
   * @param[out] oteireTargetID   お手入れ対象ポケモンのID( 対象が見つからない場合 BTL_POKEID_NULL )
   * @param      mainModule       参照する MainModule
   * @param      pokecon          参照する POKECON
   * @param      myClientId       プレイヤのクライアントID
   */
  //---------------------------------------------------------------------------
  static void GetOteireTarget(
    bool*               isFound,
    u8*                 oteireTargetIndex,
    const MainModule*   mainModule,
    const POKECON*      pokecon,
    u8                  myClientId );

  //---------------------------------------------------------------------------
  /**
   * @brief お手入れ対象ポケモンか存在するかチェックする
   * @param poke  チェック対象のパーティ
   * @retval true    お手入れ対象ポケモンが存在する
   * @retval false   お手入れ対象ポケモンが存在しない
   */
  //---------------------------------------------------------------------------
  static bool IsOteireTargetPokeExist( const BTL_PARTY& party );

  //---------------------------------------------------------------------------
  /**
   * @brief お手入れ対象ポケモンかどうかをチェックする
   * @param poke  チェック対象のポケモン
   * @retval true    お手入れ対象
   * @retval false   お手入れ対象でない
   */
  //---------------------------------------------------------------------------
  static bool IsOteireTarget( const BTL_POKEPARAM& poke );

  //---------------------------------------------------------------------------
  /**
   * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
   * @param party          操作対象のパーティ
   * @param kawaigariData  汚れが付いて良いかのデータ
   */
  //---------------------------------------------------------------------------
  static void RemoveIllegalDirt( BTL_PARTY* party, PokeTool::KawaigariParamCareCoreManager* kawaigariData );

  //---------------------------------------------------------------------------
  /**
   * @brief 設定上、付いてはいけない汚れが付いていたら、汚れを取り除く
   * @param poke           操作対象のポケモン
   * @param kawaigariData  汚れが付いて良いかのデータ
   */
  //---------------------------------------------------------------------------
  static void RemoveIllegalDirt( BTL_POKEPARAM* poke, PokeTool::KawaigariParamCareCoreManager* kawaigariData );





private:

  static void GetOteireTargetInBattleField( bool* isFound, u8* oteireTargetID, const MainModule* mainModule, const POKECON* pokecon, u8 myClientId );
  static void GetOteireTargetInParty( bool* isFound, u8* oteireTargetID, const MainModule* mainModule, const POKECON* pokecon, u8 myClientId );
  static bool IsOteireTarget_Sick( const BTL_POKEPARAM& poke );
  static bool IsOteireTarget_Dirt( const BTL_POKEPARAM& poke );
  static bool IsIllegalDirt( const BTL_POKEPARAM& poke, PokeTool::KawaigariParamCareCoreManager* kawaigariData );

};



GFL_NAMESPACE_END( btl )

#endif // BTL_OTEIRE_H_INCLUDED