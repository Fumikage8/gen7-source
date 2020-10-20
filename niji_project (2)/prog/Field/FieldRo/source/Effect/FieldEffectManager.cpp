/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectManager.cpp
 *  @brief  エフェクト管理
 *  @author miyachi_soichi
 *  @date   2015.50.20
 */

// gfl2
#include <debug/include/gfl2_DebugPrint.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include "Field/FieldStatic/include/Field3DObjectNode.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
// Sound
#include "Sound/include/Sound.h"

//garc
#include <arc_index/field_effect_2d.gaix>
#include <arc_index/field_effect_3d.gaix>
#include <arc_def_index/arc_def.h> 

// EffectContents
#include "Field/FieldRo/include/Effect/content/FieldEffectActionButton.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation1.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectQuestion.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLensFlare.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectEncGrass.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFade.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkRun.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkWait.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkJet.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadowFoot.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectPlaceName.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRock.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerEncount.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRideLight.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDummyPolygon.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGimmickEncount.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerFocus.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrialStart.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFadeSpot.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFesMessage.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFesLevelUp.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFesShopOpen.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFesStartSplash.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFesWarp.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRain.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSnow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSandStorm.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFishingBuoy.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLaplaceFlow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLaplaceWait.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectBallDemo.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRotom.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootWater.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootSand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootLand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideKenta.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideMurando.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideKairiky.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideZimenba.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFestivalFire.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectReport.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2RunSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2WalkSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2AccelSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaBreath.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaRipple.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGetItem.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDemoRide.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectBaseParticle.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectBaseModel.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial3.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDenjumokuThunder.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5ForModel.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDenjumokuThunder2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKamitsurugiSlash.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRotomCutIn.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFureaiIconBase.h"

// conv_header
#include "niji_conv_header/field/effect/rain.h"
#include "niji_conv_header/field/effect/snow.h"
#include "niji_conv_header/field/effect/festival_fire.h"
#include "niji_conv_header/field/effect/kairiky_rock_smoke.h"
#include "niji_conv_header/field/effect/kairiky_rock_down.h"
#include "niji_conv_header/field/effect/shark_run.h"
#include "niji_conv_header/field/effect/laplace_flow.h"
#include "niji_conv_header/field/effect/chara_ripple.h"
#include "niji_conv_header/field/effect/dash_smoke.h"
#include "niji_conv_header/field/effect/shark_jet.h"
#include "niji_conv_header/field/effect/roba02_run_smoke.h"
#include "niji_conv_header/field/effect/roba2_walk_smoke.h"
#include "niji_conv_header/field/effect/roba2_accel_smoke.h"
#include "niji_conv_header/field/effect/rockbreak_g.h"
#include "niji_conv_header/field/effect/rockbreak_w.h"
#include "niji_conv_header/field/effect/gimmick_encount.h"
#include "niji_conv_header/field/effect/foot_water.h"
#include "niji_conv_header/field/effect/foot_sand.h"
#include "niji_conv_header/field/effect/chara_breath.h"
#include "niji_conv_header/field/effect/demo_ride.h"
#include "niji_conv_header/field/effect/demo_trial2.h"
#include "niji_conv_header/field/effect/fes_levelup.h"
#include "niji_conv_header/field/effect/fes_shop_open.h"
#include "niji_conv_header/field/effect/fes_start_splash.h"
#include "niji_conv_header/field/effect/fes_warp.h"
#include "niji_conv_header/field/effect/step_smoke.h"
#include "niji_conv_header/field/effect/demo_trial7_1.h"
#include "niji_conv_header/field/effect/demo_trial7_2.h"
#include "niji_conv_header/field/effect/bag_effect.h"
#include "niji_conv_header/field/effect/demo_trial3.h"
#include "niji_conv_header/field/effect/demo_trial5.h"
#include "niji_conv_header/field/effect/demo_concentrate.h"
#include "niji_conv_header/field/effect/demo_firework_pink.h"
#include "niji_conv_header/field/effect/demo_firework_purple.h"
#include "niji_conv_header/field/effect/demo_firework_red.h"
#include "niji_conv_header/field/effect/demo_firework_yellow.h"
#include "niji_conv_header/field/effect/demo_flare_moon.h"
#include "niji_conv_header/field/effect/demo_flare_sun.h"
#include "niji_conv_header/field/effect/demo_flower_pink.h"
#include "niji_conv_header/field/effect/demo_flower_yellow.h"
#include "niji_conv_header/field/effect/demo_fog.h"
#include "niji_conv_header/field/effect/ef_ph0301_den.h"
#include "niji_conv_header/field/effect/ef_ph0301_den2.h"
#include "niji_conv_header/field/effect/demo_new_trial5_01.h"
#include "niji_conv_header/field/effect/demo_new_trial5_02.h"
#include "niji_conv_header/field/effect/kairiky_rock_smoke_l.h"
#include "niji_conv_header/field/effect/kairiky_rock_down_l.h"
#include "niji_conv_header/field/effect/ub_kami_slash.h"
#include "niji_conv_header/field/effect/ub_kami_blackout.h"
#include "niji_conv_header/field/effect/rotom_power.h"
#include "niji_conv_header/field/effect/demo_trial2_2.h"
#include "niji_conv_header/field/effect/r_rocket1.h"
#include "niji_conv_header/field/effect/ef_Btfes_warp.h"
#include "niji_conv_header/field/world/world_id.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

//! エフェクト用ヒープサイズ：CTR版
static const s32 FIELD_EFFECT_SYS_HEAP_SIZE = 0x10000;
static const s32 FIELD_EFFECT_WTR_HEAP_SIZE =  0x6000;

// 要素数あふれの際に使用する不正なインデックス
static const s32 ERROR_INDEX = -1;

/**
 *  @brief  コンストラクタ
 */
EffectManager::EffectManager( void )
  : m_pFieldRootNode( NULL )
  , m_pPipeLine( NULL )
  , m_pResource( NULL )
  , m_pCameraManager( NULL )
  , m_pWindowManager( NULL )
  , m_pEffectHeap( NULL )
  , m_pEffectSystem( NULL )
  , m_bInitiallized( false )
  , m_pPlaceName( NULL )
  , m_pTrainerFocus( NULL )
  , m_pWordSetLoader( NULL )
  , m_pFileManager(NULL)
  , m_cpEncountGrassColorTable(NULL)
  , m_WorldID( WORLD_ERROR )
{
  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    m_pEffectArray[ i ]     = NULL;
    m_pEffectHeapArray[ i ] = NULL;
  }
  for( u32 i = 0; i < Effect::EFFECT_TYPE_MAX; ++ i )
  {
    m_pDynamicResource[ i ] = NULL;
    m_DynamicResourceSize[ i ] = 0;
  }
}

/**
 *  @brief  デストラクタ
 */
EffectManager::~EffectManager( void )
{
}

/**
 *  @brief  初期化：常駐エフェクト
 */
void EffectManager::InitializeCommon( void *pCommonPack, void *pLayoutPack )
{
  m_CommonResource.Initialize( pCommonPack );
  m_LayoutResource.Initialize( pLayoutPack );
}

/**
 *  @brief  初期化
 */
void EffectManager::Initialize( const SetupParam &param )
{
  gfl2::heap::HeapBase *heap = param.pHeapBase;

  if( m_bInitiallized )
  {
    // すでに初期化されている
    GFL_PRINT( "FieldEffectManager Still Initialized !!\n" );
    return;
  }

  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    // 配列をクリア
    m_pEffectArray[ i ] = NULL;
    // ヒープ配列を確保
    if( SYS_EFFECT_NUM <= i && i < SYS_EFFECT_NUM + WTR_EFFECT_NUM )
    {
      m_pEffectHeapArray[ i ] = GFL_CREATE_LOCAL_HEAP_NAME( heap, FIELD_EFFECT_WTR_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldEffectWtr");
    }
    else
    {
      m_pEffectHeapArray[ i ] = GFL_CREATE_LOCAL_HEAP_NAME( heap, FIELD_EFFECT_SYS_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldEffectNormal");
    }
  }

  // メンバー設定
  m_pFieldRootNode  = param.pRootNode;
  m_pPipeLine       = param.pPipeLine;
  m_pResource       = param.pResource;
  m_pCameraManager  = param.pCameraManager;
  m_pWindowManager  = param.pWindowManager;
  m_pEffectHeap     = param.pEffectHeap;
  m_pEffectSystem   = param.pEffectSystem;
  m_pWordSetLoader  = param.pWordSetLoader;
  m_pFileManager    = param.pFileManager;
  m_cpEncountGrassColorTable = param.cpEncountGrassColorTable;
  m_WorldID         = param.worldID;

  // ARCファイルオープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_FIELD_EFFECT_3D;
    openReq.heapForFile = heap;
    openReq.heapForArcSrc = heap->GetLowerHandle();
    openReq.heapForReq = heap->GetLowerHandle();
    m_pFileManager->AddArcFileOpenReq( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_FIELD_EFFECT_2D;
    openReq.heapForFile = heap;
    openReq.heapForArcSrc = heap->GetLowerHandle();
    openReq.heapForReq = heap->GetLowerHandle();
    m_pFileManager->AddArcFileOpenReq( openReq );
  }

  // エフェクトごとの処理
  {
    gfl2::fs::BinLinkerAccessor accessor;
    // ケンタロスけむり
    {
      void *resource = GetResource( EFFECT_TYPE_DASH_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DASH_SMOKE_EF_RIDE_KENTA_DASH_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_DASH_SMOKE );
    }
    // サメハダーのジェット
    {
      void *resource = GetResource( EFFECT_TYPE_SHARK_JET );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SHARK_JET_EF_RIDE_SAME_JET_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SHARK_JET );
    }
    // 地面馬けむり(荒地アクセル)
    {
      void *resource = GetResource( EFFECT_TYPE_ROBA2_RUN_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_ROBA02_RUN_SMOKE_EF_RIDE_ZIMENBA_RUN_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_ROBA2_RUN_SMOKE );
    }
    // 地面馬けむり(荒地通常)
    {
      void *resource = GetResource( EFFECT_TYPE_ROBA2_WALK_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_ROBA2_WALK_SMOKE_EF_RIDE_ZIMENBA_WALK_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_ROBA2_WALK_SMOKE );
    }
    // 地面馬けむり(アクセル)
    {
      void *resource = GetResource( EFFECT_TYPE_ROBA2_ACCEL_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_ROBA2_ACCEL_SMOKE_EF_RIDE_ZIMENBA_DASH_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_ROBA2_ACCEL_SMOKE );
    }
    // 岩(土)
    {
      void *resource = GetResource( EFFECT_TYPE_ROCKBREAK_G );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_ROCKBREAK_G_EF_RIDE_ROCKBREAK_G_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_ROCK_G );
    }
    // 岩(水)
    {
      void *resource = GetResource( EFFECT_TYPE_ROCKBREAK_W );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_ROCKBREAK_W_EF_RIDE_ROCKBREAK_W_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_ROCK_W );
    }
    // 葉が飛び散る(通常の木)
    {
      void *resource = GetResource( EFFECT_TYPE_GIMMICK_ENCOUNT );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_GIMMICK_ENCOUNT_EF_GIMMICK_GRASS01_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SCATTER_GRASS_TREE_0 );
    }
    // 葉が飛び散る(通常の植物)
    {
      void *resource = GetResource( EFFECT_TYPE_GIMMICK_ENCOUNT );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_GIMMICK_ENCOUNT_EF_GIMMICK_GRASS02_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SCATTER_GRASS_PLANT_0 );
    }
    // 雨　空
    {
      void *resource = GetResource( EFFECT_TYPE_RAIN );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_RAIN_EF_WEATHER_RAIN_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SKY_RAIN );
    }
    // 雨　地面
    {
      void *resource = GetResource( EFFECT_TYPE_RAIN );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_RAIN_EF_WEATHER_RAIN_UNDER_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_GROUND_RAIN );
    }
    // 雪　空
    {
      void *resource = GetResource( EFFECT_TYPE_SNOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SNOW_EF_WEATHER_SNOW_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SKY_SNOW );
    }
    // 吹雪　空
    {
      void *resource = GetResource( EFFECT_TYPE_SNOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SNOW_EF_WEATHER_SNOW_STORM_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SKY_SNOWSTORM );
    }
    // ダイヤモンド　空
    {
      void *resource = GetResource( EFFECT_TYPE_SNOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SNOW_EF_WEATHER_DIAMONDDUST_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SKY_DIAMONDDUST );
    }
    // ダイヤモンド　地面
    {
      void *resource = GetResource( EFFECT_TYPE_SNOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SNOW_EF_WEATHER_DIAMONDDUST_UNDER_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_GROUND_DIAMONDDUST );
    }

    {
      void *resource = GetResource( EFFECT_TYPE_FOOT_WATER );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_FOOT_WATER_EF_CHARA_WATER_FOOT_SPRAY_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_FOOT_WATER );
    }
    {
      void *resource = GetResource( EFFECT_TYPE_FOOT_SAND );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_FOOT_SAND_EF_CHARA_SAND_FOOT_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_FOOT_SAND );
    }
    // といき
    {
      void *resource = GetResource( EFFECT_TYPE_CHARA_BREATH );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_CHARA_BREATH_EF_CHARA_BREATH_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_CHARA_BREATH );
    }
    // サメハダーの走りしぶき
    {
      void *resource = GetResource( EFFECT_TYPE_SHARK_RUN );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_SHARK_RUN_EF_RIDE_SAME_RUN_FLOW_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_SHARK_RUN );
    }
    // ラプラスの走りしぶき
    {
      void *resource = GetResource( EFFECT_TYPE_LAPLACE_FLOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_LAPLACE_FLOW_EF_RIDE_LAPU_RUN_FLOW_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_LAPLACE_FLOW );
    }
    // NPC泳ぎ波紋
    {
      void *resource = GetResource( EFFECT_TYPE_CHARA_RIPPLE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_CHARA_RIPPLE_EF_CHARA_SWIMMER_FLOW_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_CHARA_RIPPLE );
    }
    // 段差ジャンプ
    {
      void *resource = GetResource( EFFECT_TYPE_STEP_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_STEP_SMOKE_EF_CHARA_STEP_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( m_pEffectHeap, pEmitter, EFFECT_RESOURCE_STEP_SMOKE );
    }
  }

  // 地名表示を作成する
  if( m_pWordSetLoader )
  {
    gfl2::math::Vector3 dst = gfl2::math::Vector3::GetZero();
    m_pPlaceName = reinterpret_cast<Field::Effect::EffectPlaceName*>( CreateEffect( Field::Effect::EFFECT_TYPE_PLACENAME, dst ) );
  }

  {
    gfl2::math::Vector3 dst = gfl2::math::Vector3::GetZero();
    m_pTrainerFocus = reinterpret_cast<Field::Effect::EffectTrainerFocus*>( CreateEffect( Field::Effect::EFFECT_TYPE_TRAINER_FOCUS, dst ) );
  }

}

