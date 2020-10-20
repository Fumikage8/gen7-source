#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_MODELINSTANCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_INSTANCE_MODELINSTANCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_MaterialInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_DrawableNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_JointInstanceNode_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class ModelInstanceNode : public DrawableNode
{
public:
	// 純粋仮想関数を持っているのでnew出来ない. 従って外から渡す
	ModelInstanceNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNative );
	virtual ~ModelInstanceNode(){ this->!ModelInstanceNode(); }
	!ModelInstanceNode();

	//void SetLocalMatrix(gfl2::clr::math::Matrix^ mat);

  //! @brief スケール設定
  void SetScale(gfl2::clr::math::Vector^ scale);

  //! @brief スケール取得
  gfl2::clr::math::Vector^ GetScale();
  
  //! @brief 回転設定
  void SetRotateZYX(gfl2::clr::math::Vector^ rotate);

  //! @brief 回転取得
  gfl2::clr::math::Vector^ GetRotateZYX();

  //! @brief 位置設定
  void SetTranslate(gfl2::clr::math::Vector^ translate);

  //! @brief 位置取得
  gfl2::clr::math::Vector^ GetTranslate();

  //! @brief MaterialInstanceNode総数取得
  u32 GetMaterialNum();

  //! @brief MaterialInstanceNode取得
  MaterialInstanceNode^ GetMaterialInstanceNode(u32 no);

  //! @brief JointInstanceNode取得
  JointInstanceNode^ GetJointInstanceNode(u32 no);

  //! @brief 描画タグ更新
  void UpdateDrawTag(gfl2::clr::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo^ updateDrawTagInfo);

  //! @brief 描画ON/OFF切替
  void SetVisible(bool visible);

protected:
//private:
};

}}}}}

#endif

