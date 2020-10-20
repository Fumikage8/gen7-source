//======================================================================
/**
 * @file    ResortProc.cpp
 * @brief   15/05/24
 * @author  k.ohno
 * @data    描画テスト  SetBillbordRotation
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

#include "System/include/nijiAllocator.h"

// poke_3dのインクルード
#include <model/include/gfl2_BaseModel.h>

#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/TimeZone.h>
#include <Sound/include/Sound.h>

#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>

#include "App/Resort/include/ResortProc.h"
#include "ResortIslandDraw.h"
#include "ResortDrawControl.h"
#include "ResortInitLogic.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/resort.gaix>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include "AppLib/include/Fade/DemoFade.h"
#include "Savedata/include/ResortSave.h"
#include "Savedata/include/EventWork.h"

#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"

#include "ResortFrame.h"
#include "ResortDebugWin.h"

#include <Fade/include/gfl2_FadeManager.h>

#include <thread/include/gfl2_ThreadStatic.h>

#include <model/include/gfl2_BaseCamera.h>

#include "AppLib/include/Tool/PokeCapAccesser.h"

#include "niji_conv_header\app\island\island_pack.h"

#include "PokeTool/include/KawaigariParamCareCoreManager.h"

#include "App/Resort/include/ResortHeapDefine.h"
#include "App/AppEvent/include/PokeIsland/ResortDataManager.h"

#include "ResortDebugInfo.h"


#define NIJI_PROJECT_APP_RESORT_USE_DEBUG_WIN (PM_DEBUG&&(1))


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

enum{
  GROUND_FILE_READ_,
  GROUND_FILE_WAIT_,
  GROUND_FILE_WAIT2_,
  GROUND_FILE_WAIT3_,
  KAWAIGARI_WAIT_,
  SKYBOX_WAIT_,
  SOUND_WAIT_,
  POKEMODEL_INIT_,
  POKEMODEL_WAIT_,
  BILLBOARD_WAIT_,
};


enum{
  _FILEREAD_END,
  _FILEREAD_START,

  _CHANGE_INIT,
  _CHANGE_DEL,
  _CHANGE_GO,
	_CHANGE_RIN,
  _CHANGE_END,
  

};

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_START,
  LOCAL_CLOSE_WAIT,
  LOCAL_CLOSE_END,
};


static ResortDataManager s_resortDataManager;


ResortProc::ResortProc()
  : m_islandGroundLvPackLoadSeq(0),
    m_islandGroundLvPackDatId(gfl2::fs::ArcFile::ARCDATID_NULL),
    m_islandGroundLvPackBuf(NULL)
#if PM_DEBUG
    , mResortDebugWin(NULL)
    , m_pLocalDebugDeviceHeap(NULL)
#endif
    , m_pIslandGroundLvDevHeap(NULL)
    , mpRootHeap(NULL)
    , mpAppHeap(NULL)
    , mpLogicHeap(NULL)
    , mpTempHeap(NULL)
    , mpPokeModelHeap(NULL)
    , mpPokeModelNijiAllocator(NULL)
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.Init();
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_CONSTRUCTOR_START);
#endif

  {
    // CreateはPokeIslandEventかResortProcで行う。
    // UpdateはPokeIslandEventで行う。
    // DestroyはResortProcで行う。

    if( ResortDataManager::GetStaticInstance() == NULL )
    {
      ResortDataManager::SetStaticInstance(&s_resortDataManager);
      ResortDataManager::GetStaticInstance()->Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
    }
  }

  m_Seq = 0;
  m_asyncCreateDestroyPokeModelStep=0;
  island_camera_rotate = -0.31f;

  m_PokeDeleteStep=0;
  BILLBOARD_COMP=40.0f;
  mChangeStep = _CHANGE_END;
  m_bMapChangeWait=false;
}

ResortProc::~ResortProc(void)
{
  {
    ResortDataManager::GetStaticInstance()->Destroy();
    ResortDataManager::UnsetStaticInstance();
  }
}

void ResortProc::InitLayout(void)
{
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
  mpManagerHeap = GFL_CREATE_LOCAL_HEAP( devHeap, ResortHeapDefine::FRAME_MANAGER_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_03, mpManagerHeap, "FRAME_MANAGER_SIZE");
#endif

  // フェード用も別
  //mpFadeHeap = GFL_CREATE_LOCAL_HEAP( devHeap, 0x50000 + 0x18E000 + 0x2000, gfl2::heap::HEAP_TYPE_EXP, false );

  //フレームマネージャー
  mpFrameManager = GFL_NEW(mpManagerHeap) applib::frame::Manager(mpManagerHeap);

  mpAppParam->mpAppRenderingManager = m_appRenderingManager;

  //起動フレーム作成
  m_pResortFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, ResortFrame>( mpFrameManager );
  //セットアップ
  m_pResortFrame->Setup( mpAppParam, mpRootHeap );

  m_pResortFrame->SetResortChangeIslandListener(this);
  m_pResortFrame->SetResortSequenceListener(this);

  m_pResortFrame->SetLogic( mpLogic );
  
  mpResortIslandDraw->SetListener( m_pResortFrame );


}

void ResortProc::DeleteLayout(void)
{
  GFL_SAFE_DELETE(mpFrameManager);
//  gfl2::heap::Manager::DeleteHeap(mpFadeHeap);
  gfl2::heap::Manager::DeleteHeap(mpManagerHeap);

}



//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result ResortProc::InitFunc(gfl2::proc::Manager* pManager)
{
  switch(m_Seq){
  case GROUND_FILE_READ_:
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_00);
#endif

#if defined(GF_PLATFORM_CTR)
      OHNO_PRINT("ResortProc::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif

#if PM_DEBUG
      gfl2::heap::HeapBase *debugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      m_pLocalDebugDeviceHeap = GFL_CREATE_LOCAL_HEAP(debugHeap, ResortHeapDefine::DEBUG_WIN_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_04, m_pLocalDebugDeviceHeap, "DEBUG_WIN_SIZE");
#endif

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_DEBUG_WIN_START);
#endif
#if NIJI_PROJECT_APP_RESORT_USE_DEBUG_WIN
      mResortDebugWin = GFL_NEW(m_pLocalDebugDeviceHeap) ResortDebugWin(m_pLocalDebugDeviceHeap, NULL);
#endif
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_DEBUG_WIN_END);
#endif
#endif

      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      //m_pLocalDeviceHeap = GFL_CREATE_LOCAL_HEAP(devHeap, 0x190000, gfl2::heap::HEAP_TYPE_EXP, false );
      m_pLocalDeviceHeap = NULL;  // 使っていない

      m_pIslandGroundLvDevHeap = GFL_CREATE_LOCAL_HEAP(devHeap, ResortHeapDefine::ISLANDGROUND_LV_FILE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_07, m_pIslandGroundLvDevHeap, "ISLANDGROUND_LV_FILE_SIZE");
#endif



      // レンダリングマネージャ
      m_appRenderingManager = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::util::AppRenderingManager();

      m_pNijiAllocator = GFL_NEW( mpAppHeap->GetDeviceHeap() ) System::nijiAllocator( mpAppHeap->GetDeviceHeap() );


      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder =
        app::util::AppRenderingManager::RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1;
      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].lytWkMax = 32;
      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].modelNumMax = 8;


      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder =
        app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT;
      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].pokeModelNumMax = 2;
      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].modelNumMax = 64;
      m_Description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].lytWkMax = 32;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_RENDERING_MANAGER_CREATE_START);
#endif
      m_appRenderingManager->SyncCreate(m_Description,mpAppHeap,m_pNijiAllocator,mpAppParam->mpAsyncFileManager);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_RENDERING_MANAGER_CREATE_END);
#endif

      int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mpAppParam->firstOpenPos)) - 1;
      mpAppParam->groundNo = mpAppParam->firstOpenPos * _ONEGROUND_SOZAI_ALLLV_NUM + lv * _ONEGROUND_SOZAI_NUM + GARC_islandground_fureai_resort01_COMP;
      
      OHNO_PRINT("地面番号 %d\n",mpAppParam->groundNo);
      for(int j=0;j<READ_DATA_NUM;j++){

        if(j < GARC_islandground_fureai_resort01_COMP){
          /*
          使わなくなったのでコメントアウト
          mFrs[j].step = _FILEREAD_START;
          mFrs[j].pFH = GFL_NEW(mpAppHeap->GetSystemHeap()) app::util::FileAccessor::RequestData;
          fileReadStructSetup(mFrs[j].pFH, ARCID_ISLAND_GROUND,  j ,mpAppHeap, &mpResourceBuffer[j]);
          */
        }
        else if((mpAppParam->groundNo <= j) && (j < (mpAppParam->groundNo +_ONEGROUND_SOZAI_NUM))){
          /*
          使わなくなったのでコメントアウト
          mFrs[j].step = _FILEREAD_START;
          mFrs[j].pFH = GFL_NEW(mpAppHeap->GetSystemHeap()) app::util::FileAccessor::RequestData;
          fileReadStructSetup(mFrs[j].pFH, ARCID_ISLAND_GROUND,  j ,mpAppHeap, &mpResourceBuffer[j]);
          */
        }
        else{  //必要な地形以外は読み込まない
          //使わなくなったのでコメントアウトmFrs[j].pFH=NULL;
        }
      }
  
      m_islandGroundLvPackDatId = this->getIslandGroundLvPackDatId(mpAppParam->firstOpenPos, lv);  // ResortDataManagerを使う

      m_Seq = GROUND_FILE_WAIT_;
      mFileSeq = 0;
    }
    //    break;
  case GROUND_FILE_WAIT_:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_01);
