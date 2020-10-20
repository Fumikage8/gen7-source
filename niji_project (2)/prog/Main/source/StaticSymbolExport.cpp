//======================================================================
/**
 * @file StaticSymbolExport.cpp
 * @date 2015/08/20 17:34:18
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "StaticSymbolExport.h"


#include "AppLib/include/Fade/DemoFade.h"
#include "App/AppEvent/include/Shop/ShopBuyEvent.h"
#include "App/Finder/include/FinderProc.h"
#include "App/FinderMenu/include/FinderMenuProc.h"
#include "App/FinderMenu/include/FinderMenuAppParam.h"

#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldStatic/include/Encount/FieldEncountUtil.h"
#include "Field/FieldRo/include/DropItem/FieldDropItemLottery.h"
#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"

#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"

#include "Field/FieldRo/include/TrialModel/FieldTrialModelFactory.h"
#include "Field/FieldRo/include/TrialModel/FieldTrialModel.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderUtil.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectShadow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadowFoot.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGimmickEncount.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFishingBuoy.h"

#include "Field/FieldRo/include/StaticModel/StaticModel.h"

#include "Field/FieldRo/include/FieldEnvManager.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldBerryPointActor.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldFishingSpotActor.h"

#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"
#include "NetStatic/NetEvent/include/JoinFestaRecreateFieldPersonalListEvent.h"

#include "Field/FieldRo/include/Camera/FieldCameraManager.h"

#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "NetStatic/NetEvent/include/PokeListJoinEvent.h"

#include "GameSys/include/DllProc.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugGimmickEncount.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldStatic/include/Debug/EventDebugRidePokemon.h"
#include "Field/FieldStatic/include/Debug/EventDebugKisekaeCurrentPosition.h"


// for Debug EventMapCapture 
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>
#endif

GFL_NAMESPACE_BEGIN(StaticSymbolExport)

void Export( bool flag )
{
  GameSys::GameManager* p_gman = NULL;

  if( flag != false )
  {
    u32 dummy = 0;
    AppLib::Fade::DemoFade test( dummy );

    Field::DropItem::Lottery lottery;
    lottery.SetData( NULL);
    lottery.LotteryItemID( 0);

    Field::DropItem::Creator  dropitem_creator;
    dropitem_creator.CreateLotteryDropItem( 0, gfl2::math::Vector3(0,0,0), Field::FieldItemManager::TYPE_NORMAL );
    dropitem_creator.LotteryDropItem(0);

    Field::Encount::Encount encount( NULL, NULL);
    encount.LotteryEncountPoke( NULL, 0, false);
    encount.SetEncountPokeFromFirstRecord( NULL, 0,0,false );
    encount.CallSymbolEncount( NULL, 0, 0);

    Field::Encount::EncountUtil::SyncLoadSymbolEncountParam( NULL, NULL, 0, NULL);

    Field::GimmickEncount::DataAccessor accessor;
    accessor.GetPokeData( 0);
    accessor.SetData(NULL);
    accessor.GetSymbolEncountIDOffset();

    {
      Field::OrthoRenderPath path;
      path.AddRenderingTarget( NULL);
      path.RemoveRenderingTarget( NULL);
    }

    {
      Field::CaptureRenderingPipeLine rpl( NULL, NULL, NULL, NULL, 0.0f, 0.0f);
      rpl.AddEdgeRenderingTarget( NULL);
      rpl.RemoveEdgeRenderingTarget( NULL);
      rpl.InitializeMotionBlurPath( NULL, NULL, NULL, 0);
      rpl.WaitInitializeMotionBlurPath();
      rpl.TerminateMotionBlurPath();
    }

    {
      Field::FieldRenderingPipeLineManager manager;
      manager.ChangeExecuteType( Field::FieldRenderingPipeLineManager::EXECUTE_TYPE_NORMAL);
      manager.GetCaptureRenderingPipeLine();
      manager.GetMyRenderingPipeLine();

      Field::FieldRenderingPipeLineManager::CapturePipeLineCreateDescription desc;
      manager.CreateCaptureRenderingPipeLine( desc);
      manager.DeleteCaptureRenderingPipeLine();
    }

    {
      Field::TrialModel::TrialModelFactory factory;
      factory.Update();

      Field::TrialModel::FieldTrialModel model( NULL, NULL);
      gfl2::math::Vector3 pos = model.GetPosition();

    }
    {
      App::FinderMenu::APP_PARAM param;
      App::FinderMenu::Proc *procFinder = GameSys::CallFinderMenuProc();
      procFinder->SetAppParam( &param );

      Field::TrialModel::FinderModel model(NULL,NULL,NULL,0);
#if defined(GF_PLATFORM_CTR)
      (void)Field::TrialModel::FinderUtil::EncodeJpeg( NULL,NULL,NULL );
#endif
    }

    {
      Field::Effect::EffectShadow     shadow;
      Field::Effect::EffectShadowFoot shadowFoot;
      shadow.SetVisibleShadow(true);
      shadowFoot.SetVisibleShadow(true);
    }

    {
      Field::Effect::EffectGimmickEncount effect;
      effect.Create( Field::Effect::EffectGimmickEncount::EFFECT_TYPE_NONE);

      Field::Effect::EffectFishingBuoy effect_fihingbuoy;
      effect_fihingbuoy.ChangeAnimation( 1 );
    }

    {
      Field::StaticModel::StaticModel::Handle_IsEndEventAnimation handle( 0 );
      Field::StaticModel::StaticModel::Handle_StartEventAnimation handle2( 0 );
      Field::StaticModel::StaticModelManager manager;
      Field::StaticModel::StaticModel model;
      (void)model.IsEnd( handle);
      (void)manager.GetEventIDStaticModel(0);
    }

    {
      Field::FieldBerryPointActor actor;
      Field::FieldBerryPointActor::SetupDesc desc;
      actor.Initialize(NULL, NULL, desc);
      actor.StartSwing();
      actor.StopSwing();
    }

    {
      Field::FieldFishingSpotActor actor;
      actor.ExistRare();
      actor.EscapeRare();
      actor.GetLotData();
      actor.GetData();
      actor.GetMist();
      Field::FieldFishingSpotActor::Mist mist;
      mist.GetState();
      mist.StartDilute();
      mist.Update();
    }

    {
      Field::PlacementDataManager placementDataMan;
      Field::PlacementDataManager::Desc desc;
      placementDataMan.Initialize(desc);
      placementDataMan.SearchZoneBerryPoint(0);
      placementDataMan.UnRegisterZoneBerryPoint();
      placementDataMan.UnRegisterZoneBerryPoint(0);
      placementDataMan.DeleteZoneEventIDGimmickEncount(0);
      placementDataMan.UnRegistZoneEventIDGimmickEncount(0);
      placementDataMan.SearchZonePokeFinderFromIndex(0);
    }

    {
      Field::Camera::CameraManager cameraManager;
      cameraManager.GetMainViewCamera();
    }
#if PM_DEBUG
    {
      Field::Debug::DebugTypes::m_isJumpMoveEnable = false;
      Field::Debug::DebugTypes::m_jumpMoveSpeed = 0.0f;
      Field::Debug::DebugTypes::m_pokeBaseScale = 0.0f;
      Field::Debug::DebugTypes::m_isEasingEnable = false;
      Field::Debug::DebugTypes::m_easingFunc = 0;
      Field::Debug::DebugGimmickEncount::m_isEncountEffectEnable = false;
      Field::Debug::DebugGimmickEncount::m_isDebugMotionBlurEnable = false;
      Field::Debug::DebugGimmickEncount::m_motionBlurBlendType = 0;
      Field::Debug::DebugGimmickEncount::m_motionBlurAnimeType = 0;

      Field::Debug::DebugGimmickEncount::m_isDebugMotionBlurBlendInterpEnable = false;
      Field::Debug::DebugGimmickEncount::m_motionBlurBlendInterpType = 0;
      Field::Debug::DebugGimmickEncount::m_motionBlurBlendEnd = 0.0f;
      Field::Debug::DebugGimmickEncount::m_motionBlurBlendFrame = 0;
      Field::Debug::DebugGimmickEncount::m_isDebugMotionBlurColorInterpEnable = false;
      Field::Debug::DebugGimmickEncount::m_motionBlurColorInterpType = 0;
      Field::Debug::DebugGimmickEncount::m_motionBlurColorEnd = gfl2::math::Vector3::GetZero();
      Field::Debug::DebugGimmickEncount::m_motionBlurColorFrame = 0;

      Field::Debug::EventDebugRidePokemon eventDebugRidePokemon;
      eventDebugRidePokemon.InitFunc(NULL);

      Field::Debug::EventDebugKisekaeCurrentPosition eventDebugKisekae;
      eventDebugKisekae.InitFunc(NULL);
    }
#endif

    {
      Field::EnvManager manager;
      manager.GetDrawEnvNode();
    }

    {
      BattleInst::BattleInst inst(NULL,NULL);
      inst.Create();
      inst.Delete();
      inst.SetupBattleRoyalResultParam();
      inst.GetBattleRoyalEntoryFirstMonsNo();
    }

    App::Event::ShopBuyEvent::StartEvent( p_gman );
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::Event::EventPokeFinderForPlacement>( p_gman->GetGameEventManager() );
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaRecreateFieldPersonalListEvent>( p_gman->GetGameEventManager() );

#if PM_DEBUG
    {
      Field::StaticModel::StaticModelManager staticModelManager;
      staticModelManager.DebugResumeLocalAnimation();
      staticModelManager.DebugSuspendLocalAnimation( 0.0f );

      Field::Camera::CameraUnit unitA, unitB;
      unitA.CopyBaseParam( unitB );
      unitA.ChangeController( NULL );

      Field::Camera::CameraManager cameraManager;
      cameraManager.GetCameraByUnitNo( 0 );
      cameraManager.GetMainGamePlayCamera();
      cameraManager.ChangeMainCameraByUnitNo( 0 );
      cameraManager.GetUnitNoForMainGamePlayCamera();

      Field::Terrain::TerrainManager terrainManager;
      terrainManager.DebugResumeLocalAnimation();
      terrainManager.DebugSuspendLocalAnimation( 0.0f );
      terrainManager.ClearExternalBlockControlPoint();
      terrainManager.SetExternalBlockControlPointByPosition( gfl2::math::Vector3( 0.0f, 0.0, 0.0f ));
    
      Field::Camera::Area::Utility::GetEventScriptCameraParameter( NULL, NULL, NULL, NULL, NULL );

      Field::Debug::FieldDebugMenu debugMenu( NULL, NULL, NULL, NULL, NULL, NULL, NULL );
      debugMenu.CreateFlagWorkItemForSearch( NULL, NULL, 0, NULL, 0 );

      debug::KeyboardProc proc;
      proc.SetDefaultInputMode( debug::KeyboardProc::INPUT_ALPHA );
    }
#endif // #if PM_DEBUG


  }
}

GFL_NAMESPACE_END(StaticSymbolExport)
