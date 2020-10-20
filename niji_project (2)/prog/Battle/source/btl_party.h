#if !defined( __BTL_PARTY_H__ )
#define __BTL_PARTY_H__
//=============================================================================================
/**
 * @file  btl_party.h
 * @brief ポケモンXY バトルシステム ポケモンパーティクラス
 * @author  taya
 *
 * @date  2010.12.18  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_pokeparam.h"

GFL_NAMESPACE_BEGIN(btl)


class BTL_PARTY {
  GFL_FORBID_COPY_AND_ASSIGN( BTL_PARTY );

public:
  BTL_PARTY();
  ~BTL_PARTY();


  /**
   * 内容を初期化（構築後には呼ぶ必要なし）
   */
  void Initialize( void );

  /**
   * メンバー１体追加
   *
   * @param[in]   member    追加するメンバーポインタ
   */
  void AddMember( BTL_POKEPARAM* member );

  /**
   * 戦える（生きていてタマゴでない）ポケモンを先頭に移動（初期化後に１度だけ行う）
   */
  void MoveAlivePokeToFirst( void );

  /**
   *  メンバーを最後尾に移動（初期化時に戦えないポケモンに適用)
   */
  void MoveLastMember( u8 idx );

  /**
   * メンバー数を返す
   *
   * @retval  u8  メンバー数
   */
  u8 GetMemberCount( void ) const;

  /**
   *  戦えるメンバー数を返す
   *
   * @retval  u8  戦えるメンバー数
   */
  u8 GetAliveMemberCount( void ) const;

  /**
   * 指定Index 以降の戦えるメンバー数を返す
   *
   * @param   startIdx
   *
   * @retval  u8  startIdx 以降の戦えるメンバー数を返す
   */
  u8 GetAliveMemberCountRear( u8 startIdx ) const;

  /**
   *  ひん死のメンバー数を返す
   *
   *  @retval   ひん死のメンバー数
   */
  u8 GetDeadMemberCount( void ) const;

  /**
   * @brief メンバー数が最大か？
   * @retval true   メンバー数が最大
   * @retval false  メンバー数が最大でない
   */
  bool IsFull( void ) const;

  /**
   * メンバーIndex からメンバーデータポインタ取得
   *
   * @param   idx メンバーIndex
   *
   * @retval  BTL_POKEPARAM*    メンバーデータポインタ
   */
  BTL_POKEPARAM* GetMemberData( u8 idx );


  /**
   * メンバーIndex からメンバーデータポインタ取得（const版）
   *
   * @param   idx
   *
   * @retval  BTL_POKEPARAM*
   */
  const BTL_POKEPARAM* GetMemberDataConst( u8 idx ) const;

  /**
   * メンバー入れ替え
   *
   * @param   idx1
   * @param   idx2
   */
  void SwapMembers( u8 idx1, u8 idx2 );

  /**
   * 特定ポケモンのパーティ内Indexを返す（データポインタ指定）
   *
   * @param   party
   * @param   pokeID
   *
   * @retval  int   パーティ内Index（パーティに指定IDのポケモンが存在しない場合 -1）
   */
  int FindMember( const BTL_POKEPARAM* param ) const;

  /**
   * 特定ポケモンのパーティ内Indexを返す（PokeID指定）
   *
   * @param   party
   * @param   pokeID
   *
   * @retval  int   パーティ内Index（パーティに指定IDのポケモンが存在しない場合 -1）
   */
  int FindMemberByPokeID( u8 pokeID ) const;

  /**
   * 生きていて戦える先頭のポケモンデータを返す
   *
   * @param   party
   *
   * @retval  int   パーティ内Index（存在しない場合 NULL）
   */
  BTL_POKEPARAM* GetAliveTopMember( void );

  /**
   * メンバーのIndexを返す
   * @param pokeID
   * @return メンバーIndex（メンバーに含まれていない時 BTL_PARTY_MEMBER_MAX）
   */
  u8 GetMemberIndex( u8 pokeID ) const;

  /**
   * @brief 特性「イリュージョン」の対象となるポケモンのインデックスを返す
   */
  u8 GetIllusionTargetMemberIndex( void ) const;

  /**
   * @brief 倒したポケモンの数の合計を取得する
   */
  u16 GetTotalKillCount( void ) const;


private:
  BTL_POKEPARAM*  m_pMember[ BTL_PARTY_MEMBER_MAX ];
  u8  m_memberCount;
  u8  m_numCoverPos;
};

GFL_NAMESPACE_END(btl)



#endif // __BTL_PARTY_H__
