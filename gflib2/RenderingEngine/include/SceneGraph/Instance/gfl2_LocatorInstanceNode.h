//=============================================================================
/**
 * @file    gfl2_LocatorInstanceNode.h
 * @brief   LocatorInstanceNodeクラス（ロケーターインスタンスノード。TransformNodeの１種。エフェクトの発生源などに用いられる。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.19    ishiguro  LocatorInstanceNode::Util::DetachFromParentNode関数追加
 *
 */
//=============================================================================
#if !defined(GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LOCATORINSTANCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LOCATORINSTANCENODE_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_LocatorResourceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;
class LocatorInstanceNodeNodeVisitor;

class LocatorInstanceNode : public NodeTrait<LocatorInstanceNode, TransformNode>, public NodeName
{
public:

	friend class InstanceCreator;
	friend class LocatorInstanceNodeNodeVisitor;

	class Util{
	public:
		static void AttachToJointInstanceNode( JointInstanceNode *pJointNodeTop, LocatorInstanceNode *pLocatorNodeTop );

    /**
    * @brief ロケーターを親から外す関数
    * @param pLocatorNodeTop      対象ロケーター
    * @attention 指定ロケーターが子供を持っていた場合は、アサートで止まります。
    */
    static void DetachFromParentNode( LocatorInstanceNode *pLocatorNodeTop );
	};

	virtual ~LocatorInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::LocatorInstanceNode") ) );
		return hash;
	}

	const c8* GetName(){ return GetNodeName(); }
	const c8* GetParentName(){ return m_ParentName; }

	void SetResourceNode( const resource::LocatorResourceNode *pResourceNode );

protected:
	LocatorInstanceNode();

	c8																							m_ParentName[Constant::LOCATOR_NAME_MAX];
	const resource::LocatorResourceNode							*m_LocatorResourceNode;
};

}}}}

#endif

