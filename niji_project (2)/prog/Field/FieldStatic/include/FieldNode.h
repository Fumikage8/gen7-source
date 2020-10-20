//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldNode.h
 *	@brief  全フィールドインスタンス　基底ノード
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
/**
 *  @class フィールド基底ノード
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

