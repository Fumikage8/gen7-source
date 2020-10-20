#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_SCENEGRAPHMANAGER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_SCENEGRAPHMANAGER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// 全ノードダンプデバッグ
#define SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE (GFL_DEBUG && 0)

namespace gfl2 { namespace renderingengine { namespace scenegraph {

	namespace instance {
		class CullingObjectNode;
	}

class SceneGraphManager : public gfx::GLMemory, public gfx::IGLGLCallBack
{
public:
	friend class scenegraph::Node;

	struct InitDescription{
		InitDescription( u32 nodeSize ):
			m_NodeStackSize( nodeSize ),
			m_AABBTreeSize( math::Vector4(-10000.0f, -10000.0f, -10000.0f), math::Vector4(10000.0f, 10000.0f, 10000.0f) ),
			m_AABBDepth( 3 )
		{
			
		}
		u32							m_NodeStackSize;
		math::AABB			m_AABBTreeSize;
		u32							m_AABBDepth;
	};

	static void Initialize( gfx::IGLAllocator* pAllocator, InitDescription desc = InitDescription( 1024 * 2 ));
	static void Terminate();
	static void TraverseModel();
	static void TraverseModelFast();
  static void TraverseModelFastFromChild( gfl2::renderingengine::scenegraph::DagNode *pNode );
  static void TraverseModelFast( gfl2::renderingengine::scenegraph::DagNode *pNode );
  static void TraverseApplyBillboard(math::Matrix34 *viewMtx);
	static void TraverseApplyBillboard(math::Matrix44 *viewMtx);

	static void AddChild( scenegraph::Node* pNode );
	static void RemoveChild( scenegraph::Node* pNode );

	static void HangOnTheTree( scenegraph::instance::CullingObjectNode* pNode );
	static void HangOffTheTree( scenegraph::instance::CullingObjectNode* pNode );
	static void UpdateAABBTree();
	static const AABBTree& GetAABBTree(){ return s_Gp->m_AABBTreeTop; }

	template<class TatgetNodeType, class Implement>
	inline static void Traverse( Traverser::Visitor<TatgetNodeType, Implement> *pIVisitor, u32 depth = Traverser::DEFAULT_DEPTH );

	template<class TatgetNodeType, class Implement>
	inline static void TraverseFast( Traverser::Visitor<TatgetNodeType, Implement> *pIVisitor, u32 depth = Traverser::DEFAULT_DEPTH );

  /**
   * @brief ルートノード取得
   */
  inline static DagNode* GetRootNode()
  {
		if ( s_Gp == NULL )			return NULL;

    return s_Gp->m_RootNode;
  }

	static void AddDrawingNode( scenegraph::instance::ModelInstanceNode* pDrawableNode );
	static b32 IsDrawingNode( scenegraph::instance::ModelInstanceNode* pDrawableNode );


  /**
   * @brief 現在のノード数取得
   */
  inline static u32 GetNodeCount( void )
  {
    if ( s_Gp == NULL ) return 0;
    return s_Gp->m_NodeCount;
  }

  /**
   * @brief ノード数上限取得
   */
  inline static u32 GetNodeMax( void )
  {
    if ( s_Gp == NULL ) return 0;
    return s_Gp->m_NodeMax;
  }

#if GFL_DEBUG
  /**
   * @brief シーングラフをコンソールにダンプ
   */
  static void Dump( void );
#endif

private:

#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  friend class DebugDump;

  /**
   * @brief デバッグ用ノード追加
   */
  inline static void DebugAddNode( scenegraph::Node* pNode )
  {
    if ( s_Gp == NULL ) return;
    s_Gp->m_DebugNodeContainer.push_back(pNode);
  }
  /**
   * @brief デバッグ用ノード削除
   */
  inline static void DebugRemoveNode( scenegraph::Node* pNode )
  {
    if ( s_Gp == NULL ) return;
    s_Gp->m_DebugNodeContainer.remove(pNode);
  }
#endif

  void SetUp( const InitDescription &rDesc );

  inline static void IncrementNodeCount( void )
  {
    if ( s_Gp == NULL ) return;
    GFL_ASSERT( s_Gp->m_NodeCount < s_Gp->m_NodeMax );
    ++ s_Gp->m_NodeCount;
  }

  inline static void DecrementNodeCount( void )
  {
    if ( s_Gp == NULL ) return;
    GFL_ASSERT( s_Gp->m_NodeCount > 0 );
    -- s_Gp->m_NodeCount;
  }


	SceneGraphManager( const InitDescription &rDesc );
	virtual ~SceneGraphManager();

	virtual void DrawEnd();

	static SceneGraphManager													*s_Gp;

