#ifndef GFLIB2_ANIMATION_CONTROLLER_H_INCLUDED
#define GFLIB2_ANIMATION_CONTROLLER_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_JointAnimationSlot.h>
#include <Animation/include/gfl2_MaterialAnimationSlot.h>
#include <Animation/include/gfl2_VisibilityAnimationSlot.h>
#include <Animation/include/gfl2_BlendShapeAnimationSlot.h>
#include <Animation/include/gfl2_KeyAnimationSlot.h>
#include <Animation/include/gfl2_AnimationSlotSet.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

/**
 * @brief アニメーションコントローラー
 */
class AnimationController
{
public:

  /**
   * @brief アニメーションタイプ
   */
  enum AnimationType
  {
    ANIMATION_TYPE_JOINT,               ///< ジョイントアニメーション
    ANIMATION_TYPE_MATERIAL,            ///< マテリアルアニメーション
    ANIMATION_TYPE_VISIBILITY,          ///< ビジビリティアニメーション
    ANIMATION_TYPE_BLENDSHAPE,          ///< ブレンドシェイプアニメーション
    ANIMATION_TYPE_KEY,                 ///< キーアニメーション

    ANIMATION_TYPE_COUNT                ///< アニメーションタイプ総数
  };

  /**
   * @brief アニメーションビット
   */
  enum AnimationBit
  {
    ANIMATION_BIT_NONE = 0,                                     ///< 無効

    ANIMATION_BIT_JOINT = 1 << ANIMATION_TYPE_JOINT,            ///< ジョイントアニメーション
    ANIMATION_BIT_MATERIAL = 1 << ANIMATION_TYPE_MATERIAL,      ///< マテリアルアニメーション
    ANIMATION_BIT_VISIBILITY = 1 << ANIMATION_TYPE_VISIBILITY,  ///< ビジビリティアニメーション
    ANIMATION_BIT_BLENDSHAPE = 1 << ANIMATION_TYPE_BLENDSHAPE,  ///< ブレンドシェイプアニメーション
    ANIMATION_BIT_KEY = 1 << ANIMATION_TYPE_KEY,                ///< ブレンドシェイプアニメーション

    ANIMATION_BIT_ALL =                                         ///< 全アニメーション
      ANIMATION_BIT_JOINT |
      ANIMATION_BIT_MATERIAL |
      ANIMATION_BIT_VISIBILITY |
      ANIMATION_BIT_BLENDSHAPE |
      ANIMATION_BIT_KEY
  };

  class AnimationSlotList
  {
  public:

    AnimationSlotList(
      AnimationSlotBase* pJointSlot,
      AnimationSlotBase* pMaterialSlot,
      AnimationSlotBase* pVisibilitySlot,
      AnimationSlotBase* pBlendShapeSlot,
      AnimationSlotBase* pKeySlot
      )
    {
      m_pSlotList[ANIMATION_TYPE_JOINT] = pJointSlot;
      m_pSlotList[ANIMATION_TYPE_MATERIAL] = pMaterialSlot;
      m_pSlotList[ANIMATION_TYPE_VISIBILITY] = pVisibilitySlot;
      m_pSlotList[ANIMATION_TYPE_BLENDSHAPE] = pBlendShapeSlot;
      m_pSlotList[ANIMATION_TYPE_KEY] = pKeySlot;
    }
    AnimationSlotList(
      const AnimationSlotBase* pJointSlot,
      const AnimationSlotBase* pMaterialSlot,
      const AnimationSlotBase* pVisibilitySlot,
      const AnimationSlotBase* pBlendShapeSlot,
      const AnimationSlotBase* pKeySlot
      )
    {
      m_pSlotList[ANIMATION_TYPE_JOINT] = const_cast<AnimationSlotBase*>(pJointSlot);
      m_pSlotList[ANIMATION_TYPE_MATERIAL] = const_cast<AnimationSlotBase*>(pMaterialSlot);
      m_pSlotList[ANIMATION_TYPE_VISIBILITY] = const_cast<AnimationSlotBase*>(pVisibilitySlot);
      m_pSlotList[ANIMATION_TYPE_BLENDSHAPE] = const_cast<AnimationSlotBase*>(pBlendShapeSlot);
      m_pSlotList[ANIMATION_TYPE_KEY] = const_cast<AnimationSlotBase*>(pKeySlot);
    }

