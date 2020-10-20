#if !defined(POKE_3D_LIB_MODEL_BASE_CAMERA_H_INCLUDED)
#define POKE_3D_LIB_MODEL_BASE_CAMERA_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>

#include <Animation/include/gfl2_CameraAnimationSlot.h>
#include <Animation/include/gfl2_AnimationPackList.h>

#include <math/include/gfl2_EulerRotation.h>
#include <math/include/gfl2_Easing.h>

namespace poke_3d { namespace model {


/**
 * @brief カメラ基底クラス
 */
class BaseCamera
{
public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  BaseCamera(); 
  /**
   * @brief デストラクタ
   */
  virtual ~BaseCamera();

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
  bool Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 cameraNo);

  /**
   * @brief 破棄
   */
  void Destroy();

  //---------------------------------------------------------------------------------
  // ■ カメラパラメータ

  /**
   * @brief 現在の設定値から生成されるプロジェクション行列を取得する
   *
   * @return プロジェクション行列を返す
   */
  gfl2::math::Matrix44 GetProjectionMatrix() const;

  /**
   * @brief 現在の設定値から生成されるビュー行列を取得する
   *
   * @return ビュー行列を返す
   */
  gfl2::math::Matrix34 GetViewMatrix() const;

  /**
   * @brief ニアクリップ設定
   *
   * @param nearClip ニアクリップ
   */
  void SetNear(f32 nearClip);

  /**
   * @brief ニアクリップ取得
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return ニアクリップを返す
   */
  f32 GetNear(bool blendFlag = true) const;

  /**
   * @brief ファークリップ設定
   *
   * @param farClip ファークリップ
   */
  void SetFar(f32 farClip);

  /**
   * @brief ファークリップ取得
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return ファークリップを返す
   */
  f32 GetFar(bool blendFlag = true) const;

  /**
   * @brief 垂直方向画角設定
   *
   * @param fovy 垂直方向画角
   */
  void SetFovy(f32 fovy);

  /**
   * @brief 垂直方向画角取得
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return 垂直方向画角を返す
   */
  f32 GetFovy(bool blendFlag = true) const;
  
  /**
   * @brief アスペクト比設定
   *
   * @param aspectRatio アスペクト比
   */
  void SetAspectRatio(f32 aspectRatio);

  /**
   * @brief アスペクト比取得
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return アスペクト比を返す
   */
  f32 GetAspectRatio(bool blendFlag = true) const;

  /**
   * @brief 位置設定
   *
   * @param position 位置
   */
  void SetPosition(const gfl2::math::Vector3& position);

  /**
   * @brief 位置設定：要素毎
   *
   * @param positionX 位置Ｘ
   * @param positionY 位置Ｙ
   * @param positionZ 位置Ｚ
   */
  void SetPosition(f32 positionX, f32 positionY, f32 positionZ);

  /**
  * @brief ターゲット位置設定
  * @note  情報保存用のワークを更新するだけで、作成する行列には影響がない
  *
  * @param position 位置
  */
  void SetTargetPosition(const gfl2::math::Vector3& position);

  /**
   * @brief 位置取得
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return 位置を返す
   */
  const gfl2::math::Vector3 GetPosition(bool blendFlag = true) const;

  /**
   * @brief ターゲット位置取得
   * @note  SetupCameraLookAt関数で設定されているときのみ有効
   *
   * @return 位置を返す
   */
  const gfl2::math::Vector3 GetTargetPosition() const;

  /**
   * @brief アップベクター取得
   * @note  SetupCameraLookAt関数で設定されているときのみ有効
   *
   * @return アップベクターを返す
   */
  const gfl2::math::Vector3 GetUpVector() const;

  /**
   * @brief 回転設定
   *
   * @param rotationRad 回転（ラジアン）
   * @param rotationOrder 回転順：デフォルトはZYX
   */
  void SetRotation(const gfl2::math::Vector3& rotationRad, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);

  /**
   * @brief 回転設定：要素毎
   *
   * @param rotationRadX 回転Ｘ（ラジアン）
   * @param rotationRadY 回転Ｙ（ラジアン）
   * @param rotationRadZ 回転Ｚ（ラジアン）
   * @param rotationOrder 回転順：デフォルトはZYX
   */
  void SetRotation(f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);

