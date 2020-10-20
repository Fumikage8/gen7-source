//=============================================================================
/**
 * @file   ClientPublicInformation.cpp
 * @date   2015/07/28 15:59:02
 * @author obata_toshihiro
 * @brief  他クライアントに開示した、１クライアントの情報
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ClientPublicInformation.h"

#include "./btl_pokecon.h"
#include "./btl_pokeparam.h"


GFL_NAMESPACE_BEGIN( btl )


  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param pokecon  関連付けるPOKECON
   */
  //---------------------------------------------------------------------------
  ClientPublicInformation::ClientPublicInformation( const POKECON* pokecon ) : 
    m_pokecon( pokecon ),
    m_party()
  {
    this->InitPartyInformation( &m_party );
  }

  /**
   * @brief パーティの情報を初期化する
   * @param party  初期化対象のパーティ情報
   */
  void ClientPublicInformation::InitPartyInformation( Party* party )
  {
    for( u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i )
    {
      party->member[i].battlePokeId            = BTL_POKEID_NULL;
      party->member[i].monsno                  = MONSNO_NULL;
      party->member[i].formno                  = 0;
      party->member[i].sex                     = pml::SEX_UNKNOWN;
      party->member[i].level                   = 1;
      party->member[i].haveItem                = false;
      party->member[i].isAppearedOnBattleField = false;
    }
  }

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  ClientPublicInformation::~ClientPublicInformation()
  {
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンの戦闘ポケIDを取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   * @retval BTL_POKEID_NULL  バトルメンバーに選出されていない場合
   */
  //-------------------------------------------------------------------------
  u8 ClientPublicInformation::GetPokemonID( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return BTL_POKEID_NULL;
    }
    return pokemon->battlePokeId;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンのモンスターナンバーを取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   * @retval MONSNO_NULL  指定したインデックスにポケモンが存在しない場合
   */
  //-------------------------------------------------------------------------
  MonsNo ClientPublicInformation::GetPokemonMonsterNo( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return MONSNO_NULL;
    }
    return pokemon->monsno;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンのフォルムーナンバーを取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   */
  //-------------------------------------------------------------------------
  FormNo ClientPublicInformation::GetPokemonFormNo( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return 0;
    }
    return pokemon->formno;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンの性別を取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   */
  //-------------------------------------------------------------------------
  Sex ClientPublicInformation::GetPokemonSex( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return pml::SEX_UNKNOWN;
    }
    return pokemon->sex;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンのレベルを取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   */
  //-------------------------------------------------------------------------
  u8 ClientPublicInformation::GetPokemonLevel( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return 0;
    }
    return pokemon->level;
  }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したポケモンの状態異常を取得する
     * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
     */
    //-------------------------------------------------------------------------
    pml::pokepara::Sick ClientPublicInformation::GetPokemonSick( u8 memberIndex ) const
    {
      u8 pokeID = this->GetPokemonID( memberIndex );
      if( pokeID == BTL_POKEID_NULL ) {
        return pml::pokepara::SICK_NULL;
      }
      const BTL_POKEPARAM* pokePara = m_pokecon->GetPokeParamConst( pokeID );
      return pokePara->GetPokeSick();
    }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンが道具を持っているかどうかを取得する
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   */
  //-------------------------------------------------------------------------
  bool ClientPublicInformation::HavePokemonItem( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return 0;
    }
    return pokemon->haveItem;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 指定したポケモンが戦闘の場に出たことがあるか？
   * @param memberIndex  チェック対象ポケモンのパーティ内インデックス
   * @retval true   指定したポケモンは１回でも場に出たことがある
   * @retval false  指定したポケモンは１回も場に出たことがない
   */
  //---------------------------------------------------------------------------
  bool ClientPublicInformation::IsPokemonAppearedOnBattleField( u8 memberIndex ) const
  {
    const Pokemon* pokemon = this->GetPublishedPokemonByMemberIndexConst( memberIndex );
    if( pokemon == NULL ) {
      return false;
    }
    return pokemon->isAppearedOnBattleField;
  }

  /**
   * @brief 他クライアントに公開済みのポケモンの情報を取得する
   * @param memberIndex  取得対象ポケモンのパーティ内インデックス
   * @retval NULL  指定したIDのポケモンが存在しない( あるいは未公開である )場合
   */
  ClientPublicInformation::Pokemon* ClientPublicInformation::GetPublishedPokemonByMemberIndex( u8 memberIndex )
  {
    if( BTL_PARTY_MEMBER_MAX <= memberIndex )
    {
      GFL_ASSERT(0);
      return NULL;
    }

    Pokemon* pokemon = &( m_party.member[ memberIndex ] );

    if( pokemon->monsno == MONSNO_NULL )
    {
      return NULL;
    }

    return pokemon;
  }

  /**
   * @brief 他クライアントに公開済みのポケモンの情報を取得する
   * @param memberIndex  取得対象ポケモンのパーティ内インデックス
   * @retval NULL  指定したIDのポケモンが存在しない( あるいは未公開である )場合
   */
  const ClientPublicInformation::Pokemon* ClientPublicInformation::GetPublishedPokemonByMemberIndexConst( u8 memberIndex ) const
  {
    if( BTL_PARTY_MEMBER_MAX <= memberIndex ) 
    {
      GFL_ASSERT(0);
      return NULL;
    }

    const Pokemon* pokemon = &( m_party.member[ memberIndex ] );

    if( pokemon->monsno == MONSNO_NULL )
    {
      return NULL;
    }

    return pokemon;
  }

  //---------------------------------------------------------------------------
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
  //---------------------------------------------------------------------------
  void ClientPublicInformation::PublishPokemon( u8 memberIndex, u8 battlePokeId, MonsNo monsno, FormNo formno, Sex sex, u8 level, bool haveItem )
  {
    if( ( BTL_PARTY_MEMBER_MAX <= memberIndex ) ||
        ( m_party.member[ memberIndex ].battlePokeId != BTL_POKEID_NULL ) )
    {
      GFL_ASSERT(0);
      return;
    }

    m_party.member[ memberIndex ].battlePokeId = battlePokeId;
    m_party.member[ memberIndex ].monsno = monsno;
    m_party.member[ memberIndex ].formno = formno;
    m_party.member[ memberIndex ].sex = sex;
    m_party.member[ memberIndex ].level = level;
    m_party.member[ memberIndex ].haveItem = haveItem;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンが戦闘の場に出たことを記録する
   * @param battlePokeId  場に出たポケモンの戦闘ポケID
   */
  //---------------------------------------------------------------------------
  void ClientPublicInformation::SetPokemonAppearedOnBattleField( u8 battlePokeId )
  {
    Pokemon* pokemon = this->GetPublishedPokemonByBattlePokeId( battlePokeId );
    if( pokemon == NULL ) 
    {
      return;
    }
    pokemon->isAppearedOnBattleField = true;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンが道具を持っているかどうかを設定する
   * @param battlePokeId  ポケモンの戦闘ポケID
   * @param haveItem      道具を持っているかどうか？
   */
  //---------------------------------------------------------------------------
  void ClientPublicInformation::SetPokemonHaveItem( u8 battlePokeId, bool haveItem )
  {
    Pokemon* pokemon = this->GetPublishedPokemonByBattlePokeId( battlePokeId );
    if( pokemon == NULL ) 
    {
      return;
    }
    pokemon->haveItem = haveItem;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンのフォルム番号を設定する
   * @param battlePokeId  ポケモンの戦闘ポケID
   * @param formno        設定するフォルム番号
   */
  //---------------------------------------------------------------------------
  void ClientPublicInformation::SetPokemonFormNo( u8 battlePokeId, FormNo formno )
  {
    Pokemon* pokemon = this->GetPublishedPokemonByBattlePokeId( battlePokeId );
    if( pokemon == NULL ) 
    {
      return;
    }
    pokemon->formno = formno;
  }

  /**
   * @brief 他クライアントに公開済みのポケモンの情報を取得する
   * @param battlePokeId  取得対象ポケモンの戦闘ポケID
   * @retval NULL  指定したIDのポケモンが存在しない( あるいは未公開である )場合
   */
  ClientPublicInformation::Pokemon* ClientPublicInformation::GetPublishedPokemonByBattlePokeId( u8 battlePokeId )
  {
    for( u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i )
    {
      if( m_party.member[i].battlePokeId == battlePokeId )
      {
        return &( m_party.member[i] );
      }
    }
    return NULL;
  }

  /**
   * @brief 他クライアントに公開済みのポケモンの情報を取得する
   * @param battlePokeId  取得対象ポケモンの戦闘ポケID
   * @retval NULL  指定したIDのポケモンが存在しない( あるいは未公開である )場合
   */
  const ClientPublicInformation::Pokemon* ClientPublicInformation::GetPublishedPokemonByBattlePokeIdConst( u8 battlePokeId ) const
  {
    for( u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i )
    {
      if( m_party.member[i].battlePokeId == battlePokeId )
      {
        return &( m_party.member[i] );
      }
    }
    return NULL;
  }


GFL_NAMESPACE_END( btl )