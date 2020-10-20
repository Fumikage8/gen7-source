#include <Animation/include/gfl2_AnimationController.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <math/include/gfl2_Quaternion.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

AnimationController::AnimationController():
  m_CurrentJointSlotSetIndex(0),
  m_JointInterpStepFrame(1.0f),
  m_IsForceInterp(false),
  m_CalculateEnabled(true),
  m_UpdateFrameEnabled(true),
  m_LerpInterpEnabled(false),
  m_InterpSlotIndex(-1)
{
}

AnimationController::~AnimationController()
{
  this->Finalize();
}

void AnimationController::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode)
{
  m_pHeap = pHeap;
  m_pModelNode = pModelNode;

  // ジョイントスロットセット初期化
  m_JointSlotSetList[0].Initialize();
  m_JointSlotSetList[1].Initialize();

  // 補完無効に
  m_JointInterpFrame = -1.0f;

  // マテリアルスロットセット初期化
  m_MaterialSlotSet.Initialize();

  // ジョイントスロットセット初期化
  m_VisibilitySlotSet.Initialize();

  // ブレンドシェイプスロットセット初期化
  m_BlendShapeSlotSet.Initialize();

  // キースロットセット初期化
  m_KeySlotSet.Initialize();
}

void AnimationController::Finalize()
{
  m_JointSlotSetList[0].Finalize();
  m_JointSlotSetList[1].Finalize();
  m_MaterialSlotSet.Finalize();
  m_VisibilitySlotSet.Finalize();
  m_BlendShapeSlotSet.Finalize();
  m_KeySlotSet.Finalize();
}

gfl2::heap::HeapBase* AnimationController::GetAnimationHeap()
{
  return m_pHeap;
}

JointAnimationSlot* AnimationController::GetJointSlot(u32 slotIndex)
{
  return m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}
const JointAnimationSlot* AnimationController::GetJointSlot(u32 slotIndex) const
{
  return m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetSlot(slotIndex);
}

JointAnimationSlot* AnimationController::GetJointInterpSlot(u32 slotIndex)
{
  return m_JointSlotSetList[m_CurrentJointSlotSetIndex ^ 1].GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}
const JointAnimationSlot* AnimationController::GetJointInterpSlot(u32 slotIndex) const
{
  return m_JointSlotSetList[m_CurrentJointSlotSetIndex ^ 1].GetSlot(slotIndex);
}

void AnimationController::InterpRequest(f32 interpFrame, s8 slotIndex)
{
  bool interpCancel = this->IsInterp();
  if (interpCancel && (m_InterpSlotIndex != slotIndex))
  {
    // 他のスロットの補間はキャンセルできない
    return;
  }

  m_InterpSlotIndex = slotIndex;
  m_JointInterpFrame = interpFrame;
  m_CurrentJointInterpFrame = 0.0f;

  // 補間中はキャンセルして線形補間する
  m_LerpInterpEnabled = interpCancel;
  if (m_LerpInterpEnabled && (interpFrame > 0.0f))
  {
    // コールバック関数が登録されているならこのタイミングでコールバック
    if (m_InterpCancelCallbackParam.callbackFunc != NULL)
    {
      m_InterpCancelCallbackParam.callbackFunc(m_InterpCancelCallbackParam.pUserWork);
    }

    for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
    {
      scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(i);
      m_JointSlotSetList[m_CurrentJointSlotSetIndex].SetOldJointSRT(i, pJointNode->GetLocalSRT());
    }
  }
}

MaterialAnimationSlot* AnimationController::GetMaterialSlot(u32 slotIndex)
{
  return m_MaterialSlotSet.GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}

const MaterialAnimationSlot* AnimationController::GetMaterialSlot(u32 slotIndex) const
{
  return m_MaterialSlotSet.GetSlot(slotIndex);
}

VisibilityAnimationSlot* AnimationController::GetVisibilitySlot(u32 slotIndex)
{
  return m_VisibilitySlotSet.GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}

