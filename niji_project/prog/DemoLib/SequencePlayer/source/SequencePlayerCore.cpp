//======================================================================
/**
 * @file	SequencePlayerCore.C++
 * @brief	SequencePlayer中心機能
 * @author	PETE
 * @data	2015.04.27
 */
//======================================================================
#include <debug/DebugWin/include/DebugWinSystem.h>

#include "GameSys/include/GameManager.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/SequencePlayer/include/Player.h"

#include "DemoLib/Object/include/ObjectUtil.h"

#include <system/include/Timer/gfl2_PerformanceCounter.h>

// SceneGraph
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <math/include/gfl2_math.h>
#include <math/include/gfl2_Easing.h>

// CharaModel:
#include <model/include/gfl2_CharaModelFactory.h>

#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/p1_base.h>
#include <niji_conv_header/battle/chara/chara_model_dynamic_motion.h>  //非常駐モーション種類

// garc FileLoading
#include <arc_index/chara_model_field.gaix>
#include <arc_index/script_message.gaix>
#include <arc_index/message.gaix>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/fieldwindow.gaix>
#include <arc_def_index/arc_def.h>

#include "Sound/include/Sound.h"

#include <System/include/Skybox/Skybox.h>
#include <System/include/DressUp.h>

// Common Text Window:
#include  <niji_conv_header/app/common_menu/MenuWindow.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_anim_list.h>
#include  <niji_conv_header/app/common_menu/MenuWindow_pane.h>
#include <niji_conv_header/message/script/msg_test_event_0010.h>


#include  "AppLib/include/Tool/app_tool_MenuWindow.h"
#include  "AppLib/include/Util/app_util_heap.h"

// Game Data:
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <gfx/include/gfl2_DrawUtil.h>

// Network Status:
#include "NetStatic/NetLib/source/Error/NijiNetworkErrorDialogStatus.h"

enum
{
	SEQPLAYER_STATE_INIT= 0,
	SEQPLAYER_STATE_INITWAIT,
	SEQPLAYER_STATE_MAIN,
  SEQPLAYER_STATE_CLEANUP,
	SEQPLAYER_STATE_FINISH,
};


GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)
 
SequenceViewSystem :: SequenceViewSystem(bool isTestMode)
{
#if PM_DEBUG
    mUseHIO = false;
    mShowDebugInfo = false;
    mSkipMilliSec = 0;
#endif
  if ( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetGameTime()->IsAlolaOffsetExists() )
  { //GFNMCat[5618] AlolaTime使用可能になる前にイントロでデモを呼び出しているので、AlolaTimeの初期化を回避する
    mAlolatime.Initialize();
  }

  mLoadedSequenceFile = false;
  mIsTestMode = isTestMode;
  mSystemSyncSleepTrigger = false;

  mSleepRecover = false;
	
	mAsyncFileManager =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
	SetState(SEQPLAYER_STATE_INIT);

  // Pokemon:
  for(int n=0;n<DEMOSEQUENCE_MAX_POKEMON;n++)
  {
    mPokemonObj[n] = NULL;
    mPokemonIndex[n] = -1;
    mPokemonIndexUsed[n] = false;
    mPokemonParam[n] = NULL;
  }
  mUsePokemonSystem = true;
  mPokeModelSys = NULL;
  bgmWaitIdx = -1;

  // Monster Ball:
  for(int n=0;n<SEQ_MONSTERBALL_NUM;n++)
  {
    monsterBallType[n] = item::BALLID_MONSUTAABOORU; // Default Monster Ball
  }
  
  // FieldCharaModel:
  for(int n=0;n<DEMOSEQUENCE_MAX_FIELDMODEL;n++)
  {
    mFieldModelIndex[n] = -1;
    mFieldModelAttachItemIndex[n] = -1;
    mFieldDressUpParamIndex[n] = -1;
  }
  // BattleCharaModel
  for(int n=0;n<DEMOSEQUENCE_MAX_BATTLEMODEL;n++)
  {
    mBattleModelIndex[n] = -1;
    mBattleModelAttachItemIndex[n] = -1;
    mBattleDressUpParamIndex[n] = -1;
  }

  // CharaModel
  mUseCharaModel = true;
  mCharaFactoryField = NULL;
  mCharaFactoryBattle = NULL;

  for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
  {  
    mDressUpModelResourceManagerField[z] = NULL;
    mDressUpModelResourceManagerBattle[z] = NULL;
    mVSBattlePose[z] = P1_BASE_BA005_WAIT01;
    mVSBattlePoseFile[z] = -1;  //無効値
    mVSBattleStatus[z] = NULL;

    for(int n=0;n<VSBATTLE_MONSBALLSTATUS_NUM;n++)
    {
      mVSBattleMonsterBallStatus[z][n] = 0;
    }
  }

  // Effect
  mUseEffectSystem = true;
  mEffectHeap = NULL;
  mEffectSystem = NULL;
  mEffectGroupIDList[0] = mEffectGroupIDList[1] = mEffectGroupIDList[2] = NULL;
  mParticleIndexManager = NULL;

  // Skybox:
  mUseSkybox = false;
  mSkybox = NULL;
  mSkyBoxOffFrame = -1;
 
  playerSexSetting = 0; // NO setting
  
  if(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetSex()==poke_3d::model::DressUpParam::SEX_MALE)
    SetPlayerSexMale(); // Male Setting
  else SetPlayerSexFemale(); // Female Setting

  for(int n=0;n<MAX_DEBUGSEQ_POKEMON_SETTINGS;n++)
  {
    pokemonDebugSettings[n].enable = false;
    pokemonDebugSettings[n].monsNo = 25;
    pokemonDebugSettings[n].formNo = 0;
    pokemonDebugSettings[n].sex = 0;
  }

  // BGM Sync:
  numBGMSyncSettings = 0;
  mSyncBGM = false;

  //@fix GFNMCat[5756] BGMの再生ポジション巻き戻り対策のために最後のサンプル数を保存
  mStreamSoundSamplePositionLast = -1;
   
  // Playback:
  mPausePlayback = false;
  mStopPlayback = false;  
  mFastForwardPlayback = false;
  mForceOneStepPause = false;

  mSystemCancel = false;
  mCanSetSystemCancel = false;

  // Reset Network Error Dialog Status:
  NetLib::Error::NijiNetworkErrorDialogStatus::ResetNetworkErrorDialogEvent();

  // DemoType: (All Off)  
  ResetAllDemoType();

  // VS Battle Type:
  mVSDemoType = VSBATTLE_PRESTART;
  
  // System Sync:
  mStartupWaitMode = false;
  mSystemSync = false;
  mSyncFrame = 0;
  mNumSystemSync = 0;
  mCurSyncIndex = 0;
  for(int n=0;n<SEQ_MAX_SYSTEMSYNC;n++) mSyncFrames[n]=-1;  

  mSyncStartTick = 0;

  mForceOneStep = false;
  mRealTimeMode = true;
  mSystemSyncTrigger = false;

  // KeyWait:
  mKeyWaitType = -1;
  mExitOnFrameWait = false;
  mFrameWait = -1;

  // Common Text Window:
  mLayoutCommonWindowBuffer = NULL;
  for( int i=0;i<MSG_WIN_NUM;i++ )
  {
    mMessageWindow[i] = NULL;
    mUseTextWindowFlg[i] = false;
  }
  messageWindowEndFrame = -1;
  mUseCommonTextWindow = true;
  mWordset = NULL;
  mTextWindowType = -1; // No Setting, Standard Talk Window
  mTextWindowArcID = GARC_message_introdemo_DAT; // Intro Message by default!
  mCommonMsgData = NULL;
  mTrainerMsgData = NULL;
  // Camera:
  m_BaseCamera = NULL;
  m_pAnimationResourceRootNode = NULL;
  m_pDrawEnvNode = NULL;
  m_pDrawEnvResourceRootNode = NULL;
  mCameraStartAniFrame = -1;

  // InFrame Camera:
  for(int z=0;z<SEQ_NUM_INFRAME_CAMERA;z++)
  {
    mInFrameCamera[z] = NULL;
    mInFrameSys[z] = NULL;
    inFrameFollowMdl[z] = NULL;
    inFramePosX[z] = 100;
    inFramePosY[z] = 120;
    inFrameSizeX[z] = 200;
    inFrameSizeY[z] = 240;
    mSetInframe[z] = false;
    mUseInframeFollowRotate[z] = false;
    mBaseInframeRotX[z] = 0.0f;
    mBaseInframeRotY[z] = 0.0f;
    inFrameFollowNodeName[z][0] = 0;
  }
  mUseInframe = false;

  // Motion Blur:
  mBlurEndFrame = -1;

  // Replace Texture:
  mReplaceTexture = NULL;

  // System Callback:
  mSysCallBack = NULL;
  
  // Command Skip:
  mCommandSkipFrame = -1;
  mCommandSkipOnCancel = false;  
  mCommandSkipOnSync = false;  
  mCommandSkipWait = 0;
  mCommandTransType = 0;
  mCommandSkipCountdown = false;
  lastCommandSkipFrame = -1;

  mUseOption = SequenceRenderingPipeLine::UseOption();

  // Sound:
  for(int n=0;n<SEQ_EFF_SOUND_NUM;n++)
    mSoundObject[n] = NULL;

  // All Managers NULL:
  mObjectManager = NULL;
  mTaskSystem = NULL;
  mResourceManager = NULL;
  mModelObjManager = NULL;
  mCharaObjManager = NULL;
  mPokeObjManager = NULL;
  mLayoutObjManager = NULL;
  mLightObjManager = NULL;
  mEffectObjManager = NULL;

  // Default Timezone : REALTIME
  timezoneSetting = -1;

#if PM_DEBUG  //影響が怖いので、一応括る
  mIsSkipFinish = false;
#endif
}

SequenceViewSystem :: ~SequenceViewSystem(void)
{
  
}

bool SequenceViewSystem :: IsSun(void)
{
  return(GET_VERSION() == VERSION_SUN2);
}

bool SequenceViewSystem :: IsMoon(void)
{
  return(GET_VERSION() == VERSION_MOON2);
}

void SequenceViewSystem :: SetTrainerName(u32 bufID, const Savedata::MyStatus* status )
{
  if(!mUseCommonTextWindow ) return;
  mWordset->RegisterPlayerName( bufID, status );      
}

void SequenceViewSystem::SetPokeNickName(u32 bufID, const pml::pokepara::CoreParam* pCoreParam )
{
  if(!mUseCommonTextWindow ) return;
  mWordset->RegisterPokeNickName( bufID, pCoreParam );      
}


void SequenceViewSystem :: ResetAllDemoType(void)
{
  mDemoType = -1;
}

void SequenceViewSystem :: SetPlaybackParameter(int type,int param0,int param1)
{
  switch(type)
  {
    case SEQPARAMTYPE_VSBATTLE:
      mVSDemoType = param0;
      break;
  }
}

int SequenceViewSystem :: GetPlaybackParameter(int type)
{
  switch(type)
  {
    case SEQPARAMTYPE_VSBATTLE: return(mVSDemoType);
  }

  return(-1);
}


void SequenceViewSystem :: RemoveSystemCallBack(void)
{
  mSysCallBack = NULL;
}

void SequenceViewSystem :: ExecuteCallBack(int command,int paramA,int paramB)
{
  if(mSysCallBack==NULL)
  {
    GFL_PRINT("*SystemCallBack_Internal(%i,%i,%i)*\n",command,paramA,paramB);
    return; // Callback not set..
  }

  mSysCallBack->SequenceCallBackFunc(command,paramA,paramB);  
}

void SequenceViewSystem :: SetPokemonParam(u32 index, pml::pokepara::PokemonParam *p)
{
  if(index>=DEMOSEQUENCE_MAX_POKEMON) return; // Invalid index!
  mPokemonParam[index] = p;

  // Set GET Ball Type:
  if(p!=NULL)
  {
    monsterBallType[index] = mPokemonParam[index]->GetGetBall();
  }
}

void SequenceViewSystem :: LoadSkyBox(f32 rot)
{
  
  mUseSkybox = true;
  mSkybox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetSkybox();  

  if(mSkybox==NULL) return;

  System::Skybox::Skybox::Description desc;
  desc.rotateRadY = rot;
  
  mSkybox->CreateHeap();
  mSkybox->StartCreateResource(desc, mMemHeap);
  
}

void SequenceViewSystem :: SetSystemSync(bool b) 
{  
  if(b)
  {
    if(mStopPlayback) return;

    if(mSystemSyncTrigger)
    {
      mSystemSyncTrigger=false;
      return;
    }
    mSyncStartTick = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();    
  }

  mSystemSync = b; 
  
}

void SequenceViewSystem :: SystemSync(void)
{   

  if(!mPausePlayback && mCommandSkipOnSync)
  {
    mSystemSyncTrigger=true;
  }

  if(!IsSystemSync() || mPausePlayback) 
  {
    // Sync Sent during system recover:
    if(IsSystemSync() && mSleepRecover)
    {
      mSystemSyncSleepTrigger = true;
    }

     return; // System Sync is OFF!
  }

  u64 tickDif = gfl2::system::Timer::PerformanceCounter::GetMicroSecond() - mSyncStartTick;
  tickStart+=tickDif;
  SetSystemSync(false);

  // Disable Any KeyWait Options:
  if(mKeyWaitType!=-1)
  {
    mKeyWaitType = -1;  
    mExitOnFrameWait = false;
    mFrameWait = -1;
  }
}

void SequenceViewSystem :: AddSkyBoxRendering(int weatherType,f32 scale,gfl2::math::Vector3 pos,int hour,int min,int sec)
{
  if(mSkybox==NULL) return;

  for(int ps = 0; ps < System::Skybox::Skybox::PARTS_TYPE_MAX; ps++ ){
    poke_3d::model::BaseModel* pBax = mSkybox->GetPartsModel((System::Skybox::Skybox::PartsType)ps);
    if(pBax){
      {
        gfl2::math::Vector3 size = pBax->GetScale();
        size.x *= scale;
        size.y *= scale;
        size.z *= scale;
        pBax->SetScale(size);
        pBax->SetPosition(pos.x, pos.y, pos.z);
      }
      mRenderSys->GetRenderingPipeLine()->AddObject_Skybox(pBax->GetModelInstanceNode());
    }
  }
  mSkybox->Initialize((System::Skybox::Skybox::SkyType)weatherType);
  mSkybox->Update(pos,hour,min,sec);  
}

