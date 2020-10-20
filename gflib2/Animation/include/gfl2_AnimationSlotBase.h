#ifndef GFLIB2_ANIMATION_SLOT_BASE_H_INCLUDED
#define GFLIB2_ANIMATION_SLOT_BASE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_BlendShapeAnimationResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>

namespace gfl2 { namespace animation {

/**
 * @brief アニメーションスロット基底
 */
class AnimationSlotBase
{
public:

  /**
   * @brief コンストラクタ
   */
  AnimationSlotBase();

  /**
   * @brief デストラクタ
   */
  virtual ~AnimationSlotBase();

  /**
   * @brief 初期化
   *
   * @param pHeap ヒープ
   * @param pTargetNode ターゲットノード
   */
  void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::TransformNode* pTargetNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief アニメーションリソース設定
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param pResourceNode リソースノード
   */
  void SetAnimationResource(gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief アニメーションリソース設定：ヒープ指定しない版 削除したい
   *
   * @param pResourceNode リソースノード
   */
  void SetAnimationResource(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief アニメーションリソース取得
   *
   * @return 現在のアニメーションリソースを取得する
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetAnimationResource() const { return m_pReturnResourceNode; }

  /**
   * @brief アニメーションが含まれているか？
   *
   * @return アニメーションが含まれているならtrue, 含まれていないならfalse
   */
  virtual bool HasAnimation() const = 0;

  /**
   * @brief フレーム更新
   */
  void UpdateFrame();

  /**
   * @brief ループ設定
   *
   * @param loopFlag ループするならtrue, しないならfalse
   */
  void SetLoop(bool loopFlag) { m_LoopFlag = loopFlag; }

  /**
   * @brief ループするか？
   *
   * @return ループするならtrue, ループしないならfalse
   */
  bool IsLoop() const { return m_LoopFlag ? true : false; }

  /**
   * @brief データがループしているか？
   *
   * @return ループするならtrue, ループしないならfalse
   */
  bool IsDataLoop() const { return m_DataLoopFlag ? true : false; }

  /**
   * @brief ループ開始フレーム設定
   *
   * @param beginFrame ループ開始フレーム
   */
  void SetLoopBeginFrame(f32 beginFrame) { m_LoopBeginFrame = beginFrame; }

  /**
   * @brief ループ開始フレーム取得
   *
   * @return ループ開始フレームを返す
   */
  f32 GetLoopBeginFrame() const { return m_LoopBeginFrame; }

  /**
   * @brief ループ終了フレーム設定
   *
   * @param endFrame ループ終了フレーム
   */
  void SetLoopEndFrame(f32 endFrame) { m_LoopEndFrame = endFrame; }

  /**
   * @brief ループ終了フレーム取得
   *
   * @return ループ終了フレームを返す
   */
  f32 GetLoopEndFrame() const { return m_LoopEndFrame; }

  /**
   * @brief ステップフレームを設定
   *
   * @param stepFrame
   */
  void SetStepFrame(f32 stepFrame) { m_StepFrame = stepFrame; }

  /**
   * @brief ステップフレームを取得
   *
   * @return ステップフレームを返す
   */
  f32 GetStepFrame() const { return m_StepFrame; }

  /**
   * @brief フレームを設定
   *
   * @param frame
   */
  void SetFrame(f32 frame) { m_CurrentFrame = frame; }

  /**
   * @brief フレームを取得
   *
   * @return フレームを返す
   */
  f32 GetFrame() const { return m_CurrentFrame; }

  /**
   * @brief 前回フレームを取得
   *
   * @return 前回フレームを返す
   */
  f32 GetOldFrame() const { return m_OldFrame; }

  /**
   * @brief 最終フレームかどうか確認
   *
   * @return true:最終フレーム, false:最終フレームではない
   */
  bool IsLastFrame() const 
  {
    // リソースがセットされていないなら再生が終わったことにする
    if (m_pResourceNode == NULL)
    {
      return true;
    }

    return GFL_BOOL_CAST( m_LastFlag );
  }

  /**
   * @brief フレーム数を取得
   *
   * @return フレーム数を返す
   */
  u32 GetFrameCount() const { return m_FrameCount; }

protected:

  /**
   * @brief 派生クラスでの初期化
   *
   * @param pHeap ヒープ
   */
  virtual void InitializeDerived(gfl2::heap::HeapBase* pHeap) = 0;

  /**
   * @brief 派生クラスでの終了
   */
  virtual void FinalizeDerived() = 0;

  /**
   * @brief アニメーションリソース設定
   *
   * @param pHeap ヒープ
   */
  virtual void SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap) = 0;

protected:

  // @attention スロットでウェイトをかけて部分アニメーションまでは必要なさそう？
  //f32 m_Weight;                                                                 ///< このスロットのウェイト

  f32 m_StepFrame;                                                              ///< 更新で進むフレームの量、再生速度
  f32 m_CurrentFrame;                                                           ///< 現在のフレーム
  f32 m_OldFrame;                                                               ///< 前回のフレーム
	u32 m_FrameCount;                                                             ///< フレーム数

  u8 m_LoopFlag;                                                                ///< ループフラグ
  u8 m_DataLoopFlag;                                                            ///< データループフラグ
  u8 m_LastFlag;                                                                ///< 最終フレームフラグ

  f32 m_LoopBeginFrame;                                                         ///< ループ開始フレーム
  f32 m_LoopEndFrame;                                                           ///< ループ終了フレーム

  // !@attention ModelInstanceNodeとDrawEnvNode両方持つのは気持ち悪い、、分離するかテンプレートかするかしたい
  gfl2::heap::HeapBase* m_pHeap;                                                ///< アニメーション用ヒープ
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelNode; ///< データ反映対象のモデルノード
	gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;     ///< データ反映対象のシーン環境ノード
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* m_pResourceNode;   ///< リソースノード

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pReturnResourceNode;   ///< 返却用リソースノード
};

}}

#endif