const VisibilityAnimationSlot* AnimationController::GetVisibilitySlot(u32 slotIndex) const
{
  return m_VisibilitySlotSet.GetSlot(slotIndex);
}

BlendShapeAnimationSlot* AnimationController::GetBlendShapeSlot(u32 slotIndex)
{
  return m_BlendShapeSlotSet.GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}

const BlendShapeAnimationSlot* AnimationController::GetBlendShapeSlot(u32 slotIndex) const
{
  return m_BlendShapeSlotSet.GetSlot(slotIndex);
}

KeyAnimationSlot* AnimationController::GetKeySlot(u32 slotIndex)
{
  return m_KeySlotSet.GetSlotOrCreate(m_pHeap, m_pModelNode, slotIndex);
}

const KeyAnimationSlot* AnimationController::GetKeySlot(u32 slotIndex) const
{
  return m_KeySlotSet.GetSlot(slotIndex);
}

JointAnimationSlot* AnimationController::AllocateJointSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_JointSlotSetList[0].GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

JointAnimationSlot* AnimationController::AllocateJointInterpSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_JointSlotSetList[1].GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

MaterialAnimationSlot* AnimationController::AllocateMaterialSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_MaterialSlotSet.GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

VisibilityAnimationSlot* AnimationController::AllocateVisibilitySlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_VisibilitySlotSet.GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

BlendShapeAnimationSlot* AnimationController::AllocateBlendShapeSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_BlendShapeSlotSet.GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

KeyAnimationSlot* AnimationController::AllocateKeySlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex)
{
  return m_KeySlotSet.GetSlotOrCreate(pAnimationHeap, m_pModelNode, slotIndex);
}

/**
 * @brief スロットのメモリ確保
 *
 * @param pAnimationHeap メモリを確保するヒープ
 * @param slotIndex 確保するスロットの番号
 * @param animationBit 確保するアニメーションのタイプビット
 */
AnimationController::AnimationSlotList AnimationController::AllocateAnimationSlotList(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex, AnimationBit animationBit)
{
  // この関数でメモリ確保したら以降はメモリ確保禁止
  m_pHeap = NULL;

  return AnimationSlotList(
    (animationBit & ANIMATION_BIT_JOINT) ? this->AllocateJointSlot(pAnimationHeap, slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_MATERIAL) ? this->AllocateMaterialSlot(pAnimationHeap, slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_VISIBILITY) ? this->AllocateVisibilitySlot(pAnimationHeap, slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_BLENDSHAPE) ? this->AllocateBlendShapeSlot(pAnimationHeap, slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_KEY) ? this->AllocateKeySlot(pAnimationHeap, slotIndex) : NULL
    );
}

AnimationController::AnimationSlotList AnimationController::AllocateAnimationInterpSlotList(heap::HeapBase* pAnimationHeap, u32 slotIndex, AnimationBit animationBit)
{
  // この関数でメモリ確保したら以降はメモリ確保禁止
  m_pHeap = NULL;

  return AnimationSlotList(
    (animationBit & ANIMATION_BIT_JOINT) ? this->AllocateJointInterpSlot(pAnimationHeap, slotIndex) : NULL,
    NULL,
    NULL,
    NULL,
    NULL
    );
}

/**
 * @brief リソース設定用のスロットリスト取得
 *
 * @param slotIndex 取得するスロットの番号
 * @param animationBit 設定するアニメーションのタイプビット
 */
