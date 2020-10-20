//======================================================================
/**
 * @file	BgSystem.h
 * @brief	戦闘背景システム
 * @author	ariizumi
 * @data	15/04/10
 */
//======================================================================
#if !defined( __BG_SYSTEJM_H__ )
#define __BG_SYSTEJM_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <Heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <ro/include/gfl2_RoManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

#include <animation/include/gfl2_DrawEnvAnimationController.h>

#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/Skybox/Skybox.h"

#include <Battle/Background/include/BgSkybox.h>


GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(util)
    class GLHeapAllocator;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(btl)

class BgObject;
class BgSkybox;

class BgSystem
{
public:
  //天球と遠景のfar値の定義(バトルとかわいがりで使う
  static const f32 SKY_FAR_PLANE;
  static const f32 FAR_FAR_PLANE;
  
  //外部から指定される天候
  typedef enum
  {
    BG_WEATHER_NORMAL,
    BG_WEATHER_RAIN,
    BG_WEATHER_SUNLIGHT,
    BG_WEATHER_CLOUD,
    BG_WEATHER_WIND,
    BG_WEATHER_SNOW,
    BG_WEATHER_MAX
  } BG_WEATHER_TYPE;

  //外部から指定される背景種別
  typedef enum
  {
    OBJ_GROUND,
    OBJ_NEAR,
    OBJ_FAR,
    OBJ_BUILD,  //頂点アニメ

    OBJ_MAX
  }BG_OBJ_TYPE;

public:  //bgobj用
  typedef enum
  {
    ANM_LOOP,
    ANM_RAIN_IN,
    ANM_RAIN_LOOP,
    ANM_RAIN_OUT,
    ANM_SUNLIGHT_IN,
    ANM_SUNLIGHT_LOOP,
    ANM_SUNLIGHT_OUT,
    ANM_CLOUD_IN,
    ANM_CLOUD_LOOP,
    ANM_CLOUD_OUT,
    ANM_WIND_IN,
    ANM_WIND_LOOP,
    ANM_WIND_OUT,
    ANM_SNOW_IN,
    ANM_SNOW_LOOP,
    ANM_SNOW_OUT,
    ANM_1DAY,
    ANM_MORNING_IN,
    ANM_MORNING_LOOP,
    ANM_MORNING_OUT,
    ANM_NOON_IN,
    ANM_NOON_LOOP,
    ANM_NOON_OUT,
    ANM_EVENING_IN,
    ANM_EVENING_LOOP,
    ANM_EVENING_OUT,
    ANM_NIGHT_IN,
    ANM_NIGHT_LOOP,
    ANM_NIGHT_OUT,

    ANM_MAX
  }BG_ANM_TYPE;


//  typedef enum
//  {
//    PROC_ORDER_SKYBOX_BG,
//    PROC_ORDER_BG_SKYBOX
//  }BG_SKYBOX_PROC_ORDER;

public:

  struct INIT_PARAM
  {
    gfl2::heap::HeapBase *heap;
    gfl2::heap::HeapBase *workHeap;
    gfl2::fs::AsyncFileManager* fileManager;
#if PM_DEBUG
    bool useHio;
#endif

    INIT_PARAM()
      :heap(NULL)
      ,workHeap(NULL)
      ,fileManager(NULL)
#if PM_DEBUG
      ,useHio(false)
#endif
    {}
  };

  struct SETUP_PARAM
  {
    //@todo 定数使う
    int groundType;
    int nearType;
    int farType;

    System::Skybox::Skybox::SkyType skyType;

    bool useSkyBox;
    SETUP_PARAM()
      :groundType(1)
      ,nearType(1)
      ,farType(1)
      ,skyType(System::Skybox::Skybox::SKY_TYPE_SUNNY)
      ,useSkyBox(true)
    {}
  };

  //コンストラクタ
  GFL_DLL_EXPORT BgSystem();
  //デストラクタ
  GFL_DLL_EXPORT ~BgSystem();

  //システム初期化・開放
  GFL_DLL_EXPORT void Initialize(const INIT_PARAM &initParam, s32 hour, s32 minute, s32 second );
  GFL_DLL_EXPORT bool IsFinishInitialize(void);
  GFL_DLL_EXPORT void Terminate(void);
  GFL_DLL_EXPORT bool IsFinishTerminate(void);
  GFL_DLL_EXPORT void SetSkyMtx(const gfl2::math::Matrix44 &Proj, const gfl2::math::Matrix34 &View);
  GFL_DLL_EXPORT void SetFarMtx(const gfl2::math::Matrix44 &Proj, const gfl2::math::Matrix34 &View);
  GFL_DLL_EXPORT bool InitializeSkyBox(gfl2::heap::HeapBase *pDevHeap);
  GFL_DLL_EXPORT bool IsEnalbe(void){return mIsEnable;}

