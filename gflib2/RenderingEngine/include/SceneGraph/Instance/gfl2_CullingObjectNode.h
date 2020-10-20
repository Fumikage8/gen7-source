#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_CullingObjectNode_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_CullingObjectNode_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_TransformNode.h>
#include <math/include/gfl2_MathAABB.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>
#include <gfx/include/gfl2_OcclusionQuery.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class CullingObjectNode : public NodeTrait<CullingObjectNode, TransformNode>
{
public:

	friend class InstanceCreator;

	// ↓このクラスはこういう物を使った処理に用いられる予定です。
	//! ステータスの列挙型
	class State {
	public:
		enum {
			None							= 0,					//!< なし
			Visibled					= (1 << 0),		//!< 可視
			Moved							= (1 << 1),		//!< 移動された
			None_Clipping			= (1 << 2),		//!< クリップ処理で非表示にされることがない
			IgnoreAABBTree		= (1 << 3),		//!< AABBTreeの計算で無視する
			DrawableNode			= (1 << 4),		//!< 描画可能
			OcclusionCulling	= (1 << 5),		//!< オクルージョンカリング
		};
	};

	virtual ~CullingObjectNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CullingObjectNode") ) );
		return hash;
	}

	void SetMoved( const b32 moved );
	inline b32 IsMoved() const { return ((m_State & State::Moved) == State::Moved); }

	virtual void SetWorldTransformDirty( b32 f );

	//! ローカル座標系のAABBを設定
	inline void SetAABB( const math::AABB& aabb ) { m_AABB = aabb; SetMoved( true ); }

	//! ローカル座標系のAABBを取得
	inline const math::AABB& GetAABB() const { return m_AABB; }

	//! ワールド座標系のAABBを取得
	inline math::AABB GetWorldAABB() const { return m_AABB.Offset(math::Vector4(m_WorldMatrix[0][3], m_WorldMatrix[1][3], m_WorldMatrix[2][3])); }

	//! ステータスの設定
	inline void SetState(const u32 state );
	//! ステータスの取得
	inline u32 GetState() const { return m_State; }
	//! ステータス変更時に呼ばれる関数
	virtual void StateChangeCallBack( u32 beforeState )
  {
    (void)beforeState;
  }

	void SetAABBTree( AABBTree* pTree ){ m_pAABBTree = pTree; }
	AABBTree* GetAABBTree(){ return m_pAABBTree; }

	void SetNext( CullingObjectNode* pNode ){ m_pLinkNext = pNode; }
	CullingObjectNode* GetNext(){ return m_pLinkNext; }

	//const gfx::OcclusionQuery* GetOcclusionQuery() const { return m_pOcclusionQuery; }

protected:

	CullingObjectNode();

	math::AABB											m_AABB;
	math::Vector3										m_BeforePos;
	u32															m_State;
	AABBTree												*m_pAABBTree;//! 所属AABBTrr
	CullingObjectNode								*m_pLinkNext;
	//gfx::OcclusionQuery							*m_pOcclusionQuery;
};

inline void CullingObjectNode::SetState(const u32 state )
{
	u32			beforeState = m_State;
	m_State = state;

	if ( beforeState != m_State )
		StateChangeCallBack( beforeState );
}

}}}}

#endif