AnimationController::AnimationSlotList AnimationController::GetResourceAnimationSlotList(u32 slotIndex, AnimationBit animationBit)
{
  return AnimationSlotList(
    (animationBit & ANIMATION_BIT_JOINT) ? this->GetJointSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_MATERIAL) ? this->GetMaterialSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_VISIBILITY) ? this->GetVisibilitySlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_BLENDSHAPE) ? this->GetBlendShapeSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_KEY) ? this->GetKeySlot(slotIndex) : NULL
    );
}
AnimationController::AnimationSlotList AnimationController::GetResourceAnimationInterpSlotList(u32 slotIndex, AnimationBit animationBit)
{
  return AnimationSlotList(
    (animationBit & ANIMATION_BIT_JOINT) ? this->GetJointInterpSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_MATERIAL) ? this->GetMaterialSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_VISIBILITY) ? this->GetVisibilitySlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_BLENDSHAPE) ? this->GetBlendShapeSlot(slotIndex) : NULL,
    (animationBit & ANIMATION_BIT_KEY) ? this->GetKeySlot(slotIndex) : NULL
    );
}

/**
 * @brief アニメーションスロットリスト取得
 *
 * @param slotIndex 取得するスロットの番号
 */
AnimationController::AnimationSlotList AnimationController::GetAnimationSlotList(u32 slotIndex)
{
  return AnimationSlotList(
    (this->m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetSlotCount() > slotIndex) ? this->GetJointSlot(slotIndex) : NULL,
    (this->m_MaterialSlotSet.GetSlotCount() > slotIndex) ? this->GetMaterialSlot(slotIndex) : NULL,
    (this->m_VisibilitySlotSet.GetSlotCount() > slotIndex) ? this->GetVisibilitySlot(slotIndex) : NULL,
    (this->m_BlendShapeSlotSet.GetSlotCount() > slotIndex) ? this->GetBlendShapeSlot(slotIndex) : NULL,
    (this->m_KeySlotSet.GetSlotCount() > slotIndex) ? this->GetKeySlot(slotIndex) : NULL
    );
}
const AnimationController::AnimationSlotList AnimationController::GetAnimationSlotList(u32 slotIndex) const
{
  return AnimationSlotList(
    this->GetJointSlot(slotIndex),
    this->GetMaterialSlot(slotIndex),
    this->GetVisibilitySlot(slotIndex),
    this->GetBlendShapeSlot(slotIndex),
    this->GetKeySlot(slotIndex)
    );
}

AnimationController::AnimationSlotList AnimationController::GetAnimationInterpSlotList(u32 slotIndex)
{
  return AnimationSlotList(
    (this->m_JointSlotSetList[m_CurrentJointSlotSetIndex ^ 1].GetSlotCount() > slotIndex) ? this->GetJointInterpSlot(slotIndex) : NULL,
    (this->m_MaterialSlotSet.GetSlotCount() > slotIndex) ? this->GetMaterialSlot(slotIndex) : NULL,
    (this->m_VisibilitySlotSet.GetSlotCount() > slotIndex) ? this->GetVisibilitySlot(slotIndex) : NULL,
    (this->m_BlendShapeSlotSet.GetSlotCount() > slotIndex) ? this->GetBlendShapeSlot(slotIndex) : NULL,
    (this->m_KeySlotSet.GetSlotCount() > slotIndex) ? this->GetKeySlot(slotIndex) : NULL
    );
}
const AnimationController::AnimationSlotList AnimationController::GetAnimationInterpSlotList(u32 slotIndex) const
{
  return AnimationSlotList(
    this->GetJointInterpSlot(slotIndex),
    this->GetMaterialSlot(slotIndex),
    this->GetVisibilitySlot(slotIndex),
    this->GetBlendShapeSlot(slotIndex),
    this->GetKeySlot(slotIndex)
    );
}

f32 AnimationController::Update(f32 interpWeight, AnimationBit animationBit)
{
  if (animationBit & ANIMATION_BIT_KEY)
  {
    // キーアニメーション更新
    this->UpdateKeyAnimation();
  }

  if (animationBit & ANIMATION_BIT_JOINT)
  {
    // ジョイントアニメーション更新
    interpWeight = this->UpdateJointAnimation(interpWeight);
  }

  if (animationBit & ANIMATION_BIT_MATERIAL)
  {
    // マテリアルアニメーション更新
    this->UpdateMaterialAnimation();
  }

  if (animationBit & ANIMATION_BIT_VISIBILITY)
  {
    // ビジビリティアニメーション更新
    this->UpdateVisibilityAnimation();
  }

  if (animationBit & ANIMATION_BIT_BLENDSHAPE)
  {
    // ブレンドシェイプアニメーション更新
    this->UpdateBlendShapeAnimation();
  }

  return interpWeight;
}

