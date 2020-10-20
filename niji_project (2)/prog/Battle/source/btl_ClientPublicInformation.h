//=============================================================================
/**
 * @file   btl_ClientPublicInformation.h
 * @date   2015/07/28 15:59:02
 * @author obata_toshihiro
 * @brief  他クライアントに開示した、１クライアントの情報
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_CLIENTPUBLICINFORMATION_H_INCLUDED
#define BTL_CLIENTPUBLICINFORMATION_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_const.h"


GFL_NAMESPACE_BEGIN( btl )

  class POKECON;

  
  /**
   * @class ClientPublicInformation
   * @brief 他クライアントに開示した、１クライアントの情報
   */
  class ClientPublicInformation
  {
    GFL_FORBID_COPY_AND_ASSIGN( ClientPublicInformation );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param pokecon  関連付けるPOKECON
     */
    //-------------------------------------------------------------------------
    ClientPublicInformation( const POKECON* pokecon );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~ClientPublicInformation();



    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンの戦闘ポケIDを取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     * @retval BTL_POKEID_NULL  バトルメンバーに選出されていない場合
     */
    //-------------------------------------------------------------------------
    u8 GetPokemonID( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンのモンスターナンバーを取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     * @retval MONSNO_NULL  指定したインデックスにポケモンが存在しない場合
     */
    //-------------------------------------------------------------------------
    MonsNo GetPokemonMonsterNo( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンのフォルムーナンバーを取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    FormNo GetPokemonFormNo( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンの性別を取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    Sex GetPokemonSex( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンのレベルを取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    u8 GetPokemonLevel( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンの状態異常を取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    pml::pokepara::Sick GetPokemonSick( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンが道具を持っているかどうかを取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    bool HavePokemonItem( u8 memberIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンが戦闘の場に出たことがあるか？
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     * @retval true   指定したポケモンは１回でも場に出たことがある
     * @retval false  指定したポケモンは１回も場に出たことがない
     */
    //-------------------------------------------------------------------------
    bool IsPokemonAppearedOnBattleField( u8 memberIndex ) const;



    //-------------------------------------------------------------------------
    /**
     * @brief パーティのメンバーを公開する
     * @param memberIndex   公開するポケモンのパーティ内インデックス
     * @param battlePokeId  公開するポケモンの戦闘ポケID( バトルメンバーに選出されていない場合 BTL_POKEID_NULL )
     * @param monsno        公開するポケモンのモンスターナンバー
     * @param formno        公開するポケモンのフォルムナンバー
     * @param sex           公開するポケモンの性別
     * @param level         公開するポケモンのレベル
     * @param haveItem      公開するポケモンが道具を持っているか？
     */
    //-------------------------------------------------------------------------
    void PublishPokemon( u8 memberIndex, u8 battlePokeId, MonsNo monsno, FormNo formno, Sex sex, u8 level, bool haveItem );

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンが戦闘の場に出たことを記録する
     * @param battlePokeId  場に出たポケモンの戦闘ポケID
     */
    //-------------------------------------------------------------------------
    void SetPokemonAppearedOnBattleField( u8 battlePokeId );

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンが道具を持っているかどうかを設定する
     * @param battlePokeId  ポケモンの戦闘ポケID
     * @param haveItem      道具を持っているかどうか？
     */
    //-------------------------------------------------------------------------
    void SetPokemonHaveItem( u8 battlePokeId, bool haveItem );

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンのフォルム番号を設定する
     * @param battlePokeId  ポケモンの戦闘ポケID
     * @param formno        設定するフォルム番号
     */
    //-------------------------------------------------------------------------
    void SetPokemonFormNo( u8 battlePokeId, FormNo formno );



  private:

    /**
     * @brief ポケモン毎の情報
     */
    struct Pokemon
    {
      u8     battlePokeId;             // 戦闘ポケモンのID
      MonsNo monsno;                   // モンスターNo.
      FormNo formno;                   // フォルムNo.
      Sex    sex;                      // 性別
      u8     level;                    // レベル
      bool   haveItem;                 // 道具を持っているか？
      bool   isAppearedOnBattleField;  // 戦闘の場に出たか？
    };

    /**
     * @brief パーティの情報
     */
    struct Party
    {
      Pokemon member[ BTL_PARTY_MEMBER_MAX ]; // メンバーのポケモン( 見せ合いしたパーティの並び順 )
    };



  private:

    void InitPartyInformation( Party* party );
    Pokemon* GetPublishedPokemonByMemberIndex( u8 memberIndex );
    const Pokemon* GetPublishedPokemonByMemberIndexConst( u8 memberIndex ) const;
    Pokemon* GetPublishedPokemonByBattlePokeId( u8 battlePokeId );
    const Pokemon* GetPublishedPokemonByBattlePokeIdConst( u8 battlePokeId ) const;
    

  private:

    const POKECON* m_pokecon;
    Party m_party;

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_CLIENTPUBLICINFORMATION_H_INCLUDED