/**
 *  @brief  初期化の完了待ち
 */
bool EffectManager::InitializeWait()
{
  if(m_bInitiallized == false)
  {
    if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_EFFECT_3D ) )
    {
      return false;
    }
    if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_EFFECT_2D ) )
    {
      return false;
    }
    
    // 初期化を完了
    m_bInitiallized = true;
  }

  return true;
}

/**
 *  @brief  破棄
 */
void EffectManager::Terminate( void )
{
  // 全エフェクト破棄
  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( m_pEffectArray[ i ] )
    {
      m_pEffectArray[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pEffectArray[ i ] );
    }
    // ヒープ配列を解放
    GFL_DELETE_HEAP( m_pEffectHeapArray[ i ] );
  }

  // 非常駐リソースの破棄
  for( u32 i = 0; i < EFFECT_TYPE_MAX; ++ i )
  {
    this->UnLoadDynamicEffectResource( static_cast<Field::Effect::Type>(i), m_pEffectHeap->GetGFHeapBase(), true );// 間借り
  }

  // 登録リソースの破棄
  {
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_FOOT_SAND );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_FOOT_WATER );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_GROUND_RAIN );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SKY_RAIN );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_GROUND_DIAMONDDUST );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SKY_DIAMONDDUST );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SKY_SNOWSTORM );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SKY_SNOW );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SCATTER_GRASS_PLANT_0 );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SCATTER_GRASS_TREE_0 );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_ROCK_W );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_ROCK_G );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SHARK_JET );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_DASH_SMOKE );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_ROBA2_RUN_SMOKE );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_ROBA2_WALK_SMOKE );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_ROBA2_ACCEL_SMOKE );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_CHARA_BREATH );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_SHARK_RUN );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_LAPLACE_FLOW );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_CHARA_RIPPLE );
    m_pEffectSystem->ClearResource( m_pEffectHeap, Field::Effect::EFFECT_RESOURCE_STEP_SMOKE );
  }


  // ARCファイルクローズ
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_FIELD_EFFECT_3D;
    closeReq.heapForReq = m_pEffectHeap->GetGFHeapBase()->GetLowerHandle(); // 間借り
    m_pFileManager->SyncArcFileClose( closeReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_FIELD_EFFECT_2D;
    closeReq.heapForReq = m_pEffectHeap->GetGFHeapBase()->GetLowerHandle(); // 間借り
    m_pFileManager->SyncArcFileClose( closeReq );
  }
}

/**
 *  @brief  更新処理
 */
void EffectManager::Update( void )
{
  // 未初期化時点での更新を抑制
  if( !m_bInitiallized )
  {
    return;
  }

  // 各エフェクトの更新処理
  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( m_pEffectArray[ i ] == NULL )
      continue;

    if( !m_pEffectArray[ i ]->IsSafeDelete() )
    {
      // 通常更新
      m_pEffectArray[ i ]->Update();
    }
    else
    {
      // 破棄確定にて解放を行う
      m_pEffectArray[ i ]->Terminate();
      GFL_SAFE_DELETE( m_pEffectArray[ i ] );
    }
  }
}

/**
 *  @brief 非常駐リソースの読み込み
 *  @param  in_eType  エフェクト種別
 *  @param  in_pHeap  読み込みヒープ
 */
void EffectManager::LoadDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* in_pHeap, b32 sync )
{
  s32 index_3d = -1;
  s32 index_2d = -1;

  index_3d = GetDynamicResourceIndex3d( in_eType );
  index_2d = GetDynamicResourceIndexLyt( in_eType );

  // すでに読み込まれていないか？
  if( IsLoadedEffectResource( in_eType ) )
  {
    return ;
  }

  // 非常駐リソースとして登録されていない？
  if( !((index_3d != -1) || (index_2d != -1))  ) { GFL_ASSERT( (index_3d != -1) || (index_2d != -1) );  return ; }

  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  if( index_3d >= 0 )
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_FIELD_EFFECT_3D;
    loadReq.datId = index_3d;
    loadReq.ppBuf = &m_pDynamicResource[in_eType];
    loadReq.heapForBuf = in_pHeap;
    loadReq.heapForReq = in_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = in_pHeap->GetLowerHandle();
    loadReq.pBufSize = &m_DynamicResourceSize[in_eType];
    loadReq.align = 128;
    if( !sync )
    {
      m_pFileManager->AddArcFileLoadDataReq( loadReq );
    }
    else
    {
      m_pFileManager->SyncArcFileLoadData( loadReq );
    }
  }
  else if( index_2d >= 0 )
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_FIELD_EFFECT_2D;
    loadReq.datId = index_2d;
    loadReq.ppBuf = &m_pDynamicResource[in_eType];
    loadReq.heapForBuf = in_pHeap;
    loadReq.heapForReq = in_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = in_pHeap->GetLowerHandle();
    loadReq.pBufSize = &m_DynamicResourceSize[in_eType];
    loadReq.align = 128;
    if( !sync )
    {
      m_pFileManager->AddArcFileLoadDataReq( loadReq );
    }
    else
    {
      m_pFileManager->SyncArcFileLoadData( loadReq );
    }
  }
  else
  {
    GFL_ASSERT(0); //ありえない
  }
}

/**
 *  @brief 非常駐リソースの読み込み
 *  @param  in_eType  エフェクト種別
 */
