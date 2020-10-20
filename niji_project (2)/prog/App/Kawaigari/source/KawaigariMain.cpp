//======================================================================
/**
 * @file KawaigariMain.h
 * @date 2015/09/04 12:11:22
 * @author ariizumi_nobuhiko
 * @brief かわいがりメイン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <system/include/Date/gfl2_Date.h>

#include <math/include/gfl2_MathUtil.h>

#include "PokeTool/include/PokeModelSystem.h"
#include "Battle/Background/include/BgSystem.h"

#include "GameSys/include/NijiTime.h"

#include "KawaigariRenderSystem.h"
#include "KawaigariCamera.h"
#include "KawaigariMain.h"

#include <system/include/GflUse.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_debug_menu.h>              // For kawaigari debug
#include <Debug/DebugWin/include/DebugWinSystem.h>


// Fade
#include <Fade/include/gfl2_FadeManager.h>

// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

// Model, Effect, Data, Etc:
#include <arc_index/kawaigari_resource.gaix>

// Sound:
#include "Sound/include/Sound.h"

// Demo Objects:
#include "DemoLib/Object/include/ObjectUtil.h"

// Diary:
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"

// Nakayoshi Manager:
#include "PokeTool/include/NakayoshiManager.h"

// ロトム親密度
#include "AppLib/include/Tool/AppToolRotomFriendship.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kawaigari)


const int LIGHTNO_POKEMON_TOP = 4;

KawaigariMain::KawaigariMain()
:mInitSeq(0)
,mExitSeq(0)
{
  mStartFriendship = 0;
  mCreateEffectRequest = -1;
  mFinished = false;
  mMode = KAWAIGARI_MODE_NORMAL;
  mPokeMameLoadSeq = 0;
  mRenderSys = NULL;
  mModel = NULL;
  mPause = false;
  mForceLoadPikachuu = false;

  mIdx = 0;

  // Effect  
  mEffectHeap = NULL;
  mEffectSystem = NULL;
  mEffectGroupIDList[0] = mEffectGroupIDList[1] = NULL;
  mParticleIndexManager = NULL;

  for(int z=0;z<MAX_KAWAIGARI_EFFECT;z++) mEffects[z] = NULL;

  for(int z=0;z<MAX_KAWAIGARI_CARE_EFFECT;z++)
  {
    mCareEffects[z] = NULL;
    mKillCareEffect[z] = false;
    mUpdateEffectPos[z] = false;
  }

  // PokeMame:
  for(int z=0;z<MAX_POKEMAME_MODEL;z++)
  {
    mPokeMameModel[z] = NULL;
    pokeMameLoaded[z] = false;
  }

  for(int z=0;z<MAX_POKEMAME_PIECE_MODEL;z++) mPokeMamePieceModel[z] = NULL;
  mameType = 0;

  uiManager = NULL;

  mMonsterBallModel = NULL;  


  // All Managers NULL:
  mResourceManager = NULL;

  mOptionCoreParamManager = NULL;
  
  mFoodScale.Set(1.0f,1.0f,1.0f);

  // Connect Objects to Main System:
  cameraSystem.SetCoreSys(this);
  dataManager.SetCoreSys(this);
  pokemonAI.SetCoreSys(this);
  actionSelector.SetCoreSys(this);  
  mTroubleManager.SetCoreSys(this);
  mameManager.SetCoreSys(this);

#if PM_DEBUG
  debugFont.SetCoreSys(this);
  mIsDebugMode = false;
#endif

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  debugManager.SetCoreSys(this);
  mReloadPokemon = false;
  mReloadSeq = 0;
#endif
#endif

}


bool KawaigariMain::IsDebugMode(void)
{
#if PM_DEBUG
  if(mIsDebugMode)
    return(true);
#endif 

  return(false);
}

void KawaigariMain::PreloadEffectResources(void)
{
#if defined(GF_PLATFORM_WIN32)
  PreloadEffectResources_Win32();
#else
  PreloadEffectResources_CTR();
#endif
}

#if defined(GF_PLATFORM_WIN32)
void KawaigariMain::PreloadEffectResources_Win32(void)
{
  // Load Effects:  
  int arcIdx;

  arcIdx = GARC_kawaigari_resource_ek_success_kira_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_smoke_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_heart_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_yogore_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_dust_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_hair_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_water_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_hokori_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_note_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_nakayoshi_up_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_symbol03_lz_BPTCL; // Question Mark
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_success_facegame_lz_BPTCL; // For Care Tool Fadeout
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_high_five_lz_BPTCL; // High Five Effect
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_symbol01_lz_BPTCL; // EXCLAMATION Mark
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_shadow_lz_BPTCL; // Monster Ball Shadow
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_fire_lz_BPTCL; // Fire
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_ice_lz_BPTCL; // Ice
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_poison_lz_BPTCL; // Poison
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_paralysis_lz_BPTCL; // Paralysis
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_nemuri_lz_BPTCL; // Sleepy
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);        
  
  arcIdx = GARC_kawaigari_resource_ek_ball_capture_lz_BPTCL; // Ball Capture
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);        

  arcIdx = GARC_kawaigari_resource_ek_ball_in_lz_BPTCL; // Ball In
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);        

  
}
#else
void KawaigariMain::PreloadEffectResources_CTR(void)
{
  // Load Effects:  
  int arcIdx;

  arcIdx = GARC_kawaigari_resource_ek_success_kira_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_smoke_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_heart_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_note_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_nakayoshi_up_lz_BPTCL;
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_symbol03_lz_BPTCL; // Question Mark
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_success_facegame_lz_BPTCL; // For Care Tool Fadeout
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_high_five_lz_BPTCL; // High Five Effect
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_ball_capture_lz_BPTCL; // Ball Capture
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);        

  arcIdx = GARC_kawaigari_resource_ek_ball_in_lz_BPTCL; // Ball In
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);        

  arcIdx = GARC_kawaigari_resource_ek_symbol01_lz_BPTCL; // EXCLAMATION Mark
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_shadow_lz_BPTCL; // Monster Ball Shadow
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  arcIdx = GARC_kawaigari_resource_ek_nemuri_lz_BPTCL; // Sleepy
  mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);

  if(mInitParam->pokemonParam==NULL) return;

  if(mInitParam->pokemonParam->HaveSick())
  {  
    switch(mInitParam->pokemonParam->GetSick())
    {
    case pml::pokepara::SICK_DOKU: 
      arcIdx = GARC_kawaigari_resource_ek_poison_lz_BPTCL; // Poison
      mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
      break;

    case pml::pokepara::SICK_YAKEDO: 
      arcIdx = GARC_kawaigari_resource_ek_fire_lz_BPTCL; // Fire
      mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
      break;

    case pml::pokepara::SICK_KOORI:
      arcIdx = GARC_kawaigari_resource_ek_ice_lz_BPTCL; // Ice
      mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
      break;

    case pml::pokepara::SICK_MAHI:
      arcIdx = GARC_kawaigari_resource_ek_paralysis_lz_BPTCL; // Paralysis
      mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
      break;
    
    }

    return; // Can't have any other status effects!
  }
 
  switch(mInitParam->pokemonParam->GetDirtType())
  {
  case pml::pokepara::DIRT_TYPE_HAIR:
    arcIdx = GARC_kawaigari_resource_ek_hair_lz_BPTCL; // Hair
    mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
    break;

  case pml::pokepara::DIRT_TYPE_STAIN:
    arcIdx = GARC_kawaigari_resource_ek_yogore_lz_BPTCL; // Dirt
    mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
    break;

  case pml::pokepara::DIRT_TYPE_SAND:
    arcIdx = GARC_kawaigari_resource_ek_dust_lz_BPTCL; // Dust
    mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
    arcIdx = GARC_kawaigari_resource_ek_hokori_lz_BPTCL; // Dust Effect
    mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
    break;

  case pml::pokepara::DIRT_TYPE_WATER:
    arcIdx = GARC_kawaigari_resource_ek_water_lz_BPTCL; // Water
    mResourceManager->LoadRequest(mHeap,mHeap,ARCID_KAWAIGARI_RESOURCE,arcIdx,true,DemoLib::Obj::RESOURCE_EFFECT);
    break;
  }

}

#endif

int KawaigariMain::GetParameterMonsterNumber(void)
{
  if(mInitParam!=NULL && mInitParam->pokemonParam!=NULL)
  {
    return(mInitParam->pokemonParam->GetMonsNo());
  }
  return(0);
}

//初期化
bool KawaigariMain::Initialize(gfl2::heap::HeapBase *heap , KawaigariProcParam *initParam)
{
  switch( mInitSeq )
  {
  case 0:
    Savedata::IncRecord( Savedata::Record::RECID_PARLE_KIDOU );
    App::Tool::RotomFriendship::AddCharacterCounter( Savedata::FieldMenu::ROTOM_CHAR_FRIENDLY, 1 );

    mHeap = heap;
    mInitParam = initParam;

    if(mInitParam->pokemonParam!=NULL)
    {
      
      mStartFriendship = PokeTool::nakayoshi::GetFriendship(mInitParam->pokemonParam,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst());

      pokemonAI.SetParameters( mStartFriendship,mInitParam->pokemonParam->GetNadenadeValue(),mInitParam->pokemonParam->GetFeed() );

      if(mInitParam->pokemonParam->IsHpZero()) 
      {
        pokemonAI.SetIsSick(true);

        // Remove Dirt and Status Effects when sick:
        mInitParam->pokemonParam->ClearDirt();
        mInitParam->pokemonParam->RecoverSick();
      }


      SetMode(mInitParam->mode);
      
    }    

    GFL_PRINT("Kawaigari Initialize()-> Mode %i\n",Mode());

    mameManager.Init();

    pokemonAI.SetParametersByMode(); // Setup Parameters based on current mode if needed
    
    //基本的な初期化
    mGlAllocator = GFL_NEW(mHeap) System::nijiAllocator(mHeap);
    mDeviceManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
    mAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();

    mAllocator = GFL_NEW(mHeap)gfl2::heap::NwAllocator(mHeap);

    mAppHeap = GFL_NEW(mHeap)::app::util::Heap();
    mAppHeap->Create(mHeap,mHeap,mAllocator,mAllocator,NULL,NULL);

    // Effect:
    {
      GFL_PRINT("Kawaigari->Init Effect\n"); 
      mParticleIndexManager = GFL_NEW(mHeap)DemoLib::Obj::EffectIndexManager(mHeap,64);

      mEffectHeap = GFL_NEW(mHeap)gfl2::Effect::GFHeap(mHeap);

      gfl2::Effect::Config config;
      config.SetEffectHeap( mEffectHeap );
#ifdef GF_PLATFORM_CTR
      config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif //GF_PLATFORM_CTR
      config.SetEmitterNum(512);  //デフォルト64 +1で664
      //config.SetStripeNum(64);  //デフォルト32 +1で22656 だから消す
      config.SetResourceNum(mParticleIndexManager->GetIndexSize());  //現状1024.メモリによっては見直すこと
      mEffectSystem = GFL_NEW(mHeap)gfl2::Effect::System(config);

      mEffectGroupIDList[0] = GFL_NEW(mHeap) gfl2::util::List<u32>( mHeap,1);
      mEffectGroupIDList[0]->PushFront(0);
      mEffectGroupIDList[1] = GFL_NEW(mHeap) gfl2::util::List<u32>( mHeap,1);
      mEffectGroupIDList[1]->PushFront(1);      
      mEffectGroupIDList[2] = GFL_NEW(mHeap) gfl2::util::List<u32>( mHeap,1);
      mEffectGroupIDList[2]->PushFront(2);      
    }

    mResourceManager = GFL_NEW(mHeap)DemoLib::Obj::ResourceManager(mAsyncFileManager,mHeap,256);



    // Render System:
    GFL_PRINT("->Init Render System\n");
    mRenderSys = GFL_NEW(mHeap)KawaigariRenderSystem(this);
    mRenderSys->Initialize();

    // Effect Render:    
    {
      gfl2::Effect::EffectRenderPath::Config config;
      gfl2::Effect::EffectRenderPath *effRenderPath;
      config.m_pSystem        = mEffectSystem;
      config.m_cpGroupIDList  = mEffectGroupIDList[0];
      effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectRenderPath();
      effRenderPath->SetConfig(config);

      config.m_pSystem        = mEffectSystem;
      config.m_cpGroupIDList  = mEffectGroupIDList[1];
      effRenderPath = mRenderSys->GetRenderingPipeLine()->GetScreenEffectRenderPath();
      effRenderPath->SetConfig(config);

      config.m_pSystem        = mEffectSystem;
      config.m_cpGroupIDList  = mEffectGroupIDList[2];
      effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectFixedSizeRenderPath();
      effRenderPath->SetConfig(config);
    }

    // GARCS:    

    {	      
      // Monster Ball:
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= ARCID_CHARA_MODEL_BATTLE_NORMAL;
      req.heapForFile	= mHeap;
      req.heapForReq	= mHeap->GetLowerHandle();
      GetAsyncFileManager()->SyncArcFileOpen(req);
    }

    
    {	
      // Model, Camera, Animation GARC:      
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= ARCID_KAWAIGARI_RESOURCE;
      req.heapForFile	= mHeap;
      req.heapForReq	= mHeap->GetLowerHandle();
      GetAsyncFileManager()->SyncArcFileOpen(req);
    }
    
    //BG初期化
    {
      mBgSystem = GFL_NEW(mHeap)btl::BgSystem();
      btl::BgSystem::INIT_PARAM bgParam;
      bgParam.heap = mHeap;
      bgParam.fileManager = mAsyncFileManager;
      {
        GameSys::AlolaTime time;
        time.Initialize();
        mBgSystem->Initialize( bgParam,time.GetHour(),time.GetMinute(),time.GetSecond() );
      }

      mBgSystemNear = GFL_NEW(mHeap)btl::BgSystem();      
      {
        GameSys::AlolaTime time;
        time.Initialize();
        mBgSystemNear->Initialize( bgParam,time.GetHour(),time.GetMinute(),time.GetSecond() );
      }
    }
    
    //ポケモンシステム初期化
    {
      mPokeModelSys = GFL_NEW(mHeap)PokeTool::PokeModelSystem();
      mPokeModelSys->InitSystemAsync(mHeap,mHeap,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),2);
      PokeTool::PokeModelSystem::HeapOption opt;
      opt.animeType = PokeTool::MODEL_ANIMETYPE_KAWAIGARI;
      opt.useIdModel = true;
      opt.useShadow = true;
      opt.useColorShader = true;
      mPokeModelSys->CreatePokeModelHeap(mHeap,&opt);
    }

#if PM_DEBUG
    // Select menu debug functions
    InitSelectMenuDebug( GetHeap() );
#endif // PM_DEBUG

    // Care Core Param Manager:
    mOptionCoreParamManager = GFL_NEW(mHeap)PokeTool::KawaigariParamOptionCoreManager(mHeap,mHeap->GetLowerHandle(),mAsyncFileManager);
    
    mInitSeq++;
    break;

  case 1: // Load 3D Resources:
#if PM_DEBUG     
    if(!debugFont.Initialize()) return(false);    
#endif

    {
      // Monster Ball:
      int ballType = GetMonsterBallType();

      mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
        ARCID_CHARA_MODEL_BATTLE_NORMAL,
        CHARA_MODEL_ID_BTOB0201_00+ballType-1,
        false,
        DemoLib::Obj::RESOURCE_BINPACK,
        false);      

      PreloadEffectResources();

      mInitSeq++;
    }
    break;

  case 2:  
    if( mRenderSys->Setup()  ) mInitSeq++;    
    break;

  case 3:
    if( mBgSystem->IsFinishInitialize() == false || mBgSystemNear->IsFinishInitialize() == false ||
        mPokeModelSys->IsFinishInitSystemAsync() == false || !mOptionCoreParamManager->IsLoaded() ) 
    {
      return false;
    }
    mPokeModelSys->CreateSystem(mGlAllocator,true,true);
    
    
    mRenderSys->SetCamProjMtx(0.523599f,1.67f,32.0f,12000.0f);
    mInitSeq++;
    break;

  case 4:
    LoadPokemon();    
    mInitSeq++;
    break;

  case 5:
    if(mModel->IsFinishLoad()) mInitSeq++;
    break;

  case 6:
    mModel->Create();

    if(!mModel->IsAvailableIdModel())
    {
      GFL_PRINT("*** ERROR NO ID TEXTURE FOUND, LOAD PIKACHUU! ***\n");
      // Delete, and force load pikachuu:
      mPokeModelSys->DeleteModel(mModel);
      mForceLoadPikachuu = true;
      mInitSeq = 4;
      break;
    }
    else mForceLoadPikachuu = false;
    
    
    mInitSeq++;
    break;
    
  case 7: // SEQ_LOAD_BG:
    {
      btl::BgSystem::SETUP_PARAM setupParam;
      
      setupParam.groundType = initParam->bgParam.groundType;
      setupParam.nearType = initParam->bgParam.nearType;
      setupParam.farType = initParam->bgParam.farType;
      setupParam.useSkyBox = initParam->bgParam.useSkyBox;
      
      mBgSystem->StartLoad(setupParam);

      setupParam.groundType = 0;
      setupParam.farType = initParam->bgParam.farType;
      setupParam.nearType = initParam->bgParam.nearType;
      setupParam.useSkyBox = 0;

      mBgSystemNear->StartLoad(setupParam);
    }

    mInitSeq++;
    break;
    
  case 8://SEQ_WAIT_BG && Parameter Load:
    mBgSystem->LoadUpdate();
    mBgSystemNear->LoadUpdate();
    if( mBgSystem->IsFinishLoad() == false || mBgSystemNear->IsFinishLoad() == false || dataManager.Initalize(mHeap,mHeap->GetLowerHandle(),mAsyncFileManager)==false)
    {
      return false;
    }
    
    // UI
    uiManager = GFL_NEW(mHeap) KawaigariUIManager( mAppHeap, mRenderSys->GetAppRenderingManager() );
    uiManager->SetCoreSys(this);

    mInitSeq++;    
    break;
    
  case 9: // UI Init Wait:
    if(uiManager->Init() ) mInitSeq++;
    break;
  
  case 10: // PokeMame:
    if(LoadPokeMame()) mInitSeq++;
    break;

  case 11: // Resource Load:
    if(mResourceManager->IsFinishLoad()==true)
    {
      mResourceManager->CreateResource(mHeap,GetGLAllocator(),mEffectSystem,mEffectHeap,GetParticleIndexManager());
      ///mResourceManager->CreateResource(mHeap,GetGLAllocator());
      mInitSeq++;        
    }
    break;

  case 12:
    LoadMonsterBall();
    LoadEffects();
    CreatePokeMame();

    // Create Pokemon:
    
    mModel->SetupColorShader();
    mModel->SetVisible(false);

    ///mModel->SetInterestSpeed(0,5.0f);

    // Setup Light and Shadow:
    mModel->SetLightSetNo(LIGHTNO_POKEMON_TOP+mIdx);
    {
      int matNum = mModel->GetMaterialCount();
      for( int i=0;i<matNum;i++ )
      {
        mModel->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
      }
    }
    if( GetPokeModel()->GetShadowModel() )
    {
      int matNum = mModel->GetShadowModel()->GetMaterialCount();
      for( int i=0;i<matNum;i++ )
      {
        mModel->GetShadowModel()->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
      }
    }

    mRenderSys->GetRenderingPipeLine()->AddObject_Poke(mModel,LIGHTNO_POKEMON_TOP+mIdx);    
  //  mModel->SetScale(1.482908f, 1.482908f, 1.482908f );
  //  mRenderSys->SetAdjustScale(1.482908f);
    mRenderSys->SetAdjustScale(mModel->GetAdjustScale());
    

    // Set the 3D Model for the AI:
    pokemonAI.SetPokeModel(mModel);    

    if(mInitParam->pokemonParam!=NULL)
    {
      // Sick?
      if(mInitParam->pokemonParam->HaveSick() )
      {
        switch(mInitParam->pokemonParam->GetSick())
        {
        case pml::pokepara::SICK_DOKU: mTroubleManager.AddObject(CARETROUBLE_POISON); break;
        case pml::pokepara::SICK_YAKEDO: mTroubleManager.AddObject(CARETROUBLE_FIRE); break;
        case pml::pokepara::SICK_KOORI: mTroubleManager.AddObject(CARETROUBLE_ICE); break;
        case pml::pokepara::SICK_MAHI: mTroubleManager.AddObject(CARETROUBLE_PARALYSIS); break;
        case pml::pokepara::SICK_NEMURI: mTroubleManager.AddObject(CARETROUBLE_SLEEPY); break;
        }

        mInitParam->pokemonParam->ClearDirt();
      }

      int type = mInitParam->pokemonParam->GetDirtType();      

      GFL_PRINT("\n\n*PokeDirt Type: %i  \nStatus: ",type);
      GFL_PRINT("\n\n*CanUseWater: %i\n",dataManager.TypeEnabled_Water());
      GFL_PRINT("\n\n*CanUseHair: %i\n",dataManager.TypeEnabled_Hair());
      GFL_PRINT("\n\n*CanUseDust: %i\n",dataManager.TypeEnabled_Dust());
      GFL_PRINT("\n\n*CanUseDirt: %i\n",dataManager.TypeEnabled_Dirt());

      for(int z=0;z<5;z++)
      {
        int isValid = mInitParam->pokemonParam->CheckDirtPos(z);
        GFL_PRINT("%i ",isValid);

        if(isValid==0 || z>=dataManager.GetCareData()->jointCount) continue;
     
        switch(type)
        {
          case pml::pokepara::DIRT_TYPE_HAIR:
            mTroubleManager.AddObject(CARETROUBLE_HAIR,z); break;

          case pml::pokepara::DIRT_TYPE_STAIN:
            mTroubleManager.AddObject(CARETROUBLE_DIRT,z); break;

          case pml::pokepara::DIRT_TYPE_SAND:
            mTroubleManager.AddObject(CARETROUBLE_DUST,z); break;

          case pml::pokepara::DIRT_TYPE_WATER:
            mTroubleManager.AddObject(CARETROUBLE_WATER,z); break;
        }

      }
      GFL_PRINT("\n");
    }
#if defined(GF_PLATFORM_WIN32)
    //else if(GetRandom(3)==0) // Care Object Test:
    {
  //    mTroubleManager.AddObject(CARETROUBLE_SLEEPY);
  //    mTroubleManager.AddObject(CARETROUBLE_HAIR);
    }
#endif
          
    mBgSystem->Create();
    mBgSystemNear->Create();
    GetBgSystem()->SetVisible(true);    

    mModel->SetVisible(true);
    pokemonAI.SetupDualMonsterCamera();
    mRenderSys->CalculateEatCameraViewProjMatrix();
    mRenderSys->CalculateFixedSizeCameraViewProjMatrix();
    
    Update();

    return true;
    

  }

  return false;

}

bool KawaigariMain::IsIceStatus(void)
{
  if(mInitParam->pokemonParam!=NULL)
  {
    // Sick?
    if(mInitParam->pokemonParam->HaveSick() )
    {
      switch(mInitParam->pokemonParam->GetSick())
      {
        case pml::pokepara::SICK_KOORI: return(true);
      }
    }
  }
  return(false);

}

//終了
bool KawaigariMain::Finalize(void)
{
  switch( mExitSeq )
  {
  case 0:
    // Reset Ignore Days:
    {    
      Savedata::Misc* pMiscSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetMisc();
      pMiscSaveData->ResetKawaigariPastDays();
    }
    
    // Mame Data:
    mameManager.Finalize();

    // Care Objects:
    mTroubleManager.Finalize();

    // Copy Parameters back:
    if(mInitParam->pokemonParam!=NULL)
    {
      // Diary:
      PokeDiary::Set( PDID_COUNT_PARLE, mInitParam->pokemonParam->GetMonsNo());  

      switch(Mode())
      {
        // Save Changes:
        case KAWAIGARI_MODE_NORMAL:
        case KAWAIGARI_MODE_RESORT:
        case KAWAIGARI_MODE_FORCE_IDLE:
          {
            u8 feed,affection,playful;

            pokemonAI.GetParameters(affection,playful,feed);

            mInitParam->pokemonParam->SetFeed(feed);

            u8 friendshipChange = affection - mStartFriendship;
            PokeTool::nakayoshi::UpdateFriendship(mInitParam->pokemonParam,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst(),friendshipChange);
            
            mInitParam->pokemonParam->SetNadenadeValue(playful);
          }
          break;

        default: break; // No Save
      }
      
    }

    if(mOptionCoreParamManager) mOptionCoreParamManager->Finalize();
    
    mBgSystem->Delete();
    mBgSystemNear->Delete();
    mRenderSys->GetRenderingPipeLine()->RemoveObject_Poke(mModel,0);
    mExitSeq++;
    break;

  case 1:
#if PM_DEBUG     
    if(!debugFont.Finalize()) return(false);
#endif

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG     
    debugManager.Finalize();    
#endif
#endif

    if(!mBgSystem->IsFinishDelete()) return(false);
    if(!mBgSystemNear->IsFinishDelete()) return(false);
    mModel->CleanColorShader();
    mPokeModelSys->DeleteModel(mModel);
    mPokeModelSys->DeletePokeModelHeap();
    mExitSeq++;
    break;

  case 2:    
    if(dataManager.Finalize()) mExitSeq++;
    break;

  case 3:
    mBgSystem->Terminate();    
    mBgSystemNear->Terminate();    
    mPokeModelSys->TermSystemAsync();
    mExitSeq++;
    break;

  case 4:
    if(!mBgSystem->IsFinishTerminate())return(false); 
    if(!mBgSystemNear->IsFinishTerminate())return(false); 
    
    if(!mPokeModelSys->IsFinishTermSystemAsync()) return(false);

    if(!UnloadMonsterBall()) return(false);
    if(!UnloadEffects()) return(false);
    if(!UnloadPokeMame()) return(false);
    
    mResourceManager->DeleteAllResource(GetEffectSystem(),GetParticleIndexManager());		
    ///mResourceManager->DeleteAllResource();		

    // Close GARCS:
    {
      // Monster Ball:
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= ARCID_CHARA_MODEL_BATTLE_NORMAL;
      GetAsyncFileManager()->SyncArcFileClose(req);
    }

    {
      // Models, Effects, Etc:
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= ARCID_KAWAIGARI_RESOURCE;
      GetAsyncFileManager()->SyncArcFileClose(req);
    }

    mExitSeq++;
    break;
    
  case 5:
    if(uiManager->End()) mExitSeq++;
    break;

  case 6:
#if PM_DEBUG
    FinalizeSelectMenuDebug();
#endif // PM_DEBUG

    GFL_SAFE_DELETE(mOptionCoreParamManager);

    GFL_SAFE_DELETE(uiManager);
    GFL_SAFE_DELETE(mPokeModelSys);
    
    GFL_SAFE_DELETE(mBgSystem);
    GFL_SAFE_DELETE(mBgSystemNear);
    
    mRenderSys->Cleanup();
    mRenderSys->Terminate();
    GFL_SAFE_DELETE(mRenderSys);
    GFL_SAFE_DELETE(mResourceManager);

    GFL_SAFE_DELETE(mEffectGroupIDList[0]);
    GFL_SAFE_DELETE(mEffectGroupIDList[1]);      
    GFL_SAFE_DELETE(mEffectGroupIDList[2]);      
    GFL_SAFE_DELETE(mEffectSystem);
    GFL_SAFE_DELETE(mEffectHeap);
    GFL_SAFE_DELETE(mParticleIndexManager);

    mExitSeq++;
    break;

  case 7:

    GFL_SAFE_DELETE(mAppHeap);
    GFL_SAFE_DELETE(mGlAllocator);
    GFL_SAFE_DELETE(mAppHeap);
    GFL_SAFE_DELETE(mAllocator);
    return true;
    
  }
  return false;
}

void KawaigariMain::CleanPokemonPart(int index)
{
  if(mInitParam->pokemonParam!=NULL)
    mInitParam->pokemonParam->RemoveDirtPos(index);
}

void KawaigariMain::CurePokemon(void)
{
   if(mInitParam->pokemonParam!=NULL)
     mInitParam->pokemonParam->RecoverSick();
}

#if defined(GF_PLATFORM_WIN32)
void KawaigariMain::ReloadPokemon(void)
{

  switch(mReloadSeq)
  {
    case 0:      
      mModel->SetVisible(false);
      mRenderSys->GetRenderingPipeLine()->RemoveObject_Poke(mModel,0);
      mReloadSeq++;
      break;

    case 1:
      mModel->CleanColorShader();
      mPokeModelSys->DeleteModel(mModel);
      mReloadSeq++;
      break;

    case 2:
      LoadPokemon();    
      mReloadSeq++;
      break;

    case 3:
      if(mModel->IsFinishLoad()) mReloadSeq++;
      break;

    case 4:
      mModel->Create();

      if(!mModel->IsAvailableIdModel())
      {
        GFL_PRINT("*** ERROR NO ID TEXTURE FOUND, LOAD PIKACHUU! ***\n");
        // Delete, and force load pikachuu:
        mPokeModelSys->DeleteModel(mModel);
        mForceLoadPikachuu = true;
        mReloadSeq = 2;
        break;
      }
      else mForceLoadPikachuu = false;

      mReloadSeq++;
      break;

    case 5:
      // Create Pokemon:
      mModel->SetupColorShader();
      mModel->SetVisible(false);

      ///mModel->SetInterestSpeed(0,5.0f);

      // Setup Light and Shadow:
      mModel->SetLightSetNo(LIGHTNO_POKEMON_TOP+mIdx);
      {
        int matNum = mModel->GetMaterialCount();
        for( int i=0;i<matNum;i++ )
        {
          mModel->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
        }
      }
      if( GetPokeModel()->GetShadowModel() )
      {
        int matNum = mModel->GetShadowModel()->GetMaterialCount();
        for( int i=0;i<matNum;i++ )
        {
          mModel->GetShadowModel()->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
        }
      }

      mRenderSys->GetRenderingPipeLine()->AddObject_Poke(mModel,LIGHTNO_POKEMON_TOP+mIdx);          
      mRenderSys->SetAdjustScale(mModel->GetAdjustScale());

      // Set the 3D Model for the AI:
      pokemonAI.SetPokeModel(mModel);    

      // Change to default Idle Animation:
      pokemonAI.ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT,false);
      mModel->ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT,true);
      mModel->SetAnimationLoop(true);

      mModel->SetVisible(true);

      mReloadSeq = 99; // Finished!
      break;

    // Finished Reloading Pokemon
    case 99:
      pokemonAI.SetupDualMonsterCamera();
      mRenderSys->CalculateEatCameraViewProjMatrix();
      mRenderSys->CalculateFixedSizeCameraViewProjMatrix();
      mReloadSeq=0;
      mReloadPokemon = false;
      break;
  }
}
#endif

//更新
bool KawaigariMain::Update(void)
{    
#if defined(GF_PLATFORM_CTR)
///  GFL_PRINT("Memory: %iK / %iK AllocOK:%iK\n",GetHeap()->GetTotalFreeSize()/1024,GetHeap()->GetTotalSize()/1024,GetHeap()->GetTotalAllocatableSize()/1024); 
#endif

  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  if(mReloadPokemon)
  {
    ReloadPokemon();    
    return(false);
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL )) debugManager.ToggleEnable();
  debugManager.Update();

 /// if( pButton->IsTrigger( gfl2::ui::BUTTON_R )) CreateEffect(KAWAIGARI_EFFECT_HEART_TOUCH);
#endif
#endif 

  mMonsterBallModel->Update();
  mPokeModelSys->Update();



  {
    GameSys::AlolaTime time;
    time.Initialize();
    mBgSystem->Update(time.GetHour(),time.GetMinute(),time.GetSecond());
    mBgSystemNear->Update(time.GetHour(),time.GetMinute(),time.GetSecond());
  }
 
  // HIO-RELOAD:
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X )  )
  {
#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG    
    dataManager.HIOReload();    
#endif
#endif
///    mPause = (mPause==false);
  }

  if(uiManager!=NULL)
  {
      uiManager->UpdateTree();      

      if(uiManager->IsPressedCancelButton() || mFinished) 
      {
        // Render System Update:
        mRenderSys->Update();

        // Set Exit Code:
        if(uiManager->IsChangePokemon()) mInitParam->exitCode = KAWAIGARI_EXITCODE_POKEMONSELECT;
        else mInitParam->exitCode = KAWAIGARI_EXITCODE_NORMAL;
        return(true);
      }
  }


  if(!mPause)
  {  
    // Camera Update:
    cameraSystem.Update();

    // Pokemon AI Update:
    pokemonAI.Update();

    bool effectExists = false;

    // Update All Effects:
    for(int n=0;n<MAX_KAWAIGARI_EFFECT;n++)
    {
      if(mEffects[n]) 
      {
        mEffects[n]->Update();                           
        effectExists = true;

        if(mUpdateEffectPos[n])
        {
          gfl2::math::Vector3 nodePos;
          mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_HEAD,0,&nodePos);               
         
          float posX,posY;
          ConvertWorldPointToScreenPoint(posX,posY,nodePos);

          u32 tx,ty;
          tx = (u32)posX;
          ty = (u32)posY;          
          Get3DEffectPositionByXY(&nodePos,tx,ty,50.0f);

          mEffects[n]->SetPosition(nodePos);

        }
      }
    }

    // Care Objects
    mTroubleManager.Update();
    mTroubleManager.SetCanUpdate(true); // Reset CanUpdate for next frame..

    for(int n=0;n<MAX_KAWAIGARI_CARE_EFFECT;n++)
    {
      if(mCareEffects[n])
      {
        mCareEffects[n]->Update();                           
        effectExists = true;
      }
    }

    if(mEffectSystem && effectExists)
    {
      mEffectSystem->BeginFrame();
      mEffectSystem->Calc(0);
      mEffectSystem->Calc(1);
      mEffectSystem->Calc(2);
    }

    // Update PokeMame:
    for(int z=0;z<MAX_POKEMAME_MODEL;z++)
    {
      if(mPokeMameModel[z]==NULL) continue;
      mPokeMameModel[z]->SetPosition(mFoodPosition.x,mFoodPosition.y,mFoodPosition.z);
      mPokeMameModel[z]->SetRotate(mFoodRotation.x,mFoodRotation.y,mFoodRotation.z);

      mFoodScale.x = mFoodScale.y = mFoodScale.z = dataManager.GetFoodScale();      

      mPokeMameModel[z]->SetScale(mFoodScale.x,mFoodScale.y,mFoodScale.z);
      mPokeMameModel[z]->Update();
    }

    for(int z=0;z<3;z++)
    {
      if(mPokeMamePieceModel[z]==NULL) continue;
      mPokeMamePieceModel[z]->SetPosition(mFoodPiecePosition[z].x,mFoodPiecePosition[z].y,mFoodPiecePosition[z].z);
      mPokeMamePieceModel[z]->SetRotate(mFoodPieceRotation[z].x,mFoodPieceRotation[z].y,mFoodPieceRotation[z].z);

      mPokeMamePieceModel[z]->SetScale(dataManager.GetFoodScale());

      mPokeMamePieceModel[z]->Update();
    }

    // Remove Dead Effects:
    RemoveDeadEffects();

#if defined(GF_PLATFORM_CTR)
    ///    GFL_PRINT("AfterMemory: %iK / %iK AllocOK:%iK\n",GetHeap()->GetTotalFreeSize()/1024,GetHeap()->GetTotalSize()/1024,GetHeap()->GetTotalAllocatableSize()/1024); 
#endif

  }


  // Update Touch Pos:
  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
     
  // PokeMameTest:
  static gfl2::math::Vector3 vec(0.0f,0.0f,0.0f);

  if(uiManager!=NULL)
  {
    if(touchPanel->IsTouch())  
    {
      gfl2::math::VEC3 pos;
      touchPanel->GetXY(&pos);

      pokemonAI.GetCurrentTouchPos(pos.x,pos.y);

      uiManager->SetTouchPos(pos.x,pos.y);

      bool up = false;
      bool down = false;
      const s32 cur = mRenderSys->GetDualScreenMonsterCameraMode();

      static s32 tf = 0;
      const s32 ty = touchPanel->GetY();
      if(ty <= 24 && cur == KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA)
      {
        if(tf < 0)
        {
          tf = 0;
        }
        if(30 <= ++tf)
        {
          up = true;
          tf = 0;
        }
      }
      else if(216 <= ty && cur == KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_UPPER)
      {
        if(0 < tf)
        {
          tf = 0;
        }
        if(--tf < -30)
        {
          down = true;
          tf = 0;
        }
      }
      else
      {
        tf = 0;
      }

      if(up &&  (pokemonAI.GetState()==AI_STATE_TOUCH || pokemonAI.GetState()==AI_STATE_CARE || pokemonAI.GetState()==AI_STATE_CAREWAIT))
      {
        mRenderSys->SetDualScreenMonsterCameraMode(KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER,false);
      }
      else if(down && (pokemonAI.GetState()==AI_STATE_TOUCH || pokemonAI.GetState()==AI_STATE_CARE || pokemonAI.GetState()==AI_STATE_CAREWAIT))
      {
        mRenderSys->SetDualScreenMonsterCameraMode(KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER,false);
      }
                             
    }

  }

  // Render System Update:
  mRenderSys->Update();

  if(mCreateEffectRequest != -1)
  {
    CreateEffect(mCreateEffectRequest);
    mCreateEffectRequest = -1;
  }
 
  return false; // Not done yet
}

//描画
void KawaigariMain::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG 
  if(mReloadPokemon) return;
#endif
#endif

  if(uiManager->IsPressedCancelButton()) return;

   if(uiManager!=NULL) uiManager->DrawTree(displayNo);
 

  if(mRenderSys!=NULL) 
  {      
      mRenderSys->Draw(displayNo);      
  }

#if PM_DEBUG 
  debugFont.Draw(displayNo);
#endif 

}


void KawaigariMain::LoadPokemon(void)
{
  PokeTool::PokeModel::SetupOption opt;
  opt.dataHeap = GetHeap();
  opt.workHeap = GetHeap()->GetLowerHandle();
  opt.animeType = PokeTool::MODEL_ANIMETYPE_KAWAIGARI;  
  opt.useShadow = true;
  opt.useIdModel = true;
  

  PokeTool::SimpleParam pokeParam;
  // PokeTool::GetSimpleParam(&pokeParam,pp);

  if(mForceLoadPikachuu)
  {  
//    pokeParam.monsNo = (MonsNo)185;
    pokeParam.monsNo = (MonsNo)25;
    pokeParam.formNo = (pml::FormNo)0;
    pokeParam.sex = (pml::Sex)0;
  }
  else
  {
    if(mInitParam && mInitParam->pokemonParam)
    {
      pokeParam.monsNo = mInitParam->pokemonParam->GetMonsNo();
      pokeParam.formNo = mInitParam->pokemonParam->GetFormNo();
      pokeParam.sex = mInitParam->pokemonParam->GetSex();
      pokeParam.isRare = mInitParam->pokemonParam->IsRare();
      pokeParam.perRand = mInitParam->pokemonParam->GetPersonalRnd();
      pokeParam.isEgg = mInitParam->pokemonParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
    }

    else
    {
      //pokeParam.monsNo = (MonsNo)185;
      pokeParam.monsNo = (MonsNo)25;
      ///  pokeParam.monsNo = (MonsNo)722;
      pokeParam.formNo = (pml::FormNo)0;
      pokeParam.sex = (pml::Sex)0;
    }
      
  }

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  if(mReloadPokemon && !mForceLoadPikachuu)
  {
    pokeParam.monsNo = (MonsNo)mReloadNo;
    pokeParam.formNo = (pml::FormNo)mReloadFormNo;
    pokeParam.sex = (pml::Sex)mReloadSexNo;
  }
#endif
#endif
  
  DemoLib::Obj::Object_Poke::LOAD_PARAM param;
  param.modelSys = GetPokeModelSys();
  param.param = &pokeParam;
  param.opt = &opt;
  param.dataHeap = GetHeap();
  param.workHeap = GetHeap()->GetLowerHandle();
  
  mModel = GetPokeModelSys()->CreateModelSimple(param.param,*param.opt);
  dataManager.SetPokemonIDForm(pokeParam.monsNo,pokeParam.formNo,pokeParam.sex);
  pokemonAI.SetMonsterID(pokeParam.monsNo,pokeParam.formNo);

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  debugManager.SetPokemonIDForm(pokeParam.monsNo,pokeParam.formNo,pokeParam.sex);
#endif
#endif

}

KawaigariCamera *KawaigariMain::GetCamera(void)
{ 
  return(&cameraSystem); 
}

KawaigariDataManager *KawaigariMain::GetDataManager(void)
{ 
  return(&dataManager); 
}

KawaigariRandomActionSelector *KawaigariMain::GetRandomActionSelector(void)
{
  return(&actionSelector); 
}

KawaigariPokemon *KawaigariMain::GetPokemonAI(void)
{
  return(&pokemonAI);
}

KawaigariUIManager *KawaigariMain::GetUIManager(void)
{
  return(uiManager);
}

int KawaigariMain::GetRandom(int max)
{
  // Fix later!
  if(!max) return(0);
  return(System::GflUse::GetPublicRand()%max);
}

int KawaigariMain::GetMonsterBallType(void)
{
  if(mInitParam->pokemonParam!=NULL)
  {
    int type = mInitParam->pokemonParam->GetGetBall();
    return(type);
  }
  
  return(1);
}

void KawaigariMain::LoadMonsterBall(void)
{
  mMonsterBallModel = GFL_NEW(mHeap)KawaigariObject_Gfmdl(this,GetRenderSystem()->GetRenderingPipeLine());

  int ballType = GetMonsterBallType();
  int drawType = 0;
  void *buf = GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,CHARA_MODEL_ID_BTOB0201_00+ballType-1)->GetData(0);

  mMonsterBallModel->CreateBuffer(buf,mHeap,GetGLAllocator());    
  mMonsterBallModel->AddRenderPath(drawType);

  int matNum = mMonsterBallModel->GetBaseModel()->GetMaterialCount();
  for( int matNo=0;matNo<matNum;matNo++ )
  {
    mMonsterBallModel->GetBaseModel()->SetMaterialLightSetIndex(matNo,0);
  }

  mMonsterBallModel->SetScale(1.5f,1.5f,1.5f);
  mMonsterBallModel->SetVisible(false);
  mMonsterBallModel->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);

}

bool KawaigariMain::UnloadMonsterBall(void)
{
  if(mMonsterBallModel!=NULL)
  {  
    if( mMonsterBallModel->IsAddRenderPath() )
    {
      mMonsterBallModel->RemoveRenderPath();
      return(false);
    }  
    mMonsterBallModel->Delete();
    GFL_SAFE_DELETE(mMonsterBallModel);
  }
  return(true);
}

void KawaigariMain::UpdateFoodPositionLook(void)
{
  // First Convert Food Position to 2D:
  gfl2::math::Vector3 world_point_3(mFoodPosition.x, mFoodPosition.y, mFoodPosition.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  GetRenderSystem()->GetMameViewAndProjectionMatrix(view_mtx,proj_mtx);

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;

  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  float resultX,resultY;
  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  resultX = screen_point_2.x / 2.0f + 0.5f;
  resultY = screen_point_2.y / 2.0f + 0.5f;
  resultX *= 319.0f;
  resultY *= 239.0f;


  bool isTop = false;
  bool isLeft = false;
  bool isRight = false;
  bool isBottom = false;

  // Model Is top Screen:  
  if(resultY<0.0f)
  {
     isTop=true;
  }
  if(resultY>=239.0f)
  {
    isBottom=true;
  }
  if(resultX<0.0f)
  {
    isLeft = true;
  }
  if(resultX>=319.0f)
  {
    isRight = true;
  }

  u32 px = (u32)resultX;
  u32 py = (u32)resultY;

  if(isTop)
  {
    py = 0;
  }
  if(isBottom)
  {
    py = 239;
  }
  if(isLeft)
  {
    px = 0;
  }
  if(isRight)
  {
    px = 319;
  }

  // Default: Use Old Calcuation
  bool useNewCalculation = false;

  // Use New Look Calculation?  
  switch(pokemonAI.GetMonsNo())
  {
    case MONSNO_AGIRUDAA: // (617)
    case MONSNO_ZYAROODA: // (497)
    case MONSNO_TODOZERUGA: // (365)
    case MONSNO_KYATAPII: // (10)
      useNewCalculation = true;
      break;

    case MONSNO_NYAASU: // (52)
      if(pokemonAI.GetFormNo()==FORMNO_NYAASU_ALOLA) useNewCalculation = true;
      break;

    case MONSNO_NASSII: //  (103)
      if(pokemonAI.GetFormNo()==FORMNO_NASSII_ALOLA)
      {
        useNewCalculation = true;     
      }
      break;
  }

  if(useNewCalculation)
  {
    Get3DLookPositionByXY(&mFoodPositionLook,px,py);
    return;
  }

  // Old Calculation:
  // Convert 2D point to 3D:
  Get3DTouchPositionByXY(&mFoodPositionLook,px,py,TPT_MAMELOOK);

}

gfl2::math::Vector3 *KawaigariMain::GetFoodPositionLook(void)
{
  UpdateFoodPositionLook();
  return(&mFoodPositionLook);
}

bool KawaigariMain::IsMultipleHead(void)
{
  switch(pokemonAI.GetMonsNo())
  {
    case MONSNO_GARUURA: // 115    
    case MONSNO_NIDANGIRU: // 680
    case MONSNO_DOODOO: // 084
    case MONSNO_BAIBANIRA: // 584
    case MONSNO_GIGIARU: // 600
    case MONSNO_GIGIGIARU: // 601
    case MONSNO_ZIHEDDO: // 634
    case MONSNO_KAMETETE: // 688    
      return(true);

  }
  return(false);
}

void KawaigariMain::CreateEffect(int type,gfl2::math::Vector3 *pos,int drawType,gfl2::math::Vector3 *rot,bool playSE)
{
    static int posSwitcher = 0;
  
  if(type<0 || type>KAWAIGARI_EFFECT_TYPE_MAX) return;


  int source = -1;

  // Find Free Effect:
  for(int n=0;n<MAX_KAWAIGARI_EFFECT;n++)
  {
    if(mEffects[n]==NULL)
    {
      source = n;
      break;
    }
  }

  if(source==-1) return; // No Free Effects to use!

  mUpdateEffectPos[source] = false;
  mEffects[source] = GFL_NEW(mHeap)KawaigariObject_Effect(this);

  // Which Effect?:
  int arcIdx = 0;

  switch(type)
  {
    case KAWAIGARI_EFFECT_CLEANSUCCESS: 
    case KAWAIGARI_EFFECT_CLEANSPOT:                 
      arcIdx = GARC_kawaigari_resource_ek_success_kira_lz_BPTCL; break;

    case KAWAIGARI_EFFECT_SMOKE: arcIdx = GARC_kawaigari_resource_ek_smoke_lz_BPTCL; break;
        
    case KAWAIGARI_EFFECT_HEART_TOUCH: 
      arcIdx = GARC_kawaigari_resource_ek_heart_lz_BPTCL; break;
    
    case KAWAIGARI_EFFECT_DUST_EFFECT: arcIdx = GARC_kawaigari_resource_ek_hokori_lz_BPTCL; break;

    case KAWAIGARI_EFFECT_MUSICNOTE: arcIdx = GARC_kawaigari_resource_ek_note_lz_BPTCL; break;

    case KAWAIGARI_EFFECT_AFFECTIONSUCCESS: arcIdx = GARC_kawaigari_resource_ek_nakayoshi_up_lz_BPTCL; break;
    case KAWAIGARI_EFFECT_QUESTIONMARK: arcIdx = GARC_kawaigari_resource_ek_symbol03_lz_BPTCL; break;
    case KAWAIGARI_EFFECT_EXCLAMATIONMARK: arcIdx = GARC_kawaigari_resource_ek_symbol01_lz_BPTCL; break;
    case KAWAIGARI_EFFECT_CARETOOLFADE: arcIdx = GARC_kawaigari_resource_ek_smoke_lz_BPTCL; break;   
    case KAWAIGARI_EFFECT_HIGHFIVE: arcIdx = GARC_kawaigari_resource_ek_high_five_lz_BPTCL; break;   
    
    default: arcIdx = GARC_kawaigari_resource_ek_success_kira_lz_BPTCL; break;
  }
  
  int resIdx = mResourceManager->GetEffectResIdx(ARCID_KAWAIGARI_RESOURCE,arcIdx);  

  gfl2::math::Vector3 myNodePos;

  if(type==KAWAIGARI_EFFECT_CLEANSUCCESS)
  {    
    mEffects[source]->SetScale(0.20f,0.20f,0.20f);
    mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_CENTER,0,&myNodePos);       
    pos = &myNodePos;
  }
  else if(type == KAWAIGARI_EFFECT_AFFECTIONSUCCESS)
  {    
    mEffects[source]->SetScale(0.50f,0.50f,0.50f);
    mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_CENTER,0,&myNodePos);       
    pos = &myNodePos;
  }
  else if(type == KAWAIGARI_EFFECT_MUSICNOTE)
  {
    mEffects[source]->SetScale(0.40f,0.40f,0.40f);
    gfl2::math::Vector3 nodePos;
    
    // Pokemon EXCEPTION(680): Has Two Heads:
    if(IsMultipleHead()) mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&myNodePos);       
    else mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_HEAD,0,&myNodePos);    
    
    if(pos!=NULL) myNodePos.x += pos->x;        
    pos = &myNodePos;
    posSwitcher = 0;
  }
  else if(type==KAWAIGARI_EFFECT_HEART_TOUCH)
  {
    if(playSE) pokemonAI.PlaySE( SEQ_SE_KW_HEART1 );
    mEffects[source]->SetScale(0.3f,0.3f,0.3f);

    // Pokemon EXCEPTION(680): Has Two Heads:
    if(IsMultipleHead()) mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&myNodePos); 
    else mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_HEAD,0,&myNodePos); 

    float rotZ = 0.0f;
    if(posSwitcher%4==0) rotZ = -20.0f;
    else if(posSwitcher%4==1) rotZ = 20.0f;
    else if(posSwitcher%4==2) rotZ = -10.0f;
    else if(posSwitcher%4==3) rotZ = 10.0f;
    
    mEffects[source]->SetRotate(0.0f,0.0f,rotZ);
    pos = &myNodePos;
  }
  else if(type==KAWAIGARI_EFFECT_EXCLAMATIONMARK)
  {    
    mEffects[source]->SetScale(0.4f,0.4f,0.4f);

    // Pokemon EXCEPTION(680): Has Two Heads:
    if(IsMultipleHead()) mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&myNodePos); 
    else mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_HEAD,0,&myNodePos);             
    pos = &myNodePos;
  }
  else if(type==KAWAIGARI_EFFECT_QUESTIONMARK)
  {    
    mEffects[source]->SetScale(0.4f,0.4f,0.4f);

    // Pokemon EXCEPTION(680): Has Two Heads:
    if(IsMultipleHead()) mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&myNodePos); 
    else mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_HEAD,0,&myNodePos);             
    pos = &myNodePos;
  }


  if(pos!=NULL)
  {
    // Fix scale of certain effects:
    switch(type)
    {
      case KAWAIGARI_EFFECT_MUSICNOTE:
      case KAWAIGARI_EFFECT_DUST_EFFECT:
      case KAWAIGARI_EFFECT_CLEANSPOT:
      case KAWAIGARI_EFFECT_CLEANSUCCESS:
      case KAWAIGARI_EFFECT_HIGHFIVE:
      case KAWAIGARI_EFFECT_SMOKE:
      case KAWAIGARI_EFFECT_AFFECTIONSUCCESS:
      case KAWAIGARI_EFFECT_HEART_TOUCH:
      case KAWAIGARI_EFFECT_EXCLAMATIONMARK:
      case KAWAIGARI_EFFECT_QUESTIONMARK:
        {
          // Convert to 2d then back to 3d from Set Camera:
          float rx,ry;
          ConvertWorldPointToScreenPoint(rx,ry,*pos);

          if(type==KAWAIGARI_EFFECT_SMOKE)
          {
            gfl2::ui::TouchPanel * touchPanel;
            touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
            u32 touchX = touchPanel->GetX();
            u32 touchY = touchPanel->GetY();
            rx = (float)touchX;
            ry = (float)touchY;
          }

          if(ry<0.0 && pokemonAI.IsDualScreenMonster())
          {
            ConvertWorldPointToScreenPointTop(rx,ry,*pos);

            if(type==KAWAIGARI_EFFECT_EXCLAMATIONMARK || type==KAWAIGARI_EFFECT_QUESTIONMARK) ry-=16;

            // Make sure on screen:           
            if(ry<100.0f) ry=100.0f;
            if(rx<24.0f) rx = 24.0f;

            u32 tx = (u32)rx,ty=(u32)ry;

            Get3DEffectTouchPositionByXY_Top(pos,tx-32,ty-24);  
          }
          else
          {               
            GFL_PRINT("*EFFECT 2D: %f,%f\n",rx,ry);
                        
            if(type==KAWAIGARI_EFFECT_EXCLAMATIONMARK || type==KAWAIGARI_EFFECT_QUESTIONMARK) ry-=16;

            // Make sure on screen:
            if(ry<55.0f && type!=KAWAIGARI_EFFECT_DUST_EFFECT )
            {
              switch(type)              
              {
                // Don't Adjust to on screen:
                case KAWAIGARI_EFFECT_DUST_EFFECT:
                case KAWAIGARI_EFFECT_CLEANSPOT:
                case KAWAIGARI_EFFECT_CLEANSUCCESS:
                case KAWAIGARI_EFFECT_SMOKE:      
                case KAWAIGARI_EFFECT_AFFECTIONSUCCESS:
                  break;

                // Adjust to onscreen Y position:
                default:
                  ry=55.0f;
              }
            }

            u32 tx = (u32)rx,ty=(u32)ry;
            Get3DEffectTouchPositionByXY(pos,tx,ty);  
          }

          drawType = 2; // Use Fixed Effect Camera
        }
        break;
    }
        
    mEffects[source]->SetPosition(pos->x,pos->y,pos->z);
  }
  if(rot!=NULL)
  {
    const gfl2::math::Vector3 rotV = *rot;
    mEffects[source]->SetRotate(rotV);
  }

 
  if(type==KAWAIGARI_EFFECT_CLEANSPOT)
  {
    mEffects[source]->SetScale(0.15f,0.15,0.15f);
  }

  mEffects[source]->Update();
  mEffects[source]->CreateResIdx(GetEffectHeap(),resIdx);
  mEffects[source]->AddRenderPath(drawType);

  
  
  if(type==KAWAIGARI_EFFECT_DUST_EFFECT)
  {    
    mEffects[source]->SetScale(0.3f,0.3f,0.3f);    
  }
  else if(type==KAWAIGARI_EFFECT_HIGHFIVE)
  {
    mEffects[source]->SetScale(0.15f,0.15f,0.15f);
  }
  else if(type==KAWAIGARI_EFFECT_CARETOOLFADE)
  {    
    mEffects[source]->SetScale(0.125f,0.125f,0.125f);
  }
  else if(type==KAWAIGARI_EFFECT_MUSICNOTE)
  { 
     if(playSE) pokemonAI.PlaySE( SEQ_SE_KW_HEART1 );
  }
  else if(type==KAWAIGARI_EFFECT_SMOKE)
  {
    mEffects[source]->SetScale(0.5f,0.5f,0.5f);
  }
  else if(type==KAWAIGARI_EFFECT_QUESTIONMARK)
  {    
    pokemonAI.PlaySE( SEQ_SE_KW_THINK );    
  }
  
  posSwitcher++;

  // Don't adjust scale of certain effects:
  switch(type)
  {
    case KAWAIGARI_EFFECT_DUST_EFFECT:
    case KAWAIGARI_EFFECT_CLEANSPOT:
    case KAWAIGARI_EFFECT_CARETOOLFADE:
    case KAWAIGARI_EFFECT_SMOKE: 
    case KAWAIGARI_EFFECT_CLEANSUCCESS:
    case KAWAIGARI_EFFECT_HIGHFIVE:
    case KAWAIGARI_EFFECT_MUSICNOTE:
    case KAWAIGARI_EFFECT_HEART_TOUCH:
    case KAWAIGARI_EFFECT_EXCLAMATIONMARK:
    case KAWAIGARI_EFFECT_AFFECTIONSUCCESS:
    case KAWAIGARI_EFFECT_QUESTIONMARK:
      return;
  }
  
}

float KawaigariMain::GetCalculatedEffectScale( gfl2::math::Vector3 pos,bool isStatusEffect )
{   
  float dist = GetCamera()->GetCameraRangeDist(pos);
  float ADJUST_DIST = 600.0f;

  float scale = 1.0f;
  scale *= dist / ADJUST_DIST;

  float adjustScale = mModel->GetAdjustScale();
  scale *= adjustScale;
  
  if(scale < 0.11f && dist>45.0f && isStatusEffect) return(scale);
  if(scale < 0.20f && dist<80.0f) scale = 0.45f;

  return(scale);
}

void KawaigariMain::UpdateSleepEffectPos(int effectID)
{  
  if(effectID<0 || effectID>=MAX_KAWAIGARI_CARE_EFFECT) return;

  gfl2::math::Vector3 nodePos,*pos;
  mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&nodePos);         
  mCareEffects[effectID]->SetPosition(nodePos);    

  pos = &nodePos;
  // Convert to 2d then back to 3d from Set Camera:
  float rx,ry;
  ConvertWorldPointToScreenPoint(rx,ry,*pos);

  if(ry<0.0f)
  {
    ConvertWorldPointToScreenPointTop(rx,ry,*pos);

    u32 tx = (u32)rx,ty=(u32)ry;
 
    // Make sure on screen:           
    if(ty<75) ty=75;

    Get3DEffectTouchPositionByXY_Top(pos,tx-32,ty-24);  
  }
  else
  {          
    u32 tx = (u32)rx,ty=(u32)ry;

    // Make sure on screen:
    if(ty<55) ty=55;

    Get3DEffectTouchPositionByXY(pos,tx,ty);  
  }

  mCareEffects[effectID]->SetPosition(pos->x,pos->y,pos->z);    
  mCareEffects[effectID]->Update();  

}

int KawaigariMain::CreateCareEffect(int type,gfl2::math::Vector3 *pos,int drawType)
{
  if(type<0 || type>KAWAIGARI_EFFECT_TYPE_MAX) return(-1);

  int source = -1;

  // Find Free Effect:
  for(int n=0;n<MAX_KAWAIGARI_CARE_EFFECT;n++)
  {
    if(mCareEffects[n]==NULL)
    {
      source = n;
      break;
    }
  }

  if(source==-1) return(source); // No Free Care Effects to use!

  mCareEffects[source] = GFL_NEW(mHeap)KawaigariObject_Effect(this);

  // Which Effect?:
  int arcIdx = 0;

  switch(type)
  {
  case KAWAIGARI_EFFECT_DIRT: arcIdx = GARC_kawaigari_resource_ek_yogore_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_DUST: arcIdx = GARC_kawaigari_resource_ek_dust_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_HAIR: arcIdx = GARC_kawaigari_resource_ek_hair_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_WATER: arcIdx = GARC_kawaigari_resource_ek_water_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_FIRE: arcIdx = GARC_kawaigari_resource_ek_fire_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_ICE: arcIdx = GARC_kawaigari_resource_ek_ice_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_POISON: arcIdx = GARC_kawaigari_resource_ek_poison_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_PARALYSIS: arcIdx = GARC_kawaigari_resource_ek_paralysis_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_SLEEPY: arcIdx = GARC_kawaigari_resource_ek_nemuri_lz_BPTCL; break;    
  case KAWAIGARI_EFFECT_BALLCAPTURE: arcIdx = GARC_kawaigari_resource_ek_ball_capture_lz_BPTCL; break;
  case KAWAIGARI_EFFECT_BALLIN: arcIdx = GARC_kawaigari_resource_ek_ball_in_lz_BPTCL; break; 
  case KAWAIGARI_EFFECT_BALLSHADOW: arcIdx = GARC_kawaigari_resource_ek_shadow_lz_BPTCL; break; 

  default: arcIdx = GARC_kawaigari_resource_ek_yogore_lz_BPTCL; break;
  }

  gfl2::math::Vector3 nodePos;

  if(type==KAWAIGARI_EFFECT_SLEEPY)
  {    
    mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&nodePos);         
    mCareEffects[source]->SetPosition(nodePos);    
    mCareEffects[source]->SetScale(0.75f,0.75f,0.75f);

    pos = &nodePos;
    // Convert to 2d then back to 3d from Set Camera:
    float rx,ry;
    ConvertWorldPointToScreenPoint(rx,ry,*pos);
    
    if(ry<0.0f)
    {
      ConvertWorldPointToScreenPointTop(rx,ry,*pos);

      u32 tx = (u32)rx,ty=(u32)ry;      

      // Make sure on screen:           
      if(ty<75) ty=75;

      Get3DEffectTouchPositionByXY_Top(pos,tx-32,ty-24);  
    }
    else
    {          
      u32 tx = (u32)rx,ty=(u32)ry;      

      // Make sure on screen:
      if(ty<55) ty=55;

      Get3DEffectTouchPositionByXY(pos,tx,ty);  
    }

    drawType = 2; // Use Fixed Effect Camera
  }

  if(pos!=NULL)
  {
    mCareEffects[source]->SetPosition(pos->x,pos->y,pos->z);    
    mCareEffects[source]->Update();  
  }

  int resIdx = mResourceManager->GetEffectResIdx(ARCID_KAWAIGARI_RESOURCE,arcIdx);  
  mCareEffects[source]->CreateResIdx(GetEffectHeap(),resIdx);
  mCareEffects[source]->AddRenderPath(drawType);

  return(source);

}

void KawaigariMain::ConvertWorldPointToScreenPoint_Mame(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point)
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  GetRenderSystem()->GetMameViewAndProjectionMatrix(view_mtx,proj_mtx);

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;

  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  resultX = screen_point_2.x / 2.0f + 0.5f;
  resultY = screen_point_2.y / 2.0f + 0.5f;
  resultX *= 319.0f;
  resultY *= 239.0f;
}

void KawaigariMain::ConvertWorldPointToScreenPoint(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point)
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  GetRenderSystem()->GetViewAndProjectionMatrix(view_mtx,proj_mtx);

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;

  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  resultX = screen_point_2.x / 2.0f + 0.5f;
  resultY = screen_point_2.y / 2.0f + 0.5f;
  resultX *= 319.0f;
  resultY *= 239.0f;
}

void KawaigariMain::ConvertWorldPointToScreenPointTop(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point)
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  GetRenderSystem()->GetViewAndProjectionMatrixTop(view_mtx,proj_mtx);

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;

  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  resultX = screen_point_2.x / 2.0f + 0.5f;
  resultY = screen_point_2.y / 2.0f + 0.5f;
  resultX *= 399.0f;
  resultY *= 239.0f;
}


void KawaigariMain::LoadEffects(void)
{    
  
}

void KawaigariMain::RemoveDeadEffects(void)
{
  for(int z=0;z<MAX_KAWAIGARI_EFFECT;z++)
  {  
    if(mEffects[z]==NULL) continue;

    if(!mEffects[z]->IsAlive())
    {
      mUpdateEffectPos[z] = false;
      if( mEffects[z]->IsAddRenderPath() ) mEffects[z]->RemoveRenderPath();
      else
      {
        mEffects[z]->Delete();
        GFL_SAFE_DELETE(mEffects[z]);
      }
    }
  }

  for(int z=0;z<MAX_KAWAIGARI_CARE_EFFECT;z++)
  {  
    if(mCareEffects[z]==NULL) continue;

    if(mKillCareEffect[z])
    {
      if( mCareEffects[z]->IsAddRenderPath() ) mCareEffects[z]->RemoveRenderPath();
      else
      {
        mCareEffects[z]->Delete();
        GFL_SAFE_DELETE(mCareEffects[z]);
        mKillCareEffect[z] = false;
      }
    }
  }


}

bool KawaigariMain::UnloadEffects(void)
{
  bool result = true;

  for(int z=0;z<MAX_KAWAIGARI_EFFECT;z++)
  {  
    if(mEffects[z]!=NULL)
    {  
      if( mEffects[z]->IsAddRenderPath() )
      {
        mEffects[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mEffects[z]->Delete();
      GFL_SAFE_DELETE(mEffects[z]);
    }
  }

  for(int z=0;z<MAX_KAWAIGARI_CARE_EFFECT;z++)
  {  
    if(mCareEffects[z]!=NULL)
    {  
      if( mCareEffects[z]->IsAddRenderPath() )
      {
        mCareEffects[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mCareEffects[z]->Delete();
      GFL_SAFE_DELETE(mCareEffects[z]);
    }
  }


  return(result);
}

void KawaigariMain::LoadFoodResource(int n)
{
  if(n==0) // Also load piece resource
  {    
    int arcIdx = CHARA_MODEL_ID_BTPB0001_05 + (mameType*MAX_POKEMAME_VARIATION);

    mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
      ARCID_CHARA_MODEL_BATTLE_NORMAL,
      arcIdx,
      false,
      DemoLib::Obj::RESOURCE_BINPACK,
      false);         
  }

  // Load PokeMame Model:
  {
    n += (mameType*MAX_POKEMAME_VARIATION);
    int arcIdx = CHARA_MODEL_ID_BTPB0001_00 + n;

    mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
      ARCID_CHARA_MODEL_BATTLE_NORMAL,
      arcIdx,
      false,
      DemoLib::Obj::RESOURCE_BINPACK,
      false);     
  }
}

bool KawaigariMain::WaitFoodResource(void)
{
  if(GetAsyncFileManager()->IsAllReqFinished()) 
  {
    mResourceManager->CreateResource(mHeap,GetGLAllocator(),mEffectSystem,mEffectHeap,GetParticleIndexManager());
    return(true);
  }
  return(false);
}

void KawaigariMain::LoadFoodObject(int n)
{
  if(n==0) // Load Piece objects too:
  {
    for(int z=0;z<3;z++)
    {
      int type = mameType;
      int arcid = CHARA_MODEL_ID_BTPB0001_05 + (type*MAX_POKEMAME_VARIATION);
      int drawType = 1;

      mPokeMamePieceModel[z] = GFL_NEW(mHeap)KawaigariObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());

      void *buf = GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,arcid)->GetData(0);

      mPokeMamePieceModel[z]->CreateBuffer(buf,mHeap,GetGLAllocator());

      mPokeMamePieceModel[z]->AddRenderPath(drawType);

      mPokeMamePieceModel[z]->SetVisible(false);
      mPokeMamePieceModel[z]->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);
    }
  }

  n += (mameType*MAX_POKEMAME_VARIATION);

  if(pokeMameLoaded[n]) return; // already loaded

  pokeMameLoaded[n] = true;
  
  int arcid = CHARA_MODEL_ID_BTPB0001_00 + n;

  if(mPokeMameModel[n]!=NULL) return; 

  int drawType = 1;

  mPokeMameModel[n] = GFL_NEW(mHeap)KawaigariObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());

  void *buf = GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,arcid)->GetData(0);

  mPokeMameModel[n]->CreateBuffer(buf,mHeap,GetGLAllocator());

  mPokeMameModel[n]->AddRenderPath(drawType);

  mPokeMameModel[n]->SetVisible(false);
  mPokeMameModel[n]->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);

  // Set to OFFSCREEN:
  mPokeMameModel[n]->SetPosition(0.0f,-1000.0f,0.0f);      

}

void KawaigariMain::ShowFoodObject(int n,bool visible)
{
 /// GFL_PRINT("FoodObject %i: %s\n",n,visible?"ON":"OFF");

  if(n!=-1) n += (mameType*MAX_POKEMAME_VARIATION);

  for(int z=0;z<MAX_POKEMAME_MODEL;z++)
  {
    if(mPokeMameModel[z]==NULL) continue;

    if(n==z || n==-1)
    {
      mPokeMameModel[z]->SetVisible(visible);
    }
    else if(visible) mPokeMameModel[z]->SetVisible(false);

    mPokeMameModel[z]->Update();
  }


}

void KawaigariMain::ShowFoodPieceObject(int n,bool visible)
{
  
  ///n += (mameType*MAX_POKEMAME_PIECE_MODEL_SET);

  for(int z=0;z<3;z++)
  {
    
    if(n==-1 || n==z)
    {
 //     GFL_PRINT("FoodPieceObject %i: %s\n",z,visible?"ON":"OFF");
      if(mPokeMamePieceModel[z]!=NULL)
        mPokeMamePieceModel[z]->SetVisible(visible);
    }    
  }
}

bool KawaigariMain::UnloadFoodObjects(void)
{
  bool result = true;
  for(int z=0;z<MAX_POKEMAME_MODEL;z++)
  {  
    if(mPokeMameModel[z]!=NULL)
    {  
      if( mPokeMameModel[z]->IsAddRenderPath() )
      {
        mPokeMameModel[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mPokeMameModel[z]->Delete();
      GFL_SAFE_DELETE(mPokeMameModel[z]);
      pokeMameLoaded[z] = false;
    }
  }

  for(int z=0;z<3;z++)
  {  
    if(mPokeMamePieceModel[z]!=NULL)
    {  
      if( mPokeMamePieceModel[z]->IsAddRenderPath() )
      {
        mPokeMamePieceModel[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mPokeMamePieceModel[z]->Delete();
      GFL_SAFE_DELETE(mPokeMamePieceModel[z]);
    }
  }

  if(!result) return(false);

  for(int z=0;z<6;z++)
  {  
    int arcIdx = CHARA_MODEL_ID_BTPB0001_00 + (mameType*MAX_POKEMAME_VARIATION) + z;
    mResourceManager->DeleteResource(ARCID_CHARA_MODEL_BATTLE_NORMAL,arcIdx);
  }
    
  return(true);
}

bool KawaigariMain::LoadPokeMame(void)
{    
  return(true);
  /*
  switch(mPokeMameLoadSeq)
  {
    case 0:
      // Poke Mame Model: (1/3)
      {        
        for(int n=0;n<5*6;n++)
        {
          int arcIdx = CHARA_MODEL_ID_BTPB0001_00 + n;               
          mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
            ARCID_CHARA_MODEL_BATTLE_NORMAL,
            arcIdx,
            false,
            DemoLib::Obj::RESOURCE_BINPACK,
            false);         
        }
        mPokeMameLoadSeq++;
      }
      break;

    case 1:
      if(GetAsyncFileManager()->IsAllReqFinished()) mPokeMameLoadSeq++;
      break;

    case 2:
      // Poke Mame Model: (2/3)
      {        
        for(int n=5*6;n<10*6;n++)
        {
          int arcIdx = CHARA_MODEL_ID_BTPB0001_00 + n;                   
          mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
            ARCID_CHARA_MODEL_BATTLE_NORMAL,
            arcIdx,
            false,
            DemoLib::Obj::RESOURCE_BINPACK,
            false);         
        }
        mPokeMameLoadSeq++;
      }
      break;

    case 3:
      if(GetAsyncFileManager()->IsAllReqFinished()) mPokeMameLoadSeq++;
      break;

    case 4:
      // Poke Mame Model: (3/3)
      {        
        for(int n=10*6;n<15*6;n++)
        {
          int arcIdx = CHARA_MODEL_ID_BTPB0001_00 + n;                   
          mResourceManager->LoadRequest(mHeap,mHeap->GetLowerHandle(),
            ARCID_CHARA_MODEL_BATTLE_NORMAL,
            arcIdx,
            false,
            DemoLib::Obj::RESOURCE_BINPACK,
            false);         
        }
        mPokeMameLoadSeq++;
      }
      break;

    case 5:
      if(GetAsyncFileManager()->IsAllReqFinished()) return(true);
      break;
  }

  return(false);
  */
}

