#if !defined(POKE_3D_LIB_MODEL_BASE_MODEL_ANIMATION_H_INCLUDED)
#define POKE_3D_LIB_MODEL_BASE_MODEL_ANIMATION_H_INCLUDED

#include <Animation/include/gfl2_AnimationController.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MotionConfigInstanceNode.h>

namespace poke_3d { namespace model {

/**
 * @brief モデル基底クラス：アニメーションのラッパー
 */
class BaseModelAnimation
{
public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  BaseModelAnimation(gfl2::animation::AnimationController* pAnimationController, u32 slotIndex = (u32)-1, gfl2::animation::AnimationController::AnimationBit animationBit = gfl2::animation::AnimationController::ANIMATION_BIT_NONE); 

  /**
   * @brief デストラクタ
   */
  virtual ~BaseModelAnimation();

  //---------------------------------------------------------------------------------
  // ■ アニメーション

  /**
   * @brief アニメーションの更新
   */
  f32 UpdateAnimation();

  /**
   * @brief アニメーションの更新
   *
   * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
   */
  f32 UpdateAnimationManual(f32 interpWeight);

  /**
   * @brief アニメーションリソースノードの取得
   *
   * @return アニメーションリソースノードを返す、設定されてなければNULL
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceNode() const;

  /**
   * @brief アニメーション再生用のメモリを明示的に確保する
   *
   * @param pAnimationHeap アニメーションヒープ
   * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
   * @param pResourceNode リソースノード
   */
  void Allocate(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief デフォルトのブレンドフレーム数取得
   *
   * @param pResourceNode リソースノード
   * @param pMotionConfigNode モーションコンフィグノード
   *
   * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
   */
  u32 GetDefaultBlendFrame(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode) const;

  /**
   * @brief アニメーションの変更
   *
   * @param pResourceNode リソースノード
   * @param pMotionConfigNode モーションコンフィグノード
   */
  void Change(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode);

  /**
   * @brief 現在の設定で補間有効か？
   *
   * @param pResourceNode リソースノード
   * @param changeFrame 補間フレーム数
   *
   * @return true:補間有効 false:補間無効
   */
  bool IsSmoothEnable(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame);

  /**
   * @brief アニメーションの変更：補間あり
   *
   * @param pResourceNode リソースノード
   * @param changeFrame 補間フレーム数
   */
  void ChangeSmooth(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame);

  /**
   * @brief アニメーションの変更：補間率の手動設定版
   *        この関数でアニメーションをセットした後UpdateAnimationManual()の引数に0.0〜1.0の範囲を指定してください
   *
   * @param pResourceNode0 リソースノード0
   * @param pResourceNode1 リソースノード1
   */
  void ChangeSmoothManual(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode0, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode1);

  /**
   * @brief アニメーション補間中か？：ChangeSmoothの終了判定用
   *
   * @return 補間中ならtrue, 補間中でなければfalse
   */
  bool IsAnimationInterp() const;

  /**
   * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
   *
   * @param enable trueの場合はループする、 falseの場合はループしない
   */
  void SetLoop(b32 enable);

  /**
   * @brief ループ再生されるか？
   *
   * @return trueの場合はループする、 falseの場合はループしない
   */
  b32 IsLoop() const;

  /**
   * @brief フレーム設定
   *
   * @param frame フレーム
   */
  void SetFrame(f32 frame);

  /**
   * @brief フレーム取得
   *
   * @return 現在のフレームを返す
   */
  f32 GetFrame() const;

  /**
   * @brief 再生速度設定
   *
   * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
   */
  void SetStepFrame(f32 stepFrame);

  /**
   * @brief 再生速度取得
   *
   * @return 再生速度を返す
   */
  f32 GetStepFrame() const;

  /**
   * @brief ループの開始フレーム設定
   *
   * @param startFrame 開始フレーム
   */
  void SetStartFrame(f32 startFrame);

  /**
   * @brief ループの開始フレーム取得
   *
   * @return 開始フレームを返す
   */
  f32 GetStartFrame() const;

  /**
   * @brief ループの終了フレーム設定
   *
   * @param endFrame 終了フレーム
   */
  void SetEndFrame(f32 endFrame);

  /**
   * @brief ループの終了フレーム取得
   *
   * @return 終了フレームを返す
   */
  f32 GetEndFrame() const;

  /**
   * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
   *
   * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
   */
  b32 IsLastFrame() const;

  /**
   * @brief キーアニメーションのコールバックを設定
   *
   * @param pUserData ユーザーデータ
   * @param pCallback コールバック
   */
  void SetKeyCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback);

  /**
   * @brief 操作対象を補間スロットに切り替える：ビューアー用
   *
   * @param isTypeInterp 操作対象が補間スロットか？
   */
  void SetControllTargetInterp(bool isTargetInterp);

  /**
   * @brief 補間用スロットのフレームと再生速度の同期を取る：ビューアー用
   */
  void SyncFrameAndStepFrame();

private: // 内部用関数

