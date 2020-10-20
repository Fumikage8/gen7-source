#include <model/include/gfl2_CharaModel.h>
#include <math/include/gfl2_math_random.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
CharaModel::CharaModel()
{
  // フィールド初期化
  this->InitializeField_CharaModel();
}

/**
 * @brief デストラクタ
 */
CharaModel::~CharaModel()
{
}

/**
 * @brief CharaModelのフィールド初期化
 */
void CharaModel::InitializeField_CharaModel()
{
  m_pEyeMotionResourceNode = NULL;
  m_EyeIndex = EYE_INDEX_NONE;
  m_EyeIndexOld = EYE_INDEX_NONE;
  m_AutoBlinkMode = AUTO_BLINK_MODE_NONE;
  m_AutoBlinkSequence = 0;
  m_AutoBlinkCounter = 0;
  m_AutoBlinkProhibitedFrame = 0;
  m_isBlinkImmediaterly = false;
  m_pMouthMotionResourceNode = NULL;
  m_MouthIndex = MOUTH_INDEX_NONE;
  m_MouthIndexOld = MOUTH_INDEX_NONE;
  m_pNeckInterest = NULL;
  m_NeckJointIndex = -1;
  m_NeckBeginAngleXRad = 0.0f;
  m_NeckBeginAngleYRad = 0.0f;
  m_NeckTargetAngleXRad = 0.0f;
  m_NeckTargetAngleYRad = 0.0f;
  m_NeckTargetFrame = 0;
  m_NeckCurrentFrame = 0;
  GetPublicRand = NULL;
}

/**
 * @brief 作成時に呼ばれるコールバック
 */
void CharaModel::OnCreate()
{
  // フィールド初期化
  this->InitializeField_CharaModel();

  // Neckジョイントのインデックス取得
  // (Headジョイントの方が自然に見えたのでHeadに変更
  m_NeckJointIndex = this->GetJointIndex("Head");

  // Neckジョイントが見つからない
  if (m_NeckJointIndex < 0)
  {
    return;
  }

  // モーションコンフィグ取得
  const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfingNode = 
   static_cast<const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(this->GetModelInstanceNode()->GetMotionConfigInstanceNode());

  // 取得できなかった
  if (pMotionConfingNode == NULL)
  {
    return;
  }

  // Neckを探す
  // (Headジョイントの方が自然に見えたのでHeadに変更
  u32 interestCount = pMotionConfingNode->GetInterestCount();
  for (u32 i = 0; i < interestCount; ++i)
  {
    const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode::InterestNode* pInterest = pMotionConfingNode->GetInterest(i);
    if (::strcmp(pInterest->Name, "Head") != 0)
    {
      continue;
    }

    // XY限定
    if (pInterest->Type != gfl2::renderingengine::InterestType::XY)
    {
      continue;
    }

    // 見つかった
    m_pNeckInterest = pInterest;
    break;
  }
}

/**
 * @brief 目パチモーションリソースノードを設定
 *
 * @param pEyeMotionResourceNode 目パチモーションリソースノード
 */
void CharaModel::SetEyeMotionResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pEyeMotionResourceNode)
{
  m_pEyeMotionResourceNode = pEyeMotionResourceNode;
}

/**
 * @brief 目のインデックス設定
 *
 * @param eyeIndex 目インデックス
 */
void CharaModel::SetEyeIndex(EyeIndex eyeIndex)
{
  m_EyeIndex = eyeIndex;
}

/**
 * @brief 目のインデックス取得
 *
 * @return 目インデックスを返す
 */
CharaModel::EyeIndex CharaModel::GetEyeIndex() const
{
  return m_EyeIndex;
}

/**
 * @brief 自動目パチモード設定
 *
 * @param autoBlinkMode 自動目パチモード
 */
void CharaModel::SetAutoBlinkMode(AutoBlinkMode autoBlinkMode)
{
  m_AutoBlinkMode = autoBlinkMode;
  m_AutoBlinkSequence = 0;
  m_AutoBlinkCounter = 0;

  // インデックスを戻しておく
  m_EyeIndex = EYE_INDEX_NONE;
}

/**
 * @brief 自動目パチモード取得
 *
 * @return 自動目パチモードを返す
 */
CharaModel::AutoBlinkMode CharaModel::GetAutoBlinkMode() const
{
  return m_AutoBlinkMode;
}

/**
 * @brief 目の更新
 */
