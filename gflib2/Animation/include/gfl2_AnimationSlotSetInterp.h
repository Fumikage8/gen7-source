#ifndef GFLIB2_ANIMATION_SLOT_SET_INTERP_H_INCLUDED
#define GFLIB2_ANIMATION_SLOT_SET_INTERP_H_INCLUDED

#include <Animation/include/gfl2_AnimationSlotSet.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

namespace gfl2 { namespace animation {

template <class T>
class AnimationSlotSetInterp
{
public:

  /**
   * @brief コンストラクタ
   */
  AnimationSlotSetInterp() :
    m_CurrentSlotIndex(0),
    m_TotalInterpFrame(-1),
    m_CurrentInterpFrame(0),
    m_InterpStepFrame(1)
  {
  }

  /**
   * @brief デストラクタ
   */
  ~AnimationSlotSetInterp()
  {
    for (u32 i = 0; i < MAX_SLOT_SET_COUNT; ++i)
    {
      m_SlotSetList[i].Finalize();
    }
  }

  /**
   * @brief 初期化
   */
  void Initialize()
  {
    for (u32 i = 0; i < MAX_SLOT_SET_COUNT; ++i)
    {
      m_SlotSetList[i].Initialize();
    }
  }

  /**
   * @brief 終了
   */
  void Finalize()
  {
    for (u32 i = 0; i < MAX_SLOT_SET_COUNT; ++i)
    {
      m_SlotSetList[i].Finalize();
    }
  }

  /**
   * @brief スロット取得：足りなければスロット生成
   *
   * @param pHeap メモリ確保に使用するヒープ
   * @param pTargetNode ターゲットノード
   * @param slotIndex スロットインデックス
   * @param slotType スロットタイプ
   *
   * @return スロットを返す。
   */
  T* GetSlotOrCreate(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::TransformNode* pTargetNode, u32 slotIndex, u32 slotType)
  {
    u32 slotSetIndex = m_CurrentSlotIndex ^ slotType;
    return m_SlotSetList[slotSetIndex].GetSlotOrCreate(pHeap, pTargetNode, slotIndex);
  }

  /**
   * @brief スロット取得
   *
   * @param slotIndex スロットインデックス
   * @param slotType スロットタイプ
   *
   * @return スロットを返す。無効なインデックスならNULLを返す
   */
  const T* GetSlot(u32 slotIndex, u32 slotType) const
  {
    u32 slotSetIndex = m_CurrentSlotIndex ^ slotType;
    return m_SlotSetList[slotSetIndex].GetSlot(slotIndex);
  }

  /**
   * @brief アニメーションの補間リクエスト
   *
   * @param interpFrame 補間フレーム数
   */
  void InterpRequest(f32 interpFrame)
  {
    m_TotalInterpFrame = interpFrame;
    m_CurrentInterpFrame = 0.0f;
  }

  /**
   * @brief アニメーションの補間中か？
   *
   * @return true:補間中, false:通常再生中
   */
  bool IsInterp() const
  {
    return (m_TotalInterpFrame > 0);
  }

  /**
   * @brief スロット数取得
   *
   * @return スロット数を返す。
   */
  u32 GetSlotCount() const
  {
    if (this->IsInterp())
    {
      return m_SlotSetList[m_CurrentSlotIndex ^ 1].GetSlotCount();
    }

    return m_SlotSetList[m_CurrentSlotIndex].GetSlotCount();
  }

  /**
   * @brief スロット削除
   *
   * @param slotIndex スロットインデックス
   */
  void DeleteSlot(u32 slotIndex)
  {
    if (this->IsInterp())
    {
      m_SlotSetList[m_CurrentSlotIndex ^ 1].DeleteSlot();
      return;
    }

    m_SlotSetList[m_CurrentSlotIndex].DeleteSlot();
  }

  /**
   * @brief フレーム更新
   */
  void UpdateFrame()
  {
    m_SlotSetList[m_CurrentSlotIndex].UpdateFrame();

    if (this->IsInterp())
    {
      m_SlotSetList[m_CurrentSlotIndex ^ 1].UpdateFrame();
    }
  }

