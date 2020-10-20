#ifndef GFLIB2_CAMERA_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_CAMERA_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief カメラデータセット
 */
struct CameraDataSet
{
  enum CameraCurveIndex
  {
    CAM_NEAR,
    CAM_FAR,
    CAM_FOVY,
    CAM_ASPECT_RATIO,

    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,

    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,

    INDEX_COUNT
  };

  CameraDataSet() : 
    m_pAnimBody(NULL)
  {
  }

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::EnvCameraAnimBody* m_pAnimBody;
  gfl2::animation::CurvePlayer m_CurvePlayerList[INDEX_COUNT];
};

/**
 * @brief カメラアニメーションスロット
 */
class CameraAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  CameraAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~CameraAnimationSlot();

  /**
   * @brief カメラを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param cameraIndex カメラインデックス
   */
  void ApplyCamera(f32* out_pValueList, u32 cameraIndex);

  /**
   * @brief アニメーションが含まれているか？
   *
   * @return アニメーションが含まれているならtrue, 含まれていないならfalse
   */
  virtual bool HasAnimation() const
  {
    if (m_pResourceNode == NULL)
    {
      return false;
    }
    if (m_pResourceNode->m_pEnvCameraAnimHeader == NULL)
    {
      return false;
    }

    return true;
  }

private:

  /**
   * @brief 派生クラスでの初期化
   *
   * @param pHeap ヒープ
   */
  virtual void InitializeDerived(gfl2::heap::HeapBase* pHeap);

  /**
   * @brief 派生クラスでの終了
   */
  virtual void FinalizeDerived();

  /**
   * @brief 派生クラスでのアニメーションリソース設定
   *
   * @param pHeap ヒープ
   */
  virtual void SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap);

private:

	CameraDataSet* m_pCameraDataSetList;        ///< カメラデータセット
};

}}

#endif