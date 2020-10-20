#if !defined(GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_RENDERINGENGINEINSTANCECREATOR_CLR_H_INCLUDED)
#define GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_RENDERINGENGINEINSTANCECREATOR_CLR_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LocatorInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawEnvNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_LightNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_MotionConfigInstanceNode_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class InstanceCreator
{
public:
  /**
	* @brief ResourceNodeから、ModelInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ
	*/
	static ModelInstanceNode^ CreateModelInstanceNode(resource::ResourceNode^ pSrc);

  /**
	* @brief LocatorInstanceNodeを作成
	* @return			生成したノードのポインタ
	*/
  static LocatorInstanceNode^ CreateLocatorInstanceNode();

  /**
	* @brief ViewerLocatorInstanceNodeを作成
  * @param parentName 親の名前
	* @return			生成したノードのポインタ
	*/
  static ViewerLocatorInstanceNode^ CreateViewerLocatorInstanceNode(System::String^ nodeName, System::String^ parentName);

  /**
	* @brief ResourceNodeから、DrawEnvNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ
	*/
  static DrawEnvNode^ CreateDrawEnvNode(resource::ResourceNode^ pSrc);

  static DrawEnvNode^ CreateDrawEnvNode();
  static LightSetNode^ CreateLightSetNode();
  static LightNode^ CreateLightNode();

  static MotionConfigInstanceNode^ CreateMotionConfigInstanceNode(resource::ResourceNode^ pSrc);
};

}}}}}

#endif