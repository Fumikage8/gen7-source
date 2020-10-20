

#include <math/include/gfl2_Math.h>
#include <gfx/include/gfl2_GFGLUtil.h>

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

#include "System/include/RenderPath/FullPowerEffect.h"

#if USE_AURA_SHADER

#include <nn/fs.h>
#include <nn/hio.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

nn::math::MersenneTwister random;

//===================================================================================================================================================
// 
//  ローカル関数
//
//===================================================================================================================================================
struct LocalFunc {

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  ファイル読み込み
//---------------------------------------------------------------------------------------------------------------------------------------------------
  static void *ReadFile( const char* pFilePath )
  {//自前データロード
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();
    void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
    file.Read(pShaderBinary, fileSize);
		return pShaderBinary;
  }
};



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
	m_DrawableNodeContainer.CreateBuffer( 6 );  // 例えばポケモン６体分
  m_timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath0::~FullPowerEffectRenderPath0()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象に登録
void FullPowerEffectRenderPath0::AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象から解除
void FullPowerEffectRenderPath0::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
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

	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
	GFGL::BeginScene();

  {
    const IShaderDriver* pShader = &m_FullPowerEffectRimShaderDriver;
	  m_pDrawManager->SetOverrideShader( pShader );

    // 固有定数バッファの設定
    FullPowerEffectRimShaderDriver::CBuffer cBuffer;
    cBuffer.timer = m_timer;
    m_timer++;

    m_FullPowerEffectRimShaderDriver.SetCBuffer( &cBuffer );
    
    nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
	    // proj
		  nn::math::MTX44* pProj;
		  pProj = (nn::math::MTX44*)&(m_pDrawManager->GetProjectionMatrix());
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pProj );
	    // view
		  nn::math::MTX34* pView;
		  pView = (nn::math::MTX34*)&(m_pDrawManager->GetViewMatrix());

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
FullPowerEffectRimShaderDriver::FullPowerEffectRimShaderDriver() : GLMemory(), IShaderDriver()
{
  m_Color.Set(1.0f, 1.0f, 0.8f, 1.0f);

  // リソースセットアップ
  //
  //  ModelConverterを通して「簡易モデル＋シェーダー」の形で組み込むのが定番だが
  //  ここでは実験的に、直接シェーダーを差し替える手段を用いてみる
  {
    // モデルデータをセットアップ
    //  形として必要なだけなので中身は簡単なもの。
    //  ここでは法線マップ出力用のモデルを流用する。
    scenegraph::resource::GfBinaryMdlData binMdlData;

    m_pMdlBin = LocalFunc::ReadFile( "rom:/data/test/aura_shader/EdgeMapVColA.ctr.gfbmdl" );
	  binMdlData.SetModelData( (c8*)m_pMdlBin );
	  m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &binMdlData );	

    // 上記モデルのシェーダーを差し替える
    //
    //  上記モデルは
    //    ・Default.ctr.gfbvsh
    //    ・NormalToUv.ctr.gfbvsh
    //    ・Comb2_Manual.ctr.gfbfsh
    //  で成り立っているので
    //  それらの名前で成り済ます(重要なのは名前から生成されるハッシュ値)
	  scenegraph::resource::GfBinaryShdData binShdData0, binShdData1, binShdData2;

    m_vsh0 = new CtrGfbvshDummy;
    m_vsh0->Setup( "rom:/data/test/aura_shader/rimEffect.shbin", "Default", false );
    binShdData0.SetShaderData( m_vsh0->m_CtrGfbvsh );
    scenegraph::resource::ResourceNode* pBinShdNode0 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData0 );
	  m_pBinModelRootNode->AddChild( pBinShdNode0 );

    m_vsh1 = new CtrGfbvshDummy;
    m_vsh1->Setup( "rom:/data/test/aura_shader/rimEffect.shbin", "NormalToUv", false );
    binShdData1.SetShaderData( m_vsh1->m_CtrGfbvsh );
    scenegraph::resource::ResourceNode* pBinShdNode1 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData1 );
	  m_pBinModelRootNode->AddChild( pBinShdNode1 );

    m_fsh = new CtrGfbfshDummy;
    SetFshStatus(); // フラグメントシェーダー設定
    m_fsh->Setup( "Comb2_Manual" );				
    binShdData2.SetShaderData( m_fsh->m_CtrGfbfsh );
    scenegraph::resource::ResourceNode* pBinShdNode2 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData2 );
	  m_pBinModelRootNode->AddChild( pBinShdNode2 );

    // 上記モデルのテクスチャ読み込み
    //  成り済ましが目的で参照はしないのでなんでもOK
    scenegraph::resource::GfBinaryTexData binTexData;

    m_pTexBin = LocalFunc::ReadFile( "rom:/data/test/aura_shader/NormalTexture.ctr.btex" );
    binTexData.SetTextureData( (c8*)m_pTexBin );
    scenegraph::resource::ResourceNode* pBinTexNode = scenegraph::resource::ResourceManager::CreateResourceNode( &binTexData );
	  m_pBinModelRootNode->AddChild( pBinTexNode );
  }

  // 必要なリソースを設定後、このモデルのインスタンスを作成してシェーダーをぶら下げておく
  // (リソースに欠けているもの、整合性が合わないものがあれば、ここでフリーズ)
  m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );

  // このモデル自体はシェーダー保管庫にすぎないので、シーンからはずして描画はOffにしておく
	scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
	m_pModelInstanceNode->UpdateDrawTag( info );
	scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );

  // 固有定数バッファ初期化
  m_CBuffer.timer = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRimShaderDriver::~FullPowerEffectRimShaderDriver()
{
	GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );
  GLMemory::Free(m_pMdlBin);
  GLMemory::Free(m_pTexBin);

  delete m_fsh;
  delete m_vsh1;
  delete m_vsh0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 固有定数バッファ設定
