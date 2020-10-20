//======================================================================
/**
 * @file btlv_loader.cpp
 * @date 2015/12/08 15:34:28
 * @author ariizumi_nobuhiko
 * @brief バトル描画の開始時リソース読み込みの仕組み
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>


#include <arc_index/wazaeff_resource.gaix>
#include <arc_index/btlv_resource.gaix>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle//background/bgsys_normal_ground_def.h>

#include <model/include/gfl2_CharaModelFactory.h>
#include <print/include/SystemFont.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllReadThread.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "AppLib/include/Fade/DemoFade.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

#include "btlv_core.h"
#include "btlv_ui.h"
#include "btlv_loader.h"

GFL_NAMESPACE_BEGIN(btl)

//------------------------------------------------------
//ポケモン用
BtlvLoaderPoke::BtlvLoaderPoke(BattleViewSystem *core,BtlvObject_PokeModel *poke,int idx)
:BtlvLoaderBase(core)
,mPoke(poke)
,mIdx(idx)
{
}

BtlvLoaderPoke::~BtlvLoaderPoke()
{
}

//読み込み開始
void BtlvLoaderPoke::StartLoad(void)
{
  mPoke->StartLoad_Battle();
}

//読み込み完了チェック
bool BtlvLoaderPoke::IsFinishLoad(void)
{
  return mPoke->IsFinishLoad();
}
//読み込み後生成
void BtlvLoaderPoke::Create(void)
{
  mPoke->Create(mBtlvCore->GetPokeHeap(mIdx),mBtlvCore->GetPokeGLAllocator(mIdx),true);
  mPoke->AddRenderPath(0);
  mPoke->SetVisible(false);
  mPoke->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);

  gfl2::math::Vector3 pos;
  int deg;
  mBtlvCore->GetDefaultPokePos(static_cast<BtlvPos>(mIdx),&pos,&deg);
  mPoke->SetPosition( pos );
  mPoke->SetRotateY(deg);

}

//------------------------------------------------------
//トレーナー用
BtlvLoaderTrainer::BtlvLoaderTrainer(BattleViewSystem *core,BtlvObject_Chara *mdl,int idx)
:BtlvLoaderBase(core)
,mMdl(mdl)
,mIdx(idx)
{
}

BtlvLoaderTrainer::~BtlvLoaderTrainer()
{
}

//読み込み開始
void BtlvLoaderTrainer::StartLoad(void)
{
  if( mBtlvCore->IsClientTrainerExist(mIdx) == true )
  {
    mMdl->StartLoad();
  }
  if(mBtlvCore->IsBattleFesMode() && mIdx >= 2)
  {
    mMdl->StartLoad();
  }
}

//読み込み完了チェック
bool BtlvLoaderTrainer::IsFinishLoad(void)
{
  if( mMdl->IsFinishLoad() )
  {
    return true;
  }
  return false;
}
//読み込み後生成
void BtlvLoaderTrainer::Create(void)
{
  if( mBtlvCore->IsClientTrainerExist(mIdx) == true )
  {
    mMdl->CreateModel();
    mMdl->AddRenderPath();
    mMdl->ChangeAnimation(TRAINER_ANIME_WAIT1);
    mMdl->SetAnimationIsLoop(true);
    mBtlvCore->ResetTrainer(mIdx);
  }
  if (mBtlvCore->IsBattleFesMode() && mIdx >= 2)
  {
    //Addはしない
    mMdl->CreateModel();
    mMdl->SetVisible(false);
    mBtlvCore->ResetTrainer(mIdx);
  }
}

//トレーナー用(Setup非同期
BtlvLoaderTrainerAsyncSetup::BtlvLoaderTrainerAsyncSetup(BattleViewSystem *core,BtlvObject_Chara *mdl,int idx)
  :BtlvLoaderTrainer(core,mdl,idx)
{
}

BtlvLoaderTrainerAsyncSetup::~BtlvLoaderTrainerAsyncSetup()
{
}

//読み込み完了チェック
bool BtlvLoaderTrainerAsyncSetup::IsFinishLoad(void)
{
  if( mMdl->IsFinishLoad() == false )
  {
    return false;
  }
  if( mMdl->AsyncSetup() == false )
  {
    return false;
  }
  return true;
}

//------------------------------------------------------
//背景用
BtlvLoaderBg::BtlvLoaderBg(BattleViewSystem *core)
:BtlvLoaderBase(core)
{
}

BtlvLoaderBg::~BtlvLoaderBg()
{
}

//読み込み開始
void BtlvLoaderBg::StartLoad(void)
{
  btl::BgSystem::SETUP_PARAM setupParam;
  mBtlvCore->SetBgSetupParam(&setupParam );
  mBtlvCore->GetBgSystem()->StartLoad(setupParam);

}

//読み込み更新
void BtlvLoaderBg::LoadUpdate(void)
{
  mBtlvCore->GetBgSystem()->LoadUpdate();
}
//読み込み完了チェック
bool BtlvLoaderBg::IsFinishLoad(void)
{
  if( mBtlvCore->GetBgSystem()->IsFinishLoad() == true )
  {
    return true;
  }
  return false;
}
//読み込み後生成
void BtlvLoaderBg::Create(void)
{
  mBtlvCore->GetBgSystem()->Create();
  if( mBtlvCore->GetFieldSituation() && 
      mBtlvCore->GetFieldSituation()->is_cloud_enable )
  {
    if( mBtlvCore->IsEnableSkyBox() )
    {
      mBtlvCore->GetBgSystem()->GetSkyBox()->SetEnableParts(System::Skybox::Skybox::PARTS_TYPE_CLOUD,true);
    }
    if( mBtlvCore->GetFieldSituation()->sky_type == System::Skybox::Skybox::SKY_TYPE_CLOUDY )
    {
      mBtlvCore->GetBgSystem()->RequestAnime(BgSystem::ANM_CLOUD_LOOP);
    }
    else
    {
      mBtlvCore->GetBgSystem()->RequestAnime(BgSystem::ANM_LOOP);
    }
  }
  btl::BgSystem::SETUP_PARAM setupParam;
  mBtlvCore->SetBgSetupParam(&setupParam );

  if( mBtlvCore->GetBattleRule() == BTL_RULE_ROYAL &&
      setupParam.groundType == bg::PLATE_BTLROYAL )
  {
    CornerColor col = GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)mBtlvCore->GetClientID());
    switch( col )
    {
    case CORRER_COLOR_GREEN:
      mBtlvCore->GetBgSystem()->RotateY_Deg(0);
      break;
    case CORRER_COLOR_YELLOW:
      mBtlvCore->GetBgSystem()->RotateY_Deg(270);
      break;
    case CORRER_COLOR_RED:
      mBtlvCore->GetBgSystem()->RotateY_Deg(180);
      break;
    case CORRER_COLOR_BLUE: 
      mBtlvCore->GetBgSystem()->RotateY_Deg(90);
      break;
    }
  }
}

//------------------------------------------------------
//キャラ用影
BtlvLoaderCharaShadow::BtlvLoaderCharaShadow(BattleViewSystem *core)
:BtlvLoaderBase(core)
{
}

BtlvLoaderCharaShadow::~BtlvLoaderCharaShadow()
{
}

//読み込み開始
void BtlvLoaderCharaShadow::StartLoad(void)
{
  mBtlvCore->GetCharaModelFactory()->LoadModelAsync(mBtlvCore->GetResidentHeap(),CHARA_MODEL_ID_BTOB0401_00); //影モデル
  mBtlvCore->GetCharaModelFactory()->LoadModelAsync(mBtlvCore->GetResidentHeap(),CHARA_MODEL_ID_BTOB0301_00); //みがわり人形
}


//読み込み完了チェック
bool BtlvLoaderCharaShadow::IsFinishLoad(void)
{
  if( mBtlvCore->GetCharaModelFactory()->IsModelLoaded(CHARA_MODEL_ID_BTOB0401_00) &&
      mBtlvCore->GetCharaModelFactory()->IsModelLoaded(CHARA_MODEL_ID_BTOB0301_00) )
  {
    return true;
  }
  return false;
}
//読み込み後生成
void BtlvLoaderCharaShadow::Create(void)
{
  mBtlvCore->GetCharaModelFactory()->SetupModel(mBtlvCore->GetGLAllocator(),CHARA_MODEL_ID_BTOB0401_00);
  mBtlvCore->GetCharaModelFactory()->SetupModel(mBtlvCore->GetGLAllocator(),CHARA_MODEL_ID_BTOB0301_00);
}


//------------------------------------------------------
//エフェクト用モデル
BtlvLoaderEffectModel::BtlvLoaderEffectModel(BattleViewSystem *core,BtlvObject_Gfmdl *mdl,int type)
:BtlvLoaderBase(core)
,mMdl(mdl)
,mType(type)
{
}

BtlvLoaderEffectModel::~BtlvLoaderEffectModel()
{
}

//読み込み開始
void BtlvLoaderEffectModel::StartLoad(void)
{
  DemoLib::Obj::Object_GfMdl::LOAD_PARAM param;
  param.arcId = ARCID_WAZAEFF_RESOURCE;
  switch(mType)
  {
  case 0: 
    param.mdlDataId = GARC_wazaeff_resource_BgFade_lz_GFBMDLP; 
    break;
  case 1: 
    param.mdlDataId = GARC_wazaeff_resource_edge_floor_lz_GFBMDLP; 
    break;
  case 2: 
    param.mdlDataId = GARC_wazaeff_resource_mask_floor_lz_GFBMDLP; 
    break;
  }
  
  param.isComp = true;
  param.dataHeap = mBtlvCore->GetResidentHeap();
  param.workHeap = mBtlvCore->GetTempHeap()->GetLowerHandle();

  mMdl->StartLoad(mBtlvCore->GetAsyncFileManager(),param);
}


//読み込み完了チェック
bool BtlvLoaderEffectModel::IsFinishLoad(void)
{
  if( mMdl->IsFinishLoad() )
  {
    return true;
  }
  return false;
}
//読み込み後生成
void BtlvLoaderEffectModel::Create(void)
{
  mMdl->Create(mBtlvCore->GetResidentHeap(),mBtlvCore->GetGLAllocator());

  switch(mType)
  {
  case 0: //背景塗りつぶし
    {
      mMdl->AddRenderPath(BattleRenderingPipeLine::RENDER_OPT_BGFILL);
      mMdl->SetVisible(false);
      mMdl->SetPosition(0,0,50.0f);
      mMdl->SetRotate(270.0f,0,0);
    }
    break;
  case 1: //地面用エッジマスク
    {
      mMdl->AddRenderPath(BattleRenderingPipeLine::RENDER_OPT_EDGE_MASK);
#if defined(GF_PLATFORM_CTR)
      mMdl->GetModelNode()->CopyMaterialDepthStencilStateToStateObject();
#endif
      mMdl->SetVisible(true);
      //影が埋まるのでちょっと下げる
      mMdl->SetPosition(0,-0.2f,0);
    }
    break;
  case 2: //地面用モデルマスク 
    {
      mMdl->AddRenderPath(BattleRenderingPipeLine::RENDER_OPT_FLOOR_MASK);
      mMdl->SetVisible(true);
      //影が埋まるのでちょっと下げる
      mMdl->SetPosition(0,-0.2f,0);
    }
    break;
  }
}

//------------------------------------------------------
//攻撃モーションテーブル
BtlvLoaderBattleData::BtlvLoaderBattleData(BattleViewSystem *core,void **buf,gfl2::fs::BinLinkerAccessor *binLinker)
:BtlvLoaderBase(core)
,mBuf(buf)
,mBinLinker(binLinker)
{
}

BtlvLoaderBattleData::~BtlvLoaderBattleData()
{
}

//読み込み開始
void BtlvLoaderBattleData::StartLoad(void)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_BTLV_RESOURCE;
  req.datId = GARC_btlv_resource_battle_data_pack_BIN;
  req.align = 128;
  req.heapForBuf = mBtlvCore->GetResidentHeap();
  req.heapForReq = mBtlvCore->GetTempHeap()->GetLowerHandle();
  req.ppBuf = mBuf;

#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() )
  {
    if( mBtlvCore->GetDebugParam()->isHioEffResource )
    {
      req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
    }
  }
#endif

  mBtlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(req);
}


//読み込み完了チェック
bool BtlvLoaderBattleData::IsFinishLoad(void)
{
  if( mBtlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished( mBuf ) )
  {
    return true;
  }
  return false;
}
//読み込み後生成
void BtlvLoaderBattleData::Create(void)
{
  mBinLinker->Initialize(*mBuf);
  mBtlvCore->InitWazaMotionTable();
}

//------------------------------------------------------
//汎用エフェクトリソース
BtlvLoaderEffectResource::BtlvLoaderEffectResource(BattleViewSystem *core,void **buf, u32 datId)
:BtlvLoaderBase(core)
,mBuf(buf)
,mDatId(datId)
{
}

BtlvLoaderEffectResource::~BtlvLoaderEffectResource()
{
}

//読み込み開始
void BtlvLoaderEffectResource::StartLoad(void)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_WAZAEFF_RESOURCE;
  req.datId = mDatId;
  req.align = 128;
  req.heapForBuf = mBtlvCore->GetResidentHeap();
  req.heapForReq = mBtlvCore->GetTempHeap()->GetLowerHandle();
  req.heapForCompressed = mBtlvCore->GetTempHeap()->GetLowerHandle();
  req.ppBuf = mBuf;

#if PM_DEBUG
  if( mBtlvCore->GetDebugParam() )
  {
    if( mBtlvCore->GetDebugParam()->isHioEffResource )
    {
      req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
    }
  }
#endif

  mBtlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(req);
}

//読み込み完了チェック
bool BtlvLoaderEffectResource::IsFinishLoad(void)
{
  if( mBtlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished( mBuf ) )
  {
    return true;
  }
  return false;
}
void BtlvLoaderEffectResource::Create(void)
{
}

//------------------------------------------------------
//DLL読み替えよう
BtlvLoaderDll::BtlvLoaderDll(BattleViewSystem *core)
:BtlvLoaderBase(core)
,mIdx(0)
{
}

BtlvLoaderDll::~BtlvLoaderDll()
{
}

//読み込み開始
void BtlvLoaderDll::StartLoad(void)
{

}

//読み込み更新
void BtlvLoaderDll::LoadUpdate(void)
{
#if defined(GF_PLATFORM_CTR)
  switch(mIdx)
  {
  case 0:
    //シーケンス終了待ち
    {
      gfl2::Fade::FadeSuper *fadeSys = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade(gfl2::Fade::DISP_CUSTOM_UPPER);
      if( fadeSys )
      {
        AppLib::Fade::DemoFade *demoFade = reinterpret_cast<AppLib::Fade::DemoFade*>(fadeSys);
        if( demoFade->GetSequence() != NULL )
        {
          //まだデモフェード中
          return;
        }
      }
    }
    mIdx++;
    //break; through
  case 1:
    {
      //DLLの破棄
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEncountWork()->DisposeSequenceDemoDll();
    }
    {
      //DLLの読み込み
      const char *roNameArr[DLL_NUM] = {
        "Bag.cro",
        "WazaOshie.cro",
      };

      for( int i=0;i<DLL_NUM;i++ )
      {
        gfl2::fs::AsyncFileManager::FileEasyReadReq readReq;
        readReq.fileName = roNameArr[i];
        readReq.ppBuf = &mDllBuf[i];
        readReq.pFileSize = &mDllSize[i];
        readReq.heapForBuf = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD);
        readReq.align = 4096;
        readReq.heapForReq = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD)->GetLowerHandle();
        readReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;

        mBtlvCore->GetAsyncFileManager()->AddFileEasyReadReq(readReq);
      }
    }
    mIdx++;
    //break; through
  case 2:
    for( int i=0;i<DLL_NUM;i++ )
    {
      if( mBtlvCore->GetAsyncFileManager()->IsFileEasyReadFinished(&mDllBuf[i]) == false )
      {
        return;
      }
    }

    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    for( int i=0;i<DLL_NUM;i++ )
    {
      gfl2::ro::Module *module = roMgr->LoadModuleSetBuffer(mDllBuf[i],mDllSize[i]);
      mBtlvCore->GetBattleProc()->SetChildModulePtr(0,module);
    }
    mIdx++;
    //break; through
  }
#endif //defined(GF_PLATFORM_CTR)
}
//読み込み完了チェック
bool BtlvLoaderDll::IsFinishLoad(void)
{
#if defined(GF_PLATFORM_CTR)
  if( mIdx == 3 )
  {
    return true;
  }
  return false;
#else
  return true;
#endif
}
//読み込み後生成
void BtlvLoaderDll::Create(void)
{
}

//------------------------------------------------------
//バトルフォント用(一旦未使用
BtlvLoaderBattleFont::BtlvLoaderBattleFont(BattleViewSystem *core)
:BtlvLoaderBase(core)
{
}

BtlvLoaderBattleFont::~BtlvLoaderBattleFont()
{
}

//読み込み開始
void BtlvLoaderBattleFont::StartLoad(void)
{
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_BATTLE_FONT_TEMP );
  print::SystemFont_CreateBattleFont(devHeap,devHeap);
}

//読み込み更新
void BtlvLoaderBattleFont::LoadUpdate(void)
{
}
//読み込み完了チェック
bool BtlvLoaderBattleFont::IsFinishLoad(void)
{
  return print::SystemFont_IsLoadBattleFont();
}
//読み込み後生成
void BtlvLoaderBattleFont::Create(void)
{
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_BATTLE_FONT_TEMP );
  print::SystemFont_SetLoadBattleFont(devHeap);
  print::SystemFont_SetLytSystemBattleFont();
}

//------------------------------------------------------
//初期天候読み込み
BtlvLoaderWeather::BtlvLoaderWeather(BattleViewSystem *core)
  :BtlvLoaderBase(core)
{
}

BtlvLoaderWeather::~BtlvLoaderWeather()
{
}

//読み込み開始
void BtlvLoaderWeather::StartLoad(void)
{
  mBtlvCore->LoadWeatherResource(mBtlvCore->GetNowWeather());
}

//読み込み完了チェック
bool BtlvLoaderWeather::IsFinishLoad(void)
{
  return mBtlvCore->IsFinishLoadWeatherResource();
}
//読み込み後生成
void BtlvLoaderWeather::Create(void)
{
  mBtlvCore->StartWeather(mBtlvCore->GetNowWeather());
}

//------------------------------------------------------
//背景用
BtlvLoaderUiMessage::BtlvLoaderUiMessage(BattleViewSystem *core)
  :BtlvLoaderBase(core)
{
}

BtlvLoaderUiMessage::~BtlvLoaderUiMessage()
{
}

//読み込み開始
void BtlvLoaderUiMessage::StartLoad(void)
{

}

//読み込み更新
void BtlvLoaderUiMessage::LoadUpdate(void)
{
}
//読み込み完了チェック
bool BtlvLoaderUiMessage::IsFinishLoad(void)
{
  return mBtlvCore->GetUiSys()->LoadMessage();
}
//読み込み後生成
void BtlvLoaderUiMessage::Create(void)
{
}
#if 0
//------------------------------------------------------
//背景用
BtlvLoaderBg::BtlvLoaderBg(BattleViewSystem *core)
:BtlvLoaderBase(core)
{
}

BtlvLoaderBg::~BtlvLoaderBg()
{
}

//読み込み開始
void BtlvLoaderBg::StartLoad(void)
{

}

//読み込み更新
void BtlvLoaderBg::LoadUpdate(void)
{
}
//読み込み完了チェック
bool BtlvLoaderBg::IsFinishLoad(void)
{
  return false;
}
//読み込み後生成
void BtlvLoaderBg::Create(void)
{
}
#endif
GFL_NAMESPACE_END(btl)
