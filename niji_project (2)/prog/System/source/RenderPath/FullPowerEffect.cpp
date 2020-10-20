
#include "System/include/RenderPath/FullPowerEffect.h"

#if USE_AURA_SHADER

#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>

#include <debug/include/gfl2_DebugPrint.h>

#include <nn/fs.h>
#include <nn/hio.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

nn::math::MersenneTwister random;

// 開発時用
#if 0
#include "CtrGfbDummy.h"

Ctr_CommandBinary_RenderState g_cbRS; // レンダーステートコマンド生成

//  デバッグダンプ
void debugCommDump( u32* data, u32 size )
{
  u32 size32 = size / sizeof(u32);
  u32 L = size32 / 16;
  u32 C = size32 % 16;

  for( int i=0; i<L; ++i ){
    for( int j=0; j<16; ++j ){ GFL_PRINT(" %08x,", data[i*16 + j]); }
    GFL_PRINT("\n");
  }
  if( C ){
    for( int j=0; j<C; ++j ){ GFL_PRINT(" %08x,", data[L*16 + j]); }
    GFL_PRINT("\n");
  }
}

#endif

//===================================================================================================================================================
// ローカル関数
//===================================================================================================================================================
bool namecmp( const c8* p1, const c8* p2 )
{
  int i = 0;

  while( p1[i] == p2[i] ){
    if( p1[i] == '\0'){ return true; }
    i++;
    if( i >= 80 ){ return false; }  // 文字数オーバー
  }
  return false;
}

bool checkDrawableShader( gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag, const disableMaterialInfo* pDisableMaterialInfo )
{
  // マテリアルノード取得
  const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMtrNode;
  pMtrNode = pMeshDrawTag->GetMaterialInstanceNode();

  // シェーダーノード取得
  const gfl2::renderingengine::scenegraph::resource::ShaderResourceNode* pShaderNode;
  pShaderNode = pMtrNode->SetPixelShaderResourceNode();

  // シェーダー名取得
  const c8* shaderName;
  shaderName = pShaderNode->GetNodeName();

  c8* NonDrawableNameTbl[] = {
    "FireCore_FireWingGRE",
    "FireSten_lizardonGRE",
    "FireGeom_lizardonGRE",
    "FireCore_ghosGRE",
    "FireGeom_ghosGRE",
    "FireSten_ghosGRE",
    "FireSten_dogarsGRE",
    "FireSten_cotoiseGRE",
    "FireGeom_pm0872mayu_BillbordGRE",
    "FireGeom_pm0872mayu_BillbordGRE",
  };
  int numTbl = sizeof(NonDrawableNameTbl) / sizeof(c8*);

  for( int i=0; i<numTbl; ++i ){
    if( namecmp( shaderName, NonDrawableNameTbl[i] ) == true ){ return false; }
  }
  //GFL_PRINT( " %s\n", shaderName );

  u32               materialNameHash = pMtrNode->GetNameHash();

  if ( pDisableMaterialInfo )
  {
    for( u32 i = 0; i < pDisableMaterialInfo->targetNum; ++i )
    {
      u32       targetHash = pDisableMaterialInfo->materialHashList[i];

      if ( materialNameHash == targetHash )
      {
        return false;
      }
    }
  }

  return true;
}

