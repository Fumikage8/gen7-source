#include <Animation/include/gfl2_JointAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>
#include <math/include/gfl2_Quaternion.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
JointAnimationSlot::JointAnimationSlot() :
  AnimationSlotBase(),
	m_pJointDataSetList(NULL),
  m_DivertEnabled(false),
  m_DivertScale(1.0f)
{
}

/**
 * @brief デストラクタ
 */
JointAnimationSlot::~JointAnimationSlot()
{
}

/**
 * @brief 部分アニメーション設定
 *
 * @param pJointName ジョイント名
 */
void JointAnimationSlot::SetPartialAnimation(const c8* pJointName, bool flag)
{
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointInstanceNode = m_pModelNode->GetJointInstanceNode(i);
    if (std::strcmp(pJointInstanceNode->GetName(), pJointName) == 0)
    {
      this->SetPartialAnimation(i, flag);
      return;
    }
  }
}

/**
 * @brief 部分アニメーション設定
 *
 * @param jointIndex ジョイントインデックス
 * @param flag 有効ならtrue, 無効ならfalse
 */
void JointAnimationSlot::SetPartialAnimation(u32 jointIndex, bool flag)
{
  m_pJointDataSetList[jointIndex].m_Enabled = flag;
}

/**
 * @brief 共有アニメーション設定
 *
 * @param pBaseJointName 基準ジョイント名：おそらく通常は"Waist"
 * @param flag 有効ならtrue, 無効ならfalse
 */
void JointAnimationSlot::SetDivertAnimation(const c8* pBaseJointName, bool flag)
{
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointInstanceNode = m_pModelNode->GetJointInstanceNode(i);
    if (std::strcmp(pJointInstanceNode->GetName(), pBaseJointName) == 0)
    {
      this->SetDivertAnimation(i, flag);
      return;
    }
  }
}

/**
 * @brief 共有アニメーション設定
 *
 * @param baseJointIndex 基準ジョイントインデックス
 * @param flag 有効ならtrue, 無効ならfalse
 */
void JointAnimationSlot::SetDivertAnimation(u32 baseJointIndex, bool flag)
{
  m_pJointDataSetList[baseJointIndex].m_IsBaseJoint = flag;
  m_DivertEnabled = flag;

  // @attention 基準の高さをモーションデータから取得したい
	static const f32 s_BaseWaistHeight = 90.477547f;
  m_DivertScale = m_pModelNode->GetJointInstanceNode(baseJointIndex)->GetDefaultTranslate().y / s_BaseWaistHeight;
}

/**
 * @brief ジョイントインデックスからジョイントSRTを取得できるか確認
 *
 * @param jointIndex ジョイントインデックス
 * @return true:取得できる, false:取得できない
 */
bool JointAnimationSlot::CanGetJointSRT(u32 jointIndex) const
{
  // 対応するジョイントのアニメーションがあるか確認
  return (this->GetJointDataSet(jointIndex)->m_pAnimBody != NULL);
}

/**
 * @brief ジョイントトランスフォームを取得
 *
 * @param out_pJointTransform 出力用のバッファ
 * @param jointIndex ジョイントインデックス
 */
