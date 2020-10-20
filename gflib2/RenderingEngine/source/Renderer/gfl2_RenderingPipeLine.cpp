#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerCtrShaderDriver.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <gfx/include/gfl2_DrawUtil.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace renderingengine { namespace renderer { 


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
RenderingPipeLine::SceneRenderPath::SceneRenderPath() : IRenderPath(),
  m_pDrawManager( NULL ),
  m_pDisplayList( NULL)
{
}

RenderingPipeLine::SceneRenderPath::~SceneRenderPath()
{
  GFX_SAFE_DELETE( m_pDrawManager );
  GFX_SAFE_DELETE( m_pDisplayList );
}

void RenderingPipeLine::SceneRenderPath::Initialize( gfl2::gfx::IGLAllocator * pGLAllocator, const InitDescription& desc )
{
  m_pDrawManager = DrawManager::Create( pGLAllocator, desc.m_DrawManagerDesc );
  m_CullingObjectNodeContainer.CreateBuffer( desc.m_DrawManagerDesc.m_NodeStackSize, pGLAllocator );
  m_pDisplayList = gfx::GFGL::CreateDisplayList( pGLAllocator );
}

void RenderingPipeLine::SceneRenderPath::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
  m_pDrawManager->AddDrawEnv( pNode );
}

void RenderingPipeLine::SceneRenderPath::RemoveDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{
  m_pDrawManager->RemoveDrawEnv( pNode );
}

void RenderingPipeLine::SceneRenderPath::RemoveAllDrawEnv()
{
  m_pDrawManager->RemoveAllDrawEnv();
}

void RenderingPipeLine::SceneRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  if ( pRenderTarget )
    GFGL::SetRenderTarget( pRenderTarget );
  if ( pDepthStencil )
    GFGL::SetDepthStencil( pDepthStencil );
}

void RenderingPipeLine::SceneRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  GFGL::ClearRenderTargetDepthStencil( pRenderTarget, Color( 0.0f, 0.0f, 0.0f, 1.0f ), pDepthStencil, 1.0f, 255 );
}

void RenderingPipeLine::SceneRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  SetCamera(rDrawContext);
  SetDrawableNode();

  SetupDraw( rDrawContext );
  DrawManagerDraw(true);
}

void RenderingPipeLine::SceneRenderPath::SetupDraw( const DrawContext& rDrawContext, const gfl2::math::Matrix44* pProj, const gfl2::math::Matrix34* pView )
{
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
      const gfx::Surface::SurfaceDesc &rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
      GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
      GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
    }

    ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );
  }

  const IShaderDriver *pShader = ShaderOverride();
  m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

  nn::gr::CommandBufferJumpHelper *pCommandBuffer = GFGL::GetJumpHelper();
  bit32 *cmd = pCommandBuffer->GetCurrentPtr();

  if (pProj == NULL)
  {
    pProj = &(m_pDrawManager->GetProjectionMatrix());
  }
  if (pView == NULL)
  {
    pView = &(m_pDrawManager->GetViewMatrix());
  }

  {//proj
    nn::math::MTX44 *pMtx44;

    //pMtx44 = (nn::math::MTX44*)&rDrawContext.m_Projection;
    pMtx44 = (nn::math::MTX44*)pProj;
    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
  }

  {//view
    nn::math::MTX34 *pMtx34;

    //pMtx34 = (nn::math::MTX34*)&rDrawContext.m_View;
    pMtx34 = (nn::math::MTX34*)pView;

    if (m_pDrawManager->IsViewSpaceRenderEnable())
    {
      pMtx34 = (nn::math::MTX34*)&gfl2::renderingengine::renderer::CombinerCtrShaderDriver::GetViewSpaceDiffMatrix();
    }
    cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
  }

  pCommandBuffer->Put(cmd);
#endif

  GFGL::EndScene();
}

