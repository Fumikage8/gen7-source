#ifndef GFLIB2_ANIMATION_CLR_SCENEGRAPH_RESOURCE_JOINTANIMATIONRESOURCENODE_H_INCLUDED
#define GFLIB2_ANIMATION_CLR_SCENEGRAPH_RESOURCE_JOINTANIMATIONRESOURCENODE_H_INCLUDED

#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/math/gfl2_Quaternion_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

/**
 * @brief ジョイントアニメーションリソース管理ノード
 */
public ref class JointAnimationResourceNode
{
	/**
   * @brief デストラクタ
   */
	~JointAnimationResourceNode()
	{
		this->!JointAnimationResourceNode();
	}

	/**
   * @brief ファイナライザ
   */
	!JointAnimationResourceNode();

	/**
   * @brief フレーム数取得
   *
   * @return フレーム数を返す
   */
	u32 GetFrameSize();

  /**
   * @brief Originノードの平行移動成分を移動値に変換：連れ歩き用
   */
  void ConvertOriginTranslation();

private:
	//gfl2::renderingengine::scenegraph::resource::JointAnimationResourceNode* m_pNative;

};

}}}}}

#endif