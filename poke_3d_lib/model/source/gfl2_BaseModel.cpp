//=============================================================================
/**
 * @file    gfl2_BaseModel.cpp
 * @brief   
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.4    ishiguro  「returning reference to local temporary」警告対処
 *  2016.4.19   ishiguro  DetachLocatorNode関数追加
 *  2016.4.8    ishiguro  (MATERIAL_SAMPLER_STATE_OBJECT_ENABLEにて)未使用機能の封印
 *
 */
//=============================================================================
#include <model/include/gfl2_BaseModel.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

using namespace gfl2::renderingengine::scenegraph;

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
BaseModel::BaseModel()
{
  // フィールド初期化
  this->InitializeField_BaseModel();
}

/**
 * @brief デストラクタ
 */
BaseModel::~BaseModel()
{
  // 念のため破棄
  this->Destroy();
}

/**
 * @brief BaseModelのフィールド初期化
 */
void BaseModel::InitializeField_BaseModel()
{
  m_pModelInstanceNode = NULL;
  m_pParent = NULL;
  m_ParentJointIndex = -1;
  m_EulerRotation = gfl2::math::EulerRotation();
  m_pAnimationController = NULL;
  m_pAnimationPackList = NULL;
  m_pAnimationHeap = NULL;
  for( u32 i=0 ; i<SLOT_CACHE_CNT_FOR_ANIMATION_ID ; i++ )
  {
    for( u32 j=0 ; j<BIT_CACHE_CNT_FOR_ANIMATION_ID ; j++ )
    {
      m_animationIdCache[i][j] = -1;
    }
  }
  m_pInterestController = NULL;
  m_pSpringController = NULL;
  m_pStoreJointSRTPairList = NULL;
}

/**
 * @brief 共通作成処理
 */
bool BaseModel::CreateCommon(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  if (pGfxAllocator == NULL)
  {
    return false;
  }

  if (pModelInstance == NULL)
  {
    return false;
  }

  // フィールド初期化
  this->InitializeField_BaseModel();

  // モデルインスタンスノード保存
  m_pModelInstanceNode = pModelInstance;

  // アニメーションヒープ保存
  m_pAnimationHeap = pAnimationHeap;

  // 描画開放チェック追加
  this->SetDestroyCheckEnable(true);

  // 視線コントローラー初期化
  this->InitializeInterest(pGfxAllocator, pMotionConfigResource);

  // ジョイント参照しないならアニメーションコントローラーを初期化する
  if (m_pModelInstanceNode->GetJointReferenceModel() == NULL)
  {
    this->CreateAnimationController();
  }

  // SRT保存用のバッファ確保
  this->InitializeJointSRTPair();

  // 作成コールバック
  this->OnCreate();

  return true;
}

/**
 * @brief 作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelResource ベースモデルリソース
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool BaseModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  if (pGfxAllocator == NULL)
  {
    return false;
  }
  if (pModelResource == NULL)
  {
    return false;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = instance::InstanceCreator::CreateModelInstanceNode(pGfxAllocator, pModelResource);
  return this->CreateCommon(pGfxAllocator, pAnimationHeap, pModelInstance, pMotionConfigResource);
}

/**
 * @brief ジョイント共有モデルを作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelResource ベースモデルリソース
 * @param pJointReferenceModel ジョイント共有モデル
 * @param pMotionConfigResource モーションコンフィグリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool BaseModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
  BaseModel* pJointReferenceModel,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  if (pGfxAllocator == NULL)
  {
    return false;
  }
  if (pModelResource == NULL)
  {
    return false;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pJointSharedModel_instance = NULL;
  if (pJointReferenceModel != NULL)
  {
    // ジョイント参照
    pJointSharedModel_instance = pJointReferenceModel->GetModelInstanceNode();
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = instance::InstanceCreator::CreateModelInstanceNode(pGfxAllocator, pModelResource, pJointSharedModel_instance);
  return this->CreateCommon(pGfxAllocator, pAnimationHeap, pModelInstance, pMotionConfigResource);
}


/**
 * @brief ジョイント共有モデルを作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pModelResource ベースモデルリソース
 * @param pJointReferenceModel ジョイント共有モデル
 * @param pTextureShaderNodeRoot てリソース
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool BaseModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
  gfl2::renderingengine::scenegraph::DagNode* pTextureShaderNodeRoot)
{
  if (pGfxAllocator == NULL)
  {
    return false;
  }
  if (pModelResource == NULL)
  {
    return false;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance = instance::InstanceCreator::CreateModelInstanceNode(pGfxAllocator, pModelResource, pTextureShaderNodeRoot);
  return this->CreateCommon(pGfxAllocator, pAnimationHeap, pModelInstance, NULL);
}

/**
* @brief モデルインスタンスノードを指定して作成
*
* @param pGfxAllocator Gfxライブラリ用のアロケータ
* @param pAnimationHeap Animationライブラリ用のヒープ
* @param pModelInstance モデルインスタンス
* @param pMotionConfigResource モーションコンフィグリソース
*
* @return 作成成功したらtrue, 失敗したらfalse
*/
bool BaseModel::Create(
  gfl2::gfx::IGLAllocator* pGfxAllocator,
  gfl2::heap::HeapBase* pAnimationHeap,
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  return this->CreateCommon(pGfxAllocator, pAnimationHeap, pModelInstance, NULL);
}

/**
 * @brief 作成時に呼ばれるコールバック
 */
void BaseModel::OnCreate()
{
}

// アニメーションコントローラー作成
void BaseModel::CreateAnimationController()
{
  m_pAnimationController = GFL_NEW(m_pAnimationHeap) gfl2::animation::AnimationController();
  m_pAnimationController->Initialize(m_pAnimationHeap, m_pModelInstanceNode);

  for( u32 i=0 ; i<SLOT_CACHE_CNT_FOR_ANIMATION_ID ; i++ )
  {
    for( u32 j=0 ; j<BIT_CACHE_CNT_FOR_ANIMATION_ID ; j++ )
    {
      m_animationIdCache[i][j] = -1;
    }
  }
}
 
/**
 * @brief 破棄
 */
void BaseModel::Destroy()
{
  // モデルインスタンスの破棄
  GFL_SAFE_DELETE(m_pModelInstanceNode);

  // アニメーションコントローラー破棄
  GFL_SAFE_DELETE(m_pAnimationController);

  // 視線コントローラー削除
  GFL_SAFE_DELETE(m_pInterestController);

  // SRT保存用バッファの削除
  GFL_SAFE_DELETE_ARRAY(m_pStoreJointSRTPairList);
}

/**
 * @brief 破棄が可能か確認
 */
bool BaseModel::CanDestroy() const
{
  // 作成されてないならDestroy()を通しても大丈夫なのでtrueにしておく
  if (!this->IsCreated())
  {
    return true;
  }

  // インスタンスの参照カウントが０なら安全に破棄できる
  return (m_pModelInstanceNode->GetReferenceCnt() == 0);
}

/**
 * @brief 解放チェック有効無効設定
 */
void BaseModel::SetDestroyCheckEnable(bool enable)
{
  if (!this->IsCreated())
  {
    return;
  }

  m_pModelInstanceNode->SetDrawCheckListener(enable);
}

/**
 * @brief 作成済みか確認
 *
 * @return true:作成済み false:作成されていない
 */
bool BaseModel::IsCreated() const
{
  return m_pModelInstanceNode != NULL;
}

//---------------------------------------------------------------------------------
// ■ 階層構造、親子関係

/**
 * @brief 親モデルの設定
 *
 * @param pParent 親になるモデル
 * @param jointIndex 接続するジョイント番号：-1でモデル自体に接続
 */
void BaseModel::SetParent(BaseModel* pParent, s32 jointIndex)
{
  // 親がNULLなら親子関係解除
  if (pParent == NULL)
  {
    m_pModelInstanceNode->RemoveFromParent();
    m_pParent = NULL;
    m_ParentJointIndex = -1;
    return;
  }

  if (jointIndex < 0)
  {
    // モデル自体に接続
    pParent->m_pModelInstanceNode->AddChild(m_pModelInstanceNode);
  }
  else
  {
    // ジョイントに接続
    pParent->m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->AddChild(m_pModelInstanceNode);
  }

  // BaseModel側でも保持
  m_pParent = pParent;
  m_ParentJointIndex = jointIndex;
}

