#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <macro/include/gfl2_Macros.h>

// フィールド用にカスタマイズしたコード有効　マクロ
#define FIELD_CUSTOM 1

#if FIELD_CUSTOM
#include <util/include/gfl2_UtilTextRenderPath.h>
#include <renderer/include/gfl2_BloomRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include "System/include/MotionBlur.h"
#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_DofRenderPath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeBlurImagePath.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_MakeReductionImagePath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderPath.h"
#include "Field/FieldRo/include/Debug/FieldDebugInfo.h"
#endif //#if PM_DEBUG


#endif


// 前方宣言
GFL_NAMESPACE_BEGIN( System )
class NijiOutLinePostSceneRenderPath;
class NijiEdgeMapSceneRenderPath;
class nijiAllocator;
GFL_NAMESPACE_END( System )

GFL_NAMESPACE_BEGIN( Field );
class FieldNode;

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// ↓のマクロが1の場合はniji拡張分が実行されます。マージの際には気をつけてください yy-ikeuchi  ※※※
#define REMOVAL_OF_MEMORY_LEAK 1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

#if FIELD_CUSTOM
class SkyBoxRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	SkyBoxRenderPath(gfl2::gfx::IGLAllocator* allocator);
	virtual ~SkyBoxRenderPath();
	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetCameraParamAddress(void){ return &m_camera; }
  void CreateNode( gfl2::gfx::IGLAllocator *pAllocator );
  void DeleteNode();
#if defined(GF_PLATFORM_CTR)
  void SetViewSpaceDiffMatrix( const gfl2::math::Matrix34& rMat ) { m_ViewSpaceDiffMatrix = rMat;  }
#endif
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update ( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;
  gfl2::renderingengine::scenegraph::instance::TransformNode                  *m_node;

#if defined(GF_PLATFORM_CTR)
  gfl2::math::Matrix34  m_ViewSpaceDiffMatrix;
#endif
};

class WeatherBoardRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
	WeatherBoardRenderPath(gfl2::gfx::IGLAllocator* allocator, int modelNum );
	virtual ~WeatherBoardRenderPath();
	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetCameraParamAddress(void){ return &m_camera; }
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update ( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;
};


/*
 *  @brief 正射影レンダリングパス
 *  @note 事前、事後にデプスのクリアを行なっていない。　ここでレンダリングするモデルのマテリアルはデプス参照なし、デプス更新なしでないといけない。
 */
class OrthoRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:
  OrthoRenderPath(){} // ダミー
	OrthoRenderPath(gfl2::gfx::IGLAllocator* allocator, int modelNum);
	virtual ~OrthoRenderPath();
	void AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* GetCameraParamAddress(void){ return &m_camera; }
private:
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual void Update ( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;
};


#endif

class MainRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
public:

  MainRenderPath(gfl2::gfx::IGLAllocator* allocator, int modelNum);
	virtual ~MainRenderPath();

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

#if FIELD_CUSTOM
	void AddDrawEnvForFog(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode){ m_DrawEnvNodeContainerForFog.push_back(pDrawEnvNode); }
	void ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);

  void SetClearDepthFlag( b32 is_depth );
#endif

private:
#if FIELD_CUSTOM
	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
#endif

  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

	virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;

#if FIELD_CUSTOM
  gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawEnvNode*>	m_DrawEnvNodeContainerForFog;

  b32 m_IsDepthClear;
#endif

};

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
  MyRenderingPipeLine( 
    gfl2::heap::HeapBase* pHeap, 
    System::nijiAllocator* pAllocator,
    void*                   pEdgeMapResBuf,
    void*                   pOutLineResBuf,
    f32 screanWidth, 
    f32 screanHeight 
    );

	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_MainRenderPath.AddDrawEnv(pNode);
    m_SkyBoxRenderPath.AddDrawEnv(pNode);
    m_WeatherBoardRenderPath.AddDrawEnv(pNode);
    m_OrthoRenderPath.AddDrawEnv(pNode);
    m_MainRenderPath.AddDrawEnvForFog(pNode);
  }

#if FIELD_CUSTOM
	void AddSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
  
  void AddWeatherBoardRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveWeatherBoardRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );

#endif
	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void SetDepthTexture( gfl2::gfx::Texture* pTexture );

