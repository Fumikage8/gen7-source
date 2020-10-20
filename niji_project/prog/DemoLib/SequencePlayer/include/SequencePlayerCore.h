//======================================================================
/**
 * @file	SequencePlayerCore.h
 * @brief	SequencePlayer中心機能
 * @author	PETE
 * @data	2015.04.27
 */
//======================================================================
#if !defined( __SEQUENCE_PLAYER_CORE_H__ )
#define __SEQUENCE_PLAYER_CORE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include <gfx/include/gfl2_GFGL.h>
#include <ui/include/gfl2_UI.h>

#include "DemoLib/SequencePlayer/include/SequenceRenderSystem.h"
#include "DemoLib/SequencePlayer/include/SequenceLocalDef.h"
#include "DemoLib/SequencePlayer/include/SequenceObject.h"

#include <DemoLib/Object/include/ObjectManager.h>
#include <DemoLib/Object/include/TaskSystem.h>
#include <DemoLib/Object/include/ResourceManager.h>

#include <model/include/gfl2_DressUpModel.h>

#include <model/include/gfl2_BaseCamera.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>

#include <pml/include/pml_PokePara.h>

#include <System/include/Skybox/Skybox.h>

///#include  "AppLib/include/Tool/CommonMessageWindowView.h"
#include <AppLib/include/Tool/TalkWindow.h>
#include <Print/include/WordSet.h>

// Motion Blur:
#include "System/include/MotionBlur.h"

#include "GameSys/include/NijiTime.h"

// InFrame
#include <System/include/Camera/InFrameCamera.h>
#include <System/include/Camera/ModelViewerInFrame.h>

// System Event Listener & Manager:
#include <System/include/SystemEventManager.h>
#include <System/include/SystemEventListener.h>

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class CharaModelFactory;
    class DressUpModelResourceManager;
    struct DressUpParam;
  GFL_NAMESPACE_END( mdoel )
GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN( PokeTool )
  class PokeModelSystem;
GFL_NAMESPACE_END( PokeTool )

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

class SequenceSystemEventListener;


// CallBack:
//typedef void (*SequenceCallBackFunc)(int command, int paramA,int paramB);

class SequenceCallBack
{
public:
  virtual ~SequenceCallBack(){}
  virtual void SequenceCallBackFunc(int command, int paramA, int paramB) = 0;
};

// Callback Commands:
enum
{
  SEQCALLBACK_COMMAND_SEQPOSNOTIFY = 0,
  SEQCALLBACK_COMMAND_SYSWINDOW,
};

// Skip Command:
enum
{
  SEQSKIP_CMD_ALWAYS = 0,
  SEQSKIP_CMD_ONCANCEL = 1,  
  SEQSKIP_CMD_ONSYNC = 2,  
};

enum
{
  SEQSKIP_CMD_TRANSITION_NONE = 0,
  SEQSKIP_CMD_TRANSITION_FADEWHITE = 1,
  SEQSKIP_CMD_TRANSITION_FADEBLACK = 2,
};

// Set Parameter Types
enum
{
  SEQPARAMTYPE_NONE = 0,
  SEQPARAMTYPE_VSBATTLE,
};

// VS BATTLE PARAMS
enum
{
  VSBATTLE_PRESTART = 0,
  VSBATTLE_WIN,
  VSBATTLE_LOSE,
  VSBATTLE_DRAW,  
};

// VS BATTLE MONSTER BALL STATUS:
enum
{
  VSBALL_NONE = 0,
  VSBALL_NORMAL,
  VSBALL_HURT,
  VSBALL_DEAD,
};

class Player;
class DemoLib::Obj::EffectIndexManager;
typedef SequenceObjectManager<SequenceObject_Gfmdl> GfmdlSeqObjectManager;
typedef SequenceObjectManager<SequenceObject_Chara> CharaSeqObjectManager;
typedef SequenceObjectManager<SequenceObject_Poke> PokeSeqObjectManager;
typedef SequenceObjectManager<SequenceObject_Layout> LayoutSeqObjectManager;
typedef SequenceObjectManager<SequenceObject_Light> LightSeqObjectManager;
typedef SequenceObjectManager<SequenceObject_Effect> EffectSeqObjectManager;

