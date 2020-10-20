//======================================================================
/**
 * @file    ResortIslandDraw.cpp
 * @brief   島の描画
 * @author  k.ohno
 * @data    15/09/05
 */
//======================================================================



// gfl2のインクルード
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <gfx/include/gfl2_GFGL.h>
#include <cx/include/gfl2_StreamingUncomp.h>
#include <Sound/include/Sound.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

#include "System/include/nijiAllocator.h"
#include "Savedata/include/ResortSave.h"
#include "GameSys/include/NijiTime.h"
#include <AppLib/include/Tool/PokeIcon.h>

// poke_3dのインクルード
#include <model/include/gfl2_BaseModel.h>

#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>

#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h>

#include "ResortIslandDraw.h"

#include <arc_def_index/arc_def.h>
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"
#include "System/include/SkyBox/Skybox.h"
#include <Fade/include/gfl2_FadeManager.h>
#include <model/include/gfl2_BaseCamera.h>
#include "Savedata/include/BoxPokemonSave.h"
#include "AppLib/include/Ui/UIViewManager.h"

#include "ResortChangeIslandListener.h"
#include "ResortInitLogic.h"

#include "App/Resort/include/ResortHeapDefine.h"
#include "App/AppEvent/include/PokeIsland/ResortDataManager.h"

#include "ResortDebugInfo.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)



static const u32 POKE_LIGHT_SET_NO = 0;  // m_pPokeLightのライトセット番号(m_pDrawEnvNodeで使っているライトセット番号)
                                         // グラフィッカーさんのDrawEnvリソースに含まれるライトセット番号と被らない5にしたがうまくいかなかったので0にした。
 
static const s32 RESOURCE_POKE_LIGHT_SET_NO = 0;               // グラフィッカーさんのDrawEnvリソースに含まれるライトセット番号
static const c8* RESOURCE_POKE_LIGHT_NAME   = "EnvAmbLight";//"EnvDirLight";



ResortIslandDraw::ResortIslandDraw()
  : mpAppHeap(NULL)
  , mpPokeModelHeap(NULL)
  , mpPokeModelNijiAllocator(NULL)
  , m_camera(NULL)
  , m_pDrawEnvNode(NULL)
  , m_pPokeLight(NULL)
  , m_resourcePokeLightNo(0)
  , m_pokeModelExist(false)
  , mIsStartCameraMoveEnd( false )
  , mIsEndCameraMove( false )
  , mUpdateCameraTotal( 0 )
  , mUpdateCameraAdd( UPDATE_CAMERA_ADD_MAX )
  , mUpdateCameraValue( UPDATE_CAMERA_VALUE )
{
  m_Seq = 0;
  m_asyncCreateDestroyPokeModelStep=0;
  island_camera_rotate = -0.31f;

  m_PokeDeleteStep = 0;
  BILLBOARD_COMP = 0.0f;
  mbKeyEnable = true;
  m_WalkEnd = false;
  mbKinomiDeleteState=false;
  mbFukidashiDisable=false;

  mpPokeIcon3dTexParentHeap = NULL;
  for(int i=0; i<BILL_DATA_NUM; ++i)
  {
    for(int j=0; j<2; ++j)
    {
      mpPokeIcon3dTexHeap[i][j] = NULL;
    }
  }

#if PM_DEBUG
  mDebugZoom=0.0f;
  mdebugmulnum=1.0f;
#endif


}

ResortIslandDraw::~ResortIslandDraw(void)
{

}

enum{
  GROUND_FILE_READ_,
  GROUND_FILE_WAIT_,
  SKYBOX_WAIT_,
  SKYBOX_WAIT2_,
  POKEMODEL_INIT_,
  POKEMODEL_WAIT_,
  BILLBOARD_WAIT_,
  POEKMODEL_SYS_,
};


void ResortIslandDraw::SetInitCameraData(void* pReadBuffAddr)
{
  mpstrx =  (camera_island_binstr*)pReadBuffAddr;

  int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
  int datano = mpAppParam->firstOpenPos * 3 + lv;

  ISLAND_DIST_INIT = mpstrx[datano].c4;
  ISLAND_HEIGHT_INIT = mpstrx[datano].c5;
  island_camera_rotate = -0.31f;


  m_TargetPos.x = mpstrx[datano].c1;
  m_TargetPos.y = mpstrx[datano].c2;
  m_TargetPos.z = mpstrx[datano].c3;


  OHNO_PRINT("pos --  %f %f %f - %f %f\n",m_TargetPos.x,m_TargetPos.y,m_TargetPos.z, ISLAND_DIST_INIT,ISLAND_HEIGHT_INIT);

}


void ResortIslandDraw::SetHotSpaMove(void* pReadBuffAddr)
{
  mpHotSpaMoveBin =  (walk_hotspa_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetAdvMove(void* pReadBuffAddr)
{
  mpAdvMoveBin =  (walk_adv_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetFarmMove(void* pReadBuffAddr)
{
  mpFarmMoveBin =  (walk_farm_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetGimMove(void* pReadBuffAddr)
{
  mpGimMoveBin =  (walk_gim_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetFriendMove(void* pReadBuffAddr)
{
  mpFriendMoveBin =  (walk_friend_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetKinomiPos(void* pReadBuffAddr)
{
  mpKinomiTreeBin =  (kinomipos_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetPokeMoveFlg(void* pReadBuffAddr)
{
  mpPokeMoveFlg =  (moveflg_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetStandPosBin(void* pReadBuffAddr)
{
  mpStandPosBin = (standpos_island_binstr*)pReadBuffAddr;
}



void ResortIslandDraw::SetFarmTouchBin(void* pReadBuffAddr)
{
  mpFarmTouch = (farm_touch_island_binstr*)pReadBuffAddr;
}
void ResortIslandDraw::SetHotSpaTouchBin(void* pReadBuffAddr)
{
  mpHotSpaTouch = (hotspa_touch_island_binstr*)pReadBuffAddr;
}
void ResortIslandDraw::SetFriendShipTouchBin(void* pReadBuffAddr)
{
  mpFriendShipTouch = (friendship_touch_island_binstr*)pReadBuffAddr;
}
void ResortIslandDraw::SetAdventureTouchBin(void* pReadBuffAddr)
{
  mpAdventureTouch = (adventure_touch_island_binstr*)pReadBuffAddr;
}
void ResortIslandDraw::SetGimTouchBin(void* pReadBuffAddr)
{
  mpGimTouch = (gim_touch_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetBeansPosBin(void* pReadBuffAddr)
{
  mpBeansPos = (beans_pos_island_binstr*)pReadBuffAddr;
}

/*
void ResortIslandDraw::SetPokeCameraBin(void* pReadBuffAddr)
{
  mpPokeCamera = (poke_camera_island_binstr*)pReadBuffAddr;
}
*/
void ResortIslandDraw::SetPokeCameraSimpleBin(void* pReadBuffAddr)
{
  mpPokeCameraSimple = (poke_camera_s_island_binstr*)pReadBuffAddr;
}

void ResortIslandDraw::SetPokeCameraIslandBin(void* pReadBuffAddr)
{
  mpPokeCameraIsland = (poke_camera_i_island_binstr*)pReadBuffAddr;
}




void ResortIslandDraw::SetLogic( ResortInitLogic* pLogic )
{
  mpLogic = pLogic;
}


//------------------------------------------------------------------
/**
 * @brief   初期化を行う関数
 * @return  初期化完了 true
 */
//------------------------------------------------------------------
bool ResortIslandDraw::InitFunc(void)
{
  switch(m_Seq){
  case GROUND_FILE_READ_:
    {
      if( ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_POKEICON) == NULL ) break;
      
      //OpenPokemonCapture();  ResortDataManagerに置き換えたのでコメントアウト。
      m_modelAllocator = GFL_NEW(  mpAppHeap->GetDeviceHeap() ) gfl2::util::GLHeapAllocator( mpAppHeap->GetDeviceHeap() );
      mpTempPokeCorePara = GFL_NEW(mpAppHeap->GetDeviceHeap()) pml::pokepara::CoreParam(mpAppHeap->GetDeviceHeap());

      StartInputExculusiveKeyMode();

      
      {
        // ポケモンアイコン3Dのヒープ
        mpPokeIcon3dTexParentHeap = GFL_CREATE_LOCAL_HEAP( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ), ResortHeapDefine::POKEICON_3D_TEX_ALL_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_08, mpPokeIcon3dTexParentHeap, "POKEICON_3D_TEX_ALL_SIZE");
#endif
        for(int i=0; i<BILL_DATA_NUM; ++i)
        {
          for(int j=0; j<2; ++j)
          {
            mpPokeIcon3dTexHeap[i][j] = GFL_CREATE_LOCAL_HEAP( mpPokeIcon3dTexParentHeap, ResortHeapDefine::POKEICON_3D_TEX_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
          }
        }
      }

      m_Seq = GROUND_FILE_WAIT_;
      mFileSeq = 0;
    }
    //    break;
  case GROUND_FILE_WAIT_:  //ファイル読み込み済み
    {
      m_Seq = SKYBOX_WAIT_;

      System::Skybox::Skybox::Description desc;
      desc.rotateRadY = 0.0f;

      if(!mpAppParam->mpSkybox->IsCreateResource()){
        gfl2::heap::HeapBase* pTemporaryHeap = mpAppHeap->GetDeviceHeap()->GetLowerHandle();  // kawa_resort_modify
        mpAppParam->mpSkybox->CreateHeap();
        mpAppParam->mpSkybox->StartCreateResource(desc, pTemporaryHeap);
        mbSkyboxNoDel=false;
      }
      else{
        mbSkyboxNoDel=true;
        m_Seq = SKYBOX_WAIT2_;
      }
    }
    break;
  case SKYBOX_WAIT_:
    mpAppParam->mpSkybox->UpdateCreateResource();
    if(mpAppParam->mpSkybox->IsCreateResource()){
      m_Seq = SKYBOX_WAIT2_;
    }
    else{
      break;
    }
  case SKYBOX_WAIT2_:
    mpAppParam->mpSkybox->CreateParts( NULL, true, System::nijiAllocator::VRAM_MALLOC_A);

    for(int ps = 0; ps < System::Skybox::Skybox::PARTS_TYPE_MAX; ps++ ){
      poke_3d::model::BaseModel* pBax = mpAppParam->mpSkybox->GetPartsModel((System::Skybox::Skybox::PartsType)ps);
      if(pBax){
        {
          gfl2::math::Vector3 size = pBax->GetScale();
          size.x *= 0.1f;
          size.y *= 0.1f;
          size.z *= 0.1f;
          pBax->SetScale(size);
          pBax->SetPosition(0, 0, 0);
        }
        m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY ,pBax);
      }
    }
    mpAppParam->mpSkybox->Initialize(System::Skybox::Skybox::SKY_TYPE_SUNNY);
    m_Seq = POKEMODEL_INIT_;
    break;
  case POKEMODEL_INIT_:

//    GroundCreate();

    {


      // EnvNode作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData resourceData;
        resourceData.SetBinaryEnvData( (c8*) mpResourceBuffer[GARC_islandground_envenv_COMP] );
        m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator, &resourceData);
        m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pNijiAllocator, m_pDrawEnvResourceRootNode);

        m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pDrawEnvNode);
        m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pDrawEnvNode, MODEL_LAYER_BG);

      }

      // Envモーションノード作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
        resourceData.SetMotionData( (c8*)mpResourceBuffer[GARC_islandground_envmot_COMP] );

        m_pDrawEnvMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pNijiAllocator, &resourceData);
      }

      m_EnvAccessor.Initialize( mpAppHeap->GetDeviceHeap(), m_pDrawEnvNode);
      m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
      m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);

      m_appRenderingManager->CreateDefaultCamera(mpAppHeap,  &m_drawEnvNode, &m_camera ,app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);

      m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_camera);
      m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_camera, MODEL_LAYER_BG);
      m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_camera, MODEL_LAYER_POKE);  // ポケモン用レイヤー

    }


    // 下画面ポケモンモデル用のライト
    this->createPokeLight();


    m_Seq = POKEMODEL_WAIT_;
//    break;
  case POKEMODEL_WAIT_:
    m_Seq = BILLBOARD_WAIT_;

    //CreateAll();  // CreateAllでつくられるものは生成/破棄を繰り返すので、mpAppHeapを使った破棄しないものの生成が完了してから呼び出すことにしたのでコメントアウト。
    

    break;

  case BILLBOARD_WAIT_:
    {
      m_pokeModelSys = GFL_NEW(mpPokeModelHeap) PokeTool::PokeModelSystem();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_OPEN_START);
#endif
      m_pokeModelSys->InitSystemAsync(mpPokeModelHeap, mpPokeModelHeap, mpAppParam->mpAsyncFileManager, 1);  // workHeapのほうはPokeModelSystemが内部でGetLowerHandleしてくれている
    }
    m_Seq = POEKMODEL_SYS_;
    break;
  case POEKMODEL_SYS_:
    {
      if( m_pokeModelSys->IsFinishInitSystemAsync() )
      {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_OPEN_END);
#endif

        m_pokeModelSys->CreateSystem(true, mpPokeModelNijiAllocator);

        //ポケモン用ヒープ作成 
        PokeTool::PokeModelSystem::HeapOption opt; 
        opt.animeType      = PokeTool::MODEL_ANIMETYPE_FIELD; ///メモリ確保用の最大定義
        opt.useIdModel     = false;
        opt.useShadow = false;
        opt.useColorShader = false; 
        m_pokeModelSys->CreatePokeModelHeap(mpPokeModelHeap, &opt); 

        m_Seq=0;
        return true;
      }
    }
    break;
  }

  return false;
}


//初期状態のカメラ
void ResortIslandDraw::InitCamera(void)
{
  {
    int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
    int datano = mpAppParam->firstOpenPos * 3 + lv;
    OHNO_PRINT("-*- %d %f\n", datano, mpstrx[datano].c6);

    m_TargetPos.x = mpstrx[datano].c1;
    m_TargetPos.y = mpstrx[datano].c2;
    m_TargetPos.z = mpstrx[datano].c3;
    ISLAND_DIST_INIT = mpstrx[datano].c4;
    ISLAND_HEIGHT_INIT = mpstrx[datano].c5;

    m_IslandDist = ISLAND_DIST_INIT;
    island_camera_rotate = 0.0f;

    CalcCamera( &m_TargetPos, island_camera_rotate, m_IslandDist,ISLAND_HEIGHT_INIT, mpstrx[datano].c6);

    m_camera->SetNear(mpstrx[datano].c7);
    m_camera->SetFar(mpstrx[datano].c8);
  }

}


void ResortIslandDraw::InitSetMoveTable(void)
{
  switch((Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos){
  case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
    mpHotSpaBin = (walk_hotspa_island_binstr*)mpFriendMoveBin;
    break;
  case Savedata::ResortSave::RESORT_KIND_FARM:
    mpHotSpaBin = (walk_hotspa_island_binstr*)mpFarmMoveBin;
    break;
  case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
    mpHotSpaBin = (walk_hotspa_island_binstr*)mpAdvMoveBin;
    break;
  case Savedata::ResortSave::RESORT_KIND_GIM:
    mpHotSpaBin = (walk_hotspa_island_binstr*)mpGimMoveBin;
    break;
  case Savedata::ResortSave::RESORT_KIND_HOTSPA:
    mpHotSpaBin = (walk_hotspa_island_binstr*)mpHotSpaMoveBin;
    break;
  }
}



//モデルによった時のカメラ
//注視点位置  角度、距離





void ResortIslandDraw::PokeNearCamera(int debugno)
{
#if PM_DEBUG
  if(debugno==2){
    return;
  }

#endif  
  if(PokeIndividualSimpleCamera()){
    return;
  }
  if(PokeIndividualIslandCamera()){
    return;
  }
  {//加算値なしで計算
    gfl2::math::Vector3 addpos(0.0f,0.0f,0.0f);
    PokeStanderdCamera(&addpos, 0.0f);
  }
}



void ResortIslandDraw::PokeStanderdCamera(gfl2::math::Vector3* addpos, float adddist)
{
  if(m_pokeModel==NULL){
  return;
  }
  OHNO_PRINT("PokeStanderdCamera ==> %f %f %f %f  \n",addpos->GetX(),addpos->GetY(),addpos->GetZ(),adddist);
  
  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 v3 = m_pokeModel->GetPosition();

#if PM_DEBUG
  mDebugZoom = adddist;
  mDebugAddPos.SetX(addpos->GetX());
  mDebugAddPos.SetY(addpos->GetY());
  mDebugAddPos.SetZ(addpos->GetZ());
#endif
  
  island_camera_rotate = mpStandPosBin[ mpAppParam->firstOpenPos ].c3;  //mpHotSpaBin[mPokeCoseIndex].c3;

  m_TargetPos = v3;  // ターゲットはポケモン位置
  m_IslandDist  = 400.0f;    //距離はアルファでの寺地君指示

  gfl2::math::AABB aabb;
  if( m_pokeModel->GetMotionCollision( &aabb ) ) {
    gfl2::math::Vector4 v4pos = aabb.GetCenter();
    
    const f32 height = GetCamAdjustHeight();

    m_TargetPos.SetX( v4pos.GetX()+v3.GetX()+addpos->GetX() );
    m_TargetPos.SetY( v4pos.GetY()+v3.GetY()+addpos->GetY() );
    m_TargetPos.SetZ( v4pos.GetZ()+v3.GetZ()+addpos->GetZ() );

    pos = m_TargetPos;
    pos.y +=  ((mpStandPosBin[ mpAppParam->firstOpenPos ].c5+adddist) * height ) / 4;
    pos.x += gfl2::math::FSin( island_camera_rotate ) * (mpStandPosBin[ mpAppParam->firstOpenPos ].c5+adddist) * height;
    pos.z += gfl2::math::FCos( island_camera_rotate ) * (mpStandPosBin[ mpAppParam->firstOpenPos ].c5+adddist) * height;

    SetCamera(&m_TargetPos, &pos, 0.38f);
  }
  else{
    GFL_ASSERT(0);//コリジョンデータが無いとカメラは動かない
  }
}




//カメラ簡易版
bool ResortIslandDraw::PokeIndividualSimpleCamera(void)
{
  bool rete = false;
  int i;

  if(m_TouchSimple.isEgg){
    return false;
  }

  for(i=0;i<_POKE_CAMERA_MAX;i++){
    if(mpPokeCameraSimple[i].c4 == 0){//終端
      break;
    }
    if(m_TouchSimple.monsNo != mpPokeCameraSimple[i].c4){
      continue;
    }
    if(m_TouchSimple.formNo != mpPokeCameraSimple[i].c5){
      continue;
    }
    
    {
      gfl2::math::Vector3 addpos(0.0f,0.0f,0.0f);

      {
        addpos.y = mpPokeCameraSimple[ i ].c1;
        PokeStanderdCamera( &addpos, mpPokeCameraSimple[ i ].c2);
      }
      OHNO_PRINT("PokeIndividualSimpleCamera\n");
      return true;
    }
    
  }
  return false;
}



//島ごとのカメラ
bool ResortIslandDraw::PokeIndividualIslandCamera(void)
{
  bool rete = false;
  int i;

  if(m_TouchSimple.isEgg){
    return false;
  }

  for(i=0;i<_POKE_CAMERA_MAX;i++){
    if(mpPokeCameraIsland[i].c6 == 0){//終端
      break;
    }
    if( mpAppParam->firstOpenPos != mpPokeCameraIsland[i].c8 ){
      continue;
    }
    if( m_TouchSimple.monsNo != mpPokeCameraIsland[i].c6 ){
      continue;
    }
    if( m_TouchSimple.formNo != mpPokeCameraIsland[i].c7 ){
      continue;
    }

    {
      gfl2::math::Vector3 addpos(0.0f,0.0f,0.0f);

      {
        addpos.x = mpPokeCameraIsland[ i ].c1;
        addpos.y = mpPokeCameraIsland[ i ].c2;
        addpos.z = mpPokeCameraIsland[ i ].c3;
        PokeStanderdCamera( &addpos, mpPokeCameraIsland[ i ].c4);
      }
      OHNO_PRINT("PokeIndividualIslandCamera\n");
      return true;
    }
    
  }
  return false;
}






void ResortIslandDraw::SetBillboardRotation(void)
{

  for(int x = 0 ;x < BILL_DATA_NUM; x++){
    if(mpFukidashiModels[x]){
      gfl2::math::Vector3 c_v3;
      GetPokemonPos(x,&c_v3);
      mpFukidashiModels[x]->SetPosition( c_v3.x, c_v3.y, c_v3.z);
    }
  }

}


void ResortIslandDraw::CalcCamera( gfl2::math::Vector3* pTargetPos, f32 rotate, f32 dist,f32 cameraHeight,f32 fovy)
{
  gfl2::math::Vector3 pos;

  pos.y = cameraHeight;
  pos.x = gfl2::math::SinDeg( rotate ) * dist;
  pos.z = gfl2::math::CosDeg( rotate ) * dist;

  SetCamera(pTargetPos, &pos, fovy);

}

//islandカメラ計算
void ResortIslandDraw::SetCamera( gfl2::math::Vector3* pTargetPos,gfl2::math::Vector3* pMyPos,  f32 fovy)
{
  
  gfl2::math::Vector3  targetPos(*pTargetPos);
  gfl2::math::Vector3 pos(*pMyPos);
  gfl2::math::Vector3 upv(0,1,0);

  m_camera->SetupCameraLookAt( pos ,  targetPos,  upv);
  //  const  gfl2::math::Vector3 tar(0.0f,0.0f,0.0f);
  //  m_camera->SetupCameraLookAt( pos ,  tar,  upv);
  m_camera->SetFovy(fovy);
}


void ResortIslandDraw::SimpleParamCreate(void)
{
  int maxnum = GetBillMax();
  for(int x = 0 ;x < maxnum; x++){
    GetPokemon(mpTempPokeCorePara, x);
    PokeTool::GetSimpleParam( &mPara[x] , mpTempPokeCorePara );
  }
}

void ResortIslandDraw::SimpleParamUpdate(void)
{
  if(mpAppParam->pSave->CheckUpdateFlg())
  {
    SimpleParamCreate();
  }
}


int ResortIslandDraw::GetSimpPokemonMonsNo(int x)
{
  if(mPara[x].isEgg){
    return MONSNO_TAMAGO;
  }
  return mPara[x].monsNo;
}




void ResortIslandDraw::GroundCreate(void)
{
	{
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[mpAppParam->groundNo]));
    GFL_ASSERT(mpResourceBuffer[mpAppParam->groundNo]);
    m_modelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);
    
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    GFL_ASSERT(mpResourceBuffer[mpAppParam->groundNo + 2]);
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[mpAppParam->groundNo + 2] ));
    m_pGroundAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    m_pBaseModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;

    m_pBaseModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_modelResourceNode);  // 自分でつくったアロケータ(ここではm_glAllocator)を渡すと解放でハングするので、

    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBaseModel, MODEL_LAYER_BG);  //地面

    m_pBaseModel->ChangeAnimationByResourceNode( m_pGroundAnimResourceNode, 0, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
    m_pBaseModel->SetAnimationLoop( true, 0, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackB;
    modelDataPackB.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_sea_one_day_anm_COMP] ));
    m_pGroundSeaAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackB);

    m_pBaseModel->ChangeAnimationByResourceNode( m_pGroundSeaAnimResourceNode, 1, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
    m_pBaseModel->SetAnimationLoop( true, 1, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
    m_pBaseModel->SetAnimationStepFrame( 0.0f, 1, poke_3d::model::BaseModel::ANIMATION_BIT_ALL );  // 海の色のアニメはUpdateでフレーム番号を指定して進める。
    {
      // 今の時間取得
      GameSys::AlolaTime date;
      date.Initialize();

      s32 hour    = date.GetHour();
      s32 minute  = date.GetMinute();
      s32 second  = date.GetSecond();

      this->updateSeaOneDayAnim( hour, minute, second );  // 海の色のアニメ(テクスチャUVアニメ)
    }
  }

  {
    s32 size = 0;

    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData modelDataPack;
    GFL_ASSERT(mpResourceBuffer[mpAppParam->groundNo + 1]);
    modelDataPack.SetModelData(static_cast<c8*>( mpResourceBuffer[mpAppParam->groundNo + 1] ) , size);
    m_collisionResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator, &modelDataPack);

    m_pCollisionModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) gfl2::collision::CollisionModel;
    m_pCollisionModel->Initialize(m_collisionResourceNode);
  }

  {
    Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;
    int x;
    int lv = mpAppParam->pSave->GetResortLevel(kind);

    if(Savedata::ResortSave::RESORT_KIND_FARM == kind ){

      if(lv==2){
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
        modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_kinomi_resort02_komono_COMP]));
        m_pGroundPartsModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
        modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_kinomi_resort02_komono_anm_COMP] ));
        m_pGroundPartsAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);
      }
      if(lv==3){
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
        modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_kinomi_resort03_komono_COMP]));
        m_pGroundPartsModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
        modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_kinomi_resort03_komono_anm_COMP] ));
        m_pGroundPartsAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);
      }
    }
    if(Savedata::ResortSave::RESORT_KIND_GIM == kind ){
      if(lv==3){
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
        modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_undou_resort03_komono_COMP]));
        m_pGroundPartsModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);
      }
    }

  }

  
  
  if(m_pGroundPartsModelResourceNode){

    m_pPartsModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;

    m_pPartsModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_pGroundPartsModelResourceNode);  // 自分でつくったアロケータ(ここではm_glAllocator)を渡すと解放でハングするので、

    m_pPartsModel->SetPosition(0, 0, 0);
    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pPartsModel, MODEL_LAYER_BG);  //地面パーツ
    if(m_pGroundPartsAnimResourceNode){
      m_pPartsModel->ChangeAnimationByResourceNode( m_pGroundPartsAnimResourceNode, 1);
      m_pPartsModel->SetAnimationLoop( true, 1);
    }
    
  }




}




