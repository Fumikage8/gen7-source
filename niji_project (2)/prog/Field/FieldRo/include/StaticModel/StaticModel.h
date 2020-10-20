//===================================================================
/**
 * @file    StaticModel.h
 * @brief   配置モデルクラス
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

#if !defined( STATIC_MODEL_H_INCLUDED )
#define STATIC_MODEL_H_INCLUDED
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Math.h>

#include "Field/FieldRo/source/StaticModel/StaticModelPackAccessor.h"
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldStatic/include/Field3DObjectNode.h"

#include <util/include/gfl2_List.h> // 遅延解放リクエストのバッファに使用


// 前方参照
namespace Field {

  class FieldRootNode;

  namespace Terrain {
    class TerrainManager;
  }

  namespace StaticModel {
    class StaticModelManager;
    class StaticModelSEPlayer;
    class LodChangeStaticListenerStaticModel;
    class StaticModelResourceNodeManager;
  }
}

namespace System {
  class nijiAllocator;
}

class StaticActor;
class CollisionSceneDefault;

namespace poke_3d { namespace model {
  class BaseModel;
}}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 配置モデルクラス
 */
class StaticModel : public gfl2::renderingengine::scenegraph::NodeTrait<StaticModel,IField3DObjectNode>
{
  GFL_FORBID_COPY_AND_ASSIGN(StaticModel); //コピーコンストラクタ＋代入禁止

  friend class StaticModelManager;

public:

  ///< 初期化情報
  struct InitializeData
  {
    gfl2::heap::HeapBase*             pHeap;              ///< このモデル専用のヒープ
    u32                               modelID;            ///< 配置モデルID
    u32                               eventID;            ///< イベントID(niji全体でユニークなID)
    void*                             pStaticModelPack;   ///< 配置モデル1つ分のリソースのパック
    gfl2::math::SRT                   srt;                ///< SRT
    FieldRootNode*                    pFieldRootNode;     ///< フィールドルートノード
    System::nijiAllocator*            pAllocator;         ///< この配置モデル専用のアロケータ
    StaticModelSEPlayer*              pSEPlayer;          ///< SE再生インスタンス
    s32                               modelIndex;         ///< モデル管理番号
    StaticModelManager*               pManager;           ///< 管理クラス
    StaticModelResourceNodeManager*   pResourceNodeManager; ///< リソースノード管理クラス
    bool                              isDynamic;            ///< 動的生成か
  };

public:

  /**
   * @brief   コンストラクタ
   */
  StaticModel();

  /**
   * @brief   デストラクタ
   */
  virtual ~StaticModel();

  /**
   * @brief 現在のLODレベルのモデルの表示ON・OFF
   */
  virtual void SetVisible( bool isVisible );
  virtual bool GetVisible( void ) const;

  /**
   * @brief 全モデル表示ON・OFF
   */
  void SetVisibleAll( bool isVisible);

  /**
   * @brief SRTコントロール
   */
  virtual void SetLocalSRT( const gfl2::math::SRT & srt );
  virtual const gfl2::math::SRT & GetLocalSRT() const;

  /**
   * @brief モデルインスタンスを取得
   */
  virtual u32 GetModelInstanceNum() const;
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetModelInstance( int index ) const;

  /**
   *  @brief  ベースモデルの取得
   *  @note 取得したBaseModelのポインタは必ずNULLチェックをしてください
   *  @note デバッグ期間にGetBaseModelを使用している箇所でNULLチェックが行われているかを確認する
   */
  virtual u32 GetBaseModelNum() const ;
  virtual poke_3d::model::BaseModel *GetBaseModel( u32 idx ) const ;

  //! モデルインスタンスの遅延解放は自由に実装
  /**! 削除リクエストを受けたモデルインスタンスを解放
   * -実行タイミング　フィールドプロセスの先頭で実行します。
   * -制限　　　　　　PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
   */
  virtual void ExecuteTerminateRequest(void);
  //! 安全に破棄できる常態かチェックする。
  virtual bool IsCanTerminate(void);

  static gfl2::util::FnvHash GetTypeHash()
  {
    return gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::FieldNode::Field3DObjectNode::StaticModel") ) );
  }

  /**
   * @brief 遅延解放リクエスト
   * @param modelType どのモデルか。ハイ、ミドル、ロー、全てから選択
   */
  void RequestDelayTerminate( ModelType modelType);