f32 AnimationController::UpdateJointAnimation(f32 interpWeight)
{
  if (m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetSlotCount() == 0)
  {
    return interpWeight;
  }

  if (interpWeight < 0.0f)
  {
    // 自動更新
    interpWeight = this->UpdateJointAnimationAuto();
  }
  else
  {
    // 補間率指定更新
    interpWeight = this->UpdateJointAnimationManual(interpWeight);
  }

  return interpWeight;
}

f32 AnimationController::UpdateJointAnimationAuto()
{
  // 自動で補間フレームを進める
  f32 interpWeight = 0.0f;
  bool isInterp = this->UpdateInterpFrameAuto(&interpWeight);

  // アニメーション計算
  if (this->IsCalculateEnable())
  {
    if (isInterp)
    {
      // 補完あり
      this->UpdateJointAnimationInterp(interpWeight);
    }
    else
    {
      // 補完無し
      this->UpdateJointAnimationNoInterp();
    }
  }

  // フレーム更新
  if (this->IsUpdateFrameEnable())
  {
    m_JointSlotSetList[m_CurrentJointSlotSetIndex].UpdateFrame();

    if (isInterp)
    {
      u32 interpSlotSetIndex = m_CurrentJointSlotSetIndex ^ 1;
      m_JointSlotSetList[interpSlotSetIndex].UpdateFrame();
    }
  }

  return interpWeight;
}

f32 AnimationController::UpdateJointAnimationManual(f32 interpWeight)
{
  // ジョイントアニメーション更新
  this->UpdateJointAnimationInterp(interpWeight);

  // フレーム更新
  this->UpdateInterpFrameManual();

  return interpWeight;
}

bool AnimationController::UpdateInterpFrameAuto(f32* out_pInterpWeight)
{
  // 補間中か？
  if (m_JointInterpFrame < 0.0f)
  {
    return false;
  }

  // 補間用のスロットのフレーム更新
  m_CurrentJointInterpFrame += m_JointInterpStepFrame;

  *out_pInterpWeight = m_CurrentJointInterpFrame / m_JointInterpFrame;
  bool isInterpEnd = (*out_pInterpWeight >= 1.0f);

  // 順再生終了チェック
  if (*out_pInterpWeight >= 1.0f)
  {
    isInterpEnd = true;
    *out_pInterpWeight = 1.0f;
    m_CurrentJointSlotSetIndex ^= 1;
  }
  // 逆再生終了チェック
  else if (*out_pInterpWeight <= 0.0f)
  {
    isInterpEnd = true;
    *out_pInterpWeight = 0.0f;
    // 逆再生はスロットを入れ替えない
    //m_CurrentJointSlotSetIndex ^= 1;
  }

  // 再生終了
  if (isInterpEnd)
  {
    m_JointInterpFrame = -1.0f;
    m_LerpInterpEnabled = false;
    return false;
  }

  return true;
}

void AnimationController::UpdateInterpFrameManual()
{
  // 両方のフレームを進める
  m_JointSlotSetList[m_CurrentJointSlotSetIndex].UpdateFrame();
  m_JointSlotSetList[m_CurrentJointSlotSetIndex ^ 1].UpdateFrame();
}

void AnimationController::UpdateJointAnimationNoInterp()
{
  // ジョイントの行列を取得してインスタンスに反映
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(i);

    math::SRT jointSRT = pJointNode->GetDefaultSRT();
    m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetJointSRT(&jointSRT, i);

    pJointNode->SetLocalSRT(jointSRT);
  }
}