const int SEQ_MODEL_NUM = 64;
const int SEQ_LAYOUT_NUM = 16;
const int SEQ_LIGHT_NUM = 8;
const int DEMOSEQUENCE_MAX_POKEMON = 6;
const int DEMOSEQUENCE_MAX_CHARAMODEL = 32;
const int DEMOSEQUENCE_MAX_FIELDMODEL = 16;
const int DEMOSEQUENCE_MAX_BATTLEMODEL = 16;

const int DEMOSEQUENCE_MAX_MODELDRESSUP = 4;

const int SEQ_EFF_PARTICLE_NUM = 64;
const int SEQ_EFF_CLUSTER_NUM = 5;
const int SEQ_MONSTERBALL_NUM = DEMOSEQUENCE_MAX_POKEMON;

const int VSBATTLE_MONSBALLSTATUS_NUM = 6;

const int SEQ_NUM_INFRAME_CAMERA = 2;

// エフェクトID計算
//複数作成は10個までと想定
inline int SeqCalcEffectId_Effect( int grpIdx , int arrIdx ){return grpIdx*10+arrIdx;}
//複数作成は10個までで、同時作成は100個までと想定
inline int SeqCalcEffectId_ClusterEffect( int grpIdx , int arrIdx ,int num){return (SEQ_EFF_PARTICLE_NUM*10) + grpIdx*1000+arrIdx*100+num;}
inline int SeqCalcEffectId_ProgramEffect( int idx ){return (SEQ_EFF_PARTICLE_NUM*10) + (SEQ_EFF_CLUSTER_NUM*1000);}

typedef struct
{
  u32 monsNo;
  u32 formNo;
  u32 sex;
  bool enable;
} SeqPokemonDebugSetting;

typedef struct  
{  
  int bgmStartFrame;  
  long bgmPreloadPos;
  int bgmWaitIdx;
} BGMSyncSetting;

const int MAX_BGM_SYNC_SETTINGS = 8;

const int MAX_DEBUGSEQ_POKEMON_SETTINGS = DEMOSEQUENCE_MAX_POKEMON;
const int SEQ_EFF_SOUND_NUM = 6;
const int SEQ_MAX_SYSTEMSYNC = 64; // Way more than enough for one sequence.

enum
{
  DEMO_LANG_JP = 0,
  DEMO_LANG_CHN,
  DEMO_LANG_DEU,
  DEMO_LANG_ESP,
  DEMO_LANG_FRA,
  DEMO_LANG_ITA,
  DEMO_LANG_KOR,
  DEMO_LANG_TWN,
  DEMO_LANG_USA,
};

class SequenceViewSystem
{
	GFL_FORBID_COPY_AND_ASSIGN(SequenceViewSystem);

public:
	SequenceViewSystem(bool isTestMode=false);	
	~SequenceViewSystem(void);

	void Initialize(gfl2::heap::HeapBase* memHeap , gfl2::heap::HeapBase* devHeap,int memHeapSize=0x300000,int devHeapSize=0x1B00000 );
  void InitDressupParam(void);
  void InitializeInframeSystem(void);
  
	// Deallocation
	virtual bool Finalize(void);

	// Update:
  void CalculateCurrentFramePosition(void);
	virtual void Update(void);
  void UpdateInframeSystem(void);

	// Rendering:
	void PreDraw(void);
	void Draw(gfl2::gfx::CtrDisplayNo displayNo);

	// SEQ Functions:	
  wchar_t *SeqComFunc_GetSeqName(u32 index);

#if PM_DEBUG
  bool SeqComFunc_PlaySequenceFile( const wchar_t *filename ); // .SEQは含まないで
#endif

	void SeqComFunc_PlaySequenceFile( int no );
	bool SeqComFunc_IsFinishSequenceFileAuto( bool isNoRelease );	
	void SeqComFunc_PlaySequenceFileDebug(void);
	bool SeqComFunc_IsFinishSequenceFile( bool isNoRelease = false );
  bool SeqComFunc_IsReadyToPlaySequenceFile(void);

	// Camera Related:
	void LoadCameraDrawEnv(int arcID,int arcIDX,bool isRelative);
	void LoadCameraMotion(int arcID,int arcIDX,int numFrame,bool isRelative);

