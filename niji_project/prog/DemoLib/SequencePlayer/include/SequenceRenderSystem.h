//======================================================================
/**
 * @file	SequenceRenderSystem.h
 * @brief	Sequence Render System中心機能
 * @author	PETE
 * @data	2015.05.08
 */
//======================================================================
#if !defined( __SEQUENCE_RENDER_SYSTEM_H__ )
#define __SEQUENCE_RENDER_SYSTEM_H__
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

#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"

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

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

// Temporary:
class SequenceRenderingPipeLine;

class SequenceViewSystem;

class SequenceRenderSystem
{
	GFL_FORBID_COPY_AND_ASSIGN(SequenceRenderSystem);

public:
	SequenceRenderSystem(SequenceViewSystem *seqViewSys);	
	~SequenceRenderSystem(void);

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

	SequenceRenderingPipeLine* GetRenderingPipeLine(void){return mRenderingPipeLine;}

	::app::util::AppRenderingManager* GetAppRenderingManager(void){return mAppRenderingManager;}

	gfl2::math::Matrix34* GetCamViewMtx(void){return &mCamViewMtx;}
	void SetCamViewMtx(const gfl2::math::Matrix34 &mtx){mCamViewMtx = mtx;}

	void SetCamPrjMtx(const gfl2::math::Matrix44 &mtx){mCamProjMtx = mtx;}
  gfl2::math::Matrix44* GetCamProjMtx(void){return &mCamProjMtx;}

	gfl2::gfx::IGLAllocator* GetGLAllocator(void){return mAllocator;}

  void SetUseOption( const SequenceRenderingPipeLine::UseOption& opt) { mUseOption = opt; }


private:
	
	void SetupEnv(void);
	
	gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
	gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
	gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
	gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*m_pDrawEnvNode2;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*m_pDrawEnvNodeField;
	gfl2::gfx::Texture			*mDepthStencilTexture;

	int mSetupSeq;
	bool mIsFinishSetup;
	SequenceViewSystem *mSeqCore;		
	System::nijiAllocator   *mAllocator;
	    
	SequenceRenderingPipeLine *mRenderingPipeLine;
	::app::util::AppRenderingManager* mAppRenderingManager;

	void *mEdgeMapRenderPathRes;
	void *mOutLineRenderPathRes;

  SequenceRenderingPipeLine::UseOption  mUseOption;
};

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_RENDER_SYSTEM_H__