  u32                                               m_NodeMax;
  u32                                               m_NodeCount;
#if SCENE_GRAPH_MANAGER_DEBUG_DUMP_ALL_NODE
  gfx::GLFixedSizeContainer<scenegraph::Node*>      m_DebugNodeContainer;
  u8                                                *m_pDebugNodeFlagList;
#endif

  DagNode*                                          m_RootNode;
  resource::ResourceNode*                           m_ResourceRootNode;
  instance::InstanceNode*                           m_InstanceRootNode;
  instance::TransformNode*                          m_TransformRootNode;
	AABBTree																					m_AABBTreeTop;

	struct DrawingNodeList{
		u32																							m_NodeNum;
		scenegraph::instance::ModelInstanceNode					**m_pDrawableNodeList;
	};
	u32																								m_DrawingNodeListDBCnt;
	u32																								m_DrawingNodeListMax;
	DrawingNodeList																		m_DrawingNodeListDB[2];
};

template<class TatgetNodeType, class Implement>
inline void SceneGraphManager::Traverse( Traverser::Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth )
{
	/*
	m_RootNode
	   |
	   +----+------m_ResourceRootNode
		      |
					+------m_InstanceRootNode
					                |
													+----------m_TransformRootNode

  となっています。
	リソースノードを継承しているノードはm_ResourceRootNodeの子としてぶら下がり、
	インスタンスノードを継承しているノードはm_InstanceRootNodeの子としてぶら下がります。
	ただし、トランスフォームノードを継承しているノードだった場合は、m_TransformRootNodeよりぶら下がります。
	m_RootNodeから手繰れば、すべてのDagNodeにアクセスできますが、検索数は多くなります。
	*/

	if ( s_Gp )
	{//特殊化できなくなったのでここで判定
		if( TatgetNodeType::GetTypeHash() == resource::ResourceNode::GetTypeHash() )
		{
			Traverser::Traverse<TatgetNodeType, Implement>( s_Gp->m_ResourceRootNode, pVisitor, depth );
		}
		else if( TatgetNodeType::GetTypeHash() == instance::InstanceNode::GetTypeHash() )
		{
			Traverser::Traverse<TatgetNodeType, Implement>( s_Gp->m_InstanceRootNode, pVisitor, depth );
		}
		else if( TatgetNodeType::GetTypeHash() == instance::TransformNode::GetTypeHash() )
		{
			Traverser::Traverse<TatgetNodeType, Implement>( s_Gp->m_TransformRootNode, pVisitor, depth );
		}
		else
		{
			Traverser::Traverse<TatgetNodeType, Implement>( s_Gp->m_RootNode, pVisitor, depth );
		}
	}
}

template<class TatgetNodeType, class Implement>
inline void SceneGraphManager::TraverseFast( Traverser::Visitor<TatgetNodeType, Implement> *pVisitor, u32 depth )
{
	if ( s_Gp )
	{//特殊化できなくなったのでここで判定
		if( TatgetNodeType::GetTypeHash() == resource::ResourceNode::GetTypeHash() )
		{
			Traverser::TraverseFast<TatgetNodeType, Implement>( s_Gp->m_ResourceRootNode, pVisitor, depth );
		}
		else if( TatgetNodeType::GetTypeHash() == instance::InstanceNode::GetTypeHash() )
		{
			Traverser::TraverseFast<TatgetNodeType, Implement>( s_Gp->m_InstanceRootNode, pVisitor, depth );
		}
		else if( TatgetNodeType::GetTypeHash() == instance::TransformNode::GetTypeHash() )
		{
			Traverser::TraverseFast<TatgetNodeType, Implement>( s_Gp->m_TransformRootNode, pVisitor, depth );
		}
		else
		{
			Traverser::TraverseFast<TatgetNodeType, Implement>( s_Gp->m_RootNode, pVisitor, depth );
		}
	}
}

/*
template<>
inline void SceneGraphManager::Traverse<resource::ResourceNode>( Traverser::Visitor<resource::ResourceNode> *pVisitor, u32 depth )
{
	Traverser::Traverse<resource::ResourceNode>( &s_Gp->m_ResourceRootNode, pVisitor, depth );
}

template<>
inline void SceneGraphManager::Traverse<instance::InstanceNode>( Traverser::Visitor<instance::InstanceNode> *pVisitor, u32 depth )
{
	Traverser::Traverse<instance::InstanceNode>( &s_Gp->m_InstanceRootNode, pVisitor, depth );
}

template<class Implement>
inline void SceneGraphManager::Traverse<instance::TransformNode, Implement>( Traverser::Visitor<instance::TransformNode, Implement> *pVisitor, u32 depth )
{
	Traverser::Traverse<instance::TransformNode, Implement>( &s_Gp->m_TransformRootNode, pVisitor, depth );
}
*/
}}}

#endif