//===================================================================================================================================================
//
//
//
//  全力技描画パス0【リムエフェクト】
//
//
//
//===================================================================================================================================================
FullPowerEffectRenderPath0::FullPowerEffectRenderPath0()
{
	m_DrawableNodeContainer.CreateBuffer( 2 );  // バトルで2体出るのがMAX

  m_pAllocator = NULL;
  m_timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath0::FullPowerEffectRenderPath0( gfl2::gfx::IGLAllocator* pAllocator )
{
  m_pAllocator = pAllocator;
	m_DrawableNodeContainer.CreateBuffer( 2, m_pAllocator );  // バトルで2体出るのがMAX

  m_timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath0::~FullPowerEffectRenderPath0()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// リソースセットアップ
void FullPowerEffectRenderPath0::SetResource( c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  if( m_pAllocator ){
    SetResource( m_pAllocator, gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  } else {
    SetResource( gfl2::gfx::GLMemory::GetAllocator(), gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  }
}

void FullPowerEffectRenderPath0::SetResource
  ( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  m_pAllocator = pAllocator;
  m_FullPowerEffectRimShaderDriver.SetResource( pAllocator, gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  m_rimMdlManager.SetAllocator( pAllocator );
}

//BinLinker版セットアップ
void FullPowerEffectRenderPath0::SetResource( gfl2::gfx::IGLAllocator* pAllocator, void *binDataBuf )
{
  gfl2::fs::BinLinkerAccessor binData;
  binData.Initialize(binDataBuf);

   c8* dataArr[4] = 
  {
    static_cast<c8*>(binData.GetData(0)),
    static_cast<c8*>(binData.GetData(1)),
    static_cast<c8*>(binData.GetData(2)),
    static_cast<c8*>(binData.GetData(3))
  };

  SetResource( pAllocator , dataArr[0], &dataArr[1], 2, &dataArr[3], 1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象に登録
void FullPowerEffectRenderPath0::AddRenderingTarget
  ( gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus )
{
	m_DrawableNodeContainer.push_back( pNode );

  m_rimMdlManager.CreateEffectMdl( heap, pNode, pStatus );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象から解除
void FullPowerEffectRenderPath0::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );

  m_rimMdlManager.RemoveEffectMdl( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノード用のモデルを削除(レンダリング対象になっているものをいきなり削除すると問題が発生するので、時間差で実行できるように配慮)
void FullPowerEffectRenderPath0::DeleteManagedModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_rimMdlManager.DeleteEffectMdl( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// エフェクトパラメーター変更
void FullPowerEffectRenderPath0::ChangeEffectStatus
  ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus )
{
  m_rimMdlManager.ChangeStatus( pNode, pStatus );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* FullPowerEffectRenderPath0::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectRenderPath0::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectRenderPath0::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() ){
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight ){
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		} else {
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}
    // 今まで描画したものを消去しないようにする
    //ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
	}

  GFGL::BeginScene();
  {
    const IShaderDriver* pShader = &m_FullPowerEffectRimShaderDriver;
	  m_pDrawManager->SetOverrideShader( pShader );

    // 固有定数バッファの設定
    FullPowerEffectRimShaderDriver::CBuffer cBuffer;
    cBuffer.timer = m_timer;
    m_timer++;

    m_FullPowerEffectRimShaderDriver.SetCBuffer( &cBuffer, &m_rimMdlManager );

    nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
	    // proj
		  nn::math::MTX44* pProj;
		  pProj = (nn::math::MTX44*)&rDrawContext.m_Projection;
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pProj );
	    // view
		  nn::math::MTX34* pView;
		  pView = (nn::math::MTX34*)&rDrawContext.m_View;
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pView );
    }
    pCommandBuffer->Put(cmd);

    m_pDrawManager->Draw( false );
  }
  GFGL::EndScene();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
// ShaderDriver
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
// レンダーステートコマンド
static const u32 RimShadeRenderStateCommand[] = {
  0x00000001, 0x000f0111, 0x00000001, 0x000f0110, 0x00000002, 0x000f0040, 0x00e40100, 0x00030100,
  0x76760000, 0x000f0101, 0x00000006, 0x000f0102, 0x01000000, 0x000f0103, 0x00000010, 0x00030104, 
  0x0000ff10, 0x000f0105, 0x00000000, 0x000f0106, 0x00001f51, 0x000f0107, 0x00000001, 0x000f006d,
  0x00bf0000, 0x000f004d, 0x00000000, 0x000f004e, 0x0000000f, 0x00010112, 0x0000000f, 0x00010113, 
  0x00000002, 0x00010114, 0x00000002, 0x00010115, 0x00000000, 0x00000000, 0x00000001, 0x000f023d, 
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRimShaderDriver::FullPowerEffectRimShaderDriver() : GLMemory(), IShaderDriver()
{
  m_pAllocator = NULL;

  m_pBinModelRootNode = NULL;
  m_pModelInstanceNode = NULL;

  m_pRimMdlManager = NULL;
  m_renderStateCommand = NULL;

  m_CBuffer.timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRimShaderDriver::~FullPowerEffectRimShaderDriver()
{
  if( m_pAllocator )
  {
    m_pAllocator->SystemMemoryFree( m_renderStateCommand );
  }

  GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// リソース設定
void FullPowerEffectRimShaderDriver::SetResource
  ( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  m_pAllocator = pAllocator;

  // リソースセットアップ
  //
  // リソースバイナリは外部から受け取るようにするが、内容は可変するようなものではなく、以下のファイル内容で固定
  //  gfbmdlBinary    = "rom:/effect/fullPowerRimEffect.ctr.gfbmdl"			
  //  shaderBinaries  = { "rom:/effect/FullPowerRim.ctr.gfbvsh", "rom:/effect/FullPowerRim_Manual.ctr.gfbfsh" };
  //  shaderNum       = 2
  //  textureBinaries = { "rom:/effect/fullPowerEffectTexDummy.ctr.btex" }
  //  texNum          = 1

  // モデルデータをセットアップ
  scenegraph::resource::GfBinaryMdlData binMdlData;
	binMdlData.SetModelData( gfbmdlBinary );
	m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binMdlData );	

  // 上記モデルのシェーダーを関連付け
  for( s32 i=0; i < shaderNum; ++i ){
    scenegraph::resource::GfBinaryShdData binShdData;
    binShdData.SetShaderData( shaderBinaries[i] );
    scenegraph::resource::ResourceNode* pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binShdData );
	  m_pBinModelRootNode->AddChild( pBinShdNode );
  }

  // 上記モデルのテクスチャを読み込み関連付け
  for( s32 i=0; i < texNum; ++i ){
    scenegraph::resource::GfBinaryTexData binTexData;
    binTexData.SetTextureData( textureBinaries[i] );
    scenegraph::resource::ResourceNode* pBinTexNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binTexData );
	  m_pBinModelRootNode->AddChild( pBinTexNode );
  }

  // 必要なリソースを設定後、このモデルのインスタンスを作成してシェーダーをぶら下げておく
  // (リソースに欠けているもの、整合性が合わないものがあれば、ここでフリーズ)
  m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pAllocator, m_pBinModelRootNode );

  // このモデル自体はシェーダー保管庫にすぎないので、シーンからはずして描画はOffにしておく
	scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
	m_pModelInstanceNode->UpdateDrawTag( info );
	scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );

  // レンダーステート設定コマンドをデバイスメモリにコピー
  m_renderStateCommand = (u32*)( pAllocator->SystemMemoryMallocAlign( sizeof(RimShadeRenderStateCommand), 128 ) );
  for( u32 i=0; i<sizeof(RimShadeRenderStateCommand)/sizeof(u32); ++i ){ m_renderStateCommand[i] = RimShadeRenderStateCommand[i]; }

  // 開発時レンダーステートコマンド生成部
  // ↓の設定で、RimShadeRenderStateCommandコマンドデータテーブルを作成した。
  // 設定内容を確認できるようにするため、残しておく
#if 0
  {
    g_cbRS.Cleanup();

    g_cbRS.m_renderStateData.brend.isEnable = true;
    g_cbRS.m_renderStateData.brend.idxEqRGB = RDST_BLENDEQ::ADD;
    g_cbRS.m_renderStateData.brend.idxFuncSrcRGB = RDST_BLENDFUNC::SRC_ALPHA;
    g_cbRS.m_renderStateData.brend.idxFuncDstRGB = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;
    g_cbRS.m_renderStateData.brend.idxEqA = RDST_BLENDEQ::ADD;
    g_cbRS.m_renderStateData.brend.idxFuncSrcA = RDST_BLENDFUNC::SRC_ALPHA;
    g_cbRS.m_renderStateData.brend.idxFuncDstA = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;

    g_cbRS.m_renderStateData.alphaTest.isEnable = false;

    g_cbRS.m_renderStateData.depthTest.isEnable = true;
    g_cbRS.m_renderStateData.depthTest.idxFunc = RDST_DEPTHTEST::L_EQUAL;
    g_cbRS.m_renderStateData.depthTest.isEnableWrite = true;

    g_cbRS.m_renderStateData.stencilTest.isEnable = false;

    g_cbRS.m_renderStateData.cullMode.isEnable = true;
    g_cbRS.m_renderStateData.cullMode.mode = RDST_CULL::CCW;

    g_cbRS.m_renderStateData.polyOffs.isEnable = false;

    g_cbRS.Setup();
  }
#endif
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 固有定数バッファ設定
void FullPowerEffectRimShaderDriver::SetCBuffer( FullPowerEffectRimShaderDriver::CBuffer* pData, FullPowerEffectRimMdlManager* ptr )
{
  if( !pData ){ return; }

  m_CBuffer = *pData;
  m_pRimMdlManager = ptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// シェーダー適用
b32 FullPowerEffectRimShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  if( !m_pRimMdlManager ){ return true; }
  //if( !pMeshDrawTag->IsVisible() ){ return true; }

  FullPowerEffectRimMdlManager::mdlStatusSetting status;
  {
    FullPowerEffectRimMdlManager::EffectMdl* pEffectMdl;
    FullPowerEffectRimMdlManager::OverwriteTag* pOvTag;

    m_pRimMdlManager->GetParam( pMeshDrawTag, &pEffectMdl, &pOvTag );

    if( !pOvTag ){ return true; }
    if( pOvTag->isDrawable == false ){ return true; }

    status = pEffectMdl->status;
  }

  // 本来の設定を保存
	RenderState::SetUpEnable  origSetUpEnable = rRenderState.GetSetUpEnable();
	RenderState::SetUpEnable  setUpEnable = origSetUpEnable;

  // 本来のシェーダードライバ内容を無効化
	{
		setUpEnable.VertexShader = false;
		setUpEnable.GeometryShader = false;
		setUpEnable.FragmentShader = false;
		setUpEnable.UvSetting = false;
		rRenderState.SetSetUpEnable( setUpEnable );

		pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );
	}
#if 0
  // このシェーダードライバ固有の定数レジスタを設定(レジスタ上書きを見越してここでやる)
  {
	  nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
      f32 effectMin = status.rimWidth + nn::math::SinIdx( m_CBuffer.timer * status.rimMoveSpeed ) * status.rimMoveWidth;
	    // param
      nn::math::VEC4 param;
      param.x = status.rimPow;
      param.y = effectMin;
      param.z = 1.0f/(1.0f - effectMin);
      param.w = 0.00f;
      // 未使用の定数レジスタを流用する
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 7, param );

	    // color
      nn::math::VEC4 color;
      color.x = status.rimColorR;
      color.y = status.rimColorG;
      color.z = status.rimColorB;
      color.w = 1.0f;
      // 未使用の定数レジスタを流用する
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 8, color );
    }
    pCommandBuffer->Put(cmd);
  }
#endif
  // レンダリングステート設定コマンド発行
  GFGL::AddCommandJump( m_renderStateCommand, sizeof(RimShadeRenderStateCommand) );

  // このモデルのインスタンスからぶら下げておいたシェーダーを取り出して有効化
  MeshDrawTag								*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	CombinerCtrShaderDriver		*pShaderDriver = (CombinerCtrShaderDriver*)( pTag->GetShaderDriver() );
	{
		setUpEnable.value = 0;
    setUpEnable.VsIsWorldSpace = origSetUpEnable.VsIsWorldSpace;
		setUpEnable.VertexShader = true;
		setUpEnable.GeometryShader = true;
		setUpEnable.FragmentShader = true;
		setUpEnable.UvSetting = true;
		rRenderState.SetSetUpEnable( setUpEnable );

		pShaderDriver->Apply( rRenderState, pMeshDrawTag );

    // コンスタントカラーやレンダーステートなどがMeshTagの所持した値で上書きされてしまうので
    // 処理のロスはあるがここで再度コマンド転送
    // 負荷が問題になるようなら考える
    const scenegraph::resource::ShaderResourceNode* pFsh = pShaderDriver->GetPixelShader();
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( pFsh->GetFragmentShader() ), pFsh->GetFragmentShaderSize() );
	}

  // 本来の設定に復帰
	rRenderState.SetSetUpEnable( origSetUpEnable );

  return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
b32 FullPowerEffectRimShaderDriver::SetPolygonDrawOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  // 返り値
  //  true  = この後、DrawTag固有の頂点バッファ、インデックスバッファコマンドを発行しない 
  //  false = この後、DrawTag固有の頂点バッファ、インデックスバッファコマンドを発行する 
  if( !m_pRimMdlManager ){ return true; }

  FullPowerEffectRimMdlManager::EffectMdl* pEffectMdl;
  FullPowerEffectRimMdlManager::OverwriteTag* pOvTag;

  m_pRimMdlManager->GetParam( pMeshDrawTag, &pEffectMdl, &pOvTag );

  if( !pOvTag ){ return true; }  
  if( pOvTag->isDrawable == false ){ return true; }

