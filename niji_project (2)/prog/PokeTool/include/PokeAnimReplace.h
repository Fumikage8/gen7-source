//======================================================================
/**
 * @file	PokeAnimReplace.h
 * @brief	ポケモン用表情アニメ上書き用のクラス
 * @author	takebe_makoto
 * @data	16/04/05
 */
//======================================================================
#if !defined( __POKE_ANIM_REPLACE__ )
#define __POKE_ANIM_REPLACE__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <Animation/include/gfl2_JointAnimationSlot.h>

GFL_NAMESPACE_BEGIN(PokeTool)

/**
 * @brief ポケモン用表情アニメ上書き用のクラス
 */
class PokeAnimReplace
{
public:

  /**
   * @brief 更新状態
   */
  enum State
  {
    STATE_NONE,           ///< 未初期化
    STATE_STAY,           ///< 待機中
    STATE_BEGIN_SMOOTH,   ///< 上書き開始時の補間
    STATE_REPLACE,        ///< 上書き中
    STATE_END_SMOOTH      ///< 上書き終了時の補間
  };

public:

  /**
   * @brief コンストラクタ
   */
  PokeAnimReplace();

  /**
   * @brief デストラクタ
   */
  ~PokeAnimReplace();

  /**
   * @brief 初期化
   *
   * @param pHeap ヒープ
   * @param pModelNode モデルノード
   * @param pResourceNode リソースノード
   */
  void Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief 終了
   */
  void Finalize();

  /**
   * @brief 補間しつつ上書き開始
   *
   * @param frame 初期フレーム, マイナスの値を指定すると上書き終了
   * @param stepFrame 再生速度
   * @param smoothFrame 補間フレーム数
   */
  void ReplaceSmooth(f32 frame, f32 stepFrame, u32 smoothFrame);

  /**
   * @brief フレームを取得
   *
   * @return アニメーションが設定されているなら現在のフレーム数を返す、設定されていないなら-1.0fを返す
   */
  f32 GetFrame() const;

  /**
   * @brief 上書き更新
   */
  void UpdateReplace();

private:

  // 補間フレーム更新
  bool UpdateBlendFrame();

  // 補間率取得
  f32 GetBlendWeight() const;

private:

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelNode;   ///< 上書き対象のモデル
  gfl2::animation::JointAnimationSlot* m_pJointSlot;                              ///< アニメーション再生に使用するスロット
  u32 m_ReplaceCount;                                                             ///< 上書きするジョイント数
  u32* m_pReplaceIndexList;                                                       ///< 上書きするジョイント番号リスト
  gfl2::math::SRT* m_pSRTBlendBufferList;                                         ///< 補間用に前回のSRTを保存しておくバッファ
  u32 m_SmoothFrameCurrent;                                                       ///< 現在の補間フレーム数
  u32 m_SmoothFrameTotal;                                                         ///< 補間にかける合計フレーム数
  State m_State;                                                                  ///< 現在の更新状態
};

GFL_NAMESPACE_END(PokeTool)


#endif //__POKE_ANIM_REPLACE__
