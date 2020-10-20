#include "../include/gfl2_BaseCamera.h"

#include <Animation/include/gfl2_AnimationId.h>

using namespace gfl2::renderingengine::scenegraph;

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
BaseCamera::BaseCamera() :
  m_pDrawEnvNode(NULL),
  m_pCameraNode(NULL),
  m_CameraNo(0),
  m_pAnimationPackList(NULL),
  m_pAnimationHeap(NULL),
  m_AnimInterpMaxFrame(0),
  m_AnimInterpCurrentFrame(0),
  m_AnimMode(ANIM_MODE_NONE),
  m_AnimEaseFunc(gfl2::math::Easing::LINEAR),
  m_LocalAnimationScale( 1.0f ),
  m_TargetPosition( 0.0f, 0.0f, 0.0f ),
  m_UpVector( 0.0f, 1.0f, 0.0f ),
  m_NearClip(0),
  m_FarClip(0),
  m_Fovy(0),
  m_AspectRatio(0)
{
}

/**
 * @brief デストラクタ
 */
BaseCamera::~BaseCamera()
{
  // 念のため破棄
  this->Destroy();
}

/**
 * @brief 作成
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pDrawEnvNode 描画環境ノード
 * @param cameraNo カメラ番号
 *
 * @return 作成成功したらtrue, 失敗したらfalse
 */
bool BaseCamera::Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 cameraNo)
{
  if (pDrawEnvNode == NULL)
  {
    return false;
  }

  m_pAnimationHeap = pAnimationHeap;

  // カメラノード取得
  m_pCameraNode = pDrawEnvNode->GetCamera(cameraNo);
  m_CameraNo = cameraNo;

  // パラメータコピー
  this->CopyParam();

  // アニメーション初期化
  m_AnimationSlot.Initialize(m_pAnimationHeap, pDrawEnvNode);
  m_AnimMode = ANIM_MODE_NONE;
  m_AnimInterpCurrentFrame = 0;
  m_AnimInterpMaxFrame = 0;

  return true;
}
 
/**
 * @brief 破棄
 */
void BaseCamera::Destroy()
{
  m_AnimationSlot.Finalize();
}


//---------------------------------------------------------------------------------
// ■ カメラパラメータ

/**
 * @brief 現在の設定値から生成されるプロジェクション行列を取得する
 *
 * @return プロジェクション行列を返す
 */
gfl2::math::Matrix44 BaseCamera::GetProjectionMatrix() const
{
  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return gfl2::math::Matrix44::GetPerspective(
        m_Fovy,
        m_AspectRatio,
        m_NearClip,
        m_FarClip
        );
    }
  
  case ANIM_MODE_LOCAL:   // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_GLOBAL:  // グローバル再生　：　全てアニメーション結果で上書き
    {
      // プロジェクションに関してはどちらもアニメーションを優先する

      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        return gfl2::math::Matrix44::GetPerspective(
          gfl2::math::Lerp(m_Fovy, m_pCameraNode->GetFovy(), t),
          gfl2::math::Lerp(m_AspectRatio, m_pCameraNode->GetAspectRatio(), t),
          gfl2::math::Lerp(m_NearClip, m_pCameraNode->GetNear(), t),
          gfl2::math::Lerp(m_FarClip, m_pCameraNode->GetFar(), t)
          );
      }
      // 戻す補間
      return gfl2::math::Matrix44::GetPerspective(
        gfl2::math::Lerp(m_pCameraNode->GetFovy(), m_Fovy, t),
        gfl2::math::Lerp(m_pCameraNode->GetAspectRatio(), m_AspectRatio, t),
        gfl2::math::Lerp(m_pCameraNode->GetNear(), m_NearClip, t),
        gfl2::math::Lerp(m_pCameraNode->GetFar(), m_FarClip, t)
        );
    }
  }

  return gfl2::math::Matrix44();
}

static gfl2::math::Matrix34 MakeViewMatrix(const gfl2::math::Quaternion& rot, const gfl2::math::Vector3& pos)
{
  gfl2::math::Matrix34 viewMatrix;

  rot.QuaternionToMatrix(viewMatrix);
  viewMatrix.SetElem(0, 3, pos.x);
  viewMatrix.SetElem(1, 3, pos.y);
  viewMatrix.SetElem(2, 3, pos.z);
  return viewMatrix.Inverse();
}