  //読み込み
  GFL_DLL_EXPORT void StartLoad(const SETUP_PARAM &setupParam);
  GFL_DLL_EXPORT void LoadUpdate(void);
  GFL_DLL_EXPORT bool IsFinishLoad(void);
  //開放

  //読み込み後にモデル作成
  GFL_DLL_EXPORT void Create(void);
  //破棄
  GFL_DLL_EXPORT void Delete(void);
  GFL_DLL_EXPORT bool IsFinishDelete(void);
  
  GFL_DLL_EXPORT void TimeUpdate(s32 hour, s32 minute, s32 second, BgObject* pObject);
  GFL_DLL_EXPORT void Update(s32 hour, s32 minute, s32 second);

  GFL_DLL_EXPORT void SetVisible(bool flg);
  GFL_DLL_EXPORT bool IsVisible(void){return mIsVisible;}

  GFL_DLL_EXPORT void PlayAnime(int anmType);
  GFL_DLL_EXPORT void StopAnime(int anmType);

  GFL_DLL_EXPORT bool IsAnimeEnd(int anmType);

  GFL_DLL_EXPORT void TimeInOutControl(s32 hour, s32 minute, s32 second);

  GFL_DLL_EXPORT void RequestAnime(int anmType);

  GFL_DLL_EXPORT void WeatherControl(void);

  //バトルでロイヤル背景をまわす対応
  GFL_DLL_EXPORT void RotateY_Deg(f32 rot);

  //地形レンダーパス取得
  System::ModelRenderPath* GetRenderPath(void){return mRenderPath;}
  System::ModelRenderPath* GetFarRenderPath(void){return NULL;}
  System::ModelRenderPath* GetSkyRenderPath(void){return mSkyRenderPath;}

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetFarRenderCamera(void){return &mFarCamera;}

  f32 CalculateTimeToAllTimeAnimationFrame( s32 hour, s32 minute, s32 second);

  GFL_DLL_EXPORT int GetNowTimeAnimLoop(s32 hour, s32 minute, s32 second);

  //天球取得
  System::Skybox::Skybox* GetSkyBox(void){return mpSkyBox;}

  // アニメーションフレームを設定
  GFL_DLL_EXPORT void SetAnimationFrame(BG_OBJ_TYPE objType,int anmType,u32 frame);
  // アニメーションフレームを取得
  GFL_DLL_EXPORT int GetAnimationFrame(BG_OBJ_TYPE objType,int anmType);

  // 最終フレームを取得
  GFL_DLL_EXPORT int GetLastAnimationFrame(BG_OBJ_TYPE objType,int anmType);

  // 再生速度を取得
  GFL_DLL_EXPORT int GetAnimationStepFrame(BG_OBJ_TYPE objType,int anmType);

  // 現在の時間アニメを取得
  GFL_DLL_EXPORT int GetNowTimeAnimation(){ return mNowAnimType; }

	// Vramを使用するかを設定する。
	GFL_DLL_EXPORT void SetUseVram(bool flag) { mIsUseVram = flag; }
	// Vramを使用しているかを確認する。
	GFL_DLL_EXPORT bool IsUseVram(void)const { return mIsUseVram; }

  // アニメーションの更新のオンオフを切り替える
  GFL_DLL_EXPORT void SetAnimationUpdateFrameEnable(bool isUpdateAnimation);

private:
  INIT_PARAM mInitParam;
  SETUP_PARAM mSetupParam;
  gfl2::util::GLHeapAllocator *mGlAllocator;

//  int mAnimType;  //現在再生中のmAnimType

  b8 mIsEnable;
	b8 mIsUseVram;
	u8 pad[2];
  bool mIsVisible;
  int  mDeleteSeq;

  BgObject *mObject[OBJ_MAX];

  System::ModelRenderPath *mRenderPath;
  System::ModelRenderPath *mSkyRenderPath;

  System::Skybox::Skybox* mpSkyBox;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*mEnvNode;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*mSkyEnvNode;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mFarCamera;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mSkyCamera;

  //ライト関係
  void *mLightBuf;
  gfl2::fs::BinLinkerAccessor mLightBinData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mEnvResNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mEnvAnmNode;
  gfl2::animation::DrawEnvAnimationController mEnvAnmController;

  int m_SkyInitSeq;
  int mNowAnimType;

  int mWeatherAnimRequest;
  int mWeatherAnimNow;
//  int mWeatherAnim;

  bool mIsSkyboxSelfCreate;
  bool mIsInitializeSkyBox;

  void InitRenderPath(gfl2::heap::HeapBase *tHeap);
  void BgSystem_Update();
};

GFL_NAMESPACE_END(btl)


#endif //__BG_SYSTEJM_H__