#endif
    if(fileOpenSequece()){
      m_Seq = GROUND_FILE_WAIT2_;
      mFileSeq = LOCAL_READ_START;
      m_islandGroundLvPackLoadSeq = 0;  // ResortDataManagerを使う
    }
    break; 
  case GROUND_FILE_WAIT2_:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_02);
#endif
    if(fileReadSequece())
    {
      BinLinkLoad(NULL/*使っていないのでコメントアウトmpAppHeap*/);
      m_Seq = GROUND_FILE_WAIT3_;
    }
    break;
  case GROUND_FILE_WAIT3_:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_03);
#endif
    if(IsBinLinkLoad())
    {
      /*
      使っていないのでコメントアウト
      mpCareCoreParamManager =
        GFL_NEW(mpAppHeap->GetDeviceHeap())PokeTool::KawaigariParamCareCoreManager(mpAppHeap->GetDeviceHeap(),mpAppHeap->GetDeviceHeap(),
                                                                                   mpAppParam->mpAsyncFileManager);
      */
      m_Seq = KAWAIGARI_WAIT_;
    }
    break;
  case KAWAIGARI_WAIT_:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_04);
#endif
    if(true/*使っていないのでコメントアウトmpCareCoreParamManager->IsLoaded()*/)
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_00);
#endif

      mpLogic = GFL_NEW( mpAppHeap->GetDeviceHeap() ) ResortInitLogic();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_01);
#endif
      mpLogic->SetEggWalkData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_BIN));
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_02);
#endif
      mpLogic->SetFamiliarityData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_HOTSPA_BIN));  //mpReadBuffAddr[GARC_islandbin_hotspa_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_03);
#endif
      mpLogic->SetWildPokemonData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WILDPOKEMON_BIN));  //mpReadBuffAddr[GARC_islandbin_wildpokemon_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_04);
#endif
      mpLogic->SetGimPointupData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_GIM_POINTUP_BIN));  //mpReadBuffAddr[GARC_islandbin_gim_pointup_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_05);
#endif
      mpLogic->SetGimLvData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_GIM_LV_BIN));  //mpReadBuffAddr[GARC_islandbin_gim_lv_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_06);
#endif
      mpLogic->SetPokeMameData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_POKEMAME_BIN));
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_07);
#endif
      mpLogic->SetBeansData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_BEANS_BIN));  //mpReadBuffAddr[GARC_islandbin_beans_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_08);
#endif
      mpLogic->SetAdventureData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_ADVENTURE_BIN));  //mpReadBuffAddr[GARC_islandbin_adventure_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_09);
#endif
      mpLogic->SetKinomiData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_KINOMI_BIN));  //mpReadBuffAddr[GARC_islandbin_kinomi_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_10);
