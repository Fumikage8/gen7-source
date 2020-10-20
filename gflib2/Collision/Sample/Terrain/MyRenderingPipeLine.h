#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/Renderer/Util/gfl2_UtilCollisionRenderPath.h>

//-----------------------------------------------------------
//! @brief ポイント描画パスクラス
//-----------------------------------------------------------
class PointRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

  PointRenderPath()
  {
  }

  virtual ~PointRenderPath()
  {
  }

  void Initialize()
  {
  }

  virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride(){ return NULL; }
  virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride(){ return NULL; }

  virtual void SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    // レンダーターゲットの設定
    gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
    gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
  }

  virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
  {
    // レンダーターゲットのクリアはしない
  }

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  void SetPos(const gfl2::math::Vector& charaPos, const gfl2::math::Vector& footPosL, const gfl2::math::Vector& footPosR)
  {
    m_CharaPos = charaPos;
    m_FootPosL = footPosL;
    m_FootPosR = footPosR;
  }

protected:

  gfl2::math::Vector m_CharaPos;
  gfl2::math::Vector m_FootPosL;
  gfl2::math::Vector m_FootPosR;
};

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine( f32 screanWidth, f32 screanHeight );
	virtual ~MyRenderingPipeLine();

	void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_SceneRenderPath.AddDrawEnv(pNode);
  }

  void SetCurrentTerrain(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pCurrentTerrain)
  {
    m_CollisionRenderPath.SetCurrentTerrain(pCurrentTerrain);
  }

  void SetPos(const gfl2::math::Vector& charaPos, const gfl2::math::Vector& footPosL, const gfl2::math::Vector& footPosR)
  {
    m_PointRenderPath.SetPos(charaPos, footPosL, footPosR);
  }

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
  gfl2::renderingengine::renderer::util::CollisionRenderPath            m_CollisionRenderPath;
  PointRenderPath                                                       m_PointRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;
};

#endif
