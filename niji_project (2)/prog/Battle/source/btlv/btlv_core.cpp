//======================================================================
/**
 * @file	btlv_core.cpp
 * @brief	BattleVIewSystem
 * @author	ariizumi
 * @data	15/00/00
 */
//======================================================================
#include <arc_def_index/arc_def.h>
#include <arc_index/wazaeff_resource.gaix>
#include "btlv_seq_com_define.h"

#include <AppLib\include\Util\app_util_heap.h>
#include <types/include/gfl2_Typedef.h>

#include <model/include/gfl2_CharaModelFactory.h>

#include "AppLib/include/Fade/DemoFade.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/Object/include/TaskFunc.h"
#include "GameSys/include/NijiTime.h"
#include "System/include/Skybox/Skybox.h"
#include "System/include/DressUp.h"

#include "../btl_mainmodule.h"
#include "../btl_client.h"
#include "../btl_str.h"
#include "../btl_tables.h"
#include "../btl_SideEffectStatus.h"
#include "../btl_RoyalRankingContainer.h"
#include "../btl_ZenryokuWazaStatus.h"
#include "../handler/hand_side.h"
#include "../handler/hand_pos.h"

#include "./render/btlv_RenderSystem.h"

#include "btlv_core.h"
#include "btlv_camera.h"
#include "btlv_debug.h"
#include "btlv_ui.h"
#include "btlv_seq_sys.h"
#include "btlv_task.h"
#include "btlv_trainer_data.h"
#include "btlv_loader.h"

#include <arc_index/wazaeff_seqfile.gaix>
#include <niji_conv_header/trainer/trtype_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/battle_data_pack.h>



GFL_NAMESPACE_BEGIN(btl)

const f32 BattleViewSystem::NEAR_PLANE = 16.0f;
const f32 BattleViewSystem::FAR_PLANE  = 6000.0f;
const f32 BattleViewSystem::CAMERA_FOVY = 30.0f;

BattleViewSystem::BattleViewSystem(void)
:mMainModule(NULL)
,mBattleClient(NULL)
,mBattleContainer(NULL)
,mBagMode(BBAG_MODE_NORMAL)
,mMsgSpeed(Savedata::ConfigSave::MSG_NORMAL)
{
  mDeviceManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  mAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
#if PM_DEBUG
  mDebugParam = NULL;
#endif
  mInitSeq = 0;

#if defined(GF_PLATFORM_CTR)
	{
		// GPUアクセスの優先度を高める
		nn::gx::CTR::SetMemAccessPrioMode(nn::gx::CTR::MEM_ACCESS_PRIO_MODE_4);
	}
#endif
}

BattleViewSystem::BattleViewSystem(BTLV_INIT_PARAM* initParam)
:mMainModule(initParam->mMainModule)
,mBattleClient(initParam->mClient)
,mBattleContainer(initParam->mPokeCon)
,mBagMode(initParam->mBagMode)
,mMsgSpeed(initParam->mMsgSpeed)
{
  mDeviceManager = mMainModule->GetUIManager();
  mAsyncFileManager =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
#if PM_DEBUG
  mDebugParam = initParam->mDebugParam;
#endif
	mInitSeq = 0;

#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
	if (GetDebugParam() && GetDebugParam()->mIsOptimizeMode)
#endif
	{
		// GPUアクセスの優先度を高める
		nn::gx::CTR::SetMemAccessPrioMode(nn::gx::CTR::MEM_ACCESS_PRIO_MODE_4);
	}
#endif // defined(GF_PLATFORM_CTR)
}

BattleViewSystem::~BattleViewSystem()
{
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
	if (GetDebugParam() && GetDebugParam()->mIsOptimizeMode)
#endif
	{
		// GPUアクセスの優先度を通常に戻す。
		nn::gx::CTR::SetMemAccessPrioMode(nn::gx::CTR::MEM_ACCESS_PRIO_MODE_1);
	}
#endif // defined(GF_PLATFORM_CTR)

}

//初期化
//@retval true   初期化完了
//@retval false  初期化中
bool BattleViewSystem::Initialize( gfl2::heap::HeapBase* heap )
{
  switch(mInitSeq)
  {
  case 0:
    {
      GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
      setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
      setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);

      BtlvTickStart(0,"BtlvInitialize");
      BtlvTickStart(6,"Initialize1");
      BtlvTickStart(12,"Initialize1(Heap)");
      if( GetBtlvMode() == BTLV_MODE_CAPTURE )
      {
        //捕獲デモはトレーナー2体分減らす
        mHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap , 0x2080000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvMain" );
      }
      else
      {
        mHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap , 0x23FD000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvMain" );
      }

      //常駐ヒープ
      //0x70000 背景分
      mResidentHeap = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x860000 - 0x70000 , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvResi" );
      mResidentAllocator = GFL_NEW(GetResidentHeap())gfl2::heap::NwAllocator(GetResidentHeap());
      mResidentGLAllocator = GFL_NEW( GetResidentHeap() ) System::nijiAllocator(GetResidentHeap());
#if PM_DEBUG
      mResidentGLAllocator->DebugVramMemoryInfoDump();
#endif
      //演出用一時ヒープ
      //0x2B000 全力常駐分 (以前の値)
      //momijiで行動選択でのヒープ不足対応のため、下に移動させる
      //mTempHeap = GFL_CREATE_LOCAL_HEAP_NAME(mHeap, 0x580000 - 0x2B000, gfl2::heap::HEAP_TYPE_EXP, false, "BtlvTemp");;
      //mTempGLAllocator = GFL_NEW( mResidentHeap ) System::nijiAllocator(mTempHeap);
      int tempHeapHeapAddVal = 0x580000 - 0x2B000;

      //ポケモン用(メインは別で確保されるが、一応断片化対策
      for( int i=0;i<POKE_MODEL_NUM;i++ )
      {
        mPokeHeap[i] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x5000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvPoke" );
        mPokeGLAllocator[i] = GFL_NEW( mPokeHeap[i] ) System::nijiAllocator(mPokeHeap[i]);
      }
      //トレーナー用
      //着せ替えでパーツを共通領域に読み込むので、減らす。
      //通常0x1B0000 男女2x2実測 0x44B9C(重複チェック入れればもうチョイ減るかも
      int trainerHeapval = 0x48000; //主人公男女分
      int fullPowerHeapAddVal = 0;

      //momiji変更
      //全力ヒープが0x8000増えた
      int keepHeapAddVal = 0x33000;

      //戦闘背景は基本0xb0000 一部0xd0000 ただ着せ替え4人のところは通信のみなのでその場合0x62000(ロイヤル施設
      //着せ替えが居ないと1体に付き0x20000増える。 なのでストーリーは基本＋0x60000
      //あと、システム使用分で4000
      //ロイヤル実測0x74000ぐらいだった
      int bgHeapVal = 0x78000; 
      if( GetBtlvMode() == BTLV_MODE_CAPTURE )
      {
        //捕獲デモはトレーナー2体分減らす
        for( int i=0;i<TRAINER_MODEL_NUM-2;i++ )
        {
          //sango最大 0x1647F0
          trainerHeapval += 0x190000;
          bgHeapVal += 0x20000;
          mTrainerHeap[i] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x040000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTr" );
          mTrainerGLAllocator[i] = GFL_NEW( mTrainerHeap[i] ) System::nijiAllocator(mTrainerHeap[i]);
        }
        for( int i=2;i<TRAINER_MODEL_NUM;i++ )
        {
          mTrainerHeap[i] = mTrainerHeap[0];
          mTrainerGLAllocator[i] = mTrainerGLAllocator[0];
        }
      }
      else
      {
        bool isLastBoss = false;  //財団ビーストはモデルが大きいので例外対応
#if PM_DEBUG
        BTLV_DEBUG_PARAM *debParam = GetDebugParam();
        if( debParam && debParam->mTrainerType[1] == CHARA_MODEL_ID_BTTR0035_00 )
        {
          isLastBoss = true;
        }
        else
#endif
        if( GetTrainerType(1) == trainer::TRTYPE_BEAST )
        {
          isLastBoss = true;
        }
        if( isLastBoss )
        {
          //sango最大 0x1647F0
          trainerHeapval += 0x380000;
          keepHeapAddVal += 0xE0000;
          bgHeapVal += 0x100000;
          mTrainerHeap[0] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x200000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTr" );
          mTrainerHeap[1] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x080000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTrBeast" );
          mTrainerHeap[2] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x010000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTr" );
          mTrainerHeap[3] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x010000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTr" );
          for( int i=0;i<TRAINER_MODEL_NUM;i++ )
          {
            mTrainerGLAllocator[i] = GFL_NEW( mTrainerHeap[i] ) System::nijiAllocator(mTrainerHeap[i]);
          }
        }
        else
        {
          for( int i=0;i<TRAINER_MODEL_NUM;i++ )
          {
            bool isPlayer = false;
#if PM_DEBUG
            BTLV_DEBUG_PARAM *debParam = GetDebugParam();
            if( debParam && (debParam->mTrainerType[i]==0||debParam->mTrainerType[i]==1||debParam->mTrainerType[i]==-2))
            {
              isPlayer = true;
            }
            else
#endif
            if( GetClientPlayerData( i ) )
            {
              isPlayer = true;
            }

            //BattleFes
            if (IsBattleFesMode() && i >= 2)
            {
              //フェスは２，３人目にトレーナーが入っている
              isPlayer = true;
            }

            if( isPlayer )
            {
              trainerHeapval += 0x30000;
              
              //sango最大 0x1647F0
              mTrainerHeap[i] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x1D0000 , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTrPc" );
              mTrainerGLAllocator[i] = GFL_NEW( mTrainerHeap[i] ) System::nijiAllocator(mTrainerHeap[i]);
            }
            else
            if( IsClientTrainerExist(i) )
            {
              trainerHeapval += 0x130000; //Keepにまわせると思う。
              bgHeapVal += 0x30000;
              //keepHeapAddVal += 0x60000;
              if (GetBtlvMode() == BTLV_MODE_BATTLE)
              {
                tempHeapHeapAddVal += 0x60000;  //momijiで行動選択のヒープ不足対応のため追加。
              }
              else
              {
                //ただし、バトル以外(エフェクトビュワー)ではメモリチェックができなくなるため追加しない
                keepHeapAddVal += 0x60000;
              }
              
              //sango最大 0x1647F0
              mTrainerHeap[i] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x040000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTrNpc" );
              mTrainerGLAllocator[i] = GFL_NEW( mTrainerHeap[i] ) System::nijiAllocator(mTrainerHeap[i]);
            }
            else
            {
              //トレーナーが居ない時。
              keepHeapAddVal += 0x1C0000;
              bgHeapVal += 0x30000;
              //sango最大 0x1647F0
              mTrainerHeap[i] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x010000   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTrNpc" );
              mTrainerGLAllocator[i] = GFL_NEW( mTrainerHeap[i] ) System::nijiAllocator(mTrainerHeap[i]);
            }
          }
        }
      }

      mTrainerCommonHeap = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , trainerHeapval , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvTrCommon" );

      //全力ヒープ(ジガルデ100％が最大 0x96408 + 常駐0x87c0
      //基本は1体分だけ確保。ヌシとビースト戦のみもう1体分必要
      //ヌシの最大はジャラランガ0x694A8
      //トレーナーが少ないのでその分をまわす
      //momiji最大はツンデツンデ 0xA6AA8
      //こいつはウルトラビーストなので2体目にも登場する・・・
      if( GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ||
          GetSetupStatusFlag( BTL_STATUS_FLAG_VS_ULTRA_BEAST ) ||
          GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NEKUROZUMA ) ||
          IsBeastBattle() )
      {
        fullPowerHeapAddVal += 0xA8000;
        keepHeapAddVal -= 0xA8000;
      }
      mFullPowerHeap = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0xA8000 + 0x10000 + fullPowerHeapAddVal , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvFullPower" );
      mFullPowerAllocator = GFL_NEW( GetResidentHeap() ) System::nijiAllocator(GetFullPowerHeap());

      mBgHeap = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , bgHeapVal , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvBg" );

      //天候用(リソースが69kb)
      mWeatherHeap[0] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x1A000, gfl2::heap::HEAP_TYPE_EXP , false , "BtlvWeater1" );
      mWeatherHeap[1] = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , 0x1A000, gfl2::heap::HEAP_TYPE_EXP , false , "BtlvWeater2" );

      //演出用一時ヒープ
      mTempHeap = GFL_CREATE_LOCAL_HEAP_NAME(mHeap, tempHeapHeapAddVal , gfl2::heap::HEAP_TYPE_EXP, false, "BtlvTemp");;
      mTempGLAllocator = GFL_NEW(mResidentHeap) System::nijiAllocator(mTempHeap);

      mHeapKeep = NULL;
      if( keepHeapAddVal > 0 )
      {
        mHeapKeep = GFL_CREATE_LOCAL_HEAP_NAME( mHeap , keepHeapAddVal   , gfl2::heap::HEAP_TYPE_EXP , false , "BtlvKeep" );
      }


      mAppHeap = GFL_NEW(GetResidentHeap())::app::util::Heap();
      mAppHeap->Create(GetResidentHeap(),GetResidentHeap(),mResidentAllocator,mResidentAllocator,NULL,NULL);

      BtlvTickEnd(12,"Initialize1(Heap)");
      BtlvTickStart(11,"Initialize1(Snd)");
      Sound::LoadAndPushBattleGlobalSEAsync(mTempHeap);
      BtlvTickStart(13,"Initialize1(Gra)");

    //その他のbattleモジュール初期化
      if( mMainModule )
      {
        str::InitSystem( mMainModule , GetClientID() , mBattleContainer , GetResidentHeap() , true );
      }
      else
      {
        if( GetBtlvMode() == BTLV_MODE_CAPTURE )
        {
          // チュートリアル用に生成
          str::InitSystem( NULL, GetClientID() , NULL, GetResidentHeap() , false );
        }
      }

      {
        mParticleIndexManager = GFL_NEW(GetResidentHeap())DemoLib::Obj::EffectIndexManager(GetResidentHeap(),64);
    
        //ARIIZUMI_PRINT("EffectInit[%d]->",GetResidentHeap()->GetTotalFreeSize());
        mEffectHeap = GFL_NEW(GetResidentHeap())gfl2::Effect::GFHeap(GetResidentHeap());

        gfl2::Effect::Config config;
        config.SetEffectHeap( mEffectHeap );
        #ifdef GF_PLATFORM_CTR
        config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
        #endif //GF_PLATFORM_CTR
        config.SetEmitterNum(512);  //デフォルト64 +1で664
        //config.SetStripeNum(64);  //デフォルト32 +1で22656 だから消す
        config.SetResourceNum(mParticleIndexManager->GetIndexSize());  //現状1024.メモリによっては見直すこと
        mEffectSystem = GFL_NEW(GetResidentHeap())gfl2::Effect::System(config);

        for( int i=0;i<EFFGRP_MAX;i++ )
        {
          mEffectGroupIDList[i] = GFL_NEW(GetResidentHeap()) gfl2::util::List<u32>( GetResidentHeap(),1);
          mEffectGroupIDList[i]->PushFront(i);
        }
        //ARIIZUMI_PRINT("[%d]\n",GetResidentHeap()->GetTotalFreeSize());
      }
      mRenderSys = GFL_NEW(GetResidentHeap())BtlvRenderSystem(this);
      mRenderSys->Initialize();
      mCameraSys = GFL_NEW(GetResidentHeap())BtlvCameraSystem(this);
      mCameraSys->Initialize();
      {
        gfl2::math::Vector3 pos(256.0f, 106.0f, 465.0f );
        gfl2::math::Vector3 trg( -45.0f, 70.0f, -20.0f );
        mCameraSys->SetPosition(pos,trg);
        mCameraSys->SetFov(30.0f);
        mCameraSys->SetNear(NEAR_PLANE);
        mCameraSys->SetFar(FAR_PLANE);
      }

      {
        gfl2::Effect::EffectRenderPath::Config config;
        gfl2::Effect::EffectRenderPath *effRenderPath;
        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList[EFFGRP_NORMAL];
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectRenderPath();
        effRenderPath->SetConfig(config);
    
        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList[EFFGRP_SCREEN];
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetScreenEffectRenderPath();
        effRenderPath->SetConfig(config);

        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList[EFFGRP_SCREEN_BEFORE];
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectBeforeRenderPath();
        effRenderPath->SetConfig(config);

        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList[EFFGRP_NORMAL_LOW];
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectLowRenderPath();
        effRenderPath->SetConfig(config);

        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList[EFFGRP_NORMAL_HIGH];
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectHighRenderPath();
        effRenderPath->SetConfig(config);
      }

      mUiSys = GFL_NEW(GetResidentHeap())BtlvUi(this);
      //mUiSys->Initialize( mMsgSpeed );  //フォントの初期化を待たないとまずいので、ここではしない
      
      mSeqSys = GFL_NEW(GetResidentHeap())BtlvSeqSystem(this);

      mPokeModelSys = GFL_NEW(GetResidentHeap())PokeTool::PokeModelSystem();
      mPokeModelSys->InitSystemAsync(GetResidentHeap(),GetResidentHeap(),GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),POKE_MODEL_NUM);
      PokeTool::PokeModelSystem::HeapOption opt;
      opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
      opt.useIdModel = false;
      opt.useShadow = true;
      opt.useColorShader = true;
      opt.allocSize = mPokeModelSys->CalcPokeModelHeapSize(&opt) - 0x10000; //現状最大はジガルデ100％の0x30784C
      mPokeModelSys->CreatePokeModelHeap(GetMainHeap(),&opt);

      mCharaFactory = GFL_NEW(GetResidentHeap())poke_3d::model::CharaModelFactory();
      mCharaFactory->InitializeAsync(mAsyncFileManager,GetTrainerCommonHeap(),ARCID_CHARA_MODEL_BATTLE_NORMAL);

      //mDressUpModelResourceManager = GFL_NEW(GetResidentHeap())poke_3d::model::DressUpModelResourceManager();
      //mDressUpModelResourceManager->InitializeAsync(mAsyncFileManager,GetResidentHeap(),System::DressUp::GetDressUpArcIdListBattle());

      mObjectManager = GFL_NEW(GetResidentHeap())DemoLib::Obj::ObjectManager(GetResidentHeap() , 512);
      mTaskSystem = GFL_NEW(GetResidentHeap())DemoLib::Obj::TaskSystem(GetResidentHeap() ,GetResidentHeap(), 64,0x1000);
      mTaskSystemAfterCamera = GFL_NEW(GetResidentHeap())DemoLib::Obj::TaskSystem(GetResidentHeap() ,GetResidentHeap(), 8,0x1000);
      mTaskSystemAlways = GFL_NEW(GetResidentHeap())DemoLib::Obj::TaskSystem(GetResidentHeap() ,GetResidentHeap(), 4,0x100);
      mResourceManager = GFL_NEW(GetResidentHeap())DemoLib::Obj::ResourceManager(mAsyncFileManager,GetResidentHeap(),128);

      for( int i=0;i<POKEMODEL_NUM;i++ )
      {
        mPokeModel[i] = GFL_NEW(GetPokeHeap(i))BtlvObject_PokeModel(this,mRenderSys->GetRenderingPipeLine(),static_cast<BtlvPos>(i),i);
        mObjectManager->AddObject(mPokeModel[i]);
        mObjectManager->AddObject(mPokeModel[i]->GetMigawariModel(),DemoLib::Obj::OBJ_DEFAULT_PRIORITY+1);
      }
      for( int i=0;i<BTLV_EFF_SOUND_NUM;i++ )
      {
        mSoundObject[i] = GFL_NEW(GetResidentHeap())BtlvObject_Sound(this,i);
        mObjectManager->AddObject(mSoundObject[i]);
      }

      mBgFadeModel = GFL_NEW(GetResidentHeap())BtlvObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());
      mObjectManager->AddObject(mBgFadeModel);
      mEdgeMaskModel = GFL_NEW(GetResidentHeap())BtlvObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());
      mObjectManager->AddObject(mEdgeMaskModel);
      mFloorMaskModel = GFL_NEW(GetResidentHeap())BtlvObject_Gfmdl(this,mRenderSys->GetRenderingPipeLine());
      mObjectManager->AddObject(mFloorMaskModel);


      mModelObjManager = GFL_NEW(GetResidentHeap())GfmdlObjectManager(GetResidentHeap(),BTLV_EFF_MODEL_NUM);
      mEffectObjManager = GFL_NEW(GetResidentHeap())EffectObjectManager(GetResidentHeap(),BTLV_EFF_PARTICLE_NUM);
      mClusterObjManager = GFL_NEW(GetResidentHeap())ClusterObjectManager(GetResidentHeap(),BTLV_EFF_CLUSTER_NUM);

      mWorkStr[0] = GFL_NEW(GetResidentHeap()) gfl2::str::StrBuf(BTL_STRBUF_STD_LENGTH,GetResidentHeap());
      mWorkStr[1] = GFL_NEW(GetResidentHeap()) gfl2::str::StrBuf(BTL_STRBUF_STD_LENGTH,GetResidentHeap());
      mWordSet =  GFL_NEW(GetResidentHeap()) print::WordSet(GetResidentHeap());

      mIsPlayVoice = false;
      for( int i=0 ; i<VOICE_PLAYER_NUM ; i++ )
      {
        mIsPlayVoiceFlg[i] = false;
        mIsPlayVoiceFlg_Waza[i] = false;
      }

      //3Dサウンドパラメーター初期化
      const gfl2::sound::SoundSystem::Sound3DParam sysParam =
      {
        0,
        1.0f,
        11.30f
      };
  
      Sound::Set3DParam( sysParam );
  
      const gfl2::sound::SoundSystem::ListenerParam lisParam =
      {
        200.0f,
        200.0f,
        300.0f,
      };
      Sound::SetListenerParam( lisParam );
      gfl2::math::VEC3 zero(0,0,0);
      for( int i=0;i<5;i++ )
      {
        Sound::Set3DActorVelocity( i , zero );
      }
      Sound::SetListenerVelocity(zero);
      
      mBgSystem = GFL_NEW(GetResidentHeap())btl::BgSystem();
      btl::BgSystem::INIT_PARAM initParam;
      initParam.heap = mBgHeap;
      initParam.workHeap = GetTempHeap();
      initParam.fileManager = mAsyncFileManager;
#if PM_DEBUG
      if( GetDebugParam() && GetDebugParam()->isHioBgResource )
      {
        initParam.useHio = true;
      }
#endif

      //初期化待ちをSetupで行います。
      {
        s32 hour,min,sec;
        GetSkyBoxTime(&hour,&min,&sec);
        mBgSystem->Initialize( initParam,hour,min,sec );
      }

      mSetupSeq = 0;
      mSetupGraphicSeq = 0;
      mCleanupSeq = 0;
      mTermSeq = 0;
      mSequenceSeq = 0;

      mNowWeather = BTL_WEATHER_NONE;

      mWaitCamSickData.mEnable = false;

      mEnableFrameSkip = false;
      mFrameSkipSuspendCnt = 0;

      mRec_IsSkip = false;
      mRec_IsStartSkip = false;

      for (int i = 0; i < LOOKED_MONSNO_NUM; i++)
      {
        mLookedMonsNo[i] = MONSNO_NULL;
      }
    }
    if( IsRecordPlayMode() == false )
    {
      //行動選択のところまで抑制する
      mCanChangePinchBgm = false;
    }
    else
    {
      //録画できる対戦はHP満タンだから最初からONで問題ない。
      mCanChangePinchBgm = true;
    }

    ARIIZUMI_PRINT("Init end0\n");
    BtlvTickEnd(13,"Initialize1(gra)");
    BtlvTickEnd(6,"Initialize1");
    BtlvTickStart(7,"Init2(LoadFont)");
    mInitSeq++;
    //break;
    BtlvTickEnd(7,"Init2(LoadFont)");
    BtlvTickStart(8,"Init3(UI)");
  case 1:
    {
      print::SystemFont_SetLytSystemBattleFont();
    
      if( mUiSys->Initialize( mMsgSpeed ) == false )
      {
        return false;
      }
      BtlvTickEnd(8,"Init2(UI)");
      BtlvTickStart(9,"Init3(WaitChara)");
    }
    ARIIZUMI_PRINT("Init end1\n");
    mInitSeq++;
    //break;
  case 2:
    if( mCharaFactory->WaitInitializeAsync() == false )
    {
      return false;
    }
    if ( Sound::WaitLoadAndPushBattleGlobalSEAsync() == false )
    {
      return false;
    }
    BtlvTickEnd(11,"Initialize1(Snd)");
    ARIIZUMI_PRINT("Init end2\n");
    mInitSeq++;
    //break;
  case 3:
    //if( mDressUpModelResourceManager->WaitInitializeAsync() == false )
    //{
    //  return false;
    //}
    for( int i=0;i<TRAINERMODEL_NUM;i++ )
    {
      mTrainerModel[i] = GFL_NEW(GetTrainerHeap(i))BtlvObject_Chara(this,i,mRenderSys->GetRenderingPipeLine(),mCharaFactory,NULL);
      mObjectManager->AddObject(mTrainerModel[i]);
      mObjectManager->AddObject(mTrainerModel[i]->GetShadowModel(),DemoLib::Obj::OBJ_DEFAULT_PRIORITY+1);
    }
    if (IsBattleFesMode())
    {
      mTrainerModel[0]->SetBattleFesIndex(0);
      mTrainerModel[2]->SetBattleFesIndex(1);
      mTrainerModel[3]->SetBattleFesIndex(2);
    }
    mIsEnableDraw = true;

    ARIIZUMI_PRINT("Init end3\n");
    BtlvTickEnd(9,"Init3(WaitChara)");
    mInitSeq++;
    //break;

  case 4:

    mTurnWatcher = NULL;
#if PM_DEBUG
      InitBtlvDebug(this);
#endif //PM_DEBUG
    BtlvTickEnd(0,"BtlvInitialize");


    mInitSeq++;
    //break;
  case 5:
    //break;
    return true;
  }
  return false;
}

//解放
bool BattleViewSystem::Finalize(void)
{
  if( mInitSeq == 0 )
  {
    //GFL_RELEASE_PRINT("▲Initialize is not call!!\n");
    return true;
  }

  switch( mTermSeq )
  {
  case 0:
    //Initializeで作りっぱなしだったやつの待ち //エラー中断の対応
    if( mBgSystem->IsFinishInitialize() == false || 
      mPokeModelSys->IsFinishInitSystemAsync() == false )
    {
      return false;
    }
    //Setupを通らずに来た時の対応 //エラー中断の対応
    if( mUiSys->CanFinalize() == false )
    {
      return false;
    }
    {
      Sound::PopAndUnloadBattleGlobalSE();
      
      //通信エラー時などはCMD_InitFinishWaitCameraEffectを通らない事があるので対処を入れておく
      GetCameraSys()->EndCameraAnime(false);
      Finish_WaitCamSickEffect();

      mBgFadeModel->RemoveRenderPath();
      mEdgeMaskModel->RemoveRenderPath();
      mFloorMaskModel->RemoveRenderPath();

      for( int i=0;i<TRAINERMODEL_NUM;i++ )
      {
        mTrainerModel[i]->RemoveRenderPath();
      }

      //リソース残しの終了対応
      for( int i=0;i<BTLV_EFF_MODEL_NUM;i++ )
      {
        int objNum;
        BtlvObject_Gfmdl **objArr = GetModelObjManager()->GetObjectArrIdx(i,&objNum);
        if( objArr )
        {
          for( int ii=0;ii<objNum;ii++)
          {
            BtlvObject_Gfmdl *obj = objArr[ii];
            obj->RemoveRenderPath();
          }
        }
      }

      StopWeather(0);
      StopWeather(1);

      //待つ必要があるもの
      mBgSystem->Terminate();
      mTermSeq++;
    }
    break;  //待つからスルーしない
  case 1:
    Update_WaitCamSickEffect(); //WaitCamSickEffectのために必要
    if(mBgSystem->IsFinishTerminate() &&
      IsFinish_WaitCamSickEffect() )
    {
      for( int i=0;i<POKEMODEL_NUM;i++ )
      {
        mObjectManager->RemoveObject(mPokeModel[i]->GetMigawariModel());
        mObjectManager->RemoveObject(mPokeModel[i]);
        mPokeModel[i]->Delete();
        GFL_SAFE_DELETE( mPokeModel[i] );
      }
      mPokeModelSys->DeletePokeModelHeap();
      mPokeModelSys->TermSystemAsync();
      mTermSeq++;
    }
    break;  //待つからスルーしない
  case 2:
    if( mPokeModelSys->IsFinishTermSystemAsync() == false )
    {
      return false;
    }
    {
      //print::SystemFont_DeleteBattleFont();
    }
#if PM_DEBUG
    ExitBtlvDebug(this);
#endif //PM_DEBUG
    //リソース残しの終了対応
    for( int i=0;i<BTLV_EFF_MODEL_NUM;i++ )
    {
      int objNum;
      BtlvObject_Gfmdl **objArr = GetModelObjManager()->GetObjectArrIdx(i,&objNum);
      if( objArr )
      {
        for( int ii=0;ii<objNum;ii++)
        {
          BtlvObject_Gfmdl *obj = objArr[ii];
          obj->Delete();
        }
      }
    }
    SeqComFunc_CrealWazaResource();
    DeleteWeather(0);
    DeleteWeather(1);

    GflHeapSafeFreeMemory( mBattleDataPackBuf );

    //mDressUpModelResourceManager->UnloadDressUpPartsAll();
    //mDressUpModelResourceManager->Finalize();
    //GFL_SAFE_DELETE(mDressUpModelResourceManager);

    GFL_SAFE_DELETE(mPokeModelSys);
    GFL_SAFE_DELETE(mBgSystem);

    GFL_SAFE_DELETE(mClusterObjManager);
    GFL_SAFE_DELETE(mEffectObjManager);
    GFL_SAFE_DELETE(mModelObjManager);

    mObjectManager->RemoveObject(mEdgeMaskModel);
    mEdgeMaskModel->Delete();
    GFL_SAFE_DELETE(mEdgeMaskModel);
    mObjectManager->RemoveObject(mFloorMaskModel);
    mFloorMaskModel->Delete();
    GFL_SAFE_DELETE(mFloorMaskModel);
    mObjectManager->RemoveObject(mBgFadeModel);
    mBgFadeModel->Delete();
    GFL_SAFE_DELETE(mBgFadeModel);

    GflHeapSafeFreeMemory(mDigudaStoneResource);

    for( int i=0;i<BTLV_EFF_SOUND_NUM;i++ )
    {
      mObjectManager->RemoveObject(mSoundObject[i]);
      //リソースを持っているわけではないのでDeleteは無い
      //mSoundObject[i]->Delete();
      GFL_SAFE_DELETE( mSoundObject[i] );
    }

    for( int i=0;i<TRAINERMODEL_NUM;i++ )
    {
      mObjectManager->RemoveObject(mTrainerModel[i]);
      mObjectManager->RemoveObject(mTrainerModel[i]->GetShadowModel());
      mTrainerModel[i]->Delete();
      GFL_SAFE_DELETE( mTrainerModel[i] );
    }

    mCharaFactory->UnloadModelAll();
    mCharaFactory->Finalize();
    GFL_SAFE_DELETE(mCharaFactory);

    GFL_SAFE_DELETE(mResourceManager);
    GFL_SAFE_DELETE(mTaskSystemAlways);
    GFL_SAFE_DELETE(mTaskSystemAfterCamera);
    GFL_SAFE_DELETE(mTaskSystem);
    GFL_SAFE_DELETE(mObjectManager);

    GFL_SAFE_DELETE(mSeqSys);
    mUiSys->Finalize();
    GFL_SAFE_DELETE(mUiSys);

    mCameraSys->Terminate();
    GFL_SAFE_DELETE(mCameraSys);
    mRenderSys->Terminate();
    GFL_SAFE_DELETE(mRenderSys);

    for( int i=0;i<EFFGRP_MAX;i++ )
    {
      GFL_SAFE_DELETE(mEffectGroupIDList[i]);
    }
    GFL_SAFE_DELETE(mEffectSystem);
    GFL_SAFE_DELETE(mEffectHeap);
    GFL_SAFE_DELETE(mParticleIndexManager);

    GFL_SAFE_DELETE( mWordSet );
    GFL_SAFE_DELETE( mWorkStr[1] );
    GFL_SAFE_DELETE( mWorkStr[0] );
      
    str::QuitSystem();
    GFL_SAFE_DELETE(mAppHeap);
    GFL_SAFE_DELETE(mResidentAllocator);

    //待機カメラと状態異常の専用ヒープ削除
    if( mWaitCameraHeap )
    {
      GFL_SAFE_DELETE(mWaitCameraGLAllocator);
      GFL_DELETE_HEAP(mWaitCameraHeap);
      mWaitCameraHeap = NULL;
    }
    if( mWaitSickHeap )
    {
      GFL_DELETE_HEAP(mWaitSickHeap);
      mWaitSickHeap = NULL;
    }

    //トレーナー用
    if( GetBtlvMode() == BTLV_MODE_CAPTURE )
    {
      //捕獲デモはトレーナー2体分減らす
      for( int i=0;i<TRAINER_MODEL_NUM-2;i++ )
      {
        GFL_SAFE_DELETE(mTrainerGLAllocator[i]);
        GFL_DELETE_HEAP(mTrainerHeap[i]);
      }
    }
    else
    {
      for( int i=0;i<TRAINER_MODEL_NUM;i++ )
      {
        GFL_SAFE_DELETE(mTrainerGLAllocator[i]);
        GFL_DELETE_HEAP(mTrainerHeap[i]);
      }
    }

    GFL_DELETE_HEAP(mTrainerCommonHeap);
    //ポケモン用
    for( int i=0;i<POKE_MODEL_NUM;i++ )
    {
      GFL_SAFE_DELETE(mPokeGLAllocator[i]);
      GFL_DELETE_HEAP(mPokeHeap[i]);
    }
    GFL_SAFE_DELETE(mFullPowerAllocator);
    GFL_SAFE_DELETE(mTempGLAllocator);
    GFL_SAFE_DELETE(mResidentGLAllocator);
    GFL_DELETE_HEAP(mWeatherHeap[1]);
    GFL_DELETE_HEAP(mWeatherHeap[0]);
    GFL_DELETE_HEAP(mTempHeap);
    GFL_DELETE_HEAP(mFullPowerHeap);
    GFL_DELETE_HEAP(mBgHeap);
    GFL_DELETE_HEAP(mResidentHeap);
    if( mHeapKeep )
    {
      GFL_DELETE_HEAP(mHeapKeep);
    }
    GFL_DELETE_HEAP(mHeap);

    GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);

    EnableFrameSkip(false);

    return true;
    //break;
  }
  return false;
}

