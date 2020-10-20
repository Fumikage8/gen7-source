#if !defined( __BTL_DEADREC_H__ )
#define __BTL_DEADREC_H__
//=============================================================================================
/**
 * @file    btl_DeadRec.h
 * @brief   ポケモンXY 死亡ポケ記録
 * @author  taya
 *
 * @date  2011.01.06 作成
 */
//=============================================================================================
#pragma once

#include "./btl_common.h"
#include "./btl_pokeCon.h"
#include "./btl_posPoke.h"



GFL_NAMESPACE_BEGIN(btl)
class DeadRec {
  GFL_FORBID_COPY_AND_ASSIGN( DeadRec );

public:
  DeadRec(void){ };
  ~DeadRec(){ };

  void     Init( const POKECON* pokeCon );
  void     StartTurn( void );
  void     Add( u8 pokeID );
  void     Relive( u8 pokeID );
  void     SetExpCheckedFlag( u8 turn, u8 idx );
  u8       GetCount( u8 turn ) const;
  u8       GetPokeID( u8 turn, u8 idx ) const;
  bool     GetExpCheckedFlag( u8 turn, u8 idx ) const;
  u8       GetLastDeadPokeID( void ) const;

  bool     IsDeadNow( u8 pokeID ) const;
  u32      GetDeadCountByClientID( u8 clientID ) const;
  bool     IsRelivedPokePuttableEmptyPos( const PosPoke& posPoke ) const;



private:
  /**
   *  定数
   */
  enum {
    TURN_MAX = 4,   ///< 記録ターン数
    DEAD_COUNT_MAX = 100, ///< 死んだ回数のカウント最大値（ポケモンあたり）
  };

  /**
   *  ポケ死亡レコード（ターン単位）
   */
  typedef struct {
    u8 cnt;
    u8 fExpChecked[ BTL_POKEID_MAX ];    ///< 経験値チェック処理済みフラグ（ポケモンID毎）
    u8 deadPokeID[ BTL_POKEID_MAX ];
  }Unit;

  //--- members -------------------------------------
  Unit            m_turnRecord[ TURN_MAX ];
  u8              m_currentDeadFlag[ BTL_POKEID_MAX ];  ///< 現在死んでいるフラグ
  u8              m_deadCount[ BTL_POKEID_MAX ];        ///< 死んだ回数
  u8              m_relivePokeID[ BTL_POKEID_MAX ];     ///< 現ターン中に生き返ったポケモンID
  u8              m_relivePokeCount;                    ///< 現ターン中に生き返ったポケモン数

  //--- private methods -----------------------------
  void  clearTurnRecord( Unit* unit );
  void  clearReliveRecord( void );
  bool  isRelivedThisTurn( u8 pokeID ) const;


};  // class DeadRec
GFL_NAMESPACE_END(btl)

#endif // __BTL_DEADREC_H__