/**
 * @brief 親モデルの取得
 *
 * @return 親モデルを返す
 */
BaseModel* BaseModel::GetParent() const
{
  return m_pParent;
}

/**
 * @brief 親モデルの接続先のジョイント番号取得
 *
 * @return 親モデルの接続先のジョイント番号を返す、-1でモデル自体に接続
 */
s32 BaseModel::GetParentJointIndex() const
{
  return m_ParentJointIndex;
}

//---------------------------------------------------------------------------------
// ■ 位置、回転、拡縮の設定

/**
 * @brief 位置設定
 *
 * @param position 位置
 */
void BaseModel::SetPosition(const gfl2::math::Vector3& position)
{
  m_pModelInstanceNode->SetLocalTranslate(position);
}

/**
 * @brief 位置設定：要素毎
 *
 * @param positionX 位置Ｘ
 * @param positionY 位置Ｙ
 * @param positionZ 位置Ｚ
 */
void BaseModel::SetPosition(f32 positionX, f32 positionY, f32 positionZ)
{
  this->SetPosition(gfl2::math::Vector3(positionX, positionY, positionZ));
}

/**
 * @brief 位置取得
 *
 * @return 位置を返す
 */
const gfl2::math::Vector3& BaseModel::GetPosition() const
{
  return m_pModelInstanceNode->GetLocalTranslate();
}

/**
 * @brief 回転設定
 *
 * @param rotationRad 回転（ラジアン）
 * @param rotationOrder 回転順：デフォルトはZYX
 */
void BaseModel::SetRotation(const gfl2::math::Vector3& rotationRad, gfl2::math::EulerRotation::RotationOrder rotationOrder)
{
  m_EulerRotation.Set(rotationRad, rotationOrder);
  this->SetRotationQuat(m_EulerRotation.ToQuaternion());
}

/**
 * @brief 回転設定：要素毎
 *
 * @param rotationRadX 回転Ｘ（ラジアン）
 * @param rotationRadY 回転Ｙ（ラジアン）
 * @param rotationRadZ 回転Ｚ（ラジアン）
 * @param rotationOrder 回転順：デフォルトはZYX
 */
void BaseModel::SetRotation(f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ, gfl2::math::EulerRotation::RotationOrder rotationOrder)
{
  this->SetRotation(gfl2::math::Vector3(rotationRadX, rotationRadY, rotationRadZ), rotationOrder);
}

/**
 * @brief 回転取得
 *        ※ SetRotation以外で回転を設定した場合は見た目に合わない値が返ります
 *
 * @return 回転を返す
 */
gfl2::math::Vector3 BaseModel::GetRotation() const
{
  gfl2::math::Vector3 euler(m_EulerRotation.x, m_EulerRotation.y, m_EulerRotation.z);
  return euler;
}

/**
 * @brief 拡縮設定
 *
 * @param scale 拡縮
 */
void BaseModel::SetScale(const gfl2::math::Vector3& scale)
{
  m_pModelInstanceNode->SetLocalScale(scale);
}

/**
 * @brief 拡縮設定：要素毎
 *
 * @param scaleX 拡縮Ｘ
 * @param scaleY 拡縮Ｙ
 * @param scaleZ 拡縮Ｚ
 */
void BaseModel::SetScale(f32 scaleX, f32 scaleY, f32 scaleZ)
{
  this->SetScale(gfl2::math::Vector3(scaleX, scaleY, scaleZ));
}

/**
 * @brief 拡縮取得
 *
 * @return 拡縮を返す
 */
const gfl2::math::Vector3& BaseModel::GetScale() const
{
  return m_pModelInstanceNode->GetLocalScale();
}


//---------------------------------------------------------------------------------
// ■ 回転の設定：クォータニオン

/**
 * @brief 回転設定：クォータニオン
 *
 * @param rotation 回転
 */
void BaseModel::SetRotationQuat(const gfl2::math::Quaternion& rotation)
{
  m_pModelInstanceNode->SetLocalRotation(rotation);
}

/**
 * @brief 回転取得：クォータニオン
 *
 * @return 回転を返す
 */
const gfl2::math::Quaternion& BaseModel::GetRotationQuat() const
{
  return m_pModelInstanceNode->GetLocalRotation();
}

//---------------------------------------------------------------------------------
// ■ メッシュ

/**
 * @brief メッシュ数取得
 *
 * @return メッシュ数を返す
 */
s32 BaseModel::GetMeshCount() const
{
  return m_pModelInstanceNode->GetMehsNum();
}

/**
 * @brief メッシュ番号取得
 *
 * @param pName 検索するメッシュ名
 *
 * @return メッシュ番号を返す：見つからなければ-1
 */
s32 BaseModel::GetMeshIndex(const char* pName) const
{
  u32 meshCount = this->GetMeshCount();
  u32 hash = gfl2::util::FnvHash((const u8*)pName).value;

  for (u32 i = 0; i < meshCount; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = m_pModelInstanceNode->GetMeshLinkData(i);

    if (hash != pMeshLinkData->GetNameHash())
    {
      continue;
    }

    if (::std::strcmp(pName, pMeshLinkData->GetName()) == 0)
    {
      return i;
    }
  }

  return -1;
}

/**
 * @brief メッシュの表示設定
 *
 * @param meshIndex メッシュ番号
 * @param visible tureなら表示、 falseなら非表示
 */
void BaseModel::SetMeshVisible(const s32 meshIndex, b32 visible)
{
  m_pModelInstanceNode->GetMeshLinkData(meshIndex)->SetVisible(visible != 0);
}

/**
 * @brief メッシュが表示されるか？
 *
 * @param meshIndex メッシュ番号
 *
 * @return tureなら表示、 falseなら非表示
 */
b32 BaseModel::IsMeshVisible(const s32 meshIndex) const
{
  return m_pModelInstanceNode->GetMeshLinkData(meshIndex)->IsVisible();
}

//---------------------------------------------------------------------------------
// ■ 表示のON/OFF

/**
 * @brief 表示設定
 *
 * @param flag trueなら表示、falseなら非表示
 */
void BaseModel::SetVisible(b32 flag)
{
  m_pModelInstanceNode->SetVisible(flag);
}

/**
 * @brief 表示されているか？
 *
 * @return trueなら表示、falseなら非表示
 */
b32 BaseModel::IsVisible() const
{
  return m_pModelInstanceNode->IsVisible();
}

//---------------------------------------------------------------------------------
// ■ モデル行列

/**
 * @brief ワールド行列取得 ※シーンのトラバース後に有効
 *
 * @return ワールド行列を返す
 */
const gfl2::math::Matrix34& BaseModel::GetWorldMatrix()
{
  return m_pModelInstanceNode->GetWorldMatrix();
}

//---------------------------------------------------------------------------------
// ■ ジョイント

/**
 * @brief ジョイント数取得
 *
 * @return ジョイント数を返す
 */
s32 BaseModel::GetJointCount() const
{
  return m_pModelInstanceNode->GetJointNum();
}

/**
 * @brief ジョイント番号取得
 *
 * @param pName 検索するジョイント名
 *
 * @return ジョイント番号を返す：見つからなければ-1
 */
s32 BaseModel::GetJointIndex(const char* pName) const
{
  u32 jointCount = this->GetJointCount();
  //u32 hash = gfl2::util::FnvHash((const u8*)pName).value;

  for (u32 i = 0; i < jointCount; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJoint = m_pModelInstanceNode->GetJointInstanceNode(i);

    //if (hash != pJoint->GetNameHash())
    //{
    //  continue;
    //}

    if (::std::strcmp(pName, pJoint->GetName()) == 0)
    {
      return i;
    }
  }

  return -1;
}

/**
 * @brief ジョイントのローカルSRT取得
 *
 * @param jointIndex ジョイント番号
 *
 * @return ジョイントのローカルSRTを返す
 */
const gfl2::math::SRT& BaseModel::GetJointLocalSRT(s32 jointIndex) const
{
  return m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->GetLocalSRT();
}