void FullPowerEffectRimShaderDriver::SetCBuffer( FullPowerEffectRimShaderDriver::CBuffer* pData )
{
  if( !pData ){ return; }

  m_CBuffer = *pData;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// シェーダー適用
b32 FullPowerEffectRimShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
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

  // このシェーダードライバ固有の定数レジスタを設定(レジスタ上書きを見越してここでやる)
  {
	  nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
	    // timer
      nn::math::VEC4 data;
      data.x = nn::math::SinIdx( m_CBuffer.timer * 0x0800 )*2.0f + 1.0f;
      //data.x = nn::math::SinIdx( m_CBuffer.timer * 0x0800 )*0.5f - 1.0f;
      data.y= 0.0f;
      data.z= 0.0f;
      data.w= (f32)(m_CBuffer.timer%64);
      // 未使用の nw_texture_translate定数レジスタを流用する
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 9, data );
    }
    pCommandBuffer->Put(cmd);
  }

  // このモデルのインスタンスからぶら下げておいたシェーダーを取り出して有効化
  MeshDrawTag								*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	CombinerCtrShaderDriver		*pShaderDriver = (CombinerCtrShaderDriver*)( pTag->GetShaderDriver() );
	{
		setUpEnable.value = 0;
		setUpEnable.VertexShader = true;
		setUpEnable.GeometryShader = true;
		setUpEnable.FragmentShader = true;
		setUpEnable.UvSetting = true;
		rRenderState.SetSetUpEnable( setUpEnable );

		pShaderDriver->Apply( rRenderState, pMeshDrawTag );

    // コンスタントカラーやレンダーステートなど上書きされてしまうので
    // 処理のロスはあるがここで再度コマンド転送
    // 負荷が問題用になるようなら考える
    const scenegraph::resource::ShaderResourceNode* pFsh = pShaderDriver->GetPixelShader();
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( pFsh->GetFragmentShader() ), pFsh->GetFragmentShaderSize() );
	}

  // 本来の設定に復帰
	rRenderState.SetSetUpEnable( origSetUpEnable );

  return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// フラグメントシェーダー設定
void FullPowerEffectRimShaderDriver::SetFshStatus( void )
{
  // コンスタントカラー設定
  m_fsh->m_convinerData.constantCol[0].setColor( 1, 1, 1, 1 );
  m_fsh->m_convinerData.constantCol[1].setColor( m_Color.x,m_Color.y,m_Color.z,m_Color.w ); //色
  m_fsh->m_convinerData.constantCol[2].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[3].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[4].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[5].setColor( 1, 1, 1, 0 );

  // その他カラー設定
  m_fsh->m_convinerData.bufferCol.setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[0].setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[1].setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[2].setColor( 0, 0, 0, 1 );

  // コンバイナステージ設定
  // idxInputBufferFlagの内容はコマンドの仕様とクリエイティブスタジオの仕様に違いがあるので注意
  //  コマンド仕様                : 前段のバッファを使用するか否か
  //  クリエイティブスタジオ仕様  : この段バッファを持ち越すか否か
  // なので結果的に内容にずれが生じ
  //  stage0(クリエイティブスタジオ)のフラグ→stage1(コマンド)のフラグ
  //  stage1(クリエイティブスタジオ)のフラグ→stage2(コマンド)のフラグ
  //  stage2(クリエイティブスタジオ)のフラグ→stage3(コマンド)のフラグ
  //  stage3(クリエイティブスタジオ)のフラグ→stage4(コマンド)のフラグ
  //  stage4(クリエイティブスタジオ)のフラグ→stage5(コマンド)のフラグ
  // となるのでここで吸収し、格納すること
  convinerStage* pStage;

  // ステージ0設定
  pStage = &m_fsh->m_convinerData.stage[0];

  pStage->constantColIdx = 1;
  pStage->color.idxMathFormula = CONV_MATH_FML::MODULATE;
  pStage->color.idxMathFormula = CONV_MATH_FML::REPLACE;
  pStage->color.idxScale = CONV_MATH_SCALE::VAL1;
  pStage->color.idxSourceA = CONV_MATH_SRC::CONSTANTCOL;
  pStage->color.idxOperandA = CONV_MATH_OPR_RGB::RGB;
  pStage->color.idxSourceB = CONV_MATH_SRC::PRIMARYCOL;
  pStage->color.idxOperandB = CONV_MATH_OPR_RGB::ALPHA;
  pStage->color.idxSourceC = CONV_MATH_SRC::CONSTANTCOL;
  pStage->color.idxOperandC = CONV_MATH_OPR_RGB::ALPHA;
  pStage->color.idxInputBufferFlag = CONV_BUFFERINPUT::PREVIOUSBUFFER;

  //pStage->alpha.idxMathFormula = CONV_MATH_FML::MODULATE;
  pStage->alpha.idxMathFormula = CONV_MATH_FML::REPLACE;
  pStage->alpha.idxScale = CONV_MATH_SCALE::VAL1;
  pStage->alpha.idxSourceA = CONV_MATH_SRC::PRIMARYCOL;
  pStage->alpha.idxOperandA = CONV_MATH_OPR_A::ALPHA;
  pStage->alpha.idxSourceB = CONV_MATH_SRC::CONSTANTCOL;
  pStage->alpha.idxOperandB = CONV_MATH_OPR_A::ALPHA;
  pStage->alpha.idxSourceC = CONV_MATH_SRC::CONSTANTCOL;
  pStage->alpha.idxOperandC = CONV_MATH_OPR_A::ONEMINUS_ALPHA;
  pStage->alpha.idxInputBufferFlag = CONV_BUFFERINPUT::PREVIOUSBUFFER;

  // レンダーステート設定
  m_fsh->m_renderStateData.brend.isEnable = true;
  m_fsh->m_renderStateData.brend.idxEqRGB = RDST_BLENDEQ::ADD;
  m_fsh->m_renderStateData.brend.idxFuncSrcRGB = RDST_BLENDFUNC::SRC_ALPHA;
  m_fsh->m_renderStateData.brend.idxFuncDstRGB = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;
  m_fsh->m_renderStateData.brend.idxEqA = RDST_BLENDEQ::ADD;
  m_fsh->m_renderStateData.brend.idxFuncSrcA = RDST_BLENDFUNC::SRC_ALPHA;
  m_fsh->m_renderStateData.brend.idxFuncDstA = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;

  m_fsh->m_renderStateData.alphaTest.isEnable = false;

  m_fsh->m_renderStateData.depthTest.isEnable = true;
  m_fsh->m_renderStateData.depthTest.idxFunc = RDST_DEPTHTEST::L_EQUAL;
  m_fsh->m_renderStateData.depthTest.isEnableWrite = true;

  m_fsh->m_renderStateData.stencilTest.isEnable = false;

  m_fsh->m_renderStateData.cullMode.isEnable = true;
  m_fsh->m_renderStateData.cullMode.mode = RDST_CULL::CCW;

  m_fsh->m_renderStateData.polyOffs.isEnable = false;
}

