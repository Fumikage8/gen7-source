#include <model/include/gfl2_DressUpModel.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include "gfl2_DressUpModelResourceUnit.h"

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
DressUpModel::DressUpModel() :
  m_pDressUpModelResourceUnitList(NULL)
{
}

/**
 * @brief デストラクタ
 */
DressUpModel::~DressUpModel()
{
}

/**
 * @brief リソースノードから作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pDressUpResourceUnitList ドレスアップリソースユニットリスト
 * @param pModelResource ベースモデルリソース
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool DressUpModel::Create(gfl2::gfx::IGLAllocator* pGfxAllocator,
                          gfl2::heap::HeapBase* pAnimationHeap,
                          DressUpModelResourceUnit* pDressUpModelResourceUnitList,
                          gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
                          gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  m_pDressUpModelResourceUnitList = pDressUpModelResourceUnitList;
  bool result = this->BaseModel::Create(pGfxAllocator, pAnimationHeap, pModelResource, pMotionConfigResource);

  if (!result)
  {
    return false;
  }

  // ベースジョイント用のモデルは表示しない
  this->BaseModel::SetVisible(false);

  // 全ジョイント有効にする
  for (u32 i = 0; i < this->GetModelInstanceNode()->GetJointNum(); ++i)
  {
    this->GetModelInstanceNode()->GetJointInstanceNode(i)->SetNeedRendering();
  }

  // 全パーツモデル作成
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pPartsModelUnit = &pDressUpModelResourceUnitList[i];

    if (pPartsModelUnit->GetModelResourceNode() == NULL)
    {
      continue;
    }

    BaseModel* pBaseModel = &m_PartsModelList[i];

    BaseModel* pJointReferenceModel = this;
    pBaseModel->Create(pGfxAllocator, pAnimationHeap, pPartsModelUnit->GetModelResourceNode(), pJointReferenceModel, pMotionConfigResource);
    pBaseModel->SetAnimationPackList(pPartsModelUnit->GetAnimationPackList());
  }

  // 顔のパーツにはアニメーションコントローラが必要
  if (m_PartsModelList[PARTS_MODEL_FACE].IsCreated())
  {
    BaseModel* pFaceModel = &m_PartsModelList[PARTS_MODEL_FACE];
    if (!pFaceModel->HasAnimationController())
    {
      pFaceModel->CreateAnimationController();
    }
  }


  // アタッチ箇所
  s32 headIndex = this->GetJointIndex("Head");
  s32 neckIndex = this->GetJointIndex("Neck");
  s32 hipsIndex = this->GetJointIndex("Hips");
  s32 waistIndex = this->GetJointIndex("Waist");
  s32 accBadgeIndex = this->GetJointIndex("Spine3"); // ほんとはSpine3/AccBadge
  s32 accEyeIndex = this->GetJointIndex("AccEye");
  s32 accHairIndex = this->GetJointIndex("Head"); // ほんとはHead/AccHair

  // アタッチが必要なパーツの対処
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    // モデルが作成されていないなら無視
    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    // アタッチが必要なパーツ
    if (pBaseModel->GetModelInstanceNode()->GetJointReferenceModel() != NULL)
    {
      continue;
    }

    // アクセサリ系
    if (i == PARTS_MODEL_ACCBADGE)
    {
      pBaseModel->SetParent(this, accBadgeIndex);

      DressUpModelResourceUnit* pPartsModelUnit = &pDressUpModelResourceUnitList[PARTS_MODEL_TOPS];
      const DressUpModelResourceUnit::DressUpLocator* pPartsLocator = pPartsModelUnit->GetDressUpLocator();

      if (pPartsLocator != NULL)
      {
        pBaseModel->SetScale(pPartsLocator->scaleX, pPartsLocator->scaleY, pPartsLocator->scaleZ);
        pBaseModel->SetRotation(pPartsLocator->rotationX, pPartsLocator->rotationY, pPartsLocator->rotationZ);
        pBaseModel->SetPosition(pPartsLocator->translateX, pPartsLocator->translateY, pPartsLocator->translateZ);
      }
    }
    else if (i == PARTS_MODEL_ACCEYE)
    {
      pBaseModel->SetParent(this, accEyeIndex);
    }
    else if (i == PARTS_MODEL_ACCHAIR)
    {
      pBaseModel->SetParent(this, accHairIndex);

      DressUpModelResourceUnit* pHairPartsModelUnit = &pDressUpModelResourceUnitList[PARTS_MODEL_HAIR];
      const DressUpModelResourceUnit::DressUpLocator* pHairPartsLocator = pHairPartsModelUnit->GetDressUpLocator();

      DressUpModelResourceUnit* pAccHairPartsModelUnit = &pDressUpModelResourceUnitList[PARTS_MODEL_ACCHAIR];
      bool isHeadband = pAccHairPartsModelUnit->IsItemOption(DressUpModelResourceUnit::OPTION_BIT_HEADBAND); // カチューシャは例外的にロケータを使わない

      // カチューシャはHeadにアタッチ
      if (isHeadband)
      {
        // Y軸が上で作られているのでHeadのバンドポーズで回転させる必要がある
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = this->GetModelInstanceNode()->GetJointInstanceNode(accHairIndex);
        gfl2::math::Matrix34 bindPose = pJointNode->GetBindPoseMatrix();
        gfl2::math::Quaternion rot;
        rot.MatrixToQuaternion(bindPose);

        gfl2::math::EulerRotation erot(rot);
        pBaseModel->SetRotation(erot.x, erot.y, erot.z);
      }
      // それ以外はロケータの値を使う
      else if (pHairPartsLocator != NULL)
      {
        pBaseModel->SetScale(pHairPartsLocator->scaleX, pHairPartsLocator->scaleY, pHairPartsLocator->scaleZ);
        pBaseModel->SetRotation(pHairPartsLocator->rotationX, pHairPartsLocator->rotationY, pHairPartsLocator->rotationZ);
        pBaseModel->SetPosition(pHairPartsLocator->translateX, pHairPartsLocator->translateY, pHairPartsLocator->translateZ);
      }
    }
    // その他
    else
    {
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pAttachJoint = pBaseModel->GetModelInstanceNode()->GetJointInstanceNode(0);
      if (pAttachJoint == NULL)
      {
        continue;
      }

      const c8* pAttachName = pAttachJoint->GetName();
      if (::std::memcmp(pAttachName, "Head", 4) == 0)
      {
        pBaseModel->SetParent(this, headIndex);
      }
      else if (::std::memcmp(pAttachName, "Neck", 4) == 0)
      {
        pBaseModel->SetParent(this, neckIndex);
      }
      else if (::std::memcmp(pAttachName, "Hips", 4) == 0)
      {
        pBaseModel->SetParent(this, hipsIndex);
      }
      else if (::std::memcmp(pAttachName, "Waist", 5) == 0)
      {
        pBaseModel->SetParent(this, waistIndex);
      }
    }
  }


  // 帽子を被っていたら髪のnohatメッシュは非表示にする
  if (m_PartsModelList[PARTS_MODEL_HAT].IsCreated() && m_PartsModelList[PARTS_MODEL_HAIR].IsCreated())
  {
    BaseModel* pHairModel = &m_PartsModelList[PARTS_MODEL_HAIR];
    for (s32 i = 0; i < pHairModel->GetMeshCount(); ++i)
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = pHairModel->GetModelInstanceNode()->GetMeshLinkData(i);
      if (::strstr(pMeshLinkData->GetName(), "nohat") != NULL)
      {
        pHairModel->SetMeshVisible(i, false);
        break;
      }
    }
  }

  return true;
}

/**
 * @brief 破棄
 */