  /**
   * @brief 初期化
   * @param initData 初期化情報
   */
  void Initialize( InitializeData& initData);

  /**
  * @brief Readyで1度だけ呼び出す
  */
  void Ready();

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief 配置モデルIDの取得
   * @return 配置モデルID
   */
  u32 GetStaticModelID() const ;

  /**
   * @brief イベントIDの取得
   * @return イベントID
   */
  u32 GetEventID() const;

  /**
   * @brief モデル管理番号の取得
   * @return モデル管理番号
   */
  s32 GetModelIndex() const;

  /**
   * @brief 未初期化か
   * @retval true 未初期化
   * @retval false 初期化済み
   */
  bool IsEmpty() const ;

  /**
   * @brief 現在のモデルタイプを取得 (LOD段階が取れる)
   * @return 現在のモデルタイプ
   */
  ModelType GetNowModelType() const ;

  /**
   * @brief 更新フラグのセット
   * @param isEnable trueで更新する
   */
  void SetUpdateEnable( bool isEnable);

  /**
   * @brief LOD変更
   * @param changeType 変更したいLODレベル
   * @note 変更可能な場合、使用BaseModelの変更(表示ON/OFF)、アニメーションの同期が行われる
   */
  bool ChangeLOD( ModelType changeType);

  /**
   * @brief LOD変更 Highより下は表示OFF挙動用
   * @param changeType 変更したいLODレベル
   */
  bool ChangeLOD_VisibleOff( ModelType changeType);

  /**
  * @brief バウンディングボックスアニメーションStaticActor作成
  * @param pCollisionScene コリジョンシーン
  * @return 生成したStaticActor
  */
  void CreateBBAnimationStaticActor( CollisionSceneDefault* pCollisionScene);

  /**
  * @brief バウンディングボックスアニメーションが存在するか
  * @retval true 存在する
  * @retval false 存在しない
  */
  bool IsBBAnimationExist() const ;

  /**
   * @brief StaticActorの設定
   */
  void SetStaticActor( StaticActor* pStaticActor ){ m_pStaticActor = pStaticActor; }

  /**
   * @brief StaticActorの取得
   */
  StaticActor* GetStaticActor(void) const { return m_pStaticActor; }

  /**
  *  @brief  Updateを止めるフレーム数を設定
  *  @param  frame  フレーム数
  */
  void SetStopUpdateFrame(s32 frame){ m_stopUpdateFrame = frame; }

  /**
  *  @brief  Updateを止めるフレーム数を取得
  *  @return フレーム数
  */
  s32 GetStopUpdateFrame(void) const { return m_stopUpdateFrame; }

  /**
  *  @brief  カリング種類を取得
  *  @return カリング種類
  */
  u8 GetCullingType() const { return m_pStaticModelInfo->cullingType; }

  /**
  *  @brief  カリング更新が有効か
  *  @retval true 有効
  *  @retval false 無効
  */
  bool IsUpdateCullingEnable() const { return m_isUpdateCullingEnable; }

  /**
  *  @brief  カリング更新フラグの設定
  */
  void SetUpdateCullingEnable( bool isEnable) { m_isUpdateCullingEnable = isEnable; }

  /**
   *  @brief  LOD挙動の種類を取得
   *  @return LOD挙動の種類
   */
  u8 GetLODType() const { return m_pStaticModelInfo->lodType; }

  /**
   * @brief  スロット間補間アニメチェンジが再生可能なモデルか？
   * @return 配置モデルID
   */
  bool CanUseChangeAnimeSmooth() const ;

private:

  ///< モデル生成に必要な情報
  struct ModelResourceData
  {
    void*   pModelBinary;   ///< モデルリソース
  };

  ///< アニメーション生成に必要な情報
  struct MotionResourceData
  {
    void*   pMotionBinary;  ///< モーションリソース
  };

  static const u8 TIME_ZONE_NONE = 0xFF;  ///< タイムゾーン指定がされていないと扱う値。この場合は時間アニメなしor時間分指定

  // 1ファイルで複数アニメーションをフレームで制御するアニメーション用の更新関数定義
  typedef bool (StaticModel::*FrameControlAniamtionUpdateFunc)();

