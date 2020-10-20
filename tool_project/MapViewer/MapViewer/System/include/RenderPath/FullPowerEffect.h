#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include "System/include/RenderPath/CtrGfbDummy.h"

#if USE_AURA_SHADER

//===================================================================================================================================================
//
//  全力技描画パス0【リムエフェクト】
//
//===================================================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------------------------------
//  リムエフェクトShaderDriver
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRimShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
{
public:
	FullPowerEffectRimShaderDriver();
	virtual ~FullPowerEffectRimShaderDriver();

	virtual b32 Apply
    ( gfl2::renderingengine::renderer::RenderState& rRenderState,const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const;

  CtrGfbvshDummy* m_vsh0;
  CtrGfbvshDummy* m_vsh1;
  CtrGfbfshDummy* m_fsh;
  void SetFshStatus( void );

  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  void *m_pMdlBin;
  void *m_pTexBin;

  // 固有定数バッファ
	struct CBuffer
	{
    u32 timer;  // タイマー
	};
	CBuffer m_CBuffer;
  gfl2::math::Vector4 m_Color;

  void SetCBuffer( CBuffer* pData );
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  RenderPath
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRenderPath0 : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	FullPowerEffectRenderPath0();
	virtual ~FullPowerEffectRenderPath0();

	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  void SetRimColor( gfl2::math::Vector4 &col );
  const gfl2::math::Vector4& GetRimColor(void)const{return m_FullPowerEffectRimShaderDriver.m_Color;}
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
  FullPowerEffectRimShaderDriver m_FullPowerEffectRimShaderDriver;

  u32 m_timer;
};


//===================================================================================================================================================
//
//  全力技描画パス1【オーラエフェクト】
//
//===================================================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------------------------------
//  オーラエフェクト専用モデル管理クラス
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectAuraMdlManager
{
public:
  FullPowerEffectAuraMdlManager();
  ~FullPowerEffectAuraMdlManager();

  // Face = Meshをマテリアルの違いを受けて分割したもの
  // Mesh = Objectをジョイント数の制限を受けて分割したもの
  // という構造において
  // DrawableNodeが所持するDrawTagと関連付けられた差し替え用データ構造体
  // 各種コマンドに関してはあまり頻繁に異なるサイズのAllocを回避することを意図して
  // (大きな振れ幅もないはずなので)ここでは固定の長さとする
  struct OverwriteTag
	{
    u32*  comBufferEnable;
    u32   comBufferEnableSize;

    u32*  comBufferDraw;
    u32   comBufferDrawSize;

    u32*  comBufferDisable;
    u32   comBufferDisableSize;
    
    u32   numVtx;
    u8*   vtxBuffer;
    u8*   idxBuffer;

    gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag; // 関連付けされたMeshDrawTag
	};

  // DrawableNodeと関連付けられた構造体
  struct EffectMdl
	{
    OverwriteTag* arrayTag;
    u32           numTag;

    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;  // このノードを描画時に差し替える
    bool          isEnable;
	};

  void CreateEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void DeleteEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  OverwriteTag* GetOverwriteTag( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag );

private:
  #define EFFECT_RESIST_MAX ( 8 ) // とりあえず8枠まで
  EffectMdl m_effectMdl[ EFFECT_RESIST_MAX ];

  void _initEffectMdl( EffectMdl* pEffectMdl );
  void _cleanupEffectMdl( EffectMdl* pEffectMdl );

  b32 _cleateBufferData( OverwriteTag* pTag );
  b32 _cleateCommandData( OverwriteTag* pTag );

  u32 _packCommand( u32* pCommandStart, u32* pCommandEnd );
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  オーラエフェクトShaderDriver
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectAuraShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
{
public:
	FullPowerEffectAuraShaderDriver();
	virtual ~FullPowerEffectAuraShaderDriver();

	virtual b32 Apply
    ( gfl2::renderingengine::renderer::RenderState& rRenderState,const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const;

  virtual void SetTextureOverride
    ( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

  virtual b32 SetPolygonDrawOverride
    ( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

  CtrGfbvshDummy* m_vsh0;
  CtrGfbvshDummy* m_vsh1;
  CtrGfbfshDummy* m_fsh;
  void SetFshStatus( void );

	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  void *m_pMdlBin;
  void *m_pTexBin;

	// 固有定数バッファ
	struct CBuffer
	{
		nn::math::MTX34 mtxView;  // ビュー逆行列
    u32             timer;    // タイマー
	};
	CBuffer m_CBuffer;
  FullPowerEffectAuraMdlManager* m_auraMdlManager;

  void SetCBuffer( CBuffer* pData, FullPowerEffectAuraMdlManager* ptr );
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  RenderPath
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRenderPath1 : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	FullPowerEffectRenderPath1();
	virtual ~FullPowerEffectRenderPath1();

	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void DeleteAuraModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  void SetYBillboardMtx( gfl2::math::Matrix34* rotYMatInv );

private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*> m_DrawableNodeContainer;
  FullPowerEffectAuraShaderDriver m_FullPowerEffectAuraShaderDriver;

  u32 m_timer;
  gfl2::math::Matrix34 m_rotYMatInv;

  FullPowerEffectAuraMdlManager m_auraMdlManager;
};

#endif  //USE_AURA_SHADER

