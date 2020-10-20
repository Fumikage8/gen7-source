//======================================================================
/**
 * @file  btlv_RenderPipeLine.h
 * @brief バトル描画 RenderingPipeLine
 * @author  ariizumi
 * @data  15/03/07
 */
//======================================================================
#ifndef __BTLV_RENDER_PIPELINE_H__
#define __BTLV_RENDER_PIPELINE_H__
#pragma once

#include <Math/include/gfl2_math.h>
#include <Macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"

#include "System/include/RenderPath/ModelRenderPath.h"
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include "System/include/RenderPath/FullPowerEffect.h"

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

GFL_NAMESPACE_BEGIN( btl )

class BattleViewSystem;
class BtlvObject_PokeModel;

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

class BattleRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine , public DemoLib::Obj::ObjectRenderPipeLine
{
public:

  static const int RENDER_OPT_SKYBOX = 100;
  static const int RENDER_OPT_BGFILL = 99;
  static const int RENDER_OPT_EDGE_MASK = 98;
  static const int RENDER_OPT_FLOOR_MASK = 97;
  static const int RENDER_OPT_SHADOW_MODEL = 96;

  
  BattleRenderingPipeLine( BattleViewSystem *btlvCore ,f32 screanWidth, f32 screanHeight );
	virtual ~BattleRenderingPipeLine();

  void Initialize(void);
  void Terminate(void);
  void Setup(void *edgeRes,void *outLineRes,void *FullPowerRes);
  void Update(const gfl2::math::Matrix34 &viewMtx);

  //----------------------------------------------------------------------------
	/**
	*  @brief  最初のパスから描画できる状態にする
	*/
	//-----------------------------------------------------------------------------
	virtual b32 StartRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  現状のパスにあったシーンを返す
	*  @return シーン
	*/
	//-----------------------------------------------------------------------------
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  次のパスへ遷移
	*  @return trueなら次のパス、falseなら描画終了
	*/
	//-----------------------------------------------------------------------------
	virtual b32 NextRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  レイアウトのレンダーパスを取得する
	*/
	//-----------------------------------------------------------------------------
	gfl2::lyt::LayoutRenderPath* GetLayoutRenderPath( void ) { return mLayoutRenderPath; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  エフェクトのレンダーパスを取得する
	*/
	//-----------------------------------------------------------------------------
  gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ) { return mEffectRenderPath; }
  gfl2::Effect::EffectRenderPath* GetEffectLowRenderPath( void ) { return mEffectLowRenderPath; }
  gfl2::Effect::EffectRenderPath* GetEffectHighRenderPath( void ) { return mEffectHighRenderPath; }
	gfl2::Effect::EffectRenderPath* GetEffectBeforeRenderPath( void ) { return mEffectBeforeRenderPath; }
	gfl2::Effect::EffectRenderPath* GetScreenEffectRenderPath( void ) { return mScreenEffectRenderPath; }

  System::ModelRenderPath* GetShadowRenderPath(void)const{return mShadowRenderPath;}


	//----------------------------------------------------------------------------
	/**
	*  @brief  シーンパスにライト等の環境ノードを追加
	*  @param  pNode 環境ノード
	*/
	//-----------------------------------------------------------------------------
  void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );

  void SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view);

  void SetBillboardViewMatrix( const gfl2::math::Matrix34 &mtx ){mBillboardViewMatrix = mtx;}

  //全力シェーダー用
  virtual void AddPoke_FullPowerShader( BtlvObject_PokeModel *pokeModel , bool isZenryoku);
  virtual void RemovePoke_FullPowerShader( BtlvObject_PokeModel *pokeModel );
  virtual void DeletePoke_FullPowerShader( BtlvObject_PokeModel *pokeModel );
  bool CheckDeleteFPModel(void){return (mFPDeleteReserve==NULL?true:false);}
  
  //エッジだけ制御
  void AddEdge_Poke( PokeTool::PokeModel *pokeModel );
  void RemoveEdge_Poke( PokeTool::PokeModel *pokeModel );
  void AddEdge_Chara( poke_3d::model::CharaModel *charaModel );
  void RemoveEdge_Chara( poke_3d::model::CharaModel *charaModel );
  void AddEdge_DressUpChara( poke_3d::model::DressUpModel *dressModel );
  void RemoveEdge_DressUpChara( poke_3d::model::DressUpModel *dressModel );
  
  //ObjectRenderPipeLine系
  //モデルを追加
  virtual void AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );
  virtual void RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt );
  //ポケモデルを追加
  virtual void AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  virtual void RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt );
  //キャラモデルを追加
  virtual void AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  virtual void RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt );
  virtual void AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );
  virtual void RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt );

  //モデルのビルボードの更新にViewMatrixが必要。
  virtual const gfl2::math::Matrix34* GetViewMatrix(void){return &mBillboardViewMatrix;}