bool BattleViewSystem::FinalizeForce(void)
{
  return true;
}

bool BattleViewSystem::SetupGraphic(void)
{
  enum
  {
    SETUP_RENDER_SYS,
  };
  switch( mSetupGraphicSeq )
  {
  case SETUP_RENDER_SYS:
    if( mRenderSys->Setup() )
    {
      return true;
    }
    break;
  }
  return false;
}

//更新(30フレーム前半用)
void BattleViewSystem::Update(void)
{
  if( mIsEnableDraw == false )
  {
    mRenderSys->Update();
    ARIIZUMI_PRINT("Disable draw!\n");
    return;
  }
#if PM_DEBUG
  UpdateBtlvDebug(this);
#endif
  //フレームスキップ一時停止のチェック
  if( mFrameSkipSuspendCnt > 0 )
  {
    mFrameSkipSuspendCnt--;
    if( mFrameSkipSuspendCnt <= 0 )
    {
      EnableFrameSkip(true);
    }
  }

  UpdateSound();
  Update_WaitCamSickEffect();
  UpdateWaitSequenceSystem();

  //処理優先順位に関して
  //シーケンスの更新　(タスク生成・座標更新
  //タスクの更新　　　(座標更新
  //カメラ更新　　　　(座標反映・ビルボード用Mtx計算
  //ポケモン更新　　　(オブジェクト更新からの座標反映
  //オブジェクトの更新(座標反映・追従・ビルボード適用
  if( IsWaitSequenceSystem() == false )
  {
    mSeqSys->Update();
    mTaskSystem->Update();
    mCameraSys->Update();
    mTaskSystemAfterCamera->Update();
  }
  mTaskSystemAlways->Update();
  mObjectManager->Update();
  //mPokeModelSys->Update();

  mRenderSys->CalcMatrix();
  {
    s32 hour,min,sec;
    GetSkyBoxTime(&hour,&min,&sec);
    mBgSystem->Update(hour,min,sec);
    mBgSystem->WeatherControl();
  }
  mUiSys->Update();
  
  //カメラヒット判定
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( mPokeModel[i] )
    {
      mPokeModel[i]->UpdateCulling();
    }
  }
  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    if( mTrainerModel[i] )
    {
      mTrainerModel[i]->UpdateCulling();
    }
  }
  SortPokeLayerId();
  CheckPinchBgm();
  UpdateLoadFieldWaza();
  UpdateTime();

  mEffectSystem->BeginFrame();
  for( int i=0;i<EFFGRP_MAX;i++ )
  {
    mEffectSystem->Calc(i);
  }

  mRenderSys->Update();

  //3DSさかさま判定関連
  if( mTurnWatcher )
  {
    mTurnWatcher->Update();
  }

  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( mPokeModel[i] )
    {
      mPokeModel[i]->UpdateLookAt();
    }
  }
}

//更新(30フレーム後半用)
void BattleViewSystem::NextFrameUpdate(void)
{

  /*現在処理無し*/
}

//描画更新
void BattleViewSystem::PreDraw(void)
{
  if( mIsEnableDraw == false )
  {
    return;
  }
  mRenderSys->PreDraw();
}
void BattleViewSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if( mIsEnableDraw == false )
  {
    return;
  }

#if PM_DEBUG
  if(mDebugDispLyt)
#endif
  {
    mUiSys->Draw( displayNo );
  }

  mRenderSys->Draw(displayNo);
}

//-------------------------------------------------------
//取得系
//-------------------------------------------------------
BTL_POKEPARAM* BattleViewSystem::GetBattlePokeParam( BtlvPos pos ) const
{
  return mBattleContainer->GetFrontPokeData( ViewPosToBtlPos(pos) );
}
BTL_PARTY* BattleViewSystem::GetBattleParty_Self(void)const
{
  return mBattleContainer->GetPartyData( mBattleClient->GetClientID() );
}
BTL_PARTY* BattleViewSystem::GetBattleParty_Friend(void)const
{
  return mBattleContainer->GetPartyData( mMainModule->GetPlayerFriendCleintID() );
}
BTL_PARTY* BattleViewSystem::GetBattleParty_Enemy(int idx)const
{
  return mBattleContainer->GetPartyData( mMainModule->GetEnemyClientID(idx) );
}
/**
 * @brief プレイヤーの立ち位置が左側かどうかを調べる
 * @retval true   プレイヤーは左側( または, 味方がいない )
 * @retval false  プレイヤーは右側( 左側には味方トレーナーがいる )
 */
bool BattleViewSystem::IsPlayerInLeftSide( void ) const
{
  if( this->IsFriendExist() == false ) {
    return true;  // 味方がいない
  }
  u8 player_clientID = this->GetClientID();
  u8 friend_clientID = this->GetFriendCleintID();
  BtlPokePos player_front_pos = mMainModule->GetClientPokePos( player_clientID, 0 );
  BtlPokePos friend_front_pos = mMainModule->GetClientPokePos( friend_clientID, 0 );
  return ( player_front_pos < friend_front_pos );
}
BtlSound* BattleViewSystem::GetBattleSound(void)
{
  return mMainModule->GetBattleSound();
}

GameSys::GameProc* BattleViewSystem::GetBattleProc(void)const
{
  if( mMainModule )
  {
    return mMainModule->GetBattleProc();
  }
  return NULL;
}

/**
 * @brief 場の状態を取得する
 */
const FieldStatus* BattleViewSystem::GetFieldStatus( void ) const
{
  return mBattleContainer->GetFieldStatusConst();
}


/**
 * @brief サイドエフェクトの状態を取得する
 * @param side    取得対象の陣営
 * @param effect  取得対象のサイドエフェクト
 */
const SideEffectStatus* BattleViewSystem::GetSideEffectStatus( BtlSide side, BtlSideEffect effect ) const
{
  return mBattleContainer->GetSideEffectStatusConst( side, effect );
}

/**
 * @brief 位置エフェクトの状態を取得する
 * @param side    チェック対象の位置
 * @param effect  チェック対象の位置エフェクト
 */
const PosEffectStatus* BattleViewSystem::GetPosEffectStatus( BtlPokePos pos, BtlPosEffect effect ) const
{
  return mBattleContainer->GetPosEffectStatus( pos, effect );
}

const PartyParam* BattleViewSystem::GetPartySetupParam( u8 clientID ) const
{
  if( mMainModule )
  {
    return mMainModule->GetPartySetupParam(clientID);
  }
  return NULL;
}

Savedata::MyItem* BattleViewSystem::GetMyItem(void)const
{
  if( mMainModule )
  {
    return mMainModule->GetItemSaveData();
  }
  return NULL;
}

const Savedata::MyStatus* BattleViewSystem::GetClientPlayerData(u8 clientID, int btlFesIdx ) const
{
  if( mMainModule )
  {
    if (btlFesIdx != -1)
    {
      return mMainModule->GetBattleFesPlayerData(btlFesIdx);
    }
    if (mMainModule->IsClientNPC(clientID))
    {
      return NULL;
    }
    return mMainModule->GetClientPlayerData( clientID );
  }
  return NULL;
}

Savedata::ConfigSave* BattleViewSystem::GetConfigSaveData( void ) const
{
  if( mMainModule )
  {
    return mMainModule->GetConfigSaveData();
  }
  return NULL;
}

const Savedata::ZukanData* BattleViewSystem::GetZukanSaveData(void) const
{
  if (mMainModule)
  {
    return mMainModule->GetZukanSaveData();
  }
  return NULL;
}
const Savedata::ZukanData* BattleViewSystem::GetZukanSaveDataOnBattleStart(void) const
{
  if (mMainModule)
  {
    return mMainModule->GetZukanSaveDataOnBattleStart();
  }
  return NULL;
}
GameSys::GameData* BattleViewSystem::GetGameData(void) const
{
  if (mMainModule)
  {
    return mMainModule->GetGameData();
  }
  return NULL;
}

/**
 * @brief 指定クライアントが既にメガシンカ済みか？
 * @param clientID  チェック対象クライアントのID
 * @retval true  メガシンカ済み
 * @retval false メガシンカ済みでない
 */
bool BattleViewSystem::IsMegaEvolved( u8 clientID ) const
{
  if( mMainModule == NULL )
  {
    return  false;
  }

  return mMainModule->IsMegaEvolved( clientID );
}

/**
 * @brief プレイヤが既にメガシンカ済みか？
 * @retval true  メガシンカ済み
 * @retval false メガシンカ済みでない
 */
bool BattleViewSystem::IsMegaEvolved_Self( void ) const
{
  u8 clientID = this->GetClientID();
  return this->IsMegaEvolved( clientID );
}

/**
* @brief 指定クライアントが既にウルトラバースト済みか？
* @param clientID  チェック対象クライアントのID
* @retval true  ウルトラバースト済み
* @retval false ウルトラバースト済みでない
*/
bool BattleViewSystem::IsUltraBursted(u8 clientID) const
{
  if (mMainModule == NULL)
  {
    return  false;
  }
  return mMainModule->IsUltraBursted(clientID);
}

/**
* @brief プレイヤが既にウルトラバースト済みか？
* @retval true  ウルトラバースト済み
* @retval false ウルトラバースト済みでない
*/
bool BattleViewSystem::IsUltraBursted_Self(void) const
{
  u8 clientID = this->GetClientID();
  return this->IsUltraBursted(clientID);
}

/**
 * @brief 指定したクライアントのオーラ技使用状況を取得する
 * @param clientID  クライアントID
 * @retval NULL  指定したクライアントが存在しない場合
 */
const ZenryokuWazaStatus* BattleViewSystem::GetZenryokuWazaStatus( u8 clientID ) const
{
  if( mMainModule )
  {
    const BTL_CLIENT* client = mMainModule->GetClient( clientID );
    if( client == NULL ) {
      GFL_ASSERT(0);
      return NULL;
    }
    return client->GetZenryokuWazaStatus();
  }

  return  NULL;
}

/**
 * @brief 指定したクライアントが他クライアントに公開している情報を取得する
 * @param clientID  クライアントID
 * @retval NULL  指定したクライアントが存在しない場合
 */
const ClientPublicInformation* BattleViewSystem::GetClientPublicInformation( u8 clientID ) const
{
  if( mMainModule == NULL ) {
    return NULL;
  }

  return mMainModule->GetClientPublicInformation( clientID );
}

/**
 * @brief バトルロイヤルにおける、現在順位を取得する
 * @param clientID  順位を取得するクライアントのID
 * @retval 指定クライアントの順位( 0オリジン、1位なら0 )
 */
u8 BattleViewSystem::GetBattleRoyalRanking( u8 clientID ) const
{
  const RoyalRankingContainer& rankingContainer = mBattleClient->GetRoyalRankingContainer();
  return rankingContainer.GetClientRanking( clientID );
}

const BTL_FIELD_SITUATION* BattleViewSystem::GetFieldSituation(void)
{
  if( mMainModule )
  {
    return mMainModule->GetFieldSituation();
  }
  return NULL;
}

void BattleViewSystem::SetBgSetupParam(btl::BgSystem::SETUP_PARAM *param)
{
  if( GetFieldSituation() )
  {
    param->groundType = GetFieldSituation()->bgComponent.groudnType;
    param->nearType = GetFieldSituation()->bgComponent.nearType;
    param->farType = GetFieldSituation()->bgComponent.farType;
    param->skyType = static_cast<System::Skybox::Skybox::SkyType>(GetFieldSituation()->sky_type);
    param->useSkyBox = GetFieldSituation()->bgComponent.useSkyBox;
  }
  else
  {
    param->groundType = btl::bg::PLATE_GRASS;
    param->nearType = btl::bg::NEAR_GRASS;
    param->farType = btl::bg::FAR_MOUNT;
    param->skyType = System::Skybox::Skybox::SKY_TYPE_SUNNY;
    param->useSkyBox = true;
  }
}

//対戦種類(野生・トレーナー・サブウェイ
BtlCompetitor BattleViewSystem::GetBattleCompetitor(void)const
{
  if( mMainModule )
  {
    return mMainModule->GetCompetitor();
  }
  else
  {
    return BTL_COMPETITOR_WILD;
  }
}

BtlRule BattleViewSystem::GetBattleRule(void)const
{
  if( mMainModule )
  {
    return mMainModule->GetRule();
  }
  else
  {
    return BTL_RULE_SINGLE;
  }
}

BtlCommMode BattleViewSystem::GetCommMode( void ) const  //通信モード
{
  if( mMainModule )
  {
    return mMainModule->GetCommMode();
  }
  else
  {
    return BTL_COMM_NONE;
  }
}

bool BattleViewSystem::IsCommMode(void)const
{
  if( GetCommMode() == BTL_COMM_NONE )
  {
    return false;
  }
  return true;
}

bool BattleViewSystem::IsCommChild(void)const
{
  if( mMainModule )
  {
    if( GetClientID() == BTL_CLIENT_ENEMY1 ||
        GetClientID() == BTL_CLIENT_ENEMY2 )
    {
      return true;
    }
  }
  return false;
}

bool BattleViewSystem::IsMultiMode( void ) const
{
  if( mMainModule )
  {
    return mMainModule->IsMultiMode();
  }
  return false;
}

bool BattleViewSystem::IsBattleFesMode(void) const
{
  //mainmoduleチェックは↓のなかでやってる
  return GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES);
}

//ポケモンがいるか調べる
bool BattleViewSystem::IsPokeExist( BtlvPos pos ) const
{
  if( mMainModule )
  {
    return mMainModule->IsPokePosExist( ViewPosToBtlPos(pos) );
  }
  if( pos < 2 )
  {
    return true;
  }
  return false;  
}

/**
 * @brief 味方トレーナーがいるかどうかを調べる
 * @retval true   味方がいる
 * @retval false  味方はいない
 */
bool BattleViewSystem::IsFriendExist( void ) const
{
  u8 clientID = this->GetFriendCleintID();
  return ( clientID != BTL_CLIENTID_NULL );
}

bool BattleViewSystem::IsClientTrainerExist( u8 clientID ) const
{
  if( mMainModule )
  {
    return mMainModule->IsClientTrainerExist(clientID);
  }
  if( clientID < 2 )
  {
    return true;
  }
  return false;
}

u8 BattleViewSystem::GetClientID(void)const
{
  if( mBattleClient )
  {
    return mBattleClient->GetClientID();
  }
  return 0;
}
u8 BattleViewSystem::GetFriendCleintID( void ) const
{
  if( mMainModule )
  {
    return mMainModule->GetPlayerFriendCleintID();
  }
  return 0;
}

u8 BattleViewSystem::GetEnemyClientID( u8 idx ) const
{
  if( mMainModule )
  {
    return mMainModule->GetEnemyClientID( idx );
  }
  return 0;
}

/**
 * @brief 敵クライアントの数を取得する
 * @param clientID  自分のクライアントID
 */
u8 BattleViewSystem::GetEnemyClientNum( u8 clientID ) const
{
  if( mMainModule == NULL ) {
    return 0;
  }
  return mMainModule->GetEnemyClientNum( clientID );
}

bool BattleViewSystem::IsClientNpc( u8 idx ) const
{
  if( mMainModule )
  {
    return mMainModule->IsClientNPC(idx);
  }
  return (idx==0?true:false);
}

BtlvPos BattleViewSystem::GetClientIdToBtlvPos( u8 clientId , u8 pokeIdx ) const
{
  if( mMainModule )
  {
    return mMainModule->BtlPosToViewPos(mMainModule->GetClientPokePos(clientId,pokeIdx));
  }
  return static_cast<BtlvPos>(clientId);
}

u8 BattleViewSystem::GetBtlvPosToClientId( BtlvPos vpos ) const
{
  if( mMainModule )
  {
    return mMainModule->BtlPosToClientID(mMainModule->ViewPosToBtlPos(vpos));
  }
  return vpos%2;
}

u8 BattleViewSystem::GetBtlvPosToPosIdx( BtlvPos vpos ) const
{
  if( mMainModule )
  {
    u8 clientID, posIdx;
    mMainModule->BtlPosToClientID_and_PosIdx(mMainModule->ViewPosToBtlPos(vpos), &clientID, &posIdx);
    return posIdx;
  }
  return vpos%2;
}
u8 BattleViewSystem::GetBtlvPosToTrainerIdx( BtlvPos vpos ) const
{
  if( mMainModule )
  {
    u8 clientId = GetBtlvPosToClientId( vpos );
    return GetClientIdToBtlvPos(clientId);
  }
  return vpos%2;
}
u16 BattleViewSystem::GetTrainerID( u8 clientID ) const
{
  return mMainModule->GetClientTrainerID( clientID );
}
u16 BattleViewSystem::GetTrainerType( u8 clientID ) const
{
  if( mMainModule )
  {
    return mMainModule->GetClientTrainerType( clientID );
  }
  return 0;
}

/**
 * @brief クライアントのレートを表示するか？
 * @retval true   表示する
 * @retval false  表示しない
 */
bool BattleViewSystem::IsClientRatingDisplay( void ) const
{
  return mMainModule->IsPlayerRatingDisplay();
}

/**
 * @brief 指定したクライアントのレーティングを取得する
 * @param clientID  クライアントID
 *
 * @attention 通信対戦でのみ有効です
 */
u16 BattleViewSystem::GetClientRating( u8 clientID ) const
{
  return mMainModule->GetClientRating( clientID );
}

u8 BattleViewSystem::GetFrontPosCount_Self( void ) const
{
  u8 clientID = this->GetClientID();
  return mMainModule->GetClientFrontPosCount( clientID );
}
u8 BattleViewSystem::GetFrontPosCount_Friend( void ) const
{
  u8 clientID = this->GetFriendCleintID();
  return mMainModule->GetClientFrontPosCount( clientID );
}

u8 BattleViewSystem::GetBenchPosIndex_Self() const
{
  u8 clientID = this->GetClientID();
  return mMainModule->GetClientBenchPosIndex( clientID );
}

u8 BattleViewSystem::GetBenchPosIndex_Friend() const
{
  u8 clientID = this->GetFriendCleintID();
  return mMainModule->GetClientBenchPosIndex( clientID );
}

bool BattleViewSystem::IsSkyBattle(void) const
{
  return mMainModule->IsSkyBattle();
}

bool BattleViewSystem::IsZukanRegistered( const BTL_POKEPARAM* bpp ) const
{
  return mMainModule->IsZukanRegistered(bpp);
}

bool BattleViewSystem::IsZukanLooked(const BTL_POKEPARAM* bpp) const
{
  //バトル開始時図鑑の参照にしておく。バトル内の見たチェックは別途行う
  const Savedata::ZukanData *zknData = GetZukanSaveDataOnBattleStart();
  if (zknData)
  {
    return zknData->GetPokeSeeFlag(static_cast<MonsNo>(bpp->GetMonsNo()));
  }
  //図鑑がない場合見たことにしておく
  return true;
}

bool BattleViewSystem::IsEnableWazaEffect(void) const
{
  return mMainModule->IsWazaEffectEnable();
}

bool BattleViewSystem::IsItemEnable(void) const
{
  return mMainModule->IsItemEnable();
}

bool BattleViewSystem::GetSetupStatusFlag( BTL_STATUS_FLAG flag ) const
{
  if( mMainModule )
  {
    return mMainModule->GetSetupStatusFlag(flag);
  }
  return false;
}

bool BattleViewSystem::GetEnableTimeStop(void) const
{
  if( mMainModule )
  {
    return mMainModule->GetEnableTimeStop();
  }
  return false;
}

bool BattleViewSystem::GetEnableVoiceChat(void) const
{
  if( mMainModule )
  {
    return mMainModule->GetEnableVoiceChat();
  }
  return false;
}

bool BattleViewSystem::IsRecordPlayMode() const
{
  if( mMainModule )
  {
    return mMainModule->IsRecordPlayMode();
  }
  return false;
}

//  にげるモードを取得
BtlEscapeMode BattleViewSystem::GetEscapeMode()
{
  if( mMainModule )
  {
    return  mMainModule->GetEscapeMode();
  }

  return  BTL_ESCAPE_MODE_MAX;
}

/**
 * @brief 経過ターン数を取得する
 */
u32 BattleViewSystem::GetTurnCount( void ) const
{
  if( mBattleClient )
  {
    return mBattleClient->GetTurnCount();
  }
  return 0;
}

/**
 * @brief 技エフェクト再生が有効か？
 */
bool BattleViewSystem::IsWazaEffectEnable(void)const
{
  if( mMainModule )
  {
    return mMainModule->IsWazaEffectEnable();
  }
  return true;
}


void BattleViewSystem::GetSkyBoxTime(s32 *hour,s32 *min,s32 *sec)
{
  if( GetFieldSituation() )
  {
    if( GetFieldSituation()->fix_time )
    {
      *hour = GetFieldSituation()->hour;
      *min = GetFieldSituation()->minute;
      *sec = 0;
      return;
    }
  }

  GameSys::AlolaTime time;
  time.Initialize();
  *hour = time.GetHour();
  *min = time.GetMinute();
  *sec = time.GetSecond();
}

bool BattleViewSystem::IsUnselectableWaza( const BTL_POKEPARAM* bpp, WazaID waza ) const
{
  return mBattleClient->IsUnselectableWaza( bpp, waza );
}
const BSP_ENCOUNT_EFFECT* BattleViewSystem::GetEncountEffect( void )
{
  if( mMainModule )
  {
    return mMainModule->GetEncountEffect();
  }
  return NULL;
}

bool BattleViewSystem::IsQuickStart(void)
{
#if PM_DEBUG
  if( mMainModule && mMainModule->GetDebugFlag(BTL_DEBUGFLAG_SKIP_BTLIN)  )
  {
    return true;
  }
#endif
  return false;
}

//代表ビースト戦か？
bool BattleViewSystem::IsBeastBattle(void)
{
  if( GetSetupStatusFlag( BTL_STATUS_FLAG_VS_BOSS ) )
  {
    return mMainModule->GetEncountEffect();
  }
  return false;
}

void BattleViewSystem::EnableFrameSkip(bool flg)
{
  if( mEnableFrameSkip != flg )
  {
     mEnableFrameSkip = flg;
    //VRAM転送中にやるとヤバイ(天球のテクスチャ配置とポケモン生成時のSetupShader時
    if( flg == true )
    {
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->SetEnableSkipDrawMode(GameSys::GameManager::SKIP_DRAWMODE_CPU_OVERFLOW_LIGHT);
    }
    else
    {
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->SetEnableSkipDrawMode(GameSys::GameManager::SKIP_DRAWMODE_NONE);
    }
  }
}

//ポケモン読み替えのため数フレーム止める
void BattleViewSystem::SuspendFrameSkip(void)
{
  EnableFrameSkip(false);
  mFrameSkipSuspendCnt = 3;

}

void BattleViewSystem::GetStringUi(gfl2::str::StrBuf* dst, u16 strID )
{
  str::GetUIString( dst , strID );
}

/**
 * @brief 指定クライアントのトレーナー名を取得する
 * @param[out] grammer   取得したトレーナー名の文法特性
 * @param      clientID  トレーナー名を取得したいクライアントのID
 * @return 指定クライアントのトレーナー名
 */
const gfl2::str::StrBuf* BattleViewSystem::GetClientTrainerName( print::GrammaticalAttribute* grammer, u8 clientID )
{
  return mMainModule->GetClientTrainerName( clientID, grammer );
}

//陣営別のサイズ取得
PokeTool::POKE_SIZE BattleViewSystem::GetPokeSize(bool isEnemy)
{
  switch( GetBattleRule() )
  {
  case BTL_RULE_SINGLE:    ///< シングル
    if( isEnemy )
    {
      return GetPokeModel(BTL_VPOS_FAR_1)->GetSize();
    }
    else
    {
      return GetPokeModel(BTL_VPOS_NEAR_1)->GetSize();
    }
  case BTL_RULE_INTRUDE:    ///< シングル
    if( isEnemy )
    {
      return gfl2::math::Max( GetPokeModel(BTL_VPOS_FAR_1)->GetSize(),GetPokeModel(BTL_VPOS_FAR_2)->GetSize());
    }
    else
    {
      return GetPokeModel(BTL_VPOS_NEAR_1)->GetSize();
    }
  case BTL_RULE_DOUBLE:    ///< ダブル
    if( isEnemy )
    {
      return gfl2::math::Max( GetPokeModel(BTL_VPOS_FAR_1)->GetSize(),GetPokeModel(BTL_VPOS_FAR_2)->GetSize());
    }
    else
    {
      return gfl2::math::Max( GetPokeModel(BTL_VPOS_NEAR_1)->GetSize(),GetPokeModel(BTL_VPOS_NEAR_2)->GetSize());
    }
  case BTL_RULE_ROYAL:    ///< ロイヤル
    {
      PokeTool::POKE_SIZE ret = PokeTool::POKE_SIZE_S;
      for( int i=0;i<POKE_MODEL_NUM;i++ )
      {
        ret = gfl2::math::Max( GetPokeModel((BtlvPos)i)->GetSize(),ret);
      }
      return ret;
    }
  }
  return PokeTool::POKE_SIZE_M;
}

