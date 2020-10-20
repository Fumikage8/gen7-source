#if !defined(GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_LOCATORINSTANCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_LOCATORINSTANCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/math/gfl2_Quaternion_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_JointInstanceNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class LocatorInstanceNode
{
public:
  ref class Util
  {
  public:
		static void AttachToJointInstanceNode( JointInstanceNode^ pJointNodeTop, LocatorInstanceNode^ pLocatorNodeTop );
  };

	LocatorInstanceNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pNative);
	virtual ~LocatorInstanceNode(){ this->!LocatorInstanceNode(); }
	!LocatorInstanceNode();

  //<! @brief 名前取得
  System::String^ GetName();

  //<! @brief 親ノードの名前取得
  System::String^ GetParentName();

  //<! @brief 平行移動値設定
  void SetTranslate(gfl2::clr::math::Vector^ translate);

  //<! @brief 回転設定
  void SetRotation(gfl2::clr::math::Quaternion^ quaternion);

  //<! @brief スケール設定
  void SetScale(gfl2::clr::math::Vector^ scale);

  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* GetNative()
  {
    return m_pNative;
  }

private:
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* m_pNative;
};

}}}}}

#endif