void CharaModel::UpdateEye()
{
  // 自動目パチ
  this->UpdateAutoBlink();

  // 前回と同じなら何もしない
  if (m_EyeIndex == m_EyeIndexOld)
  {
    return;
  }

  // 前回のインデックス保存
  EyeIndex oldIndex = m_EyeIndexOld;
  m_EyeIndexOld = m_EyeIndex;

  // 指定無し、再生中のアニメーションに連動
  if (m_EyeIndex == EYE_INDEX_NONE)
  {
    this->ChangeAnimationByResourceNode_Face(NULL, EYE_ANIMATION_SLOT);
    return;
  }

  // 前回アニメーションが設定されていないなら設定する
  if (oldIndex == EYE_INDEX_NONE)
  {
    this->ChangeAnimationByResourceNode_Face(m_pEyeMotionResourceNode, EYE_ANIMATION_SLOT);
    this->SetAnimationStepFrame_Face(0.0f, EYE_ANIMATION_SLOT);
  }

  // 該当するフレームに設定
  s32 frame = (static_cast<s32>(m_EyeIndex) - 1) * EYE_ANIMATION_FRAME_INTERVAL;
  this->SetAnimationFrame_Face(frame, EYE_ANIMATION_SLOT);
}

/**
 * @brief 目のアニメーションメモリ確保
 */
void CharaModel::AllocateEye()
{
  this->ChangeAnimationByResourceNode_Face(m_pEyeMotionResourceNode, EYE_ANIMATION_SLOT);
  this->ChangeAnimationByResourceNode_Face(NULL, EYE_ANIMATION_SLOT);
}

/**
 * @brief 口パクモーションリソースノードを設定
 *
 * @param pMouthMotionResourceNode 口パクモーションリソースノード
 */
void CharaModel::SetMouthMotionResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pMouthMotionResourceNode)
{
  m_pMouthMotionResourceNode = pMouthMotionResourceNode;
}

/**
 * @brief 口のインデックス設定
 *
 * @param mouthIndex 口インデックス
 */
void CharaModel::SetMouthIndex(MouthIndex mouthIndex)
{
  m_MouthIndex = mouthIndex;
}

/**
 * @brief 口のインデックス取得
 *
 * @return 口インデックスを返す
 */
CharaModel::MouthIndex CharaModel::GetMouthIndex() const
{
  return m_MouthIndex;
}

/**
 * @brief 口の更新
 */
void CharaModel::UpdateMouth()
{
  // 前回と同じなら何もしない
  if (m_MouthIndex == m_MouthIndexOld)
  {
    return;
  }

  // 前回のインデックス保存
  MouthIndex oldIndex = m_MouthIndexOld;
  m_MouthIndexOld = m_MouthIndex;

  // 指定無し、再生中のアニメーションに連動
  if (m_MouthIndex == MOUTH_INDEX_NONE)
  {
    this->ChangeAnimationByResourceNode_Face(NULL, MOUTH_ANIMATION_SLOT);
    return;
  }

  // 前回アニメーションが設定されていないなら設定する
  if (oldIndex == MOUTH_INDEX_NONE)
  {
    this->ChangeAnimationByResourceNode_Face(m_pMouthMotionResourceNode, MOUTH_ANIMATION_SLOT);
    this->SetAnimationStepFrame_Face(0.0f, MOUTH_ANIMATION_SLOT);
  }

  // 該当するフレームに設定
  s32 frame = (static_cast<s32>(m_MouthIndex) - 1) * MOUTH_ANIMATION_FRAME_INTERVAL;
  this->SetAnimationFrame_Face(frame, MOUTH_ANIMATION_SLOT);
}

/**
 * @brief 口のアニメーションメモリ確保
 */
void CharaModel::AllocateMouth()
{
  this->ChangeAnimationByResourceNode_Face(m_pMouthMotionResourceNode, MOUTH_ANIMATION_SLOT);
  this->ChangeAnimationByResourceNode_Face(NULL, MOUTH_ANIMATION_SLOT);
}

/**
 * @brief 首制御更新： ※シーンのトラバース後に呼んでください
 */