void SequenceViewSystem :: InitDressupParam(void)
{

  if( mUseCharaModel == false)
  {
    return;
  }

  for(int n=0;n<DEMOSEQUENCE_MAX_MODELDRESSUP;n+=2)
  {      
    // Female Set:
    {
      System::DressUp::GetDressUpParamDefaultHeroine(&mDressUpParam[n]);

      poke_3d::model::DressUpParam param = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetDressup();

      if(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetSex()==poke_3d::model::DressUpParam::SEX_FEMALE)
      {              
        mDressUpParam[n] = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetDressup();        
      }
    }

    // Male Set:
    {
      System::DressUp::GetDressUpParamDefaultHero(&mDressUpParam[n+1]);

      poke_3d::model::DressUpParam param = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetDressup();

      if(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetSex()==poke_3d::model::DressUpParam::SEX_MALE)
      {          
        mDressUpParam[n+1] = param;        
      }
    }
  }
}

void SequenceViewSystem :: SetDressUpParam(const poke_3d::model::DressUpParam *param,bool isMale)
{
  int index = 0;
  if(isMale) index++;
  memcpy(&mDressUpParam[index],param,sizeof(poke_3d::model::DressUpParam));  
}

void SequenceViewSystem :: SetVSBattleDressUpParam(Savedata::MyStatus *status,int index)
{
  if(index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) return;

  mDressUpParam[index] = status->GetDressup();

  // @fix MMCat[337]nijiの場合は、バングルをnijiのに上書きする
  if( status->IsDressUpParamRomVersionNiji() )
  {
    ::System::DressUpParamSaveData::OverrideNijiBngl( &mDressUpParam[index] );
  }

  // Remember the Battle Pose:
  //niji_conv_header/battle/chara/chara_model_dynamic_motion.h 参照
  if( status->GetSex() == PM_MALE )
  {
    if (status->GetBallThrowType() == 8)
    {
      //momiji追加のためIndexが飛んでる
      mVSBattlePoseFile[index] = BTPC0001_00_SELECT9;
    }
    else
    {
      mVSBattlePoseFile[index] = BTPC0001_00_SELECT1 + status->GetBallThrowType() * 0x10000;
    }
    ARIIZUMI_PRINT("[DinamicSet %d:%x]\n",index,mVSBattlePoseFile[index]);
  }
  else
  {
    if (status->GetBallThrowType() == 8)
    {
      //momiji追加のためIndexが飛んでる
      mVSBattlePoseFile[index] = BTPC0002_00_SELECT9;
    }
    else
    {
      mVSBattlePoseFile[index] = BTPC0002_00_SELECT1 + status->GetBallThrowType() * 0x10000;
    }
    ARIIZUMI_PRINT("[DinamicSet %d:%x]\n",index,mVSBattlePoseFile[index]);
  }
  //exec\conv_header\niji_conv_header\battle\chara\btpc000X_00_selectX.h 参照
  if (status->GetBallThrowType() == 8)
  {
    //momiji追加のためIndexが飛んでる
    mVSBattlePose[index] = (0xB) * 0x10000 + 1;
  }
  else
  {
    mVSBattlePose[index] = (status->GetBallThrowType() + 1) * 0x10000 + 1;
  }

  // Save the SaveData Status for later:
  mVSBattleStatus[index] = status;

}

bool SequenceViewSystem :: GetVSBattleName(gfl2::str::StrBuf *buf,int index)
{
  if(mVSBattleStatus[index]!=NULL)
  {
    mVSBattleStatus[index]->GetNameString(buf);
    return(true);
  }

  return(false);
}

poke_3d::model::DressUpParam *SequenceViewSystem :: GetDressUpParam(int index,bool isMale)
{
  if(index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) return(NULL);

  if(isMale) return(&mDressUpParam[index+1]);
  return(&mDressUpParam[index]);
}

poke_3d::model::DressUpParam *SequenceViewSystem :: GetDressUpParamVSBattle(int index)
{
  if(index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) return(NULL);
  return(&mDressUpParam[index]);
}

int SequenceViewSystem :: GetVSBattleModelID(int index)
{
  if(mDressUpParam[index].bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == poke_3d::model::DressUpParam::SEX_FEMALE)
    return(1); // FEMALE

  return(0); // MALE
}

int SequenceViewSystem :: GetVSBattleStartAnimation(int index)
{
  if(index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) return(P1_BASE_BA005_WAIT01);

  int result = mVSBattlePose[index];
  return(result);
}

SequenceObject_Poke *SequenceViewSystem :: GetIndexPokemonAndRemove(int n)
{
  for(int p=0;p<DEMOSEQUENCE_MAX_POKEMON;p++)
  {    
     if(!mPokemonIndexUsed[p]) 
     {
        mPokemonIndexUsed[p] = true;
        return(mPokemonObj[p]);
    }
  }
  return(NULL);
}

void SequenceViewSystem :: LoadPokemon(int n,int type)
{
  if(GetPokeModelSys()==NULL) return;

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_POKEMON;p++)
  {
    if(mPokemonIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }

  if(loadPos==-1) return; // Can't load anymore pokemon!

  mPokemonObj[loadPos] = GFL_NEW(mMemHeap)SequenceObject_Poke(this,GetRenderSys()->GetRenderingPipeLine(),loadPos+LIGHTNO_POKEMON_TOP);
  mPokemonIndex[loadPos] = n;
  
  PokeTool::PokeModel::SetupOption opt;
  opt.dataHeap = GetDevHeap();
  opt.workHeap = GetDevHeap()->GetLowerHandle();
  opt.animeType = (PokeTool::MODEL_ANIMETYPE)type;
  opt.useShadow = true;
  PokeTool::SimpleParam pokeParam;
 
  if(mPokemonParam[n]!=NULL)
  {
    pokeParam.monsNo = mPokemonParam[n]->GetMonsNo();
    pokeParam.formNo = mPokemonParam[n]->GetFormNo();
    pokeParam.sex = mPokemonParam[n]->GetSex();
    pokeParam.isEgg = mPokemonParam[n]->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
    pokeParam.isRare = mPokemonParam[n]->IsRare();
    pokeParam.perRand = mPokemonParam[n]->GetPersonalRnd();
  }
  else
  {      
  //  pokeParam.monsNo = (MonsNo)25;  
    pokeParam.monsNo = (MonsNo)pokemonDebugSettings[n].monsNo;
    pokeParam.formNo = (FormNo)pokemonDebugSettings[n].formNo;
    pokeParam.sex = (pml::Sex)pokemonDebugSettings[n].sex;    
  }

  mPokemonObj[loadPos]->SetSimpleParam(static_cast<int>(pokeParam.monsNo),static_cast<int>(pokeParam.formNo));

  DemoLib::Obj::Object_Poke::LOAD_PARAM param;
  param.modelSys = GetPokeModelSys();
  param.param = &pokeParam;
  param.opt = &opt;
  param.dataHeap = GetDevHeap();
  param.workHeap = GetDevHeap()->GetLowerHandle();
  mPokemonObj[loadPos]->StartLoad(GetAsyncFileManager(),param);
   
}

void SequenceViewSystem :: LoadPokemonFromParameters(int monsNo,int formNo,int sexType,int type)
{
  if(GetPokeModelSys()==NULL) return;

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_POKEMON;p++)
  {
    if(mPokemonIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }

  if(loadPos==-1) return; // Can't load anymore pokemon!

  mPokemonObj[loadPos] = GFL_NEW(mMemHeap)SequenceObject_Poke(this,GetRenderSys()->GetRenderingPipeLine(),loadPos+LIGHTNO_POKEMON_TOP);
  mPokemonIndex[loadPos] = DEMOSEQUENCE_MAX_POKEMON; // Mark it with MAX INDEX

  PokeTool::PokeModel::SetupOption opt;
  opt.dataHeap = GetDevHeap();
  opt.workHeap = GetDevHeap()->GetLowerHandle();
  opt.animeType = (PokeTool::MODEL_ANIMETYPE)type;
  opt.useShadow = true;

  PokeTool::SimpleParam pokeParam;
  // PokeTool::GetSimpleParam(&pokeParam,pp);
  pokeParam.monsNo = (MonsNo)monsNo;
  pokeParam.formNo = (pml::FormNo)formNo;
  pokeParam.sex = (pml::Sex)sexType;

  mPokemonObj[loadPos]->SetSimpleParam(monsNo,formNo);

  DemoLib::Obj::Object_Poke::LOAD_PARAM param;
  param.modelSys = GetPokeModelSys();
  param.param = &pokeParam;
  param.opt = &opt;
  param.dataHeap = GetDevHeap();
  param.workHeap = GetDevHeap()->GetLowerHandle();
  mPokemonObj[loadPos]->StartLoad(GetAsyncFileManager(),param);
}

bool SequenceViewSystem :: AllPokemonLoadingFinished(void)
{
  int count = 0;
  for(int n=0;n<DEMOSEQUENCE_MAX_POKEMON;n++)
  {
    if(mPokemonIndex[n]==-1 || mPokemonObj[n]==NULL) continue;
    if(!mPokemonObj[n]->IsFinishLoad()) return(false);
    count++;
  }

  if(count) GFL_PRINT("*Finished Loading %i Pokemon*\n",count);
  return(true);
}

void SequenceViewSystem :: LoadCharaFieldModel(int n)
{
  if( mUseCharaModel == false || mCharaFactoryField==NULL)
  {
    return;
  }

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_FIELDMODEL;p++)
  {
    if(mFieldModelIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }

  if(loadPos==-1) return; // Can't load anymore CharaFieldModels!

  mFieldModelIndex[loadPos] = n;
  mCharaFactoryField->LoadModelAsync(n);
}

void SequenceViewSystem :: AddDressUpParamLoadField(int n)
{
  if( mUseCharaModel == false )
  {
    return;
  }

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_FIELDMODEL;p++)
  {
    if(mFieldDressUpParamIndex[p] == n) return; // Already in list!

    if(mFieldDressUpParamIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }
  if(loadPos==-1) return; // Can't load anymore!
  mFieldDressUpParamIndex[loadPos]  = n;

}

void SequenceViewSystem :: AddDressUpParamLoadBattle(int n)
{
  if( mUseCharaModel == false)
  {
    return;
  }

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_BATTLEMODEL;p++)
  {
    if(mBattleDressUpParamIndex[p] == n) return; // Already in list!

    if(mBattleDressUpParamIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }
  if(loadPos==-1) return; // Can't load anymore!
  mBattleDressUpParamIndex[loadPos]  = n;
}

void SequenceViewSystem :: LoadCharaBattleModel(int n)
{
  if( mUseCharaModel == false || mCharaFactoryBattle==NULL)
  {
    return;
  }

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_BATTLEMODEL;p++)
  {
    if(mBattleModelIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }

  if(loadPos==-1) return; // Can't load anymore CharaBattleModels!

  mBattleModelIndex[loadPos] = n;
  mCharaFactoryBattle->LoadModelAsync(n);
}

void SequenceViewSystem :: LoadCharaBattleAttachItemModel(int n)
{
  // Remember for later to check loading:
  for(int p=0;p<DEMOSEQUENCE_MAX_BATTLEMODEL;p++)
  {
    if(mBattleModelAttachItemIndex[p] == -1)
    {
      mBattleModelAttachItemIndex[p] = n;
      break;
    }
  }
  mCharaFactoryBattle->LoadModelAsync(n);
}

void SequenceViewSystem :: LoadCharaFieldAttachItemModel(int n)
{
  // Remember for later to check loading:
  for(int p=0;p<DEMOSEQUENCE_MAX_BATTLEMODEL;p++)
  {
    if(mFieldModelAttachItemIndex[p] == -1)
    {
      mFieldModelAttachItemIndex[p] = n;
      break;
    }
  }
  mCharaFactoryField->LoadModelAsync(n);
}

void SequenceViewSystem :: LoadCharaVSBattleModel(int n)
{
  if( mUseCharaModel == false || mCharaFactoryBattle==NULL)
  {
    return;
  }

  int loadPos = -1;

  for(int p=0;p<DEMOSEQUENCE_MAX_BATTLEMODEL;p++)
  {
    if(mBattleModelIndex[p] == -1)
    {
      loadPos = p; break;
    }
  }

  if(loadPos==-1) return; // Can't load anymore CharaBattleModels!

  mBattleModelIndex[loadPos] = n;
  mCharaFactoryBattle->LoadModelAsync(n);
}

void SequenceViewSystem :: LoadAllDressUpParts(void)
{
  if( mUseCharaModel == false)
  {
    return;
  }

  for(int n=0;n<DEMOSEQUENCE_MAX_FIELDMODEL;n++)
  {
    int index = mFieldDressUpParamIndex[n];
    if(index==-1 || index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) continue;    

    if(mDressUpModelResourceManagerField[index])
      mDressUpModelResourceManagerField[index]->LoadDressUpPartsAsync(mDressUpParam[index]);
  }

  for(int n=0;n<DEMOSEQUENCE_MAX_BATTLEMODEL;n++)
  {
    int index = mBattleDressUpParamIndex[n];
    if(index==-1 || index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) continue;    

    if(mDressUpModelResourceManagerBattle[index])
    {
      mDressUpModelResourceManagerBattle[index]->LoadDressUpPartsAsync(mDressUpParam[index]);    
      if( mVSBattlePoseFile[index] != -1 )
      {
        //非常駐のモーションの読み込み
        mCharaFactoryBattle->LoadDynamicAnimationAsync(mVSBattlePoseFile[index]);
        mDressUpModelResourceManagerBattle[index]->LoadDressUpDynamicAnimationAsync(mDressUpParam[index],mVSBattlePoseFile[index]);
        ARIIZUMI_PRINT("[DinamicLoad %d:%x]\n",index,mVSBattlePoseFile[index]);
      }
    }
  }
}