#if 1
  FullPowerEffectRimMdlManager::mdlStatusSetting status = pEffectMdl->status;

  // このシェーダードライバ固有の定数レジスタを設定(レジスタ上書きを見越してここでやる)
  {
    nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
    bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
      f32 effectMin = status.rimWidth + nn::math::SinIdx( m_CBuffer.timer * status.rimMoveSpeed ) * status.rimMoveWidth;
      // param
      nn::math::VEC4 param;
      param.x = status.rimPow;
      param.y = effectMin;
      param.z = 1.0f/(1.0f - effectMin);
      param.w = 0.00f;
      // 未使用の定数レジスタを流用する
      cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 7, param );

      // color
      nn::math::VEC4 color;
      color.x = status.rimColorR;
      color.y = status.rimColorG;
      color.z = status.rimColorB;
      color.w = 1.0f;
      // 未使用の定数レジスタを流用する
      cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 8, color );
    }
    pCommandBuffer->Put(cmd);
  }
#endif

  return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//  リムエフェクト専用モデル管理クラス
//
//  モデルの差し替えはないが、パラメーター格納クラスとしてMeshTagから検索できるようなものを用意する
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRimMdlManager::FullPowerEffectRimMdlManager()
{
  m_pAllocator = NULL; 

  // 専用モデル構造体初期化
  for( int i=0; i<EFFECT_RESIST_MAX; ++i ){ _initEffectMdl( &m_effectMdl[i] ); }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRimMdlManager::~FullPowerEffectRimMdlManager()
{
  for( int i=0; i<EFFECT_RESIST_MAX; ++i ){ _cleanupEffectMdl( &m_effectMdl[i] ); }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectRimMdlManager::SetAllocator( gfl2::gfx::IGLAllocator* pAllocator )
{
  m_pAllocator = pAllocator; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 構造体初期化 & クリーンアップ
void FullPowerEffectRimMdlManager::_initEffectMdl( FullPowerEffectRimMdlManager::EffectMdl* pEffectMdl )
{
  pEffectMdl->arrayTag = NULL;
  pEffectMdl->numTag = 0;
  pEffectMdl->pMdlNode = NULL;
  pEffectMdl->isEnable = false;

  mdlStatusSetting initStatus;
  pEffectMdl->status = initStatus;
}

void FullPowerEffectRimMdlManager::_cleanupEffectMdl( FullPowerEffectRimMdlManager::EffectMdl* pEffectMdl )
{
  GflHeapSafeFreeMemory( pEffectMdl->arrayTag );
 
  _initEffectMdl( pEffectMdl );
}

//-----------------------------------------------
// エフェクト用モデル作成
//  該当ノードに関連付けしながらモデルデータを作成
void FullPowerEffectRimMdlManager::CreateEffectMdl
  ( gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus  )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  // 登録場所の検索
  EffectMdl* pEffectMdl = NULL;

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    if( m_effectMdl[ idxMdl ].pMdlNode == pMdlNode ){ return; } // 二重登録回避
  }

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    if( m_effectMdl[ idxMdl ].pMdlNode == NULL ){
      pEffectMdl = &m_effectMdl[ idxMdl ];
      break;
    }
  }
  if( pEffectMdl == NULL ){ return; } // 登録オーバーエラー

  // ノード登録
  pEffectMdl->pMdlNode = pMdlNode;
  pEffectMdl->isEnable = true;

  // ステータス保存
  pEffectMdl->status = *pStatus;

  // ノードからMeshTag情報を取り出し、構造体の初期設定
  pEffectMdl->numTag = pEffectMdl->pMdlNode->GetDrawTagNum();
  pEffectMdl->arrayTag = (OverwriteTag*)( GflHeapAllocMemoryAlign(heap, sizeof(OverwriteTag) * pEffectMdl->numTag, 128 ) );
  //ARIIZUMI_PRINT("FP_Rim Tag[%d * %d = %d]\n",pEffectMdl->numTag,sizeof(OverwriteTag),sizeof(OverwriteTag) * pEffectMdl->numTag);
  // MeshTagを保存
  for( u32 idxTag = 0; idxTag < pEffectMdl->numTag; ++idxTag ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxTag ];

    pTag->pMeshDrawTag = pEffectMdl->pMdlNode->GetMeshDrawTag(idxTag);
    pTag->isDrawable = checkDrawableShader( pTag->pMeshDrawTag, &pStatus->m_disableMaterialInfo );
	}
}

//-----------------------------------------------
// エフェクト用モデルを描画対象からはずす
void FullPowerEffectRimMdlManager::RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      pEffectMdl->isEnable = false;
      return;
    }
  }
}

//-----------------------------------------------
// エフェクト用モデル削除
//  該当ノードに関連付けされたモデルデータを削除
void FullPowerEffectRimMdlManager::DeleteEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      _cleanupEffectMdl( pEffectMdl );
      return;
    }
  }
}

//-----------------------------------------------
// 上書き用タグ取得
//  該当タグに対して差し替えるデータ構造体を取得
void FullPowerEffectRimMdlManager::GetParam( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag,
                                              FullPowerEffectRimMdlManager::EffectMdl** ppOutEffectMdl,
                                              FullPowerEffectRimMdlManager::OverwriteTag** ppOutTag )
{
  *ppOutEffectMdl = NULL;
  *ppOutTag = NULL;

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pEffectMdl->isEnable == true ){
      for( u32 idxTag = 0; idxTag < pEffectMdl->numTag; ++idxTag ){
        OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxTag ];

        if( pMeshDrawTag == pTag->pMeshDrawTag ){
          *ppOutEffectMdl = pEffectMdl;
          *ppOutTag = pTag;
        }
	    }
    }
  }
}

//-----------------------------------------------
// ステータス変更
void FullPowerEffectRimMdlManager::ChangeStatus
  ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectRimMdlManager::mdlStatusSetting* pStatus  )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      pEffectMdl->status = *pStatus;
      return;
    }
  }
}