void KawaigariMain::CreatePokeMame(void)
{    
  /*
  for(int n=0;n<MAX_POKEMAME_MODEL;n++)
  {
    int arcid = CHARA_MODEL_ID_BTPB0001_00 + n;
    if(n && n%5==0) continue; // Don't create piece models..

    int drawType = 0;

    mPokeMameModel[n] = GFL_NEW(mHeap)KawaigariObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());
        
    void *buf = GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,arcid)->GetData(0);

    mPokeMameModel[n]->CreateBuffer(buf,mHeap,GetGLAllocator());

    mPokeMameModel[n]->AddRenderPath(drawType);

    mPokeMameModel[n]->SetVisible(false);
    mPokeMameModel[n]->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);
  }
  */
  /*
  for(int n=0;n<MAX_POKEMAME_PIECE_MODEL;n++)
  {
    int type = n/MAX_POKEMAME_PIECE_MODEL_SET;
    int arcid = CHARA_MODEL_ID_BTPB0001_05 + (type*MAX_POKEMAME_VARIATION);
    int drawType = 0;

    mPokeMamePieceModel[n] = GFL_NEW(mHeap)KawaigariObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());

    void *buf = GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,arcid)->GetData(0);

    mPokeMamePieceModel[n]->CreateBuffer(buf,mHeap,GetGLAllocator());

    mPokeMamePieceModel[n]->AddRenderPath(drawType);

    mPokeMamePieceModel[n]->SetVisible(false);
    mPokeMamePieceModel[n]->GetBaseModel()->GetModelInstanceNode()->SetSortPriority(192);
  }
  */
}

