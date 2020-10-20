#if !defined(GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTSETNODE_CLR_H_INCLUDED)
#define GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTSETNODE_CLR_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LightNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class LightSetNode
{
public:
	LightSetNode(gfl2::renderingengine::scenegraph::instance::LightSetNode* pNode);
	~LightSetNode()
	{
		this->!LightSetNode();
	}
	!LightSetNode();

	gfl2::renderingengine::scenegraph::instance::LightSetNode* GetNative()
	{
		return m_pNative;
	}

	void AddLight(gfl2::clr::renderingengine::scenegraph::instance::LightNode^ pNode);
	void RemoveLight(gfl2::clr::renderingengine::scenegraph::instance::LightNode^ pNode);
	void RemoveAllLight();

	u32 GetLightNum();
	LightNode^ GetLightNode(s32 no);

  void SetSetNo(u32 no);
  u32 GetSetNo();

  void Release();

private:
	gfl2::renderingengine::scenegraph::instance::LightSetNode* m_pNative;
};

}}}}}

#endif