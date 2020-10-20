//=============================================================================
/**
 * @file    gfl2_DagNode.h
 * @brief   DagNodeクラス（親子関係を持つ基本ノード）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2015.10.30    tamada  兄弟同士のリンクを双方向リストに変更

 */
//=============================================================================
#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_DAGNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_DAGNODE_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_NodeTrait.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Node.h>
#include <util/include/gfl2_FnvHash.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

class DagNode : public NodeTrait<DagNode, Node>
{
public:
	friend class SceneGraphManager;

	virtual ~DagNode();

	//! @brief 子として追加する。
	//! @note 子がすでに親を持っていた場合、その親からは外して、自分の子とする。親は一人。
	virtual void AddChild( DagNode* child);
	bool RemoveChild(DagNode* child);
	bool RemoveFromParent();

	inline DagNode* GetParent( void ){ return m_pParent; }
	inline const DagNode* GetParent( void ) const { return m_pParent; }
	inline DagNode* GetSibling( void ){ return m_pSibling; }
	inline const DagNode* GetSibling( void ) const { return m_pSibling; }

	inline u32 GetChildCount() const { return m_ChildCnt; }
	inline DagNode* GetChild( void ){ return m_pChild; }
	inline const DagNode* GetChild( void ) const { return m_pChild; }
	DagNode* GetChild( u32 childNo );
	const DagNode* GetChild( u32 childNo ) const;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode") ) );
		return hash;
	}

protected:
	DagNode();

	DagNode*	m_pParent;
	DagNode*	m_pChild;
  DagNode*  m_pPrev;
	DagNode*	m_pSibling;
	u32				m_ChildCnt;

  bool IsCorrectDagNode( void ) const;
};

}}}

#endif