bool KawaigariMain::UnloadPokeMame(void)
{
  bool result = true;

  for(int z=0;z<MAX_POKEMAME_MODEL;z++)
  {  
    if(mPokeMameModel[z]!=NULL)
    {  
      if( mPokeMameModel[z]->IsAddRenderPath() )
      {
        mPokeMameModel[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mPokeMameModel[z]->Delete();
      GFL_SAFE_DELETE(mPokeMameModel[z]);
    }
  }

  for(int z=0;z<MAX_POKEMAME_PIECE_MODEL;z++)
  {  
    if(mPokeMamePieceModel[z]!=NULL)
    {  
      if( mPokeMamePieceModel[z]->IsAddRenderPath() )
      {
        mPokeMamePieceModel[z]->RemoveRenderPath();
        result = false;
        continue;
      }

      mPokeMamePieceModel[z]->Delete();
      GFL_SAFE_DELETE(mPokeMamePieceModel[z]);
    }
  }

  return(result);
}

int KawaigariMain::ConvertCareModeJointToPokeJoint(int joint,int &index)
{
  switch(joint)
  {
    case 0: return(0); // INVALID!
    case 1: return(PokeTool::MODEL_NODE_CENTER); // WAIST
    case 2: return(PokeTool::MODEL_NODE_HEAD); // HEAD
    case 3: return(PokeTool::MODEL_NODE_MOUTH); // MOUTH
    case 4: return(PokeTool::MODEL_NODE_HAND); // HAND

    case 5: return(PokeTool::MODEL_NODE_FOOT); // FOOT
    case 6: index = 1; return(PokeTool::MODEL_NODE_FOOT); // FOOT2

    case 7: index = 1; return(PokeTool::MODEL_NODE_HAND); // HAND2

    case 8: return(PokeTool::MODEL_NODE_TAIL); // TAIL
    case 9: index = 1; return(PokeTool::MODEL_NODE_TAIL); // TAIL2

  }

  return(PokeTool::MODEL_NODE_CENTER);
}

void KawaigariMain::Get3DLookPositionByXY(gfl2::math::Vector3 *pos, u32 touchx, u32 touchy)
{
  if(pos==NULL) return;

  // Calculate Area Around Camera Position:
  gfl2::math::Vector3 cam_pos,target_pos,up_pos,dist_vec;
  GetCamera()->GetPosition(cam_pos,target_pos,up_pos);

  dist_vec = cam_pos - target_pos;
  
  float DISTADJUST = 200.0f;

  dist_vec = dist_vec.Normalize();
  dist_vec *= DISTADJUST * mModel->GetAdjustScale();

  float dist = DISTADJUST * mModel->GetAdjustScale();

  *pos = target_pos + dist_vec;          

  // Use Smooth SIN Curve with MAX = DIST*SIN
  float YANG = 15.0f;
  float XANG = 90.0f;

  float minX = pos->x - (gfl2::math::SinDeg(XANG)*dist);
  float maxX = pos->x + (gfl2::math::SinDeg(XANG)*dist);
  float tx = (float)(touchx / 319.0f); // 0 ~ 1.0f 
  pos->x = minX + (tx *(maxX-minX) );

  float minY = pos->y - (gfl2::math::SinDeg(YANG)*dist);
  float maxY = pos->y + (gfl2::math::SinDeg(YANG)*dist);
  float ty = (float)( (239 - touchy) / 239.0f); // 0 ~ 1.0f
  pos->y = minY + (ty *(maxY-minY) );

}

void KawaigariMain::Get3DTouchPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy, const s32 type )
{
  // Use New Calculation?
  if(type==TPT_LOOK)
  {  
    switch(pokemonAI.GetMonsNo())
    {

      case MONSNO_ZYAROODA: // (497)
      case MONSNO_TUTAAZYA: // (495)
      case MONSNO_KEIKOUO: // (456)
      case MONSNO_NEORANTO: // (457)
      case MONSNO_TODOZERUGA: // (365)
      case MONSNO_EAAMUDO: // (227)
      case MONSNO_TEPPOUO: // (223)
      case MONSNO_KYATAPII: // (10)
        Get3DLookPositionByXY(pos,touchx,touchy);
        return;

      case MONSNO_NASSII: //  (103)
        if(pokemonAI.GetFormNo()==FORMNO_NASSII_ALOLA)
        {
          Get3DLookPositionByXY(pos,touchx,touchy);
          return;
        }
        break;
    }
    
  }

  // Ported From SANGO...
  
  ///f32 scr_x;
  if( TPT_PARTICLE == type && cameraSystem.GetDualScreenMonsterCameraMode() && !cameraSystem.CheckCameraType(KG_CAMERA_FAR))
  {
  /// cam = gra->GetCameraUpper( ); ???
  ///  scr_x = 200.0f;
////    touchx += 40;
  } 
  else 
  {
  ///  cam = gra->GetCamera( ); ???
  ///  scr_x = 160.0f;
  }
  gfl2::math::Matrix34 view_matrix;
  gfl2::math::Matrix44 projection_matrix;

  
  gfl2::math::Vector3 cam_pos,target_pos,up_pos;

  if(type==TPT_MAME || type==TPT_MAMESHAKE)
  {
    mRenderSys->GetMameViewAndProjectionMatrix(view_matrix, projection_matrix);

    cameraSystem.SaveCurrentCamera();
    cameraSystem.SetLocationDirect(KG_CAMERA_EAT_FIXED);
    cameraSystem.GetPosition(cam_pos,target_pos,up_pos);
    cameraSystem.RestoreCurrentCamera();

    cam_pos *= 1.65f;
    target_pos *= 1.65f;
  }
  else
  {
    mRenderSys->GetViewAndProjectionMatrix(view_matrix, projection_matrix);
    cameraSystem.GetPosition(cam_pos,target_pos,up_pos);

    cam_pos *= mModel->GetAdjustScale();
    target_pos *= mModel->GetAdjustScale();
  }
  
  
  
  
 

  gfl2::math::Vector4 tri_[3], cam_pos4( cam_pos ), hit;

  gfl2::math::Vector4 tri[3];
 
  f32 z = (type == TPT_FOOD)?dataManager.GetFoodDist():((type == TPT_LOOK)?100.0f:80.0f);

  float zRANGEFOOD = dataManager.GetFoodDist(); // 130.0f by default
  ///static float zRANGELOOK = 20.0f;
  float zRANGELOOK = 100.0f; 

  if(type==TPT_FOOD || type==TPT_MAME) z = zRANGEFOOD;
  else if(type==TPT_LOOK) z = zRANGELOOK;
  else if(type==TPT_MAMESHAKE)     
  {
      z=zRANGEFOOD - 15.0f;
  }
  else if(type==TPT_PARTICLE) z = 80.0f;
  else if(type==TPT_MAMELOOK) z = zRANGEFOOD;
  else z = 200.0f;

  // EXCEPTION FOR MONSNO MONSNO_RESIRAMU(643) ADJUST LOOK:
  if(type==TPT_LOOK && pokemonAI.IsCharacterID(MONSNO_RESIRAMU)) z = -25.0f;  
   
  gfl2::math::Matrix34 camMat; 

  if(type==TPT_MAME || type==TPT_MAMESHAKE)
  {
    mRenderSys->GetMameViewAndProjectionMatrix(camMat, projection_matrix);
  }
  else
  {
    mRenderSys->GetViewAndProjectionMatrix(camMat, projection_matrix);
  }
    
  /// mat.Trans( &tri[0], 0.0f, METER( 20.0f ), z );
  {  
    gfl2::math::Vector4 trans(0.0f, METER( 20.0f ), z);    
 
    tri[0] = camMat.TransformVec(trans); 
    tri[0].w = 0.0f;

    //GFL_PRINT( "Tri0: %f %f %f  Z:%f\n",tri[0].x,tri[0].y,tri[0].z,z);

  }
 
  /// mat.Trans( &tri[1], METER( 20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( 20.0f ), METER( -10.0f ), z);    
    
    tri[1] = camMat.TransformVec(trans);   
    tri[1].w = 0.0f;

    ///GFL_PRINT( "Tri1: %f %f %f  Z:%f\n",tri[1].x,tri[1].y,tri[1].z,z);

  }

  ///mat.Trans( &tri[2], METER( -20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( -20.0f ), METER( -10.0f ), z);    
    
    tri[2] = camMat.TransformVec(trans);        
    tri[2].w = 0.0f;

    ///GFL_PRINT( "Tri2: %f %f %f  Z:%f\n",tri[2].x,tri[2].y,tri[2].z,z);

  }

  tri_[0].Set( tri[0].x, tri[0].y, -tri[0].z, 0.0f );
  tri_[1].Set( tri[1].x, tri[1].y, -tri[1].z, 0.0f );
  tri_[2].Set( tri[2].y, tri[2].y, -tri[2].z, 0.0f );

  const f32 tx = ( f32 )touchx;
  const f32 ty = ( f32 )touchy;

  //		gfl::core::Debug::PrintConsole( "x %f y %f => ", tx, ty );

  //f32 y = ( tx - scr_x ) / -scr_x;
  f32 y = ( ty - 160.0f ) / -160.0f;
  f32 x = ( tx - 120.0f ) / -120.0f;

  //		gfl::core::Debug::PrintConsole( "x %f y %f\n", x, y );

  y = -(ty - 120.0f) / 120.0f;
  x = (tx - 160.0f) / 160.0f;
  //GFL_PRINT("*Float XY: %f,%f\n",x,y);

  gfl2::math::Vector4 vv = gfl2::math::MathUtil::CalculateNearPlaneVectorPosition( view_matrix, projection_matrix, x, y );
  vv *= METER( 100.0f );
   
  if( gfl2::math::MathUtil::RayPickPolygon( &hit, cam_pos4, vv, tri_[0], tri_[1], tri_[2] ) )
  {    
    pos->Set( hit.x,hit.y,hit.z );    
  } 
  else 
  {    
    if(type==TPT_PARTICLE)
    {
       Get3DTouchPositionByXY(pos,touchx,touchy,TPT_PARTICLE_FAR);
       return;
    }
    pos->Set(1.0f,1.0f,1.0f);
  }
  
}