	// Frame:
	int GetCurrentFrame(void);
  int GetMaxFrame(void);
	void AddBGMSync(long bgmPos,int bgmIdx,int frame);
  void SetBGMWait(int bgmIdx) { bgmWaitIdx = bgmIdx; }

  // Time
  void GetTime(int &hour,int &min,int &sec);
  f32 CalculateMotionTimeFrame(int hour,int min,int sec);

	// Accessors:
	gfl2::fs::AsyncFileManager* GetAsyncFileManager(void){return mAsyncFileManager;}
	virtual gfl2::heap::HeapBase* GetMemHeap(void){return mMemHeap;}
	virtual gfl2::heap::HeapBase* GetDevHeap(void){return mDevHeap;}
	virtual gfl2::heap::NwAllocator* GetMemAllocator(void){return mMemAllocator;}
	virtual gfl2::heap::NwAllocator* GetDevAllocator(void){return mDevAllocator;}
	virtual ::app::util::Heap* GetAppHeap(void){return mAppHeap;}

	DemoLib::Obj::ObjectManager* GetObjectManager(void){return mObjectManager;}
	DemoLib::Obj::TaskSystem* GetTaskSystem(void){return mTaskSystem;}
	DemoLib::Obj::ResourceManager* GetResourceManager(void){return mResourceManager;}
  DemoLib::Obj::EffectIndexManager* GetParticleIndexManager(void){return mParticleIndexManager;}
	
	gfl2::gfx::IGLAllocator* GetGLAllocator(void){return mRenderSys->GetGLAllocator();}
  gfl2::Effect::GFHeap* GetEffectHeap(void){return mEffectHeap;}
  gfl2::Effect::System* GetEffectSystem(void){return mEffectSystem;}
	SequenceRenderSystem* GetRenderSys(void){return mRenderSys;}
  PokeTool::PokeModelSystem* GetPokeModelSys(void){return mPokeModelSys;}
  poke_3d::model::CharaModelFactory *GetCharaModelFactoryField(void) { return mCharaFactoryField; }
  poke_3d::model::CharaModelFactory *GetCharaModelFactoryBattle(void) { return mCharaFactoryBattle; }
  poke_3d::model::DressUpModelResourceManager *GetDressUpModelResourceManagerField(int index) { return mDressUpModelResourceManagerField[index]; }
  poke_3d::model::DressUpModelResourceManager *GetDressUpModelResourceManagerBattle(int index) { return mDressUpModelResourceManagerBattle[index]; }

  GfmdlSeqObjectManager* GetModelObjManager(void){return mModelObjManager;}
  CharaSeqObjectManager* GetCharaObjManager(void){return mCharaObjManager;}
  PokeSeqObjectManager* GetPokeObjManager(void){return mPokeObjManager;}
  LayoutSeqObjectManager* GetLayoutObjManager(void){return mLayoutObjManager;}
  LightSeqObjectManager* GetLightObjManager(void){return mLightObjManager;}
  EffectSeqObjectManager* GetEffectObjManager(void){return mEffectObjManager;}

  // Pokemon:
  SequenceObject_Poke *GetIndexPokemonAndRemove(int n);
  void LoadPokemon(int n,int type);
  void LoadPokemonFromParameters(int monsNo,int formNo,int sexType,int type);
  bool AllPokemonLoadingFinished(void);  
  void SetPokemonParam(u32 index, pml::pokepara::PokemonParam *p);
  const pml::pokepara::PokemonParam * GetPokemonParam(u32 index ) const { GFL_ASSERT(index<DEMOSEQUENCE_MAX_POKEMON); return mPokemonParam[index]; }
  //PokeModelSystemを初期化しない。Initialize前に呼ぶ
  void SetUsePokemonSystem(bool flg){mUsePokemonSystem = flg;}

  // MonsterBall:
  int SeqComFunc_GetTargetBall(int idx);
  void SetMonsterBallType(int idx,int type);

  // CharaModel:
  void LoadCharaFieldModel(int n);
  void LoadCharaFieldAttachItemModel(int n);
  void LoadCharaBattleModel(int n);
  void LoadCharaBattleAttachItemModel(int n);
  void LoadCharaVSBattleModel(int n);
  void AddDressUpParamLoadField(int n);
  void AddDressUpParamLoadBattle(int n);
  void LoadAllDressUpParts(void);
  void SetupAllCharaModels(void);
  bool AllCharaModelLoadingFinished(void);
  void SetUseCharaModel(bool flg){mUseCharaModel = flg;}

