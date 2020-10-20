#if !defined(GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODE_CLR_H_INCLUDED)
#define GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODE_CLR_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class MaterialResourceNode
{
public:
  ref class AttributeParam
  {
  public:
    AttributeParam(gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* pNative);
    ~AttributeParam()
    {
      this->!AttributeParam();
    }
    !AttributeParam();

    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* GetNative()
    {
      return m_pNative;
    }

    void SetLightSetNo(s16 no);
    s16 GetLightSetNo();

  private:
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* m_pNative;
  };
};

}}}}}

#endif