void SequenceViewSystem :: SetupAllCharaModels(void)
{
  if( mUseCharaModel == false || mCharaFactoryField==NULL || mCharaFactoryBattle==NULL)
  {
    return;
  }

  for(int n=0;n<DEMOSEQUENCE_MAX_FIELDMODEL;n++)
  {
    if(mFieldModelIndex[n]!=-1) 
      mCharaFactoryField->SetupModel(GetGLAllocator(),mFieldModelIndex[n]);

    if(mFieldModelAttachItemIndex[n]!=-1) 
      mCharaFactoryField->SetupModel(GetGLAllocator(),mFieldModelAttachItemIndex[n]);
  }
  for(int n=0;n<DEMOSEQUENCE_MAX_BATTLEMODEL;n++)
  {
    if(mBattleModelIndex[n]!=-1) 
      mCharaFactoryBattle->SetupModel(GetGLAllocator(),mBattleModelIndex[n]);

    if(mBattleModelAttachItemIndex[n]!=-1) 
      mCharaFactoryBattle->SetupModel(GetGLAllocator(),mBattleModelAttachItemIndex[n]);
  }
  
  for(int n=0;n<DEMOSEQUENCE_MAX_BATTLEMODEL;n++)
  {
    int index = mBattleDressUpParamIndex[n];
    if(index==-1 || index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) continue;        

    if(mDressUpModelResourceManagerBattle[index])
    {
      mDressUpModelResourceManagerBattle[index]->SetupDressUpParts(GetGLAllocator(),mDressUpParam[index]);    
      if( mVSBattlePoseFile[index] != -1 )
      {
        //非常駐のモーションの設定
        mCharaFactoryBattle->SetupDynamicAnimation(GetGLAllocator(),GetDevHeap(),mVSBattlePoseFile[index]);
        mDressUpModelResourceManagerBattle[index]->SetupDressUpDynamicAnimation(GetGLAllocator(),GetDevHeap(),mDressUpParam[index],mVSBattlePoseFile[index]);
        ARIIZUMI_PRINT("[DinamicSetup %d:%x]\n",index,mVSBattlePoseFile[index]);
      }
    }
  }
  for(int n=0;n<DEMOSEQUENCE_MAX_FIELDMODEL;n++)
  {
    int index = mFieldDressUpParamIndex[n];
    if(index==-1 || index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) continue; 

    if(mDressUpModelResourceManagerField[index])
      mDressUpModelResourceManagerField[index]->SetupDressUpParts(GetGLAllocator(),mDressUpParam[index]);          
  }
}

bool SequenceViewSystem :: AllCharaModelLoadingFinished(void)
{
  if( mUseCharaModel == false || mCharaFactoryField==NULL || mCharaFactoryBattle==NULL)
  {
    return(true);
  }

  int count = 0;
  // Field Models:
  for(int n=0;n<DEMOSEQUENCE_MAX_FIELDMODEL;n++)
  {
    if(mFieldModelIndex[n]!=-1 && !mCharaFactoryField->IsModelLoaded(mFieldModelIndex[n]) ) return(false);
    else if(mFieldModelAttachItemIndex[n]!=-1 && !mCharaFactoryField->IsModelLoaded(mFieldModelAttachItemIndex[n]) ) return(false);
    count++;
  }

  // Battle Models:
  for(int n=0;n<DEMOSEQUENCE_MAX_BATTLEMODEL;n++)
  {
    if(mBattleModelIndex[n]!=-1 && !mCharaFactoryBattle->IsModelLoaded(mBattleModelIndex[n]) ) return(false);
    else if(mBattleModelAttachItemIndex[n]!=-1 && !mCharaFactoryBattle->IsModelLoaded(mBattleModelAttachItemIndex[n]) ) return(false);
    count++;
  }

  // Dress Up Parts:
  for(int n=0;n<DEMOSEQUENCE_MAX_MODELDRESSUP;n++)
  {
    int index = mBattleDressUpParamIndex[n];
    if( !(index==-1 || index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) )
    {
      if(!mDressUpModelResourceManagerBattle[index]->IsDressUpPartsLoaded(mDressUpParam[index]) ) return(false);
        //非常駐のモーションの待ち
      if( mVSBattlePoseFile[index] != -1 )
      {
        if(!mCharaFactoryBattle->IsDynamicAnimationLoaded(mVSBattlePoseFile[index]) ) return(false);
        if(!mDressUpModelResourceManagerBattle[index]->IsDressUpDynamicAnimationLoaded(mDressUpParam[index],mVSBattlePoseFile[index]) ) return(false);
        ARIIZUMI_PRINT("[DinamicWait %d:%x]\n",index,mVSBattlePoseFile[index]);
      }
    }
    int index2 = mFieldDressUpParamIndex[n];
    if( !(index2==-1 || index2<0 || index2>=DEMOSEQUENCE_MAX_MODELDRESSUP) )
    {
      if(!mDressUpModelResourceManagerField[index2]->IsDressUpPartsLoaded(mDressUpParam[index2]) ) return(false);
    }
    count++;
  }

  if(count) GFL_PRINT("*Finished Loading %i CharaModel*\n",count);
  return(true);
}

void SequenceViewSystem :: Initialize(gfl2::heap::HeapBase* memHeap , gfl2::heap::HeapBase* devHeap,int memHeapSize,int devHeapSize )	
{
	GFL_PRINT("SeqViewSystem->Init Heap\n");

	tickStart = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();

	mMemHeap = GFL_CREATE_LOCAL_HEAP_NAME( memHeap ,  memHeapSize   , gfl2::heap::HEAP_TYPE_EXP , false , "SeqViewMem" );
	mDevHeap = GFL_CREATE_LOCAL_HEAP_NAME( devHeap , devHeapSize   , gfl2::heap::HEAP_TYPE_EXP , false , "SeqViewDev" );;

	mMemAllocator = GFL_NEW(mMemHeap)gfl2::heap::NwAllocator(mMemHeap);
	mDevAllocator = GFL_NEW(mMemHeap)gfl2::heap::NwAllocator(mDevHeap);
	mAppHeap = GFL_NEW(memHeap)::app::util::Heap();
	mAppHeap->Create(mMemHeap,mDevHeap,mMemAllocator,mDevAllocator,NULL,NULL);

	mSeqPlayer = GFL_NEW(mMemHeap)Player(this);

  mSequenceEventListener = GFL_NEW(mMemHeap)SequenceSystemEventListener(this);

  if( mUseEffectSystem )
  {
    GFL_PRINT("SeqViewSystem->Init Effect\n"); 
    mParticleIndexManager = GFL_NEW(mMemHeap)DemoLib::Obj::EffectIndexManager(mMemHeap,64);
    
    mEffectHeap = GFL_NEW(mMemHeap)gfl2::Effect::GFHeap(mDevHeap);

    gfl2::Effect::Config config;
    config.SetEffectHeap( mEffectHeap );
#ifdef GF_PLATFORM_CTR
    config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif //GF_PLATFORM_CTR
    config.SetEmitterNum(512);  //デフォルト64 +1で664
    //config.SetStripeNum(64);  //デフォルト32 +1で22656 だから消す
    config.SetResourceNum(mParticleIndexManager->GetIndexSize());  //現状1024.メモリによっては見直すこと
    mEffectSystem = GFL_NEW(mMemHeap)gfl2::Effect::System(config);

    mEffectGroupIDList[0] = GFL_NEW(mMemHeap) gfl2::util::List<u32>( mMemHeap,1);
    mEffectGroupIDList[0]->PushFront(0);
    mEffectGroupIDList[1] = GFL_NEW(mMemHeap) gfl2::util::List<u32>( mMemHeap,1);
    mEffectGroupIDList[1]->PushFront(1);

    mEffectGroupIDList[2] = GFL_NEW(mMemHeap) gfl2::util::List<u32>( mMemHeap,1);
    mEffectGroupIDList[2]->PushFront(2);

    ARIIZUMI_PRINT("[%d]\n",mDevHeap->GetTotalFreeSize());
  }

	GFL_PRINT("SeqViewSystem->Init Camera\n"); 
	// Camera:
	mIsUpdateCamera = true;
	mCameraPos.Set(256.0f, 106.0f, 465.0f );
	mCameraTrg.Set( -45.0f, 70.0f, -20.0f );
  mCameraOffset.Set(0.0f,0.0f,0.0f);
	mCameraFov = gfl2::math::ConvDegToRad(30.0f);
	cameraCurFrame = 0;
	
  
	GFL_PRINT("SeqViewSystem->Init Object & Resource\n");

	mObjectManager = GFL_NEW(mMemHeap)DemoLib::Obj::ObjectManager(mMemHeap , 128);
	mTaskSystem = GFL_NEW(mMemHeap)DemoLib::Obj::TaskSystem(mMemHeap ,mMemHeap, 64,0x1000);
	mResourceManager = GFL_NEW(mMemHeap)DemoLib::Obj::ResourceManager(mAsyncFileManager,mMemHeap,128);

  mModelObjManager = GFL_NEW(mMemHeap)GfmdlSeqObjectManager(mMemHeap,SEQ_MODEL_NUM);
  mCharaObjManager = GFL_NEW(mMemHeap)CharaSeqObjectManager(mMemHeap,DEMOSEQUENCE_MAX_CHARAMODEL);
  mPokeObjManager = GFL_NEW(mMemHeap)PokeSeqObjectManager(mMemHeap,DEMOSEQUENCE_MAX_POKEMON);
  mLayoutObjManager = GFL_NEW(mMemHeap)LayoutSeqObjectManager(mMemHeap,SEQ_LAYOUT_NUM);
  mLightObjManager = GFL_NEW(mMemHeap)LightSeqObjectManager(mMemHeap,SEQ_LIGHT_NUM);
  mEffectObjManager = GFL_NEW(mMemHeap)EffectSeqObjectManager(mMemHeap,SEQ_EFF_PARTICLE_NUM);
  
	// Camera Animation:
	m_pDrawEnvNode = NULL;	
	m_pDrawEnvResourceRootNode = NULL;
	m_pAnimationResourceRootNode = NULL;
	m_useCameraAnimation = false;

	// Model, Camera, Animation GARC:
	{	
		GFL_PRINT("SeqViewSystem->Init ARCID_DEMOSEQUENCE_RESOURCE GARC\n");

		gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
		req.arcId				= ARCID_DEMOSEQUENCE_RESOURCE;
		req.heapForFile	= GetDevHeap();
		req.heapForReq	= GetDevHeap()->GetLowerHandle();

#if PM_DEBUG
    if(mUseHIO)
    {
      req.hioUseSetting       = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
      req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
    }    
#endif

		GetAsyncFileManager()->SyncArcFileOpen(req);
	}

	// Layout GARC:
	{	
		GFL_PRINT("SeqViewSystem->Init ARCID_DEMOSEQUENCELAYOUT_RESOURCE GARC\n");

    {
		  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
		  req.arcId				= ARCID_DEMOSEQUENCELAYOUT_RESOURCE;
		  req.heapForFile	= GetDevHeap();
		  req.heapForReq	= GetDevHeap()->GetLowerHandle();

#if PM_DEBUG
      if(mUseHIO)
      {
        req.hioUseSetting       = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
        req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
      }    
#endif

		  GetAsyncFileManager()->SyncArcFileOpen(req);
    }
    
    {
      // Multi-Language:
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= (gfl2::fs::ArcFile::ARCID)GetLanguageLayoutGARC();
      req.heapForFile	= GetDevHeap();
      req.heapForReq	= GetDevHeap()->GetLowerHandle();

#if PM_DEBUG
      if(mUseHIO)
      {
        req.hioUseSetting       = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
        req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
      }    
#endif

      GetAsyncFileManager()->SyncArcFileOpen(req);
    }

	}

	//  Render GARC:
	{	
		GFL_PRINT("SeqViewSystem->Init ARCID_RENDERPATH GARC\n");

		gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
		req.arcId				= ARCID_RENDERPATH;
		req.heapForFile	= GetDevHeap();
		req.heapForReq	= GetDevHeap()->GetLowerHandle();
		GetAsyncFileManager()->SyncArcFileOpen(req);
	}
	
	//  SEQUENCE SCRIPT GARC:
	{	
		GFL_PRINT("SeqViewSystem->Init ARCID_DEMOSEQUENCE_SCRIPT GARC\n");

		gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
		req.arcId				= ARCID_DEMOSEQUENCE_SCRIPT;
		req.heapForFile	= GetDevHeap();
		req.heapForReq	= GetDevHeap()->GetLowerHandle();
		GetAsyncFileManager()->SyncArcFileOpen(req);
	}

  seqFileListTable = NULL;

  // Load Debug String??
  {        
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = ARCID_DEMOSEQUENCE_SCRIPT;  
    req.datId = 0;
    req.align = 128;
    req.heapForBuf = GetDevHeap();
    req.heapForReq = GetDevHeap()->GetLowerHandle();
    req.heapForCompressed = NULL;

    req.pBufSize = NULL;
    req.ppBuf = &seqFileListTable;

    GetAsyncFileManager()->AddArcFileLoadDataReq(req);
  }


  //  Common Text Window:
  if( mUseCommonTextWindow)
  {	
    {
      GFL_PRINT("SeqViewSystem->Init LYTARC CommonMenuWindow GARC\n");

      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= ARCID_FIELDWINDOW;
      req.heapForFile	= GetDevHeap();
      req.heapForReq	= GetDevHeap()->GetLowerHandle();
      GetAsyncFileManager()->SyncArcFileOpen(req);
    }

    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req; 
      req.arcId = ARCID_FIELDWINDOW;   
      req.datId = GARC_fieldwindow_fieldwindow_applyt_COMP;
      req.align = 128;
      req.heapForBuf = GetDevHeap();
      req.heapForReq = GetDevHeap()->GetLowerHandle();
      req.heapForCompressed = GetDevHeap();

      req.pBufSize = NULL;
      req.ppBuf = &mLayoutCommonWindowBuffer;

      GetAsyncFileManager()->AddArcFileLoadDataReq(req);

      mWordset = GFL_NEW( GetDevHeap() ) print::WordSet( GetDevHeap() );
      mWordset->RegisterPlayerName( 0, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst() );      
    }
  }

  if( mUseCharaModel)
  {
    // CharaModelFactoryField:
    mCharaFactoryField = GFL_NEW(mDevHeap)poke_3d::model::CharaModelFactory();
    mCharaFactoryField->Initialize(mAsyncFileManager,mDevHeap,ARCID_CHARA_MODEL_FIELD_NORMAL);

    mCharaFactoryField->SetupRandomFunc(System::GflUse::GetPublicRand);

    for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
    {    
      mDressUpModelResourceManagerField[z] = GFL_NEW(mDevHeap)poke_3d::model::DressUpModelResourceManager();
      mDressUpModelResourceManagerField[z]->Initialize(mAsyncFileManager,mDevHeap,System::DressUp::GetDressUpArcIdListField());
    }

    // CharaModelFactoryBattle:
    mCharaFactoryBattle = GFL_NEW(mDevHeap)poke_3d::model::CharaModelFactory();
    mCharaFactoryBattle->Initialize(mAsyncFileManager,mDevHeap,ARCID_CHARA_MODEL_BATTLE_NORMAL);

    mCharaFactoryBattle->SetupRandomFunc(System::GflUse::GetPublicRand);

    for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
    { 
      mDressUpModelResourceManagerBattle[z] = GFL_NEW(mDevHeap)poke_3d::model::DressUpModelResourceManager();
      mDressUpModelResourceManagerBattle[z]->Initialize(mAsyncFileManager,mDevHeap,System::DressUp::GetDressUpArcIdListBattle());
    }

    InitDressupParam();
  }
  else
  {
    mCharaFactoryField = NULL;
    mCharaFactoryBattle = NULL;
    for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
    { 
      mDressUpModelResourceManagerField[z] = NULL;
      mDressUpModelResourceManagerBattle[z] = NULL;
    }
  }
  
  // Sound:
  for( int i=0;i<SEQ_EFF_SOUND_NUM;i++ )
  {
    mSoundObject[i] = GFL_NEW(mMemHeap)SequenceObject_Sound(this,i);
    mObjectManager->AddObject(mSoundObject[i]);
  }

	GFL_PRINT("SeqViewSystem->Init Finished\n");

  // Set Sex:  
  u8 sex = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetSex();
  if(sex==PM_MALE) SetPlayerSexMale();
  else SetPlayerSexFemale();

}

