#pragma once
#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_DRAWABLENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_DRAWABLENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawableNode.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class DrawableNode
{
public:
  ref class UpdateDrawTagInfo
  {
  public:
    UpdateDrawTagInfo( gfl2::clr::math::Matrix^ viewProjectionMatrix );
    ~UpdateDrawTagInfo()
    {
      this->!UpdateDrawTagInfo();
    }
    !UpdateDrawTagInfo();

    gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo* GetNative()
    {
      return m_pNative;
    }

    gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo* m_pNative;
  };

	// 純粋仮想関数を持っているのでnew出来ない. 従って外から渡す
	DrawableNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNative );
	virtual ~DrawableNode(){ this->!DrawableNode(); }
	!DrawableNode();

	gfl2::renderingengine::scenegraph::instance::DrawableNode* GetNative()
	{
		return m_pNative;
	}

protected:
//private:
	gfl2::renderingengine::scenegraph::instance::DrawableNode* m_pNative;
};

}}}}}

#endif