#endif
      mpLogic->SetTalkTypeData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_TALKTYPE_BIN));  //mpReadBuffAddr[GARC_islandbin_talktype_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_11);
#endif
      mpLogic->SetPokemonCreateData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WILDCREATE_BIN));  // mpReadBuffAddr[GARC_islandbin_wildcreate_BIN] );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_12);
#endif
      mpLogic->SetDropBeansData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_DROPBEANS_BIN));  // mpReadBuffAddr[GARC_islandbin_dropbeans_BIN] );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_13);
#endif
      //mpLogic->SetAdventureItemTblData( GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_ADVENTURE_ITEMTBL_BIN));
      mpLogic->SetAdventureItemTblData( GetBinLinkReadBuff( BL_IDX_ISLAND_PACK_ADVENTURE_ITEMTBL_BIN ), GetBinLinkBuffSize( BL_IDX_ISLAND_PACK_ADVENTURE_ITEMTBL_BIN ) );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_14);
#endif


      //データは必ず前に
      mpLogic->InitFunc( mpAppParam->pSave,mpAppParam->pEventSave , mpLogicHeap, mpTempHeap, mpAppParam, mpAppParam->pBoxPokemon, NULL/*使っていないのでコメントアウトmpCareCoreParamManager*/);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_15);
#endif

      m_Seq = SKYBOX_WAIT_;
      mpResortIslandDraw = GFL_NEW(mpAppHeap->GetDeviceHeap()) ResortIslandDraw();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_16);
#endif
      mpResortIslandDraw->SetParam(mpAppParam, m_appRenderingManager, mpResourceBuffer,mpAppHeap,mpPokeModelHeap,mpPokeModelNijiAllocator,m_pNijiAllocator,NULL );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_17);
#endif
      mpResortIslandDraw->SetInitCameraData(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_CAMERA_BIN)); // mpReadBuffAddr[GARC_islandbin_camera_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_18);
#endif
      mpResortIslandDraw->SetHotSpaMove(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_HOTSPA_BIN)); // mpReadBuffAddr[GARC_islandbin_walk_hotspa_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_19);
#endif
      mpResortIslandDraw->SetFarmMove(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_FARM_BIN)); // mpReadBuffAddr[GARC_islandbin_walk_farm_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_20);
#endif
      mpResortIslandDraw->SetAdvMove(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_ADV_BIN)); // mpReadBuffAddr[GARC_islandbin_walk_adv_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_21);
#endif
      mpResortIslandDraw->SetGimMove(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_GIM_BIN)); // mpReadBuffAddr[GARC_islandbin_walk_gim_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_22);
#endif
      mpResortIslandDraw->SetFriendMove(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_WALK_FRIEND_BIN)); // mpReadBuffAddr[GARC_islandbin_walk_friend_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_23);
#endif
      mpResortIslandDraw->SetKinomiPos(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_KINOMIPOS_BIN)); // mpReadBuffAddr[GARC_islandbin_kinomipos_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_24);
#endif
      mpResortIslandDraw->SetPokeMoveFlg(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_MOVEFLG_BIN)); // mpReadBuffAddr[GARC_islandbin_moveflg_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_25);
#endif
      mpResortIslandDraw->SetStandPosBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_STANDPOS_BIN)); // mpReadBuffAddr[GARC_islandbin_standpos_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_26);
#endif
      mpResortIslandDraw->SetGimTouchBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_GIM_TOUCH_BIN)); // mpReadBuffAddr[GARC_islandbin_gim_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_27);
#endif
      mpResortIslandDraw->SetHotSpaTouchBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_HOTSPA_TOUCH_BIN)); // mpReadBuffAddr[GARC_islandbin_hotspa_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_28);
#endif
      mpResortIslandDraw->SetFarmTouchBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_FARM_TOUCH_BIN)); // mpReadBuffAddr[GARC_islandbin_farm_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_29);
#endif
      mpResortIslandDraw->SetFriendShipTouchBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_FRIENDSHIP_TOUCH_BIN)); // mpReadBuffAddr[GARC_islandbin_friendship_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_30);
#endif
      mpResortIslandDraw->SetAdventureTouchBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_ADVENTURE_TOUCH_BIN)); // mpReadBuffAddr[GARC_islandbin_adventure_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_31);
#endif
      mpResortIslandDraw->SetBeansPosBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_BEANS_POS_BIN)); // mpReadBuffAddr[GARC_islandbin_adventure_touch_BIN]);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_32);
#endif
      mpResortIslandDraw->SetPokeCameraSimpleBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_POKE_CAMERA_S_BIN));
      mpResortIslandDraw->SetPokeCameraIslandBin(GetBinLinkReadBuff(BL_IDX_ISLAND_PACK_POKE_CAMERA_I_BIN));
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_33);
#endif

      mpResortIslandDraw->SetLogic( mpLogic );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_34);
#endif

      mpResortDrawControl = GFL_NEW(mpAppHeap->GetDeviceHeap()) ResortDrawControl();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_35);
#endif
      mpResortDrawControl->SetParam(mpResortIslandDraw,mpLogic);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_C_36);
#endif

    }
    break;
  case SKYBOX_WAIT_:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_05);
#endif
    if(mpResortIslandDraw->InitFunc())
    {
      if( ReadBinData() )
      {
        mpResortIslandDraw->CreateAll();  // CreateAllでつくられるものは生成/破棄を繰り返すので、mpAppHeapを使った破棄しないものの生成が完了してから呼び出すことにしたのでここで呼ぶ。

        //ReadBinData();  // ResortDataManagerを使うので、bool ReadBinData(void);に変える。
        InitLayout();

#if defined(GF_PLATFORM_CTR)
        OHNO_PRINT("ResortProc::InitFuncEND %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
        m_Seq = SOUND_WAIT_;
      }
    }
    break;
  case SOUND_WAIT_:
    if(mpAppParam->bgmReq){
      if(!Sound::IsBGMPlaying()){
        GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
        if( timeZone < GameSys::TimeZone::NIGHT ){
          Sound::StartBGMReq( STRM_BGM_PISLAND );  //曲再生
          }
        else{
          Sound::StartBGMReq( STRM_BGM_PISLAND_N );  //曲再生
        }
        mpAppParam->bgmReq=false;
        return gfl2::proc::RES_FINISH;
      }
    }
    else{
      return gfl2::proc::RES_FINISH;
    }
  }
  return gfl2::proc::RES_CONTINUE;
}