  /**
   * @brief 回転取得
   *        ※ SetRotation以外で回転を設定した場合は見た目に合わない値が返ります
   *
   * @return 回転を返す
   */
  gfl2::math::Vector3 GetRotation() const;

  //---------------------------------------------------------------------------------
  // ■ 回転の設定：クォータニオン

  /**
   * @brief 回転設定：クォータニオン
   *
   * @param rotation 回転
   */
  void SetRotationQuat(const gfl2::math::Quaternion& rotation);

  /**
   * @brief 回転取得：クォータニオン
   *
   * @param blendFlag true:アニメーションとブレンドした結果の値、 false:ブレンド前の値
   * @return 回転を返す
   */
  const gfl2::math::Quaternion GetRotationQuat( bool blendFlag = true ) const;

  //---------------------------------------------------------------------------------
  // ■ カメラ位置、回転の便利設定

  /**
   * @brief カメラセットアップ：ルックアット
   *
   * @param cameraPosition カメラ位置
   * @param cameraTarget カメラ注視点
   * @param cameraUpVector カメラアップベクター
   */
  void SetupCameraLookAt(const gfl2::math::Vector3& cameraPosition, const gfl2::math::Vector3& cameraTarget, const gfl2::math::Vector3& cameraUpVector);

  //---------------------------------------------------------------------------------
  // ■ アニメーション

  /**
   * @brief アニメーションの更新
   */
  void UpdateAnimation();

  /**
  * @brief アニメーションの終了
  */
  void EndAnimation();

  /**
   * @brief アニメーションパックリストの設定
   *
   * @param pAnimationPackList セットするアニメーションパックリスト
   */
  void SetAnimationPackList(gfl2::animation::AnimationPackList* pAnimationPackList);

  /**
   * @brief アニメーションパックリストの取得
   *
   * @return セットされているアニメーションパックリスト
   */
  const gfl2::animation::AnimationPackList* GetAnimationPackList() const;

  /**
   * @brief 現在のアニメーションIDを取得する
   *
   * @return アニメーションIDを返す、設定されていなければ-1を返す
   */
  s32 GetAnimationId() const;

  /**
   * @brief アニメーションの変更：ローカルアニメーション：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   */
  void ChangeAnimationLocal(u32 animationId);

  /**
   * @brief アニメーションの変更：ローカルアニメーション：補間あり：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   * @param changeFrame 補間フレーム数
   * @param easeFunc イージング関数
   */
  void ChangeAnimationLocalSmooth(u32 animationId, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR);

  /**
   * @brief アニメーションの変更：グローバルアニメーション：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   */
  void ChangeAnimationGlobal(u32 animationId);

  /**
   * @brief アニメーションの変更：グローバルアニメーション：補間あり：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   * @param changeFrame 補間フレーム数
   * @param easeFunc イージング関数
   */
  void ChangeAnimationGlobalSmooth(u32 animationId, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR);

  /**
   * @brief 現在のアニメーションリソースノードを取得
   *
   * @return アニメーションリソースノードを返す、設定されていなければNULLを返す
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetAnimationResourceNode() const;

  /**
   * @brief アニメーションの変更：ローカルアニメーション：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   */
  void ChangeAnimationLocalByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief アニメーションの変更：ローカルアニメーション：補間あり：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   * @param changeFrame 補間フレーム数
   * @param easeFunc イージング関数
   */
  void ChangeAnimationLocalSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR);