void ResortIslandDraw::GroundReAdd(void)
{
  if(m_pPartsModel){
    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pPartsModel, MODEL_LAYER_BG);
  }
}


void ResortIslandDraw::GroundPause(void)
{
  if(m_pPartsModel){
    m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pPartsModel,MODEL_LAYER_BG);
  }


}


void ResortIslandDraw::GroundRemove(void)
{
  m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBaseModel,MODEL_LAYER_BG);
  if(m_pPartsModel){
    m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pPartsModel,MODEL_LAYER_BG);
  }
}


bool ResortIslandDraw::GroundDelete(void)
{
  if(m_pBaseModel==NULL){
    return true;
  }
  if(m_pPartsModel){
    if(!m_pPartsModel->CanDestroy()){
      return false;
    }
  }
  if(m_pBaseModel->CanDestroy()){

      GFL_SAFE_DELETE(m_pPartsModel);
      GFL_SAFE_DELETE( m_pGroundPartsAnimResourceNode);
      GFL_SAFE_DELETE(m_pGroundPartsModelResourceNode);
    
      GFL_SAFE_DELETE(m_collisionResourceNode);
      
      GFL_SAFE_DELETE( m_pCollisionModel );

    GFL_SAFE_DELETE(  m_pGroundSeaAnimResourceNode );
      GFL_SAFE_DELETE(m_pBaseModel);
      GFL_SAFE_DELETE( m_pGroundAnimResourceNode);
      GFL_SAFE_DELETE(m_modelResourceNode);
      return true;
  }
  return false;
}


gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* ResortIslandDraw::GetMeshNode( char *name, poke_3d::model::BaseModel* pBase )
{
  u32 num = pBase->GetMeshCount();
  for( u32 i=0;i<num;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = pBase->GetModelInstanceNode()->GetMeshLinkData(i);
  //     OHNO_PRINT("mesh_ohno>>  %s\n",pMeshLinkData->GetName());
    if( gfl2::std::MemComp( name,pMeshLinkData->GetName(),strlen(name))==0 )
    {
      return pMeshLinkData;
    }
  }
  return NULL;
}


gfl2::renderingengine::scenegraph::instance::JointInstanceNode* ResortIslandDraw::GetJointNode( char *name, poke_3d::model::BaseModel* pBase )
{
  u32 num = pBase->GetModelInstanceNode()->GetJointNum();
  for( u32 i=0;i<num;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* node = pBase->GetModelInstanceNode()->GetJointInstanceNode(i);

  //     OHNO_PRINT("join_ohno>>  %s\n",node->GetName());
    if( gfl2::std::MemComp( name,node->GetName(),strlen(name))==0 )
    {
      return node;
    }
  }
  return NULL;
}





gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* ResortIslandDraw::GetMaterialNode( char *name, poke_3d::model::BaseModel* pBase )
{
  u32 num = pBase->GetModelInstanceNode()->GetMaterialNum();
  for( u32 i=0;i<num;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = pBase->GetModelInstanceNode()->GetMaterialInstanceNode(i);

 //   OHNO_PRINT("mate >>  %s\n",mate->GetName());
    if( gfl2::std::MemComp( name, mate->GetName(),strlen(name))==0 )
    {
      return mate;
    }
  }
  return NULL;
}


int ResortIslandDraw::GetMaterialNodeIdx( char *name, poke_3d::model::BaseModel* pBase )
{
  u32 num = pBase->GetModelInstanceNode()->GetMaterialNum();
  for( u32 i=0;i<num;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = pBase->GetModelInstanceNode()->GetMaterialInstanceNode(i);

 //   OHNO_PRINT("mate >>  %s\n",mate->GetName());
    if( gfl2::std::MemComp( name, mate->GetName(),strlen(name))==0 )
    {
      return i;
    }
  }
  return -1; //影が無い等見つからないときがある GFNMCat[2472]
}




gfl2::renderingengine::scenegraph::instance::JointInstanceNode* ResortIslandDraw::getPokemonJoint(int index)
{
  char* nametbl[]={
    "joint01",
    "joint02",
    "joint03",
    "joint04",
    "joint05",
    "joint06",
    "joint07",
    "joint08",
    "joint09",
    "joint10",
    "joint11",
    "joint12",
    "joint13",
    "joint14",
    "joint15",
    "joint16",
    "joint17",
    "joint18",
    "joint19",
    "joint20",
    "joint21",
  };

  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = GetJointNode( nametbl[index], m_pBillboardModel );
  return pn;
}




bool ResortIslandDraw::SetApperPokemonMulScaleTable(int index, int frame)
{
  float scales[]={0.1f,1.3f,1.3f,1.3f,1.3f,1.0f,1.0f};
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( index );
  
  if(frame == 6){
    if(pn){
      pn->SetLocalScale(mPokemonIconDefScale);
    }
    return true;  //作業終了
  }
  SetPokemonMulScale(index, scales[frame]);
  return false;
}




bool ResortIslandDraw::SetSelectPokemonMulScaleTable(int frame)
{
  float scales[]={1.1f,1.1f,1.0f,0.9f,1.0f,1.0f,1.0f};
  //6フレ

  if(frame == 0){
    backupPokemonIconScale(m_TouchPokeIndex);
  }
  if(frame == 6){
    restorePokemonIconScale(m_TouchPokeIndex);
    return true;
  }
  SetPokemonMulScale(m_TouchPokeIndex,scales[frame]);
  return false;
}


void ResortIslandDraw::backupPokemonIconScale(int index)
{
	GFL_ASSERT(index<BILL_DATA_NUM);
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( index );
  if(pn){
    pokeIconBackupScale = pn->GetLocalScale();
  }
}

void ResortIslandDraw::restorePokemonIconScale(int index)
{
	GFL_ASSERT(index<BILL_DATA_NUM);
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( index );
  if(pn){
    pn->SetLocalScale(pokeIconBackupScale);
  }
}


void ResortIslandDraw::SetPokemonMulScale(int index,float scale)
{
	GFL_ASSERT(index<BILL_DATA_NUM);
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( index );
  if(pn){
			gfl2::math::Vector3	scalevec3(scale,scale,scale);

    scalevec3 = pn->GetLocalScale();

    OHNO_PRINT("%f %f %f -- \n",scalevec3.GetX(),scalevec3.GetY(),scalevec3.GetZ());

    scalevec3 *= scale;

    pn->SetLocalScale(scalevec3);
			
      //pn->SetLocalScale(scalevec3);
  }
}











bool ResortIslandDraw::GetPokemonPos(int index,gfl2::math::Vector3* ppos)
{

  if( m_billPokeNo[index]  )
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( index );
    if(pn){
      //gfl2::math::Vector3 vec3 = pn->GetLocalTranslate();  // ジョイントのTranslateの座標空間は不明なので、ローカル座標は使えない。
      const gfl2::math::Matrix34* mat34 = pn->GetWorldMatrixPtr();
      gfl2::math::Vector3 vec3(mat34->GetElem(0,3),  mat34->GetElem(1,3), mat34->GetElem(2,3));

      //      OHNO_PRINT("jointpos %f %f%f\n", vec3.GetX(),vec3.GetY(),vec3.GetZ());
      ppos->Set(vec3);
      return true;
    }
  }
  return false;
}


void ResortIslandDraw::SetPokemonVisible(int index, bool visible)
{

	gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode;
	gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTexNode;
	int no;

	char* nametbl[]={
		"egg01_f",
		"egg02_f",
		"egg03_f",
		"egg04_f",
		"egg05_f",
		"egg06_f",
		"egg07_f",
		"egg08_f",
		"egg09_f",
		"egg10_f",
		"egg11_f",
		"egg12_f",
		"egg13_f",
		"egg14_f",
		"egg15_f",
		"egg16_f",
		"egg17_f",
		"egg18_f",
		"egg19_f",
		"egg20_f",
		"egg21_f",
	};

	char* shanametbl[]={
    "poke_shadow_01",
    "poke_shadow_02",
    "poke_shadow_03",
    "poke_shadow_04",
    "poke_shadow_05",
    "poke_shadow_06",
    "poke_shadow_07",
    "poke_shadow_08",
    "poke_shadow_09",
    "poke_shadow_10",
    "poke_shadow_11",
    "poke_shadow_12",
    "poke_shadow_13",
    "poke_shadow_14",
    "poke_shadow_15",
    "poke_shadow_16",
    "poke_shadow_17",
    "poke_shadow_18",
    "poke_shadow_19",
    "poke_shadow_20",
    "poke_shadow_21",
  };
  
	int idx  = GetMaterialNodeIdx( nametbl[index] , m_pBillboardModel);
	int shaidx  = GetMaterialNodeIdx( shanametbl[index] , m_pBillboardModel);
  if(idx>=0){
    gfl2::math::Vector4 col = m_pBillboardModel->GetMaterialColorConstant(idx, 0);
		if(visible){
      col.w=1.0f;
		}
		else{
      col.w=0.0f;
		}
    m_pBillboardModel->SetMaterialColorConstant(idx,0,col);
    if(shaidx>=0){
      m_pBillboardModel->SetMaterialColorConstant(shaidx,0,col);
    }
	}
  if(mpFukidashiModels[index]){
    if(!visible){
      if(!mbFukidashiDisable){
        mpFukidashiModels[index]->SetVisible(visible);
      }
    }
  }
}



void ResortIslandDraw::SetPokemonMaterialInstance(int i,bool flip)
{
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode;
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTexNode;
  int no;
  
  char* nametbl[]={
    "egg01_f",
    "egg02_f",
    "egg03_f",
    "egg04_f",
    "egg05_f",
    "egg06_f",
    "egg07_f",
    "egg08_f",
    "egg09_f",
    "egg10_f",
    "egg11_f",
    "egg12_f",
    "egg13_f",
    "egg14_f",
    "egg15_f",
    "egg16_f",
    "egg17_f",
    "egg18_f",
    "egg19_f",
    "egg20_f",
    "egg21_f",
  };


  pMaterialNode = GetMaterialNode( nametbl[i] , m_pBillboardModel);
  if(!flip){
    pTexNode = mTexGetter.GetTextureResourceNode(m_CapResourceNode[i]);
    no=0;
  }
  else{
    pTexNode = mTexGetter.GetTextureResourceNode(m_CapResourceNodeRecv[i]);
    no=1;
  }
  if(pMaterialNode){
    pMaterialNode->SetTextureResourceNode( no, pTexNode);
		pMaterialNode->GetAttributeParam()->m_ConstantColor->a = 0;
  }


}


void ResortIslandDraw::GetPokemon(pml::pokepara::CoreParam* pPoke, int x)
{
  switch((Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos){
  case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
    if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
      mpAppParam->pSave->GetWildPokemon(x-Savedata::ResortSave::STOCKPOKEMON_MAX, pPoke);
    }
    else{
      mpAppParam->pSave->GetFriendShipPokemon(x, pPoke);
    }
    break;
  case Savedata::ResortSave::RESORT_KIND_FARM:
    if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
      pPoke->Clear();
    }
		else{
	    mpAppParam->pSave->GetFarmPokemon(x, pPoke);
		}
    break;
  case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
    {
      u8 h, m, s;
      mpAppParam->pSave->GetAdventureTime( &h, &m, &s );
      if( !( h == 0 && m == 0 && s == 0 ) ){
  			pPoke->Clear();
      }
      else if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
  			pPoke->Clear();
  		}
  		else{
  	    mpAppParam->pSave->GetAdventurePokemon(x, pPoke);
  		}
    }
    break;
  case Savedata::ResortSave::RESORT_KIND_GIM:
		if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
			pPoke->Clear();
		}
		else{
	    mpAppParam->pSave->GetGimPokemon(x, pPoke);
		}
    break;
  case Savedata::ResortSave::RESORT_KIND_HOTSPA:
		if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
			pPoke->Clear();
		}
		else{
	    mpAppParam->pSave->GetHotSpaPokemon(x, pPoke);
		}
    break;
  }
}



void ResortIslandDraw::SetPokemon(pml::pokepara::CoreParam* pPoke, int x)
{
  switch((Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos){
  case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
    if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
      mpAppParam->pSave->SetWildPokemon(x - Savedata::ResortSave::STOCKPOKEMON_MAX, pPoke);
    }
    else{
      mpAppParam->pSave->SetFriendShipPokemon(x, pPoke);
    }
    break;
  case Savedata::ResortSave::RESORT_KIND_FARM:
    if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
      pPoke->Clear();
    }
		else{
	    mpAppParam->pSave->SetFarmPokemon(x, pPoke);
		}
    break;
  case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
		if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
			pPoke->Clear();
		}
		else{
	    mpAppParam->pSave->SetAdventurePokemon(x, pPoke);
		}
    break;
  case Savedata::ResortSave::RESORT_KIND_GIM:
		if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
			pPoke->Clear();
		}
		else{
	    mpAppParam->pSave->SetGimPokemon(x, pPoke);
		}
    break;
  case Savedata::ResortSave::RESORT_KIND_HOTSPA:
		if(x>=Savedata::ResortSave::STOCKPOKEMON_MAX){
			pPoke->Clear();
		}
		else{
	    mpAppParam->pSave->SetHotSpaPokemon(x, pPoke, false);
		}
    break;
  }
}





/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortIslandDraw::OpenPokemonCapture(void)
{
  app::util::FileAccessor::RequestData frs;
  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_OPEN_START);
#endif

    frs.openr.arcId       = ARCID_POKEICON_3D;
    frs.openr.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.openr.heapForFile = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    frs.openr.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    pAsyncFileMan->SyncArcFileOpen( frs.openr );

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_OPEN_END);
#endif
  }
}
*/




/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortIslandDraw::ClosePokemonCapture(void)
{
  app::util::FileAccessor::RequestData frs;
  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    frs.closer.arcId       = ARCID_POKEICON_3D;
    frs.closer.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.closer.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    pAsyncFileMan->SyncArcFileClose( frs.closer );
  }
}
*/


void ResortIslandDraw::PokemonCaptureRead(int arcFileIndex, void** pReadBuff, gfl2::heap::HeapBase* heap)
{
  gfl2::heap::HeapBase* devMem  = heap;
  gfl2::heap::HeapBase* tempMem = mpPokeIcon3dTexParentHeap->GetLowerHandle();

  void*                    dataBuf = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_POKEICON);
  const gfl2::fs::ArcFile* arcFile = ResortDataManager::GetStaticInstance()->GetArcFile(ResortDataManager::FILE_ID_POKEICON);

  // オフセット
  size_t arcOffsetHead = 0;
  arcFile->GetOffsetFromArchiveTop(&arcOffsetHead, 0);
  size_t arcOffsetCurr = 0;
  arcFile->GetOffsetFromArchiveTop(&arcOffsetCurr, arcFileIndex);
  size_t bufOffset = arcOffsetCurr - arcOffsetHead;
 
  // サイズ
  //size_t compSize = 0;  // 展開にはリアルサイズのほうを使うので、こちらはコメントアウト。
  //arcFile->GetDataSize(&compSize, arcFileIndex, tempMem);
  size_t realCompSize = 0; 
  arcFile->GetRealDataSize(&realCompSize, arcFileIndex, tempMem);  // 一気読みしているので、ArcSrcを使いたくない。fs側で防ぐフラグがないのでこのままにしておく。

  // 圧縮データの先頭アドレス
  void* compBuf = reinterpret_cast<void*>( (reinterpret_cast<u32>(dataBuf)) + (static_cast<u32>(bufOffset)) );

  // 展開
  u32 uncompSize = gfl2::cx::GetUncompressedSize(compBuf, realCompSize);
  void* uncompBuf = GflHeapAllocMemoryAlign(devMem, uncompSize, 128);  // uncompBufは呼び出し元で解放してもらう
  u32 uncompRes = gfl2::cx::SecureUncompress( compBuf, realCompSize, uncompBuf, tempMem );
  GFL_ASSERT(uncompRes == 0);  // 開発中に気付かせるためのASSERT。解凍エラー。

  *pReadBuff = uncompBuf;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortIslandDraw::PokemonCaptureRead(int arcFileIndex, void** pReadBuff)
{
  app::util::FileAccessor::RequestData frs;
  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  {
    frs.loadr.datId      = arcFileIndex;  //poke                      // [in]  ファイルindex
    frs.loadr.arcId         = ARCID_POKEICON_3D;                               // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    frs.loadr.prio          = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;    // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    frs.loadr.ppBuf         = pReadBuff;                                  // [out] 読み込んだデータの格納先( 4 byte alignment )
    frs.loadr.heapForBuf    = mpAppHeap->GetDeviceHeap();                      // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    frs.loadr.heapForReq    = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    frs.loadr.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
    frs.loadr.align = 128;

    pAsyncFileMan->SyncArcFileLoadData( frs.loadr );
  }

}
*/



int ResortIslandDraw::GetBillMax(void)
{

	int maxnum=BILL_DATA_NUM;

  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP != (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos ){
//	if(Savedata::ResortSave::RESORT_KIND_HOTSPA == (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos ){
		maxnum = BILL_DATA_NUM_NOT;
	}   ///@caution 全部データがそろったら再度修正
	return maxnum;
}