/**
 * @brief ジョイントのワールド位置取得 ※シーンのトラバース後に有効
 *
 * @param jointIndex ジョイント番号
 *
 * @return ジョイントのワールド位置を返す
 */
gfl2::math::Vector3 BaseModel::GetJointWorldPosition(s32 jointIndex) const
{
  return m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->GetWorldMatrix().GetElemPosition();
}

/**
 * @brief ジョイントのワールド行列取得 ※シーンのトラバース後に有効
 *
 * @param jointIndex ジョイント番号
 *
 * @return ジョイントのワールド行列を返す
 */
const gfl2::math::Matrix34& BaseModel::GetJointWorldMatrix(s32 jointIndex) const
{
  return m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->GetWorldMatrix();
}

/**
 * @brief ロケーターノードをアタッチ
 *
 * @param pLocatorNodeTop ロケーターノード
 */
void BaseModel::AttachLocatorNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pLocatorNodeTop)
{
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode::Util::AttachToJointInstanceNode(
    m_pModelInstanceNode->GetJointInstanceNode(0),
    pLocatorNodeTop
    );
}

/**
 * @brief ロケーターノードをデタッチ
 *
 * @param pLocatorNodeTop ロケーターノード
 */
void BaseModel::DetachLocatorNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pLocatorNodeTop)
{
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode::Util::DetachFromParentNode(
    pLocatorNodeTop
    );
}

//---------------------------------------------------------------------------------
// ■ マテリアル

/**
 * @brief マテリアル数取得
 *
 * @return マテリアル数を返す
 */
u32 BaseModel::GetMaterialCount() const
{
  return m_pModelInstanceNode->GetMaterialNum();
}

/**
 * @brief マテリアル番号取得
 *
 * @return マテリアル番号を返す：見つからなければ-1
 */
s32 BaseModel::GetMaterialIndex(const c8* pName) const
{
  u32 materialCount = this->GetMaterialCount();
  u32 hash = gfl2::util::FnvHash((const u8*)pName).value;

  for (u32 i = 0; i < materialCount; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterial = m_pModelInstanceNode->GetMaterialInstanceNode(i);

    if (hash != pMaterial->GetNameHash())
    {
      continue;
    }

    if (::std::strcmp(pName, pMaterial->GetName()) == 0)
    {
      return i;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------------
// ■ マテリアルカラー

/**
 * @brief エミッションカラー設定
 *
 * @param materialIndex マテリアル番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorEmission(s32 materialIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetEmissionColor(color);
}

/**
 * @brief エミッションカラー設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorEmission(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetEmissionColor(colorR, colorG, colorB, colorA);
}

/**
 * @brief エミッションカラー取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return エミッションカラーを返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorEmission(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetEmissionColor();
}

/**
 * @brief アンビエントカラー設定
 *
 * @param materialIndex マテリアル番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorAmbient(s32 materialIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetAmbientColor(color);
}

/**
 * @brief アンビエントカラー設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorAmbient(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetAmbientColor(colorR, colorG, colorB, colorA);
}

/**
 * @brief アンビエントカラー取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return アンビエントカラーを返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorAmbient(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetAmbientColor();
}

/**
 * @brief ディフューズカラー設定
 *
 * @param materialIndex マテリアル番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorDiffuse(s32 materialIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetDiffuseColor(color);
}

/**
 * @brief ディフューズカラー設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorDiffuse(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetDiffuseColor(colorR, colorG, colorB, colorA);
}

/**
 * @brief ディフューズカラー取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return ディフューズカラーを返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorDiffuse(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetDiffuseColor();
}

/**
 * @brief スペキュラカラー０設定
 *
 * @param materialIndex マテリアル番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorSpecular0(s32 materialIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetSpecularColor0(color);
}

/**
 * @brief スペキュラカラー０設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorSpecular0(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetSpecularColor0(colorR, colorG, colorB, colorA);
}

/**
 * @brief スペキュラカラー０取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return スペキュラカラー０を返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorSpecular0(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetSpecularColor0();
}

/**
 * @brief スペキュラカラー１設定
 *
 * @param materialIndex マテリアル番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorSpecular1(s32 materialIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetSpecularColor1(color);
}

/**
 * @brief スペキュラカラー１設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorSpecular1(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetSpecularColor1(colorR, colorG, colorB, colorA);
}

/**
 * @brief スペキュラカラー１取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return スペキュラカラー１を返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorSpecular1(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetSpecularColor1();
}

//---------------------------------------------------------------------------------
// ■ コンスタントカラー

/**
 * @brief コンスタントカラー設定
 *
 * @param materialIndex マテリアル番号
 * @param constantColorIndex コンスタントカラー番号
 * @param color カラー
 */
void BaseModel::SetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex, const gfl2::math::Vector4& color)
{
  this->GetBaseModelMaterial(materialIndex).SetConstantColor(constantColorIndex, color);
}

/**
 * @brief コンスタントカラー設定：要素毎
 *
 * @param materialIndex マテリアル番号
 * @param constantColorIndex コンスタントカラー番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModel::SetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->GetBaseModelMaterial(materialIndex).SetConstantColor(constantColorIndex, colorR, colorG, colorB, colorA);
}

/**
 * @brief コンスタントカラー取得
 *
 * @param materialIndex マテリアル番号
 * @param constantColorIndex コンスタントカラー番号
 *
 * @return コンスタントカラーを返す
 */
gfl2::math::Vector4 BaseModel::GetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetConstantColor(constantColorIndex);
}

//---------------------------------------------------------------------------------
// ■ フォグ

/**
 * @brief フォグの有効設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueで有効、falseで無効
 */
void BaseModel::SetMaterialFogEnable(s32 materialIndex, b32 enable)
{
  this->GetBaseModelMaterial(materialIndex).SetFogEnable(enable);
}

/**
 * @brief フォグが有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return enable trueで有効、falseで無効
 */
b32 BaseModel::IsMaterialFogEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsFogEnable();
}

/**
 * @brief フォグの有効設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueで有効、falseで無効
 */
void BaseModel::SetMaterialFogIndex(s32 materialIndex, s16 fogIndex)
{
  this->GetBaseModelMaterial(materialIndex).SetFogIndex(fogIndex);
}

/**
 * @brief フォグの有効設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueで有効、falseで無効
 */
s16 BaseModel::GetMaterialFogIndex(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetFogIndex();
}

//---------------------------------------------------------------------------------
// ■ ライトセット

/**
 * @brief フォグの有効設定
 *
 * @param materialIndex マテリアル番号
 * @param lightSetIndex trueで有効、falseで無効
 */
void BaseModel::SetMaterialLightSetIndex(s32 materialIndex, s16 lightSetIndex)
{
  this->GetBaseModelMaterial(materialIndex).SetLightSetIndex(lightSetIndex);
}

/**
 * @brief フォグが有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return trueで有効、falseで無効
 */
s16 BaseModel::GetMaterialLightSetIndex(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetLightSetIndex();
}

//---------------------------------------------------------------------------------
// ■ カリングモード

/**
 * @brief カリングモード設定
 *
 * @param materialIndex マテリアル番号
 * @param cullingMode カリングモード
 */
void BaseModel::SetMaterialCullingMode(s32 materialIndex, MaterialCullingMode cullingMode)
{
  this->GetBaseModelMaterial(materialIndex).SetCullingMode((gfl2::gfx::CullMode)cullingMode);
}

/**
 * @brief カリングモード取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return カリングモードを返す
 */
BaseModel::MaterialCullingMode BaseModel::GetMaterialCullingMode(s32 materialIndex) const
{
  return (MaterialCullingMode)this->GetBaseModelMaterial(materialIndex).GetCullingMode().Get();
}

//---------------------------------------------------------------------------------
// ■ アルファテスト

/**
 * @brief アルファテスト設定
 *
 * @param materialIndex マテリアル番号
 * @param enable tureなら有効, falseなら無効
 */
void BaseModel::SetMaterialAlphaTestEnable(s32 materialIndex, b32 enable)
{
  this->GetBaseModelMaterial(materialIndex).SetAlphaTestEnable(enable);  
}