  // Chara DressUp Param:
  void SetDressUpParam(const poke_3d::model::DressUpParam *param,bool isMale=false); // FEMALE DEFAULT
  poke_3d::model::DressUpParam *GetDressUpParam(int index,bool isMale);  
  
  // VS Battle DressUp Param:
  void SetVSBattleDressUpParam(Savedata::MyStatus *status,int index); // Index 0 - 3
  poke_3d::model::DressUpParam *GetDressUpParamVSBattle(int index);  
  int GetVSBattleModelID(int index);
  int GetVSBattleStartAnimation(int index);

  // VS Battle Monster Ball Setting
  void SetVSBattleMonsterBallSetting(int index, u8 data[VSBATTLE_MONSBALLSTATUS_NUM]);
  u8 *GetVSBattleMonsterBallSetting(int index) { return(mVSBattleMonsterBallStatus[index]); }

  //Effect
  //EffectSystemを初期化しない。Initialize前に呼ぶ
  void SetUseEffectSystem(bool flg){mUseEffectSystem = flg;}

  // Sound:
  SequenceObject_Sound* GetSoundObj(int idx){return mSoundObject[idx];}
  void SetSyncBGM(bool b) { mSyncBGM = b; }  

  // Texture Replacement for GFMDL:
  void SetReplaceTexture(int index, gfl2::renderingengine::scenegraph::DagNode* pTextureShaderNodeRoot) { mReplaceTexture = pTextureShaderNodeRoot; }
  gfl2::renderingengine::scenegraph::DagNode *GetReplaceTexture(int index) { return(mReplaceTexture); }

	//カメラ系  
	void UpdateCamera(int step);
	void SetCameraPosition( const gfl2::math::Vector3 &pos , const gfl2::math::Vector3 &trg );
  void GetDefaultCamera(gfl2::math::Vector3 &pos , gfl2::math::Vector3 &trg,f32 &fov );
	const gfl2::math::Vector3& GetCameraPosition(void)const{return mCameraPos;}
	const gfl2::math::Vector3& GetCameraTarget(void)const{return mCameraTrg;}
	void SetCameraFov(f32 fov){mCameraFov = fov;mIsUpdateCamera = true;}
	const f32 GetCameraFov(void){return mCameraFov;}
  const gfl2::math::Vector3& GetCameraOffset(void)const{return mCameraOffset;}
  void SetCameraOffset(const gfl2::math::Vector3 &pos);
  void SetCameraStartAniFrame(int n) { mCameraStartAniFrame=n; }

  // InFrame Camera
  void Inframe_AttachPokemon(int index,PokeTool::PokeModel *mdl);
  void Inframe_RemovePokemon(int index);
  void Inframe_SetSizeAndPosition(int index,int pos,int sizeX,int sizeY,int posX,int posY);
  void Inframe_SetFollowModel(int index,SequenceObject_Gfmdl *mdl,char *nodeName);
  bool UseInframeSystem(void) { return(mUseInframe); }
  void Inframe_SetRotation(int index,gfl2::math::Vector3 rot);
  void Inframe_SetRotationOffset(int index,gfl2::math::Vector3 rot);
  void Inframe_SetVisible(int index,bool b);
  void Inframe_UseFollowRotate(int index,bool b);

  // SkyBox
  void LoadSkyBox(f32 rot);
  void AddSkyBoxRendering(int weatherType,f32 scale,gfl2::math::Vector3 pos,int hour,int min,int sec);
  void SetSkyboxOffFrame(int frame) { mSkyBoxOffFrame = frame; }

	// Util:
	u32 CheckDataId_Model( u32 datId , u16 idx = 0 );

  // WordSet:
  void SetTrainerName(u32 bufID, const Savedata::MyStatus* status );
  void SetPokeNickName(u32 bufID, const pml::pokepara::CoreParam* pCoreParam );

	// State Functions:
	void SetState(u32 s) { mStateReg=s; mStateRegSub=0; }
	void State_Init(void);
	void State_InitWait(void);
  int State_Main_Prelogic(void);
	void State_Main(int step);
  void State_Cleanup(void);
	void State_Finish(void);

  void ProcessToFinalize(void);

