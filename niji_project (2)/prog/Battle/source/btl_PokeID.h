//=============================================================================
/**
 * @file   btl_PokeID.h
 * @date   2015/10/15 11:36:48
 * @author obata_toshihiro
 * @brief  ポケIDの定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_POKEID_H_INCLUDED
#define BTL_POKEID_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h> // for BTL_CLIENT_ID

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class PokeID
   * @brief バトルに登場する全てのポケモンを一意に識別する値
   */
  class PokeID
  {
  public:

    /**
     * @brief 指定クライアントのポケIDの最小値を取得する
     * @param clientID  クライアントID
     */
    static u8 GetClientBasePokeId( BTL_CLIENT_ID clientID );

    /**
     * @brief 指定クライアントのポケIDを取得する
     * @param clientID   クライアントID
     * @param pokeIndex  ポケモンのインデックス
     */
    static u8 GetClientPokeId( BTL_CLIENT_ID clientID, u8 pokeIndex );

    /**
     * @brief ポケID　→　戦闘開始時のパーティ内インデックス
     */
    static u8 PokeIdToStartMemberIndex( u8 pokeID );

    /**
     * @brief ポケID　→　クライアントID
     */
    static BTL_CLIENT_ID PokeIdToClientId( u8 pokeID );

    /**
     * @brief ポケID　→　ショートポケID( 3bit )
     */
    static u8 PokeIdToShortId( u8 pokeID );

    /**
     * @brief クライアントID & ショートポケID( 3bit )　→　ポケID
     */
    static u8 ShortIdToPokeId( BTL_CLIENT_ID clientID, u8 shortID );




  private:

    // 実体化を禁止します
    PokeID( void ){}
    ~PokeID(){}

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_POKEID_H_INCLUDED