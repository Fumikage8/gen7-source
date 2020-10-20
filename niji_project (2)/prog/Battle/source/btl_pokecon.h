#if !defined( __BTL_POKECON_H__ )
#define __BTL_POKECON_H__
//=============================================================================================
/**
 * @file  btl_pokecon.h
 * @brief ポケモンXY バトルシステム ポケモンデータコンテナクラス
 * @author  taya
 *
 * @date  2010.12.18  作成
 */
//=============================================================================================
#pragma once


#include "./btl_common.h"
#include "./btl_party.h"
#include "./btl_field.h"
#include "./btl_SideEff.h"
#include "./btl_PosEff.h"

GFL_NAMESPACE_BEGIN(btl)

class MainModule;
class SideEffectManager;
class SideEffectStatus;
class PosEffectManager;
class PosEffectStatus;


class POKECON {
  GFL_FORBID_COPY_AND_ASSIGN( POKECON );

public:
  static  void InitSystem( HeapHandle heap );
  static  void QuitSystem( void );

  POKECON( HeapHandle heap );
  ~POKECON();

  void  Init( MainModule* mainModule, bool fForServer );
  bool  IsInitialized( void )  const;
  void  Clear( void );
  void  SetParty( u8 clientID, PokeParty* srcParty, const PartyParam& partyParam, const Savedata::MyStatus* playerData );
  bool  AddPokemon( u8 clientID, u8 pokeID, const pml::pokepara::PokemonParam& srcParam, const DefaultPowerUpDesc& defaultPowerUpDesc, const Savedata::MyStatus* playerData );
  bool  ReplacePokemon( u8 clientID, u8 pokeID, const pml::pokepara::PokemonParam& srcParam, const DefaultPowerUpDesc& defaultPowerUpDesc, const Savedata::MyStatus* playerData );

  BTL_PARTY*        GetPartyData( u32 clientID );
  const BTL_PARTY*  GetPartyDataConst( u32 clientID ) const;
  PokeParty*        GetSrcParty( u8 clientID );


  BTL_POKEPARAM*         GetClientPokeData( u8 clientID, u8 posIdx );
  const BTL_POKEPARAM*   GetClientPokeDataConst( u8 clientID, u8 memberIdx ) const;
  BTL_POKEPARAM*         GetPokeParam( u8 pokeID );
  const BTL_POKEPARAM*   GetPokeParamConst( u8 pokeID ) const;
  int                    FindPokemon( u8 clientID, u8 pokeID ) const;
  bool                   IsExistPokemon( u8 pokeID ) const;

  void          RefrectBtlParty( u8 clientID, bool fDefaultForm );
  void          RefrectBtlPartyStartOrder( u8 clientID, u8 orgPokeID, u8* fightPokeIdx );

  BTL_POKEPARAM*        GetFrontPokeData( BtlPokePos pos );
  const BTL_POKEPARAM*  GetFrontPokeDataConst( BtlPokePos pos ) const;

  FieldStatus*         GetFieldStatus( void );
  const FieldStatus*   GetFieldStatusConst( void ) const;

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの状態を取得する
   * @param side        取得したい陣営
   * @param sideEffect  取得したいサイドエフェクト
   * @retval NULL  指定したサイドエフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  SideEffectStatus* GetSideEffectStatus( BtlSide side, BtlSideEffect sideEffect );

  //-------------------------------------------------------------------------
  /**
   * @brief サイドエフェクトの状態を取得する
   * @param side        取得したい陣営
   * @param sideEffect  取得したいサイドエフェクト
   * @retval NULL  指定したサイドエフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  const SideEffectStatus* GetSideEffectStatusConst( BtlSide side, BtlSideEffect sideEffect ) const;

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトの状態を取得する
   * @param pos        取得したい位置
   * @param posEffect  取得したい位置エフェクト
   * @retval NULL  指定した位置エフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  PosEffectStatus* GetPosEffectStatus( BtlPokePos pos, BtlPosEffect posEffect );

  //-------------------------------------------------------------------------
  /**
   * @brief 位置エフェクトの状態を取得する
   * @param pos        取得したい位置
   * @param posEffect  取得したい位置エフェクト
   * @retval NULL  指定した位置エフェクトが存在しない場合
   */
  //-------------------------------------------------------------------------
  const PosEffectStatus* GetPosEffectStatusConst( BtlPokePos pos, BtlPosEffect posEffect ) const;


  /**
   * @brief 乱入により新たに追加されたポケモンIDを記録しておく（ServerFlowで対象ポケモンの追加Indexを調べるために使う）
   * @param[in]  pokeID   新たに追加されたポケモンのID
   */
  void  IntrudePokeID_Set( u8 pokeID );

  /**
   * @brief 乱入により新たに追加されたポケモンIDの記録を消去する（記録領域は１件分なのでClient, Server 双方不要になったタイミングで消去する）
   */
  void  IntrudePokeID_Clear( void );

  /**
   * @brief 乱入により新たに追加されたポケモンIDを取得する
   * @return  乱入により新たに追加されたポケモンID
   */
  u8    IntrudePokeID_Get( void ) const;


private:
  MainModule*        m_mainModule;
  BTL_PARTY          m_party[ BTL_CLIENT_MAX ];
  PokeParty*         m_srcParty[ BTL_CLIENT_MAX ];
  BTL_POKEPARAM*     m_activePokeParam[ BTL_POKEID_MAX ];
  BTL_POKEPARAM*     m_storedPokeParam[ BTL_POKEID_MAX ];
  bool               m_fForServer;
  FieldStatus        m_fieldStatus;
  SideEffectManager* m_sideEffectManager;
  PosEffectManager*  m_posEffectManager;
  u8                 m_latestIntrudePokeID;

  static PokeParty* s_tmpPokeParty;


private:
  void    updateIllusionTarget( u8 clientID );
  void    storeAllActivePokeParam( void );
  void    addPokeParam( u8 clientID, u8 pokeID, pml::pokepara::PokemonParam* srcParam, const DefaultPowerUpDesc& defaultPowerUpDesc, const Savedata::MyStatus* playerData );
  void    replacePokeParam( u8 clientID, u8 pokeID, pml::pokepara::PokemonParam* srcParam, const DefaultPowerUpDesc& defaultPowerUpDesc, const Savedata::MyStatus* playerData );
  void    setupPokeParam( BTL_POKEPARAM* pokeParam, u8 pokeID, pml::pokepara::PokemonParam* srcParam, const DefaultPowerUpDesc& defaultPowerUpDesc, const Savedata::MyStatus* playerData );

};

GFL_NAMESPACE_END(btl)
#endif // __BTL_POKECON_H__