//初期状態のカメラ
void ResortProc::InitCamera(void)
{
  {

    m_IslandDist = ISLAND_DIST_INIT;
    island_camera_rotate = 0.0f;


  }
  


}





//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result ResortProc::EndFunc(gfl2::proc::Manager* pManager)
{
  OHNO_PRINT("ResortProc::EndFunc seq %d \n",m_Seq);


  switch(m_Seq){
  case 0:
//    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
    m_Seq++;
    break;
  case 1:
    if(!GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_DOUBLE)){
      return gfl2::proc::RES_CONTINUE;
    }
    if(false == mpResortIslandDraw->EndFunc()){
      return gfl2::proc::RES_CONTINUE;
    }

    FreeBinData();

    //開放
    //使っていないのでコメントアウトmpCareCoreParamManager->Finalize();
    //使っていないのでコメントアウトGFL_SAFE_DELETE(mpCareCoreParamManager);

    BinLinkClose(NULL/*使っていないのでコメントアウトmpAppHeap*/);
    
    mpLogic->EndFunc();
    GFL_SAFE_DELETE(mpLogic);
    m_Seq++;
    // break throught
  case 2:
    if(mpFrameManager->End()){
      break;
    }
    m_Seq++;
    // break throught
  case 3:
    GFL_SAFE_DELETE(mpResortDrawControl);
    GFL_SAFE_DELETE(mpResortIslandDraw);
    DeleteLayout();
    m_Seq++;
    mFileSeq=LOCAL_CLOSE_START;
    break;
  case 4:
    if(fileCloseSequece())
    {
      m_Seq++;
    }
    break;
  case 5:
    {
      m_Seq++;
    }
    break;
  case 6:
    if(m_appRenderingManager->CanDestroy())
    {
#if PM_DEBUG
#if NIJI_PROJECT_APP_RESORT_USE_DEBUG_WIN
      GFL_SAFE_DELETE(mResortDebugWin);
#endif
      gfl2::heap::Manager::DeleteHeap(m_pLocalDebugDeviceHeap);
#endif

      GFL_SAFE_DELETE(m_appRenderingManager);

      for(int j=0;j<READ_DATA_NUM;j++){
        //使わなくなったのでコメントアウトGFL_SAFE_DELETE(mFrs[j].pFH);
        //GFL_SAFE_DELETE(mpResourceBuffer[j]);  // ResortDataManagerを使っているのでコメントアウト。
      }

      // ResortDataManagerを使っている
      GflHeapSafeFreeMemory(m_islandGroundLvPackBuf);

//      gfl2::gfx::GLMemory::SetAllocator(mpOldAllocator);  //ふっきさせる
//      GFL_SAFE_DELETE(m_pLocalDeviceHeapAllocator);

      GFL_SAFE_DELETE(m_pNijiAllocator);

      if(m_pIslandGroundLvDevHeap)
      {
        gfl2::heap::Manager::DeleteHeap(m_pIslandGroundLvDevHeap);
        m_pIslandGroundLvDevHeap = NULL;
      }

//      m_pLocalDeviceHeap->Dump();
     
      if(m_pLocalDeviceHeap)
      {
        gfl2::heap::Manager::DeleteHeap(m_pLocalDeviceHeap);
        m_pLocalDeviceHeap = NULL;
      }

      GFL_SAFE_DELETE(mpPokeModelNijiAllocator);
      if(mpPokeModelHeap)
      {
        gfl2::heap::Manager::DeleteHeap(mpPokeModelHeap);
        mpPokeModelHeap = NULL;
      }

      if(mpTempHeap)
      {
        gfl2::heap::Manager::DeleteHeap(mpTempHeap);
        mpTempHeap = NULL;
      }
      if(mpLogicHeap)
      {
        gfl2::heap::Manager::DeleteHeap(mpLogicHeap);
        mpLogicHeap = NULL;
      }

      GFL_SAFE_DELETE(mpAppHeap);

      if(mpRootHeap)
      {
        gfl2::heap::Manager::DeleteHeap(mpRootHeap);
        mpRootHeap = NULL;
      }


      return gfl2::proc::RES_FINISH;
    }
    break;
  }


  return gfl2::proc::RES_CONTINUE;
}







void ResortProc::BillboardVisible(bool bVisible)
{

}






bool ResortProc::ModelTouchHitCheck( u16 x, u16 y)
{
  return false;
}


bool ResortProc::PokeModelTouchHitCheck( u16 x, u16 y)
{
  return false;
}






enum{
  BILLBOARD_MODE,
  POKE_READING,
  POKE_MODE,
  HAPPY,
  HAPPY2,
  BILL_READING,
  BILL_READING2,
};



bool ResortProc::IsPokemonCameraMode(void)
{
  if(m_ModeSeq != BILLBOARD_MODE){
    return true;
  }
  return false;
}



//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result ResortProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_A_06);
#endif

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.Update();
#endif


  bool bkeye = false;

  switch(mChangeStep){
  case _CHANGE_INIT:
    ChangeGo();
    break;
  case _CHANGE_DEL:
    ChangeDel();
    break;
  case _CHANGE_GO:
    if(fileReadSequece()){
      mChangeStep = _CHANGE_RIN;
      mpResortIslandDraw->CreateAll();

    }
    break;
	case _CHANGE_RIN:
		mChangeStep = _CHANGE_END;
    if(mbFadeFlg){
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS);
      OpenHomeMneu(false);
      mpResortIslandDraw->ResortInputEnabled(true);
    }
    break;

  }
  if(mChangeStep == _CHANGE_END){
    bkeye=true;
  }

#if PM_DEBUG
#if NIJI_PROJECT_APP_RESORT_USE_DEBUG_WIN
  m_bMapChangeWait = mpResortDrawControl->ControlFunc(mResortDebugWin->GetDebugCameraMode(),bkeye);
  mpResortDrawControl->UpdateFunc(mResortDebugWin->GetDebugCameraMode());
  mpResortDrawControl->UpdateCameraMove( m_pResortFrame->IsFadeInStart(), m_pResortFrame->IsFadeOutStart() );
  
  mpAppParam->debugFullMode = mResortDebugWin->GetDebugFullMode();
  mpAppParam->debugCameraMode = mResortDebugWin->GetDebugCameraMode();
  mpLogic->DebugSetSpeed( mResortDebugWin->GetSpeedWork() );