  gfl2::animation::AnimationSlotBase* GetAnimationSlot_(gfl2::animation::AnimationController::AnimationSlotList& slotList, gfl2::animation::AnimationController::AnimationType type)
  {
    return slotList.GetSlot((gfl2::animation::AnimationController::AnimationType)type);
  }
  const gfl2::animation::AnimationSlotBase* GetAnimationSlot_(const gfl2::animation::AnimationController::AnimationSlotList& slotList, gfl2::animation::AnimationController::AnimationType type) const
  {
    return slotList.GetSlot((gfl2::animation::AnimationController::AnimationType)type);
  }

  gfl2::animation::AnimationSlotBase* AllocateAnimationSlot_(gfl2::heap::HeapBase* pAnimationHeap, gfl2::animation::AnimationController::AnimationSlotList& slotList, gfl2::animation::AnimationController::AnimationType type)
  {
    return slotList.GetSlot((gfl2::animation::AnimationController::AnimationType)type);
  }

  template <typename S, typename F, typename A0>
  void SetAnimationForeach_(F func, A0 arg0)
  {
    // 補間処理中か？
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = (m_pAnimationController->IsInterp())
      ? m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex)
      : m_pAnimationController->GetAnimationSlotList(m_SlotIndex);

    for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
    {
      if (!(m_AnimationBit & (1 << i)))
      {
        continue;
      }

      S* pSlot = (S*)this->GetAnimationSlot_(animationSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
      if (pSlot == NULL)
      {
        continue;
      }
      if (!pSlot->HasAnimation())
      {
        continue;
      }

      (pSlot->*func)(arg0);
    }
  }
  template <typename F, typename A0>
  void SetAnimationForeach_(F func, A0 arg0)
  {
    this->SetAnimationForeach_<gfl2::animation::AnimationSlotBase, F, A0>(func, arg0);
  }

  template <typename S, typename F, typename A0, typename A1>
  void SetAnimationForeach_(F func, A0 arg0, A1 arg1)
  {
    // 補間処理中か？
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = (m_pAnimationController->IsInterp())
      ? m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex)
      : m_pAnimationController->GetAnimationSlotList(m_SlotIndex);

    for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
    {
      if (!(m_AnimationBit & (1 << i)))
      {
        continue;
      }

      S* pSlot = (S*)this->GetAnimationSlot_(animationSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
      if (pSlot == NULL)
      {
        continue;
      }
      if (!pSlot->HasAnimation())
      {
        continue;
      }

      (pSlot->*func)(arg0, arg1);
    }
  }
  template <typename F, typename A0, typename A1>
  void SetAnimationForeach_(F func, A0 arg0, A1 arg1)
  {
    this->SetAnimationForeach_<gfl2::animation::AnimationSlotBase, F, A0, A1>(func, arg0, arg1);
  }

  template <typename S, typename T, typename F>
  T GetAnimationForeach_(F func) const
  {
    // 補間処理中か？
    const gfl2::animation::AnimationController::AnimationSlotList animationSlotList = (m_pAnimationController->IsInterp())
      ? m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex)
      : m_pAnimationController->GetAnimationSlotList(m_SlotIndex);

    for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
    {
      if (!(m_AnimationBit & (1 << i)))
      {
        continue;
      }

      const S* pSlot = (const S*)this->GetAnimationSlot_(animationSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
      if (pSlot == NULL)
      {
        continue;
      }
      if (!pSlot->HasAnimation())
      {
        continue;
      }

      return (pSlot->*func)();
    }

    return T();
  }

  template <typename T, typename F>
  T GetAnimationForeach_(F func) const
  {
    return this->GetAnimationForeach_<gfl2::animation::AnimationSlotBase, T, F>(func);
  }