//===================================================================================================================================================
//
//
//
//  全力技描画パス1【オーラエフェクト】
//
//
//
//===================================================================================================================================================
FullPowerEffectRenderPath1::FullPowerEffectRenderPath1()
{
	m_DrawableNodeContainer.CreateBuffer( 2 );  // バトルで2体出るのがMAX

  m_pAllocator = NULL;
  m_timer = 0;

  // とりあえずの乱数
  random.Initialize();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath1::FullPowerEffectRenderPath1( gfl2::gfx::IGLAllocator* pAllocator )
{
  m_pAllocator = pAllocator;
	m_DrawableNodeContainer.CreateBuffer( 2, m_pAllocator );  // バトルで2体出るのがMAX

  m_timer = 0;

  // とりあえずの乱数
  random.Initialize();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath1::~FullPowerEffectRenderPath1()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// リソースセットアップ
void FullPowerEffectRenderPath1::SetResource( c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  if( m_pAllocator ){
    SetResource( m_pAllocator, gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  } else {
    SetResource( gfl2::gfx::GLMemory::GetAllocator(), gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  }
}

void FullPowerEffectRenderPath1::SetResource
  ( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  m_pAllocator = pAllocator;
  m_FullPowerEffectAuraShaderDriver.SetResource( pAllocator, gfbmdlBinary, shaderBinaries, shaderNum, textureBinaries, texNum );
  m_auraMdlManager.SetAllocator( pAllocator );
}

//BinLinker版セットアップ
void FullPowerEffectRenderPath1::SetResource( gfl2::gfx::IGLAllocator* pAllocator, void *binDataBuf )
{
  gfl2::fs::BinLinkerAccessor binData;
  binData.Initialize(binDataBuf);

   c8* dataArr[4] = 
  {
    static_cast<c8*>(binData.GetData(4)),
    static_cast<c8*>(binData.GetData(5)),
    static_cast<c8*>(binData.GetData(6)),
    static_cast<c8*>(binData.GetData(7))
  };

  SetResource( pAllocator , dataArr[0], &dataArr[1], 2, &dataArr[3], 1 );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象に登録
void FullPowerEffectRenderPath1::AddRenderingTarget
  (  gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus )
{
	m_DrawableNodeContainer.push_back( pNode );

  m_auraMdlManager.CreateEffectMdl(heap, pNode, pStatus );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象から解除
void FullPowerEffectRenderPath1::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );

  m_auraMdlManager.RemoveEffectMdl( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノード用のモデルを削除(レンダリング対象になっているものをいきなり削除すると問題が発生するので、時間差で実行できるように配慮)
void FullPowerEffectRenderPath1::DeleteManagedModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_auraMdlManager.DeleteEffectMdl( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// エフェクトパラメーター変更
void FullPowerEffectRenderPath1::ChangeEffectStatus
  ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus )
{
  m_auraMdlManager.ChangeStatus( pNode, pStatus );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* FullPowerEffectRenderPath1::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectRenderPath1::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectRenderPath1::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() ){
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight ){
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		} else {
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}
    // 今まで描画したものを消去しないようにする
    //ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
	}

	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
	GFGL::BeginScene();

  {
    const IShaderDriver* pShader = &m_FullPowerEffectAuraShaderDriver;
	  m_pDrawManager->SetOverrideShader( pShader );

    // 固有定数バッファの設定
    FullPowerEffectAuraShaderDriver::CBuffer cBuffer;

    // Y軸ビルボード行列設定
    math::Matrix44 mtxView = math::Matrix44( rDrawContext.m_View );
    math::Matrix44 mtxBB = math::Matrix44::GetBillboardMatrixRotationY( mtxView );
    math::Matrix34 mtxBB34 = math::Matrix34( mtxBB[0], mtxBB[1], mtxBB[2] );
		nn::math::MTX34* pMtx34 = (nn::math::MTX34*)&mtxBB34;

    cBuffer.mtxView = *pMtx34;
    cBuffer.timer = m_timer;
    m_timer++;

    m_FullPowerEffectAuraShaderDriver.SetCBuffer( &cBuffer, &m_auraMdlManager );

    nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
	    // proj
		  nn::math::MTX44* pProj;
		  pProj = (nn::math::MTX44*)&rDrawContext.m_Projection;
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pProj );
	    // view
		  nn::math::MTX34* pView;
		  pView = (nn::math::MTX34*)&rDrawContext.m_View;
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pView );
    }
    pCommandBuffer->Put(cmd);

    m_pDrawManager->Draw( false );
  }
  GFGL::EndScene();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
// ShaderDriver
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
// レンダーステートコマンド
static const u32 AuraShadeRenderStateCommand[] = {
  0x00000001, 0x000f0111, 0x00000001, 0x000f0110, 0x00000001, 0x000f0040, 0x00e40100, 0x00030100, 
  0x76160000, 0x000f0101, 0x00000006, 0x000f0102, 0x01000000, 0x000f0103, 0x00000010, 0x00030104, 
  0x0000ff10, 0x000f0105, 0x00000000, 0x000f0106, 0x00000f41, 0x000f0107, 0x00000001, 0x000f006d, 
  0x00bf0000, 0x000f004d, 0x00000000, 0x000f004e, 0x0000000f, 0x00010112, 0x0000000f, 0x00010113, 
  0x00000002, 0x00010114, 0x00000000, 0x00010115, 0x00000000, 0x00000000, 0x00000001, 0x000f023d, 
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraShaderDriver::FullPowerEffectAuraShaderDriver() : GLMemory(), IShaderDriver()
{
  m_pAllocator = NULL;

  m_pBinModelRootNode = NULL;
	m_pModelInstanceNode = NULL;

  m_pAuraMdlManager = NULL;
  m_renderStateCommand = NULL;

  m_CBuffer.mtxView = nn::math::MTX34( 1,0,0,0, 0,1,0,0, 0,0,1,0 );
  m_CBuffer.timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraShaderDriver::~FullPowerEffectAuraShaderDriver()
{
  GflHeapSafeFreeMemory( m_renderStateCommand );

  GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// リソース設定
void FullPowerEffectAuraShaderDriver::SetResource
  ( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8** shaderBinaries, s32 shaderNum, c8** textureBinaries, s32 texNum )
{
  m_pAllocator = pAllocator;

  // リソースセットアップ
  //
  // リソースバイナリは外部から受け取るようにするが、内容は可変するようなものではなく、以下のファイル内容で固定
  //  gfbmdlBinary    = "rom:/effect/fullPowerAuraEffect.ctr.gfbmdl"			
  //  shaderBinaries  = { "rom:/effect/FullPowerAura.ctr.gfbvsh", "rom:/effect/FullPowerAura_Manual.ctr.gfbfsh" };
  //  shaderNum       = 2
  //  textureBinaries = { "rom:/effect/fullPowerEffectTexAura.ctr.btex" }
  //  texNum          = 1

  // モデルデータをセットアップ
  scenegraph::resource::GfBinaryMdlData binMdlData;
	binMdlData.SetModelData( gfbmdlBinary );
	m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binMdlData );	

  // 上記モデルのシェーダーを関連付け
  for( s32 i=0; i < shaderNum; ++i ){
#if 1
    scenegraph::resource::GfBinaryShdData binShdData;
    binShdData.SetShaderData( shaderBinaries[i] );
    scenegraph::resource::ResourceNode* pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binShdData );
	  m_pBinModelRootNode->AddChild( pBinShdNode );
#else
    {
      scenegraph::resource::GfBinaryShdData binShdData;
      CtrGfbvshDummy* m_vsh = new CtrGfbvshDummy;
      m_vsh->Setup( pAllocator, "rom:/effect/auraEffectVGSH.shbin", "AuraEffect", true );
      binShdData.SetShaderData( m_vsh->m_CtrGfbvsh );
      scenegraph::resource::ResourceNode* pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binShdData );
	    m_pBinModelRootNode->AddChild( pBinShdNode );
    }
    {
      scenegraph::resource::GfBinaryShdData binShdData;
      binShdData.SetShaderData( shaderBinaries[1] );
      scenegraph::resource::ResourceNode* pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binShdData );
	    m_pBinModelRootNode->AddChild( pBinShdNode );
    }
#endif
  }

  // 上記モデルのテクスチャを読み込み関連付け
  for( s32 i=0; i < texNum; ++i ){
    scenegraph::resource::GfBinaryTexData binTexData;
    binTexData.SetTextureData( textureBinaries[i] );
    scenegraph::resource::ResourceNode* pBinTexNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &binTexData );
	  m_pBinModelRootNode->AddChild( pBinTexNode );
  }

  // 必要なリソースを設定後、このモデルのインスタンスを作成してシェーダーをぶら下げておく
  // (リソースに欠けているもの、整合性が合わないものがあれば、ここでフリーズ)
  m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pAllocator, m_pBinModelRootNode );

  // このモデル自体はシェーダー保管庫にすぎないので、シーンからはずして描画はOffにしておく
	scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
	m_pModelInstanceNode->UpdateDrawTag( info );
	scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );

  // レンダーステート設定コマンドをデバイスメモリにコピー
  m_renderStateCommand = (u32*)( pAllocator->SystemMemoryMallocAlign( sizeof(AuraShadeRenderStateCommand), 128 ) );
  for( u32 i=0; i<sizeof(AuraShadeRenderStateCommand)/sizeof(u32); ++i ){ m_renderStateCommand[i] = AuraShadeRenderStateCommand[i]; }

  // 開発時レンダーステートコマンド生成部
  // ↓の設定で、RimShadeRenderStateCommandコマンドデータテーブルを作成した。
  // 設定内容を確認できるようにするため、残しておく
#if 0
  {
    g_cbRS.Cleanup();

    g_cbRS.m_renderStateData.brend.isEnable = true;
    g_cbRS.m_renderStateData.brend.idxEqRGB = RDST_BLENDEQ::ADD;
    g_cbRS.m_renderStateData.brend.idxFuncSrcRGB = RDST_BLENDFUNC::SRC_ALPHA;
    g_cbRS.m_renderStateData.brend.idxFuncDstRGB = RDST_BLENDFUNC::ONE;
    g_cbRS.m_renderStateData.brend.idxEqA = RDST_BLENDEQ::ADD;
    g_cbRS.m_renderStateData.brend.idxFuncSrcA = RDST_BLENDFUNC::SRC_ALPHA;
    g_cbRS.m_renderStateData.brend.idxFuncDstA = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;

    g_cbRS.m_renderStateData.alphaTest.isEnable = false;

    g_cbRS.m_renderStateData.depthTest.isEnable = true;
    g_cbRS.m_renderStateData.depthTest.idxFunc = RDST_DEPTHTEST::LESS;
    g_cbRS.m_renderStateData.depthTest.isEnableWrite = false;

    g_cbRS.m_renderStateData.stencilTest.isEnable = false;

    g_cbRS.m_renderStateData.cullMode.isEnable = true;
    g_cbRS.m_renderStateData.cullMode.mode = RDST_CULL::CW;

    g_cbRS.m_renderStateData.polyOffs.isEnable = false;

    g_cbRS.Setup();
  }