#else
  m_bMapChangeWait = mpResortDrawControl->ControlFunc(false, bkeye);
  mpResortDrawControl->UpdateFunc();
  mpResortDrawControl->UpdateCameraMove( m_pResortFrame->IsFadeInStart(), m_pResortFrame->IsFadeOutStart() );
#endif

#else
  m_bMapChangeWait = mpResortDrawControl->ControlFunc(bkeye);
  mpResortDrawControl->UpdateFunc();
  mpResortDrawControl->UpdateCameraMove( m_pResortFrame->IsFadeInStart(), m_pResortFrame->IsFadeOutStart() );
  
#endif
  
  if(m_bMapChangeWait){
    mpLogic->SecFunc( m_pResortFrame->GetResortIndex() );
  }
  if(mpFrameManager){
    if(mpFrameManager->Main()==applib::frame::MAINSTATUS_NULL){
      m_Seq=0;
      GFL_PRINT("mpFrameManagerでの終了処理\n");
      ResortDataManager::GetStaticInstance()->ReleaseStop();
      return gfl2::proc::RES_FINISH;
    }
  }


  
  return gfl2::proc::RES_CONTINUE;
}


enum{
  STATE_STAND,
  STATE_MOVE1,
  STATE_MOVE2,
  STATE_MOVE3,
};





/**
 * @brief	PROCの描画処理(下準備)
 * @param pManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void ResortProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  //  if(mChangeStep==_CHANGE_END){
  if(m_appRenderingManager){
    m_appRenderingManager->PreDraw();
  }
//  }
}

/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void ResortProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
//  if(mChangeStep==_CHANGE_END){

  if(mpFrameManager){
    mpFrameManager->Draw(displayNo);
  }
  if(m_appRenderingManager){
    m_appRenderingManager->Draw(displayNo);
  }
 // }
}



//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

/*
使わなくなったのでコメントアウト
void ResortProc::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
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



bool ResortProc::fileOpenSequece(void)
{
  bool  ret = false;
  void* buf = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_ISLANDGROUND_PACK);
  if(    ( ResortDataManager::GetStaticInstance()->IsFileAccessible(ResortDataManager::FILE_ID_RESORT) )
      && ( ResortDataManager::GetStaticInstance()->GetUncompWorkHeap() )
      && ( buf != NULL ) )
  {
    // BL_IDX_RESORT_PACK_ISLANDGROUND_PACK_PACKをセットアップする
    gfl2::fs::BinLinkerAccessor  pack(buf);
    for(u32 i=0; i<GARC_islandground_fureai_resort01_COMP; ++i)
    {
      mpResourceBuffer[i] = pack.GetData(i);
    }
    ret = true;
  }
  return ret;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
bool ResortProc::fileOpenSequece(void)
{

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  switch(mFileSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_FILE_OPEN_START);
#endif

      pAsyncFileMan->AddArcFileOpenReq(mFrs[0].pFH->openr);
    }
    mFileSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( mFrs[0].pFH->openr.arcId ))
    {
      return true;
    }
    break;
  }
  return false;
}
*/



// ResortDataManagerを使う
gfl2::fs::ArcFile::ARCDATID ResortProc::getIslandGroundLvPackDatId(u32 island, u32 lv) const
{
  static const u32 ISLAND_NUM = 5;
  static const u32 LV_NUM     = 3;
  static gfl2::fs::ArcFile::ARCDATID ISLAND_GROUND_LV_PACK_DATID[ISLAND_NUM][LV_NUM] =
  {
    { GARC_resort_islandground_fureai_1_pack_pack_COMP, GARC_resort_islandground_fureai_2_pack_pack_COMP, GARC_resort_islandground_fureai_3_pack_pack_COMP },
    { GARC_resort_islandground_kinomi_1_pack_pack_COMP, GARC_resort_islandground_kinomi_2_pack_pack_COMP, GARC_resort_islandground_kinomi_3_pack_pack_COMP },
    { GARC_resort_islandground_tanken_1_pack_pack_COMP, GARC_resort_islandground_tanken_2_pack_pack_COMP, GARC_resort_islandground_tanken_3_pack_pack_COMP },
    { GARC_resort_islandground_undou_1_pack_pack_COMP , GARC_resort_islandground_undou_2_pack_pack_COMP , GARC_resort_islandground_undou_3_pack_pack_COMP  },
    { GARC_resort_islandground_onsen_1_pack_pack_COMP , GARC_resort_islandground_onsen_2_pack_pack_COMP , GARC_resort_islandground_onsen_3_pack_pack_COMP  },
  };

  u32 l_island = island;
  u32 l_lv     = lv;
  if( l_island >= ISLAND_NUM )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    l_island = 0;
  }
  if( l_lv >= LV_NUM )
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    l_lv = 0;
  }

  return ISLAND_GROUND_LV_PACK_DATID[l_island][l_lv];
}
u32 ResortProc::getIslandGroundLvStartIndex(gfl2::fs::ArcFile::ARCDATID datId) const
{
  u32 island = 0;
  u32 lv     = 0;
  switch(datId)
  {
  case GARC_resort_islandground_fureai_1_pack_pack_COMP:  island = 0;  lv = 0;  break;
  case GARC_resort_islandground_fureai_2_pack_pack_COMP:  island = 0;  lv = 1;  break;
  case GARC_resort_islandground_fureai_3_pack_pack_COMP:  island = 0;  lv = 2;  break;
  case GARC_resort_islandground_kinomi_1_pack_pack_COMP:  island = 1;  lv = 0;  break;
  case GARC_resort_islandground_kinomi_2_pack_pack_COMP:  island = 1;  lv = 1;  break;
  case GARC_resort_islandground_kinomi_3_pack_pack_COMP:  island = 1;  lv = 2;  break;
  case GARC_resort_islandground_tanken_1_pack_pack_COMP:  island = 2;  lv = 0;  break;
  case GARC_resort_islandground_tanken_2_pack_pack_COMP:  island = 2;  lv = 1;  break;
  case GARC_resort_islandground_tanken_3_pack_pack_COMP:  island = 2;  lv = 2;  break;
  case GARC_resort_islandground_undou_1_pack_pack_COMP :  island = 3;  lv = 0;  break;
  case GARC_resort_islandground_undou_2_pack_pack_COMP :  island = 3;  lv = 1;  break;
  case GARC_resort_islandground_undou_3_pack_pack_COMP :  island = 3;  lv = 2;  break;
  case GARC_resort_islandground_onsen_1_pack_pack_COMP :  island = 4;  lv = 0;  break;
  case GARC_resort_islandground_onsen_2_pack_pack_COMP :  island = 4;  lv = 1;  break;
  case GARC_resort_islandground_onsen_3_pack_pack_COMP :  island = 4;  lv = 2;  break;
  default                                              :  GFL_ASSERT(0);        break;  // 開発中に気付かせるためのASSERT。
  }
  u32 startIndex = island * _ONEGROUND_SOZAI_ALLLV_NUM + lv * _ONEGROUND_SOZAI_NUM + GARC_islandground_fureai_resort01_COMP;
  return startIndex;
}