//ビルボードポケモン生成
void ResortIslandDraw::BillCreate(void)
{
  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;
  int maxnum=GetBillMax();
  //モデル構造を調べる為のコード
  int lv = mpAppParam->pSave->GetResortLevel(kind)-1;

  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(   mpResourceBuffer[mpAppParam->groundNo + 5 ] ));
    m_billResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);
  }
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[mpAppParam->groundNo + 6] ));
    m_pAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);
  }

  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(   mpResourceBuffer[GARC_islandground_pokemon_icon_COMP ] ));
    mpFukidashiResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);
  }
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_pokemon_icon_anm_COMP] ));
    mpFukidashiAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);
  }


	if(Savedata::ResortSave::RESORT_KIND_ADVENTURE == kind )
  {
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackIn;
      modelDataPackIn.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_tanken_poke_in_anm_COMP] ));
      m_pTreasurePokemonInAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackIn);
    }
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackOut;
      modelDataPackOut.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_tanken_poke_out_anm_COMP] ));
      m_pTreasurePokemonOutAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackOut);
    }
  }


  m_pBillboardModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
  m_pBillboardModel->Create(m_pNijiAllocator,  mpAppHeap->GetDeviceHeap(), m_billResourceNode);
  m_pBillboardModel->SetPosition( 0 , 0, 0);

  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pn = getPokemonJoint( 0 );
    GFL_ASSERT(pn);
    mPokemonIconDefScale = pn->GetLocalScale();
  }

  
  m_pBillboardModel->ChangeAnimationByResourceNode( m_pAnimResourceNode, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
  m_pBillboardModel->SetAnimationLoop( true, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

  m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBillboardModel, MODEL_LAYER_BG);


  for(int y = 0 ;y < BILL_DATA_NUM; y++){  //初期化
    m_billPokeNo[y]=MONSNO_NULL;
  }
  
  for(int x = 0 ;x < maxnum; x++){
    mpFukidashiModels[x] = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
    mpFukidashiModels[x]->Create(m_pNijiAllocator,  mpAppHeap->GetDeviceHeap(), mpFukidashiResourceNode);
    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,mpFukidashiModels[x], MODEL_LAYER_BG);
    mpFukidashiModels[x]->SetAnimationLoop( true );

    { //ポケモンいない場合
      SetPokemonVisible(x,false);
      mpFukidashiModels[x]->SetVisible(false);
      m_billPokeNo[x]=MONSNO_NULL;
    }

  }

  BillPokemonChange();
}


void ResortIslandDraw::TreasurePokemonInAnim(void)
{

  m_pBillboardModel->ChangeAnimationByResourceNode( m_pTreasurePokemonInAnimResourceNode, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
  m_pBillboardModel->SetAnimationLoop( false, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

  Sound::PlaySE( SEQ_SE_SYS_NJ_053 );

    
}

bool ResortIslandDraw::TreasurePokemonAnimEnd(void)
{
  if(m_pBillboardModel->IsAnimationLastFrame(3)){
    return true;
  }
  return false;
}


void ResortIslandDraw::TreasurePokemonOutAnim(void)
{
  m_pBillboardModel->ChangeAnimationByResourceNode( m_pTreasurePokemonOutAnimResourceNode, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
  m_pBillboardModel->SetAnimationLoop( false, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

  Sound::PlaySE( SEQ_SE_SYS_NJ_057 );

}


void ResortIslandDraw::TreasurePokemonNormalAnim(void)
{
  m_pBillboardModel->ChangeAnimationByResourceNode( m_pAnimResourceNode, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
  m_pBillboardModel->SetAnimationLoop( true, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
}





void ResortIslandDraw::BillPokemonChange(void)
{
  if(m_pBillboardModel==NULL){
    return;
  }
  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ))
    ){
    //管理人画面起動まで反応なし
    return;
  }


  int maxnum=GetBillMax();

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_LOAD_START);
#endif
  for(int x = 0 ;x < maxnum; x++){

    if( GetSimpPokemonMonsNo(x)!=MONSNO_NULL && (m_billPokeNo[x] != GetSimpPokemonMonsNo(x) )){
      if(m_billPokeNo[x]!=MONSNO_NULL){
        mpFukidashiModels[x]->SetVisible(false);
        m_billPokeNo[x]=MONSNO_NULL;
        SetPokemonVisible(x,false);
        continue;
      }
      
      GetPokemon(mpTempPokeCorePara, x);
      GflHeapSafeFreeMemory(mpCapturePoke[x]);
      GflHeapSafeFreeMemory(mpCapturePokeRecv[x]);
      GFL_SAFE_DELETE(m_CapResourceNode[x]);
      GFL_SAFE_DELETE(m_CapResourceNodeRecv[x]);

      gfl2::fs::ArcFile::ARCDATID arci = app::tool::PokeIcon::GetResourceIndex( mpTempPokeCorePara );
      gfl2::fs::ArcFile::ARCDATID arcirecv = app::tool::PokeIcon::GetResourceIndex( mpTempPokeCorePara,true );

      PokemonCaptureRead( arci , &mpCapturePoke[x], mpPokeIcon3dTexHeap[x][0] );
      PokemonCaptureRead( arcirecv , &mpCapturePokeRecv[x], mpPokeIcon3dTexHeap[x][1] );

      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
        texData.SetTextureData(static_cast<c8*>(mpCapturePoke[ x ]));
        m_CapResourceNode[x] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_modelAllocator,&texData);

        texData.SetTextureData(static_cast<c8*>(mpCapturePokeRecv[ x ]));
        m_CapResourceNodeRecv[x] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_modelAllocator,&texData);
      }

      {
        m_billPokeNo[x] = mpTempPokeCorePara->GetMonsNo();

        if( mpTempPokeCorePara->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ){
          m_billPokeNo[x] = MONSNO_TAMAGO;
        }

        SetPokemonMaterialInstance(x,false);
        SetPokemonMaterialInstance(x,true);

      }
      SetPokemonVisible(x,true);

      //吹き出しの作成 リアルタイム変更
      {
        int id = mpTempPokeCorePara->GetResortEventStatus();

        mpFukidashiModels[x]->ChangeAnimationByResourceNode( mpFukidashiAnimResourceNode );
        m_billFukidashiOn[x] = _FUKIDASI_END;
        FukidashiChange(x,id);
      }
    }
		else if(m_billPokeNo[x] != MONSNO_NULL)
    { //ポケモンいない場合
      GetPokemon(mpTempPokeCorePara, x);
      if( mpTempPokeCorePara->IsNull()){
        SetPokemonVisible(x,false);
        mpFukidashiModels[x]->SetVisible(false);
        m_billPokeNo[x]=MONSNO_NULL;
      }
    }
  }
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_LOAD_END);
#endif

}


void ResortIslandDraw::FukidashiChange(int x,int id)
{
  if(mpFukidashiModels[x]==NULL){
    return;
  }


  if( m_billFukidashiOn[x] == _FUKIDASI_START){
    if(!mbFukidashiDisable){
      mpFukidashiModels[x]->SetVisible(true);
    }
    m_billFukidashiOn[x] = _FUKIDASI_END;
  }


  
//０ｆ：！マーク
//１ｆ：ハートマーク
//２ｆ：モヤモヤマーク
//３ｆ：？マーク

  /*
      RESORTEVENT_NONE =0,
      RESORTEVENT_SEIKAKU,  //性格表示     
      RESORTEVENT_CARE,      //かまってほしい
      RESORTEVENT_LIKE_RESORT,         //リゾート好き
      RESORTEVENT_LIKE_BATTLE,            //バトル好き
      RESORTEVENT_LIKE_ADV,           //冒険好き
      RESORTEVENT_GOOD_FRIEND,     //いちゃいちゃ状態
      RESORTEVENT_GIM,             //運動希望
      RESORTEVENT_HOTSPA,          //温泉希望
      RESORTEVENT_WILD,          //野生出現
      RESORTEVENT_WILD_LOVE,       //野生問題解決状態
      RESORTEVENT_WILD_LIVE,     // 野生が住んでいる  ==仲間になる
      RESORTEVENT_POKEMAME_GET1,    //ポケマメGET1
      RESORTEVENT_POKEMAME_GET2,    //ポケマメGET2
      RESORTEVENT_POKEMAME_GET3,    //ポケマメGET3
      RESORTEVENT_KINOMI_HELP,      // 15: きのみ栽培お手伝い
      RESORTEVENT_PLAY_STATE,       // 16: 遊びの様子
      RESORTEVENT_HOTSPA_STATE,     // 17: おんせんの様子
      RESORTEVENT_HOTSPA_DIZZY,     // 18: のぼせ
			RESORTEVENT_HOTSPA_EGG_HATCHING, //19:タマゴが孵化直前
     */
  //アイコンの見た目のテーブル
  int tbl[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,3,
    -1, 1,0,0,0,-1,-1,-1,2,0};
  int hukino = tbl[id];

//  OHNO_PRINT("吹き出し表示 %d %d\n", id, hukino);
  
  if(hukino >= 0){

    if(hukino==0){
      OHNO_PRINT("ふきだし０指定\n");
    }
    
    mpFukidashiModels[x]->SetAnimationStartFrame( hukino,0 );
    mpFukidashiModels[x]->SetAnimationEndFrame( hukino,0 );

    m_billFukidashiOn[x] = _FUKIDASI_START;

 //   if(!mbFukidashiDisable){
   //   mpFukidashiModels[x]->SetVisible(true);
//    }

  }
  else{
    mpFukidashiModels[x]->SetVisible(false);

  }

}




void ResortIslandDraw::_fukidashiUpdateCheck(void)
{


  if(mFukidasiUpdate < 0){
    int x;
    int maxnum=GetBillMax();
		for( x = 0 ;x < maxnum; x++){ //定期更新
			if(m_billPokeNo[x] != MONSNO_NULL){
	      GetPokemon(mpTempPokeCorePara, x);
        if( !mpTempPokeCorePara->IsNull()){
          int id = mpTempPokeCorePara->GetResortEventStatus();
          FukidashiChange(x,id);
        }
      }
    }
    mFukidasiUpdate = _FUKIDASHI_UPDATE;
  }
  mFukidasiUpdate--;
}






void ResortIslandDraw::SetFukidashiAllVisible(bool onoff)
{
  mbFukidashiDisable = !onoff;
	int x;
	int maxnum=GetBillMax();

  if(!onoff){
    for(x = 0 ;x < maxnum; x++){ //すぐ全部非表示
      mpFukidashiModels[x]->SetVisible(false);
    }
  }
}



void ResortIslandDraw::BillReAdd(void)
{
  int x;
  if(m_pBillboardModel)
  {
    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBillboardModel, MODEL_LAYER_BG);
  }

  for(x=0 ;x < BILL_DATA_NUM; x++)
  {
    if(mpFukidashiModels[x])
    {
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, mpFukidashiModels[x], MODEL_LAYER_BG);
    }
  }


}

void ResortIslandDraw::BillPause(void)
{
  BillRemove();
}



void ResortIslandDraw::BillRemove(void)
{
  int x;
  if(m_pBillboardModel)
  {
    m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBillboardModel,MODEL_LAYER_BG);
  }
  for(x=0 ;x < BILL_DATA_NUM; x++)
  {
    if(mpFukidashiModels[x])
    {
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, mpFukidashiModels[x],MODEL_LAYER_BG);
    }
  }
}



bool ResortIslandDraw::BillDelete(void)
{
  int x;
  if(m_pBillboardModel)
  {
    if(!m_pBillboardModel->CanDestroy())
    {
      return false;
    }
  }

  for(x=0 ;x < BILL_DATA_NUM; x++)
  {
    if(mpFukidashiModels[x])
    {
      if(!mpFukidashiModels[x]->CanDestroy())
      {
        return false;
      }
    }
  }

  GFL_SAFE_DELETE(m_pBillboardModel);
  for( x=0 ;x < BILL_DATA_NUM; x++){
    GflHeapSafeFreeMemory(mpCapturePoke[x]);
    GflHeapSafeFreeMemory(mpCapturePokeRecv[x]);
    GFL_SAFE_DELETE(m_CapResourceNode[x]);
    GFL_SAFE_DELETE(m_CapResourceNodeRecv[x]);
    GFL_SAFE_DELETE(mpFukidashiModels[x]);

  }
  GFL_SAFE_DELETE( mpFukidashiAnimResourceNode ); //ふきだしのアニメリソース
  GFL_SAFE_DELETE( mpFukidashiResourceNode ); //ふきだしのリソース

  GFL_SAFE_DELETE( m_pTreasurePokemonInAnimResourceNode );
  GFL_SAFE_DELETE( m_pTreasurePokemonOutAnimResourceNode );
  
  GFL_SAFE_DELETE(m_pAnimResourceNode);
  GFL_SAFE_DELETE(m_billResourceNode);

  return true;
}



void ResortIslandDraw::KinomiCreate(void)
{

  if(Savedata::ResortSave::RESORT_KIND_FARM == (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos )
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_kinomimdl_COMP]));
    m_pKinomiModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_kinomianm_COMP] ));
    m_pKinomiAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackB;
    modelDataPackB.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_kinomianm_ki_COMP] ));
    m_pKinomiKiAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackB);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackC;
    modelDataPackC.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_kinomianm_mi_COMP] ));
    m_pKinomiMiAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackC);

    /*
　　　　0:なし
　　　　1:花
　　　　2～15：花予備
　　　　16～82：木の実１番～67番
　　　　83～95：実予備
     */

    for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX ;x++){

      m_pKinomiModel[x].pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      m_pKinomiModel[x].pModel->Create(m_pNijiAllocator,
                                       mpAppHeap->GetDeviceHeap(), m_pKinomiModelResourceNode);
    }

    KinomiStateUpdate();
  }
}



void ResortIslandDraw::KinomiStateUpdate(void)
{
  if(m_pKinomiModel[0].pModel==NULL){  //初期化したかどうかみている
    return;
  }
  if(mbKinomiDeleteState){
    return;//終了処理中
  }

  
  for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX ;x++){
    int kinomino = mpAppParam->pSave->GetFarmItemKind(x);
    if( mpAppParam->pSave->GetFarmItemKind(x) != ITEM_DUMMY_DATA  && !m_pKinomiModel[x].bAdd)
    {

      m_pKinomiModel[x].bAdd = true;
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pKinomiModel[x].pModel, MODEL_LAYER_BG);

      m_pKinomiModel[x].pModel->SetPosition( mpKinomiTreeBin[x].c0 , mpKinomiTreeBin[x].c1, mpKinomiTreeBin[x].c2 );

      m_pKinomiModel[x].pModel->ChangeAnimationByResourceNode( m_pKinomiAnimResourceNode, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);
      m_pKinomiModel[x].pModel->SetAnimationLoop( true, 3, poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

      m_pKinomiModel[x].pModel->ChangeAnimationByResourceNode( m_pKinomiKiAnimResourceNode, 2, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
      m_pKinomiModel[x].pModel->SetAnimationLoop( false, 2, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

      m_pKinomiModel[x].pModel->ChangeAnimationByResourceNode( m_pKinomiMiAnimResourceNode, 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
      m_pKinomiModel[x].pModel->SetAnimationLoop( false, 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
      m_pKinomiModel[x].flower = 0;
      m_pKinomiModel[x].bview = true;
      //KinomiKindStateUpdate(x, kinomino);

    }
    else if(mpAppParam->pSave->GetFarmItemKind(x) == ITEM_DUMMY_DATA  && m_pKinomiModel[x].bAdd)
    {
      m_pKinomiModel[x].bAdd = false;
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pKinomiModel[x].pModel,MODEL_LAYER_BG);
    }
    KinomiGrowStateUpdate(x, kinomino);
  }
}



/*
●kinomi00.gfmdl
木のモデルデータ。木の成長、木の実の全種類データも既に含んでいます。

●kinomi00.gfmot
常時アニメデータ。常時アニメーションのアニメデータです。

●kinomi00_anim_ki.gfmot
木の成長アニメデータ。木の成長のためのアニメデータです。
以下のようになります。

フレーム
　　　　0:土
　　　　1:土（水やり後）
　　　　2:芽
　　　　3:芽（水やり後）
　　　　4:小木
　　　　5:小木（水やり後）
　　　　6:大木
　　　　7:大木（水やり後）

今回は水やりフレームは使わないかもしれませんね…。


●kinomi00_anim_mi.gfmot
木の実アニメデータ。木の実の種類変更のためのアニメデータです。
以下のようになります。

フレーム
　　　　0:なし
　　　　1:花
　　　　2～15：花予備
　　　　16～82：木の実１番～67番
　　　　83～95：実予備
*/
void ResortIslandDraw::KinomiGrowStateUpdate(int x, int kinomino)
{
  if(m_pKinomiModel[x].bAdd){
    u8 hour;
    u8 min;
    u8 sec;
    int flower=0;

    mpAppParam->pSave->GetFarmTime(x, &hour, &min, &sec);

    int buff[]={0,2,4,6};

    if(m_pKinomiModel[x].flower==0 && hour < 18){
      m_pKinomiModel[x].flower = 1;
      m_pKinomiModel[x].bview = true;
    }
    if(m_pKinomiModel[x].flower==1 && hour < 12){
      m_pKinomiModel[x].flower = 2;
      m_pKinomiModel[x].bview = true;
    }
    if(m_pKinomiModel[x].flower==2 && hour < 4){
      m_pKinomiModel[x].flower = 3;
      m_pKinomiModel[x].bview = true;
      flower = 1;
    }
    if(m_pKinomiModel[x].flower==3 && hour == 0 && min==0 && sec==0)
    {
      u8 originno = item::ITEM_GetNutsNo(kinomino);
      flower = 16 + originno;
      GFL_ASSERT(flower >= 16);
      GFL_ASSERT(flower <= 82);
      m_pKinomiModel[x].bview = true;
    }
    if(m_pKinomiModel[x].bview){

      if(flower){
        m_pKinomiModel[x].pModel->SetAnimationStartFrame( flower , 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
        m_pKinomiModel[x].pModel->SetAnimationEndFrame( flower , 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL );
      }
      else{
        m_pKinomiModel[x].pModel->SetAnimationStartFrame( 0 , 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
        m_pKinomiModel[x].pModel->SetAnimationEndFrame( 0 , 1, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL );
      }
      
      m_pKinomiModel[x].pModel->SetAnimationStartFrame( buff[m_pKinomiModel[x].flower] , 2, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
      m_pKinomiModel[x].pModel->SetAnimationEndFrame( buff[m_pKinomiModel[x].flower] , 2, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL );
      m_pKinomiModel[x].bview = false;
    }
  }
}



void ResortIslandDraw::KinomiUpdate(void)
{
  KinomiStateUpdate();

  for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX;x++){
    if(m_pKinomiModel[x].bAdd){
      m_pKinomiModel[x].pModel->UpdateAnimation();
    }
  }
}

void ResortIslandDraw::KinomiReAdd(void)
{
  mbKinomiDeleteState=false;

}



void ResortIslandDraw::KinomiPause(void)
{
  KinomiRemove();
}


void ResortIslandDraw::KinomiRemove(void)
{
  for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX;x++){
    if(m_pKinomiModel[x].bAdd){
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pKinomiModel[x].pModel,MODEL_LAYER_BG);
      m_pKinomiModel[x].bAdd=false;
    }
  }
  mbKinomiDeleteState=true;
}


bool ResortIslandDraw::KinomiDelete(void)
{
  for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX;x++){
    if(m_pKinomiModel[x].pModel)
    {
      if(!m_pKinomiModel[x].pModel->CanDestroy())
      {
        return false;
      }
    }
  }
  for(int x = 0;x < Savedata::ResortSave::RESORT_FARM_MAX;x++){
    GFL_SAFE_DELETE(m_pKinomiModel[x].pModel);
  }
  GFL_SAFE_DELETE(m_pKinomiKiAnimResourceNode);
  GFL_SAFE_DELETE(m_pKinomiMiAnimResourceNode);
  GFL_SAFE_DELETE(m_pKinomiAnimResourceNode);
  GFL_SAFE_DELETE(m_pKinomiModelResourceNode);
  mbKinomiDeleteState=false;

  return true;
}


//まめびんの作成
void ResortIslandDraw::BeansBottleCreate(void)
{
  int x;

  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_beans_bin_COMP]));
    m_pBeansBottleModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_bin_anm_COMP] ));
    m_pBeansBottleAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackB;
    modelDataPackB.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_bin_touch_anm_COMP] ));
    m_pBeansBottlePickupAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackB);

    m_pBeansBottleModel.pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
    m_pBeansBottleModel.pModel->Create(m_pNijiAllocator,mpAppHeap->GetDeviceHeap(), m_pBeansBottleModelResourceNode);
    
    m_pBeansBottleModel.pModel->AllocateAnimationByResourceNode(mpAppHeap->GetDeviceHeap(), false, m_pBeansBottleAnimResourceNode);
    m_pBeansBottleModel.pModel->AllocateAnimationByResourceNode(mpAppHeap->GetDeviceHeap(), false, m_pBeansBottlePickupAnimResourceNode);

    m_pBeansBottleModel.anim = BEANS_BOTTLE_ANIM_NONE;

    const gfl2::math::Vector3 c_v3 = m_pBeansBottleModel.pModel->GetPosition();
    gfl2::math::Vector3 v3(c_v3);
    UpdateCollision_Height( &v3 );
    OHNO_PRINT("まめびんのいちx1 %f\n",v3.y );
    
    m_pBeansBottleModel.pModel->SetPosition(v3);

    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansBottleModel.pModel, MODEL_LAYER_BG);
    m_pBeansBottleModel.pModel->SetVisible(false);

  }

  PutBeansBottle();

}