bool SequenceViewSystem :: Finalize(void)
{
  if(mStateReg != SEQPLAYER_STATE_FINISH) return(false);

  switch(mStateRegSub)
  {
    case 0:      
      /*
        2016/04/21  Yu Muto
          NMCat[1071]   ポケモンを進化させた後、会話や調べるなどを行うととまる
          進化→図鑑→（技を覚えずに）終了
          と進行すると、Initialize() -> ProcessToFinalize() -> Finalize() の順にコールされるが、Update()はコールされないため、
          ARCID_DEMOSEQUENCE_SCRIPT とARCID_FIELDWINDOW のロード完了待ちを待たずに、ARCがCloseされてしまう
          この問題の対処のため、ここで全てのFileManagerへのリクエストの完了を待つ
          ※ピートさん提供
      */
      if(!GetAsyncFileManager()->IsAllReqFinished()) return(false);

      GFL_SAFE_DELETE(mBloomAlphaMask);

      mSequenceEventListener->Stop();
      GFL_SAFE_DELETE(mSequenceEventListener);

      if(seqFileListTable!=NULL)
      {      
        GflHeapFreeMemory(seqFileListTable);
      }

      // InFrame Camera:
      {      
        for(int z=0;z<SEQ_NUM_INFRAME_CAMERA;z++)
        {
          GFL_SAFE_DELETE(mInFrameCamera[z]);
          GFL_SAFE_DELETE(mInFrameSys[z]);
        }
      }
      
      // Common Text Window
      for( int i=0;i<MSG_WIN_NUM;i++ )
      {
        if(mMessageWindow[i]!=NULL) mMessageWindow[i]->End();
        GFL_SAFE_DELETE(mMessageWindow[i]);    
      }
      GFL_SAFE_DELETE( mCommonMsgData );
      GFL_SAFE_DELETE( mTrainerMsgData );

      GFL_SAFE_DELETE( mWordset );

      if(mLayoutCommonWindowBuffer!=NULL)
      {      
        GflHeapFreeMemory(mLayoutCommonWindowBuffer);
      }  
      mStateRegSub++;
      break;

    case 1:
      mStateRegSub++;
      break;

    case 2:
      if( mUsePokemonSystem && mPokeModelSys!=NULL )
      {
        // NOT USED: メモリ足りない！
        mPokeModelSys->DeletePokeModelHeap();
        mPokeModelSys->TermSystemAsync();
      }
      mStateRegSub++;
      break;

    case 3:

    case 4:
      if( mUsePokemonSystem == false || mPokeModelSys==NULL)
      {
         mStateRegSub++;
      }      
      else if(mPokeModelSys->IsFinishTermSystemAsync())
      {
         mStateRegSub++;
      }
      break;

    case 5:

      for( int i=0;i<SEQ_EFF_SOUND_NUM;i++ )
      {
        if(mObjectManager!=NULL) mObjectManager->RemoveObject(mSoundObject[i]);
        //リソースを持っているわけではないのでDeleteは無い
        //mSoundObject[i]->Delete();
        GFL_SAFE_DELETE( mSoundObject[i] );
      }
      
      if( mUseCharaModel)
      {
        
        if(mCharaFactoryField) mCharaFactoryField->UnloadModelAll();
        if(mCharaFactoryField) mCharaFactoryField->Finalize();
        GFL_SAFE_DELETE(mCharaFactoryField);

        for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
        { 
          if(mDressUpModelResourceManagerField[z]) mDressUpModelResourceManagerField[z]->UnloadDressUpPartsAll();
          if(mDressUpModelResourceManagerField[z]) mDressUpModelResourceManagerField[z]->Finalize();
          GFL_SAFE_DELETE(mDressUpModelResourceManagerField[z]);
        }

        for(int z=0;z<DEMOSEQUENCE_MAX_MODELDRESSUP;z++)
        {
          if( mVSBattlePoseFile[z] != -1 )
          {
            //非常駐のモーションの開放
            mCharaFactoryBattle->UnloadDynamicAnimation(mVSBattlePoseFile[z]);
            mDressUpModelResourceManagerBattle[z]->UnloadDressUpDynamicAnimation(mDressUpParam[z],mVSBattlePoseFile[z]);
            ARIIZUMI_PRINT("[DinamicRelease %d:%x]\n",z,mVSBattlePoseFile[z]);
          }
          if(mDressUpModelResourceManagerBattle[z]) mDressUpModelResourceManagerBattle[z]->UnloadDressUpPartsAll();
          if(mDressUpModelResourceManagerBattle[z]) mDressUpModelResourceManagerBattle[z]->Finalize();
          GFL_SAFE_DELETE(mDressUpModelResourceManagerBattle[z]);
        }
        if(mCharaFactoryBattle) mCharaFactoryBattle->UnloadModelAll();
        if(mCharaFactoryBattle) mCharaFactoryBattle->Finalize();
        GFL_SAFE_DELETE(mCharaFactoryBattle);
      }

      GFL_SAFE_DELETE(mPokeModelSys);

	    // Model, Camera, Animation GARC:
	    {	
		    GFL_PRINT("SeqViewSystem->Close ARCID_DEMOSEQUENCE_RESOURCE\n");

		    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
		    req.arcId				= ARCID_DEMOSEQUENCE_RESOURCE;
		    GetAsyncFileManager()->SyncArcFileClose(req);
	    }

	    // Layout GARC:
	    {	
		    GFL_PRINT("SeqViewSystem->Close ARCID_DEMOSEQUENCELAYOUT_RESOURCE\n");
        
        {
		      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
		      req.arcId				= ARCID_DEMOSEQUENCELAYOUT_RESOURCE;
		      GetAsyncFileManager()->SyncArcFileClose(req);
        }

        // Multi-Language:
        {        
          gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
          req.arcId				= (gfl2::fs::ArcFile::ARCID)GetLanguageLayoutGARC();
          GetAsyncFileManager()->SyncArcFileClose(req);
        }
	    }

	    //  SEQUENCE SCRIPT GARC:
	    {	
		    GFL_PRINT("SeqViewSystem->Close ARCID_DEMOSEQUENCE_SCRIPT\n");

		    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
		    req.arcId				= ARCID_DEMOSEQUENCE_SCRIPT;
		    GetAsyncFileManager()->SyncArcFileClose(req);
	    }

      //  Render GARC:
      {	
        GFL_PRINT("SeqViewSystem->Close ARCID_RENDERPATH\n");

        gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
        req.arcId				= ARCID_RENDERPATH;
        GetAsyncFileManager()->SyncArcFileClose(req);
      }

      //  MENU WINDOW GARC:
      if( mUseCommonTextWindow)      
      {	
        GFL_PRINT("SeqViewSystem->Close ARCID_FIELDWINDOW\n");

        gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
        req.arcId				= ARCID_FIELDWINDOW;
        GetAsyncFileManager()->SyncArcFileClose(req);
      }

	    GFL_SAFE_DELETE(m_pAnimationResourceRootNode);
	    GFL_SAFE_DELETE(m_pDrawEnvNode);
	    GFL_SAFE_DELETE(m_pDrawEnvResourceRootNode);	

	    GFL_SAFE_DELETE(mSeqPlayer);

      GFL_SAFE_DELETE(mModelObjManager);
      GFL_SAFE_DELETE(mCharaObjManager);
      GFL_SAFE_DELETE(mPokeObjManager);
      GFL_SAFE_DELETE(mLayoutObjManager);  
      GFL_SAFE_DELETE(mLightObjManager);  
      GFL_SAFE_DELETE(mEffectObjManager);  

	    GFL_SAFE_DELETE(mResourceManager);
	    GFL_SAFE_DELETE(mTaskSystem);
	    GFL_SAFE_DELETE(mObjectManager);

      GFL_SAFE_DELETE(mEffectGroupIDList[0]);
      GFL_SAFE_DELETE(mEffectGroupIDList[1]);      
      GFL_SAFE_DELETE(mEffectGroupIDList[2]);      
      GFL_SAFE_DELETE(mEffectSystem);
      GFL_SAFE_DELETE(mEffectHeap);
      GFL_SAFE_DELETE(mParticleIndexManager);
      

	    // Camera Related:	
	    GFL_SAFE_DELETE(m_pAnimationResourceRootNode);
	    GFL_SAFE_DELETE(m_pDrawEnvNode);
	    if(m_BaseCamera!=NULL) m_BaseCamera->Destroy();
      GFL_SAFE_DELETE(m_BaseCamera);

      if ( mRenderSys )
      {
        mRenderSys->Cleanup();
        mRenderSys->Terminate();
      
        GFL_SAFE_DELETE(mRenderSys);
      }

      GFL_SAFE_DELETE(mAppHeap);
      GFL_SAFE_DELETE(mDevAllocator);
      GFL_SAFE_DELETE(mMemAllocator);
      GFL_SAFE_DELETE(mResidentGLAllocator);

      GFL_DELETE_HEAP(mDevHeap);
      GFL_DELETE_HEAP(mMemHeap);

      return true;
  }

	return false;
}

int SequenceViewSystem :: GetCurrentFrame(void)
{
  if(mSeqPlayer==NULL) return(0);
	return(mSeqPlayer->GetSeqFrame() - 1);
}

int SequenceViewSystem :: GetMaxFrame(void)
{
  if(mSeqPlayer==NULL) return(0);
  return(mSeqPlayer->GetMaxSeqFrame());
}

void SequenceViewSystem :: SetTargetFrame(int f) 
{ 
  if(mPausePlayback || mStopPlayback || mSystemSync) return;
	targetFrame = f; 
}


/*
 * @fix GFNMCat[5756]GFNMCat[5761] BGMの再生ポジション巻き戻り対策
 * 
 * ループBGMには対応しないアルゴリズムだが、BGM同期デモでは
 * ループせず終端サンプル数を返す挙動でいい
 */
int SequenceViewSystem :: GetStreamSoundPlaySamplePosition(void)
{
  int nowSamplePosion = Sound::GetStreamSoundPlaySamplePosition(); //有効な再生ハンドルがない場合-1を返す.

  //mStreamSoundSamplePositionLastの初期値は-1としている
  if( nowSamplePosion < mStreamSoundSamplePositionLast ){
    GFL_RELEASE_PRINT("#DemoBgmPostion 巻き戻り発生 now=%d, last=%d\n", nowSamplePosion, mStreamSoundSamplePositionLast );
    return mStreamSoundSamplePositionLast; //巻き戻り発生中は追いつくまで前回と同値を返す
  }

  mStreamSoundSamplePositionLast = nowSamplePosion;
  return mStreamSoundSamplePositionLast;
}
//↑GFNMCat[5756] GFNMCat[5761] BGMの再生ポジション巻き戻り対策


