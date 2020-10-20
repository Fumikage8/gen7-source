//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldEnvSoundAccessor.h
 *	@brief  環境音データアクセサー
 *	@author	 tomoya takahashi
 *	@date		2015.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// gflib2
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldEnvSoundAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

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
 *  @brief  コンストラクタ
 */
FieldEnvSoundAccessor::FieldEnvSoundAccessor( void ) : m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldEnvSoundAccessor::~FieldEnvSoundAccessor( void )
{
  m_pData = NULL;
}
 

/**
 *  @brief  初期化
 */
void FieldEnvSoundAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const EnvSoundDataHeader*>(data);
}

/**
 *  @brief  配置情報最大数取得
 */
u32 FieldEnvSoundAccessor::GetPlacementMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->placement_num;
}

/**
 *  @brief  配置情報取得
 */
const FieldEnvSoundAccessor::EnvSoundPlacement * FieldEnvSoundAccessor::GetPlacement( u32 index ) const
{
  if( index < m_pData->placement_num ){
    return &m_pData->placement[index];
  }

  GFL_ASSERT_STOP(0);
  return NULL;
}

/** 
 *  @brief  音情報最大数取得
 */
u32 FieldEnvSoundAccessor::GetParamMax() const
{
  uptr addr = reinterpret_cast<uptr>(&m_pData->placement[0]);
  addr += sizeof(EnvSoundPlacement) * m_pData->placement_num;
  const u32 * p_param_num = reinterpret_cast<const u32*>(addr);

  return *p_param_num;
}

/**
 *  @brief  音情報取得
 */
const FieldEnvSoundAccessor::EnvSoundParam * FieldEnvSoundAccessor::GetParam( u32 index ) const
{
  u32 param_max = GetParamMax();

  uptr addr = reinterpret_cast<uptr>(&m_pData->placement[0]);
  addr += sizeof(EnvSoundPlacement) * m_pData->placement_num + sizeof(u32);

  const EnvSoundParam* cpParam = reinterpret_cast<const EnvSoundParam*>(addr);

  if( index < param_max )
  {
    return &cpParam[index];
  }

  GFL_ASSERT_STOP(0);
  return NULL;
}

/**
 *  @brief  コリジョン情報の取得
 */
const void * FieldEnvSoundAccessor::GetCollisionData( const FieldEnvSoundAccessor::EnvSoundPlacement &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}


GFL_NAMESPACE_END(Field)