//アニメの処理
void ResortIslandDraw::BeansBottleUpdate(void)
{
  if(m_pBeansBottleModel.pModel){
    if(m_pBeansBottleModel.pModel->IsCreated()){
      m_pBeansBottleModel.pModel->UpdateAnimation();
      if(m_pBeansBottleModel.pModel->IsAnimationLastFrame() && m_pBeansBottleModel.anim==BEANS_BOTTLE_ANIM_PICKUP ){
        m_pBeansBottleModel.pModel->SetVisible(false);
        m_pBeansBottleModel.anim = BEANS_BOTTLE_ANIM_NONE;
      }
    }
  }
}


/*
   豆びんを更新する おく
 */
void ResortIslandDraw::PutBeansBottle(void)
{

  if( mpAppParam->pSave->IsBeansBottleGround(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) ){ //まめびんがある
    if(m_pBeansBottleModel.anim == BEANS_BOTTLE_ANIM_NONE && m_pBeansBottleModel.pModel){  //表示していない もでるはある
      
      gfl2::math::Vector3 pos;
      GetBottleCheckPos(&pos);
      OHNO_PRINT("まめびんのいちx2 %f\n",pos.y );
      m_pBeansBottleModel.pModel->SetPosition( pos.x , pos.y, pos.z );
 //     m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansBottleModel.pModel);
      m_pBeansBottleModel.pModel->ChangeAnimationByResourceNode( m_pBeansBottleAnimResourceNode);  //アニメ開始
      m_pBeansBottleModel.pModel->SetAnimationLoop( true);
      
      m_pBeansBottleModel.pModel->SetAnimationFrame(0.0f);
      
      m_pBeansBottleModel.pModel->SetVisible(true);
      m_pBeansBottleModel.anim = BEANS_BOTTLE_ANIM_ADD;
    }
  }

}


bool ResortIslandDraw::BeansBottleTouchAnim(void)
{
  if(m_pBeansBottleModel.anim == BEANS_BOTTLE_ANIM_ADD){
    m_pBeansBottleModel.anim = BEANS_BOTTLE_ANIM_PICKUP;
    m_pBeansBottleModel.pModel->ChangeAnimationByResourceNode( m_pBeansBottlePickupAnimResourceNode);
    m_pBeansBottleModel.pModel->SetAnimationLoop( false);
    m_pBeansBottleModel.pModel->SetAnimationFrame(0.0f);
    return true;
  }
  return false;
}

bool ResortIslandDraw::IsBeansBottleTouchLastFrame(void)
{
  if(m_pBeansBottleModel.anim == BEANS_BOTTLE_ANIM_NONE){
    return true;
  }
  return false;
}

void ResortIslandDraw::BeansBottleRemove(void)
{
  if(m_pBeansBottleModel.pModel){
    m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBeansBottleModel.pModel,MODEL_LAYER_BG);
  }
}




bool ResortIslandDraw::BeansBottleDelete(void)
{
  {
    if(m_pBeansBottleModel.pModel){
      if(!m_pBeansBottleModel.pModel->CanDestroy())
      {
        return false;
      }
    }
  }
  GFL_SAFE_DELETE(m_pBeansBottleModel.pModel);

  GFL_SAFE_DELETE(m_pBeansBottlePickupAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansBottleAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansBottleModelResourceNode);
  return true;
}

////---------------------------------------------------------------------------------------

//まめのきの作成
void ResortIslandDraw::BeansTreeCreate(void)
{
  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;
  int x;

  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP != kind ){
    return;
  }
  int lv = mpAppParam->pSave->GetResortLevel(kind)-1;
  
  {
    int garcno[]={GARC_islandground_beans_tree01_COMP,GARC_islandground_beans_tree02_COMP,GARC_islandground_beans_tree03_COMP};
    int garcanmno[]={GARC_islandground_beans_tree01_anm_COMP,GARC_islandground_beans_tree02_anm_COMP,GARC_islandground_beans_tree03_anm_COMP};



    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[garcno[ lv ]]));
    m_pBeansTreeModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>( mpResourceBuffer[garcanmno[ lv ] ] ));
    m_pBeansTreeAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    {
      mBeansTreeModel.pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      mBeansTreeModel.pModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_pBeansTreeModelResourceNode);

      mBeansTreeModel.pModel->SetPosition( 0.0f , 0.0f, 0.0f );
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, mBeansTreeModel.pModel, MODEL_LAYER_BG);

    }
  }


}

//アニメの処理
void ResortIslandDraw::BeansTreeUpdate(void)
{
  if(mBeansTreeModel.pModel){
    if(mBeansTreeModel.pModel->IsCreated()){
      mBeansTreeModel.pModel->UpdateAnimation();
    }
  }
}


void ResortIslandDraw::BeansTreeReAdd(void)
{
  if(mBeansTreeModel.pModel){
    m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, mBeansTreeModel.pModel, MODEL_LAYER_BG);
  }
}


void ResortIslandDraw::BeansTreePause(void)
{
  BeansTreeRemove();
}





void ResortIslandDraw::BeansTreeRemove(void)
{
  if(mBeansTreeModel.pModel){
    m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, mBeansTreeModel.pModel,MODEL_LAYER_BG);
  }
}


void ResortIslandDraw::BeansTreeAnim(void)
{
  if(mBeansTreeModel.pModel){
    mBeansTreeModel.pModel->ChangeAnimationByResourceNode( m_pBeansTreeAnimResourceNode, 0);  //アニメ開始
    mBeansTreeModel.pModel->SetAnimationLoop( false, 0);
  }
}


bool ResortIslandDraw::BeansTreeDelete(void)
{
  {
    if(mBeansTreeModel.pModel){
      if(!mBeansTreeModel.pModel->CanDestroy())
      {
        return false;
      }
    }
  }
  GFL_SAFE_DELETE(mBeansTreeModel.pModel);
  GFL_SAFE_DELETE(m_pBeansTreeAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansTreeModelResourceNode);
  return true;
}


////-------------------------------------------------------------------------------------


//まめＢＯＸの作成
void ResortIslandDraw::BeansBoxCreate(void)
{
  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;

  int lv = mpAppParam->pSave->GetResortLevel(kind);
  int num = mpAppParam->pSave->GetBeansBoxNum(kind);

  GFL_ASSERT(lv!=0);
  lv=lv-1;
  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }

  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_bean_box01_0_COMP + lv * 2]));  //状況に応じたまめばこ
    m_pBeansBoxModelResourceNode[0] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_bean_box01_0_COMP + lv * 2 + 1]));  //状況に応じたまめばこ
    m_pBeansBoxModelResourceNode[1] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>( mpResourceBuffer[GARC_islandground_bean_box_anm_COMP] ));
    m_pBeansBoxAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    for(int x=0;x<_BEANSBOX_MAX;x++)
    {
      m_pBeansBoxModel[x].pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      m_pBeansBoxModel[x].pModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_pBeansBoxModelResourceNode[x]);

      gfl2::math::Vector3 pos;
      GetPokeBoxPos(&pos);

      OHNO_PRINT("まめＢＯＸ %f %f %f\n",pos.x, pos.y, pos.z);
      
      m_pBeansBoxModel[x].pModel->SetPosition( pos.x, pos.y, pos.z );
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansBoxModel[x].pModel, MODEL_LAYER_BG);

    }
  }
  {
    int m = GetBeansBoxTouchAnimNum();
    mBeansBoxAnimNo = m;  //終了時に確認する用 GFNMCat[3060]
  }

}


//タッチアニメの終了判定 mBeansBoxAnimNoにどちらのアニメか記憶している
bool ResortIslandDraw::IsBeansBoxTouchAnimLastFrame(void)
{
  if(m_pBeansBoxModel[mBeansBoxAnimNo].pModel){
    return m_pBeansBoxModel[mBeansBoxAnimNo].pModel->IsAnimationLastFrame();
  }
  return true;
}


int ResortIslandDraw::GetBeansBoxTouchAnimNum(void)
{
  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;
  int lv = mpAppParam->pSave->GetResortLevel(kind);
  int num = mpAppParam->pSave->GetBeansBoxNum(kind);

  GFL_ASSERT(lv!=0);
  lv=lv-1;
  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }
  if(!mpLogic->GetBonusTimerMove()){
    num=0;  //GFNMCat[2289]
  }
  return num;
}


void ResortIslandDraw::BeansBoxTouchAnim(void)
{
  int num = GetBeansBoxTouchAnimNum();


  GFL_ASSERT(m_pBeansBoxModel[num].pModel);
  if(m_pBeansBoxModel[num].pModel){
    m_pBeansBoxModel[num].pModel->ChangeAnimationByResourceNode( m_pBeansBoxAnimResourceNode, 0);  //アニメ開始
    m_pBeansBoxModel[num].pModel->SetAnimationLoop( false, 0);

    mBeansBoxAnimNo = num;  //終了時に確認する用
    
  }
}






//アニメの処理
void ResortIslandDraw::BeansBoxUpdate(void)
{
  int num = mBeansBoxAnimNo;

  if(num==-1){
    num = GetBeansBoxTouchAnimNum();
  }
  
  if(m_pBeansBoxModel[num].pModel){
    if(m_pBeansBoxModel[num].pModel->IsCreated()){
      if(!m_pBeansBoxModel[num].pModel->IsVisible()){
	      m_pBeansBoxModel[num].pModel->SetVisible(true);
			}
      m_pBeansBoxModel[num].pModel->UpdateAnimation();
    }
  }
  num=1-num;

  if(m_pBeansBoxModel[num].pModel){
    if(m_pBeansBoxModel[num].pModel->IsCreated()){
      m_pBeansBoxModel[num].pModel->SetVisible(false);
    }
  }
  
}


void ResortIslandDraw::BeansBoxReAdd(void)
{
  for(int x=0;x<_BEANSBOX_MAX;x++){
    if(m_pBeansBoxModel[x].pModel){
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansBoxModel[x].pModel, MODEL_LAYER_BG);
    }
  }


}




void ResortIslandDraw::BeansBoxPause(void)
{
  BeansBoxRemove();
}



void ResortIslandDraw::BeansBoxRemove(void)
{
  for(int x=0;x<_BEANSBOX_MAX;x++){
    if(m_pBeansBoxModel[x].pModel){
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansBoxModel[x].pModel,MODEL_LAYER_BG);
    }
  }
}




bool ResortIslandDraw::BeansBoxDelete(void)
{
  for(int x=0;x<_BEANSBOX_MAX;x++)
  {
    if(m_pBeansBoxModel[x].pModel){
      if(!m_pBeansBoxModel[x].pModel->CanDestroy())
      {
        return false;
      }
    }
    GFL_SAFE_DELETE(m_pBeansBoxModel[x].pModel);
  }
  GFL_SAFE_DELETE(m_pBeansBoxAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansBoxModelResourceNode[1]);
  GFL_SAFE_DELETE(m_pBeansBoxModelResourceNode[0]);
  return true;
}



////-------------------------------------------------------------------------------------


//宝箱ＢＯＸの作成
void ResortIslandDraw::TreasureBoxCreate(void)
{

  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;

	if(Savedata::ResortSave::RESORT_KIND_ADVENTURE != kind ){
		return;
	}


  int lv = mpAppParam->pSave->GetResortLevel(kind);
  int num = mpAppParam->pSave->GetTreasureItemNum();

  GFL_ASSERT(lv!=0);
  lv=lv-1;
  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }

  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_treasure_box01_0_COMP + lv * 2 ]));  //状況に応じたまめばこ
    m_pTreasureBoxModelResourceNode[0] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_treasure_box01_0_COMP + lv * 2 + 1]));  //状況に応じたまめばこ
    m_pTreasureBoxModelResourceNode[1] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>( mpResourceBuffer[GARC_islandground_treasure_box_anm_COMP] ));
    m_pTreasureBoxAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    for(int x=0;x<_TREASUREBOX_MAX;x++){
      m_pTreasureBoxModel[x].pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      m_pTreasureBoxModel[x].pModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_pTreasureBoxModelResourceNode[x]);

      gfl2::math::Vector3 pos;
      GetTreasureCheckPos(&pos);  

      OHNO_PRINT("たからＢＯＸ %f %f %f\n",pos.x, pos.y, pos.z);
      
      m_pTreasureBoxModel[x].pModel->SetPosition( pos.x, pos.y, pos.z );
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pTreasureBoxModel[x].pModel, MODEL_LAYER_BG);

    }
  }

}


void ResortIslandDraw::TreasureBoxTouchAnim(void)
{

  int num = mpAppParam->pSave->GetTreasureItemNum();

  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }

  GFL_ASSERT(m_pTreasureBoxModel[num].pModel);
  if(m_pTreasureBoxModel[num].pModel){
    m_pTreasureBoxModel[num].pModel->ChangeAnimationByResourceNode( m_pTreasureBoxAnimResourceNode, 0);  //アニメ開始
    m_pTreasureBoxModel[num].pModel->SetAnimationLoop( false, 0);
  }
}


bool ResortIslandDraw::IsTreasureBoxTouchAnimLastFrame(void)
{
  int num = mpAppParam->pSave->GetTreasureItemNum();
  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }
  GFL_ASSERT(m_pTreasureBoxModel[num].pModel);
  if(m_pTreasureBoxModel[num].pModel){
    return m_pTreasureBoxModel[num].pModel->IsAnimationLastFrame(0);
  }
  return true;
}




//アニメの処理
void ResortIslandDraw::TreasureBoxUpdate(void)
{
	//数を確認し表示変更

  int num = mpAppParam->pSave->GetTreasureItemNum();

  if(num){
    num = 1; //入ってなければ０　入ってたら１
  }

  if(m_pTreasureBoxModel[num].pModel){
    if(m_pTreasureBoxModel[num].pModel->IsCreated()){
			m_pTreasureBoxModel[num].pModel->SetVisible(true);
      m_pTreasureBoxModel[num].pModel->UpdateAnimation();
    }
  }
  num=1-num;
  
  if(m_pTreasureBoxModel[num].pModel){
	  m_pTreasureBoxModel[num].pModel->SetVisible(false);
  }

}

void ResortIslandDraw::TreasureBoxReAdd(void)
{
	for(int x=0;x<_TREASUREBOX_MAX;x++){
    if(m_pTreasureBoxModel[x].pModel){
      m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pTreasureBoxModel[x].pModel, MODEL_LAYER_BG);
    }
	}
}


void ResortIslandDraw::TreasureBoxPause(void)
{
  TreasureBoxRemove();
}


void ResortIslandDraw::TreasureBoxRemove(void)
{
	for(int x=0;x<_TREASUREBOX_MAX;x++){
    if(m_pTreasureBoxModel[x].pModel){
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pTreasureBoxModel[x].pModel,MODEL_LAYER_BG);
    }
	}
}




bool ResortIslandDraw::TreasureBoxDelete(void)
{
	for(int x=0;x<_TREASUREBOX_MAX;x++)
  {
    if(m_pTreasureBoxModel[x].pModel){
      if(!m_pTreasureBoxModel[x].pModel->CanDestroy())
      {
        return false;
      }
    }
		GFL_SAFE_DELETE(m_pTreasureBoxModel[x].pModel);
  }
  GFL_SAFE_DELETE(m_pTreasureBoxAnimResourceNode);
  GFL_SAFE_DELETE(m_pTreasureBoxModelResourceNode[0]);
	GFL_SAFE_DELETE(m_pTreasureBoxModelResourceNode[1]);

  return true;
}



////-------------------------------------------------------------------------------------


//かんばんの作成
void ResortIslandDraw::SignBoardCreate(void)
{
  // 看板の種類
  enum
  {
    SIGNBOARD_TYPE_KINOMI,  //!< 看板きのみ用          // signboard
    SIGNBOARD_TYPE_COURSE,  //!< 看板たんけんコース用  // signboard_course
    SIGNBOARD_TYPE_BALL,    //!< 看板運動・温泉用      // signboard_ball
    SIGNBOARD_TYPE_NUM
  };
  // 看板の種類に応じたgarcのdatId
  static const u32 GARC_SIGNBOARD_DAT_ID[SIGNBOARD_TYPE_NUM][2] =
  {
    { GARC_islandground_signboard_COMP       , GARC_islandground_signboard_anm_COMP        },
    { GARC_islandground_signboard_course_COMP, GARC_islandground_signboard_course_anm_COMP },
    { GARC_islandground_signboard_ball_COMP  , GARC_islandground_signboard_ball_anm_COMP   },
  };
  // このリゾートでどの看板を使うか
  static const u32 RESORT_SIGNBOARD_TYPE[Savedata::ResortSave::RESORT_KIND_MAX] =
  {
    SIGNBOARD_TYPE_KINOMI,  // ふれあい
    SIGNBOARD_TYPE_KINOMI,  // きのみ
    SIGNBOARD_TYPE_COURSE,  // たんけん
    SIGNBOARD_TYPE_BALL,    // うんどう
    SIGNBOARD_TYPE_BALL,    // おんせん
  };


  Savedata::ResortSave::PokeResortKind kind =  (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos;


  u32 garcSingBoardDatIdModel = GARC_SIGNBOARD_DAT_ID[RESORT_SIGNBOARD_TYPE[kind]][0];
  u32 garcSingBoardDatIdAnim  = GARC_SIGNBOARD_DAT_ID[RESORT_SIGNBOARD_TYPE[kind]][1];


  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[garcSingBoardDatIdModel]));  //かんばん
    m_pSignBoardModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>( mpResourceBuffer[garcSingBoardDatIdAnim] ));
    m_pSignBoardAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);

    for(int z=0;z<_SHINBOARD_MAX;z++)
    {
      m_pSignBoardModel[z].pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      m_pSignBoardModel[z].pModel->Create(m_pNijiAllocator, mpAppHeap->GetDeviceHeap(), m_pSignBoardModelResourceNode);
      
      m_pSignBoardModel[z].pModel->AllocateAnimationByResourceNode(mpAppHeap->GetDeviceHeap(), false, m_pSignBoardAnimResourceNode);

      gfl2::math::Vector3 pos;
      if(GetSignBoardPos(&pos, z)){
        OHNO_PRINT("かんばん %f %f %f\n",pos.x, pos.y, pos.z);
        m_pSignBoardModel[z].pModel->SetPosition( pos.x, pos.y, pos.z );
        m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pSignBoardModel[z].pModel, MODEL_LAYER_BG);
      }
    }
  }
}


void ResortIslandDraw::SignBoardTouchAnim(int z)
{
  GFL_ASSERT(m_pSignBoardModel[z].pModel);
  if(m_pSignBoardModel[z].pModel){
    m_pSignBoardModel[z].pModel->ChangeAnimationByResourceNode( m_pSignBoardAnimResourceNode, 0);  //アニメ開始
    m_pSignBoardModel[z].pModel->SetAnimationLoop( false, 0);
  }
}

bool ResortIslandDraw::IsSignBoardTouchAnim(int z)
{
  GFL_ASSERT(m_pSignBoardModel[z].pModel);
  if(m_pSignBoardModel[z].pModel){
    return m_pSignBoardModel[z].pModel->IsAnimationLastFrame(0);
  }
  return true;
}


//アニメの処理
void ResortIslandDraw::SignBoardUpdate(void)
{
  for(int z=0;z<_SHINBOARD_MAX;z++){
    if(m_pSignBoardModel[z].pModel){
      if(m_pSignBoardModel[z].pModel->IsCreated()){
        m_pSignBoardModel[z].pModel->UpdateAnimation();
      }
    }
  }
}


void ResortIslandDraw::SignBoardReAdd(void)
{
  for(int z=0;z<_SHINBOARD_MAX;z++){
    if(m_pSignBoardModel[z].pModel){
      gfl2::math::Vector3 pos;
      if(GetSignBoardPos(&pos, z)){
        OHNO_PRINT("看板表示 %d\n",z);
        m_pSignBoardModel[z].pModel->SetPosition( pos.x, pos.y, pos.z );
        m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pSignBoardModel[z].pModel, MODEL_LAYER_BG);
      }
    }
  }
}

void ResortIslandDraw::SignBoardPause(void)
{
  SignBoardRemove();
}



void ResortIslandDraw::SignBoardRemove(void)
{
  for(int z=0;z<_SHINBOARD_MAX;z++){
    if(m_pSignBoardModel[z].pModel){
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pSignBoardModel[z].pModel,MODEL_LAYER_BG);
    }
  }
}




bool ResortIslandDraw::SignBoardDelete(void)
{
  for(int z=0;z<_SHINBOARD_MAX;z++)
  {
    if(m_pSignBoardModel[z].pModel){
      if(!m_pSignBoardModel[z].pModel->CanDestroy())
      {
        return false;
      }
    }
		GFL_SAFE_DELETE(m_pSignBoardModel[z].pModel);
  }
  GFL_SAFE_DELETE(m_pSignBoardAnimResourceNode);
  GFL_SAFE_DELETE(m_pSignBoardModelResourceNode);
  return true;
}



////-------------------------------------------------------------------------------------


