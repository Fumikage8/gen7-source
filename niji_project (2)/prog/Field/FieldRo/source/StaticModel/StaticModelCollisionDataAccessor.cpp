//======================================================================
/**
 * @file StaticModelCollisionDataAccessor.cpp
 * @date 2015/08/21 19:46:28
 * @author saita_kazuki
 * @brief 配置モデルコリジョンデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/StaticModel/StaticModelCollisionDataAccessor.h"
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @brief コンストラクタ
 * @param pData パックデータ。FieldResidentから取得してください
 */
CollisionDataAccessor::CollisionDataAccessor()
  : m_pData( NULL)
  , m_Accessor()
{
}
CollisionDataAccessor::CollisionDataAccessor( void* pData)
  : m_pData( pData)
  , m_Accessor( pData)
{
}

/**
 * @brief デストラクタ
 */
CollisionDataAccessor::~CollisionDataAccessor()
{
}

/**
 * @brief パックデータセット
 * @param pData パックデータ。FieldResidentから取得してください
 */
void CollisionDataAccessor::SetData( void* pData)
{
  m_pData = pData;
  m_Accessor.Initialize( pData);
}

/**
 * @brief データ取得
 * @param staticModelID 配置モデルID
 * @return コリジョンデータ
 * @note 見つからない場合はNULL
 */
const CollisionDataAccessor::COLLISION_DATA* CollisionDataAccessor::GetData( u16 staticModelID) const
{
  if( m_pData == NULL)
  {
    return NULL;
  }

  for( u32 i = 0; i < m_Accessor.GetDataMax(); ++i)
  {
    if( m_Accessor.GetDataSize( i) == 0)
    {
      return NULL;
    }

    DATA* pData = reinterpret_cast<DATA*>( m_Accessor.GetData( i));

    if( staticModelID == pData->header.staticModelID)
    {
      return &pData->collision;
    }
  }
  return NULL;
}

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )
