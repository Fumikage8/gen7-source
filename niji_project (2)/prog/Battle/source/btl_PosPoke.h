#if !defined( __BTL_POSPOKE_H__ )
#define __BTL_POSPOKE_H__
//=============================================================================================
/**
 * @file    btl_PosPoke.h
 * @brief   ポケモンXY バトルシステム  場にいるポケモンの生存確認用構造体と処理ルーチン
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_PokeCon.h"

GFL_NAMESPACE_BEGIN(btl)


class MainModule;

class PosPoke {
  GFL_FORBID_COPY_AND_ASSIGN( PosPoke );

public:
  PosPoke( void );
  ~PosPoke(){};

  void        Init( const MainModule* mainModule, const POKECON* pokeCon );
  void        ExtendPos( const MainModule& mainModule, BtlPokePos pos );
  void        PokeIn( const MainModule* mainModule, BtlPokePos pos,  u8 pokeID, POKECON* pokeCon );
  void        PokeOut( u8 pokeID );
  void        Swap( BtlPokePos pos1, BtlPokePos pos2 );

  u8          GetClientEmptyPos( u8 clientID, u8* pos ) const;
  u8          GetClientEmptyPosCount( u8 clientID ) const;
  bool        IsExist( u8 pokeID ) const ;
  bool        IsExistFrontPos( const MainModule* mainModule, u8 pokeID ) const;
  BtlPokePos  GetPokeExistPos( u8 pokeID ) const;
  BtlPokePos  GetPokeLastPos( u8 pokeID ) const;
  u8          GetExistPokeID( BtlPokePos pos ) const;


private:

  /**
   * 状態保持構造体
   */
  typedef struct {
    u8  fEnable;
    u8  clientID;
    u8  existPokeID;
  }State;

  State                m_state[ BTL_POS_NUM ];            ///< 位置ごとの状態

  BtlPokePos           m_lastPosInst[ BTL_POKEID_MAX ];   ///< ポケモンごとの最終位置（死んだり交替したりした時にも消えない）
  SafeAry<BtlPokePos>  m_lastPos;

  // --- private methods -------
  void   setInitialFrontPokemon( const MainModule* mainModule, const POKECON* pokeCon, BtlPokePos pos );
  void   checkConfrontRec( const MainModule* mainModule, BtlPokePos pos, POKECON* pokeCon );
  void   updateLastPos( BtlPokePos pos );


}; // class PosPoke

GFL_NAMESPACE_END(btl)

#endif // __BTL_POSPOKE_H__