#endif
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 固有定数バッファ設定
void FullPowerEffectAuraShaderDriver::SetCBuffer( FullPowerEffectAuraShaderDriver::CBuffer* pData, FullPowerEffectAuraMdlManager* ptr )
{
  if( !pData ){ return; }

  m_CBuffer = *pData;
  m_pAuraMdlManager = ptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// シェーダー適用
b32 FullPowerEffectAuraShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  //if( !pMeshDrawTag->IsVisible() ){ return true; }
  if( !m_pAuraMdlManager ){ return true; }
  FullPowerEffectAuraMdlManager::EffectMdl* pEffectMdl;
  FullPowerEffectAuraMdlManager::OverwriteTag* pOvTag;
  m_pAuraMdlManager->GetParam( pMeshDrawTag, &pEffectMdl, &pOvTag );
  if( !pOvTag ){ return true; }
  if( !pOvTag->numVtx ){ return true; }
  if( pOvTag->isDrawable == false ){ return true; }

  // 本来の設定を保存
	RenderState::SetUpEnable  origSetUpEnable = rRenderState.GetSetUpEnable();
	RenderState::SetUpEnable  setUpEnable = origSetUpEnable;

  // 本来のシェーダードライバ内容を無効化
	{
		setUpEnable.VertexShader = false;
		setUpEnable.GeometryShader = false;
		setUpEnable.FragmentShader = false;
		setUpEnable.UvSetting = false;
		rRenderState.SetSetUpEnable( setUpEnable );

		pMeshDrawTag->GetShaderDriver()->Apply( rRenderState, pMeshDrawTag );
	}

  // このモデルのインスタンスからぶら下げておいたシェーダーを取り出して有効化
  MeshDrawTag								*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	CombinerCtrShaderDriver		*pShaderDriver = (CombinerCtrShaderDriver*)( pTag->GetShaderDriver() );
	{
		setUpEnable.value = 0;
    setUpEnable.VsIsWorldSpace = origSetUpEnable.VsIsWorldSpace;
		setUpEnable.VertexShader = true;
		setUpEnable.GeometryShader = true;
		setUpEnable.FragmentShader = true;
		setUpEnable.UvSetting = true;
		rRenderState.SetSetUpEnable( setUpEnable );

		pShaderDriver->Apply( rRenderState, pMeshDrawTag );

    // コンスタントカラーやレンダーステートなどがMeshTagの所持した値で上書きされてしまうので
    // 処理のロスはあるがここで再度コマンド転送
    // 負荷が問題になるようなら考える
    //const scenegraph::resource::ShaderResourceNode* pFsh = pShaderDriver->GetPixelShader();
    //GFGL::AddCommandJump( reinterpret_cast<const u32*>( pFsh->GetFragmentShader() ), pFsh->GetFragmentShaderSize() );
  }
  // レンダリングステート設定コマンド発行
  GFGL::AddCommandJump( m_renderStateCommand, sizeof(AuraShadeRenderStateCommand) );

  // 本来の設定に復帰
	rRenderState.SetSetUpEnable( origSetUpEnable );

  return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectAuraShaderDriver::SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  //if( !pMeshDrawTag->IsVisible() ){ return true; }
  if( !m_pAuraMdlManager ){ return; }
  FullPowerEffectAuraMdlManager::EffectMdl* pEffectMdl;
  FullPowerEffectAuraMdlManager::OverwriteTag* pOvTag;
  m_pAuraMdlManager->GetParam( pMeshDrawTag, &pEffectMdl, &pOvTag );
  if( !pOvTag ){ return; }
  if( !pOvTag->numVtx ){ return; }
  if( pOvTag->isDrawable == false ){ return; }

  MeshDrawTag* pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	RenderState::ShaderStateMap shaderStateMap = rRenderState.GetShaderStateMap();
	{
		const gfx::Texture* pTexture = pTag->GetTexture( 0 );
		{
			u32 textureCommand[8];

			const gfl2::gfx::ctr::CTRTexture*       pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc&  rTextureDesc = pCTRTexture->GetTextureDesc();

      textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	PICA_DATA_TEXTURE_MAG_FILTER_LINEAR, 
                                                              PICA_DATA_TEXTURE_MIN_FILTER_LINEAR,
																															pCTRTexture->GetFormat(),
																															PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,
                                                              PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,
																															0, PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D );

      textureCommand[1] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_WRAP_FILTER );
				
			//デフォルトではTexture0に設定するコマンド
			// 0x082
			textureCommand[2] = PICA_CMD_DATA_TEXTURE_SIZE( rTextureDesc.width, rTextureDesc.height );
			textureCommand[3] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_SIZE );

			// 0x085
			textureCommand[4] = PICA_CMD_DATA_TEXTURE_ADDR( pCTRTexture->GetPhysicalAddr() );
			textureCommand[5] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_ADDR1 );

			// 0x08e
			textureCommand[6] = PICA_CMD_DATA_TEXTURE_FORMAT_TYPE( pCTRTexture->GetFormat() );
			textureCommand[7] = PICA_CMD_HEADER_SINGLE( PICA_REG_TEXTURE0_FORMAT );
				
			GFGL::AddCommandCopy( textureCommand, sizeof(textureCommand) );//@todo 直接書き込みたい
		}
		shaderStateMap.m_Uv0Enable = true;
	}
	rRenderState.SetShaderStateMap( shaderStateMap );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
b32 FullPowerEffectAuraShaderDriver::SetPolygonDrawOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  //if( !pMeshDrawTag->IsVisible() ){ return true; }
  if( !m_pAuraMdlManager ){ return true; }
  FullPowerEffectAuraMdlManager::EffectMdl* pEffectMdl;
  FullPowerEffectAuraMdlManager::OverwriteTag* pOvTag;
  m_pAuraMdlManager->GetParam( pMeshDrawTag, &pEffectMdl, &pOvTag );
  if( !pOvTag ){ return true; }
  if( !pOvTag->numVtx ){ return true; }
  if( pOvTag->isDrawable == false ){ return true; }

	nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	bit32* cmd = pCommandBuffer->GetCurrentPtr();

  // status
  nn::math::VEC4 status;
  status.x = pEffectMdl->status.auraScaleX;    // ポリゴンサイズXスケール値
  status.y = pEffectMdl->status.auraScaleY;    // ポリゴンサイズYスケール値
  status.z = (f32)pEffectMdl->status.density;
  status.w = 0.0f;
  // 未使用の定数レジスタを流用する
	cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 7, status );

  // color
  nn::math::VEC4 color;
  color.x = pEffectMdl->status.auraColorR;
  color.y = pEffectMdl->status.auraColorG;
  color.z = pEffectMdl->status.auraColorB;
  color.w = 1.0f;
  // 未使用の定数レジスタを流用する
	cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 8, color );

  // timer
  nn::math::VEC4 data;
  data.x = nn::math::SinIdx( m_CBuffer.timer * 0x100 );
  data.y = (f32)(m_CBuffer.timer % pEffectMdl->status.density);
  data.z = 0.7f;   // ポリゴン中心より上方向のLength割合
  data.w = 0.3f;   // ポリゴン中心より下方向のLength割合
  // 未使用の nw_texture_translate定数レジスタを流用する
	cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 9, data );

  // ビルボード行列
  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 82, m_CBuffer.mtxView );

  pCommandBuffer->Put(cmd);

  GFGL::AddCommandJump( pOvTag->comBufferEnable,  pOvTag->comBufferEnableSize );
  GFGL::AddCommandJump( pOvTag->comBufferDraw,    pOvTag->comBufferDrawSize );
  GFGL::AddCommandJump( pOvTag->comBufferDisable, pOvTag->comBufferDisableSize );

  return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//  オーラエフェクト専用モデル管理クラス
