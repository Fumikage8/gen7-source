//======================================================================
/**
 * @file FieldSkyboxPosPosDataAccessor.h
 * @date 2016/2/19
 * @author saita_kazuki
 * @brief 天球位置データアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Skybox/FieldSkyboxPosDataAccessor.h"

// gfl2
#include <debug/include/gfl2_Assert.h>

#include <niji_conv_header/field/world/world_id.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 * @param pData バイナリデータ
 */
PosDataAccessor::PosDataAccessor( void* pData)
  : m_pData( NULL)
{
  this->SetData( pData);
}

/**
 * @brief デストラクタ
 */
PosDataAccessor::~PosDataAccessor()
{
}

/**
 *  @brief データセット
 *  @param pData バイナリデータ
 */
void PosDataAccessor::SetData( void* pData)
{
  if( pData == NULL)
  {
    GFL_ASSERT( 0);
    return;
  }

  m_pData = reinterpret_cast<Data*>( pData);
}

/**
 *  @brief データが存在するか
 *  @param worldID ワールドID
 *  @retval true 存在する
 *  @retval false 存在しない
 */
bool PosDataAccessor::IsExistData( u16 worldID)
{
  return ( this->GetCoreData( worldID) != NULL );
}

/**
 *  @brief 位置Yの取得
 *  @param worldID ワールドID
 *  @return Y座標
 *  @note データが存在しなければ0を返す
 */
f32 PosDataAccessor::GetPositionY( u16 worldID)
{
  CoreData* pCoreData = this->GetCoreData( worldID);
  if( pCoreData)
  {
    return pCoreData->posY;
  }
  return 0.0f;
}

/**
 *  @brief コアデータ取得
 *  @param worldID ワールドID
 *  @return 見つかったコアデータのポインタ
 *  @note 見つからなければNULL
 */
PosDataAccessor::CoreData* PosDataAccessor::GetCoreData( u16 worldID) const
{
  if( m_pData == NULL)
  {
    GFL_ASSERT( 0);
    return NULL;
  }

  for( u32 i = 0; i < m_pData->header.num; ++i)
  {
    // 設定されているワールドIDがエラー値ならダミーデータ
    if( m_pData->data[i].worldID == WORLD_ERROR)
    {
      continue;
    }

    if( m_pData->data[i].worldID == worldID )
    {
      return &(m_pData->data[i]);
    }
  }

  // 見つからない
  return NULL;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( Field )
