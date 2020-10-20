//=============================================================================
/**
 * @file    gfl2_DrawableNode.cpp
 * @brief   DrawableNodeクラス（描画可能ノードを示すクラスであり、その機能を保持するクラス。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.18    ishiguro  [fix]cov_ctr[10295][10314] :範囲外コピーが発生していたので修正
 *
 */
//=============================================================================
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawableNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

DrawableNode::DrawableNode() : NodeTrait<DrawableNode, CullingObjectNode>(),
	m_IsVisible(true),
	m_BillboardType( BillboardType::None )
{
	m_State |= CullingObjectNode::State::DrawableNode;
}

DrawableNode::~DrawableNode()
{
}

void DrawableNode::SetVisible( b32 f )
{
	m_IsVisible = f;
	if ( m_IsVisible )
	{
		m_State |= CullingObjectNode::State::Visibled;
		SceneGraphManager::HangOnTheTree( this );
	}
	else
	{
		m_State &= ~CullingObjectNode::State::Visibled;
		SceneGraphManager::HangOffTheTree( this );
	}
}

void DrawableNode::SetBillboardRotation( const math::Matrix34 &rViewMat )
{
	this->SetBillboardRotation( rViewMat, m_BillboardType );
}

void DrawableNode::SetBillboardRotation( const math::Matrix34 &rViewMat, BillboardType type )
{
	if ( type == BillboardType::None )
		return;

  math::Matrix44          viewMat( rViewMat );

	math::Vector4						targetPosition( m_LocalSRT.translate.x, m_LocalSRT.translate.y, m_LocalSRT.translate.z );
	math::Matrix44						billboardMat;
	math::Matrix34					billboardMat34;

	switch( type ){
	case BillboardType::ScreenY:
		billboardMat = math::Matrix44::GetBillboardMatrixRotationY( viewMat );
		break;
	case BillboardType::ScreenXYZ:
		billboardMat = math::Matrix44::GetBillboardMatrixRotationXYZ( viewMat );
		break;
	}
			
	math::Quaternion			q;

	billboardMat[3] = targetPosition;
	billboardMat = billboardMat.Transpose();
	std::memcpy( &billboardMat34, &billboardMat, sizeof(billboardMat34) );
			
	q.MatrixToQuaternion( billboardMat34 );

	this->SetLocalRotation( q );
}

}}}}//gfl2::renderingengine::scenegraph::instance