//
//  オーラはモデルの頂点を基準に発生させることにする（アニメーションなどでの追従を考えて）
//  そのため頂点一つにつき一回シェーダーを通ってくれば良いが
//  通常モデルデータはインデックス式に頂点を複数回参照する可能性があり、無駄な処理負荷およびポリゴンの重なりが発生する
//  これを回避するため、元モデルから座標関連を中心に必要なデータを抽出し、
//  オーラそれぞれの固有パラメータも含んだ独自の頂点バッファ、インデックスバッファを準備、描画コマンドを作成する
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraMdlManager::FullPowerEffectAuraMdlManager()
{
  m_pAllocator = NULL; 

  // 専用モデル構造体初期化
  for( int i=0; i<EFFECT_RESIST_MAX; ++i ){ _initEffectMdl( &m_effectMdl[i] ); }

  // とりあえずの乱数
  random.Initialize();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraMdlManager::~FullPowerEffectAuraMdlManager()
{
  for( int i=0; i<EFFECT_RESIST_MAX; ++i ){ _cleanupEffectMdl( &m_effectMdl[i] ); }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectAuraMdlManager::SetAllocator( gfl2::gfx::IGLAllocator* pAllocator )
{
  m_pAllocator = pAllocator; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 構造体初期化 & クリーンアップ
void FullPowerEffectAuraMdlManager::_initEffectMdl( FullPowerEffectAuraMdlManager::EffectMdl* pEffectMdl )
{
  pEffectMdl->arrayTag = NULL;
  pEffectMdl->numTag = 0;
  pEffectMdl->pMdlNode = NULL;
  pEffectMdl->isEnable = false;

  mdlStatusSetting initStatus;
  pEffectMdl->status = initStatus;
}

void FullPowerEffectAuraMdlManager::_cleanupEffectMdl( FullPowerEffectAuraMdlManager::EffectMdl* pEffectMdl )
{
  for( int idxFace=0; idxFace<pEffectMdl->numTag; ++idxFace ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxFace ];

    GflHeapSafeFreeMemory( pTag->comBufferEnable );
    GflHeapSafeFreeMemory( pTag->comBufferDraw );
    GflHeapSafeFreeMemory( pTag->comBufferDisable );

    GflHeapSafeFreeMemory( pTag->vtxBuffer );
    GflHeapSafeFreeMemory( pTag->idxBuffer );
  }
  GflHeapSafeFreeMemory( pEffectMdl->arrayTag );

  _initEffectMdl( pEffectMdl );
}

//-----------------------------------------------
// エフェクト用モデル作成
//  該当ノードに関連付けしながらモデルデータを作成
void FullPowerEffectAuraMdlManager::CreateEffectMdl
  ( gfl2::heap::HeapBase *heap, gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus  )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  // 登録場所の検索
  EffectMdl* pEffectMdl = NULL;

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    if( m_effectMdl[ idxMdl ].pMdlNode == pMdlNode ){ return; } // 二重登録回避
  }

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    if( m_effectMdl[ idxMdl ].pMdlNode == NULL ){
      pEffectMdl = &m_effectMdl[ idxMdl ];
      break;
    }
  }
  if( pEffectMdl == NULL ){ return; } // 登録オーバーエラー

  // ノード登録
  pEffectMdl->pMdlNode = pMdlNode;
  pEffectMdl->isEnable = true;

  // ステータス保存
  pEffectMdl->status = *pStatus;

  // ノードからMeshTag情報を取り出し、構造体の初期設定
  pEffectMdl->numTag = pEffectMdl->pMdlNode->GetDrawTagNum();
  pEffectMdl->arrayTag = (OverwriteTag*)( GflHeapAllocMemoryAlign(heap,sizeof(OverwriteTag) * pEffectMdl->numTag, 128 ) );
  //ARIIZUMI_PRINT("FP_Aura Tag[%d * %d + 128*4*3  = %d]\n",pEffectMdl->numTag,sizeof(OverwriteTag),sizeof(OverwriteTag) * pEffectMdl->numTag + (128*4*3) );

  // ワークエリア初期化
  for( int i = 0; i < pEffectMdl->numTag; ++i ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[i];

    // デバイスメモリからアロケートする
    pTag->comBufferEnable = (u32*)( GflHeapAllocMemoryAlign(heap,128 * sizeof(u32), 128 ) );
    pTag->comBufferDraw   = (u32*)( GflHeapAllocMemoryAlign(heap,128 * sizeof(u32), 128 ) );
    pTag->comBufferDisable = (u32*)( GflHeapAllocMemoryAlign(heap,128 * sizeof(u32), 128 ) );
    for( int j=0; j<128; ++j ){ pTag->comBufferEnable[j] = pTag->comBufferDraw[j] = pTag->comBufferDisable[j] = 0; }

    pTag->comBufferEnableSize = pTag->comBufferDrawSize = pTag->comBufferDisableSize = 0;
    pTag->numVtx = 0;
    pTag->vtxBuffer = pTag->idxBuffer = NULL;
    pTag->pMeshDrawTag = NULL;
  }

  // MeshTagから頂点バッファ情報を取り出す
  for( u32 idxTag = 0; idxTag < pEffectMdl->numTag; ++idxTag ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxTag ];

    pTag->pMeshDrawTag = pEffectMdl->pMdlNode->GetMeshDrawTag(idxTag);
    pTag->isDrawable = checkDrawableShader( pTag->pMeshDrawTag, &pStatus->m_disableMaterialInfo );

    if( pTag->isDrawable == true ){
      b32 f = _cleateBufferData( heap ,pTag, pEffectMdl->status.density );
      if( f == true ){
        _cleateCommandData( pTag );
      } else {
        pTag->isDrawable = false;  // 無効化
      }
    }
	}
}

//-----------------------------------------------
// エフェクト用モデルを描画対象からはずす
void FullPowerEffectAuraMdlManager::RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      pEffectMdl->isEnable = false;
      return;
    }
  }
}

//-----------------------------------------------
// エフェクト用モデル削除
//  該当ノードに関連付けされたモデルデータを削除
void FullPowerEffectAuraMdlManager::DeleteEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      _cleanupEffectMdl( pEffectMdl );
      return;
    }
  }
}

//-----------------------------------------------
// 頂点入力定義データ
enum { VTX_ELEM_POS = 0, VTX_ELEM_COL, VTX_ELEM_UV0, VTX_ELEM_JID, VTX_ELEM_JWT, VTX_ELEM_NUM };
struct VSINPUT_ELEM { char* name; PicaDataVertexAttrType format; u8 dummy[3]; }; 
static const VSINPUT_ELEM vtxInputElemTbl[] = {
  { "aPosition.xyz",  PICA_DATA_SIZE_3_FLOAT,         {0,0,0} },
  { "aColor",         PICA_DATA_SIZE_4_FLOAT,         {0,0,0} },
  { "aTexCoord0.xy",  PICA_DATA_SIZE_2_FLOAT,         {0,0,0} },
  { "aBoneIndex",     PICA_DATA_SIZE_4_UNSIGNED_BYTE, {0,0,0} },
  { "aBoneWeight",    PICA_DATA_SIZE_4_UNSIGNED_BYTE, {0,0,0} }
};

