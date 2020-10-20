//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldTransformNode.h
 *	@brief  フィールドトランスフォームノード基底クラス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_TRANSFORM_NODE_H__ )
#define __FIELD_TRANSFORM_NODE_H__
#pragma once

#include <renderingengine/include/SceneGraph/Instance/gfl2_TransformNode.h>

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
class FieldTransformNode :public gfl2::renderingengine::scenegraph::NodeTrait<FieldTransformNode,gfl2::renderingengine::scenegraph::instance::TransformNode>
{
public:
  virtual ~FieldTransformNode()
  {
  }

	static gfl2::util::FnvHash GetTypeHash()
	{
		return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::FieldTransformNode") ) );
	}

protected:
  FieldTransformNode()
  {
  }
};


GFL_NAMESPACE_END(Field);

#endif	// __FIELD_TRANSFORM_NODE_H__