/**
 * @brief 現在の設定値から生成されるビュー行列を取得する
 *
 * @return ビュー行列を返す
 */
gfl2::math::Matrix34 BaseCamera::GetViewMatrix() const
{
  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return MakeViewMatrix(m_Rotation, m_Position);
    }

  // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_LOCAL:
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        gfl2::math::Quaternion rot = m_Rotation;
        gfl2::math::Vector3 pos = m_Position;
        rot.Slerp(m_Rotation * m_pCameraNode->GetLocalRotation(), t * m_LocalAnimationScale );
        pos.Lerp(m_Position + m_pCameraNode->GetLocalTranslate(), t * m_LocalAnimationScale );

        return MakeViewMatrix(rot, pos);
      }

      // 戻す補間
      gfl2::math::Quaternion rot = m_Rotation * m_pCameraNode->GetLocalRotation();
      gfl2::math::Vector3 pos = m_Position + m_pCameraNode->GetLocalTranslate();
      rot.Slerp(m_Rotation, t);
      pos.Lerp(m_Position, t);

      return MakeViewMatrix(rot, pos);
    }

  // グローバル再生　：　全てアニメーション結果で上書き
  case ANIM_MODE_GLOBAL:
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        gfl2::math::Quaternion rot = m_Rotation;
        gfl2::math::Vector3 pos = m_Position;

        rot.Slerp(m_pCameraNode->GetLocalRotation(), t);
        pos.Lerp(m_pCameraNode->GetLocalTranslate(), t);

        return MakeViewMatrix(rot, pos);
      }

      // 戻す補間
      gfl2::math::Quaternion rot = m_pCameraNode->GetLocalRotation();
      gfl2::math::Vector3 pos = m_pCameraNode->GetLocalTranslate();
      rot.Slerp(m_Rotation, t);
      pos.Lerp(m_Position, t);

      return MakeViewMatrix(rot, pos);
    }
  }

  return gfl2::math::Matrix34();
}

/**
 * @brief ニアクリップ設定
 *
 * @param nearClip ニアクリップ
 */
void BaseCamera::SetNear(f32 nearClip)
{
  m_NearClip = nearClip;
}

/**
 * @brief ニアクリップ取得
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return ニアクリップを返す
 */
f32 BaseCamera::GetNear(bool blendFlag) const
{
  if (blendFlag == false)
  {
    return m_NearClip;
  }

  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_NearClip;
    }
  
  case ANIM_MODE_LOCAL:   // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_GLOBAL:  // グローバル再生　：　全てアニメーション結果で上書き
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        return gfl2::math::Lerp(m_NearClip, m_pCameraNode->GetNear(), t);
      }

      // 戻す補間
      return gfl2::math::Lerp(m_pCameraNode->GetNear(), m_NearClip, t);
    }
  }

  return m_NearClip;
}

/**
 * @brief ファークリップ設定
 *
 * @param farClip ファークリップ
 */
void BaseCamera::SetFar(f32 farClip)
{
  m_FarClip = farClip;
}

/**
 * @brief ファークリップ取得
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return ファークリップを返す
 */
f32 BaseCamera::GetFar(bool blendFlag) const
{
  if (blendFlag == false)
  {
    return m_FarClip;
  }

  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_FarClip;
    }
  
  case ANIM_MODE_LOCAL:   // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_GLOBAL:  // グローバル再生　：　全てアニメーション結果で上書き
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        return gfl2::math::Lerp(m_FarClip, m_pCameraNode->GetFar(), t);
      }

      // 戻す補間
      return gfl2::math::Lerp(m_pCameraNode->GetFar(), m_FarClip, t);
    }
  }

  return m_FarClip;
}

/**
 * @brief 垂直方向画角設定
 *
 * @param fovy 垂直方向画角
 */
void BaseCamera::SetFovy(f32 fovy)
{
  m_Fovy = fovy;
}

/**
 * @brief 垂直方向画角取得
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return 垂直方向画角を返す
 */