void KawaigariMain::Get3DEffectTouchPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy)
{
  gfl2::math::Matrix34 view_matrix;
  gfl2::math::Matrix44 projection_matrix;

  mRenderSys->GetEffectViewAndProjectionMatrix(view_matrix, projection_matrix);

  gfl2::math::Vector3 cam_pos,target_pos;

//  cameraSystem.GetPosition(cam_pos,target_pos,up_pos);

  cam_pos.x = 0.0f;
  cam_pos.y = 36.0f;
  cam_pos.z = 144.0f;

  target_pos.x = 0.0f;
  target_pos.y = 24.0f;
  target_pos.z = 0.0f;    

  cam_pos *= 1.65f;
  target_pos *= 1.65f;

  gfl2::math::Vector4 tri_[3], cam_pos4( cam_pos ), hit;

  gfl2::math::Vector4 tri[3];

  f32 z = 200.0f;

  gfl2::math::Matrix34 camMat; 

  mRenderSys->GetEffectViewAndProjectionMatrix(camMat, projection_matrix);

  /// mat.Trans( &tri[0], 0.0f, METER( 20.0f ), z );
  {  
    gfl2::math::Vector4 trans(0.0f, METER( 20.0f ), z);    

    tri[0] = camMat.TransformVec(trans); 
    tri[0].w = 0.0f;

    //GFL_PRINT( "Tri0: %f %f %f  Z:%f\n",tri[0].x,tri[0].y,tri[0].z,z);

  }

  /// mat.Trans( &tri[1], METER( 20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( 20.0f ), METER( -10.0f ), z);    

    tri[1] = camMat.TransformVec(trans);   
    tri[1].w = 0.0f;

    ///GFL_PRINT( "Tri1: %f %f %f  Z:%f\n",tri[1].x,tri[1].y,tri[1].z,z);

  }

  ///mat.Trans( &tri[2], METER( -20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( -20.0f ), METER( -10.0f ), z);    

    tri[2] = camMat.TransformVec(trans);        
    tri[2].w = 0.0f;

    ///GFL_PRINT( "Tri2: %f %f %f  Z:%f\n",tri[2].x,tri[2].y,tri[2].z,z);

  }

  tri_[0].Set( tri[0].x, tri[0].y, -tri[0].z, 0.0f );
  tri_[1].Set( tri[1].x, tri[1].y, -tri[1].z, 0.0f );
  tri_[2].Set( tri[2].y, tri[2].y, -tri[2].z, 0.0f );

  const f32 tx = ( f32 )touchx;
  const f32 ty = ( f32 )touchy;

  //		gfl::core::Debug::PrintConsole( "x %f y %f => ", tx, ty );

  //f32 y = ( tx - scr_x ) / -scr_x;
  f32 y = ( ty - 160.0f ) / -160.0f;
  f32 x = ( tx - 120.0f ) / -120.0f;

  //		gfl::core::Debug::PrintConsole( "x %f y %f\n", x, y );

  y = -(ty - 120.0f) / 120.0f;
  x = (tx - 160.0f) / 160.0f;
  //GFL_PRINT("*Float XY: %f,%f\n",x,y);

  gfl2::math::Vector4 vv = gfl2::math::MathUtil::CalculateNearPlaneVectorPosition( view_matrix, projection_matrix, x, y );
  vv *= METER( 100.0f );

  if( gfl2::math::MathUtil::RayPickPolygon( &hit, cam_pos4, vv, tri_[0], tri_[1], tri_[2] ) )
  {    
    pos->Set( hit.x,hit.y,hit.z );    
  } 
  else 
  {    
    pos->Set(1.0f,1.0f,1.0f);
  }

}


