//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPosEventAccessor.h
 *	@brief  POSイベント配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldPosEventAccessor.h"

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
FieldPosEventAccessor::FieldPosEventAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldPosEventAccessor::~FieldPosEventAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldPosEventAccessor::Initialize( const void * data )
{
  m_pData = static_cast<const PosDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldPosEventAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief 情報取得
 */
const FieldPosEventAccessor::PosData * FieldPosEventAccessor::GetData( u32 idx ) const
{
  if( m_pData->num > idx ){
    return &m_pData->data[ idx ];
  }

  return NULL;
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * FieldPosEventAccessor::GetCollisionData( const FieldPosEventAccessor::PosData & data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}



GFL_NAMESPACE_END(Field);

