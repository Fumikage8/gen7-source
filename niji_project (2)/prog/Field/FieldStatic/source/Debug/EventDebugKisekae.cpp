// ============================================================================
/*
 * @file   EventDebugKisekae.cpp
 * @brief  きせかえのデバッグイベント
 * @date   2016.05.20
 * @author yuya_ikeuchi
 */
// ============================================================================
#if PM_DEBUG

#include "Field/FieldStatic/include/Debug/EventDebugKisekae.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

namespace Field{ namespace Debug {

void EventDebugKisekae::StartEvent( GameSys::GameManager* pGameManager,u32 continueCnt )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  EventDebugKisekae* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugKisekae>( pGameEventManager );
  pEvent->SetKisekaeContinueCnt(continueCnt);
}

EventDebugKisekae::EventDebugKisekae( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap )
, m_continueCnt     ( 0 )
, m_loopCnt         ( 0 )
{
}

EventDebugKisekae::~EventDebugKisekae()
{
}

bool EventDebugKisekae::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}

void EventDebugKisekae::InitFunc( GameSys::GameManager* pGameManager )
{
}

void EventDebugKisekae::EndFunc( GameSys::GameManager* pGameManager )
{
}

GameSys::GMEVENT_RESULT EventDebugKisekae::MainFunc( GameSys::GameManager* pGameManager )
{
  enum
  {
    SEQ_RIDE_OFF,
    SEQ_KISEKAE,
    SEQ_CLOSE_FIELD,
    SEQ_OPEN_FIELD,
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
    case SEQ_KISEKAE:
    {
      poke_3d::model::DressUpParam dressUpParam;
      pGameManager->GetFieldmap()->GetMoveModelManager()->GetDressUpParamRand(&dressUpParam);
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup( dressUpParam );

      GFL_PRINT("================================================================== \n");
      GFL_PRINT("KISEKAE CNT                    [%d] \n", m_loopCnt);
      GFL_PRINT("BODY_CATEGORY_SEX              [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX               ]);
      GFL_PRINT("BODY_CATEGORY_BODY_COLOR       [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR        ]);
      GFL_PRINT("BODY_CATEGORY_EYE_COLOR        [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR         ]);
      GFL_PRINT("BODY_CATEGORY_HAIR_COLOR       [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR        ]);
      GFL_PRINT("BODY_CATEGORY_FEMALE_LIP_COLOR [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR  ]);
      GFL_PRINT("ITEM_CATEGORY_FACE             [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE              ]);
      GFL_PRINT("ITEM_CATEGORY_HAIR_STYLE       [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE        ]);
      GFL_PRINT("ITEM_CATEGORY_ACCBADGE         [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCBADGE          ]);
      GFL_PRINT("ITEM_CATEGORY_ACCEYE           [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE            ]);
      GFL_PRINT("ITEM_CATEGORY_ACCHAIR          [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR           ]);
      GFL_PRINT("ITEM_CATEGORY_BAG              [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG               ]);
      GFL_PRINT("ITEM_CATEGORY_BNGL             [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL              ]);
      GFL_PRINT("ITEM_CATEGORY_BODY             [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BODY              ]);
      GFL_PRINT("ITEM_CATEGORY_BOTTOMS          [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS           ]);
      GFL_PRINT("ITEM_CATEGORY_HAT              [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT               ]);
      GFL_PRINT("ITEM_CATEGORY_LEGS             [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS              ]);
      GFL_PRINT("ITEM_CATEGORY_SHOES            [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES             ]);
      GFL_PRINT("ITEM_CATEGORY_TOPS             [%d] \n",dressUpParam.bodyParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS              ]);
      GFL_PRINT("================================================================== \n");

      this->AddSeqNo();
      break;
    }
    case SEQ_CLOSE_FIELD:
    {
      // クローズイベント
      Field::EventFieldmapClose * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Field::EventFieldmapClose >( pGameManager->GetGameEventManager() );
      Field::EventFieldmapClose::Desc desc;
      desc.closeType = Field::FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackDefault();         //フェードはデフォルト
      this->AddSeqNo();
      break;
    }
    case SEQ_OPEN_FIELD:
    {
      Field::EventFieldmapOpen * p_event = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Field::EventFieldmapOpen >( pGameManager->GetGameEventManager() );
      p_event->SetDesc( Field::FM_OPEN_MODE_APP_RECOVER, *pGameManager->GetGameData()->GetStartLocation() );
      p_event->SetFadeCallBackDefault();
      this->AddSeqNo();
      break;
    }
    case SEQ_END:
    {
      if( m_loopCnt < m_continueCnt )
      {
        m_loopCnt++;
        SetSeqNo(SEQ_CLOSE_FIELD);
      }
      else
      {
        return GameSys::GMEVENT_RES_FINISH;
      }
    }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

}}

#endif // PM_DEBUG