void EffectManager::SetupDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* in_pHeap )
{
  gfl2::Effect::GFHeap* pHeap = GFL_NEW( in_pHeap->GetLowerHandle() ) gfl2::Effect::GFHeap( in_pHeap );
  gfl2::fs::BinLinkerAccessor accessor;
  switch(in_eType)
  {
  case EFFECT_TYPE_RIDE_APPER_LAND:
    // ライド乗り降り演出　陸用
    {
      void *resource = GetResource( EFFECT_TYPE_RIDE_APPER_LAND );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( 0 );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_RIDE_APPER_LAND );
    }
    break;

  case EFFECT_TYPE_RIDE_APPER_SEA:
    // ライド乗り降り演出　海用
    {
      void *resource = GetResource( EFFECT_TYPE_RIDE_APPER_SEA );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( 0 );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_RIDE_APPER_SEA );
    }
    break;

  case EFFECT_TYPE_B_DEMO:
    // モンスターボール演出用
    {
      void *resource = GetResource( EFFECT_TYPE_B_DEMO );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( 0 );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_BALL_DEMO );
    }
    break;

  case EFFECT_TYPE_FESTIVAL_FIRE:
    // まつり演出用
    {
      void *resource = GetResource( EFFECT_TYPE_FESTIVAL_FIRE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_FESTIVAL_FIRE_EF_B_DEMO_FESTIVAL_FIRE_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_FESFIRE );
    }
    break;

  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:
    // カイリキ岩
    {
      void *resource = GetResource( EFFECT_TYPE_KAIRIKY_ROCK_SMOKE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_KAIRIKY_ROCK_SMOKE_EF_RIDE_KAIRIKY_ROCK_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE );
    }
    break;

  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN:
    // カイリキ岩
    {
      void *resource = GetResource( EFFECT_TYPE_KAIRIKY_ROCK_DOWN );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_KAIRIKY_ROCK_DOWN_EF_RIDE_KAIRIKY_ROCK_DOWN_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN );
    }
    break;

  case EFFECT_TYPE_DEMO_RIDE:
    // イベント初回ライド演出用
    {
      void *resource = GetResource( EFFECT_TYPE_DEMO_RIDE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_RIDE_EF_B_DEMO_RIDE_FLY_FLASH_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_RIDE );
    }
    break;

  case EFFECT_TYPE_DEMO_TRIAL2:
    // イベント試練２演出用
    {
      void *resource = GetResource( EFFECT_TYPE_DEMO_TRIAL2 );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL2_EF_B_DEMO_TRIAL2_WATER_SPLASH_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL2 );
    }
    break;


  case  EFFECT_TYPE_FES_LEVELUP:
    /*  JoinFesta(FesCircle):エフェクト  */
    {
      void *resource = GetResource( EFFECT_TYPE_FES_LEVELUP );
      accessor.Initialize( resource );
      m_pEffectSystem->EntryResource( pHeap, accessor.GetData( BL_IDX_FES_LEVELUP_EF_JOINFESTA_SCREEN_KIRA_BPTCL  ), EFFECT_RESOURCE_JF_LEVELUP);
    }
    break;

  case  EFFECT_TYPE_FES_SHOP_OPEN:
    /*  JoinFesta(FesCircle):エフェクト  */
    {
      void *resource = GetResource( EFFECT_TYPE_FES_SHOP_OPEN );
      accessor.Initialize( resource );
      m_pEffectSystem->EntryResource( pHeap, accessor.GetData( BL_IDX_FES_SHOP_OPEN_EF_JOINFEST_SCREEN_CONFETTI_BPTCL  ), EFFECT_RESOURCE_JF_SHOP_OPEN);
    }
    break;

  case  EFFECT_TYPE_FES_START_SPLASH:
    /*  JoinFesta(FesCircle):エフェクト  */
    {
      void *resource = GetResource( EFFECT_TYPE_FES_START_SPLASH );
      accessor.Initialize( resource );
      m_pEffectSystem->EntryResource( pHeap, accessor.GetData( BL_IDX_FES_START_SPLASH_EF_JOINFESTA_START_SPLASH_BPTCL  ), EFFECT_RESOURCE_JF_START_SPLASH);
    }
    break;

  case  EFFECT_TYPE_FES_WARP:
    /*  JoinFesta(FesCircle):エフェクト  */
    {
      void *resource = GetResource( EFFECT_TYPE_FES_WARP );
      accessor.Initialize( resource );
      m_pEffectSystem->EntryResource( pHeap, accessor.GetData( BL_IDX_FES_WARP_EF_JOINFESTA_WARP_BPTCL  ), EFFECT_RESOURCE_JF_WARP);
/*
	BL_IDX_FES_WARP_EF_JOINFESTA_WARP_CTR_GFBMDLP,
	BL_IDX_FES_WARP_EF_JOINFESTA_WARP_GFBMOT,
*/
    }
    break;

  case EFFECT_TYPE_DEMO_TRIAL7_1:
    // イベント試練２演出用
    {
      void *resource = GetResource( EFFECT_TYPE_DEMO_TRIAL7_1 );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL7_1_EF_B_DEMO_TRIAL7_SMOKE_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL7_1 );
    }
    break;

  case EFFECT_TYPE_DEMO_TRIAL3:
    // イベント試練２演出用
    {
      void *resource = GetResource( EFFECT_TYPE_DEMO_TRIAL3 );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL3_EF_B_DEMO_TRIAL3_01_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL3_1 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL3_EF_B_DEMO_TRIAL3_02_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL3_2 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL3_EF_B_DEMO_TRIAL3_03_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL3_3 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL3_EF_B_DEMO_TRIAL3_04_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL3_4 );
    }
    break;

  case EFFECT_TYPE_DEMO_TRIAL5:
    // イベント試練２演出用
    {
      void *resource = GetResource( EFFECT_TYPE_DEMO_TRIAL5 );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL5_EF_B_DEMO_TRIAL5_03_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL5_3 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL5_EF_B_DEMO_TRIAL5_04_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL5_4 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL5_EF_B_DEMO_TRIAL5_05_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL5_5 );
      pEmitter = accessor.GetData( BL_IDX_DEMO_TRIAL5_EF_B_DEMO_TRIAL5_06_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL5_6 );
    }
    break;

  case EFFECT_TYPE_DEMO_FLOWER_YELLOW:
    {
      // 花びら：黄
      void *resource = GetResource( EFFECT_TYPE_DEMO_FLOWER_YELLOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FLOWER_YELLOW_EF_B_DEMO_FLOWER_YELLOW_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FLOWER_YELLOW );
    }
    break;
  case EFFECT_TYPE_DEMO_FLOWER_PINK:
    {
      // 花びら：桃
      void *resource = GetResource( EFFECT_TYPE_DEMO_FLOWER_PINK );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FLOWER_PINK_EF_B_DEMO_FLOWER_PINK_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FLOWER_PINK );
    }
    break;
  case EFFECT_TYPE_DEMO_FIREWORK_YELLOW:
    {
      // 花火：黄
      void *resource = GetResource( EFFECT_TYPE_DEMO_FIREWORK_YELLOW );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FIREWORK_YELLOW_EF_B_DEMO_HANABI_YELLOW_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FIREWORK_YELLOW );
    }
    break;
  case EFFECT_TYPE_DEMO_FIREWORK_PINK:
    {
      // 花火：桃
      void *resource = GetResource( EFFECT_TYPE_DEMO_FIREWORK_PINK );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FIREWORK_PINK_EF_B_DEMO_HANABI_PINK_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FIREWORK_PINK );
    }
    break;
  case EFFECT_TYPE_DEMO_FIREWORK_RED:
    {
      // 花火：赤
      void *resource = GetResource( EFFECT_TYPE_DEMO_FIREWORK_RED );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FIREWORK_RED_EF_B_DEMO_HANABI_RED_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FIREWORK_RED );
    }
    break;
  case EFFECT_TYPE_DEMO_FIREWORK_PURPLE:
    {
      // 花火：紫
      void *resource = GetResource( EFFECT_TYPE_DEMO_FIREWORK_PURPLE );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FIREWORK_PURPLE_EF_B_DEMO_HANABI_PURPLE_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FIREWORK_PURPLE );
    }
    break;
  case EFFECT_TYPE_DEMO_FLARE_SUN:
    {
      // レンズフレア：太陽
      void *resource = GetResource( EFFECT_TYPE_DEMO_FLARE_SUN );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FLARE_SUN_EF_B_DEMO_FLARE_SUN_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FLARE_SUN );
    }
    break;
  case EFFECT_TYPE_DEMO_FLARE_MOON:
    {
      // レンズフレア：月
      void *resource = GetResource( EFFECT_TYPE_DEMO_FLARE_MOON );
      accessor.Initialize( resource );
      void *pEmitter = accessor.GetData( BL_IDX_DEMO_FLARE_MOON_EF_B_DEMO_FLARE_MOON_BPTCL );
      m_pEffectSystem->EntryResource( pHeap, pEmitter, EFFECT_RESOURCE_DEMO_FLARE_MOON );
    }
    break;

  case EFFECT_TYPE_EF_PH0301_DEN:
    // デンジュモクマップの雷用
  {
    void *resource = GetResource(EFFECT_TYPE_EF_PH0301_DEN);
    accessor.Initialize(resource);
    void *pEmitter = accessor.GetData(BL_IDX_EF_PH0301_DEN_EF_PH0301_DEN_EXP01_BPTCL);
    m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DENJUMOKU_EXP); 
    pEmitter = accessor.GetData(BL_IDX_EF_PH0301_DEN_EF_PH0301_DEN_THUNDER01_BPTCL);
    m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DENJUMOKU_THUNDER);
  }
  break;
  case EFFECT_TYPE_DEMO_NEW_TRIAL5_01:
    // 電気試練用エフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_DEMO_NEW_TRIAL5_01);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_01_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_01_LINE);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_02_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_02_STR_LINE);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_03_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_03_STR_SLANT);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_KUWA_CHARGE_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CHARGE);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_TOGE_CHARGE_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_CHARGE);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_TOGE_END_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_END);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_Z_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_Z);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_04_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_04_WEAK_SLANT);
      pEmitter = accessor.GetData(BL_IDX_DEMO_NEW_TRIAL5_01_EF_B_DEMO_NEW_TRIAL5_KUWA_CRASH_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CRASH);
    }
    break;

  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L:
    // 巨大カイリキ岩（フェローチェマップ用）
    {
      void *resource = GetResource(EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_KAIRIKY_ROCK_SMOKE_L_EF_RIDE_KAIRIKY_ROCK_SMOKE_L_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE_L);
    }
    break;

  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L:
    // 巨大カイリキ岩（フェローチェマップ用）
    {
      void *resource = GetResource(EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_KAIRIKY_ROCK_DOWN_L_EF_RIDE_KAIRIKY_ROCK_DOWN_L_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN_L);
    }
    break;
  case EFFECT_TYPE_EF_PH0301_DEN2:
    // デンジュモクマップの雷2用
    {
      void *resource = GetResource(EFFECT_TYPE_EF_PH0301_DEN2);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_EF_PH0301_DEN2_EF_PH0301_DEN_THUNDER02_ROOT_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DENJUMOKU_THUNDER2_ROOT);
      pEmitter = accessor.GetData(BL_IDX_EF_PH0301_DEN2_EF_PH0301_DEN_THUNDER02_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DENJUMOKU_THUNDER2);
    }
    break;
  case EFFECT_TYPE_UB_KAMI_SLASH:
    // カミツルギイベント用斬撃エフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_UB_KAMI_SLASH);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_UB_KAMI_SLASH_EF_B_DEMO_UBFIELD_00_KAMISLASH_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_UB_KAMI_SLASH);
    }
    break;
  case EFFECT_TYPE_UB_KAMI_BLACKOUT:
    // カミツルギイベント用暗転エフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_UB_KAMI_BLACKOUT);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_UB_KAMI_BLACKOUT_EF_B_DEMO_UBFIELD_00_BLACKOUT_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_UB_KAMI_BLACKOUT);
    }
    break;
  case EFFECT_TYPE_ROTOM_POWER:
    // ロトムパワーエフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_ROTOM_POWER);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_ROTOM_POWER_EF_ROTOM_POWER_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_ROTOM_POWER);
    }
    break;

  case EFFECT_TYPE_DEMO_TRIAL2_2:
    // イベント試練２演出用その２
    {
      void *resource = GetResource(EFFECT_TYPE_DEMO_TRIAL2_2);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_DEMO_TRIAL2_2_EF_B_DEMO_TRIAL2_WATER_SPLASH2_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_DEMO_TRIAL2_2);
    }
    break;

    case EFFECT_TYPE_R_ROCKET1:
    // レインボーロケット団用エフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_R_ROCKET1);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_R_ROCKET1_EF_B_DEMO_RROCKET_WARP_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_R_ROCKET_1);
    }
    break;

    case EFFECT_TYPE_EF_BTFES_WARP:
    // バトルフェス用エフェクト
    {
      void *resource = GetResource(EFFECT_TYPE_EF_BTFES_WARP);
      accessor.Initialize(resource);
      void *pEmitter = accessor.GetData(BL_IDX_EF_BTFES_WARP_EF_BTFES_WARP_BPTCL);
      m_pEffectSystem->EntryResource(pHeap, pEmitter, EFFECT_RESOURCE_EF_BTFES_WARP);
    }
    break;
  }

  GFL_SAFE_DELETE( pHeap );
}


/**
 *  @brief 非常駐リソースの読み込みが完了しているか確認
 */
b32 EffectManager::IsLoadedEffectResource( Field::Effect::Type in_eType ) const
{
  if( !(in_eType < EFFECT_TYPE_MAX) )
  {
    return false;
  }

  // 常駐エフェクト
  if( m_CommonResource.GetDataSize( in_eType ) != 0 )
  {
    return true;
  }
  // レイアウト
  if( m_LayoutResource.GetDataSize( in_eType ) != 0 )
  {
    // 指定リソースの返却
    return true;
  }

  // 読み込み完了待ち
  if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_EFFECT_3D ) ){
    return false;
  }
  if( !m_pFileManager->IsArcFileOpenFinished( ARCID_FIELD_EFFECT_2D ) ){
    return false;
  }

  // ファイルの読み込みが完了しているか？
  if( m_pFileManager->IsArcFileLoadDataFinished( const_cast<void**>(&m_pDynamicResource[in_eType] )) )
  {
    if( m_pDynamicResource[in_eType] )
    {
      return true;
    }
  }

  return false;
}

/**
 *  @brief 非常駐リソースの破棄
 *  @param  in_eType  エフェクト種別
 */
