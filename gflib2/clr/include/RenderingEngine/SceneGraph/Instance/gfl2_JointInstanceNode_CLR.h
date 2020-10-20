#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_LIGHTNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_LIGHTNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/RenderingEngine/gfl2_RenderingEngineEnums_CLR.h>
#include <clr/include/gfx/gfl2_enums_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class JointInstanceNode
{
public:
	JointInstanceNode(gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pNative);
	~JointInstanceNode()
	{
		this->!JointInstanceNode();
	}
	!JointInstanceNode();

	gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetNative()
	{
		return m_pNative;
	}

	System::String^ GetName();
	System::String^ GetParentName();
  math::Matrix^ GetWorldMatrix();

  void Release();

private:
	gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pNative;
};

}}}}}

#endif