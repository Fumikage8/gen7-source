#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMatchMakingRule.cpp
 *
 */
//=============================================================================

#include <gflnet2/include/nex/gflnet2_NexMatchMakingRule.h>



namespace gflnet2{
namespace nex{



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
NexMatchMakingRule::NexMatchMakingRule( u32 index, u32 baseValue, u32 valueMin, u32 valueMax , VALUE_TYPE type ) : node()
{
  m_index     = index;
  m_baseValue = baseValue;
  m_valueMin  = valueMin;
  m_valueMax  = valueMax;
  m_valueType = type;
}

NexMatchMakingRule::NexMatchMakingRule( void ) : node()
{
  m_index     = 0;
  m_baseValue = 0;
  m_valueMin  = 0;
  m_valueMax  = 0;
  m_valueType = VALUE_TYPE_BOTH;
}

//------------------------------------------------------------------
/**
* @brief 条件設定インデックスを取得する
*
* @return 条件設定インデックス値
*/
//------------------------------------------------------------------
u32  NexMatchMakingRule::GetIndex( void ) const
{
  return m_index;
}

//------------------------------------------------------------------
/**
* @brief 検索ベース値を取得する
*
* @return 検索ベース値
*/
//------------------------------------------------------------------
u32  NexMatchMakingRule::GetBaseValue( void ) const
{
  return m_baseValue;
}

//------------------------------------------------------------------
/**
* @brief 検索条件が範囲指定であるか判定
*
* @return 検索条件が範囲指定であるならtrueを返却
*/
//------------------------------------------------------------------
bool  NexMatchMakingRule::IsRangeValue( void ) const
{
  return (m_valueMin != m_valueMax) || (m_valueMax != m_baseValue);
}

//------------------------------------------------------------------
/**
* @brief 検索範囲指定値の最小値を取得する
*
* @return 検索範囲指定値の最小値
*/
//------------------------------------------------------------------
u32  NexMatchMakingRule::GetRangeMinValue( void ) const
{
  return m_valueMin;
}

//------------------------------------------------------------------
/**
* @brief 検索範囲指定値の最大値を取得する
*
* @return 検索範囲指定値の最大値
*/
//------------------------------------------------------------------
u32  NexMatchMakingRule::GetRangeMaxValue( void ) const
{
  return m_valueMax;
}

//------------------------------------------------------------------
/**
* @brief 値のタイプを取得する
*
* @return 値のタイプ
*/
//------------------------------------------------------------------
gflnet2::nex::NexMatchMakingRule::VALUE_TYPE  NexMatchMakingRule::GetValueType( void ) const
{
  return m_valueType;
}

//------------------------------------------------------------------
/**
 * @brief 検索条件のコピー
 */
//------------------------------------------------------------------
void NexMatchMakingRule::Copy( const NexMatchMakingRule& rule )
{
  m_index     = rule.m_index;
  m_baseValue = rule.m_baseValue;
  m_valueMin  = rule.m_valueMin;
  m_valueMax  = rule.m_valueMax;
  m_valueType = rule.m_valueType;
}



} // nex
} // gflnet

#endif