#if FIELD_CUSTOM

  //----------------------------------------------------------------------------
  /**
  *  @brief  OrthoRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  OrthoRenderPath* GetOrthoRenderPath( void ){ return &m_OrthoRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  SkyBoxRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  SkyBoxRenderPath* GetSkyBoxRenderPath( void ){ return &m_SkyBoxRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  WeatherBoardRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  WeatherBoardRenderPath* GetWeatherBoardRenderPath( void ){ return &m_WeatherBoardRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  DrawUtilTextのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::util::UtilTextRenderPath* GetUtilTextRenderPath( void ) { return &m_UtilTextRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  Bloomのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  poke_3d::renderer::BloomRenderPath* GetBloomRenderPath( void ){ return &m_BloomRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  Effectのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ){ return &m_EffectRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モーションブラーのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  System::MotionBlur* GetMotionBlurPath( void ){ return &m_MotionBlurPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  メインレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  MainRenderPath* GetMainRenderPath( void ){ return &m_MainRenderPath; }
  System::NijiEdgeMapSceneRenderPath* GetEdgeMapSceneRenderPath( void ){ return m_pEdgeMapSceneRenderPath; }

    //----------------------------------------------------------------------------
  /**
  *  @brief  被写界深度レンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::renderingengine::renderer::util::MakeBlurImagePath *GetMakeBlurImagePath_Front( void ){ return m_pMakeBlurImagePath_Front; }
  gfl2::renderingengine::renderer::util::MakeBlurImagePath *GetMakeBlurImagePath_Back( void ){ return m_pMakeBlurImagePath_Back; }
  gfl2::renderingengine::renderer::util::DofRenderPath *GetDefRenderPath_Front( void ){ return m_pDofRenderPath_Front; }
  gfl2::renderingengine::renderer::util::DofRenderPath *GetDefRenderPath_Back( void ){ return m_pDofRenderPath_Back; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  被写界深度レンダーパスを生成する
  */
  //-----------------------------------------------------------------------------
  struct DofRenderPathInitDescription
  {
    gfl2::gfx::IGLAllocator *pAllocator;  // アロケータ
    void *pDofModelData;                  // Dof用リソース
    void *pDofVertexShaderData;           // Dof用リソース
    void *pDofFrontPixelShaderData;       // Dof用リソース
    void *pDofBackPixelShaderData;        // Dof用リソース
    void *pDofTextureData;                // Dof用リソース
    u32 screen_width;                     // スクリーンの横幅
    u32 screen_height;                    // スクリーンの縦幅
  };
  void InitializeDofRenderPath( gfl2::heap::HeapBase *pHeap, const DofRenderPathInitDescription &desc );

  //----------------------------------------------------------------------------
  /**
  *  @brief  被写界深度レンダーパスを破棄する
  */
  //-----------------------------------------------------------------------------
  void FinalizeDofRenderPath();

  //----------------------------------------------------------------------------
  /**
  *  @brief  被写界深度レンダーパスの設定
  */
  //-----------------------------------------------------------------------------
  void SetEnableDofRender( b32 flag ){ m_EnableDofRender = flag; }
  b32 IsEnableDofRender( void ){ return m_EnableDofRender; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  DrawManager::スキニング精度アップ機能ON / OFF
  */
  //-----------------------------------------------------------------------------
  void SetViewSpaceRenderEnable( b32 flag );


  //----------------------------------------------------------------------------
  /**
  *  @brief 法線マップのON/OFFを行なう
  */
  //-----------------------------------------------------------------------------
  void SetEdgeNormalMapEnable( b32 flag );
  b32  IsEdgeNormalMapEnable() const;


#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief デバッグでのテキスト描画より前に処理するデバッグ描画
  */
  //-----------------------------------------------------------------------------
  DebugTextBeforeRenderPath* GetDebugTextBeforeRenderPath()  { return &m_DebugTextBeforeRenderPath; }
#endif

#endif

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();


	u32																																		m_RenderPathCnt;
	
  System::NijiEdgeMapSceneRenderPath*																		m_pEdgeMapSceneRenderPath;
  System::NijiOutLinePostSceneRenderPath*															  m_pOutLinePostSceneRenderPath;
	MainRenderPath																												m_MainRenderPath;

#if PM_DEBUG
  gfl2::DebugCollisionRenderPath*                                       m_pDebugCollisionRenderPath;
#endif//  #if PM_DEBUG

#if FIELD_CUSTOM
  SkyBoxRenderPath                                                      m_SkyBoxRenderPath;
  WeatherBoardRenderPath                                                m_WeatherBoardRenderPath;
  OrthoRenderPath                                                       m_OrthoRenderPath;
  poke_3d::renderer::BloomRenderPath                                    m_BloomRenderPath;
  gfl2::util::UtilTextRenderPath                                        m_UtilTextRenderPath;
  gfl2::Effect::EffectRenderPath                                        m_EffectRenderPath;
  System::MotionBlur                                                    m_MotionBlurPath;
  b32                                                                   m_IsEdgeNormalMap;

  // Dof用
  gfl2::renderingengine::renderer::util::MakeBlurImagePath              *m_pMakeBlurImagePath_Front;
  gfl2::renderingengine::renderer::util::MakeReductionImagePath         *m_pMakeReductionImagePath_Front;
  gfl2::renderingengine::renderer::util::DofRenderPath                  *m_pDofRenderPath_Front;
  gfl2::renderingengine::renderer::util::MakeBlurImagePath              *m_pMakeBlurImagePath_Back;
  gfl2::renderingengine::renderer::util::MakeReductionImagePath         *m_pMakeReductionImagePath_Back;
  gfl2::renderingengine::renderer::util::DofRenderPath                  *m_pDofRenderPath_Back;
  b32                                                                   m_EnableDofRender;

  // Dofリソース
  gfl2::renderingengine::scenegraph::resource::ResourceNode             *m_pDofResourceNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode        *m_pDofModelInstanceNode_Front;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode        *m_pDofModelInstanceNode_Back;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData          *m_pDofModelData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData          *m_pDofVertexShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData          *m_pDofFrontPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryShdData          *m_pDofBackPixelShaderData;
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData          *m_pDofTextureData;

#if PM_DEBUG
  DebugTextBeforeRenderPath       m_DebugTextBeforeRenderPath;
#endif
#endif

};