void SequenceViewSystem :: CalculateCurrentFramePosition(void)
{
  //終了スキップがされたので無視
#if PM_DEBUG  //影響が怖いので、一応括る
  if( mIsSkipFinish ){return;}
#endif
  // Speed up?
  if(mFastForwardPlayback && !mSyncBGM)
  {
    tickStart -=  (1.0/0.000030);     
  }
  
  // Calculate Target Frame:
  u64 tickEnd = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
  u64 tickDif = tickEnd - tickStart;

  /*
  if(mForceOneStep)
  {
    tickStart = tickEnd -  (1.0/0.000030); 
    tickDif = tickEnd - tickStart;
  }
  */

  double frame = tickDif*0.000030; 

  if(!mRealTimeMode || mForceOneStep)
  {
    // Just set to SEQ current frame:
    int tframe = GetCurrentFrame();
    if(tframe<0) tframe=0;
    SetTargetFrame(tframe);    
  }

  else if(mSyncBGM)
  {  
    int bgmPos = this->GetStreamSoundPlaySamplePosition();  //@fix GFNMCat[5756]GFNMCat[5761] BGMの再生ポジション巻き戻り対策 巻き戻り補正した値を取得
    f32 rate = (Sound::GetStreamSoundSampleRate()/30.0);
    f32 pos = (f32)bgmPos / rate;
    SetTargetFrame((int)pos);
  }
  else SetTargetFrame((int)frame);

  // Make sure we dont miss any System Syncs:
  if(mSyncFrames[mCurSyncIndex]!=-1)
  {
    int syncFrame = mSyncFrames[mCurSyncIndex];
    
    // Need to sync:
    if(syncFrame<=targetFrame+1)
    {
      GFL_PRINT("*** Force To Sync Frame:%i  Target:%i  Cur:%i\n",syncFrame,targetFrame,GetCurrentFrame());

      int frameDif = (syncFrame-1) - GetCurrentFrame();

      GFL_PRINT("*Jumping %i frames*\n",frameDif);
      tickStart -=  frameDif*(1.0/0.000030);     

      targetFrame = syncFrame;//+1; // Fixes executing one frame after sync..

      mSyncFrames[mCurSyncIndex] = -1;
      mCurSyncIndex++;
    }
  }
  
}

bool SequenceViewSystem :: SeqComFunc_IsReadyToPlaySequenceFile(void)
{
  if(mStateReg != SEQPLAYER_STATE_MAIN) return(false);
  return(true);
}

void SequenceViewSystem :: SetWindowCursor(bool visible)
{
  for( int i=0;i<MSG_WIN_NUM;i++ )
  {
    if(mMessageWindow[i])
    {
      mMessageWindow[i]->SetMsgCursorUserVisible(true,visible);
    }
  }
}

void SequenceViewSystem :: ProcessKeyWait(void)
{
  if(mKeyWaitType!=-1)
  {
    bool finishedWait = false;

    gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

    if(mExitOnFrameWait && mFrameWait > 0)
    {
      // Wait over:
      mFrameWait--;
      if(mFrameWait<=0)
      {
        finishedWait = true;
      }
    }
    
    if(!finishedWait)
    {
      switch(mKeyWaitType)
      {
      case 0: // AB
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) || pButton->IsTrigger( gfl2::ui::BUTTON_B )) finishedWait = true;
        break;

      case 1: // Text A Wait
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
        {          
          Sound::PlaySE(SEQ_SE_MESSAGE);
          SetWindowCursor(false);
          finishedWait = true;
        }
        break;
      }      
    }

    if(finishedWait) SystemSync();
  }
}

//更新(30フレーム前半用)
void SequenceViewSystem :: Update(void)
{  

  if(mSleepRecover)
  {
    mSleepRecover = false;
    TogglePausePlayback(false);
  }

  // Send System Sync During Sleep, make sure it triggers: (MCAT:5746)
  if(mSystemSyncSleepTrigger && IsSystemSync())
  {
    SystemSync();
    if(!IsSystemSync()) mSystemSyncSleepTrigger = false;
  }

  // Check for Network Dialog Error Display Event:
  if(NetLib::Error::NijiNetworkErrorDialogStatus::IsNetworkErrorDialogEvent())
  {   
    u64 tickDif = gfl2::system::Timer::PerformanceCounter::GetMicroSecond() - NetLib::Error::NijiNetworkErrorDialogStatus::GetErrorDialogStartTick();
    if(!mSystemSync) tickStart+=tickDif;

    // Clear the Event:
    NetLib::Error::NijiNetworkErrorDialogStatus::ResetNetworkErrorDialogEvent();
  }

  ///GFL_PRINT("*SEQVIEW::TARGETFRAME:%i  CURFRAME: %i\n",targetFrame,GetCurrentFrame());

  // Process Key wait if in SystemSync:
  if(mSystemSync) ProcessKeyWait();

	switch(mStateReg)
	{
		case SEQPLAYER_STATE_INIT:
			State_Init();
			break;

		case SEQPLAYER_STATE_INITWAIT:
			State_InitWait();
			if(mStateReg==SEQPLAYER_STATE_MAIN)  { ResetStartTick(); } // Fall through to SEQPLAYER_STATE_MAIN!
      else break;

		case SEQPLAYER_STATE_MAIN:
      if(mStartupWaitMode) // Don't start playing sequence until ready
      {        
        return;
      }

      {
        int step = State_Main_Prelogic();
			  if(step!=-1) State_Main(step);			
        mRenderSys->GetRenderingPipeLine()->UpdateBloom();
      }
			break;

    case SEQPLAYER_STATE_CLEANUP:
      State_Cleanup();
      break;

		case SEQPLAYER_STATE_FINISH:
			State_Finish();
			break;
	}
	
  lastFrame = targetFrame;
}

int SequenceViewSystem :: State_Main_Prelogic(void)
{
  if( ( (mCommandSkipOnCancel && IsSystemCancel() ) || (mCommandSkipOnSync && mSystemSyncTrigger) ) && mCommandSkipFrame!=-1)
  {
    GFL_PRINT("*CommandSkipCancel-> %i to %i\n*",targetFrame,mCommandSkipFrame);
    mCommandSkipCountdown = true;
    if(mCommandSkipOnCancel) mCommandSkipOnCancel=false;
    else if(mCommandSkipOnSync)
    {
      //mSystemSyncTrigger = false;
      mCommandSkipOnSync=false;
      //mSystemSync=true;
    }
    
    int frame = mCommandSkipWait - 2;
    if(frame<0) frame = 0;
    // Transition type:
    switch(mCommandTransType)
    {
    case SEQSKIP_CMD_TRANSITION_NONE:
      break;

    case SEQSKIP_CMD_TRANSITION_FADEBLACK:
      {      
        gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);	    
        gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);	      
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &blackCol, &blackCol2,frame);
      }
      break;

    case SEQSKIP_CMD_TRANSITION_FADEWHITE:
      {      
        gfl2::math::Vector4 whiteCol(255.0f,255.0f,255.0f,0.0f);
        gfl2::math::Vector4 whiteCol2(255.0f,255.0f,255.0f,255.0f);
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &whiteCol, &whiteCol2,frame);
      }
      break;
    }
  }

  if(mCommandSkipCountdown && mCommandSkipWait<=0)
  {    
    // Make sure screen is faded out if needed:
    switch(mCommandTransType)
    {
      case SEQSKIP_CMD_TRANSITION_FADEBLACK:
        {      
          gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);	    
          gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);	      
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &blackCol, &blackCol2,0);
        }
        break;

      case SEQSKIP_CMD_TRANSITION_FADEWHITE:
        {      
          gfl2::math::Vector4 whiteCol(255.0f,255.0f,255.0f,0.0f);
          gfl2::math::Vector4 whiteCol2(255.0f,255.0f,255.0f,255.0f);
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &whiteCol, &whiteCol2,0);
        }
        break;
    }

    mCommandSkipWait = 0;
    mCommandSkipCountdown = false;
    CommandSkip(mCommandSkipFrame);
    mCommandSkipFrame=-1;          
  }

  // Finished?
  if(mSeqPlayer->IsFinishAll() && GetTaskSystem()->GetActiveTaskNum()==0)
  {
    SetState(SEQPLAYER_STATE_CLEANUP);
    return(-1);
  }

  // Calculate Current Frame: (Frame Skip)
  if(!mPausePlayback && !mSystemSync) CalculateCurrentFramePosition();

  int step = 1;

  if(mStateReg == SEQPLAYER_STATE_MAIN)
  {

    ///	mSeqPlayer->SetSeqFrame(targetFrame);
    step = targetFrame - GetCurrentFrame();
    if(step<=0 && IsSystemSync() && !mPausePlayback) step = 1;
    if( mForceOneStep )
    {
      step = 1;
    }

    //@fix GFNMCat[5756]GFNMCat[5761] BGMの再生ポジション巻き戻り対策 全体的な巻き戻り観測用アサートを仕込む
    GFL_ASSERT_MSG((step>=0),"#Demo %dframeの巻き戻りが発生しました\n",step*-1);

    if(!mPausePlayback)
    {

      mTaskSystem->SetSyncMode(IsSystemSync());
      mTaskSystem->Update(step);	

      mObjectManager->Update(step);

      // Fade Faster if frame skipping:
      if(step>1)
      {
        for(int n=0;n<step-1;n++)
        {
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->Update();
        }
      }
      
      if( mPokeModelSys )
      {
        mPokeModelSys->Update();
      }

      UpdateInframeSystem();

      if( mUseEffectSystem && mEffectSystem!=NULL )
      {
        mEffectSystem->BeginFrame();
        mEffectSystem->Calc(0);
        mEffectSystem->Calc(1);
        mEffectSystem->Calc(2);
      }
    }
    else if(mPausePlayback)
    {
      mObjectManager->Update(0);
    }

    if(mPausePlayback) 
    {     
      step = 0;
    }

    else
    {
      if(IsSystemSync())
      {
        if(targetFrame>=GetCurrentFrame() ) mSeqPlayer->Update(0);
        if(mFastForwardPlayback) step = 2;
      }
      else if(targetFrame>=GetCurrentFrame() ) mSeqPlayer->Update(step);
    }

    UpdateCamera(step);
  }
  else ResetStartTick();
  
  return(step);
}

void SequenceViewSystem :: State_Init(void)
{	
  switch(mStateRegSub)
  {
    case 0:
        if( mUseCommonTextWindow && !GetAsyncFileManager()->IsArcFileLoadDataFinished(&mLayoutCommonWindowBuffer) ) break;
        mStateRegSub++;
      // break through   
    case 1:
      SetState(SEQPLAYER_STATE_INITWAIT); // Wait until Sequence Is Loaded!
      break;

  }	
}

void SequenceViewSystem :: State_InitWait(void)
{
  // Init & Load files needed for Sequence:
	switch(mStateRegSub)
	{	
    case 0: // Wait for Sequence to be loaded
      if(!mLoadedSequenceFile)
      {
        break;
      }
      mSeqPlayer->PreprocessCount(); // Count Pokemon and Other Objects
      
      // Render System:
      GFL_PRINT("SeqViewSystem->Init Render System\n");
      mRenderSys = GFL_NEW(mMemHeap)SequenceRenderSystem(this);

      mUseOption.isUseInframe1 = mSeqPlayer->UseInframe1();
      mUseOption.isUseInframe2 = mSeqPlayer->UseInframe2();
      mUseOption.isUseMotionBlur = mSeqPlayer->UseMotionBlur();
      mUseOption.isUseBloomRenderPath = mSeqPlayer->UseBloom();
      mUseOption.isUseWarpModel = mSeqPlayer->UseWarpModelDraw();

      mRenderSys->SetUseOption( mUseOption);
      mRenderSys->Initialize();

      // Effect Render:
      if( mUseEffectSystem )
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
        effRenderPath = mRenderSys->GetRenderingPipeLine()->GetEffectBeforeModelRenderPath();
        effRenderPath->SetConfig(config);
      }
      mStateRegSub++;
      // BREAK THROUGH
    case 1:
      if( mRenderSys->Setup() == false ) return;
      mStateRegSub++;
      // BREAK-THROUGH

    case 2:
      if(!GetAsyncFileManager()->IsArcFileLoadDataFinished(&seqFileListTable)) break;
      if(!GetAsyncFileManager()->IsAllReqFinished()) break; 
      mStateRegSub++;
      // BREAK THROUGH
    case 3:

      if(GetRenderSys()->GetRenderingPipeLine()->GetMotionBlurPath()!=NULL)
      {
        if(!GetRenderSys()->GetRenderingPipeLine()->GetMotionBlurPath()->InitializeMotionBlurUpdate()) 
        {
          break;
        }
        GFL_PRINT("*MotionBlurPass Init Finished*\n");
        mStateRegSub++;		
      }
      else
      {
        GFL_PRINT("*MotionBlur Not Available!*\n");
        mStateRegSub++;		
      }

      // BREAK THROUGH
    case 4:
      // PokeModelSys:
      if( mUsePokemonSystem )
      {
        GFL_PRINT("SeqViewSystem->Init PokeModelSys\n"); 
        mPokeModelSys = GFL_NEW(mDevHeap)PokeTool::PokeModelSystem();
        mPokeModelSys->InitSystemAsync(mDevHeap,mDevHeap->GetLowerHandle(),GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),mSeqPlayer->GetPokemonCount());
                  
        // Create Heap:
        PokeTool::PokeModelSystem::HeapOption heapOption;
        heapOption.useColorShader = mSeqPlayer->UsePokeColorShader();
        heapOption.animeType       = PokeTool::MODEL_ANIMETYPE_BATTLE;
        heapOption.useIdModel      = false;
        heapOption.useShadow       = true;
        
        mResidentGLAllocator = GFL_NEW( mDevHeap ) System::nijiAllocator(mDevHeap);
        GFL_PRINT("*MEM HEAP BEFORE POKEMODELHEAP*\n");
#if PM_DEBUG
        mResidentGLAllocator->DebugVramMemoryInfoDump();
#endif
        /// メモリ足りないので、一応コメントアウト：
        mPokeModelSys->CreatePokeModelHeap(mDevHeap,&heapOption);

        GFL_PRINT("\n\n*MEM HEAP AFTER POKEMODELHEAP*\n");
#if PM_DEBUG
        mResidentGLAllocator->DebugVramMemoryInfoDump();
#endif
          
        InitializeInframeSystem();
      }
      mStateRegSub++;	
      // break through

		case 5:
      if( mPokeModelSys )
      {
        if(!mPokeModelSys->IsFinishInitSystemAsync()) return;      
        mPokeModelSys->CreateSystem(true,GetGLAllocator());
      }      
			// Start Sequence Load:      
      {      
        ResetStartTick();
        mSeqPlayer->PreloadAll();          
        LoadAllDressUpParts();
        mSeqPlayer->SetPause(true);
        mStateRegSub++;	        
      }
      // break through

    case 6:
      // Common Text Window:
      {
        if( mUseCommonTextWindow)
        {

          app::util::Heap appUtilHeap;
          appUtilHeap.Create(mMemHeap,mDevHeap,mMemAllocator,mDevAllocator,NULL,NULL);

          //メッセージの読み込み
          if (!mUseFieldMessageArc) // Use normal message Script:
          {
            mCommonMsgData = GFL_NEW(mDevHeap)gfl2::str::MsgData(print::GetMessageArcId(), mTextWindowArcID, mDevHeap, gfl2::str::MsgData::LOAD_FULL);
          }
          else
          { 
            mTrainerMsgData = GFL_NEW(mDevHeap)gfl2::str::MsgData(print::GetMessageArcId_Script(), mTextWindowArcID, mDevHeap, gfl2::str::MsgData::LOAD_FULL);
          }

          for( int i=0;i<MSG_WIN_NUM;i++ )
          {
            if( mUseTextWindowFlg[i] == true )
            {
              mMessageWindow[i] = GFL_NEW(GetDevHeap()) App::Tool::TalkWindow(&appUtilHeap,GetRenderSys()->GetAppRenderingManager());
              switch(i)
              {
              case 1: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_SYS_DOWN); break;
              case 2: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_TELOP); break;
              case 3: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_CREDIT); break;
              case 4: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_TELOP_WHITE); break;
              case 5: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_TELOP_CLEAR); break;
              default: mMessageWindow[i]->SetWindowType(App::Tool::WIN_TYPE_TALK_DOWN); break;
              }
              mMessageWindow[i]->Setup(mLayoutCommonWindowBuffer);        
              if(!mUseFieldMessageArc) // Use normal message Script:
                mMessageWindow[i]->InitMsgData(mCommonMsgData);
              else // Use Field Script:
                mMessageWindow[i]->InitMsgData(mTrainerMsgData);
              mMessageWindow[i]->InitTrainerMsgData();
              mMessageWindow[i]->SetScriptWordSet(mWordset);
              mMessageWindow[i]->SetEnableMessageBoost(false);   
              mMessageWindow[i]->SetMessageSpeed(  Savedata::ConfigSave::MSG_NORMAL );      
            }
          }
        }

        mStateRegSub++;									
      }
      // break through


    case 7: // Skybox:
      if(!mUseSkybox || mSkybox==NULL)
      {
        ResetStartTick();
        mStateRegSub++;
        break;
      }

      mSkybox->UpdateCreateResource();

      if(!mSkybox->IsCreateResource())
      {
        break;
      }
      mSkybox->CreateParts( NULL, true, System::nijiAllocator::VRAM_MALLOC_A);     
      ResetStartTick();
      mStateRegSub++;
      // break through

    case 8: // Resource Manager Load:
      if( !(GetResourceManager()->IsFinishLoad()==true && AllPokemonLoadingFinished() && AllCharaModelLoadingFinished()) )
      {			
        break;
      }

      GetResourceManager()->CreateResource(GetMemHeap(),GetGLAllocator(),mEffectSystem,mEffectHeap,GetParticleIndexManager());
      SetupAllCharaModels();
      mSeqPlayer->SetPause(false);
      mStateRegSub++;        
      // break through

    case 9:      
      //BGM同期モードだけ：新しくサウンドのコマンド消化（＝デモ曲のPrepare実行）を待ってから先へ進む
      if(mSyncBGM && Sound::HaveCommand() )
      {
        break;
      }

      if(bgmWaitIdx!=-1)
      {
        Sound::PreparePlayBGMReq();
      }
      mSequenceEventListener->Start();
			SetState(SEQPLAYER_STATE_MAIN);			
      ResetStartTick();
      
			break;
	}

}

