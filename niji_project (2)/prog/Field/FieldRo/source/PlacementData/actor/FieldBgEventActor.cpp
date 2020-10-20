/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBgEventActor.cpp
 *  @brief  Bgイベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.06.01
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldBgEventActor.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"

// ConvHeader
#include <niji_conv_header/field/script/inc/zigarude_scr.inc>
#include <niji_conv_header/field/script/inc/field_item.inc>

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldBgEventActor::FieldBgEventActor( void )
  : IFieldActorBase()
  , m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldBgEventActor::~FieldBgEventActor( void )
{
}

/**
 *  @brief  起動
 */
bool FieldBgEventActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
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
bool FieldBgEventActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }
  if( m_pData->scriptID == FieldScript::SCRID_NULL ){ return true; }

  return false;
}


/**
 *  @brief  フラグワークの値セット
 */
bool FieldBgEventActor::SetEventFlagWork( u16 value )
{
  if( m_pData->flagwork == 0 ) return false;

  Field::EventWork * pEventWork = GameSys::GameManager::GetInstance()->GetGameData()->GetEventWork();

  // フラグワークのチェック
  if( EventWork::CheckIDFlagRenge( m_pData->flagwork ) ){
    if( value == 0 ){
      pEventWork->ResetEventFlag( m_pData->flagwork );
    }else{
      pEventWork->SetEventFlag( m_pData->flagwork );
    }
  }else{
    pEventWork->SetEventWork( m_pData->flagwork, value );
  }
  return true;
}


/**
 *  @brief  専用データ設定
 */
void FieldBgEventActor::SetData( const FieldBgEventAccessor::BgEventData *pData )
{
  m_pData = pData;
}

/**
*  @brief  アクションボタンの注視をクリアするフラグ  BgEventはクリアしない。
*/
bool FieldBgEventActor::IsActionButtonInterestClear( void ) const
{
  if( m_pData->scriptID == SCRID_ZIGARUDE_CORE_01
   || m_pData->scriptID == SCRID_ZIGARUDE_CORE_02
   || m_pData->scriptID == SCRID_ZIGARUDE_CORE_03
   || m_pData->scriptID == SCRID_ZIGARUDE_CORE_04
   || m_pData->scriptID == SCRID_ZIGARUDE_CORE_05
   || m_pData->scriptID == SCRID_ZIGARUDE_CELL
   || m_pData->scriptID == SCRID_MITSUSUI_YAMABUKI01
   || m_pData->scriptID == SCRID_MITSUSUI_YAMABUKI02
   || m_pData->scriptID == SCRID_MITSUSUI_USUMOMO01
   || m_pData->scriptID == SCRID_MITSUSUI_USUMOMO02
   || m_pData->scriptID == SCRID_MITSUSUI_KURENAI01
   || m_pData->scriptID == SCRID_MITSUSUI_KURENAI02
   || m_pData->scriptID == SCRID_MITSUSUI_MURASAKI01
   || m_pData->scriptID == SCRID_MITSUSUI_MURASAKI02 )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END( Field );
