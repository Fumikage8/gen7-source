//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldNode.h
 *	@brief  �S�t�B�[���h�C���X�^���X�@���m�[�h
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_NODE_H__ )
#define __FIELD_NODE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <renderingengine/include/SceneGraph/Instance/gfl2_InstanceNode.h>

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
/**
 *  @class �t�B�[���h���m�[�h
 */
class FieldNode :public gfl2::renderingengine::scenegraph::NodeTrait<FieldNode,gfl2::renderingengine::scenegraph::instance::InstanceNode>
{
public:
  virtual ~FieldNode()
  {
  }

	static gfl2::util::FnvHash GetTypeHash()
	{
		return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::FieldNode") ) );
	}

protected:
  FieldNode()
  {
  }
};


GFL_NAMESPACE_END(Field);

#endif	// __FIELD_NODE_H__