void ResortIslandDraw::BeansCreate(void)
{
  int x;

  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP == (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos )
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(mpResourceBuffer[GARC_islandground_beans_COMP]));
    m_pBeansModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackA;
    modelDataPackA.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_anm_COMP] ));
    m_pBeansAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackA);


    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackB;
    modelDataPackB.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_type_anm_COMP] ));
    m_pBeansTypeAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackB);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackC;
    modelDataPackC.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_in_anm_COMP] ));
    m_pBeansInAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackC);


    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData modelDataPackD;
    modelDataPackD.SetMotionData(static_cast<c8*>(  mpResourceBuffer[GARC_islandground_beans_roll_anm_COMP] ));
    m_pBeansRollAnimResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPackD);
    
    
    
    for( x = 0;x < BEANS_DATA_NUM;x++){
      m_pBeansModel[x].pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
      m_pBeansModel[x].pModel->Create(m_pNijiAllocator,mpAppHeap->GetDeviceHeap(), m_pBeansModelResourceNode);
      m_pBeansModel[x].anim = BEANS_ANIM_NONE;
    }

    for( x = 0;x < BEANS_DATA_NUM;x++){
      if( mpAppParam->pSave->GetGroundBeans(x) != Savedata::ResortSave::POKEBEANS_NONE){ //まめがある
        m_pBeansModel[x].anim = BEANS_ANIM_START;
      }
    }

  }

  PutBeans(false);
}


void ResortIslandDraw::BeansReAdd(void)
{
  for(int x = 0;x < BEANS_DATA_NUM;x++){
    if(m_pBeansModel[x].anim != BEANS_ANIM_NONE){  //表示している
      if(m_pBeansModel[x].pModel){
        m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBeansModel[x].pModel, MODEL_LAYER_BG);
      }
    }
  }
}


void ResortIslandDraw::BeansPause(void)
{
  BeansRemove();
}



void ResortIslandDraw::BeansRemove(void)
{
  for(int x = 0;x < BEANS_DATA_NUM;x++){
    if(m_pBeansModel[x].pModel){
      m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBeansModel[x].pModel,MODEL_LAYER_BG);
    }
  }
}


void ResortIslandDraw::BeansAnim(void)
{
  for(int x = 0;x < BEANS_DATA_NUM;x++){
    if(m_pBeansModel[x].pModel){
      m_pBeansModel[x].pModel->ChangeAnimationByResourceNode( m_pBeansRollAnimResourceNode, 0);  //アニメ開始
      m_pBeansModel[x].pModel->SetAnimationLoop( false, 0);
    }
  }
}




bool ResortIslandDraw::BeansDelete(void)
{
  for(int x = 0;x < BEANS_DATA_NUM;x++)
  {
    if(m_pBeansModel[x].pModel){
      if(!m_pBeansModel[x].pModel->CanDestroy())
      {
        return false;
      }
    }
  }
  for(int x = 0;x < BEANS_DATA_NUM;x++){
    GFL_SAFE_DELETE(m_pBeansModel[x].pModel);
  }
  GFL_SAFE_DELETE(m_pBeansRollAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansInAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansTypeAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansAnimResourceNode);
  GFL_SAFE_DELETE(m_pBeansModelResourceNode);
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
bool ResortIslandDraw::EndFunc(void)
{
  switch(m_Seq){
  case 0:
    if(m_asyncCreateDestroyPokeModelStep){
      if(!asyncCreatePokeModel()){
        return false;
      }
    }
    m_Seq++;
  case 1:
    if(false == DeletePokemonASync()){
      return false;
    }

    mpAppParam->mpSkybox->Terminate();
    mpAppParam->mpSkybox->DestroyParts();

    if(m_pokeModelSys){//GFNMCat 5696
      m_pokeModelSys->TermSystemAsync();
    }
    m_Seq++;
    break;
  case 2:
    if(m_pokeModelSys){
      if( m_pokeModelSys->IsFinishTermSystemAsync() ){
        m_pokeModelSys->DeletePokeModelHeap();
        GFL_SAFE_DELETE(m_pokeModelSys);
        m_Seq++;
      }
    }
    else{
      m_Seq++;
    }
    RemoveAll();
    break;
  case 3:
    {
      if(!mbSkyboxNoDel){
        mpAppParam->mpSkybox->DestroyAll();
        mpAppParam->mpSkybox->DeleteHeap();
      }
      GFL_ASSERT(BillDelete());
      GFL_ASSERT(BeansBottleDelete());
      GFL_ASSERT(SignBoardDelete());
      GFL_ASSERT(BeansDelete());
      GFL_ASSERT(KinomiDelete());
      GFL_ASSERT(GroundDelete());
      GFL_ASSERT(BeansTreeDelete());
      GFL_ASSERT(BeansBoxDelete());
      GFL_ASSERT(TreasureBoxDelete());
      EndInputExculusiveKeyMode();

      PokeShadowDelete();
      
      m_EnvAccessor.Terminate();

      this->destroyPokeLight();

      m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, MODEL_LAYER_POKE);  // ポケモン用レイヤー
      m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, MODEL_LAYER_BG);
      m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
      m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pDrawEnvNode,MODEL_LAYER_BG);
      m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pDrawEnvNode);


      GFL_SAFE_DELETE( mpTempPokeCorePara );
      GFL_SAFE_DELETE(m_drawEnvNode);
      GFL_SAFE_DELETE(m_camera);
      GFL_SAFE_DELETE(m_pDrawEnvNode);
      GFL_SAFE_DELETE(m_pDrawEnvMotionResourceRootNode);
      GFL_SAFE_DELETE(m_pDrawEnvResourceRootNode);
      {
        // ポケモンアイコン3Dのヒープ
        for(int i=0; i<BILL_DATA_NUM; ++i)
        {
          for(int j=0; j<2; ++j)
          {
            if( mpPokeIcon3dTexHeap[i][j] )
            {
              gfl2::heap::Manager::DeleteHeap(mpPokeIcon3dTexHeap[i][j]);
              mpPokeIcon3dTexHeap[i][j] = NULL;
            }
          }
        }
        if( mpPokeIcon3dTexParentHeap )
        {
          gfl2::heap::Manager::DeleteHeap(mpPokeIcon3dTexParentHeap);
          mpPokeIcon3dTexParentHeap = NULL;
        }
      }
      GFL_SAFE_DELETE(  m_modelAllocator  );


      m_Seq++;
    }
    break;
  case 4:
    {
      //ClosePokemonCapture();  ResortDataManagerに置き換えたのでコメントアウト。
    }
		return true;
  }
  return false;
}



//一応補足なのですが、バトルでポケモン単体にカメラがフォーカスする場合、
//ポケモンの身長に応じたスケールをかけてカメラを表示しております。
//バトルのモデルなどにポケモンのサイズをかける
f32 ResortIslandDraw::GetCamAdjustHeight(void)
{
  f32 height = m_pokeModel->GetAdjustHeight();
  f32 diff = height - 1.0f;
  return 1.0f + (diff/2);
}



f32 ResortIslandDraw::GetAdjustHeight(void)
{
  return m_pokeModel->GetAdjustHeight();
}







//-----------------------------------------------------------------------------
/*
 *  @brief        ワールド座標系の点をスクリーン座標系の点に変換する
 *
 *  @param[out]    screen_point_x        スクリーン座標系の点の位置座標(左上が(0.0f,0.0f)、右下が(1.0f,1.0f))
 *  @param[out]    screen_point_y
 *  @param[in]     world_point           ワールド座標系の点の位置座標
 *  @param[in]     render_target         レンダーターゲット
 */
//-----------------------------------------------------------------------------
void ResortIslandDraw::ConvertWorldPointToScreenPoint(
  f32*                          screen_point_x,
  f32*                          screen_point_y,
  const gfl2::math::Vector3&        world_point
  ) const
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx = m_camera->GetProjectionMatrix();
  gfl2::math::Matrix34 view_mtx = m_camera->GetViewMatrix();

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;


  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  *screen_point_x = screen_point_2.x / 2.0f + 0.5f;
  *screen_point_y = screen_point_2.y / 2.0f + 0.5f;
}



//-----------------------------------------------------------------------------
/*
 *  @brief        ワールド座標系の点を下画面スクリーン座標系の点に変換する
 *
 *  @param[out]    lower_screen_point_x  下画面スクリーン座標系の点の位置座標(左上が(0,0)、右下が(320-1,240-1))
 *  @param[out]    lower_screen_point_y  画面範囲外も表せるようにsignedにしました。
 *  @param[in]     world_point           ワールド座標系の点の位置座標
 *  @param[in]     lower_render_target   レンダーターゲット
 */
//-----------------------------------------------------------------------------
void ResortIslandDraw::ConvertWorldPointToLowerScreenPoint(
  s32*                          lower_screen_point_x,
  s32*                          lower_screen_point_y,
  const gfl2::math::Vector3&        world_point
  ) const
{
  f32 screen_point_x;
  f32 screen_point_y;

  ConvertWorldPointToScreenPoint(
    &screen_point_x,
    &screen_point_y,
    world_point
    );

  *lower_screen_point_x = static_cast<s32>( screen_point_x * (static_cast<f32>(320-1)) );
  *lower_screen_point_y = static_cast<s32>( screen_point_y * (static_cast<f32>(240-1)) );
}





bool ResortIslandDraw::IsBeansPickuping(void)
{
  for(int i=0;i<BEANS_DATA_NUM;i++){
    if( m_pBeansModel[i].pModel && (m_pBeansModel[i].anim == BEANS_ANIM_PICKUP)){
      return true;
    }
  }
  return false;
}



Savedata::ResortSave::PokeBeansRank ResortIslandDraw::ModelTouchHitCheck( u16 x, u16 y)
{

  for(int i=0;i<BEANS_DATA_NUM;i++){
    if( m_pBeansModel[i].pModel && (m_pBeansModel[i].anim == BEANS_ANIM_GROUND)){
      const gfl2::math::Vector3 c_v3 = m_pBeansModel[i].pModel->GetPosition();
      s32 screenX;
      s32 screenY;

      ConvertWorldPointToLowerScreenPoint(&screenX, &screenY, c_v3);

      if( ((screenX-10) < x) && ((screenX+10) > x)){
        if( ((screenY-10) < y) && ((screenY+10) > y)){
          GFL_PRINT("まめHIT %d %d %d %d \n",x,y ,screenX,screenY);

          Savedata::ResortSave::PokeBeansRank rank = mpAppParam->pSave->GetGroundBeansRank( i );

          if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS ) ){
            // kawa_resort_modify
            // チュートリアルでも正規のポケマメタッチと同じ工程を行う
            GFL_ASSERT(m_pBeansModel[i].anim==BEANS_ANIM_GROUND);
            m_pBeansModel[i].anim=BEANS_ANIM_PICKUP;
            m_pBeansModel[i].pModel->ChangeAnimationByResourceNode( m_pBeansAnimResourceNode,3 );
            m_pBeansModel[i].pModel->SetAnimationLoop( false, 3);
            mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_BEANS, i);
            mpAppParam->pSave->SetGroundBeans(i, Savedata::ResortSave::POKEBEANS_NONE);  //けす
            
            mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_BEANS, i);
          }
          else{
            GFL_ASSERT(m_pBeansModel[i].anim==BEANS_ANIM_GROUND);
            m_pBeansModel[i].anim=BEANS_ANIM_PICKUP;
            m_pBeansModel[i].pModel->ChangeAnimationByResourceNode( m_pBeansAnimResourceNode,3 );
            m_pBeansModel[i].pModel->SetAnimationLoop( false, 3);
            mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_BEANS, i);
            mpAppParam->pSave->SetGroundBeans(i, Savedata::ResortSave::POKEBEANS_NONE);  //けす
          }

          return rank;
        }
      }
    }
  }
  return Savedata::ResortSave::POKEBEANS_RANK_NONE;
}






bool ResortIslandDraw::PokeModelTouchHitCheck( u16 x, u16 y)
{

  f32 distmax = 100000.0f;
  f32 dist = 0.0f;
  bool bHit=false;
  int nearindex = 0;

  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ))
    ){
    //管理人画面起動まで反応なし
    return false;
  }



  
  for(int i=0;i<BILL_DATA_NUM;i++){
    gfl2::math::Vector3 pos = m_camera->GetPosition();
    gfl2::math::Vector3 c_v3;

    if( GetPokemonPos(i,&c_v3) ){

      s32 screenX;
      s32 screenY;
      c_v3.y += BILLBOARD_COMP;
      ConvertWorldPointToLowerScreenPoint(&screenX, &screenY, c_v3);
      if( ((screenX - 15) < x) && ((screenX + 15) > x)){
        if( ((screenY - 30) < y) && ((screenY + 5) > y)){
          bHit = true;
          c_v3 =  pos - c_v3;
          dist = c_v3.Length();
          GFL_PRINT("ぽけ %f %f cam %f -- %d  %f\n", c_v3.x, c_v3.z, c_v3.Length(), i, dist);

          if(dist < distmax){
            distmax = dist;
            nearindex = i;
          }
        }
      }
    }
  }
  if(bHit){
    m_TouchPokeIndex = nearindex;
    mpAppParam->selectPokemonNo = nearindex;

    pml::pokepara::PokemonParam * pp = GFL_NEW(mpAppHeap->GetDeviceHeap()->GetLowerHandle()) pml::pokepara::PokemonParam(mpAppHeap->GetDeviceHeap()->GetLowerHandle());
    GetPokemon(pp, m_TouchPokeIndex);   ///checkok

#if PM_DEBUG
    debugPokemonSelecetMonsNo = pp->GetMonsNo();
    debugPokemonSelecetFormNo = pp->GetFormNo();
#endif
    
    if(mpAppParam->pPokeParty->GetMemberCount()>0){
      mpAppParam->pPokeParty->RemoveMember(0);
    }
    mpAppParam->pPokeParty->ReplaceMember(0, *pp);
    mpAppParam->pokemonSelectIndex = m_TouchPokeIndex;

    GFL_DELETE(pp);

    return true;
  }
  return false;
}






enum{
  BILLBOARD_MODE,
  POKE_READING,
  POKE_CAMERA_INIT,
  POKE_MODE,
  POKE_MODE1,
  POKE_MODE2,
  POKE_MODE3,
  POKE_MODE4,
  HAPPY,
  HAPPY2,
  BILL_READING,
  BILL_READING2,
};


int ResortIslandDraw::UpdateTouch(void)
{
  gfl2::ui::TouchPanel * pTouchPanel;            //!< タッチパネル
  int control=CONTROL_NONE;
  int retcode=0;

  if(!mpAppParam->inputEnable){
    return control;
  }

  if( mIsStartCameraMoveEnd == false )
  {
    return control;
  }
  if(!mpResortTouchToFlowListener->IsButtonIdleState()){
    OHNO_PRINT("アニメ中アクセス不可\n");
    return control;
  }
  
  
  pTouchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  if(pTouchPanel->IsTouchTrigger()){
    control = kanbanCheck(pTouchPanel->GetX(), pTouchPanel->GetY());

    if(control != CONTROL_NONE ){
    }
    else if( GroundBeansHitCheck(pTouchPanel) ){
    }
    else if(nutsPosCheck(pTouchPanel->GetX(), pTouchPanel->GetY(), &retcode )){
      if(retcode==1){
        control = CONTROL_TOUCH_FARM_HARVEST;
      }
    }
    else if(BeansIvyCheck(pTouchPanel->GetX(), pTouchPanel->GetY())){
      mpResortTouchToFlowListener->ResortInputEnabled(false);
      if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE )){
        control = CONTROL_TOUCH_TREE_FIRST;
      }
      else{
        control = CONTROL_TOUCH_TREE;
      }
      Sound::PlaySE( SEQ_SE_SYS_NJ_056 );
    }
    else if(PokeModelTouchHitCheck(pTouchPanel->GetX(), pTouchPanel->GetY())){
      mpResortTouchToFlowListener->ResortInputEnabled(false);
      control=CONTROL_TOUCH_POKEMON;
      mpAppParam->seqReq = NEXTSEQ_START;//初期化 //GFNMCat4176
    }
  }
  else if(pTouchPanel->IsTouchRepeat()){
    if( GroundBeansHitCheck(pTouchPanel) ){
    }
  }
  return control;
  
}


void ResortIslandDraw::PokemonTouchAnimEnd(void)
{

  gfl2::math::Vector4 start_col(0.0f,0.0f,0.0f,0.0f);
  gfl2::math::Vector4 end_col(0.0f,0.0f,0.0f,0.0f);
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS,&start_col,&end_col , gfl2::Fade::FADE_DEFAULT_SYNC);

  m_ModeSeq = POKE_READING;  //ポケモンへちかづく
  
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_POKEMON, m_TouchPokeIndex);
}



bool ResortIslandDraw::GroundBeansHitCheck( gfl2::ui::TouchPanel * tp )
{
  switch( ModelTouchHitCheck(tp->GetX(),tp->GetY()) )
  {
  case Savedata::ResortSave::POKEBEANS_RANK_NONE:
    return false;

  case Savedata::ResortSave::POKEBEANS_RANK_NORMAL:
    Sound::PlaySE( SEQ_SE_ISLAND_POKEMAME );
    break;
  case Savedata::ResortSave::POKEBEANS_RANK_RARE:
    Sound::PlaySE( SEQ_SE_SYS_NJ_045 );
    break;
  case Savedata::ResortSave::POKEBEANS_RANK_S_RARE:
    Sound::PlaySE( SEQ_SE_SYS_NJ_046 );
    break;
  }

  return true;
}


bool ResortIslandDraw::IsPokemonMoveSeq(void)
{
  if(BILLBOARD_MODE==m_ModeSeq){
    return false;
  }
  return true;
}



void ResortIslandDraw::UpdatePokemonMove(int debugno)
{
  
  m_pokeModelSys->Update();

  switch(m_ModeSeq){
  case BILLBOARD_MODE:
    break;
  case POKE_READING:
    if(!asyncCreatePokeModel()){
      break;
    }

    m_ModeSeq = POKE_CAMERA_INIT;
    break;
  case POKE_CAMERA_INIT:
    {
      BillPause();//描画したくないものを書く
      BeansPause();
      GroundPause();
      BeansBoxPause();
      BeansTreePause();
      TreasureBoxPause();
      KinomiPause();
      SignBoardPause();
      KinomiPause();
      //BillboardVisible(false);
      PokemonRotateInit();  //ポケモンの移動
      PokeNearCamera(debugno);   //ポケモンにあわせたカメラの移動
    }
    m_ModeSeq = POKE_MODE;
    break;
  case POKE_MODE:
    PokemonRotate();
    PokeNearCamera(debugno);
    mCountTimer=0;
    m_ModeSeq=POKE_MODE1;
    break;
  case POKE_MODE1:
    mCountTimer++;
    PokemonRotate();
//    PokeNearCamera(debugno);
    if(mCountTimer==2){
      m_ModeSeq=POKE_MODE2;
    }
    break;
  case POKE_MODE2:
    PokemonRotate();
 //   PokeNearCamera(debugno);
    m_ModeSeq=POKE_MODE3;
    break;
  case POKE_MODE3:
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS, _POKEMON_NEAR_FRAME);
    //タッチ有効にします
    mpResortTouchToFlowListener->ResortInputEnabled(true);
//    PokeNearCamera(debugno);
    PokemonRotate();
    m_ModeSeq=POKE_MODE4;
    break;
  case POKE_MODE4:
   if(PokemonRotate()){
      m_ModeSeq = HAPPY2;
    }
    if(m_WalkEnd){
      OHNO_PRINT("mpAppParam->seqReq %d\n",mpAppParam->seqReq);
      switch(mpAppParam->seqReq){
      case NEXTSEQ_STATUS:
      case NEXTSEQ_TAMAGO:
      case NEXTSEQ_ZUKAN:
        break;
      default:
        {
          gfl2::math::Vector4 start_col(0.0f,0.0f,0.0f,0.0f);
          gfl2::math::Vector4 end_col(0.0f,0.0f,0.0f,0.0f);
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS,&start_col,&end_col , gfl2::Fade::FADE_DEFAULT_SYNC);
        }
        break;
      }
      m_ModeSeq = HAPPY2;
      m_WalkEnd=false;
    }
    break;
  case HAPPY2:
    m_pokeModel->SetVisible(false);
    m_ModeSeq = BILL_READING;
    InitCamera();
    break;
  case BILL_READING:
    {
      BillReAdd();
      BeansReAdd();
      GroundReAdd();
      BeansBoxReAdd();
      TreasureBoxReAdd();
      BeansTreeReAdd();
      SignBoardReAdd();
      KinomiReAdd();
      m_ModeSeq = BILL_READING2;
    }
    break;
  case BILL_READING2:
    if(DeletePokemonASync()){
      if(mpAppParam->firstOpenPos == Savedata::ResortSave::RESORT_KIND_HOTSPA){
        if(m_TouchSimple.isEgg == false){  //NMCat1805
          mpAppParam->pSave->ResultBoxHotSpa( mpAppHeap->GetDeviceHeap()->GetLowerHandle()  , m_TouchPokeIndex/6);
        }
      }
      switch(mpAppParam->seqReq){
      case NEXTSEQ_STATUS:
      case NEXTSEQ_TAMAGO:
      case NEXTSEQ_ZUKAN:
        break;
      default:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS, _POKEMON_NEAR_FRAME);
        break;
      }
      m_ModeSeq = BILLBOARD_MODE;
    }
    break;
  }
}


void ResortIslandDraw::SetPokemonWalkEnd(void)
{
  m_WalkEnd=true;

}



bool ResortIslandDraw::IsPokemonCameraMode(void)
{
  if( !GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_LOWER ) ){
    return true;  //フェードが終わるまでカメラモード
  }
  if(m_ModeSeq != BILLBOARD_MODE){
    return true;
  }
  return false;
}