void SequenceViewSystem :: State_Main(int step)
{
  if(mCommandSkipCountdown && mCommandSkipWait>0 && !mPausePlayback)  
    mCommandSkipWait-=step;
  
  if(mCommandSkipWait<0) mCommandSkipWait = 0;

  // Turn Off Skybox?
  if(mSkyBoxOffFrame!=-1 && targetFrame>=mSkyBoxOffFrame) mSkybox->SetEnable(false);

	mRenderSys->Update();
  if(targetFrame>=messageWindowEndFrame && messageWindowEndFrame!=-1)
  {
    for( int i=0;i<MSG_WIN_NUM;i++ )
    {
      if(mMessageWindow[i]!=NULL) 
      {
        mMessageWindow[i]->CloseMsgWin();
      }
    }
    messageWindowEndFrame = -1;
  }
  for( int i=0;i<MSG_WIN_NUM;i++ )
  {
    if(mMessageWindow[i]!=NULL) 
    {
      mMessageWindow[i]->Update();
    }
  }

  // Remove Motion blur if needed:
  if(GetMotionBlur()!=NULL)
  {
    if(mBlurEndFrame!=-1 && targetFrame>=mBlurEndFrame)
    {
      mBlurEndFrame = -1;
      GetMotionBlur()->RenderOff();
      GFL_PRINT("*Turn Motion Blur Off!*\n");
    }
  }

  if(mForceOneStepPause)
  {
    mForceOneStepPause = false;
    TogglePausePlayback();
  }
}

void SequenceViewSystem :: State_Cleanup(void)
{
  switch(mStateRegSub)
  {
    case 0:      
      // Turn Motion Blur off just in case...
      if(GetMotionBlur())
      {      
        GetMotionBlur()->RenderOff();
        GFL_PRINT("*Turn Motion Blur Off!*\n");
      }
      mStateRegSub++;
      // break through

    case 1:
      if(mUseSkybox && mSkybox!=NULL)
      {      
        // Add all parts to render:
        for( u32 i=0 ; i<System::Skybox::Skybox::PARTS_TYPE_MAX ; i++ )
        {
          mRenderSys->GetRenderingPipeLine()->RemoveObject_Skybox(mSkybox->GetPartsModel((System::Skybox::Skybox::PartsType)i)->GetModelInstanceNode());            
        }
      }

      for( int i=0;i<MSG_WIN_NUM;i++ )
      {
        if(mMessageWindow[i]!=NULL && mMessageWindow[i]->IsVisible()) mMessageWindow[i]->CloseMsgWin();
      }

      mStateRegSub++;
      break; // REQUIRED! (Still rendering Skybox)

    case 2:
        /*	while(GetTaskSystem()->GetActiveTaskNum() != 0)
		  {
			  GetTaskSystem()->Update();
		  } */

      for( int i=0;i<SEQ_EFF_SOUND_NUM;i++ )
      {
        mSoundObject[i]->SetSoundId(-1);
      }

      if(mModelObjManager) mModelObjManager->ClearBuffer(mObjectManager);
      if(mCharaObjManager) mCharaObjManager->ClearBuffer(mObjectManager);
      if(mPokeObjManager) mPokeObjManager->ClearBuffer(mObjectManager);
      if(mLayoutObjManager) mLayoutObjManager->ClearBuffer(mObjectManager);
      if(mLightObjManager) mLightObjManager->ClearBuffer(mObjectManager);
      if(mEffectObjManager) mEffectObjManager->ClearBuffer(mObjectManager);
      
      mResourceManager->DeleteAllResource(GetEffectSystem(),GetParticleIndexManager());			
      
      if(mUseSkybox && mSkybox!=NULL)
      {
        mSkybox->DestroyAll();
        mSkybox->DeleteHeap();
      }    
      mStateRegSub++;
      // break through

    case 3:
      SetState(SEQPLAYER_STATE_FINISH);
      break;
  }
}

void SequenceViewSystem :: State_Finish(void)
{

}

void SequenceViewSystem :: ProcessToFinalize(void)
{
  SetState(SEQPLAYER_STATE_FINISH);
}

//-------------------------------------------------------
//シーケンス系
//-------------------------------------------------------
wchar_t *SequenceViewSystem :: SeqComFunc_GetSeqName(u32 index)
{
  if(seqFileListTable == NULL) return(NULL);

  u32 *val = (u32*)seqFileListTable;
  u32 dataCount = *val;

  if(index==0 || index>dataCount) return(NULL);
  
  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(seqFileListTable);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;
 
  return(reinterpret_cast<wchar_t *>(ptr));
}

#if PM_DEBUG
bool SequenceViewSystem::SeqComFunc_PlaySequenceFile( const wchar_t *filename )
{
  if(seqFileListTable == NULL) return(NULL);

  u32 *val = (u32*)seqFileListTable;
  u32 dataCount = *val;

  for(u32 n=1;n<dataCount;n++)
  {  
    if(SeqComFunc_GetSeqName(n)==NULL) continue;
    if(wcsncmp(filename,SeqComFunc_GetSeqName(n),wcslen(filename) )==0)    
    {
      // Found Matching Sequence Filename:
      SeqComFunc_PlaySequenceFile(n);
      return(true);
    }
  }
  // Couldn't Find Sequence:
  return(false);
}
#endif

void SequenceViewSystem::SeqComFunc_PlaySequenceFile( int no )
{
	// Reset Camera:	
	m_useCameraAnimation = false;	
	mCameraPos.Set(256.0f, 106.0f, 465.0f );
	mCameraTrg.Set( -45.0f, 70.0f, -20.0f );
	mCameraFov = gfl2::math::ConvDegToRad(30.0f);
	gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
	gfl2::math::Matrix34 viewMtx = gfl2::math::Matrix34::GetLookAt(mCameraPos, mCameraTrg, upDirection);
	//mRenderSys->SetCamViewMtx(viewMtx);

	mSequenceSeq = 0;
	mSeqPlayer->LoadSeqFile(no);
	mSeqPlayer->SetPause(true);

	targetFrame = 0;

  SetLoadedSequenceFile();
}
void SequenceViewSystem::SeqComFunc_PlaySequenceFileDebug(void)
{
	mSequenceSeq = 0;
	mSeqPlayer->DebugLoadPath("R:/home/momiji/resource/demo/event/tool/test.bseq");
	mSeqPlayer->SetPause(true);

  SetLoadedSequenceFile();
}

void SequenceViewSystem::ResetStartTick(void)
{
	tickStart = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
	targetFrame = 0;
  lastFrame = 0;
}

bool SequenceViewSystem::SeqComFunc_IsFinishSequenceFile( bool isNoRelease )
{ 
  return(mStateReg == SEQPLAYER_STATE_FINISH);  
}

//描画更新
void SequenceViewSystem :: PreDraw(void)
{
  if(mStateReg==SEQPLAYER_STATE_FINISH) return;

	if(GetCurrentFrame()==-1) return;
	
	mRenderSys->PreDraw();
}
void SequenceViewSystem :: Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(mStateReg==SEQPLAYER_STATE_FINISH) return;

	if(GetCurrentFrame()==-1) return;		
	mRenderSys->Draw(displayNo);
  mObjectManager->DrawLayout(displayNo); // Draw all layouts
  for( int i=0;i<MSG_WIN_NUM;i++ )
  {
    if(mMessageWindow[i]!=NULL) mMessageWindow[i]->Draw(displayNo);
  }

#if PM_DEBUG

  if(displayNo != 0 || !mShowDebugInfo) return;
  
  for(int z=0;z<SEQ_NUM_INFRAME_CAMERA;z++)
  {  
    if(!mSetInframe[z]) continue;

    s32 left,top,right,bottom;

    left = inFramePosX[z] - (inFrameSizeX[z]/2);
    right = inFramePosX[z] + (inFrameSizeX[z]/2);
    top = inFramePosY[z] - (inFrameSizeY[z]/2);
    bottom = inFramePosY[z] + (inFrameSizeY[z]/2);

    f32 xOff=0;
    f32 yOff = 0;

    if(inFrameFollowMdl[z]!=NULL)
    {                
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
      jointNode = inFrameFollowMdl[z]->GetJointNode(inFrameFollowNodeName[z]);
      if( jointNode )
      {
        const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();  
        Inframe_ConvertWorldPointToScreenPoint(z,xOff,yOff,mtx.GetElemPosition());        
      }      
    }
    
    gfl2::gfx::GFGL::BeginScene();
    gfl2::gfx::DrawUtil::BeginRender();

    gfl2::gfx::DrawUtil::SetScreenSize( 400, 240);
    gfl2::gfx::GFGL::SetViewport( 0, 0, 400, 240);

//    gfl2::gfx::DrawUtil::DrawRect(16.0f,16.0f,32.0f,32.0f);


    gfl2::gfx::DrawUtil::DrawRect(left+xOff,top+yOff,inFrameSizeX[z],1);
    gfl2::gfx::DrawUtil::DrawRect(left+xOff,top+yOff,1,inFrameSizeY[z]);
    gfl2::gfx::DrawUtil::DrawRect(right+xOff,top+yOff,1,inFrameSizeY[z]);
    gfl2::gfx::DrawUtil::DrawRect(left+xOff,bottom+yOff,inFrameSizeX[z],1);
  
    gfl2::gfx::DrawUtil::EndRender();
    gfl2::gfx::GFGL::EndScene();
    
  }
#endif

}

// Util:
u32 SequenceViewSystem::CheckDataId_Model( u32 datId , u16 idx )
{
	return datId;
}