bool ResortProc::fileReadSequece(void)
{
  gfl2::heap::HeapBase* uncompWorkHeap = ResortDataManager::GetStaticInstance()->GetUncompWorkHeap();
  
  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  bool ret = false;
  switch( m_islandGroundLvPackLoadSeq )
  {
  case 0:
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_ISLANDGROUND_LV_PACK_LOAD_START);
#endif
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      {
        req.arcId             = ARCID_RESORT;
        req.datId             = m_islandGroundLvPackDatId;
        req.ppBuf             = &m_islandGroundLvPackBuf;
        req.heapForBuf        = m_pIslandGroundLvDevHeap;
        req.align             = 128;
        req.heapForReq        = m_pIslandGroundLvDevHeap->GetLowerHandle();
        req.heapForCompressed = uncompWorkHeap;
      }
      pAsyncFileMan->AddArcFileLoadDataReq(req);
      
      ++m_islandGroundLvPackLoadSeq;
    }
    //breakしない
  case 1:
    if( app::util::FileAccessor::IsFileLoad(&m_islandGroundLvPackBuf) )
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_ISLANDGROUND_LV_PACK_LOAD_END);
#endif
      gfl2::fs::BinLinkerAccessor  pack;
      pack.Initialize(m_islandGroundLvPackBuf);
      u32 startIndex = this->getIslandGroundLvStartIndex(m_islandGroundLvPackDatId);
      for(s32 i=0; i<_ONEGROUND_SOZAI_NUM; ++i)
      {
        mpResourceBuffer[startIndex+i] = pack.GetData(i);
      }
      ++m_islandGroundLvPackLoadSeq;
      //breakしない
    }
    else
    {
      break;
    }
  case 2:
    {
      ret = true;
      break;
    }
  }
  return ret; 
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
bool ResortProc::fileReadSequece(void)
{

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  switch(mFileSeq){
  case LOCAL_READ_START:
    {
      for(int k = 0 ; k < READ_DATA_NUM ; k++)
      {
        if(mFrs[k].pFH && mFrs[k].step == _FILEREAD_START)
        {
          pAsyncFileMan->AddArcFileLoadDataReq(mFrs[k].pFH->loadr);
        }
      }
      mFileSeq = LOCAL_READ_WAIT;
    }
    break;
  case LOCAL_READ_WAIT:
    {
      int x=0;
      for(x =0 ;x < READ_DATA_NUM;x++)
      {
        if(mFrs[x].pFH==NULL)
        {
        }
        else if(mFrs[x].step == _FILEREAD_END){
        }
        else if(mFrs[x].step == _FILEREAD_START &&  pAsyncFileMan->IsArcFileLoadDataFinished( mFrs[x].pFH->loadr.ppBuf ) )
        {
        }
        else
        {
          break;
        }
      }
      if(x!=READ_DATA_NUM){
        break;
      }

      for(x =0 ;x < READ_DATA_NUM;x++){
        mFrs[x].step = _FILEREAD_END;
      }
     
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_FILE_LOAD_END);
#endif
      return true;
    }
  //  break;
  }
  return false;
}
*/



bool ResortProc::fileCloseSequece(void)
{
  return true;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
bool ResortProc::fileCloseSequece(void)
{

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  switch(mFileSeq){
  case LOCAL_CLOSE_START:
    {
      pAsyncFileMan->AddArcFileCloseReq(mFrs[0].pFH->closer);
      mFileSeq = LOCAL_CLOSE_WAIT;
    }
    //break;
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished( mFrs[0].pFH->openr.arcId ) ){
      mFileSeq = LOCAL_CLOSE_END;
      return true;
    }
    break;
  }
  return false;
}
*/




//二番目

void ResortProc::SetAppParam(App::Resort::APP_PARAM* pAppParam)
{
	mpAppParam = pAppParam;
  mpAppParam->mpAppHeap = mpAppHeap;

}

//一番最初

void ResortProc::CreateMemory(void)
{
  static const int SIZE_MY_APP = ResortHeapDefine::PROC_HEAP_SIZE;       ///  アプリメモリも容量管理のため分けて管理
  static const int SIZE_MY_APPDEV = ResortHeapDefine::PROC_DEV_SIZE;    /// アプリメモリも容量管理のため分けて管理

  //HEAPID_APP  本体に関するものはAPPめもりーから確保します
  //gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // Procで使うヒープの大元
  mpRootHeap = GFL_CREATE_LOCAL_HEAP( devHeap, ResortHeapDefine::ROOT_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_16, mpRootHeap, "ROOT_HEAP_SIZE");
#endif

  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(mpRootHeap) app::util::Heap();

  mpAppHeap->LocalHeapCreate(mpRootHeap,mpRootHeap,SIZE_MY_APP,SIZE_MY_APPDEV);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_05, mpAppHeap->GetSystemHeap(), "PROC_HEAP_SIZE");
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_06, mpAppHeap->GetDeviceHeap(), "PROC_DEV_SIZE");
#endif

  //FileReadHeapは使わなくなったのでコメントアウトmpAppHeap->SetFileReadHeap(devHeap);

