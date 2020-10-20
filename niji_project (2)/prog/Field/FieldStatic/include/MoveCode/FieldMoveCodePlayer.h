#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.h
*  @brief  フィールド用 動作コード プレイヤー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Quaternion.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodePlayer : public FieldMoveCodeBase
{
public:

  // 海底から水面までの高さ
  static const f32 WATER_DEPTH;

  // ステートの種類
  enum PlayerMove
  {
    PLAYER_MOVE_WAIT=0,
    PLAYER_MOVE_WAIT_TOKUSHU,
    PLAYER_MOVE_WAIT_DISCOVER,      // ムーランド専用ステート:発見状態待機
    
    PLAYER_MOVE_WALK_START,         // 通常歩き
    PLAYER_MOVE_WALK_LOOP,
    PLAYER_MOVE_WALK_LOOP_SLOPE,
    PLAYER_MOVE_WALK_LOOP_KUSAMURA,
    PLAYER_MOVE_WALK_END,

    PLAYER_MOVE_RUN_START,          // 走り
    PLAYER_MOVE_RUN_LOOP,
    PLAYER_MOVE_RUN_LOOP_SLOPE,
    PLAYER_MOVE_RUN_LOOP_KUSAMURA,
    PLAYER_MOVE_RUN_LOOP_DISCOVER,  // ムーランド専用ステート:発見状態走り
    PLAYER_MOVE_RUN_END,

    PLAYER_MOVE_RUN_TURN,           // 走り旋回

    PLAYER_MOVE_WALLHIT,            // 壁ぶつかり

    PLAYER_MOVE_PIYORI_RIGHT_START, // ぴよりポーズ(右)開始
    PLAYER_MOVE_PIYORI_RIGHT_LOOP,  // ぴよりポーズ(右)ループ
    PLAYER_MOVE_PIYORI_RIGHT_END,   // ぴよりポーズ(右)終了

    PLAYER_MOVE_PIYORI_LEFT_START,  // ぴよりポーズ(左)開始
    PLAYER_MOVE_PIYORI_LEFT_LOOP,   // ぴよりポーズ(左)ループ
    PLAYER_MOVE_PIYORI_LEFT_END,    // ぴよりポーズ(左)終了

    PLAYER_MOVE_IDLE_A,             // アイドル
    PLAYER_MOVE_IDLE_B,
    PLAYER_MOVE_IDLE_WARM,
    PLAYER_MOVE_IDLE_COOL,

    PLAYER_MOVE_TSUMADUKI,          // ダッシュボタンを連打した場合の躓き

    PLAYER_MOVE_MAX
  };

  // ステートの内容
  struct PlayerMoveSetting
  {
    c16  name[256];
    f32  moveSpeed;     // 移動速度
    f32  motionSpeed;   // モーション再生速度
    s32  motionId;      // 再生モーション
    f32  stickLevel;    // 入力値がこの値以下なら発動対象
  };

  // ステートの遷移等に必要なワーク
  struct Work
  {
    u8                     nextFoot;              // 次の走りor歩きで踏み出す足
    u8                     nowState;
    u8                     oldState;
    gfl2::math::Quaternion targetQua;
    f32                    angleToTarget;
    b32                    isTargetRightSide;     // 目標が右側にいるか
    u32                    piyoriNowValue;        // 現在のピヨリ値
    f32                    inputDir;              // スティックを倒している方向
    f32                    inputDirPrev;          // スティックを倒している方向(1フレーム前)
    b32                    isLockDir;             // 特定方向のスティック入力を無視するかどうか
    b32                    isLockFromOutside;     // 外部からロックされたかどうか
    f32                    lockDir;               // 無視をするスティックの方向
    u32                    moveStartTurnFrame;    // 待機から移動時の旋回レートを適用するフレーム
    b32                    isMoveSlope;           // 坂道を移動中かどうか
    u32                    moveSlopeCnt;          // 坂道を移動しているフレーム
    b32                    isSetupFromAppData;    // コンバートリストによるデータ初期化がなされたかどうか
    b32                    isDiscover;            // ムーランド専用:発見中かどうか
    u32                    waitFrame;             // 待機しているフレーム
    u32                    idleFrame;             // アイドルステートに遷移するフレーム
    u8                     idleState;             // 使用するアイドルステート
    u8                     isContinueMotion;      // ステートが切り替わってもモーションを切り替えなかったかどうか
    u8                     repeatDashButtonCnt;   // ダッシュボタン連打カウント
    u8                     timeLeftForRepeat;     // 連打と見做す残り時間
    b8                     isMoveKusamura;        // 草むらを移動中かどうか
    u8                     moveKusamuraCnt;       // 草むらを移動しているフレーム
    u8                     dashDisableFrame;      // Bボタンの入力を受け付けないフレーム
    u8                     lockDirDisableFrame;   // スティック入力無視機能を利用しないフレーム
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodePlayer(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodePlayer();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_PLAYER; }
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  ポスト
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void PostProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Resume(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback(FieldMoveCodeWork* pMoveCodeWork,b32 isLoad) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向をロックする
  *
  *  @param  pMoveCodeWork      動作コードワーク
  *  @param  isCallFromOutside  外部(イベント)からのロックかどうか
  *  @return trueなら成功、falseなら失敗
  *  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
  */
  //-----------------------------------------------------------------------------
  b32 LockDir(FieldMoveCodeWork* pMoveCodeWork,b32 isCallFromOutside) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動いている状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsMoveState(const FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  歩きループor歩きループ(坂道)or歩きループ(草むら)状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsWalkLoopState(const FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  走りループor走りループ(坂道)or走りループ(草むら)状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsRunLoopState(const FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  歩き終了状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsWalkEndState(const FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  走り終了状態かどうか
  *
  *  @return trueなら可能、falseなら不可
  */
  //-----------------------------------------------------------------------------
  b32 IsRunEndState(const FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  有効なスティック入力がされているかどうか
  *
  *  @return trueなら有効な入力あり、falseならなし
  */
  //-----------------------------------------------------------------------------
  b32 IsInputStick(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダッシュボタン入力がされているかどうか
  *
  *  @return trueなら有効な入力あり、falseならなし
  */
  //-----------------------------------------------------------------------------
  b32 IsInputDashButton(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダッシュボタン入力がされているかどうか(Trigger)
  *
  *  @return trueなら有効な入力あり、falseならなし
  */
  //-----------------------------------------------------------------------------
  b32 IsTrgInputDashButton(void) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  他からの入力を受け付ける状態かどうか
  *
  *  @return trueなら受け付ける、falseなら受け付けない
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableInput(const FieldMoveCodeWork* pMoveCodeWork) const;


  static PlayerMove GetPlayerMove(const FieldMoveCodeWork* pMoveCodeWork);

protected:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向のロックを解除
  *
  *  @param  pMoveCodeWork      動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void UnLockDir(FieldMoveCodeWork* pMoveCodeWork) const;

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向をロックしているかどうか
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
  */
  //-----------------------------------------------------------------------------
  b32 IsLockDir(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  最後に有効だった入力方向を外部(イベント)からロックしているかどうか
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
  */
  //-----------------------------------------------------------------------------
  b32 IsLockDirFromOutside(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

protected:

  //----------------------------------------------------------------------------
  /**
  *  @brief  アナログスティックの倒し具合からステートを取得
  *
  *  @param  stick  アナログスティックの倒し具合
  *  @return ステート
  */
  //-----------------------------------------------------------------------------
  u32 GetStateFromStickLevel( f32 stick ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual u32 MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const;

protected:

  //----------------------------------------------------------------------------
  /**
  *  @brief  デフォルトのスティック入力の補完レート(%)の設定
  *
  *  @param  interRate  補完レート(%)
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetInterpStickDefault(f32 interRate){ m_interpStickDefault = interRate; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  走り時のスティック入力の補完レート(%)の設定
  *
  *  @param  interRate  補完レート(%)
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetInterpStickRun(f32 interRate){ m_interpStickRun = interRate; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  走り旋回モーションのスピード(%)の設定
  *
  *  @param  turnSpeed  旋回スピード(%)
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetTurnSpeed(f32 turnSpeed){ m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN ].motionSpeed = turnSpeed/100.0f; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  走り旋回モーションの再生条件の設定
  *
  *  @param  turnDegree  旋回する角度
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetTurnDeg(u32 turnDeg){ m_runTurnDeg = turnDeg; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ぴより易さ
  *
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetPiyoriMaxValue(u32 piyoriMaxValue){ m_piyoriMaxValue = piyoriMaxValue; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  レバガチャとみなす角度
  *
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetPiyoriAngle(u32 piyoriAngle){ m_piyoriAngle = piyoriAngle; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  スティックをニュートラルとみなす範囲
  *
  *  @note   InitProcess関数以外では利用しないでください
  *          自身の動作コードを持つ全動作モデルに影響します
  */
  //-----------------------------------------------------------------------------
  void SetNeutralRange(u32 neutralRange){ m_neutralRange = neutralRange; }

private:

  //note: 動作コードに持たせてもいいメンバ変数は、自身の動作コードを持つ全動作モデルに影響させてもいいもののみ

  // 補完速度(%)
  u32 m_interpStickDefault;
  u32 m_interpStickRun;
  // 反転角度(Deg)
  u32 m_runTurnDeg;
  // ぴより易さ
  u32 m_piyoriMaxValue;
  // レバガチャとみなす角度
  u32 m_piyoriAngle;
  // スティックをニュートラルとみなす範囲
  u32 m_neutralRange;
  // 坂道での移動減衰率
  f32 m_slopeDampingRate;

protected:

  // 各ステートの内容
  PlayerMoveSetting m_PlayerMoveSettingTable[ PLAYER_MOVE_MAX ];

};  // end of class FieldMoveCodePlayer;
  
}; //end of namespace mmodel
}; //end of namespace fld
