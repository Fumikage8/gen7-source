#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelPlayer.h
*  @brief  フィールド用 プレイヤー動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <niji_conv_header/field/chara/chara_model_id.h>
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModel.h"
#include "Field/FieldStatic/include/Field3DObjectNode.h"
#include "Field/FieldStatic/include/FootSound/FieldFootSoundAccessor.h"
#include "FieldMoveModel.h"
#include "FieldMoveModel_define.h"
#include "../MoveCode/FieldMoveCode_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"

// エフェクト
#include <niji_conv_header/field/FieldEffectTypes.h>
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkJet.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkRun.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkWait.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLaplaceFlow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectLaplaceWait.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation1.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2AccelSmoke.h"

//=================================================================================================
//
// ※注意※  このクラスでは座標/姿勢/拡縮を保持してはいけません。
//           座標/姿勢/拡縮を保持して良いのは描画インスタンス(poke_3d::model::CharaModel)のみです。
//           ただ諸々の処理を簡単/高速に行うために、描画インスタンスと同内容のツリーを形成する必要があります
//
//=================================================================================================
namespace Field{ namespace MoveModel {

class FieldMoveModelPlayer : public FieldMoveModel
{
public:

  //-------------------------------------------------------------------
  // ライドポケモンのデータについて
  // 
  // キャラID・動作コードID・モーションスロットは決まった順番で並べる！
  //
  // 0番目：ケンタロス
  // 1番目：サメハダー
  // 2番目：ムーランド
  // 3番目：地面馬
  // 4番目：カイリキー
  // 5番目：ラプラス
  //
  //-------------------------------------------------------------------

  // ライドポケモンの総数
  static const u32 RIDE_POKEMON_MAX_CNT = 6;

  // ライドポケモンの動作コードID
  static const u32 RIDE_POKEMON_MOVE_CODE_ARRAY[ RIDE_POKEMON_MAX_CNT ];

  // ライドポケモンのキャラID
  static const u32 RIDE_POKEMON_CHARA_ID_ARRAY[ RIDE_POKEMON_MAX_CNT ];

  // ライドポケモン全体での一番最初のモーション
  static const u32 RIDE_POKEMON_MOTION_START    = P2_BASE_RI001_WAIT01;
  static const u32 RIDE_POKEMON_MOTION_STRIDE   = 20;

  // キャラIDからライドポケモンのインデックスを調べる
  static u32 GetRideIndex( u32 charaId )
  { 
    for( u32 i=0; i<RIDE_POKEMON_MAX_CNT; i++)
    { 
      if( RIDE_POKEMON_CHARA_ID_ARRAY[i]==charaId ){ return i; }
    }
    return RIDE_POKEMON_MAX_CNT;
  }


