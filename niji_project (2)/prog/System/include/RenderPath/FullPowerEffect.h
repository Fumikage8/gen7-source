#pragma once

#if defined(GF_PLATFORM_CTR)
#define USE_AURA_SHADER (1)
#elif defined(GF_PLATFORM_WIN32)
#define USE_AURA_SHADER (0)
#endif  //プラットフォーム分岐

#if USE_AURA_SHADER

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_GLMemory.h>

//===================================================================================================================================================
//
//  全力技描画パス0【リムエフェクト】
//
//===================================================================================================================================================

//! マテリアル単位でのエフェクト無効設定構造体
struct disableMaterialInfo
{
  static const      DISABLE_MATERIAL_MAX = 4;           //! ４つあれば十分と思われる。

  u32         targetNum;                                //! 検索対象数
  u32         materialHashList[DISABLE_MATERIAL_MAX];   //! 検索対名ハッシュ
  disableMaterialInfo()
  {
    targetNum = 0;
    for( u32 i = 0; i < DISABLE_MATERIAL_MAX; ++i )
    {
      materialHashList[i] = 0;
    }
  }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  リムエフェクト専用モデル管理クラス
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRimMdlManager
{
public:
  FullPowerEffectRimMdlManager();
  ~FullPowerEffectRimMdlManager();

  struct mdlStatusSetting
	{
    f32 rimColorR;    // リムカラー赤成分
    f32 rimColorG;    // リムカラー緑成分
    f32 rimColorB;    // リムカラー青成分
    f32 rimPow;       // リムグラデーションの滑らかさ。数値が大きいほどリムカラー→テクスチャカラーの境目がはっきりする
    f32 rimWidth;     // リムの幅
    f32 rimMoveSpeed; // リムの幅変更エフェクトのスピード。0=変更エフェクトなし。数値が大きいほど速い。
    f32 rimMoveWidth; // リムの幅変更エフェクトにおいて、rimWidthに加減算される幅

    disableMaterialInfo     m_disableMaterialInfo;

    mdlStatusSetting(){
      rimColorR = rimColorG = rimColorB = 1.0f;
      rimPow = 4.0f;
      rimWidth = 0.3f;
      rimMoveSpeed = 0;
      rimMoveWidth = 0.0f;
    }
	};

  // 上書き項目はないが、pMeshDrawTagからNodeを逆算できるようにしておく
  struct OverwriteTag
	{
    gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag; // 関連付けされたMeshDrawTag
    bool isDrawable;  // 描画判別用(一部Mask用Mesh等、描画したくないものを区分する)
	};

  // DrawableNodeと関連付けられた構造体
  struct EffectMdl
	{
    OverwriteTag* arrayTag;
    u32           numTag;

    mdlStatusSetting status;

    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;  // このノードを描画時に差し替える
    bool          isEnable;
    u8            dummy[3];
	};

  gfl2::gfx::IGLAllocator* m_pAllocator; 
  void SetAllocator( gfl2::gfx::IGLAllocator* pAllocator );

  void CreateEffectMdl( gfl2::heap::HeapBase *heap , gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, mdlStatusSetting* pStatus );
	void RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void DeleteEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  void GetParam( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag, EffectMdl** ppOutEffectMdl, OverwriteTag** ppOutTag );

  void ChangeStatus( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, mdlStatusSetting* pStatus );

private:
  #define EFFECT_RESIST_MAX ( 32 ) // とりあえず32枠まで
  EffectMdl m_effectMdl[ EFFECT_RESIST_MAX ];

  void _initEffectMdl( EffectMdl* pEffectMdl );
  void _cleanupEffectMdl( EffectMdl* pEffectMdl );
};

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
  virtual b32 SetPolygonDrawOverride
    ( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag ) const;

  u32* m_renderStateCommand;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  gfl2::gfx::IGLAllocator* m_pAllocator;
  // 固有定数バッファ
	struct CBuffer
	{
    u32 timer;  // タイマー
	};
	CBuffer m_CBuffer;
  FullPowerEffectRimMdlManager* m_pRimMdlManager;

  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetCBuffer( CBuffer* pData, FullPowerEffectRimMdlManager* ptr );
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  RenderPath
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRenderPath0 : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	FullPowerEffectRenderPath0();
	FullPowerEffectRenderPath0( gfl2::gfx::IGLAllocator* pAllocator );
	virtual ~FullPowerEffectRenderPath0();

  // リソースセットアップ
  //
  // リソースバイナリは外部から受け取るようにするが、内容は可変するようなものではなく、以下のファイル内容で固定
  //  gfbmdlBinary    = "rom:/effect/fullPowerRimEffect.ctr.gfbmdl"			
  //  shaderBinaries  = { "rom:/effect/FullPowerRim.ctr.gfbvsh", "rom:/effect/FullPowerRim_Manual.ctr.gfbfsh" };
  //  shaderNum       = 2
  //  textureBinaries = { "rom:/effect/fullPowerEffectTexDummy.ctr.btex" }
  //  texNum          = 1
  void SetResource( c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, void *binDataBuf );

  // 指定ノードをレンダリング対象に登録
  void AddRenderingTarget
    (  gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus );

  // 指定ノードをレンダリング対象から解除
  void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  // 指定ノード用のモデルを削除(レンダリング対象になっているものをいきなり削除すると問題が発生するので、時間差で実行できるように配慮)
  void DeleteManagedModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  // エフェクトパラメーター変更
  void ChangeEffectStatus
    ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus );

private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
  FullPowerEffectRimShaderDriver m_FullPowerEffectRimShaderDriver;

  gfl2::gfx::IGLAllocator* m_pAllocator;
  u32 m_timer;

  FullPowerEffectRimMdlManager m_rimMdlManager;
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

  struct mdlStatusSetting
	{
    f32 auraColorR; // オーラカラー赤成分
    f32 auraColorG; // オーラカラー緑成分
    f32 auraColorB; // オーラカラー青成分
    f32 auraScaleX; // オーラテクスチャ幅スケーリング(1=等倍)
    f32 auraScaleY; // オーラテクスチャ高さスケーリング(1=等倍)
    u32 density;    // オーラ密度（量）。8～数値が大きいほど多い

    disableMaterialInfo     m_disableMaterialInfo;

    mdlStatusSetting(){
      auraColorR = auraColorG = auraColorB = auraScaleX = auraScaleY = 1.0f;
      density = 512.0f;
    }
	};

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
    bool isDrawable;  // 描画判別用(一部Mask用Mesh等、描画したくないものを区分する)
	};

  // DrawableNodeと関連付けられた構造体
  struct EffectMdl
	{
    OverwriteTag* arrayTag;
    u32           numTag;

    mdlStatusSetting status;

    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;  // このノードを描画時に差し替える
    bool          isEnable;
    u8            dummy[3];
	};

  gfl2::gfx::IGLAllocator* m_pAllocator; 
  void SetAllocator( gfl2::gfx::IGLAllocator* pAllocator );

  void CreateEffectMdl( gfl2::heap::HeapBase *heap , gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, mdlStatusSetting* pStatus );
	void RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void DeleteEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  void GetParam( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag, EffectMdl** ppOutEffectMdl, OverwriteTag** ppOutTag );

  void ChangeStatus( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, mdlStatusSetting* pStatus );

private:
  #define EFFECT_RESIST_MAX ( 32 ) // とりあえず32枠まで
  EffectMdl m_effectMdl[ EFFECT_RESIST_MAX ];

  void _initEffectMdl( EffectMdl* pEffectMdl );
  void _cleanupEffectMdl( EffectMdl* pEffectMdl );

  b32 _cleateBufferData( gfl2::heap::HeapBase *heap ,OverwriteTag* pTag, u32 density );
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

  u32* m_renderStateCommand;
	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pBinModelRootNode;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  gfl2::gfx::IGLAllocator* m_pAllocator;
	// 固有定数バッファ
	struct CBuffer
	{
    //gfl2::math::Matrix  mtxBillBoardY;  // Y軸ビルボード行列
    nn::math::MTX34     mtxView;        // 
    nn::math::MTX44     mtxProj;        // 
    u32                 timer;          // タイマー
	};
	CBuffer m_CBuffer;
  FullPowerEffectAuraMdlManager* m_pAuraMdlManager;

  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetCBuffer( CBuffer* pData, FullPowerEffectAuraMdlManager* ptr );
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  RenderPath
//---------------------------------------------------------------------------------------------------------------------------------------------------
class FullPowerEffectRenderPath1 : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	FullPowerEffectRenderPath1();
	FullPowerEffectRenderPath1( gfl2::gfx::IGLAllocator* pAllocator );
	virtual ~FullPowerEffectRenderPath1();

  // リソースセットアップ
  //
  // リソースバイナリは外部から受け取るようにするが、内容は可変するようなものではなく、以下のファイル内容で固定
  //  gfbmdlBinary    = "rom:/effect/fullPowerAuraEffect.ctr.gfbmdl"			
  //  shaderBinaries  = { "rom:/effect/FullPowerAura.ctr.gfbvsh", "rom:/effect/FullPowerAura_Manual.ctr.gfbfsh" };
  //  shaderNum       = 2
  //  textureBinaries = { "rom:/effect/fullPowerEffectTexAura.ctr.btex" }
  //  texNum          = 1
  void SetResource( c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum );
  void SetResource( gfl2::gfx::IGLAllocator* pAllocator, void *binDataBuf );

  // 指定ノードをレンダリング対象に登録
	void AddRenderingTarget
    (  gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus );

  // 指定ノードをレンダリング対象から解除
  void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  // 指定ノード用のモデルを削除(レンダリング対象になっているものをいきなり削除すると問題が発生するので、時間差で実行できるように配慮)
  void DeleteManagedModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

  // エフェクトパラメーター変更
  void ChangeEffectStatus
    ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus );

private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();

  virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
  FullPowerEffectAuraShaderDriver m_FullPowerEffectAuraShaderDriver;

  gfl2::gfx::IGLAllocator* m_pAllocator;
  u32 m_timer;

  FullPowerEffectAuraMdlManager m_auraMdlManager;
};





//===================================================================================================================================================
//
//  頂点要素解析クラス
//
//===================================================================================================================================================
class AnalizeVertexInputData
{
public:
  AnalizeVertexInputData();
  ~AnalizeVertexInputData();

  void Analize( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag );

	struct VtxInputData
	{
    b32 isFixed;  // true = 固定化
    f32 data[4];  // true = 固定化された要素の実データ
	};
  VtxInputData  m_vtxInputData[16];       // 最大16要素
  u16           m_vtxInputDataFixedMask;  // 上記構造体のisFixedのみを抽出したマスクデータ
};

#endif //USE_AURA_SHADER
