//=============================================================================
/**
 * @file    gfl2_DagNode.cpp
 * @brief   DagNodeクラス（親子関係を持つ基本ノード）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2015.10.30    tamada  兄弟同士のリンクを双方向リストに変更

 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>

//下記定義を有効にするとDagNodeの整合性チェックを実行時に行います。
//#define CHECK_DAGNODE_LINK


namespace gfl2 { namespace renderingengine { namespace scenegraph {

  DagNode::DagNode() : NodeTrait<DagNode, Node>(),
	m_pParent( NULL ),
	m_pChild( NULL ),
  m_pPrev( NULL ),
	m_pSibling( NULL ),
	m_ChildCnt( 0 )
{
}

bool DagNode::IsCorrectDagNode( void ) const
{
  GFL_ASSERT( this->m_pParent != (void*)0xffffffff );  // released memory
#ifndef CHECK_DAGNODE_LINK
   return true;
#else
  DagNode * cur = m_pChild;
  DagNode * prev = NULL;
  u32 sib_count = 0;
  while ( cur )
  {
    GFL_ASSERT( cur->m_pParent == this );
    GFL_ASSERT( cur->m_pParent != (void*)0xffffffff );  // released memory
    ++ sib_count;
    prev = cur;
    cur = cur->m_pSibling;
    if ( cur != NULL && prev != cur->m_pPrev )
    {
      GFL_PRINT("DagNode Link Broken!!(%08p)-->(%08p)\n",prev, cur );
    }
  }
  return ( m_ChildCnt == sib_count );
#endif  // CHECK_DAGNODE_LINK
}

DagNode::~DagNode()
{
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif

  if ( m_pParent )
  {
    m_pParent->RemoveChild( this );
  }

	DagNode		*sibling;
	DagNode		*child = m_pChild;

	while ( child )
	{
		sibling = child->m_pSibling;
		//child->m_pParent = NULL;
		delete child;
		child = sibling;
	}
}

void DagNode::AddChild( DagNode *child )
{
	GFL_ASSERT( child );
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif
  if ( child->m_pParent )
  {
    child->m_pParent->RemoveChild( child );
  }

	child->m_pParent = this;
	
	if( m_pChild == NULL )
	{
    child->m_pPrev = NULL;
    child->m_pSibling = NULL;
		m_pChild = child;
	}
  else
  {
    //先頭に追加する（検索速度0なので）
    DagNode* next = m_pChild;
    next->m_pPrev = child;
    child->m_pPrev = NULL;
    child->m_pSibling = next;
    m_pChild = child;
  }
	++m_ChildCnt;
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif
}

bool DagNode::RemoveChild( DagNode *child )
{
  GFL_ASSERT( child );
  //@attention 子を持たないリクエストを製品版ではケアしておく。本来、開発中はAssertすべき。
  if ( m_pChild == NULL )
  {
    //GFL_PRINT("DagNode::RemoveChild() called but HAS NO CHILD!!\n");
    return false;
  }

#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
  child->IsCorrectDagNode();
#endif

  if( child->m_pParent != this )
  {
    //GFL_PRINT("DagNode::RemoveChild() called but IS NOT PARENT!!\n");
    //GFL_ASSERT( 0 );
    return false;
  }

  if ( m_pChild == child )
  {
    GFL_ASSERT( child->m_pPrev == NULL );
    m_pChild = child->m_pSibling;
  }
  if ( child->m_pPrev )
  {
    child->m_pPrev->m_pSibling = child->m_pSibling;
  }
  if ( child->m_pSibling )
  {
    child->m_pSibling->m_pPrev = child->m_pPrev;
  }
  child->m_pParent = NULL;
  child->m_pSibling = NULL;
  child->m_pPrev = NULL;

	--m_ChildCnt;
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif
	return true;
}

bool DagNode::RemoveFromParent()
{
	if( GetParent() )
	{
		return GetParent()->RemoveChild( this );
	}

	return true;
}

DagNode* DagNode::GetChild( u32 childNo )
{
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif
	if ( childNo == 0 )
		return m_pChild;

	DagNode	*current = m_pChild;

	while ( current->m_pSibling )
	{
		current = current->m_pSibling;
		--childNo;
		if ( childNo == 0 )
			return current;
	}

	return NULL;
}

const DagNode* DagNode::GetChild( u32 childNo ) const
{
#ifdef CHECK_DAGNODE_LINK
  IsCorrectDagNode();
#endif
	if ( childNo == 0 )
		return m_pChild;

	const DagNode	*current = m_pChild;

	while ( current->m_pSibling )
	{
		current = current->m_pSibling;
		--childNo;
		if ( childNo == 0 )
			return current;
	}

	return NULL;
}

}}}

