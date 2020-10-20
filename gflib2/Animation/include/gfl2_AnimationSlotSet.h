#ifndef GFLIB2_ANIMATION_SLOT_SET_H_INCLUDED
#define GFLIB2_ANIMATION_SLOT_SET_H_INCLUDED

#include <Animation/include/gfl2_AnimationSlotBase.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace animation {

template <class T>
class AnimationSlotSet
{
public:

  /**
   * @brief コンストラクタ
   */
  AnimationSlotSet() :
    m_SlotCount(0)
  {
  }

  /**
   * @brief デストラクタ
   */
  ~AnimationSlotSet()
  {
    this->Finalize();
  }

  /**
   * @brief 初期化
   */
  void Initialize()
  {
    // 全スロット初期化
    for (u32 i = 0; i < MAX_SLOT_COUNT; ++i)
    {
      m_pSlotList[i] = NULL;
    }
    m_SlotCount = 0;
  }

  /**
   * @brief 終了
   */
  void Finalize()
  {
    // スロット開放
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      this->DeleteSlot(i);
    }

    m_SlotCount = 0;
  }

  /**
   * @brief スロット取得：足りなければスロット生成
   *
   * @param pHeap メモリ確保に使用するヒープ
   * @param pTargetNode ターゲットノード
   * @param slotIndex スロットインデックス
   *
   * @return スロットを返す。
   */
  T* GetSlotOrCreate(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::TransformNode* pTargetNode, u32 slotIndex)
  {
    // 最大スロット数チェック
    GFL_ASSERT(slotIndex < MAX_SLOT_COUNT);

    // スロット数変更
    if (slotIndex >= m_SlotCount)
    {
      // 追加のスロット確保は禁止
      // 一度でもAllocate()で確保されていたら動的確保を禁止するためにpHeapはNULLになる。
      // ここで引っかかる場合はAllocate()処理を見直す必要がある。
      GFL_ASSERT(pHeap != NULL);
      if (pHeap == NULL)
      {
        return NULL;
      }

      m_SlotCount = slotIndex + 1;
    }

    // スロットが無ければ作成
    if (m_pSlotList[slotIndex] == NULL)
    {
      // ヒープが指定されていないので確保できない
      if (pHeap == NULL)
      {
        return NULL;
      }

      T* pSlot = GFL_NEW(pHeap) T();
      pSlot->Initialize(pHeap, pTargetNode);
      m_pSlotList[slotIndex] = pSlot;
    }

    return m_pSlotList[slotIndex];
  }

  /**
   * @brief スロット取得
   *
   * @param slotIndex スロットインデックス
   *
   * @return スロットを返す。無効なインデックスならNULLを返す
   */
  const T* GetSlot(u32 slotIndex) const
  {
    // const呼び出しなので足りなければNULL
    if (slotIndex >= m_SlotCount)
    {
      return NULL;
    }

    return m_pSlotList[slotIndex];
  }

  /**
   * @brief スロット数取得
   *
   * @return スロット数を返す。
   */
  u32 GetSlotCount() const
  {
    return m_SlotCount;
  }

  /**
   * @brief スロット削除
   *
   * @param slotIndex スロットインデックス
   */
  void DeleteSlot(u32 slotIndex)
  {
    if (slotIndex >= m_SlotCount)
    {
      return;
    }

    if (m_pSlotList[slotIndex] == NULL)
    {
      return;
    }

    m_pSlotList[slotIndex]->Finalize();
    GFL_SAFE_DELETE(m_pSlotList[slotIndex]);
  }

  /**
   * @brief フレーム更新
   */
  void UpdateFrame()
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->UpdateFrame();
    }
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
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->GetJointSRT(out_pJointSRT, jointIndex);
    }
  }

  /**
   * @brief 線形補間用に前回のSRTを保存
   */
  void SetOldJointSRT(u32 jointIndex, const math::SRT& oldSRT)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->SetOldSRT(jointIndex, oldSRT);
      break;
    }
  }

  /**
   * @brief 線形補間用に前回のSRTを取得
   */
  void GetOldJointSRT(u32 jointIndex, math::SRT* out_pOldSRT)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->GetOldSRT(jointIndex, out_pOldSRT);
      break;
    }
  }

  /**
   * @brief マテリアルを適用
   *
   * @param materialIndex マテリアルインデックス
   */
  void ApplyMaterial(u32 materialIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyMaterial(materialIndex);
    }
  }

  /**
   * @brief ビジビリティを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyVisibility(u32 meshIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyVisibility(meshIndex);
    }
  }

  /**
   * @brief ブレンドシェイプを適用
   *
   * @param meshIndex メッシュインデックス
   */
  void ApplyBlendShape(u32 meshIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyBlendShape(meshIndex);
    }
  }

  /**
   * @brief キーを適用
   */
  void ApplyKey()
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyKey();
    }
  }

  /**
   * @brief ライトを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param lightSetIndex ライトセットインデックス
   * @param lightIndex ライトインデックス
   */
  void ApplyLight(f32* out_pValueList, u32 lightSetIndex, u32 lightIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyLight(out_pValueList, lightSetIndex, lightIndex);
    }
  }

  /**
   * @brief カメラを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param cameraIndex カメラインデックス
   */
  void ApplyCamera(f32* out_pValueList, u32 cameraIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyCamera(out_pValueList, cameraIndex);
    }
  }

  //kawa20150514
  /**
   * @brief フォグを適用
   *
   * @param out_pValueList 出力する値リスト
   * @param fogIndex フォグインデックス
   */
  void ApplyFog(f32* out_pValueList, u32 fogIndex)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyFog(out_pValueList, fogIndex);
    }
  }
  /**
   * @brief グレア(ブルーム)を適用
   *
   * @param out_pValueList 出力する値リスト
   * @note  グレア(ブルーム)は1個しかないのでインデックスは必要ない。
   */
  void ApplyGlare(f32* out_pValueList)
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->ApplyGlare(out_pValueList);
    }
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
    math::Vector3 pos(0.0f, 0.0f, 0.0f);
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      if (m_pSlotList[i] == NULL)
      {
        continue;
      }
      m_pSlotList[i]->GetWalkPosition(&pos, frame);
    }
    return pos;
  }

  /**
   * @brief 歩き用に現在のフレームと再生速度を取得
   *
   * @param out_pFrame 出力用：フレーム
   * @param out_pStepFrame 出力用：再生速度
   */
  void GetWalkFrameAndStepFrame(f32* out_pFrame, f32* out_pStepFrame) const
  {
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      const T* pSlot = m_pSlotList[i];
      if (pSlot != NULL)
      {
        *out_pFrame = pSlot->GetOldFrame();
        *out_pStepFrame = pSlot->GetStepFrame();
        break;
      }
    }
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
    // 速度が０なら何もしない
    if (stepFrame == 0.0f)
    {
      return frame;
    }

    bool isDataLoop = false;
    u32 frameCount = 0;
    for (u32 i = 0; i < m_SlotCount; ++i)
    {
      const T* pSlot = m_pSlotList[i];
      if (pSlot != NULL)
      {
        isDataLoop = pSlot->IsDataLoop();
        frameCount = pSlot->GetFrameCount();
        break;
      }
    }

    // 順再生
    if (stepFrame > 0.0f)
    {
      if (frame <= 0.0f)
      {
        if (isDataLoop)
        {
          return frameCount;
        }
        else
        {
          return stepFrame;
        }
      }
    }
    // 逆再生
    else
    {
      if (frame >= frameCount)
      {
        if (isDataLoop)
        {
          return 0.0f;
        }
        else
        {
          return frameCount + stepFrame;
        }
      }
    }

    // ループフレームに設定

    return frame;
  }


private:

  static const s32 MAX_SLOT_COUNT = 12; ///< 最大スロット数(nijiでは12個, GFModelViewerでは11個使用)
  T* m_pSlotList[MAX_SLOT_COUNT];       ///< スロットリスト
  u32 m_SlotCount;                      ///< スロット数
};

}}

#endif