  ///< 時間アニメーション更新シーケンス
  enum TimeAnimationUpdateSeq
  {
    TIME_ANIMATION_UPDATE_SEQ_NONE,         ///< アニメーション時間範囲外
    TIME_ANIMATION_UPDATE_SEQ_WAIT_START,   ///< 開始アニメーション終了待ち
    TIME_ANIMATION_UPDATE_SEQ_WAIT_LOOP,    ///< ループアニメーション終了待ち
    TIME_ANIMATION_UPDATE_SEQ_WAIT_END,     ///< 終了アニメーション終了待ち

    TIME_ANIMATION_UPDATE_SEQ_MAX,
  };
  // 初期化時には再計算も必要

  ///< 時間アニメーション更新関数テーブル
  static const FrameControlAniamtionUpdateFunc m_TimeAnimationUpdateFunc[ TIME_ANIMATION_UPDATE_SEQ_MAX ];

  ///< バウンディングボックスアニメーション更新シーケンス
  enum BBAnimationUpdateSeq
  {
    BB_ANIMATION_UPDATE_SEQ_NONE,         ///< アニメーションなし
    BB_ANIMATION_UPDATE_SEQ_WAIT_START,   ///< 開始アニメーション終了待ち
    BB_ANIMATION_UPDATE_SEQ_WAIT_LOOP,    ///< ループアニメーション終了待ち
    BB_ANIMATION_UPDATE_SEQ_WAIT_END,     ///< 終了アニメーション終了待ち

    BB_ANIMATION_UPDATE_SEQ_MAX,
  };

  ///< バウンディングボックスアニメーション更新関数テーブル
  static const FrameControlAniamtionUpdateFunc m_BBAnimationUpdateFunc[ BB_ANIMATION_UPDATE_SEQ_MAX ];

  ///< 状態
  enum State
  {
    STATE_NONE,                 ///< 未初期化
    STATE_INITIALIZE_FINISHED,  ///< 初期化完了
    STATE_UPDATE,               ///< 更新処理有効
  };

  ///< バウンディングボックスアニメーションコリジョン判定の状態
  struct BBAnimationCollisionState
  {
    BBAnimationCollisionState()
      : isHit( false)
    {}

    bool isHit;   ///< 当たっているか
  };

private:

  // モデルモーションリソース関連
  void CreateModelWork();
  bool CreateModelAndMotions( ModelType modelType);
  void CreateModel( ModelType modelType);

  void CreateModelWorkDynamic();
  bool CreateModelAndMotionsDynamic( ModelType modelType);
  void CreateModelResourceNode( ModelType modelType, const ModelResourceData& modelResourceData);
  void CreateMotionResourceNode( ModelType modelType, u32 index, const MotionResourceData& motionResourceData);

  // アニメーション関連
  bool IsAnimationExist( AnimationIndex index) const;
  bool IsAnimationExist( ModelType type, AnimationIndex index) const;
  bool IsAnimationPlaying( ModelType type, AnimationIndex index) const ;
  bool IsAnimationPlayingEx( ModelType type, AnimationIndex index, s32 slotIndex) const ;
  bool ChangeAnimation( AnimationIndex index, bool isLoop = true, f32 stepFrame = 1.0f);
  void SyncAnimation( ModelType changeType, ModelType oldType);
  void SetAnimationFrame( AnimationIndex index, f32 frame);
  void SetAnimationStepFrame( AnimationIndex index, f32 stepFrame);

  // ハイモデル専用のアニメーション関連
  // ※ LOD_TYPE_VISIBLE_OFFのときにのみ使用する
  bool ChangeAnimation_High( AnimationIndex index, bool isLoop = true, f32 stepFrame = 1.0f);
  bool ChangeAnimationEx_High( s32 slotIndex, AnimationIndex index, bool isLoop = true, f32 stepFrame = 1.0f, f32 startFrame = 1.0f, f32 lerpTime = -1.0f );
  void StopAnimation_High( AnimationIndex index);
  void StopAnimationEx_High( s32 slotIndex, AnimationIndex index);
  void SetAnimationFrame_High( AnimationIndex index, f32 frame);
  void SetAnimationLastFrame_High( AnimationIndex index);
  void SetAnimationLastFrameEx_High( s32 slotIndex, AnimationIndex index);
  void SetAnimationStepFrame_High( AnimationIndex index, f32 stepFrame);

  // SE関連
  void PlayEvnetAnimationSE( u32 index, bool isInverse);

  // 時間関連
  void GetNowHourAndMinute( s32* pOutHour, s32* pOutMinute) const ;

  // ループアニメーション関連
  void UpdateLoopAnimation();