/**
 * @brief アルファテスト有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return tureなら有効, falseなら無効
 */
b32 BaseModel::IsMaterialAlphaTestEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsAlphaTestEnable();
}

/**
 * @brief アルファテストの比較関数設定
 *
 * @param materialIndex マテリアル番号
 * @param testFunction 比較関数
 */
void BaseModel::SetMaterialAlphaTestFunction(s32 materialIndex, MaterialTestFunction testFunction)
{
  this->GetBaseModelMaterial(materialIndex).SetAlphaTestFunction((gfl2::gfx::CompareFunc)testFunction);
}

/**
 * @brief アルファテストの比較関数取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 比較関数を返す
 */
BaseModel::MaterialTestFunction BaseModel::GetMaterialAlphaTestFunction(s32 materialIndex) const
{
  return (MaterialTestFunction)this->GetBaseModelMaterial(materialIndex).GetAlphaTestFunction().Get();
}

/**
 * @brief アルファテストの参照値設定
 *
 * @param materialIndex マテリアル番号
 * @param reference 参照値
 */
void BaseModel::SetMaterialAlphaTestReference(s32 materialIndex, f32 reference)
{
  this->GetBaseModelMaterial(materialIndex).SetAlphaTestReference(reference);
}

/**
 * @brief アルファテストの参照値取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 参照値を返す
 */
f32 BaseModel::GetMaterialAlphaTestReference(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetAlphaTestReference();
}

//---------------------------------------------------------------------------------
// ■ デプス書き込み

/**
 * @brief デプス書き込み設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueならデプスを更新する、 falseなら更新しない
 */
void BaseModel::SetMaterialDepthWriteEnable(s32 materialIndex, b32 enable)
{
  // trueのときデプスバッファを更新する
  this->GetBaseModelMaterial(materialIndex).SetDepthWriteEnable(enable);
}

/**
 * @brief デプス書き込み有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return tureなら有効、 falseなら無効
 */
b32 BaseModel::IsMaterialDepthWriteEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsDepthWriteEnable();
}

//---------------------------------------------------------------------------------
// ■ デプステスト

/**
 * @brief デプステスト設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModel::SetMaterialDepthTestEnable(s32 materialIndex, b32 enable)
{
  this->GetBaseModelMaterial(materialIndex).SetDepthTestEnable(enable);
}

/**
 * @brief デプステスト有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return tureなら有効、 falseなら無効
 */
b32 BaseModel::IsMaterialDepthTestEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsDepthTestEnable();
}

/**
 * @brief デプステスト比較関数設定
 *
 * @param materialIndex マテリアル番号
 * @param testFunction 比較関数
 */
void BaseModel::SetMaterialDepthTestFunction(s32 materialIndex, MaterialTestFunction testFunction)
{
  this->GetBaseModelMaterial(materialIndex).SetDepthTestFunction((gfl2::gfx::CompareFunc)testFunction);
}

/**
 * @brief デプステスト比較関数の取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 比較関数を返す
 */
BaseModel::MaterialTestFunction BaseModel::GetMaterialDepthTestFunction(s32 materialIndex) const
{
  return (MaterialTestFunction)this->GetBaseModelMaterial(materialIndex).GetDepthTestFunction().Get();
}

//---------------------------------------------------------------------------------
// ■ ステンシルテスト

/**
 * @brief ステンシルテスト設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModel::SetMaterialStencilTestEnable(s32 materialIndex, b32 enable)
{
  this->GetBaseModelMaterial(materialIndex).SetStencilTestEnable(enable);
}

/**
 * @brief ステンシルテスト有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return trueなら有効、 falseなら無効
 */
b32 BaseModel::IsMaterialStencilTestEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsStencilTestEnable();
}

/**
 * @brief ステンシルテスト比較関数設定：両面設定
 *
 * @param materialIndex マテリアル番号
 * @param testFunction 比較関数
 * @param reference 参照値
 * @param mask マスク値
 */
void BaseModel::SetMaterialStencilTestFunction(s32 materialIndex, MaterialTestFunction testFunction, u8 reference, u8 mask)
{
  this->GetBaseModelMaterial(materialIndex).SetStencilTestFunction((gfl2::gfx::CompareFunc)testFunction, reference, mask);
}

/**
 * @brief ステンシルテスト比較関数取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 比較関数を返す
 */
BaseModel::MaterialTestFunction BaseModel::GetMaterialStencilTestFunction(s32 materialIndex) const
{
  return (MaterialTestFunction)this->GetBaseModelMaterial(materialIndex).GetStencilTestFunction().Get();
}

/**
 * @brief ステンシルテスト参照値取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 参照値を返す
 */
u8 BaseModel::GetMaterialStencilTestReference(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetStencilTestReference();
}

/**
 * @brief ステンシルテストマスク値取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return マスク値を返す
 */
u8 BaseModel::GetMaterialStencilTestMask(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).GetStencilTestMask();
}

/**
 * @brief ステンシルテスト処理設定：両面設定
 *
 * @param materialIndex マテリアル番号
 * @param failOp ステンシルテスト通過できない場合の処理
 * @param depthFailOp デプステスト通過できない場合の処理
 * @param depthPassOp デプステスト通過時の処理
 */
void BaseModel::SetMaterialStencilTestOperation(s32 materialIndex, MaterialStencilOperation failOp, MaterialStencilOperation depthFailOp, MaterialStencilOperation depthPassOp)
{
  this->GetBaseModelMaterial(materialIndex).SetStencilTestOperation((gfl2::gfx::StencilOp)failOp, (gfl2::gfx::StencilOp)depthFailOp, (gfl2::gfx::StencilOp)depthPassOp);
}

/**
 * @brief ステンシルテスト：ステンシルテスト通過できない場合の処理の取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 処理を返す
 */
BaseModel::MaterialStencilOperation BaseModel::GetMaterialStencilTestFailOperation(s32 materialIndex) const
{
  return (MaterialStencilOperation)this->GetBaseModelMaterial(materialIndex).GetStencilTestFailOperation().Get();
}

/**
 * @brief ステンシルテスト：デプステスト通過できない場合の処理の取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 処理を返す
 */
BaseModel::MaterialStencilOperation BaseModel::GetMaterialStencilTestZFailOperation(s32 materialIndex) const
{
  return (MaterialStencilOperation)this->GetBaseModelMaterial(materialIndex).GetStencilTestZFailOperation().Get();
}

/**
 * @brief ステンシルテスト：デプステスト通過時の処理の取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return 処理を返す
 */
BaseModel::MaterialStencilOperation BaseModel::GetMaterialStencilTestPassOperation(s32 materialIndex) const
{
  return (MaterialStencilOperation)this->GetBaseModelMaterial(materialIndex).GetStencilTestPassOperation().Get();
}

//---------------------------------------------------------------------------------
// ■ ブレンド

/**
 * @brief ブレンド設定
 *
 * @param materialIndex マテリアル番号
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModel::SetMaterialBlendEnable(s32 materialIndex, b32 enable)
{
  this->GetBaseModelMaterial(materialIndex).SetBlendEnable(enable);
}

/**
 * @brief ブレンド有効か？
 *
 * @param materialIndex マテリアル番号
 *
 * @return trueなら有効、 falseなら無効
 */
b32 BaseModel::IsMaterialBlendEnable(s32 materialIndex) const
{
  return this->GetBaseModelMaterial(materialIndex).IsBlendEnable();
}

/**
 * @brief ブレンド関数設定
 *
 * @param materialIndex マテリアル番号
 * @param srcRgb ソースＲＧＢ
 * @param dstRgb ディスティネーションＲＧＢ
 * @param srcAlpha ソースα
 * @param dstAlpha ディスティネーションα
 */
void BaseModel::SetMaterialBlendFunction(s32 materialIndex, MaterialBlendFactor srcRgb, MaterialBlendFactor dstRgb, MaterialBlendFactor srcAlpha, MaterialBlendFactor dstAlpha)
{
  this->GetBaseModelMaterial(materialIndex).SetBlendFunction((gfl2::gfx::BlendFunc)srcRgb, (gfl2::gfx::BlendFunc)dstRgb, (gfl2::gfx::BlendFunc)srcAlpha, (gfl2::gfx::BlendFunc)dstAlpha);
}

