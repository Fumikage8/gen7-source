#include <RenderingEngine/include/Renderer/Util/gfl2_ModelRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief コンストラクタ
 */
ModelRenderPath::ModelRenderPath() : IRenderPath(),
  m_LightSetNo(0),
	m_pDrawManager(NULL),
  m_pDrawEnvNode(NULL),
  m_pModelInstanceNode(NULL)
{
  m_Camera.m_UseCamera = false;
}

/**
 * @brief デストラクタ
 */
ModelRenderPath::~ModelRenderPath()
{
  GFX_SAFE_DELETE( m_pDrawManager );
}

void ModelRenderPath::Initialize( IGLAllocator* pObj, const InitDescription& desc )
{
	m_pDrawManager = DrawManager::Create( pObj, desc.m_DrawManagerDesc );
}

void ModelRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
	if ( pRenderTarget )
		GFGL::SetRenderTarget( pRenderTarget );
	if ( pDepthStencil )
		GFGL::SetDepthStencil( pDepthStencil );
}

void ModelRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  GFGL::ClearRenderTargetDepthStencil( pRenderTarget, Color( 0.4f, 0.4f, 0.4f, 1.0f ), pDepthStencil, 1.0f, 255 );
}

void ModelRenderPath::Update( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if ((!m_pDrawEnvNode) && (!m_pModelInstanceNode))
  {// 描画対象のノードがなければ終了.
    return;
  }

  scenegraph::DagNode* pDrawEnvChildNode = m_pDrawEnvNode->GetChild();
  // ライトセットに当たるノードがDrawEnvの子になっていなければ, 正常にライティング出来ないので止める.
  GFL_ASSERT_STOP(pDrawEnvChildNode && "ModelRenderPath::m_pDrawEnvNode has no LightSetNodes.");

  // カメラの設定
  SetCamera(rDrawContext);

  // いったん全てのオブジェクトを外して, ノードがあれば追加.
  m_pDrawManager->RemoveAllDrawEnv();
  
  // 毎回ライトセット番号を更新する
  scenegraph::instance::LightSetNode* pLightSetNode = pDrawEnvChildNode->SafeCast<gfl2::renderingengine::scenegraph::instance::LightSetNode>();
  // 子ノードがLightSetNodeに変換できなかったら止める.
  GFL_ASSERT_STOP(pLightSetNode && "ChildNode of ModelRenderPath::m_pDrawEnvNode is not kind of LightSetNode.");

  // LightNoを設定してから, DrawManagerにDrawEnvNodeを登録.
  pLightSetNode->SetSetNo(m_LightSetNo);
  m_pDrawManager->AddDrawEnv(m_pDrawEnvNode);

  // DrawManagerに登録されているオブジェクトを全て外し, 描画対象のモデルを追加.
  m_pDrawManager->RemoveAllObject();
  m_pDrawManager->AddObject(m_pModelInstanceNode);

  m_pDrawManager->Update();
}

void ModelRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  if ((!m_pDrawEnvNode) && (!m_pModelInstanceNode))
  {// 描画対象のノードがなければ終了.
    return;
  }

	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}
	}

	const IShaderDriver		*pShader = ShaderOverride();
	m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = gfl2::gfx::GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
    pMtx44 = (nn::math::MTX44*)&(m_pDrawManager->GetProjectionMatrix());
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
    pMtx34 = (nn::math::MTX34*)&(m_pDrawManager->GetViewMatrix());
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

	m_pDrawManager->Draw(false);

	GFGL::EndScene();
}


void ModelRenderPath::SetCamera(const RenderingPipeLine::DrawContext& rDrawContext)
{
	const Camera		*pCameraOverride = CameraOverride();
  if ( pCameraOverride->m_UseCamera )
	{
		m_pDrawManager->SetProjectionMatrix( pCameraOverride->m_Projection );
		m_pDrawManager->SetViewMatrix( pCameraOverride->m_View );
	}
	else
	{
		m_pDrawManager->SetProjectionMatrix( rDrawContext.m_Projection );
		m_pDrawManager->SetViewMatrix( rDrawContext.m_View );
	}
}

void ModelRenderPath::SetDrawEnvNode(scenegraph::instance::DrawEnvNode* pNode)
{
  m_pDrawEnvNode = pNode;
}

void ModelRenderPath::SetModelInstanceNode(scenegraph::instance::ModelInstanceNode* pNode)
{
  m_pModelInstanceNode = pNode;
  if (m_pModelInstanceNode == NULL)
  {
    return;
  }

  u32 materialNum = m_pModelInstanceNode->GetMaterialNum();
  if (materialNum == 0)
  {// マテリアルがひとつもないということは考えにくいが念のため.
    return;
  }

  s16 lightSetNo = 0;
  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pParam;
  pParam = m_pModelInstanceNode->GetMaterialInstanceNode(0)->GetAttributeParam();
  lightSetNo = pParam->m_LightSetNo;

  for (u32 i = 1; i < materialNum; ++i)
  {
    pParam = m_pModelInstanceNode->GetMaterialInstanceNode(i)->GetAttributeParam();
    assert((lightSetNo == pParam->m_LightSetNo) && "LightSetNo is wrong.");
  }

  m_LightSetNo = lightSetNo;
}

scenegraph::instance::DrawEnvNode* ModelRenderPath::GetDrawEnvNode() const
{
  return m_pDrawEnvNode;
}

scenegraph::instance::ModelInstanceNode* ModelRenderPath::GetModelInstanceNode() const
{
  return m_pModelInstanceNode;
}

void ModelRenderPath::SetCameraData(const ModelRenderPath::Camera& camera)
{
  m_Camera = camera;
}

}}}}