/**
 *  GAME FREAK inc.
 *
 *  @file   FieldIntrPosEventActor.cpp
 *  @brief  配置イベント用アクター
 *  @author tomoya takahashi
 *  @date   2015.11.09
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldIntrPosEventActor.h"

// Script
#include "FieldScript/include/FieldScriptSystem.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldIntrPosEventActor::FieldIntrPosEventActor( void )
  : FieldPosEventActor()
{
}

/**
 *  @brief  デストラクタ
 */
FieldIntrPosEventActor::~FieldIntrPosEventActor( void )
{
}

/**
 *  @brief  起動
 */
bool FieldIntrPosEventActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  // 起動条件を満たしていない
  if( IsInvalid() ){ return false; }

  // GameManagerがほしい
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gameManager->GetFieldScriptSystem()->CallIntrScript( m_pData->scriptID, 0 );
  return true;
}

/**
 *  @brief  判定
 */
bool FieldIntrPosEventActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->scriptID == FieldScript::SCRID_NULL ){ return true; }

  return false;
}

/**
 *  @brief  専用データ設定
 */
void FieldIntrPosEventActor::SetData( const FieldPosEventAccessor::PosData *pData )
{
  m_pData = pData;
}

GFL_NAMESPACE_END( Field );