void DressUpModel::Destroy()
{
  // 勝手に殺されるので子供を外す
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    pBaseModel->SetParent(NULL);
  }

  // 削除
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    pBaseModel->Destroy();
  }

  this->BaseModel::Destroy();
}

/**
 * @brief 破棄が可能か確認
 */
bool DressUpModel::CanDestroy() const
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->CanDestroy())
    {
      return false;
    }
  }

  return this->BaseModel::CanDestroy();
}

/**
 * @brief 解放チェック有効無効設定
 */
void DressUpModel::SetDestroyCheckEnable(bool enable)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    pBaseModel->SetDestroyCheckEnable(enable);
  }

  this->BaseModel::SetDestroyCheckEnable(enable);
}

//---------------------------------------------------------------------------------
// ■ 表示のON/OFF

/**
 * @brief 表示設定
 *
 * @param flag trueなら表示、falseなら非表示
 */
void DressUpModel::SetVisible(b32 flag)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    pBaseModel->SetVisible(flag);
  }
}

/**
 * @brief 表示されているか？
 *
 * @return trueなら表示、falseなら非表示
 */
b32 DressUpModel::IsVisible() const
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (pBaseModel->IsVisible())
    {
      return true;
    }
  }

  return false;
}

//---------------------------------------------------------------------------------
// ■ アニメーション