	void GetSeqStartTick(u64 &tick) { tick = tickStart; }

  void SetTargetFrame(int f); // DO NOT USE! (--->PRIVATE)
  
  int GetTargetFrame(void) { return(targetFrame); }
	void ResetStartTick(void);
	
  // PlayerSex Setting:
  bool IsPlayerSexMale(void) { return(playerSexSetting==1); }
  bool IsPlayerSexFemale(void) { return(playerSexSetting==2); }
  void SetPlayerSexMale(void) { playerSexSetting = 1; }
  void SetPlayerSexFemale(void) { playerSexSetting = 2; }

  // VS Battle Name:
  bool GetVSBattleName(gfl2::str::StrBuf *buf,int index);

  // Timezone Setting:
  void SetTimezone_Realtime(void) { timezoneSetting = -1; }
  void SetTimezone_Morning(void) { timezoneSetting = 0; }
  void SetTimezone_Noon(void) { timezoneSetting = 1; }
  void SetTimezone_Evening(void) { timezoneSetting = 2; }
  void SetTimezone_Night(void) { timezoneSetting = 3; }

  bool IsTimezone_Realtime(void) { return(timezoneSetting==-1); }
  bool IsTimezone_Morning(void);
  bool IsTimezone_Noon(void);
  bool IsTimezone_Evening(void);
  bool IsTimezone_Night(void);

  // Sun & Moon Check:
  bool IsSun(void);
  bool IsMoon(void);

  // Debug Pokemon Settings:
  void SetDebugPokemonSetting(int index,int monsNo,int formNo,int sex,bool enable);

  // Playback control:
  void TogglePausePlayback(bool soundControl = true);
  void PausePlaybackAdvanceFrame(void);
  void StopPlayback(void);
  void SetFastForwardPlayback(bool b) { mFastForwardPlayback = b; }
  void SetSystemCancel(bool b);
  bool IsSystemCancel(void) { return(mSystemCancel); }
  void SetCanSetSystemCancel(bool b) { mCanSetSystemCancel = b; }  
  void RecoverSleep(void) { mSleepRecover = true; }

  // Demo Type:
  void SetDemoType(int type) { mDemoType = type; }  
  bool IsDemoType(int type) { return(mDemoType==type); }
  int GetDemoType(void) { return(mDemoType); }

  // Playback Parameters:
  void SetPlaybackParameter(int type,int param0=0,int param1=0);
  int GetPlaybackParameter(int type);
  
  void ResetAllDemoType(void); // Set all DemoType to FALSE

  // System Sync:
  void SetSystemSync(bool b);
  bool IsSystemSync(void) { return(mSystemSync); }
  void SystemSync(void);
  void AddSystemSync(int frame);

  // Key Wait:
  void ProcessKeyWait(void);
  void SetKeyWait(int type,bool exitOnFrame,int exitFrameWait) { mKeyWaitType=type; mExitOnFrameWait=exitOnFrame; mFrameWait=exitFrameWait; }
  void SetWindowCursor(bool visible);

  void SetForceOneStep(bool flg){mForceOneStep=flg;}
  // TextWindow
  void SetTextWindowStyle(int windowStyle);
  void DisplayTextWindowMessage(int messageID,int endFrame,int windowStyle);
  void TextWindowMessageSetting(int speed,bool buttonSpeedUp);
  void SetUseCommonTextWindow(bool flg){mUseCommonTextWindow = flg;}
  void SetTextWindowArcID(u32 n,bool useFieldArc=false) { mTextWindowArcID=n; mUseFieldMessageArc=useFieldArc; }
 
  // Motion Blur:
  System::MotionBlur *GetMotionBlur(void);  
  void SetMotionBlurEndFrame(int frame) { mBlurEndFrame = frame; }

  //Bloom
  void StartBloom(void *texBuf);
  void EndBloom(void);

  // System CallBack:  
  void SetSystemCallBack(SequenceCallBack *callBack) { mSysCallBack = callBack; }
  void RemoveSystemCallBack(void);
  void ExecuteCallBack(int command,int paramA,int paramB);

  // Command Skip:
  void SetCommandSkipOnCancel(int executeType,int frame,int wait,int type);
  void SetCommandSkipOnSync(int executeType,int frame,int wait,int type);
  void CommandSkip(int tFrame);
  bool IsAfterCommandSkip(int frame);