  /**
   * @brief 補間フレーム更新
   */
  f32 UpdateInterpFrame()
  {
    // 補間中で無いなら終了
    if (!this->IsInterp())
    {
      return -1;
    }

    // 補間フレームの更新
    m_CurrentInterpFrame += m_InterpStepFrame;

    // 補間終了判定
    if (m_CurrentInterpFrame >= m_TotalInterpFrame)
    {
      // 補間終了
      m_TotalInterpFrame = -1;

      // スロット入れ替え
      m_CurrentSlotIndex ^= 1;

      return -1;
    }

    return (m_CurrentInterpFrame / m_TotalInterpFrame);
  }

  /**
   * @brief 補間率取得
   */
  f32 GetInterpWeight()
  {
    if (!this->IsInterp)
    {
      return 0.0f;
    }

    return m_CurrentInterpFrame / m_TotalInterpFrame;
  }

  //========================================================================
  // ここからタイプ毎の処理

  /**
   * @brief ジョイントトランスフォームを取得
   *
   * @param out_pJointTransform 出力用のバッファ
   * @param jointIndex ジョイントインデックス
   */
  void GetJointSRT(math::SRT* out_pJointSRT, u32 jointIndex)
  {
    m_SlotSetList[m_CurrentSlotIndex].GetJointSRT(out_pJointSRT, jointIndex);
  }

  /**
   * @brief マテリアルを適用
   *
   * @param materialIndex マテリアルインデックス
   */
  void ApplyMaterial(u32 materialIndex)
  {
    m_SlotSetList[m_CurrentSlotIndex].ApplyMaterial(materialIndex);
  }

  /**
   * @brief ビジビリティを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyVisibility(u32 meshIndex)
  {
    m_SlotSetList[m_CurrentSlotIndex].ApplyVisibility(meshIndex);
  }

  /**
   * @brief ブレンドシェイプを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyBlendShape(u32 meshIndex)
  {
    m_SlotSetList[m_CurrentSlotIndex].ApplyBlendShape(meshIndex);
  }

  /**
   * @brief キーを適用
   */
  void ApplyKey()
  {
    m_SlotSetList[m_CurrentSlotIndex].ApplyKey();
  }

  /**
   * @brief ライトを適用：補間なし
   *
   * @param pDrawEnvNode ライトを適用する描画環境ノード
   */
  void ApplyLight(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
  {
    for (u32 lightSetIndex = 0; lightSetIndex < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax; ++lightSetIndex)
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSet = pDrawEnvNode->GetLightSet(lightSetIndex);

      if (pLightSet == NULL)
      {
        continue;
      }

      for (u32 lightIndex = 0; lightIndex < pLightSet->GetLightNum(); ++lightIndex)
      {
        gfl2::renderingengine::scenegraph::instance::LightNode* pLightNode = pLightSet->GetLightNode(lightIndex);

        if (pLightNode == NULL)
        {
          continue;
        }

        f32 valueList[LightDataSet::INDEX_COUNT] = 
        {
          pLightNode->GetPosition().x,
          pLightNode->GetPosition().y,
          pLightNode->GetPosition().z,

          pLightNode->GetDirection().x,
          pLightNode->GetDirection().y,
          pLightNode->GetDirection().z,

          pLightNode->GetColor().x,
          pLightNode->GetColor().y,
          pLightNode->GetColor().z,

          pLightNode->GetConeAngle(),
          pLightNode->GetIntensity(),
        };

        // ライト適用
        m_SlotSetList[m_CurrentSlotIndex].ApplyLight(valueList, lightSetIndex, lightIndex);

        pLightNode->SetPosition(math::Vector3(valueList[LightDataSet::POSITION_X], valueList[LightDataSet::POSITION_Y], valueList[LightDataSet::POSITION_Z]));
        pLightNode->SetDirection(math::Vector3(valueList[LightDataSet::DIRECTION_X], valueList[LightDataSet::DIRECTION_Y], valueList[LightDataSet::DIRECTION_Z]));
        pLightNode->SetColor(math::Vector4(valueList[LightDataSet::COLOR_R], valueList[LightDataSet::COLOR_G], valueList[LightDataSet::COLOR_B], 1.0f));
        pLightNode->SetConeAngle(valueList[LightDataSet::CONE_ANGLE]);
        pLightNode->SetIntensity(valueList[LightDataSet::INTENSITY]);
      }
    }
  }

