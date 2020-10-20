//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldRootNode.h
 *	@brief  フィールドの各種ルートノード管理
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_ROOT_NODE_H__ )
#define __FIELD_ROOT_NODE_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// Field
#include "Field/FieldStatic/include/FieldManagerNode.h"

GFL_NAMESPACE_BEGIN(Field);
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class FieldRootNode
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldRootNode );

public:
  FieldRootNode() : m_pResourceRoot(NULL),m_pTransformRoot(NULL),m_pManagerRoot(NULL),m_pInstanceRoot(NULL),m_pAfterTransRoot(NULL)
  {
  }

  ~FieldRootNode()
  {
    Terminate();
  }

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::gfx::IGLAllocator* pAllocator )
  {
    m_pResourceRoot   = gfl2::renderingengine::scenegraph::resource::ResourceNode::Create( pAllocator );
    m_pTransformRoot  = gfl2::renderingengine::scenegraph::instance::TransformNode::Create( pAllocator );
    m_pManagerRoot    = gfl2::renderingengine::scenegraph::instance::InstanceNode::Create( pAllocator );
    m_pInstanceRoot   = gfl2::renderingengine::scenegraph::instance::InstanceNode::Create( pAllocator );
    m_pAfterTransRoot = gfl2::renderingengine::scenegraph::instance::TransformNode::Create( pAllocator );
  }

  /**
   *  @brief  破棄
   */
  void Terminate(void)
  {
#if PM_DEBUG
    if(m_pResourceRoot){
      // このタイミングで、全て子ノードは全て解放されていること
      GFL_ASSERT( m_pResourceRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pTransformRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pManagerRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pInstanceRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pAfterTransRoot->GetChildCount() == 0 );
    }
#endif

    GFL_SAFE_DELETE( m_pResourceRoot );
    GFL_SAFE_DELETE( m_pTransformRoot );
    GFL_SAFE_DELETE( m_pManagerRoot );
    GFL_SAFE_DELETE( m_pInstanceRoot );
    GFL_SAFE_DELETE( m_pAfterTransRoot );
  }

  /**
   *  @brief  リソースルートノードの子ノードに追加
   */
  inline void AddResourceChild( gfl2::renderingengine::scenegraph::resource::ResourceNode* resource )
  {
    m_pResourceRoot->AddChild(resource);
  }

  /**
   *  @brief  リソースルートノードの子ノードから外す
   */
  inline bool RemoveResourceChild( gfl2::renderingengine::scenegraph::resource::ResourceNode* resource )
  {
    return m_pResourceRoot->RemoveChild(resource);
  }

  /**
   *  @brief  トランスフォームノードの子ノードに追加
   */
  inline void AddTransformChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    m_pTransformRoot->AddChild(transform);
  }

  /**
   *  @brief  トランスフォームノードの子ノードから外す
   */
  inline bool RemoveTransformChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    return m_pTransformRoot->RemoveChild(transform);
  }

  /**
   *  @brief  マネージャールートノードの子ノードに追加
   */
  inline void AddManagerChild( IFieldManagerNode* manager )
  {
    m_pManagerRoot->AddChild(manager);
  }

  /**
   *  @brief  マネージャールートノードの子ノードから外す
   */
  inline bool RemoveManagerChild( IFieldManagerNode* manager )
  {
    return m_pManagerRoot->RemoveChild(manager);
  }

  /**
   *  @brief  インスタンスルートノードの子ノードに追加
   */
  inline void AddInstanceChild( FieldNode* instance )
  {
    m_pInstanceRoot->AddChild(instance);
  }

  /**
   *  @brief  インスタンスルートノードの子ノードから外す
   */
  inline bool RemoveInstanceChild( FieldNode* instance )
  {
    return m_pInstanceRoot->RemoveChild(instance);
  }

  /**
   *  @brief  トランスフォームノードの子ノードに追加
   */
  inline void AddAfterTransChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    m_pAfterTransRoot->AddChild(transform);
  }

  /**
   *  @brief  トランスフォームノードの子ノードから外す
   */
  inline bool RemoveAfterTransChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    return m_pAfterTransRoot->RemoveChild(transform);
  }

  /**
   *  @brief  各種ルートノードのGetter
   */
  inline gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceRootNode() const { return m_pResourceRoot; }
  inline gfl2::renderingengine::scenegraph::instance::TransformNode* GetTransformRootNode() const { return m_pTransformRoot; }
  inline gfl2::renderingengine::scenegraph::instance::InstanceNode* GetManagerRootNode() const { return m_pManagerRoot; }
  inline gfl2::renderingengine::scenegraph::instance::InstanceNode* GetInstanceRootNode() const { return m_pInstanceRoot; }
  inline gfl2::renderingengine::scenegraph::instance::TransformNode* GetAfterTransRootNode() const { return m_pAfterTransRoot; }

private:
  gfl2::renderingengine::scenegraph::resource::ResourceNode*   m_pResourceRoot;  //! フィールドリソースルートノード
  gfl2::renderingengine::scenegraph::instance::TransformNode*  m_pTransformRoot; //! トランスフォームルートノード
  gfl2::renderingengine::scenegraph::instance::InstanceNode*   m_pManagerRoot;   //! マネージャールートノード
  gfl2::renderingengine::scenegraph::instance::InstanceNode*   m_pInstanceRoot;  //! インスタンスルートノード
  gfl2::renderingengine::scenegraph::instance::TransformNode*  m_pAfterTransRoot;//! 後更新トランスフォームルートノード
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_ROOT_NODE_H__

