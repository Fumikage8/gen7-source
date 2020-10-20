//======================================================================
/**
 * @file	KawaigariRenderSystem.h
 * @brief	Kawaigari Render System中心機能
 * @author	PETE
 * @data	2015.10.13
 */
//======================================================================
#if !defined( __KAWAIGARI_RENDER_SYSTEM_H__ )
#define __KAWAIGARI_RENDER_SYSTEM_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <Macro/include/gfl2_Macros.h>

#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include "System/include/nijiAllocator.h"

#include <gfx/include/gfl2_GFGL.h>
#include <ui/include/gfl2_UI.h>

#include "KawaigariRenderPipeline.h"


GFL_NAMESPACE_BEGIN( gfl2 )
	GFL_NAMESPACE_BEGIN( renderingengine )
		GFL_NAMESPACE_BEGIN( scenegraph )
			GFL_NAMESPACE_BEGIN( resource )
				class ResourceNode;
			GFL_NAMESPACE_END( resource )
	
			GFL_NAMESPACE_BEGIN( instance )
				class ModelInstanceNode;
				class DrawEnvNode;
				class LightSetNode;
				class LightNode;
			GFL_NAMESPACE_END( instance )
		
		GFL_NAMESPACE_END( scenegraph )
	GFL_NAMESPACE_END( renderingengine )
	
	GFL_NAMESPACE_BEGIN( gfx )
		class IGLAllocator;
	GFL_NAMESPACE_END( gfx )
	
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
	GFL_NAMESPACE_BEGIN( util )
		class AppRenderingManager;
	GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(App)
	GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariRenderingPipeLine;

class KawaigariMain;

class KawaigariRenderSystem
{
	GFL_FORBID_COPY_AND_ASSIGN(KawaigariRenderSystem);

public:
  // ２画面モンスターカメラ
  enum {
    NOT_DUAL_SCREEN_MONSTER_CAMERA,
    DUAL_SCREEN_MONSTER_CAMERA,
    DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER,
    DUAL_SCREEN_MONSTER_CAMERA_UPPER,
    DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER,
  };

	KawaigariRenderSystem(KawaigariMain *mainSys);	
	~KawaigariRenderSystem(void);

	void Initialize(void);
	void Terminate(void);

	//初期化
	bool Setup(void);
	//終了
	bool Cleanup(void);

	//更新
	void Update(void);
	//描画
	void PreDraw(void);
	void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  KawaigariRenderingPipeLine* GetRenderingPipeLine(void){return mRenderingPipeLine;}

	::app::util::AppRenderingManager* GetAppRenderingManager(void){return mAppRenderingManager;}

	gfl2::math::Matrix34* GetCamViewMtx(void){return &mCamViewMtx;}
	void SetCamViewMtx(const gfl2::math::Matrix34 &mtx){mCamViewMtx = mtx;}

	void SetCamPrjMtx(const gfl2::math::Matrix44 &mtx){mCamProjMtx = mtx;}

  void SetCamProjMtx(f32 fovy, f32 aspect,f32 nearVal,f32 farVal);

	gfl2::gfx::IGLAllocator* GetGLAllocator(void){return mAllocator;}

  void SetAdjustScale(f32 scale) { mAdjustScale = scale; }  

  void GetViewAndProjectionMatrix(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj) { proj = mProjMtxBottom; view=mViewMtxBottom; }
  void GetViewAndProjectionMatrixTop(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj) { proj = mProjMtxTop; view=mViewMtxTop; }

  void GetMameViewAndProjectionMatrix(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj) { proj = mProjMtxBottom; view=mEatViewMtx[0]; }

  void GetEffectViewAndProjectionMatrix(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj,int index=0) 
  { 
    if(index==0) proj = mProjMtxBottom; 
    else proj = mProjMtxTop; 
    view=mEffectViewMtx[index]; 
  }

  u8 GetDualScreenMonsterCameraMode() const
  {
    return m_DualScreenMonsterCameraMode;
  }

  void SetDualScreenMonsterCameraMode(u8 mode,bool force)
  {
    if(force) m_DualScreenMonsterCameraMode = mode;
    else
    {
      u8 cur = m_DualScreenMonsterCameraMode;
      if( cur && ( mode == DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER        
          && cur != DUAL_SCREEN_MONSTER_CAMERA ) ||
          ( mode == DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER          
          && cur != DUAL_SCREEN_MONSTER_CAMERA_UPPER ) ){
            m_DualScreenMonsterCameraMode = mode;
      }
    }
  }

  void CalculateEatCameraViewProjMatrix(void);
  void CalculateFixedSizeCameraViewProjMatrix(void);
  void CalculateCurrentCameraMatrix(bool force=false);

private:
	void SetupEnv(void);
	
	gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
	gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
	gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
	gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  gfl2::math::Matrix44 mProjMtxBottom;   // 透視変換行列
  gfl2::math::Matrix34 mViewMtxBottom;   // ビュー変換行列

  gfl2::math::Matrix44 mProjMtxTop;   // 透視変換行列
  gfl2::math::Matrix34 mViewMtxTop;   // ビュー変換行列

  gfl2::math::Matrix44 mCamSkyProjMtx;   // 透視変換行列(天球用にNear・Farが違う)
  gfl2::math::Matrix44 mSkyProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mSkyViewMtx[2];   // ビュー変換行列

  gfl2::math::Matrix44 mCamFarProjMtx;   // 透視変換行列(天球用にNear・Farが違う)
  gfl2::math::Matrix44 mFarProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mFarViewMtx[2];   // ビュー変換行列

  gfl2::math::Matrix44 mFinalProjMtx[2];   // FINAL 透視変換行列
  gfl2::math::Matrix34 mFinalViewMtx[2];   // FINAL ビュー変換行列
  gfl2::math::Matrix44 mFinalSkyProjMtx[2];   // FINAL 透視変換行列
  gfl2::math::Matrix44 mFinalFarProjMtx[2];   // FINAL 透視変換行列

  gfl2::math::Matrix44 mEatProjMtx[2];   // EAT 透視変換行列
  gfl2::math::Matrix34 mEatViewMtx[2];   // EAT ビュー変換行列

  gfl2::math::Matrix44 mEffectProjMtx[2];   // EFFECT 透視変換行列
  gfl2::math::Matrix34 mEffectViewMtx[2];   // EFFECT ビュー変換行列

	gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*m_pDrawEnvNode;
	gfl2::gfx::Texture			*mDepthStencilTexture;

	int mSetupSeq;
	bool mIsFinishSetup;
	KawaigariMain *mCore;		
	System::nijiAllocator   *mAllocator;
	
	KawaigariRenderingPipeLine *mRenderingPipeLine;
	::app::util::AppRenderingManager* mAppRenderingManager;

	void *mEdgeMapRenderPathRes;
	void *mOutLineRenderPathRes;

  f32 mAdjustScale;

  gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
  gfl2::renderingengine::scenegraph::instance::LightNode			*m_pDirectionalLightNode;
  gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;
  
  //sangoから持ってきた
  u8 m_DualScreenMonsterCameraMode;
  u8 m_DualScreenMonsterCameraFrame;

  
};

	GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif //__KAWAIGARI_RENDER_SYSTEM_H__