  /**
   * @brief ライトを適用：補間あり
   *
   * @param pDrawEnvNode ライトを適用する描画環境ノード
   * @param interpRate 補間率
   */
  void ApplyLightInterp(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, f32 interpRate)
  {
    u32 interpSlotIndex = m_CurrentSlotIndex ^ 1;

    for (u32 lightSetIndex = 0; lightSetIndex < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax; ++lightSetIndex)
    {
      gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSet = pDrawEnvNode->GetLightSet(lightSetIndex);

      if (pLightSet == NULL)
      {
        continue;
      }

      for (u32 lightIndex = 0; lightIndex < pLightSet->GetLightNum(); ++lightIndex)
      {
        gfl2::renderingengine::scenegraph::instance::LightNode* pLightNode = pLightSet->GetLightNode(lightIndex);

        if (pLightNode == NULL)
        {
          continue;
        }

        f32 valueList[LightDataSet::INDEX_COUNT] = 
        {
          pLightNode->GetPosition().x,
          pLightNode->GetPosition().y,
          pLightNode->GetPosition().z,

          pLightNode->GetDirection().x,
          pLightNode->GetDirection().y,
          pLightNode->GetDirection().z,

          pLightNode->GetColor().x,
          pLightNode->GetColor().y,
          pLightNode->GetColor().z,

          pLightNode->GetConeAngle(),
          pLightNode->GetIntensity(),
        };
        f32 valueListInterp[LightDataSet::INDEX_COUNT] = 
        {
          pLightNode->GetPosition().x,
          pLightNode->GetPosition().y,
          pLightNode->GetPosition().z,

          pLightNode->GetDirection().x,
          pLightNode->GetDirection().y,
          pLightNode->GetDirection().z,

          pLightNode->GetColor().x,
          pLightNode->GetColor().y,
          pLightNode->GetColor().z,

          pLightNode->GetConeAngle(),
          pLightNode->GetIntensity(),
        };

        // ライト適用
        m_SlotSetList[m_CurrentSlotIndex].ApplyLight(valueList, lightSetIndex, lightIndex);
        m_SlotSetList[interpSlotIndex].ApplyLight(valueListInterp, lightSetIndex, lightIndex);

        for (u32 i = 0; i < LightDataSet::INDEX_COUNT; ++i)
        {
          valueList[i] = gfl2::math::Lerp(valueList[i], valueListInterp[i], interpRate);
        }

        pLightNode->SetPosition(math::Vector3(valueList[LightDataSet::POSITION_X], valueList[LightDataSet::POSITION_Y], valueList[LightDataSet::POSITION_Z]));
        pLightNode->SetDirection(math::Vector3(valueList[LightDataSet::DIRECTION_X], valueList[LightDataSet::DIRECTION_Y], valueList[LightDataSet::DIRECTION_Z]));
        pLightNode->SetColor(math::Vector4(valueList[LightDataSet::COLOR_R], valueList[LightDataSet::COLOR_G], valueList[LightDataSet::COLOR_B], 1.0f));
        pLightNode->SetConeAngle(valueList[LightDataSet::CONE_ANGLE]);
        pLightNode->SetIntensity(valueList[LightDataSet::INTENSITY]);
      }
    }
  }

