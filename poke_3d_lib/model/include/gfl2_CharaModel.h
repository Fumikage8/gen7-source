#if !defined(POKE_3D_LIB_MODEL_CHARA_MODEL_H_INCLUDED)
#define POKE_3D_LIB_MODEL_CHARA_MODEL_H_INCLUDED

#include "gfl2_BaseModel.h"

namespace poke_3d { namespace model {

/**
 * @brief キャラクター用モデルクラス
 */
class CharaModel : public BaseModel
{
public:

  /**
   * @brief 目のインデックス定義
   */
  enum EyeIndex
  {
    EYE_INDEX_NONE,                           ///< 指定無し、再生中のアニメーションに連動

    EYE_INDEX_OPEN,                           ///< 全開き
    EYE_INDEX_HALF_OPEN,                      ///< 半開き
    EYE_INDEX_CLOSE,                          ///< 閉じる

    EYE_INDEX_UNIQUE0,                        ///< ユニーク0
    EYE_INDEX_UNIQUE1,                        ///< ユニーク1
    EYE_INDEX_UNIQUE2,                        ///< ユニーク2
    EYE_INDEX_UNIQUE3,                        ///< ユニーク3
    EYE_INDEX_UNIQUE4,                        ///< ユニーク4
    EYE_INDEX_UNIQUE5,                        ///< ユニーク5
    EYE_INDEX_UNIQUE6,                        ///< ユニーク6

    // 喜怒哀楽があるモデルではユニーク0～3が相当する
    EYE_INDEX_DELIGHT = EYE_INDEX_UNIQUE0,    ///< 喜
    EYE_INDEX_ANGER = EYE_INDEX_UNIQUE1,      ///< 怒
    EYE_INDEX_SORROW = EYE_INDEX_UNIQUE2,     ///< 哀
    EYE_INDEX_PLEASURE = EYE_INDEX_UNIQUE3    ///< 楽
  };

  /**
   * @brief 自動目パチのモード定義
   */
  enum AutoBlinkMode
  {
    AUTO_BLINK_MODE_NONE,                     ///< 自動目パチ無し

    AUTO_BLINK_MODE_FIELD,                    ///< フィールド用
    AUTO_BLINK_MODE_BATTLE                    ///< バトル用
  };

  /**
   * @brief 目のアニメーションスロット番号
   */
  static const s32 EYE_ANIMATION_SLOT = 9;

  /**
   * @brief 目のアニメーションフレーム間隔
   */
  static const s32 EYE_ANIMATION_FRAME_INTERVAL = 10;


  /**
   * @brief 口のインデックス定義
   */
  enum MouthIndex
  {
    MOUTH_INDEX_NONE,                           ///< 指定無し、再生中のアニメーションに連動

    MOUTH_INDEX_CLOSE,                          ///< 閉じる
    MOUTH_INDEX_HALF_OPEN,                      ///< 半開き
    MOUTH_INDEX_OPEN,                           ///< 開く

    MOUTH_INDEX_UNIQUE0,                        ///< ユニーク0
    MOUTH_INDEX_UNIQUE1,                        ///< ユニーク1
    MOUTH_INDEX_UNIQUE2,                        ///< ユニーク2
    MOUTH_INDEX_UNIQUE3,                        ///< ユニーク3
    MOUTH_INDEX_UNIQUE4,                        ///< ユニーク4
    MOUTH_INDEX_UNIQUE5,                        ///< ユニーク5
    MOUTH_INDEX_UNIQUE6,                        ///< ユニーク6

    // 喜怒哀楽があるモデルではユニーク0～3が相当する
    MOUTH_INDEX_DELIGHT = MOUTH_INDEX_UNIQUE0,    ///< 喜
    MOUTH_INDEX_ANGER = MOUTH_INDEX_UNIQUE1,      ///< 怒
    MOUTH_INDEX_SORROW = MOUTH_INDEX_UNIQUE2,     ///< 哀
    MOUTH_INDEX_PLEASURE = MOUTH_INDEX_UNIQUE3    ///< 楽
  };

  /**
   * @brief 口のアニメーションスロット番号
   */
  static const s32 MOUTH_ANIMATION_SLOT = 10;

  /**
   * @brief 口のアニメーションフレーム間隔
   */
  static const s32 MOUTH_ANIMATION_FRAME_INTERVAL = 10;

public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  CharaModel(); 
  /**
   * @brief デストラクタ
   */
  virtual ~CharaModel();

  //---------------------------------------------------------------------------------
  // ■ 目パチ

  /**
   * @brief 目パチモーションリソースノードを設定
   *
   * @param pEyeMotionResourceNode 目パチモーションリソースノード
   */
  void SetEyeMotionResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pEyeMotionResourceNode);

  /**
   * @brief 目のインデックス設定
   *
   * @param eyeIndex 目インデックス
   */
  void SetEyeIndex(EyeIndex eyeIndex);

  /**
   * @brief 目のインデックス取得
   *
   * @return 目インデックスを返す
   */
  EyeIndex GetEyeIndex() const;

  /**
   * @brief 自動目パチモード設定
   *
   * @param autoBlinkMode 自動目パチモード
   */
  void SetAutoBlinkMode(AutoBlinkMode autoBlinkMode);

  /**
   * @brief 自動目パチモード取得
   *
   * @return 自動目パチモードを返す
   */
  AutoBlinkMode GetAutoBlinkMode() const;

  /**
   * @brief 目の更新
   */
  void UpdateEye();

