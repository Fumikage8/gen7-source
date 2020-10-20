#ifndef GFLIB2_KEY_ANIMATION_SLOT_H_INCLUDED
#define GFLIB2_KEY_ANIMATION_SLOT_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_AnimationSlotBase.h>

namespace gfl2 { namespace animation {

/**
 * @brief キーアニメーションコールバック
 *
 * @param pKeyName キー名："AK_EffectStart01"など
 * @param value 値
 * @param pUserData ユーザーデータ
 */
typedef void (KeyAnimationCallback)(const c8* pKeyName, f32 value, void* pUserData);

/**
 * @brief キーアニメーションスロット
 */
class KeyAnimationSlot : public AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  KeyAnimationSlot();

  /**
   * @brief デストラクタ
   */
  virtual ~KeyAnimationSlot();

  /**
   * @brief キーを適用
   */
  void ApplyKey();

  void SetCallback(void* pUserData, KeyAnimationCallback* pCallback)
  {
    m_pUserData = pUserData;
    m_pCallback = pCallback;

    // 最初のコールバック
    this->ApplyKey();
  }

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
    if (m_pResourceNode->m_pKeyAnimHeader == NULL)
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

  void* m_pUserData;
  KeyAnimationCallback* m_pCallback;

  s32 m_CallbackOldFrame;
};

}}

#endif