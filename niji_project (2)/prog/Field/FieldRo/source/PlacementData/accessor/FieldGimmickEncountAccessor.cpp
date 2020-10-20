//======================================================================
/**
 * @file FieldGimmickEncountAccessor.cpp
 * @date 2015/08/19 21:47:53
 * @author saita_kazuki
 * @brief ギミックエンカウント配置情報アクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

/**
*  @brief  コンストラクタ
*/
FieldGimmickEncountAccessor::FieldGimmickEncountAccessor( void )
  : m_pData( NULL)
{
}

/**
*  @brief  デストラクタ
*/
FieldGimmickEncountAccessor::~FieldGimmickEncountAccessor( void )
{
}

/**
*  @brief  初期化
*/
void FieldGimmickEncountAccessor::Initialize( const void *data )
{
  m_pData = static_cast<const Header*>( data);
}

/**
*  @brief  最大数取得
*/
u32 FieldGimmickEncountAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
*  @brief  情報取得
*/
const FieldGimmickEncountAccessor::Data* FieldGimmickEncountAccessor::GetData( u32 index ) const
{
  if( m_pData->num > index )
  {
    return &m_pData->data[ index ];
  }
  return NULL;
}

/**
*  @brief  エンカウント開始コリジョン情報の取得
*/
const void* FieldGimmickEncountAccessor::GetEncountCollisionData( const Data &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.encountCollisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
*  @brief  アクション実行コリジョンの取得
*/
const void* FieldGimmickEncountAccessor::GetActionCollisionData( const Data &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.actionCollisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
*  @brief  アクション実行コリジョンの取得(2つめ)
*/
const void* FieldGimmickEncountAccessor::GetActionCollisionData2( const Data &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.actionCollisionOffset2;
  return reinterpret_cast<void*>( addr );
}

/**
*  @brief  パスデータの取得
*/
const Traffic::PathCoreData* FieldGimmickEncountAccessor::GetPathCoreData( const Data &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.pathCoreOffset;
  return reinterpret_cast<Traffic::PathCoreData*>( addr );
}

/**
*  @brief  パスデータの取得(2つめ)
*/
const Traffic::PathCoreData* FieldGimmickEncountAccessor::GetPathCoreData2( const Data &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.pathCoreOffset2;
  return reinterpret_cast<Traffic::PathCoreData*>( addr );
}

GFL_NAMESPACE_END(Field)