void BattleViewSystem::GetDefaultPokePos( BtlvPos vPos , gfl2::math::Vector3 *pos , int *deg )
{
  {
/*
    static const gfl2::math::Vector3 vacArr[poke_tool::POKE_SIZE_MAX][poke_tool::POKE_SIZE_MAX] =
    {
      {gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f)},
      {gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f)},
      {gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f)},
    };
*/

  const PokeTool::POKE_SIZE mPos = GetPokeSize(false);
  const PokeTool::POKE_SIZE ePos = GetPokeSize(true);


    switch( GetBattleRule() )
    {
    case BTL_RULE_SINGLE:    ///< シングル
      {
        switch(vPos)
        {
        case BTL_VPOS_NEAR_1:
          {
            static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
            {
              gfl2::math::Vector3(0.0f,0.0f,200.0f),
              gfl2::math::Vector3(0.0f,0.0f,220.0f),
              gfl2::math::Vector3(0.0f,0.0f,250.0f)
            };
            pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
            *deg = 180;
          }
          break;
        case BTL_VPOS_FAR_1:
          {
            static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
            {
              gfl2::math::Vector3(0.0f,0.0f,-200.0f),
              gfl2::math::Vector3(0.0f,0.0f,-220.0f),
              gfl2::math::Vector3(0.0f,0.0f,-250.0f)
            };
            pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
            *deg = 0;
          }
          break;
        }
      }
      break;
    case BTL_RULE_DOUBLE:    ///< ダブル
      switch(vPos)
      {
      case BTL_VPOS_NEAR_1:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(-100.0f,0.0f,200.0f),
            gfl2::math::Vector3(-150.0f,0.0f,220.0f),
            gfl2::math::Vector3(-190.0f,0.0f,250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 180;
        }
        break;
      case BTL_VPOS_FAR_1:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(100.0f,0.0f,-200.0f),
            gfl2::math::Vector3(150.0f,0.0f,-220.0f),
            gfl2::math::Vector3(190.0f,0.0f,-250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 0;
        }
        break;
      case BTL_VPOS_NEAR_2:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(100.0f,0.0f,200.0f),
            gfl2::math::Vector3(150.0f,0.0f,220.0f),
            gfl2::math::Vector3(190.0f,0.0f,250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 180;
        }
        break;
      case BTL_VPOS_FAR_2:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(-100.0f,0.0f,-200.0f),
            gfl2::math::Vector3(-150.0f,0.0f,-220.0f),
            gfl2::math::Vector3(-190.0f,0.0f,-250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 0;
        }
      }
      break;
    case BTL_RULE_ROYAL:    ///< ロイヤル
      {
        //サイズ変動なし
        static const f32 lenArr[PokeTool::POKE_SIZE_NUM] = {350.0f,350.0f,350.0f};
        static const f32 len = lenArr[mPokeModel[vPos]->GetSize()];
        switch(vPos)
        {
        case BTL_VPOS_ROYAL_NEAR:
          pos->Set( 0,0,len);
          *deg = 180;
          break;
        case BTL_VPOS_ROYAL_RIGHT:
          {
            pos->Set( len,0,0);
            *deg = 270;
          }
          break;
        case BTL_VPOS_ROYAL_FAR:
          {
            pos->Set( 0,0,-len);
            *deg = 0;
          }
          break;
        case BTL_VPOS_ROYAL_LEFT:
          {
            pos->Set(-len,0,0);
            *deg = 90;
          }
          break;
        }
      }
      break;
    case BTL_RULE_INTRUDE:    ///< 乱入
      switch(vPos)
      {
      case BTL_VPOS_NEAR_1:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(0.0f,0.0f,200.0f),
            gfl2::math::Vector3(0.0f,0.0f,220.0f),
            gfl2::math::Vector3(0.0f,0.0f,250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 180;
        }
        break;
      case BTL_VPOS_FAR_1:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(100.0f,0.0f,-200.0f),
            gfl2::math::Vector3(150.0f,0.0f,-220.0f),
            gfl2::math::Vector3(190.0f,0.0f,-250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 0;
        }
        break;
      case BTL_VPOS_NEAR_2:
        {
          //いない！
        }
        break;
      case BTL_VPOS_FAR_2:
        {
          static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM] =
          {
            gfl2::math::Vector3(-100.0f,0.0f,-200.0f),
            gfl2::math::Vector3(-150.0f,0.0f,-220.0f),
            gfl2::math::Vector3(-190.0f,0.0f,-250.0f)
          };
          pos->Set( posArr[mPokeModel[vPos]->GetSize()] );
          *deg = 0;
        }
      }
      break;
    }
  }
}

void BattleViewSystem::ResetPokemon( BtlvPos vPos , int frame )
{
  gfl2::math::Vector3 pos;
  gfl2::math::Vector3 scl(1.0f,1.0f,1.0f);
  int deg;

  BtlvObject_PokeModel *poke = GetPokeModel(vPos);
  GetDefaultPokePos(vPos,&pos,&deg);

  gfl2::math::Vector3 rot(0,deg,0);
  
  poke->SetRotationOrder(gfl2::math::EulerRotation::RotationOrderZYX);
  if( frame == 0 )
  {
    poke->SetPosition(pos);
    poke->SetRotate(rot);
    poke->SetScale(scl);
  }
  else
  {
    DemoLib::Obj::CreateTask_ObjectMove(GetTaskSystem(),GetTempHeap(),poke,poke->GetPosition(),pos,frame);
    DemoLib::Obj::CreateTask_ObjectRotate(GetTaskSystem(),GetTempHeap(),poke,poke->GetRotate(),rot,frame);
    DemoLib::Obj::CreateTask_ObjectScale(GetTaskSystem(),GetTempHeap(),poke,poke->GetScale(),scl,frame);
  }

}

void BattleViewSystem::GetDefaultCamera( gfl2::math::Vector3 *pos , gfl2::math::Vector3 *trg , f32 *fov )
{
  const PokeTool::POKE_SIZE mPos = GetPokeSize(false);
  const PokeTool::POKE_SIZE ePos = GetPokeSize(true);

  *fov = CAMERA_FOVY;
  switch( GetBattleRule() )
  {
  case BTL_RULE_SINGLE:    ///< シングル
    {
      //自分が縦、相手が横
      //s_s,s_m,s_b
      //m_s,m_m,m_b
      //b_s,b_m,b_b
      static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM][PokeTool::POKE_SIZE_NUM] =
      {
        {gfl2::math::Vector3(125.0f,50.0f,360.0f),gfl2::math::Vector3(155.0f,56.0f,380.0f),gfl2::math::Vector3(185.0f,62.0f,400.0f)},
        {gfl2::math::Vector3(236.0f,110.0f,460.0f),gfl2::math::Vector3(256.0f,106.0f,465.0f),gfl2::math::Vector3(276.0f,102.0f,470.0f)},
        {gfl2::math::Vector3(220.0f,150.0f,470.0f),gfl2::math::Vector3(240.0f,140.0f,485.0f),gfl2::math::Vector3(260.0f,130.0f,500.0f)},
      };
      static const gfl2::math::Vector3 trgArr[PokeTool::POKE_SIZE_NUM][PokeTool::POKE_SIZE_NUM] =
      {
        {gfl2::math::Vector3(-50.0f,50.0f,-40.0f),gfl2::math::Vector3(-45.0f,56.0f,-40.0f),gfl2::math::Vector3(-40.0f,62.0f,-40.0f)},
        {gfl2::math::Vector3(-50.0f,60.0f,-20.0f),gfl2::math::Vector3(-45.0f,70.0f,-20.0f),gfl2::math::Vector3(-40.0f,80.0f,-20.0f)},
        {gfl2::math::Vector3(0.0f,130.0f,90.0f),gfl2::math::Vector3(0.0f,130.0f,90.0f),gfl2::math::Vector3(0.0f,130.0f,90.0f)},
      };
      *fov = CAMERA_FOVY;
      trg->Set( trgArr[mPos][ePos] );
      pos->Set( posArr[mPos][ePos] );
    }
    break;
  case BTL_RULE_DOUBLE:    ///< ダブル
  case BTL_RULE_INTRUDE:    ///< 乱入
    {
      //自分が縦、相手が横
      //s_s,s_m,s_b
      //m_s,m_m,m_b
      //b_s,b_m,b_b
      static const gfl2::math::Vector3 posArr[PokeTool::POKE_SIZE_NUM][PokeTool::POKE_SIZE_NUM] =
      {
        {gfl2::math::Vector3(180.0f,130.0f,480.0f),gfl2::math::Vector3(200.0f,130.0f,530.0f),gfl2::math::Vector3(220.0f,120.0f,580.0f)},
        {gfl2::math::Vector3(220.0f,220.0f,600.0f),gfl2::math::Vector3(260.0f,180.0f,630.0f),gfl2::math::Vector3(300.0f,160.0f,660.0f)},
        {gfl2::math::Vector3(380.0f,340.0f,620.0f),gfl2::math::Vector3(360.0f,320.0f,640.0f),gfl2::math::Vector3(400.0f,300.0f,700.0f)},
      };
      static const gfl2::math::Vector3 trgArr[PokeTool::POKE_SIZE_NUM][PokeTool::POKE_SIZE_NUM] =
      {
        {gfl2::math::Vector3(-40.0f, 20.0f,30.0f),gfl2::math::Vector3(-25.0f,40.0f,0.0f),gfl2::math::Vector3(-10.0f, 70.0f,-30.0f)},
        {gfl2::math::Vector3(-40.0f, 40.0f,30.0f),gfl2::math::Vector3(-40.0f,80.0f,0.0f),gfl2::math::Vector3(-40.0f, 100.0f,-40.0f)},
        {gfl2::math::Vector3(-100.0f, 0.0f,0.0f),gfl2::math::Vector3(-80.0f,0.0f,-20.0f),gfl2::math::Vector3(-70.0f, 30.0f,-50.0f)},
      };
      *fov = CAMERA_FOVY;
      trg->Set( trgArr[mPos][ePos] );
      pos->Set( posArr[mPos][ePos] );
    }
    break;
  case BTL_RULE_ROYAL:    ///< ロイヤル
    {
      //サイズ変動なし
      *fov = CAMERA_FOVY;
      trg->Set( 0,0,-100 );
      pos->Set( 300,250,750 );
    }
    break;
  }
}

void BattleViewSystem::ResetDefaultCamera(int frame)
{
  gfl2::math::Vector3 pos,trg;
  f32 fov;
  GetDefaultCamera(&pos,&trg,&fov);

  GetCameraSys()->SetNear(NEAR_PLANE);
  GetCameraSys()->SetFar(FAR_PLANE);
  if( frame == 0 )
  {
    ResetZenryokuTrainerMotion();
    GetCameraSys()->SetPosition(pos,trg);
    GetCameraSys()->SetFov(fov);
    GetCameraSys()->SetTwist(0);
    ResetHitBack();
  }
  else
  {
    CreateTask_MoveCamera(GetTaskSystem(),GetTempHeap(),this,pos,trg,fov,0,frame);
  }
}

//一応カメラ関係なのでここ
void BattleViewSystem::ResetHitBack(void)
{
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( mPokeModel[i] && mPokeModel[i]->GetHitBackFlg() )
    {
      mPokeModel[i]->SetHitBackFlg(false);
      mPokeModel[i]->SetOffset(0,0,0);
    }
  }
}

void BattleViewSystem::GetFollowCameraPos( f32 camLen , gfl2::math::Vector3 *ret )
{
  gfl2::math::Vector3 camPos = GetCameraSys()->GetPositionAnime() + GetCameraSys()->GetOffset();
  gfl2::math::Vector3 camTrg = GetCameraSys()->GetTargetAnime() + GetCameraSys()->GetOffset();
  const gfl2::math::Vector3 camOfs = camPos - camTrg;
  f32 len = camOfs.Length();
  f32 rate = camLen/len;

  gfl2::math::Vector3 posOfs = camOfs * rate;
  *ret = camPos - posOfs;
}

void BattleViewSystem::GetFollowCameraRot( gfl2::math::Vector3 *ret )
{
  const gfl2::math::Vector3 ofs = GetCameraSys()->GetPositionAnime() - GetCameraSys()->GetTargetAnime();
  const f32 len =  gfl2::math::Vector2(ofs.x,ofs.z).Length();
    
  gfl2::math::Vector3 rot;
  ret->x = gfl2::math::Atan2Deg(-ofs.y, len );
  ret->y = gfl2::math::Atan2Deg(ofs.x , ofs.z );
  ret->z = 0;

  //GFL_PRINT("[%d][%d][%d]\n", (int)GFL_MATH_RAD_TO_DEG(ret->x),(int)GFL_MATH_RAD_TO_DEG(ret->y),(int)GFL_MATH_RAD_TO_DEG(ret->z));
}

BTLV_LAND_TYPE BattleViewSystem::CheckLandType(BtlvPos pos )
{
  BTL_POKEPARAM* bpp = GetBattlePokeParam( pos );

  if( bpp->GetItem() == ITEM_HUUSEN ||
    GetPokeModel(pos)->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) == false )
  {
    return BTLV_LAND_NONE;
  }

  const pml::pokepara::PokemonParam* pp = GetBattlePokeParam( pos )->GetViewSrcData();
  pml::personal::LoadPersonalData( pp->GetMonsNo() , pp->GetFormNo() ); 
  const u32 weight = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_weight );
  
  if( weight >= 2000 )
  {
    return BTLV_LAND_HEAVY;
  }
  else
  if( weight >= 1000 )
  {
    return BTLV_LAND_LIGHT_HEAVY;
  }
  else
  if( weight >= 500 )
  {
    return BTLV_LAND_MIDDLE;
  }
  return BTLV_LAND_LIGHT;
}

//Rotom関係
bool BattleViewSystem::IsEnableRotom(void)const
{
  if (mMainModule == NULL)
  {
    return false;
  }
  return mMainModule->IsEnableRotom();
}

Savedata::FieldMenu::RotomRank BattleViewSystem::GetRotomRank(void)const
{
  if (mMainModule == NULL)
  {
    return Savedata::FieldMenu::ROTOM_RANK_1;
  }
  return mMainModule->GetRotomRank();
}

//Rotom導入を再生するか？(乱入時も)
bool BattleViewSystem::IsEnableRotomSearch(BTL_POKEPARAM *target)
{
  if (IsEnableRotom() == false)
  {
    //図鑑入手前
    return false;
  }
  if (GetBattleCompetitor() != BTL_COMPETITOR_WILD)
  {
    //野生以外
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NUSI))
  {
    //ぬし戦
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_VS_ULTRA_BEAST))
  {
    //UB戦
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_IN_POKE_SANCTUARY))
  {
    //保護区で捕獲不可
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_SIREN_BATTLE))
  {
    //試練中で捕獲不可
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NEKUROZUMA))
  {
    //ネクロズマ戦
    return false;
  }
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_NO_CAPTURE))
  {
    //捕獲不可戦闘
    return false;
  }

  if (IsZukanLooked(target) == true)
  {
    //すでに知っている
    return false;
  }

  //BTL_SETUP_FLAGの残数が不安なのでMonsNoでブロックする
  if (target->GetMonsNo() == MONSNO_THIKISAKANA ||
      target->GetMonsNo() == MONSNO_THIKITORI ||
      target->GetMonsNo() == MONSNO_THIKITYOU ||
      target->GetMonsNo() == MONSNO_THIKIUSI ||
      target->GetMonsNo() == MONSNO_NIKKOU ||
      target->GetMonsNo() == MONSNO_GEKKOU ||
      target->GetMonsNo() == MONSNO_ZIGARUDE)
  {
    //ティキ、ルナソル、ジガルデは普通の戦闘なので上に引っかからない
    return false;
  }

  int localZukanNo = PokeTool::ExtendData::GetLocalNumberStatic(target->GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A);
  if (localZukanNo == PokeTool::ExtendData::LOCAL_NUMBER_NONE)
  {
    //図鑑番号が範囲外なので出さない
    return false;
  }

  for (int i = 0; i < LOOKED_MONSNO_NUM; i++)
  {
    if (target->GetMonsNo() == mLookedMonsNo[i])
    {
      //すでにそのバトルでは見た
      return false;
    }
  }

  return true;
}

//図鑑セーブを見るとそのバトル内で見たものが記録されないので、バトルで管理
void BattleViewSystem::RegistLookedMonsNo(BTL_POKEPARAM *bpp)
{
  for (int i = 0; i < LOOKED_MONSNO_NUM; i++)
  {
    if (mLookedMonsNo[i] == MONSNO_NULL)
    {
      mLookedMonsNo[i] = bpp->GetMonsNo();
      return;
    }
  }
  GFL_ASSERT_MSG(0, "Btlv:Looked monsno is over!!\n");//@check 
  //最悪ここにきても、そのバトル内で見たポケモンにもう一度ロトム演出が出るだけ
}

void BattleViewSystem::ResetTrainer(int idx)
{
  gfl2::math::Vector3 pos;
  int deg;
  GetDefaultTrainerPos(idx,&pos,&deg);

  mTrainerModel[idx]->SetPosition(pos);
  mTrainerModel[idx]->SetRotateY(deg);
}

//トレーナー初期座標
void BattleViewSystem::GetDefaultTrainerPos( int idx , gfl2::math::Vector3 *pos , int *deg , bool isOrigin )
{
  //ラスボス特殊対応
  if( idx == 1 )
  {
    bool isLastBoss = false;
#if PM_DEBUG
    BTLV_DEBUG_PARAM *debParam = GetDebugParam();
    if( debParam && debParam->mTrainerType[1] == CHARA_MODEL_ID_BTTR0035_00 )
    {
      isLastBoss = true;
    }
    else
#endif
    if( GetTrainerType(1) == trainer::TRTYPE_BEAST )
    {
      isLastBoss = true;
    }
    if( isLastBoss == true )
    {
      *deg = 0;
      pos->Set(100.0f,0.0f,-620.0f);
      return;
    }
  }

  //バトルフェス例外対応(控えトレーナー
  if (IsBattleFesMode() == true && idx >=2)
  {
    *deg = 180;
    if (idx == 2)
    {
      pos->Set(-200.0f, 0, 800.0f);
    }
    else
    {
      pos->Set(-300.0f, 0, 800.0f);
    }
    return;
  }

  if( idx % 2 == 0 )
  {
    *deg = 180;
  }
  else
  {
    *deg = 0;
  }

  //長いポケモンとのチェック
  f32 len = 0.0f;
  if( isOrigin == false )
  {
    gfl2::math::Vector3 pokePos;
    int pokeDeg;
    BtlvObject_PokeModel *poke = GetPokeModel(static_cast<BtlvPos>(idx));
    GetDefaultPokePos(static_cast<BtlvPos>(idx),&pokePos,&pokeDeg);
    f32 pokePosLen;
    if( GetBattleRule() == BTL_RULE_ROYAL && 
        (idx==BTL_VPOS_ROYAL_LEFT || idx==BTL_VPOS_ROYAL_RIGHT ))
    {
      //ロイヤルの左右だけX座標
      pokePosLen = gfl2::math::FAbs(pokePos.x);
    }
    else
    {
      pokePosLen = gfl2::math::FAbs(pokePos.z);
    }

    if( poke->IsEnable() )
    {
      gfl2::math::AABB box;
      bool ret = poke->GetCollision(&box);
      if( ret == true )
      {
        len = box.GetMin().z * -1;
        len += pokePosLen;
      }
    }
    //ダブルは隣も見る
    if( GetBattleRule() == BTL_RULE_DOUBLE )
    {
      poke = GetPokeModel(static_cast<BtlvPos>(idx+2));
      GetDefaultPokePos(static_cast<BtlvPos>(idx+2),&pokePos,&pokeDeg);
      if( poke->IsEnable() )
      {
        gfl2::math::AABB box;
        bool ret = poke->GetCollision(&box);
        if( ret == true )
        {
          f32 tempLen = box.GetMin().z * -1;
          tempLen += gfl2::math::FAbs(pokePos.z);
          len = gfl2::math::max(len,tempLen);
        }
      }
    }

    //トレーナーサイズの加味
    BtlvObject_Chara *chara = GetTrainerModelObj(idx);
    if( chara && chara->IsEnable() )
    {
      len += chara->GetTrainerData()->mMaxZ;
    }
  }

  switch( GetBattleRule() ) // 
  {
  case BTL_RULE_SINGLE:    ///< シングル
  case BTL_RULE_INTRUDE:    ///< 乱入
    {
      len = gfl2::math::max(len,500.0f);
      static const gfl2::math::Vector3 posArr[TRAINER_MODEL_NUM] =
      {
        //Z座標は掛け算で使うので1or-1
        gfl2::math::Vector3( -70.0f,0, 1.0f),
        gfl2::math::Vector3(  70.0f,0,-1.0f),
        gfl2::math::Vector3( -70.0f,0, 1.0f),
        gfl2::math::Vector3(  70.0f,0,-1.0f),
      };
      *pos = posArr[idx];
      pos->z *= len;
    }
    break;
  case BTL_RULE_DOUBLE:    ///< ダブル
    {
      bool isMulti = false;
      if( (idx%2 == 0 && IsClientTrainerExist(BTL_VPOS_TR_NEAR_2) ) || 
          (idx%2 == 1 && IsClientTrainerExist(BTL_VPOS_TR_FAR_2) ) )
      {
        isMulti = true;
      }

      if( isMulti )
      {
        len = gfl2::math::max(len,580.0f);
        static const gfl2::math::Vector3 posArr[TRAINER_MODEL_NUM] =
        {
          //Z座標は掛け算で使うので1or-1
          gfl2::math::Vector3( -100.0f,0, 1.0f),
          gfl2::math::Vector3(  100.0f,0,-1.0f),
          gfl2::math::Vector3(  100.0f,0, 1.0f),
          gfl2::math::Vector3( -100.0f,0,-1.0f),
        };
        *pos = posArr[idx];
        pos->z *= len;
      }
      else
      {
        len = gfl2::math::max(len,500.0f);
        static const gfl2::math::Vector3 posArr[TRAINER_MODEL_NUM] =
        {
          //Z座標は掛け算で使うので1or-1
          gfl2::math::Vector3( 0.0f,0, 1.0f),
          gfl2::math::Vector3( 0.0f,0,-1.0f),
          gfl2::math::Vector3( 0.0f,0, 1.0f),
          gfl2::math::Vector3( 0.0f,0,-1.0f),
        };
        *pos = posArr[idx];
        pos->z *= len;
      }
    }
    break;
  case BTL_RULE_ROYAL:    ///< ロイヤル
    {
      {
        //サイズ変動なし
        len = gfl2::math::max(len,600.0f);
        switch(idx)
        {
        case 0:
          pos->Set(-70.0f,0,len);
          *deg = 180;
          break;
        case 1:
          pos->Set( len,0, 70.0f);
          *deg = 270;
          break;
        case 2:
          pos->Set( 70.0f,0,-len);
          *deg = 0;
          break;
        case 3:
          pos->Set(-len,0,-70.0f);
          *deg = 90;
          break;
        }
      }
    }
    break;
  }
}

//-------------------------------------------------------
//数値変換
//-------------------------------------------------------
BtlvPos BattleViewSystem::BtlPosToViewPos( BtlPokePos pos )const
{
  return mMainModule->BtlPosToViewPos(pos);
}
BtlPokePos BattleViewSystem::ViewPosToBtlPos( BtlvPos pos )const
{
  return mMainModule->ViewPosToBtlPos(pos);
}

/**
 * @brief 数値変換　BtlvPos --> BtlSide
 */
BtlSide BattleViewSystem::ViewPosToBtlSide( BtlvPos viewPos ) const
{
  if( mMainModule )
  {
    BtlRule    rule = mMainModule->GetRule();
    BtlPokePos pos  = this->ViewPosToBtlPos( viewPos );
    return MainModule::PosToSide( rule, pos );
  }
  else
  {
    if( viewPos == BTL_VPOS_NEAR_1 )
    {
      return BTL_SIDE_1ST;
    }
    else
    {
      return BTL_SIDE_2ND;
    }
  }
}

BtlvPos BattleViewSystem::PokeIDToViewPos( int pos )const
{
  return mMainModule->PokeIDtoViewPos( mBattleContainer , pos );
}

//自分側？
bool BattleViewSystem::IsPlayerSide(BtlvPos pos )
{
  BtlRule rule = mMainModule->GetRule();
  return mMainModule->IsPlayerSide( mMainModule->PosToSide( rule, ViewPosToBtlPos(pos) ) );
}



