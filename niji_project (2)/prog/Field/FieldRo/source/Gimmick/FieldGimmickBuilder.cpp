/**
 *  GAME FREAK inc.
 *
 *  @file   FieldGimmickBuilder.cpp
 *  @brief  フィールドのシーケンスに割り込み独自の処理を行うクラス
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

// gfl2
#include <ro/include/gfl2_RoManager.h>

// Field
#include "Field/FieldRo/include/Gimmick/FieldGimmickBuilder.h"
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"

// conv_header
#include "niji_conv_header/field/FieldGimmickTypes.h"

// Gimmick
#include "Field/FieldRo/include/Gimmick/FieldGimmickKairikyRock.h"
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"
#include "Field/FieldRos/FieldGimmickPokecen/include/FieldGimmickPokecen.h"
#include "Field/FieldRos/FieldGimmickLift/include/FieldGimmickLift.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/FieldGimmickBattleInst.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/FieldGimmickBattleFes.h"
#include "Field/FieldRos/FieldGimmickShirenDenki/include/FieldGimmickShirenDenki.h"
#include "Field/FieldRos/FieldGimmickTriggerPos/include/FieldGimmickTriggerPos.h"

GFL_NAMESPACE_BEGIN( Field );

// ギミック用シーケンス生成
FieldGimmick *FieldGimmickBuilder::CreateGimmick( gfl2::heap::HeapBase *pHeap, gfl2::fs::AsyncFileManager *pFileMgr, Fieldmap *pFieldmap, u32 nGimmickID, u32 index, u32 zoneId, PlacementDataManager* pPlacementDataManager )
{
  FieldGimmick *pResult = NULL;
  gfl2::ro::Module *pModule = NULL;
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  switch( nGimmickID ){
  case GIMMICK_TYPE_JOIN_FESTA:

    pModule = roMgr->LoadModule( pFileMgr , "FieldGimmickJoinFesta.cro" );
    roMgr->StartModule( pModule, false );

    pResult = GFL_NEW( pHeap ) FieldGimmickJoinFesta( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;

  case GIMMICK_TYPE_JOIN_FESTA_ROOM:

    pModule = roMgr->LoadModule( pFileMgr , "FieldGimmickJoinFestaRoom.cro" );
    roMgr->StartModule( pModule, false );

    pResult = GFL_NEW( pHeap ) FieldGimmickJoinFestaRoom( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;

  case GIMMICK_TYPE_POKECEN:

    pModule = roMgr->LoadModule( pFileMgr , "FieldGimmickPokecen.cro" );
    roMgr->StartModule( pModule, true );

    pResult = GFL_NEW( pHeap ) FieldGimmickPokecen( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;
  case GIMMICK_TYPE_KAIRIKY_ROCK:

    pResult = GFL_NEW( pHeap ) FieldGimmickKairikyRock( pHeap, pFieldmap, zoneId, pPlacementDataManager );

    break;
  case GIMMICK_TYPE_LIFT:

    pModule = roMgr->LoadModule( pFileMgr , "FieldGimmickLift.cro" );
    roMgr->StartModule( pModule, true );

    pResult = GFL_NEW( pHeap ) FieldGimmickLift( pHeap, pFieldmap, zoneId, pPlacementDataManager );
    pResult->SetModule( pModule );

    break;
  case GIMMICK_TYPE_BATTLE_INST:

    pModule = roMgr->LoadModule( pFileMgr , "FieldGimmickBattleInst.cro" );
    roMgr->StartModule( pModule, true );

    pResult = GFL_NEW( pHeap ) FieldGimmickBattleInst( pHeap, pFieldmap, zoneId );
    pResult->SetModule( pModule );

    break;

  case GIMMICK_TYPE_BATTLE_FES:

    pModule = roMgr->LoadModule(pFileMgr, "FieldGimmickBattleFes.cro");
    roMgr->StartModule(pModule, true);

    pResult = GFL_NEW( pHeap ) FieldGimmickBattleFes( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;
  
  case GIMMICK_TYPE_SHIREN_DENKI:

    pModule = roMgr->LoadModule(pFileMgr, "FieldGimmickShirenDenki.cro");
    roMgr->StartModule(pModule, true);

    pResult = GFL_NEW( pHeap ) FieldGimmickShirenDenki( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;

  case GIMMICK_TYPE_TRIGGER_POS:

    pModule = roMgr->LoadModule(pFileMgr, "FieldGimmickTriggerPos.cro");
    roMgr->StartModule(pModule, true);

    pResult = GFL_NEW( pHeap ) FieldGimmickTriggerPos( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;


  case GIMMICK_TYPE_UB01:
    pModule = roMgr->LoadModule(pFileMgr, "FieldGimmickTriggerPos.cro");
    roMgr->StartModule(pModule, true);

    pResult = GFL_NEW( pHeap ) FieldGimmickUb01( pHeap, pFieldmap );
    pResult->SetModule( pModule );

    break;

  }

  return pResult;
}

GFL_NAMESPACE_END( Field );