#if USE_AURA_SHADER
  FullPowerEffectRenderPath0* GetFPRenderPath_Rim(void){return mFullPowerEffectRenderPath_Rim;}
#endif

  //描画を２回に分ける対応
  void SetIsDraw(bool flg){mIsDraw = flg;}
  void SetIsAfterDraw(bool flg){mIsAfterDraw = flg;}

  //デバッグ用パス取得
  System::ModelRenderPath* GetModelRenderPath(void){return mModelRenderPath;}
private:
  BattleViewSystem *mBtlvCore;

  int                                     mRenderPathCnt;

  //スクリーン貼り付け用カメラ
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mScreenCamera;
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mBgFillCamera;
  gfl2::Effect::EffectRenderPath::Camera mEffectScreenCamera;
  //正射影カメラ
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera mOrthoCamera;


  System::NijiEdgeMapSceneRenderPath      *mEdgeMapPath;
  System::NijiOutLinePostSceneRenderPath  *mOutLinePath;
  gfl2::lyt::LayoutRenderPath             *mLayoutRenderPath;
  gfl2::Effect::EffectRenderPath          *mEffectLowRenderPath;       //通常用
  gfl2::Effect::EffectRenderPath          *mEffectRenderPath;       //通常用
  gfl2::Effect::EffectRenderPath          *mEffectHighRenderPath;       //通常用
  gfl2::Effect::EffectRenderPath          *mEffectBeforeRenderPath;       //通常用
  EffectScreenRenderPath          *mScreenEffectRenderPath; //画面貼り付け用
  System::ModelRenderPath *mSkyRenderPath;
  System::ModelRenderPath *mModelRenderPath;
  System::ModelRenderPath *mModelBeforeRenderPath;
  System::ModelRenderPath *mModelAfterRenderPath;
  System::ModelRenderPath *mModelBallRenderPath;
  System::ModelRenderPath *mModelBgRenderPath;
  System::ModelRenderPath *mModelBgAfterRenderPath;
  System::ModelRenderPath *mFloorMaskPath;
  System::ModelRenderPath *mShadowRenderPath;
  ModelScreenRenderPath *mModelOrthoRenderPath; //正射影用
  ModelScreenRenderPath *mModelAfterOrthoRenderPath; //正射影用
  ModelScreenRenderPath *mScreenModelRenderPath;
  ModelScreenRenderPath *mScreenModelAfterLayoutRenderPath;
  ModelScreenRenderPath *mScreenModelBeforeRenderPath;
  ModelScreenRenderPath *mFillRenderPath;
  //System::FillRenderPath  *mFillRenderPath;

  //ゆがみシェーダー用
  //技エフェクト用
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath *mStretchBltFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                *mDistortionPostRenderPath;
  int mDistModelCnt;  //処理を食うので、歪みシェーダーがあるときしかPathを呼ばないため
  //メガヤミラミ用
  gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath *mStretchBltPokeFrameBufferPath;
  poke_3d::renderer::DistortionPostRenderPath                *mDistortionPokePostRenderPath;
  int mDistPokeModelCnt;  //処理を食うので、歪みシェーダーがあるときしかPathを呼ばないため

  //エッジ用
  gfl2::gfx::Texture* mDepthStencilTexture;


  //全力シェーダー
#if USE_AURA_SHADER
  FullPowerEffectRenderPath0	*mFullPowerEffectRenderPath_Rim;
  FullPowerEffectRenderPath1	*mFullPowerEffectRenderPath_Aura;
#endif //USE_AURA_SHADER

  //描画を２回に分ける対応
  bool mIsDraw;
  bool mIsAfterDraw;

  //6末仮
  BtlvObject_PokeModel *mFPDeleteReserve;
  int mFPDeleteCnt;

  //計算タイミングとかセットタイミングを制御しやすいように輪分ける
  gfl2::math::Matrix34 mBillboardViewMatrix;
};

GFL_NAMESPACE_END(btl)

#endif //__BTLV_RENDER_PIPELINE_H__