void KawaigariMain::Get3DEffectTouchPositionByXY_Top( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy)
{
  gfl2::math::Matrix34 view_matrix;
  gfl2::math::Matrix44 projection_matrix;

  mRenderSys->GetEffectViewAndProjectionMatrix(view_matrix, projection_matrix,1);

  gfl2::math::Vector3 cam_pos,target_pos;

  //  cameraSystem.GetPosition(cam_pos,target_pos,up_pos);

  cam_pos.x = 0.0f;
  cam_pos.y = 36.0f;
  cam_pos.z = 144.0f;

  target_pos.x = 0.0f;
  target_pos.y = 24.0f;
  target_pos.z = 0.0f;    

  cam_pos *= 1.65f;
  target_pos *= 1.65f;

  gfl2::math::Vector4 tri_[3], cam_pos4( cam_pos ), hit;

  gfl2::math::Vector4 tri[3];
  
  f32 z = 300.0f;

  gfl2::math::Matrix34 camMat; 

  mRenderSys->GetEffectViewAndProjectionMatrix(camMat, projection_matrix,1);

  /// mat.Trans( &tri[0], 0.0f, METER( 20.0f ), z );
  {  
    gfl2::math::Vector4 trans(0.0f, METER( 20.0f ), z);    

    tri[0] = camMat.TransformVec(trans); 
    tri[0].w = 0.0f;

    //GFL_PRINT( "Tri0: %f %f %f  Z:%f\n",tri[0].x,tri[0].y,tri[0].z,z);

  }

  /// mat.Trans( &tri[1], METER( 20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( 20.0f ), METER( -10.0f ), z);    

    tri[1] = camMat.TransformVec(trans);   
    tri[1].w = 0.0f;

    ///GFL_PRINT( "Tri1: %f %f %f  Z:%f\n",tri[1].x,tri[1].y,tri[1].z,z);

  }

  ///mat.Trans( &tri[2], METER( -20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( -20.0f ), METER( -10.0f ), z);    

    tri[2] = camMat.TransformVec(trans);        
    tri[2].w = 0.0f;

    ///GFL_PRINT( "Tri2: %f %f %f  Z:%f\n",tri[2].x,tri[2].y,tri[2].z,z);

  }

  tri_[0].Set( tri[0].x, tri[0].y, -tri[0].z, 0.0f );
  tri_[1].Set( tri[1].x, tri[1].y, -tri[1].z, 0.0f );
  tri_[2].Set( tri[2].y, tri[2].y, -tri[2].z, 0.0f );

  const f32 tx = ( f32 )touchx;
  const f32 ty = ( f32 )touchy;

  //		gfl::core::Debug::PrintConsole( "x %f y %f => ", tx, ty );

  //f32 y = ( tx - scr_x ) / -scr_x;
  f32 y = ( ty - 160.0f ) / -160.0f;
  f32 x = ( tx - 120.0f ) / -120.0f;

  //		gfl::core::Debug::PrintConsole( "x %f y %f\n", x, y );

  y = -(ty - 120.0f) / 120.0f;
  x = (tx - 160.0f) / 160.0f;
  //GFL_PRINT("*Float XY: %f,%f\n",x,y);

  gfl2::math::Vector4 vv = gfl2::math::MathUtil::CalculateNearPlaneVectorPosition( view_matrix, projection_matrix, x, y );
  vv *= METER( 100.0f );

  if( gfl2::math::MathUtil::RayPickPolygon( &hit, cam_pos4, vv, tri_[0], tri_[1], tri_[2] ) )
  {    
    pos->Set( hit.x,hit.y,hit.z );    
  } 
  else 
  {    
    pos->Set(1.0f,1.0f,1.0f);
  }

}


