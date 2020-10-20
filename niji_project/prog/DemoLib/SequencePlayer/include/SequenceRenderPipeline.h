//======================================================================
/**
 * @file  SequenceRenderPipeLine.h
 * @brief Sequence Rendering PipeLine
 * @author  pete
 * @data  15/05/08
 */
//======================================================================
#ifndef __SEQUENCE_RENDER_PIPELINE_H__
#define __SEQUENCE_RENDER_PIPELINE_H__
#pragma once

#include <Macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"

#include "System/include/RenderPath/ModelRenderPath.h"
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

// Motion Blur:
#include "System/include/MotionBlur.h"

// Bloom:
#include <renderer/include/gfl2_BloomRenderPath.h>

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
  GFL_NAMESPACE_BEGIN( scenegraph )
  GFL_NAMESPACE_BEGIN( resource )
class ResourceNode;
GFL_NAMESPACE_END( resource )
  GFL_NAMESPACE_BEGIN( instance )
class ModelInstanceNode;
GFL_NAMESPACE_END( instance )
  GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_BEGIN( renderer )
  GFL_NAMESPACE_BEGIN( util )
class StretchBltFrameBufferPath;
GFL_NAMESPACE_END( util )
  GFL_NAMESPACE_END( renderer )
  GFL_NAMESPACE_END( renderingengine )
  GFL_NAMESPACE_END( gfl2 )

  GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( renderer )
class DistortionPostRenderPath;
GFL_NAMESPACE_END( renderer )
  GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

class SequenceViewSystem;

class gfl2::gfx::Surface;

//画面貼り付け用継承
class ModelScreenRenderPath : public System::ModelRenderPath
{
public:
  ModelScreenRenderPath(gfl2::gfx::IGLAllocator *allocator,int mdlNum):System::ModelRenderPath(allocator,mdlNum){};
  virtual ~ModelScreenRenderPath(){}

  virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(){ return mCameraParam; }
  void SetCameraParam(gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera *cam){mCameraParam = cam;}
private:
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera *mCameraParam;
};

//画面貼り付け用継承
class EffectScreenRenderPath : public gfl2::Effect::EffectRenderPath
{
public:
  EffectScreenRenderPath():gfl2::Effect::EffectRenderPath(){}
  virtual ~EffectScreenRenderPath(){}

  virtual const gfl2::Effect::EffectRenderPath::Camera* CameraOverride(){ return mCameraParam; }
  void SetCameraParam(gfl2::Effect::EffectRenderPath::Camera *cam){mCameraParam = cam;}
private:
  gfl2::Effect::EffectRenderPath::Camera *mCameraParam;
};

class ModelRenderPath_NoDepthClear : public System::ModelRenderPath
{
public:
  ModelRenderPath_NoDepthClear(gfl2::gfx::IGLAllocator *allocator,int mdlNum):System::ModelRenderPath(allocator,mdlNum){};
  virtual ~ModelRenderPath_NoDepthClear(){}  
  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil ) {}   
};

class SequenceRenderingPipeLine :public gfl2::renderingengine::renderer::RenderingPipeLine , public DemoLib::Obj::ObjectRenderPipeLine
{
public:

  struct UseOption
  {
    UseOption()
      : isUseModelRenderPathAfterEdge(true)
      , isUseSkyRenderPath(true)
      , isUseEdgeMapPath(true)
      , isUseModelOutLinePath(true)
      , isUseLayoutRenderPath(true)
      , isUseScreenEffectRenderPath(true)
      , isUseBloomRenderPath(true) // シークエンス見て自動計算
      , isUseInframePath(true)
      , isUseInframe1(false) // シークエンス見て自動計算　(未使用だったら、ＦＡＬＳＥ　使用だったらＴＲＵＥ）
      , isUseInframe2(false) // シークエンス見て自動計算
      , isUseMotionBlur(false) // シークエンス見て自動計算
      , isUseWarpModel(false) // シークエンス見て自動計算
    {}

    bool isUseModelRenderPathAfterEdge;
    bool isUseSkyRenderPath;    
    bool isUseEdgeMapPath;
    bool isUseModelOutLinePath;
    bool isUseLayoutRenderPath;
    bool isUseScreenEffectRenderPath;
    bool isUseBloomRenderPath; // シークエンス見て自動計算
    bool isUseInframePath;    
    bool isUseInframe1; // シークエンス見て自動計算
    bool isUseInframe2; // シークエンス見て自動計算   
    bool isUseMotionBlur; // シークエンス見て自動計算   
    bool isUseWarpModel; // シークエンス見て自動計算   
  };

public:
	SequenceRenderingPipeLine( SequenceViewSystem *seqCore ,f32 screenWidth, f32 screenHeight );
	virtual ~SequenceRenderingPipeLine();

	void Initialize(void);
	void Terminate(void);
	void Setup(void *edgeRes,void *outLineRes);