void JointAnimationSlot::GetJointSRT(math::SRT* out_pJointSRT, u32 jointIndex)
{
  // 対応するジョイントのアニメーションが無いなら無視
  if (this->GetJointDataSet(jointIndex)->m_pAnimBody == NULL)
  {
    return;
  }

  f32* valueList[] =
  {
    &out_pJointSRT->scale.x,
    &out_pJointSRT->scale.y,
    &out_pJointSRT->scale.z,

    &out_pJointSRT->rotation.x,
    &out_pJointSRT->rotation.y,
    &out_pJointSRT->rotation.z,

    &out_pJointSRT->translate.x,
    &out_pJointSRT->translate.y,
    &out_pJointSRT->translate.z,

    &out_pJointSRT->rotation.w,
  };

  JointDataSet* pJointDataSet = this->GetJointDataSet(jointIndex);
  // static
  for (u32 index = 0; index < pJointDataSet->m_StaticPlayerIndexList.m_PlayerCount; ++index)
  {
    u32 curveIndex = pJointDataSet->m_StaticPlayerIndexList.m_PlayerIndexList[index];

    CurvePlayer* pCurvePlayer = &pJointDataSet->m_CurvePlayerList[curveIndex];
    *valueList[curveIndex] = pCurvePlayer->GetValueStatic();
  }
  // hermite
  for (u32 index = 0; index < pJointDataSet->m_HermiteU16PlayerIndexList.m_PlayerCount; ++index)
  {
    u32 curveIndex = pJointDataSet->m_HermiteU16PlayerIndexList.m_PlayerIndexList[index];

    CurvePlayer* pCurvePlayer = &pJointDataSet->m_CurvePlayerList[curveIndex];
    *valueList[curveIndex] = pCurvePlayer->GetValueHermiteU16(m_CurrentFrame);
  }
  for (u32 index = 0; index < pJointDataSet->m_HermiteF32PlayerIndexList.m_PlayerCount; ++index)
  {
    u32 curveIndex = pJointDataSet->m_HermiteF32PlayerIndexList.m_PlayerIndexList[index];

    CurvePlayer* pCurvePlayer = &pJointDataSet->m_CurvePlayerList[curveIndex];
    *valueList[curveIndex] = pCurvePlayer->GetValueHermiteF32(m_CurrentFrame);
  }

  // 対数クォータニオンから変換
  if (pJointDataSet->m_HasQuatLnCurve)
  {
    out_pJointSRT->rotation.ConvExp();
  }
  // オイラー角から変換
  if (pJointDataSet->m_HasEulerCurve)
  {
    math::Quaternion qx(math::Vector3(1, 0, 0), out_pJointSRT->rotation.x);
    math::Quaternion qy(math::Vector3(0, 1, 0), out_pJointSRT->rotation.y);
    math::Quaternion qz(math::Vector3(0, 0, 1), out_pJointSRT->rotation.z);

    out_pJointSRT->rotation = qz;
    out_pJointSRT->rotation *= qy;
    out_pJointSRT->rotation *= qx;
  }

  // 0スケール対策
  // マイナススケールで調整するものもあるのでジャスト0だけを判定する
  static const f32 s_AdjustScale = 0.00001f;
  if (out_pJointSRT->scale.x == 0.0f)
  {
    out_pJointSRT->scale.x = s_AdjustScale;
  }
  if (out_pJointSRT->scale.y == 0.0f)
  {
    out_pJointSRT->scale.y = s_AdjustScale;
  }
  if (out_pJointSRT->scale.z == 0.0f)
  {
    out_pJointSRT->scale.z = s_AdjustScale;
  }

#if 0
  // 共有モーション
  if (m_DivertEnabled)
  {
    // 基準ボーンならスケールを掛けて適用する
    if (m_pJointDataSetList[jointIndex].m_IsBaseJoint)
    {
      out_pJointSRT->translate.y *= m_DivertScale;
    }
    else
    {
      out_pJointSRT->translate = translation;
    }
  }
#endif
}

/**
 * @brief 歩き用座標を取得：ポケモン用の拡張データ
 *
 * @param out_pPosition 出力用のバッファ
 * @param frame 取得するフレーム数
 */
void JointAnimationSlot::GetWalkPosition(math::Vector3* out_pPosition, f32 frame)
{
  if (m_WalkCurvePositionY.HasData())
  {
    m_WalkCurvePositionY.GetValue(&out_pPosition->y, frame);
  }
  if (m_WalkCurvePositionZ.HasData())
  {
    m_WalkCurvePositionZ.GetValue(&out_pPosition->z, frame);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void JointAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // ジョイントが無いなら終了
  if (m_pModelNode->GetJointNum() == 0)
  {
    return;
  }

  GFL_ASSERT(pHeap != NULL);
  m_pJointDataSetList = GFL_NEW_ARRAY(pHeap) JointDataSet[m_pModelNode->GetJointNum()];

#if 0
  // JointInstanceNodeとJointAnimationResourceNodeを関連付ける。
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    scenegraph::instance::JointInstanceNode* pJointInstanceNode = m_pModelNode->GetJointInstanceNode(i);

    const c8*			pInstanceName = pJointInstanceNode->GetNodeName();
    if ( strcmp( pInstanceName, "Waist" ) == 0 )
    {
      m_WaistMat = pJointInstanceNode->GetLocalMatrix();
      m_WaistScale = m_WaistMat[3].y / s_BaseWaistHeight;
    }
  }
#endif
}

/**
 * @brief 派生クラスでの終了
 */