//  devHeap->Dump();
//    m_pHeap    = GFL_CREATE_LOCAL_HEAP( heap,    heapSize,    gfl2::heap::HEAP_TYPE_EXP, false );


  // ResortInitLogic内で使用 
  mpLogicHeap = GFL_CREATE_LOCAL_HEAP( mpRootHeap, ResortHeapDefine::LOGIC_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_14, mpLogicHeap, "LOGIC_HEAP_SIZE");
#endif
  // 小物用一時ヒープ。確保したらすぐ解放するようなものにしか使わない。
  mpTempHeap = GFL_CREATE_LOCAL_HEAP( mpRootHeap, ResortHeapDefine::LOGIC_TEMP_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_09, mpTempHeap, "LOGIC_TEMP_HEAP_SIZE");
#endif

  // ポケモン3Dモデルシステム、ポケモン3Dモデル用
  mpPokeModelHeap = GFL_CREATE_LOCAL_HEAP( mpRootHeap, ResortHeapDefine::POKEMODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_15, mpPokeModelHeap, "POKEMODEL_HEAP_SIZE");
#endif
  mpPokeModelNijiAllocator = GFL_NEW( mpPokeModelHeap ) System::nijiAllocator( mpPokeModelHeap );
}





// ------------------------------------
// 高さコリジョンとの判定
void ResortProc::UpdateCollision_Height(  gfl2::math::Vector3* pVec )
{
  if(m_pCollisionModel==NULL){
    return;
  }

}




bool ResortProc::asyncCreatePokeModel(void)
{
  bool isCreated = false;

  switch(m_asyncCreateDestroyPokeModelStep)
  {
  case 0:
    {
      m_asyncCreateDestroyPokeModelStep = 1;
      //breakしない
    }
  case 1:
    {
      {
        m_asyncCreateDestroyPokeModelStep = 2;
        //breakしない
      }
    }
  case 2:
    {

      {
        m_asyncCreateDestroyPokeModelStep = 3;
        //breakしない
      }
    }
  case 3:
  default:
    {
			//ぽけもんをとうろく

      BillboardVisible(false);

      m_asyncCreateDestroyPokeModelStep = 0;
      isCreated = true;
    }
    break;
  }

  return isCreated;
}



bool ResortProc::DeletePokemonASync(void)
{
  switch(m_PokeDeleteStep){
  case 0:
    m_PokeDeleteStep=1;
    break;
  case 1:
    m_PokeDeleteStep=0;
    return true;
  }
  return false;
}



//--------------------------------------------------------------------------
/**
 * @brief   切り替え開始
 * @param   index   リゾートインデックス
 *  @return  切りかえれる場合true
 */
//--------------------------------------------------------------------------
bool ResortProc::ChangeStart( int index,bool bfade )
{
#if PM_DEBUG
  if( mResortDebugWin->GetDebugCameraMode()){
    return false;
  }
#endif


  if(mChangeStep != _CHANGE_END){
    return false;
  }
  if(!m_bMapChangeWait){
    return false;
  }

  
  mChangeStep = _CHANGE_INIT;

  mpResortIslandDraw->RemoveAll();
  ChangeFadeStart(bfade);
  mChangeNo = index;

  return true;
}


void ResortProc::ChangeFadeStart(bool bfade)
{
  if(bfade){
    gfl2::math::Vector4 start_col(0.0f,0.0f,0.0f,0.0f);
    gfl2::math::Vector4 end_col(0.0f,0.0f,0.0f,0.0f);
    
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_CROSS, &start_col, &end_col, gfl2::Fade::FADE_DEFAULT_SYNC );
  }
  mbFadeFlg = bfade;

  
}


void ResortProc::ChangeGo(void)
{
  if(mpResortIslandDraw->DeleteAll()){
    mChangeStep= _CHANGE_DEL;
  }
}


void ResortProc::ChangeDel(void)
{
  int j=0;
  for( j=mpAppParam->groundNo;j<mpAppParam->groundNo + _ONEGROUND_SOZAI_NUM;j++){
    //使わなくなったのでコメントアウトGFL_SAFE_DELETE(mFrs[j].pFH);
    //GFL_SAFE_DELETE(mpResourceBuffer[j]);  // ResortDataManagerを使っているのでコメントアウト。
  }

  int lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(mChangeNo)) - 1;
  mpAppParam->groundNo = mChangeNo * _ONEGROUND_SOZAI_ALLLV_NUM + lv * _ONEGROUND_SOZAI_NUM + GARC_islandground_fureai_resort01_COMP;
  mpAppParam->firstOpenPos = mChangeNo;
  mpAppParam->pSave->SetSelectResortPos(static_cast<Savedata::ResortSave::PokeResortKind>(mChangeNo));
  OHNO_PRINT("地面番号 %d %d  %d %d\n",mpAppParam->groundNo, mChangeNo, lv, GARC_islandground_fureai_resort01_COMP);
  
  for( j=mpAppParam->groundNo;j<mpAppParam->groundNo + _ONEGROUND_SOZAI_NUM;j++){
    /*
    使わなくなったのでコメントアウト
    mFrs[j].pFH = GFL_NEW(mpAppHeap->GetSystemHeap()) app::util::FileAccessor::RequestData;
    mFrs[j].step = _FILEREAD_START;
    fileReadStructSetup(mFrs[j].pFH, ARCID_ISLAND_GROUND,  j ,mpAppHeap, &mpResourceBuffer[j]);
    */
  }

  mChangeStep= _CHANGE_GO;
  mFileSeq = LOCAL_READ_START;

  // ResortDataManagerを使っている
  GflHeapSafeFreeMemory(m_islandGroundLvPackBuf);
  m_islandGroundLvPackDatId = this->getIslandGroundLvPackDatId(mChangeNo, lv);
  m_islandGroundLvPackLoadSeq = 0;
}



//--------------------------------------------------------------------------
/**
 * @brief   ホームメニューオープン
 */