    AnimationSlotBase* GetSlot(AnimationType type)
    {
      return m_pSlotList[type];
    }
    const AnimationSlotBase* GetSlot(AnimationType type) const
    {
      return m_pSlotList[type];
    }

  private:
    AnimationSlotBase* m_pSlotList[ANIMATION_TYPE_COUNT];
  };

  /**
   * @brief コールバック関数型 特定のタイミングでpUserWorkを引数としたコールバックを行う
   */
  typedef void (*AnimationCallbackFunc)(void* pUserWork);

  /**
   * @brief コールバックパラメータ
   */
  struct AnimationCallbackParam
  {
    AnimationCallbackFunc  callbackFunc;  ///< コールバック関数
    void*                  pUserWork;     ///< ユーザー任意のワーク用ポインタ

    /**
     * @brief コンストラクタ
     */
    AnimationCallbackParam() :
      callbackFunc(NULL),
      pUserWork(NULL)
    {
    }
  };

public:

  /**
   * @brief コンストラクタ
   */
	AnimationController();

  /**
   * @brief デストラクタ
   */
  ~AnimationController();

  /**
   * @brief 初期化
   */
	void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode);
  
  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief アニメーション用のヒープを取得
   */
  gfl2::heap::HeapBase* GetAnimationHeap();

  //---------------------------------------------------------
  // 各種スロットの取得
  // @attention 次設計する時はモデルベースクラスではスロットは直接触らせない仕組みにする

  /**
   * @brief ジョイントスロット取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  JointAnimationSlot* GetJointSlot(u32 slotIndex = 0);
  const JointAnimationSlot* GetJointSlot(u32 slotIndex = 0) const;

  /**
   * @brief ジョイントスロット取得：補間用
   *
   * @param slotIndex 取得するスロットの番号
   */
  JointAnimationSlot* GetJointInterpSlot(u32 slotIndex = 0);
  const JointAnimationSlot* GetJointInterpSlot(u32 slotIndex = 0) const;

  /**
   * @brief マテリアルスロット取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  MaterialAnimationSlot* GetMaterialSlot(u32 slotIndex = 0);
  const MaterialAnimationSlot* GetMaterialSlot(u32 slotIndex = 0) const;

  /**
   * @brief ビジビリティスロット取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  VisibilityAnimationSlot* GetVisibilitySlot(u32 slotIndex = 0);
  const VisibilityAnimationSlot* GetVisibilitySlot(u32 slotIndex = 0) const;

  /**
   * @brief ブレンドシェイプスロット取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  BlendShapeAnimationSlot* GetBlendShapeSlot(u32 slotIndex = 0);
  const BlendShapeAnimationSlot* GetBlendShapeSlot(u32 slotIndex = 0) const;

  /**
   * @brief キーアニメーションスロット取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  KeyAnimationSlot* GetKeySlot(u32 slotIndex = 0);
  const KeyAnimationSlot* GetKeySlot(u32 slotIndex = 0) const;

  /**
   * @brief ジョイントスロット確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  JointAnimationSlot* AllocateJointSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief ジョイントスロット確保：補間用
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  JointAnimationSlot* AllocateJointInterpSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief マテリアルスロット確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  MaterialAnimationSlot* AllocateMaterialSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief ビジビリティスロット確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  VisibilityAnimationSlot* AllocateVisibilitySlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief ブレンドシェイプスロット確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  BlendShapeAnimationSlot* AllocateBlendShapeSlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief キーアニメーションスロット確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   */
  KeyAnimationSlot* AllocateKeySlot(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex = 0);

  /**
   * @brief スロットのメモリ確保
   *
   * @param pAnimationHeap メモリを確保するヒープ
   * @param slotIndex 確保するスロットの番号
   * @param animationBit 確保するアニメーションのタイプビット
   */
  AnimationSlotList AllocateAnimationSlotList(gfl2::heap::HeapBase* pAnimationHeap, u32 slotIndex, AnimationBit animationBit);
  AnimationSlotList AllocateAnimationInterpSlotList(heap::HeapBase* pAnimationHeap, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief リソース設定用のスロットリスト取得
   *
   * @param slotIndex 取得するスロットの番号
   * @param animationBit 設定するアニメーションのタイプビット
   */
  AnimationSlotList GetResourceAnimationSlotList(u32 slotIndex, AnimationBit animationBit);
  AnimationSlotList GetResourceAnimationInterpSlotList(u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief アニメーションスロットリスト取得
   *
   * @param slotIndex 取得するスロットの番号
   */
  AnimationSlotList GetAnimationSlotList(u32 slotIndex);
  const AnimationSlotList GetAnimationSlotList(u32 slotIndex) const;
  AnimationSlotList GetAnimationInterpSlotList(u32 slotIndex);
  const AnimationSlotList GetAnimationInterpSlotList(u32 slotIndex) const;

  //---------------------------------------------------------
  // ジョイントアニメーションの補間設定
  // @attention 次設計する時はモデルベースクラスではスロットは直接触らせない仕組みにする

  /**
   * @brief ジョイントアニメーションの補間リクエスト
   *
   * @param interpFrame 補間フレーム数
   * @param slotIndex 補間スロット番号
   */
  void InterpRequest(f32 interpFrame, s8 slotIndex = 0);

  /**
   * @brief ジョイントアニメーションの補間中か？
   *
   * @return 補間中ならtrue
   */
  bool IsInterp() const { return m_IsForceInterp || (m_JointInterpFrame > 0.0f); }

  /**
   * @brief 補間スロット取得
   *
   * @return 補間中のスロットのインデックス取得
   */
  s8 GetInterpSlotIndex() const { return m_InterpSlotIndex; }

  /**
   * @brief 補間のフレーム数取得
   *
   * @return 補間のフレーム数を返す
   */
  f32 GetInterpTotalFrame() const { return m_JointInterpFrame; }

  /**
   * @brief 補間のフレーム数設定
   *
   * @param frame 補間フレーム数
   */
  void SetInterpTotalFrame(f32 frame) { m_JointInterpFrame = frame; }

  /**
   * @brief 補間の現在フレーム数取得
   *
   * @return 補間の現在フレーム数を返す
   */
  f32 GetInterpFrame() const { return m_CurrentJointInterpFrame; }

  /**
   * @brief 補間の現在フレーム数設定
   *
   * @param frame 補間フレーム数
   */
  void SetInterpFrame(f32 frame) { m_CurrentJointInterpFrame = frame; }

  /**
   * @brief 補間のフレームステップ取得
   *
   * @return 補間の現在フレームステップを返す
   */
  f32 GetInterpStepFrame() const { return m_JointInterpStepFrame; }

  /**
   * @brief 補間のフレームステップ設定
   *
   * @param 補間の現在フレームステップ
   */
  void SetInterpStepFrame(f32 stepFrame) { m_JointInterpStepFrame = stepFrame; }

  /**
   * @brief 強制補間中フラグ設定
   *
   */
  void SetForceInterpMode(bool isInterp) { m_IsForceInterp = isInterp; }

  /**
   * @brief アニメーション計算有効フラグの設定
   *
   * @param enable true:計算有効, false:計算無効
   */
  void SetCalculateEnable(bool enable) { m_CalculateEnabled = enable; }

  /**
   * @brief アニメーション計算有効フラグの取得
   *
   * @return true:計算有効, false:計算無効
   */
  bool IsCalculateEnable() const { return GFL_BOOL_CAST(m_CalculateEnabled); }

  /**
   * @brief フレーム更新有効フラグの設定
   *
   * @param enable true:更新有効, false:更新無効
   */
  void SetUpdateFrameEnable(bool enable) { m_UpdateFrameEnabled = enable; }

  /**
   * @brief フレーム更新有効フラグの取得
   *
   * @return true:更新有効, false:更新無効
   */
  bool IsUpdateFrameEnable() const { return GFL_BOOL_CAST(m_UpdateFrameEnabled); }

  //---------------------------------------------------------
  // 更新

  /**
   * @brief アニメーション更新
   *
   * @param interpWeight ブレンド率を手動で設定するようの値 0.0 ~ 1.0
   * @param animationBit 更新するアニメーションのタイプ
   *
   * @return 補間率
   */
  f32 Update(f32 interpWeight = -1.0f, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief 再生中のアニメーションのAABBを取得
   */
  math::AABB GetAABB() const
  {
    return m_CurrentAABB;
  }

  /**
   * @brief 歩き用速度を取得：ビューアー用：ポケモン用の拡張データ
   *
   * @param stepFrame 再生速度
   * @param prevFrame 補間用：アニメーション０のフレーム
   * @param nextFrame 補間用：アニメーション１のフレーム
   * @param interpWeight 補間率
   *
   * @return 移動速度ベクトル
   */
  math::Vector3 GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight = -1.0f) const;

  /**
   * @brief 歩き用速度を取得：ポケモン用の拡張データ
   *
   * @param interpWeight 補間率
   *
   * @return 移動速度ベクトル
   */
  math::Vector3 GetWalkSpeedSmooth(f32 interpWeight = -1.0f) const;
  math::Vector3 GetWalkSpeed() const;

  /**
   * @brief 補間キャンセル時のコールバックを登録
   *
   * @param callbackFunc コールバックさせる関数
   * @param pUserWork コールバック時の引数、ユーザー任意のワークポインタ
   */
  void SetInterpCancelCallback(AnimationCallbackFunc callbackFunc, void* pUserWork);

private:

  f32 UpdateJointAnimation(f32 interpWeight);

  f32 UpdateJointAnimationAuto();
  f32 UpdateJointAnimationManual(f32 interpWeight);

  bool UpdateInterpFrameAuto(f32* out_pInterpWeight);
  void UpdateInterpFrameManual();

  void UpdateJointAnimationNoInterp();
  void UpdateJointAnimationInterp(f32 interpWeight);

  void UpdateMaterialAnimation();
  void UpdateVisibilityAnimation();
  void UpdateBlendShapeAnimation();
  void UpdateKeyAnimation();

private:

  gfl2::heap::HeapBase*                                             m_pHeap;                        ///< アニメーションに使用するヒープ

	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelNode;                   ///< データ反映対象のモデルノード

  AnimationSlotSet<JointAnimationSlot>                              m_JointSlotSetList[2];          ///< ジョイントアニメーションスロットセット：補完用に２つ用意
  u32                                                               m_CurrentJointSlotSetIndex;     ///< 現在のスロットセットのインデックス
  f32                                                               m_JointInterpFrame;             ///< 補完フレーム
  f32                                                               m_CurrentJointInterpFrame;      ///< 現在の補完フレーム
  f32                                                               m_JointInterpStepFrame;         ///< 補間速度
  b8                                                                m_IsForceInterp;                ///< 強制補完中フラグ
  b8                                                                m_CalculateEnabled : 1;         ///< アニメーション計算有効フラグ
  b8                                                                m_UpdateFrameEnabled : 1;       ///< フレーム更新有効フラグ
  b8                                                                : 6;                            // パディング
  b8                                                                m_LerpInterpEnabled;            ///< 前回の結果との線形補間フラグ
  s8                                                                m_InterpSlotIndex;              ///< 補間スロットインデックス

  AnimationSlotSet<MaterialAnimationSlot>                           m_MaterialSlotSet;              ///< マテリアルスロットセット
  AnimationSlotSet<VisibilityAnimationSlot>                         m_VisibilitySlotSet;            ///< ビジビリティスロットセット
  AnimationSlotSet<BlendShapeAnimationSlot>                         m_BlendShapeSlotSet;            ///< ブレンドシェイプスロットセット
  AnimationSlotSet<KeyAnimationSlot>                                m_KeySlotSet;                   ///< キースロットセット

  math::AABB                                                        m_CurrentAABB;                  ///< 再生中のモーションのAABB

  AnimationCallbackParam                                            m_InterpCancelCallbackParam;    ///< 補間キャンセル時にコールバックパラメータ
};

}}

#endif