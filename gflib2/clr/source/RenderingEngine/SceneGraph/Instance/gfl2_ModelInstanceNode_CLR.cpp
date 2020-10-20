#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>

#include <math/include/gfl2_EulerRotation.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

ModelInstanceNode::ModelInstanceNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNative ) : 
	DrawableNode(pNative)
{
}

ModelInstanceNode::!ModelInstanceNode()
{
}

#if 0
void ModelInstanceNode::SetLocalMatrix(gfl2::clr::math::Matrix^ mat)
{
	gfl2::math::Matrix				nativeMat;
	for(s32 col = 0; col < 4; ++col)
	{
		for(s32 row = 0; row < 4; ++row)
		{
			float elem = mat->GetElem(col, row);
			nativeMat.SetElem(col, row, elem);
		}
	}
	this->m_pNative->SetLocalMatrix( nativeMat );
}
#endif

//! @brief スケール設定
void ModelInstanceNode::SetScale(gfl2::clr::math::Vector^ scale)
{
  this->m_pNative->SetLocalScale(scale->GetNativeVector3());
}

//! @brief スケール取得
gfl2::clr::math::Vector^ ModelInstanceNode::GetScale()
{
  return gcnew gfl2::clr::math::Vector(
    this->m_pNative->GetLocalScale()
    );
}

//! @brief 回転設定
void ModelInstanceNode::SetRotateZYX(gfl2::clr::math::Vector^ rotate)
{
  this->m_pNative->SetLocalRotation(
    gfl2::math::EulerRotation(rotate->x, rotate->y, rotate->z, gfl2::math::EulerRotation::RotationOrderZYX).ToQuaternion()
    );
}

//! @brief 回転取得
gfl2::clr::math::Vector^ ModelInstanceNode::GetRotateZYX()
{
  gfl2::math::EulerRotation rot = gfl2::math::EulerRotation(this->m_pNative->GetLocalRotation());
  return gcnew gfl2::clr::math::Vector(rot.x, rot.y, rot.z);
}

//! @brief 位置設定
void ModelInstanceNode::SetTranslate(gfl2::clr::math::Vector^ translate)
{
  this->m_pNative->SetLocalTranslate(translate->GetNativeVector3());
}

//! @brief 位置取得
gfl2::clr::math::Vector^ ModelInstanceNode::GetTranslate()
{
  return gcnew gfl2::clr::math::Vector(
    this->m_pNative->GetLocalTranslate()
    );
}

//! @brief MaterialInstanceNode総数取得
u32 ModelInstanceNode::GetMaterialNum()
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNative = m_pNative->SafeCast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>();
  return pNative->GetMaterialNum();
}

//! @brief MaterialInstanceNode取得
MaterialInstanceNode^ ModelInstanceNode::GetMaterialInstanceNode(u32 no)
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNative = m_pNative->SafeCast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>();
  MaterialInstanceNode^ node = gcnew MaterialInstanceNode(pNative->GetMaterialInstanceNode(no));
  return node;
}

//! @brief JointInstanceNode取得
JointInstanceNode^ ModelInstanceNode::GetJointInstanceNode(u32 no)
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNative = m_pNative->SafeCast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>();
  JointInstanceNode^ node = gcnew JointInstanceNode(pNative->GetJointInstanceNode(no));
  return node;
}

//! @brief 描画タグ更新
void ModelInstanceNode::UpdateDrawTag(gfl2::clr::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo^ updateDrawTagInfo)
{
  m_pNative->UpdateDrawTag(*updateDrawTagInfo->GetNative());
}

//! @brief 描画ON/OFF切替
void ModelInstanceNode::SetVisible(bool visible)
{
  m_pNative->SetVisible(visible);
}

}}}}}