  // Pipe Line
  void SetUseOption( const SequenceRenderingPipeLine::UseOption& opt) { mUseOption = opt; }

  // Multi-Language Layout:
  int GetLanguageLayoutGARC(void);
  int GetLanguageType(void);

  // Startup Wait Mode: (Wait until start playing Sequence)
  void SetStartupWaitMode(bool b) { mStartupWaitMode=b; ResetStartTick(); }
  void SetRealTimeMode(bool b) { mRealTimeMode=b; }
  bool IsRealTimeMode(void) { return(mRealTimeMode && !mForceOneStep); }

  // Use HIO & Debug Info
#if PM_DEBUG
  bool UseHIO(void) { return(mUseHIO); }
  void SetUseHIO(bool b) { mUseHIO=b; }
  void ShowDebugInfo(bool b) { mShowDebugInfo = b; }
  bool IsTestMode(void) { return(mIsTestMode); }
  void SetSkipMilliSec(int ms){mSkipMilliSec = ms;}
  int  GetSkipMilliSec(void){return mSkipMilliSec;}
#endif

private:
  void SetLoadedSequenceFile(void) { mLoadedSequenceFile = true; }
  
	// State:
	u32 mStateReg,mStateRegSub;
	int mSequenceSeq;
	u64 tickStart;
	int targetFrame,lastFrame;
  bool mLoadedSequenceFile;
  bool mIsTestMode;
	
  // BGM Sync:
  BGMSyncSetting bgmSyncSettings[MAX_BGM_SYNC_SETTINGS];
  int numBGMSyncSettings;
  bool mSyncBGM;  
  long bgmPreloadPos;
  int bgmWaitIdx;


  void *seqFileListTable;

  // Common TextWindow Layout Buffer:
  void *mLayoutCommonWindowBuffer;
	

	// Set by Initialize()
	gfl2::fs::AsyncFileManager *mAsyncFileManager;
	gfl2::heap::HeapBase *mMemHeap;
	gfl2::heap::HeapBase *mDevHeap;
	gfl2::heap::NwAllocator *mMemAllocator;
	gfl2::heap::NwAllocator *mDevAllocator;	
	::app::util::Heap *mAppHeap;
	Player *mSeqPlayer;
	SequenceRenderSystem *mRenderSys;
  System::nijiAllocator *mResidentGLAllocator; 

	//Object & Manager:
	DemoLib::Obj::ObjectManager *mObjectManager;
	DemoLib::Obj::TaskSystem    *mTaskSystem;
	DemoLib::Obj::ResourceManager *mResourceManager;
  GfmdlSeqObjectManager* mModelObjManager;
  CharaSeqObjectManager* mCharaObjManager;
	PokeSeqObjectManager* mPokeObjManager;
  LayoutSeqObjectManager* mLayoutObjManager;
  LightSeqObjectManager* mLightObjManager;
  EffectSeqObjectManager* mEffectObjManager;

  // Effect:
  gfl2::Effect::GFHeap  *mEffectHeap;
  gfl2::Effect::System  *mEffectSystem;
  gfl2::util::List<u32> *mEffectGroupIDList[3];     ///< グループIDリスト
  DemoLib::Obj::EffectIndexManager *mParticleIndexManager;
  bool mUseEffectSystem;

  // Pokemon:
  PokeTool::PokeModelSystem *mPokeModelSys;
  SequenceObject_Poke *mPokemonObj[DEMOSEQUENCE_MAX_POKEMON];
  int mPokemonIndex[DEMOSEQUENCE_MAX_POKEMON];
  bool mPokemonIndexUsed[DEMOSEQUENCE_MAX_POKEMON];
  pml::pokepara::PokemonParam *mPokemonParam[DEMOSEQUENCE_MAX_POKEMON];
  bool mUsePokemonSystem;

  // CharaModel:
  int mFieldModelIndex[DEMOSEQUENCE_MAX_FIELDMODEL];
  int mBattleModelIndex[DEMOSEQUENCE_MAX_BATTLEMODEL];  

  int mFieldModelAttachItemIndex[DEMOSEQUENCE_MAX_FIELDMODEL];
  int mBattleModelAttachItemIndex[DEMOSEQUENCE_MAX_BATTLEMODEL];  