/**
 * @brief ブレンド関数：ソースＲＧＢ取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return ソースＲＧＢを返す
 */
BaseModel::MaterialBlendFactor BaseModel::GetMaterialBlendFunctionSourceRgb(s32 materialIndex) const
{
  return (MaterialBlendFactor)this->GetBaseModelMaterial(materialIndex).GetBlendFunctionSourceRgb().Get();
}

/**
 * @brief ブレンド関数：ソースα取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return ソースαを返す
 */
BaseModel::MaterialBlendFactor BaseModel::GetMaterialBlendFunctionSourceAlpha(s32 materialIndex) const
{
  return (MaterialBlendFactor)this->GetBaseModelMaterial(materialIndex).GetBlendFunctionSourceAlpha().Get();
}

/**
 * @brief ブレンド関数：ディスティネーションＲＧＢ取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return ディスティネーションＲＧＢを返す
 */
BaseModel::MaterialBlendFactor BaseModel::GetMaterialBlendFunctionDestinationRgb(s32 materialIndex) const
{
  return (MaterialBlendFactor)this->GetBaseModelMaterial(materialIndex).GetBlendFunctionDestinationRgb().Get();
}

/**
 * @brief ブレンド関数：ディスティネーションα取得
 *
 * @param materialIndex マテリアル番号
 *
 * @return ディスティネーションαを返す
 */
BaseModel::MaterialBlendFactor BaseModel::GetMaterialBlendFunctionDestinationAlpha(s32 materialIndex) const
{
  return (MaterialBlendFactor)this->GetBaseModelMaterial(materialIndex).GetBlendFunctionDestinationAlpha().Get();
}

/**
 * @brief ブレンド式設定
 *
 * @param materialIndex マテリアル番号
 * @param colorEq カラー式
 * @param alphaEq α式
 */
void BaseModel::SetMaterialBlendEquation(s32 materialIndex, MaterialBlendEquation colorEq, MaterialBlendEquation alphaEq)
{
  this->GetBaseModelMaterial(materialIndex).SetBlendEquation((gfl2::gfx::BlendOp)colorEq, (gfl2::gfx::BlendOp)alphaEq);
}

/**
 * @brief ブレンド式取得：カラー
 *
 * @param materialIndex マテリアル番号
 *
 * @return カラーの式を返す
 */
BaseModel::MaterialBlendEquation BaseModel::GetMaterialBlendEquationRgb(s32 materialIndex) const
{
  return (MaterialBlendEquation)this->GetBaseModelMaterial(materialIndex).GetBlendEquationRgb().Get();
}

/**
 * @brief ブレンド式取得：α
 *
 * @param materialIndex マテリアル番号
 *
 * @return αの式を返す
 */
BaseModel::MaterialBlendEquation BaseModel::GetMaterialBlendEquationAlpha(s32 materialIndex) const
{
  return (MaterialBlendEquation)this->GetBaseModelMaterial(materialIndex).GetBlendEquationAlpha().Get();
}

//---------------------------------------------------------------------------------
// ■ テクスチャマッパー

/**
 * @brief テクスチャの差し替え
 *
 * @param materialIndex マテリアル番号
 * @param no マッパー番号
 * @param pTextureResourceNode 差し替え先のテクスチャリソース
 */
void BaseModel::SetMaterialTexture(s32 materialIndex, s32 no, const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResourceNode)
{
  this->GetBaseModelMaterial(materialIndex).SetTexture(no, pTextureResourceNode);
}

#if MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
//@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
//                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
//                使う側に混乱が起きないよう、nijiでは封印します。
//                本来は実装されるべき機能です。

/**
 * @brief ＵＶのラップ方法設定
 *
 * @param materialIndex マテリアル番号
 * @param no マッパー番号
 * @param wrapU Ｕ方向の設定
 * @param wrapV Ｖ方向の設定
 */
void BaseModel::SetMaterialTextureMapperWrap(s32 materialIndex, s32 no, MaterialTextureWrap wrapU, MaterialTextureWrap wrapV)
{
  this->GetBaseModelMaterial(materialIndex).SetTextureMapperWrap(no, (gfl2::gfx::TextureAddress)wrapU, (gfl2::gfx::TextureAddress)wrapV);
}

/**
 * @brief ＵＶのラップ方法設定取得：Ｕ方向
 *
 * @param materialIndex マテリアル番号
 * @param no マッパー番号
 *
 * @return Ｕ方向のラップ設定を返す
 */
BaseModel::MaterialTextureWrap BaseModel::GetMaterialTextureMapperWrapU(s32 materialIndex, s32 no) const
{
  return (MaterialTextureWrap)this->GetBaseModelMaterial(materialIndex).GetTextureMapperWrapU(no).Get();
}

/**
 * @brief ＵＶのラップ方法設定取得：Ｖ方向
 *
 * @param materialIndex マテリアル番号
 * @param no マッパー番号
 *
 * @return Ｖ方向のラップ設定を返す
 */
BaseModel::MaterialTextureWrap BaseModel::GetMaterialTextureMapperWrapV(s32 materialIndex, s32 no) const
{
  return (MaterialTextureWrap)this->GetBaseModelMaterial(materialIndex).GetTextureMapperWrapV(no).Get();
}

#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE

//---------------------------------------------------------------------------------
// ■ テクスチャコーディネーター

/**
 * @brief ＵＶのスケール設定
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 * @param scaleUv スケール値
 */
void BaseModel::SetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 no, const gfl2::math::Vector2& scaleUv)
{
  this->GetBaseModelMaterial(materialIndex).SetTextureCoordinatorScaleUV(no, scaleUv);
}

/**
 * @brief ＵＶのスケール取得
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 *
 * @return ＵＶのスケールを返す
 */
gfl2::math::Vector2 BaseModel::GetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 no) const
{
  return this->GetBaseModelMaterial(materialIndex).GetTextureCoordinatorScaleUV(no);
}

/**
 * @brief ＵＶの回転設定
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 * @param rotateRad 回転値
 */
void BaseModel::SetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 no, const f32 rotateRad)
{
  this->GetBaseModelMaterial(materialIndex).SetTextureCoordinatorRotateUV(no, rotateRad);
}

/**
 * @brief ＵＶの回転取得
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 *
 * @return 回転値を返す
 */
f32 BaseModel::GetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 no) const
{
  return this->GetBaseModelMaterial(materialIndex).GetTextureCoordinatorRotateUV(no);
}

/**
 * @brief ＵＶの平行移動成分設定
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 * @param transUv 平行移動成分
 */
void BaseModel::SetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 no, const gfl2::math::Vector2& transUv)
{
  this->GetBaseModelMaterial(materialIndex).SetTextureCoordinatorTranslateUV(no, transUv);
}

/**
 * @brief ＵＶの平行移動成分取得
 *
 * @param materialIndex マテリアル番号
 * @param no コーディネーター番号
 *
 * @return 平行移動値を返す
 */
gfl2::math::Vector2 BaseModel::GetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 no) const
{
  return this->GetBaseModelMaterial(materialIndex).GetTextureCoordinatorTranslateUV(no);
}

//---------------------------------------------------------------------------------
// ■ アニメーション

/**
 * @brief アニメーションコントローラーを持っているか？
 *
 * @return true:持っている false:持っていない
 */
bool BaseModel::HasAnimationController() const
{
  return m_pAnimationController != NULL;
}

/**
 * @brief アニメーションの更新
 *
 * @param animationBit アニメーションの種類
 */
f32 BaseModel::UpdateAnimation(AnimationBit animationBit )
{
  f32 returnValue = this->GetBaseModelAnimation(0, animationBit).UpdateAnimation();

  // 視線制御に書き換えられる前のアニメーション直後のSRTを保存
  this->StoreJointSRT();

  return returnValue;
}

/**
 * @brief アニメーションの更新
 *
 * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
 * @param animationBit アニメーションの種類
 */
f32 BaseModel::UpdateAnimationManual(f32 interpWeight, AnimationBit animationBit)
{
  return this->GetBaseModelAnimation(0, animationBit).UpdateAnimationManual(interpWeight);
}