/**
 * @brief アニメーションの更新
 *
 * @param animationBit アニメーションの種類
 */
f32 DressUpModel::UpdateAnimation(AnimationBit animationBit)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->UpdateAnimation(animationBit);
  }

  return this->BaseModel::UpdateAnimation(animationBit);
}

/**
 * @brief アニメーション再生用のメモリを明示的に確保する
 *
 * @param pAnimationHeap アニメーションヒープ
 * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::AllocateAnimation(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, u32 animationId, u32 slotIndex, AnimationBit animationBit)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    AnimationBit tmpAnimationBit = animationBit;

    // 顔はマテリアルアニメだけ
    if (i == PARTS_MODEL_FACE)
    {
      tmpAnimationBit = ANIMATION_BIT_MATERIAL;
    }

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->AllocateAnimation(pAnimationHeap, smoothFlag, animationId, slotIndex, tmpAnimationBit);
  }

  this->BaseModel::AllocateAnimation(pAnimationHeap, smoothFlag, animationId, slotIndex, animationBit);
}

/**
 * @brief アニメーション再生用のメモリを明示的に確保する：リソースノード指定版
 *
 * @param pAnimationHeap アニメーションヒープ
 * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
 * @param pResourceNode リソースノード
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::AllocateAnimationByResourceNode(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex, AnimationBit animationBit)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    AnimationBit tmpAnimationBit = animationBit;

    // 顔はマテリアルアニメだけ
    if (i == PARTS_MODEL_FACE)
    {
      tmpAnimationBit = ANIMATION_BIT_MATERIAL;
    }

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->AllocateAnimationByResourceNode(pAnimationHeap, smoothFlag, pResourceNode, slotIndex, tmpAnimationBit);
  }

  this->BaseModel::AllocateAnimationByResourceNode(pAnimationHeap, smoothFlag, pResourceNode, slotIndex, animationBit);
}

/**
 * @brief アニメーションの変更：バインドポーズに戻す
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::ChangeAnimationBindpose(u32 slotIndex, AnimationBit animationBit)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    AnimationBit tmpAnimationBit = animationBit;

    // 顔はマテリアルアニメだけ
    if (i == PARTS_MODEL_FACE)
    {
      tmpAnimationBit = ANIMATION_BIT_MATERIAL;
    }

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->ChangeAnimationByResourceNode(NULL, slotIndex, tmpAnimationBit);
  }

  this->BaseModel::ChangeAnimationByResourceNode(NULL, slotIndex, animationBit);
}

/**
 * @brief アニメーションの変更：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::ChangeAnimation(u32 animationId, u32 slotIndex, AnimationBit animationBit)
{
  u32 defaultBlendFrame = this->BaseModel::GetAnimationDefaultBlendFrame(animationId, slotIndex, animationBit);

  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    AnimationBit tmpAnimationBit = animationBit;

    // 顔はマテリアルアニメだけ
    if (i == PARTS_MODEL_FACE)
    {
      tmpAnimationBit = ANIMATION_BIT_MATERIAL;
    }

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->ChangeAnimationSmooth(animationId, defaultBlendFrame, slotIndex, tmpAnimationBit);
  }

  this->BaseModel::ChangeAnimationSmooth(animationId, defaultBlendFrame, slotIndex, animationBit);
}

/**
 * @brief アニメーションの変更：補間あり：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param changeFrame 補間フレーム数
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::ChangeAnimationSmooth(u32 animationId, u32 changeFrame, u32 slotIndex, AnimationBit animationBit)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    AnimationBit tmpAnimationBit = animationBit;

    // 顔はマテリアルアニメだけ
    if (i == PARTS_MODEL_FACE)
    {
      tmpAnimationBit = ANIMATION_BIT_MATERIAL;
    }

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (!pBaseModel->HasAnimationController())
    {
      continue;
    }

    pBaseModel->ChangeAnimationSmooth(animationId, changeFrame, slotIndex, tmpAnimationBit);
  }

  this->BaseModel::ChangeAnimationSmooth(animationId, changeFrame, slotIndex, animationBit);
}

/**
 * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
 *
 * @param enable trueの場合はループする、 falseの場合はループしない
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::SetAnimationLoop(b32 enable, u32 slotIndex, AnimationBit animationBit)
{
  this->BaseModel::SetAnimationLoop(enable, slotIndex, animationBit);
  this->SetAnimationCore(&BaseModel::SetAnimationLoop, enable, slotIndex, animationBit);
}

/**
 * @brief フレーム設定
 *
 * @param frame フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::SetAnimationFrame(f32 frame, u32 slotIndex, AnimationBit animationBit)
{
  this->BaseModel::SetAnimationFrame(frame, slotIndex, animationBit);
  this->SetAnimationCore(&BaseModel::SetAnimationFrame, frame, slotIndex, animationBit);
}

/**
 * @brief 再生速度設定
 *
 * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::SetAnimationStepFrame(f32 stepFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->BaseModel::SetAnimationStepFrame(stepFrame, slotIndex, animationBit);
  this->SetAnimationCore(&BaseModel::SetAnimationStepFrame, stepFrame, slotIndex, animationBit);
}

/**
 * @brief ループの開始フレーム設定
 *
 * @param startFrame 開始フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::SetAnimationStartFrame(f32 startFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->BaseModel::SetAnimationStartFrame(startFrame, slotIndex, animationBit);
  this->SetAnimationCore(&BaseModel::SetAnimationStartFrame, startFrame, slotIndex, animationBit);
}

/**
 * @brief ループの終了フレーム設定
 *
 * @param endFrame 終了フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void DressUpModel::SetAnimationEndFrame(f32 endFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->BaseModel::SetAnimationEndFrame(endFrame, slotIndex, animationBit);
  this->SetAnimationCore(&BaseModel::SetAnimationEndFrame, endFrame, slotIndex, animationBit);
}

/**
 * @brief アニメーション計算有効フラグの設定、falseを設定した場合はフレーム更新のみ有効
 *
 * @param enable true:計算有効, false:計算無効
 */