void KawaigariMain::Get3DEffectPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy,float zPos)
{
  // Ported From SANGO...

  ///f32 scr_x = 160.0f;
  
  gfl2::math::Matrix34 view_matrix;
  gfl2::math::Matrix44 projection_matrix;

  // FIX for Upper Camera MODE (LATER):
  mRenderSys->GetViewAndProjectionMatrix(view_matrix, projection_matrix);

  gfl2::math::Vector3 cam_pos,target_pos,up_pos;

  cameraSystem.GetPosition(cam_pos,target_pos,up_pos);

  cam_pos *= mModel->GetAdjustScale();
  target_pos *= mModel->GetAdjustScale();

  gfl2::math::Vector4 tri_[3], cam_pos4( cam_pos ), hit;

  gfl2::math::Vector4 tri[3];

  f32 z = zPos;
  
  gfl2::math::Matrix34 camMat; 
  mRenderSys->GetViewAndProjectionMatrix(camMat, projection_matrix);

  /// mat.Trans( &tri[0], 0.0f, METER( 20.0f ), z );
  {  
    gfl2::math::Vector4 trans(0.0f, METER( 20.0f ), z);    

    tri[0] = camMat.TransformVec(trans); 
    tri[0].w = 0.0f;

    //GFL_PRINT( "Tri0: %f %f %f  Z:%f\n",tri[0].x,tri[0].y,tri[0].z,z);

  }

  /// mat.Trans( &tri[1], METER( 20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( 20.0f ), METER( -10.0f ), z);    

    tri[1] = camMat.TransformVec(trans);   
    tri[1].w = 0.0f;

    ///GFL_PRINT( "Tri1: %f %f %f  Z:%f\n",tri[1].x,tri[1].y,tri[1].z,z);

  }

  ///mat.Trans( &tri[2], METER( -20.0f ), METER( -10.0f ), z );
  {  
    gfl2::math::Vector4 trans(METER( -20.0f ), METER( -10.0f ), z);    

    tri[2] = camMat.TransformVec(trans);        
    tri[2].w = 0.0f;

    ///GFL_PRINT( "Tri2: %f %f %f  Z:%f\n",tri[2].x,tri[2].y,tri[2].z,z);

  }

  tri_[0].Set( tri[0].x, tri[0].y, -tri[0].z, 0.0f );
  tri_[1].Set( tri[1].x, tri[1].y, -tri[1].z, 0.0f );
  tri_[2].Set( tri[2].y, tri[2].y, -tri[2].z, 0.0f );

  const f32 tx = ( f32 )touchx;
  const f32 ty = ( f32 )touchy;

  //		gfl::core::Debug::PrintConsole( "x %f y %f => ", tx, ty );

  //f32 y = ( tx - scr_x ) / -scr_x;
  f32 y = ( ty - 160.0f ) / -160.0f;
  f32 x = ( tx - 120.0f ) / -120.0f;

  //		gfl::core::Debug::PrintConsole( "x %f y %f\n", x, y );

  y = -(ty - 120.0f) / 120.0f;
  x = (tx - 160.0f) / 160.0f;
  //GFL_PRINT("*Float XY: %f,%f\n",x,y);

  gfl2::math::Vector4 vv = gfl2::math::MathUtil::CalculateNearPlaneVectorPosition( view_matrix, projection_matrix, x, y );
  vv *= METER( 100.0f );

  if( gfl2::math::MathUtil::RayPickPolygon( &hit, cam_pos4, vv, tri_[0], tri_[1], tri_[2] ) )
  {    
    pos->Set( hit.x,hit.y,hit.z );    
  } 
  else 
  {       
    pos->Set(1.0f,1.0f,1.0f);
  }

}

