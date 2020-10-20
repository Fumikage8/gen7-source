//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldStaticModelAccessor.h
 *	@brief  STATIC_MODELイベント配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldStaticModelAccessor.h"

GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @biref  コンストラクタ
 */
FieldStaticModelAccessor::FieldStaticModelAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldStaticModelAccessor::~FieldStaticModelAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldStaticModelAccessor::Initialize( const void * data )
{
  m_pData = static_cast<const StaticModelDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldStaticModelAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief エイリアスのデータ数
 */
u32 FieldStaticModelAccessor::GetAliasDataMax() const
{
  u32 count = 0;

  for( int i=0; i<m_pData->num; ++i )
  {
    if( m_pData->data[i].alias )
    {
      count ++;
    }
  }

  return count;
}

/**
 *  @brief エイリアスでないデータ数
 */
u32 FieldStaticModelAccessor::GetNotAliasDataMax() const
{
  u32 alias = GetAliasDataMax();

  return GetDataMax() - alias;
}

/**
 *  @brief 情報取得
 */
const FieldStaticModelAccessor::StaticModelData * FieldStaticModelAccessor::GetData( u32 idx ) const
{
  if( m_pData->num > idx ){
    return &m_pData->data[ idx ];
  }

  return NULL;
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * FieldStaticModelAccessor::GetCollisionData( const FieldStaticModelAccessor::StaticModelData & data ) const
{
  if( data.collisionOffset == 0)
  {
    return NULL;
  }
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}



GFL_NAMESPACE_END(Field);

