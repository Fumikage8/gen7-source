#ifndef MyRenderingPipeLine_H_INCLUDED
#define MyRenderingPipeLine_H_INCLUDED

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <renderingengine/include/renderer/Util/gfl2_UtilJointRenderPath.h>
#include <util/include/gfl2_FixedSizeContainer.h>

class MyRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	MyRenderingPipeLine();
	virtual ~MyRenderingPipeLine();

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
  gfl2::renderingengine::renderer::util::JointRenderPath                m_JointRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

};

#endif