  /**
   * @brief アニメーションの変更：グローバルアニメーション：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   */
  void ChangeAnimationGlobalByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief アニメーションの変更：グローバルアニメーション：補間あり：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   * @param changeFrame 補間フレーム数
   * @param easeFunc イージング関数
   */
  void ChangeAnimationGlobalSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR);

  /**
  * @brief アニメーション中か？
  *
  * @return アニメーション中ならtrue, アニメーション中でなければfalse
  */
  bool IsAnimation() const;

  /**
   * @brief アニメーション補間中か？：ChangeAnimationSmoothの終了判定用
   *
   * @return 補間中ならtrue, 補間中でなければfalse
   */
  bool IsAnimationInterp() const;

  /**
   * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
   *
   * @param enable trueの場合はループする、 falseの場合はループしない
   */
  void SetAnimationLoop(b32 enable);

  /**
   * @brief ループ再生されるか？
   *
   * @return trueの場合はループする、 falseの場合はループしない
   */
  b32 IsAnimationLoop() const;

  /**
   * @brief フレーム設定
   *
   * @param frame フレーム
   */
  void SetAnimationFrame(f32 frame);

  /**
   * @brief フレーム取得
   *
   * @return 現在のフレームを返す
   */
  f32 GetAnimationFrame() const;

  /**
   * @brief 再生速度設定
   *
   * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
   */
  void SetAnimationStepFrame(f32 stepFrame);

  /**
   * @brief 再生速度取得
   *
   * @return 再生速度を返す
   */
  f32 GetAnimationStepFrame() const;

  /**
   * @brief ループの開始フレーム設定
   *
   * @param startFrame 開始フレーム
   */
  void SetAnimationStartFrame(f32 startFrame);

  /**
   * @brief ループの開始フレーム取得
   *
   * @return 開始フレームを返す
   */
  f32 GetAnimationStartFrame() const;

  /**
   * @brief ループの終了フレーム設定
   *
   * @param endFrame 終了フレーム
   */
  void SetAnimationEndFrame(f32 endFrame);

  /**
   * @brief ループの終了フレーム取得
   *
   * @return 終了フレームを返す
   */
  f32 GetAnimationEndFrame() const;

  /**
   * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
   *
   * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
   */
  b32 IsAnimationLastFrame() const;

  /**
   * @brief フレーム数を取得
   *
   * @return フレーム数を返す
   */
  u32 GetFrameCount() const;

  /**
  * @brief ローカルアニメーションの反映率を設定
  *
  * @param scale
  *
  * @return 無し
  */
  void SetLocalAnimationScale( f32 scale );

private:

  enum AnimMode
  {
    ANIM_MODE_NONE,   // アニメーションしない
    ANIM_MODE_LOCAL,  // ローカル再生　：　現在のパラメータ＋アニメーション結果
    ANIM_MODE_GLOBAL, // グローバル再生　：　全てアニメーション結果で上書き
  };

  f32 GetInterpFrameCore() const
  {
    if (m_AnimInterpMaxFrame == 0)
    {
      return 1.0f;
    }
    if (m_AnimInterpCurrentFrame >= m_AnimInterpMaxFrame)
    {
      return 1.0f;
    }

    return (f32)m_AnimInterpCurrentFrame / (f32)m_AnimInterpMaxFrame;
  }

  f32 GetInterpFrame() const
  {
    f32 t = this->GetInterpFrameCore();
    return gfl2::math::Easing::GetValue(m_AnimEaseFunc, t);
  }

  void CopyParam()
  {
    m_Position = m_pCameraNode->GetLocalTranslate();
    m_Rotation = m_pCameraNode->GetLocalRotation();
    m_NearClip = m_pCameraNode->GetNear();
    m_FarClip = m_pCameraNode->GetFar();
    m_Fovy = m_pCameraNode->GetFovy();
    m_AspectRatio = m_pCameraNode->GetAspectRatio();
    m_EulerRotation = gfl2::math::EulerRotation(m_Rotation);
  }

private:

  // 描画環境ノード
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*                 m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::CameraNode*                  m_pCameraNode;
  u32                                                                       m_CameraNo;

  // SRT用
  gfl2::math::EulerRotation                                                 m_EulerRotation;

  // アニメーション用
  gfl2::animation::CameraAnimationSlot                                      m_AnimationSlot;
  gfl2::animation::AnimationPackList*                                       m_pAnimationPackList;
  gfl2::heap::HeapBase*                                                     m_pAnimationHeap;
  u32                                                                       m_AnimInterpMaxFrame;
  u32                                                                       m_AnimInterpCurrentFrame;
  AnimMode                                                                  m_AnimMode;
  gfl2::math::Easing::EaseFunc                                              m_AnimEaseFunc;
  f32                                                                       m_LocalAnimationScale;

  // 補間用にパラメータを持たせる
  gfl2::math::Vector3                                                       m_Position;
  gfl2::math::Vector3                                                       m_TargetPosition;
  gfl2::math::Vector3                                                       m_UpVector;
  gfl2::math::Quaternion                                                    m_Rotation;
  f32                                                                       m_NearClip;
  f32                                                                       m_FarClip;
  f32                                                                       m_Fovy;
  f32                                                                       m_AspectRatio;
};

}}

#endif