bool ResortIslandDraw::IsPokemonBillboardMode(void)
{
  if(m_ModeSeq != BILLBOARD_MODE){
    return true;
  }
  return false;
}





#if PM_DEBUG
int ResortIslandDraw::ControlFunc(bool bDebugCameraMode,bool bkeye)
#else
int ResortIslandDraw::ControlFunc(bool bkeye)
#endif
{
  int control=CONTROL_NONE;

#if PM_DEBUG
  mIsDebugCameraMode = bDebugCameraMode;
#endif  // PM_DEBUG

#if PM_DEBUG
  if(bDebugCameraMode){
    if(m_pokeModel){
      debugkey_nearpoke();
    }
    else{
      debugkey();
    }
  }
#endif // PM_DEBUG

  
  if(mbKeyEnable){
    if(bkeye){
      if(!IsPokemonMoveSeq()){
        control = UpdateTouch();
      }
    }
  }



  if(BILLBOARD_MODE==m_ModeSeq){
    return control;
  }
  return CONTROL_DISABLE;

}



void ResortIslandDraw::UpdateFuncBefore(void)
{
  if(!IsPokemonMoveSeq()){
    BillboardMove();
  }
}

//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
void ResortIslandDraw::UpdateFunc(int debugno)
{
  // 今の時間取得
  GameSys::AlolaTime date;
  date.Initialize();

  s32 hour    = date.GetHour();
  s32 minute  = date.GetMinute();
  s32 second  = date.GetSecond();

  /*
  //kawa_test
  static s32 s_hourCount = 0;
  static s32 s_minuteCount = 0;
  hour = s_hourCount%24;
  minute = s_minuteCount%60;
  second = 0;
  ++s_minuteCount;
  if(s_minuteCount == 60)
  {
    ++s_hourCount;
    s_minuteCount = 0;
  }
  */

  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);

  m_EnvAccessor.Update();

  // @fix GFNMCat[3065] ポケリゾートの島全てに、そのとき主人公が立っているフィールドのフォグが当たっています。 ←フォグのUpdateFogがなかったのが原因。
  // フォグの更新
  if(m_pDrawEnvNode && m_camera)
  {
    // 他は変更していないので
    //   ・Zバッファ
    //   ・Zバッファなのでカメラのニアクリップ距離、ファークリップ距離は使わない
    //   ・深度バッファの範囲はデフォルトから変更しない
    // となっている。

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = m_camera->GetProjectionMatrix();
    m_pDrawEnvNode->UpdateFog(param);
  }

  this->updatePokeLight();  // ポケモンモデルにライトを反映する

  this->updateSeaOneDayAnim( hour, minute, second );  // 海の色のアニメ(テクスチャUVアニメ)


  
  SimpleParamUpdate();

  UpdatePokemonMove(debugno);
  
  if(!IsPokemonMoveSeq()){
    _fukidashiUpdateCheck();
    BillPokemonChange();
    BeansUpdate();
    BeansTreeUpdate();
    BeansBoxUpdate();
    TreasureBoxUpdate();
    SignBoardUpdate();
    KinomiUpdate();
//    BillboardMove();
    PutBeansBottle();
    SetBillboardRotation();
  }
  if(m_appRenderingManager){
    m_appRenderingManager->Update();
  }

  //---地面に設置する為の処理

  for(int i=0;i<BEANS_DATA_NUM;i++){
    if(m_pBeansModel[i].pModel){
      if(m_pBeansModel[i].pModel->IsCreated()){

        if(m_pBeansModel[i].anim==BEANS_ANIM_PICKUP || m_pBeansModel[i].anim==BEANS_ANIM_FALL){
          m_pBeansModel[i].pModel->UpdateAnimation();
        }
        if(m_pBeansModel[i].pModel->IsAnimationLastFrame(3) && m_pBeansModel[i].anim==BEANS_ANIM_FALL ){
          m_pBeansModel[i].anim=BEANS_ANIM_GROUND;//おちた
        }
        if(m_pBeansModel[i].pModel->IsAnimationLastFrame(3) && m_pBeansModel[i].anim==BEANS_ANIM_PICKUP ){
          m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pBeansModel[i].pModel,MODEL_LAYER_BG);
          m_pBeansModel[i].anim=BEANS_ANIM_NONE;//ひろいおわった
        }
      }
    }
  }

  BeansBottleUpdate();

  
  {
    if(m_pokeModel){
      if(m_pokeModel->IsCreated()){

      }
    }
  }
  //----
  if(m_pBaseModel){
    if(m_pBaseModel->IsCreated()){
      m_pBaseModel->SetAnimationStartFrame( (hour * 20) + (minute/3)  , 1);
      m_pBaseModel->SetAnimationEndFrame( (hour * 20) + (minute/3) , 1 );
      m_pBaseModel->UpdateAnimation();
    }
  }
  if(m_pPartsModel){
    if(m_pPartsModel->IsCreated()){
      m_pPartsModel->UpdateAnimation();
    }
  }

  
  if(m_pokeModel){
    if(m_pokeModel->IsCreated()){
      //    m_pokeModel->UpdateAnimation();
    }
  }


  {
    gfl2::math::Vector3 pos(0,200,0);

    mpAppParam->mpSkybox->Update( pos, hour, minute, second);
  }

}


enum{
  STATE_STAND,
  STATE_MOVE1,
  STATE_MOVE2,
  STATE_MOVE3,
};





void ResortIslandDraw::BillboardMove(void)
{
  if(m_pBillboardModel){

    m_pBillboardModel->UpdateAnimation( );
    if(mpLogic->IsAdventureReturned()){
      TreasurePokemonNormalAnim();
      mpLogic->SetAdventureReturned(false);
    }

  }
  for(int x = 0 ;x < BILL_DATA_NUM; x++){
    if(mpFukidashiModels[x]){
      mpFukidashiModels[x]->UpdateAnimation();
    }
  }
}







//------------------------------------------------------------------
/**
 * @brief	  デバッグ操作は直読み
 */
//------------------------------------------------------------------
#if PM_DEBUG
void ResortIslandDraw::debugkey(void)
{

  const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice* crossKey = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);
  const gfl2::ui::VectorDevice* stick = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetStick(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 upv(0,1,0);

  /*
　→ひとまず、カメラの回転、ズーム、注視点変更、画角調整、がほしいです。
　　要素がそろえば、カメラをだいたい決められると思います。
　　以下のような操作でいかがでしょう。


	・アナログスティック		上下：上下回転   				左右：左右回転

  ・十字キー			上下：ズームイン／アウト
					左右：画角の変更


　　＜Ｙ押しながら＞
	・十字キー　			上下：注視点上下移動  gfl2::math::Vector3 m_TargetPos;
					左右：注視点左右移動
	・Ａボタン			カメラを初期値に戻す


　　＜R押しながら＞
     xy  画角
     up down Near
     left right Far

   */

  if(button->IsTrigger(gfl2::ui::BUTTON_A)){
    pos = m_camera->GetPosition();
    GFL_PRINT("x = %f y = %f z = %f rotate = %f dist  = %f\n", pos.x, pos.y, pos.z, island_camera_rotate, m_IslandDist);
    GFL_PRINT("targetx =  %f targety = %f  targetz =%f  fovy = %f\n",m_TargetPos.x, m_TargetPos.y, m_TargetPos.z, m_camera->GetFovy());
    GFL_PRINT("near =  %f far = %f\n", m_camera->GetNear(), m_camera->GetFar());

  }


  {
    pos = m_camera->GetPosition();
    {
      f32 fx = stick->GetX();
      if(fx != 0.0f){
        island_camera_rotate += fx;
        pos.x = gfl2::math::SinDeg( island_camera_rotate ) * m_IslandDist;
        pos.z = gfl2::math::CosDeg( island_camera_rotate ) * m_IslandDist;
        m_camera->SetupCameraLookAt( pos ,  m_TargetPos,  upv);
      }
    }
  }


  if(button->IsHold(gfl2::ui::BUTTON_R)){


    if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
    {
      float fo = m_camera->GetNear();
      fo += 0.001f;
      m_camera->SetNear( fo );
    }
    if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      float fo = m_camera->GetNear();
      fo -= 0.001f;
      m_camera->SetNear( fo );
    }

    if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
    {
      float fo = m_camera->GetFar();
      fo += 0.001f;
      m_camera->SetFar( fo );
    }
    if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
    {
      float fo = m_camera->GetFar();
      fo -= 0.001f;
      m_camera->SetFar( fo );
    }
    return;
  }


#if ENVNODE_OFF_OPEN_

  if(!button->IsHold(gfl2::ui::BUTTON_Y)){


    pos = m_camera->GetPosition();
    {
      f32 fz = stick->GetY();
      if(fz != 0.0f){
        pos.y += fz*10.0f;
        m_camera->SetPosition(pos);
        return;
      }
    }


    pos = m_camera->GetPosition();
    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
      {
        m_IslandDist -= 10.0f;
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
      {
        m_IslandDist += 10.0f;
      }

      pos.x = gfl2::math::SinDeg( island_camera_rotate ) * m_IslandDist;
      pos.z = gfl2::math::CosDeg( island_camera_rotate ) * m_IslandDist;
      m_camera->SetupCameraLookAt( pos ,  m_TargetPos,  upv);
    }

    f32 fovy = m_camera->GetFovy();
    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
      {
        fovy -= 0.02f;
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
      {
        fovy += 0.02f;
      }
      m_camera->SetFovy(fovy);
    }
  }


  else
  {

    /*
　　＜Ｙ押しながら＞
	・十字キー　
					上下左右：注視点左右移動
    アナログ   注視点上下移動
     */

    if(button->IsTrigger(gfl2::ui::BUTTON_A)){
      InitCamera();
    }

    pos = m_camera->GetPosition();
    {
      f32 fy = stick->GetY();
      if(fy != 0.0f){
        m_TargetPos.y += fy;
        m_camera->SetupCameraLookAt( pos ,  m_TargetPos,  upv);
      }
    }



    pos = m_camera->GetPosition();
    {

      if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
      {
        m_TargetPos.x += 10.0f;
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
      {
        m_TargetPos.x -= 10.0f;
      }

      m_camera->SetupCameraLookAt( pos ,  m_TargetPos,  upv);
    }


    pos = m_camera->GetPosition();
    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
      {
        m_TargetPos.z += 10.0f;
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
      {
        m_TargetPos.z -= 10.0f;
      }
      m_camera->SetupCameraLookAt( pos ,  m_TargetPos,  upv);
    }

  }

#endif


}





//------------------------------------------------------------------
/**
 * @brief	  デバッグ操作は直読み
 */
//------------------------------------------------------------------
void ResortIslandDraw::debugkey_nearpoke(void)
{

  const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice* crossKey = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);
  const gfl2::ui::VectorDevice* stick = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetStick(gfl2::ui::DeviceManager::BUTTON_STANDARD);



  
  /*



   Rトリガー 速度変更 1 or 10
   L リセット
   Aぼたん結果出力

  ・十字キー			  上下：ズームイン／アウト
				            左右：注視点Y移動
   ＜Ｙ押しながら＞
	・十字キー　			上下：注視点Z移動
           					左右：注視点X移動
   ＜X押しながら＞
    左右   ポケモン回転

   */

  if(button->IsTrigger(gfl2::ui::BUTTON_A)){

 //  PokeStanderdCamera( &mDebugAddPos, mDebugZoom);

    gfl2::math::Vector3 pos;
    pos = m_camera->GetPosition();
    GFL_PRINT("x = %f y = %f z = %f \n", pos.x, pos.y, pos.z );
    GFL_PRINT("targetx =  %f targety = %f  targetz =%f  fovy = %f\n",m_TargetPos.x, m_TargetPos.y, m_TargetPos.z, m_camera->GetFovy());
    GFL_PRINT("near =  %f far = %f\n", m_camera->GetNear(), m_camera->GetFar());

    
    GFL_PRINT("簡易版\n");
    GFL_PRINT("%f,%f,%f,%d,%d\n", mDebugAddPos.y, mDebugZoom, mDebugPokeRotate, debugPokemonSelecetMonsNo, debugPokemonSelecetFormNo );
    GFL_PRINT("島ごと版\n");
    GFL_PRINT("%f,%f,%f,%f,%f,%d,%d\n", mDebugAddPos.x,mDebugAddPos.y,mDebugAddPos.z,
              mDebugZoom, mDebugPokeRotate, debugPokemonSelecetMonsNo, debugPokemonSelecetFormNo );

    //_pokemonSetRotate
    
  }

  if(button->IsHold(gfl2::ui::BUTTON_X)){

    if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) ){
      mDebugPokeRotate = mDebugPokeRotate + mdebugmulnum/2;
      _pokemonSetRotate( mDebugPokeRotate );
    }
    else if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) ){
      mDebugPokeRotate = mDebugPokeRotate - mdebugmulnum/2;
      _pokemonSetRotate( mDebugPokeRotate );
    }
  }
  else if(!button->IsHold(gfl2::ui::BUTTON_Y)){
  
    if(button->IsTrigger(gfl2::ui::BUTTON_L)){
      mDebugAddPos.x=0.0f;
      mDebugAddPos.y=0.0f;
      mDebugAddPos.z=0.0f;
      mDebugZoom=0.0f;
      mDebugPokeRotate=0.0f;
      PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
    }

    if(button->IsTrigger(gfl2::ui::BUTTON_R)){
      if(mdebugmulnum!=10.0f){
        mdebugmulnum = 10.0f;
        GFL_PRINT("１０倍\n");
      }
      else{
        mdebugmulnum = 1.0f;
        GFL_PRINT("等倍\n");
      }
    }
    

    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
      {
        mDebugZoom -= mdebugmulnum;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
      {
        mDebugZoom += mdebugmulnum;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
    }

    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
      {
        mDebugAddPos.SetY( mDebugAddPos.GetY()+mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
      {
        mDebugAddPos.SetY( mDebugAddPos.GetY()-mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
    }
  }
  else
  {
/*
   ＜Ｙ押しながら＞
	・十字キー　			上下：注視点Z移動
           					左右：注視点X移動
    RL   ポケモン回転
*/



    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
      {
        mDebugAddPos.SetZ( mDebugAddPos.GetZ()+mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
      {
        mDebugAddPos.SetZ( mDebugAddPos.GetZ()-mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
    }
    {
      if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
      {
        mDebugAddPos.SetX( mDebugAddPos.GetX()+mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
      else if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
      {
        mDebugAddPos.SetX( mDebugAddPos.GetX()-mdebugmulnum ) ;
        PokeStanderdCamera( &mDebugAddPos, mDebugZoom);
      }
    }
  }



}


#endif  // PM_DEBUG


//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

/*
使っていないのでコメントアウト
void ResortIslandDraw::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}
*/

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_CLOSE_END,
};

#if 0
bool ResortIslandDraw::fileReadSequece(void)
{

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  switch(mFileSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
      pAsyncFileMan->AddArcFileOpenReq(m_pFrs[0]->openr);
    }
    mFileSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( m_pFrs[0]->openr.arcId )){
      for(int k = 0 ; k < READ_DATA_NUM ; k++){
        pAsyncFileMan->AddArcFileLoadDataReq(m_pFrs[k]->loadr);
      }
      mFileSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    {
      int x=0;
      for(x =0 ;x < READ_DATA_NUM;x++){
        if(pAsyncFileMan->IsArcFileLoadDataFinished( m_pFrs[x]->loadr.ppBuf ) ){
        }
        else{
          break;
        }
      }
      if(x!=READ_DATA_NUM){
        break;
      }
      pAsyncFileMan->AddArcFileCloseReq(m_pFrs[0]->closer);
      mFileSeq = LOCAL_CLOSE_WAIT;
    }
    //break;
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished( m_pFrs[0]->openr.arcId ) ){
      mFileSeq = LOCAL_CLOSE_END;
      return true;
    }
    break;
  }
  return false;
}
#endif
//二番目

void ResortIslandDraw::SetParam(App::Resort::APP_PARAM* pAppParam, app::util::AppRenderingManager* pApR,void** pResource, app::util::Heap* pHeap,
                                gfl2::heap::HeapBase* pPokeModelHeap, System::nijiAllocator* pPokeModelNijiAllocator,
                                System::nijiAllocator* pniji, System::nijiAllocator* /*使っていないのでコメントアウトplocal*/)
{
  mpAppParam = pAppParam;
  m_appRenderingManager = pApR;
  mpResourceBuffer = pResource;
  mpAppHeap = pHeap;
  mpPokeModelHeap = pPokeModelHeap;
  mpPokeModelNijiAllocator = pPokeModelNijiAllocator;
  m_pNijiAllocator = pniji;
  //使っていないのでコメントアウトm_pLocalDeviceHeapAllocator = plocal;
  mIsStartCameraMoveEnd = !mpAppParam->isStart;
}


// ------------------------------------
// 高さコリジョンとの判定
void ResortIslandDraw::UpdateCollision_Height(  gfl2::math::Vector3* pVec )
{
  if(m_pCollisionModel==NULL){
    return;
  }

  // 当たり判定に使用する位置を取得

  gfl2::math::Vector4 pos(pVec->x, pVec->y, pVec->z, 0.0f);

  // 高さ判定コールバッククラスを作成
  SimpleCollisionCallbackable_Height callBack;

  // コリジョンモデルのGetHeightで高さを取得。コールバッククラスを渡します。
  // この中でメッシュの全ポリゴンのXZ平面に位置が含まれているかを判定し、当たっていれば最大10件まで衝突情報を溜めます。
  m_pCollisionModel->GetHeight(
    pos,
    NULL,
    &callBack
    );

  // 一度でも当たったか判定
  if( callBack.IsHit())
  {
    // 指定高さに一番近い&&指定位置よりも上にある高さを返します
    f32 newHeight = callBack.GetNearestHeight( pos.y );
    // 取得した高さに更新します
    pVec->y = newHeight;
  }
}



#if 1

bool ResortIslandDraw::asyncCreatePokeModel(void)
{
  bool isCreated = false;

  switch(m_asyncCreateDestroyPokeModelStep)
  {
  case 0:
    {
      m_asyncCreateDestroyPokeModelStep = 1;

      
    }
    break;
  case 1:
    if(GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_LOWER))
    {
      m_asyncCreateDestroyPokeModelStep = 2;
    }
    break;
  case 2:
    {
      {
        PokeTool::PokeModel::SetupOption opt;
        opt.dataHeap = mpPokeModelHeap;
        {
          gfl2::heap::HeapBase * heap     = ResortDataManager::GetStaticInstance()->GetUncompWorkHeap();  // 島の生成が終わっているならこのヒープは空いているので、ワークヒープとして使える。
          opt.workHeap = heap->GetLowerHandle(); 
        }
        opt.animeType = PokeTool::MODEL_ANIMETYPE_APP;
        {
          pml::pokepara::PokemonParam * pp = mpAppParam->pPokeParty->GetMemberPointer(0);
          
          // kawa_resort_modify
          m_TouchSimple.monsNo  = pp->GetMonsNo();
          m_TouchSimple.formNo  = pp->GetFormNo();
          m_TouchSimple.sex     = pp->GetSex();
          m_TouchSimple.isRare  = pp->IsRare();
          m_TouchSimple.perRand = pp->GetPersonalRnd();
          m_TouchSimple.isEgg   = pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG);

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
          ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_LOAD_START);
#endif
          m_pokeModel = m_pokeModelSys->CreateModelSimple(&m_TouchSimple,opt);

          if(m_TouchSimple.isEgg == false)
          {
            Sound::PlayVoice( 0, m_TouchSimple.monsNo, m_TouchSimple.formNo );
          }

        }

        m_asyncCreateDestroyPokeModelStep = 3;
        //breakしない
      }
    }
  case 3:
    {

      if(m_pokeModel->IsFinishLoad())
      {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_LOAD_END);
#endif

        m_pokeModel->Create();
        m_pokeModelExist = true;

        {
          // ライトセット番号を変更しておく
          m_pokeModel->SetLightSetNo(POKE_LIGHT_SET_NO);
          int matNum = m_pokeModel->GetMaterialCount();
          for( int i=0;i<matNum;i++ )
          {
            m_pokeModel->SetMaterialLightSetIndex(i, POKE_LIGHT_SET_NO);
          }
        }
        
        this->updatePokeLight();  // ポケモンモデルにライトを反映する

				{  //影のサイズ
          int size = 1;//  Ｍのみ
          
          m_ModelPokeShadow.pModel = GFL_NEW(mpAppHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
          m_ModelPokeShadow.pModel->Create(m_pNijiAllocator,mpAppHeap->GetDeviceHeap(), m_pPokeShadowModelResourceNode[ size ]);
          m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_ModelPokeShadow.pModel, MODEL_LAYER_BG);
				}

        {
          mPokemonWaistIndex = m_pokeModel->GetJointIndex("Waist");

        }
        
        
        m_asyncCreateDestroyPokeModelStep = 4;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 4:
  default:
    {
      m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pokeModel, MODEL_LAYER_POKE);

      m_asyncCreateDestroyPokeModelStep = 0;
      isCreated = true;

      
      
    }
    break;
  }

  return isCreated;
}



bool ResortIslandDraw::DeletePokemonASync(void)
{
  switch(m_PokeDeleteStep){
  case 0:
    {
      if(m_pokeModel){
        m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY,m_pokeModel, MODEL_LAYER_POKE);
      }
      if(m_ModelPokeShadow.pModel){
        m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_ModelPokeShadow.pModel,MODEL_LAYER_BG);
      }

    }
    m_PokeDeleteStep=1;
    break;
  case 1:
    {
      if(m_ModelPokeShadow.pModel){
        if(!m_ModelPokeShadow.pModel->CanDestroy()){
          break;
        }
        GFL_SAFE_DELETE(m_ModelPokeShadow.pModel);
      }

      if(m_pokeModel){
        m_pokeModelExist = false;
        m_pokeModelSys->DeleteModel(m_pokeModel);
        m_pokeModel = NULL;
      }
    }
    m_PokeDeleteStep=0;
    return true;
  }
  return false;
}