void CharaModel::UpdateNeck()
{
  // Neckジョイントが無ければ何もしない
  if (m_NeckJointIndex < 0)
  {
    return;
  }

  // 目標フレームまでフレームを進める
  ++m_NeckCurrentFrame;
  if (m_NeckCurrentFrame >= m_NeckTargetFrame)
  {
    m_NeckCurrentFrame = m_NeckTargetFrame;
  }

  // 回転量取得
  f32 angleX = this->GetNeckCurrentAngleXRad();
  f32 angleY = this->GetNeckCurrentAngleYRad();

  // 回す必要がなければここで終わり
  if ((angleX == 0.0f) && (angleY == 0.0f))
  {
    return;
  }

  // Neckノード取得
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pNeckNode = this->GetModelInstanceNode()->GetJointInstanceNode(m_NeckJointIndex);

  // X軸回転
  {
    gfl2::math::Quaternion q(gfl2::math::Vector3(1, 0, 0), angleX);
    pNeckNode->SetLocalRotation(pNeckNode->GetLocalRotation() * q);
  }
  // Y軸回転
  {
    gfl2::math::Quaternion q(gfl2::math::Vector3(0, 1, 0), angleY);
    pNeckNode->SetLocalRotation(pNeckNode->GetLocalRotation() * q);
  }

  // Neckノード以下を再トラバース
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pNeckNode);

  // 着せ替え用の首更新処理
  this->OnUpdateNeck();
}

/**
 * @brief 首の角度設定
 * 
 * @param angleXRad X方向の角度（ラジアン） +方向で左を向く -方向で右を向く
 * @param angleYRad Y方向の角度（ラジアン） +方向で上を向く -方向で下を向く
 * @param frame 何フレームかけて回転させるか
 */
void CharaModel::SetNeckAngle(f32 angleXRad, f32 angleYRad, u32 frame)
{
  // 角度制限が取得できない可能性もあるので初期値は適当に巨大な値
  f32 angleXRadMin = -10000.0f;
  f32 angleXRadMax = 10000.0f;
  f32 angleYRadMin = -10000.0f;
  f32 angleYRadMax = 10000.0f;

  // モーションコンフィグから角度限界取得
  if (m_pNeckInterest != NULL)
  {
    // X角度制限
    angleXRadMin = m_pNeckInterest->Low;
    angleXRadMax = m_pNeckInterest->High;

    // Y角度制限
    angleYRadMin = m_pNeckInterest->Low2;
    angleYRadMax = m_pNeckInterest->High2;
  }
  
  // 現在の角度を保存
  m_NeckBeginAngleXRad = this->GetNeckCurrentAngleXRad();
  m_NeckBeginAngleYRad = this->GetNeckCurrentAngleYRad();

  // パラメータ設定
  m_NeckTargetAngleXRad = gfl2::math::Clamp(angleXRad, angleXRadMin, angleXRadMax);
  m_NeckTargetAngleYRad = gfl2::math::Clamp(angleYRad, angleYRadMin, angleYRadMax);
  m_NeckCurrentFrame = 0;
  m_NeckTargetFrame = frame;
}

/**
 * @brief 首の角度リセット
 * 
 * @param frame 何フレームかけて回転させるか
 */
void CharaModel::ResetNeckAngle(u32 frame)
{
  // 現在の角度を保存
  m_NeckBeginAngleXRad = this->GetNeckCurrentAngleXRad();
  m_NeckBeginAngleYRad = this->GetNeckCurrentAngleYRad();

  // パラメータ設定
  m_NeckTargetAngleXRad = 0;
  m_NeckTargetAngleYRad = 0;
  m_NeckCurrentFrame = 0;
  m_NeckTargetFrame = frame;
}

/**
 * @brief 首制御終了確認
 * 
 * @return true:終了 false:更新中
 */
bool CharaModel::IsFinishNeck() const
{
  // 目標フレームまで進んだら終了
  // 目標フレームが設定されてなくても0なのでここで終了
  if (m_NeckCurrentFrame >= m_NeckTargetFrame)
  {
    return true;
  }

  // 更新中
  return false;
}

/**
 * @brief 首制御で変更が発生しているか？
 * @note  現在0度でも、指定フレームに達していない場合はtrueを返す
 *
 * @return true:発生している
 */
bool CharaModel::IsChangedNeck() const
{
  if (IsFinishNeck() == false || GetNeckCurrentAngleXRad() != 0.0f || GetNeckCurrentAngleYRad() != 0.0f)
  {
    return true;
  }

  return false;
}

