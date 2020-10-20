//======================================================================
/**
 * @file FieldItemActor.cpp
 * @date 2015/09/04 16:00:16
 * @author ikawa_hiroki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldItemActor.h"
#include "Field/FieldStatic/include/Item/FieldItemManager.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"

#include "niji_conv_header/field/script/inc/field_item.inc"

#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"

#include  "Savedata/include/Record.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldItemActor::FieldItemActor( void )
  : IFieldActorBase()
  , m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldItemActor::~FieldItemActor( void )
{
}

/**
 *  @brief  起動
 */
bool FieldItemActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;

  // 起動条件を満たしていない
  if( IsInvalid() ){ return false; }

  // eventIDがドロップアイテムだったらレコード追加
  if( (m_pData->eventID >= DROPITEM_ACTOR_START) && (m_pData->eventID <= DROPITEM_ACTOR_START + Field::FieldItemManager::MAX_DROP_ITEM) )
  {
    Savedata::IncRecord( Savedata::Record::RECID_DROP_ITEM );
  }

  // GameManagerがほしい
  EventScriptCall::CallScript( GameSys::GameManager::GetInstance(), FieldItemAccessor::GetScriptId( *m_pData ), this, NULL, m_pData->itemID, m_pData->item_num, m_pData->eventID, m_pData->flagwork );
  if( p_outIsEventCreate ) *p_outIsEventCreate = true;

  return true;
}

/**
 *  @brief  判定
 */
bool FieldItemActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->itemID == 0 ){ return true; }

  return false;
}

/**
 *  @brief  専用データ設定
 */
void FieldItemActor::SetData( const FieldItemAccessor::FieldItemData *pData )
{
  m_pData = pData;
}

GFL_NAMESPACE_END( Field );