//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、天候のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 *
 * @note 天候が永続する場合、戻り値が 0/0 になります。
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_Turn_Weather( u32* numerator, u32* denominator ) const
{
  *numerator = 0;
  *denominator = 0;

  const FieldStatus* fieldStatus = this->GetFieldStatus();
  const BtlWeather weather = fieldStatus->GetWeather();

  if( weather == BTL_WEATHER_NONE )
  {
    return;
  }

  const u8  myClientID  = this->GetClientID();
  const u8  causePokeID = fieldStatus->GetWeatherCausePokeID();
  const u32 totalTurn   = fieldStatus->GetWeatherWholeTurn();
  const u32 upTurn      = fieldStatus->GetWeatherTurnUpCount();
  const u32 remainTurn  = fieldStatus->GetWeatherRemainingTurn();
  const u32 passedTurn  = fieldStatus->GetWeatherPassedTurn();

  // 永続
  if( totalTurn >= BTL_WEATHER_TURN_PERMANENT )
  {
    return;
  }

  this->GetUiDisplay_Turn( numerator, denominator, myClientID, causePokeID, totalTurn, upTurn, remainTurn, passedTurn );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、地形効果のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_Turn_Ground( u32* numerator, u32* denominator ) const
{
  *numerator = 0;
  *denominator = 0;

  const FieldStatus* fieldStatus = this->GetFieldStatus();
  const BtlGround ground = fieldStatus->GetGround();

  if( ground == BTL_GROUND_NONE )
  {
    return;
  }

  const u8  myClientID  = this->GetClientID();
  const u8  causePokeID = fieldStatus->GetGroundCausePokeID();
  const u32 totalTurn   = fieldStatus->GetGroundWholeTurn();
  const u32 upTurn      = fieldStatus->GetGroundTurnUpCount();
  const u32 remainTurn  = fieldStatus->GetGroundRemainingTurn();
  const u32 passedTurn  = fieldStatus->GetGroundPassedTurn();

  this->GetUiDisplay_Turn( numerator, denominator, myClientID, causePokeID, totalTurn, upTurn, remainTurn, passedTurn );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、「ひかりのかべ」効果のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 * @param      side        「ひかりのかべ」が存在する陣営
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_Turn_Hikarinokabe( u32* numerator, u32* denominator, BtlSide side ) const
{
  this->GetUiDisplay_Turn_SideEffect( numerator, denominator, side, BTL_SIDEEFF_HIKARINOKABE );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、「リフレクター」効果のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 * @param      side        「リフレクター」が存在する陣営
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_Turn_Reflector( u32* numerator, u32* denominator, BtlSide side ) const
{
  this->GetUiDisplay_Turn_SideEffect( numerator, denominator, side, BTL_SIDEEFF_REFRECTOR );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、「オーロラベール」効果のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 * @param      side        「オーロラベール」が存在する陣営
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_Turn_AuroraVeil( u32* numerator, u32* denominator, BtlSide side ) const
{
  this->GetUiDisplay_Turn_SideEffect( numerator, denominator, side, BTL_SIDEEFF_AURORAVEIL );
}

/**
 * @brief UIに表示する、サイドエフェクト効果のターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 * @param      side         サイドエフェクトが存在する陣営
 * @param      sideEffect   サイドエフェクトの種類
 */
void BattleViewSystem::GetUiDisplay_Turn_SideEffect( u32* numerator, u32* denominator, BtlSide side, BtlSideEffect sideEffect ) const
{
  *numerator = 0;
  *denominator = 0;

  if( ( sideEffect != BTL_SIDEEFF_HIKARINOKABE ) &&
      ( sideEffect != BTL_SIDEEFF_REFRECTOR ) &&
      ( sideEffect != BTL_SIDEEFF_AURORAVEIL ) )
  {
    return;
  }

  const SideEffectStatus* sideEffectStatus = this->GetSideEffectStatus( side, sideEffect );

  if( !sideEffectStatus->IsEffective() )
  {
    return;
  }

  const u8  myClientID  = this->GetClientID();
  const u8  causePokeID = sideEffectStatus->GetCausePokeID();
  const u32 totalTurn   = sideEffectStatus->GetMaxTurnCount();
  const u32 upTurn      = sideEffectStatus->GetTurnUpCount();
  const u32 remainTurn  = sideEffectStatus->GetRemainingTurn();
  const u32 passedTurn  = sideEffectStatus->GetCurrentTurnCount();

  this->GetUiDisplay_Turn( numerator, denominator, myClientID, causePokeID, totalTurn, upTurn, remainTurn, passedTurn );
}

/**
 * @brief UIに表示する、効果ターン数を取得する
 * @param[out] numerator    分子
 * @param[out] denominator  分母
 * @param      myClientID   自分のクライアントID
 * @param      causePokeID  効果を引き起こしたポケモンのID
 * @param      totalTurn    効果の総ターン数
 * @param      upTurn       効果の総ターン数の内、ターン数の増加分( 効果を引き起こしたクライアントのみが知り得る情報 )
 * @param      remainTurn   効果の残りターン数
 * @param      passedTurn   効果の経過ターン数
 */
void BattleViewSystem::GetUiDisplay_Turn( u32* numerator, u32* denominator, u8 myClientID, u8 causePokeID, u32 totalTurn, u32 upTurn, u32 remainTurn, u32 passedTurn ) const
{
  *numerator = 0;
  *denominator = 0;

  if( mMainModule == NULL )
  {
    return;
  }

  // 誰によって引き起こされたのか不明なら、隠さずに表示する
  if( causePokeID == BTL_POKEID_NULL )
  {
    *numerator = remainTurn;
    *denominator = totalTurn;
    return;
  }

  const BtlSide causePokeSide = mMainModule->PokeIDtoSide( causePokeID );
  const BtlSide mySide        = mMainModule->GetClientSide( myClientID );

  // 効果が味方陣営のポケモンによって引き起こされたものなら、隠さずに表示する
  if( mMainModule->IsFriendClientID( mySide, causePokeSide ) )
  {
    *numerator = remainTurn;
    *denominator = totalTurn;
    return;
  }

  // 効果が敵陣営のポケモンによって引き起こされたものなら、ターン数の増加分を隠して表示する

  const u32 defaultTurn = totalTurn - upTurn;

  if( passedTurn < defaultTurn )
  {
    // デフォルトターン数が経過するまでは、増加分を隠す
    *numerator = remainTurn - upTurn;
    *denominator = defaultTurn;
  }
  else
  {
    // デフォルトターン数が経過した後は、増加分を隠さない
    *numerator = remainTurn;
    *denominator = totalTurn;
  }
  
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、ポケモンのタイプを取得する
 * @param[out] type1  表示するタイプ1( タイプが存在しない場合 POKETYPE_NULL )
 * @param[out] type2  表示するタイプ2( タイプが存在しない場合 POKETYPE_NULL )
 * @param      poke   表示対象のポケモン
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::GetUiDisplay_PokeType( pml::PokeType* type1, pml::PokeType* type2, const BTL_POKEPARAM& poke ) const
{
  // 基本は、バトルパラメータをそのまま表示する
  const PokeTypePair typePair = poke.GetPokeType();
  *type1 = PokeTypePair_GetType1( typePair );
  *type2 = PokeTypePair_GetType2( typePair );

  if( mMainModule == NULL )
  {
    return;
  }

  // 表示対象が敵陣営のイリュージョン中ポケモンなら、イリュージョン先のポケモンのタイプを表示する
  // 「みずびたし」を受けた状態でも、イリュージョン先のポケモンのタイプが表示されるため、イリュージョンであることがバレてしまいますが許容します。
  const u8      pokeID     = poke.GetID();
  const BtlSide pokeSide   = mMainModule->PokeIDtoSide( pokeID );
  const u8      myClientID = this->GetClientID();
  const BtlSide mySide     = mMainModule->GetClientSide( myClientID );

#if PM_DEBUG
  //@fix GFNMCat[#5752 【観戦機能】観戦ROMで、大会演出表示がされていない]
  if( mMainModule->GetBattleSetupParam()->isLiveRecPlayMode == true )
  {//観戦ROMだったらイリュージョンの中身見えなくする
    if( poke.IsFakeEnable() )
    {
      const pml::pokepara::PokemonParam* fakeTargetPoke = poke.GetViewSrcData();
      *type1 = fakeTargetPoke->GetType1();
      *type2 = fakeTargetPoke->GetType2();
    }

    return;
  }
#endif

  if( ( pokeSide != mySide ) &&
      ( poke.IsFakeEnable() ) )
  {
    const pml::pokepara::PokemonParam* fakeTargetPoke = poke.GetViewSrcData();
    *type1 = fakeTargetPoke->GetType1();
    *type2 = fakeTargetPoke->GetType2();
  }  
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、技のタイプを取得する
 *
 * @note 
 * 汎用ｚ技のパラメータは、元となった技によって変化します。
 * 専用ｚ技のパラメータは、元となった技に関わらず一定です。
 * この関数を使用することで、これらの差異を吸収することが出来ます。
 *
 * @param orgWaza  ｚ技の元となった技の技ID( 表示対象がｚ技でないなら、通常技の技ID )
 * @param zWaza    ｚ技の技ID( 表示対象がｚ技でないなら WAZANO_NULL )
 */
//-----------------------------------------------------------------------------
pml::PokeType BattleViewSystem::GetUiDisplay_WazaType( WazaID orgWaza, WazaID zWaza ) const
{
  if( orgWaza == WAZANO_NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  // ｚ技は、ｚ技の技データを参照する
  if( tables::IsZWaza( zWaza ) ) 
  {
    return WAZADATA_GetParam( zWaza, pml::wazadata::PARAM_ID_TYPE );
  }
  
  return WAZADATA_GetParam( orgWaza, pml::wazadata::PARAM_ID_TYPE );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、技の威力を取得する
 *
 * @note 
 * 汎用ｚ技のパラメータは、元となった技によって変化します。
 * 専用ｚ技のパラメータは、元となった技に関わらず一定です。
 * この関数を使用することで、これらの差異を吸収することが出来ます。
 *
 * @param orgWaza  ｚ技の元となった技の技ID( 表示対象がｚ技でないなら、通常技の技ID )
 * @param zWaza    ｚ技の技ID( 表示対象がｚ技でないなら WAZANO_NULL )
 */
//-----------------------------------------------------------------------------
u16 BattleViewSystem::GetUiDisplay_WazaPower( WazaID orgWaza, WazaID zWaza ) const
{
  if( orgWaza == WAZANO_NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  // 汎用ｚ技は、元となった技のデータを参照する
  if( tables::IsZWaza_General( zWaza ) )
  {
    int power = WAZADATA_GetParam( orgWaza, pml::wazadata::PARAM_ID_ZENRYOKU_POWER );
    return static_cast<u16>( power );
  }
 
  // 専用ｚ技は、専用ｚ技の技データを参照する
  if( tables::IsZWaza_Special( zWaza ) )
  {
    int power = WAZADATA_GetParam( zWaza, pml::wazadata::PARAM_ID_POWER );
    return static_cast<u16>( power );
  }

  int power = WAZADATA_GetParam( orgWaza, pml::wazadata::PARAM_ID_POWER );
  return static_cast<u16>( power );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、技の命中を取得する
 *
 * @note 
 * 汎用ｚ技のパラメータは、元となった技によって変化します。
 * 専用ｚ技のパラメータは、元となった技に関わらず一定です。
 * この関数を使用することで、これらの差異を吸収することが出来ます。
 *
 * @param orgWaza  ｚ技の元となった技の技ID( 表示対象がｚ技でないなら、通常技の技ID )
 * @param zWaza    ｚ技の技ID( 表示対象がｚ技でないなら WAZANO_NULL )
 */
//-----------------------------------------------------------------------------
u16 BattleViewSystem::GetUiDisplay_WazaHitPer( WazaID orgWaza, WazaID zWaza ) const
{
  if( orgWaza == WAZANO_NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }
 
  // ｚ技は、ｚ技の技データを参照する
  if( tables::IsZWaza( zWaza ) )
  {
    int hitper = WAZADATA_GetParam( zWaza, pml::wazadata::PARAM_ID_HITPER );
    return static_cast<u16>( hitper );
  }

  int hitper = WAZADATA_GetParam( orgWaza, pml::wazadata::PARAM_ID_HITPER );
  return static_cast<u16>( hitper );
}

//-----------------------------------------------------------------------------
/**
 * @brief UIに表示する、技のダメージタイプを取得する
 *
 * @note 
 * 汎用ｚ技のパラメータは、元となった技によって変化します。
 * 専用ｚ技のパラメータは、元となった技に関わらず一定です。
 * この関数を使用することで、これらの差異を吸収することが出来ます。
 *
 * @param orgWaza  ｚ技の元となった技の技ID( 表示対象がｚ技でないなら、通常技の技ID )
 * @param zWaza    ｚ技の技ID( 表示対象がｚ技でないなら WAZANO_NULL )
 */
//-----------------------------------------------------------------------------
WazaDamageType BattleViewSystem::GetUiDisplay_WazaDamageType( WazaID orgWaza, WazaID zWaza ) const
{
  // 汎用ｚ技は、元となった技のデータを参照する
  if( tables::IsZWaza_General( zWaza ) )
  {
    return WAZADATA_GetDamageType( orgWaza );
  }
 
  // 専用ｚ技は、専用ｚ技の技データを参照する
  if( tables::IsZWaza_Special( zWaza ) )
  {
    return WAZADATA_GetDamageType( zWaza );
  }

  return WAZADATA_GetDamageType( orgWaza );
}

//-----------------------------------------------------------------------------
/**
 * @brief 技の相性を表示可能かチェックする
 *
 * @param  defPokeID  防御側ポケモンのID
 *
 * @retval true   相性を表示してよい
 * @retval false  相性を表示してはいけない
 */
//-----------------------------------------------------------------------------
bool BattleViewSystem::IsWazaAffinityDisplayEnable( u8 defPokeID ) const
{
  // バトルロイヤルなら、必ず表示する
  const BtlRule rule = this->GetBattleRule();
  if( rule == BTL_RULE_ROYAL )
  {
    return true;
  }

  // バトル施設・通信対戦なら、必ず表示する
  const BtlCompetitor competitor = mMainModule->GetCompetitor();
  if( ( competitor == BTL_COMPETITOR_INST ) ||
      ( competitor == BTL_COMPETITOR_COMM ) )
  {
    return true;
  }

  // 相手のポケモンについて、バトル開始時点で見たことがあるなら、表示する
  const BTL_POKEPARAM* pokeParam = mBattleContainer->GetPokeParamConst( defPokeID );
  const MonsNo monsno = static_cast<MonsNo>( pokeParam->GetMonsNo() );
  const Savedata::ZukanData* zukanData = mMainModule->GetZukanSaveDataOnBattleStart();
  return zukanData->GetPokeSeeFlag( monsno );
}

//-----------------------------------------------------------------------------
/**
 * @brief UI に表示するための、技の相性を計算する
 *
 * @param      atkPokeID        攻撃側ポケモンのID
 * @param      defPokeID        防御側ポケモンのID
 * @param      wazaNo           使用する技
 * @param[out] isDisplayEnable  相性表示を表示するか？
 *
 * @return  技の相性
 */
//-----------------------------------------------------------------------------
btl::BtlTypeAffAbout BattleViewSystem::CalcWazaAffinityAbout( u8 atkPokeID, u8 defPokeID, WazaID waza, bool* isDisplayEnable )
{
  // momiji限定？
  // @fix MMCat[768] 真ネクロズマ戦にて初戦闘にも関わらず技の相性がわかる
  // ヒカリドラゴン戦の場合、表示しない
  if (GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NEKUROZUMA))
  {
#if PM_DEBUG
    const BTL_POKEPARAM* pokeParam = mBattleContainer->GetPokeParamConst(defPokeID);
    GFL_ASSERT(pokeParam->GetMonsNo() == MONSNO_PURIZUMU);
    GFL_ASSERT(pokeParam->GetFormNo() == FORMNO_PURIZUMU_FORM_N);
#endif
    *isDisplayEnable = false;
    return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
  }

  // 表示条件を満たしていない
  if( !this->IsWazaAffinityDisplayEnable( defPokeID ) )
  {
    *isDisplayEnable = false;
    return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
  }

  // 変化技なら表示しない
  WazaDamageType damageType = pml::wazadata::GetDamageType( waza );
  if( damageType == pml::wazadata::DMG_NONE ) 
  {
    *isDisplayEnable = false;
    return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
  }

  // 物理、特殊でも特定の技は表示しない
  if( tables::IsNotDisplayUiAffinity( waza ) )
  {
    *isDisplayEnable = false;
    return  pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
  }

  *isDisplayEnable = true;

  // 防御側の特性等を考慮しないで相性計算する
  BtlTypeAff affinity = mMainModule->SimulationWazaAffinityOnlyAttacker( atkPokeID, defPokeID, waza );
  BtlTypeAffAbout affinityAbout = calc::TypeAffAbout( affinity );
  return affinityAbout;
}

//-----------------------------------------------------------------------------
/**
 * @brief 「お手入れ」ボタンが押されたことを通知する
 */
//-----------------------------------------------------------------------------
void BattleViewSystem::NotifyOteireButtonPressed( void )
{
  mMainModule->NotifyOteireButtonPressed();
}


//-------------------------------------------------------------------------
/**
 * @brief 戦闘終了後の「お手入れ」が可能か？
 * @retval true   戦闘後、「お手入れ( ポケパルレ )」への遷移が可能
 * @retval false  戦闘後、「お手入れ( ポケパルレ )」へ遷移してはいけない
 */
//-------------------------------------------------------------------------
bool BattleViewSystem::IsOteireEnable( void ) const
{
  return mMainModule->IsOteireEnable();
}

/*--------------------------------------------------------------------------------------------------*/
/* 文字列パラメータ設定                                                                             */
/*--------------------------------------------------------------------------------------------------*/
void BTLV_STRPARAM_Setup( BTLV_STRPARAM* sp, BtlStrType strType, u16 strID )
{
  int i;
  for(i=0; i<GFL_NELEMS(sp->args); ++i){
    sp->args[i] = 0;
  }
  sp->argCnt = 0;
  sp->strID = strID;
  sp->strType = strType;
  sp->wait = BTLV_MSGWAIT_NONE;
}
void BTLV_STRPARAM_AddArg( BTLV_STRPARAM* sp, int arg )
{
  if( sp->argCnt < GFL_NELEMS(sp->args) ){
    sp->args[ sp->argCnt++ ] = arg;
  }
}
void BTLV_STRPARAM_SetWait( BTLV_STRPARAM* sp, u8 wait )
{
  sp->wait = wait;
}
//-------------------------------------------------------
//技エフェクト用
//-------------------------------------------------------
void BattleViewSystem::ResetZenryokuParam(void)
{
  mZenryokuParam.isZenryoku = false;
  mZenryokuParam.isAttack = false;
  mZenryokuParam.isUnique = false;
  mZenryokuParam.seq = WAZANO_NULL;
}

//全力の変化・汎用でトレーナーが決めポーズのままとまっている。
//それをリセットさせる。条件は下記
//・カメラが0フレで移動された時
//・シーケンスが終わってリソースが開放されるタイミングに来た時(保険
//・失敗した時で失敗専用シーケンスが流れない時
void BattleViewSystem::ResetZenryokuTrainerMotion(void)
{
  if( mReserveResetZenryokuTrainer )
  {
    BtlvObject_Chara *chara = GetTrainerModelObj(mReserveResetZenryokuTrainerTarget);
    if( chara->GetDressUpModel() )
    {
      if( chara->GetIsDressUpAnime() )
      {
        chara->FreeDressUpAnimationNodes();
        chara->ClearDressUpAnimation();
      }
    }
    chara->ChangeAnimation(TRAINER_ANIME_WAIT1);
    mReserveResetZenryokuTrainer = false;
  }
}


//上昇下降エフェクトの連続再生を抑制するため
bool BattleViewSystem::CheckCanPlayEffect( BtlvPos vpos, u16 effectNo )
{
  if( effectNo > BTLEFF_MAX )
  {
    //BTLEFF_CAMERA_INIT = 2000,
    //BTLEFF_CAMERA_INIT_ORTHO = 2001,
    //BTLEFF_CAMERA_INIT_ORTHO_NO_WAIT = 2002,
    return false;
  }

  if( effectNo == BTLEFF_STATUS_UP ||
      effectNo == BTLEFF_STATUS_DOWN ||
      effectNo == BTLEFF_PP_RECOVER )
  {
    if( mBefPlayEffPos == vpos &&
        mBefPlayEffNo == effectNo )
    {
      ARIIZUMI_PRINT("SkipEffect!![%d][%d]\n",vpos,effectNo);
      return false;
    }
  }
  mBefPlayEffPos = vpos;
  mBefPlayEffNo = effectNo;
  return true;
}

void BattleViewSystem::ResetCheckCanPlayEffect(void)
{
  mBefPlayEffPos = BTL_VPOS_ERROR;
  mBefPlayEffNo = BTLEFF_MAX;
}


//技エフェクト再生
void BattleViewSystem::StartWazaEffect(void)
{
  //mComScene は外で使ってる
  mComSceneSub = 0;

  BtlvPos vpos = BtlPosToViewPos(mWazaParam.mAtkPos);
  if( mWazaParam.mId == WAZANO_BUNMAWASU &&
      GetPokeModel(vpos)->IsEnable() == false )
  {
    //該当ポケモンが死んでいるか退場済み
    //[fix]NMCat[4034]: 「ぶんまわす」の処理中に「ニードルガード」のダメージで「ききかいひ」が発動するととまる
    //ロジックの対処が難しいため、エフェクト再生側でケアをして止まらないようにする
    //対象範囲を狭めるため「ぶんまわす」のみに限定しておく
    //@attention もしかすると自爆のダメージ部は発動者死亡で来ている可能性がある。修正時は注意
    mComSceneSub = 200;
    return;
  }

  mComTargetPoke[0] = BtlPosToViewPos(mWazaParam.mAtkPos);
  if( mWazaParam.mDefPos == BTL_POS_NULL )
  {
    mComTargetPoke[1] = BTL_VPOS_ERROR;
  }
  else
  {
    mComTargetPoke[1] = BtlPosToViewPos(mWazaParam.mDefPos);
  }

  mIsMigawariWaza = GetPokeModel(mComTargetPoke[0])->IsVisibleMigawari();
  
  if(( mWazaParam.mId == WAZANO_ZIBAKU || 
       mWazaParam.mId == WAZANO_DAIBAKUHATU ) &&
       mWazaParam.mTurnType == 1 )
  {
    //じばく・だいばくはつのダメージ部は身代わりを入れ替えしない。
    mIsMigawariWaza = false;
  }

  mTargetTrainer[0] = GetBtlvPosToTrainerIdx(mComTargetPoke[0]);

  /*
  SetStartFieldEffect(false);
  SetVisibleRareEffect(false);

#if BTLV_AUTO_DEBUG
  if( GetBtlvDebugWork() && 
      GetBtlvDebugWork()->IsEnable() )
  {
    GetBtlvDebugWork()->SetPlayWazaNo(mWazaParam.mId);
  }
#endif
  */
}

void BattleViewSystem::StartWazaEffectCore(void)
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    mComTargetDamage[i] = BTL_VPOS_ERROR;
  }

  const BtlvPos atkPos = BtlPosToViewPos(mWazaParam.mAtkPos);
  BtlvPos defPos;
  if( mWazaParam.mDefPos == BTL_POS_NULL )
  {
    defPos = BTL_VPOS_ERROR;
  }
  else
  {
    defPos = BtlPosToViewPos(mWazaParam.mDefPos);
  }
  BtlvPos tempArr[BTL_EXIST_POS_MAX];
  BtlvPos comCheckArr[BTL_EXIST_POS_MAX];

  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    tempArr[i] = BTL_VPOS_ERROR;
    comCheckArr[i] = BTL_VPOS_ERROR;
  }

  switch( mWazaParam.mRange )
  {
  case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
  case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
  case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
  case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
  case pml::wazadata::TARGET_ENEMY_RANDOM:       ///< 相手ポケ１体ランダム
    tempArr[0] = BtlPosToViewPos(mWazaParam.mDefPos);
    mComTargetPoke[1] = BtlPosToViewPos(mWazaParam.mDefPos);
    break;

  case pml::wazadata::TARGET_OTHER_ALL:          ///< 自分以外の全ポケ
    //(なみのりとか)
    {
      switch( GetBattleRule() )
      {
      case BTL_RULE_SINGLE:    ///< シングル
        {
          BtlvPos arr[2]={BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
          tempArr[0] = arr[atkPos];
        }
        break;
      case BTL_RULE_DOUBLE:    ///< ダブル
        {
          BtlvPos arr[4]={BTL_VPOS_FAR_2,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[4]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_2};
          for( int i=0;i<4;i++ )
          {
            if( arr[i] != atkPos )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;
      case BTL_RULE_INTRUDE:    ///< 乱入
        {
          BtlvPos arr[4]={BTL_VPOS_FAR_2,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[3]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_FAR_2};
          for( int i=0;i<3;i++ )
          {
            if( arr[i] != atkPos )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;
      case BTL_RULE_ROYAL:    ///< ロイヤル
        //処理的にはダブルと一緒だが、一応定義が違うので分けておく
        {
          BtlvPos arr[4]={BTL_VPOS_ROYAL_NEAR,BTL_VPOS_ROYAL_RIGHT,BTL_VPOS_ROYAL_FAR,BTL_VPOS_ROYAL_LEFT};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[4]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_2};
          for( int i=0;i<4;i++ )
          {
            if( arr[i] != atkPos )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;
      }
      break;
    case pml::wazadata::TARGET_ENEMY_ALL:          ///< 相手側全ポケ
      //(ふぶきとか)
      switch( GetBattleRule() )
      {
      case BTL_RULE_SINGLE:    ///< シングル
        {
          BtlvPos arr[2]={BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
          tempArr[0] = arr[atkPos];
        }
        break;
      case BTL_RULE_DOUBLE:    ///< ダブル
        {
          BtlvPos arr[4]={BTL_VPOS_FAR_2,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[4]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_2};
          for( int i=0;i<4;i++ )
          {
            if( IsPlayerSide(arr[i]) != IsPlayerSide(atkPos) )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;
      case BTL_RULE_ROYAL:    ///< ロイヤル
        {
          BtlvPos arr[4]={BTL_VPOS_ROYAL_NEAR,BTL_VPOS_ROYAL_RIGHT,BTL_VPOS_ROYAL_FAR,BTL_VPOS_ROYAL_LEFT};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[4]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_2};
          for( int i=0;i<4;i++ )
          {
            if( arr[i] != atkPos )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;
     case BTL_RULE_INTRUDE:    ///< 乱入
        {
          BtlvPos arr[4]={BTL_VPOS_FAR_2,BTL_VPOS_NEAR_2,BTL_VPOS_FAR_1,BTL_VPOS_NEAR_1};
          mComTargetPoke[1] = arr[atkPos];
        }
        {
          BtlvPos arr[3]={BTL_VPOS_NEAR_1,BTL_VPOS_FAR_1,BTL_VPOS_FAR_2};
          for( int i=0;i<3;i++ )
          {
            if( IsPlayerSide(arr[i]) != IsPlayerSide(atkPos) )
            {
              tempArr[i] = arr[i];
            }
          }
        }
        break;      }
    }
    break;
  case pml::wazadata::TARGET_FRIEND_ALL:         ///< 味方側全ポケ
    {
      mComTargetPoke[1] = atkPos;
      switch( GetBattleRule() )
      {
      case BTL_RULE_SINGLE:    ///< シングル
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        break;
      case BTL_RULE_DOUBLE:    ///< ダブル
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        comCheckArr[2] = BTL_VPOS_NEAR_2;
        comCheckArr[3] = BTL_VPOS_FAR_2;
        break;
      case BTL_RULE_ROYAL:    ///< ロイヤル
        //ロイヤルは自分以外全員敵。
        comCheckArr[0] = atkPos;
        break;
      case BTL_RULE_INTRUDE:    ///< 乱入
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        comCheckArr[2] = BTL_VPOS_FAR_2;
        break;
      }

      for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
      {
        if( comCheckArr[i] != BTL_VPOS_ERROR &&
            ViewPosToBtlSide(comCheckArr[i]) == ViewPosToBtlSide(atkPos) )
        {
          tempArr[i] = comCheckArr[i];
        }
      }
    }
    break;

  case pml::wazadata::TARGET_USER:               ///< 自分のみ
    //がまんは攻撃になる。
    tempArr[0] = defPos;
    mComTargetPoke[1] = defPos;
    break;

  case pml::wazadata::TARGET_UNKNOWN:            ///< ゆびをふるなど特殊型
    tempArr[0] = mComTargetPoke[1];
    mComTargetPoke[1] = mComTargetPoke[1];
    break;

  case pml::wazadata::TARGET_ALL:                ///< 場に出ている全ポケ
  case pml::wazadata::TARGET_FIELD:              ///< 場全体（天候など）
  case pml::wazadata::TARGET_SIDE_ENEMY:         ///< 敵側陣営
  case pml::wazadata::TARGET_SIDE_FRIEND:        ///< 自分側陣営
    {
      //TARGET_SIDE_ENEMYの時は下でチェックして入れなおす。
      mComTargetPoke[1] = atkPos;
      switch( GetBattleRule() )
      {
      case BTL_RULE_SINGLE:    ///< シングル
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        break;
      case BTL_RULE_DOUBLE:    ///< ダブル
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        comCheckArr[2] = BTL_VPOS_NEAR_2;
        comCheckArr[3] = BTL_VPOS_FAR_2;
        break;
      case BTL_RULE_ROYAL:    ///< ロイヤル
        comCheckArr[0] = BTL_VPOS_ROYAL_NEAR;
        comCheckArr[1] = BTL_VPOS_ROYAL_RIGHT;
        comCheckArr[2] = BTL_VPOS_ROYAL_FAR;
        comCheckArr[3] = BTL_VPOS_ROYAL_LEFT;
        break;
      case BTL_RULE_INTRUDE:    ///< 乱入
        comCheckArr[0] = BTL_VPOS_NEAR_1;
        comCheckArr[1] = BTL_VPOS_FAR_1;
        comCheckArr[2] = BTL_VPOS_FAR_2;
        break;
      }
      for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
      {
        if( comCheckArr[i] != BTL_VPOS_ERROR )
        {
          if( mWazaParam.mRange == pml::wazadata::TARGET_ALL ||
              mWazaParam.mRange == pml::wazadata::TARGET_FIELD )
          {
            tempArr[i] = comCheckArr[i];
          }
          else
          if( mWazaParam.mRange == pml::wazadata::TARGET_SIDE_ENEMY )
          {
            if( ViewPosToBtlSide(comCheckArr[i]) != ViewPosToBtlSide(atkPos) )
            {
              if( mComTargetPoke[1] == atkPos )
              {
                mComTargetPoke[1] = comCheckArr[i];
              }
              tempArr[i] = comCheckArr[i];
            }
          }
          else
          if( mWazaParam.mRange == pml::wazadata::TARGET_SIDE_FRIEND )
          {
            if( ViewPosToBtlSide(comCheckArr[i]) == ViewPosToBtlSide(atkPos) )
            {
              tempArr[i] = comCheckArr[i];
            }
          }
        }
      }
    }
    break;
  default:
    GFL_ASSERT_MSG(0,"未対応範囲[%d]\n",mWazaParam.mRange); //@fix 通常の単体わざと同じ挙動
    tempArr[0] = BtlPosToViewPos(mWazaParam.mDefPos);
    mComTargetPoke[1] = BtlPosToViewPos(mWazaParam.mDefPos);
    break;
  }

  int idx = 0;
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( tempArr[i] != BTL_VPOS_ERROR )
    {
      if( GetPokeModel(tempArr[i]) != NULL &&
          GetPokeModel(tempArr[i])->IsEnable() )
      {
        mComTargetDamage[idx] = tempArr[i];
        idx++;
      }
    }
  }

  if( idx == 0 )
  {
    //自爆は対象がいなくても動く・・・
    GFL_PRINT("対象がいない\n");
#if defined(DEBUG_ONLY_FOR_ariuizumi_nobuhiko)
    GFL_ASSERT_MSG(0,"対象がいない\n");//@check 有泉専用
#endif
  }

  GFL_ASSERT_MSG(mWazaParam.mId<800,"技の番号が仮データの数を越えました。技[%d]\n",mWazaParam.mId);
  int playSeq;

  if( mZenryokuParam.isZenryoku == true && mZenryokuParam.seq != WAZANO_NULL )
  {
    playSeq = mZenryokuParam.seq;
  }
  else
  {
    playSeq = GetWazaSequenceFile( mWazaParam.mId , mWazaParam.mTurnType );
  }
  //sango追加 レックウザの時だけ特殊処理
  if( mWazaParam.mId == WAZANO_GARYOUTENSEI )
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if( atkPoke->GetMonsNo() == MONSNO_REKKUUZA &&
        atkPoke->GetFormNo() == FORMNO_REKKUUZA_DELTA )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
  }
  //niji追加 サトシゲッコウガの時だけ特殊処理
  //本来は威力も変わるので、ロジックでやるべきだが、連続攻撃数でmTurnTypeを使っているので、こちらで対応
  if( mWazaParam.mId == WAZANO_MIZUSYURIKEN )
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if( atkPoke->GetMonsNo() == MONSNO_GEKKOUGA &&
        atkPoke->GetFormNo() == FORMNO_GEKKOUGA_SATOSHI )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
    else
    {
      //mTurnTypeには１が入ってくるので通常の方も分岐が必要。
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 0 );
    }
  }  
  //niji追加 ガオガエンの時だけ特殊処理
  if( mWazaParam.mId == WAZANO_ddRARIATTO )
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if( atkPoke->GetMonsNo() == MONSNO_TORA3 )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
  }  
  //niji追加 ジュナイパーの時だけ特殊処理
  if( mWazaParam.mId == WAZANO_KAGENUI )
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if( atkPoke->GetMonsNo() == MONSNO_HUKUROU3 )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
  }  
  //だんがいのつるぎ軽量版
#if 0
  //NMCat3979 豪華版と軽量版があるが、間違えて豪華版をロイヤルだけで使っていて止まりになった。
  //シングルは豪華版を再生するのが正しいが、チェックされていないのでそのまま軽量版を使う。
  //ロイヤルでも豪華版を出ないようにするためこの処理自体をコメントアウト
  if( mWazaParam.mId == WAZANO_DANGAINOTURUGI )
  {
    //0の方が軽量版 1の方が豪華版
    if( GetBattleRule() == BTL_RULE_SINGLE )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
  }
#endif
  //niji追加 ルソルガレオの時だけ特殊処理
  if (mWazaParam.mId == WAZANO_METEODORAIBU)
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if (atkPoke->GetMonsNo() == MONSNO_NIKKOU)
    {
      playSeq = GetWazaSequenceFile(mWazaParam.mId, 1);
    }
    //momiji追加 ネクソル用
    if (atkPoke->GetMonsNo() == MONSNO_PURIZUMU &&
        atkPoke->GetFormNo() == FORMNO_PURIZUMU_FORM_S )
    {
      playSeq = GetWazaSequenceFile(mWazaParam.mId, 2);
    }
  }
  //niji追加 ルナアーラの時だけ特殊処理
  if( mWazaParam.mId == WAZANO_SYADOOREI )
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if( atkPoke->GetMonsNo() == MONSNO_GEKKOU )
    {
      playSeq = GetWazaSequenceFile( mWazaParam.mId , 1 );
    }
    //momiji追加 ネクルナ用
    if (atkPoke->GetMonsNo() == MONSNO_PURIZUMU &&
        atkPoke->GetFormNo() == FORMNO_PURIZUMU_FORM_L)
    {
      playSeq = GetWazaSequenceFile(mWazaParam.mId, 2);
    }
  }
  //momiji追加 プラズマフィスト ゼオゼラの時だけ特殊処理
  if (mWazaParam.mId == WAZANO_MABOROSIWAZA)
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if (atkPoke->GetMonsNo() == MONSNO_MABOROSI18)
    {
      playSeq = GetWazaSequenceFile(mWazaParam.mId, 1);
    }
  }
  //momiji追加 ビックリヘッド ズガドーンの時だけ特殊処理
  if (mWazaParam.mId == WAZANO_HANABISENYOU)
  {
    BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
    if (atkPoke->GetMonsNo() == MONSNO_BIISUTOh)
    {
      playSeq = GetWazaSequenceFile(mWazaParam.mId, 1);
    }
  }
  SeqComFunc_PlaySequenceFile(playSeq);

  mWazaParam.mEnableKeyAdjust = false;
  mWazaParam.mFrame = 0;
  GetSeqSys()->SetPause(true);

  GetBattleSound()->BgmVolumeDown_Waza();
}

//技再生用のシーケンス番号を返す
#define SUBSEQ_MACRO2(wazaNo,sIdx) case wazaNo: return GARC_wazaeff_seqfile_ew0 ## wazaNo ## _ ## sIdx ## _BSEQ
#define SUBSEQ_MACRO3(wazaNo,sIdx) case wazaNo: return GARC_wazaeff_seqfile_ew ## wazaNo ## _ ## sIdx ## _BSEQ

int BattleViewSystem::GetWazaSequenceFile(int mainNo , int subNo )
{
  if( mainNo == WAZANO_HIMITUNOTIKARA )
  {
    if( subNo != 0 )
    {
      return GARC_wazaeff_seqfile_ew000_BSEQ + subNo;
    }
    return GARC_wazaeff_seqfile_ew000_BSEQ + WAZANO_HIMITUNOTIKARA;
  }
  if( subNo == 1 )
  {
    switch( mainNo )
    {
      SUBSEQ_MACRO2(13,1);
      SUBSEQ_MACRO2(18,1);
      SUBSEQ_MACRO2(19,1);
      SUBSEQ_MACRO2(20,1);
      SUBSEQ_MACRO2(35,1);
      SUBSEQ_MACRO2(46,1);
      SUBSEQ_MACRO2(73,1);
      SUBSEQ_MACRO2(76,1);
      SUBSEQ_MACRO2(83,1);
      SUBSEQ_MACRO2(91,1);
      SUBSEQ_MACRO3(117,1);
      SUBSEQ_MACRO3(120,1);
      SUBSEQ_MACRO3(128,1);
      SUBSEQ_MACRO3(130,1);
      SUBSEQ_MACRO3(143,1);
      SUBSEQ_MACRO3(153,1);
      SUBSEQ_MACRO3(171,1);
      SUBSEQ_MACRO3(174,1);
      SUBSEQ_MACRO3(248,1);
      SUBSEQ_MACRO3(250,1);
      SUBSEQ_MACRO3(264,1);
      SUBSEQ_MACRO3(275,1);
      SUBSEQ_MACRO3(280,1);
      SUBSEQ_MACRO3(291,1);
      SUBSEQ_MACRO3(311,1);
      SUBSEQ_MACRO3(328,1);
      SUBSEQ_MACRO3(340,1);
      SUBSEQ_MACRO3(343,1);
      SUBSEQ_MACRO3(353,1);
      SUBSEQ_MACRO3(361,1);
      SUBSEQ_MACRO3(369,1);
      SUBSEQ_MACRO3(461,1);
      SUBSEQ_MACRO3(463,1);
      SUBSEQ_MACRO3(467,1);
      SUBSEQ_MACRO3(507,1);
      SUBSEQ_MACRO3(509,1);
      SUBSEQ_MACRO3(525,1);
      SUBSEQ_MACRO3(546,1);
      SUBSEQ_MACRO3(553,1);
      SUBSEQ_MACRO3(554,1);
      SUBSEQ_MACRO3(558,1);
      SUBSEQ_MACRO3(559,1);
      SUBSEQ_MACRO3(566,1);
      SUBSEQ_MACRO3(580,1);
      SUBSEQ_MACRO3(581,1);
      SUBSEQ_MACRO3(594,1);
      SUBSEQ_MACRO3(600,1);
      SUBSEQ_MACRO3(601,1);
      SUBSEQ_MACRO3(604,1);
      SUBSEQ_MACRO3(611,1);    
      SUBSEQ_MACRO3(619,1);
      SUBSEQ_MACRO3(620,1);    
      SUBSEQ_MACRO3(661,1);
      SUBSEQ_MACRO3(662,1);
      SUBSEQ_MACRO3(663,1);
      SUBSEQ_MACRO3(669,1);
      SUBSEQ_MACRO3(676,1);
      SUBSEQ_MACRO3(678,1);
      SUBSEQ_MACRO3(690,1);
      SUBSEQ_MACRO3(704,1);
      SUBSEQ_MACRO3(706,1);
      SUBSEQ_MACRO3(713,1);
      SUBSEQ_MACRO3(714, 1);
      SUBSEQ_MACRO3(720, 1);
      SUBSEQ_MACRO3(721, 1);
    }
  }
  else if( subNo == 2 )
  {
    switch( mainNo )
    {
      SUBSEQ_MACRO3(174,2);
      SUBSEQ_MACRO3(311,2);
      SUBSEQ_MACRO3(546, 2);
      SUBSEQ_MACRO3(713, 2);
      SUBSEQ_MACRO3(714, 2);
    }
  }
  else if( subNo == 3 )
  {
    switch( mainNo )
    {
      SUBSEQ_MACRO3(311,3);
      SUBSEQ_MACRO3(546,3);
    }
  }
  else if( subNo == 4 )
  {
    switch( mainNo )
    {
      SUBSEQ_MACRO3(311,4);
      SUBSEQ_MACRO3(546,4);
    }
  }

  return GARC_wazaeff_seqfile_ew000_BSEQ + mainNo;
}


bool BattleViewSystem::UpdateWazaEffect(void)
{
  enum
  {
    SEQ_SETUP = 0,

    //全力技名の表示
    SEQ_DISP_ZENRYOKU_NAME,
    SEQ_DISP_ZENRYOKU_NAME_WAIT,

    //実際の技再生
    SEQ_MIGAWARI_CHECK,

    SEQ_MIGAWARI_OUT,
    SEQ_MIGAWARI_OUT_WAIT,
    SEQ_MIGAWARI_OUT_UPDATE,

    SEQ_WAZA_PLAY,
    SEQ_WAZA_PLAY_WAIT,

    SEQ_MIGAWARI_IN,
    SEQ_MIGAWARI_IN_WAIT,
    SEQ_MIGAWARI_IN_UPDATE,

    SEQ_END,

    SEQ_SKIP = 200,
    SEQ_SKIP_WAIT_DAMAGE,
  };

  BtlvObject_PokeModel *atkPoke = GetPokeModel(mComTargetPoke[0]);
  switch( mComSceneSub )
  {
  case SEQ_SETUP:
    if( mZenryokuParam.isZenryoku == true && mZenryokuParam.isAttack == true && mZenryokuParam.isUnique == false )
    {
      mComSceneSub = SEQ_DISP_ZENRYOKU_NAME;
    }
    else
    {
      mComSceneSub = SEQ_MIGAWARI_CHECK;
    }
    break;

  case SEQ_DISP_ZENRYOKU_NAME:
    SeqComFunc_PlaySequenceFile( GARC_wazaeff_seqfile_ez_cmn_namecutin_BSEQ );
    mComSceneSub = SEQ_DISP_ZENRYOKU_NAME_WAIT;
    //break; //スルー
  case SEQ_DISP_ZENRYOKU_NAME_WAIT:
    if( SeqComFunc_IsFinishSequenceFile(true) )
    {
      mComSceneSub = SEQ_MIGAWARI_CHECK;
    }
    break;

  case SEQ_MIGAWARI_CHECK:
    if( mWazaParam.mId == WAZANO_HENSIN )
    {
      //BTS3617 へんしんは身代わりが出るのを防ぐため、技をそもそも再生しない。
      //変身の技のエフェクトはダミーなので問題なし。
      mComSceneSub = SEQ_END;
    }
    else
    if( mIsMigawariWaza )
    {
      mComSceneSub = SEQ_MIGAWARI_OUT;
    }
    else
    {
      mComSceneSub = SEQ_WAZA_PLAY;
    }
    break;

  case SEQ_MIGAWARI_OUT:
    if( atkPoke->IsVisibleTame() == false )
    {
      atkPoke->SetVisibleMigawari(false);
      mComSceneSub = SEQ_WAZA_PLAY;
    }
    else
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_BEFORE );
      mComSceneSub = SEQ_MIGAWARI_OUT_UPDATE;
    }
    break;
  case SEQ_MIGAWARI_OUT_UPDATE:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      mComSceneSub = SEQ_WAZA_PLAY;
    }
    break;

  case SEQ_WAZA_PLAY:
    StartWazaEffectCore();
    mComSceneSub = SEQ_WAZA_PLAY_WAIT;
    //break; //スルー
  case SEQ_WAZA_PLAY_WAIT:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      if( mIsMigawariWaza )
      {
        if( pml::wazadata::GetParam( mWazaParam.mId , pml::wazadata::PARAM_ID_HITCOUNT_MAX ) > 1 )
        {
          //連続技の時はここで戻さず後で戻す。
          //エフェクトのBTLEFF_ZOOM_IN_RESETの時
          mIsMigawariReserve = true;
          mComTargetMigawariReserve = mComTargetPoke[0];
          mComSceneSub = SEQ_END;
        }
        else
        {
          mComSceneSub = SEQ_MIGAWARI_IN;
        }
      }
      else
      {
        mComSceneSub = SEQ_END;
      }
    }
    break;

    
  case SEQ_MIGAWARI_IN:
    if( atkPoke->IsVisibleTame() == false )
    {
      atkPoke->SetVisibleMigawari(true);
      mComSceneSub = SEQ_END;
    }
    else
    {
      SeqComFunc_PlaySequenceFileEffect( BTLEFF_MIGAWARI_WAZA_AFTER );
      mComSceneSub = SEQ_MIGAWARI_IN_UPDATE;
    }
    break;
  case SEQ_MIGAWARI_IN_UPDATE:
    if( SeqComFunc_IsFinishSequenceFile() )
    {
      mComSceneSub = SEQ_END;
    }
    break;

  case SEQ_END:
    {
      mWazaParam.mSetData = false;
      mWazaParam.mSyncDamageEffect = false;
      mWazaParam.mId = WAZANO_NULL;//AddEffect系で攻撃モーションを見てしまうから

      GetBattleSound()->BgmVolumeReset_Waza();
      ResetDefaultCamera(0);
      return true;
    }
    //break;
  case SEQ_SKIP:
    {
      //該当ポケモンが死んでいるか退場済み
      //[fix]NMCat[4034]: 「ぶんまわす」の処理中に「ニードルガード」のダメージで「ききかいひ」が発動するととまる
      //ロジックの対処が難しいため、エフェクト再生側でケアをして止まらないようにする
      for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
      {
        GetUiSys()->StartGaugeDamage( BtlvPos(i) , BtlvGaugeBase::GAUGE_ANIME_DEF_FRAME );
      }
    }
  case SEQ_SKIP_WAIT_DAMAGE:
    if( GetUiSys()->IsGaugeDamage() == false )
    {
      return true;
    }
  }
  return false;

  /*
  //この処理は1フレームに複数回呼ばれる場合がある。
  //ダブルバトルで片方みがわりの時に全体攻撃を打つと、
  //PluralとMigawariが同時に呼ばれる。

  enum
  {
    MIGAWARI_CHECK = 0,

    MIGAWARI_OUT,
    MIGAWARI_OUT_WAIT,
    MIGAWARI_OUT_UPDATE,
    
    START_WAZA,
    LOAD_WAIT,
    UPDATE_EFFECT,

    MIGAWARI_IN,
    MIGAWARI_IN_WAIT,
    MIGAWARI_IN_UPDATE,

    FINISH_WAZA,
  };
  
  BtlvModelPoke *atkPoke = GetPokeModel(mComTargetPoke[0]);

  switch( mComSceneSub )
  {
  case MIGAWARI_CHECK:
    if( mWazaParam.mId == WAZANO_HENSIN )
    {
      //BTS3617 へんしんは身代わりが出るのを防ぐため、技をそもそも再生しない。
      //変身の技のエフェクトはダミーなので問題なし。
      mComSceneSub = FINISH_WAZA;
    }
    else
    if( mIsMigawariWaza )
    {
      mComSceneSub = MIGAWARI_OUT;
    }
    else
    {
      mComSceneSub = START_WAZA;
    }
    break;

  case MIGAWARI_OUT:
    if( atkPoke->IsVisibleTame() == false )
    {
      atkPoke->SetMigawariVisible(false);
      mComSceneSub = START_WAZA;
    }
    else
    {
      SeqComFunc_PlaySequenceFileAuto( BTLEFF_MIGAWARI_WAZA_BEFORE );
      mComSceneSub = MIGAWARI_OUT_UPDATE;
    }
    break;
  case MIGAWARI_OUT_UPDATE:
    if( SeqComFunc_IsFinishSequenceFileAuto() )
    {
      mComSceneSub = START_WAZA;
    }
    break;
  
  case START_WAZA:
    StartWazaEffectCore();
    mIsEnableWazaCmaeraCollision = true;
    mComSceneSub = LOAD_WAIT;
    break;
  case LOAD_WAIT:
    if( GetPreloadSys()->IsFinishLoad() == true &&
        IsFinishLoadFieldWaza() )
    {
      GetPreloadSys()->CreateResource();
      GetPreloadSys()->Dump();
      mComSceneSub = UPDATE_EFFECT;
      GetSeqSys()->SetPause(false);
    }
    break;
  case UPDATE_EFFECT:
    if( SeqComFunc_IsFinishSequenceFile() &&
        IsFinishClusterAll() &&
        GetBgSystem()->IsFinishBreakObj() &&
        mUi->IsAnimeGauge() == false &&
        GetTaskSys()->IsAliveDeleteTask() == false )
    {
      ClearWazaResource();
      mIsEnableWazaCmaeraCollision = false;

      if( mIsMigawariWaza )
      {
        if( pml::wazadata::GetParam( mWazaParam.mId , pml::wazadata::PARAM_ID_HITCOUNT_MAX ) > 1 )
        {
          //連続技の時はここで戻さず後で戻す。
          //エフェクトのBTLEFF_ZOOM_IN_RESETの時
          mIsMigawariReserve = true;
          mComTargetMigawariReserve = mComTargetPoke[0];
          mComSceneSub = FINISH_WAZA;
        }
        else
        {
          mComSceneSub = MIGAWARI_IN;
        }
      }
      else
      {
        mComSceneSub = FINISH_WAZA;
      }
    }
    break;


  case MIGAWARI_IN:
    if( atkPoke->IsVisibleTame() == false )
    {
      atkPoke->SetMigawariVisible(true);
      mComSceneSub = FINISH_WAZA;
    }
    else
    {
      SeqComFunc_PlaySequenceFileAuto( BTLEFF_MIGAWARI_WAZA_AFTER );
      mComSceneSub = MIGAWARI_IN_UPDATE;
    }
    break;
  case MIGAWARI_IN_UPDATE:
    if( SeqComFunc_IsFinishSequenceFileAuto() )
    {
      mComSceneSub = FINISH_WAZA;
    }
    break;

  case FINISH_WAZA:

    mIsActiveTodome = false;
  //デバッグ確認用
    mWazaParam.mSetData = false;
    mWazaParam.mSyncDamageEffect = false;
    GetBattleSound()->BgmVolumeReset_Waza();
    SetStartFieldEffect(true);
    SetVisibleRareEffect(true);
    SetCameraDefault(0);
    return true;
    //break;
  }
*/
}

//----------------------------------------------------
//グループオプションの判断
//技エフェクト中のみ有効！
bool BattleViewSystem::CheckCanPlayCommand(int grpOpt,u32 comNo)
{
  switch(grpOpt)
  {
  case GRP_OPT_NONE:      //オプション無
    return true;
  case GRP_OPT_SINGLE:    //シングル
    if( GetBattleRule() == BTL_RULE_SINGLE )
    {
      return true;
    }
    else
    {
      return false;
    }
  case GRP_OPT_DOUBLE:    //ダブル
    if( GetBattleRule() == BTL_RULE_DOUBLE )
    {
      return true;
    }
    else
    {
      return false;
    }
  case GRP_OPT_ROYAL:     //ロイヤル
    if( GetBattleRule() == BTL_RULE_ROYAL )
    {
      return true;
    }
    else
    {
      return false;
    }
  case GRP_OPT_NOT_SINGLE://シングル以外
    if( GetBattleRule() != BTL_RULE_SINGLE )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_ATK_NEAR:  //攻撃側が手前
    if( SeqComFunc_IsFlipEffect(SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK),SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF)) )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_ATK_FAR:   //攻撃側が奥
    if( SeqComFunc_IsFlipEffect(SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK),SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF)) )
    {
      return false;
    }
    else
    {
      return true;
    }  
  case GRP_OPT_DEF_NEAR:  //防御側が手前
    if( SeqComFunc_IsFlipEffect(SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF),SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK)) )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_DEF_FAR:   //防御側が奥
    if( SeqComFunc_IsFlipEffect(SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF),SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK)) )
    {
      return false;
    }
    else
    {
      return true;
    }  

    //ロイヤル用分岐
  case GRP_OPT_ATK_R_NEAR:  //攻撃側が手前(ロイヤル)
    if( SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) == BTL_VPOS_ROYAL_NEAR )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_ATK_R_RIGHT: //攻撃側が右(ロイヤル)
    if( SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) == BTL_VPOS_ROYAL_RIGHT )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_ATK_R_FAR:   //攻撃側が奥(ロイヤル)
    if( SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) == BTL_VPOS_ROYAL_FAR )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_ATK_R_LEFT:  //攻撃側が左(ロイヤル)
    if( SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) == BTL_VPOS_ROYAL_LEFT )
    {
      return true;
    }
    else
    {
      return false;
    }  
  case GRP_OPT_NOT_COMM:
#if PM_DEBUG
    //デバッグ用偽装
    if( GetDebugParam() && GetDebugParam()->mSeqOptFakeComm != -1 )
    {
      if( GetDebugParam()->mSeqOptFakeComm == 0)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
#endif

    //if( GetBattleCompetitor() != BTL_COMPETITOR_COMM )
    if( mIsCommIntro == false )  //プレイヤー相手の模擬戦でも使うのでこちらのチェック
    {
      return true;
    }
    else
    {
      return false;
    }
  case GRP_OPT_COMM:
#if PM_DEBUG
    //デバッグ用偽装
    if( GetDebugParam() && GetDebugParam()->mSeqOptFakeComm != -1 )
    {
      if( GetDebugParam()->mSeqOptFakeComm == 1)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
#endif
    //if( GetBattleCompetitor() != BTL_COMPETITOR_COMM )
    if( mIsCommIntro == false )  //プレイヤー相手の模擬戦でも使うのでこちらのチェック
    {
      return false;
    }
    else
    {
      return true;
    }
  case GRP_OPT_TIME_DAY:        //時間帯(朝昼/晴れ)
    //朝昼は例外を拾うため夕方・夜以外で判定する
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone != GameSys::TimeZone::EVENING &&
          mComTimeZone != GameSys::TimeZone::NIGHT &&
          mComTimeZone != GameSys::TimeZone::MIDNIGHT )
      {
        if( GetFieldSituation()->sky_type != System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    return false;
  case GRP_OPT_TIME_EVENING:    //時間帯(夕方/晴れ)
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone == GameSys::TimeZone::EVENING )
      {
        if( GetFieldSituation()->sky_type != System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    return false;
  case GRP_OPT_TIME_NIGHT:      //時間帯(夜/晴れ)
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone == GameSys::TimeZone::NIGHT || mComTimeZone == GameSys::TimeZone::MIDNIGHT )
      {
        if( GetFieldSituation()->sky_type != System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    return false;
  case GRP_OPT_TIME_DAY_CLOUD:  //時間帯(朝昼/曇り)
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone != GameSys::TimeZone::EVENING &&
        mComTimeZone != GameSys::TimeZone::NIGHT &&
        mComTimeZone != GameSys::TimeZone::MIDNIGHT )
      {
        if( GetFieldSituation()->sky_type == System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    else
    {
      //屋内は昼・曇りにする
      //※ nijiでは屋内は昼・晴れだった。変更理由は、シーケンスで屋外の昼の時にレンズフレアっぽいエフェクトを入れるため。
      //　 屋内が昼・晴れだと屋内でも出てしまうので屋内は昼曇りに変更
      //   影響が出るのは屋内の水場の導入くらいだった。
      return true;
    }
    return false;
  case GRP_OPT_TIME_EVENING_CLOUD:  //時間帯(夕方/曇り)
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone == GameSys::TimeZone::EVENING )
      {
        if( GetFieldSituation()->sky_type == System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    return false;
  case GRP_OPT_TIME_NIGHT_CLOUD:  //時間帯(夜/曇り)
    if( IsEnableSkyBox() )
    {
      if( mComTimeZone == GameSys::TimeZone::NIGHT || mComTimeZone == GameSys::TimeZone::MIDNIGHT )
      {
        if( GetFieldSituation()->sky_type == System::Skybox::Skybox::SKY_TYPE_CLOUDY )
        {
          return true;
        }
      }
    }
    return false;

  case GRP_OPT_ATK_RARE:  //攻撃側が色違い
    {
      BtlvPos vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
      // @fix MMCat[359] 通常色のメタモンが色違いの「ズガドーン/ビーストH」に「へんしん」した状態で「ビックリヘッド/はなびせんよう」を使用すると技演出が通常色のものになる
      BtlvObject_PokeModel *mdl = GetPokeModel(vPos);
      if (mdl->IsRare())
      {
        return true;
      }
      return false;
    }
  case GRP_OPT_ATK_NO_RARE://攻撃側が色違いではない
    {
      BtlvPos vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
      // @fix MMCat[359] 通常色のメタモンが色違いの「ズガドーン/ビーストH」に「へんしん」した状態で「ビックリヘッド/はなびせんよう」を使用すると技演出が通常色のものになる
      BtlvObject_PokeModel *mdl = GetPokeModel(vPos);
      if (mdl->IsRare())
      {
        return false;
      }
      return true;
    }

  case GRP_OPT_DEF_CAPTURE_DEMO:  //捕獲チュートリアル
    if( GetBtlvMode() == BTLV_MODE_CAPTURE )
    {
      return true;
    }
    else
    {
      return false;
    }
  case GRP_OPT_DEF_NOT_CAPTURE_DEMO:  //捕獲チュートリアル以外
    if( GetBtlvMode() != BTLV_MODE_CAPTURE )
    {
      return true;
    }
    else
    {
      return false;
    }


  case GRP_OPT_DEF_WIN:  //Winのみ
#if defined(GF_PLATFORM_CTR)
    return false;
#elif defined(GF_PLATFORM_WIN32)
    return true;
#endif  //プラットフォーム分岐
  case GRP_OPT_DEF_CTR:  //CTRのみ
#if defined(GF_PLATFORM_CTR)
    return true;
#elif defined(GF_PLATFORM_WIN32)
    return false;
#endif  //プラットフォーム分岐
  }  
  return true;
}

u32 BattleViewSystem::CheckWazaDataId_Model( u32 datId , u16 idx )
{
  return datId;
}

u32 BattleViewSystem::CheckWazaDataId_Camera( u32 datId , u16 idx )
{
  if( datId == GARC_wazaeff_resource_ee380_CAM_lz_CGFBMOT )
  {
    BtlvObject_Chara *chara = SeqComFunc_GetTargetChara(idx);
    TrainerExtraInfoStruct *data = chara->GetTrainerData();
    if( data )
    {
      return data->mEntryCam;
    }
  }
  if( datId == GARC_wazaeff_resource_ee351_CAM01_lz_CGFBMOT )
  {
    BtlvObject_Chara *chara = SeqComFunc_GetTargetChara(idx);
    TrainerExtraInfoStruct *data = chara->GetTrainerData();
    if( data )
    {
      return data->mDefeatCam;
    }
  }
  if( datId == GARC_wazaeff_resource_ee311_CAM_02_lz_CGFBMOT )
  {
    BtlvObject_Chara *chara = SeqComFunc_GetTargetChara(idx);
    TrainerExtraInfoStruct *data = chara->GetTrainerData();
    if( data )
    {
      return data->mThrowCam;
    }
  }
  return datId;
}

u32 BattleViewSystem::CheckWazaDataId_Env( u32 datId , u16 idx )
{
  //ライトのみのenvのキャラはカメラが入ってないのでまずい
  /*
  if( datId == GARC_wazaeff_resource_ee380_CAM_lz_GFBENV )
  {
    return GetTrainerLightEnv(SEQ_DEF_TR_ENV_TOUJYO,idx);
  }
  if( datId == GARC_wazaeff_resource_ee351_CAM01_lz_GFBENV )
  {
    return GetTrainerLightEnv(SEQ_DEF_TR_ENV_LOSE,idx);
  }
  */

  return datId;
}

u32 BattleViewSystem::GetTrainerLightEnv( int type , u16 idx )
{
  BtlvObject_Chara *chara = SeqComFunc_GetTargetChara(idx);
  TrainerExtraInfoStruct *data = chara->GetTrainerData();
  if( data )
  {
    switch(type)
    {
    default:
    case SEQ_DEF_TR_ENV_TOUJYO:
      return data->mEntryEnv;
    case SEQ_DEF_TR_ENV_LOSE:
      return data->mDefeatEnv;
    }
  }
  else
  {
    return GARC_wazaeff_resource_ee380_CAM_lz_GFBENV;
  }
}

u32 BattleViewSystem::CheckWazaDataId_Effect( u32 datId , u16 idx )
{
  if( datId == GARC_wazaeff_resource_eb001_ballout_lz_BPTCL )
  {
    //ボールエフェクト
    if( mEffectBallId[idx] > 0 && mEffectBallId[idx] <= item::BALLID_MAX )
    {
      return GARC_wazaeff_resource_eb001_ballout_lz_BPTCL + (mEffectBallId[idx]-1)*2;
    }
    else
    {
      return GARC_wazaeff_resource_eb004_ballout_lz_BPTCL;
    }
  }
  else
  if( datId == GARC_wazaeff_resource_eb001_capture_lz_BPTCL )
  {
    //ボールエフェクト
    if( mEffectBallId[idx] > 0 && mEffectBallId[idx] <= item::BALLID_MAX )
    {
      return GARC_wazaeff_resource_eb001_capture_lz_BPTCL + (mEffectBallId[idx]-1)*2;
    }
    else
    {
      return GARC_wazaeff_resource_eb004_capture_lz_BPTCL;
    }
  }
  else
  if( datId >= GARC_wazaeff_resource_ee394_heart00_lz_BPTCL && 
      datId <= GARC_wazaeff_resource_ee394_heart05_lz_BPTCL &&
      GetMainModule() )
  {
    //かわいがりハート
    BtlvPos vpos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    u8 val = GetMainModule()->GetPokeFriendship(GetBattlePokeParam(vpos));
    if( val == 0 )
    {
      return GARC_wazaeff_resource_ee394_heart00_lz_BPTCL;
    }
    else
    if( val < 50 )
    {
      return GARC_wazaeff_resource_ee394_heart01_lz_BPTCL;
    }
    else
    if( val < 100 )
    {
      return GARC_wazaeff_resource_ee394_heart02_lz_BPTCL;
    }
    else
    if( val < 150 )
    {
      return GARC_wazaeff_resource_ee394_heart03_lz_BPTCL;
    }
    else
    if( val < 254 )
    {
      return GARC_wazaeff_resource_ee394_heart04_lz_BPTCL;
    }
    else
    {
      return GARC_wazaeff_resource_ee394_heart05_lz_BPTCL;
    }
  }
  else
  if( datId == GARC_wazaeff_resource_ez_cmn_namecutin_beads01_lz_BPTCL )
  {
    if( GetZenryokuParam()->isZenryoku )
    {
      return GARC_wazaeff_resource_ez_cmn_namecutin_beads01_lz_BPTCL + GetZenryokuParam()->wazaType;
    }
  }

  return datId;
}

u32 BattleViewSystem::CheckWazaDataId_Texture( u32 datId , u16 idx )
{
  if( datId == GARC_wazaeff_resource_ez_cmn_name_01_lz_BTEX )
  {
    if( mZenryokuParam.seq >= GARC_wazaeff_seqfile_ez_0_01_BSEQ &&
        mZenryokuParam.seq <= GARC_wazaeff_seqfile_ez_0_18_BSEQ )
    {
      return GARC_wazaeff_resource_ez_cmn_name_01_lz_BTEX + mZenryokuParam.wazaType;
    }
  }
  return datId;
}

u32 BattleViewSystem::CheckWazaDataId_TrainerTexture( int trgChara , int type )
{
  BtlvObject_Chara *chara = SeqComFunc_GetTargetChara(trgChara);

  u32 arcIdBase = GARC_wazaeff_resource_tr9999_99_A_lz_BTEX;
  ARIIZUMI_PRINT("TrainerTex[%d]",trgChara);
  if( chara )
  {
    TrainerExtraInfoStruct *data = chara->GetTrainerData();
    if( data )
    {
      arcIdBase = data->mTexture;
    }
  }
  ARIIZUMI_PRINT("[%d]\n",arcIdBase);
  return arcIdBase+type;
}

u32 BattleViewSystem::CheckWazaDataId_Se( u32 datId , u16 idx)
{
  if( datId == 	SEQ_SE_BT_WAB_000047 && 
      mZenryokuParam.isZenryoku == true )
  {
    u32 seIdArr[] =
    {
      SEQ_SE_BT_WAB_000047,//	ノーマル
      SEQ_SE_BT_WAB_000048,//	かくとう
      SEQ_SE_BT_WAB_000049,//	ひこう
      SEQ_SE_BT_WAB_000050,//	どく
      SEQ_SE_BT_WAB_000051,//	じめん
      SEQ_SE_BT_WAB_000052,//	いわ
      SEQ_SE_BT_WAB_000053,//	むし
      SEQ_SE_BT_WAB_000054,//	ゴースト
      SEQ_SE_BT_WAB_000055,//	はがね
      SEQ_SE_BT_WAB_000056,//	ほのお
      SEQ_SE_BT_WAB_000057,//	みず
      SEQ_SE_BT_WAB_000058,//	くさ
      SEQ_SE_BT_WAB_000059,//	でんき
      SEQ_SE_BT_WAB_000096,//	エスパー
      SEQ_SE_BT_WAB_000097,//	こおり
      SEQ_SE_BT_WAB_000098,//	ドラゴン
      SEQ_SE_BT_WAB_000099,//	あく
      SEQ_SE_BT_WAB_000100,//	フェアリー
    };

    return seIdArr[mZenryokuParam.wazaType];
  }

  //ボールのSE分岐
  if( datId >= SEQ_SE_BT_035 &&
      datId <= SEQ_SE_BT_039)
  {
    switch( mEffectBallId[idx] )
    {
    case item::BALLID_MASUTAABOORU:
      return SEQ_SE_BT_038;
      //break;
    case item::BALLID_HAIPAABOORU:
      return SEQ_SE_BT_037;
      //break;
    case item::BALLID_SUUPAABOORU:
      return SEQ_SE_BT_036;
      //break;
    case item::BALLID_MONSUTAABOORU:
      return SEQ_SE_BT_035;
      //break;
    default:
      return SEQ_SE_BT_039;
      //break;
    }
  }
  return datId;
}

//シーケンスの一時停止系の関数の設定
void BattleViewSystem::SetSequenceWaitFunc( int type )
{
  switch( type )
  {
  case SEQ_DEF_WAIT_KEY:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_KeyInput;
    mComWaitIsSusspend = true;
    break;
  case SEQ_DEF_WAIT_MESSAGE:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_Message;
    mComWaitIsSusspend = false;
    break;
  case SEQ_DEF_WAIT_LOAD:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_Load;
    mComWaitIsSusspend = false;
    break;
  case SEQ_DEF_WAIT_LOAD_POKE:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_LoadPoke;
    mComWaitIsSusspend = false;
    break;
  case SEQ_DEF_WAIT_LOAD_BG:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_LoadBg;
    mComWaitIsSusspend = false;
    break;
  case SEQ_DEF_WAIT_POKE_INTRO:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_PokeIntro;
    mComWaitIsSusspend = false;
    break;
  case SEQ_DEF_WAIT_FADE:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_WaitFade;
    mComWaitIsSusspend = true;
    break;
  case SEQ_DEF_WAIT_DEMOFADE_SYNC:
    mComWaitFunc = &BattleViewSystem::SeqComWaitFunc_WaitDemoFadeSync;
    mComWaitIsSusspend = true;
    break;
  }
}

//シーケンスシステムの一時停止の更新
void BattleViewSystem::UpdateWaitSequenceSystem(void)
{
  if( mComWaitFunc )
  {
    if( (this->*mComWaitFunc)() )
    {
      mComWaitFunc = NULL;
    }
  }

  mWazaParam.mFrame = GetSeqSys()->GetSeqFrame();

  if( GetSeqSys()->GetIsPause() == false )
  {
    //KeyFrame対応
    if( !(mWazaParam.mKeyDiffFrame == 1 && mWazaParam.mFrame == 0 ) && //どうしてもずれるので対処・・・(再生開始が1フレーム目になるとき・・・)
        mWazaParam.mKeyDiffFrame <= mWazaParam.mFrame &&  //この後更新が来るので+1
        mWazaParam.mIsPlayMotion == false )
    {
      mWazaParam.mIsPlayMotion = true;
      BtlvPos vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
      BtlvObject_PokeModel *mdl = GetPokeModel(vPos);

      if( mdl && mdl->IsEnable() )
      {
        mdl->ChangeAnimationDirect( mWazaParam.mAttackMotion , true );
        mdl->SetAnimationIsLoop( false );
        mdl->SetReserveAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
      }

      ARIIZUMI_PRINT("KeyAdjust Play!!\n");

    }
    else
    if( mWazaParam.mEnableKeyAdjust )
    {
      if( mWazaParam.mKeyDiffFrame < 0 )
      {
        mWazaParam.mKeyDiffFrame++;
      }
      else
      {
        mWazaParam.mEnableKeyAdjust = false;
      }
    }
  }
}

//シーケンスシステムを一時停止するか？
bool BattleViewSystem::IsWaitSequenceSystem(void)
{
  if( mComWaitFunc )
  {
    return true;
  }

  //KeyFrame対応
  if( mWazaParam.mEnableKeyAdjust )
  {
    if( mWazaParam.mKeyDiffFrame < 0 &&
        mWazaParam.mFrame == 0 )
    {
      return true;
    }
  }

  return false;
}

//シーケンス一時停止の待ち関数
bool BattleViewSystem::SeqComWaitFunc_KeyInput( void )
{
  if( IsCommMode() || IsRecordPlayMode() )
  {
    return true;
  }

  // メッセージカーソルの三角を表示
  mUiSys->SetVisibleMsgCursor( true );

  const gfl2::ui::Button *uiButton = GetButton();
  if( uiButton->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) )
  {
    return true;
  }

  const gfl2::ui::TouchPanel *tp = GetTP();
  if( tp->IsTouchTrigger() ){
    return true;
  }

  return false;
}

bool BattleViewSystem::SeqComWaitFunc_Message( void )
{
  // メッセージカーソルの三角を表示
  mUiSys->SetVisibleMsgCursor( true );

  if( mUiSys->IsUpdateMessage() == false )
  {
    return true;
  }
  return false;
}

//開始時読み込み待ち
bool BattleViewSystem::SeqComWaitFunc_Load( void )
{
  if( DataLoadWait(2) == false )
  {
    return false;
  }
  return true;
}

//ポケモン読み込み待ち
bool BattleViewSystem::SeqComWaitFunc_LoadPoke( void )
{
  if(IsLoadPokeModel())
  {
    return false;
  }
  return true;
}

//戦闘背景読み込み待ち(フィールド技用
bool BattleViewSystem::SeqComWaitFunc_LoadBg( void )
{
  if(IsFinishLoadFieldWaza())
  {
    return true;
  }
  return false;
}

//ポケモン導入処理系待ち
bool BattleViewSystem::SeqComWaitFunc_PokeIntro( void )
{
  if( GetTaskSystemAlways()->GetActiveTaskNum() == 0 )
  {
    return true;
  }
  return false;
}

bool BattleViewSystem::SeqComWaitFunc_WaitFade( void )
{
  //純粋な上画面フェードと、特殊フェードの場合がある
  if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER) == false ||
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_CUSTOM_UPPER) == false )
  {
    return false;
  }
  return true;
}

bool BattleViewSystem::SeqComWaitFunc_WaitDemoFadeSync( void)
{
  gfl2::Fade::FadeSuper *fadeSys = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade(gfl2::Fade::DISP_CUSTOM_UPPER);
  if( fadeSys )
  {
    AppLib::Fade::DemoFade *demoFade = reinterpret_cast<AppLib::Fade::DemoFade*>(fadeSys);
    if( demoFade->GetSequence() == NULL )
    {
      //シーケンス終わってる？
      return true;
    }
    else
    {
      //シーケンス一時停止で止まっている
      return true;
    }
  }
  //そもそもデモフェードじゃないから待てない
  return true;
}

//-------------------------------------------------------
//シーケンス系
//-------------------------------------------------------
void BattleViewSystem::SeqComFunc_PlaySequenceFile( int no )
{
  mPlaySequenceNo = no;
  mSequenceSeq = 0;
  GetSeqSys()->LoadSeqFile(no);
  GetSeqSys()->SetPause(true);
  mIsLoadSequenceResource = true;
}
void BattleViewSystem::SeqComFunc_PlaySequenceFileEffect( int effNo ) //エフェクト定義を使った再生
{
  if( CheckCanPlayEffect( mComTargetPoke[0] , effNo ) )
  {
    //ダメージ音が鳴ってしまうので、mDmageAboutを切っておく。ただし例外有。
    if( effNo != BTLEFF_SIMPLE_DAMAGE_NO_SOUND &&     //ダメージエフェクトから呼ばれるから残す必要あり
        effNo != BTLEFF_MIGAWARI_WAZA_BEFORE &&       //身代わりの時は技のパラメータが入っているからまずい。
        effNo != BTLEFF_MIGAWARI_WAZA_AFTER )
    {
      mWazaParam.mDmageAbout = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE;
    }

    mPlaySequenceNo = effNo + GARC_wazaeff_seqfile_ee001_BSEQ ;
    mSequenceSeq = 0;
    GetSeqSys()->LoadSeqFile(mPlaySequenceNo);
    GetSeqSys()->SetPause(true);
    mIsLoadSequenceResource = true;
  }
  else
  {
    mPlaySequenceNo = -1;
    mSequenceSeq = 0xFF;
  }
}

void BattleViewSystem::SeqComFunc_PlaySequenceFileDebug(void)
{
  mPlaySequenceNo = -1;
  mSequenceSeq = 0;
  GetSeqSys()->DebugLoadPath("R:/home/momiji/resource/effect/battle/tool/WazaEditor/test.bseq");
  GetSeqSys()->SetPause(true);
  mIsLoadSequenceResource = true;
}

bool BattleViewSystem::SeqComFunc_IsFinishSequenceFile( bool isNoRelease )
{
  enum
  {
    SEQ_START_LOAD=0,
    SEQ_WAIT_LOAD,
    SEQ_WAIT_SEQ,
    SEQ_FINISH,

    SEQ_SKIP = 0xFF,
  };
  switch(mSequenceSeq)
  {
  case SEQ_START_LOAD:
    {
      //Taeget1に入っていて、Damageに入っていない時は入れておく
      if( mComTargetDamage[0] == BTL_VPOS_ERROR &&
          mComTargetPoke[1] != BTL_VPOS_ERROR )
      {
        mComTargetDamage[0] = mComTargetPoke[1];
      }
      //@attention 時間の移り変わりのタイミングで違和感が出るので間の時間を取るように修正
      //mComTimeZone = GameSys::TimeZone::GetNowTimeZone();
      {
        s32 min;
        GameSys::AlolaTime time;
        time.Initialize();
        //分単位にしてチェック
        min = time.GetMinute() + time.GetHour()*60;
        //夜　→　昼 　5：00　～　 6：00 　　　 　なので　　 5:29まで夜　 　5:30から昼
        //昼　→　夕　16：45　～　17：00　　　　　なので　　16:53まで昼　  16:54から夕
        //夕　→　夜　17：30　～　18：00　　　　　なので　　17:44まで夕　　17:45から夜
        //実表示で微調整済み
        if( min <= 5*60+29 )
        {
          mComTimeZone = GameSys::TimeZone::NIGHT;
        }
        else
        if( min <= 16*60+53 )
        {
          mComTimeZone = GameSys::TimeZone::NOON;
        }
        else
        if( min <= 17*60+44 )
        {
          mComTimeZone = GameSys::TimeZone::EVENING;
        }
        else
        {
          mComTimeZone = GameSys::TimeZone::NIGHT;
        }
      }
      GetSeqSys()->CheckWazaState();
      GetSeqSys()->SetPause(true);
      mSequenceSeq++;
    }
    //break;  //through
  case SEQ_WAIT_LOAD:
    if( GetResourceManager()->IsFinishLoadStep(1)==false ||
        GetAsyncFileManager()->IsAllReqFinished() == false )
    {
      ARIIZUMI_PRINT("ReousrceManagerLoading...\n");
      return false;
    }
    ARIIZUMI_PRINT("ReousrceManagerLoadFinish..\n");
    GetResourceManager()->CreateResource(GetTempHeap(), GetTempGLAllocator(), GetEffectSystem(), GetEffectHeap(), GetParticleIndexManager());
    GetSeqSys()->SetPause(false);
    mIsLoadSequenceResource = false;
    BtlvTickEnd(3,"LoadSeq"); 
    mSequenceSeq++;
    //break;  //through
  case SEQ_WAIT_SEQ:
    if( GetSeqSys()->IsFinishAll() &&
        GetTaskSystem()->GetActiveTaskNum() == 0 &&
        GetUiSys()->IsGaugeDamage() == false )
    {

      for( int i=0;i<BTLV_EFF_SOUND_NUM;i++ )
      {
        mSoundObject[i]->SetSoundId(-1);
      }
      if( isNoRelease == false )
      {
        ResetZenryokuTrainerMotion();
        SeqComFunc_CrealWazaResource();
      }
      mSequenceSeq++;
    }
    break;
  case SEQ_FINISH:
  case SEQ_SKIP:
    mPlaySequenceNo = -1;
    return true;
    //break;
  }
  return false;
}

void BattleViewSystem::SeqComFunc_CrealWazaResource(void)
{
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( GetPokeModel((BtlvPos)i) )
    {
      //外部モーションを破棄しておく
      GetPokeModel((BtlvPos)i)->ChangeAnimationResource(NULL,1);
      //プライオリティが追従で変わっている可能性があるので戻しておく
      GetObjectManager()->RemoveObject(GetPokeModel((BtlvPos)i));
      GetObjectManager()->AddObject(GetPokeModel((BtlvPos)i));
    }
  }

  //技モデル用ライトの方向を戻しておく
  gfl2::math::Vector3 dir = gfl2::math::Vector3(0.0f, -1.0f,-1.0f);
  if( GetRenderSys()->GetLightNode() )
  {
    GetRenderSys()->GetLightNode()->SetDirection(dir);
  }

  GetEffectObjManager()->ClearBuffer(mObjectManager);
  GetModelObjManager()->ClearBuffer(mObjectManager);
  GetClusterObjManager()->ClearBuffer(mObjectManager);
  GetResourceManager()->DeleteAllResource(GetEffectSystem(),GetParticleIndexManager());

  //ロトムの破棄
  GetUiSys()->Rotom_Delete();
}

