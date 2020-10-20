#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_INSTANCECREATOR_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_INSTANCECREATOR_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CameraNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_EffectnodeLocatorInstanceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator
{
public:

	/**
	* @brief ResourceNodeから、ModelInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	static ModelInstanceNode* CreateModelInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc);
	static ModelInstanceNode* CreateModelInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc, ModelInstanceNode* pJointReferenceModel);

	/**
  * @brief ResourceNodeから、ModelInstanceNodeを作成(テクスチャとシェーダーを別に渡せる版)
	* @param pModelSrc                 モデルリソースノードのポインタ
  * @param pTextureShaderNodeRoot    テクスチャとシェーダーリソースノード配列のポインタ 
  *
	* @return			生成したノードのポインタ。
	*/ 
	static ModelInstanceNode* CreateModelInstanceNode(
      gfx::IGLAllocator* pAllocator,
      resource::ResourceNode* pSrc,
      DagNode* pTextureShaderNodeRoot
      );
	
  /**
	* @brief ResourceNodeから、ModelInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	static void SetupModelInstanceNode( ModelInstanceNode *pNode, resource::ResourceNode* pSrc );
	
  /**
	* @brief ResourceNodeから、ModelInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
  * @param pTextureShaderNode テクスチャ・シェーダーノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
  static void SetupModelInstanceNode( ModelInstanceNode *pNode, resource::ResourceNode* pSrc, DagNode* pTextureShaderNodeRoot );

	/**
	* @brief ResourceNodeから、LocatorInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	//static LocatorInstanceNode* CreateLocatorInstanceNode(resource::ResourceNode* pSrc);
  static LocatorInstanceNode* CreateLocatorInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc);

  /**
	* @brief 単一のLocatorInstanceNodeを作成
	* @return			生成したノードのポインタ。
	*/
  static LocatorInstanceNode* CreateLocatorInstanceNode(gfx::IGLAllocator* pAllocator);

  /**
	* @brief ResourceNodeから、ViewerLocatorInstanceNodeを作成
	* @param nodeName ノードの名前
	* @param parentName 親の名前
	* @return			生成したノードのポインタ。
	*/ 
  static ViewerLocatorInstanceNode* CreateViewerLocatorInstanceNode(gfx::IGLAllocator* pAllocator, const c8* nodeName, const c8* parentName);

	/**
	* @brief ResourceNodeから、LightSetNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	static LightSetNode* CreateLightSetNode(gfx::IGLAllocator * pAllocator, resource::ResourceNode* pSrc = NULL);

	/**
	* @brief LightNodeを作成
	* @return			生成したノードのポインタ。
	*/ 
	static LightNode* CreateLightNode(gfx::IGLAllocator* pAllocator);

	/**
	* @brief ResourceNodeから、CameraNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	static CameraNode* CreateCameraNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc = NULL);

	/**
	* @brief ResourceNodeから、DrawEnvNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
  static DrawEnvNode* CreateDrawEnvNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc = NULL);
	
	/**
	* @brief ResourceNodeから、MotionConfigInstanceNodeを作成
	* @param pSrc リソースノードのポインタ
	* @return			生成したノードのポインタ。
	*/ 
	static MotionConfigInstanceNode* CreateMotionConfigInstanceNode(gfx::IGLAllocator* pAllocator, resource::ResourceNode* pSrc = NULL);

  /**
	* @brief EffectNodeLocatorInstanceNodeを作成
	* @param parentName 親の名前
	* @return			生成したノードのポインタ。
	*/ 
  static EffectNodeLocatorInstanceNode* CreateEffectNodeLocatorInstanceNode(gfx::IGLAllocator* pAllocator, const c8* parentName);

};

}}}}

#endif