void RenderingPipeLine::SceneRenderPath::DrawManagerDraw(bool fUpdate)
{
#if defined(GF_PLATFORM_CTR)
  GFGL_BEGIN_DISPLAY_LIST(m_pDisplayList);
  m_pDrawManager->Draw(fUpdate);
  GFGL_END_DISPLAY_LIST(m_pDisplayList);
#else
  GFGL::BeginScene();
  m_pDrawManager->Draw(fUpdate);
  GFGL::EndScene();
#endif
}

void RenderingPipeLine::SceneRenderPath::ReuseCommand()
{
  GFGL::BeginScene();
  gfl2::gfx::GFGL::CallDisplayList(m_pDisplayList);
  GFGL::EndScene();
}

//DrawableNode用ビジターを作成
class DrawableNodeVisitor
{
public:
  DrawableNodeVisitor() : 
    m_pDrawManager(NULL)
  {
  }

  virtual ~DrawableNodeVisitor()
  {
  }

  void Visit( scenegraph::instance::TransformNode* pNode )
  {
    scenegraph::instance::DrawableNode* pDrawableNode = pNode->SafeCast<scenegraph::instance::DrawableNode>();
    if (pDrawableNode == NULL)
    {
      return;
    }

    if ( pDrawableNode->IsVisible() )
      m_pDrawManager->AddObject( pDrawableNode );
  }

  DrawManager *m_pDrawManager;
};

void RenderingPipeLine::SceneRenderPath::SetDrawableNode()
{
  m_pDrawManager->RemoveAllObject();

  const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*> *pDrawableNodeContainer = DrawableNodeOverride();
  if ( pDrawableNodeContainer )
  {
    const gfx::GLFixedSizeContainer<scenegraph::instance::DrawableNode*> &rDrawableNodeContainer = *pDrawableNodeContainer;

    for( u32 i = 0; i < rDrawableNodeContainer.size(); ++i )
    {
      m_pDrawManager->AddObject( rDrawableNodeContainer[i] );
    }
  }
  else
  {
    scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, DrawableNodeVisitor> visitor;
    visitor.m_pDrawManager = m_pDrawManager;

    //DrawableNodeに対しトラバース処理 @attention ここはいずれＡＡＢＢツリー視野カリング収集関数ができたら、それに置き換える。
    scenegraph::SceneGraphManager::TraverseFast<scenegraph::instance::TransformNode, DrawableNodeVisitor>( &visitor );
  }
}

void RenderingPipeLine::SceneRenderPath::SetCamera(const RenderingPipeLine::DrawContext& rDrawContext)
{
  const Camera *pCameraOverride = CameraOverride();
  if ( pCameraOverride )
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

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
void RenderingPipeLine::PresentPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  gfl2::gfx::GFGL::Finish();
  GFGL::SwapDisplay();
  DrawUtil::SwapBuffers();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
RenderingPipeLine::RenderingPipeLine()
{

}

RenderingPipeLine::~RenderingPipeLine()
{

}

void RenderingPipeLine::Update()
{
  b32 isEnable = this->StartRenderPath();
  if ( !isEnable ) return;

  do
  {
    IRenderPath *pRenderPath = this->GetRenderPath();

    if ( pRenderPath )
      pRenderPath->Update( m_DrawContext );

  }while( this->NextRenderPath() );
}

void RenderingPipeLine::Execute()
{
  b32 isEnable = this->StartRenderPath();
  if ( !isEnable ) return;

  do
  {
    IRenderPath *pRenderPath = this->GetRenderPath();

    if ( pRenderPath )
    {
      CombinerCtrShaderDriver::CacheClear();
      MeshDrawTag::CacheClear();
      pRenderPath->Execute( m_DrawContext );
    }

  }while( this->NextRenderPath() );
}

void RenderingPipeLine::CallDisplayList()
{
  b32 isEnable = this->StartRenderPath();
  if ( !isEnable ) return;

  do
  {
    IRenderPath *pRenderPath = this->GetRenderPath();

    if ( pRenderPath )
    {
      CombinerCtrShaderDriver::CacheClear();
      MeshDrawTag::CacheClear();
      pRenderPath->CallDisplayList( m_DrawContext );
    }

  }while( this->NextRenderPath() );
}

}}}