/*
using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;
*/
// Camera
void SequenceViewSystem::LoadCameraDrawEnv(int arcID,int arcIDX,bool isRelative)
{
  if(m_BaseCamera!=NULL) 
  {
    m_BaseCamera->Destroy();  
    GFL_SAFE_DELETE(m_BaseCamera);
  }

  m_BaseCamera = GFL_NEW(mMemHeap) poke_3d::model::BaseCamera();
 
  GFL_SAFE_DELETE(m_pDrawEnvNode);
  GFL_SAFE_DELETE(m_pAnimationResourceRootNode);
  GFL_SAFE_DELETE(m_pDrawEnvResourceRootNode);
  

	gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envResourceData;

	void *data = mResourceManager->GetDataBuf(arcID,arcIDX);
	if(data==NULL)
	{
		GFL_PRINT("SeqSys :: Camera Env Load Fail [%d,%d]\n",arcID,arcIDX);
	}

	envResourceData.SetBinaryEnvData( static_cast<c8*>(data) );


	m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(GetGLAllocator(),&envResourceData);
	m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(GetGLAllocator(),m_pDrawEnvResourceRootNode);

	// ライトは邪魔
	for (u32 i = 0; i < m_pDrawEnvNode->LightSetMax; ++i)
	{
		gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = m_pDrawEnvNode->GetLightSet(i);
		if (pLightSetNode == NULL)
		{
			continue;
		}
		pLightSetNode->RemoveAllLight();
	}

	m_BaseCamera->Create(GetGLAllocator(), mMemHeap, m_pDrawEnvNode, 0);
	
  if(isRelative)
  {  
    m_BaseCamera->SetPosition(mCameraPos.x,mCameraPos.y,mCameraPos.z);
    m_BaseCamera->SetTargetPosition(mCameraTrg);
  }
}


void SequenceViewSystem::LoadCameraMotion(int arcID,int arcIDX,int numFrame,bool isRelative)
{
  GFL_SAFE_DELETE(m_pAnimationResourceRootNode);

	void *data = mResourceManager->GetDataBuf(arcID,arcIDX);
	if(data==NULL)
	{
		GFL_PRINT("SeqSys :: Camera Motion Load Fail [%d,%d]\n",arcID,arcIDX);
	}

	animationResourceData.SetMotionData(static_cast<c8*>(data));
	m_pAnimationResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(GetGLAllocator(),&animationResourceData);

	m_useCameraAnimation = true;
	static gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR;

  if(!isRelative)
  {
	  m_BaseCamera->ChangeAnimationGlobalSmoothByResourceNode(m_pAnimationResourceRootNode, 0, easeFunc);	
  }
  else // Move from current camera position:
  {
    m_BaseCamera->ChangeAnimationLocalSmoothByResourceNode(m_pAnimationResourceRootNode, 0, easeFunc);	
  }
	  
  m_BaseCamera->SetAnimationLoop(false);
	cameraCurFrame = 0;///GetCurrentFrame();// targetFrame;
}

void SequenceViewSystem::UpdateCamera(int step)
{  
  gfl2::math::Matrix34 sndViewMtx;

	if(m_useCameraAnimation)
	{
		// カメラアニメーション更新
//     cameraCurFrame+=step;
// 		f32 cameraAniFrame = (f32)(cameraCurFrame);
// 		m_BaseCamera->SetAnimationFrame(cameraAniFrame);

    /*
    f32 cameraAniFrame = m_BaseCamera->GetAnimationFrame();
    f32 frame = cameraAniFrame + (step-1);
    if(frame>cameraAniFrame || !m_BaseCamera->IsAnimationLoop()) m_BaseCamera->SetAnimationFrame(frame);
    */


    if(!m_BaseCamera->IsAnimationLoop() && IsRealTimeMode())
    {
      int f = GetTargetFrame();
      f-=mCameraStartAniFrame;
      m_BaseCamera->SetAnimationFrame((f32)f);
    }

    // Frame Skip for Looping Camera:
    else if(step>1)
    {
      f32 cameraAniFrame = m_BaseCamera->GetAnimationFrame();
      f32 frame = cameraAniFrame + (step);
      if(frame>cameraAniFrame) m_BaseCamera->SetAnimationFrame(frame);
    }

///		GFL_PRINT("CameraFrame: %f  Start:%i  Target:%i Cur:%i \n",cameraAniFrame,cameraCurFrame,targetFrame,GetCurrentFrame() );

    m_BaseCamera->UpdateAnimation();    
    ///GFL_PRINT("CameraFrame: %f  Target:%i\n\n",m_BaseCamera->GetAnimationFrame(),GetTargetFrame() );
		
		mRenderSys->SetCamPrjMtx(m_BaseCamera->GetProjectionMatrix());
		gfl2::math::Matrix34 viewMatrix = m_BaseCamera->GetViewMatrix();
		mRenderSys->SetCamViewMtx(m_BaseCamera->GetViewMatrix());			    
    mRenderSys->GetRenderingPipeLine()->SetBillboardViewMatrix(m_BaseCamera->GetViewMatrix());
    
    sndViewMtx = m_BaseCamera->GetViewMatrix();
		
	}

	else if( mIsUpdateCamera )
	{		
		gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
		gfl2::math::Matrix34 viewMtx = gfl2::math::Matrix34::GetLookAt(mCameraPos+mCameraOffset, mCameraTrg+mCameraOffset, upDirection);
		mRenderSys->SetCamViewMtx(viewMtx);
    mRenderSys->GetRenderingPipeLine()->SetBillboardViewMatrix(viewMtx);
    sndViewMtx = viewMtx; 
	}

  //3Dサウンド対応
  {
    gfl2::math::MTX34 mtx34;
    mtx34._00 = sndViewMtx[0].x;
    mtx34._01 = sndViewMtx[0].y;
    mtx34._02 = sndViewMtx[0].z;
    mtx34._03 = sndViewMtx[0].w;
    mtx34._10 = sndViewMtx[1].x;
    mtx34._11 = sndViewMtx[1].y;
    mtx34._12 = sndViewMtx[1].z;
    mtx34._13 = sndViewMtx[1].w;
    mtx34._20 = sndViewMtx[2].x;
    mtx34._21 = sndViewMtx[2].y;
    mtx34._22 = sndViewMtx[2].z;
    mtx34._23 = sndViewMtx[2].w;
    Sound::SetListenerMatrix( mtx34 );
  }
}

void SequenceViewSystem::GetDefaultCamera(gfl2::math::Vector3 &pos , gfl2::math::Vector3 &trg,f32 &fov )
{
  pos.Set(256.0f, 106.0f, 465.0f );
  trg.Set( -45.0f, 70.0f, -20.0f );
  fov = gfl2::math::ConvDegToRad(30.0f);
}

void SequenceViewSystem::SetCameraPosition( const gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &trg )
{
	mCameraPos = pos;
	mCameraTrg = trg;
	mIsUpdateCamera = true;
}

void SequenceViewSystem::SetCameraOffset( const gfl2::math::Vector3 &pos )
{
  mCameraOffset = pos;
  mIsUpdateCamera = true;
}

void SequenceViewSystem::SetDebugPokemonSetting(int index,int monsNo,int formNo,int sex,bool enable)
{
  if(index < 0 || index>=MAX_DEBUGSEQ_POKEMON_SETTINGS) return; // Invalid Index

  pokemonDebugSettings[index].monsNo = monsNo;
  pokemonDebugSettings[index].formNo = formNo;
  pokemonDebugSettings[index].sex = sex;
  pokemonDebugSettings[index].enable = enable;

}

void SequenceViewSystem::PausePlaybackAdvanceFrame(void)
{
   if(mStopPlayback || !mPausePlayback) return; // Not in PAUSE!

   TogglePausePlayback();
   mForceOneStepPause = true;
}

void SequenceViewSystem::TogglePausePlayback(bool soundControl)
{
#if PM_DEBUG
  if (soundControl || mForceOneStepPause )
  { // soundControl == true はデバッグモード（DemoViewer）でしか呼ばれない
    // mForceOneStep == trueも同様
    GFL_ASSERT( this->IsTestMode());
  }
#endif

  if(mStopPlayback || mForceOneStepPause) return;

  // Currently Paused:
  if(mPausePlayback)
  {
#if PM_DEBUG
    if(soundControl)
    {
      Sound::RestartBGM();
    }
#endif
    u64 tickDif = gfl2::system::Timer::PerformanceCounter::GetMicroSecond() - mPauseStartTick;
    
    if(!mSystemSync) tickStart+=tickDif;
    
  }
  else
  {
#if PM_DEBUG
    if(soundControl)
    {
      Sound::PauseBGM(Sound::BGM_FADE_NONE);
    }
#endif
    mPauseStartTick = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();    
  }

  mPausePlayback = (mPausePlayback==false);
}

void SequenceViewSystem::StopPlayback(void)
{
  if(mPausePlayback) return;

  if(mSystemSync) SystemSync();

#if PM_DEBUG  //影響が怖いので、一応括る
  mIsSkipFinish = true;
#endif
  CommandSkip(99999);
//  mStopPlayback = true;
}

void SequenceViewSystem::AddBGMSync(long bgmPos,int bgmIdx,int frame) 
{ 
  if(numBGMSyncSettings<0 || numBGMSyncSettings>=MAX_BGM_SYNC_SETTINGS) return; // Too many

  bgmSyncSettings[numBGMSyncSettings].bgmPreloadPos = bgmPos;
  bgmSyncSettings[numBGMSyncSettings].bgmWaitIdx = bgmIdx;
  bgmSyncSettings[numBGMSyncSettings].bgmStartFrame = frame;
  
  numBGMSyncSettings++;


}

void SequenceViewSystem::SetSystemCancel(bool b)
{
  if(!mCanSetSystemCancel) return;
  mSystemCancel = b;
}

void SequenceViewSystem::SetTextWindowStyle(int windowStyle)
{
  mUseTextWindowFlg[windowStyle] = true;
}

void SequenceViewSystem::DisplayTextWindowMessage(int messageID,int endFrame,int windowStyle)
{
  if(mMessageWindow[windowStyle]==NULL || endFrame<GetTargetFrame()) return;

  mMessageWindow[windowStyle]->SetMessage((u32)messageID);
  mMessageWindow[windowStyle]->CallMsgWin();
  messageWindowEndFrame = endFrame;
  
}

void SequenceViewSystem::TextWindowMessageSetting(int speed,bool buttonSpeedUp)
{
  for( int i=0;i<MSG_WIN_NUM;i++ )
  {
    if(mMessageWindow[i])
    {

      mMessageWindow[i]->SetEnableMessageBoost(buttonSpeedUp);

      if(speed!=0)
      {
        mMessageWindow[i]->SetMessageSpeed(  (Savedata::ConfigSave::MSG_SPEED)(speed-1));
      }
      else // USER Setting:
      {
        mMessageWindow[i]->SetMessageSpeed(  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetConfig()->GetTalkingSpeed() );
      }
    }
  }
}

System::MotionBlur *SequenceViewSystem::GetMotionBlur(void) 
{ 
  return(GetRenderSys()->GetRenderingPipeLine()->GetMotionBlurPath()); 
}

void SequenceViewSystem::StartBloom(void *texBuf)
{
  //連続使用対策
  GFL_SAFE_DELETE(mBloomAlphaMask);

  GetRenderSys()->GetRenderingPipeLine()->EnableBloom();

  // Bloom Mask Texture:
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
  texData.SetTextureData(static_cast<c8*>(texBuf));
  mBloomAlphaMask = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(GetGLAllocator(), &texData);

  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(mBloomAlphaMask);
  GetRenderSys()->GetRenderingPipeLine()->SetBloomMaskTexture(texResNode->GetTexture());

}

void SequenceViewSystem::EndBloom(void)
{
  GetRenderSys()->GetRenderingPipeLine()->DisableBloom();
  GetRenderSys()->GetRenderingPipeLine()->SetBloomMaskTexture(NULL); // Remove Mask Texture
  
  //ここでは記するとまずいのでシステムに任せる
  //GFL_SAFE_DELETE(mBloomAlphaMask);
}

void SequenceViewSystem::CommandSkip(int tFrame)
{
  lastCommandSkipFrame = tFrame;

  tFrame--;
  int frameDif = tFrame - GetCurrentFrame();
   
  GFL_PRINT("*Jumping %i frames*\n",frameDif);
  tickStart -=  frameDif*(1.0/0.000030);     
 // SetTargetFrame(tFrame);
  if(!mRealTimeMode)
  {
    targetFrame = tFrame;
  }

#if PM_DEBUG  //影響が怖いので、一応括る
  if( mIsSkipFinish )
  {
    targetFrame = tFrame;
  }
#endif

  if(!IsRealTimeMode())
  {
    mSeqPlayer->Update(frameDif);
    targetFrame = tFrame;
    mTaskSystem->Update(frameDif);	
    mObjectManager->Update(frameDif);
  }

  // Clear any syncs:
  for(int n=0;n<SEQ_MAX_SYSTEMSYNC;n++)
  {
    if(mSyncFrames[n]<=tFrame) mSyncFrames[n] = -1;    
  }

  // Reset Cur Index:
  mCurSyncIndex = 0;
  for(int n=0;n<SEQ_MAX_SYSTEMSYNC;n++)
  {
    if(mSyncFrames[n]!=-1) { mCurSyncIndex = n; break; }
  }
}

bool SequenceViewSystem::IsAfterCommandSkip(int frame)
{
  return(frame>=lastCommandSkipFrame);
}

void SequenceViewSystem::SetCommandSkipOnCancel(int executeType,int frame,int wait,int type) 
{ 
  
  mCommandSkipFrame=frame; 
  mCommandSkipWait = wait; 
  mCommandTransType=type; 

  switch(executeType)
  {
    case SEQSKIP_CMD_ONCANCEL:
      mCommandSkipOnCancel=true; 
      break;
  }
   
}

void SequenceViewSystem::SetCommandSkipOnSync(int executeType,int frame,int wait,int type) 
{ 

  mCommandSkipFrame=frame; 
  mCommandSkipWait = wait; 
  mCommandTransType=type; 

  switch(executeType)
  {
  case SEQSKIP_CMD_ONSYNC:
    mCommandSkipOnSync=true; 
    break;
  }

}


void SequenceViewSystem::AddSystemSync(int frame)
{
  if(mNumSystemSync>=SEQ_MAX_SYSTEMSYNC) 
  {
    GFL_PRINT("XXX TOO MANY SYSTEM SYNCS! XXX\n");
      return;
  }

  GFL_PRINT("*Added System Sync at frame %i\n",frame);
  mSyncFrames[mNumSystemSync++]=frame;

}