void JointAnimationSlot::FinalizeDerived()
{
  // ジョイントデータセットリスト開放
  GFL_SAFE_DELETE_ARRAY(m_pJointDataSetList);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void JointAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // ジョイントが無ければ無視
  if (m_pJointDataSetList == NULL)
  {
    return;
  }

  // 全ジョイント初期化
  for (u32 jointNo = 0; jointNo < m_pModelNode->GetJointNum(); ++jointNo)
  {
    JointDataSet* pJointDataSet = this->GetJointDataSet(jointNo);
    pJointDataSet->m_pAnimBody = NULL;
    pJointDataSet->m_StaticPlayerIndexList.m_PlayerCount = 0;
    pJointDataSet->m_HermiteU16PlayerIndexList.m_PlayerCount = 0;
    pJointDataSet->m_HermiteF32PlayerIndexList.m_PlayerCount = 0;
    pJointDataSet->m_HasEulerCurve = 0;
    pJointDataSet->m_HasQuatLnCurve = 0;
  }
  m_WalkCurvePositionY.Reset();
  m_WalkCurvePositionZ.Reset();

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pJointAnimHeader == NULL)
  {
    return;
  }

  // ジョイントアニメーション関連付け
  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pJointAnimHeader) + sizeof(scenegraph::resource::AnimationResourceNode::JointAnimHeader);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pJointAnimHeader->nameBufferSize;
  for (u32 animJointNo = 0; animJointNo < m_pResourceNode->m_pJointAnimHeader->jointCount; ++animJointNo)
  {
    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::JointAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::JointAnimBody*>(bodyAddr);
    bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::JointAnimBody) - sizeof(u32);

    uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

    // 歩き用データ
    if (pName->IsSameName("_"))
    {
      // カーブ読み込み：要らないもの
      for (int curveIndex = 0; curveIndex < JointDataSet::TRANSLATE_Y; ++curveIndex)
      {
        CurvePlayer dummyCurve;
        dummyCurve.Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }
      m_WalkCurvePositionY.Initialize(curveAddr, pBody->flags, JointDataSet::TRANSLATE_Y, m_FrameCount);
      m_WalkCurvePositionZ.Initialize(curveAddr, pBody->flags, JointDataSet::TRANSLATE_Z, m_FrameCount);

      continue;
    }

    // 対応するジョイントを探す
    for (u32 jointNo = 0; jointNo < m_pModelNode->GetJointNum(); ++jointNo)
    {
      scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(jointNo);
      if (!pName->IsSameName(pJointNode->GetNodeName()))
      {
        continue;
      }

      JointDataSet* pJointDataSet = this->GetJointDataSet(jointNo);
      pJointDataSet->m_pAnimBody = pBody;

      if (pBody->flags & 0x80000000)
      {
        // オイラーでカーブを持っている
        pJointDataSet->m_HasEulerCurve = 1;
      }
      else
      {
        // クォータニオンでカーブを持っている
        pJointDataSet->m_HasEulerCurve = 0;
      }

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < JointDataSet::INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pJointDataSet->m_CurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);

        // @attention メモリが厳しいようなら最適化
        if (pCurvePlayer->m_type == CURVE_KEY_TYPE_BASEPOSE)
        {
        }
        else if (pCurvePlayer->m_type == CURVE_KEY_TYPE_HERMITE_U16)
        {
          pJointDataSet->m_HermiteU16PlayerIndexList.m_PlayerIndexList[pJointDataSet->m_HermiteU16PlayerIndexList.m_PlayerCount++] = curveIndex;
        }
        else if (pCurvePlayer->m_type == CURVE_KEY_TYPE_HERMITE_F32)
        {
          pJointDataSet->m_HermiteF32PlayerIndexList.m_PlayerIndexList[pJointDataSet->m_HermiteF32PlayerIndexList.m_PlayerCount++] = curveIndex;
        }
        else
        {
          pJointDataSet->m_StaticPlayerIndexList.m_PlayerIndexList[pJointDataSet->m_StaticPlayerIndexList.m_PlayerCount++] = curveIndex;
        }
      }

      // 回転の復元が必要か？
      pJointDataSet->m_HasQuatLnCurve = 0;
      if (!pJointDataSet->m_HasEulerCurve)
      {
        pJointDataSet->m_HasQuatLnCurve = 
          (pJointDataSet->m_CurvePlayerList[JointDataSet::ROTATE_X].m_type != CURVE_KEY_TYPE_BASEPOSE)
          || (pJointDataSet->m_CurvePlayerList[JointDataSet::ROTATE_Y].m_type != CURVE_KEY_TYPE_BASEPOSE)
          || (pJointDataSet->m_CurvePlayerList[JointDataSet::ROTATE_Z].m_type != CURVE_KEY_TYPE_BASEPOSE);
      }

      // 検索終了
      break;
    }
  }

}

/**
 * @brief 線形補間用に前回のSRTを保存、取得
 */
void JointAnimationSlot::SetOldSRT(u32 jointIndex, const gfl2::math::SRT& oldSRT)
{
  // 仮想関数を使っていないので、構造体サイズさえ満たせば保存領域に使える
  STATIC_ASSERT(sizeof(JointDataSet) >= sizeof(gfl2::math::SRT));

  gfl2::math::SRT* pSRT = reinterpret_cast<gfl2::math::SRT*>(this->GetJointDataSet(jointIndex));
  *pSRT = oldSRT;
}

/**
 * @brief 線形補間用に前回のSRTを取得
 */
void JointAnimationSlot::GetOldSRT(u32 jointIndex, gfl2::math::SRT* out_pOldSRT)
{
  gfl2::math::SRT* pSRT = reinterpret_cast<gfl2::math::SRT*>(this->GetJointDataSet(jointIndex));
  *out_pOldSRT = *pSRT;
}

}}