void AnimationController::UpdateJointAnimationInterp(f32 interpWeight)
{
  u32 interpSlotSetIndex = m_CurrentJointSlotSetIndex ^ 1;

  // 前回の結果との線形補間
  if (m_LerpInterpEnabled)
  {
    // ジョイントの行列を取得してインスタンスに反映
    for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
    {
      scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(i);

      math::SRT jointSRT0;
      math::SRT jointSRT1 = pJointNode->GetDefaultSRT();

      m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetOldJointSRT(i, &jointSRT0);
      m_JointSlotSetList[interpSlotSetIndex].GetJointSRT(&jointSRT1, i);

      jointSRT0.Slerp(jointSRT1, interpWeight);
      pJointNode->SetLocalSRT(jointSRT0);
    }
  }
  // 通常の補間
  else
  {
    // ジョイントの行列を取得してインスタンスに反映
    for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
    {
      scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(i);

      math::SRT jointSRT0 = pJointNode->GetDefaultSRT();
      math::SRT jointSRT1 = jointSRT0;

      m_JointSlotSetList[m_CurrentJointSlotSetIndex].GetJointSRT(&jointSRT0, i);
      m_JointSlotSetList[interpSlotSetIndex].GetJointSRT(&jointSRT1, i);

      jointSRT0.Slerp(jointSRT1, interpWeight);
      pJointNode->SetLocalSRT(jointSRT0);
    }
  }
}

void AnimationController::UpdateMaterialAnimation()
{
  // アニメーション計算
  if (this->IsCalculateEnable())
  {
    for (u32 i = 0; i < m_pModelNode->GetMaterialNum(); ++i)
    {
      m_MaterialSlotSet.ApplyMaterial(i);
    }
  }
  // フレーム更新
  if (this->IsUpdateFrameEnable())
  {
    m_MaterialSlotSet.UpdateFrame();
  }
}

void AnimationController::UpdateVisibilityAnimation()
{
  // アニメーション計算
  if (this->IsCalculateEnable())
  {
    for (u32 i = 0; i < m_pModelNode->GetMehsNum(); ++i)
    {
      m_VisibilitySlotSet.ApplyVisibility(i);
    }
  }
  // フレーム更新
  if (this->IsUpdateFrameEnable())
  {
    m_VisibilitySlotSet.UpdateFrame();
  }
}

void AnimationController::UpdateBlendShapeAnimation()
{
  // アニメーション計算
  if (this->IsCalculateEnable())
  {
    for (u32 i = 0; i < m_pModelNode->GetMehsNum(); ++i)
    {
      m_BlendShapeSlotSet.ApplyBlendShape(i);
    }
  }
  // フレーム更新
  if (this->IsUpdateFrameEnable())
  {
    m_BlendShapeSlotSet.UpdateFrame();
  }
}

void AnimationController::UpdateKeyAnimation()
{
  // アニメーション計算
  if (this->IsCalculateEnable())
  {
    m_KeySlotSet.ApplyKey();
  }
  // フレーム更新
  if (this->IsUpdateFrameEnable())
  {
    m_KeySlotSet.UpdateFrame();
  }
}

static math::Vector3 CalcWalkSpeed(const AnimationSlotSet<JointAnimationSlot>* pSlotSet, f32 frame, f32 stepFrame)
{
  f32 adjustFrame = pSlotSet->GetWalkAdjustFrame(frame, stepFrame);
  math::Vector3 pos_prev = pSlotSet->GetWalkPosition(adjustFrame - stepFrame);
  math::Vector3 pos_next = pSlotSet->GetWalkPosition(adjustFrame);

  f32 posY = pos_next.y;
  if (adjustFrame != frame)
  {
    posY = pSlotSet->GetWalkPosition(frame).y;
  }

  return math::Vector3(0.0f, posY, pos_next.z - pos_prev.z);
}

