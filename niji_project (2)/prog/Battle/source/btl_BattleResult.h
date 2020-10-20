//=============================================================================
/**
 * @file   btl_BattleResult.h
 * @date   2015/08/24 11:45:12
 * @author obata_toshihiro
 * @brief  戦闘結果の構築
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_BATTLERESULT_H_INCLUDED
#define BTL_BATTLERESULT_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_SetupParam.h>
#include "./btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class POKECON;
  class ZenryokuWazaStatus;


  /**
   * @class BattleResult
   * @brief 戦闘結果を BATTLE_SETUP_PARAM に格納する
   */
  class BattleResult
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleResult );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief バトル内パーティデータを、結果としてセットアップパラメータに書き戻す
     *
     * @param[in,out] setupParam        結果の出力先
     * @param[in,out] pokeconForServer  サーバー側の POKECON
     * @param[in,out] pokeconForClient  クライアント側の POKECON
     * @param         mainModule        参照する MainModule
     * @param         myClientId        プレイヤのクライアントID
     */
    //-------------------------------------------------------------------------
    static void ApplyBattlePartyData( 
      BATTLE_SETUP_PARAM* setupParam, 
      POKECON*            pokeconForServer, 
      POKECON*            pokeconForClient, 
      const MainModule*   mainModule,
      u8                  myClientId );

    //-------------------------------------------------------------------------
    /**
     * @brief バトルロイヤルの結果を、セットアップパラメータに書き戻す
     *
     * @param[in,out] setupParam   結果の出力先
     * @param         mainModule   参照する MainModule
     * @param         pokecon      参照する POKECON
     * @param         myClientId   プレイヤのクライアントID
     */
    //-------------------------------------------------------------------------
    static void ApplyBattleRoyalResult( 
      BATTLE_SETUP_PARAM* setupParam, 
      const MainModule*   mainModule,
      const POKECON*      pokecon,
      u8                  myClientId );

    //-------------------------------------------------------------------------
    /**
     * @brief ぜんりょくパワー関連の結果を、セットアップパラメータに書き戻す
     *
     * @param[out] setupParam                結果の出力先
     * @param      playerZenryokuWazaStatus  プレイヤの全力技ステータス管理オブジェクト
     */
    //-------------------------------------------------------------------------
    static void ApplyZenryokuPowerResult( 
      BATTLE_SETUP_PARAM*       setupParam, 
      const ZenryokuWazaStatus& playerZenryokuWazaStatus );

    //-------------------------------------------------------------------------
    /**
     * @brief お手入れ対象となるポケモンを、セットアップパラメータに書き戻す
     *
     * @param[out] setupParam   結果の出力先
     * @param      mainModule   参照する MainModule
     * @param      pokecon      参照する POKECON
     * @param      myClientId   プレイヤのクライアントID
     */
    //-------------------------------------------------------------------------
    static void ApplyOteireTarget(
      BATTLE_SETUP_PARAM* setupParam, 
      const MainModule*   mainModule,
      const POKECON*      pokecon,
      u8                  myClientId );

    //-------------------------------------------------------------------------
    /**
     * @brief バトル録画のヘッダ情報を、セットアップパラメータに格納する
     *
     * @param[out] setupParam   結果の出力先
     * @param      mainModule   参照する MainModule
     * @param      myClientId   プレイヤのクライアントID
     * @param      result       バトルの結果
     */
    //-------------------------------------------------------------------------
    static void ApplyRecordHeader(
      BATTLE_SETUP_PARAM* setupParam,
      const MainModule*   mainModule,
      u8                  myClientId,
      BtlResult           result );


  private:

    static void srcParty_FormReset_OnBattleEnd( PokeParty* party, const PokeParty& orgParty );
    static void srcParty_FormReset_OnBattleEnd( pml::pokepara::PokemonParam* pokeParam, const pml::pokepara::PokemonParam& orgParam );
    static void clearUnknownUBNickName( const MainModule* mainModule, PokeParty* party );
    static void ApplyBattleRoyalResult_Ranking( BattleRoyalResult* result, const MainModule* mainModule, u8 myClientId );
    static void ApplyBattleRoyalResult_KillCount( BattleRoyalResult* result, const POKECON* pokecon );
    static void ApplyBattleRoyalResult_Hp( BattleRoyalResult* result, const POKECON* pokecon );
    static void ApplyBattleRoyalResult_LastPoke( BattleRoyalResult* result, const POKECON* pokecon );
    static u32 GetRecTurnCount( const void* recordData, u32 recordDataSize );
    static BtlRecordResult1 GetRecResult1( BtlResult result );
    static BtlRecordResult2 GetRecResult2( const MainModule* mainModule, BtlResult result );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_BATTLERESULT_H_INCLUDED