  // 1日アニメーション関連
  void UpdateAllTimeAnimation();

  // イベントアニメーション関連
  void UpdateEventAnimation();

  // 時間アニメーション関連
  void UpdateTimeAnimation();
  bool TimeAnimationUpdateFunc_None();
  bool TimeAnimationUpdateFunc_WaitStart();
  bool TimeAnimationUpdateFunc_WaitLoop();
  bool TimeAnimationUpdateFunc_WaitEnd();

  // バウンディングボックスアニメーション関連
  void UpdateBBAnimation();
  bool BBAnimationUpdateFunc_None();
  bool BBAnimationUpdateFunc_WaitStart();
  bool BBAnimationUpdateFunc_WaitLoop();
  bool BBAnimationUpdateFunc_WaitEnd();
  void NotificationBBAnimationCollisionState( const BBAnimationCollisionState& state);
  void GetBBAnimationCollisionModelTransformMatrix( gfl2::math::Matrix44* pOut);

private:

  /**
   * @class 1日アニメーションワーク
   */
  class AllTimeAnimationWork
  {
  public:

    /**
     * @brief コンストラクタ
     */
    AllTimeAnimationWork();

    /**
     * @brief デストラクタ
     */
    ~AllTimeAnimationWork();

    /**
     * @brief 初期化
     */
    void Initialize();

    /**
     * @brief クリア
     */
    void Clear();

    /**
     * @brief アニメーション更新リクエストがあるか
     * @retval true 更新
     * @retval false 必要なし
     */
    bool IsUpdateAnimationRequest() const ;

    /**
     * @brief 更新タイミングか
     * @retval true 更新する
     * @retval false 更新しない
     */
    bool IsUpdate( s32 hour, s32 minute);

    /**
     * @brief アニメーションフレームを計算
     * @param maxFrame 最大フレーム数
     * @param hour 現在の時間
     * @param minute 現在の分
     * @return 現在の時間のアニメーションのフレーム数
     */
    f32 CalculateAnimationFrame( f32 maxFrame, s32 hour, s32 minute) const ;

  private:

    bool  m_isUpdateRequest;    ///< そのフレームのアニメーション更新リクエスト
    s32   m_beforeHour;         ///< 前回の時間
    s32   m_beforeMinute;       ///< 前回の分
  };

  /**
   * @class 時間アニメーションワーク
   */
  class TimeAnimationWork
  {
  public:

    /**
     * @brief コンストラクタ
     */
    TimeAnimationWork();

    /**
     * @brief デストラクタ
     */
    ~TimeAnimationWork();

    /**
     * @brief 初期化
     * @param startHour 開始時間
     * @param startMinute 開始分
     * @param endHour 終了時間
     * @param endMinute 終了分
     */
    void Initialize( s32 startHour, s32 startMinute, s32 endHour, s32 endMinute);

    /**
     * @brief クリア
     */
    void Clear();

    /**
     * @brief アニメーション更新リクエストがあるか
     * @retval true 更新
     * @retval false 必要なし
     */
    bool IsUpdateAnimationRequest() const ;

    /**
    * @brief 時間アニメ開始時間と終了時間の範囲内か
    * @param hour 比較したい時間
    * @param minute 比較したい分
    * @retval true 範囲内
    * @retval false 範囲外
    */
    bool IsTimeInRange( s32 hour, s32 minute) const ;

    /**
     * @brief 時間アニメ開始時間と終了時間の範囲外か
     * @param hour 比較したい時間
     * @param minute 比較したい分
     * @retval true 範囲内
     * @retval false 範囲外
     */
    bool IsTimeOverRange( s32 hour, s32 minute) const ;

    /**
     * @brief 更新シーケンスを進める
     * @retval true シーケンスをリセットした
     * @retval true シーケンスを進めた
     */
    bool AddUpdateSeq();

    /**
     * @brief 更新シーケンス設定
     */
    void SetUpdateSeq( u8 seq);

    /**
     * @brief 更新シーケンスを取得
     * @return シーケンス
     */
    u8 GetUpdateSeq() const ;

  private:

    void GetCombineTime( s32* pOutStart, s32* pOutEnd, s32* pOutNow, s32 nowHour, s32 nowMinute) const ;
    void ConvertTimeToCombineTime( s32* pOut, s32 hour, s32 minute, bool isIncrementDigit = false) const ;

  private:

    bool  m_isUpdateRequest;      ///< そのフレームのアニメーション更新リクエスト
    u8    m_updateSeq;            ///< 時間アニメーション更新シーケンス
    s32   m_startHour;            ///< 時間アニメ開始時間
    s32   m_startMinute;          ///< 時間アニメ開始分
    s32   m_endHour;              ///< 時間アニメ終了時間
    s32   m_endMinute;            ///< 時間アニメ開始分
  };

  /**
   * @class バウンディングボックスアニメーションワーク
   */
  class BBAnimationWork
  {
  public:

    /**
     * @brief コンストラクタ
     */
    BBAnimationWork();

    /**
     * @brief デストラクタ
     */
    ~BBAnimationWork();

    /**
     * @brief 初期化
     */
    void Initialize();

    /**
     * @brief クリア
     */
    void Clear();

    /**
     * @brief アニメーション更新リクエストがあるか
     * @retval true 更新
     * @retval false 必要なし
     */
    bool IsUpdateAnimationRequest() const ;

    /**
     * @brief 更新シーケンスを進める
     * @retval true シーケンスをリセットした
     * @retval true シーケンスを進めた
     */
    bool AddUpdateSeq();

    /**
     * @brief 更新シーケンス設定
     */
    void SetUpdateSeq( u8 seq);

    /**
     * @brief 更新シーケンスを取得
     * @return シーケンス
     */
    u8 GetUpdateSeq() const ;

    /**
    * @brief バウンディングボックスアニメーションコリジョン判定の状態を通知してもらう
    * @param state バウンディングボックスアニメーションのコリジョン判定の状態
    */
    void NotificationBBAnimationCollisionState( const BBAnimationCollisionState& state);

    /**
    * @brief コリジョンと当たっているか
    * @retval true 当たっている
    * @retval false 当たっていない
    */
    bool IsHitCollision() const ;

    /**
    * @brief アニメーションを開始するか
    * @retval true 開始する
    * @retval false 開始しない
    */
    bool IsStartAnimation() const ;

  private:

    bool                      m_isUpdateRequest;  ///< そのフレームのアニメーション更新リクエスト
    u8                        m_updateSeq;        ///< 時間アニメーション更新シーケンス
    BBAnimationCollisionState m_state;            ///< 現在のフレームのコリジョン判定状態
  };

  /**
   * @class アニメーションスロット制御用ワーク
   */
  class AnimationSlotControlWork
  {
  public:

    static const u32 INVALID_SLOT_INDEX = 0xFFFFFFFF; ///< 無効値

    /**
     * @brief コンストラクタ
     */
    AnimationSlotControlWork();

    /**
     * @brief デストラクタ
     */
    ~AnimationSlotControlWork();

    /**
     * @brief 初期化
     */
    void Initialize();

    /**
     * @brief クリア
     */
    void Clear();

    /**
     * @brief スロット番号取得
     * @param animationndex アニメーション番号
     * @return スロット番号
     */
    u32 GetSlotIndex( u32 modelType, u32 animationIndex) const ;

    /**
     * @brief スロット番号設定
     * @param animationndex アニメーション番号
     */
    void SetSlotIndex( u32 modelType, u32 animationIndex);

    /**
     * @brief スロット番号クリア
     * @param animationndex アニメーション番号
     */
    void ClearSlotIndex( u32 modelType, u32 animationIndex);

    /**
     * @brief 有効なスロット番号か
     * @param animationndex アニメーション番号
     * @retval true 有効
     * @retval true 無効
     */
    bool IsEnableSlotIndex( u32 modelType, u32 animationIndex) const ;

  private:

    bool m_isUseSlotIndex[ MODEL_TYPE_MAX ][ ANIMATION_INDEX_MAX ];   ///< スロット番号
  };

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelResourceRootNode[ MODEL_TYPE_MAX ];                         ///< モデルリソースノード配列
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pMotionResourceRootNode[ MODEL_TYPE_MAX ][ ANIMATION_INDEX_MAX ]; ///< アニメーションリソースノード配列。アニメーションの制御に使用
  poke_3d::model::BaseModel*                                  m_pModel[ MODEL_TYPE_MAX ];                                         ///< モデルインスタンスノード。LOD分持つ。
  bool                                                        m_isModelVisible;

