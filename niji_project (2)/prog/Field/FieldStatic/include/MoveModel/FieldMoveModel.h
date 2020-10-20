#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel.h
*  @brief  フィールド用 動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <model/include/gfl2_CharaModel.h>
#include "System/include/nijiAllocator.h"
#include "Field/FieldStatic/include/Field3DObjectNode.h"
#include "FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"

class BaseCollisionScene;

namespace poke_3d { namespace model {
   class CharaModelFactory;
   class DressUpModelResourceManager;
}}

namespace Field {
  class MotionSignal;
  class MyRenderingPipeLine;
  namespace Effect
  {
    class IEffectBase;
  }
}

//=================================================================================================
//
// ※注意※  このクラスでは座標/姿勢/拡縮を保持してはいけません。
//           座標/姿勢/拡縮を保持して良いのは描画インスタンス(poke_3d::model::CharaModel)のみです。
//           ただ諸々の処理を簡単/高速に行うために、描画インスタンスと同内容のツリーを形成する必要があります
//
//=================================================================================================
namespace Field{ namespace MoveModel {

class FieldMoveCodeBase;
class FieldSubobject;
class FieldFacialAnimeEye;
class FieldFacialAnimeMouth;
class IFieldMoveModelEffectCreaterBase;

class FieldMoveModel : public gfl2::renderingengine::scenegraph::NodeTrait<FieldMoveModel,Field::IField3DObjectNode>
{
public:

  static const f32 GROUND_OFFSET;  // 床コリジョンの結果に足しこむ高さのオフセット

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModel();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(ワーク)
  *
  *  @param  pFieldMoveModelHeader  作成リクエスト
  */
  //-----------------------------------------------------------------------------
  virtual void SetupWork(const FieldMoveModelHeaderWork * pFieldMoveModelHeaderWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化(リソース)
  *
  *  @param  pHeap                  ヒープ
  *  @param  pMyRenderingPipeLine   レンダリングパイプライン
  *  @param  pCharaModelFactory     キャラリソースファクトリー
  *  @param  pFieldMoveModelHeader  作成リクエスト
  */
  //-----------------------------------------------------------------------------
  virtual void SetupResource(gfl2::heap::HeapBase*                        pHeap,
                             Field::MyRenderingPipeLine*                  pMyRenderingPipeLine,
                             poke_3d::model::CharaModelFactory*           pCharaModelFactory,
                             poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager,
                             const FieldMoveModelHeaderResource*          pFieldMoveModelHeaderResource);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(ワーク)
  */
  //-----------------------------------------------------------------------------
  virtual void ClearWork(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放(リソース)
  */
  //-----------------------------------------------------------------------------
  virtual void ClearResource(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
  */
  //-----------------------------------------------------------------------------
  virtual void PrevUpdate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  後更新(座標の決定後に必要な処理を行う)
  *
  *  @param  pCharaModelFactory  キャラリソースファクトリー
  */
  //-----------------------------------------------------------------------------
  virtual void PostUpdate(poke_3d::model::CharaModelFactory* pCharaModelFactory);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクト更新
  */
  //-----------------------------------------------------------------------------
  virtual void UpdateSubobject(void);
  virtual void PostUpdateSubobject(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  エフェクト生成
  */
  //-----------------------------------------------------------------------------
  virtual void UpdateEffect(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  地面にキャラを引っ付ける
  */
  //-----------------------------------------------------------------------------
  virtual void AdjustGround(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの座標/姿勢をダイナミックアクターの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyMoveModelToActor(FieldMoveModelCollisionType       colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルの座標/姿勢をスタティックアクターの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyMoveModelToActor(FieldMoveModelStaticCollisionType colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダイナミックアクターの座標/姿勢を動作モデルの座標/姿勢に適用する
  */
  //-----------------------------------------------------------------------------
  virtual void ApplyActorToMoveModel(FieldMoveModelCollisionType colType);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コリジョンアクターが衝突を解決しているかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void CheckCollisionSolved(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  表世界にいるかどうか
  *
  *  @return 表世界にいればtrue、裏世界にいればfalseを返す
  */
  //-----------------------------------------------------------------------------
  virtual b32 CheckValidGround(b32 isDisableAssert=false);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化しているかどうか
  *
  *  @return trueは初期化済み、falseは未初期化
  */
  //-----------------------------------------------------------------------------
  b32 IsSetup         (void){ if( m_isSetupResource && m_isSetupWork ){return true;} return false; }
  b32 IsSetupWork     (void){ return m_isSetupWork; }
  b32 IsSetupResource (void){ return m_isSetupResource; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  エッジIDを設定
  *
  *  @param  id エッジ用ステンシルID
  */
  //-----------------------------------------------------------------------------
  void SetEdgeId(u32 id){ m_edgeId = id; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  PreUpdateを止めるフレーム数を設定
  *
  *  @param  frame  フレーム数
  */
  //-----------------------------------------------------------------------------
  void SetStopPreUpdateFrame(s32 frame){ m_stopPreUpdateFrame = frame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  PreUpdateを止めるフレーム数を取得
  *
  *  @return フレーム数
  */
  //-----------------------------------------------------------------------------
  s32 GetStopPreUpdateFrame(void){ return m_stopPreUpdateFrame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デフォルト待機アニメーションの設定
  *
  *  @param  defaultIdleAnimationId  デフォルト待機アニメーション
  */
  //-----------------------------------------------------------------------------
  void SetDefaultIdleAnimationId(u32 defaultIdleAnimationId){ m_defaultIdleAnimationId = defaultIdleAnimationId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デフォルト待機アニメーションの取得
  *
  *  @return デフォルト待機アニメーション
  */
  //-----------------------------------------------------------------------------
  u32 GetDefaultIdleAnimationId(void){ return m_defaultIdleAnimationId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  前回のアニメーションフレームを取得
  *
  *  @return 前回のアニメーションフレーム
  */
  //-----------------------------------------------------------------------------
  f32 GetPrevAnimationFrame(void){ return m_prevAnimationFrame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  前回のアニメーションステップフレームを取得
  *
  *  @return 前回のアニメーションステップフレーム
  */
  //-----------------------------------------------------------------------------
  f32 GetPrevAnimationStepFrame(void){ return m_prevAnimationStepFrame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  Originジョイントによる移動ベクトルを変換するスケール/クォータニオンを設定(毎フレーム呼ぶ必要があります)
  *          この関数を呼ばなかった場合、スケールには1が使われ、クォータニオンにはCharaModelインスタンスのクォータニオンが使われます
  *
  *  @param  スケール
  *  @param  クォータニオン
  */
  //-----------------------------------------------------------------------------
  void SetCustomValueForOrigin(f32 scale,gfl2::math::Quaternion qua){ m_customScaleForOrigin = scale; m_customQuaternionForOrigin = qua; m_isUseCustomValueForOrigin=true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  当たる側アクターの取得
  *
  *  @param  index    取得したいアクター
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  DynamicActor* GetDynamicActor(u32 index){ return m_pDynamicActor[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  当たられる側アクターの取得
  *
  *  @param  index    取得したいアクター
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  StaticActor*  GetStaticActor(u32 index){ return m_pStaticActor[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コリジョンシーンコンテナの設定
  *
  *  @param  pCollisionSceneContainer  コリジョンシーン
  *  @note   高さ取得用レイチェックで対象とするシーンです
  */
  //-----------------------------------------------------------------------------
  void SetCollisionSceneContainerForGround(BaseCollisionScene* pCollisionSceneContainer){ m_pCollisionSceneContainerForGround = pCollisionSceneContainer; }
  void SetCollisionSceneContainerForWall  (BaseCollisionScene* pCollisionSceneContainer){ m_pCollisionSceneContainerForWall   = pCollisionSceneContainer; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コリジョンシーンコンテナが設定されているかどうか
  *
  *  @return 設定されているかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsSetCollisionSceneContainerForGround(void){ return m_pCollisionSceneContainerForGround ? true:false; }
  b32 IsSetCollisionSceneContainerForWall  (void){ return m_pCollisionSceneContainerForWall   ? true:false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コリジョンシーンコンテナの取得
  *
  *  @retval  pCollisionSceneContainer  コリジョン
  */
  //-----------------------------------------------------------------------------
  BaseCollisionScene* GetCollisionSceneContainerForGround(void){ return m_pCollisionSceneContainerForGround; }
  BaseCollisionScene* GetCollisionSceneContainerForWall  (void){ return m_pCollisionSceneContainerForWall; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画インスタンスの取得
  *
  *  @return 描画インスタンス
  */
  //-----------------------------------------------------------------------------
  const poke_3d::model::CharaModel* GetCharaDrawInstance(void) const { return m_pCharaDrawInstance; }
        poke_3d::model::CharaModel* GetCharaDrawInstance(void)       { return m_pCharaDrawInstance; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント状態にするリクエストを発行
  *
  *  @note   リクエストが0から1になるとムーブコードを停止する
  */
  //-----------------------------------------------------------------------------
  void AddEventRequest(void)
  {
    m_eventRequestCnt++;

    // @fix GFNMCat[887] 1フレーム前の値が'1'で今回の値も'0'から'1'になったということは、1フレーム内でV字回復をしているので強制的にResume/Suspendを通す
    if( m_eventRequestCnt == 1 && m_eventRequestCntPrev == 1 )
    {
      m_isNeedForceResume  = true;
      m_isNeedForceSuspend = true;
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント状態を解除するリクエストを発行
  *
  *  @note   リクエストが1から0なるとムーブコードを再開する
  */
  //-----------------------------------------------------------------------------
  void PullEventRequest(void){ if( m_eventRequestCnt > 0 ){ m_eventRequestCnt--; } }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント状態がV字回復しResumeが必要になったフレームかどうか
  *
  *  @note   Resumeが必要
  */
  //-----------------------------------------------------------------------------
  b32 IsNeedForceResume   (void){ return m_isNeedForceResume;  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント状態がV字回復しSuspendが必要になったフレームかどうか
  *
  *  @note   Suspendが必要
  */
  //-----------------------------------------------------------------------------
  b32 IsNeedForceSuspend(void){ return m_isNeedForceSuspend; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント状態にするリクエストが何回発行されているか取得する
  *
  *  @return リクエスト発行数
  */
  //-----------------------------------------------------------------------------
  u32 GetEventRequestCnt(void){ return m_eventRequestCnt; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークに設定しているアクションコマンドを実行する
  */
  //-----------------------------------------------------------------------------
  void EnableActionCommand(void){ m_isExecuteActionCommand = true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークに設定しているアクションコマンドを実行しない
  */
  //-----------------------------------------------------------------------------
  void DisableActionCommand(void){ m_isExecuteActionCommand = false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドを実行中かどうか
  *
  *  @return 実行中ならtrue、なれければfalse
  */
  //-----------------------------------------------------------------------------
  b32 IsExecuteActionCommand(void){ return m_isExecuteActionCommand; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  動作ビット  ON
  *
  *  @param  fieldMoveModelStateBit  ビット
  */
  //-----------------------------------------------------------------------------
  virtual void OnStateBit(FieldMoveModelStateBit fieldMoveModelStateBit);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作ビット  OFF
  *
  *  @param  fieldMoveModelStateBit  ビット
  */
  //-----------------------------------------------------------------------------
  virtual void OffStateBit(FieldMoveModelStateBit fieldMoveModelStateBit);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作ビット  チェック
  *
  *  @param  fieldMoveModelStateBit  ビット
  *  @return trueはbitOn、falseはbitOff
  */
  //-----------------------------------------------------------------------------
  bool CheckStateBit(FieldMoveModelStateBit fieldMoveModelStateBit) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  1フレーム前の動作ビット  チェック
  *
  *  @param  fieldMoveModelStateBit  ビット
  *  @return trueはbitOn、falseはbitOff
  */
  //-----------------------------------------------------------------------------
  bool CheckStateBitPrev(FieldMoveModelStateBit fieldMoveModelStateBit) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの設定
  *
  *  @param  actionCommand  アクションコマンド
  */
  //-----------------------------------------------------------------------------
  void SetActionCommand(FieldActionCommandId actionCommand);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(回転)の設定
  *
  *  @param  actionCommand  アクションコマンド
  *  @param  dirY             回転Y
  *  @param  dirX             回転X
  *  @param  dirZ             回転Z
  *  @param  timeToUse        回転にかかるフレーム
  *  @param  isEnableIdleInit 待機モーションに切り替えるか
  *  @param  isCompressMotion 旋回モーションの再生をtimeToUseで指定したフレーム内で収めるかどうか
  *  @param  isEnableTurnMotionInit 旋回開始時にモーションを切り替えるか
  *  @param  isEnableIdleWait アイドルモーション待ちを行うか？
  */
  //-----------------------------------------------------------------------------
  void SetActionCommandRotation(FieldActionCommandId actionCommand,f32 dirY,f32 dirX,f32 dirZ, u32 timeToUse, b32 isEnableIdleInit = true, b32 isCompressMotion = false, b32 isEnableTurnMotionInit = true, b32 isEnableIdleWait = false );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(指定地点移動)の設定
  *
  *  @param  actionCommand  アクションコマンド
  *  @param  isWalk         歩きかどうか
  *  @param  pos            目標座標
  *  @param  qua            目標姿勢
  *  @param  isAdjustQua    目標姿勢を利用するかどうか
  *  @param  isEnableActionStartMove 歩き始めを再生するか
  *  @param  isEnableActionEndMove   歩き終わりを再生するか
  *  @param  thresholdAngleForMotion 旋回モーションを再生する角度差
  *  @param  isSlow         低速歩きかどうか
  *  @param  isCustomGoalCheck   ゴール判定を最適化するか(ワープがなくなる、momiji追加)
  */
  //-----------------------------------------------------------------------------
	void SetActionCommandTargetMove(FieldActionCommandId actionCommand, b32 isWalk, gfl2::math::Vector3 pos, gfl2::math::Quaternion qua, b32 isAdjustQua = true, b32 isEnableActionStartMove = true, b32 isEnableActionEndMove = true, f32 thresholdAngleForMotion = -1.0f, b32 isSlow = false, b32 isCustomGoalCheck = false, b32 isMoveEndIdleWait = true, b32 isRunToWalk = false, f32 runToWalkDistance = 0.0f, s32 mositonChangeFrame = -1 );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(任意モーションによる指定地点移動)の設定
  *
  *  @param  actionCommand  アクションコマンド
  *  @param  motionId       モーションID
  *  @param  pos            目標座標
  *  @param  qua            目標姿勢
  */
  //-----------------------------------------------------------------------------
  void SetActionCommandTargetMoveWithOptionalMotion(FieldActionCommandId actionCommand, u32 motionId,gfl2::math::Vector3 pos, gfl2::math::Quaternion qua);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(ステップ移動)の設定
  *
  *  @param  actionCommand   アクションコマンド
  *  @param  isWalk          歩きかどうか
  *  @param  stepCnt         ステップ数
  *  @param  enableCollision コリジョンON or OFF
  *  @param  moveRate        モーションによる移動ベクトルを適用する割合
  *  @param  isEnableWalkEnd     歩き終わりをおこなうか 
  *  @param  isEnableIdleInit    待機に設定して終了するか
  *  @param  isSlow              低速歩きかどうか
  */
  //-----------------------------------------------------------------------------
  void SetActionCommandStepMove(FieldActionCommandId actionCommand, b32 isWalk, u32 stepCnt, b32 enableCollision, f32 moveRate, b32 isEnableWalkEnd = true, b32 isEnableIdleInit = true, b32 isSlow = false );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンド(ステップ移動(開始・終了無し))の設定
  *
  *  @param  actionCommand   アクションコマンド
  *  @param  stepCnt         ステップ数
  *  @param  isOneFrame      1フレーム実行するかどうか
  */
  //-----------------------------------------------------------------------------
  void SetActionCommandStepMoveWithoutStartEnd(FieldActionCommandId actionCommand, u32 stepCnt, b32 isOneFrame = false);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにモーション連続再生アクションコマンドの設定
  *
  *  @param  actionCommand       アクションコマンド
  *  @param  pAnimationIdArray   アニメーションID配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pFrameStartArray    フレーム開始    配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pLoopMaxArray       ループ回数      配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  *  @param  pBlendArray         ブレンドフレーム配列(ActionCommandContinuousPlayback::REGIST_MAX_CNT長の配列を送ってください)
  */
  //-----------------------------------------------------------------------------
  void SetActionCommandContinuousPlayBack(FieldActionCommandId actionCommand,
                                          u32*                 pAnimationIdArray,
                                          s16*                 pFrameStartArray,
                                          s16*                 pLoopMaxArray,
                                          s16*                 pBlendArray);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの強制停止
  */
  //-----------------------------------------------------------------------------
  void ClearActionCommand(b32 isDispAssert);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  */
  //-----------------------------------------------------------------------------
  virtual void CheckActionCommand(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDの取得
  *
  *  @retval アクションコマンドID
  */
  //-----------------------------------------------------------------------------
  FieldActionCommandId GetActionCommandId(void){ return m_actionCommandWork.actionCommandId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief   リクエストヘッダーを取得
  *
  *  @return  リクエストヘッダー
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelHeaderWork     GetHeaderWork    (void){ return m_headerWork;     }
  FieldMoveModelHeaderResource GetHeaderResource(void){ return m_headerResource; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  キャラクターIDを取得
  *
  *  @param  キャラクターID
  */
  //-----------------------------------------------------------------------------
  u32 GetCharacterId(void){ return m_headerResource.characterId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードIDを取得
  *
  *  @param  動作コードID
  */
  //-----------------------------------------------------------------------------
  FIELD_MOVE_CODE_ID GetMoveCodeId(void) const { return m_headerWork.moveCodeId; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードワークを取得
  *
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeWork* GetMoveCodeWork(void){ return &m_moveCodeWork; }
  const FieldMoveCodeWork* GetMoveCodeWork(void) const { return &m_moveCodeWork; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード再開停止
  *  @note   FieldMoveModelManager::AddEventRequestAll関数以外から呼び出さないでください
  */
  //-----------------------------------------------------------------------------
  void MoveCodeResumeSuspend(void);

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  イベントID(識別用のユニークな値)を取得 
  *
  *  @return イベントID(識別用のユニークな値) 
  */ 
  //----------------------------------------------------------------------------- 
  u32 GetEventId(void){ return m_headerWork.eventId; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  作成時のゾーンIDを取得 
  *
  *  @return ゾーンID
  */ 
  //----------------------------------------------------------------------------- 
  u16 GetZoneId(void){ return m_headerWork.zoneId; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  真下にある地面が作成されているか設定
  */ 
  //----------------------------------------------------------------------------- 
  void SetCreateGround(b32 isCreateGround){ m_isCreateGround = isCreateGround; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  真下にある地面が作成されているか取得
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsCreateGround(void){ return m_isCreateGround; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に吸着させるかどうかを設定
  *
  *  @param  地面に吸着させるかどうか
  */ 
  //----------------------------------------------------------------------------- 
  void SetForceLanding(b32 isForceLanding){ m_isForceLanding = isForceLanding; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に吸着させるかどうかを取得
  *
  *  @return 地面に吸着させるかどうか
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsForceLanding(void){ return m_isForceLanding; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  地面に立っているかどうかを取得
  *
  *  @return 地面に立っているかどうか
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsOnGround(void){ return m_isOnGround; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  立っている地面の高さを取得
  *
  *  @return 地面の高さ
  */ 
  //----------------------------------------------------------------------------- 
  f32 GetGroundHeight(void)
  {
    if( !IsSetCollisionSceneContainerForGround() || !IsOnGround() )
    {
      return GetLocalSRT().translate.y;
    }
    return m_groundHeight;
  }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  立っている地面の法線を取得
  *
  *  @return 地面の法線
  */ 
  //----------------------------------------------------------------------------- 
  gfl2::math::Vector3 GetGroundNormal(void){ return m_groundNormal; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  座標が移動しておらず前回のレイキャストが失敗した場合でも、強制的にレイキャストを行わせるようにする
  */
  //-----------------------------------------------------------------------------
  void ResetFailedRaycastInfo(void)
  {
    if( m_isFailedRaycast )
    {
      m_isFailedRaycast = false;
    }
  }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  立っている地面の属性を取得
  *
  *  @return 地面の属性
  */ 
  //----------------------------------------------------------------------------- 
  u32 GetGroundAttribute(void){ return m_groundAttribute; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  一番最後に有効な地面の属性を取得
  *
  *  @return 地面の属性
  */ 
  //----------------------------------------------------------------------------- 
  u32 GetGroundAttributeLastSuccesful(void){ return m_groundAttributeLastSuccessful; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  1歩分進んだフレームかどうか
  *
  *  @return trueなら1歩分進み、falseなら進まなかったフレーム
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsOneStepFrame(void){ return m_isOneStepFrame; }

  //-----------------------------------------------------------------------------
  /** 
  *  @brief  旋回したフレームかどうか
  *
  *  @return trueなら旋回、falseなら旋回しなかったフレーム
  */ 
  //----------------------------------------------------------------------------- 
  b32 IsTurnFrame(void){ return m_isTurnFrame; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタに自身のインスタンスを登録
  *
  *  @param  pDispName  エディタに表示したい名前
  */
  //-----------------------------------------------------------------------------
  virtual void CreateRttiEditorPage(c16* pDispName);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIエディタから自身のインスタンスを削除
  */
  //-----------------------------------------------------------------------------
  virtual void DeleteRttiEditorPage(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  モーションシグナルの設定
  */
  //-----------------------------------------------------------------------------
  void SetMotionSignal( Field::MotionSignal *pSignal );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  モーションシグナルの取得
  */
  //-----------------------------------------------------------------------------
  Field::MotionSignal *GetMotionSignal( void );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  PrevUpdate関数に入ったタイミングでの自身の座標を取得
  *
  *  @return 座標
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 GetPositionPrevUpdateStart( void ){ return m_positionPrevUpdateStart; }
  gfl2::math::Vector3 GetPositionPrevUpdateEnd  ( void ){ return m_positionPrevUpdateEnd;   }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ワークからリソースを強制的に復元させる
  */
  //-----------------------------------------------------------------------------
  void EnableReturnFromWork(void){ m_isForceReturnFromWork = true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ワークからリソースを復元時に描画状況を復元させない
  */
  //-----------------------------------------------------------------------------
  void DisableVisibleForReturn(void){ m_enableVisibleForReturn = false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトを設定
  *
  *  @param  index 何番目の持ち物にするか
  *  @param  index 持たせたいサブオブジェクト
  *  @note   外したい場合はNULLを入れる
  */
  //-----------------------------------------------------------------------------
  void SetSubobject(u32 index,FieldSubobject* pSubobject,c8* jointName);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトを取得
  *
  *  @param  index 何番目の持ち物を取得するか
  *  @return サブオブジェクト
  */
  //-----------------------------------------------------------------------------
  FieldSubobject* GetSubobject(u32 index){ return m_pSubobject[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトのキャラクターIDを取得
  *
  *  @return  サブオブジェクトキャラクターID
  */
  //-----------------------------------------------------------------------------
  u32  GetSubobjectCharacterId(u32 index);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースが消される前にサブオブジェクトを持っていたかどうか
  *
  *  @param  サブオブジェクトの要素
  *  @return サブオブジェクトを持っていたか
  */
  //-----------------------------------------------------------------------------
  b32 IsSubobjectPrevClearResource(u32 index){ return m_isSubobjectPrevClearResource[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースが消される前に乗っていたサブオブジェクトのヘッダー
  *
  *  @param  サブオブジェクトの要素
  *  @return サブオブジェクトのリクエストヘッダー
  */
  //-----------------------------------------------------------------------------
  FieldSubobjectHeader GetSubobjectHeaderPrevClearResource(u32 index){ return m_subobjectHeaderPrevClearResource[index]; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを有効
  */
  //-----------------------------------------------------------------------------
  void EnableCulling(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを無効
  */
  //-----------------------------------------------------------------------------
  void DisableCulling(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  カリングを状態を取得
  *
  *  @return カリング状態か取得
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableCulilng    (void){ return m_isCulling;     }
  b32 IsEnableCulilngPrev(void){ return m_isCullingPrev; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトとしてモンスターボールを必要とするタイミングかどうか取得
  *
  *  @return trueなら必要、falseなら不要
  */
  //-----------------------------------------------------------------------------
  b32 IsNeedMonsterBall(void){ return m_isNeedMonsterBall; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトとしてライドギアを必要とするタイミングかどうか取得
  *
  *  @return trueなら必要、falseなら不要
  */
  //-----------------------------------------------------------------------------
  b32 IsNeedRideGear(void){ return m_isNeedRideGear; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  影をOriginジョイントにつけるかどうか設定
  *
  *  @param  trueならOrigin、falseならWaistにアタッチ
  */
  //-----------------------------------------------------------------------------
  void SetAttachShadowToOrigin(b32 isAttachShadowToOrigin)
  {
    if( !isAttachShadowToOrigin )
    {
      OnStateBit(FIELD_MOVE_MODEL_ATTACH_SHADOW_TO_WAIST);
    }
    else
    {
      OffStateBit(FIELD_MOVE_MODEL_ATTACH_SHADOW_TO_WAIST);
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  影をOriginジョイントにつけるかどうか取得
  *
  *  @return trueならOrigin、falseならWaistにアタッチ
  */
  //-----------------------------------------------------------------------------
  b32 IsAttachShadowToOrigin(void)
  {
    if( CheckStateBit(FIELD_MOVE_MODEL_ATTACH_SHADOW_TO_WAIST) )
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  現在の座標からジャンプ座標までの間に障害物がない、且つジャンプ座標周辺に障害物が無いかを調べる
  *
  *  @param  jumpPos         ジャンプ座標
  *  @param  acceptWallCnt   レイチェック時に許容する壁の枚数
  *  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
  *  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
  *  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
  *  @return trueなら障害物がなし、falseなら障害物があり
  */
  //-----------------------------------------------------------------------------
  b32 CheckEnableJump(const gfl2::math::Vector3& jumpPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定座標にコリジョンを配置した際、何箇所の壁に触れているか調べる
  *
  *  @param  checkPos   チェック座標
  *  @param  pAdjustPos チェック後の座標
  *  @return 触れている箇所の数
  */
  //-----------------------------------------------------------------------------
  u32 GetHitCntFromOshikaeshiScene(const gfl2::math::Vector3& checkPos,gfl2::math::Vector3* pAdjustPos);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  開始座標から終了座標までの間に指定枚数の壁があるか調べる
  *
  *  @param  startPos        開始座標
  *  @param  endPos          終了座標
  *  @param  acceptWallCnt   チェックする壁の枚数
  *  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
  *  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
  *  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
  *  @return trueなら指定枚数分の壁がある、falseならない
  */
  //-----------------------------------------------------------------------------
  b32 GetHitCntFromOshikaeshiScene(const gfl2::math::Vector3& startPos,const gfl2::math::Vector3& endPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  開始座標から終了座標までの間に指定枚数以下の壁があるか調べる(配置モデルの判定は含めません)
  *
  *  @param  startPos        開始座標
  *  @param  endPos          終了座標
  *  @param  acceptWallCnt   チェックする壁の枚数
  *  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
  *  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
  *  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
  *  @return trueなら指定枚数以下の壁がある、falseならない
  */
  //-----------------------------------------------------------------------------
  b32 GetHitCntFromOshikaeshiSceneEx(const gfl2::math::Vector3& startPos,const gfl2::math::Vector3& endPos,u32 acceptWallCnt,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アンレジストに起因するアニメーション停止を行うか？
  */
  //-----------------------------------------------------------------------------
  b32 IsAnimationStopByUnRegist(void) const;

  //
  // バミリ関連　開始
  //

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント時にフリー移動をした状態か取得する
  */
  //-----------------------------------------------------------------------------
  b32 IsFreeMoveForEventScript(void){ return m_isFreeMoveForEventScript;}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント時にフリー移動をした状態にする
  */
  //-----------------------------------------------------------------------------
  void EnableFreeMoveForEventScript(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント時にバミリ移動をした状態か取得する
  */
  //-----------------------------------------------------------------------------
  b32 IsPointMoveForEventScript(void){ return m_isPointMoveForEventScript;}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  イベント時にバミリ移動をした状態にする
  */
  //-----------------------------------------------------------------------------
  void EnablePointMoveForEventScript(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  バミリチェックフラグをクリア
  */
  //-----------------------------------------------------------------------------
  void ClearPositionCheckForEventScript(void);

  //
  // バミリ関連　終了
  //

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画パイプラインに自身のキャラモデルを追加
  */
  //-----------------------------------------------------------------------------
  void AddEdgeRenderingTarget(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  描画パイプラインから自身のキャラモデルを削除
  */
  //-----------------------------------------------------------------------------
  void RemoveEdgeRenderingTarget(void);

public:
  //-----------------------------------------------------------------------------
  /**
  *  @brief  モデルインスタンスノードを親から登録を外す
  */
  //-----------------------------------------------------------------------------
  void RemoveParentNode(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  モデルインスタンスノードを親に再設定する
  */
  //-----------------------------------------------------------------------------
  void ReSetParentNode(void);

private:
  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトとしてモンスターボールを必要とするタイミングかどうか更新
  */
  //-----------------------------------------------------------------------------
  void UpdateMonsterBallRequest(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  サブオブジェクトとしてライドギアを必要とするタイミングかどうか更新
  */
  //-----------------------------------------------------------------------------
  void UpdateRideGearRequest(void);

public:

  //================================================================================================================
  // ■ 親のインターフェースの実装
  //================================================================================================================

  //-----------------------------------------------------------------------------
  /**
  *  @brief 表示ON・OFFを設定
  *
  *  @param isVisible  表示ON・OFF
  */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(bool isVisible);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  表示ON・OFFを取得
  *
  *  @return 表示ON・OFF
  */
  //-----------------------------------------------------------------------------
  virtual bool GetVisible(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief SRTを設定
  *
  *  @param srt 設定したいSRT
  */
  //-----------------------------------------------------------------------------
  virtual void SetLocalSRT(const gfl2::math::SRT & srt);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  SRTを取得
  *
  *  @return SRT
  */
  //-----------------------------------------------------------------------------
  virtual const gfl2::math::SRT & GetLocalSRT(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンス数を取得
  *
  *  @return モデルインスタンス数
  */
  //-----------------------------------------------------------------------------
  virtual u32 GetModelInstanceNum(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンスを取得
  *
  *  @return モデルインスタンス
  */
  //-----------------------------------------------------------------------------
  virtual const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetModelInstance(int index) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルインスタンスの遅延解放は自由に実装
  *
  *  @note:
  *   削除リクエストを受けたモデルインスタンスを解放
  *   -実行タイミング  フィールドプロセスの先頭で実行します。
  *   -制限            PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
  */
  //-----------------------------------------------------------------------------
  virtual void ExecuteTerminateRequest(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  安全に破棄できる常態かチェック
  *
  *  @return 安全に解放できるかどうか
  */
  //-----------------------------------------------------------------------------
  virtual bool IsCanTerminate(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目開始
  *
  *  @note   注目をさせるためにはシステム側ではなくゲーム側が注目座標の変数を保持する必要があるので、
  *          注目開始と注目終了だけは動作モデル側で担当する
  */
  //-----------------------------------------------------------------------------
  void EnableInterest( gfl2::math::Vector3 interestPos )
  { 
    m_interestResetRequest = false;
    m_interestResetFrame   = 0;
    m_interestEnable       = true;
    m_interestPosition     = interestPos;
  }

  void EnableInterestMoveModel( FieldMoveModel* pTarget, s32 joint_index )
  { 
    if (!pTarget->IsSetup())
    {
      return;
    }
    
    m_pInterestTarget = pTarget;
    m_interestTargetJointIndex = joint_index;
    EnableInterest( m_pInterestTarget->GetCharaDrawInstance()->GetJointWorldPosition( joint_index ) );
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルを注目する設定を保持しているか？
  *
  *  @note   動作モデル注目解除中、動作モデル注目解除(首はそのまま)などはfalseが返る
  */
  //-----------------------------------------------------------------------------
  b32 HasInterestMoveModel(void) const { return (m_pInterestTarget != NULL); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目終了
  *
  *  @note   注目をさせるためにはシステム側ではなくゲーム側が注目座標の変数を保持する必要があるので、
  *          注目開始と注目終了だけは動作モデル側で担当する
  */
  //-----------------------------------------------------------------------------
  void DisableInterest( u32 frame )
  {
    if( !m_interestResetRequest && m_interestEnable )
    {
      m_interestResetRequest = true;
      m_interestResetFrame   = frame;
      m_pInterestTarget = NULL;
      m_interestTargetJointIndex = -1;
      if( m_pCharaDrawInstance )
      {
        for( u32 i=0; i<m_pCharaDrawInstance->GetInterestJointCount() ; i++)
        {
          m_pCharaDrawInstance->SetInterestResetFrameCount(i,frame);
        }
        m_pCharaDrawInstance->SetInterestPosition( NULL );
      }
    }
  }
  
  void DisableInterestMoveModel( b32 isContinue = true, u32 frame = 1 )
  {
    if (isContinue)
    {
      m_pInterestTarget = NULL;
      m_interestTargetJointIndex = -1;
    }
    else
    {
      DisableInterest( frame );
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  強制的に注目終了
  *
  *  @note   注目をさせるためにはシステム側ではなくゲーム側が注目座標の変数を保持する必要があるので、
  *          注目開始と注目終了だけは動作モデル側で担当する
  */
  //-----------------------------------------------------------------------------
  void DisableInterestForce( void )
  {
    if( m_interestEnable )
    {
      m_interestResetRequest = false;
      m_interestResetFrame   = 0;
      m_interestEnable       = false;
      m_interestPosition     = gfl2::math::Vector3(0.0f,0.0f,0.0f);
      m_pInterestTarget = NULL;
      m_interestTargetJointIndex = -1;

      if( m_pCharaDrawInstance )
      {
        for( u32 i=0; i<m_pCharaDrawInstance->GetInterestJointCount() ; i++)
        {
          m_pCharaDrawInstance->SetInterestResetFrameCount(i,0);
        }
        m_pCharaDrawInstance->SetInterestPosition( NULL );
      }
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目しているかどうか
  *
  *  @note   注目をさせるためにはシステム側ではなくゲーム側が注目座標の変数を保持する必要があるので、
  *          注目開始と注目終了だけは動作モデル側で担当する
  */
  //-----------------------------------------------------------------------------
  b32 isEnableInterest( void ){ return m_interestEnable; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目している座標を取得
  *
  *  @note   注目をさせるためにはシステム側ではなくゲーム側が注目座標の変数を保持する必要があるので、
  *          注目開始と注目終了だけは動作モデル側で担当する
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 GetInterestPosition( void ){ return m_interestPosition; }

#if 0
  //-----------------------------------------------------------------------------
  /**
  *  @brief  自動口パクをするかどうか
  *
  *  @param  isEnable 口パクのオンオフ
  */
  //-----------------------------------------------------------------------------
  void SetAutoKutipaku (b32 isEnable)
  { 
    m_isAutoKutipaku       = isEnable;
    m_AutoKutipakuSequence = 0;
    m_AutoKutipackFrame    = 0;
    m_pCharaDrawInstance->SetMouthIndex( poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE );
  }
  //-----------------------------------------------------------------------------
  /**
  *  @brief  自動口パクしているかどうかを得る
  */
  //-----------------------------------------------------------------------------
  b32 IsAutoKuktipaku() const { return m_isAutoKutipaku;  }
#endif

  //-----------------------------------------------------------------------------
  /**
  *  @brief  目アニメ制御モジュールを取得
  */
  //-----------------------------------------------------------------------------
  FieldFacialAnimeEye* GetFacialAnimeEye() const { return m_pFacialAnimeEye; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  口アニメ制御モジュールを取得
  */
  //-----------------------------------------------------------------------------
  FieldFacialAnimeMouth* GetFacialAnimeMouth() const { return m_pFacialAnimeMouth; }

protected:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  自動口パクの更処理
  */
  //-----------------------------------------------------------------------------
  void UpdateAutoKutipaku(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ワークからリソースの状態を元に戻す
  */
  //-----------------------------------------------------------------------------
  void ReturnResourceStateFromWork(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードを変更
  *
  *  @param  変更したい動作コードID
  */
  //-----------------------------------------------------------------------------
  virtual void ChangeMoveCode(FIELD_MOVE_CODE_ID moveCodeId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードに拡張ワークを設定
  *
  *  @param  ワークサイズ
  *  @param  ワーク
  *  @note   拡張ワークはサメハダー動作コードを起動する等に必要です
  */
  //-----------------------------------------------------------------------------
  void SetMoveCodeExtendWork(u32 extendWorkSize, u32* pExtendWork){ m_moveCodeExtendWorkSize = extendWorkSize; m_pMoveCodeExtendWork = pExtendWork; }

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  NPC泳ぎ波紋の取得
  */
  //-----------------------------------------------------------------------------
  Field::Effect::IEffectBase* GetCharaRipple(void){ return m_pCharaRipple; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  エンカウント草用のブロックとグリッド座標を更新
  */
  //-----------------------------------------------------------------------------
  void EncountGrassCreaterUpdateBlockAndGridPos(void);

  /**
  *  @brief  アクター用のワーク操作
  */
  void SetActorUserWork(void* pWork){ m_pActorUserWork = pWork; }
  void* GetActorUserWork(void){ return m_pActorUserWork; }
 
private:

  //! アロケータ
  System::nijiAllocator       *m_pLocalAllocator;
  Field::MyRenderingPipeLine  *m_pMyRenderingPipeLine;
  //! ステート
  FieldMoveModelHeaderWork     m_headerWork;
  FieldMoveModelHeaderResource m_headerResource;
  b32                          m_isSetupWork;
  b32                          m_isSetupResource;
  u32                          m_edgeId;
  u32                          m_stateBit;
  u32                          m_stateBitPrev;
  u32                          m_appDataType;
  s32                          m_stopPreUpdateFrame;
  u32                          m_eventRequestCnt;
  u32                          m_eventRequestCntPrev;
  b32                          m_isExecuteActionCommand;
  b32                          m_isExecuteActionCommandPrev;
  b32                          m_isCulling;
  b32                          m_isCullingPrev;
  b32                          m_isRegistToPipeline;
  gfl2::renderingengine::scenegraph::DagNode* m_pParentNode;

  //! キャラ用描画インスタンス
  poke_3d::model::CharaModel* m_pCharaDrawInstance;

  //! アニメーション
  u32                         m_defaultIdleAnimationId;
  s32                         m_animationId;
  b32                         m_isUseCustomValueForOrigin;
  f32                         m_customScaleForOrigin;
  gfl2::math::Quaternion      m_customQuaternionForOrigin;
  gfl2::math::Vector4         m_lastEnableOriginMoveVector;
  f32                         m_prevAnimationFrame;
  f32                         m_prevAnimationStepFrame;
  f32                         m_prevWalkSpeedY;
  b32                         m_interestEnable;
  gfl2::math::Vector3         m_interestPosition;
  s32                         m_interestResetFrame;
  b32                         m_interestResetRequest;
  b32                         m_interestInitSpeedRequest;
  FieldMoveModel*             m_pInterestTarget;
  s32                         m_interestTargetJointIndex;

  b32                         m_isAutoKutipaku;
  u32                         m_AutoKutipakuSequence;
  u32                         m_AutoKutipackFrame;
  //! ムーブコード
  const FieldMoveCodeBase    *m_pMoveCode;
  FieldMoveCodeWork           m_moveCodeWork;
  u32                         m_moveCodeExtendWorkSize;
  u32*                        m_pMoveCodeExtendWork;
  b32                         m_isNeedForceResume;
  b32                         m_isNeedForceSuspend;
  //! アクションコマンド
  ActionCommandWork           m_actionCommandWork;
  //! 表情アニメーション
  FieldFacialAnimeEye         *m_pFacialAnimeEye;
  FieldFacialAnimeMouth       *m_pFacialAnimeMouth;

  //! コリジョン
  b32                         m_isFailedRaycast;                          //< 前回のレイキャストが失敗しているかどうか(同じ座標でレイキャストをするとき処理を高速化するために必要です)
  b32                         m_isCreateGround;                           //< 真下にある地面が作成されているかどうか
  b32                         m_isForceLanding;                           //< 地面に吸着させるかどうか
  b32                         m_isOnGround;                               //< 地面に立っているかどうか
  u32                         m_groundAttribute;                          //< 立っている地面の属性
  u32                         m_groundAttributePrev;                      //< 立っている地面の属性(1F前)
  u32                         m_groundAttributeLastSuccessful;            //< 一番最後に有効な地面の属性
  f32                         m_groundHeight;                             //< 立っている地面の高さ
  f32                         m_groundHeightPrev;                         //< 立っている地面の高さ(1F前)
  gfl2::math::Vector3         m_groundNormal;                             //< 立っている地面の法線
  gfl2::math::Vector3         m_groundNormalPrev;                         //< 立っている地面の法線(1F前)
  DynamicActor*               m_pDynamicActor[COLLISION_TYPE_MAX];        //< 壁等から押し返しを受けるためのアクター
  StaticActor*                m_pStaticActor [COLLISION_STATIC_TYPE_MAX]; //< 壁になる為のアクター
  BaseCollisionScene*         m_pCollisionSceneContainerForGround;        //< 高さ取得用レイチェックで対象とするシーン
  BaseCollisionScene*         m_pCollisionSceneContainerForWall;          //< 表世界・裏世界取得用レイチェックで対象とするシーン
  gfl2::math::Vector3         m_positionPrevUpdateStart;
  gfl2::math::Vector3         m_positionPrevUpdateEnd;
  gfl2::math::Vector3         m_positionPrevAdjustGround;
  gfl2::math::Vector4         m_onTrianglePosition[3];
  gfl2::math::Vector4         m_onTriangleNormal;

  //! 別アプリからの復帰用
  b8                          m_isForceReturnFromWork;
  b8                          m_isNeedReturnFromWork;
  b8                          m_visibleForReturn;
  u8                          m_enableVisibleForReturn;
  gfl2::math::Vector3         m_positionForReturn;
  gfl2::math::Quaternion      m_quaternionForReturn;
  gfl2::math::SRT             m_srtForReturn;

  //! 旋回・一歩分進んだか調査用
  b32                         m_isTurnFrame;
  b32                         m_isOneStepFrame;
  f32                         m_oneStepCnt;
  u32                         m_oneStepMaxCnt;

  //! モーション中のシグナル処理
  MotionSignal                *m_pSignal;

  //! サブオブジェクト
  FieldSubobject*              m_pSubobject     [FIELD_MOVE_MODEL_SUB_OBJECT_CNT];
  s32                          m_pSubobjectJoint[FIELD_MOVE_MODEL_SUB_OBJECT_CNT];
  FieldSubobjectHeader         m_subobjectHeaderPrevClearResource[FIELD_MOVE_MODEL_SUB_OBJECT_CNT];
  b32                          m_isSubobjectPrevClearResource    [FIELD_MOVE_MODEL_SUB_OBJECT_CNT];
  b32                          m_isNeedMonsterBall;
  b32                          m_isNeedRideGear;

  //! エンカウント草エフェクト
  IFieldMoveModelEffectCreaterBase* m_pEncountGrassCreater;

  //! NPC泳ぎ波紋
  Field::Effect::IEffectBase* m_pCharaRipple;


  void*                       m_pActorUserWork;   // アクター毎に使うワーク


  // バミリ用(u32)
  b8                                           m_isFreeMoveForEventScript;
  b8                                           m_isPointMoveForEventScript;
};


}; //end of namespace MoveModel
}; //end of namespace Field