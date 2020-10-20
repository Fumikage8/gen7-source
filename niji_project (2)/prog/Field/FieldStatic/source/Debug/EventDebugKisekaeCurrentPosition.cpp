// ============================================================================
/*
 * @file   EventDebugKisekaeCurrentPosition.cpp
 * @brief  きせかえのデバッグイベント(暗転なし)
 * @date   2016.05.20
 * @author yuya_ikeuchi
 */
// ============================================================================
#if PM_DEBUG

#include "Field/FieldStatic/include/Debug/EventDebugKisekaeCurrentPosition.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

namespace Field{ namespace Debug {

void EventDebugKisekaeCurrentPosition::StartEvent( GameSys::GameManager* pGameManager,u32 continueCnt )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  EventDebugKisekaeCurrentPosition* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugKisekaeCurrentPosition>( pGameEventManager );
  pEvent->SetKisekaeContinueCnt(continueCnt);
}

EventDebugKisekaeCurrentPosition::EventDebugKisekaeCurrentPosition()
: GameSys::GameEvent( NULL )
, m_continueCnt     ( 0 )
, m_loopCnt         ( 0 )
{
}

EventDebugKisekaeCurrentPosition::EventDebugKisekaeCurrentPosition( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap )
, m_continueCnt     ( 0 )
, m_loopCnt         ( 0 )
{
}

EventDebugKisekaeCurrentPosition::~EventDebugKisekaeCurrentPosition()
{
}

bool EventDebugKisekaeCurrentPosition::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}

void EventDebugKisekaeCurrentPosition::InitFunc( GameSys::GameManager* pGameManager )
{
}

void EventDebugKisekaeCurrentPosition::EndFunc( GameSys::GameManager* pGameManager )
{
}

GameSys::GMEVENT_RESULT EventDebugKisekaeCurrentPosition::MainFunc( GameSys::GameManager* pGameManager )
{
  enum
  {
    SEQ_RIDE_OFF,
    SEQ_EVENT_START,
    SEQ_HIDE,
    SEQ_UNLOAD,
    SEQ_LOAD_START,
    SEQ_LOAD_WAIT,
    SEQ_CREATE_PLAYER,
    SEQ_END,
  };

  switch( this->GetSeqNo() )
  {
    case SEQ_RIDE_OFF:
    {
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( pGameManager->GetGameEventManager() );
      this->AddSeqNo();
      break;
    }
    case SEQ_EVENT_START:
    {
      pGameManager->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
      this->AddSeqNo();
      break;
    }
    case SEQ_HIDE:
    {
      pGameManager->GetFieldmap()->GetPlayerCharacter()->SetVisible(false);
      this->AddSeqNo();
      break;
    }
    case SEQ_UNLOAD:
    {
      poke_3d::model::DressUpParam dressupParam = pGameManager->GetGameData()->GetPlayerStatus()->GetDressup();
      pGameManager->GetFieldmap()->GetMoveModelManager()->UnloadDressUpParts(dressupParam);
       this->AddSeqNo();
      break;
    }
    case SEQ_LOAD_START:
    {
      pGameManager->GetFieldmap()->GetMoveModelManager()->GetDressUpParamRand(&m_dressUpParam);
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup( m_dressUpParam );

      GFL_PRINT("================================================================== \n");
      GFL_PRINT("KISEKAE CNT                    [%d] \n", m_loopCnt);
      GFL_PRINT("BODY_CATEGORY_SEX              [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX               ]);
      GFL_PRINT("BODY_CATEGORY_BODY_COLOR       [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR        ]);
      GFL_PRINT("BODY_CATEGORY_EYE_COLOR        [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR         ]);
      GFL_PRINT("BODY_CATEGORY_HAIR_COLOR       [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR        ]);
      GFL_PRINT("BODY_CATEGORY_FEMALE_LIP_COLOR [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR  ]);
      GFL_PRINT("ITEM_CATEGORY_FACE             [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE              ]);
      GFL_PRINT("ITEM_CATEGORY_HAIR_STYLE       [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE        ]);
      GFL_PRINT("ITEM_CATEGORY_ACCBADGE         [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCBADGE          ]);
      GFL_PRINT("ITEM_CATEGORY_ACCEYE           [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE            ]);
      GFL_PRINT("ITEM_CATEGORY_ACCHAIR          [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR           ]);
      GFL_PRINT("ITEM_CATEGORY_BAG              [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG               ]);
      GFL_PRINT("ITEM_CATEGORY_BNGL             [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL              ]);
      GFL_PRINT("ITEM_CATEGORY_BODY             [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BODY              ]);
      GFL_PRINT("ITEM_CATEGORY_BOTTOMS          [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS           ]);
      GFL_PRINT("ITEM_CATEGORY_HAT              [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT               ]);
      GFL_PRINT("ITEM_CATEGORY_LEGS             [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS              ]);
      GFL_PRINT("ITEM_CATEGORY_SHOES            [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES             ]);
      GFL_PRINT("ITEM_CATEGORY_TOPS             [%d] \n",m_dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS              ]);
      GFL_PRINT("================================================================== \n");

      pGameManager->GetFieldmap()->GetMoveModelManager()->LoadDressUpPartsAsync( m_dressUpParam );
      this->AddSeqNo();
      break;
    }
    case SEQ_LOAD_WAIT:
    {
      if( pGameManager->GetFieldmap()->GetMoveModelManager()->IsDressUpPartsLoaded(m_dressUpParam) )
      {
        this->AddSeqNo();
      }
      break;
    }
    case SEQ_CREATE_PLAYER:
    {
      pGameManager->GetFieldmap()->GetMoveModelManager()->TerminateMoveModelResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
      pGameManager->GetFieldmap()->GetMoveModelManager()->SetupDressUpParts(&m_dressUpParam);
      pGameManager->GetFieldmap()->CreatePlayer(false);
      pGameManager->GetFieldmap()->GetPlayerCharacter()->SetVisible(true);
      this->AddSeqNo();
      break;
    }
    case SEQ_END:
    {
      if( m_loopCnt < m_continueCnt )
      {
        m_loopCnt++;
        SetSeqNo(SEQ_HIDE);
      }
      else
      {
        pGameManager->GetFieldmap()->GetMoveModelManager()->PullEventRequestAll();
        return GameSys::GMEVENT_RES_FINISH;
      }
    }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

}}

#endif // PM_DEBUG