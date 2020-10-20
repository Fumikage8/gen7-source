#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_DRAWABLENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_DRAWABLENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>
#include <RenderingEngine/include/renderer/gfl2_DrawTag.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class DrawableNode : public NodeTrait<DrawableNode, CullingObjectNode>
{
public:

  //!< @brief 描画タグ更新情報
  struct UpdateDrawTagInfo
  {
    math::Matrix44 m_ViewMatrix;  //!< @brief ビュープ行列(深度情報を求めるのに使う)
    f32            m_Far;         //!< @brief ファークリップ

    /**
     * @brief コンストラクタ
     * @note ビュー行列を単位行列で, Farの値を1.0fで初期化
     */
    UpdateDrawTagInfo()
    {
      m_ViewMatrix = gfl2::math::Matrix44::GetIdentity();
      m_Far = 1.0f;
    }
  };

	friend class InstanceCreator;

	virtual ~DrawableNode();

	virtual u32 GetDrawTagNum() = 0;
	virtual void UpdateDrawTag(const UpdateDrawTagInfo& updateDrawTagInfo) = 0;
	virtual renderer::DrawTag* GetDrawTag( u32 no ) = 0;

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawableNode") ) );
		return hash;
	}

	void SetVisible( b32 f );
	b32 IsVisible() const { return m_IsVisible; }

	void SetBillboardType( const BillboardType& type ){ m_BillboardType = type; }
	const BillboardType& GetBillboardType() const { return m_BillboardType; }


	void SetBillboardRotation( const math::Matrix34 &rViewMat );
	void SetBillboardRotation( const math::Matrix34 &rViewMat, BillboardType type );

protected:

	DrawableNode();

	b32																								m_IsVisible;
	BillboardType																			m_BillboardType;
};

}}}}

#endif