//--------------------------------------------------------------------------
void ResortProc::OpenHomeMneu( bool flg )
{
  mpResortIslandDraw->SetKeyEnable(!flg);
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケモンをタッチした後、メニューフローが終わったときに呼ばれる
 */
//--------------------------------------------------------------------------

void ResortProc::ClosePokemonTouchMenu( void )
{
  mpResortIslandDraw->SetPokemonWalkEnd();
  
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケモンをタッチ出来る状態になっていればtrue 忙しいとfalse
 */
//--------------------------------------------------------------------------
bool ResortProc::IsPokemonTouchEnable( void )
{
  return !mpResortIslandDraw->IsPokemonCameraMode();
}





bool ResortProc::ReadBinData(void)
{
  void* buf = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_ISLANDSHADOW_PACK);
  if( buf != NULL )
  {
    gfl2::fs::BinLinkerAccessor  pack(buf);
    for(u32 i=0; i<_SHADOW_READBUFF_MAX; ++i)
    {
      mpShadowReadBuffAddr[i] = pack.GetData(i);
    }

    mpResortIslandDraw->PokeShadowCreate(mpShadowReadBuffAddr[0],mpShadowReadBuffAddr[1],mpShadowReadBuffAddr[2],
      mpShadowReadBuffAddr[3],mpShadowReadBuffAddr[4]);

    return true;
  }
  return false;
}
#if 0
ResortDataManagerに置き換えたのでコメントアウト。
void ResortProc::ReadBinData(void)
{
  app::util::FileAccessor::RequestData frs;

  {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_FILE_SYNC_OPEN_START);
#endif

    frs.openr.arcId       = ARCID_ISLAND_POKESHADOW;
    frs.openr.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.openr.heapForFile = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    frs.openr.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAppParam->mpAsyncFileManager->SyncArcFileOpen( frs.openr );
  }
  

  for(int i = 0; i < _SHADOW_READBUFF_MAX; i++)
  {
    frs.loadr.datId      = GARC_islandshadow_shadow_s_COMP + i;                        // [in]  ファイル名。終端は'\0'になっていること。
    frs.loadr.arcId         = ARCID_ISLAND_POKESHADOW;                               // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    frs.loadr.prio          = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;    // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    frs.loadr.ppBuf         = &mpShadowReadBuffAddr[i];                                  // [out] 読み込んだデータの格納先( 4 byte alignment )
    frs.loadr.heapForBuf    = mpAppHeap->GetDeviceHeap();                      // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    frs.loadr.heapForReq    = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    frs.loadr.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込み
    frs.loadr.align = 128;
    mpAppParam->mpAsyncFileManager->SyncArcFileLoadData( frs.loadr );
  }

  
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    frs.closer.arcId       = ARCID_ISLAND_POKESHADOW;
    frs.closer.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.closer.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAppParam->mpAsyncFileManager->SyncArcFileClose( frs.closer );

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_PROC_FILE_SYNC_CLOSE_END);
#endif
  }

  mpResortIslandDraw->PokeShadowCreate(mpShadowReadBuffAddr[0],mpShadowReadBuffAddr[1],mpShadowReadBuffAddr[2],
		mpShadowReadBuffAddr[3],mpShadowReadBuffAddr[4]);
/*  
  walk_island_binstr* pstrx =  (walk_island_binstr*)mpReadBuffAddr[2];

  for(int i=0;i<3;i++){
    OHNO_PRINT("  %d %d \n", pstrx[i].c1, pstrx[i].c2);
  }
*/

/* 
  camera_island_binstr* pstrx =  (camera_island_binstr*)mpReadBuffAddr[2];

  for(int i=0;i<2;i++){
    OHNO_PRINT("  %f %f %f \n", pstrx[i].c0,  pstrx[i].c1, pstrx[i].c2);
  }
*/

}
#endif


void ResortProc::FreeBinData(void)
{
  for(int i = 0; i < _SHADOW_READBUFF_MAX;i++){
    mpShadowReadBuffAddr[i] = NULL;
  }
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortProc::FreeBinData(void)
{
  for(int i = 0; i < _SHADOW_READBUFF_MAX;i++){
    GFL_SAFE_DELETE(mpShadowReadBuffAddr[i]);
  }
}
*/


//ARCID_ISLAND_POKESHADOW

void ResortProc::BinLinkLoad(app::util::Heap* /*pAppHeap*/)
{}
/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortProc::BinLinkLoad(app::util::Heap* pAppHeap)
{
  app::util::FileAccessor::FileOpen( ARCID_ISLAND_BIN, pAppHeap->GetDeviceHeap(), true );
  app::util::FileAccessor::FileLoad( ARCID_ISLAND_BIN, GARC_islandbin_island_pack_PACK, &m_dataBuf, pAppHeap->GetDeviceHeap(), false, 128 );
}
*/


bool ResortProc::IsBinLinkLoad(void)
{
  void* buf = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_ISLANDBIN_PACK);
  if( buf != NULL )
  {
    m_dataBuf = buf;
    return true;
  }
  return false;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
bool ResortProc::IsBinLinkLoad(void)
{
  return app::util::FileAccessor::IsFileOpen(ARCID_ISLAND_BIN) && app::util::FileAccessor::IsFileLoad(&m_dataBuf);
}
*/


void* ResortProc::GetBinLinkReadBuff(int linkno)
{
  m_binLinker.Initialize(m_dataBuf);
  return m_binLinker.GetData(linkno);
}


u32 ResortProc::GetBinLinkBuffSize( int linkno )
{
  m_binLinker.Initialize( m_dataBuf );
  return m_binLinker.GetDataSize( linkno );
}


void ResortProc::BinLinkClose(app::util::Heap* /*pAppHeap*/)
{
  m_dataBuf = NULL;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
void ResortProc::BinLinkClose(app::util::Heap* pAppHeap)
{
  app::util::FileAccessor::FileCloseSync( ARCID_ISLAND_BIN );
  GFL_SAFE_DELETE(m_dataBuf);
}
*/


//--------------------------------------------------------------------------
/**
 * @brief   モデルアニメ開始
 * @param   type  ModelAnim_e
 */
//--------------------------------------------------------------------------
void ResortProc::StartModelAnim( int type )
{
  switch(type){
  case ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_IN:
  case ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_OUT:
    mpResortDrawControl->StartModelAnim(type);
    break;
  default:
    mpResortIslandDraw->StartModelAnim(type);
    break;
  }
}




GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)