#if PM_DEBUG
// ------------------------------------------
// Select menu debug functions
// ------------------------------------------
void KawaigariMain::InitSelectMenuDebug( gfl2::heap::HeapBase *pHeap )
{

  mDebug_BadStatus = 0;
  mDebug_DirtType = 0;
  mDebug_DirtPos = 0;

  GetKawaigariParam();

  // debug_menuの文字列データ
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  mpKawaigariTop = gfl2::debug::DebugWin_AddGroup(pHeap,pMsgData, msg_dmenu_kawaigari_menuitem_17, NULL);

  gfl2::debug::DebugWinGroup *kawaParam = gfl2::debug::DebugWin_AddGroup(pHeap,pMsgData, msg_dmenu_kawaigari_menuitem_6, mpKawaigariTop );
  gfl2::debug::DebugWin_AddItemValueU8( kawaParam ,pHeap, pMsgData,msg_dmenu_kawaigari_menuitem_7, &mDebug_Nakayosi, 0, 255  );
  gfl2::debug::DebugWin_AddItemValueU8( kawaParam ,pHeap, pMsgData,msg_dmenu_kawaigari_menuitem_8, &mDebug_Manpuku, 0, 255  );
  gfl2::debug::DebugWin_AddItemValueU8( kawaParam ,pHeap, pMsgData,msg_dmenu_kawaigari_menuitem_9, &mDebug_Nadenade, 0, 255  );
  gfl2::debug::DebugWin_AddItemUpdateFunc( kawaParam, pHeap,L"Set",this,KawaigariMain::SelectMenuFookFunc_SetKawaigariParam );
  gfl2::debug::DebugWin_AddItemUpdateFunc( kawaParam, pHeap,L"Get",this,KawaigariMain::SelectMenuFookFunc_GetKawaigariParam );

#if defined(GF_PLATFORM_WIN32)
  gfl2::debug::DebugWinGroup *kawaStatus = gfl2::debug::DebugWin_AddGroup( pHeap,pMsgData, msg_dmenu_kawaigari_menuitem_11, mpKawaigariTop );
  gfl2::debug::DebugWin_AddItemValueIntMsg( kawaStatus ,pHeap, pMsgData, msg_dmenu_kawaigari_menuitem_11, &mDebug_BadStatus, print::GetMessageArcId_Debug(), GARC_debug_message_debug_menu_DAT, 0, 4, msg_dmenu_kawaigari_bad_status_2 );
  gfl2::debug::DebugWin_AddItemUpdateFunc( kawaStatus, pHeap,L"Set",this,KawaigariMain::SelectMenuFookFunc_SetBadStatus );
#endif


  gfl2::debug::DebugWin_AddItemUpdateFunc( mpKawaigariTop,pHeap,pMsgData, msg_dmenu_kawaigari_menuitem_19, this,KawaigariMain::SelectMenuFookFunc_MameMax );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mpKawaigariTop,pHeap, L"PlayAllAnimation", this,KawaigariMain::SelectMenuFookFunc_PlayAllAnimation );

}