#endif




//=================================================================================================
// コリジョンライブラリ、高さ判定コールバッククラス
//=================================================================================================

// コンストラクタ
SimpleCollisionCallbackable_Height::SimpleCollisionCallbackable_Height() :
m_hitCount( 0 )
{
  for( u32 i = 0; i < HIT_COUNT_MAX; i++)
  {
    m_hitDataTable[i].height = 0.0f;
    m_hitDataTable[i].pTriangle = NULL;
  }
}

// デストラクタ
SimpleCollisionCallbackable_Height::~SimpleCollisionCallbackable_Height()
{
}

// 当たり判定コールバック関数 交点取得
bool SimpleCollisionCallbackable_Height::CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
{
  // 交点を記憶
  m_hitDataTable[m_hitCount].height = height;
  m_hitDataTable[m_hitCount].pTriangle = pTriangle;
  m_hitCount++;

  if( m_hitCount >= HIT_COUNT_MAX)
  {
    // 衝突検出終了
    return false;
  }
  else
  {
    // 衝突検出続行
    return true;
  }
}

// 指定の高さに一番近い高さを取得する
f32 SimpleCollisionCallbackable_Height::GetNearestHeight(f32 height) const
{
  // 一番近い衝突メッシュを探す
  f32 resultDiff = FLT_MAX;
  s32 hitIndex = -1;
  for( u32 i = 0; i < m_hitCount; i++)
  {
    {
      f32 diff = gfl2::math::Abs( m_hitDataTable[i].height - height);
      if( diff < resultDiff)
      {
        resultDiff = diff;
        hitIndex = i;
      }
    }
  }

  if( hitIndex != -1)
  {
    return m_hitDataTable[hitIndex].height;
  }
  return height;
}

// 当たったか
bool SimpleCollisionCallbackable_Height::IsHit() const
{
  return ( m_hitCount != 0 );
}

// 再取得可能な状態にする
void SimpleCollisionCallbackable_Height::Clear( void)
{
  m_hitCount = 0;
}


//--------------------------------------------------------------------------
/**
 * @brief   リゾートの変更
 * @param   dir   移動方向(0: 左、1: 右)
 */
//--------------------------------------------------------------------------
void ResortIslandDraw::FeedResort( u8 dir )
{

  // 切り替えスタート


}



//タッチのデータテーブルの取得
farm_touch_island_binstr* ResortIslandDraw::getTouchCheckTable(void)
{
  farm_touch_island_binstr* pTouchCheck = NULL;

	switch( mpAppParam->firstOpenPos )
  {
	case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
		pTouchCheck = (farm_touch_island_binstr*)mpAdventureTouch;
		break;
	case Savedata::ResortSave::RESORT_KIND_HOTSPA:
		pTouchCheck = (farm_touch_island_binstr*)mpHotSpaTouch;
		break;
	case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
		pTouchCheck = (farm_touch_island_binstr*)mpFriendShipTouch;
		break;
	case Savedata::ResortSave::RESORT_KIND_GIM:
		pTouchCheck = (farm_touch_island_binstr*)mpGimTouch;
		break;
	default:
	case Savedata::ResortSave::RESORT_KIND_FARM:
		pTouchCheck = (farm_touch_island_binstr*)mpFarmTouch;
		break;
	}
  return pTouchCheck;
}


//このデータを解析するかどうか
bool ResortIslandDraw::getSkipCheck(farm_touch_island_binstr* pTouchCheck, int lv, int index)
{

  switch(pTouchCheck[index].c3){
  case 0:  //levelall
    break;
  case 1:   //level2 3
    if(lv<2){
      return false;
    }
    break;
  case 2:  //lv3
			if(lv<3){
      return false;
			}
			break;
		case 3:  //lv1
			if(lv!=1){
      return false;
			}
			break;
		case 4:  //lv2
			if(lv!=2){
      return false;
			}
			break;
		case 5:  //lv3
			if(lv!=3){
      return false;
			}
			break;
		}
  return true;
}


bool ResortIslandDraw::GetBottleCheckPos(gfl2::math::Vector3* pPos)
{
	int lv = mpAppParam->pSave->GetResortLevel( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos );
	farm_touch_island_binstr* pTouchCheck = getTouchCheckTable();
	gfl2::math::Vector3 kanban;

	for(int i=0;i<100;i++){
		if(pTouchCheck[i].c3==6){
			break;
		}
		if(!getSkipCheck(pTouchCheck,lv,i))
		{
			continue;
		}
		switch(pTouchCheck[i].c4){
		case 8:  //ボトル
			pPos->x = pTouchCheck[i].c0;
			pPos->y = pTouchCheck[i].c1;
			pPos->z = pTouchCheck[i].c2;
			return true;
		}
	}
	return false;
}




bool ResortIslandDraw::GetTreasureCheckPos(gfl2::math::Vector3* pPos)
{
	int lv = mpAppParam->pSave->GetResortLevel( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos );
	farm_touch_island_binstr* pTouchCheck = getTouchCheckTable();
	gfl2::math::Vector3 kanban;

	for(int i=0;i<100;i++){
		if(pTouchCheck[i].c3==6){
			break;
		}
		if(!getSkipCheck(pTouchCheck,lv,i))
		{
			continue;
		}

		switch(pTouchCheck[i].c4){
		case 6:  //たからばこ
			pPos->x = pTouchCheck[i].c0;
			pPos->y = pTouchCheck[i].c1;
			pPos->z = pTouchCheck[i].c2;
			return true;
		}
	}
	return false;
}



void ResortIslandDraw::GetPokeBoxPos(gfl2::math::Vector3* pPos)
{
  int lv = mpAppParam->pSave->GetResortLevel( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos );
  farm_touch_island_binstr* pTouchCheck = getTouchCheckTable();
  gfl2::math::Vector3 kanban;

	for(int i=0;i<100;i++){
    if(pTouchCheck[i].c3==6){
      break;
    }
    if(!getSkipCheck(pTouchCheck,lv,i))
    {
      continue;
    }

    switch(pTouchCheck[i].c4){
    case 0:  //box
      pPos->x = pTouchCheck[i].c0;
      pPos->y = pTouchCheck[i].c1;
      pPos->z = pTouchCheck[i].c2;
      return;
    }
  }
}


bool ResortIslandDraw::GetSignBoardPos(gfl2::math::Vector3* pPos, int index)
{
  int lv = mpAppParam->pSave->GetResortLevel( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos );
  farm_touch_island_binstr* pTouchCheck = getTouchCheckTable();
  gfl2::math::Vector3 kanban;

	for(int i=0;i<100;i++){
    if(pTouchCheck[i].c3==6){
      break;
    }
    if(!getSkipCheck(pTouchCheck,lv,i))
    {
      continue;
    }
    pPos->x = pTouchCheck[i].c0;
    pPos->y = pTouchCheck[i].c1;
    pPos->z = pTouchCheck[i].c2;

    switch(pTouchCheck[i].c4){
    case 1:  //看板は４種類
    case 2:  //
      if(index==0){
        return true;
      }
      break;
    case 3:  //
      if(index==1){
        return true;
      }
      break;
    case 4:  //
      if(index==2){
        return true;
      }
      break;
    }
  }
  return false;
}





///看板類の汎用検査

int ResortIslandDraw::kanbanCheck(float x,float y)
{
  int lv = mpAppParam->pSave->GetResortLevel( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos );
	farm_touch_island_binstr* pTouchCheck = getTouchCheckTable();
  gfl2::math::Vector3 kanban;

  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ))||
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS ))||
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ))
    ){
    //起動時は看板類反応なし
    return CONTROL_NONE;
  }

  for(int i=0;i<100;i++){
    if(pTouchCheck[i].c3==6){
      break;
    }
    if(!getSkipCheck(pTouchCheck,lv, i))
    {
      continue;
    }
  	kanban.x = pTouchCheck[i].c0;
    kanban.y = pTouchCheck[i].c1;
    kanban.z = pTouchCheck[i].c2;

    if( touchCheck(x,y,&kanban))
    {
/*
			TOUCH_BEANS_BOX,
				TOUCH_BOARD,
				TOUCH_KINOMI_FARM,
				TOUCH_ADVENTURE,
				TOUCH_FARM_IMPUT_LIST,
				TOUCH_GIM,
				TOUCH_POKEMON_SELECT_GIM,
				TOUCH_POKEMON_SELECT_HOTSPA,
				TOUCH_POKEMON,   //ポケモンをタッチ
				TOUCH_BEANS,     //落ちてるまめをタッチ
				TOUCH_BEANS_BOTTLE,  //まめびんをたっち


  "BOX"=>0, # 
  "BOARD"=>1, # 
  "BOARD1"=>2, # 
  "BOARD2"=>3, #  
  "BOARD3"=>4, #  
  "DRINK"=>5, #  
  "TREASURE_CHECK"=>6, #  
  "SET"=>7, #
  "BOTTLE"=>8, 
  "NULL"=>9,

 */
      OHNO_PRINT("Touch kanban %d -  %f %f %f -種類 %d\n", i,kanban.x, kanban.y, kanban.z, pTouchCheck[i].c4);


      switch(pTouchCheck[i].c4){
			case 0:  //box
        BeansBoxTouchAnim();
        mpResortTouchToFlowListener->ResortInputEnabled(false);
        Sound::PlaySE( SEQ_SE_DECIDE1 );
        return CONTROL_TOUCH_BOX;
        
			case 1:  //board
				switch( mpAppParam->firstOpenPos )
				{
				case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
          SignBoardTouchAnim(CONTROL_BOARD1);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_ADV_KANBAN;
				case Savedata::ResortSave::RESORT_KIND_GIM:
          SignBoardTouchAnim(CONTROL_BOARD1);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_GIM;
				}
				break;
			case 2:  //board1
        mpAppParam->selectBoxGroup = CONTROL_BOARD1;
				switch( mpAppParam->firstOpenPos )
				{
				case Savedata::ResortSave::RESORT_KIND_HOTSPA:
          SignBoardTouchAnim(CONTROL_BOARD1);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_HOTSPA_KANBAN;
				case Savedata::ResortSave::RESORT_KIND_GIM:
          SignBoardTouchAnim(CONTROL_BOARD1);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_GIM_KANBAN;
				case Savedata::ResortSave::RESORT_KIND_FARM:
          SignBoardTouchAnim(CONTROL_BOARD1);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_FARM_KANBAN;
				}
				break;
			case 3:  //board2
        mpAppParam->selectBoxGroup = CONTROL_BOARD2;
				switch( mpAppParam->firstOpenPos )
				{
				case Savedata::ResortSave::RESORT_KIND_HOTSPA:
          SignBoardTouchAnim(CONTROL_BOARD2);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_HOTSPA_KANBAN2;
				case Savedata::ResortSave::RESORT_KIND_GIM:
          SignBoardTouchAnim(CONTROL_BOARD2);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_GIM_KANBAN2;
				case Savedata::ResortSave::RESORT_KIND_FARM:
          SignBoardTouchAnim(CONTROL_BOARD2);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_FARM_KANBAN2;
				}
				break;
			case 4:  //board3
        mpAppParam->selectBoxGroup = CONTROL_BOARD3;
				switch( mpAppParam->firstOpenPos )
				{
				case Savedata::ResortSave::RESORT_KIND_HOTSPA:
          SignBoardTouchAnim(CONTROL_BOARD3);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_HOTSPA_KANBAN3;
				case Savedata::ResortSave::RESORT_KIND_GIM:
          SignBoardTouchAnim(CONTROL_BOARD3);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_GIM_KANBAN3;
				case Savedata::ResortSave::RESORT_KIND_FARM:
          SignBoardTouchAnim(CONTROL_BOARD3);
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          return CONTROL_TOUCH_FARM_KANBAN3;
				}
				break;
			case 5:  //drink
//				mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_GIM,0);
//        Sound::PlaySE( SEQ_SE_DECIDE1 );
        GFL_ASSERT(0);
        return CONTROL_NONE;
			case 6:   //treasure_check
        TreasureBoxTouchAnim();
        mpResortTouchToFlowListener->ResortInputEnabled(false);
        Sound::PlaySE( SEQ_SE_DECIDE1 );
        return CONTROL_TOUCH_ADV_CHECK;
			case 7:   //set
//				mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_ADVENTURE,0);
 //       Sound::PlaySE( SEQ_SE_DECIDE1 );
        GFL_ASSERT(0);
        return CONTROL_NONE;

      case 8:
        if(BeansBottleTouchAnim())
        {
          Sound::PlaySE( SEQ_SE_ISLAND_POKEMAME );  // まめびん取得SE(まめSEと同じ)
          mpResortTouchToFlowListener->OnTouchToFlowXY(ResortTouchToFlowListener::TOUCH_BEANS_BOTTLE, 0, x,y);
          return CONTROL_TOUCH_BEANS_BOTTLE;
        }
        
			}
    }
  }
  return CONTROL_NONE;
}


void ResortIslandDraw::BeansBottleTouchEnd(void)
{
  SetKeyEnable(true);

}

void ResortIslandDraw::GimTouchEnd(int boardno)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_GIM,boardno);
}

void ResortIslandDraw::GimSignBoardTouchEnd(int boardno)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_GIM,boardno);
}

void ResortIslandDraw::AdventureSignBoardTouchEnd(int boardno)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_ADVENTURE,boardno);
}

void ResortIslandDraw::HotSpaSignBoardTouchEnd(int boardno)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_HOTSPA,boardno);
}

void ResortIslandDraw::FarmSignBoardTouchEnd(int boardno)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_FARM_IMPUT_LIST,boardno);
}

void ResortIslandDraw::AdventureTreasureBoxTouchEnd(void)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_ADVENTURE,1);
}


void ResortIslandDraw::BeansBoxTouchAnimEnd(void)
{
  if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX ) ){
    mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_BOX, 0);
  }
  else if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU )){
    mpResortTouchToFlowListener->ResortInputEnabled(true);
    SetKeyEnable(true);
  }
  else{
    mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_BEANS_BOX,0);
  }
  mBeansBoxAnimNo = -1;  //終了時に確認する用
}



bool ResortIslandDraw::touchCheck(float x,float y, gfl2::math::Vector3* pos)
{

  {
    s32 screenX;
    s32 screenY;

    ConvertWorldPointToLowerScreenPoint(&screenX, &screenY, *pos);
    GFL_PRINT("けんさ %d %d -> %f %f\n", screenX, screenY, x ,y);


    if( ((screenX-15) < x) && ((screenX+15) > x)){ 
      if( ((screenY-30) < y) && ((screenY+5) > y)){
        return true;
      }
    }
  }
  return false;
}




bool ResortIslandDraw::nutsPosCheck(float x,float y, int* retcode)
{
	
  gfl2::math::Vector3 pos;
  *retcode = 0;

  if(Savedata::ResortSave::RESORT_KIND_FARM != mpAppParam->firstOpenPos){
    return false;
  }

  for(int i = 0;i< Savedata::ResortSave::RESORT_FARM_MAX;i++){
    pos.x = mpKinomiTreeBin[i].c0;
    pos.y = mpKinomiTreeBin[i].c1;
    pos.z = mpKinomiTreeBin[i].c2;

    //if(mpAppParam->pSave->GetFarmNum(i))
    {
      if( touchCheck(x,y,&pos)){
        GFL_PRINT("nuts HIT \n");

        if(mpAppParam->pSave->GetFarmNum(i)!=0){
          mpResortTouchToFlowListener->OnTouchToFlowXY(ResortTouchToFlowListener::TOUCH_FARM_PLANT_PLACE,i, x, y  );  //収穫の時のみ
          *retcode = 1;//収穫の時のみ

        }
        else if(mpAppParam->pSave->GetFarmItemKind(i)!=ITEM_DUMMY_DATA){
          mpResortTouchToFlowListener->ResortInputEnabled(false);
          Sound::PlaySE( SEQ_SE_DECIDE1 );
          mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_KINOMI_FARM, i); //畑
        }
        return true;
      }
    }
  }
  return false;
}





//つたからまめをふらすかどうか
bool ResortIslandDraw::BeansIvyCheck(float x,float y)
{
  gfl2::math::Vector3 pos;
  int xn;

  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP != mpAppParam->firstOpenPos){
    return false;
  }

  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ))||
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS ))
    ){
    return false;
  }
  if(IsBeansPickuping()){  //GFNMCat3115  拾っている時には揺らせない
    return false;
  }

  

  for(int i = 0;i< 9;i++){
    pos.x = 0.0f;
    pos.y = 30.0f+50.0f*i;
    pos.z = 0.0f;
    if( touchCheck(x, y, &pos)){
      GFL_PRINT("豆ふり HIT %d\n",i);
      BeansTreeAnim();
      return true;
    }
  }
  return false;



}




//つたからまめをふらす
bool ResortIslandDraw::BeansTreePut(u8* buff)
{
  gfl2::math::Vector3 pos;
  int xn;
  GFL_ASSERT(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP == mpAppParam->firstOpenPos);

  for( xn = 0 ; xn < Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX; xn++){
    if(buff[xn] == 1){
      GFL_PRINT("豆おきます %d\n",xn);
      buff[xn] = 0;
      GFL_ASSERT(m_pBeansModel[xn].anim==BEANS_ANIM_NONE);
      m_pBeansModel[xn].anim = BEANS_ANIM_START;
      Sound::PlaySE( SEQ_SE_SYS_NJ_055 );
      return true;
    }
  }
  return false;

}

void ResortIslandDraw::BeansTreePutEnd(void)
{
  mpResortTouchToFlowListener->ResortInputEnabled(true);
  if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ) ){
    mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_TREE, 0);
  }
}



//
bool ResortIslandDraw::PokeIndividualSimpleRotate(void)
{
  bool rete = false;
	int i;

  if(m_TouchSimple.isEgg){
    return false;
  }
  for(i=0;i<_POKE_CAMERA_MAX;i++){
    if(mpPokeCameraSimple[i].c4 == 0){//終端
      break;
    }
    if(m_TouchSimple.monsNo != mpPokeCameraSimple[i].c4){
      continue;
    }
    if(m_TouchSimple.formNo != mpPokeCameraSimple[i].c5){
      continue;
    }
    _pokemonSetRotate(mpPokeCameraSimple[ i ].c3);
    OHNO_PRINT("PokeIndividualSimpleRotate\n");
    return true;
  }
  return false;
}


bool ResortIslandDraw::PokeIndividualIslandRotate(void)
{
  bool rete = false;
	int i;
  
  if(m_TouchSimple.isEgg){
    return false;
  }
  for(i=0;i<_POKE_CAMERA_MAX;i++){
    if(mpPokeCameraIsland[i].c6 == 0){//終端
      break;
    }
    if( mpAppParam->firstOpenPos != mpPokeCameraIsland[i].c8 ){
      continue;
    }
    if( m_TouchSimple.monsNo != mpPokeCameraIsland[i].c6 ){
      continue;
    }
    if( m_TouchSimple.formNo != mpPokeCameraIsland[i].c7 ){
      continue;
    }
    _pokemonSetRotate(mpPokeCameraIsland[ i ].c5);
    OHNO_PRINT("PokeIndividualIslandRotate\n");
    return true;
  }
  return false;
}



void ResortIslandDraw::_pokemonSetRotate(float rotate)
{
  if(m_pokeModel==NULL){
    return;
  }
  
  const PokeTool::PokeSettingData* pokeSetData = m_pokeModel->GetSettingData();
  gfl2::math::Vector3 rotv3 = m_pokeModel->GetRotation();

//  OHNO_PRINT("ワイのdef値 %f\n",rotv3.y);

  rotv3.y = GFL_MATH_DEG_TO_RAD(rotate + pokeSetData->camRotate);
  m_pokeModel->SetRotation(rotv3);
}



void ResortIslandDraw::PokemonRotateInit(void)
{
  mPokeCoseIndex=0;
  gfl2::math::Vector3 c_v3;
  
  c_v3.x = mpStandPosBin[ mpAppParam->firstOpenPos ].c0;
  c_v3.y = mpStandPosBin[ mpAppParam->firstOpenPos ].c1;
  c_v3.z = mpStandPosBin[ mpAppParam->firstOpenPos ].c2;

//  OHNO_PRINT("動かない場合たち位置 %f %f %f\n",c_v3.x,c_v3.y,c_v3.z);


#if PM_DEBUG
  mDebugPokeRotate = 0.0f;
#endif
  
  if(PokeIndividualSimpleRotate()){
  }
  else if(PokeIndividualIslandRotate()){
  }
  else{
    _pokemonSetRotate( 0.0f );
  }

  m_pokeModel->SetPosition(c_v3);

  m_pokeModel->ChangeAnimation(PokeTool::MODEL_ANIME_FI_WAIT_A);

  m_pokeModel->SetAnimationIsLoop(true);
  mPokeCoseCount = 60;
  mPokeCoseIndex = 0;
 // OHNO_PRINT("方向変更 %d\n",mPokeCoseIndex);

  m_PokeTargetPos.x = c_v3.x;
  m_PokeTargetPos.y = c_v3.y;
  m_PokeTargetPos.z = c_v3.z;

  mbPokemonRotateChange = false;
  
  PokemonRotate();
}





