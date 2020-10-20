//======================================================================
/**
 * @file FieldItemAccessor.cpp
 * @date 2015/09/04 15:43:47
 * @author ikawa_hiroki
 * @brief 配置アイテム用アクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/accessor/FieldItemAccessor.h"
#include "Field/FieldStatic/include/Item/FieldItemManager.h"

#include <niji_conv_header/field/script/inc/field_item.inc>
#include <niji_conv_header/field/placement_data/FieldPlacementItemTypes.h>


GFL_NAMESPACE_BEGIN(Field)

const u32 FieldItemAccessor::sc_SCRIPT_ID_LIST[FieldItemAccessor::SCRIPT_ID_LIST_MAX] =  
{
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT0,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT1,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT2,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT3,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT4,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT5,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT6,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT7,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT8,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT9,
  SCRID_FIELD_ITEM_GET_EX_SCRIPT10,
};

/**
 *  @brief  コンストラクタ
 */
FieldItemAccessor::FieldItemAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldItemAccessor::~FieldItemAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldItemAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const FieldItemDataHeader*>( data );
}

/**
 *  @brief  最大数
 */
u32 FieldItemAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldItemAccessor::FieldItemData *FieldItemAccessor::GetData( u32 index ) const
{
  if( m_pData->num > index )
  {
    return &m_pData->data[ index ];
  }
  return NULL;
}

/**
 *  @brief  コリジョン情報取得
 */
const void *FieldItemAccessor::GetCollisionData( const FieldItemAccessor::FieldItemData &data ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}


/**
 *  @brief スクリプトIDの取得
 */
u32 FieldItemAccessor::GetScriptId( const FieldItemAccessor::FieldItemData& data )
{
  if( data.shapeType < GFL_NELEMS(sc_SCRIPT_ID_LIST) )
  {
    return sc_SCRIPT_ID_LIST[data.shapeType];
  }

  GFL_ASSERT( data.shapeType < GFL_NELEMS(sc_SCRIPT_ID_LIST) );
  return SCRID_FIELD_ITEM_GET;
}

/** 
 *  @brief shapeTypeが隠しアイテムかチェック
 */
b32 FieldItemAccessor::IsHiddenShapeType( u32 shapeType )
{
  // Hidden以下は隠しアイテム
  if( shapeType >= ITEM_TYPE_HIDDEN )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(Field)
