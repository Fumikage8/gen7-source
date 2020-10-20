/**
 *  GAME FREAK inc.
 *
 *  @file   FieldPosEventActor.cpp
 *  @brief  配置イベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.27
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldPosEventActor.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

#include "Field/FieldRo/include/Collision/BaseShape.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldPosEventActor::FieldPosEventActor( void )
  : IFieldActorBase()
  , m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldPosEventActor::~FieldPosEventActor( void )
{
}

/**
 *  @brief  起動
 */
bool FieldPosEventActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  // 起動条件を満たしていない
  if( IsInvalid() ){ return false; }

  // GameManagerがほしい
  EventScriptCall::CallScript( GameSys::GameManager::GetInstance(), m_pData->scriptID, this );
  if( p_outIsEventCreate ) *p_outIsEventCreate = true; 
  return true;
}

/**
 *  @brief  判定
 */
bool FieldPosEventActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->scriptID == FieldScript::SCRID_NULL ){ return true; }

  MoveModel::FieldMoveModelPlayer* pPlayer = GameSys::GameManager::GetInstance()->GetFieldmap()->GetPlayerCharacter();


  //============================================================================================================
  // @fix GFNMCat[2379]関連 
  // プレイヤー座標からPOSの中心にレイを飛ばし壁にかからないかをチェックする。
  BaseActor * pTargetActor = pPlayer->GetDynamicActor( MoveModel::COLLISION_TYPE_EVENT_POS )->GetCollisionResult(0)->pTargetActor;
  BaseActor::SHAPE shape_type = pTargetActor->GetShapeType();
  gfl2::math::Vector3 eventPos(0,0,0);
  switch(shape_type)
  {
  case BaseActor::SHAPE_CYLINDER:
    {
      CylinderShape * shape = static_cast<CylinderShape*>( pTargetActor->GetShape() );
      eventPos = shape->m_pos + pTargetActor->GetPosition();
    }
    break;

  case BaseActor::SHAPE_BOX:
    {
      BoxShape * shape = static_cast<BoxShape*>( pTargetActor->GetShape() );
      eventPos = shape->m_pos + pTargetActor->GetPosition();
    }
    break;

  case BaseActor::SHAPE_LINE: //16/05/18現在 配置データにLINEを使ったPOSイベントはないが、念のため書いておく。
    {
      LineShape * shape = static_cast<LineShape*>( pTargetActor->GetShape() );
      gfl2::math::Vector3 half_diff = (shape->m_pos1 - shape->m_pos0) * 0.50f; // half length
      eventPos = (shape->m_pos0 + half_diff) + pTargetActor->GetPosition();
    }
    break;

  case BaseActor::SHAPE_TRIANGLE: // POSでは使われていない。
  default:
    GFL_ASSERT_STOP(0);
    break;
  }
  gfl2::math::Vector3 nowPos(pPlayer->GetLocalSRT().translate);
  
  b32 is_wall_hit = pPlayer->GetHitCntFromOshikaeshiSceneEx( nowPos, eventPos, 1, true, true, false );    // 壁があったら起動できない。 ライド禁止フィルターと配置モデル/NPCは除外 また壁1枚＝裏世界に中心なのでこちらもOKとする。

  TOMOYA_PRINT( "\n壁の向こうのPOSイベントチェック log player(%d,%d,%d) posCol(%d,%d,%d) refPosPl(%d,%d,%d)\n", (s32)nowPos.x, (s32)nowPos.y, (s32)nowPos.z, (s32)eventPos.x, (s32)eventPos.y, (s32)eventPos.z, (s32)m_pData->position[0], (s32)m_pData->position[1], (s32)m_pData->position[2] );
  if( !is_wall_hit )
  {
    GFL_ASSERT_MSG( 0, "\n壁の向こうのPOSイベントにヒットしました player(%d,%d,%d) posCol(%d,%d,%d) refPosPl(%d,%d,%d)\n", (s32)nowPos.x, (s32)nowPos.y, (s32)nowPos.z, (s32)eventPos.x, (s32)eventPos.y, (s32)eventPos.z, (s32)m_pData->position[0], (s32)m_pData->position[1], (s32)m_pData->position[2] );
  }
  //============================================================================================================

  return !is_wall_hit;// 壁にヒットしているので起動はさせない。
}

/**
 *  @brief  専用データ設定
 */
void FieldPosEventActor::SetData( const FieldPosEventAccessor::PosData *pData )
{
  m_pData = pData;
}

GFL_NAMESPACE_END( Field );