void BattleViewSystem::SeqComFunc_CleanAllWazaResource(void)
{
  gfl2::heap::HeapBase* heap = GetTempHeap();
  for( int i=0;i<BTLV_EFF_MODEL_NUM;i++ )
  {
    int objNum;
    BtlvObject_Gfmdl **objArr = GetModelObjManager()->GetObjectArrIdx(i,&objNum);
    if( objArr )
    {
      for( int j=0;j<objNum;j++)
      {
        DemoLib::Obj::CreateTask_DeleteObject_GfMdl(GetTaskSystem(),heap,objArr[j],0);
      }
    }
  }
  for( int i=0;i<BTLV_EFF_PARTICLE_NUM;i++ )
  {
    int objNum;
    BtlvObject_Effect **objArr = GetEffectObjManager()->GetObjectArrIdx(i,&objNum);
    if( objArr )
    {
      for( int j=0;j<objNum;j++)
      {
        DemoLib::Obj::CreateTask_DeleteObject_Effect(GetTaskSystem(),heap,objArr[j],0);
      }
    }
  }
  for( int i=0;i<BTLV_EFF_CLUSTER_NUM;i++ )
  {
    int objNum;
    BtlvObject_Cluster **objArr = GetClusterObjManager()->GetObjectArrIdx(i,&objNum);
    if( objArr )
    {
      for( int j=0;j<objNum;j++)
      {
        DemoLib::Obj::CreateTask_DeleteObject_Cluster(GetTaskSystem(),heap,objArr[j],0);
      }
    }
  }

}

//技エフェクト用関数
void BattleViewSystem::SetComTargetReset(void)
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    mComTargetDamage[i] = BTL_VPOS_ERROR;
  }

  mComTargetPoke[0] = BTL_VPOS_ERROR;
  mComTargetPoke[1] = BTL_VPOS_ERROR;
  mTargetTrainer[0] = BTL_VPOS_ERROR;
  mTargetTrainer[1] = BTL_VPOS_ERROR;
  mEffectBallId[0] = item::BALLID_MONSUTAABOORU;  //モンスターボール
  mEffectBallId[1] = item::BALLID_MONSUTAABOORU;

  mIsMigawariWaza = false;
}

//シーケンスの攻撃側・防御側からBtlvPosを返す
BtlvPos BattleViewSystem::SeqComFunc_GetTargetPoke(int trg,int idx )
{
  switch( trg )
  {
  case SEQ_DEF_POS_ATK:  //攻撃側
    return mComTargetPoke[0];
    //break;
  case SEQ_DEF_POS_DEF:  //防御側
    return mComTargetDamage[idx];
    //break;
  case SEQ_DEF_POS_DEFTRG:  //防御側中心キャラ
    return mComTargetPoke[0];
    //break;
  }
  return BTL_VPOS_ERROR;
}

BtlvPos BattleViewSystem::SeqComFunc_GetTargetPokeSub(int trg)
{
  switch( trg )
  {
  case SEQ_DEF_POS_ATK:  //攻撃側
    return mComTargetPoke[1];
    //break;
  case SEQ_DEF_POS_DEF:  //防御側
    return mComTargetPoke[0];
    //break;
  case SEQ_DEF_POS_DEFTRG:  //防御側中心キャラ
    return mComTargetPoke[1];
    //break;
  }
  return BTL_VPOS_ERROR;
}

//複数ターゲットの体数取得
int BattleViewSystem::SeqComFunc_GetTargetPokeNum(bool isCheck )
{
  int num = 0;

  //@ attention コンパイラ最適化不具合でループの展開がおかしくなる場合がある。
  //            ここは比較的呼ばれる場所なので手動展開で対処する。
  /*
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( mComTargetDamage[i] != BTL_VPOS_ERROR )
    {
      num++;
    }
  }
  */

  if( mComTargetDamage[0] != BTL_VPOS_ERROR ){num++;}
  if( mComTargetDamage[1] != BTL_VPOS_ERROR ){num++;}
  if( mComTargetDamage[2] != BTL_VPOS_ERROR ){num++;}
  if( mComTargetDamage[3] != BTL_VPOS_ERROR ){num++;}
  if( mComTargetDamage[4] != BTL_VPOS_ERROR ){num++;}
  if( mComTargetDamage[5] != BTL_VPOS_ERROR ){num++;}

  if( num == 0 && isCheck == true )
  {
    //GFL_ASSERT_MSG(0,"複数生成が不正\n");
    return 1;
  }
  return num;
}

bool BattleViewSystem::SeqComFunc_IsFlipEffect(BtlvPos target,BtlvPos subTarget)
{
  if( GetBattleRule() != BTL_RULE_ROYAL )
  {
    //ロイヤル以外は自分が反転する
    return IsPlayerSide(target);
  }
  else
  {
    //攻防が一緒
    if( target==subTarget ||
        subTarget == BTL_VPOS_ERROR )
    {
      if( target == BTL_VPOS_ROYAL_NEAR ||
          target == BTL_VPOS_ROYAL_LEFT )
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      //位置に点をつけて、点の高い方が反転する
      //自分から反時計回り
      const u8 posPoint[4] = {4,1,2,3};
      //ARIIZUMI_PRINT("[%d(%d)]->[%d(%d)]",target,posPoint[target],subTarget,posPoint[subTarget]);
      if( posPoint[target] > posPoint[subTarget] )
      {
        //ARIIZUMI_PRINT("Flip!\n");
        return true;
      }
      else
      {
        //ARIIZUMI_PRINT("\n");
        return false;
      }
    }
  }
}

void BattleViewSystem::SeqComFunc_GetPokeRelativePos( const BTLV_RELARIVE_POKE_OPTION &opt , gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot ,gfl2::math::Vector3 *retScale , bool isCameraAdjust )
{
  retRot->Set(0,0,0);
  retScale->Set(1,1,1);

  if( opt.node == SEQ_DEF_NODE_BASE||
      opt.node == SEQ_DEF_NODE_INTRO)
  {
    BtlvObject_PokeModel *mdl = GetPokeModel(opt.vpos);
    gfl2::math::Vector3 pos;
    gfl2::math::Vector3 ofs;
    int deg;

    GetDefaultPokePos(opt.vpos,&pos,&deg);

    if( opt.autoRotateModel )
    {
      retRot->y = deg;
    }

    ofs = opt.offset;
    if( opt.adjustFlip )
    {
      if( SeqComFunc_IsFlipEffect(opt.vpos,opt.vposSub) )
      {
        ofs.x *= -1;
      }
    }
    if( opt.autoRotatePos )
    {
      BtlvRot_Deg( ofs.x,ofs.z,-deg );
    }
    if( mdl && mdl->IsEnable() )
    {
      gfl2::math::Vector3 adjustScale;
      //カメラの時は1倍以下の補正率を半分にする。
      //モデル、パーティクルはサイズのみ補正なのでSetAdjustScaleにて対応
      if( isCameraAdjust )
      {
        const f32 height = mdl->GetCamAdjustHeight();
        adjustScale.Set(height,height,height);
      }
      else
      {
        const f32 height = mdl->GetAdjustHeight();
        adjustScale.Set(height,height,height);
      }
      if( opt.adjustScale )
      {
        ofs = ofs.MulPerElem(adjustScale);
        *retScale = adjustScale;
      }
    }

    if( opt.node == SEQ_DEF_NODE_INTRO )
    {
      if( mdl && mdl->IsEnable() )
      {
        gfl2::math::Vector3 adjustScale;
        if( mdl->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) )
        {
          ofs.y += POKE_INTOR_HEIGHT;
        }
        else
        {
          gfl2::math::Vector3 center;
          mdl->GetNodePosition_Sequence( SEQ_DEF_NODE_CENTER , &center );
          ofs.y += center.y;
        }
      }
    }

    *retPos = pos+ofs;
  }
  else
  {
    BtlvObject_PokeModel *mdl = GetPokeModel(opt.vpos);
    if( mdl && mdl->IsEnable() )
    {

      gfl2::math::Vector3 pos;
      gfl2::math::Vector3 ofs;

      if( opt.forceUpdate )
      {
        //ノードの強制更新
        mdl->UpdatePositionForce();
        mdl->UpdateAnimationForce();
      }

      mdl->GetNodePosition_Sequence( opt.node , &pos );

      ofs = opt.offset;
      if( opt.adjustFlip )
      {
        if( SeqComFunc_IsFlipEffect(opt.vpos,opt.vposSub) )
        {
          ofs.x *= -1;
        }
      }

      gfl2::math::Vector3 adjustScale;
      //カメラの時は1倍以下の補正率を半分にする。
      //モデル、パーティクルはサイズのみ補正なのでSetAdjustScaleにて対応
      if( isCameraAdjust )
      {
        const f32 height = mdl->GetCamAdjustHeight();
        adjustScale.Set(height,height,height);
      }
      else
      {
        const f32 height = mdl->GetAdjustHeight();
        adjustScale.Set(height,height,height);
      }

      if( opt.autoRotateModel )
      {
        *retRot = mdl->GetRotate();
      }
      if( opt.autoRotatePos )
      {
        BtlvRot_Deg( ofs.x,ofs.z,-mdl->GetRotateY() );
      }

      if( opt.adjustScale )
      {
        ofs = ofs.MulPerElem(adjustScale);
        *retScale = adjustScale;
      }

      *retPos = pos+ofs;
    }
  }
}

void BattleViewSystem::SeqComFunc_MoveRelativePoke( DemoLib::Obj::Object *obj , int frame , const BTLV_RELARIVE_POKE_OPTION &opt )
{
  BtlvObject_PokeModel *mdl = GetPokeModel(opt.vpos);
  if( mdl && mdl->IsEnable() )
  {
    gfl2::math::Vector3 pos;
    gfl2::math::Vector3 rot;
    gfl2::math::Vector3 scale;
    SeqComFunc_GetPokeRelativePos( opt , &pos , &rot , &scale , false );

    if( opt.adjustFlip )
    {
      if( SeqComFunc_IsFlipEffect(opt.vpos,opt.vposSub) )
      {
        obj->SetIsFlip(true);
      }
      else
      {
        obj->SetIsFlip(false);
      }
    }

    if(opt.rate != 100)
    {
      gfl2::math::Vector3 ofs = pos - obj->GetPosition();
      ofs *= (opt.rate*0.01f);
      pos = obj->GetPosition() + ofs;
    }

    if( opt.autoRotateModel )
    {
      obj->SetRotate(rot);
    }
    if( opt.adjustScale )
    {
      obj->SetAdjustScale(scale);
    }

    if( frame == 0 )
    {
      obj->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(GetTaskSystem(),GetTempHeap(),obj,obj->GetPosition(),pos,frame);
    }
  }
}

//特殊座標へオブジェを移動(敵場とか)
void BattleViewSystem::SeqComFunc_MoveSpecialPos( DemoLib::Obj::Object *obj , int frame , int trgType , gfl2::math::Vector3 &ofs , bool isRotate , bool isFlip )
{
  gfl2::math::Vector3 pos,rot,scale;

  BtlvPos targetVpos;

  SeqComFunc_GetSpecialPos(trgType,&pos,&rot);
  switch( trgType )
  {
  case SEQ_DEF_SPPOS_SELF_FIELD:
    targetVpos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    break;
  case SEQ_DEF_SPPOS_ENEMY_FIELD:
  case SEQ_DEF_SPPOS_ENEMY_CENTER:
    targetVpos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
    break;
  }

  if( isFlip && SeqComFunc_IsFlipEffect(targetVpos,targetVpos) )
  {
    obj->SetIsFlip(true);
    ofs.x *= -1;
  }

  if( obj )
  {
    gfl2::math::Vector3 tempOfs;
    tempOfs = ofs;
    if( isRotate )
    {
      BtlvRot_Deg( tempOfs.x,tempOfs.z,-rot.y );
      obj->SetRotate(rot);
    }

    pos += tempOfs;
    if( frame == 0 )
    {
      obj->SetPosition( pos );
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(GetTaskSystem(),GetTempHeap(),obj,obj->GetPosition(),pos,frame);
    }
  }
}

void BattleViewSystem::SeqComFunc_GetSpecialPos(int trgType , gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot )
{
  switch( trgType )
  {
  case SEQ_DEF_SPPOS_SELF_FIELD:
    SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    SeqComFunc_GetPokeFiledPos( retPos , retRot , SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) , true );
    break;
  case SEQ_DEF_SPPOS_ENEMY_FIELD:
    SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
    SeqComFunc_GetPokeFiledPos( retPos , retRot , SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK) , false );
    break;
  case SEQ_DEF_SPPOS_ENEMY_CENTER:
    {
      SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
      retPos->Set(0,0,0);
      retRot->Set(0,0,0);
      BtlvPos vPos;
      int num = 0;
      for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
      {
        if( mComTargetDamage[i] != BTL_VPOS_ERROR )
        {
          gfl2::math::Vector3 tempPos(0,0,0);
          int deg;
          GetDefaultPokePos( mComTargetDamage[i] , &tempPos , &deg );
          *retPos += tempPos;
          num++;
        }
      }
      if( num > 1 )
      {
        retPos->x /= num;
        retPos->y /= num;
        retPos->z /= num;
      }

      //方向は攻撃者の反対
      {
        BtlvPos atkVpos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
        gfl2::math::Vector3 tempPos(0,0,0);
        int deg;
        GetDefaultPokePos( atkVpos , &tempPos , &deg );
        retRot->y = (deg+180)%360;
      }
    }

    break;
  }
}

void BattleViewSystem::SeqComFunc_GetPokeFiledPos( gfl2::math::Vector3 *retPos , gfl2::math::Vector3 *retRot , BtlvPos player , bool isAttack )
{
  retPos->Set(0,0,0);
  retRot->Set(0,0,0);
  int num = 0;
  
  for( int i=0;i<POKE_MODEL_NUM ;i++)
  {
    BtlvPos vPos = static_cast<BtlvPos>(i);
    if( GetBattleRule() == BTL_RULE_ROYAL )
    {
      if( (player == vPos && isAttack == true) || 
          (player != vPos && isAttack == false))
      {
        if( IsPokeExist(vPos) == true )
        {
          gfl2::math::Vector3 tempPos(0,0,0);
          int deg;
          GetDefaultPokePos( vPos , &tempPos , &deg );
          *retPos += tempPos;
          num++;
        }
      }
    }
    else
    {
      if( IsPokeExist(vPos) == true )
      {
        bool atkSide = IsPlayerSide(player);
        bool trgSide = IsPlayerSide(vPos);
        if( (atkSide == trgSide && isAttack == true) || 
            (atkSide != trgSide && isAttack == false))
        {
          gfl2::math::Vector3 tempPos(0,0,0);
          int deg;
          GetDefaultPokePos( vPos , &tempPos , &deg );
          *retPos += tempPos;
          num++;
        }
      }
    }
  }

  if( num > 1 )
  {
    retPos->x /= num;
    retPos->y /= num;
    retPos->z /= num;
  }

  //回転は発動者基準
  {
    gfl2::math::Vector3 tempPos(0,0,0);
    int deg;
    GetDefaultPokePos( player , &tempPos , &deg );
    if( isAttack )
    {
      retRot->y = deg;
    }
    else
    {
      retRot->y = (deg+180)%360;
    }
  }

}


//指定ポケモンへの回転を取得。縦はフラグで制御
void BattleViewSystem::SeqComFunc_CalcPokeDir( const gfl2::math::Vector3 &nowPos , BtlvPos trgPoke , int trgNode , gfl2::math::Vector3 *retRot , bool isVertical )
{
  BtlvObject_PokeModel *mdl = GetPokeModel(trgPoke);

  gfl2::math::Vector3 pokePos;
  
  if( mdl && mdl->IsEnable() )
  {
    mdl->GetNodePosition_Sequence( trgNode , &pokePos );
  }
  else
  {
    int deg;
    GetDefaultPokePos( trgPoke , &pokePos , &deg );
  }
  SeqComFunc_CalcPosDir(nowPos,pokePos,retRot,isVertical);
}

//指定座標への回転を取得。縦はフラグで制御
void BattleViewSystem::SeqComFunc_CalcPosDir( const gfl2::math::Vector3 &nowPos , const gfl2::math::Vector3 &trgPos  , gfl2::math::Vector3 *retRot , bool isVertical )
{
  const gfl2::math::Vector3 ofs = trgPos - nowPos;
  f32 rotY = gfl2::math::Atan2Deg(ofs.x,ofs.z);

  //回転方向の調整(180度基準)
  if( rotY - retRot->y > 180 )
  {
    rotY -= 180*2;
  }
  if( rotY - retRot->y < -180 )
  {
    rotY += 180*2;
  }

  retRot->y = rotY;

  if( isVertical )
  {
    const f32 tempLen = gfl2::math::Vector2(ofs.x,ofs.z).Length();
    const f32 rotV = gfl2::math::Atan2Deg(ofs.y,tempLen);
    retRot->x = -rotV;
  }
}

int BattleViewSystem::SeqComFunc_GetTargetCharaIndex(int trg)
{
  switch( trg )
  {
  case SEQ_DEF_TRAINER_SELF1:
    return 0;
  case SEQ_DEF_TRAINER_ENEMY1:
    return 1;
  case SEQ_DEF_TRAINER_SELF2:
    return 2;
  case SEQ_DEF_TRAINER_ENEMY2:
    return 3;
  case SEQ_DEF_TRAINER_SEQ_TARGET1:
    return mTargetTrainer[0];
  case SEQ_DEF_TRAINER_SEQ_TARGET2:
    return mTargetTrainer[1];
  }
  return -1;
}

//シーケンスの定義から対象のトレーナーを返す
BtlvObject_Chara* BattleViewSystem::SeqComFunc_GetTargetChara(int trg)
{
  int idx = SeqComFunc_GetTargetCharaIndex(trg);
  if( idx != -1 &&
      idx != BTL_VPOS_ERROR )
  {
    return mTrainerModel[idx];
  }
  return NULL;
}

//演出対象ボールの取得(IDの0はBALLID_NULLなので注意)
int BattleViewSystem::SeqComFunc_GetTargetBall(int idx)
{
  if( mEffectBallId[idx] < item::BALLID_MASUTAABOORU || mEffectBallId[idx] > item::BALLID_MAX )
  {
    return item::BALLID_MONSUTAABOORU;
  }

  return mEffectBallId[idx];
}

f32 BattleViewSystem::SeqComFunc_GetPanOffset( int panType )
{
  BtlvPos vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
  switch( panType )
  {
  case SEQ_DEF_SEPAN_NONE://パン補正なし
    return 0;
    //break;
  case SEQ_DEF_SEPAN_ATK: //攻撃側
    vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    break;
  case SEQ_DEF_SEPAN_DEF: //防御側
    vPos = SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
    break;
  }

  switch( GetBattleRule() ) 
  {
  case BTL_RULE_SINGLE:    ///< シングル
    {
      static const f32 panArr[2] =
      {
        -0.75f,
         0.75f,
      };
      return panArr[vPos];
    }
    //break;
  case BTL_RULE_DOUBLE:    ///< ダブル
  case BTL_RULE_ROYAL:    ///< ロイヤル
    {
      static const f32 panArr[4] =
      {
        -0.8f,
         0.8f,
        -0.4f,
         0.4f,
      };
      return panArr[vPos];
    }
    //break;
  case BTL_RULE_INTRUDE:    ///< 乱入
    {
      static const f32 panArr[4] =
      {
        -0.75f,
         0.8f,
        -0.4f,  //実際は居ない
         0.4f,
      };
      return panArr[vPos];
    }
    //break;

  }
  return 0;
}


//攻撃モーション分岐処理
void BattleViewSystem::InitWazaMotionTable(void)
{
  //技モーションテーブルの初期化
  mWazaMotionTableBuf = mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_WAZA_MOTION_TABLE_BIN);
  mWazaMotionTableIdx = (u16*)mWazaMotionTableBuf;

  void* dataTop = (void*)(((int)mWazaMotionTableBuf) + ((MONSNO_END+1)*2));

  mWazaMotionTable = (WAZA_MOTION_TABLE*)(dataTop);
