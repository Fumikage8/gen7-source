//======================================================================
/**
 * @file  KawaigariRenderPipeLine.h
 * @brief Kawaigari Rendering PipeLine
 * @author  pete
 * @data  15/10/13
 * 
 *  2016.4.13    ishiguro  SetViewProjectionDirect, SetSkyMtxDirect, SetFarMtxDirect, MinimalUpdate関数追加
 * 
 */
//======================================================================
#ifndef __KAWAIGARI_RENDER_PIPELINE_H__
#define __KAWAIGARI_RENDER_PIPELINE_H__
#pragma once

#include <Macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"

#include "System/include/RenderPath/ModelRenderPath.h"
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include "System/include/RenderPath/UtilRenderPath.h"




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
  GFL_NAMESPACE_END( renderingengine )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN(App)
	GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
class KawaigariEdgeIDRenderPath;

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

class KawaigariRenderingPipeLine :public gfl2::renderingengine::renderer::RenderingPipeLine , public DemoLib::Obj::ObjectRenderPipeLine
{
  
public:
	KawaigariRenderingPipeLine( KawaigariMain *core ,f32 screenWidth, f32 screenHeight );
	virtual ~KawaigariRenderingPipeLine();

  // TouchID:
  void Dump(void);
  int GetTouchColor(void);
  void SetUseTouchIDTexture(bool b) { mIsUseTouchID = b; }

	void Initialize(void);
	void Terminate(void);
	void Setup(void *edgeRes,void *outLineRes);

	virtual b32 StartRenderPath(void);

	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	virtual b32 NextRenderPath(void);

	
  // Layout:
	virtual gfl2::lyt::LayoutRenderPath* GetLayoutPath( void ) { return mLayoutRenderPath; }

  // GFMDL:
  virtual void AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );
  virtual void RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );

  //Pokemon Model:
  virtual void AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  virtual void RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  
  // Light:  
  virtual void AddObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt  );
  virtual void RemoveObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt  );

	// Effect:
	gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ) { return mEffectRenderPath; }  
  gfl2::Effect::EffectRenderPath* GetEffectFixedSizeRenderPath( void ) { return mEffectFixedSizeRenderPath; }  
  gfl2::Effect::EffectRenderPath* GetScreenEffectRenderPath( void ) { return mScreenEffectRenderPath; }
  void SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);


  // Screen Camera:
  void SetBillboardViewMatrix( const gfl2::math::Matrix34 &mtx ){mBillboardViewMatrix = mtx;}

  //モデルのビルボードの更新にViewMatrixが必要。
  virtual const gfl2::math::Matrix34* GetViewMatrix(void){return &mBillboardViewMatrix;}

	void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );
  virtual gfl2::renderingengine::scenegraph::instance::DrawEnvNode *GetDrawEnvNode(int opt) { return(pEnvNodeBase); }

  void SetIsBottomScreen(bool b) { mIsBottomScreenRender=b; }

  // --------------------------------------------------------------------------
  /**
    * @brief  モデル描画パスへのViewProjectionMatrix書き換え関数
    * @param  projection       プロジェクションマトリックス
    * @param  view             ビューマトリックス
    */
  // --------------------------------------------------------------------------
  void SetViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

    // --------------------------------------------------------------------------
  /**
    * @brief  MAMEモデル描画パスへのViewProjectionMatrix書き換え関数
    * @param  projection       プロジェクションマトリックス
    * @param  view             ビューマトリックス
    */
  // --------------------------------------------------------------------------
  void SetMameViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

      // --------------------------------------------------------------------------
  /**
    * @brief  MAMEモデル描画パスへのViewProjectionMatrix書き換え関数
    * @param  projection       プロジェクションマトリックス
    * @param  view             ビューマトリックス
    */
  // --------------------------------------------------------------------------
  void SetEffectViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

  // --------------------------------------------------------------------------
  /**
    * @brief  天球描画パスへのViewProjectionMatrix書き換え関数(Near, Farが他のパスと異なるため別途用意)
    * @param  projection       プロジェクションマトリックス
    * @param  view             ビューマトリックス
    */
  // --------------------------------------------------------------------------
  void SetSkyMtxDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

  // --------------------------------------------------------------------------
  /**
    * @brief  遠景描画パスへのViewProjectionMatrix書き換え関数(Near, Farが他のパスと異なるため別途用意)
    * @param  projection       プロジェクションマトリックス
    * @param  view             ビューマトリックス
    */
  // --------------------------------------------------------------------------
  void SetFarMtxDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

  // --------------------------------------------------------------------------
  /**
    * @brief  特定パスのアップデート（最小限のアップデート）
    */
  // --------------------------------------------------------------------------
  void MinimalUpdate();

  // --------------------------------------------------------------------------
  /**
    * @brief  ＩＤモデル描画オフスクリーンバッファコピー先切り替え
    */
  // --------------------------------------------------------------------------
  void SwapIDCopyBuffer();

  void SetBaseViewProjMatrix( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view) { mBaseProjMtx = projection; mBaseViewMtx = view; }

private:
  KawaigariMain *mCore;
  u32 mRenderPathCnt;

  //スクリーン貼り付け用カメラ
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mScreenCamera;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mBgFillCamera;
  gfl2::Effect::EffectRenderPath::Camera mEffectScreenCamera;


  System::NijiEdgeMapSceneRenderPath *mEdgeMapPath;
  System::NijiEdgeMapSceneRenderPath *mMameEdgeMapPath;
  
  System::NijiOutLinePostSceneRenderPath *mOutLinePath;
  System::NijiOutLinePostSceneRenderPath *mMameOutLinePath;

  gfl2::lyt::LayoutRenderPath *mLayoutRenderPath;
  
  gfl2::Effect::EffectRenderPath *mEffectRenderPath;
  gfl2::Effect::EffectRenderPath *mEffectFixedSizeRenderPath;

  System::ModelRenderPath *mSkyRenderPath;
  System::ModelRenderPath *mModelRenderPath,*mModelRenderPathShadow,*mMameModelRenderPath;
  KawaigariEdgeIDRenderPath *mIDTextureRenderPath;
  System::ModelRenderPath *mModelRenderPathAfterEdge;
  EffectScreenRenderPath *mScreenEffectRenderPath; //画面貼り付け用
  ModelScreenRenderPath *mScreenModelRenderPath;
  System::ClearDepthRenderPath *mClearDepthPath;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pEnvNodeBase;

  //計算タイミングとかセットタイミングを制御しやすいように輪分ける
  gfl2::math::Matrix34 mBillboardViewMatrix;

  u32 mDBCnt;                 //! ダブルバッファカウンタ
  u32 *mOffscreenBuffer[2];   //! コピー先ダブルバッファ

  gfl2::gfx::Texture* mDepthStencilTexture;
  bool mIsBottomScreenRender;
  bool mIsUseTouchID;

  gfl2::math::Matrix44 mEatProjMtx;   // EAT 透視変換行列
  gfl2::math::Matrix34 mEatViewMtx;   // EAT ビュー変換行列

  gfl2::math::Matrix44 mBaseProjMtx;   // BASE 透視変換行列
  gfl2::math::Matrix34 mBaseViewMtx;   // BASE ビュー変換行列

  gfl2::math::Matrix44 mEffectProjMtx;   // EFFECT 透視変換行列
  gfl2::math::Matrix34 mEffectViewMtx;   // EFFECT ビュー変換行列
  
};

	GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif //__KAWAIGARI_RENDER_PIPELINE_H__
