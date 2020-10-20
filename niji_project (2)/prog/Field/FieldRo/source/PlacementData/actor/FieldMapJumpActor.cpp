/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMapJumpActor.cpp
 *  @brief  マップジャンプ用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldMapJumpActor.h"

// Event
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "GameSys/include/GameManager.h"

// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldMapJumpActor::FieldMapJumpActor( void )
  : IFieldActorBase()
{
}

/**
 *  @brief  デストラクタ
 */
FieldMapJumpActor::~FieldMapJumpActor( void )
{
}

/**
 *  @brief  起動
 */
bool FieldMapJumpActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;

  if( IsInvalid() ){ return false; }

  // マップジャンプする
  EventMapJump::CallMapJumpPlacement( GameSys::GameManager::GetInstance(), m_pData );
  if( p_outIsEventCreate ) *p_outIsEventCreate = true;
  
  // 集中演出を解除
  {
    // カメラフォーカスエフェクト影響のSTOP
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap* pFieldMap = pGameManager->GetFieldmap();
    if( pFieldMap->GetCameraManager()->GetFocus() )
    {
      pFieldMap->GetCameraManager()->GetFocus()->EventStop( Camera::Area::Focus::EventStopBit::EFFECT | Camera::Area::Focus::EventStopBit::BGM | Camera::Area::Focus::EventStopBit::SE | Camera::Area::Focus::EventStopBit::INTERES );
    }
  }


  return true;
}

/**
 *  @brief  判定
 */
bool FieldMapJumpActor::IsInvalid( void )
{
  return false;  
}

/**
 *  @brief  専用データ設定
 */
void FieldMapJumpActor::SetData( const FieldMapJumpAccessor::MapJumpData *pData )
{
  m_pData = pData;
}

GFL_NAMESPACE_END( Field );