#if 0
  for( int i=0;i<MONSNO_END+1;i++ )
  {
    ARIIZUMI_PRINT("[%d][%d]\n",i,mWazaMotionTableIdx[i]);
  }
  for( int i=0;i<964;i++ )
  {
    ARIIZUMI_PRINT("[%3d]:",i);
    for( int j=0;j<14;j++ )
    {
      ARIIZUMI_PRINT("[%d]",mWazaMotionTable[i].motIdx[j]);
    }
    ARIIZUMI_PRINT(":[%d][%d]\n",mWazaMotionTable[i].spWaza,mWazaMotionTable[i].spMotIdx);
  }
#endif
}

int BattleViewSystem::SeqComFunc_GetWazaMotionType( int monsNo , int formNo , int waza , int motionType )
{

  int dataIdx = mWazaMotionTableIdx[monsNo-1];
  int dataNum = mWazaMotionTableIdx[monsNo-1+1] - dataIdx;
  
  ARIIZUMI_PRINT("BTLV WazaMotion[waza:%d][type:%d] [monsNo:%d][formNo:%d]\n",waza,motionType,monsNo,formNo);
  if( dataNum > formNo )
  {
    dataIdx += formNo;
  }
  
  if( mWazaMotionTable[dataIdx].spWaza != 0 &&
      mWazaMotionTable[dataIdx].spWaza == waza )
  {
    ARIIZUMI_PRINT("BTLV WazaMotion specialMotion[%d]\n",mWazaMotionTable[dataIdx].spMotIdx);
    return mWazaMotionTable[dataIdx].spMotIdx;
  }
  
  ARIIZUMI_PRINT("BTLV WazaMotion Motion[%d]\n",mWazaMotionTable[dataIdx].motIdx[motionType]);
  return mWazaMotionTable[dataIdx].motIdx[motionType];
}

//バトルのメッセージがMainModuleに依存しているので、捕獲チュートリアルなどで出す場合の関数
// @param msgId メッセージID(btl_std.h)
// @param str   表示する文字列
void BattleViewSystem::SeqComFunc_SetEffectMessage_Std(int msgId , gfl2::str::StrBuf* str)
{
  str->SetStr(L"DummyMsg(Std)");
}
void BattleViewSystem::SeqComFunc_SetEffectMessage_Set(int msgId , gfl2::str::StrBuf* str)
{
  str->SetStr(L"DummyMsg(Set)");
}

//------------------------------------------------------
// バイナリテーブルアクセス関連

PokeExtraInfoStruct* BattleViewSystem::getPokeExtraInfo( int monsNo , int formNo )
{
  PokeExtraInfoStruct *baseFormData = NULL;
  int num = getBinaryTableNum(mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_POKE_EXTRA_DATA_BIN));
  //データはmonsNo順で入っているので、0スタートではなくてもOK・・・のはず
  for( int i=monsNo-1;i<num;i++ )
  {
    PokeExtraInfoStruct *data = static_cast<PokeExtraInfoStruct*>(getBinaryTableRow( mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_POKE_EXTRA_DATA_BIN), i));
    if( data->mMonsNo == monsNo )
    {
      if( data->mFormNo == formNo )
      {
        return data;
      }
      else
      if( data->mFormNo == 0 )
      {
        baseFormData = data;
      }
    }
    //データを通り過ぎた フォルム0のデータを返す
    if( data->mMonsNo > monsNo )
    {
      GFL_ASSERT_MSG(baseFormData,"PokeData is not found![idx:%d][mons:%d form:%d]\n",i,monsNo,formNo);
      ARIIZUMI_PRINT("PokeData Return base data[mons:%d form:%d]\n",monsNo,formNo);
      return baseFormData;
    }
  }
  GFL_ASSERT_MSG(baseFormData,"PokeData is not found![idx:end][mons:%d form:%d]\n",monsNo,formNo);
  return NULL;
  //return( (PokeExtraInfoStruct *)getBinaryTableRow( mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_POKE_EXTRA_DATA_BIN), idx) ); 
}

TrainerExtraInfoStruct* BattleViewSystem::getTrainerExtraInfo( int idx )
{ 
  return( (TrainerExtraInfoStruct *)getBinaryTableRow( mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_TRAINER_EXTRA_DATA_BIN), idx) );
}

int BattleViewSystem::getPokeExtraInfoVersion(void)
{
  return( getBinaryTableVersion(mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_POKE_EXTRA_DATA_BIN)) );
}

int BattleViewSystem::getTrainerExtraInfoVersion(void)
{
  return( getBinaryTableVersion(mBattleDataPack.GetData(BL_IDX_BATTLE_DATA_PACK_TRAINER_EXTRA_DATA_BIN)) );
}

void* BattleViewSystem::getBinaryTableRow( void *buf, int idx )const
{
  if (!checkBinaryTableMAGIC (buf))
    {
      return NULL;
    }

  int num =   ((PokeExtraInfoStruct_file_headder *)buf)->mNumOfElem;
  int dsize =   ((PokeExtraInfoStruct_file_headder *)buf)->mElemSize;
  if (num <= idx)
    {
      return NULL;
    }

  u8 *ptr = (u8 *)buf;

  ptr += sizeof (PokeExtraInfoStruct_file_headder);
  ptr += (dsize * idx);

  return (ptr);

}

int BattleViewSystem::getBinaryTableVersion ( void *buf )const
{
  if (!checkBinaryTableMAGIC (buf))
    {
      return -1;
    }

  return ( ((PokeExtraInfoStruct_file_headder *)buf)->mVersion );

}

bool BattleViewSystem::checkBinaryTableMAGIC( void *buf )const
{
  if (((PokeExtraInfoStruct_file_headder *)buf)->mMagic == 0x4647 )  // 0x4647 = 'GF'
    {
      return true;
    }

  return false;
}

int BattleViewSystem::getBinaryTableNum( void *buf )const
{
  if (!checkBinaryTableMAGIC (buf))
  {
    return 0;
  }
  int num =   ((PokeExtraInfoStruct_file_headder *)buf)->mNumOfElem;
  return num;
}

//------------------------------------------------------
//ポケモンの描画ソート
void BattleViewSystem::SortPokeLayerId(void)
{
  BtlvObject_PokeModel *pokeArr[POKEMODEL_NUM];
  f32 lenArr[POKEMODEL_NUM];

  gfl2::math::Vector3 camPos = GetCameraSys()->GetPositionAnime();
  //距離計測
  for(int i=0;i<POKEMODEL_NUM;i++ )
  {
    pokeArr[i] = GetPokeModel(static_cast<BtlvPos>(i));
    if( pokeArr[i] )
    {
      gfl2::math::Vector3 pos = pokeArr[i]->GetPosition();

      pos -= camPos;

      lenArr[i] = pos.LengthSq();
    }
  }

  //ソート
  for( int i=0;i<POKEMODEL_NUM-1;i++ )
  {
    for( int ii=i;ii<POKEMODEL_NUM;ii++ )
    {
      if( lenArr[i] < lenArr[ii] )
      {
        f32 temp = lenArr[i];
        lenArr[i] = lenArr[ii];
        lenArr[ii] = temp;

        BtlvObject_PokeModel *tempIdx = pokeArr[i];
        pokeArr[i] = pokeArr[ii];
        pokeArr[ii] = tempIdx;
      }
    }
  }

  //ID指定
  static const u8 defaultPriority = gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::s_DefaultPriority;
  for(int i=0;i<POKEMODEL_NUM;i++ )
  {
    if( pokeArr[i] && pokeArr[i]->IsEnable() )
    {
      pokeArr[i]->GetPokeModel()->GetModelInstanceNode()->SetSortPriority(defaultPriority + i + 1);
    }
  }
}

//------------------------------------------------------
//サウンド系

void BattleViewSystem::UpdateSound(void)
{
  //ボリューム戻しのチェック
  if( mIsPlayVoice )
  {
    bool isPlay = false;
    //for( int i=0 ; i<=BTL_VPOS_FAR_3 ; i++ )
    for( int i=0 ; i<VOICE_PLAYER_NUM ; i++ )
    {
      if( mIsPlayVoiceFlg[i] )
      {
        if( mIsPlayVoice3D[i] )
        {
          if( Sound::IsVoice3DActorFinished(i) == false )
          {
            isPlay = true;
            break;
          }
        }
        else
        {
          //GFL_PRINT("[%d]",xy_snd::IsVoiceFinished(i));
          if( Sound::IsVoiceFinished(i) == false )
          {
            isPlay = true;
            break;
          }
        }
      }
    }
    //GFL_PRINT("\n");
    if( isPlay == false )
    {
      mIsPikachu5Fade = false;
      GetBattleSound()->BgmVolumeReset_Voice();
      mIsPlayVoice = false;
      for( int i=0 ; i<VOICE_PLAYER_NUM ; i++ )
      {
        mIsPlayVoiceFlg[i] = false;
      }
    }
  }
}

void BattleViewSystem::PlaySe( int seNo , u32 player , s32 ctrlId )
{
  if( IsRecordStartSkip() ){ return; }

  Sound::PlaySE( seNo , 0 , ctrlId , player );
}

bool BattleViewSystem::IsFinishSe( int seNo , s32 ctrlId)
{
  return Sound::IsSEFinished( seNo , ctrlId );
}

void BattleViewSystem::StopSe( int seNo )
{
  Sound::StopSE( seNo );
}

void BattleViewSystem::PlayVoice( BtlvPos pos , bool isFadeBgm , Sound::VoiceType type )
{
  if( IsRecordStartSkip() ){ return; }
  
  //変身はbppに変化が無いので、モデルからMonsnoとFormnoを取って鳴き声を鳴らす。
  BtlvObject_PokeModel *poke = GetPokeModel(pos);
  Sound::PlayVoice( ((u8)pos)/2 , static_cast<MonsNo>(poke->GetMonsNo()) , static_cast<FormNo>(poke->GetFormNo()) , type );
  
  mIsPlayVoiceFlg_Waza[pos/2] = true;
  mIsPlayVoice3D[pos/2] = false;
  if( isFadeBgm )
  {
    GetBattleSound()->BgmVolumeDown_Voice(mIsPikachu5Fade);
    mIsPlayVoiceFlg[pos/2] = true;
    mIsPlayVoice = true;
  }
}

void BattleViewSystem::PlayVoiceId( BtlvPos pos , int id ,bool isFadeBgm , Sound::VoiceType type )
{
  if( IsRecordStartSkip() ){ return; }

  //変身はbppに変化が無いので、モデルからMonsnoとFormnoを取って鳴き声を鳴らす。
  BtlvObject_PokeModel *poke = GetPokeModel(pos);
  Sound::PlayVoice( ((u8)pos)/2 , (VoiceID)id );

  mIsPlayVoiceFlg_Waza[pos/2] = true;
  mIsPlayVoice3D[pos/2] = false;
  if( isFadeBgm )
  {
    GetBattleSound()->BgmVolumeDown_Voice(mIsPikachu5Fade);
    mIsPlayVoiceFlg[pos/2] = true;
    mIsPlayVoice = true;
  }
}

void BattleViewSystem::PlayVoice3D( BtlvPos pos , bool isFadeBgm , Sound::VoiceType type )
{
  if( IsRecordStartSkip() ){ return; }

  //変身はbppに変化が無いので、モデルからMonsnoとFormnoを取って鳴き声を鳴らす。
  BtlvObject_PokeModel *poke = GetPokeModel(pos);
  Sound::PlayVoice3DActor( (int)pos/2 , static_cast<MonsNo>(poke->GetMonsNo()) , static_cast<FormNo>(poke->GetFormNo()) , type );

  const gfl2::math::Vector3 vecPos = GetPokeModel(pos)->GetPosition();

  Sound::ChangeVoice3DActorPos( (u8)pos/2, gfl2::math::VEC3(vecPos.x,vecPos.y,vecPos.z) );

  Sound::ChangeVoice3DActorPitch( (u8)pos/2 , 1.0f );
  Sound::ChangeVoice3DActorVolume( (u8)pos/2 , 1.0f );

  mIsPlayVoiceFlg_Waza[pos/2] = true;
  mIsPlayVoice3D[pos/2] = true;
  if( isFadeBgm )
  {
    GetBattleSound()->BgmVolumeDown_Voice(mIsPikachu5Fade);
    mIsPlayVoiceFlg[pos/2] = true;
    mIsPlayVoice = true;
  }
}

void BattleViewSystem::PlayVoiceId3D( BtlvPos pos , int id ,bool isFadeBgm , Sound::VoiceType type )
{
  if( IsRecordStartSkip() ){ return; }

  //変身はbppに変化が無いので、モデルからMonsnoとFormnoを取って鳴き声を鳴らす。
  BtlvObject_PokeModel *poke = GetPokeModel(pos);
  Sound::PlayVoice3DActor( (int)pos/2 , (VoiceID)id );

  const gfl2::math::Vector3 vecPos = GetPokeModel(pos)->GetPosition();

  Sound::ChangeVoice3DActorPos( (u8)pos/2, gfl2::math::VEC3(vecPos.x,vecPos.y,vecPos.z) );

  Sound::ChangeVoice3DActorPitch( (u8)pos/2 , 1.0f );
  Sound::ChangeVoice3DActorVolume( (u8)pos/2 , 1.0f );

  mIsPlayVoiceFlg_Waza[pos/2] = true;
  mIsPlayVoice3D[pos/2] = true;
  if( isFadeBgm )
  {
    GetBattleSound()->BgmVolumeDown_Voice(mIsPikachu5Fade);
    mIsPlayVoiceFlg[pos/2] = true;
    mIsPlayVoice = true;
  }
}

void BattleViewSystem::PlayDeadVoice3D( BtlvPos pos , bool isFadeBgm , Sound::VoiceType type )
{
  if( IsRecordStartSkip() ){ return; }

  //変身はbppに変化が無いので、モデルからMonsnoとFormnoを取って鳴き声を鳴らす。
  BtlvObject_PokeModel *poke = GetPokeModel(pos);

  if( poke->GetMonsNo() == MONSNO_PIKATYUU )
  {
    //ピカチュウのみ
    PlayVoice3D( pos , true , Sound::VOICE_TYPE_DOWN );
  }
  else
  {
    PlayVoice3D( pos );
    Sound::ChangeVoice3DActorPitch( (u8)pos/2 , 0.83f );
  }
}


void BattleViewSystem::CheckPinchBgm(void)
{
  //SangoBTS1319
  //入れ替えの時にゲージの更新のたびにリクエストがかかると、
  //Aピンチ B平気 を入れ替えた場合、Aが先に更新が入り、
  //一瞬両方ともピンチじゃなくなる。
  
  if( mReqCheckPinchBgm )
  {
    mReqCheckPinchBgm = false;
    if( mCanChangePinchBgm == false )
    {
      return;
    }
    
    if( IsRecordStartSkip() )
    {
      return;
    }
    
    for( int i=0;i<POKEMODEL_NUM;i++ )
    {
      BtlvPos vPos = static_cast<BtlvPos>(i);
      if( IsPlayerSide(vPos) == true )
      {
        BtlvObject_PokeModel *poke = GetPokeModel(vPos);
        
        if( poke && poke->IsEnable() )
        {
          if( GetUiSys()->IsPinch( vPos ) == true )
          {
            if( poke->GetIsPlayPinchSe() == false )
            {
              poke->SetIsPlayPinchSe(true);
              if( GetBattleSound() )
              {
                GetBattleSound()->StartPinchBGM();
              }
            }
          }
          else
          {
            if( poke->GetIsPlayPinchSe() )
            {
              poke->SetIsPlayPinchSe(false);
              if( GetBattleSound() )
              {
                GetBattleSound()->StopPinchBGM();
              }
            }
          }
        }
      }
    }
  }
}
//------------------------------------------------------
//時間更新処理
void BattleViewSystem::UpdateTime(void)
{
  if( IsFinishLoadFieldWaza() == false )
  {
    //読み込み中は背景が消えるので、時間による更新を一旦無効化。
    return;
  }
  if( GetBgSystem()->IsEnalbe() == false )
  {
    return;
  }

  //トレーナー用
  f32 valDegTr;
  f32 holDegTr;
  //ポケモン用
  f32 valDegPoke;
  f32 holDegPoke;
  bool enalbeLightDir = false;

  PokeTool::POKE_ENV_TYPE envType;

  // 室内ではSkyboxは無効
  gfl2::math::Vector3 pos(0,0.5f,0.5f);

  //天球が有効かで見てしまうと非表示時に無効になってしまう
  if( IsEnableSkyBox() )
  {
    //大森さん要望で暗くしない
    envType = PokeTool::POKE_ENV_NONE;
/*
    GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
    switch( timeZone )
    {
    case GameSys::TimeZone::MORNING: ///< 朝
    case GameSys::TimeZone::NOON:    ///< 昼
      envType = PokeTool::POKE_ENV_NONE;
      break;
    case GameSys::TimeZone::EVENING: ///< 夕
      envType = PokeTool::POKE_ENV_EVENING;
      break;
    case GameSys::TimeZone::NIGHT:   ///< 夜
    case GameSys::TimeZone::MIDNIGHT:///< 深夜
      envType = PokeTool::POKE_ENV_NIGHT;
      break;
    }
*/

    enalbeLightDir = true;

    //ARIIZUMI_PRINT("[%f][%f][%f]\n",pos.x,pos.y,pos.z);
    GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
    bool isNoon = false;
    if (timeZone == GameSys::TimeZone::MORNING ||
        timeZone == GameSys::TimeZone::NOON ||
        timeZone == GameSys::TimeZone::EVENING)
    {
      isNoon = true;
    }

    s32 hour,min,sec;
    GetSkyBoxTime(&hour,&min,&sec);
    f32 calcMin = hour * 60 + min;
    f32 tempX = 0;
    f32 tempY = 0;
    if (isNoon == true)
    {
      // 6時～17時
      //1～-1に変換する
      tempX = (calcMin - 360 - 360) / -360.0f;
      //0～1～0に変換する
      tempY = -(gfl2::math::Abs(tempX) - 1.0f);
    }
    else
    {
      // 18時～5時
      // -1～1に変換する(太陽の位置計算でやっていたため日没後は下で反転するようになってるので-1～1)
      if (hour >= 18)
      {
        // 18時～23時
        tempX = (calcMin - 1080 - 360) /360.0f;
      }
      else
      {
        // 0時～5時
        tempX = (calcMin +360 - 360) / 360.0f;
      }
      //0～-1～0に変換する
      tempY = gfl2::math::Abs(tempX) - 1.0f;
    }
    pos.x = tempX;
    pos.y = tempY;

    {
      //横位置は太陽のX座標のみをチェック。
      //-1.0～1.0なのでMINは0で良い。
      {
        //-45～45を振る
        static const int MAX_DEG = 55;  //ちょっと大きめに。
        static const int MIN_DEG = 0;
      
        holDegPoke = MAX_DEG * -pos.x;
        holDegPoke = gfl2::math::Clamp( holDegPoke , -45.0f , 45.0f );
      }
      {
        //トレーナーは-35～35
        static const int MAX_DEG = 40;  //ちょっと大きめに。
        static const int MIN_DEG = 0;
      
        holDegTr = MAX_DEG * -pos.x;
        holDegTr = gfl2::math::Clamp( holDegTr , -35.0f , 35.0f );
      }
    }

    //縦位置
    if (isNoon)
    {
      //日中。
      {
        //1～0で70～30を振る
        static const int MAX_DEG = 70;
        static const int MIN_DEG = 30;
        valDegPoke = MIN_DEG + (MAX_DEG-MIN_DEG)*pos.y;
        valDegPoke = gfl2::math::Clamp( valDegPoke , 30.0f , 70.0f );
      }
      {
        //トレーナーは0～15
        static const int MAX_DEG = 15;
        static const int MIN_DEG = 0;
        valDegTr = MIN_DEG + (MAX_DEG-MIN_DEG)*pos.y;
        valDegTr = gfl2::math::Clamp( valDegTr , 00.0f , 15.0f );
      }
    }
    else
    {
      //日没後。
      //1～0で50～30を振る 。
      //ただし、MAXを大きめに渡しClampすることで、50の範囲を大きくする
      {
        //1～0で50～30を振る 。
        //ただし、MAXを大きめに渡しClampすることで、50の範囲を大きくする
        static const int MAX_DEG = 50+20;
        static const int MIN_DEG = 30;
        valDegPoke = MIN_DEG + (MAX_DEG-MIN_DEG)*-pos.y;
        valDegPoke = gfl2::math::Clamp( valDegPoke , 30.0f , 50.0f );
        //横位置にも補正を掛ける
        holDegPoke *= (20.0f - (valDegPoke-30.0f))/-20.0f; 
      }
      {
        //トレーナーは横-35～35
        //トレーナーは縦0～15
        static const int MAX_DEG = 15+6;
        static const int MIN_DEG = 0;
        valDegTr = MIN_DEG + (MAX_DEG-MIN_DEG)*-pos.y;
        valDegTr = gfl2::math::Clamp( valDegTr , 0.0f , 15.0f );
        //横位置にも補正を掛ける
        holDegTr *= (20.0f - valDegTr)/-20.0f;
      }
    }
    
    //モンスターボールは生成時にやってる
    /*
    switch( GetPeriodType() )
    {
    case xy_system::skybox::PERIOD_TYPE_NIGHT:
      envShader = poke_tool::POKE_ENV_NIGHT;
      trainerEnvShader = trainer::TrModel::ENV_LIGHT_TIME_NIGHT;
      break;
    case xy_system::skybox::PERIOD_TYPE_EVENING:
      envShader = poke_tool::POKE_ENV_EVENING;
      trainerEnvShader = trainer::TrModel::ENV_LIGHT_TIME_EVENING;
      break;
    case xy_system::skybox::PERIOD_TYPE_MORNING:
      envShader = poke_tool::POKE_ENV_NONE;
      trainerEnvShader = trainer::TrModel::ENV_LIGHT_TIME_MORNING;
      break;
    case xy_system::skybox::PERIOD_TYPE_DAYTIME:
      envShader = poke_tool::POKE_ENV_NONE;
      trainerEnvShader = trainer::TrModel::ENV_LIGHT_TIME_DAYTIME;
      break;
    }
    */
  }
  else
  {
    valDegTr = 7.5f;
    holDegTr = 15.0f;
    valDegPoke = 50;
    holDegPoke = 40;
    enalbeLightDir = true;
    //屋内でも暗くしない(明るい場合がある
    //envType = PokeTool::POKE_ENV_CAVE;
    envType = PokeTool::POKE_ENV_NONE;
  }

  //ライトの向き適用
  if( enalbeLightDir == true )
  {
    const f32 valRadTr = GFL_MATH_DEG_TO_RAD( valDegTr );
    const f32 holRadTr = GFL_MATH_DEG_TO_RAD( holDegTr );
    const f32 valRadPoke = GFL_MATH_DEG_TO_RAD( valDegPoke );
    const f32 holRadPoke = GFL_MATH_DEG_TO_RAD( holDegPoke );

    gfl2::math::Vector3 lightVecTr( 0,0,1 );
    gfl2::math::Vector3 lightVecPoke( 0,0,1 );
      
    BtlvRot_Rad( lightVecTr.z , lightVecTr.y , valRadTr );
    BtlvRot_Rad( lightVecTr.x , lightVecTr.z , holRadTr );
    BtlvRot_Rad( lightVecPoke.z , lightVecPoke.y , valRadPoke );
    BtlvRot_Rad( lightVecPoke.x , lightVecPoke.z , holRadPoke );

    lightVecTr *= -1;
    lightVecPoke *= -1;

    for( int i=0;i<4;i++ )
    {
      gfl2::math::Vector3 tempVecTr = lightVecTr;
      gfl2::math::Vector3 tempVecPoke = lightVecPoke;

      if( GetBattleRule() == BTL_RULE_ROYAL )
      {
        //ロイヤルはまわす
        BtlvRot_Rad( tempVecTr.x , tempVecTr.z , GFL_MATH_DEG_TO_RAD(180+(-90*i)) );
        BtlvRot_Rad( tempVecPoke.x , tempVecPoke.z , GFL_MATH_DEG_TO_RAD(180+(-90*i)) );
      }
      else
      {
        if( i%2 == 0 )
        {
          tempVecTr.z *= -1;
          tempVecPoke.z *= -1;
        }
        //バトルフェスの控えトレーナー例外(3番の立ち位置のトレーナーは自分側なので、反転
        if (IsBattleFesMode() && i == 3)
        {
          tempVecTr.z *= -1;
        }
      }
  #if PM_DEBUG
      if( mDebugUseLight )
      {
        tempVecTr = mDebugCharaLightVec[i];
        tempVecPoke = mDebugPokeLightVec[i];
      }
  #endif

      if( GetRenderSys()->GetCharaLightNode(i) )
      {
        BtlvObject_Chara *chara = GetTrainerModelObj(i);
        if( chara )
        {
          if( chara->IsUseEffectLight() )
          {
            GetRenderSys()->GetCharaLightNode(i)->SetDirection(chara->GetEffectLightDir());
          }
          else
          {
            GetRenderSys()->GetCharaLightNode(i)->SetDirection(tempVecTr);
          }
        }
      }
      if( GetPokeModel((BtlvPos)i) && 
          GetPokeModel((BtlvPos)i)->IsAddRenderPath())
      {
        int lightNo = GetPokeModel((BtlvPos)i)->GetLightNo();
        gfl2::renderingengine::scenegraph::instance::LightNode *light = GetRenderSys()->GetDrawEnvNode()->GetLightSet(lightNo)->GetLightNode(0);
        if( GetPokeModel((BtlvPos)i)->IsUseEffectLight() )
        {
          light->SetDirection(GetPokeModel((BtlvPos)i)->GetEffectLightDir());
        }
        else
        {
          light->SetDirection(tempVecPoke);
        }
      }
    }
  }
  else
  {
    for( int i=0;i<4;i++ )
    {
      //ライトの指定が無効でもエフェクト指定のライトの値は反映させる
      if( GetRenderSys()->GetCharaLightNode(i) )
      {
        BtlvObject_Chara *chara = GetTrainerModelObj(i);
        if( chara )
        {
          if( chara->IsUseEffectLight() )
          {
            GetRenderSys()->GetCharaLightNode(i)->SetDirection(chara->GetEffectLightDir());
          }
        }
      }
      if( GetPokeModel((BtlvPos)i) && 
        GetPokeModel((BtlvPos)i)->IsAddRenderPath())
      {
        int lightNo = GetPokeModel((BtlvPos)i)->GetLightNo();
        if( GetPokeModel((BtlvPos)i)->IsUseEffectLight() )
        {
          gfl2::renderingengine::scenegraph::instance::LightNode *light = GetRenderSys()->GetDrawEnvNode()->GetLightSet(lightNo)->GetLightNode(0);
          light->SetDirection(GetPokeModel((BtlvPos)i)->GetEffectLightDir());
        }
      }
    }
  }

  //明るさ適用
  for( int i=0;i<4;i++ )
  {
    BtlvObject_Chara *chara = GetTrainerModelObj(i);
    if( chara )
    {
      f32 tempCol = 1.0f;
      if( mPlayingTrainerIntro == true )
      {
        //トレーナー導入演出中はふしぎ背景なので、明るい
        tempCol = 1.0f;
      }
      else
      {
        switch( envType )
        {
        case PokeTool::POKE_ENV_NONE:    //朝昼
          tempCol = 1.0f;
          break;
        case PokeTool::POKE_ENV_EVENING: //夕
          tempCol = 0.9f;
          break;
        case PokeTool::POKE_ENV_NIGHT:   //夜
          tempCol = 0.7f;
          break;
        case PokeTool::POKE_ENV_CAVE:    //洞窟(バトルで天球が無い場合はNONEへ流れる
          tempCol = 0.7f;
          break;
        }

      }
      gfl2::math::Vector4 envColor(tempCol,tempCol,tempCol,1.0f);
      chara->SetConstantColor(5,envColor);
    }
    if( GetPokeModel((BtlvPos)i) && 
        GetPokeModel((BtlvPos)i)->IsAddRenderPath())
    {
      GetPokeModel((BtlvPos)i)->GetPokeModel()->SetEnvColor(envType);
    }
  }
#if PM_DEBUG
  if( mDebugUseLight )
  {
    GetRenderSys()->GetLightNode()->SetDirection(mDebugModelLightVec);
  }
#endif

}

//------------------------------------------------------
//開始時読み込み
void BattleViewSystem::DataLoadStart(int idx)
{
  if( mPreLoadList[idx] )
  {
    ARIIZUMI_PRINT("■StartLoad[%d]■\n",idx);
    gfl2::util::List<BtlvLoaderBase*>::Iterator it = mPreLoadList[idx]->Begin();
    while( (*it) != NULL )
    {
      BtlvLoaderBase *data = (*it);
      ++it;

      data->StartLoad();
      data->SetIsLoad(true);
    }
  }
}

void BattleViewSystem::DataLoadUpdateLoad(int idx)
{
  if( mPreLoadList[idx] )
  {
    gfl2::util::List<BtlvLoaderBase*>::Iterator it = mPreLoadList[idx]->Begin();
    while( (*it) != NULL )
    {
      BtlvLoaderBase *data = (*it);
      ++it;

      data->LoadUpdate();
    }
  }
}

bool BattleViewSystem::DataLoadWait(int idx)
{
  if( mPreLoadList[idx] )
  {
    gfl2::util::List<BtlvLoaderBase*>::Iterator it = mPreLoadList[idx]->Begin();
    while( (*it) != NULL )
    {
      BtlvLoaderBase *data = (*it);
      ++it;
      if( data->IsFinishLoad() == false )
      {
        ARIIZUMI_PRINT("■WaitLoad[%d]■\n",idx);
        return false;
      }
    }
    ARIIZUMI_PRINT("■WaitLoadEnd[%d]■\n",idx);
  }
  return true;
}

void BattleViewSystem::DataLoadEnd(int idx)
{
  if( mPreLoadList[idx] )
  {
    ARIIZUMI_PRINT("■EndLoad[%d]■\n",idx);
    gfl2::util::List<BtlvLoaderBase*>::Iterator it = mPreLoadList[idx]->Begin();
    while( (*it) != NULL )
    {
      BtlvLoaderBase *data = (*it);
      ++it;

      data->Create();
      mPreLoadList[0]->Remove(data);
      GFL_SAFE_DELETE( data );
    }
  }
}

//ファイル読み込みようのヒープの関係で1個ずつ読む版
void BattleViewSystem::DataLoadStartOne(int idx)
{
  //処理無し
}

void BattleViewSystem::DataLoadUpdateLoadOne(int idx)
{
  bool isLoading = false;
  if( mPreLoadList[idx] )
  {
    gfl2::util::List<BtlvLoaderBase*>::Iterator it = mPreLoadList[idx]->Begin();
    while( (*it) != NULL )
    {
      BtlvLoaderBase *data = (*it);
      ++it;
      
      if( data->GetIsLoad() )
      {
        data->LoadUpdate();
        if( data->IsFinishLoad() == false )
        {
          isLoading = true;
          return;
        }
      }
      else
      {
        if( isLoading == false )
        {
          data->StartLoad();
          data->SetIsLoad(true);
          return;
        }
      }
    }
  }
}