/**
 * @brief 現在のアニメーションリソースノードを取得
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return アニメーションリソースノードを返す、設定されていなければNULLを返す
 */
const gfl2::renderingengine::scenegraph::resource::ResourceNode* BaseModel::GetAnimationResourceNode(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetResourceNode();
}

/**
 * @brief アニメーションの変更：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).Change(pResourceNode, reinterpret_cast<const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(m_pModelInstanceNode->GetMotionConfigInstanceNode()));
}

/**
 * @brief アニメーションの変更：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 * @param changeFrame 補間フレーム数
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).ChangeSmooth(pResourceNode, changeFrame);
}

/**
 * @brief アニメーションの変更：補間率の手動設定版：リソースノード指定版
 *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
 *
 * @param pResourceNode0 リソースノード0
 * @param pResourceNode1 リソースノード1
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationSmoothManualByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode0, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode1, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).ChangeSmoothManual(pResourceNode0, pResourceNode1);
}

/**
 * @brief アニメーション補間中か？：ChangeAnimationSmoothの終了判定用
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 補間中ならtrue, 補間中でなければfalse
 */
bool BaseModel::IsAnimationInterp(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).IsAnimationInterp();
}

/**
 * @brief アニメーションパックリストの設定
 *
 * @param pAnimationPackList セットするアニメーションパックリスト
 */
void BaseModel::SetAnimationPackList(gfl2::animation::AnimationPackList* pAnimationPackList)
{
  m_pAnimationPackList = pAnimationPackList;
}

/**
 * @brief アニメーションパックリストの取得
 *
 * @return セットされているアニメーションパックリスト
 */
const gfl2::animation::AnimationPackList* BaseModel::GetAnimationPackList() const
{
  return m_pAnimationPackList;
}

/**
 * @brief アニメーション再生用のメモリを明示的に確保する
 *
 * @param pAnimationHeap アニメーションヒープ
 * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::AllocateAnimation(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, u32 animationId, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);
  this->AllocateAnimationByResourceNode(pAnimationHeap, smoothFlag, m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId), slotIndex, animationBit);
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
void BaseModel::AllocateAnimationByResourceNode(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).Allocate(pAnimationHeap, smoothFlag, pResourceNode);
}

/**
 * @brief 現在のアニメーションIDを取得する
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return アニメーションIDを返す、設定されていなければ-1を返す
 */
s32 BaseModel::GetAnimationId(u32 slotIndex, AnimationBit animationBit) const
{
  s32 animationId = -1;

  for (u32 i = 0; i < BIT_CACHE_CNT_FOR_ANIMATION_ID; i++)
  {
    if (animationBit & (1 << i))
    {
      animationId = m_animationIdCache[slotIndex][i];
      break;
    }
  }

  return animationId;
}

/**
 * @brief デフォルトのブレンドフレーム数取得
 *
 * @param animationId アニメーションパック内のID
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
 */
u32 BaseModel::GetAnimationDefaultBlendFrame(u32 animationId, u32 slotIndex, AnimationBit animationBit) const
{
  gfl2::animation::PackedAnimationId                         packedAnimationId(animationId);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId));

  return this->GetAnimationDefaultBlendFrameByResourceNode(pResourceNode, slotIndex, animationBit);
}

/**
 * @brief ブレンド機能有効無効設定
 *
 * @param enable true:ブレンド機能有効 false:ブレンド機能無効、以降のSmooth系関数は全て補間無しに設定
 */
void BaseModel::SetAnimationBlendEnable(bool enable)
{
  gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode = 
    reinterpret_cast<gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(m_pModelInstanceNode->GetMotionConfigInstanceNode());

  if (pMotionConfigNode == NULL)
  {
    return;
  }

  pMotionConfigNode->SetBlendEnable(enable);
}

/**
 * @brief ブレンド機能有効か？
 *
 * @return true:ブレンド機能有効 false:ブレンド機能無効、以降のSmooth系関数は全て補間無しに設定
 */
bool BaseModel::IsAnimationBlendEnable() const
{
  const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode = 
    reinterpret_cast<const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(m_pModelInstanceNode->GetMotionConfigInstanceNode());

  if (pMotionConfigNode == NULL)
  {
    return false;
  }

  return pMotionConfigNode->IsBlendEnable();
}

/**
 * @brief デフォルトのブレンドフレーム数取得：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
 */
u32 BaseModel::GetAnimationDefaultBlendFrameByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetDefaultBlendFrame(pResourceNode, reinterpret_cast<const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode*>(m_pModelInstanceNode->GetMotionConfigInstanceNode()));
}

/**
 * @brief アニメーションの変更：バインドポーズに戻す
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationBindpose(u32 slotIndex, AnimationBit animationBit)
{
  this->ChangeAnimationByResourceNode(NULL, slotIndex, animationBit);

  for (u32 i = 0; i < BIT_CACHE_CNT_FOR_ANIMATION_ID; i++)
  {
    if (animationBit & (1 << i))
    {
      m_animationIdCache[slotIndex][i] = -1;
    }
  }
}

/**
 * @brief アニメーションの変更：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimation(u32 animationId, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::PackedAnimationId                         packedAnimationId(animationId);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId));
  this->ChangeAnimationByResourceNode(pResourceNode, slotIndex, animationBit);

  s32 animationIdCache = pResourceNode ? (s32)animationId : (s32)-1;
  for (u32 i = 0; i < BIT_CACHE_CNT_FOR_ANIMATION_ID; i++)
  {
    if (animationBit & (1 << i))
    {
      m_animationIdCache[slotIndex][i] = animationIdCache;
    }
  }
}

/**
* @brief 指定アニメーションデータが存在するか取得
*
* @param animationId アニメーションパック内のID
*
* @return true : ある  false : ない
*/
b32 BaseModel::IsAnimationExist(u32 animationId) const
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);

  if( packedAnimationId.packId >= m_pAnimationPackList->GetAnimationPackCount() )
  {
    return false;
  }

  if( packedAnimationId.animationId >= m_pAnimationPackList->GetResourceCount( packedAnimationId.packId ) )
  {
    return false;
  }

  return true;
}

/**
 * @brief アニメーションの変更：補間あり：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param changeFrame 補間フレーム数
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationSmooth(u32 animationId, u32 changeFrame, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::PackedAnimationId                         packedAnimationId(animationId);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId));
  this->ChangeAnimationSmoothByResourceNode(pResourceNode, changeFrame, slotIndex, animationBit);

  s32 animationIdCache = pResourceNode ? (s32)animationId : (s32)-1;
  for (u32 i = 0; i < BIT_CACHE_CNT_FOR_ANIMATION_ID; i++)
  {
    if (animationBit & (1 << i))
    {
      m_animationIdCache[slotIndex][i] = animationIdCache;
    }
  }
}

/**
 * @brief アニメーションの変更：補間率の手動設定版、ビューアー用に用意：パック内のID指定版
 *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
 *
 * @param animationId0 アニメーションパック内のID0
 * @param animationId1 アニメーションパック内のID1
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::ChangeAnimationSmoothManual(u32 animationId0, u32 animationId1, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::PackedAnimationId packedAnimationId0(animationId0);
  gfl2::animation::PackedAnimationId packedAnimationId1(animationId1);

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode0(m_pAnimationPackList->GetResourceNode(packedAnimationId0.packId, packedAnimationId0.animationId));
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode1(m_pAnimationPackList->GetResourceNode(packedAnimationId1.packId, packedAnimationId1.animationId));

  this->ChangeAnimationSmoothManualByResourceNode(
    pResourceNode0,
    pResourceNode1,
    slotIndex,
    animationBit);

  s32 animationIdCache = pResourceNode0 ? (s32)animationId0 : (s32)-1;
  for (u32 i = 0; i < BIT_CACHE_CNT_FOR_ANIMATION_ID; i++)
  {
    if (animationBit & (1 << i))
    {
      m_animationIdCache[slotIndex][i] = animationIdCache;
    }
  }
}

/**
 * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
 *
 * @param enable trueの場合はループする、 falseの場合はループしない
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetAnimationLoop(b32 enable, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetLoop(enable);
}

/**
 * @brief ループ再生されるか？
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return trueの場合はループする、 falseの場合はループしない
 */
