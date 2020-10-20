#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMatchingRule.h
 *
 *  @brief MatchMakingClient検索条件用クラス
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MATCHING_RULE_H__
#define __GFLNET_NEX_MATCHING_RULE_H__
#pragma once


#include <nex.h>
#include <nex_P2p.h>

#include <nn/friends.h>

#include <nw/ut.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/**
*  @brief MatchMakingClient検索条件用クラス
*/
//------------------------------------------------------------------
class NexMatchMakingRule
{

public:

  enum VALUE_TYPE
  {
    VALUE_TYPE_BOTH = 0,
    VALUE_TYPE_CREATE_ONLY,
    VALUE_TYPE_SEARCH_ONRY
  };

  static const u32 RULE_MAX_NUM = (nn::nex::NUM_MATCHMAKE_SESSION_ATTRIBUTES - 1);   //!< マッチング条件MAX(=5)
public:
  //------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param index      条件設定インデックス(0〜4)
   * @param baseValue  検索ベース値。検索条件の基本値として使用される。
   * @param valueMin   検索範囲最少値。検索範囲を指定しない場合はbaseValueと同じ値を指定すること。
   * @param valueMax   検索範囲最大値。検索範囲を指定しない場合はbaseValueと同じ値を指定すること。
   *
   * @note  (baseValue==valueMin==valueMax)の場合は、検索条件は「値が等しいこと」として解釈される。
   */
  //------------------------------------------------------------------
  NexMatchMakingRule( u32 index, u32 baseValue, u32 valueMin, u32 valueMax , VALUE_TYPE type = VALUE_TYPE_BOTH );

  NexMatchMakingRule( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief 条件設定インデックスを取得する
  *
  * @return 条件設定インデックス値
  */
  //------------------------------------------------------------------
  u32     GetIndex( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 検索条件が範囲指定であるか判定
  *
  * @return 検索条件が範囲指定であるならtrueを返却
  */
  //------------------------------------------------------------------
  u32     GetBaseValue( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 検索範囲指定値の最小値を取得する
  *
  * @return 検索範囲指定値の最小値
  */
  //------------------------------------------------------------------
  bool    IsRangeValue( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 検索範囲指定値の最小値を取得する
  *
  * @return 検索範囲指定値の最小値
  */
  //------------------------------------------------------------------
  u32     GetRangeMinValue( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 検索範囲指定値の最大値を取得する
  *
  * @return 検索範囲指定値の最大値
  */
  //------------------------------------------------------------------
  u32     GetRangeMaxValue( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 値のタイプを取得する
  *
  * @return 値のタイプ
  */
  //------------------------------------------------------------------
  gflnet2::nex::NexMatchMakingRule::VALUE_TYPE  GetValueType( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief 検索条件のコピー
  *
  * @param rule コピー元となるルール
  */
  //------------------------------------------------------------------
  void    Copy( const NexMatchMakingRule& rule );

private:
  u32     m_index;      //!< 条件設定インデックス
  u32     m_baseValue;  //!< 検索ベース値
  u32     m_valueMin;   //!< 検索範囲最少値
  u32     m_valueMax;   //!< 検索範囲最大値

  VALUE_TYPE m_valueType;//!< 値のタイプ
public:
  nw::ut::LinkListNode node;   //!< List Node
};


} // nex
} // gflnet


#endif  // __GFLNET_NEX_MATCHING_RULE_H__
#endif
