#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_MOTION_CONFIG_NODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_MOTION_CONFIG_NODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/RenderingEngine/gfl2_RenderingEngineEnums_CLR.h>
#include <clr/include/gfx/gfl2_enums_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class MotionConfigInstanceNode
{
public:
	MotionConfigInstanceNode(gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pNative);
	~MotionConfigInstanceNode()
	{
		this->!MotionConfigInstanceNode();
	}
	!MotionConfigInstanceNode();

	gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* GetNative()
	{
		return m_pNative;
	}

  void Release();

private:
	gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* m_pNative;
};

}}}}}

#endif