f32 BaseCamera::GetFovy(bool blendFlag) const
{
  if (blendFlag == false)
  {
    return m_Fovy;
  }

  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_Fovy;
    }
  
  case ANIM_MODE_LOCAL:   // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_GLOBAL:  // グローバル再生　：　全てアニメーション結果で上書き
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        return gfl2::math::Lerp(m_Fovy, m_pCameraNode->GetFovy(), t);
      }

      // 戻す補間
      return gfl2::math::Lerp(m_pCameraNode->GetFovy(), m_Fovy, t);
    }
  }

  return m_Fovy;
}

/**
 * @brief アスペクト比設定
 *
 * @param aspectRatio アスペクト比
 */
void BaseCamera::SetAspectRatio(f32 aspectRatio)
{
  m_AspectRatio = aspectRatio;
}

/**
 * @brief アスペクト比取得
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return アスペクト比を返す
 */
f32 BaseCamera::GetAspectRatio(bool blendFlag) const
{
  if (blendFlag == false)
  {
    return m_AspectRatio;
  }

  switch (m_AnimMode)
  {
  // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_AspectRatio;
    }
  
  case ANIM_MODE_LOCAL:   // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_GLOBAL:  // グローバル再生　：　全てアニメーション結果で上書き
    {
      f32 t = this->GetInterpFrame();

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        return gfl2::math::Lerp(m_AspectRatio, m_pCameraNode->GetAspectRatio(), t);
      }

      // 戻す補間
      return gfl2::math::Lerp(m_pCameraNode->GetAspectRatio(), m_AspectRatio, t);
    }
  }

  return m_AspectRatio;
}

/**
 * @brief 位置設定
 *
 * @param position 位置
 */
void BaseCamera::SetPosition(const gfl2::math::Vector3& position)
{
  m_Position = position;
}

/**
 * @brief 位置設定：要素毎
 *
 * @param positionX 位置Ｘ
 * @param positionY 位置Ｙ
 * @param positionZ 位置Ｚ
 */
void BaseCamera::SetPosition(f32 positionX, f32 positionY, f32 positionZ)
{
  this->SetPosition(gfl2::math::Vector3(positionX, positionY, positionZ));
}

/**
* @brief ターゲット位置設定
* @note  情報保存用のワークを更新するだけで、作成する行列には影響がない
*
* @param position 位置
*/
void BaseCamera::SetTargetPosition(const gfl2::math::Vector3& position)
{
  m_TargetPosition = position;
}

/**
 * @brief 位置取得
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return 位置を返す
 */
const gfl2::math::Vector3 BaseCamera::GetPosition(bool blendFlag) const
{
  if (blendFlag == false)
  {
    return m_Position;
  }

  switch (m_AnimMode)
  {
    // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_Position;
    }

    // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_LOCAL:
    {
      f32 t = this->GetInterpFrame();

      gfl2::math::Vector3 pos;

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        pos = m_Position;
        pos.Lerp(m_Position + m_pCameraNode->GetLocalTranslate(), t * m_LocalAnimationScale );

        return pos;
      }

      // 戻す補間
      pos = m_Position + m_pCameraNode->GetLocalTranslate();
      pos.Lerp(m_Position, t);

      return pos;
    }

    // グローバル再生　：　全てアニメーション結果で上書き
  case ANIM_MODE_GLOBAL:
    {
      f32 t = this->GetInterpFrame();

      gfl2::math::Vector3 pos;

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        pos = m_Position;

        pos.Lerp(m_pCameraNode->GetLocalTranslate(), t);
        return pos;
      }

      // 戻す補間
      pos = m_pCameraNode->GetLocalTranslate();
      pos.Lerp(m_Position, t);
      return pos;
    }
  }

  return m_Position;
}

/**
* @brief ターゲット位置取得
* @note  SetupCameraLookAt関数で設定されているときのみ有効
*
* @return 位置を返す
*/
const gfl2::math::Vector3 BaseCamera::GetTargetPosition() const
{
  return m_TargetPosition;
}

/**
 * @brief アップベクター取得
 * @note  SetupCameraLookAt関数で設定されているときのみ有効
 *
 * @return アップベクターを返す
 */
const gfl2::math::Vector3 BaseCamera::GetUpVector() const
{
  return m_UpVector;
}