void EffectManager::UnLoadDynamicEffectResource( Field::Effect::Type in_eType, gfl2::heap::HeapBase* pHeapBase, bool isTerminate /*=false*/ )
{
  if( in_eType < EFFECT_TYPE_MAX )
  {
    if(m_pDynamicResource[in_eType])
    {
      if( isTerminate )
      {
        // 破棄時のチェック
        GFL_ASSERT_MSG( GetEffectNumPreTerminate(in_eType)<=0, "Effect still alive(type=%d)\n", in_eType );
      }
      else
      {
        // 通常時のチェック
        GFL_ASSERT_MSG( GetEffectNum(in_eType)<=0, "Effect still alive(type=%d)\n", in_eType );
      }
      gfl2::Effect::GFHeap* pHeap = GFL_NEW_LOW( pHeapBase ) gfl2::Effect::GFHeap( pHeapBase );
      switch(in_eType)
      {
      case Field::Effect::EFFECT_TYPE_RIDE_APPER_LAND:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_RIDE_APPER_LAND );
        }
        break;
      case Field::Effect::EFFECT_TYPE_RIDE_APPER_SEA:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_RIDE_APPER_SEA );
        }
        break;
      case Field::Effect::EFFECT_TYPE_B_DEMO:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_BALL_DEMO );
        }
        break;
      case Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_FESFIRE );
        }
        break;
      case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE );
        }
        break;
      case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_RIDE:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_RIDE );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_TRIAL2:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL2 );
        }
        break;
      case  EFFECT_TYPE_FES_LEVELUP:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_JF_LEVELUP );
        }
        break;

      case  EFFECT_TYPE_FES_SHOP_OPEN:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_JF_SHOP_OPEN );
        }
        break;
      case  EFFECT_TYPE_FES_START_SPLASH:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_JF_START_SPLASH );
        }
        break;
      case  EFFECT_TYPE_FES_WARP:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_JF_WARP );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_TRIAL7_1:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL7_1 );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_TRIAL3:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL3_1 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL3_2 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL3_3 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL3_4 );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_TRIAL5:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL5_3 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL5_4 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL5_5 );
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_TRIAL5_6 );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FLOWER_YELLOW:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FLOWER_YELLOW );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FLOWER_PINK:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FLOWER_PINK );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_YELLOW:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FIREWORK_YELLOW );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PINK:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FIREWORK_PINK );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_RED:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FIREWORK_RED );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PURPLE:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FIREWORK_PURPLE );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FLARE_SUN:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FLARE_SUN );
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_FLARE_MOON:
        {
          m_pEffectSystem->ClearResource( pHeap, EFFECT_RESOURCE_DEMO_FLARE_MOON );
        }
        break;
      case Field::Effect::EFFECT_TYPE_EF_PH0301_DEN:
      {
        m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DENJUMOKU_THUNDER);
        m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DENJUMOKU_EXP);
      }
      break;
      case Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_01_LINE);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_02_STR_LINE);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_03_STR_SLANT);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CHARGE);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_CHARGE);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_END);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_Z);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_04_WEAK_SLANT);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CRASH);
        }
        break;
      case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE_L);
        }
        break;
      case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN_L);
        }
        break;
      case Field::Effect::EFFECT_TYPE_EF_PH0301_DEN2:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DENJUMOKU_THUNDER2);
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DENJUMOKU_THUNDER2_ROOT);
        }
        break;
      case Field::Effect::EFFECT_TYPE_UB_KAMI_SLASH:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_UB_KAMI_SLASH);
        }
        break;
      case Field::Effect::EFFECT_TYPE_UB_KAMI_BLACKOUT:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_UB_KAMI_BLACKOUT);
        }
        break;
      case Field::Effect::EFFECT_TYPE_ROTOM_POWER:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_ROTOM_POWER);
        }
        break;
      case Field::Effect::EFFECT_TYPE_DEMO_TRIAL2_2:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_DEMO_TRIAL2_2);
        }
        break;
      case Field::Effect::EFFECT_TYPE_R_ROCKET1:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_R_ROCKET_1);
        }
        break;
      case Field::Effect::EFFECT_TYPE_EF_BTFES_WARP:
        {
          m_pEffectSystem->ClearResource(pHeap, EFFECT_RESOURCE_EF_BTFES_WARP);
        }
        break;
      }
      GFL_SAFE_DELETE( pHeap );
      GflHeapSafeFreeMemory( m_pDynamicResource[in_eType] );
    }
  }
}


/**
 *  @brief  エフェクト生成：オブジェクト指定
 */
Field::Effect::IEffectBase *EffectManager::CreateEffect( Field::Effect::Type in_eType, Field::IField3DObjectNode *in_pObject, bool in_bPlaySe, WorkType workType, s32* pIndex )
{
  if (pIndex)
  {
    *pIndex = ERROR_INDEX;
  }

  Field::Effect::IEffectBase *pEffect = NULL;
  s32 index = GetEmptyIndex( in_eType, workType );
  if( index == ERROR_INDEX )
  {
    GFL_PRINT( "FieldEffectManager is Full\n" );
    return pEffect;
  }
  // リソースが取得できないので返却
  void *pResource = GetResource( in_eType );
  if( pResource == NULL )
  {
    GFL_ASSERT_MSG( false, "EffectResource is NULL\n" );
    return pEffect;
  }
  
  // タイプごとのエフェクトクラスを生成
  switch( in_eType )
  {
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1:
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectExclamation1 *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectExclamation1();
      pBallon->SetPlaySe( in_bPlaySe );
      EffectBallonIcon::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      setup.pCameraManager = m_pCameraManager;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2:
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectExclamation2 *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectExclamation2();
      pBallon->SetPlaySe( in_bPlaySe );
      EffectBallonIcon::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      setup.pCameraManager = m_pCameraManager;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;
  case Field::Effect::EFFECT_TYPE_QUESTION_ICON:
  case Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectQuestion *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectQuestion();
      pBallon->SetPlaySe( in_bPlaySe );
      EffectBallonIcon::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      setup.pCameraManager = m_pCameraManager;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DASH_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectDashSmoke *pDashSmoke = GFL_NEW( pHeapBase ) Field::Effect::EffectDashSmoke();
      EffectDashSmoke::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      pDashSmoke->Initialize( setup );
      m_pEffectArray[ index ] = pDashSmoke;
      pEffect = pDashSmoke;
    }
    break;

  case Field::Effect::EFFECT_TYPE_SHARK_RUN:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectSharkRun *pSharkRun = GFL_NEW( pHeapBase ) Field::Effect::EffectSharkRun();
      EffectSharkRun::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pParentObject = in_pObject;
      pSharkRun->Initialize( setup );
      m_pEffectArray[ index ] = pSharkRun;
      pEffect = pSharkRun;
    }
    break;

  case Field::Effect::EFFECT_TYPE_SHARK_WAIT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectSharkWait *pShark = GFL_NEW( pHeapBase ) Field::Effect::EffectSharkWait();
      EffectSharkWait::SetupData setup;
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      pShark->Initialize( setup );
      m_pEffectArray[ index ] = pShark;
      pEffect = pShark;
    }
    break;

  case Field::Effect::EFFECT_TYPE_SHARK_JET:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectSharkJet *pShark = GFL_NEW( pHeapBase ) Field::Effect::EffectSharkJet();
      EffectSharkJet::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pParentObject = in_pObject;
      pShark->Initialize( setup );
      m_pEffectArray[ index ] = pShark;
      pEffect = pShark;
    }
    break;

  case Field::Effect::EFFECT_TYPE_LAPLACE_WAIT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectLaplaceWait *pLaplace = GFL_NEW( pHeapBase ) Field::Effect::EffectLaplaceWait();
      EffectLaplaceWait::SetupData setup;
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      pLaplace->Initialize( setup );
      m_pEffectArray[ index ] = pLaplace;
      pEffect = pLaplace;
    }
    break;

  case Field::Effect::EFFECT_TYPE_LAPLACE_FLOW:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectLaplaceFlow *pLaplace = GFL_NEW( pHeapBase ) Field::Effect::EffectLaplaceFlow();
      EffectLaplaceFlow::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pParentObject = in_pObject;
      setup.pParentObject = in_pObject;
      pLaplace->Initialize( setup );
      m_pEffectArray[ index ] = pLaplace;
      pEffect = pLaplace;
    }
    break;

  case Field::Effect::EFFECT_TYPE_SHADOW_CIRCLE:
  case Field::Effect::EFFECT_TYPE_SHADOW_DEPTH:
  case Field::Effect::EFFECT_TYPE_SHADOW_WIDTH:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectShadow *pShadow = GFL_NEW( pHeapBase ) Field::Effect::EffectShadow();
      EffectShadow::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      pShadow->Initialize( setup );
      m_pEffectArray[ index ] = pShadow;
      pEffect = pShadow;
    }
    break;

  case Field::Effect::EFFECT_TYPE_SHADOW_FOOT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectShadowFoot *pShadow = GFL_NEW( pHeapBase ) Field::Effect::EffectShadowFoot();
      EffectShadowFoot::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      pShadow->Initialize( setup );
      m_pEffectArray[ index ] = pShadow;
      pEffect = pShadow;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROCKBREAK_G:
  case Field::Effect::EFFECT_TYPE_ROCKBREAK_W:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRock *pRock = GFL_NEW( pHeapBase ) Field::Effect::EffectRock();
      EffectRock::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pParentObject = in_pObject;
      pRock->Initialize( setup );
      m_pEffectArray[ index ] = pRock;
      pEffect = pRock;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FES_MESSAGE:
    {
      gfl2::heap::HeapBase*         pHeapBase = m_pEffectHeapArray[ index ];
      EffectFesMessage::SetupData   setup;
      EffectFesMessage*             pEffectFesMessage;

      setup.pResource           = pResource;
      setup.pHeapBase           = pHeapBase;
      setup.pFieldWindowManager = m_pWindowManager;

      pEffectFesMessage = GFL_NEW( pHeapBase ) Field::Effect::EffectFesMessage();
      pEffectFesMessage->Setup(setup);

      m_pEffectArray[ index ] = pEffectFesMessage;
      pEffect = pEffectFesMessage;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_LAND:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootLand *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootLand();
      EffectFootLand::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootRideKenta *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootRideKenta();
      EffectFootRideKenta::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootRideMurando *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootRideMurando();
      EffectFootRideMurando::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootRideKairiky *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootRideKairiky();
      EffectFootRideKairiky::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootRideZimenba *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootRideZimenba();
      EffectFootRideZimenba::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_REPORT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectReport *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectReport();
      EffectReport::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pParentObject = in_pObject;
      setup.pWinMgr  = m_pWindowManager;
      setup.pCameraManager = m_pCameraManager;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;

  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectKairikyRockSmoke *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectKairikyRockSmoke();
      EffectKairikyRockSmoke::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_CHARA_BREATH:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectCharaBreath *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectCharaBreath();
      EffectCharaBreath::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_CHARA_RIPPLE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectCharaRipple *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectCharaRipple();
      EffectCharaRipple::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROBA2_ACCEL_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRoba2AccelSmoke *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRoba2AccelSmoke();
      EffectRoba2AccelSmoke::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_RIDE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectDemoRide *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectDemoRide();
      p_effect->SetPlaySe( in_bPlaySe );
      EffectDemoRide::SetupData setup;
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;;
      setup.pParentObject = in_pObject;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectKairikyRockSmoke2 *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectKairikyRockSmoke2();
      EffectKairikyRockSmoke2::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.pParentObject = in_pObject;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01:
  {
    // エフェクトの生成
    gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
    EffectNewTrial5 *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectNewTrial5();
    EffectNewTrial5::SetupData setup;
    // 初期化処理
    setup.pResource = pResource;
    setup.pEffectHeap = m_pEffectHeap;
    setup.pEffectSystem = m_pEffectSystem;
    setup.pParentObject = in_pObject;
    setup.position = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
    p_effect->Initialize(setup);
    m_pEffectArray[index] = p_effect;
    pEffect = p_effect;
  }
  break;
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_HEART1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_TEN1:
  case Field::Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1:
  {
    // エフェクトの生成
    gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
    EffectFureaiIconBase *pBallon = GFL_NEW(pHeapBase) Field::Effect::EffectFureaiIconBase();
    pBallon->SetPlaySe(in_bPlaySe);
    EffectFureaiIconBase::SetupData setup;
    // 初期化処理
    setup.baseSetup.pResource     = pResource;
    setup.baseSetup.pHeap         = pHeapBase;
    setup.baseSetup.pRootNode     = m_pFieldRootNode;
    setup.baseSetup.pPipeLine     = m_pPipeLine;
    setup.baseSetup.pParentObject = in_pObject;
    setup.baseSetup.pCameraManager = m_pCameraManager;
    setup.type = in_eType;
    pBallon->Initialize(setup);
    m_pEffectArray[index] = pBallon;
    pEffect = pBallon;
  }
  break;

  default:
    GFL_ASSERT_MSG( false, "CreateEffect(mode:character) Failed!! EffectType is not exist Type(%d)", in_eType );
    break;
  }

  // 生成したエフェクトを返却する
  if( pEffect )
  {
    pEffect->SetType( in_eType );
    pEffect->SetParent( in_pObject );

    if (pIndex)
    {
      *pIndex = index;
    }
  }
  return pEffect;
}

/**
 *  @brief  エフェクトの生成：座標指定
 */