//-----------------------------------------------
// エフェクト用モデルの頂点バッファ、インデックスバッファを作成
b32 FullPowerEffectAuraMdlManager::_cleateBufferData( gfl2::heap::HeapBase *heap ,OverwriteTag* pTag, u32 density )
{
  const scenegraph::resource::MeshResourceNode::FaceData* pFaceData = pTag->pMeshDrawTag->GetFaceData();

  // 頂点バッファ情報を取得
  u32 numVtx = pFaceData->m_VertexNum;
  u32 bufSizeVtx = pFaceData->GetVertexBufferSize(); 
  u32 elemSize = bufSizeVtx / numVtx; 
  u32 bufferAddress = (u32)pFaceData->GetVertexBuffer();
      
  if( numVtx * elemSize != bufSizeVtx ){
    bufSizeVtx -= 8;  // コマンドpadding補正分削除 
    elemSize = bufSizeVtx / numVtx; 

    if( numVtx * elemSize != bufSizeVtx ){ return false; }  // データ異常とみなす
  }
#if 0
  GFL_PRINT(" Tag[%d] 頂点バッファ情報\n", (u32)pTag);
  GFL_PRINT(" 頂点数[%d], 頂点バッファサイズ[%d], 1頂点当たりのデータサイズ[%d]\n", numVtx, bufSizeVtx, elemSize);
#endif

  // 頂点バッファの解析
  // ジョイントデータが固定化されているかどうかを判定
  AnalizeVertexInputData analizeVtx;
  analizeVtx.Analize( pTag->pMeshDrawTag );

  // ジョイントIndexの要素番号は7
  u32 jID0 = (u32)(analizeVtx.m_vtxInputData[7].data[0]);
  u32 jID1 = (u32)(analizeVtx.m_vtxInputData[7].data[1]);
  u32 jID2 = (u32)(analizeVtx.m_vtxInputData[7].data[2]);
  u32 jID3 = (u32)(analizeVtx.m_vtxInputData[7].data[3]);
  u32 JointIdxFixedData = (jID3<<24) + (jID2<<16) + (jID1<<8) + (jID0);

  // ジョイントWeightの要素番号は8
  u32 jWT0 = (u32)(analizeVtx.m_vtxInputData[8].data[0]);
  u32 jWT1 = (u32)(analizeVtx.m_vtxInputData[8].data[1]);
  u32 jWT2 = (u32)(analizeVtx.m_vtxInputData[8].data[2]);
  u32 jWT3 = (u32)(analizeVtx.m_vtxInputData[8].data[3]);
  u32 JointWeightFixedData = (jWT3<<24) + (jWT2<<16) + (jWT1<<8) + (jWT0);

  enum {
    JOINT_NO_FIX = 0,
    JOINT_ID_FIX = 0x0001,
    JOINT_WT_FIX = 0x0002,
  };
  int isJointFixed = 0;

  if( analizeVtx.m_vtxInputDataFixedMask & 0x0080 ){ isJointFixed |= JOINT_ID_FIX; }
  if( analizeVtx.m_vtxInputDataFixedMask & 0x0100 ){ isJointFixed |= JOINT_WT_FIX; }
#if 0
  switch( isJointFixed ){
  case 0:
    GFL_PRINT( " Tag[%d] Joint is No Fixed\n\n", pTag );
    break;
  case 1:
    GFL_PRINT( " Tag[%d] Joint ID is Fixed\n\n", pTag );
    break;
  case 2:
    GFL_PRINT( " Tag[%d] Joint Weight is Fixed\n\n", pTag );
    break;
  case 3:
    GFL_PRINT( " Tag[%d] Joint All Data is Fixed\n\n", pTag );
    break;
  }
#endif
  // 頂点バッファの作成
  // 理想を言えばオブジェクト単位で頂点の重複を排除し、同一地点にエミッターが置かれるのは回避したいが
  // メッシュデータの構造上、手間と結果において割が合わないので、今はやらない
  struct VertexBufferData {
    gfl2::math::Vector3 pos;
    gfl2::math::Vector4 param0;
    gfl2::math::Vector2 param1;
    u32                 jointIdx;
    u32                 jointWeight;
  };
  VertexBufferData* vtxBufferData = (VertexBufferData*)( GflHeapAllocMemoryAlign(heap,numVtx * sizeof(VertexBufferData), 128 ) );
  //ARIIZUMI_PRINT("FP_Aura vtx[%d * (%d+2) = %d]\n",numVtx,sizeof(VertexBufferData),(sizeof(VertexBufferData)+2)* numVtx );
  {
    for( int idxVtx=0; idxVtx<numVtx; ++idxVtx ){
      VertexBufferData* pVtx = &vtxBufferData[ idxVtx ];

      // 頂点データ格納アドレスの計算
      u32 vtxDataAddrres = bufferAddress + elemSize * idxVtx;

      // 位置座標の取得
      float* pPos = (float*)( vtxDataAddrres );                     // 位置座標は先頭に格納されている
      pVtx->pos = gfl2::math::Vector3( pPos[0], pPos[1], pPos[2] );

      // ジョイントデータの取得
      u32 *pJointIdxData, *pJointWeightData;
      switch( isJointFixed ){
      case 0: // No_fix
        pJointIdxData = (u32*)( vtxDataAddrres + elemSize - 8 );     // Offsetは 最後尾 u8*4(ID)+u8*4(weight) の8byte分を引いた位置     
        pJointWeightData = (u32*)( vtxDataAddrres + elemSize - 4 );  // Offsetは 最後尾 u8*4(weight) の4byte分を引いた位置
        break;
      case 1: // Idx fix
        pJointIdxData = &JointIdxFixedData;
        pJointWeightData = (u32*)( vtxDataAddrres + elemSize - 4 );  // Offsetは 最後尾 u8*4(weight) の4byte分を引いた位置
        break;
      case 2: // Weight fix
        pJointIdxData = (u32*)( vtxDataAddrres + elemSize - 4 );     // Offsetは 最後尾 u8*4(ID) の4byte分を引いた位置     
        pJointWeightData = &JointWeightFixedData;
        break;
      case 3: // Idx & Weight fix
        pJointIdxData = &JointIdxFixedData;
        pJointWeightData = &JointWeightFixedData;
        break;
      }
      pVtx->jointIdx = *pJointIdxData;
      pVtx->jointWeight = *pJointWeightData;

      // 固有パラメータの設定        
      f32 param0_x = (f32)random.GenerateRandomN( 100 )/200.0f + 0.5f;  // スケール値( 0.5f ～ 1.0f )
      f32 param0_y = (f32)random.GenerateRandomN( density );        // 発生オフセット
      f32 param0_z = ( random.GenerateRandomU32() & 1 )? 0 : 1; // 反転フラグ
      f32 param0_w = 0;
      f32 param1_x = 0;
      f32 param1_y = 0;

      pVtx->param0 = gfl2::math::Vector4( param0_x, param0_y, param0_z, param0_w );
      pVtx->param1 = gfl2::math::Vector2( param1_x, param1_y );
    }
  }
  pTag->vtxBuffer = (u8*)vtxBufferData;

  // インデックスバッファの作成
  // インデックスバッファは必須かつPointListモードが存在しないが、TriangleStripで頂点数分のリストとすることで代用
  // ※ジオメトリシェーダーの方でPointListとして解釈することは可能
  u16* idxBufferData = (u16*)( GflHeapAllocMemoryAlign(heap,numVtx * sizeof(u16), 128 ) );
  {
    for( u32 i=0; i<numVtx; ++i ){ idxBufferData[i] = (u16)i; }  // 頂点Indexを指し示す単純な配列
  }
  pTag->idxBuffer = (u8*)idxBufferData;

  pTag->numVtx = numVtx;

  return true;
}

//-----------------------------------------------
// エフェクト用モデルの描画コマンドを作成
b32 FullPowerEffectAuraMdlManager::_cleateCommandData( OverwriteTag* pTag )
{
  // 任天堂Apiを利用してコマンド作成
  nn::gr::CTR::Vertex::InterleaveInfo		interleaveInfo;
	nn::gr::CTR::Vertex										grVertex;
	nn::gr::CTR::BindSymbolVSInput        vsInput[ VTX_ELEM_NUM ];
	nn::gr::CTR::Vertex::IndexStream			indexStream;

  // バッファポインタを物理アドレスに変換
  uptr vaddr = nngxGetPhysicalAddr( (u32)pTag->vtxBuffer );
  uptr iaddr = nngxGetPhysicalAddr( (u32)pTag->idxBuffer );

  // 頂点入力要素データ設定
  for( int i=0; i<VTX_ELEM_NUM; ++i ){
	  vsInput[i].start = vsInput[i].end = i;
    vsInput[i].name = vtxInputElemTbl[i].name;
	  interleaveInfo.symbol[i] = &vsInput[i];
    interleaveInfo.dataType[i] = vtxInputElemTbl[i].format;
  }

  interleaveInfo.dataNum = VTX_ELEM_NUM;
  grVertex.EnableInterleavedArray( interleaveInfo, vaddr );  // 頂点バッファアドレスの設定

	indexStream.physicalAddr = iaddr;  // インデックスバッファアドレスの設定
	indexStream.drawVtxNum = pTag->numVtx;
  indexStream.isUnsignedByte = false;

  u32* pStart;
  u32* pTail;

  // EnableAttrCommand
  pStart = pTag->comBufferEnable;
  pTail = grVertex.MakeEnableAttrCommand( pStart );
  pTag->comBufferEnableSize = _packCommand( pStart, pTail );

  // DrawCommand
  pStart = pTag->comBufferDraw;
  pTail = grVertex.MakeDrawCommand( pStart, indexStream );
  pTag->comBufferDrawSize = _packCommand( pStart, pTail );

  // DisableAttrCommand
  pStart = pTag->comBufferDisable;
  pTail = grVertex.MakeDisableAttrCommand( pStart );
  pTag->comBufferDisableSize = _packCommand( pStart, pTail );

  return true;
}