/**
 * @brief 回転設定
 *
 * @param rotationRad 回転（ラジアン）
 * @param rotationOrder 回転順：デフォルトはZYX
 */
void BaseCamera::SetRotation(const gfl2::math::Vector3& rotationRad, gfl2::math::EulerRotation::RotationOrder rotationOrder)
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
void BaseCamera::SetRotation(f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ, gfl2::math::EulerRotation::RotationOrder rotationOrder)
{
  this->SetRotation(gfl2::math::Vector3(rotationRadX, rotationRadY, rotationRadZ), rotationOrder);
}

/**
 * @brief 回転取得
 *        ※ SetRotation以外で回転を設定した場合は見た目に合わない値が返ります
 *
 * @return 回転を返す
 */
gfl2::math::Vector3 BaseCamera::GetRotation() const
{
  gfl2::math::Vector3 euler(m_EulerRotation.x, m_EulerRotation.y, m_EulerRotation.z);
  return euler;
}

//---------------------------------------------------------------------------------
// ■ 回転の設定：クォータニオン

/**
 * @brief 回転設定：クォータニオン
 *
 * @param rotation 回転
 */
void BaseCamera::SetRotationQuat(const gfl2::math::Quaternion& rotation)
{
  m_Rotation = rotation;
}

/**
 * @brief 回転取得：クォータニオン
 *
 * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
 * @return 回転を返す
 */
const gfl2::math::Quaternion BaseCamera::GetRotationQuat( bool blendFlag ) const
{
  if (blendFlag == false)
  {
    return m_Rotation;
  }


  switch (m_AnimMode)
  {
    // アニメーションしない
  case ANIM_MODE_NONE:
    {
      return m_Rotation;
    }

    // ローカル再生　：　現在のパラメータ＋アニメーション結果
  case ANIM_MODE_LOCAL:
    {
      f32 t = this->GetInterpFrame();
      gfl2::math::Quaternion rot;

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        rot = m_Rotation;
        rot.Slerp(m_Rotation * m_pCameraNode->GetLocalRotation(), t * m_LocalAnimationScale );

        return rot;
      }

      // 戻す補間
      rot = m_Rotation * m_pCameraNode->GetLocalRotation();
      rot.Slerp(m_Rotation, t);

      return rot;
    }

    // グローバル再生　：　全てアニメーション結果で上書き
  case ANIM_MODE_GLOBAL:
    {
      f32 t = this->GetInterpFrame();
      gfl2::math::Quaternion rot;

      // 補間
      if (this->GetAnimationResourceNode() != NULL)
      {
        rot = m_Rotation;
        rot.Slerp(m_pCameraNode->GetLocalRotation(), t);

        return rot;
      }

      // 戻す補間
      rot = m_pCameraNode->GetLocalRotation();
      rot.Slerp(m_Rotation, t);

      return rot;
    }
  }

  return m_Rotation;
}

//---------------------------------------------------------------------------------
// ■ カメラ位置、回転の便利設定

/**
 * @brief カメラセットアップ：ルックアット
 *
 * @param cameraPosition カメラ位置
 * @param cameraTarget カメラ注視点
 * @param cameraUpVector カメラアップベクター
 */
void BaseCamera::SetupCameraLookAt(const gfl2::math::Vector3& cameraPosition, const gfl2::math::Vector3& cameraTarget, const gfl2::math::Vector3& cameraUpVector)
{
  gfl2::math::Vector3 v = cameraPosition - cameraTarget;

  gfl2::math::Vector3 zAxis = v.Normalize();
  gfl2::math::Vector3 xAxis = cameraUpVector.Cross(zAxis).Normalize();
  gfl2::math::Vector3 yAxis = zAxis.Cross(xAxis).Normalize();

  gfl2::math::Matrix34 rotMatrix(
    gfl2::math::Vector4(xAxis.x, yAxis.x, zAxis.x, 0.0f),
    gfl2::math::Vector4(xAxis.y, yAxis.y, zAxis.y, 0.0f),
    gfl2::math::Vector4(xAxis.z, yAxis.z, zAxis.z, 0.0f)
    );

  m_Rotation.MatrixToQuaternion(rotMatrix);
  m_Position = cameraPosition;
  m_TargetPosition = cameraTarget;
  m_UpVector = cameraUpVector;
}