  gfl2::heap::HeapBase*                 m_pHeap;                          ///< この配置モデル専用のヒープ。マネージャーでローカルヒープを作成して受け取る
  System::nijiAllocator*                m_pAllocator;                     ///< アロケーター
  void*                                 m_pStaticModelPack;               ///< 配置モデルリソースパックバイナリデータ(LOD、アニメ込み)
  StaticModelPackAccessor::StaticModelInfo* m_pStaticModelInfo;               ///< 配置モデル情報

  u16                                   m_modelID;                        ///< 配置モデルID
  u32                                   m_modelNum;                       ///< モデル数
  u32                                   m_eventID;                        ///< イベントID
  ModelType                             m_nowModelType;                   ///< 現在のモデルの種類
  gfl2::math::SRT                       m_srt;                            ///< SRT
  s32                                   m_modelIndex;                     ///< モデル管理番号

  bool                                  m_isExecuteTerminate;             ///< 破棄しても良いか
  gfl2::util::List<ModelType>           m_dirayTerminateList;             ///< 遅延解放リクエストを受けたモデルリスト

  StaticActor*                          m_pStaticActor;                   ///< 衝突アクター
  StaticActor*                          m_pStaticActorBBAnimation;        ///< 衝突アクター。バウンディングボックスアニメーション用
  CollisionSceneDefault*                m_pCollisionSceneForBBAnimation;  ///< バウンディングボックスアニメーション用コリジョンシーン

  FieldRootNode*                        m_pFieldRootNode;                 ///< フィールドルートノード。ノードへの登録削除に使用

  u32                                   m_state;                          ///< 状態
  s32                                   m_stopUpdateFrame;                ///< 更新を止めるフレーム数

  StaticModelSEPlayer*                  m_pSEPlayer;                      ///< SE再生

  AllTimeAnimationWork                  m_allTimeAnimationWork;           ///< 1日アニメーションワーク
  TimeAnimationWork                     m_timeAnimationWork;              ///< 時間アニメーションワーク
  BBAnimationWork                       m_BBAnimationWork;                ///< バウンディングボックスアニメーションワーク
  AnimationSlotControlWork              m_animationSlotControlWork;       ///< アニメーションスロット制御用ワーク

  StaticModelManager*                   m_pManager;                       ///< 管理クラス
  StaticModelResourceNodeManager*       m_pResourceNodeManager;           ///< リソースノード管理クラス
  s32                                   m_resourceWorkIndex;              ///< リソースノードワーク番号
  bool                                  m_isDynamic;                      ///< 動的生成配置モデルか

  bool                                  m_isUpdateCullingEnable;          ///< カリング更新が有効か

#if PM_DEBUG
  u32     m_instanceAllocSize;
  u32     m_animationAllocSize;

public:

  u32 GetInstanceAllocSize() const { return m_instanceAllocSize; }
  u32 GetAnimationAllocSize() const { return m_animationAllocSize; }

#endif

public:
  //*******************************************************************
  //*******************************************************************
  // ここから下はハンドルクラス
  //*******************************************************************
  //*******************************************************************