int SequenceViewSystem::GetLanguageType(void)
{
  // Return Current Language Setting:
  switch(print::GetMessageLangId())
  {
  case System::MSGLANGID_KANA:
  case System::MSGLANGID_KANJI:
    return(DEMO_LANG_JP);
    
  case System::MSGLANGID_ENGLISH: return(DEMO_LANG_USA);
  case System::MSGLANGID_FRENCH: return(DEMO_LANG_FRA);
  case System::MSGLANGID_ITALIAN: return(DEMO_LANG_ITA);
  case System::MSGLANGID_GERMAN: return(DEMO_LANG_DEU);
  case System::MSGLANGID_SPANISH: return(DEMO_LANG_ESP);
  case System::MSGLANGID_KOREAN: return(DEMO_LANG_KOR);
  case System::MSGLANGID_CHINA: return(DEMO_LANG_CHN);
  case System::MSGLANGID_TAIWAN: return(DEMO_LANG_TWN);
  }
  return(DEMO_LANG_JP);
}

int SequenceViewSystem::GetLanguageLayoutGARC(void)
{
  int arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_JP;

  // Return Correct GARC Based on Current Language Setting:
  switch(print::GetMessageLangId())
  {
    case System::MSGLANGID_KANA:
    case System::MSGLANGID_KANJI:
      break;
    
    case System::MSGLANGID_ENGLISH: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_USA; break;
    case System::MSGLANGID_FRENCH: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_FRA; break;
    case System::MSGLANGID_ITALIAN: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_ITA; break;
    case System::MSGLANGID_GERMAN: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_DEU; break;
    case System::MSGLANGID_SPANISH: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_ESP; break;
    case System::MSGLANGID_KOREAN: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_KOR; break;
    case System::MSGLANGID_CHINA: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_CHN; break;
    case System::MSGLANGID_TAIWAN: arcID = ARCID_DEMOSEQUENCELAYOUT_RESOURCE_TWN; break;
  }

  return(arcID);
}

//演出対象ボールの取得(IDの0はBALLID_NULLなので注意)
int SequenceViewSystem::SeqComFunc_GetTargetBall(int idx)
{
  if(idx<0 || idx>=SEQ_MONSTERBALL_NUM) return item::BALLID_MONSUTAABOORU;
  return(monsterBallType[idx]);  
}

void SequenceViewSystem::SetVSBattleMonsterBallSetting(int index, u8 data[VSBATTLE_MONSBALLSTATUS_NUM])
{
  if(index<0 || index>=DEMOSEQUENCE_MAX_MODELDRESSUP) return;
  for(int z=0;z<VSBATTLE_MONSBALLSTATUS_NUM;z++) mVSBattleMonsterBallStatus[index][z] = data[z];
}

void SequenceViewSystem::InitializeInframeSystem(void)
{
  if(!mUsePokemonSystem) return;

  for(int z=0;z<SEQ_NUM_INFRAME_CAMERA;z++)
  {    
    // Setup by defaults:
    s32 left = 0;
    s32 top = 0;
    s32 right = 200;
    s32 bottom = 200;

    mInFrameSys[z] = GFL_NEW(GetMemHeap()) System::Camera::CModelViewerInFrame();

    mInFrameSys[z]->Init(System::Camera::CModelInFrame::SCREEN_UPPER, left, top, right, bottom, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT);    
    mInFrameSys[z]->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER);

    mInFrameCamera[z] = GFL_NEW(GetMemHeap()) System::Camera::InFrameCamera();
    
  }

}

void SequenceViewSystem::Inframe_ConvertWorldPointToScreenPoint(int index,f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point)
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  ///GetRenderSys()->GetRenderingPipeLine()->GetViewAndProjectionMatrixPivot(view_mtx,proj_mtx);

  proj_mtx = *GetRenderSys()->GetCamProjMtx();
  view_mtx = *GetRenderSys()->GetCamViewMtx();

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

void SequenceViewSystem::UpdateInframeSystem(void)
{
  if(!mUseInframe) return;  

  for(int z=0;z<SEQ_NUM_INFRAME_CAMERA;z++)
  {
    if(mInFrameCamera[z]==NULL || mInFrameSys[z]==NULL) continue;
    
    if(inFrameFollowMdl[z]!=NULL)
    {     
      inFrameFollowMdl[z]->UpdateAnimationForce();
      s32 left,top,right,bottom;

      left = inFramePosX[z] - (inFrameSizeX[z]/2);
      right = inFramePosX[z] + (inFrameSizeX[z]/2);
      top = inFramePosY[z] - (inFrameSizeY[z]/2);
      bottom = inFramePosY[z] + (inFrameSizeY[z]/2);

      f32 xOff=0;
      f32 yOff = 0;
      f32 sclX = 1.0f;
      f32 sclY = 1.0f;
      f32 rotY = 0.0f;
      f32 rotX = 0.0f;

      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
      jointNode = inFrameFollowMdl[z]->GetJointNode(inFrameFollowNodeName[z]);
      if( jointNode )
      {
        const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();  
        sclX = mtx.GetElemScaling().GetX();
        sclY = mtx.GetElemScaling().GetY();
        Inframe_ConvertWorldPointToScreenPoint(z,xOff,yOff,mtx.GetElemPosition());        

        gfl2::math::Vector3 rotRad = mtx.GetElemRotationZYX_fix();
        rotY = rotRad.y;
        rotX = rotRad.x;
      }  
      
      left *= sclX; right*=sclX;
      top *= sclY; bottom*=sclY;

      left += (s32)xOff;
      right += (s32)xOff;
      top += (s32)yOff;
      bottom += (s32)yOff;
      
      mInFrameSys[z]->SetFrame( left, top, right, bottom);
  
      

      if(mUseInframeFollowRotate[z])
      {                          
        if(rotX!=0.0f)
        {
          float rot = PI / 2.0f;
          rot += (rot - rotY);
          mInFrameSys[z]->SetCameraYaw(rot + mBaseInframeRotY[z]);
          GFL_PRINT("Index:%i  Pos: %f,%f  Scale:%f,%f RotY:%f  RotX:%f\n",z,xOff,yOff,sclX,sclY,rot,rotX);
        }
        else if(rotY<0.0f)
        {
          float rot = (PI + rotY) + PI;
          mInFrameSys[z]->SetCameraYaw(rot + mBaseInframeRotY[z]);
          GFL_PRINT("Index:%i  Pos: %f,%f  Scale:%f,%f RotY:%f  RotX:%f\n",z,xOff,yOff,sclX,sclY,rot,rotX);
        }
        else
        {
          mInFrameSys[z]->SetCameraYaw(rotY + mBaseInframeRotY[z]);
          GFL_PRINT("Index:%i  Pos: %f,%f  Scale:%f,%f RotY:%f  RotX:%f\n",z,xOff,yOff,sclX,sclY,rotY,rotX);
        }
     //   mInFrameSys[z]->SetCameraPitch(-rotX + mBaseInframeRotX[z]);
      }

      
          
    }

    mInFrameSys[z]->Update(mInFrameCamera[z]);

    const gfl2::math::Matrix44 projMat =  mInFrameCamera[z]->GetProjectionMatrix();
    const gfl2::math::Matrix34 viewMat =  mInFrameCamera[z]->GetViewMatrix();

    if(GetRenderSys()!=NULL && GetRenderSys()->GetRenderingPipeLine()!=NULL)
      GetRenderSys()->GetRenderingPipeLine()->SetInframeCamera(z,projMat,viewMat);
  }

}

void SequenceViewSystem::Inframe_AttachPokemon(int index,PokeTool::PokeModel *mdl)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;
  if(mInFrameSys[index]==NULL) return;

  mInFrameSys[index]->SetAndInitModel(mdl);
  mUseInframe = true;

  mBaseInframeRotX[index] = mInFrameSys[index]->GetCameraPitch();
  mBaseInframeRotY[index] = mInFrameSys[index]->GetCameraYaw();
}

void SequenceViewSystem::Inframe_SetFollowModel(int index,SequenceObject_Gfmdl *mdl,char *nodeName)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;  

  inFrameFollowMdl[index] = mdl;
  memcpy(inFrameFollowNodeName[index],nodeName,32);
  mUseInframe = true;
  
  UpdateInframeSystem();

  
}

void SequenceViewSystem::Inframe_RemovePokemon(int index)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;

  mInFrameSys[index]->SetAndInitModel(NULL);
  inFrameFollowMdl[index] = NULL;  

  UpdateInframeSystem();
}

void SequenceViewSystem::Inframe_SetSizeAndPosition(int index,int pos,int sizeX,int sizeY,int posX,int posY)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;

  mSetInframe[index] = true;
  inFrameSizeX[index] = sizeX;
  inFrameSizeY[index] = sizeY;
  inFramePosX[index] = posX;
  inFramePosY[index] = posY;

  s32 left,top,right,bottom;

  left = posX - (sizeX/2);
  right = posX + (sizeX/2);
  top = posY - (sizeY/2);
  bottom = posY + (sizeY/2);

  mInFrameSys[index]->SetFrame( left, top, right, bottom);

  if(pos==0) mInFrameSys[index]->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM);
  else mInFrameSys[index]->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_CENTER);

  mUseInframe = true;

  mBaseInframeRotX[index] = mInFrameSys[index]->GetCameraPitch();
  mBaseInframeRotY[index] = mInFrameSys[index]->GetCameraYaw();
}
void SequenceViewSystem::Inframe_SetRotation(int index,gfl2::math::Vector3 rot)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;

  mInFrameSys[index]->SetCameraYaw(gfl2::math::ConvDegToRad(rot.y));
  mInFrameSys[index]->SetCameraPitch(gfl2::math::ConvDegToRad(rot.x));

  UpdateInframeSystem();
}

void SequenceViewSystem::Inframe_SetRotationOffset(int index,gfl2::math::Vector3 rot)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;
  
  mInFrameSys[index]->AddCameraYaw(gfl2::math::ConvDegToRad(rot.y));
  mInFrameSys[index]->AddCameraPitch(gfl2::math::ConvDegToRad(rot.x));

  UpdateInframeSystem();
}

void SequenceViewSystem::Inframe_SetVisible(int index,bool b)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;
  mInFrameSys[index]->SetModelVisible(b);
}

void SequenceViewSystem::Inframe_UseFollowRotate(int index,bool b)
{
  if(index<0 || index>=SEQ_NUM_INFRAME_CAMERA) return;
  mUseInframeFollowRotate[index] = b;
}

bool SequenceViewSystem::IsTimezone_Morning(void) 
{ 
  if(IsTimezone_Realtime())
  {
    int hour,min,sec;

    GetTime(hour,min,sec);

    // Don't use morning....
    return(false);
  }
  return(timezoneSetting==0); 
}

bool SequenceViewSystem::IsTimezone_Noon(void) 
{
  if(IsTimezone_Realtime())
  {
    int hour,min,sec;

    GetTime(hour,min,sec);

    // 5:30 ~ 16:53
    if(hour==5 && min<=29) return(false); // 5:00~5:29
    else if(hour<5) return(false); // 00:00 - 4:59
    else if(hour==16 && min>=54) return(false); // 16:54~
    else if(hour>=17) return(false); // 17:00~
    return(true);
  }
  return(timezoneSetting==1); 
}

bool SequenceViewSystem::IsTimezone_Evening(void) 
{
  if(IsTimezone_Realtime())
  {
    int hour,min,sec;

    GetTime(hour,min,sec);

    GFL_PRINT("*TIME CHECK: %i-%i-%i\n",hour,min,sec);

    // 16:54 ~ 17:44
    if(hour==16 && min<=53) return(false); // ~16:00 - 16:53
    else if(hour<16) return(false); // 00:00 - 15:59
    else if(hour==17 && min>=45) return(false);    // 17:45~
    else if(hour>17) return(false); // 18:00~

    GFL_PRINT("*IS EVENING *TRUE*\n");
    return(true);
  }
  return(timezoneSetting==2); 
}

bool SequenceViewSystem::IsTimezone_Night(void) 
{
  if(IsTimezone_Realtime())
  {
    int hour,min,sec;

    GetTime(hour,min,sec);

    GFL_PRINT("*TIME CHECK: %i-%i-%i\n",hour,min,sec);

    // 17:45 ~ 5:29
    if(hour<5) return(true); // ~5
    else if(hour==5 && min<=29) return(true); // 5:00 ~ 5:29
    
    if(hour>=18) return(true); // 18~
    if(hour==17 && min>=45) return(true); // 17:45~

    GFL_PRINT("*IS EVENING *FALSE*\n");

    return(false);
  }
  return(timezoneSetting==3); 
}

void SequenceViewSystem::GetTime(int &hour,int &min,int &sec) 
{ 
#if PM_DEBUG
  // Override Realtime?
  if(!IsTimezone_Realtime())
  {
    if(IsTimezone_Morning())
    {
      hour = 9;
      min = sec = 0;
      return;
    }
    else if(IsTimezone_Noon())
    {
      hour = 12;
      min = sec = 0;
      return;
    }
    else if(IsTimezone_Evening())
    {
      hour = 17;
      min = 15;
      sec = 0;
      return;
    }
    else if(IsTimezone_Night())
    {
      hour = 21;
      min = sec = 0;
      return;
    }
    
  }
#endif
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetGameTime()->IsAlolaOffsetExists() )
  {
    hour = mAlolatime.GetHour(); min = mAlolatime.GetMinute(); sec = mAlolatime.GetSecond(); 
  }
  else
  { // AlolaTimeが初期化されていないのにリアルタイムを要求されるデモは存在しないはず！！
    GFL_ASSERT(0);
    hour = 12; min = 0; sec = 0;
  }
  GFL_PRINT("*GETTIME: %i - %i - %i***\n",hour,min,sec);
}  

f32 SequenceViewSystem::CalculateMotionTimeFrame(int hour,int min,int sec)
{
  f32 totalSecond = ( 24 * 60 * 60 );
  f32 nowSecond = ( hour * 60 * 60 ) + ( min * 60 ) + sec;

  f32 t = nowSecond / totalSecond;
  t = gfl2::math::Clamp( t, 0.0f, 1.0f );
  return(t);
}

  GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

