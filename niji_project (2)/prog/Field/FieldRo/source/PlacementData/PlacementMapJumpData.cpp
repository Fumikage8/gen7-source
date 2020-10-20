//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		PlacementMapJumpData.h
 *	@brief  マップジャンプデータ
 *	@author	tomoya takahashi
 *	@date		2015.05.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "Field/FieldRo/include/PlacementData/PlacementMapJumpData.h"

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

#include "niji_conv_header/field/mapjump/FieldMapJumpType.h"

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
 *  @brief  初期化
 */
void PlacementMapJumpData::Initialize( const void * cpMapJumpParam, u32 mapJumpParamSize )
{
  m_cpMapJumpParam = reinterpret_cast<const MapJumpParam*>( cpMapJumpParam );
#if PM_DEBUG
  m_MapJumpParamNum = mapJumpParamSize / sizeof(MapJumpParam);
#else
  m_MapJumpParamNum = MAPJUMP_TYPE_MAX;
#endif
}



/**
 *  @brief  破棄
 */
void PlacementMapJumpData::Terminate( void )
{
  m_cpMapJumpParam = NULL;
  m_MapJumpParamNum = 0;
}
  
/**
 *  @brief  マップジャンプタイプごとのパラメータを取得
 */
const PlacementMapJumpData::MapJumpParam* PlacementMapJumpData::GetMapJumpParam( u32 idx ) const
{
  if( idx < m_MapJumpParamNum ){
    return &m_cpMapJumpParam[idx];
  }
  GFL_ASSERT( idx < m_MapJumpParamNum );
  return &m_cpMapJumpParam[0];  //フリーズ回避
}


GFL_NAMESPACE_END(Field);