void FullPowerEffectRenderPath0::SetRimColor( gfl2::math::Vector4 &col )
{
  m_FullPowerEffectRimShaderDriver.m_Color = col;
  m_FullPowerEffectRimShaderDriver.m_fsh->m_convinerData.constantCol[1].setColor( col.x,col.y,col.z,col.w ); //色
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
	m_DrawableNodeContainer.CreateBuffer( 6 );  // 例えばポケモン６体分

  m_timer = 0;

  // とりあえずの乱数
  random.Initialize();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectRenderPath1::~FullPowerEffectRenderPath1()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象に登録
void FullPowerEffectRenderPath1::AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );

  m_auraMdlManager.CreateEffectMdl( pNode );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 指定ノードをレンダリング対象から解除
void FullPowerEffectRenderPath1::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );

  m_auraMdlManager.RemoveEffectMdl( pNode );
}

void FullPowerEffectRenderPath1::DeleteAuraModel( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_auraMdlManager.DeleteEffectMdl( pNode );
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
    nn::math::MTX34* pMtx34;
#if 0
		pMtx34 = (nn::math::MTX34*)&m_rotYMatInv;
#else
    math::Matrix44  mtxView = math::Matrix44( rDrawContext.m_View );
    math::Vector    col0 = math::Vector( mtxView.GetElem(0,0), mtxView.GetElem(1,0), mtxView.GetElem(2,0), mtxView.GetElem(3,0) );
    math::Vector    col1 = math::Vector( mtxView.GetElem(0,1), mtxView.GetElem(1,1), mtxView.GetElem(2,1), mtxView.GetElem(3,1) );
    math::Vector    col2 = math::Vector( mtxView.GetElem(0,2), mtxView.GetElem(1,2), mtxView.GetElem(2,2), mtxView.GetElem(3,2) );
    math::Vector    col3 = math::Vector( mtxView.GetElem(0,3), mtxView.GetElem(1,3), mtxView.GetElem(2,3), mtxView.GetElem(3,3) );
    math::Matrix    mtxBB = math::Matrix::GetBillboardMatrixRotationY( math::Matrix( col0, col1, col2, col3 ) );
    math::Vector4   row0 = math::Vector4( mtxBB.GetElem(0,0), mtxBB.GetElem(0,1), mtxBB.GetElem(0,2), mtxBB.GetElem(0,3) );
    math::Vector4   row1 = math::Vector4( mtxBB.GetElem(1,0), mtxBB.GetElem(1,1), mtxBB.GetElem(1,2), mtxBB.GetElem(1,3) );
    math::Vector4   row2 = math::Vector4( mtxBB.GetElem(2,0), mtxBB.GetElem(2,1), mtxBB.GetElem(2,2), mtxBB.GetElem(2,3) );
    math::Matrix34  mtxBB34 = math::Matrix34( row0, row1, row2 );

    pMtx34 = (nn::math::MTX34*)&mtxBB34;
#endif
    cBuffer.mtxView = *pMtx34;
    cBuffer.timer = m_timer;
    m_timer++;

    m_FullPowerEffectAuraShaderDriver.SetCBuffer( &cBuffer, &m_auraMdlManager );

    nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
	    // proj
		  nn::math::MTX44* pProj;
		  pProj = (nn::math::MTX44*)&(m_pDrawManager->GetProjectionMatrix());
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pProj );
	    // view
		  nn::math::MTX34* pView;
		  pView = (nn::math::MTX34*)&(m_pDrawManager->GetViewMatrix());

		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pView );
    }
    pCommandBuffer->Put(cmd);

    m_pDrawManager->Draw( false );
  }

  GFGL::EndScene();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Y軸ビルボード用行列設定