  /**
   * @brief 目のアニメーションメモリ確保
   */
  void AllocateEye();

  //---------------------------------------------------------------------------------
  // ■ 口パク

  /**
   * @brief 口パクモーションリソースノードを設定
   *
   * @param pMouthMotionResourceNode 口パクモーションリソースノード
   */
  void SetMouthMotionResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pMouthMotionResourceNode);

  /**
   * @brief 口のインデックス設定
   *
   * @param mouthIndex 口インデックス
   */
  void SetMouthIndex(MouthIndex mouthIndex);

  /**
   * @brief 口のインデックス取得
   *
   * @return 口インデックスを返す
   */
  MouthIndex GetMouthIndex() const;

  /**
   * @brief 口の更新
   */
  void UpdateMouth();

  /**
   * @brief 口のアニメーションメモリ確保
   */
  void AllocateMouth();

  //---------------------------------------------------------------------------------
  // ■ 首制御

  /**
   * @brief 首制御更新： ※シーンのトラバース後に呼んでください
   */
  void UpdateNeck();

  /**
   * @brief 首の角度設定
   * 
   * @param angleXRad X方向の角度（ラジアン） +方向で左を向く -方向で右を向く
   * @param angleYRad Y方向の角度（ラジアン） +方向で上を向く -方向で下を向く
   * @param frame 何フレームかけて回転させるか
   */
  void SetNeckAngle(f32 angleXRad, f32 angleYRad, u32 frame);

  /**
   * @brief 首の角度リセット
   * 
   * @param frame 何フレームかけて回転させるか
   */
  void ResetNeckAngle(u32 frame);

  /**
   * @brief 首制御終了確認
   * 
   * @return true:終了 false:更新中
   */
  bool IsFinishNeck() const;

  /**
   * @brief 首制御で変更が発生しているか？
   * @note  現在0度でも、指定フレームに達していない場合はtrueを返す
   * 
   * @return true:発生している
   */
  bool IsChangedNeck() const;

  //---------------------------------------------------------------------------------
  // ■ キャラ制御用の乱数

  /**
   * @brief 目パチ用乱数取得関数のセットアップ
   * @param 乱数取得関数のポインタ
   * @detail nijiではSystem::GflUse::GetPublicRandのポインタを渡す
   */
  void SetupRandomFuc( u32 (*pGetRandomFunc)(u32) );

  /**
   * @brief  乱数取得関数が有効かどうか
   * @return trueなら利用可、falseなら利用不可
   */
  b32 isSetupRandomFunc(void){ return GetPublicRand!=NULL ? true:false; }

  /**
   * @brief  乱数取得
   * @param  乱数の最大値
   * @return 乱数
   */
  u32 GetRandomValue(u32 maxValue){ return GetPublicRand(maxValue); }

private:  // システム用

  /**
   * @brief 作成時に呼ばれるコールバック
   */
  virtual void OnCreate();

  /**
   * @brief CharaModelのフィールド初期化
   */
  void InitializeField_CharaModel();

  f32 GetNeckCurrentAngleXRad() const;
  f32 GetNeckCurrentAngleYRad() const;

  void UpdateAutoBlink();
  void UpdateAutoBlinkField();
  void UpdateAutoBlinkBattle();

  // 着せ替えは顔のパーツにセットする必要がある
  virtual void ChangeAnimationByResourceNode_Face(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, s32 slot);
  virtual void SetAnimationStepFrame_Face(s32 stepFrame, s32 slot);
  virtual void SetAnimationFrame_Face(s32 frame, s32 slot);

  // 首更新後処理
  virtual void OnUpdateNeck();

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEyeMotionResourceNode;  ///< 目パチモーションリソースノード
  EyeIndex m_EyeIndex;                                                                  ///< 目のインデックス
  EyeIndex m_EyeIndexOld;                                                               ///< 前回の目のインデックス
  AutoBlinkMode m_AutoBlinkMode;                                                        ///< 自動目パチモード
  u8 m_AutoBlinkSequence;                                                               ///< 自動目パチ用シーケンス番号
  u8 m_AutoBlinkCounter;                                                                ///< 自動目パチ用カウンタ
  u8 m_AutoBlinkProhibitedFrame;                                                        ///< 目パチ禁止フレーム
  b32 m_isBlinkImmediaterly;                                                            ///< 目パチをした後にすぐにもう一度目パチを行うかどうか

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMouthMotionResourceNode;///< 口パクモーションリソースノード
  MouthIndex m_MouthIndex;                                                              ///< 口のインデックス
  MouthIndex m_MouthIndexOld;                                                           ///< 前回の口のインデックス

  const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode::InterestNode* m_pNeckInterest; ///< 首制御：モーションコンフィグ
  s32 m_NeckJointIndex;                                                                 ///< 首制御：Neckジョイントのインデックス
  f32 m_NeckBeginAngleXRad;                                                             ///< 首制御：開始時の角度 X方向
  f32 m_NeckBeginAngleYRad;                                                             ///< 首制御：開始時の角度 Y方向
  f32 m_NeckTargetAngleXRad;                                                            ///< 首制御：目標角度 X方向
  f32 m_NeckTargetAngleYRad;                                                            ///< 首制御：目標角度 Y方向
  u32 m_NeckTargetFrame;                                                                ///< 首制御：目標フレーム
  u32 m_NeckCurrentFrame;                                                               ///< 首制御：現在のフレーム

  u32 (*GetPublicRand) (u32);                                                           ///< 乱数取得関数の関数ポインタ
};

}}

#endif