f32 CharaModel::GetNeckCurrentAngleXRad() const
{
  // 0割り防止
  if (m_NeckTargetFrame == 0)
  {
    return 0.0f;
  }

  // 目標の角度まで線形補間
  return m_NeckBeginAngleXRad + (((m_NeckTargetAngleXRad - m_NeckBeginAngleXRad) * m_NeckCurrentFrame) / m_NeckTargetFrame);
}

f32 CharaModel::GetNeckCurrentAngleYRad() const
{
  // 0割り防止
  if (m_NeckTargetFrame == 0)
  {
    return 0.0f;
  }

  // 目標の角度まで線形補間
  return m_NeckBeginAngleYRad + (((m_NeckTargetAngleYRad - m_NeckBeginAngleYRad) * m_NeckCurrentFrame) / m_NeckTargetFrame);
}

void CharaModel::SetupRandomFuc( u32 (*pGetRandomFunc)(u32) )
{
  GetPublicRand = pGetRandomFunc;
}

void CharaModel::UpdateAutoBlink()
{
  switch (m_AutoBlinkMode)
  {
  // フィールド用
  case AUTO_BLINK_MODE_FIELD:
    this->UpdateAutoBlinkField();
    break;

  // バトル用
  case AUTO_BLINK_MODE_BATTLE:
    this->UpdateAutoBlinkBattle();
    break;

  // 自動目パチ無し
  default:
    break;
  }
}

void CharaModel::UpdateAutoBlinkField()
{
  switch (m_AutoBlinkSequence)
  {
  // 開ける：初期化
  case 0:
    {
      m_EyeIndex = EYE_INDEX_OPEN;

      // 乱数取得関数が設定されていなければbreak
      if(GetPublicRand == NULL)
      {
        break;
      }

      // 連続瞬き
      if( m_isBlinkImmediaterly )
      {
        // 次へ
        ++m_AutoBlinkSequence;
        break;
      }

      // 瞬き禁止フレーム
      {
        if( m_AutoBlinkProhibitedFrame > 0 )
        {
          m_AutoBlinkProhibitedFrame--;
        }

        if( m_AutoBlinkProhibitedFrame != 0 )
        {
          break;
        }
      }

      // 瞬きはポアソン分布 平均到着率 10 times / minute
      if (GetPublicRand(128) == 0 )
      {
        // 次へ
        ++m_AutoBlinkSequence;
      }
    }
    break;

  // 半目
  case 1:

    m_EyeIndex = EYE_INDEX_HALF_OPEN;

    // 次へ
    ++m_AutoBlinkSequence;
    break;

  // 閉じる
  case 2:

    m_EyeIndex = EYE_INDEX_CLOSE;

    // 次へ
    ++m_AutoBlinkSequence;
    break;

  // 半目
  case 3:

    m_EyeIndex = EYE_INDEX_HALF_OPEN;

    // 次へ
    ++m_AutoBlinkSequence;
    break;

  // 半目 開く速度は1フレーム遅い
  case 4:

    m_EyeIndex = EYE_INDEX_HALF_OPEN;

    // 次へ
    ++m_AutoBlinkSequence;
    break;

  // 開く
  case 5:

    m_EyeIndex = EYE_INDEX_OPEN;

    // 33%の確立で連続で瞬きをする
    if ( !m_isBlinkImmediaterly && GetPublicRand(3) == 0 )
    {
      m_isBlinkImmediaterly      = true;
      m_AutoBlinkProhibitedFrame = 60;
    }
    else
    {
      m_isBlinkImmediaterly      = false;
      m_AutoBlinkProhibitedFrame = 30;
    }

    // 最初へ
    m_AutoBlinkSequence = 0;
    break;
  }
}

void CharaModel::UpdateAutoBlinkBattle()
{
  UpdateAutoBlinkField();
}

void CharaModel::ChangeAnimationByResourceNode_Face(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, s32 slot)
{
  this->ChangeAnimationByResourceNode(pResourceNode, slot, ANIMATION_BIT_MATERIAL);
}

void CharaModel::SetAnimationStepFrame_Face(s32 stepFrame, s32 slot)
{
  this->SetAnimationStepFrame(0.0f, slot, ANIMATION_BIT_MATERIAL);
}

void CharaModel::SetAnimationFrame_Face(s32 frame, s32 slot)
{
  this->SetAnimationFrame(frame, slot, ANIMATION_BIT_MATERIAL);
}

void CharaModel::OnUpdateNeck()
{
}

}}