//------------------------------------------------------
//天候関連
bool BattleViewSystem::IsEnableSkyBox(void)
{
  //天球が有効かで見てしまうと非表示時に無効になってしまう
  if( GetFieldSituation() && 
    GetFieldSituation()->bgComponent.useSkyBox == true )
  {
    return true;
  }
  return false;
}

int BattleViewSystem::GetWeatehrLoadIndex(void)
{
  return mWeatehrLoadIdx;
}

int BattleViewSystem::GetWeatehrReverceLoadIndex(void)
{
  if( mWeatehrLoadIdx == 0 )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


//リソース取得
int BattleViewSystem::GetWeatherResourceIdx(int type , int idx)
{
  switch(type)
  {
  case SEQ_DEF_WEATHER_NONE:  //リセット
    //平常時エフェクト
    if( IsEnableSkyBox() )
    {
      GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
      switch( timeZone )
      {
      case GameSys::TimeZone::MORNING: ///< 朝
      case GameSys::TimeZone::NOON:    ///< 昼
      case GameSys::TimeZone::NIGHT:   ///< 夜
      case GameSys::TimeZone::MIDNIGHT:///< 深夜
      case GameSys::TimeZone::EVENING: ///< 夕
        if( idx == 0 )
        {
          return GARC_wazaeff_resource_et007_screen_sun_add_lz_BPTCL;
        }
        break;
      }
    }
    break;
  case SEQ_DEF_WEATHER_SHINE: //晴れ
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et004_sunlight_lz_BPTCL;
    }
    break;
  case SEQ_DEF_WEATHER_RAIN:  //雨
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et001_rain_lz_BPTCL;
    }
    break;
  case SEQ_DEF_WEATHER_SNOW:  //雪
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et002_arare_lz_BPTCL;
    }
    break;

  case SEQ_DEF_WEATHER_SAND:  //砂嵐
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et003_sandstorm_bill_lz_GFBMDLP;
    }
    else
    if( idx == 1 )
    {
      return GARC_wazaeff_resource_et003_sandstorm_bill_lz_MGFBMOT;
    }
    //break;  //trough
  case SEQ_DEF_WEATHER_STORM: //大雨
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et005_rain_lz_BPTCL;
    }
    else
    if( idx == 1 )
    {
      switch(GetBattleRule())
      {
      case BTL_RULE_SINGLE:
      case BTL_RULE_INTRUDE:
        return GARC_wazaeff_resource_et005_thunder_s_lz_BPTCL;
      case BTL_RULE_DOUBLE:
        return GARC_wazaeff_resource_et005_thunder_m_lz_BPTCL;
      case BTL_RULE_ROYAL:
        return GARC_wazaeff_resource_et005_thunder_l_lz_BPTCL;
      }
    }
    //break;  //trough
  case SEQ_DEF_WEATHER_DAY:   //大日照り
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et006_sunlight_lz_BPTCL;
    }
    else
    if( idx == 1 )
    {
      switch(GetBattleRule())
      {
      case BTL_RULE_SINGLE:
      case BTL_RULE_INTRUDE:
        return GARC_wazaeff_resource_et006_fire_s_lz_BPTCL;
      case BTL_RULE_DOUBLE:
        return GARC_wazaeff_resource_et006_fire_m_lz_BPTCL;
      case BTL_RULE_ROYAL:
        return GARC_wazaeff_resource_et006_fire_l_lz_BPTCL;
      }
    }
    else
    {
      return GARC_wazaeff_resource_et006_screen_add_lz_BPTCL;
    }
    //break;  //trough
  case SEQ_DEF_WEATHER_TURBULENCE:  //乱気流
    if( idx == 0 )
    {
      return GARC_wazaeff_resource_et007_storm_bill_lz_GFBMDLP;
    }
    else
    if( idx == 1 )
    {
      return GARC_wazaeff_resource_et007_storm_bill_lz_MGFBMOT;
    }
    //break;  //trough
  }
  return -1;
}

//読み込み開始(先読み)
void BattleViewSystem::LoadWeatherResource(int type)
{
  int loadIdx = GetWeatehrLoadIndex();
  //前々回の中身のクリア
  DeleteWeather(loadIdx);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_WAZAEFF_RESOURCE;
  req.align = 128;
  req.heapForBuf = GetWeatherHeap(loadIdx);
  req.heapForReq = GetTempHeap()->GetLowerHandle();
	req.heapForCompressed = GetTempHeap()->GetLowerHandle();

  for( int i=0;i<3;i++ )
  {
    int datIdx = GetWeatherResourceIdx(type,i);
    if( datIdx != -1 )
    {
      req.datId = datIdx;
      req.ppBuf = &mWeatherData[loadIdx].mDataBuf[i];
      GetAsyncFileManager()->AddArcFileLoadDataReq(req);
    }
    else
    {
      mWeatherData[loadIdx].mDataBuf[i] = NULL;
    }
  }
}

bool BattleViewSystem::IsFinishLoadWeatherResource(void)
{
  int loadIdx = GetWeatehrLoadIndex();

  for( int i=0;i<3;i++ )
  {
    if( GetAsyncFileManager()->IsArcFileLoadDataFinished( &mWeatherData[loadIdx].mDataBuf[i] ) == false )
    {
      return false;
    }
  }
  return true;
}

//天候開始
void BattleViewSystem::StartWeather(int type )
{
  int loadIdx = GetWeatehrLoadIndex();

  //前回の停止
  StopWeather(GetWeatehrReverceLoadIndex());

  switch(type)
  {
  case SEQ_DEF_WEATHER_NONE:  //リセット
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(static_cast<System::Skybox::Skybox::SkyType>(GetFieldSituation()->sky_type),10);
      if( GetFieldSituation()->sky_type == System::Skybox::Skybox::SKY_TYPE_CLOUDY )
      {
        GetBgSystem()->RequestAnime(BgSystem::ANM_CLOUD_LOOP);
      }
      else
      {
        GetBgSystem()->RequestAnime(BgSystem::ANM_LOOP);
      }

      GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::GetNowTimeZone();
      switch( timeZone )
      {
      case GameSys::TimeZone::MORNING: ///< 朝
      case GameSys::TimeZone::NOON:    ///< 昼
      case GameSys::TimeZone::NIGHT:   ///< 夜
      case GameSys::TimeZone::MIDNIGHT:///< 深夜
        WeatherFunc_CreateScreenEffect(&mWeatherData[loadIdx],0);
        mWeatherData[loadIdx].mEffectObj[0]->SetPosition(0.0f,1.4f,5.0f);
        mWeatherData[loadIdx].mEffectObj[0]->SetRotate(0.0f,0.0f,-15.0f);
        mWeatherData[loadIdx].mEffectObj[0]->SetScale(0.15f,0.07f,0.10f);
        break;

      case GameSys::TimeZone::EVENING: ///< 夕
        //none
        break;
      }
    }
    else
    {
      //天球が無い場合は通常ループに戻す
      GetBgSystem()->RequestAnime(BgSystem::ANM_LOOP);
    }
    break;
    //エフェクト単発物
  case SEQ_DEF_WEATHER_RAIN:  //雨
    GetBgSystem()->RequestAnime(BgSystem::ANM_RAIN_LOOP);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],0);
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_CLOUDY,10);
    }
  break;
  case SEQ_DEF_WEATHER_SNOW:  //雪
    GetBgSystem()->RequestAnime(BgSystem::ANM_SNOW_LOOP);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],0);
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_CLOUDY,10);
    }
    break;
  case SEQ_DEF_WEATHER_SHINE: //晴れ]
    GetBgSystem()->RequestAnime(BgSystem::ANM_SUNLIGHT_LOOP);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],0);
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_SUNNY,10);
    }
    break;
  case SEQ_DEF_WEATHER_STORM: //大雨
    GetBgSystem()->RequestAnime(BgSystem::ANM_RAIN_LOOP);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],0);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],1);
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_CLOUDY,10);
    }
    break;
  case SEQ_DEF_WEATHER_DAY:   //大日照り
    GetBgSystem()->RequestAnime(BgSystem::ANM_SUNLIGHT_LOOP);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],0);
    WeatherFunc_CreateEffect(&mWeatherData[loadIdx],1);
    WeatherFunc_CreateScreenEffect(&mWeatherData[loadIdx],2);
    if( IsEnableSkyBox() )
    {
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_SUNNY,10);
    }
    break;

  case SEQ_DEF_WEATHER_SAND:  //砂嵐
  case SEQ_DEF_WEATHER_TURBULENCE:  //乱気流
    GetBgSystem()->RequestAnime(BgSystem::ANM_WIND_LOOP);
    WeatherFunc_CreateModel(&mWeatherData[loadIdx]);
    if( IsEnableSkyBox() )
    {
      //砂嵐はデフォルトの天候にする
      GetBgSystem()->GetSkyBox()->ChangeSkyRequest(static_cast<System::Skybox::Skybox::SkyType>(GetFieldSituation()->sky_type),10);
    }
    break;
  }

  mWeatehrLoadIdx = GetWeatehrReverceLoadIndex();
}

//単発エフェクト初期化
void BattleViewSystem::WeatherFunc_CreateEffect(WEATHER_EFFECT_DATA *data,int resIdx)
{
  int loadIdx = GetWeatehrLoadIndex();
  int effResIdx = GetParticleIndexManager()->GetIndex();
  GFL_ASSERT_STOP( effResIdx==-1 ? 0:1 );

  data->mEffectObj[resIdx] = GFL_NEW(GetWeatherHeap(loadIdx))BtlvObject_Effect(this);
  data->mEffectObj[resIdx]->CreateBuffer(data->mDataBuf[resIdx],GetEffectHeap(),effResIdx);
  data->mEffectObj[resIdx]->AddRenderPath(SEQ_DEF_EFF_DRAWTYPE_NORAML);

  GetObjectManager()->AddObject(data->mEffectObj[resIdx]);

}

void BattleViewSystem::WeatherFunc_CreateScreenEffect(WEATHER_EFFECT_DATA *data,int resIdx)
{
  int loadIdx = GetWeatehrLoadIndex();
  int effResIdx = GetParticleIndexManager()->GetIndex();
  GFL_ASSERT_STOP( effResIdx==-1 ? 0:1 );

  data->mEffectObj[resIdx] = GFL_NEW(GetWeatherHeap(loadIdx))BtlvObject_Effect(this);
  data->mEffectObj[resIdx]->CreateBuffer(data->mDataBuf[resIdx],GetEffectHeap(),effResIdx);
  data->mEffectObj[resIdx]->AddRenderPath(SEQ_DEF_EFF_DRAWTYPE_SCREEN);
  data->mEffectObj[resIdx]->SetPosition(0.0f,0.0f,5.0f);
  data->mEffectObj[resIdx]->SetRotate(0.0f,180.0f,0.0f);
  data->mEffectObj[resIdx]->SetScale(0.12f,0.12f,0.12f);

  GetObjectManager()->AddObject(data->mEffectObj[resIdx]);

}

void BattleViewSystem::WeatherFunc_CreateModel(WEATHER_EFFECT_DATA *data)
{
  int loadIdx = GetWeatehrLoadIndex();

  data->mModelObj = GFL_NEW(GetWeatherHeap(loadIdx))BtlvObject_Gfmdl(this,GetRenderSys()->GetRenderingPipeLine());
  data->mModelObj->CreateBuffer(data->mDataBuf[0],GetWeatherHeap(loadIdx),GetTempGLAllocator());
  data->mModelObj->AddRenderPath(SEQ_DEF_DRAWTYPE_SCREEN);
  data->mModelObj->SetAnimation(GetTempGLAllocator(),data->mDataBuf[1]);
  data->mModelObj->SetAnimationIsLoop(true);
  data->mModelObj->SetPosition(0.0f,0.0f,5.0f);
  data->mModelObj->SetRotate(0.0f,180.0f,0.0f);
  data->mModelObj->SetScale(0.12f,0.12f,0.12f);

  GetObjectManager()->AddObject(data->mModelObj);
}

//天候の停止 (破棄ではない
void BattleViewSystem::StopWeather(int idx)
{
  WEATHER_EFFECT_DATA *data = &mWeatherData[idx];
  for( int i=0;i<3;i++ )
  {
    if( data->mEffectObj[i] )
    {
      data->mEffectObj[i]->RemoveRenderPath();
      GetObjectManager()->RemoveObject(data->mEffectObj[i]);
    }
  }
  if( data->mModelObj )
  {
    data->mModelObj->RemoveRenderPath();
    GetObjectManager()->RemoveObject(data->mModelObj);
  }
}

//天候の破棄
void BattleViewSystem::DeleteWeather(int idx)
{
  WEATHER_EFFECT_DATA *data = &mWeatherData[idx];
  for( int i=0;i<3;i++ )
  {
    if( data->mEffectObj[i] )
    {
      GetParticleIndexManager()->ReturnIndex(data->mEffectObj[i]->GetResourceIndex());
      data->mEffectObj[i]->Delete();
      GFL_SAFE_DELETE(data->mEffectObj[i]);
    }
  }
  if( data->mModelObj )
  {
    data->mModelObj->Delete();
    GFL_SAFE_DELETE(data->mModelObj);
  }
  GflHeapSafeFreeMemory(data->mDataBuf[0]);
  GflHeapSafeFreeMemory(data->mDataBuf[1]);
  GflHeapSafeFreeMemory(data->mDataBuf[2]);
}

void BattleViewSystem::SetVisibleWeather(bool isVisible)
{
  int idx = GetWeatehrReverceLoadIndex();
  WEATHER_EFFECT_DATA *data = &mWeatherData[idx];
  for( int i=0;i<3;i++ )
  {
    if( data->mEffectObj[i] )
    {
      data->mEffectObj[i]->SetVisible(isVisible);
    }
  }
  if( data->mModelObj )
  {
    data->mModelObj->SetVisible(isVisible);
  }
}

//フィールド技関連関連
void BattleViewSystem::StartLoadFieldWaza(int type)
{
  mFieldWazaType = type;
  mFieldWazaSeq = 1;
}
bool BattleViewSystem::IsFinishLoadFieldWaza(void)
{
  if( mFieldWazaSeq == 0 )
  {
    return true;
  }
  return false;
}
void BattleViewSystem::UpdateLoadFieldWaza(void)
{
  enum
  {
    SEQ_NONE,
    SEQ_DELETE,
    SEQ_DELETE_WAIT,
    SEQ_LOAD,
    SEQ_LOAD_WAIT,
  };
  switch(mFieldWazaSeq)
  {
  case SEQ_NONE:
    break;
  case SEQ_DELETE:
    mBgSystem->SetVisible(false);
    mBgSystem->Delete();
    mFieldWazaSeq++;
    //break; through
  case SEQ_DELETE_WAIT:
    if( mBgSystem->IsFinishDelete() == false )
    {
      return;
    }
    mFieldWazaSeq++;
    //break; through
  case SEQ_LOAD:
    {
      btl::BgSystem::SETUP_PARAM setupParam;
      if( mFieldWazaType == BTL_GROUND_NONE )
      {
        SetBgSetupParam(&setupParam);
      }
      else
      {
        btl::bg::bgsys_normal_groundID arr[] =
        {
          btl::bg::PLATE_NONE,              //BTL_GROUND_NONE = 0,   ///< なし
          btl::bg::PLATE_GRASS_FIELD,       //BTL_GROUND_GRASS,    ///< グラスフィールド
          btl::bg::PLATE_MIST_FIELD,        //BTL_GROUND_MIST,     ///< ミストフィールド
          btl::bg::PLATE_ELECTRICAL_FIELD,  //BTL_GROUND_ELEKI,    ///< エレキフィールド
          btl::bg::PLATE_PSYCHO_FIELD,      //BTL_GROUND_PHYCHO,   ///< サイコフィールド
        };

        setupParam.groundType = arr[mFieldWazaType];
        setupParam.nearType = btl::bg::NEAR_NONE;
        setupParam.farType = btl::bg::NEAR_NONE;
        setupParam.useSkyBox = false;

      }
      mBgSystem->StartLoad(setupParam);
    }
    mFieldWazaSeq++;
    //break; through
  case SEQ_LOAD_WAIT:
    if( mBgSystem->IsFinishLoad() )
    {
      mBgSystem->Create();
      mBgSystem->SetVisible(true);
      mFieldWazaSeq = 0;
			this->SuspendFrameSkip(); // BgSystemでTextureをVRAM転送する状態になったので、数フレーム、フレームスキップを無効にする。
    }
    break;
  }
}

//待機時状態異常
void BattleViewSystem::Start_WaitCamSickEffect(void)
{
  //ソート
  //同数の配列に乱数入れて、降順ソートと一緒に入れ替える。
  u32 sortArr[POKE_MODEL_NUM];
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    sortArr[i] = static_cast<int>(System::GflUse::GetPublicRand(100));
    mWaitCamSickData.mOrderArr[i] = static_cast<BtlvPos>(i);
  }

  for( int i=0;i<POKE_MODEL_NUM-1;i++ )
  {
    for( int j=0;j<POKE_MODEL_NUM-i;j++ )
    {
      if( sortArr[i] > sortArr[j] )
      {
        u32 temp = sortArr[i];
        sortArr[i] = sortArr[j];
        sortArr[j] = temp;

        BtlvPos temp2 = mWaitCamSickData.mOrderArr[i];
        mWaitCamSickData.mOrderArr[i] = mWaitCamSickData.mOrderArr[j];
        mWaitCamSickData.mOrderArr[j] = temp2;
      }
    }
  }
  mWaitCamSickData.mEnable = true;
  mWaitCamSickData.mSeq = 0;
}

void BattleViewSystem::Finish_WaitCamSickEffect(void)
{
  mWaitCamSickData.mEnable = false;
}

bool BattleViewSystem::IsFinish_WaitCamSickEffect(void)
{
  if( mWaitCamSickData.mSeq == 0 )
  {
    return true;
  }
  return false;
}

void BattleViewSystem::Update_WaitCamSickEffect(void)
{
  enum
  {
    SEQ_WAIT,
    SEQ_START_LOAD,
    SEQ_WAIT_LOAD,
    SEQ_PLAY_EFFECT,
    SEQ_WAIT_EFFECT,
    SEQ_WAIT_NEXT,

    SEQ_NOSICK_WAIT,

    SEQ_GO_NEXT,
  };
  switch(mWaitCamSickData.mSeq )
  {
  case SEQ_WAIT:
    if( mWaitCamSickData.mEnable == false )
    {
      //無効なので待機中
      return;
    }
    mWaitCamSickData.mSeq++;
    //break;
  case SEQ_START_LOAD:
    {
      PokeSick sick = pml::pokepara::SICK_NULL;
      
      BtlvPos trg = mWaitCamSickData.mOrderArr[mWaitCamSickData.mNowOrder];
      BtlvObject_PokeModel *poke = GetPokeModel(trg);

      if( poke->IsEnable() && poke->GetVisible())
      {
        sick = poke->GetSick();
      }

      if( sick == pml::pokepara::SICK_NULL )
      {
        mWaitCamSickData.mCnt = 0;
        mWaitCamSickData.mSeq = SEQ_NOSICK_WAIT;
      }
      else
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.arcId = ARCID_WAZAEFF_RESOURCE;
        req.align = 128;
        req.heapForBuf = GetWaitSickHeap();
        req.heapForReq = GetWaitSickHeap()->GetLowerHandle();
        req.heapForCompressed = GetWaitSickHeap()->GetLowerHandle();
        req.ppBuf = &mWaitCamSickData.mDataBuf;
        switch(sick)
        {
        case pml::pokepara::SICK_MAHI:    // まひ
          req.datId = GARC_wazaeff_resource_es005_paralysis_lz_BPTCL;
          break;
        case pml::pokepara::SICK_NEMURI:  // ねむり
          req.datId = GARC_wazaeff_resource_es001_nemuri_lz_BPTCL;
          break;
        case pml::pokepara::SICK_KOORI:   // こおり
          req.datId = GARC_wazaeff_resource_es004_ice_lz_BPTCL;
          break;
        case pml::pokepara::SICK_YAKEDO:  // やけど
          req.datId = GARC_wazaeff_resource_es003_fire_lz_BPTCL;
          break;
        case pml::pokepara::SICK_DOKU:    // どく
        case pml::pokepara::SICK_MAX:    // どくどく
          req.datId = GARC_wazaeff_resource_es002_poison_lz_BPTCL;
          break;
        }

        GetAsyncFileManager()->AddArcFileLoadDataReq(req);
        mWaitCamSickData.mSeq++;
      }

    }
    break;
  case SEQ_WAIT_LOAD:
    if( GetAsyncFileManager()->IsArcFileLoadDataFinished( &mWaitCamSickData.mDataBuf ) == false )
    {
      return;
    }
    mWaitCamSickData.mSeq++;
    //break;
  case SEQ_PLAY_EFFECT:
    {
      PokeSick sick = pml::pokepara::SICK_NULL;
      BtlvPos trg = mWaitCamSickData.mOrderArr[mWaitCamSickData.mNowOrder];
      BtlvObject_PokeModel *poke = GetPokeModel(trg);

      if( poke->IsEnable() && poke->GetVisible())
      {
        sick = poke->GetSick();
      }

      int effResIdx = GetParticleIndexManager()->GetIndex();
      GFL_ASSERT_STOP( effResIdx==-1 ? 0:1 );

      mWaitCamSickData.mEffectObj = GFL_NEW(GetWaitSickHeap())BtlvObject_Effect(this);
      mWaitCamSickData.mEffectObj->CreateBuffer(mWaitCamSickData.mDataBuf,GetEffectHeap(),effResIdx);
      mWaitCamSickData.mEffectObj->AddRenderPath(SEQ_DEF_EFF_DRAWTYPE_NORAML);

      GetObjectManager()->AddObject(mWaitCamSickData.mEffectObj);

      BTLV_RELARIVE_POKE_OPTION opt;
      opt.vpos = trg;
      opt.vposSub = trg;
      opt.adjustFlip = false;
      opt.rate = 100;
      opt.adjustScale = false;
      opt.autoRotateModel = true;
      opt.autoRotatePos = true;
      opt.forceUpdate = false;
      switch(sick)
      {
      case pml::pokepara::SICK_MAHI:    // まひ
        opt.offset.Set(0,0,0);
        opt.node = SEQ_DEF_NODE_CENTER;
        break;
      case pml::pokepara::SICK_NEMURI:  // ねむり
        opt.offset.Set(0,15.0f,20.0f);
        mWaitCamSickData.mEffectObj->SetScale(1.5f);
        opt.node = SEQ_DEF_NODE_FACE;
        break;
      case pml::pokepara::SICK_KOORI:   // こおり
        opt.offset.Set(0,0,0);
        mWaitCamSickData.mEffectObj->SetScale(2.0f);
        opt.node = SEQ_DEF_NODE_CENTER;
        break;
      case pml::pokepara::SICK_YAKEDO:  // やけど
        opt.offset.Set(0,0,0);
        mWaitCamSickData.mEffectObj->SetScale(1.2f);
        opt.node = SEQ_DEF_NODE_CENTER;
        break;
      case pml::pokepara::SICK_DOKU:    // どく
      case pml::pokepara::SICK_MAX:    // どくどく
        opt.offset.Set(0,20.0f,0.0f);
        mWaitCamSickData.mEffectObj->SetScale(1.0f);
        opt.node = SEQ_DEF_NODE_CENTER;
        break;
      }
      SeqComFunc_MoveRelativePoke(mWaitCamSickData.mEffectObj,0,opt);
      mWaitCamSickData.mCnt = 0;
      mWaitCamSickData.mSeq++;
    }
    //break;
  case SEQ_WAIT_EFFECT:
    mWaitCamSickData.mCnt++;
    if( mWaitCamSickData.mCnt > 80 )
    {
      mWaitCamSickData.mEffectObj->RemoveRenderPath();
      GetObjectManager()->RemoveObject(mWaitCamSickData.mEffectObj);
      mWaitCamSickData.mCnt = 0;
      mWaitCamSickData.mSeq++;
    }
    break;
  case SEQ_WAIT_NEXT:
    mWaitCamSickData.mCnt++;
    if( mWaitCamSickData.mCnt > 10 )
    {
      GetParticleIndexManager()->ReturnIndex(mWaitCamSickData.mEffectObj->GetResourceIndex());
      mWaitCamSickData.mEffectObj->Delete();
      GFL_SAFE_DELETE(mWaitCamSickData.mEffectObj);
      GflHeapSafeFreeMemory(mWaitCamSickData.mDataBuf);

      mWaitCamSickData.mSeq = SEQ_GO_NEXT;
    }
    break;
  case SEQ_NOSICK_WAIT:
    mWaitCamSickData.mCnt++;
    if( mWaitCamSickData.mCnt > 80 )
    {
      mWaitCamSickData.mSeq = SEQ_GO_NEXT;
    }
    if( mWaitCamSickData.mEnable == false )
    {
      //即終わらせてしまう
      mWaitCamSickData.mSeq = SEQ_GO_NEXT;
    }
    break;

  case SEQ_GO_NEXT:
    //次へ
    mWaitCamSickData.mNowOrder++;
    if( mWaitCamSickData.mNowOrder >= POKE_MODEL_NUM )
    {
      mWaitCamSickData.mNowOrder = 0;
    }

    mWaitCamSickData.mSeq = SEQ_WAIT;
    break;
  }
}

// 大会演出開始を表示するモードかどうかの判定
bool BattleViewSystem::IsConventionDirection(void)
{

  //  大会演出は、インターネット or 赤外線の場合のみ
  if( !IsMultiMode() )
    {
      if( GetCommMode() == BTL_COMM_WIFI ||
          GetCommMode() == BTL_COMM_IR )
        {
          return true;
        }
    }

#if PM_DEBUG
  //@fix GFNMCat[#5752 【観戦機能】観戦ROMで、大会演出表示がされていない]
  if( mMainModule )
  {
    if( mMainModule->GetBattleSetupParam()->isLiveRecPlayMode == true )
    {//観戦ROMだったら演出表示する
      return true;
    }
  }
#endif
  
  return false;

}

// 大会演出用情報
BATTLE_CONVENTION_INFO* BattleViewSystem::GetBattleConventionInfo( u8 id ) const
{
  if( mMainModule )
  {
    return mMainModule->GetBattleConventionInfo( id );
  }
  return NULL;
}

//待機Bの一斉許可と解除
void BattleViewSystem::SetAllEnableWaitB(void)
{
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    BtlvObject_PokeModel *poke = GetPokeModel(static_cast<BtlvPos>(i));
    if( poke->IsEnable() )    
    {
      poke->SetEnableWaitB(true);
    }
  }
  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    GetTrainerModelObj(i)->SetEnableWaitB(true);
    GetTrainerModelObj(i)->SetAnimationIsLoop(false);
    GetTrainerModelObj(i)->SetAutoResetAniation(true);
  }
}

void BattleViewSystem::ResetAllEnableWaitB(void)
{
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    BtlvObject_PokeModel *poke = GetPokeModel(static_cast<BtlvPos>(i));
    if( poke->IsEnable() )    
    {
      poke->SetEnableWaitB(false);
      poke->StartWaitAnime(true,true);
    }
  }
  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    GetTrainerModelObj(i)->SetEnableWaitB(false);
    GetTrainerModelObj(i)->ChangeAnimationRand(TRAINER_ANIME_WAIT1,true);
    GetTrainerModelObj(i)->SetAnimationIsLoop(true);
    GetTrainerModelObj(i)->SetAutoResetAniation(false);
  }

}

//フェスの控えトレーナーの表示制御
void BattleViewSystem::DispFesTrainer(bool flg)
{
  //フェスの控えトレーナーはロード時間削減のため、常駐している。
  //ただ、処理負荷を抑えるため描画パスには追加されていない。

  if (IsBattleFesMode() == false)
  {
    return;
  }

  if (flg == true)
  {
    if (GetTrainerModelObj(2))
    {
      GetTrainerModelObj(2)->AddRenderPath();
      GetTrainerModelObj(2)->ChangeAnimationRand(TRAINER_ANIME_WAIT1, true);
      GetTrainerModelObj(2)->SetAnimationIsLoop(true);
      GetTrainerModelObj(2)->SetVisible(true);
    }
    if (GetTrainerModelObj(3))
    {
      GetTrainerModelObj(3)->AddRenderPath();
      GetTrainerModelObj(3)->ChangeAnimationRand(TRAINER_ANIME_WAIT1, true);
      GetTrainerModelObj(3)->SetAnimationIsLoop(true);
      GetTrainerModelObj(3)->SetVisible(true);
    }
  }
  else
  {
    if (GetTrainerModelObj(2))
    {
      GetTrainerModelObj(2)->SetVisible(false);
      GetTrainerModelObj(2)->RemoveRenderPath();
    }
    if (GetTrainerModelObj(3))
    {
      GetTrainerModelObj(3)->SetVisible(false);
      GetTrainerModelObj(3)->RemoveRenderPath();
    }
  }
}


//デバッグ
void BattleViewSystem::DumpBattleHeap(void)
{
#ifdef DEBUG_ONLY_FOR_ariizumi_nobuhiko
  GFL_PRINT("-----Dump heap-----\n");
  GFL_PRINT("MainHeap     Total[%12x] Free[%12x] Alloc[%12x]\n",mHeap->GetTotalSize(),mHeap->GetTotalFreeSize(),mHeap->GetTotalAllocatableSize());
  GFL_PRINT("ResidentHeap Total[%12x] Free[%12x] Alloc[%12x]\n",mResidentHeap->GetTotalSize(),mResidentHeap->GetTotalFreeSize(),mResidentHeap->GetTotalAllocatableSize());
  GFL_PRINT("TempHeap     Total[%12x] Free[%12x] Alloc[%12x]\n",mTempHeap->GetTotalSize(),mTempHeap->GetTotalFreeSize(),mTempHeap->GetTotalAllocatableSize());
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    GFL_PRINT("PokeHeap     Total[%12x] Free[%12x] Alloc[%12x]\n",mPokeHeap[i]->GetTotalSize(),mPokeHeap[i]->GetTotalFreeSize(),mPokeHeap[i]->GetTotalAllocatableSize());
  }
  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    GFL_PRINT("TrainerHeap  Total[%12x] Free[%12x] Alloc[%12x]\n",mTrainerHeap[i]->GetTotalSize(),mTrainerHeap[i]->GetTotalFreeSize(),mTrainerHeap[i]->GetTotalAllocatableSize());
  }

  GFL_PRINT("-----Dump heap-----\n");
#endif// PM_DEBUG
}

GFL_NAMESPACE_END(btl)