void DressUpModel::SetAnimationCalculateEnable(bool enable)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    pBaseModel->SetAnimationCalculateEnable(enable);
  }

  this->BaseModel::SetAnimationCalculateEnable(enable);
}

/**
 * @brief アニメーション計算有効フラグの取得
 *
 * @return true:計算有効, false:計算無効
 */
bool DressUpModel::IsAnimationCalculateEnable() const
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (pBaseModel->IsAnimationCalculateEnable())
    {
      return true;
    }
  }

  return this->BaseModel::IsAnimationCalculateEnable();
}

/**
 * @brief フレーム更新有効フラグの設定
 *
 * @param enable true:更新有効, false:更新無効
 */
void DressUpModel::SetAnimationUpdateFrameEnable(bool enable)
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    pBaseModel->SetAnimationUpdateFrameEnable(enable);
  }

  this->BaseModel::SetAnimationUpdateFrameEnable(enable);
}

/**
 * @brief フレーム更新有効フラグの取得
 *
 * @return true:更新有効, false:更新無効
 */
bool DressUpModel::IsAnimationUpdateFrameEnable() const
{
  for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pBaseModel = &m_PartsModelList[i];

    if (!pBaseModel->IsCreated())
    {
      continue;
    }

    if (pBaseModel->IsAnimationUpdateFrameEnable())
    {
      return true;
    }
  }

  return this->BaseModel::IsAnimationUpdateFrameEnable();
}



void DressUpModel::ChangeAnimationByResourceNode_Face(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, s32 slot)
{
  BaseModel* pFaceModel = &m_PartsModelList[PARTS_MODEL_FACE];
  if (!pFaceModel->IsCreated())
  {
    return;
  }
  pFaceModel->ChangeAnimationByResourceNode(pResourceNode, slot, ANIMATION_BIT_MATERIAL);
}

void DressUpModel::SetAnimationStepFrame_Face(s32 stepFrame, s32 slot)
{
  BaseModel* pFaceModel = &m_PartsModelList[PARTS_MODEL_FACE];
  if (!pFaceModel->IsCreated())
  {
    return;
  }
  pFaceModel->SetAnimationStepFrame(0.0f, slot, ANIMATION_BIT_MATERIAL);
}

void DressUpModel::SetAnimationFrame_Face(s32 frame, s32 slot)
{
  BaseModel* pFaceModel = &m_PartsModelList[PARTS_MODEL_FACE];
  if (!pFaceModel->IsCreated())
  {
    return;
  }
  pFaceModel->SetAnimationFrame(frame, slot, ANIMATION_BIT_MATERIAL);
}

