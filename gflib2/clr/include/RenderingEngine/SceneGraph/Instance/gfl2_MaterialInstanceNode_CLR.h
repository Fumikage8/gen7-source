#if !defined(GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MATERIALINSTANCENODE_CLR_H_INCLUDED)
#define GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MATERIALINSTANCENODE_CLR_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_MaterialResourceNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class MaterialInstanceNode
{
public:
  MaterialInstanceNode(gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pNative);
  virtual ~MaterialInstanceNode(){ this->!MaterialInstanceNode(); }
	!MaterialInstanceNode();

  //!< @brief マテリアルの設定値取得
  scenegraph::resource::MaterialResourceNode::AttributeParam^ GetAttributeParam();

  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* GetNative()
  {
    return m_pNative;
  }

private:
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* m_pNative;
};

}}}}}

#endif