bool ResortIslandDraw::PokemonRotateChange(void)
{
  {

    float zz = mpStandPosBin[ mpAppParam->firstOpenPos ].c2 - m_PokeTargetPos.z; //   float zz = mpHotSpaBin[mPokeCoseIndex + 1].c2 - m_PokeTargetPos.z;
    float xx = mpStandPosBin[ mpAppParam->firstOpenPos ].c0 - m_PokeTargetPos.x;  //  float xx = mpHotSpaBin[mPokeCoseIndex + 1].c0 - m_PokeTargetPos.x;

    gfl2::math::Vector2 v1(xx , zz);  //ベクトル
    gfl2::math::Vector2 v2(0, 1);  //ベクトル
    v1.NormalizeRet();
    v2.NormalizeRet();
    float nai = v1.Dot(v2);
    float kakudo = acosf(nai);

    if(xx < 0.0f){
      m_pokeModel->SetRotation( 0.0f , -kakudo,  0.0f);
    }
    else{
      m_pokeModel->SetRotation( 0.0f , kakudo,  0.0f);
    }



  }

  
  return true;
}


//無限ループ移動
bool ResortIslandDraw::PokemonRotate(void)
{
  PokeMove();

  return false;
}







void ResortIslandDraw::PokeMove(void)
{

  gfl2::math::Vector3 spe(0.0f,0.0f,0.0f);
  gfl2::math::Vector3 vec;

  {
    gfl2::math::Vector3 houkou(0.0f,0.0f,0.1f);

    gfl2::math::Matrix44 mat44 = gfl2::math::Matrix44::GetRotationY( m_pokeModel->GetRotation().GetY() );

    vec = mat44 * spe;
  }

  m_PokeTargetPos.x += vec.x;
  m_PokeTargetPos.y = spe.y;
  m_PokeTargetPos.z += vec.z;


  gfl2::math::Vector3 v3 = m_PokeTargetPos;
  v3.y += 60.0f;

  {
    const gfl2::math::Vector3 c_v3 = m_PokeTargetPos;
    gfl2::math::Vector3 ve3(c_v3);
    UpdateCollision_Height( &ve3 );
    //  v3.y = ve3.y + 60.0f;
    //コリジョンを計算した後、アニメの移動を足す
    m_PokeTargetPos.y = ve3.y + spe.y;
    
    m_pokeModel->SetPosition(m_PokeTargetPos);
  


    if(m_ModelPokeShadow.pModel){
      if(m_ModelPokeShadow.pModel->IsCreated()){
        gfl2::math::Matrix34 mtx;
        gfl2::math::Vector3 pos;
        mtx = m_pokeModel->GetJointWorldMatrix(mPokemonWaistIndex);
        pos = mtx.GetElemPosition();
        
        UpdateCollision_Height( &pos );

        pos.y += 2.0f; // 地面から浮かせる値
        m_ModelPokeShadow.pModel->SetPosition(pos);

        // 角度はモデルの角度に合わせる
        m_ModelPokeShadow.pModel->SetRotation( m_pokeModel->GetRotation() );

        m_ModelPokeShadow.pModel->UpdateAnimation();
      }
    }
  }

  
}


void ResortIslandDraw::TextureResourceNodeGetter::Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  if(m_textureResourceNode == NULL)
  {
    if( dagNode->IsKindOf<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>() )
    {
      m_textureResourceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    }
  }
}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* ResortIslandDraw::TextureResourceNodeGetter::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>  visitor;
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>( dagNode, &visitor, 0xFFFFFFFF );
  return visitor.getTextureResourceNode();
}


ResortIslandDraw::TextureResourceNodeGetter::TextureResourceNodeGetter(void)
   : m_textureResourceNode(NULL)
{
}

ResortIslandDraw::TextureResourceNodeGetter::~TextureResourceNodeGetter()
{
}

gfl2::renderingengine::scenegraph::resource::TextureResourceNode* ResortIslandDraw::TextureResourceNodeGetter::getTextureResourceNode(void)
{
  return m_textureResourceNode;
}



void ResortIslandDraw::SetBeansAnim(int x, float fx, float fz,bool banim)
{

  m_pBeansModel[x].pModel->SetPosition( fx , 0, fz );
  m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBeansModel[x].pModel, MODEL_LAYER_BG);

  m_pBeansModel[x].pModel->SetAnimationLoop( false, 0);

  m_pBeansModel[x].pModel->ChangeAnimationByResourceNode( m_pBeansTypeAnimResourceNode, 1);  //アニメ開始
  m_pBeansModel[x].pModel->SetAnimationStartFrame( mpAppParam->pSave->GetGroundBeans(x) , 1);
  m_pBeansModel[x].pModel->SetAnimationEndFrame( mpAppParam->pSave->GetGroundBeans(x) , 1 );

  OHNO_PRINT("まめをおく  %d %d %d\n",mpAppParam->pSave->GetGroundBeans(x) , x, banim);

  {
    const gfl2::math::Vector3 c_v3 = m_pBeansModel[x].pModel->GetPosition();
    gfl2::math::Vector3 v3(c_v3);
    UpdateCollision_Height( &v3 );
    m_pBeansModel[x].pModel->SetPosition(v3);
  }
  
  GFL_ASSERT(m_pBeansModel[x].anim == BEANS_ANIM_START);
  if(banim){
          
    m_pBeansModel[x].pModel->ChangeAnimationByResourceNode( m_pBeansInAnimResourceNode, 3);  //アニメ開始
    m_pBeansModel[x].pModel->SetAnimationLoop( false, 3);
    m_pBeansModel[x].anim = BEANS_ANIM_FALL;
  }
  else{
    m_pBeansModel[x].anim = BEANS_ANIM_GROUND;
  }

}



/*
   豆を更新する おく
 */
void ResortIslandDraw::PutBeans(bool banim)
{
  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP != mpAppParam->firstOpenPos){
    return;
  }

  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ))
    )
  {
    //チュートリアルは指定した場所にいっこおく

    if(m_pBeansModel[0].anim == BEANS_ANIM_NONE){  //表示していない
      SetBeansAnim(0, 150.0f,180.0f, false);
		}
    return;
  }

  for(int x = 0;x < BEANS_DATA_NUM;x++){
    if(m_pBeansModel[x].pModel==NULL){
      continue;
    }
//    if( mpAppParam->pSave->GetGroundBeans(x) != Savedata::ResortSave::POKEBEANS_NONE){ //まめがある
      if(m_pBeansModel[x].anim == BEANS_ANIM_START){  //表示していない  スタートした
        SetBeansAnim(x, mpBeansPos[x].c0, mpBeansPos[x].c2, banim);
  //    }
    }
  }

}

void ResortIslandDraw::BeansUpdate(void)
{
  if(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP != mpAppParam->firstOpenPos){
    return;
  }

  for(int x = 0;x < BEANS_DATA_NUM;x++){
    if(m_pBeansModel[x].pModel){
      if(m_pBeansModel[x].anim != BEANS_ANIM_NONE){  //表示している
        if(IsPokemonBillboardMode()){
          m_pBeansModel[x].pModel->SetVisible(false);
        }
        else{
          m_pBeansModel[x].pModel->SetVisible(true);
          m_pBeansModel[x].pModel->UpdateAnimation();
        }
      }
    }
  }
  if(IsPokemonBillboardMode()){
    return;
  }
  PutBeans(true);

}


void ResortIslandDraw::PokeShadowCreate(void* pSData,void* pMData,void* pLData,void* pHData,void* pWData)
{

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
  modelDataPack.SetModelData(static_cast<c8*>(pSData));
  m_pPokeShadowModelResourceNode[0] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

  modelDataPack.SetModelData(static_cast<c8*>(pMData));
  m_pPokeShadowModelResourceNode[1] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

  modelDataPack.SetModelData(static_cast<c8*>(pLData));
  m_pPokeShadowModelResourceNode[2] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

  modelDataPack.SetModelData(static_cast<c8*>(pHData));
  m_pPokeShadowModelResourceNode[3] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

  modelDataPack.SetModelData(static_cast<c8*>(pWData));
  m_pPokeShadowModelResourceNode[4] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&modelDataPack);

  
}


void ResortIslandDraw::PokeShadowDelete(void)
{
  GFL_SAFE_DELETE(m_pPokeShadowModelResourceNode[0]);
  GFL_SAFE_DELETE(m_pPokeShadowModelResourceNode[1]);
  GFL_SAFE_DELETE(m_pPokeShadowModelResourceNode[2]);
  GFL_SAFE_DELETE(m_pPokeShadowModelResourceNode[3]);
  GFL_SAFE_DELETE(m_pPokeShadowModelResourceNode[4]);
}


void ResortIslandDraw::CreateAll(void)
{
  SimpleParamCreate();
  GroundCreate();
  BillCreate();
  BeansCreate();
  SignBoardCreate();
  BeansTreeCreate();
  BeansBoxCreate();
  TreasureBoxCreate();
  BeansBottleCreate();
  KinomiCreate();
//  InitCamera();
  SetStartCamera();
  InitSetMoveTable();
}



void ResortIslandDraw::RemoveAll(void)
{
  GroundRemove();
  BeansBottleRemove();
  SignBoardRemove();
  BeansRemove();
  KinomiRemove();
  BillRemove();
  BeansTreeRemove();
  BeansBoxRemove();
  TreasureBoxRemove();

}



bool ResortIslandDraw::DeleteAll(void)
{
  if(GroundDelete()){
    if(BeansDelete()){
      if(BeansTreeDelete()){
        if(SignBoardDelete()){
          if(BeansBoxDelete()){
            if(TreasureBoxDelete()){
              if(BeansBottleDelete()){
                if(KinomiDelete()){
                  if(BillDelete()){
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}




void ResortIslandDraw::StartModelAnim(int type)
{
  switch(type){
	case App::Resort::ResortChangeIslandListener::ISLAND_MODELANIM_BEANS:
    BeansAnim();
    break;
	case ResortChangeIslandListener::ISLAND_MODELANIM_TREE:
    BeansTreeAnim();
    break;
	case ResortChangeIslandListener::ISLAND_MODELANIM_BEANSBOX:
    BeansBoxTouchAnim();
    break;
  }



}



void ResortIslandDraw::createPokeLight(void)
{
  // 下画面ポケモンモデル用のライト
  m_pPokeLight = app::util::AppRenderingManager::CreateDefaultDrawEnv(mpAppHeap, app::util::AppRenderingManager::PresetType::DEFAULT, POKE_LIGHT_SET_NO);
  app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame( m_pPokeLight->GetLightSet(POKE_LIGHT_SET_NO)->GetLightNode(0), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );
  m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pPokeLight, MODEL_LAYER_POKE);
 
  // リソースからポケモン用のライトの番号を得る
  if(m_pDrawEnvNode)
  {
    bool lightExist = false;
    s32 lightNum = m_pDrawEnvNode->GetLightSet(RESOURCE_POKE_LIGHT_SET_NO)->GetLightNum();
    for(s32 i=0; i<lightNum; ++i)
    {
      const gfl2::renderingengine::scenegraph::instance::LightNode* lightNode = m_pDrawEnvNode->GetLightSet(RESOURCE_POKE_LIGHT_SET_NO)->GetLightNode(i);
      const c8* nodeName = lightNode->GetNodeName();
      if( strncmp(nodeName, RESOURCE_POKE_LIGHT_NAME, strlen(RESOURCE_POKE_LIGHT_NAME)) == 0 )
      {
        m_resourcePokeLightNo = 0;
        lightExist = true;
        break;
      }
    }
    GFL_ASSERT(lightExist);  // 開発中に気付かせるためのASSERT。RESOURCE_POKE_LIGHT_NAMEという名前のライトがない。
  }
}
void ResortIslandDraw::destroyPokeLight(void)
{
  m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pPokeLight, MODEL_LAYER_POKE);
  GFL_SAFE_DELETE(m_pPokeLight);
}
void ResortIslandDraw::updatePokeLight(void)
{
  if(m_pPokeLight)
  {
    if(m_pokeModelExist)
    {
      if(m_pDrawEnvNode)
      {
        const gfl2::renderingengine::scenegraph::instance::LightNode* lightNode = m_pDrawEnvNode->GetLightSet(RESOURCE_POKE_LIGHT_SET_NO)->GetLightNode(m_resourcePokeLightNo);
        gfl2::math::Vector4 color = lightNode->GetColor();

        //m_pokeModel->SetEnabledAmbientColor(true);  // nijiディレクター要望。夜でもポケモンは明るいままにする。だからコメントアウトした。
        //m_pokeModel->SetAmbientColor(color);
      }
    }
  }
}

void ResortIslandDraw::updateSeaOneDayAnim(s32 hour, s32 minute, s32 second)
{
  if(m_pBaseModel)
  {
    // git_program/niji_project/prog/System/source/EnvAccessor.cpp
    // f32 EnvAccessor::CalculateAllTimeAnimationFrame( f32 frameMax, s32 hour, s32 minute, s32 second) const
    
    const f32 frameMax = 480.0f;
    const f32 ONE_HOUR_FRAME   = frameMax / 24.0f;
    const f32 ONE_MINUTE_FRAME = frameMax / 1440.0f;
    const f32 ONE_SECOND_FRAME = frameMax / 86400.0f;

    f32 frame = (hour * ONE_HOUR_FRAME) + (minute * ONE_MINUTE_FRAME) + (second * ONE_SECOND_FRAME);

    m_pBaseModel->SetAnimationFrame( frame, 1, poke_3d::model::BaseModel::ANIMATION_BIT_ALL );
  }
}



/*
//0.175, 0.15, 0.125, 0.1, 0.075, 0.05, 0.025, 0, -0.025, -0.05, -0.075
const f32 ResortIslandDraw::START_CAMERA_POSITION = 0.55f;
const f32 ResortIslandDraw::START_CAMERA_MOVE_VALUE = -0.175f;
const f32 ResortIslandDraw::START_CAMERA_MOVE_ADD = 0.025f;
const u32 ResortIslandDraw::START_CAMERA_MOVE_COUNT_MAX = 10;
*/
//0.07, 0.06, 0.05, 0.04, 0.03, 0.02, 0.01, 0, -0.01, -0.02, -0.03
const f32 ResortIslandDraw::START_CAMERA_POSITION = 0.22f;
const f32 ResortIslandDraw::START_CAMERA_MOVE_VALUE = -0.07f;
const f32 ResortIslandDraw::START_CAMERA_MOVE_ADD = 0.01f;
const u32 ResortIslandDraw::START_CAMERA_MOVE_COUNT_MAX = 10;


void ResortIslandDraw::SetStartCamera(void)
{
  if( mIsStartCameraMoveEnd != false )
  {
    InitCamera();
  }
  else
  {
    int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
    int datano = mpAppParam->firstOpenPos * 3 + lv;
    mStartCameraFovy = mpstrx[datano].c6 + START_CAMERA_POSITION;
    mStartCameraMoveValue = START_CAMERA_MOVE_VALUE;
    mStartCameraMoveCount = 0;
    OHNO_PRINT("-*- %d %f\n", datano, mpstrx[datano].c6);

    m_TargetPos.x = mpstrx[datano].c1;
    m_TargetPos.y = mpstrx[datano].c2;
    m_TargetPos.z = mpstrx[datano].c3;
    ISLAND_DIST_INIT = mpstrx[datano].c4;
    ISLAND_HEIGHT_INIT = mpstrx[datano].c5;

    m_IslandDist = ISLAND_DIST_INIT;
    island_camera_rotate = 0.0f;

    CalcCamera( &m_TargetPos, island_camera_rotate, m_IslandDist, ISLAND_HEIGHT_INIT, mStartCameraFovy );

    m_camera->SetNear(mpstrx[datano].c7);
    m_camera->SetFar(mpstrx[datano].c8);
  }
}

void ResortIslandDraw::StartCameraMove( bool is_fade_start )
{
  if( is_fade_start == false )
  {
    return;
  }
  if( mIsStartCameraMoveEnd != false )
  {
    return;
  }

  if( mStartCameraMoveCount == START_CAMERA_MOVE_COUNT_MAX )
  {
    int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
    int datano = mpAppParam->firstOpenPos * 3 + lv;

    mStartCameraFovy = mpstrx[datano].c6;
    mIsStartCameraMoveEnd = true;
  }
  else
  {
    mStartCameraFovy += mStartCameraMoveValue;
    mStartCameraMoveValue += START_CAMERA_MOVE_ADD;
    mStartCameraMoveCount++;
  }
  CalcCamera( &m_TargetPos, island_camera_rotate, m_IslandDist, ISLAND_HEIGHT_INIT, mStartCameraFovy );
}

bool ResortIslandDraw::IsStartCameraMoveEnd(void)
{
  return mIsStartCameraMoveEnd;
}

void ResortIslandDraw::EndCameraMove( bool is_fade_out )
{
  if( is_fade_out == false )
  {
    return;
  }
  if( mIsEndCameraMove == false )
  {
    int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
    int datano = mpAppParam->firstOpenPos * 3 + lv;

    mStartCameraFovy = mpstrx[datano].c6;
    mStartCameraMoveValue = START_CAMERA_MOVE_ADD;
    mIsEndCameraMove = true;
  }
  mStartCameraFovy += mStartCameraMoveValue;
  mStartCameraMoveValue += START_CAMERA_MOVE_ADD;
//  CalcCamera( &m_TargetPos, island_camera_rotate, m_IslandDist, ISLAND_HEIGHT_INIT, mStartCameraFovy );
  {
    gfl2::math::Vector3 pos;

    pos.y = ISLAND_HEIGHT_INIT;
    pos.x = gfl2::math::SinDeg(island_camera_rotate) * m_IslandDist + static_cast<f32>(mUpdateCameraTotal)/100.0f;
    pos.z = gfl2::math::CosDeg(island_camera_rotate) * m_IslandDist;

    SetCamera( &m_TargetPos, &pos, mStartCameraFovy );
  }
}

void ResortIslandDraw::UpdateCameraMove(void)
{
  // 演出中
  if( mIsStartCameraMoveEnd == false ||   // 開始演出中
      mIsEndCameraMove != false )         // 終了演出中
  {
    return;
  }
  // ポケモンを見ている
  if( IsPokemonMoveSeq() != false )
  {
    // @note 復帰時におかしくなるので初期化しておく
    mUpdateCameraTotal = 0;
    mUpdateCameraAdd = UPDATE_CAMERA_ADD_MAX;
    mUpdateCameraValue = UPDATE_CAMERA_VALUE;
    return;
  }
#if PM_DEBUG
  // デバッグカメラ動作中
  if( mIsDebugCameraMode != false )
  {
    mUpdateCameraTotal = 0;
    mUpdateCameraAdd = UPDATE_CAMERA_ADD_MAX;
    mUpdateCameraValue = UPDATE_CAMERA_VALUE;
    return;
  }
#endif

  mUpdateCameraAdd += mUpdateCameraValue;
  mUpdateCameraTotal += mUpdateCameraAdd;
  if( gfl2::math::Abs(mUpdateCameraAdd) >= UPDATE_CAMERA_ADD_MAX )
  {
    mUpdateCameraValue *= -1;
  }

  {
    int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
    int datano = mpAppParam->firstOpenPos * 3 + lv;
    f32 fovy = mpstrx[datano].c6;

    gfl2::math::Vector3 pos;

    pos.y = ISLAND_HEIGHT_INIT;
    pos.x = gfl2::math::SinDeg(island_camera_rotate) * m_IslandDist + static_cast<f32>(mUpdateCameraTotal)/100.0f;
    pos.z = gfl2::math::CosDeg(island_camera_rotate) * m_IslandDist;

    SetCamera( &m_TargetPos, &pos, fovy );
  }
}



void ResortIslandDraw::ButtonDisplayTypeEnabled(bool bEnabled)
{
  mpResortTouchToFlowListener->ButtonDisplayTypeEnabled(bEnabled);
}

void ResortIslandDraw::ResortInputEnabled(bool bEnabled)
{
  mpResortTouchToFlowListener->ResortInputEnabled(bEnabled);
}

void ResortIslandDraw::SetAdventureInEnd(void)
{
  mpResortTouchToFlowListener->OnTouchToFlow(ResortTouchToFlowListener::ADVENTURE_INANIM_END, 0);
}


bool ResortIslandDraw::IsAnimEndGetEffect(void)
{
  return mpResortTouchToFlowListener->IsAnimEndGetEffect();
}



//--------------------------------------------------------------------------
/**
 * @brief UIViewマネージャのボタン監視モード開始
 */
//--------------------------------------------------------------------------
void ResortIslandDraw::StartInputExculusiveKeyMode(void)
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  mBackupExclusiveKey = man->GetInputExclusiveKeyByButtonAnimation();
  man->SetInputExclusiveKeyByButtonAnimation( true );
  
}

//--------------------------------------------------------------------------
/**
 * @brief UIViewマネージャのボタン監視モード終了
 * @param flg   true = ON, false = OFF
 */
//--------------------------------------------------------------------------
void ResortIslandDraw::EndInputExculusiveKeyMode( void )
{
  app::ui::UIViewManager * man = GFL_SINGLETON_INSTANCE(app::ui::UIViewManager);
  man->SetInputExclusiveKeyByButtonAnimation( mBackupExclusiveKey );
}




GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)