	virtual b32 StartRenderPath(void);

	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	virtual b32 NextRenderPath(void);

	// GFMDL:
	virtual void AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );
	virtual void RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );

  // Layout:
	virtual gfl2::lyt::LayoutRenderPath* GetLayoutPath( void ) { return mLayoutRenderPath; }

  //Pokemon Model:
  virtual void AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  virtual void RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt );

  // CharaModel:
  virtual void AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  virtual void RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  virtual void AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );
  virtual void RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );

  // Light:  
  virtual void AddObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt );
  virtual void RemoveObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt );

  // Skybox:
  virtual void AddObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode  );
  virtual void RemoveObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode  );

  // Bloom:
  void UpdateBloom(void);
  void EnableBloom(void);
  void DisableBloom(void);
  void SetBloomParameters(const gfl2::math::Vector4 weightRGB,const f32 intensity,const f32 range,const f32 strength);
  void SetBloomMaskTexture(const gfl2::gfx::Texture *tex);
  bool IsEnabledBloom(void);

  // Fog:
  void EnableFog(void);
  void DisableFog(void);
  void UpdateFog(void);
  void SetFogParameters(const gfl2::math::Vector4 colorRGB,const f32 near,const f32 far,const f32 strength);

	// Effect:
	gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ) { return mEffectRenderPath; }  
  gfl2::Effect::EffectRenderPath* GetEffectBeforeModelRenderPath( void ) { return mEffectBeforeModelRenderPath; }
  gfl2::Effect::EffectRenderPath* GetScreenEffectRenderPath( void ) { return mScreenEffectRenderPath; }
  void SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

  // Motion Blur:
  System::MotionBlur *GetMotionBlurPath(void) { return(mMotionBlurPath); }

  // Screen Camera:
  void SetBillboardViewMatrix( const gfl2::math::Matrix34 &mtx ){mBillboardViewMatrix = mtx;}

  //モデルのビルボードの更新にViewMatrixが必要。
  virtual const gfl2::math::Matrix34* GetViewMatrix(void){return &mBillboardViewMatrix;}

	void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
  virtual gfl2::renderingengine::scenegraph::instance::DrawEnvNode *GetDrawEnvNode(int opt);

  void AddDrawEnv2( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
  
  void AddDrawEnvField( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
  
  void SetUseOption( const UseOption& opt) { mUseOption = opt; }

  // Inframe:
  void SetInframeCamera(int index, const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);
  void SetBaseCamera(const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);


  void GetViewAndProjectionMatrix(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj) { proj = baseCam.m_Projection; view=baseCam.m_View; }
  void GetViewAndProjectionMatrixPivot(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj);
  
private:
  SequenceViewSystem *mSeqCore;
  u32 mRenderPathCnt;

  //スクリーン貼り付け用カメラ
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mScreenCamera;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mBgFillCamera;
  gfl2::Effect::EffectRenderPath::Camera mEffectScreenCamera;

  System::NijiEdgeMapSceneRenderPath *mEdgeMapPath,*mEdgeMapInframePath[2];
  System::NijiOutLinePostSceneRenderPath *mOutLinePath,*mOutLineInframePath[2];
  gfl2::lyt::LayoutRenderPath *mLayoutRenderPath;
  gfl2::Effect::EffectRenderPath *mEffectRenderPath,*mEffectBeforeModelRenderPath;
  System::ModelRenderPath *mSkyRenderPath,*mModelRenderBeforePath,*mModelRenderPath,*mModelFieldRenderPath;
  ModelRenderPath_NoDepthClear *mModelRenderPath2;
  System::ModelRenderPath *mModelRenderPathAfterEdge,*mModelRenderPathAfterEdge_Inframe1;
  EffectScreenRenderPath *mScreenEffectRenderPath; //画面貼り付け用
  ModelScreenRenderPath *mScreenModelRenderPath;
  ModelScreenRenderPath *mScreenBeforeModelRenderPath;
  ModelScreenRenderPath *mModelInframeRenderPath[2];
  System::MotionBlur *mMotionBlurPath;
  poke_3d::renderer::BloomRenderPath mBloomRenderPath;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pEnvNodeBase;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pEnvNodeBase2;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pEnvNodeBaseField;

  //計算タイミングとかセットタイミングを制御しやすいように輪分ける
  gfl2::math::Matrix34 mBillboardViewMatrix;

  gfl2::gfx::Texture* m_pDepthStencilTexture;
  gfl2::gfx::Texture* m_pDepthStencilInframeTexture[2];

  UseOption     mUseOption;
  bool mUseInframe1,mUseInframe2;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera inFrameCam[2];
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera baseCam;


  //ゆがみシェーダー用
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath *mStretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                *mDistortionPostRenderPath;
  int mDistModelCnt;  //処理を食うので、歪みシェーダーがあるときしかPathを呼ばないため

};

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

#endif //__SEQUENCE_RENDER_PIPELINE_H__