//---------------------------------------------------------------------------------
// ■ アニメーション

/**
 * @brief アニメーションの更新
 */
void BaseCamera::UpdateAnimation()
{
  // アニメーション無しなら無視
  if (m_AnimMode == ANIM_MODE_NONE)
  {
    return;
  }
  
  // アニメーションの適用
  if (m_pCameraNode != NULL)
  {
    f32 valueList[gfl2::animation::CameraDataSet::INDEX_COUNT] =
    {
      // @fix GFNMCat[794] アニメーションのデフォルト値を現在の値にしておく
      m_NearClip,
      m_FarClip,
      m_Fovy,
      m_AspectRatio,

      m_pCameraNode->GetLocalRotation().x,
      m_pCameraNode->GetLocalRotation().y,
      m_pCameraNode->GetLocalRotation().z,

      m_pCameraNode->GetLocalTranslate().x,
      m_pCameraNode->GetLocalTranslate().y,
      m_pCameraNode->GetLocalTranslate().z,
    };

    m_AnimationSlot.ApplyCamera(valueList, m_CameraNo);

    m_pCameraNode->SetNear(valueList[gfl2::animation::CameraDataSet::CAM_NEAR]);
    m_pCameraNode->SetFar(valueList[gfl2::animation::CameraDataSet::CAM_FAR]);
    m_pCameraNode->SetFovy(valueList[gfl2::animation::CameraDataSet::CAM_FOVY]);
    m_pCameraNode->SetAspectRatio(valueList[gfl2::animation::CameraDataSet::CAM_ASPECT_RATIO]);
    gfl2::math::Quaternion rot;
    rot.MatrixToQuaternion(gfl2::math::Matrix34::GetRotationZYX(valueList[gfl2::animation::CameraDataSet::ROTATE_Z], valueList[gfl2::animation::CameraDataSet::ROTATE_Y], valueList[gfl2::animation::CameraDataSet::ROTATE_X]));
    m_pCameraNode->SetLocalRotation(rot);
    m_pCameraNode->SetLocalTranslate(gfl2::math::Vector3(valueList[gfl2::animation::CameraDataSet::TRANSLATE_X], valueList[gfl2::animation::CameraDataSet::TRANSLATE_Y], valueList[gfl2::animation::CameraDataSet::TRANSLATE_Z]));
  }

  // グローバルアニメーションの最終フレームなら値をコピーして再生終了
  if (!m_AnimationSlot.IsLoop() && m_AnimationSlot.IsLastFrame())
  {
    if (m_AnimMode == ANIM_MODE_GLOBAL &&
      m_pCameraNode != NULL ) // @fix cov_ctr[11800]NullPtrCheck
    {
      // パラメータコピー
      this->CopyParam();
    }

    // 再生終了
    m_AnimMode = ANIM_MODE_NONE;
    m_AnimationSlot.SetAnimationResource(NULL);
  }

  // アニメーションフレームの更新
  m_AnimationSlot.UpdateFrame();

  // 補間フレームを進める
  ++m_AnimInterpCurrentFrame;
  if (m_AnimInterpCurrentFrame >= m_AnimInterpMaxFrame)
  {
    m_AnimInterpCurrentFrame = m_AnimInterpMaxFrame;
  }
}

/**
* @brief アニメーションの終了
*/
void BaseCamera::EndAnimation()
{
  // アニメーション無しなら無視
  if (m_AnimMode == ANIM_MODE_NONE)
  {
    return;
  }

  // グローバルアニメーションなら値をコピーして再生終了
  if (m_AnimMode == ANIM_MODE_GLOBAL)
  {
    // パラメータコピー
    this->CopyParam();
  }

  // 再生終了
  m_AnimMode = ANIM_MODE_NONE;
  m_AnimationSlot.SetAnimationResource(NULL);
}

/**
 * @brief アニメーションパックリストの設定
 *
 * @param pAnimationPackList セットするアニメーションパックリスト
 */
void BaseCamera::SetAnimationPackList(gfl2::animation::AnimationPackList* pAnimationPackList)
{
  m_pAnimationPackList = pAnimationPackList;
}

