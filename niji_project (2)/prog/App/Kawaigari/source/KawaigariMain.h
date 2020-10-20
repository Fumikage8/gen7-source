//======================================================================
/**
 * @file KawaigariMain.h
 * @date 2015/09/04 12:11:22
 * @author ariizumi_nobuhiko -> Pete
 * @brief かわいがりメイン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __KAWAIGARI_MAIN_H_INCLUDED__
#define __KAWAIGARI_MAIN_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_UI.h>
#include <fs/include/gfl2_Fs.h>
#include <AppLib/include/Util/app_util_heap.h>
#include "System/include/nijiAllocator.h"


//#include "DemoLib/SequencePlayer/include/SequenceRenderSystem.h"
//#include "DemoLib/SequencePlayer/include/SequenceLocalDef.h"
//#include "DemoLib/SequencePlayer/include/SequenceObject.h"

#include "PokeTool/include/PokeModel.h"

#include <DemoLib/Object/include/Object_Poke.h>

#include <DemoLib/Object/include/ObjectManager.h>
#include <DemoLib/Object/include/TaskSystem.h>
#include <DemoLib/Object/include/ResourceManager.h>

#include <pml/include/pml_PokePara.h>

#include <App/Kawaigari/include/KawaigariProc.h>
#include <App/Kawaigari/source/KawaigariCamera.h>
#include <App/Kawaigari/source/KawaigariPokemon.h>

#include <Debug/include/gfl2_DebugWinItemTypes.h>       // For Select menu debug
//#include <Debug/DebugWin/include/DebugWinSystem.h>

enum
{
  KAWAIGARI_EFFECT_CLEANSUCCESS = 0,
  KAWAIGARI_EFFECT_SMOKE,  
  KAWAIGARI_EFFECT_HEART_TOUCH,
  KAWAIGARI_EFFECT_DIRT,
  KAWAIGARI_EFFECT_DUST,
  KAWAIGARI_EFFECT_HAIR,
  KAWAIGARI_EFFECT_WATER,
  KAWAIGARI_EFFECT_DUST_EFFECT,
  KAWAIGARI_EFFECT_MUSICNOTE,
  KAWAIGARI_EFFECT_AFFECTIONSUCCESS,
  KAWAIGARI_EFFECT_QUESTIONMARK,
  KAWAIGARI_EFFECT_CLEANSPOT,
  KAWAIGARI_EFFECT_CARETOOLFADE,
  KAWAIGARI_EFFECT_FIRE,
  KAWAIGARI_EFFECT_ICE,
  KAWAIGARI_EFFECT_PARALYSIS,
  KAWAIGARI_EFFECT_POISON,
  KAWAIGARI_EFFECT_SLEEPY,
  KAWAIGARI_EFFECT_HIGHFIVE,
  KAWAIGARI_EFFECT_BALLCAPTURE,
  KAWAIGARI_EFFECT_BALLIN,
  KAWAIGARI_EFFECT_EXCLAMATIONMARK,
  KAWAIGARI_EFFECT_BALLSHADOW,
  KAWAIGARI_EFFECT_TYPE_MAX,
};

#include <App/Kawaigari/source/KawaigariTroubleObject.h>
#include <App/Kawaigari/source/KawaigariDirtObject.h>
#include <App/Kawaigari/source/KawaigariDustObject.h>
#include <App/Kawaigari/source/KawaigariHairObject.h>
#include <App/Kawaigari/source/KawaigariWaterObject.h>
#include <App/Kawaigari/source/KawaigariSickStatusObject.h>
#include <App/Kawaigari/source/KawaigariTroubleManager.h>

#include <App/Kawaigari/source/KawaigariObject.h>
#include <App/Kawaigari/source/KawaigariRandomActionSelector.h>
#include <App/Kawaigari/source/KawaigariDataManager.h>
#include <App/Kawaigari/source/KawaigariUIManager.h>
#include <App/Kawaigari/source/data/KawaigariMameDataManager.h>

// Option Core Param Test:
#include <PokeTool/include/KawaigariParamOptionCoreManager.h>

#if PM_DEBUG  
#include <App/Kawaigari/source/debug/KawaigariDebugFont.h>
#include <App/Kawaigari/source/debug/KawaigariDebugManager.h>
#endif

GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)
GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kawaigari)

struct KawaigariProcParam;
class KawaigariRenderSystem;

const int MAX_KAWAIGARI_EFFECT = 20;
const int MAX_KAWAIGARI_CARE_EFFECT = 20;

// TouchPositionType
enum
{  
  TPT_LOOK=0,
  TPT_FOOD,
  TPT_PARTICLE,
  TPT_PARTICLE_FAR,
  TPT_PARTICLE_TOUCH,
  TPT_MAMESHAKE,
  TPT_MAME,
  TPT_MAMELOOK,
};

// Camera Type:
enum {
  CAM_TYPE_LOWER,
  CAM_TYPE_UPPER
};


class KawaigariMain
{
public:
  KawaigariMain();
  ~KawaigariMain(){}

  //初期化
  bool Initialize(gfl2::heap::HeapBase *heap , KawaigariProcParam *initParam);
  //終了
  bool Finalize(void);
  //更新
  bool Update(void);
  //描画
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  void StartReloadPokemon(int monsNo,int formNo,int sexNo) { mReloadPokemon=true; mReloadNo=monsNo; mReloadFormNo=formNo; mReloadSexNo=sexNo; }
  void ReloadPokemon(void);
#endif
#endif

  void PreloadEffectResources(void);
  #if defined(GF_PLATFORM_WIN32)
  void PreloadEffectResources_Win32(void);
  #else
  void PreloadEffectResources_CTR(void);
  #endif

  // Mode:
  void SetMode(int m) { mMode = m; }
  bool IsMode(int m) { return(mMode==m); }
  int Mode(void) { return(mMode); }
  bool IsDebugMode(void);
  void SetFinished(void) { mFinished = true;}

  // System Accessors:
  gfl2::heap::HeapBase* GetHeap(void){return mHeap;}
  System::nijiAllocator* GetGLAllocator(void){return mGlAllocator;}  
  gfl2::heap::NwAllocator* GetAllocator(void){return mAllocator;}  
  ::app::util::Heap* GetAppHeap(void){return mAppHeap;}

  gfl2::fs::AsyncFileManager* GetAsyncFileManager(void){return mAsyncFileManager;}
  const gfl2::ui::DeviceManager* GetUIDevice(void){ return mDeviceManager; }
  
  DemoLib::Obj::ResourceManager* GetResourceManager(void){return mResourceManager;}
  gfl2::Effect::GFHeap* GetEffectHeap(void){return mEffectHeap;}
  DemoLib::Obj::EffectIndexManager* GetParticleIndexManager(void){return mParticleIndexManager;}
  gfl2::Effect::System* GetEffectSystem(void){return mEffectSystem;}


  PokeTool::PokeModelSystem* GetPokeModelSys(void){return mPokeModelSys;}
  btl::BgSystem* GetBgSystem(void){return mBgSystem;}
  btl::BgSystem* GetBgSystemNear(void){return mBgSystemNear;}

  // Camera:
  KawaigariCamera *GetCamera(void);

  // DataManager:
  KawaigariDataManager *GetDataManager(void);

  // RandomActionSelector:
  KawaigariRandomActionSelector *GetRandomActionSelector(void);

  // UI Manager:
  KawaigariUIManager *GetUIManager(void);

  // PokemonAI
  KawaigariPokemon *GetPokemonAI(void);

  // PokeModel
  PokeTool::PokeModel *GetPokeModel(void) { return(mModel); }

  // RenderSystem:
  KawaigariRenderSystem *GetRenderSystem(void) { return(mRenderSys); }

  // Load Debug Pokemon:
  void LoadPokemon(void);

  // Random Number:
  int GetRandom(int max);

  // Parameter MonsNo:
  int GetParameterMonsterNumber(void);

  // Monster Ball
  int GetMonsterBallType(void);
  KawaigariObject_Gfmdl *GetMonsterBall(void)  { return(mMonsterBallModel); }

  // Effect:
  void RequestCreateEffect(int type) { mCreateEffectRequest = type; }
  void CreateEffect(int type,gfl2::math::Vector3 *pos=NULL,int drawType=0,gfl2::math::Vector3 *rot=NULL,bool playSE=true);
  void UpdateSleepEffectPos(int effectID);
  int CreateCareEffect(int type,gfl2::math::Vector3 *pos=NULL,int drawType=0);
  KawaigariObject_Effect *GetCareEffect(int n) { return(mCareEffects[n]); }
  void KillCareEffect(int n) { mKillCareEffect[n] = true; }

  // TroubleManager (Care Mode)
  KawaigariTroubleManager *GetTroubleManager(void) { return(&mTroubleManager); }
  void CleanPokemonPart(int index);
  void CurePokemon(void);
  bool IsIceStatus(void);

  // Util:
  void ConvertWorldPointToScreenPoint(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point);
  void ConvertWorldPointToScreenPoint_Mame(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point);
  void ConvertWorldPointToScreenPointTop(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point);
  
  void Get3DTouchPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy, const s32 type );
  void Get3DLookPositionByXY(gfl2::math::Vector3 *pos, u32 touchx, u32 touchy); // NIJI *NEW*

  void Get3DEffectTouchPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy);
  void Get3DEffectTouchPositionByXY_Top( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy);

  void Get3DEffectPositionByXY( gfl2::math::Vector3 *pos, u32 touchx, const u32 touchy,float zPos = 100.0f);
  float GetCalculatedEffectScale( gfl2::math::Vector3 pos,bool isStatusEffect=false );
  int ConvertCareModeJointToPokeJoint(int joint,int &index);

  // Mame/Food
  KawaigariMameDataManager *GetMameManager(void) { return(&mameManager); }
  void SetMameType(int n) { mameType = n; }
  int GetMameType(void) { return(mameType); }
  void ShowFoodObject(int n,bool visible);
  void ShowFoodPieceObject(int n,bool visible);
  gfl2::math::Vector3 *GetFoodPosition(void) { return(&mFoodPosition); }

  void UpdateFoodPositionLook(void);
  gfl2::math::Vector3 *GetFoodPositionLook(void);

  gfl2::math::Vector3 *GetFoodRotation(void) { return(&mFoodRotation); }
  gfl2::math::Vector3 *GetFoodScale(void) { return(&mFoodScale); }
  gfl2::math::Vector3 *GetFoodPiecePosition(int n) { return(&mFoodPiecePosition[n]); }
  gfl2::math::Vector3 *GetFoodPieceRotation(int n) { return(&mFoodPieceRotation[n]); }
  void LoadFoodObject(int n);
  void LoadFoodResource(int n);
  bool WaitFoodResource(void);
  bool UnloadFoodObjects(void);

#if PM_DEBUG
  gfl2::util::DrawUtilText *TextUtil(void) { return(debugFont.TextUtil()); }
  void SetIsDebugMode(bool b) { mIsDebugMode = b; }
#endif

private:
  KawaigariProcParam *mInitParam;

  // Mode:
  int mMode;
  bool mFinished;
  
  //コアなシステムとか
  gfl2::heap::HeapBase *mHeap;  
  ::app::util::Heap *mAppHeap;
  gfl2::heap::NwAllocator *mAllocator;
  System::nijiAllocator   *mGlAllocator;
  gfl2::ui::DeviceManager* mDeviceManager;
  gfl2::fs::AsyncFileManager *mAsyncFileManager;
  DemoLib::Obj::ResourceManager *mResourceManager;
  
  // Effect:
  gfl2::Effect::GFHeap  *mEffectHeap;
  gfl2::Effect::System  *mEffectSystem;
  gfl2::util::List<u32> *mEffectGroupIDList[3];     ///< グループIDリスト
  DemoLib::Obj::EffectIndexManager *mParticleIndexManager;

  // Pokemon Actor AI Object:
  KawaigariPokemon pokemonAI;

  // Random Action Selector:
  KawaigariRandomActionSelector actionSelector;

  // Camera System:
  KawaigariCamera cameraSystem;

  // DataManager:
  KawaigariDataManager dataManager;

  // Render System:
  KawaigariRenderSystem *mRenderSys;

  // UI Manager:
  KawaigariUIManager *uiManager;

  //各種システム
  PokeTool::PokeModelSystem *mPokeModelSys;
  btl::BgSystem *mBgSystem;
  btl::BgSystem *mBgSystemNear;

  // PokeModel:
  PokeTool::PokeModel *mModel;

  // MonsterBall:
  void LoadMonsterBall(void);
  bool UnloadMonsterBall(void);
  KawaigariObject_Gfmdl *mMonsterBallModel;

  // Effects
  bool IsMultipleHead(void);
  void RemoveDeadEffects(void);
  void LoadEffects(void);
  bool UnloadEffects(void);
  KawaigariObject_Effect *mEffects[MAX_KAWAIGARI_EFFECT];
  KawaigariObject_Effect *mCareEffects[MAX_KAWAIGARI_CARE_EFFECT];
  bool mKillCareEffect[MAX_KAWAIGARI_CARE_EFFECT];
  bool mUpdateEffectPos[MAX_KAWAIGARI_CARE_EFFECT];

  // Poke Mame:
  bool LoadPokeMame(void);
  void CreatePokeMame(void);
  bool UnloadPokeMame(void);
  

  KawaigariObject_Gfmdl *mPokeMameModel[MAX_POKEMAME_MODEL];
  KawaigariObject_Gfmdl *mPokeMamePieceModel[MAX_POKEMAME_PIECE_MODEL];
  gfl2::math::Vector3 mFoodPosition,mFoodRotation,mFoodScale,mFoodPositionLook;
  gfl2::math::Vector3 mFoodPiecePosition[MAX_POKEMAME_PIECE_MODEL];
  gfl2::math::Vector3 mFoodPieceRotation[MAX_POKEMAME_PIECE_MODEL];
  bool pokeMameLoaded[MAX_POKEMAME_MODEL];
  int mameType;
  KawaigariMameDataManager mameManager;

  int mInitSeq;
  int mPokeMameLoadSeq;
  int mExitSeq;
  bool mPause;
  int mIdx;
  int mCreateEffectRequest;
  u8 mStartFriendship;

  // Temp:
  bool mForceLoadPikachuu;  

  // Trouble Manager (Care Objects)
  KawaigariTroubleManager mTroubleManager;

  PokeTool::KawaigariParamOptionCoreManager *mOptionCoreParamManager;

  // DEBUG:
#if PM_DEBUG
  KawaigariDebugFont debugFont;
  bool mIsDebugMode;
#endif

#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  KawaigariDebugManager debugManager;
  bool mReloadPokemon;
  int mReloadSeq;
  int mReloadNo,mReloadFormNo;
  int mReloadSexNo;
#endif
#endif

#if PM_DEBUG
  // ------------------------------------------
  // Select menu debug functions
  // ------------------------------------------
 public:
  void PlayAllAnimation(void);
  void SetPokeMameMax(void);
  void SetBadStatus(void);
  void SetDirt(void);
  void SetDirtAll(void);

  void SetKawaigariParam(void);
  void GetKawaigariParam(void);


 private:
  void InitSelectMenuDebug( gfl2::heap::HeapBase *pHeap );
  void FinalizeSelectMenuDebug(void);

  // For debug call back
  static void SelectMenuFookFunc_PlayAllAnimation( void* userWork, gfl2::debug::DebugWinItem *item );
  static void SelectMenuFookFunc_MameMax( void* userWork, gfl2::debug::DebugWinItem *item );
  static void SelectMenuFookFunc_SetKawaigariParam( void* userWork  , gfl2::debug::DebugWinItem *item );
  static void SelectMenuFookFunc_GetKawaigariParam( void* userWork  , gfl2::debug::DebugWinItem *item );
  static void SelectMenuFookFunc_SetBadStatus( void* userWork  , gfl2::debug::DebugWinItem *item );

  static void SelectMenuFookFunc_SetDirt( void* userWork  , gfl2::debug::DebugWinItem *item );
  static void SelectMenuFookFunc_SetDirtAll( void* userWork  , gfl2::debug::DebugWinItem *item );

  u8   mDebug_Nakayosi;
  u8   mDebug_Manpuku;
  u8   mDebug_Nadenade;

  int   mDebug_BadStatus;

  int   mDebug_DirtType;
  int   mDebug_DirtPos;

  gfl2::debug::DebugWinGroup *mpKawaigariTop;

#endif // PM_DEBUG



};
GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_MAIN_H_INCLUDED__