// ビューアー用に旧関数も残しておく
static math::Vector3 CalcWalkSpeed_Old(const AnimationSlotSet<JointAnimationSlot>* pSlotSet, f32 frame, f32 stepFrame)
{
  frame = pSlotSet->GetWalkAdjustFrame(frame, stepFrame);
  math::Vector3 pos_prev = pSlotSet->GetWalkPosition(frame - stepFrame);
  math::Vector3 pos_next = pSlotSet->GetWalkPosition(frame);

  return math::Vector3(0.0f, pos_next.y, pos_next.z - pos_prev.z);
}

math::Vector3 AnimationController::GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight) const
{
  const AnimationSlotSet<JointAnimationSlot>* pPrevSlotSet = &m_JointSlotSetList[m_CurrentJointSlotSetIndex];
  math::Vector3 v0 = CalcWalkSpeed_Old(pPrevSlotSet, prevFrame, stepFrame);
  if (interpWeight <= 0)
  {
    return v0;
  }

  const AnimationSlotSet<JointAnimationSlot>* pNextSlotSet = &m_JointSlotSetList[m_CurrentJointSlotSetIndex ^ 1];
  math::Vector3 v1 = CalcWalkSpeed_Old(pNextSlotSet, nextFrame, stepFrame);

  v0.Lerp(v1, interpWeight);
  return v0;
}

math::Vector3 AnimationController::GetWalkSpeedSmooth(f32 interpWeight) const
{
  // 補間なし
  if (interpWeight <= 0)
  {
    return this->GetWalkSpeed();
  }

  u32 prevSlotSetIndex = (this->IsInterp())
    ? m_CurrentJointSlotSetIndex
    : m_CurrentJointSlotSetIndex ^ 1;
  u32 nextSlotSetIndex = prevSlotSetIndex ^ 1;

  const AnimationSlotSet<JointAnimationSlot>* pPrevSlotSet = &m_JointSlotSetList[prevSlotSetIndex];
  f32 prevFrame = 0.0f;
  f32 prevStepFrame = 0.0f;

  const AnimationSlotSet<JointAnimationSlot>* pNextSlotSet = &m_JointSlotSetList[nextSlotSetIndex];
  f32 nextFrame = 0.0f;
  f32 nextStepFrame = 0.0f;

  pPrevSlotSet->GetWalkFrameAndStepFrame(&prevFrame, &prevStepFrame);
  pNextSlotSet->GetWalkFrameAndStepFrame(&nextFrame, &nextStepFrame);

  math::Vector3 v0 = CalcWalkSpeed(pPrevSlotSet, prevFrame, nextStepFrame);
  math::Vector3 v1 = CalcWalkSpeed(pNextSlotSet, nextFrame, nextStepFrame);

  v0.Lerp(v1, interpWeight);
  return v0;
}

math::Vector3 AnimationController::GetWalkSpeed() const
{
  u32 jointSlotSetIndex = (this->IsInterp())
    ? m_CurrentJointSlotSetIndex ^ 1
    : m_CurrentJointSlotSetIndex;

  const AnimationSlotSet<JointAnimationSlot>* pCurrentSlotSet = &m_JointSlotSetList[jointSlotSetIndex];
  f32 frame = 0.0f;
  f32 stepFrame = 0.0f;

  pCurrentSlotSet->GetWalkFrameAndStepFrame(&frame, &stepFrame);
  return CalcWalkSpeed(pCurrentSlotSet, frame, stepFrame);
}

/**
 * @brief 補間キャンセル時のコールバックを登録
 *
 * @param callbackFunc コールバックさせる関数
 * @param pUserWork コールバック時の引数、ユーザー任意のワークポインタ
 */
void AnimationController::SetInterpCancelCallback(AnimationCallbackFunc callbackFunc, void* pUserWork)
{
  m_InterpCancelCallbackParam.callbackFunc = callbackFunc;
  m_InterpCancelCallbackParam.pUserWork = pUserWork;
}

}}