b32 BaseModel::IsAnimationLoop(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).IsLoop();
}

/**
 * @brief フレーム設定
 *
 * @param frame フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetAnimationFrame(f32 frame, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetFrame(frame);
}

/**
 * @brief フレーム取得
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 現在のフレームを返す
 */
f32 BaseModel::GetAnimationFrame(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetFrame();
}

/**
 * @brief 再生速度設定
 *
 * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetAnimationStepFrame(f32 stepFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetStepFrame(stepFrame);
}

/**
 * @brief 再生速度取得
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 再生速度を返す
 */
f32 BaseModel::GetAnimationStepFrame(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetStepFrame();
}

/**
 * @brief ループの開始フレーム設定
 *
 * @param startFrame 開始フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetAnimationStartFrame(f32 startFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetStartFrame(startFrame);
}

/**
 * @brief ループの開始フレーム取得
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return 開始フレームを返す
 */
f32 BaseModel::GetAnimationStartFrame(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetStartFrame();
}

/**
 * @brief ループの終了フレーム設定
 *
 * @param endFrame 終了フレーム
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetAnimationEndFrame(f32 endFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetEndFrame(endFrame);
}

/**
 * @brief ループの終了フレーム取得
 *
 * @param endFrame 終了フレーム
 * @param animationBit アニメーションの種類
 *
 * @return 終了フレームを返す
 */
f32 BaseModel::GetAnimationEndFrame(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).GetEndFrame();
}

/**
 * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 *
 * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
 */
b32 BaseModel::IsAnimationLastFrame(u32 slotIndex, AnimationBit animationBit) const
{
  return this->GetBaseModelAnimation(slotIndex, animationBit).IsLastFrame();
}

/**
 * @brief キーアニメーションのコールバックを設定
 *
 * @param pUserData ユーザーデータ
 * @param pCallback コールバック
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SetKeyAnimationCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback, u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SetKeyCallback(pUserData, pCallback);
}

/**
 * @brief アニメーションパックに含まれるリソース数を返す
 *
 * @param packIndex パック番号
 *
 * @return リソース数。読み込み完了前なら0を返す
 */
u32 BaseModel::GetAnimationPackResouceCount( u32 packIndex)
{
  if( packIndex >= m_pAnimationPackList->GetAnimationPackCount())
  {
    return 0;
  }
  return m_pAnimationPackList->GetResourceCount( packIndex);
}

/**
 * @brief 操作対象を補間スロットに切り替える：ビューアー用
 *
 * @param isTypeInterp 操作対象が補間スロットか？
 */
void BaseModel::SetAnimationControllTargetInterp(bool isTargetInterp)
{
  this->GetBaseModelAnimation(0, ANIMATION_BIT_ALL).SetControllTargetInterp(isTargetInterp);
}

/**
 * @brief 歩き用速度を取得：ポケモン用の拡張データ
 * @param interpWeight 補間率
 * @return 移動速度ベクトル
 */
gfl2::math::Vector3 BaseModel::GetWalkSpeed(f32 interpWeight) const
{
  return m_pAnimationController->GetWalkSpeedSmooth(interpWeight);
}

/**
 * @brief 総ポリゴン数取得
 * @return u32 総ポリゴン数
 */
u32 BaseModel::GetPolygonCount() const
{
  return m_pModelInstanceNode->GetTotalPolygonNum();
}

/**
 * @brief 総頂点数取得
 * @return u32 総頂点数
 */
u32 BaseModel::GetVertexCount() const
{
  return m_pModelInstanceNode->GetTotalVertexNum();
}

/**
 * @brief テクスチャ数取得
 * @return u32 テクスチャ数
 */
u32 BaseModel::GetTextureCount() const
{
  return m_pModelInstanceNode->GetTextureNum();
}

/**
 * @brief 補間用スロットのフレームと再生速度の同期を取る：ビューアー用
 *
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類
 */
void BaseModel::SyncAnimationFrameAndStepFrame(u32 slotIndex, AnimationBit animationBit)
{
  this->GetBaseModelAnimation(slotIndex, animationBit).SyncFrameAndStepFrame();
}

/**
 * @brief アニメーション計算有効フラグの設定
 *
 * @param enable true:計算有効, false:計算無効
 */
void BaseModel::SetAnimationCalculateEnable(bool enable)
{
  if (m_pAnimationController == NULL)
  {
    return;
  }

  m_pAnimationController->SetCalculateEnable(enable);
}

/**
 * @brief アニメーション計算有効フラグの取得
 *
 * @return true:計算有効, false:計算無効
 */
bool BaseModel::IsAnimationCalculateEnable() const
{
  if (m_pAnimationController == NULL)
  {
    return true;
  }

  return m_pAnimationController->IsCalculateEnable();
}

/**
 * @brief フレーム更新有効フラグの設定
 *
 * @param enable true:更新有効, false:更新無効
 */
void BaseModel::SetAnimationUpdateFrameEnable(bool enable)
{
  if (m_pAnimationController == NULL)
  {
    return;
  }

  m_pAnimationController->SetUpdateFrameEnable(enable);
}

/**
 * @brief フレーム更新有効フラグの取得
 *
 * @return true:更新有効, false:更新無効
 */
bool BaseModel::IsAnimationUpdateFrameEnable() const
{
  if (m_pAnimationController == NULL)
  {
    return true;
  }

  return m_pAnimationController->IsUpdateFrameEnable();
}

/**
 * @brief 注視処理初期化
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pMotionConfigResource モーションコンフィグリソース
 */
void BaseModel::InitializeInterest(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource)
{
  // 無効な引数
  if (pMotionConfigResource == NULL)
  {
    return;
  }

  // 既に作成済みなら削除
  instance::InstanceNode* pOldMotionConfigInstanceNode = m_pModelInstanceNode->GetMotionConfigInstanceNode();
  GFL_SAFE_DELETE(pOldMotionConfigInstanceNode);
  m_pModelInstanceNode->SetMotionConfigInstanceNode(NULL);
  GFL_SAFE_DELETE(m_pInterestController);


  // モーションコンフィグインスタンス作成
  instance::MotionConfigInstanceNode* pMotionConfigInstanceNode = instance::InstanceCreator::CreateMotionConfigInstanceNode(pGfxAllocator, pMotionConfigResource);
  m_pModelInstanceNode->SetMotionConfigInstanceNode(pMotionConfigInstanceNode);

  // 視線コントローラー初期化
  m_pInterestController = GFL_NEW(m_pAnimationHeap) gfl2::animation::InterestController();
  m_pInterestController->Initialize(m_pAnimationHeap, m_pModelInstanceNode, pMotionConfigInstanceNode);
}

/**
 * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
 */
void BaseModel::UpdateInterest()
{
  m_pInterestController->Update();
}

/**
 * @brief 注視処理終了確認
 *
 * @return true:終了 false:更新中
 */
bool BaseModel::IsFinishInterest() const
{
  return m_pInterestController->IsFinish();
}

/**
 * @brief 注視位置設定
 *
 * @param pPosition 注視位置：NULLなら終了
 */
void BaseModel::SetInterestPosition(const gfl2::math::Vector3* pPosition)
{
  m_pInterestController->SetInterestPosition(pPosition);
}

/**
 * @brief 注視用のジョイント数取得
 *
 * @return 注目用ジョイント数
 */
u32 BaseModel::GetInterestJointCount() const
{
  return m_pInterestController->GetInterestJointCount();
}

/**
 * @brief 注目タイプ設定
 *
 * @param index インデックス
 * @param type 注目タイプ
 */
void BaseModel::SetInterestType(u32 index, gfl2::renderingengine::InterestType type)
{
  m_pInterestController->SetType(index, type);
}

/**
 * @brief 注目タイプ取得
 *
 * @param index インデックス
 *
 * @return 注目タイプを返す
 */
gfl2::renderingengine::InterestType BaseModel::GetInterestType(u32 index) const
{
  return m_pInterestController->GetType(index);
}