  int mFieldDressUpParamIndex[DEMOSEQUENCE_MAX_FIELDMODEL];
  int mBattleDressUpParamIndex[DEMOSEQUENCE_MAX_FIELDMODEL];
  
  poke_3d::model::DressUpParam mDressUpParam[DEMOSEQUENCE_MAX_MODELDRESSUP];    
  
  // CharaModel:
  poke_3d::model::CharaModelFactory *mCharaFactoryField;
  poke_3d::model::CharaModelFactory *mCharaFactoryBattle;
  poke_3d::model::DressUpModelResourceManager *mDressUpModelResourceManagerField[DEMOSEQUENCE_MAX_MODELDRESSUP];
  poke_3d::model::DressUpModelResourceManager *mDressUpModelResourceManagerBattle[DEMOSEQUENCE_MAX_MODELDRESSUP];
  bool mUseCharaModel;

	// Camera:
	bool  mIsUpdateCamera;
	int cameraCurFrame,mCameraStartAniFrame;
	gfl2::math::Vector3 mCameraPos;
  gfl2::math::Vector3 mCameraTrg;
  gfl2::math::Vector3 mCameraOffset;
	f32 mCameraFov;
	poke_3d::model::BaseCamera                *m_BaseCamera;
	gfl2::renderingengine::scenegraph::resource::ResourceNode*       m_pAnimationResourceRootNode;                 ///< アニメーションリソース
	gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
	gfl2::renderingengine::scenegraph::instance::DrawEnvNode*        m_pDrawEnvNode;	
	gfl2::renderingengine::scenegraph::resource::ResourceNode*       m_pDrawEnvResourceRootNode;                   ///< DrawEnvリソース
	bool m_useCameraAnimation;
	
  // SkyBox:
  bool mUseSkybox;
  System::Skybox::Skybox *mSkybox;
  int mSkyBoxOffFrame;

  //Bloom
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mBloomAlphaMask;


  // Sound:
  SequenceObject_Sound *mSoundObject[SEQ_EFF_SOUND_NUM];
  
  // Playback options/parameters
  int playerSexSetting,timezoneSetting;
  SeqPokemonDebugSetting pokemonDebugSettings[MAX_DEBUGSEQ_POKEMON_SETTINGS];
  bool mPausePlayback;
  bool mForceOneStepPause;
  u64 mPauseStartTick,mSyncStartTick;
  bool mStopPlayback;  
  bool mFastForwardPlayback;
  bool mSystemCancel,mCanSetSystemCancel;
  bool mSystemSyncTrigger;
  bool mSystemSyncSleepTrigger;

  int mDemoType;
  int mVSDemoType;

  // System Sync:
  bool mSystemSync;
  u32 mSyncFrame;
  int mSyncFrames[SEQ_MAX_SYSTEMSYNC];
  int mNumSystemSync;
  int mCurSyncIndex;

  // KeyWait
  int mKeyWaitType;
  bool mExitOnFrameWait;
  int mFrameWait;

  //フレームスキップ無し
  bool mForceOneStep;

  // RealTime Off
  bool mRealTimeMode;

  // Sleep Recover:
  bool mSleepRecover;
  
  // Common Text Window
  static const int MSG_WIN_NUM = 6;
  App::Tool::TalkWindow *mMessageWindow[MSG_WIN_NUM];
  bool mUseTextWindowFlg[MSG_WIN_NUM];
  int messageWindowEndFrame;
  bool mUseCommonTextWindow;
  print::WordSet* mWordset;  
  u32 mTextWindowArcID;
  bool mUseFieldMessageArc;
  int mTextWindowType;

  gfl2::str::MsgData *mCommonMsgData;
  gfl2::str::MsgData *mTrainerMsgData;

  // Motion Blur:  
  int mBlurEndFrame;

  // Texture Replace:
  gfl2::renderingengine::scenegraph::DagNode *mReplaceTexture;

  // System Callback:
  SequenceCallBack *mSysCallBack;

  // Command Skip:
  int mCommandSkipFrame;
  bool mCommandSkipOnCancel;
  bool mCommandSkipOnSync;
  int mCommandSkipWait;
  int mCommandTransType;
  bool mCommandSkipCountdown;
  int lastCommandSkipFrame;