  static const f32 MAX_COLLISION_R; ///< コリジョン最大幅

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelPlayer();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModelPlayer();

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
  *  @brief  後更新(座標の決定後に必要な処理を行う)
  *
  *  @param  pCharaModelFactory  キャラリソースファクトリー
  */
  //-----------------------------------------------------------------------------
  virtual void PostUpdate(poke_3d::model::CharaModelFactory* pCharaModelFactory);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  エフェクト生成
  */
  //-----------------------------------------------------------------------------
  virtual void UpdateEffect(void);
  void         UpdateEffectStepJump(void);
  void         UpdateEffectFoot(void);
  void         UpdateEffectBreath(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  乗っている動作モデルを取得
  *
  *  @return 乗っている動作モデル
  */
  //-----------------------------------------------------------------------------
  FieldMoveModel* GetOnMoveModel(void){ return m_pRideOnMoveModel; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルに乗る
  *
  *  @param  pFieldMoveModel    乗りたい動作モデル
  */
  //-----------------------------------------------------------------------------
  void RideOnMoveModel (FieldMoveModel* pFieldMoveModel);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作モデルから降りる
  */
  //-----------------------------------------------------------------------------
  void RideOffMoveModel(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  他の動作モデルに乗っているかどうか
  *
  *  @return 乗っているかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsRideMoveModel(void) const { return m_pRideOnMoveModel ? true : false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースが消される前に他の動作モデルに乗っていたかどうか
  *
  *  @return 乗っていたかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsRideMoveModelPrevClearResource(void){ return m_isRideMoveModelPrevClearResource; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  波乗り用壁にあたっているかどうか
  *
  *  @return あたっているかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsHitNaminoriWall(void){ return GetDynamicActor(COLLISION_TYPE_EVENT_NAMINORI)->GetCollisionResult(0)->pTargetActor ? true : false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ジャンプした先で地上系ライドできるかどうか(間の障害物を考慮する)
  *
  *  @param  check_pos          チェック座標
  *  @param  pOutAdjustPos      ライド状態時に降りるであろう座標
  *  @param  isCheckNaminori    チェックする壁に押し返しがある波乗り    フィルターも含める
  *  @param  isCheckRoba2       チェックする壁に押し返しがあるロバ2     フィルターも含める
  *  @param  isCheckRide        チェックする壁に押し返しがあるライド禁止フィルターも含める
  *  @param  isUseRideCollision ライドのコリジョン幅を使って判定をするかどうか
  *  @return 地上系ライド可否
  *  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableGroundRideBetween(const gfl2::math::Vector3& check_poss,gfl2::math::Vector3* pOutAdjustPos,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide,b32 isUseRideCollision=true);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ジャンプした先で地上系ライドできるかどうか(間の障害物を考慮しない)
  *
  *  @param  check_pos          チェック座標
  *  @param  p_pushout_pos      壁に押し出されたのであれば押し出された座標
  *  @param  isUseRideCollision ライドのコリジョン幅を使って判定をするかどうか
  *  @return 地上系ライド可否
  *  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableGroundRide(const gfl2::math::Vector3& check_pos, gfl2::math::Vector3* p_pushout_pos,b32 isUseRideCollision=true);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ジャンプしたい座標にキチンとジャンプできるかどうか
  *
  *  @param  check_pos       チェック座標
  *  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
  *  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
  *  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
  *  @return 解決可否
  *  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableStepJump(const gfl2::math::Vector3& check_pos,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  地上ライドした際に地面馬壁に当たっているかどうか
  *
  *  @return あたっているかどうか
  *  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
  */
  //-----------------------------------------------------------------------------
  b32 IsHitRoba2WallGroundRide(void);
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  非ライド状態になった際に地面馬壁に当たっているかどうか
  *
  *  @return あたっているかどうか
  *  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
  */
  //-----------------------------------------------------------------------------
  b32 IsHitRoba2WallRideOff(void);
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  リソースが消される前に乗っていた動作モデルのリクエストヘッダー
  *
  *  @return リクエストヘッダー
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelHeaderWork     GetRideMoveModelHeaderWorkPrevClearResource    (void){ return m_rideMoveModelHeaderWorkPrevClearResource;     }
  FieldMoveModelHeaderResource GetRideMoveModelHeaderResourcePrevClearResource(void){ return m_rideMoveModelHeaderResourcePrevClearResource; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ポケライド情報を上書き
  *
  *  @param  ポケライド用ワークヘッダー
  *  @param  ポケライド用リソースヘッダー
  *  @note   セーブ復帰時またはフィールドが無い状態以外では利用しないでください
  */
  //-----------------------------------------------------------------------------
  void SetRideMoveModelHeader(FieldMoveModelHeaderWork rideMoveModelHeaderWork,FieldMoveModelHeaderResource rideMoveModelHeaderResource)
  { 
    m_isRideMoveModelPrevClearResource             = true;
    m_rideMoveModelHeaderWorkPrevClearResource     = rideMoveModelHeaderWork; 
    m_rideMoveModelHeaderResourcePrevClearResource = rideMoveModelHeaderResource;
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ポケライド情報を初期化
  *
  *  @note   セーブ復帰時またはフィールドが無い状態以外では利用しないでください
  */
  //-----------------------------------------------------------------------------
  void InitRideMoveModelHeader()
  { 
    m_isRideMoveModelPrevClearResource             = false;
    m_rideMoveModelHeaderWorkPrevClearResource.Init();
    m_rideMoveModelHeaderResourcePrevClearResource.Init();
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  一番最後に乗った海用ライドのキャラクターIDを取得
  */
  //-----------------------------------------------------------------------------
  u32 GetLastSeaRideCharacterId()
  {
    return m_lastSeaRideCharacterId;
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  現在がフリームーブ中で且つ入力を受け付ける状態かどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableInput(void){ return (!GetEventRequestCnt() && !IsExecuteActionCommand() && IsEnbaleInputMoveCode() ) ? true:false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  主人公が男かどうか
  *
  *  @return trueなら男、falseなら女
  */
  //-----------------------------------------------------------------------------
  b32 IsMale(void){ return (GetCharacterId() == CHARA_MODEL_ID_PC0001_00)? true:false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  各エフェクトを取得
  *
  *  @return NULLならアンロード
  */
  //-----------------------------------------------------------------------------
  Field::Effect::EffectDashSmoke*            GetKentarosRushSmoke        (void){ return m_pEffectDashSmoke;                 }
  Field::Effect::EffectSharkJet*             GetSamehaderJet             (void){ return m_pEffectSharkJet;                  }
  Field::Effect::EffectSharkRun*             GetSamehaderRun             (void){ return m_pEffectSharkRun;                  }
  Field::Effect::EffectSharkWait*            GetSamehaderWait            (void){ return m_pEffectSharkWait;                 }
  Field::Effect::EffectLaplaceWait*          GetLaplaceWait              (void){ return m_pEffectLaplaceWait;               }
  Field::Effect::EffectLaplaceFlow*          GetLaplaceFlow              (void){ return m_pEffectLaplaceFlow;               }
  Field::Effect::EffectExclamation1*         GetMuurandoExclamationNormal(void){ return m_pEffectMuurandoExclamationNormal; }
  Field::Effect::EffectExclamation2*         GetMuurandoExclamationEx    (void){ return m_pEffectMuurandoExclamationEx;     }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向を外部(イベント)からロックする
  *
  *  @return trueなら成功、falseなら失敗
  *  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
  */
  //-----------------------------------------------------------------------------
  b32 LockDirFromOutside(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向を外部(イベント)からロックしているかどうか
  *
  *  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
  */
  //-----------------------------------------------------------------------------
  b32 IsLockDirFromOutside(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動いている状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsMoveState(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効なスティック入力がされているかどうか
  *
  *  @return trueなら有効な入力あり、falseならなし
  */
  //-----------------------------------------------------------------------------
  b32 IsInputStick(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダッシュボタン入力がされているかどうか
  *
  *  @return trueなら有効な入力あり、falseならなし
  */
  //-----------------------------------------------------------------------------
  b32 IsInputDashButton(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  歩きループor歩きループ(坂道)状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsWalkLoopState(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  走りループor走りループ(坂道)状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsRunLoopState(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  歩き終了状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsWalkEndState(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  走り終了状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsRunEndState(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ムーランドのサーチ状態かどうか
  *
  *  @return trueならその状態、falseならその状態ではない
  */
  //-----------------------------------------------------------------------------
  b32 IsMuurandoSearchState(void) const;
  b32 IsMuurandoSearchStateEx(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードが他からの入力を受け付ける状態かどうか
  *
  *  @return trueなら受け付ける、falseなら受け付けない
  */
  //-----------------------------------------------------------------------------
  b32 IsEnbaleInputMoveCode(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
  */
  //-----------------------------------------------------------------------------
  virtual void CheckActionCommand(void);

protected:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コードを変更
  *
  *  @param  変更したい動作コードID
  */
  //-----------------------------------------------------------------------------
  virtual void ChangeMoveCode(FIELD_MOVE_CODE_ID moveCodeId);

private:

  // サウンド用
  FootSoundAccessor                            m_accessor;

  // ポケライド用
  FieldMoveModel*                              m_pRideOnMoveModel;
  gfl2::renderingengine::scenegraph::DagNode*  m_pRideOnMoveModelParentDagNode;
  FIELD_MOVE_CODE_ID                           m_defaultMoveCodeId;

  // ムーブコード
  static const u32                             FIELD_MOVE_CODE_EXTEND_WORK_SIZE = 512;
  u32                                          m_moveCodeExtendWork[FIELD_MOVE_CODE_EXTEND_WORK_SIZE/4];

  // ポケライドからの復帰用
  b32                                          m_isRideMoveModelPrevClearResource;
  FieldMoveModelHeaderWork                     m_rideMoveModelHeaderWorkPrevClearResource;
  FieldMoveModelHeaderResource                 m_rideMoveModelHeaderResourcePrevClearResource;
  u32                                          m_defaultIdleAnimationIdPrevRide;
  u32                                          m_lastSeaRideCharacterId;

  // エフェクト
  Field::Effect::EffectDashSmoke*              m_pEffectDashSmoke;
  Field::Effect::EffectSharkJet*               m_pEffectSharkJet;
  Field::Effect::EffectSharkRun*               m_pEffectSharkRun;
  Field::Effect::EffectSharkWait*              m_pEffectSharkWait;
  Field::Effect::EffectLaplaceWait*            m_pEffectLaplaceWait;
  Field::Effect::EffectLaplaceFlow*            m_pEffectLaplaceFlow;
  Field::Effect::EffectExclamation1*           m_pEffectMuurandoExclamationNormal;
  Field::Effect::EffectExclamation2*           m_pEffectMuurandoExclamationEx;
  static const u32                             ROBA2_ACCEL_EFFECT_NUM = 4;
  Field::Effect::EffectRoba2AccelSmoke*        m_pEffectRoba2AccelSmoke[ ROBA2_ACCEL_EFFECT_NUM ];
};

}; //end of namespace MoveModel
}; //end of namespace Field