/**
 * @brief 注目速度設定
 *
 * @param index インデックス
 * @param speed 注目速度
 */
void BaseModel::SetInterestSpeed(u32 index, f32 speed)
{
  m_pInterestController->SetSpeed(index, speed);
}

/**
 * @brief 注目速度取得
 *
 * @param index インデックス
 *
 * @return 注目速度を返す
 */
f32 BaseModel::GetInterestSpeed(u32 index) const
{
  return m_pInterestController->GetSpeed(index);
}

/**
 * @brief 角度制限：最小値設定（ラジアン）
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void BaseModel::SetInterestLimitAngleLow(u32 index, f32 low)
{
  m_pInterestController->SetLow(index, low);
}

/**
 * @brief 角度制限：最小値取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値を返す
 */
f32 BaseModel::GetInterestLimitAngleLow(u32 index) const
{
  return m_pInterestController->GetLow(index);
}

/**
 * @brief 角度制限：最大値設定（ラジアン）
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void BaseModel::SetInterestLimitAngleHigh(u32 index, f32 high)
{
  m_pInterestController->SetHigh(index, high);
}

/**
 * @brief 角度制限：最大値取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値を返す
 */
f32 BaseModel::GetInterestLimitAngleHigh(u32 index) const
{
  return m_pInterestController->GetHigh(index);
}

/**
 * @brief 角度制限：最小値２設定（ラジアン）
 *
 * @param index インデックス
 * @param low 角度制限：最小値
 */
void BaseModel::SetInterestLimitAngleLow2(u32 index, f32 low)
{
  m_pInterestController->SetLow2(index, low);
}

/**
 * @brief 角度制限：最小値２取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最小値２を返す
 */
f32 BaseModel::GetInterestLimitAngleLow2(u32 index) const
{
  return m_pInterestController->GetLow2(index);
}

/**
 * @brief 角度制限：最大値２設定（ラジアン）
 *
 * @param index インデックス
 * @param high 角度制限：最大値
 */
void BaseModel::SetInterestLimitAngleHigh2(u32 index, f32 high)
{
  m_pInterestController->SetHigh2(index, high);
}

/**
 * @brief 角度制限：最大値２取得（ラジアン）
 *
 * @param index インデックス
 *
 * @return 角度制限：最大値２を返す
 */
f32 BaseModel::GetInterestLimitAngleHigh2(u32 index) const
{
  return m_pInterestController->GetHigh2(index);
}

/**
 * @brief 注目解除にかかるフレーム数を設定
 *
 * @param index インデックス
 * @param resetFrameCount 注目解除にかかるフレーム数
 */
void BaseModel::SetInterestResetFrameCount(u32 index, s32 resetFrameCount)
{
  m_pInterestController->SetResetFrameCount(index, resetFrameCount);
}

/**
 * @brief 注目解除にかかるフレーム数を取得
 *
 * @param index インデックス
 *
 * @return 注目解除にかかるフレーム数を返す
 */
s32 BaseModel::GetInterestResetFrameCount(u32 index) const
{
  return m_pInterestController->GetResetFrameCount(index);
}

/**
 * 顔向け補正の有効無効設定
 * @param isEnabled true:補正有効, false:補正無効
 */
void BaseModel::SetInterestAdjustAngleEnable(bool isEnabled)
{
  m_pInterestController->SetAdjustAngleEnable(isEnabled);
}

/**
 * 顔向け補正の有効無効取得
 */
bool BaseModel::IsInterestAdjustAngleEnabled() const
{
  return m_pInterestController->IsAdjustAngleEnabled();
}

/**
 * 顔向け補正角度を設定
 * @param tagAdjustAngle　顔向け補正角度（ラジアン）
 */
void BaseModel::SetInterestTagAdjustAngle(f32 tagAdjustAngle)
{
  m_pInterestController->SetTagAdjustAngle(tagAdjustAngle);
}

/**
 * 顔向け補正角度をリセット
 */
void BaseModel::ResetInterestTagAdjustAngle()
{
  m_pInterestController->ResetTagAdjustAngle();
}

/**
 * 顔向け補正角度を取得
 */
f32 BaseModel::GetInterestTagAdjustAngle() const
{
  return m_pInterestController->GetTagAdjustAngle();
}

/**
 * 顔向け傾きを設定
 * @param tagDecline　顔向け傾き（ラジアン）
 */
void BaseModel::SetInterestTagDecline(f32 tagDecline)
{
  m_pInterestController->SetTagDecline(tagDecline);
}

/**
 * 顔向け補間用バネ係数を指定
 * @param adjustSpringVal バネ係数(初期値0.050f)
 * @param DeclineSpringVal バネ係数(初期値0.150f)
 */
void BaseModel::SetInterestSpringVal( f32 adjustSpringVal, f32 declineSpringVal )
{
  m_pInterestController->SetSpringVal( adjustSpringVal, declineSpringVal );
}

/**
 * 顔向け傾きを取得
 */
f32 BaseModel::GetInterestTagDecline( ) const
{
  return m_pInterestController->GetTagDecline();
}

/**
 * @brief バネ制御更新
 */
void BaseModel::UpdateSpring()
{
  m_pSpringController->Update();
}

// @fix GFNMCat[879]
/**
 * @brief 保存用のSRT構造体の確保と初期化
 */
void BaseModel::InitializeJointSRTPair()
{
  // アニメーションコントローラー、視線コントローラーが作成されていないなら終了
  if (m_pAnimationController == NULL)
  {
    return;
  }
  if (m_pInterestController == NULL)
  {
    return;
  }

  // 視線制御されるジョイントが無いなら終了
  u32 interestJointCount = m_pInterestController->GetInterestJointCount();
  if (interestJointCount == 0)
  {
    return;
  }

  // 視線制御されるジョイント数だけ保存用の領域を確保
  m_pStoreJointSRTPairList = GFL_NEW_ARRAY(m_pAnimationHeap) JointSRTPair[interestJointCount];

  // 視線制御対象のジョイントノードを取得しておく
  for (u32 i = 0; i < interestJointCount; ++i)
  {
    JointSRTPair& jointSrtPair = m_pStoreJointSRTPairList[i];
    jointSrtPair.pJointNode = m_pInterestController->GetInterestJointNode(i);
  }

  // 初期化用に一度保存しておく
  this->StoreJointSRT();

  // キャンセル時のコールバックを登録
  m_pAnimationController->SetInterpCancelCallback(&BaseModel::RestoreJointSRT, this);
}

// @fix GFNMCat[879]
/**
 * @brief SRTの保存
 */
void BaseModel::StoreJointSRT()
{
  // SRT保存用バッファが確保されていないなら終了
  if (m_pStoreJointSRTPairList == NULL)
  {
    return;
  }

  // 現在のローカルSRTを保存
  u32 interestJointCount = m_pInterestController->GetInterestJointCount();
  for (u32 i = 0; i < interestJointCount; ++i)
  {
    JointSRTPair& jointSrtPair = m_pStoreJointSRTPairList[i];
    if (jointSrtPair.pJointNode == NULL)
    {
      continue;
    }

    jointSrtPair.localSRT = jointSrtPair.pJointNode->GetLocalSRT();
  }
}

// @fix GFNMCat[879]
/**
 * @brief 保存していたSRTの復元
 */
void BaseModel::RestoreJointSRT(void* pWork)
{
  // ワークポインタが無いなら終了
  if (pWork == NULL)
  {
    return;
  }

  BaseModel* pBaseModel = reinterpret_cast<BaseModel*>(pWork);

  // SRT保存用バッファが確保されていないなら終了
  if (pBaseModel->m_pStoreJointSRTPairList == NULL)
  {
    return;
  }

  // ローカルSRTを復元
  u32 interestJointCount = pBaseModel->m_pInterestController->GetInterestJointCount();
  for (u32 i = 0; i < interestJointCount; ++i)
  {
    JointSRTPair& jointSrtPair = pBaseModel->m_pStoreJointSRTPairList[i];
    if (jointSrtPair.pJointNode == NULL)
    {
      continue;
    }

    jointSrtPair.pJointNode->SetLocalSRT(jointSrtPair.localSRT);
  }
}

}}
