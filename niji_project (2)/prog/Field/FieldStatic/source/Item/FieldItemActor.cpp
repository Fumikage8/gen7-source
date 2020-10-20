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
#include "Field/FieldStatic/include/Item/FieldItemActor.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"

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

  // GameManagerがほしい
//  EventScriptCall::CallScript( GameSys::GameManager::GetInstance(), m_pData->scriptID );
  if( p_outIsEventCreate ) *p_outIsEventCreate = true;

  return true;
}

/**
 *  @brief  判定
 */
bool FieldItemActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->itemID == FieldScript::SCRID_NULL ){ return true; }

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