Field::Effect::IEffectBase *EffectManager::CreateEffect(Field::Effect::Type in_eType, const gfl2::math::Vector3 &in_vPosition, bool in_bPlaySe, WorkType workType, s32* pIndex, const f32 in_fScale, const gfl2::math::Vector3 &in_vRotation)
{
  if (pIndex)
  {
    *pIndex = ERROR_INDEX;
  }

  Field::Effect::IEffectBase *pEffect = NULL;
  s32 index = GetEmptyIndex( in_eType, workType );
  if( index == ERROR_INDEX )
  {
    GFL_PRINT( "FieldEffectManager is Full\n" );
    return pEffect;
  }
  // リソースが取得できないので返却
  void *pResource = GetResource( in_eType );
  if( pResource == NULL )
  {
    GFL_ASSERT_MSG( false, "EffectResource is NULL\n" );
    return pEffect;
  }

  // タイプごとのエフェクトクラスを生成
  switch( in_eType )
  {
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectExclamation1 *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectExclamation1();
      pBallon->SetPlaySe( in_bPlaySe );
      EffectBallonIcon::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = NULL;
      setup.pCameraManager = m_pCameraManager;
      setup.vPosition = in_vPosition;
      setup.bPosMode = true;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;
  case Field::Effect::EFFECT_TYPE_QUESTION_ICON:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectQuestion *pBallon = GFL_NEW( pHeapBase ) Field::Effect::EffectQuestion();
      pBallon->SetPlaySe( in_bPlaySe );
      EffectBallonIcon::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pParentObject = NULL;
      setup.pCameraManager = m_pCameraManager;
      setup.vPosition = in_vPosition;
      setup.bPosMode = true;
      pBallon->Initialize( setup );
      m_pEffectArray[ index ] = pBallon;
      pEffect = pBallon;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ACTION_BUTTON:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectActionButton *pActionButton = GFL_NEW( pHeapBase ) Field::Effect::EffectActionButton();
      EffectActionButton::SetupData setup;
      gfl2::math::Quaternion quat;
      quat.RadianToQuaternion( 0.0f, 0.0f, 0.0f );
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pCameraManager = m_pCameraManager;
      setup.pWinMgr = m_pWindowManager;
      setup.vPos = in_vPosition;
      setup.quaternion = quat;
      pActionButton->Initialize( setup );
      m_pEffectArray[ index ] = pActionButton;
      pEffect = pActionButton;
    }
    break;
  case Field::Effect::EFFECT_TYPE_W_F01R0102_EFF:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectLensFlare *pLensFlare = GFL_NEW( pHeapBase ) Field::Effect::EffectLensFlare();
      EffectLensFlare::SetupData setup;
      gfl2::math::Quaternion quat;
      quat.RadianToQuaternion( 0.0f, 0.0f, 0.0f );
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.vPos = in_vPosition;
      setup.quaternion = quat;
      pLensFlare->Initialize( setup );
      m_pEffectArray[ index ] = pLensFlare;
      pEffect = pLensFlare;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ENCGRASS01:
  case Field::Effect::EFFECT_TYPE_ENCGRASS02:
  case Field::Effect::EFFECT_TYPE_ENCGRASS03:
  case Field::Effect::EFFECT_TYPE_ENCGRASS04:
  case Field::Effect::EFFECT_TYPE_ENCGRASS05:
  case Field::Effect::EFFECT_TYPE_ENCGRASS06:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectEncGrass *pEncGrass = GFL_NEW( pHeapBase ) Field::Effect::EffectEncGrass();
      EffectEncGrass::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.vPos = in_vPosition;
      setup.onLengthType  = EffectEncGrass::ON_LENGTH_TYPE_NORMAL;
      setup.cpColorTable = m_cpEncountGrassColorTable;
      setup.worldID       = m_WorldID;
      if( in_eType == Field::Effect::EFFECT_TYPE_ENCGRASS02 )
      {  
        setup.onLengthType  = EffectEncGrass::ON_LENGTH_TYPE_SHORT;
      }
      else if( in_eType == Field::Effect::EFFECT_TYPE_ENCGRASS05 )
      {
        setup.onLengthType  = EffectEncGrass::ON_LENGTH_TYPE_YELLOW;
      }
      pEncGrass->Initialize( setup );
      m_pEffectArray[ index ] = pEncGrass;
      pEffect = pEncGrass;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FADE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFade *pFade = GFL_NEW( pHeapBase ) Field::Effect::EffectFade();
      EffectFade::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      pFade->Initialize( setup );
      m_pEffectArray[ index ] = pFade;
      pEffect = pFade;
    }
    break;

  case Field::Effect::EFFECT_TYPE_PLACENAME:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectPlaceName *pPlaceName = GFL_NEW( pHeapBase ) Field::Effect::EffectPlaceName();
      EffectPlaceName::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      setup.pWordSetLoader = m_pWordSetLoader;
      pPlaceName->Initialize( setup );
      m_pEffectArray[ index ] = pPlaceName;
      pEffect = pPlaceName;
    }
    break;

#if 1
  case EFFECT_TYPE_TRAINER_FOCUS:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectTrainerFocus *pTrainerFocus = GFL_NEW( pHeapBase ) Field::Effect::EffectTrainerFocus();
      EffectTrainerFocus::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      pTrainerFocus->Initialize( setup );
      m_pEffectArray[ index ] = pTrainerFocus;
      pEffect = pTrainerFocus;
      break;
    }