  // Wait Mode:
  bool mStartupWaitMode;

  // VS Battle:
  int mVSBattlePose[DEMOSEQUENCE_MAX_MODELDRESSUP];
  int mVSBattlePoseFile[DEMOSEQUENCE_MAX_MODELDRESSUP];
  Savedata::MyStatus *mVSBattleStatus[DEMOSEQUENCE_MAX_MODELDRESSUP];
  u8 mVSBattleMonsterBallStatus[DEMOSEQUENCE_MAX_MODELDRESSUP][VSBATTLE_MONSBALLSTATUS_NUM];

  SequenceRenderingPipeLine::UseOption    mUseOption;

  // Monster Ball:
  int monsterBallType[SEQ_MONSTERBALL_NUM];

  // InFrame Camera:
  System::Camera::CModelViewerInFrame *mInFrameSys[SEQ_NUM_INFRAME_CAMERA];
  System::Camera::InFrameCamera *mInFrameCamera[SEQ_NUM_INFRAME_CAMERA];
  int inFrameSizeX[SEQ_NUM_INFRAME_CAMERA];
  int inFrameSizeY[SEQ_NUM_INFRAME_CAMERA];
  int inFramePosX[SEQ_NUM_INFRAME_CAMERA];
  int inFramePosY[SEQ_NUM_INFRAME_CAMERA];
  SequenceObject_Gfmdl *inFrameFollowMdl[SEQ_NUM_INFRAME_CAMERA];
  static const int NODE_STR_LEN = 32;  
  char inFrameFollowNodeName[SEQ_NUM_INFRAME_CAMERA][NODE_STR_LEN];
  bool mUseInframe;
  bool mSetInframe[SEQ_NUM_INFRAME_CAMERA];
  bool mUseInframeFollowRotate[SEQ_NUM_INFRAME_CAMERA];
  float mBaseInframeRotX[SEQ_NUM_INFRAME_CAMERA];
  float mBaseInframeRotY[SEQ_NUM_INFRAME_CAMERA];

  // Util:
  void Inframe_ConvertWorldPointToScreenPoint(int index,f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point);

  // Event Listener:
  SequenceSystemEventListener *mSequenceEventListener;

  // Time:
  GameSys::AlolaTime mAlolatime;

  //momijiから追加
#if PM_DEBUG  //影響が怖いので、一応括る
  bool mIsSkipFinish; //終了スキップがされた
#endif //PM_DEBUG

#if PM_DEBUG
  bool mUseHIO;
  bool mShowDebugInfo;
  int mSkipMilliSec;
#endif

  //↓GFNMCat[5756]GFNMCat[5761] BGMの再生ポジション巻き戻り対策
private:
  int mStreamSoundSamplePositionLast; //前回のサンプルポジションを覚えておく
  int GetStreamSoundPlaySamplePosition(void); //ストリーミングの再生中サンプルポジションを返す

  //↑GFNMCat[5756]GFNMCat[5761] BGMの再生ポジション巻き戻り対策
};


class SequenceSystemEventListener : public System::SystemEventListener
{
public:

  SequenceSystemEventListener(SequenceViewSystem *core) { mCore = core; }
  virtual ~SequenceSystemEventListener(void) {}

  /**
  * @brief スリープ遷移時イベント（スリープ禁止の場合は呼ばれない）
  */
  //------------------------------------------------------------------
  virtual void OnSuspendWithSleep( void ){ mCore->TogglePausePlayback(false); }

  //------------------------------------------------------------------
  /**
  * @brief スリープからの復帰時イベント（スリープ禁止の場合は呼ばれない）
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithSleep( void ){ mCore->RecoverSleep(); }

    //------------------------------------------------------------------
  /**
  * @brief ホームメニュー遷移時イベント
  */
  //------------------------------------------------------------------
  virtual void OnSuspendWithHomeMenu( void ){ mCore->TogglePausePlayback(false); }

  //------------------------------------------------------------------
  /**
  * @brief ホームメニューからの復帰時イベント
  */
  //------------------------------------------------------------------
  virtual void OnResumeWithHomeMenu( void ){ mCore->RecoverSleep(); }

  void Start(void) { GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);}
  void Stop(void) { GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);}

  SequenceViewSystem *mCore;
};


	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_PLAYER_CORE_H__