/**
 * @brief アニメーションパックリストの取得
 *
 * @return セットされているアニメーションパックリスト
 */
const gfl2::animation::AnimationPackList* BaseCamera::GetAnimationPackList() const
{
  return m_pAnimationPackList;
}

/**
 * @brief 現在のアニメーションIDを取得する
 *
 * @return アニメーションIDを返す、設定されていなければ-1を返す
 */
s32 BaseCamera::GetAnimationId() const
{
  // アニメーションパックがセットされていないなら無視
  if (m_pAnimationPackList == NULL)
  {
    return -1;
  }

  // リソースノード取得
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode = this->GetAnimationResourceNode();
  if (pResourceNode == NULL)
  {
    return -1;
  }

  // リニアサーチ
  // @attention 速度が問題になるなら別の実装に変更
  for (u32 packNo = 0; packNo < m_pAnimationPackList->GetAnimationPackCount(); ++packNo)
  {
    for (u32 animationNo = 0; animationNo < m_pAnimationPackList->GetResourceCount(packNo); ++animationNo)
    {
      const gfl2::animation::AnimationPack* pAnimationPack = m_pAnimationPackList->GetAnimationPack(packNo);
      if (pResourceNode == pAnimationPack->GetResourceNode(animationNo))
      {
        return (packNo << 16) | animationNo;
      }
    }
  }

  // 見つからない
  return -1;
}

/**
 * @brief アニメーションの変更：ローカルアニメーション：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 */
void BaseCamera::ChangeAnimationLocal(u32 animationId)
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);
  this->ChangeAnimationLocalByResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId));
}

/**
 * @brief アニメーションの変更：ローカルアニメーション：補間あり：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param changeFrame 補間フレーム数
 * @param easeFunc イージング関数
 */
void BaseCamera::ChangeAnimationLocalSmooth(u32 animationId, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc)
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);
  this->ChangeAnimationLocalSmoothByResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId), changeFrame, easeFunc);
}

/**
 * @brief アニメーションの変更：グローバルアニメーション：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 */
void BaseCamera::ChangeAnimationGlobal(u32 animationId)
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);
  this->ChangeAnimationGlobalByResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId));
}

/**
 * @brief アニメーションの変更：グローバルアニメーション：補間あり：パック内のID指定版
 *
 * @param animationId アニメーションパック内のID
 * @param changeFrame 補間フレーム数
 * @param easeFunc イージング関数
 */
void BaseCamera::ChangeAnimationGlobalSmooth(u32 animationId, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc)
{
  gfl2::animation::PackedAnimationId packedAnimationId(animationId);
  this->ChangeAnimationGlobalSmoothByResourceNode(m_pAnimationPackList->GetResourceNode(packedAnimationId.packId, packedAnimationId.animationId), changeFrame, easeFunc);
}

/**
 * @brief 現在のアニメーションリソースノードを取得
 *
 * @return アニメーションリソースノードを返す、設定されていなければNULLを返す
 */
const gfl2::renderingengine::scenegraph::resource::ResourceNode* BaseCamera::GetAnimationResourceNode() const
{
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode = m_AnimationSlot.GetAnimationResource();
  if (pResourceNode == NULL)
  {
    return NULL;
  }

  return static_cast<const gfl2::renderingengine::scenegraph::resource::ResourceNode*>(pResourceNode->GetParent());
}

/**
 * @brief アニメーションの変更：ローカルアニメーション：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 */
void BaseCamera::ChangeAnimationLocalByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  this->ChangeAnimationLocalSmoothByResourceNode(pResourceNode, 0);
}

/**
 * @brief アニメーションの変更：ローカルアニメーション：補間あり：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 * @param changeFrame 補間フレーム数
 * @param easeFunc イージング関数
 */
void BaseCamera::ChangeAnimationLocalSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc)
{
  m_AnimMode = ANIM_MODE_LOCAL;
  m_AnimInterpCurrentFrame = 0;
  m_AnimInterpMaxFrame = changeFrame;
  m_AnimEaseFunc = easeFunc;
  m_AnimationSlot.SetAnimationResource(pResourceNode);
}

/**
 * @brief アニメーションの変更：グローバルアニメーション：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 */
void BaseCamera::ChangeAnimationGlobalByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  this->ChangeAnimationGlobalSmoothByResourceNode(pResourceNode, 0);
}

/**
 * @brief アニメーションの変更：グローバルアニメーション：補間あり：リソースノード指定版
 *
 * @param pResourceNode リソースノード
 * @param changeFrame 補間フレーム数
 * @param easeFunc イージング関数
 */
void BaseCamera::ChangeAnimationGlobalSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc)
{
  m_AnimMode = ANIM_MODE_GLOBAL;
  m_AnimInterpCurrentFrame = 0;
  m_AnimInterpMaxFrame = changeFrame;
  m_AnimEaseFunc = easeFunc;
  m_AnimationSlot.SetAnimationResource(pResourceNode);
}

/**
* @brief アニメーション中か？
*
* @return アニメーション中ならtrue, アニメーション中でなければfalse
*/
bool BaseCamera::IsAnimation() const
{
  return ( m_AnimMode != ANIM_MODE_NONE );
}

/**
 * @brief アニメーション補間中か？：ChangeAnimationSmoothの終了判定用
 *
 * @return 補間中ならtrue, 補間中でなければfalse
 */
bool BaseCamera::IsAnimationInterp() const
{
  return m_AnimInterpMaxFrame > 0;
}

/**
 * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
 *
 * @param enable trueの場合はループする、 falseの場合はループしない
 */
void BaseCamera::SetAnimationLoop(b32 enable)
{
  m_AnimationSlot.SetLoop(GFL_BOOL_CAST(enable));
}

/**
 * @brief ループ再生されるか？
 *
 * @return trueの場合はループする、 falseの場合はループしない
 */
b32 BaseCamera::IsAnimationLoop() const
{
  return m_AnimationSlot.IsLoop();
}

/**
 * @brief フレーム設定
 *
 * @param frame フレーム
 */
void BaseCamera::SetAnimationFrame(f32 frame)
{
  m_AnimationSlot.SetFrame(frame);
}

/**
 * @brief フレーム取得
 *
 * @return 現在のフレームを返す
 */
f32 BaseCamera::GetAnimationFrame() const
{
  return m_AnimationSlot.GetFrame();
}

/**
 * @brief 再生速度設定
 *
 * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
 */
void BaseCamera::SetAnimationStepFrame(f32 stepFrame)
{
  m_AnimationSlot.SetStepFrame(stepFrame);
}

/**
 * @brief 再生速度取得
 *
 * @return 再生速度を返す
 */
f32 BaseCamera::GetAnimationStepFrame() const
{
  return m_AnimationSlot.GetStepFrame();
}

/**
 * @brief ループの開始フレーム設定
 *
 * @param startFrame 開始フレーム
 */
void BaseCamera::SetAnimationStartFrame(f32 startFrame)
{
  m_AnimationSlot.SetLoopBeginFrame(startFrame);
}

/**
 * @brief ループの開始フレーム取得
 *
 * @return 開始フレームを返す
 */
f32 BaseCamera::GetAnimationStartFrame() const
{
  return m_AnimationSlot.GetLoopBeginFrame();
}

/**
 * @brief ループの終了フレーム設定
 *
 * @param endFrame 終了フレーム
 */
void BaseCamera::SetAnimationEndFrame(f32 endFrame)
{
  m_AnimationSlot.SetLoopEndFrame(endFrame);
}

/**
 * @brief ループの終了フレーム取得
 *
 * @return 終了フレームを返す
 */
f32 BaseCamera::GetAnimationEndFrame() const
{
  return m_AnimationSlot.GetLoopEndFrame();
}

/**
 * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
 *
 * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
 */
b32 BaseCamera::IsAnimationLastFrame() const
{
  return m_AnimationSlot.IsLastFrame();
}

/**
  * @brief フレーム数を取得
  *
  * @return フレーム数を返す
  */
u32 BaseCamera::GetFrameCount() const
{
  return m_AnimationSlot.GetFrameCount();
}

/**
* @brief ローカルアニメーションの反映倍率を設定
*
* @param scale 反映倍率
*
* @return 無し
*/
void BaseCamera::SetLocalAnimationScale( f32 scale )
{
  m_LocalAnimationScale = scale;
}

}}