//-----------------------------------------------
// 描画コマンドの整形
u32 FullPowerEffectAuraMdlManager::_packCommand( u32* pCommandStart, u32* pCommandEnd )
{
  // コマンドサイズ算出
	u32 comLength = (u32)pCommandEnd - (u32)pCommandStart;

  u32 wp = comLength / sizeof(u32); // 終端書き込み位置
  //assert( (comLength % 8) == 0 ); // 本当はAlignチェックした方がいいが、普通は正しく出てくるはずなので省略
        
  // Alignment用dummy書き込み
  if( (comLength % 16) == 0 ){
    pCommandStart[wp] = pCommandStart[wp + 1] = 0;

    wp += 2;        // 書き込み位置オフセット進行
    comLength += 8; // u32*2 = 8byte分増加
  }

  // returnコマンドの挿入
  u32		returnCommand[] = { 0x00000001, 0x000f023d };

  pCommandStart[wp]     = returnCommand[0];
  pCommandStart[wp + 1] = returnCommand[1];

  comLength += 8; // u32*2 = 8byte分増加

  return comLength; 
}

//-----------------------------------------------
// 上書き用タグ取得
//  該当タグに対して差し替えるデータ構造体を取得
void FullPowerEffectAuraMdlManager::GetParam( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag,
                                              FullPowerEffectAuraMdlManager::EffectMdl** ppOutEffectMdl,
                                              FullPowerEffectAuraMdlManager::OverwriteTag** ppOutTag )
{
  *ppOutEffectMdl = NULL;
  *ppOutTag = NULL;

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];
      
    if( pEffectMdl->isEnable == true ){
      for( u32 idxTag = 0; idxTag < pEffectMdl->numTag; ++idxTag ){
        OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxTag ];

        if( pMeshDrawTag == pTag->pMeshDrawTag ){
          *ppOutEffectMdl = pEffectMdl;
          *ppOutTag = pTag;
        }
	    }
    }
  }
}

//-----------------------------------------------
// ステータス変更
void FullPowerEffectAuraMdlManager::ChangeStatus
  ( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode, FullPowerEffectAuraMdlManager::mdlStatusSetting* pStatus  )
{
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];

    if( pMdlNode == pEffectMdl->pMdlNode ){
      u32 density = pEffectMdl->status.density; // 密度はモデル作成時の設定に影響されるので、変更不可

      pEffectMdl->status = *pStatus;
      pEffectMdl->status.density = density;
      return;
    }
  }
}





//===================================================================================================================================================
//
//
//
//  頂点要素解析クラス
//
//  頂点要素(index)が固定化されているかどうかを解析し、固定データを取得
//
//  ちなみにポケモンモデルは
//  頂点要素0 : 座標(3)
//  頂点要素1 : 法線(3)
//  頂点要素2 : 接線(3)
//  頂点要素3 : 頂点カラー(4)
//  頂点要素4 : uv0(2)
//  頂点要素5 : マルチテクスチャ用uv1(2)
//  頂点要素6 : マルチテクスチャ用uv2(2)
//  頂点要素7 : スキンアニメ用ボーンIndex(4)
//  頂点要素8 : スキンアニメ用ボーンWeight(4)
//
//  ()内は要素数
//
//  となっている。
//
//
//===================================================================================================================================================
AnalizeVertexInputData::AnalizeVertexInputData()
{
}

AnalizeVertexInputData::~AnalizeVertexInputData()
{
}

void AnalizeVertexInputData::Analize( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag )
{
  // 解析データ初期化
  for( u32 i=0; i<16; ++i ){
    m_vtxInputData[i].isFixed = false;
    m_vtxInputData[i].data[0] = m_vtxInputData[i].data[1] = m_vtxInputData[i].data[2] = m_vtxInputData[i].data[3] = 0;
  }
  m_vtxInputDataFixedMask = 0;

  // MeshDrawTagよりDrawEnableコマンドを取得
  // DrawEnableコマンドを解析して判定
  const scenegraph::resource::MeshResourceNode::FaceData* pFaceData = pMeshDrawTag->GetFaceData();
  u32*  commEnable = (u32*)( pFaceData->GetCommand( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrEnable ) );
  u32   sizeEnable = pFaceData->GetCommandSize( scenegraph::resource::MeshResourceNode::FaceData::CommandType::VertexAttrEnable );
  u32   sizeLoop = sizeEnable / sizeof(u32);

  //  2BBおよび2BCレジスタに対してのコマンド発行命令[0x000f02bb][0x000f02bc]を目安に、その直前のu32データ(配列データ)を取得。
  //
  //  [0x00f02bb] 0～ 3bit 配列index0
  //  [0x00f02bb] 4～ 7bit 配列index1
  //  [0x00f02bb] 8～11bit 配列index2
  //  [0x00f02bb]12～15bit 配列index3
  //  [0x00f02bb]16～19bit 配列index4
  //  [0x00f02bb]20～23bit 配列index5
  //  [0x00f02bb]24～27bit 配列index6
  //  [0x00f02bb]28～31bit 配列index7
  //  [0x00f02bc] 0～ 3bit 配列index8
  //  [0x00f02bc] 4～ 7bit 配列index9
  //  [0x00f02bc] 8～11bit 配列index10
  //  [0x00f02bc]12～15bit 配列index11
  //  [0x00f02bc]16～19bit 配列index12
  //  [0x00f02bc]20～23bit 配列index13
  //  [0x00f02bc]24～27bit 配列index14
  //  [0x00f02bc]28～31bit 配列index15
  //
  //  ※0番目にコマンド発行命令があることはないので1番目のデータから確認
  u32 tbl2BB = 0;
  u32 tbl2BC = 0;
  for( u32 i=1; i<sizeLoop; ++i ){
    // 設定されていない状態は考えられないので存在確認まではしない
    if( commEnable[i] == 0x000f02bb ){ tbl2BB = commEnable[i-1]; }
    if( commEnable[i] == 0x000f02bc ){ tbl2BC = commEnable[i-1]; }
  }

  u8  vtxIndexTbl[16] = { (tbl2BB>> 0)&0x0f, (tbl2BB>> 4)&0x0f, (tbl2BB>> 8)&0x0f, (tbl2BB>>12)&0x0f,
                          (tbl2BB>>16)&0x0f, (tbl2BB>>20)&0x0f, (tbl2BB>>24)&0x0f, (tbl2BB>>28)&0x0f,
                          (tbl2BC>> 0)&0x0f, (tbl2BC>> 4)&0x0f, (tbl2BC>> 8)&0x0f, (tbl2BC>>12)&0x0f,
                          (tbl2BC>>16)&0x0f, (tbl2BC>>20)&0x0f, (tbl2BC>>24)&0x0f, (tbl2BC>>28)&0x0f };

  //  232レジスタに対してのコマンド発行命令[0x803f0232]を目安に、その直前のu32データ(固定化されている要素の上記配列内index)を取得。
  //  ※0番目にコマンド発行命令があることはないので1番目のデータから確認
  for( u32 i=1; i<sizeLoop; ++i ){
    if( commEnable[i] == 0x803f0232 ){
      u8 arrayIdx = commEnable[i-1];
      u8 elemIdx = vtxIndexTbl[arrayIdx];

      u32 data0 = commEnable[i+1];
      u32 data1 = commEnable[i+2];
      u32 data2 = commEnable[i+3];

      u32 bit24_0 = (data2 & 0x00ffffff);
      u32 bit24_1 = ((data1<<8) & 0x00ffff00) | ((data2>>24) & 0x000000ff);
      u32 bit24_2 = ((data0<<16) & 0x00ff0000) | ((data1>>16) & 0x0000ffff);
      u32 bit24_3 = ((data0>>8) & 0x00ffffff);

      VtxInputData* pElem = &m_vtxInputData[ elemIdx ];
      pElem->isFixed = true;
      pElem->data[0] = nn::util::Float24::Bits24ToFloat32( bit24_0 );
      pElem->data[1] = nn::util::Float24::Bits24ToFloat32( bit24_1 );
      pElem->data[2] = nn::util::Float24::Bits24ToFloat32( bit24_2 );
      pElem->data[3] = nn::util::Float24::Bits24ToFloat32( bit24_3 );
#if 0
      GFL_PRINT(" 頂点固定要素Index[%d]\n", elemIdx);
      GFL_PRINT(" 固定データ [%f], [%f], [%f], [%f]\n", pElem->data[0], pElem->data[1], pElem->data[2], pElem->data[3]);
#endif
    }
  }

  //  マスクデータ作成
  for( u32 i=0; i<16; ++i ){
    m_vtxInputDataFixedMask += (m_vtxInputData[i].isFixed << i);
  }
}


#endif //USE_AURA_SHADER

