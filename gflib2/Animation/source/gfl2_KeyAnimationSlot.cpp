#include <Animation/include/gfl2_KeyAnimationSlot.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
KeyAnimationSlot::KeyAnimationSlot() :
  AnimationSlotBase(),
  m_pUserData(NULL),
  m_pCallback(NULL),
  m_CallbackOldFrame(-1)
{
}

/**
 * @brief デストラクタ
 */
KeyAnimationSlot::~KeyAnimationSlot()
{
}

/**
 * @brief キーを適用
 */
void KeyAnimationSlot::ApplyKey()
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pKeyAnimHeader == NULL)
  {
    return;
  }

  // コールバックが設定されていなければ無視
  if (m_pCallback == NULL)
  {
    return;
  }

  // 前回とフレームが変わっていなければ無視
  s32 frame = static_cast<s32>(m_CurrentFrame);
  if (frame == m_CallbackOldFrame)
  {
    return;
  }

  // 最小値最大値
  s32 minRange = m_CallbackOldFrame;
  s32 maxRange = frame;
  if (frame < m_CallbackOldFrame)
  {// 逆再生の場合は-1.0fして対応(1フレームずれるため対処)
    minRange = frame - 1.0f;
    maxRange = m_CallbackOldFrame - 1.0f;
  }

  // ３フレーム以上のスキップは無視する
  static const s32 s_SkipFrameLength = 3;
  s32 length = maxRange - minRange;
  if (length >= s_SkipFrameLength)
  {
    // 順再生
    if (frame < m_CallbackOldFrame)
    {
      minRange = frame - s_SkipFrameLength;
      maxRange = frame;
    }
    // 逆再生
    else 
    {
      minRange = frame;
      maxRange = frame + s_SkipFrameLength;

      if (length >= s_SkipFrameLength + 1)
      {// 4フレーム以上の差があったら, 一周したとみなしてframeを0にする
        minRange = 0.0f;
        maxRange = 0.0f;
      }
    }
  }

  // リソースを巡回する
  uptr dataAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pKeyAnimHeader);
  dataAddr += sizeof(scenegraph::resource::AnimationResourceNode::KeyAnimHeader);
  for (u32 attributeNo = 0; attributeNo < m_pResourceNode->m_pKeyAnimHeader->attributeCount; ++attributeNo)
  {
    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(dataAddr);
    dataAddr += pName->length + sizeof(char);

    // キーフレーム数
    const scenegraph::resource::AnimationResourceNode::KeyAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::KeyAnimBody*>(dataAddr);
    dataAddr += (sizeof(scenegraph::resource::AnimationResourceNode::KeyAnimBody) - sizeof(scenegraph::resource::AnimationResourceNode::KeyAnimBody::Key)) + (pBody->keyCount * sizeof(scenegraph::resource::AnimationResourceNode::KeyAnimBody::Key));

    for (u32 keyNo = 0; keyNo < pBody->keyCount; ++keyNo)
    {
      const scenegraph::resource::AnimationResourceNode::KeyAnimBody::Key* pKey = &pBody->keyList[keyNo];
      if ((minRange < pKey->frame) && (pKey->frame <= maxRange))
      {
        m_pCallback(pName->name, pKey->value, m_pUserData);
      }
    }
  }

  m_CallbackOldFrame = frame;
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void KeyAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
}

/**
 * @brief 派生クラスでの終了
 */
void KeyAnimationSlot::FinalizeDerived()
{
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void KeyAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  m_CallbackOldFrame = -1;

  // 最初のコールバック
  this->ApplyKey();
}

}}