//------------------------------------------------------------------------------
// キャプチャー時に使用するパイプライン
//------------------------------------------------------------------------------
class CaptureRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
  CaptureRenderingPipeLine( 
    gfl2::heap::HeapBase* pHeap, 
    System::nijiAllocator*  pAllocator, 
    void*                   pEdgeMapResBuf,
    void*                   pOutLineResBuf,
    f32 screanWidth, 
    f32 screanHeight 
    );

	virtual ~CaptureRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_MainRenderPath.AddDrawEnv(pNode);
    m_OrthoRenderPath.AddDrawEnv(pNode);
    m_MainRenderPath.AddDrawEnvForFog(pNode);
  }

	void AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void SetDepthTexture( gfl2::gfx::Texture* pTexture );

#if FIELD_CUSTOM

  //----------------------------------------------------------------------------
  /**
  *  @brief  OrthoRenderPathのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  OrthoRenderPath* GetOrthoRenderPath( void ){ return &m_OrthoRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  Effectのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::Effect::EffectRenderPath* GetEffectRenderPath( void ){ return &m_EffectRenderPath; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モーションブラーのレンダーパス生成、初期化
  */
  //-----------------------------------------------------------------------------
  void InitializeMotionBlurPath(
    gfl2::gfx::IGLAllocator*  pAllocator,
    gfl2::heap::HeapBase* pHeap,
    gfl2::fs::AsyncFileManager* pAsyncFileManager,
    u32 useTextureTarget
    );

  //----------------------------------------------------------------------------
  /**
  *  @brief  モーションブラーのレンダーパス初期化待ち
  *  @retval true 初期化完了
  *  @retval false 初期化実行中 or 生成されていない
  */
  //-----------------------------------------------------------------------------
  bool WaitInitializeMotionBlurPath();

  //----------------------------------------------------------------------------
  /**
  *  @brief  モーションブラーのレンダーパス破棄
  *  @note 必ず1フレーム前にMotionBlur::RenderOffを呼んでください
  */
  //-----------------------------------------------------------------------------
  void TerminateMotionBlurPath();

  //----------------------------------------------------------------------------
  /**
  *  @brief  モーションブラーのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  System::MotionBlur* GetMotionBlurPath( void ){ return m_pMotionBlurPath; }

  void AddPokeShaderPath( gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* pPath) { if( pPath) { m_PokeShaderRenderPaths.push_back( pPath); } }
  void RemovePokeShaderPath( gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* pPath) { if( pPath) { m_PokeShaderRenderPaths.remove( pPath); } }

#endif

private:

	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();


	u32																																		m_RenderPathCnt;
	
  System::NijiEdgeMapSceneRenderPath*																		m_pEdgeMapSceneRenderPath;
  System::NijiOutLinePostSceneRenderPath*															  m_pOutLinePostSceneRenderPath;
	MainRenderPath																												m_MainRenderPath;

#if FIELD_CUSTOM
  OrthoRenderPath                                                       m_OrthoRenderPath;
  gfl2::Effect::EffectRenderPath                                        m_EffectRenderPath;
  System::MotionBlur*                                                   m_pMotionBlurPath;
  gfl2::util::FixedSizeContainer<gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath*> m_PokeShaderRenderPaths;  ///< ポケモンカラーシェーダー
#endif

};


GFL_NAMESPACE_END( Field );

#endif