  //===================================================================
  /**
   * @class ループアニメーション開始
   */
  //===================================================================
  class Handle_StartLoopAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StartLoopAnimation( f32 startFrame = 0.0f);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    f32  startFrame;  // 開始フレーム
  };

  //===================================================================
  /**
   * @class ループアニメーション停止
   */
  //===================================================================
  class Handle_StopLoopAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StopLoopAnimation();
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );
  };

  //===================================================================
  /**
   * @class イベントアニメーション開始
   * @note stepFrameをマイナスにすると逆再生
   */
  //===================================================================
  class Handle_StartEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StartEventAnimation( u32 index, f32 stepFrame = 1.0f, bool isLoop = false);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  index;     // イベントアニメーション番号。0〜3の範囲
    f32  stepFrame; // ステップフレーム
    bool isLoop;    // trueでループ
  };

  //===================================================================
  /**
   * @class イベントアニメーション開始 (スロット指定版)
   * @note stepFrameをマイナスにすると逆再生
   * @caution ※ 非推奨です！勝手に使用しないでください。ポケセンのカウンターなどの例外のために用意したものです。
   */
  //===================================================================
  class Handle_StartEventAnimationEx : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StartEventAnimationEx( u32 slotIndex, u32 index, f32 stepFrame = 1.0f, bool isLoop = false, u32 lerpTime = 0, s32 startFrame = -1 );
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
    f32  m_stepFrame; // ステップフレーム
    bool m_isLoop;    // trueでループ
    u32  m_slotIndex; // スロット番号
    u32  m_lerpTime;  // Smoothの時間（指定しないなら０）
    s32  m_startFrame;  //アニメーションのスタートフレーム(マイナス値を指定すると、フレームセットしない)
  };

  //===================================================================
  /**
   * @class イベントアニメーション停止
   * @note ChangeAnimationにNULLをいれる
   */
  //===================================================================
  class Handle_StopEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StopEventAnimation( u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  index;     // イベントアニメーション番号。0〜3の範囲
  };

  //===================================================================
  /**
   * @class イベントアニメーション停止 (スロット指定版)
   * @note ChangeAnimationにNULLをいれる
   * @caution ※ 非推奨です！勝手に使用しないでください。ポケセンのカウンターなどの例外のために用意したものです。
   */
  //===================================================================
  class Handle_StopEventAnimationEx : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StopEventAnimationEx( u32 slotIndex, u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
    u32  m_slotIndex; // スロット番号
  };

  //===================================================================
  /**
   * @class イベントアニメーションの終了待ち
   */
  //===================================================================
  class Handle_IsEndEventAnimation : public IField3DObjectNode::IHandle
  {
  public:
    Handle_IsEndEventAnimation( u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;

    u32  index;     // イベントアニメーション番号。0〜3の範囲
  };

  //===================================================================
  /**
   * @class イベントアニメーションの終了待ち (スロット指定版)
   * @caution ※ 非推奨です！勝手に使用しないでください。ポケセンのカウンターなどの例外のために用意したものです。
   */
  //===================================================================
  class Handle_IsEndEventAnimationEx : public IField3DObjectNode::IHandle
  {
  public:
    Handle_IsEndEventAnimationEx( u32 slotIndex, u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
    u32  m_slotIndex; // スロット番号
  };

  //===================================================================
  /**
   * @class イベントアニメーションフレームセット
   */
  //===================================================================
  class Handle_SetFrameEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_SetFrameEventAnimation( u32 index, f32 frame);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  index;     // イベントアニメーション番号。0〜3の範囲
    u32  frame; // フレーム
  };

  //===================================================================
  /**
   * @class イベントアニメーションに終了フレームをセット
   * @note ChangeAnimationにNULLはいれず、SetStepFrameを0にする
   */
  //===================================================================
  class Handle_SetLastFrameEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_SetLastFrameEventAnimation( u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
  };

  //===================================================================
  /**
   * @class イベントアニメーション一時停止
   * @note ChangeAnimationにNULLはいれず、StepFrameを0にする
   */
  //===================================================================
  class Handle_PauseEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_PauseEventAnimation( u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
  };

  //===================================================================
  /**
   * @class イベントアニメーション再開
   * @note ChangeAnimationせずに指定されたStepFrameを設定する
   */
  //===================================================================
  class Handle_RestartEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_RestartEventAnimation( u32 index, f32 stepFrame = 1.0f);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
    f32  m_stepFrame; // ステップフレーム
  };

  //===================================================================
  /**
   * @class イベントアニメーションを終了状態にする
   * @note ChangeAnimation、SetAnimationFrame、SetAnimationStepFrameをして終了状態にする
   * @note SEは鳴らさない
   */
  //===================================================================
  class Handle_EndEventAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_EndEventAnimation( u32 index);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

    u32  m_index;     // イベントアニメーション番号。0〜3の範囲
  };

  //===================================================================
  /**
   * @class ドアが開くアニメーション開始
   */
  //===================================================================
  class Handle_StartDoorOpenAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );
  };

  //===================================================================
  /**
   * @class ドアが閉まるアニメーション開始
   */
  //===================================================================
  class Handle_StartDoorCloseAnimation : public IField3DObjectNode::ISetHandle
  {
  public:
    Handle_StartDoorCloseAnimation(f32 startFrame = -1.0f);
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
    virtual void Handle( IField3DObjectNode * node );

  private:
    f32 m_StartFrame;
  };

  //===================================================================
  /**
   * @class ドアアニメーションの終了待ち
   */
  //===================================================================
  class Handle_IsEndDoorAnimation : public IField3DObjectNode::IHandle
  {
  public:
    virtual bool IsSupport( const IField3DObjectNode & node ) const ;
    virtual bool IsEnd( const IField3DObjectNode * node ) const ;
  };
};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_H_INCLUDED