void KawaigariMain::FinalizeSelectMenuDebug(void)
{
  gfl2::debug::DebugWin_RemoveGroup( mpKawaigariTop );
}

void KawaigariMain::PlayAllAnimation(void)
{
  pokemonAI.SetState(AI_STATE_PLAY_ALL_MOTION);
}

void KawaigariMain::SetPokeMameMax(void)
{
  Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

  for (int i=Savedata::ResortSave::POKEBEANS_NORMAL_01; i < Savedata::ResortSave::POKEBEANS_KIND_MAX ;i++)
    {
      save->SetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)i ,255);
    }

  KIJIMA_PRINT("Set Mame Max.\n");
}

void KawaigariMain::SetBadStatus(void)
{
  int BAD_STATUS[] = 
    {
      CARETROUBLE_PARALYSIS,
      CARETROUBLE_SLEEPY,
      CARETROUBLE_ICE,
      CARETROUBLE_FIRE,
      CARETROUBLE_POISON,
    };
  
  mTroubleManager.AddObject( BAD_STATUS[mDebug_BadStatus] );
 
  KIJIMA_PRINT("Set Bad Status[%d]\n", mDebug_BadStatus);

}

int DIRT_STATUS[] = {
  CARETROUBLE_NONE,
  CARETROUBLE_DIRT,
  CARETROUBLE_DUST,
  CARETROUBLE_HAIR,
  CARETROUBLE_WATER,
};

void KawaigariMain::SetDirt(void)
{

  if( mDebug_DirtType > 0 )
    {
      mTroubleManager.AddObject( DIRT_STATUS[mDebug_DirtType], mDebug_DirtPos );
    }

  KIJIMA_PRINT("Set Dirt\n");

}

void KawaigariMain::SetDirtAll(void)
{

  int dt_type = DIRT_STATUS[mDebug_DirtType];

  for(int i=0; i<5;i++)
    {
      mTroubleManager.AddObject( dt_type, i );
    }

  KIJIMA_PRINT("Set Dirt ALL\n");
}

void KawaigariMain::SetKawaigariParam(void)
{

  u8 feed,affection,playful;
  
  affection =  mDebug_Nakayosi;
  feed = mDebug_Manpuku;
  playful = mDebug_Nadenade;
  
  pokemonAI.SetParameters(affection,playful,feed);

  KIJIMA_PRINT("Set Kawaigari Param\n");

}
void KawaigariMain::GetKawaigariParam(void)
{
  u8 feed,affection,playful;
  pokemonAI.GetParameters(affection,playful,feed);

  mDebug_Nakayosi = affection;
  mDebug_Manpuku = feed;
  mDebug_Nadenade = playful;

  KIJIMA_PRINT("Get Kawaigari Param\n");
 
}

// --------------------------
// For debug call back
void KawaigariMain::SelectMenuFookFunc_PlayAllAnimation( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->PlayAllAnimation();

  }
}

void KawaigariMain::SelectMenuFookFunc_MameMax( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->SetPokeMameMax();
    
  }
}

void KawaigariMain::SelectMenuFookFunc_SetKawaigariParam( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->SetKawaigariParam();
  }
}
void KawaigariMain::SelectMenuFookFunc_GetKawaigariParam( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->GetKawaigariParam();
  }
}

void KawaigariMain::SelectMenuFookFunc_SetBadStatus( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->SetBadStatus();
  }
}

void KawaigariMain::SelectMenuFookFunc_SetDirt( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->SetDirt();
  }
}
void KawaigariMain::SelectMenuFookFunc_SetDirtAll( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    ((KawaigariMain*)userWork)->SetDirtAll();
  }
}

#endif // PM_DEBUG

GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)
