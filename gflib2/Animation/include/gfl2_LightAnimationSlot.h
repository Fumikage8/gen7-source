#ifndef GFLIB2_LIGHT_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_LIGHT_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <Animation/include/gfl2_CurvePlayer.h>

namespace gfl2 { namespace animation {

/**
 * @brief ライトデータセット
 */
struct LightDataSet
{
  enum LightCurveIndex
  {
    POSITION_X,
    POSITION_Y,
    POSITION_Z,

    DIRECTION_X,
    DIRECTION_Y,
    DIRECTION_Z,

    COLOR_R,
    COLOR_G,
    COLOR_B,

    CONE_ANGLE,
    INTENSITY,

    INDEX_COUNT
  };

  LightDataSet() : 
    m_pAnimBody(NULL)
  {
  }

  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::EnvLightAnimBody* m_pAnimBody;
  gfl2::animation::CurvePlayer m_CurvePlayerList[INDEX_COUNT];
};

/**
 * @brief ライトアニメーションスロット
 */
class LightAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  LightAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~LightAnimationSlot();

  /**
   * @brief ライトを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param lightSetIndex ライトセットインデックス
   * @param lightIndex ライトインデックス
   */
  void ApplyLight(f32* out_pValueList, u32 lightSetIndex, u32 lightIndex);

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
    if (m_pResourceNode->m_pEnvLightAnimHeader == NULL)
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

	LightDataSet* m_pLightDataSetList[gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax];        ///< ライトデータセット
};

}}

#endif