/**
 * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
 */
void DressUpModel::UpdateInterest()
{
  // 本体の更新
  BaseModel::UpdateInterest();

  // 髪に伝達
  this->UpdateInterestParts(PARTS_MODEL_HAIR);
}

// パーツの視線更新
void DressUpModel::UpdateInterestParts(PartsModel partsModel)
{
  BaseModel* pParts = this->GetPartsModel(partsModel);

  // パーツが生成されていないなら終了
  if (pParts == NULL)
  {
    return;
  }

  // アニメーションを使わないなら終了
  if (pParts->GetAnimationController() == NULL)
  {
    return;
  }

  // ベースのHeadジョイント取得
  s32 baseHeadIndex = this->GetJointIndex("Head");
  if (baseHeadIndex < 0)
  {
    return;
  }
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pBaseHeadJoint = this->GetModelInstanceNode()->GetJointInstanceNode(baseHeadIndex);
  gfl2::math::SRT baseHeadSRT = pBaseHeadJoint->GetLocalSRT();

  // パーツのHeadにSRTをコピー
  s32 partsHeadIndex = pParts->GetJointIndex("Head");
  if (partsHeadIndex < 0)
  {
    return;
  }
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pPartsHeadJoint = pParts->GetModelInstanceNode()->GetJointInstanceNode(partsHeadIndex);
  pPartsHeadJoint->SetLocalSRT(pBaseHeadJoint->GetLocalSRT());

  // ワールド行列再計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast(pPartsHeadJoint);
}

// 首更新後処理
void DressUpModel::OnUpdateNeck()
{
  // 着せ替えは首更新後に髪パーツに伝達する必要がある
  this->UpdateInterestParts(PARTS_MODEL_HAIR);
}

// 全力ビーズのON/OFF設定
void DressUpModel::SetZenBeadsVisible(b32 flag)
{
  this->SetPartsModelMeshVisible(PARTS_MODEL_BNGL, "bngl_beadsSkin", flag);
}

// 全力ビーズのON/OFF取得
b32 DressUpModel::IsZenBeadsVisible() const
{
  return this->IsPartsModelMeshVisible(PARTS_MODEL_BNGL, "bngl_beadsSkin");
}

// バッグ証のON/OFF設定
void DressUpModel::SetBagCharmVisible(b32 flag)
{
  this->SetPartsModelMeshVisible(PARTS_MODEL_BAG, "bag_charmSkin", flag);
}

// バッグ証のON/OFF取得
b32 DressUpModel::IsBagCharmVisible() const
{
  return this->IsPartsModelMeshVisible(PARTS_MODEL_BAG, "bag_charmSkin");
}

// パーツモデルのメッシュON/OFF設定
void DressUpModel::SetPartsModelMeshVisible(PartsModel partsModel, const char* pMeshName, b32 flag)
{
  // メッシュを探す
  s32 meshIndex = this->GetPartsModelMeshIndex(partsModel, pMeshName);

  // メッシュが無い
  if (meshIndex < 0)
  {
    return;
  }

  // メッシュON/OFF設定
  return m_PartsModelList[partsModel].SetMeshVisible(meshIndex, flag);
}

// パーツモデルのメッシュON/OFF取得
b32 DressUpModel::IsPartsModelMeshVisible(PartsModel partsModel, const char* pMeshName) const
{
  // メッシュを探す
  s32 meshIndex = this->GetPartsModelMeshIndex(partsModel, pMeshName);

  // メッシュが無い
  if (meshIndex < 0)
  {
    return false;
  }

  // メッシュON/OFF確認
  return m_PartsModelList[partsModel].IsMeshVisible(meshIndex);
}

// パーツモデルからメッシュインデックス取得
s32 DressUpModel::GetPartsModelMeshIndex(PartsModel partsModel, const char* pMeshName) const
{
  if (!m_PartsModelList[partsModel].IsCreated())
  {
    return -1;
  }
  if (pMeshName == NULL)
  {
    return -1;
  }

  BaseModel* pPartsModel = const_cast<BaseModel*>(&m_PartsModelList[partsModel]);
  for (s32 i = 0; i < pPartsModel->GetMeshCount(); ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = pPartsModel->GetModelInstanceNode()->GetMeshLinkData(i);
    if (::strstr(pMeshLinkData->GetName(), pMeshName) != NULL)
    {
      return i;
    }
  }

  return -1;
}

}}