#endif
  case Field::Effect::EFFECT_TYPE_TRAINER_ENCOUNT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectTrainerEncount *pFade = GFL_NEW( pHeapBase ) Field::Effect::EffectTrainerEncount();
      pFade->SetPlaySe( in_bPlaySe );
      EffectTrainerEncount::SetupData setup;

      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      pFade->Initialize( setup );
      m_pEffectArray[ index ] = pFade;
      pEffect = pFade;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DUMMY_POLYGON:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectDummyPolygon *pDummyPolygon = GFL_NEW( pHeapBase ) EffectDummyPolygon();
      EffectDummyPolygon::SetupData setup;
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pCameraManager = m_pCameraManager;
      setup.vPos = in_vPosition;
      pDummyPolygon->Initialize( setup );
      m_pEffectArray[ index ] = pDummyPolygon;
      pEffect = pDummyPolygon;
    }
    break;

  case Field::Effect::EFFECT_TYPE_GIMMICK_ENCOUNT:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectGimmickEncount *pEffectGimmickEncount = GFL_NEW( pHeapBase ) EffectGimmickEncount();
      // 初期化
      EffectGimmickEncount::SetupData setup;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.vPos          = in_vPosition;
      pEffectGimmickEncount->Initialize( setup );
      // 追加
      m_pEffectArray[ index ] = pEffectGimmickEncount;
      pEffect = pEffectGimmickEncount;
    }
    break;

  case Field::Effect::EFFECT_TYPE_TRIAL_START:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectTrialStart *pTrialStart = GFL_NEW( pHeapBase ) EffectTrialStart();
      pTrialStart->SetPlaySe( in_bPlaySe );
      EffectTrialStart::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      pTrialStart->Initialize( setup );
      m_pEffectArray[ index ] = pTrialStart;
      pEffect = pTrialStart;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FADE_SPOT:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFadeSpot *pEffectFadeSpot = GFL_NEW( pHeapBase ) EffectFadeSpot();
      EffectFadeSpot::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      pEffectFadeSpot->Initialize( setup );
      m_pEffectArray[ index ] = pEffectFadeSpot;
      pEffect = pEffectFadeSpot;
    }
    break;

  case Field::Effect::EFFECT_TYPE_RAIN:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRain *pEffectRain = GFL_NEW( pHeapBase ) EffectRain();
      // 初期化
      EffectRain::SetupData setup;
      setup.pResource = pResource;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      pEffectRain->Initialize( setup );
      // 追加
      m_pEffectArray[ index ] = pEffectRain;
      pEffect = pEffectRain;
    }
    break;
  case Field::Effect::EFFECT_TYPE_SNOW:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectSnow *pEffectSnow = GFL_NEW( pHeapBase ) EffectSnow();
      // 初期化
      EffectSnow::SetupData setup;
      setup.pResource = pResource;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      pEffectSnow->Initialize( setup );
      // 追加
      m_pEffectArray[ index ] = pEffectSnow;
      pEffect = pEffectSnow;
    }
    break;
  case Field::Effect::EFFECT_TYPE_SANDSTORM:
    {
      // 生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectSandStorm *pEffectSandStorm = GFL_NEW( pHeapBase ) EffectSandStorm();
      // 初期化
      EffectSandStorm::SetupData setup;
      setup.pResource = pResource;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      pEffectSandStorm->Initialize( setup );
      // 追加
      m_pEffectArray[ index ] = pEffectSandStorm;
      pEffect = pEffectSandStorm;
    }
    break;
  case Field::Effect::EFFECT_TYPE_FISHING_BUOY:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase* pHeapBase = m_pEffectHeapArray[ index ];
      EffectFishingBuoy* pObj = GFL_NEW( pHeapBase ) EffectFishingBuoy();
      // 初期化処理
      EffectFishingBuoy::SetupData setupData;
      setupData.pHeap     = pHeapBase;
      setupData.pResource = pResource;
      setupData.pPipeLine = m_pPipeLine;
      setupData.pRootNode = m_pFieldRootNode;
      setupData.pos       = in_vPosition;
      pObj->Initialize(setupData);

      m_pEffectArray[ index ] = pObj;
      pEffect = pObj;
    }
    break;

  case Field::Effect::EFFECT_TYPE_RIDE_APPER_LAND:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRideApperLand *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRideApperLand();
      IEffectRideBase::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      p_effect->Initialize( setup );
      p_effect->CreateEffect( in_vPosition );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_RIDE_APPER_SEA:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRideApperSea *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRideApperSea();
      IEffectRideBase::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      p_effect->Initialize( setup );
      p_effect->CreateEffect( in_vPosition );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_B_DEMO:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBallDemo *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBallDemo();
      EffectBallDemo::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROTOM:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRotom *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRotom();
      EffectRotom::SetupData setup;
      // 初期化処理
      setup.pResource   = pResource;
      setup.pHeap       = pHeapBase;
      setup.pWinMgr     = m_pWindowManager;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_WATER:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootWater *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootWater();
      EffectFootWater::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.vPos = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FOOT_SAND:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFootSand *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFootSand();
      EffectFootSand::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.vPos = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectFestivalFire *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectFestivalFire();
      EffectFestivalFire::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectKairikyRockDown *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectKairikyRockDown();
      EffectKairikyRockDown::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROBA2_RUN_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRoba2RunSmoke *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRoba2RunSmoke();
      EffectRoba2RunSmoke::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.vPos = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_ROBA2_WALK_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectRoba2WalkSmoke *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectRoba2WalkSmoke();
      EffectRoba2WalkSmoke::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.vPos = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_GET_ITEM_1:
  case Field::Effect::EFFECT_TYPE_GET_ITEM_2:
  case Field::Effect::EFFECT_TYPE_GET_ITEM_3:
  case Field::Effect::EFFECT_TYPE_GET_ITEM_4:
    {
      // エフェクトの生成
      // リソース構成は同じなのでクラスも使いまわす
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectGetItem *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectGetItem();
      EffectGetItem::SetupData setup;
      // 初期化処理
      setup.pResource   = pResource;
      setup.pHeap       = pHeapBase;
      setup.pWinMgr     = m_pWindowManager;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL2:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, gfl2::math::ConvDegToRad( -108.0f ), 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_TRIAL2;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FES_LEVELUP:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase*           pHeapBase = m_pEffectHeapArray[ index ];
      EffectFesEffectBase*            p_effect  = GFL_NEW( pHeapBase ) Field::Effect::EffectFesLevelUp();
      EffectFesEffectBase::SetupData  setup;
      // 初期化処理
      setup.pResource       = pResource;
      setup.pEffectHeap     = m_pEffectHeap;
      setup.pEffectSystem   = m_pEffectSystem;
      setup.pHeapBase       = pHeapBase;
      setup.pFieldRootNode  = m_pFieldRootNode;
      setup.pFieldPipeLine  = m_pPipeLine;

      p_effect->Setup( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FES_SHOP_OPEN:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase*           pHeapBase = m_pEffectHeapArray[ index ];
      EffectFesEffectBase*            p_effect  = GFL_NEW( pHeapBase ) Field::Effect::EffectFesShopOpen();
      EffectFesEffectBase::SetupData  setup;
      // 初期化処理
      setup.pResource       = pResource;
      setup.pEffectHeap     = m_pEffectHeap;
      setup.pEffectSystem   = m_pEffectSystem;
      setup.pHeapBase       = pHeapBase;
      setup.pFieldRootNode  = m_pFieldRootNode;
      setup.pFieldPipeLine  = m_pPipeLine;

      p_effect->Setup( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FES_START_SPLASH:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase*           pHeapBase = m_pEffectHeapArray[ index ];
      EffectFesEffectBase*            p_effect  = GFL_NEW( pHeapBase ) Field::Effect::EffectFesStartSplash();
      EffectFesEffectBase::SetupData  setup;
      // 初期化処理
      setup.pResource       = pResource;
      setup.pEffectHeap     = m_pEffectHeap;
      setup.pEffectSystem   = m_pEffectSystem;
      setup.pHeapBase       = pHeapBase;
      setup.pFieldRootNode  = m_pFieldRootNode;
      setup.pFieldPipeLine  = m_pPipeLine;

      p_effect->Setup( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_FES_WARP:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase*           pHeapBase = m_pEffectHeapArray[ index ];
      EffectFesEffectBase*            p_effect  = GFL_NEW( pHeapBase ) Field::Effect::EffectFesWarp();
      EffectFesEffectBase::SetupData  setup;
      // 初期化処理
      setup.pResource       = pResource;
      setup.pEffectHeap     = m_pEffectHeap;
      setup.pEffectSystem   = m_pEffectSystem;
      setup.pHeapBase       = pHeapBase;
      setup.pFieldRootNode  = m_pFieldRootNode;
      setup.pFieldPipeLine  = m_pPipeLine;

      p_effect->Setup( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_STEP_SMOKE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_STEP_SMOKE;
      setup.bSuicide = true;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL7_1:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_TRIAL7_1;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL7_2:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseModel *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseModel();
      EffectBaseModel::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.position = in_vPosition;
      setup.nModelID = BL_IDX_DEMO_TRIAL7_2_FXM_PM0818_00_EV2600_C02_U_CTR_GFBMDLP;
      setup.nMotionID = BL_IDX_DEMO_TRIAL7_2_FXM_PM0818_00_EV2600_C02_U_GFBMOT;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_BAG_EFFECT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseModel *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseModel();
      EffectBaseModel::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pCameraManager = m_pCameraManager;
      setup.position = in_vPosition;
      setup.nModelID = BL_IDX_BAG_EFFECT_BAG_EFFECT_CTR_GFBMDLP;
      setup.nMotionID = BL_IDX_BAG_EFFECT_BAG_EFFECT_GFBMOT;
      setup.bSuicide = false;
      setup.bBillboard = true;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL3:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectTrial3 *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectTrial3();
      EffectTrial3::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.pCameraManager = m_pCameraManager;
      setup.position = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL5:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectTrial5 *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectTrial5();
      EffectTrial5::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.pCameraManager = m_pCameraManager;
      setup.position = in_vPosition;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_CONCENTRATE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseModel *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseModel();
      EffectBaseModel::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pCameraManager = m_pCameraManager;
      setup.position = in_vPosition;
      setup.scale.Set( 7.0f, 7.0f, 1.0f );
      setup.nModelID = BL_IDX_DEMO_CONCENTRATE_EF_B_DEMO_CONCENTRATE_CTR_GFBMDLP;
      setup.nMotionID = BL_IDX_DEMO_CONCENTRATE_EF_B_DEMO_CONCENTRATE_GFBMOT;
      setup.bSuicide = false;
      setup.bBillboard = false;
      setup.bScreen = true;
      setup.bLoop = true;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FOG:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseModel *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseModel();
      EffectBaseModel::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pRootNode = m_pFieldRootNode;
      setup.pPipeLine = m_pPipeLine;
      setup.pCameraManager = m_pCameraManager;
      setup.position = in_vPosition;
      setup.scale.Set( 7.0f, 7.0f, 1.0f );
      setup.nModelID = BL_IDX_DEMO_FOG_EF_B_DEMO_FOG_CTR_GFBMDLP;
      setup.nMotionID = BL_IDX_DEMO_FOG_EF_B_DEMO_FOG_GFBMOT;
      setup.bSuicide = false;
      setup.bBillboard = false;
      setup.bScreen = true;
      setup.bLoop = true;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FLOWER_YELLOW:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FLOWER_YELLOW;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FLOWER_PINK:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FLOWER_PINK;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_YELLOW:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FIREWORK_YELLOW;
      setup.nSe = SEQ_SE_EXTRA_0209;
      setup.bSuicide = false;
      p_effect->SetPlaySe( in_bPlaySe );
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PINK:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FIREWORK_PINK;
      setup.nSe = SEQ_SE_EXTRA_0210;
      setup.bSuicide = false;
      p_effect->SetPlaySe( in_bPlaySe );
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_RED:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FIREWORK_RED;
      setup.nSe = SEQ_SE_EXTRA_0211;
      setup.bSuicide = false;
      p_effect->SetPlaySe( in_bPlaySe );
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FIREWORK_PURPLE:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FIREWORK_PURPLE;
      setup.nSe = SEQ_SE_EXTRA_0214;
      setup.bSuicide = false;
      p_effect->SetPlaySe( in_bPlaySe );
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FLARE_SUN:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FLARE_SUN;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_DEMO_FLARE_MOON:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[ index ];
      EffectBaseParticle *p_effect = GFL_NEW( pHeapBase ) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set( 0.0f, 0.0f, 0.0f );
      setup.eResource = EFFECT_RESOURCE_DEMO_FLARE_MOON;
      setup.bSuicide = false;
      p_effect->Initialize( setup );
      m_pEffectArray[ index ] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_EF_PH0301_DEN:
  {
    // エフェクトの生成
    gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
    EffectDenjumokuThunder *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectDenjumokuThunder();
    EffectDenjumokuThunder::SetupData setup;
    // 初期化処理
    setup.pResource       = pResource;
    setup.pEffectHeap     = m_pEffectHeap;
    setup.pEffectSystem   = m_pEffectSystem;
    setup.position        = in_vPosition;
    setup.fScaleCoeff     = in_fScale;
    setup.rotation.Set(gfl2::math::ConvDegToRad(in_vRotation.x), gfl2::math::ConvDegToRad(in_vRotation.y), gfl2::math::ConvDegToRad(in_vRotation.z));
    p_effect->Initialize(setup);
    m_pEffectArray[index] = p_effect;
    pEffect = p_effect;
  }
  break;

  case Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01:
  {
    // エフェクトの生成
    gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
    EffectNewTrial5 *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectNewTrial5();
    EffectNewTrial5::SetupData setup;
    // 初期化処理
    setup.pResource = pResource;
    setup.pEffectHeap = m_pEffectHeap;
    setup.pEffectSystem = m_pEffectSystem;
    setup.position = in_vPosition;
    setup.pParentObject = NULL;
    p_effect->Initialize(setup);
    m_pEffectArray[index] = p_effect;
    pEffect = p_effect;
  }
  break;

  case Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02:
  {
    // エフェクトの生成
    gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
    EffectNewTrial5ForModel *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectNewTrial5ForModel();
    EffectNewTrial5ForModel::SetupData setup;
    // 初期化処理
    setup.pResource = pResource;
    setup.pHeap = pHeapBase;
    setup.pRootNode = m_pFieldRootNode;
    setup.pPipeLine = m_pPipeLine;
    setup.position = in_vPosition;
    p_effect->Initialize(setup);
    m_pEffectArray[index] = p_effect;
    pEffect = p_effect;
  }
  break;

  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectKairikyRockDown2 *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectKairikyRockDown2();
      EffectKairikyRockDown2::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;
  case Field::Effect::EFFECT_TYPE_EF_PH0301_DEN2:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectDenjumokuThunder2 *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectDenjumokuThunder2();
      EffectDenjumokuThunder2::SetupData setup;
      // 初期化処理
      setup.pResource       = pResource;
      setup.pEffectHeap     = m_pEffectHeap;
      setup.pEffectSystem   = m_pEffectSystem;
      setup.position        = in_vPosition;
      setup.fScaleCoeff     = in_fScale;
      setup.rotation.Set(gfl2::math::ConvDegToRad(in_vRotation.x), gfl2::math::ConvDegToRad(in_vRotation.y), gfl2::math::ConvDegToRad(in_vRotation.z));
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
  break;

  case Field::Effect::EFFECT_TYPE_UB_KAMI_SLASH:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectKamitsurugiSlash *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectKamitsurugiSlash();
      EffectKamitsurugiSlash::SetupData setup;
      // 初期化処理
      setup.pResource     = pResource;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position      = in_vPosition;
      setup.rotation.Set(gfl2::math::ConvDegToRad(in_vRotation.x), gfl2::math::ConvDegToRad(in_vRotation.y), gfl2::math::ConvDegToRad(in_vRotation.z));
      setup.fScaleCoeff   = in_fScale;
      setup.resorceID     = EFFECT_RESOURCE_UB_KAMI_SLASH;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_UB_KAMI_BLACKOUT:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectKamitsurugiSlash *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectKamitsurugiSlash();
      EffectKamitsurugiSlash::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set(gfl2::math::ConvDegToRad(in_vRotation.x), gfl2::math::ConvDegToRad(in_vRotation.y), gfl2::math::ConvDegToRad(in_vRotation.z));
      setup.fScaleCoeff = in_fScale;
      setup.resorceID = EFFECT_RESOURCE_UB_KAMI_BLACKOUT;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROTOM_POWER:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectBaseParticle *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource     = pResource;
      setup.pEffectHeap   = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position      = in_vPosition;
      setup.rotation.Set(0.0f, 0.0f, 0.0f);
      setup.eResource     = EFFECT_RESOURCE_ROTOM_POWER;
      setup.bSuicide      = false;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_ROTOM_CUT_IN:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectRotomCutIn *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectRotomCutIn();
      EffectRotomCutIn::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pHeap = pHeapBase;
      setup.pWinMgr = m_pWindowManager;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
      p_effect->Start();
    }
    break;

  case Field::Effect::EFFECT_TYPE_DEMO_TRIAL2_2:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectBaseParticle *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set(0.0f, 0.0f, 0.0f);
      setup.eResource = EFFECT_RESOURCE_DEMO_TRIAL2_2;
      setup.bSuicide = false;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_R_ROCKET1:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectBaseParticle *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set(0.0f, 0.0f, 0.0f);
      setup.eResource = EFFECT_RESOURCE_R_ROCKET_1;
      setup.bSuicide = false;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  case Field::Effect::EFFECT_TYPE_EF_BTFES_WARP:
    {
      // エフェクトの生成
      gfl2::heap::HeapBase *pHeapBase = m_pEffectHeapArray[index];
      EffectBaseParticle *p_effect = GFL_NEW(pHeapBase) Field::Effect::EffectBaseParticle();
      EffectBaseParticle::SetupData setup;
      // 初期化処理
      setup.pResource = pResource;
      setup.pEffectHeap = m_pEffectHeap;
      setup.pEffectSystem = m_pEffectSystem;
      setup.position = in_vPosition;
      setup.rotation.Set(0.0f, 0.0f, 0.0f);
      setup.eResource = EFFECT_RESOURCE_EF_BTFES_WARP;
      setup.bSuicide = false;
      p_effect->Initialize(setup);
      m_pEffectArray[index] = p_effect;
      pEffect = p_effect;
    }
    break;

  default:
    GFL_ASSERT_MSG( false, "CreateEffect(mode:position) Failed!! EffectType is not exist Type(%d)", in_eType );
    break;
  }

  // 生成したエフェクトを返却する
  if( pEffect )
  {
    pEffect->SetType( in_eType );
    pEffect->SetParent(NULL); // 念のため

    if (pIndex)
    {
      *pIndex = index;
    }
  }
  return pEffect;
}

/**
 *  @brief  エフェクト取得処理：データIndex指定
 *  @param  data_index 取得するエフェクトのIndex
 *
 *  @return エフェクトのポインタを返却する
 */
Field::Effect::IEffectBase* EffectManager::GetEffect(s32 data_index)
{
  if (data_index >= MAX_EFFECT_NUM || m_pEffectArray[data_index] == NULL)
  {
    return NULL;
  }

  return m_pEffectArray[data_index];
}

/**
 *  @brief  エフェクトの破棄：エフェクト指定
 */
void EffectManager::DeleteEffect( Field::Effect::IEffectBase *in_pEffect )
{
  // エフェクトの破棄
  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( in_pEffect == m_pEffectArray[ i ] )
    {
      // 破棄予約
      m_pEffectArray[ i ]->RequestDelete();
    }
  }
}

/**
 *  @brief  エフェクトの破棄：親オブジェクト指定
 */
void EffectManager::DeleteEffect( Field::IField3DObjectNode *in_pObject )
{
  // 親の指定がない場合は破棄しない
  if( in_pObject == NULL )
  {
    return;
  }

  // エフェクトの破棄
  for( u32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( m_pEffectArray[ i ]->IsSameParent( in_pObject ) )
    {
      // 親が一致するものは破棄予約する
      m_pEffectArray[ i ]->RequestDelete();
    }
  }
}

/**
 *  @brief  リソースの取得
 */
void *EffectManager::GetResource( Field::Effect::Type in_eType )
{
  // 常駐エフェクト
  if( m_CommonResource.GetDataSize( in_eType ) != 0 )
  {
    // 指定リソースの返却
    return m_CommonResource.GetData( in_eType );
  }
  // レイアウト
  if( m_LayoutResource.GetDataSize( in_eType ) != 0 )
  {
    // 指定リソースの返却
    return m_LayoutResource.GetData( in_eType );
  }
  // 非常駐リソース
  if( GetDyanamicResourceData( in_eType ) )
  {
    return GetDyanamicResourceData( in_eType );
  }


  return NULL;
}

/**
 *  @brief  地名表示リクエスト
 *  @param  bForce    強制で出す
 */
void EffectManager::RequestStartPlaceName( bool bForce )
{
  // 地名表示が存在しない
  if( !m_pPlaceName ){ return; }
  // データを取得する
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  if( !pGameManager ){ return; }
  Savedata::Situation *pSit = pGameManager->GetGameData()->GetSituation();
  Field::Fieldmap *pFieldmap = pGameManager->GetFieldmap();
  Field::ZoneDataLoader *pLoader = pGameManager->GetGameData()->GetFieldZoneDataLoader();
  if( !pFieldmap || !pLoader || !pSit ){ return; }

  ZoneID zoneID = pFieldmap->GetZoneID();
  ZoneID lastZoneID = pSit->GetLastZoneID();
  u32 placeStrID;
  u32 placeOldStrID = 0xFFFFFFFF;
  {
    void *zoneData = pLoader->GetZoneData( zoneID );
    if( !zoneData ){ return; }
    Field::ZoneDataAccessor accessor( zoneData );
    placeStrID = accessor.GetPlaceLabelID();
  }
  if( lastZoneID != ZONEID_ERROR )
  {
    void *zoneData = pLoader->GetZoneData( lastZoneID );
    if( !zoneData ){ return; }
    Field::ZoneDataAccessor accessor( zoneData );
    placeOldStrID = accessor.GetPlaceLabelID();
  }

  // リクエストを出す
  m_pPlaceName->Start( placeStrID, placeOldStrID, bForce );
}

/**
 *  @brief  地名表示をけす
 */
void EffectManager::RequestEndPlaceName( void )
{
  // 地名表示が存在しない
  if( !m_pPlaceName ){ return; }

  m_pPlaceName->End();
}

/**
 *  @brief  指定タイプのエフェクト数を取得する
 */
s32 EffectManager::GetEffectNum( Field::Effect::Type in_eType )
{
  s32 result = 0;
  for( s32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( !m_pEffectArray[i] ){ continue; }
    // タイプが一致している and 安全に破棄できない状態
    if( m_pEffectArray[i]->GetType() == in_eType && !m_pEffectArray[i]->IsSafeDelete() )
    {
      ++ result;
    }
  }

  return result;
}
/**
 *  @brief  指定タイプのエフェクト数を取得する
 *  @param  in_eType    タイプの指定
 *  @return 生存中のエフェクト数を返却する
 *
 *  @note   削除直前用にExecのみを探すモード
 */
s32 EffectManager::GetEffectNumPreTerminate( Field::Effect::Type in_eType )
{
  s32 result = 0;
  for( s32 i = 0; i < MAX_EFFECT_NUM; ++ i )
  {
    if( !m_pEffectArray[i] ){ continue; }
    // タイプが一致している and 実行中の状態
    if( m_pEffectArray[i]->GetType() == in_eType && m_pEffectArray[i]->IsExec() )
    {
      ++ result;
    }
  }

  return result;
}

/**
*  @brief  エフェクト配列の空きを取得する
*  @param  in_eType      エフェクト種別指定
*  @param  in_eWorkType  エフェクトワーク種別指定
*  @retval ERROR_INDEX  空きが存在しない
*  @retval 空きインデックスを返却する
*/
s32 EffectManager::GetEmptyIndex( Field::Effect::Type in_eType, WorkType in_eWorkType ) const
{
  if( in_eWorkType == WORK_TYPE_DEFAULT )
  {
    return GetEmptyIndexByEffectType( in_eType );
  }
  else
  {
    return GetEmptyIndexByWorkType( in_eWorkType );
  }
}

/**
 *  @brief  エフェクト配列の空きを取得する
 */
s32 EffectManager::GetEmptyIndexByEffectType( Field::Effect::Type in_eType ) const
{
  s32 index = ERROR_INDEX;

  switch( in_eType )
  {
  case EFFECT_TYPE_SHADOW_CIRCLE:       return GetEmptyIndexForSys();
  case EFFECT_TYPE_SHADOW_DEPTH:        return GetEmptyIndexForSys();
  case EFFECT_TYPE_SHADOW_WIDTH:        return GetEmptyIndexForSys();
  case EFFECT_TYPE_SHADOW_FOOT:         return GetEmptyIndexForSys();
  case EFFECT_TYPE_ROCKBREAK_G:         return GetEmptyIndexForSys();
  case EFFECT_TYPE_DUMMY_POLYGON:       return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_WATER:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_SAND:           return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_LAND:           return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_RIDE_KENTA:     return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_RIDE_MURANDO:   return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_RIDE_KAIRIKY:   return GetEmptyIndexForSys();
  case EFFECT_TYPE_FOOT_RIDE_ZIMENBA:   return GetEmptyIndexForSys();
  case EFFECT_TYPE_PLACENAME:           return GetEmptyIndexForSys();
  case EFFECT_TYPE_TRAINER_FOCUS:       return GetEmptyIndexForSys();
  case EFFECT_TYPE_FES_MESSAGE:         return GetEmptyIndexForSys();
  case EFFECT_TYPE_FES_LEVELUP:         return GetEmptyIndexForSys();
  case EFFECT_TYPE_FES_SHOP_OPEN:       return GetEmptyIndexForSys();
  case EFFECT_TYPE_FES_START_SPLASH:    return GetEmptyIndexForSys();
  case EFFECT_TYPE_FES_WARP:            return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS01:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS02:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS03:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS04:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS05:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ENCGRASS06:          return GetEmptyIndexForSys();
  case EFFECT_TYPE_ROBA2_RUN_SMOKE:     return GetEmptyIndexForSys();
  case EFFECT_TYPE_ROBA2_WALK_SMOKE:    return GetEmptyIndexForSys();
  case EFFECT_TYPE_CHARA_BREATH:        return GetEmptyIndexForSys();
  case EFFECT_TYPE_CHARA_RIPPLE:        return GetEmptyIndexForSys();
  case EFFECT_TYPE_STEP_SMOKE:          return GetEmptyIndexForSys(); // @fix GFNMCat[919] Evt->Sys
  case EFFECT_TYPE_ROCKBREAK_W:         return GetEmptyIndexForSys(); // @fix GFNMCat[919] Evt->Sys

  case EFFECT_TYPE_ACTION_BUTTON:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_EXCLAMATION_ICON1:   return GetEmptyIndexForEvt();
  case EFFECT_TYPE_EXCLAMATION_ICON2:   return GetEmptyIndexForEvt();
  case EFFECT_TYPE_QUESTION_ICON:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_B_DEMO:              return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FISHING_BUOY:        return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_W_F01R0102_EFF:      return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_FADE:                return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_FADE_SPOT:           return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_TRAINER_ENCOUNT:     return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_TRIAL_START:         return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_ROTOM:               return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_FESTIVAL_FIRE:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_REPORT:              return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_GET_ITEM_1:          return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_GET_ITEM_2:          return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_GET_ITEM_3:          return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_GET_ITEM_4:          return GetEmptyIndexForEvt(); // o
  case EFFECT_TYPE_DEMO_RIDE:           return GetEmptyIndexForEvt();
  case EFFECT_TYPE_DEMO_TRIAL2:         return GetEmptyIndexForEvt();
    //BTS-5716対応
  case EFFECT_TYPE_RIDE_APPER_LAND:     return GetEmptyIndexForEvt();
  case EFFECT_TYPE_RIDE_APPER_SEA:      return GetEmptyIndexForEvt();
  case EFFECT_TYPE_GIMMICK_ENCOUNT:     return GetEmptyIndexForEvt(); // @fix GFNMCat[919] Sys->Evt
  case EFFECT_TYPE_DEMO_TRIAL7_1:       return GetEmptyIndexForEvt(); // 谷さん要望対応：試練７演出用エフェクト
  case EFFECT_TYPE_DEMO_TRIAL7_2:       return GetEmptyIndexForEvt(); // 谷さん要望対応：試練７演出用エフェクト
  case EFFECT_TYPE_BAG_EFFECT:          return GetEmptyIndexForEvt(); // 小島さん要望対応：イベント：カバンが光る演出用エフェクト
  case EFFECT_TYPE_DEMO_TRIAL3:         return GetEmptyIndexForEvt(); // 貫田さん要望対応：試練３演出用エフェクト
  case EFFECT_TYPE_DEMO_TRIAL5:         return GetEmptyIndexForEvt(); // 貫田さん要望対応：試練５演出用エフェクト
  case EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY:   return GetEmptyIndexForEvt(); // 谷さん要望対応：オーバーレイ型のアイコン
  case EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY:   return GetEmptyIndexForEvt(); // 谷さん要望対応：オーバーレイ型のアイコン
  case EFFECT_TYPE_QUESTION_ICON_OVERLAY:       return GetEmptyIndexForEvt(); // 谷さん要望対応：オーバーレイ型のアイコン
  case EFFECT_TYPE_DEMO_CONCENTRATE:    return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FOG:            return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FLOWER_YELLOW:  return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FLOWER_PINK:    return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FIREWORK_YELLOW:return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FIREWORK_PINK:  return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FIREWORK_RED:   return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FIREWORK_PURPLE:return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FLARE_SUN:      return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_DEMO_FLARE_MOON:     return GetEmptyIndexForEvt(); // 大森さん要望対応：エフェクト追加
  case EFFECT_TYPE_EF_PH0301_DEN:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_EF_PH0301_DEN2:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_UB_KAMI_SLASH:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_UB_KAMI_BLACKOUT:    return GetEmptyIndexForEvt();
  case EFFECT_TYPE_DEMO_NEW_TRIAL5_01:  return GetEmptyIndexForEvt();
  case EFFECT_TYPE_DEMO_NEW_TRIAL5_02:  return GetEmptyIndexForEvt();
  case EFFECT_TYPE_ROTOM_POWER:         return GetEmptyIndexForEvt();
  case EFFECT_TYPE_ROTOM_CUT_IN:        return GetEmptyIndexForEvt();
  case EFFECT_TYPE_DEMO_TRIAL2_2:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1:    return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_EXCLAMATION2:    return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_HEART1:          return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_NOTE1:           return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_QUESTION1:       return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_SMILE1:          return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_TEN1:            return GetEmptyIndexForEvt();
  case EFFECT_TYPE_FUREAI_ICON_ZZ1:             return GetEmptyIndexForEvt();
  case EFFECT_TYPE_R_ROCKET1:           return GetEmptyIndexForEvt();
  case EFFECT_TYPE_EF_BTFES_WARP:           return GetEmptyIndexForEvt();


  case EFFECT_TYPE_RAIN:                return GetEmptyIndexForWtr();
  case EFFECT_TYPE_SNOW:                return GetEmptyIndexForWtr();
  case EFFECT_TYPE_SANDSTORM:           return GetEmptyIndexForWtr();

  case EFFECT_TYPE_DASH_SMOKE:          return GetEmptyIndexForRID();
  case EFFECT_TYPE_SHARK_RUN:           return GetEmptyIndexForRID();
  case EFFECT_TYPE_SHARK_WAIT:          return GetEmptyIndexForRID();
  case EFFECT_TYPE_SHARK_JET:           return GetEmptyIndexForRID();
  case EFFECT_TYPE_LAPLACE_WAIT:        return GetEmptyIndexForRID();
  case EFFECT_TYPE_LAPLACE_FLOW:        return GetEmptyIndexForRID();
  case EFFECT_TYPE_ROBA2_ACCEL_SMOKE:   return GetEmptyIndexForRID();
  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:  return GetEmptyIndexForRID(); // @fix GFNMCat[919] Evt->RID
  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN:   return GetEmptyIndexForRID(); // @fix GFNMCat[919] Evt->RID
  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L:return GetEmptyIndexForRID();
  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L: return GetEmptyIndexForRID();

  }

  // 検索結果を返却する
  return index;
}

/**
*  @brief  エフェクト配列の空きを取得する
*  @param  in_eType  ワーク種別指定
*  @retval ERROR_INDEX  空きが存在しない
*  @retval 空きインデックスを返却する
*/
s32 EffectManager::GetEmptyIndexByWorkType( WorkType in_eType ) const
{
  s32 index = ERROR_INDEX;

  switch( in_eType )
  {
  case WORK_TYPE_SYS:       return GetEmptyIndexForSys();
  case WORK_TYPE_EVT:       return GetEmptyIndexForEvt();
  case WORK_TYPE_WTR:
    {
      GFL_ASSERT(0);  // 天候での置き換えはあり得ない
      return GetEmptyIndexForWtr();
    }
  case WORK_TYPE_RID:       return GetEmptyIndexForRID();
  }

  // 検索結果を返却する
  return index;
}

/**
 *  @brief  エフェクト配列の空きを取得する
 */
s32 EffectManager::GetEmptyIndexForSys( void ) const
{
  s32 index = ERROR_INDEX;
  for( s32 i = 0; i < SYS_EFFECT_NUM; ++ i )
  {
    // 未精製インデックスを保持する
    if( m_pEffectArray[ i ] == NULL )
    {
      index = i;
      break;
    }
  }

  // 検索結果を返却する
  return index;
}

/**
 *  @brief  エフェクト配列の空きを取得する
 */
s32 EffectManager::GetEmptyIndexForWtr( void ) const
{
  s32 index = ERROR_INDEX;
  s32 maxi = SYS_EFFECT_NUM + WTR_EFFECT_NUM;
  for( s32 i = SYS_EFFECT_NUM; i < maxi; ++ i )
  {
    s32 idx = i;
    // 未精製インデックスを保持する
    if( m_pEffectArray[ idx ] == NULL )
    {
      index = idx;
      break;
    }
  }

  // 検索結果を返却する
  return index;
}

/**
 *  @brief  エフェクト配列の空きを取得する
 */
s32 EffectManager::GetEmptyIndexForRID( void ) const
{
  s32 index = ERROR_INDEX;
  s32 mini = SYS_EFFECT_NUM + WTR_EFFECT_NUM;
  s32 maxi = SYS_EFFECT_NUM + WTR_EFFECT_NUM + RID_EFFECT_NUM;
  for( s32 i = mini; i < maxi; ++ i )
  {
    s32 idx = i;
    // 未精製インデックスを保持する
    if( m_pEffectArray[ idx ] == NULL )
    {
      index = idx;
      break;
    }
  }

  // 検索結果を返却する
  return index;
}

/**
 *  @brief  エフェクト配列の空きを取得する
 */
s32 EffectManager::GetEmptyIndexForEvt( void ) const
{
  s32 index = ERROR_INDEX;
  for( s32 i = 0; i < EVT_EFFECT_NUM; ++ i )
  {
    // イベントは後ろから確認する
    s32 idx = MAX_EFFECT_NUM - ( i + 1 );
    // 未精製インデックスを保持する
    if( m_pEffectArray[ idx ] == NULL )
    {
      index = idx;
      break;
    }
  }

  // 検索結果を返却する
  return index;
}



/**
 *  @brief 非常駐のエフェクトリソースインデックスを取得 : 非lyt
 */
s32 EffectManager::GetDynamicResourceIndex3d( Field::Effect::Type in_eType ) const
{
  switch( in_eType )
  {
  case EFFECT_TYPE_W_F01R0102_EFF:
    return GARC_field_effect_3d_w_f01r0102_eff_COMP;
  case EFFECT_TYPE_FISHING_BUOY:
    return GARC_field_effect_3d_fishing_buoy_COMP;
  case EFFECT_TYPE_RIDE_APPER_LAND:
    return GARC_field_effect_3d_ride_apper_land_COMP;
  case EFFECT_TYPE_RIDE_APPER_SEA:
    return GARC_field_effect_3d_ride_apper_sea_COMP;
  case EFFECT_TYPE_B_DEMO:
    return GARC_field_effect_3d_b_demo_COMP;
  case EFFECT_TYPE_FESTIVAL_FIRE:
    return GARC_field_effect_3d_festival_fire_COMP;
  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:
    return GARC_field_effect_3d_kairiky_rock_smoke_COMP;
  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN:
    return GARC_field_effect_3d_kairiky_rock_down_COMP;
  case EFFECT_TYPE_DEMO_RIDE:
    return GARC_field_effect_3d_demo_ride_COMP;
  case EFFECT_TYPE_DEMO_TRIAL2:
    return GARC_field_effect_3d_demo_trial2_COMP;
  case  EFFECT_TYPE_FES_LEVELUP:
    return GARC_field_effect_3d_fes_levelup_COMP;
  case  EFFECT_TYPE_FES_SHOP_OPEN:
    return GARC_field_effect_3d_fes_shop_open_COMP;
  case  EFFECT_TYPE_FES_START_SPLASH:
    return GARC_field_effect_3d_fes_start_splash_COMP;
  case  EFFECT_TYPE_FES_WARP:
    return GARC_field_effect_3d_fes_warp_COMP;
  case EFFECT_TYPE_DEMO_TRIAL7_1:
    return GARC_field_effect_3d_demo_trial7_1_COMP;
  case EFFECT_TYPE_DEMO_TRIAL7_2:
    return GARC_field_effect_3d_demo_trial7_2_COMP;
  case EFFECT_TYPE_BAG_EFFECT:
    return GARC_field_effect_3d_bag_effect_COMP;
  case EFFECT_TYPE_DEMO_TRIAL3:
    return GARC_field_effect_3d_demo_trial3_COMP;
  case EFFECT_TYPE_DEMO_TRIAL5:
    return GARC_field_effect_3d_demo_trial5_COMP;
  case EFFECT_TYPE_DEMO_CONCENTRATE:
    return GARC_field_effect_3d_demo_concentrate_COMP;
  case EFFECT_TYPE_DEMO_FOG:
    return GARC_field_effect_3d_demo_fog_COMP;
  case EFFECT_TYPE_DEMO_FLOWER_YELLOW:
    return GARC_field_effect_3d_demo_flower_yellow_COMP;
  case EFFECT_TYPE_DEMO_FLOWER_PINK:
    return GARC_field_effect_3d_demo_flower_pink_COMP;
  case EFFECT_TYPE_DEMO_FIREWORK_YELLOW:
    return GARC_field_effect_3d_demo_firework_yellow_COMP;
  case EFFECT_TYPE_DEMO_FIREWORK_PINK:
    return GARC_field_effect_3d_demo_firework_pink_COMP;
  case EFFECT_TYPE_DEMO_FIREWORK_RED:
    return GARC_field_effect_3d_demo_firework_red_COMP;
  case EFFECT_TYPE_DEMO_FIREWORK_PURPLE:
    return GARC_field_effect_3d_demo_firework_purple_COMP;
  case EFFECT_TYPE_DEMO_FLARE_SUN:
    return GARC_field_effect_3d_demo_flare_sun_COMP;
  case EFFECT_TYPE_DEMO_FLARE_MOON:
    return GARC_field_effect_3d_demo_flare_moon_COMP;
  case EFFECT_TYPE_EF_PH0301_DEN:
    return GARC_field_effect_3d_ef_ph0301_den_COMP;
  case EFFECT_TYPE_EF_PH0301_DEN2:
    return GARC_field_effect_3d_ef_ph0301_den2_COMP;
  case EFFECT_TYPE_DEMO_NEW_TRIAL5_01:
    return GARC_field_effect_3d_demo_new_trial5_01_COMP;
  case EFFECT_TYPE_DEMO_NEW_TRIAL5_02:
    return GARC_field_effect_3d_demo_new_trial5_02_COMP;
  case EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L:
    return GARC_field_effect_3d_kairiky_rock_smoke_l_COMP;
  case EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L:
    return GARC_field_effect_3d_kairiky_rock_down_l_COMP;
  case EFFECT_TYPE_UB_KAMI_SLASH:
    return GARC_field_effect_3d_ub_kami_slash_COMP;
  case EFFECT_TYPE_UB_KAMI_BLACKOUT:
    return GARC_field_effect_3d_ub_kami_blackout_COMP;
  case EFFECT_TYPE_ROTOM_POWER:
    return GARC_field_effect_3d_rotom_power_COMP;
  case EFFECT_TYPE_DEMO_TRIAL2_2:
    return GARC_field_effect_3d_demo_trial2_2_COMP;
  case EFFECT_TYPE_R_ROCKET1:
    return GARC_field_effect_3d_r_rocket1_COMP;
  case EFFECT_TYPE_EF_BTFES_WARP:
    return GARC_field_effect_3d_ef_Btfes_warp_COMP;

  }

  return -1;  // 常駐リソース
}

/**
 *  @brief 非常駐のエフェクトリソースインデックスを取得 : lyt
 */
s32 EffectManager::GetDynamicResourceIndexLyt( Field::Effect::Type in_eType ) const
{
  switch( in_eType )
  {
  case EFFECT_TYPE_TRIAL_START:
    return GARC_field_effect_2d_trial_start_COMP;

  case EFFECT_TYPE_FES_MESSAGE:
    return GARC_field_effect_2d_fes_message_COMP;

  case EFFECT_TYPE_ROTOM:
    return GARC_field_effect_2d_rotom_COMP;

  case EFFECT_TYPE_REPORT:
    return GARC_field_effect_2d_report_COMP;

  case EFFECT_TYPE_TRAINER_ENCOUNT:
    return GARC_field_effect_2d_trainer_encount_COMP;

  case EFFECT_TYPE_GET_ITEM_1:
    return GARC_field_effect_2d_get_item_1_COMP;

  case EFFECT_TYPE_GET_ITEM_2:
    return GARC_field_effect_2d_get_item_2_COMP;

  case EFFECT_TYPE_GET_ITEM_3:
    return GARC_field_effect_2d_get_item_3_COMP;

  case EFFECT_TYPE_GET_ITEM_4:
    return GARC_field_effect_2d_get_item_4_COMP;

  case EFFECT_TYPE_ROTOM_CUT_IN:
    return GARC_field_effect_2d_rotom_cut_in_COMP;
  }

  return -1;  // 常駐リソース
}

/** 
 *  @brief 非常駐リソースの取得
 */
void * EffectManager::GetDyanamicResourceData( Field::Effect::Type in_eType )
{
  if( in_eType < EFFECT_TYPE_MAX )
  {
    return m_pDynamicResource[in_eType];
  }
  return NULL;
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