  /**
   * @brief カメラを適用：補間なし
   *
   * @param pDrawEnvNode ライトを適用する描画環境ノード
   */
  void ApplyCamera(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
  {
    for (u32 cameraIndex = 0; cameraIndex < pDrawEnvNode->GetCameraNum(); ++cameraIndex)
    {
      gfl2::renderingengine::scenegraph::instance::CameraNode* pCameraNode = pDrawEnvNode->GetCamera(cameraIndex);

      // カメラノードが無ければ次へ
      if (pCameraNode == NULL)
      {
        continue;
      }

      f32 valueList[CameraDataSet::INDEX_COUNT] =
      {
        pCameraNode->GetNear(),
        pCameraNode->GetFar(),
        pCameraNode->GetFovy(),
        pCameraNode->GetAspectRatio(),

        pCameraNode->GetLocalRotation().x,
        pCameraNode->GetLocalRotation().y,
        pCameraNode->GetLocalRotation().z,

        pCameraNode->GetLocalTranslate().x,
        pCameraNode->GetLocalTranslate().y,
        pCameraNode->GetLocalTranslate().z,
      };

      m_SlotSetList[m_CurrentSlotIndex].ApplyCamera(valueList, cameraIndex);

      pCameraNode->SetNear(valueList[CameraDataSet::CAM_NEAR]);
      pCameraNode->SetFar(valueList[CameraDataSet::CAM_FAR]);
      pCameraNode->SetFovy(valueList[CameraDataSet::CAM_FOVY]);
      pCameraNode->SetAspectRatio(valueList[CameraDataSet::CAM_ASPECT_RATIO]);
      math::Quaternion rot;
      rot.MatrixToQuaternion(math::Matrix34::GetRotationZYX(valueList[CameraDataSet::ROTATE_Z], valueList[CameraDataSet::ROTATE_Y], valueList[CameraDataSet::ROTATE_X]));
      pCameraNode->SetLocalRotation(rot);
      pCameraNode->SetLocalTranslate(math::Vector3(valueList[CameraDataSet::TRANSLATE_X], valueList[CameraDataSet::TRANSLATE_Y], valueList[CameraDataSet::TRANSLATE_Z]));
    }
  }

  /**
   * @brief カメラを適用：補間あり
   *
   * @param pDrawEnvNode ライトを適用する描画環境ノード
   * @param interpRate 補間率
   */
  void ApplyCameraInterp(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, f32 interpRate)
  {
    u32 interpSlotIndex = m_CurrentSlotIndex ^ 1;

    for (u32 cameraIndex = 0; cameraIndex < pDrawEnvNode->GetCameraNum(); ++cameraIndex)
    {
      gfl2::renderingengine::scenegraph::instance::CameraNode* pCameraNode = pDrawEnvNode->GetCamera(cameraIndex);

      // カメラノードが無ければ次へ
      if (pCameraNode == NULL)
      {
        continue;
      }

      f32 valueList[CameraDataSet::INDEX_COUNT] =
      {
        pCameraNode->GetNear(),
        pCameraNode->GetFar(),
        pCameraNode->GetFovy(),
        pCameraNode->GetAspectRatio(),

        pCameraNode->GetLocalRotation().x,
        pCameraNode->GetLocalRotation().y,
        pCameraNode->GetLocalRotation().z,

        pCameraNode->GetLocalTranslate().x,
        pCameraNode->GetLocalTranslate().y,
        pCameraNode->GetLocalTranslate().z,
      };
      f32 valueListInterp[CameraDataSet::INDEX_COUNT] =
      {
        pCameraNode->GetNear(),
        pCameraNode->GetFar(),
        pCameraNode->GetFovy(),
        pCameraNode->GetAspectRatio(),

        pCameraNode->GetLocalRotation().x,
        pCameraNode->GetLocalRotation().y,
        pCameraNode->GetLocalRotation().z,

        pCameraNode->GetLocalTranslate().x,
        pCameraNode->GetLocalTranslate().y,
        pCameraNode->GetLocalTranslate().z,
      };

      m_SlotSetList[m_CurrentSlotIndex].ApplyCamera(valueList, cameraIndex);
      m_SlotSetList[interpSlotIndex].ApplyCamera(valueListInterp, cameraIndex);

      for (u32 i = 0; i < CameraDataSet::INDEX_COUNT; ++i)
      {
        valueList[i] = gfl2::math::Lerp(valueList[i], valueListInterp[i], interpRate);
      }

      pCameraNode->SetNear(valueList[CameraDataSet::CAM_NEAR]);
      pCameraNode->SetFar(valueList[CameraDataSet::CAM_FAR]);
      pCameraNode->SetFovy(valueList[CameraDataSet::CAM_FOVY]);
      pCameraNode->SetAspectRatio(valueList[CameraDataSet::CAM_ASPECT_RATIO]);
      math::Quaternion rot;
      rot.MatrixToQuaternion(math::Matrix34::GetRotationZYX(valueList[CameraDataSet::ROTATE_Z], valueList[CameraDataSet::ROTATE_Y], valueList[CameraDataSet::ROTATE_X]));
      pCameraNode->SetLocalRotation(rot);
      pCameraNode->SetLocalTranslate(math::Vector3(valueList[CameraDataSet::TRANSLATE_X], valueList[CameraDataSet::TRANSLATE_Y], valueList[CameraDataSet::TRANSLATE_Z]));
    }
  }

  /**
   * @brief フォグを適用：補間なし
   *
   * @param pDrawEnvNode フォグを適用する描画環境ノード
   */
  void ApplyFog(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
  {
    f32 valueList[FogDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetFogNearLength(),
      pDrawEnvNode->GetFogFarLength(),
      pDrawEnvNode->GetFogColor().x,
      pDrawEnvNode->GetFogColor().y,
      pDrawEnvNode->GetFogColor().z,
      pDrawEnvNode->GetFogStrength(),
    };

    u32 fogIndex = 0; // nijiは0番固定
    m_SlotSetList[m_CurrentSlotIndex].ApplyFog(valueList, fogIndex);

    pDrawEnvNode->SetFogNearLength(valueList[FogDataSet::NEAR_LENGTH]);
    pDrawEnvNode->SetFogFarLength(valueList[FogDataSet::FAR_LENGTH]);
    pDrawEnvNode->SetFogStrength(valueList[FogDataSet::STRENGTH]);
    math::Vector4 color(
        valueList[FogDataSet::COLOR_R],
        valueList[FogDataSet::COLOR_G],
        valueList[FogDataSet::COLOR_B],
        1.0f
    );
    pDrawEnvNode->SetFogColor(color);
  }

  /**
   * @brief フォグを適用：補間あり
   *
   * @param pDrawEnvNode フォグを適用する描画環境ノード
   * @param interpRate 補間率
   */
  void ApplyFogInterp(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, f32 interpRate)
  {
    f32 valueList[FogDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetFogNearLength(),
      pDrawEnvNode->GetFogFarLength(),
      pDrawEnvNode->GetFogColor().x,
      pDrawEnvNode->GetFogColor().y,
      pDrawEnvNode->GetFogColor().z,
      pDrawEnvNode->GetFogStrength(),
    };
    f32 valueListInterp[FogDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetFogNearLength(),
      pDrawEnvNode->GetFogFarLength(),
      pDrawEnvNode->GetFogColor().x,
      pDrawEnvNode->GetFogColor().y,
      pDrawEnvNode->GetFogColor().z,
      pDrawEnvNode->GetFogStrength(),
    };

    u32 interpSlotIndex = m_CurrentSlotIndex ^ 1;
    u32 fogIndex = 0; // nijiは0番固定
    m_SlotSetList[m_CurrentSlotIndex].ApplyFog(valueList, fogIndex);
    m_SlotSetList[interpSlotIndex].ApplyFog(valueListInterp, fogIndex);

    for (u32 i = 0; i < FogDataSet::INDEX_COUNT; ++i)
    {
      valueList[i] = gfl2::math::Lerp(valueList[i], valueListInterp[i], interpRate);
    }

    pDrawEnvNode->SetFogNearLength(valueList[FogDataSet::NEAR_LENGTH]);
    pDrawEnvNode->SetFogFarLength(valueList[FogDataSet::FAR_LENGTH]);
    pDrawEnvNode->SetFogStrength(valueList[FogDataSet::STRENGTH]);
    math::Vector4 color(
        valueList[FogDataSet::COLOR_R],
        valueList[FogDataSet::COLOR_G],
        valueList[FogDataSet::COLOR_B],
        1.0f
    );
    pDrawEnvNode->SetFogColor(color);
  }

  /**
   * @brief グレア(ブルーム)を適用：補間なし
   *
   * @param pDrawEnvNode グレアを適用する描画環境ノード
   * @note  グレア(ブルーム)は1個しかないのでインデックスは必要ない。
   */
  void ApplyGlare(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
  {
    f32 valueList[GlareDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetGlareStrength(),
      pDrawEnvNode->GetGlareRange(),
      pDrawEnvNode->GetGlareIntensity(),
      pDrawEnvNode->GetGlareWeight().x,
      pDrawEnvNode->GetGlareWeight().y,
      pDrawEnvNode->GetGlareWeight().z,
    };

    m_SlotSetList[m_CurrentSlotIndex].ApplyGlare(valueList);

    pDrawEnvNode->SetGlareStrength(valueList[GlareDataSet::GLARE_STRENGTH]);
    pDrawEnvNode->SetGlareRange(valueList[GlareDataSet::GLARE_RANGE]);
    pDrawEnvNode->SetGlareIntensity(valueList[GlareDataSet::GLARE_INTENSITY]);
    math::Vector4 weight(
        valueList[GlareDataSet::GLARE_WEIGHT_R],
        valueList[GlareDataSet::GLARE_WEIGHT_G],
        valueList[GlareDataSet::GLARE_WEIGHT_B],
        1.0f
    );
    pDrawEnvNode->SetGlareWeight(weight);
  }

  /**
   * @brief グレア(ブルーム)を適用：補間あり
   *
   * @param pDrawEnvNode グレアを適用する描画環境ノード
   * @param interpRate 補間率
   * @note  グレア(ブルーム)は1個しかないのでインデックスは必要ない。
   */
  void ApplyGlareInterp(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, f32 interpRate)
  {
    f32 valueList[GlareDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetGlareStrength(),
      pDrawEnvNode->GetGlareRange(),
      pDrawEnvNode->GetGlareIntensity(),
      pDrawEnvNode->GetGlareWeight().x,
      pDrawEnvNode->GetGlareWeight().y,
      pDrawEnvNode->GetGlareWeight().z,
    };
    f32 valueListInterp[GlareDataSet::INDEX_COUNT] = 
    {
      pDrawEnvNode->GetGlareStrength(),
      pDrawEnvNode->GetGlareRange(),
      pDrawEnvNode->GetGlareIntensity(),
      pDrawEnvNode->GetGlareWeight().x,
      pDrawEnvNode->GetGlareWeight().y,
      pDrawEnvNode->GetGlareWeight().z,
    };

    u32 interpSlotIndex = m_CurrentSlotIndex ^ 1;
    m_SlotSetList[m_CurrentSlotIndex].ApplyGlare(valueList);
    m_SlotSetList[interpSlotIndex].ApplyGlare(valueListInterp);

    for (u32 i = 0; i < GlareDataSet::INDEX_COUNT; ++i)
    {
      valueList[i] = gfl2::math::Lerp(valueList[i], valueListInterp[i], interpRate);
    }

    pDrawEnvNode->SetGlareStrength(valueList[GlareDataSet::GLARE_STRENGTH]);
    pDrawEnvNode->SetGlareRange(valueList[GlareDataSet::GLARE_RANGE]);
    pDrawEnvNode->SetGlareIntensity(valueList[GlareDataSet::GLARE_INTENSITY]);
    math::Vector4 weight(
        valueList[GlareDataSet::GLARE_WEIGHT_R],
        valueList[GlareDataSet::GLARE_WEIGHT_G],
        valueList[GlareDataSet::GLARE_WEIGHT_B],
        1.0f
    );
    pDrawEnvNode->SetGlareWeight(weight);
  }

  /**
   * @brief 歩き用座標を取得：ポケモン用の拡張データ
   *
   * @param frame 取得するフレーム数
   *
   * @return 位置座標
   */
  math::Vector3 GetWalkPosition(f32 frame) const
  {
    return m_SlotSetList[m_CurrentSlotIndex].GetWalkPosition(frame);
  }

  /**
   * @brief 歩き用に現在のフレームと再生速度を取得
   *
   * @param out_pFrame 出力用：フレーム
   * @param out_pStepFrame 出力用：再生速度
   */
  void GetWalkFrameAndStepFrame(f32* out_pFrame, f32* out_pStepFrame) const
  {
    m_SlotSetList[m_CurrentSlotIndex].GetWalkFrameAndStepFrame(out_pFrame, out_pStepFrame);
  }

  /**
   * @brief 歩き用にフレームを補正
   *
   * @param frame フレーム
   * @param stepFrame 再生速度
   *
   * @return 補正されたフレームを返す
   */
  f32 GetWalkAdjustFrame(f32 frame, f32 stepFrame) const
  {
    return m_SlotSetList[m_CurrentSlotIndex].GetWalkAdjustFrame(frame, stepFrame);
  }


private:

  static const u32 MAX_SLOT_SET_COUNT = 2;                ///< 最大スロットセット数
  AnimationSlotSet<T> m_SlotSetList[MAX_SLOT_SET_COUNT];  ///< スロットセットリスト
  u32 m_CurrentSlotIndex;                                 ///< 操作対象のスロットインデックス
  f32 m_TotalInterpFrame;                                 ///< 合計補間フレーム数
  f32 m_CurrentInterpFrame;                               ///< 現在の補間フレーム
  f32 m_InterpStepFrame;                                  ///< １フレームで補間フレームを進める速度
};

}}

#endif