void FullPowerEffectRenderPath1::SetYBillboardMtx( math::Matrix34* rotYMatInv )
{
  //m_rotYMatInv = gfl2::math::Matrix34(gfl2::math::Vector4(1,0,0,0),gfl2::math::Vector4(0,1,0,0),gfl2::math::Vector4(0,0,1,0));
  m_rotYMatInv = *rotYMatInv;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// エフェクト用モデル作成
//  オーラはモデルの頂点を基準に発生させることにする（アニメーションなどでの追従を考えて）
//  そのため頂点一つにつき一回シェーダーを通ってくれば良いが
//  通常モデルデータはインデックス式に頂点を複数回参照する可能性があり、無駄な処理負荷およびポリゴンの重なりが発生する
//  これを回避するため、元モデルから座標関連を中心に必要なデータを抽出し、
//  オーラそれぞれの固有パラメータも含んだ独自の頂点バッファ、インデックスバッファを準備、描画コマンドを作成する
#if 0
//-----------------------------------------------
void FullPowerEffectRenderPath1::CreateEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  // 格納位置サーチ（暫定処理）
  EffectMdl* pEM = NULL;

  for( int i=0; i<6; ++i ){
    EffectMdl* p = &m_effectMdl[i];
    if( p->pNode == NULL ){ pEM = p; break; }
  }
  if( pEM == NULL ){ return; }

  // 関連付け（暫定）
  pEM->pNode = pNode;

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelOrg;
  pModelOrg = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  struct VertexList {
    u32   numVtx;         // 頂点数
    u32   elemSize;       // 頂点1つあたりのサイズ
    u32   bufferAddress;  // 頂点バッファにするデータのアドレス
    s32   isPadding;      // パディング領域の有無( -1 = error )
  };
  VertexList vtxDataList[32]; // 数は暫定
  int numVtxDataList = 0;
  u32 totalNumVtx = 0;

  // 元モデルから頂点バッファリストの作成
  {
    // 【 Mesh 】分のループ
    // ここでいう【 Mesh 】は一つの【 Object(pNode内モデルオブジェクト) 】を分割したもので
    // 主にジョイント最大数の制約を受けてポリゴン分割を行ったデータとなる
	  u32 numMesh = pModelOrg->GetMehsNum();

    for( u32 idxMesh=0; idxMesh<numMesh; ++idxMesh ){
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMdlLinkData;  
      pMdlLinkData = pModelOrg->GetMeshLinkData( idxMesh );

      // 【 Face 】分のループ
      // ここでいう【 Face 】は一つの【 Mesh 】を分割したもので
      // マテリアルの違いを受けてポリゴン分割を行ったデータとなる
      u32 numFace = pMdlLinkData->GetFaceNum();

      for( u32 idxFace=0; idxFace<numFace; ++idxFace ){
        const gfl2::renderingengine::scenegraph::resource::MeshResourceNode::FaceData* pFaceData;
        pFaceData = pMdlLinkData->GetFaceData( idxFace );

        // 【 Face 】それぞれから頂点バッファ情報を取得
        VertexList* pVD = &vtxDataList[ numVtxDataList ];

        pVD->numVtx = pFaceData->m_VertexNum;
        u32 bufSizeVtx = pFaceData->GetVertexBufferSize(); 
        pVD->elemSize = bufSizeVtx / pVD->numVtx; 
        pVD->bufferAddress = (u32)pFaceData->GetVertexBuffer();
      
        u32 dataSize = pVD->numVtx * pVD->elemSize;

        if( bufSizeVtx == dataSize )            { pVD->isPadding = 0; }   // パディングなし
        else if( (bufSizeVtx - dataSize) == 8 ) { pVD->isPadding = 1; }   // 正しいパディング
        else                                    { pVD->isPadding = -1; }  // おかしい

        numVtxDataList++;
        totalNumVtx += pVD->numVtx;
      }
    }
  }

  // 元モデルから頂点データを抽出
  struct VertexData {
    gfl2::math::Vector3 pos;          // 位置
    u32                 jointIdx;     // ジョイントIndex
    u32                 jointWeight;  // ジョイント重み
    bool                isMarge;      // マージ対象判別フラグ
    u8                  dummy[3];
  };
  VertexData* vtxData = (VertexData*)GLMemory::Malloc( totalNumVtx * sizeof(VertexData), 128 );
  {
    int wp = 0;

    for( int idxList=0; idxList<numVtxDataList; ++idxList ){
      VertexList* pVD = &vtxDataList[idxList];
    
      for( int idxVtx=0; idxVtx<pVD->numVtx; ++idxVtx ){
        // 頂点データ格納アドレスの計算
        u32 vtxDataAddrres = pVD->bufferAddress + pVD->elemSize * idxVtx;

        // 取得データ格納アドレスの計算
        //  内部構造をキチンと解析するのが理想だが時間がかかりそうなので
        //  頂点入力要素の最初は位置座標、最後がジョイントIndex＆重みであると（事実上そのようになっているので）決め打ちして取得
        float* pPos = (float*)( vtxDataAddrres );                     // 位置座標は先頭に格納されている
        u32 jntDataOffset = pVD->elemSize - 8;                        // Offsetは jointIdx = u8*4 jointWeight = u8*4 の8byte分を引いたもの
        u32* pJointData = (u32*)( vtxDataAddrres + jntDataOffset );   // ジョイントデータは最後に格納されている

        vtxData[wp].pos = gfl2::math::Vector3( pPos[0], pPos[1], pPos[2] );
        vtxData[wp].jointIdx = pJointData[0];
        vtxData[wp].jointWeight = pJointData[1];
        vtxData[wp].isMarge = false;  // 初期化設定
        wp++;
      }
    }
  }

  // 同一座標の頂点データを判別
  int margeNumVtx = 0;
  {
    for( int idxVtx=1; idxVtx<totalNumVtx; ++idxVtx ){
      for( int idxRefVtx=0; idxRefVtx<idxVtx; ++idxRefVtx ){
        if( vtxData[idxVtx].pos == vtxData[idxRefVtx].pos ){
          vtxData[idxVtx].isMarge = true;
          break;
        }
      }
    }
    for( int idxVtx=0; idxVtx<totalNumVtx; ++idxVtx ){
      if( vtxData[idxVtx].isMarge == false ){ margeNumVtx++; }
    }
  }

  // 頂点バッファの作成（同一座標の頂点は捨てる）
  struct VertexBufferData {
    gfl2::math::Vector3 pos;
    gfl2::math::Vector4 param0;
    gfl2::math::Vector2 param1;
    u32                 jointIdx;
    u32                 jointWeight;
  };
  VertexBufferData* vtxBufferData = (VertexBufferData*)GLMemory::Malloc( margeNumVtx * sizeof(VertexBufferData), 128 );
  {
    int wp = 0;

    for( u32 i=0; i<totalNumVtx; ++i ){
      if( vtxData[i].isMarge == false ){
        // 抽出データのコピー
        vtxBufferData[wp].pos         = vtxData[i].pos;
        vtxBufferData[wp].jointIdx    = vtxData[i].jointIdx;
        vtxBufferData[wp].jointWeight = vtxData[i].jointWeight;

        // 固有パラメータの設定        
        f32 param0_x = (f32)(random.GenerateRandomN( 16 )-8);     // 高さ幅
        f32 param0_y = (f32)random.GenerateRandomN( 512 );        // 発生オフセット
        f32 param0_z = ( random.GenerateRandomU32() & 1 )? 0 : 1; // 反転フラグ
        f32 param0_w = 0;
        f32 param1_x = 0;
        f32 param1_y = 0;

        vtxBufferData[wp].param0 = gfl2::math::Vector4( param0_x, param0_y, param0_z, param0_w );
        vtxBufferData[wp].param1 = gfl2::math::Vector2( param1_x, param1_y );

        wp++;
      }
    }
  }
  GLMemory::Free( vtxData );  // 使用済み廃棄

  // インデックスバッファの作成
  // インデックスバッファは必須かつPointListモードが存在しないが、TriangleStripで頂点数分のリストとすることで代用
  // ※ジオメトリシェーダーの方でPointListとして解釈することは可能
  u16* idxBufferData = (u16*)GLMemory::Malloc( margeNumVtx * sizeof(u16), 128 );
  for( u32 i=0; i<margeNumVtx; ++i ){ idxBufferData[i] = (u16)i; }  // 頂点Indexを指し示す単純な配列

  // 描画コマンドの作成
  {
	  nn::gr::CTR::Vertex::InterleaveInfo		interleaveInfo;
	  nn::gr::CTR::Vertex										grVertex;
	  nn::gr::CTR::BindSymbolVSInput        vsInput[ VTX_ELEM_NUM ];
		nn::gr::CTR::Vertex::IndexStream			indexStream;

    uptr vaddr = nngxGetPhysicalAddr( (u32)vtxBufferData );
    uptr iaddr = nngxGetPhysicalAddr( (u32)idxBufferData );

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
		indexStream.drawVtxNum = margeNumVtx;


    // 任天堂Apiを利用してコマンド作成
    // 全体サイズが不明なので、まずはテンポラリバッファにコマンド作成
    u32* pCommandBufferTemp0 = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
    u32* pCommandBufferTemp1 = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
    u32* pCommandBufferTemp2 = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
    for( int i=0; i<128; ++i ){ pCommandBufferTemp0[i] = pCommandBufferTemp1[i] = pCommandBufferTemp2[i] = 0; } // 初期化
    u32 comLen0 = 0;
    u32 comLen1 = 0;
    u32 comLen2 = 0;

    // returnコマンド
    u32		returnCommand[] = { 0x00000001, 0x000f023d };

    // EnableAttrCommand
    {
      // 実コマンド部
      u32* pStart = &pCommandBufferTemp0[0];  // 先頭u32*4はヘッダ
      u32* pTail = grVertex.MakeEnableAttrCommand( pStart );
	    comLen0 = (u32)pTail - (u32)pStart;
	    u32 wp = comLen0 / sizeof(u32);         // 終端書き込み位置
      //assert( (comLen0 % 8) == 0 );         // 本当はAlignチェックした方がいいが、普通は正しく出てくるはずなので省略
        
      // returnコマンドの挿入
      if( (comLen0 % 16) == 0 ){ pCommandBufferTemp0[wp] = pCommandBufferTemp0[wp+1] = 0; wp += 2; comLen0 += 8; }  //Alignment用dummy書き込み
      pCommandBufferTemp0[wp] = returnCommand[0];
      pCommandBufferTemp0[wp+1] = returnCommand[1];
      comLen0 += 8;
    }

    // DisableAttrCommand
    {
      // 実コマンド部
      u32* pStart = &pCommandBufferTemp1[0];  // 先頭u32*4はヘッダ
      u32* pTail = grVertex.MakeDisableAttrCommand( pStart );
	    comLen1 = (u32)pTail - (u32)pStart;
	    u32 wp = comLen1 / sizeof(u32);         // 終端書き込み位置
      //assert( (comLen1 % 8) == 0 );         // 本当はAlignチェックした方がいいが、普通は正しく出てくるはずなので省略
        
      // returnコマンドの挿入
      if( (comLen1 % 16) == 0 ){ pCommandBufferTemp1[wp] = pCommandBufferTemp1[wp+1] = 0; wp += 2; comLen1 += 8; }  //Alignment用dummy書き込み
      pCommandBufferTemp1[wp] = returnCommand[0];
      pCommandBufferTemp1[wp+1] = returnCommand[1];
      comLen1 += 8;
    }

    // DrawCommand
    {
      // 実コマンド部
      u32* pStart = &pCommandBufferTemp2[0];  // 先頭u32*4はヘッダ
      u32* pTail = grVertex.MakeDrawCommand( pStart, indexStream );
	    comLen2 = (u32)pTail - (u32)pStart;
	    u32 wp = comLen2 / sizeof(u32);         // 終端書き込み位置
      //assert( (comLen2 % 8) == 0 );         // 本当はAlignチェックした方がいいが、普通は正しく出てくるはずなので省略
        
      // returnコマンドの挿入
      if( (comLen2 % 16) == 0 ){ pCommandBufferTemp2[wp] = pCommandBufferTemp2[wp+1] = 0; wp += 2; comLen2 += 8; }  //Alignment用dummy書き込み
      pCommandBufferTemp2[wp] = returnCommand[0];
      pCommandBufferTemp2[wp+1] = returnCommand[1];
      comLen2 += 8;
    }
    // サイズ確定後コマンド保存
    pEM->comBufferEnable  = (u32*)GLMemory::Malloc( comLen0, 128 );
    pEM->comBufferDisable = (u32*)GLMemory::Malloc( comLen1, 128 );
    pEM->comBufferDraw    = (u32*)GLMemory::Malloc( comLen2, 128 );

    pEM->comBufferEnableSize  = comLen0;
    pEM->comBufferDisableSize = comLen1;
    pEM->comBufferDrawSize    = comLen2;

    for( u32 i=0; i<comLen0/sizeof(u32); ++i ){ pEM->comBufferEnable[i]   = pCommandBufferTemp0[i]; }
    for( u32 i=0; i<comLen1/sizeof(u32); ++i ){ pEM->comBufferDisable[i]  = pCommandBufferTemp1[i]; }
    for( u32 i=0; i<comLen2/sizeof(u32); ++i ){ pEM->comBufferDraw[i]     = pCommandBufferTemp2[i]; }

    GLMemory::Free( pCommandBufferTemp0 );  // 使用済み廃棄
    GLMemory::Free( pCommandBufferTemp1 );  // 使用済み廃棄
    GLMemory::Free( pCommandBufferTemp2 );  // 使用済み廃棄
  }
  pEM->vtxBuffer = (u8*)vtxBufferData;
  pEM->idxBuffer = (u8*)idxBufferData;
}
#endif


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
// ShaderDriver
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraShaderDriver::FullPowerEffectAuraShaderDriver() : GLMemory(), IShaderDriver()
{
  // リソースセットアップ
  //
  //  ModelConverterを通して「簡易モデル＋シェーダー」の形で組み込むのが定番だが
  //  ここでは実験的に、直接シェーダーを差し替える手段を用いてみる
  {
    // モデルデータをセットアップ
    //  形として必要なだけなので中身は簡単なもの。
    //  ここでは法線マップ出力用のモデルを流用する。
    scenegraph::resource::GfBinaryMdlData binMdlData;

	  m_pMdlBin = LocalFunc::ReadFile( "rom:/data/test/aura_shader/EdgeMapVColA.ctr.gfbmdl");			
	  binMdlData.SetModelData( (c8*)m_pMdlBin );
	  m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &binMdlData );	

    // 上記モデルのシェーダーを差し替える
    //
    //  上記モデルは
    //    ・Default.ctr.gfbvsh
    //    ・NormalToUv.ctr.gfbvsh
    //    ・Comb2_Manual.ctr.gfbfsh
    //  で成り立っているので
    //  それらの名前で成り済ます(重要なのは名前から生成されるハッシュ値)
	  scenegraph::resource::GfBinaryShdData binShdData0, binShdData1, binShdData2;

    m_vsh0 = new CtrGfbvshDummy;
    m_vsh0->Setup( "rom:/data/test/aura_shader/rimEffect.shbin", "Default", false );
    binShdData0.SetShaderData( m_vsh0->m_CtrGfbvsh );
    scenegraph::resource::ResourceNode* pBinShdNode0 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData0 );
	  m_pBinModelRootNode->AddChild( pBinShdNode0 );

    m_vsh1 = new CtrGfbvshDummy;
    m_vsh1->Setup( "rom:/data/test/aura_shader/auraEffectVGSH.shbin", "NormalToUv", true );
    binShdData1.SetShaderData( m_vsh1->m_CtrGfbvsh );
    scenegraph::resource::ResourceNode* pBinShdNode1 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData1 );
	  m_pBinModelRootNode->AddChild( pBinShdNode1 );

    m_fsh = new CtrGfbfshDummy;
    SetFshStatus(); // フラグメントシェーダー設定
    m_fsh->Setup( "Comb2_Manual" );				
    binShdData2.SetShaderData( m_fsh->m_CtrGfbfsh );
    scenegraph::resource::ResourceNode* pBinShdNode2 = scenegraph::resource::ResourceManager::CreateResourceNode( &binShdData2 );
	  m_pBinModelRootNode->AddChild( pBinShdNode2 );

    // 上記モデルのテクスチャ読み込み
    //  TextureConverterで名前が同じの成り済ましテクスチャファイルを作成
    scenegraph::resource::GfBinaryTexData binTexData;

    m_pTexBin = LocalFunc::ReadFile( "rom:/data/test/aura_shader/NormalTexture.ctr.btex" );
    binTexData.SetTextureData( (c8*)m_pTexBin );
    scenegraph::resource::ResourceNode* pBinTexNode = scenegraph::resource::ResourceManager::CreateResourceNode( &binTexData );
	  m_pBinModelRootNode->AddChild( pBinTexNode );
  }

  // 必要なリソースを設定後、このモデルのインスタンスを作成してシェーダーをぶら下げておく
  // (リソースに欠けているもの、整合性が合わないものがあれば、ここでフリーズ)
  m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );

  // このモデル自体はシェーダー保管庫にすぎないので、シーンからはずして描画はOffにしておく
	scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
	m_pModelInstanceNode->UpdateDrawTag( info );
	scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );

  // 固有定数バッファ初期化
  m_CBuffer.mtxView = nn::math::MTX34( 1,0,0,0, 0,1,0,0, 0,0,1,0 );
  m_CBuffer.timer = 0;

  m_auraMdlManager = NULL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraShaderDriver::~FullPowerEffectAuraShaderDriver()
{
	GFX_SAFE_DELETE( m_pModelInstanceNode );
	GFX_SAFE_DELETE( m_pBinModelRootNode );

  GLMemory::Free(m_pMdlBin);
  GLMemory::Free(m_pTexBin);

  delete m_fsh;
  delete m_vsh1;
  delete m_vsh0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 固有定数バッファ設定
void FullPowerEffectAuraShaderDriver::SetCBuffer( FullPowerEffectAuraShaderDriver::CBuffer* pData, FullPowerEffectAuraMdlManager* ptr )
{
  if( !pData ){ return; }

  m_CBuffer = *pData;
  m_auraMdlManager = ptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// シェーダー適用
b32 FullPowerEffectAuraShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
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

  // このシェーダードライバ固有の定数レジスタを設定(レジスタ上書きを見越してここでやる)
  {
	  nn::gr::CommandBufferJumpHelper* pCommandBuffer = GFGL::GetJumpHelper();
	  bit32* cmd = pCommandBuffer->GetCurrentPtr();
    {
      // キューブマップ時に使用する予定の nw_inverse_view_matrix定数レジスタ を流用する
      // →この後、上書きされている可能性が高いのでLightPowScaleの位置に変更
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 82, m_CBuffer.mtxView );
 
	    // timer
      nn::math::VEC4 data;
      data.x = nn::math::SinIdx( m_CBuffer.timer * 0x100 );
      data.y= 0.0f;
      data.z= 0.0f;
      data.w= (f32)(m_CBuffer.timer%512);
      // 未使用の nw_texture_translate定数レジスタを流用する
		  cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 9, data );
    }
    pCommandBuffer->Put(cmd);
  }

  // このモデルのインスタンスからぶら下げておいたシェーダーを取り出して有効化
  MeshDrawTag								*pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	CombinerCtrShaderDriver		*pShaderDriver = (CombinerCtrShaderDriver*)( pTag->GetShaderDriver() );
	{
		setUpEnable.value = 0;
		setUpEnable.VertexShader = true;
		setUpEnable.GeometryShader = true;
		setUpEnable.FragmentShader = true;
		setUpEnable.UvSetting = true;
		rRenderState.SetSetUpEnable( setUpEnable );

		pShaderDriver->Apply( rRenderState, pMeshDrawTag );

    // コンスタントカラーやレンダーステートなど上書きされてしまうので
    // 処理のロスはあるがここで再度コマンド転送
    // 負荷が問題用になるようなら考える
    const scenegraph::resource::ShaderResourceNode* pFsh = pShaderDriver->GetPixelShader();
    GFGL::AddCommandJump( reinterpret_cast<const u32*>( pFsh->GetFragmentShader() ), pFsh->GetFragmentShaderSize() );
	}

  // 本来の設定に復帰
	rRenderState.SetSetUpEnable( origSetUpEnable );

  return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void FullPowerEffectAuraShaderDriver::SetTextureOverride( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
	MeshDrawTag* pTag = reinterpret_cast<MeshDrawTag*>( m_pModelInstanceNode->GetMeshDrawTag( 0 ) );
	RenderState::ShaderStateMap shaderStateMap = rRenderState.GetShaderStateMap();
	{
		const gfx::Texture* pTexture = pTag->GetTexture( 0 );
		{
			u32 textureCommand[8];

			const gfl2::gfx::ctr::CTRTexture*       pCTRTexture = reinterpret_cast<const gfl2::gfx::ctr::CTRTexture*>(pTexture);
			const gfl2::gfx::Texture::TextureDesc&  rTextureDesc = pCTRTexture->GetTextureDesc();
#if 0
			textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	rTexInfo.m_Attribute.m_MagFilter, rTexInfo.m_Attribute.m_MinFilter,
																															pCTRTexture->GetFormat(),
																															rTexInfo.m_Attribute.m_RepeatV, rTexInfo.m_Attribute.m_RepeatU,
																															0, PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D );
#else
			textureCommand[0] = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(	PICA_DATA_TEXTURE_MAG_FILTER_LINEAR, 
                                                              PICA_DATA_TEXTURE_MIN_FILTER_LINEAR,
																															pCTRTexture->GetFormat(),
																															PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,
                                                              PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,
																															0, PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D );
#endif
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
  if( m_auraMdlManager ){
    FullPowerEffectAuraMdlManager::OverwriteTag* pTag;

    pTag = m_auraMdlManager->GetOverwriteTag( pMeshDrawTag );

    if( pTag ){
      GFGL::AddCommandJump( pTag->comBufferEnable,  pTag->comBufferEnableSize );
      GFGL::AddCommandJump( pTag->comBufferDraw,    pTag->comBufferDrawSize );
      GFGL::AddCommandJump( pTag->comBufferDisable, pTag->comBufferDisableSize );
    }
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// フラグメントシェーダー設定
void FullPowerEffectAuraShaderDriver::SetFshStatus( void )
{
  // コンスタントカラー設定
  m_fsh->m_convinerData.constantCol[0].setColor( 1, 1, 1, 1 );
  m_fsh->m_convinerData.constantCol[1].setColor( 1, 0.8f, 0.5f, 0.3f );
  m_fsh->m_convinerData.constantCol[2].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[3].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[4].setColor( 0, 0, 0, 0 );
  m_fsh->m_convinerData.constantCol[5].setColor( 1, 1, 1, 0 );

  // その他カラー設定
  m_fsh->m_convinerData.bufferCol.setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[0].setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[1].setColor( 0, 0, 0, 1 );
  m_fsh->m_convinerData.textureBorderCol[2].setColor( 0, 0, 0, 1 );

  // コンバイナステージ設定
  // idxInputBufferFlagの内容はコマンドの仕様とクリエイティブスタジオの仕様に違いがあるので注意
  //  コマンド仕様                : 前段のバッファを使用するか否か
  //  クリエイティブスタジオ仕様  : この段バッファを持ち越すか否か
  // なので結果的に内容にずれが生じ
  //  stage0(クリエイティブスタジオ)のフラグ→stage1(コマンド)のフラグ
  //  stage1(クリエイティブスタジオ)のフラグ→stage2(コマンド)のフラグ
  //  stage2(クリエイティブスタジオ)のフラグ→stage3(コマンド)のフラグ
  //  stage3(クリエイティブスタジオ)のフラグ→stage4(コマンド)のフラグ
  //  stage4(クリエイティブスタジオ)のフラグ→stage5(コマンド)のフラグ
  // となるのでここで吸収し、格納すること
  convinerStage* pStage;

  // ステージ0設定
  pStage = &m_fsh->m_convinerData.stage[0];

  pStage->constantColIdx = 1;
  pStage->color.idxMathFormula = CONV_MATH_FML::MODULATE;
  pStage->color.idxScale = CONV_MATH_SCALE::VAL1;
  pStage->color.idxSourceA = CONV_MATH_SRC::TEX0;
  pStage->color.idxOperandA = CONV_MATH_OPR_RGB::RGB;
  pStage->color.idxSourceB = CONV_MATH_SRC::CONSTANTCOL;
  pStage->color.idxOperandB = CONV_MATH_OPR_RGB::RGB;
  pStage->color.idxSourceC = CONV_MATH_SRC::CONSTANTCOL;
  pStage->color.idxOperandC = CONV_MATH_OPR_RGB::ALPHA;
  pStage->color.idxInputBufferFlag = CONV_BUFFERINPUT::PREVIOUSBUFFER;

  pStage->alpha.idxMathFormula = CONV_MATH_FML::MODULATE;
  pStage->alpha.idxScale = CONV_MATH_SCALE::VAL1;
  pStage->alpha.idxSourceA = CONV_MATH_SRC::TEX0;
  pStage->alpha.idxOperandA = CONV_MATH_OPR_A::ALPHA;
  pStage->alpha.idxSourceB = CONV_MATH_SRC::CONSTANTCOL;
  pStage->alpha.idxOperandB = CONV_MATH_OPR_A::ALPHA;
  pStage->alpha.idxSourceC = CONV_MATH_SRC::CONSTANTCOL;
  pStage->alpha.idxOperandC = CONV_MATH_OPR_A::ONEMINUS_ALPHA;
  pStage->alpha.idxInputBufferFlag = CONV_BUFFERINPUT::PREVIOUSBUFFER;

  // レンダーステート設定
  m_fsh->m_renderStateData.brend.isEnable = true;
  m_fsh->m_renderStateData.brend.idxEqRGB = RDST_BLENDEQ::ADD;
  m_fsh->m_renderStateData.brend.idxFuncSrcRGB = RDST_BLENDFUNC::SRC_ALPHA;
  m_fsh->m_renderStateData.brend.idxFuncDstRGB = RDST_BLENDFUNC::ONE;
  m_fsh->m_renderStateData.brend.idxEqA = RDST_BLENDEQ::ADD;
  m_fsh->m_renderStateData.brend.idxFuncSrcA = RDST_BLENDFUNC::SRC_ALPHA;
  m_fsh->m_renderStateData.brend.idxFuncDstA = RDST_BLENDFUNC::ONE_MINUS_SRC_ALPHA;

  m_fsh->m_renderStateData.alphaTest.isEnable = false;

  m_fsh->m_renderStateData.depthTest.isEnable = true;
  m_fsh->m_renderStateData.depthTest.idxFunc = RDST_DEPTHTEST::LESS;
  m_fsh->m_renderStateData.depthTest.isEnableWrite = true;

  m_fsh->m_renderStateData.stencilTest.isEnable = false;

  m_fsh->m_renderStateData.cullMode.isEnable = true;
  m_fsh->m_renderStateData.cullMode.mode = RDST_CULL::CW;

  m_fsh->m_renderStateData.polyOffs.isEnable = false;

  // テクスチャステート設定
  m_fsh->m_textureStateData.slot[0].idxMinMagFilter = TXST_FILTER::LINEAR;
  m_fsh->m_textureStateData.slot[0].idxWrapU = TXST_WRAP::BORDER;
  m_fsh->m_textureStateData.slot[0].idxWrapV = TXST_WRAP::BORDER;

  m_fsh->m_textureStateData.slot[1].idxMinMagFilter = TXST_FILTER::LINEAR;
  m_fsh->m_textureStateData.slot[1].idxWrapU = TXST_WRAP::REPEAT;
  m_fsh->m_textureStateData.slot[1].idxWrapV = TXST_WRAP::REPEAT;

  m_fsh->m_textureStateData.slot[2].idxMinMagFilter = TXST_FILTER::LINEAR;
  m_fsh->m_textureStateData.slot[2].idxWrapU = TXST_WRAP::REPEAT;
  m_fsh->m_textureStateData.slot[2].idxWrapV = TXST_WRAP::REPEAT;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//  オーラエフェクト専用モデル管理クラス
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
FullPowerEffectAuraMdlManager::FullPowerEffectAuraMdlManager()
{
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
// 構造体初期化 & クリーンアップ
void FullPowerEffectAuraMdlManager::_initEffectMdl( EffectMdl* pEffectMdl )
{
  pEffectMdl->arrayTag = NULL;
  pEffectMdl->numTag = 0;
  pEffectMdl->pMdlNode = NULL;
  pEffectMdl->isEnable = false;
}

void FullPowerEffectAuraMdlManager::_cleanupEffectMdl( EffectMdl* pEffectMdl )
{
  for( int idxFace=0; idxFace<pEffectMdl->numTag; ++idxFace ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxFace ];

    if( pTag->comBufferEnable )   { GLMemory::Free( pTag->comBufferEnable ); }
    if( pTag->comBufferDraw )     { GLMemory::Free( pTag->comBufferDraw ); }
    if( pTag->comBufferDisable )  { GLMemory::Free( pTag->comBufferDisable ); }

    if( pTag->vtxBuffer )         { GLMemory::Free( pTag->vtxBuffer ); }
    if( pTag->idxBuffer )         { GLMemory::Free( pTag->idxBuffer ); }
  }
  GLMemory::Free( pEffectMdl->arrayTag );
 
  _initEffectMdl( pEffectMdl );
}

//-----------------------------------------------
// エフェクト用モデル作成
//  該当ノードに関連付けしながらモデルデータを作成
void FullPowerEffectAuraMdlManager::CreateEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
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
  pEffectMdl->isEnable = true;
  pEffectMdl->pMdlNode = pMdlNode;

  // ノードからMeshTag情報を取り出し、構造体の初期設定
  pEffectMdl->numTag = pEffectMdl->pMdlNode->GetDrawTagNum();
  pEffectMdl->arrayTag = (OverwriteTag*)GLMemory::Malloc( sizeof(OverwriteTag) * pEffectMdl->numTag, 128 );

  // ワークエリア初期化
  for( int i = 0; i < pEffectMdl->numTag; ++i ){
    OverwriteTag* pTag = &pEffectMdl->arrayTag[i];

    // デバイスメモリからアロケートする
    pTag->comBufferEnable = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
    pTag->comBufferDraw   = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
    pTag->comBufferDisable = (u32*)GLMemory::Malloc( 128 * sizeof(u32), 128 );
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

    b32 f = _cleateBufferData( pTag );
    if( f == true ){
      _cleateCommandData( pTag );
    } else {
      pTag->pMeshDrawTag = NULL;  // 無効化
    }
	}
}

//-----------------------------------------------
// エフェクト用モデルを描画対象からはずす
void FullPowerEffectAuraMdlManager::RemoveEffectMdl( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	//gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pMdlNode;
  //pMdlNode = reinterpret_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(pNode);

  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];
    pEffectMdl->isEnable = false;
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
b32 FullPowerEffectAuraMdlManager::_cleateBufferData( OverwriteTag* pTag )
{
  const scenegraph::resource::MeshResourceNode::FaceData* pFaceData = pTag->pMeshDrawTag->GetFaceData();

  // 頂点バッファ情報を取得
  u32 numVtx = pFaceData->m_VertexNum;
  u32 bufSizeVtx = pFaceData->GetVertexBufferSize(); 
  u32 elemSize = bufSizeVtx / numVtx; 
  u32 bufferAddress = (u32)pFaceData->GetVertexBuffer();
      
  u32 dataSize = numVtx * elemSize;
  s32 isPadding = 0;

  // データ異常を判定しておくが、だからといってなにをするわけでもない
  if( bufSizeVtx == dataSize )            { isPadding = 0; }   // パディングなし
  else if( (bufSizeVtx - dataSize) == 8 ) { isPadding = 1; }   // 正しいパディング
  else                                    { isPadding = -1; }  // おかしい

  if( isPadding == -1 ){ return false; }

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
  VertexBufferData* vtxBufferData = (VertexBufferData*)GLMemory::Malloc( numVtx * sizeof(VertexBufferData), 128 );
  {
    for( int idxVtx=0; idxVtx<numVtx; ++idxVtx ){
      VertexBufferData* pVtx = &vtxBufferData[ idxVtx ];

      // 頂点データ格納アドレスの計算
      u32 vtxDataAddrres = bufferAddress + elemSize * idxVtx;

      // 取得データ格納アドレスの計算
      //  内部構造をキチンと解析するのが理想だが時間がかかりそうなので
      //  頂点入力要素の最初は位置座標、最後がジョイントIndex＆重みであると（事実上そのようになっているので）決め打ちして取得
      float* pPos = (float*)( vtxDataAddrres );                     // 位置座標は先頭に格納されている
      u32 jntDataOffset = elemSize - 8;                             // Offsetは jointIdx = u8*4 jointWeight = u8*4 の8byte分を引いたもの
      u32* pJointData = (u32*)( vtxDataAddrres + jntDataOffset );   // ジョイントデータは最後に格納されている

      // 抽出データのコピー
      pVtx->pos = gfl2::math::Vector3( pPos[0], pPos[1], pPos[2] );
      pVtx->jointIdx = pJointData[0];
      pVtx->jointWeight = pJointData[1];

      // 固有パラメータの設定        
      f32 param0_x = (f32)(random.GenerateRandomN( 16 )-8);     // 高さ幅
      f32 param0_y = (f32)random.GenerateRandomN( 512 );        // 発生オフセット
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
  u16* idxBufferData = (u16*)GLMemory::Malloc( numVtx * sizeof(u16), 128 );
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
FullPowerEffectAuraMdlManager::OverwriteTag* FullPowerEffectAuraMdlManager::GetOverwriteTag
  ( const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag )
{
  for( int idxMdl=0; idxMdl<EFFECT_RESIST_MAX; ++idxMdl ){
    EffectMdl* pEffectMdl = &m_effectMdl[ idxMdl ];
    if( pEffectMdl->isEnable ){
      for( u32 idxTag = 0; idxTag < pEffectMdl->numTag; ++idxTag ){
        OverwriteTag* pTag = &pEffectMdl->arrayTag[ idxTag ];

        if( pMeshDrawTag == pTag->pMeshDrawTag ){ return pTag; }
	    }
    }
  }
  return NULL;
}

#endif //USE_AURA_SHADER
