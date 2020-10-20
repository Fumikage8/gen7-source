//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldRootNode.h
 *	@brief  �t�B�[���h�̊e�탋�[�g�m�[�h�Ǘ�
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
#include "ViewerProc/include/FieldStatic/FieldManagerNode.h"

GFL_NAMESPACE_BEGIN(Field);
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
class FieldRootNode
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( FieldRootNode );

public:
  FieldRootNode() : m_pResourceRoot(NULL),m_pTransformRoot(NULL),m_pManagerRoot(NULL),m_pInstanceRoot(NULL)
  {
  }

  ~FieldRootNode()
  {
    Terminate();
  }

  /**
   *  @brief  ������
   */
  void Initialize( gfl2::gfx::IGLAllocator* pAllocator )
  {
    m_pResourceRoot   = gfl2::renderingengine::scenegraph::resource::ResourceNode::Create( pAllocator );
    m_pTransformRoot  = gfl2::renderingengine::scenegraph::instance::TransformNode::Create( pAllocator );
    m_pManagerRoot    = gfl2::renderingengine::scenegraph::instance::InstanceNode::Create( pAllocator );
    m_pInstanceRoot   = gfl2::renderingengine::scenegraph::instance::InstanceNode::Create( pAllocator );
  }

  /**
   *  @brief  �j��
   */
  void Terminate(void)
  {
#if PM_DEBUG
    if(m_pResourceRoot){
      // ���̃^�C�~���O�ŁA�S�Ďq�m�[�h�͑S�ĉ������Ă��邱��
      GFL_ASSERT( m_pResourceRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pTransformRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pManagerRoot->GetChildCount() == 0 );
      GFL_ASSERT( m_pInstanceRoot->GetChildCount() == 0 );
    }
#endif

    GFL_SAFE_DELETE( m_pResourceRoot );
    GFL_SAFE_DELETE( m_pTransformRoot );
    GFL_SAFE_DELETE( m_pManagerRoot );
    GFL_SAFE_DELETE( m_pInstanceRoot );
  }

  /**
   *  @brief  ���\�[�X���[�g�m�[�h�̎q�m�[�h�ɒǉ�
   */
  inline void AddResourceChild( gfl2::renderingengine::scenegraph::resource::ResourceNode* resource )
  {
    m_pResourceRoot->AddChild(resource);
  }

  /**
   *  @brief  ���\�[�X���[�g�m�[�h�̎q�m�[�h����O��
   */
  inline bool RemoveResourceChild( gfl2::renderingengine::scenegraph::resource::ResourceNode* resource )
  {
    return m_pResourceRoot->RemoveChild(resource);
  }

  /**
   *  @brief  �g�����X�t�H�[���m�[�h�̎q�m�[�h�ɒǉ�
   */
  inline void AddTransformChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    m_pTransformRoot->AddChild(transform);
  }

  /**
   *  @brief  �g�����X�t�H�[���m�[�h�̎q�m�[�h����O��
   */
  inline bool RemoveTransformChild( gfl2::renderingengine::scenegraph::instance::TransformNode* transform )
  {
    return m_pTransformRoot->RemoveChild(transform);
  }

  /**
   *  @brief  �}�l�[�W���[���[�g�m�[�h�̎q�m�[�h�ɒǉ�
   */
  inline void AddManagerChild( IFieldManagerNode* manager )
  {
    m_pManagerRoot->AddChild(manager);
  }

  /**
   *  @brief  �}�l�[�W���[���[�g�m�[�h�̎q�m�[�h����O��
   */
  inline bool RemoveManagerChild( IFieldManagerNode* manager )
  {
    return m_pManagerRoot->RemoveChild(manager);
  }

  /**
   *  @brief  �C���X�^���X���[�g�m�[�h�̎q�m�[�h�ɒǉ�
   */
  inline void AddInstanceChild( FieldNode* instance )
  {
    m_pInstanceRoot->AddChild(instance);
  }

  /**
   *  @brief  �C���X�^���X���[�g�m�[�h�̎q�m�[�h����O��
   */
  inline bool RemoveInstanceChild( FieldNode* instance )
  {
    return m_pInstanceRoot->RemoveChild(instance);
  }

  /**
   *  @brief  �e�탋�[�g�m�[�h��Getter
   */
  inline gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceRootNode() const { return m_pResourceRoot; }
  inline gfl2::renderingengine::scenegraph::instance::TransformNode* GetTransformRootNode() const { return m_pTransformRoot; }
  inline gfl2::renderingengine::scenegraph::instance::InstanceNode* GetManagerRootNode() const { return m_pManagerRoot; }
  inline gfl2::renderingengine::scenegraph::instance::InstanceNode* GetInstanceRootNode() const { return m_pInstanceRoot; }

private:
  gfl2::renderingengine::scenegraph::resource::ResourceNode*   m_pResourceRoot;  //! �t�B�[���h���\�[�X���[�g�m�[�h
  gfl2::renderingengine::scenegraph::instance::TransformNode*  m_pTransformRoot; //! �g�����X�t�H�[�����[�g�m�[�h
  gfl2::renderingengine::scenegraph::instance::InstanceNode*   m_pManagerRoot;   //! �}�l�[�W���[���[�g�m�[�h
  gfl2::renderingengine::scenegraph::instance::InstanceNode*   m_pInstanceRoot;  //! �C���X�^���X���[�g�m�[�h
};

GFL_NAMESPACE_END(Field);

#endif	// __FIELD_ROOT_NODE_H__