  void SetAnimationResourceForeach_(gfl2::animation::AnimationController::AnimationSlotList& animationSlotList, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
    {
      gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot_(animationSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
      if (pSlot == NULL)
      {
        continue;
      }

      gfl2::heap::HeapBase* pAnimationHeap = this->m_pAnimationController->GetAnimationHeap();
      pSlot->SetAnimationResource(pAnimationHeap, pResourceNode);
    }
  }

  gfl2::animation::AnimationController::AnimationBit GetResourceAnimationBit(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, const gfl2::animation::AnimationController::AnimationSlotList& animationSlotList)
  {
    u32 animationBit = gfl2::animation::AnimationController::ANIMATION_BIT_NONE;

    if (pResourceNode == NULL)
    {
      // 現在スロットが作成されているものだけ
      for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
      {
        u32 checkAnimationBit = 1 << i;

        if (!(m_AnimationBit & checkAnimationBit))
        {
          continue;
        }

        if (animationSlotList.GetSlot(static_cast<gfl2::animation::AnimationController::AnimationType>(i)) == NULL)
        {
          continue;
        }
        
        animationBit |= checkAnimationBit;
      }

      return (gfl2::animation::AnimationController::AnimationBit)animationBit;
    }

    gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = CastAnimationResourceNode(pResourceNode);

    if (pAnimResourceNode == NULL)
    {
      return (gfl2::animation::AnimationController::AnimationBit)animationBit;
    }

    bool hasAnimationList[] =
    {
      pAnimResourceNode->HasJointAnimation(),       //ANIMATION_TYPE_JOINT,               ///< ジョイントアニメーション
      pAnimResourceNode->HasMaterialAnimation(),    //ANIMATION_TYPE_MATERIAL,            ///< マテリアルアニメーション
      pAnimResourceNode->HasVisibilityAnimation(),  //ANIMATION_TYPE_VISIBILITY,          ///< ビジビリティアニメーション
      pAnimResourceNode->HasBlendShapeAnimation(),  //ANIMATION_TYPE_BLENDSHAPE,          ///< ブレンドシェイプアニメーション
      pAnimResourceNode->HasKeyAnimation(),         //ANIMATION_TYPE_KEY,                 ///< キーアニメーション
    };

    for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
    {
      u32 checkAnimationBit = 1 << i;

      if (!(m_AnimationBit & checkAnimationBit))
      {
        continue;
      }

      if (!hasAnimationList[i] && (animationSlotList.GetSlot(static_cast<gfl2::animation::AnimationController::AnimationType>(i)) == NULL))
      {
        continue;
      }
      
      animationBit |= checkAnimationBit;
    }

    return (gfl2::animation::AnimationController::AnimationBit)animationBit;
  }

  void SetAnimationResourceForeach_(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    gfl2::animation::AnimationController::AnimationBit animationBit = this->GetResourceAnimationBit(pResourceNode, m_pAnimationController->GetAnimationSlotList(m_SlotIndex));

    // 通常スロット
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = m_pAnimationController->GetResourceAnimationSlotList(m_SlotIndex, animationBit);

    this->SetAnimationResourceForeach_(animationSlotList, pResourceNode);
  }
  void SetAnimationResourceForeachInterp_(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    gfl2::animation::AnimationController::AnimationBit animationBit = this->GetResourceAnimationBit(pResourceNode, m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex));

    // 補間スロット
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = m_pAnimationController->GetResourceAnimationInterpSlotList(m_SlotIndex, animationBit);

    this->SetAnimationResourceForeach_(animationSlotList, pResourceNode);
  }

  void AllocateAnimationResourceForeach_(gfl2::heap::HeapBase* pAnimationHeap, gfl2::animation::AnimationController::AnimationSlotList& animationSlotList, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    // マテリアルアニメーションだけはリソースを見て動的確保
    gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot_(animationSlotList, gfl2::animation::AnimationController::ANIMATION_TYPE_MATERIAL);
    if (pSlot == NULL)
    {
      return;
    }

    pSlot->SetAnimationResource(pAnimationHeap, pResourceNode);
  }

  void AllocateAnimationResourceForeach_(gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    gfl2::animation::AnimationController::AnimationBit animationBit = this->GetResourceAnimationBit(pResourceNode, m_pAnimationController->GetAnimationSlotList(m_SlotIndex));

    // 通常スロット
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = m_pAnimationController->AllocateAnimationSlotList(pAnimationHeap, m_SlotIndex, animationBit);

    this->AllocateAnimationResourceForeach_(pAnimationHeap, animationSlotList, pResourceNode);
  }
  void AllocateAnimationResourceForeachInterp_(gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    gfl2::animation::AnimationController::AnimationBit animationBit = this->GetResourceAnimationBit(pResourceNode, m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex));

    // 補間スロット
    gfl2::animation::AnimationController::AnimationSlotList animationSlotList = m_pAnimationController->AllocateAnimationInterpSlotList(pAnimationHeap, m_SlotIndex, animationBit);

    this->AllocateAnimationResourceForeach_(pAnimationHeap, animationSlotList, pResourceNode);
  }

  static gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* CastAnimationResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    if (pResourceNode == NULL)
    {
      return NULL;
    }

    gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = NULL;

    // 子供にぶら下がっている可能性がある
    if (pResourceNode->GetChildCount() > 0)
    {
      // 子供がAnimationResourceNode
      pAnimResourceNode = pResourceNode->GetChild()->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
    }
    else
    {
      // 自分がAnimationResourceNode
      pAnimResourceNode = pResourceNode->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
    }

    // キャストに失敗するのはおかしい
    GFL_ASSERT(pAnimResourceNode != NULL);

    return pAnimResourceNode;
  }

  static const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* CastAnimationResourceNode(const gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
  {
    return const_cast<const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode*>(
      CastAnimationResourceNode(const_cast<gfl2::renderingengine::scenegraph::resource::ResourceNode*>(pResourceNode)));
  }

private:

  // アニメーション用
  gfl2::animation::AnimationController*                             m_pAnimationController;
  u32                                                               m_SlotIndex;
  gfl2::animation::AnimationController::AnimationBit                m_AnimationBit;
};

}}

#endif
