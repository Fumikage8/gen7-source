#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeNpc.cpp
*  @brief  フィールドNPC用 動作コード
*  @author Junya Ikuta
*  @date   2016.03.23
*/
//=================================================================================================

// gfl2
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

// Field
#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"

namespace Field{ namespace MoveModel {

// NPC挙動   注目[無] 回転[無] 初期配置戻り[有] ( MOVE_CODE_NPC_MEET_RETURN )
class FieldMoveCodeNpc : public FieldMoveCodeBase
{
public:

  // オプション指定
  struct Option
  {
    enum Value
    {
      INVALID_ROTATION = 0x01,        // 回転無効化
      INVALID_INTEREST = 0x01 << 1,   // 注目無効化
      INVALID_RETURN   = 0x01 << 2,   // 初期状態に戻る、無効化
      ENABLE_LOOK_WIDE  = 0x01 << 3,   // 注目範囲拡大版
      ENABLE_LOOK_SHORT = 0x01 << 4,   // 注目範囲縮小版
    };
  };

  // 回転状態定義
  struct RotationState
  {
    enum Value
    {
      INIT = 0,         // 初期化
      MAIN,             // 回転中
      END
    };
  };

  // 保存情報(回転用)
  struct RotationWork
  {
    f32                    startRotationY;
    f32                    targetRotationY;
    u16                    frame;
    u16                    frameMax;
    b8										 isReturn;
  };
  
  // 保存情報
  struct Work
  {
    u32 option;             // 挙動指定オプション(現在)
    u8  state;
    u8  nextState;
    u32 count;
    u16 rotEnterWaitCount;  // 回転範囲に入った後の回転実行待ちカウンタ、指定フレーム加算で実行
    u16 dirResetWaitCount;  // 向きリセットするまでの待ち時間カウンタ、0で実行
    b8  isDirResetWait;     // 向きリセット待ち中か
    u8  dummy;
    u8  rotationCount;   // 回転実行回数記憶

    b8  isTalkEnd;            // 会話済みか？  再生成でリセットされる
    u16 frontInterestCount;   // 前方で注目している期間のカウント
    f32 returnRotationDegY;   // 方向戻し機能で戻す方向

    // 回転用情報
    RotationWork  rotationWork;
    
    
    u32 defaultOption;      // 挙動指定オプション(初期状態)
    u8  dirDirtyFlag:1;     // 方向が変わっているか？
    u8  hideInterestFlag:1;       // 背後の注目が発生しているか？ (トリガー判定に仕様)
    u8  hideInterestFirstDir:1;   // 背後の注目が発生した際の最初の向き( 0:右 1:左 )
    u8  padding1:5;
    u8  padding2;
    u16 padding3;
    s32 prevMotionId;
  };

  // 状態定義
  struct State
  {
    enum Value
    {
      READY = 0,        // 初回更新
      IDLE,             // 通常待機
      ROTATION,         // 回転中
    };
  };



  // 挙動設定
  struct SETTING
  {
    f32 degreeRangeForInterest;           // 注目発動範囲[角度]
    f32 degreeRangeForRotation;           // 回転発動範囲[角度]

    f32 distanceRangeForInterest;         // 注目発動範囲[距離]
    f32 distanceRangeForInterestWide;     // 注目発動範囲(ワイド版)[距離]
    f32 distanceRangeForInterestShort;    // 注目発動範囲(縮小版)[距離]
    f32 distanceRangeForRotation;         // 回転発動範囲[距離]

    // 内側の判定を別に持つ
    f32 inSideDegreeRangeForInterest;     // 注目発動範囲[角度][内側]
    f32 inSideDegreeRangeForRotation;     // 回転発動範囲[角度][内側]
    f32 inSideDistanceRangeForInterest;   // 注目発動範囲[距離][内側]
    f32 inSideDistanceRangeForRotation;   // 回転発動範囲[距離][内側]

    // 最近接の判定を別に持つ
    f32 nearestDegreeRangeForInterest;    // 注目発動範囲[角度][最近接]
    f32 nearestDegreeRangeForRotation;    // 回転発動範囲[角度][最近接]
    f32 nearestDistanceRangeForInterest;  // 注目発動範囲[距離][最近接]
    f32 nearestDistanceRangeForRotation;  // 回転発動範囲[距離][最近接]

    f32 talkEndResetDistanceRange;        // 会話終了をリセットする距離
    f32 dirResetDistanceRange;            // 向きをリセットする距離
    u32 dirResetWaitBase;                 // 向きをリセットする際の待ちフレーム(最低保障)
    u32 dirResetWaitRand;                 // 向きをリセットする際の待ちフレーム(最低保障に足しこむ乱数値)
    
    f32 talkEndResetDistanceRangeShort;   // 会話終了をリセットする距離(縮小版)
    f32 dirResetDistanceRangeShort;       // 向きをリセットする距離(縮小版)

    f32 degreeDiffForRotationChange;      // 回転を実行する角度の差分
    u32 enterWaitForInterestToRotation;   // 回転発動待ち時間、このフレーム間注目範囲にいたら回転を実行する


    u32 disableInterestFrame;             // 注目解除にかけるフレーム
    u32 enterWaitForReturn;               // 元の状態に戻す発動待ち時間、このフレーム間何も反応がなければ実行する


    u32 rotationFrame;                    // 回転にかけるフレーム数
    b32 isCullingReset;                   // true:カリング時に再度有効化される
    f32 hideRangeDeg;                     // 背後扱いの角度(片翼)
  };

#if PM_DEBUG
  
  // 各ジョイントの注視用設定
  struct DEBUG_INTEREST_LIMIT
  {
    f32 _Low;
    f32 _High;
    f32 _Low2;
    f32 _High2;
    f32 _Speed;
  };
  
  // デバッグ用挙動設定
  struct DEBUG_SETTING
  {
    b32 isUse;                            // デバッグ設定を使うか？
    b32 invalidRotation;                  // 回転無効化
    b32 invalidInterest;                  // 注目無効化
    b32 invalidReturn;                    // 初期状態に戻る、無効化
    b32 enableLookWide;                   // 注目範囲拡大版
    b32 enableLookShort;                  // 注目範囲縮小版

    b32 isMcnfOverWrite;                    // モーションコンフィグファイルを上書きする
    DEBUG_INTEREST_LIMIT interestLimit[2];  // 0:Neck 1:Head
  };
#endif


  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpc(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpc();

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  会話開始処理  話しかけによるスクリプト起動の直前に実行される
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static void TalkStart(FieldMoveCodeWork* pMoveCodeWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目終了処理  イベントチェックでイベント起動時に実行される
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static void StopInterest(FieldMoveCodeWork* pMoveCodeWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定のオプションを有効化する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  optionBit      指定するオプション
  */
  //-----------------------------------------------------------------------------
  static void OnOption(FieldMoveCodeWork* pMoveCodeWork, u32 optionBit );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  指定のオプションを無効化する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  optionBit      指定するオプション
  */
  //-----------------------------------------------------------------------------
  static void OffOption(FieldMoveCodeWork* pMoveCodeWork, u32 optionBit );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  オプションを初期状態に戻します
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  optionBit      指定するオプション
  */
  //-----------------------------------------------------------------------------
  static void ResetOption(FieldMoveCodeWork* pMoveCodeWork );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  話しかけ済み状態を解除します
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static void ClearTalkEnd(FieldMoveCodeWork* pMoveCodeWork );
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  ダーティフラグ状態をクリアします
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static void ClearDirtyFlag(FieldMoveCodeWork* pMoveCodeWork );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  戻すべき向きを上書きします
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  deg            戻すべき向き(度)
  */
  //-----------------------------------------------------------------------------
  static void OverWriteReturnAngle(FieldMoveCodeWork* pMoveCodeWork, f32 deg );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  NPCアクションが起きていないか取得する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static b32 IsNoAction(FieldMoveCodeWork* pMoveCodeWork );

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief  設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
  static SETTING* GetSettingData( void ) { return &m_Setting; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  デバッグ設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
  static DEBUG_SETTING* GetDebugSettingData( void ) { return &m_DebugSetting; }
#endif

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目対象キャラから注目座標を取得する
  *
  *  @param  pMoveModel 注目対象
  *  @param  pOut       注目座標
  */
  //-----------------------------------------------------------------------------
  static void GetInterestPosition(FieldMoveModel* pMoveModel, gfl2::math::Vector3* pOut);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目キャラの体の厚みを取得
  *
  *  @param  pMoveModel 対象
  *  @param  isPlayer   対象がプレイヤーならtrueを指定
  *
  *  @return 厚み(cm)
  */
  //-----------------------------------------------------------------------------
  static f32 GetBodyWidth(FieldMoveModel* pMoveModel, b32 isPlayer);

protected:

  //----------------------------------------------------------------------------
  /**
  *  @brief  初回更新処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubReadyProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  待機中処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubIdleProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  回転中処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  注視範囲にヒットしているかチェック　
  *
  *  @param  distanceSq       距離の二乗
  *  @param  rotationDiffDeg  向きの差分(度)
  *  @param  playerBodyWidth  自機の厚み
  *  @param  npcBodyWidth     NPCの厚み
  *  @param  isWide           範囲を拡大する
  *  @param  isShort          範囲を縮小する
  *  @param  pOutHitFront     前方範囲でのHITならtrue
  *
  *  @retval true ヒットしている
  */
  //-----------------------------------------------------------------------------
  virtual b32 HitCheckInterestRange( f32 distanceSq, f32 rotationDiffDeg, f32 playerBodyWidth, f32 npcBodyWidth, b32 isWide, b32 isShort, b32* pOutHitFront ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  回転範囲にヒットしているかチェック　
  *
  *  @param  distanceSq       距離の二乗
  *  @param  rotationDiffDeg  向きの差分(度)
  *  @param  playerBodyWidth  自機の厚み
  *  @param  npcBodyWidth     NPCの厚み
  *
  *  @retval true ヒットしている
  */
  //-----------------------------------------------------------------------------
  virtual b32 HitCheckRotationRange( f32 distanceSq, f32 rotationDiffDeg, f32 playerBodyWidth, f32 npcBodyWidth ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  会話リセット範囲にヒットしているかチェック　
  *
  *  @param  distanceSq       距離の二乗
  *  @param  playerBodyWidth  自機の厚み
  *  @param  npcBodyWidth     NPCの厚み
  *  @param  isShort          範囲を縮小する
  *
  *  @retval true ヒットしている
  */
  //-----------------------------------------------------------------------------
  virtual b32 HitCheckTalkResetRange( f32 distanceSq, f32 playerBodyWidth, f32 npcBodyWidth, b32 isShort ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  向きリセット範囲にヒットしているかチェック　
  *
  *  @param  distanceSq       距離の二乗
  *  @param  playerBodyWidth  自機の厚み
  *  @param  npcBodyWidth     NPCの厚み
  *  @param  isShort          範囲を縮小する
  *
  *  @retval true ヒットしている
  */
  //-----------------------------------------------------------------------------
  virtual b32 HitCheckDirResetRange( f32 distanceSq, f32 playerBodyWidth, f32 npcBodyWidth, b32 isShort ) const;
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  回転開始処理　
  *
  *  @param  pMoveCodeWork        動作コードワーク
  *  @param  targetRotationDegY   回転後の方向(度)
  *  @param  frame                回転にかけるフレーム
  *  @param  isReturn             true = 初期状態に戻る回転
  *  @param  isCompressMotion     true = モーションの速度を旋回速度にあわせる
  */
  //-----------------------------------------------------------------------------
  virtual void StartRotation(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY, u32 frame, b32 isReturn = false, b32 isCompressMotion = true, s32 nextState = State::IDLE ) const;
	virtual b32 CheckRotationDir(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY ) const; // true:左

  //----------------------------------------------------------------------------
  /**
  *  @brief  デフォルト待機モーション開始( 既に待機モーションなら何も再生しない )
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval モーションID
  */
  //-----------------------------------------------------------------------------
  virtual void StartDefaultIdleMotion(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  振り向いた後のモーションIDを取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval モーションID
  */
  //-----------------------------------------------------------------------------
  static u32 GetRotationAfterMotionId(FieldMoveCodeWork* pMoveCodeWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  現在の向きを戻り先向きとして保存する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval 無し
  */
  //-----------------------------------------------------------------------------
  static void SaveNowDirForReturn(FieldMoveCodeWork* pMoveCodeWork);
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目座標を更新する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  rPos           注目座標
  */
  //-----------------------------------------------------------------------------
  void UpdateInterestPosition( FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rPos ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目を解除する
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  static void DisableInterest( FieldMoveCodeWork* pMoveCodeWork );




  // オプション確認関連

  //-----------------------------------------------------------------------------
  /**
  *  @brief  回転オプションが有効か取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 有効
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableRotation(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目オプションが有効か取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 有効
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableInterest(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期状態に戻るオプションが有効か取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 有効
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableReturn(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目範囲拡大オプションが有効か取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 有効
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableLookWide(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  注目範囲縮小オプションが有効か取得
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 有効
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableLookShort(FieldMoveCodeWork* pMoveCodeWork) const;

#if PM_DEBUG
  //-----------------------------------------------------------------------------
  /**
  *  @brief  デバッグ設定を反映
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void ApplyDebugSetting(FieldMoveCodeWork* pMoveCodeWork) const;
#endif // PM_DEBUG

  static SETTING m_Setting;             // 挙動設定、デバッグメニューで操作します

#if PM_DEBUG
  static DEBUG_SETTING m_DebugSetting;  // デバッグ用挙動設定、デバッグメニューで操作します
#endif // PM_DEBUG

};  // end of class FieldMoveCodeNpc;
  

//
// NPC挙動 @note 違いはOptionの初期化だけなので、同じファイルに書いてしまいます
//

// NPC挙動   注目[無] 回転[無] 初期配置戻り[有]
class FieldMoveCodeNpcReturn : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcReturn(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcReturn();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcReturn

// NPC挙動    注目[有] 回転[有] 初期配置戻り[無]
class FieldMoveCodeNpcMeet : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcMeet(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcMeet();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcMeet

// NPC挙動    注目[有] 回転[無] 初期配置戻り[無]
class FieldMoveCodeNpcLook : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcLook(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcLook();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcLook

// NPC挙動    注目[有] 回転[無] 初期配置戻り[有]
class FieldMoveCodeNpcLookReturn : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcLookReturn(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcLookReturn();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcLookReturn

// NPC挙動　　注目[有] 回転[有] 初期配置戻り[無]  注目距離縮小
class FieldMoveCodeNpcMeetShort : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcMeetShort(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcMeetShort();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcMeetShort

// NPC挙動　　注目[有] 回転[有] 初期配置戻り[有]  注目距離縮小
class FieldMoveCodeNpcMeetShortReturn : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcMeetShortReturn(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcMeetShortReturn();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcMeetShortReturn

// NPC挙動   注目[有] 回転[無] 初期配置戻り[無]  注目距離縮小
class FieldMoveCodeNpcLookShort : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcLookShort(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcLookShort();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcLookShort

// NPC挙動   注目[有] 回転[無] 初期配置戻り[有]  注目距離縮小
class FieldMoveCodeNpcLookShortReturn : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcLookShortReturn(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcLookShortReturn();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

}; // class FieldMoveCodeNpcLookShort


// NPC挙動   (預かり屋 ※育て屋前のNPC)
class FieldMoveCodeNpcAzukariya : public FieldMoveCodeNpc
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeNpcAzukariya(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeNpcAzukariya();

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  /**
  *  @brief  動作コード処理  復帰
  */
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const;

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  タマゴ発見モーション開始( 既に待機モーションなら何も再生しない )
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  forceFlag      true: モーション補間無しで切り替え
  *
  *  @retval モーションID
  */
  //-----------------------------------------------------------------------------
  virtual void StartEggExistMotion(FieldMoveCodeWork* pMoveCodeWork, b32 forceFlag = false ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  タマゴ発見しているか？
  *
  *  @retval true 発見している false 発見していない 
  */
  //-----------------------------------------------------------------------------
  virtual b32 IsEggExist( void ) const;


}; // class FieldMoveCodeNpcAzukariya


// ふれあい挙動
class FieldMoveCodeFureaiTypeA : public FieldMoveCodeNpc
{
public:

    // 回転状態定義
  struct RotationState
  {
    enum Value
    {
      INIT = 0,         // 初期化
      MAIN,             // 回転中
      END
    };
  };

  // 保存情報(回転用)
  struct RotationWork
  {
    f32                    startRotationY;
    f32                    targetRotationY;
    u32                    frame;
    u32                    frameMax;
    b32                    isReturn;
  };
	enum IconMode
	{
		ICON_MODE_NORMAL = 0,
		ICON_MODE_FAST,
		ICON_MODE_VERY_FAST
	};

  union StateWork
  {
    struct YOROKOBI
    {
      s8  type;
			b8  is_icon;
			b8  is_end_icon;
      u32 count;
    } yorokobi;

	  struct ONE_ROTATION
	  {
		  s8  type;
		  u8  isLeft:1;
			u8  isPiyori:1;
			u8  state;
		  u16 frame;
		  u16 frameMax;
		  f32 startRotationY;
	  } one_rotation;

		struct ICON
		{

			s32 icon_type;
			u16 frame;
			s32 iconMode;
		} icon;
  };

  // 保存情報
  struct Work
  {
		u32 option;             // 挙動指定オプション(現在)
		u8  state;
		u8  nextState;
		u32 count;
		u16 rotEnterWaitCount;  // 回転範囲に入った後の回転実行待ちカウンタ、指定フレーム加算で実行
		u16 dirResetWaitCount;  // 向きリセットするまでの待ち時間カウンタ、0で実行
		b8  isDirResetWait;     // 向きリセット待ち中か
		u8  dummy;
		u8  rotationCount;   // 回転実行回数記憶

		b8  isTalkEnd;            // 会話済みか？  再生成でリセットされる
		u16 frontInterestCount;   // 前方で注目している期間のカウント
		f32 returnRotationDegY;   // 方向戻し機能で戻す方向

    // 回転用情報
    RotationWork  rotationWork;
    
    
    u32 defaultOption;      // 挙動指定オプション(初期状態)
    u8  dirDirtyFlag:1;     // 方向が変わっているか？
    u8  hideInterestFlag:1;       // 背後の注目が発生しているか？ (トリガー判定に仕様)
    u8  hideInterestFirstDir:1;   // 背後の注目が発生した際の最初の向き( 0:右 1:左 )
    u8  padding1:5;
    u8  padding2;
    u16 padding3;
    s32 prevMotionId;

    // 以下、ふれあい
    u8  subState;
    u16 subStateCount;

    // 遅延追従用処理
    u16 moveKeepCount;
    u16 moveWaitCount;

    // ランダム待機
    u16 randActionWaitCount;
    u8  randActionCount;

    // 走り追従アイコン制御
    u16 runIconWaitCount;
    u8  runIconShowCount;
		
    u8  returnWalkWaitCount;                // ずっと0なので実質動作していない
    
    // 遅延追従用処理
    u8  isMoveStart:1;
    
    // 初回完了系制御
    u8  isIconEndFlag:1;
    u8  isFirstMoveEndFlag:1;
    
    u8  isComEventEndFlag:1;                // 初期化されてからコミュニケーションイベントを一度でも行ったか？ 未使用
    u8  isComEventEndFlagDiscoverAfter:1;   // 発見されてからコミュニケーションイベントを一度でも行ったか？  参照のみしている、必ず0
    
    // ムーランド反応
    u8  isMuurandAciton:1;

    // バイバイ状態制御
    u8  isByeByeEnd:1;
    u8  isByeByeClear:1;


		u8  isFirstRun:1;     // 未使用
		u8  isRunUreshii:1;   // 未使用
		u8	playerActionReq;
    u8  playerInterestReqCount;


    // 連続回転
#if 1
    u8  renzokuRotCount;
    u8  renzokuRotWait;
    u8  renzokuRotIsLeft;
#endif
    StateWork stateWork;
  };

  // 状態定義
  struct State
  {
    enum Value
    {
      READY = 0,        // 初回更新
      IDLE,             // 通常待機
      ROTATION,         // 回転中
      ONE_ROTATION,     // 一回転回転中

      // fureai
      URESHII,          // 嬉しいアクション
      SLEEP,
			ICON
    };
  };

  struct SubState
  {
    enum Value
    {
      LOST = 0,
      DISCOVER_ICON_WAIT,
      DISCOVER,
      LOST_ICON_WAIT,
      END,
      KININARU,
      DISCOVER_STOP,
      MAX,
    };
  };

  // 挙動設定
  struct SETTING
  {
    struct WALK
    {
      f32 discoverDistance;               // 開始距離[cm]
      f32 lostDistance;                   // 停止距離[cm]
      f32 stopCount;                      // 停止距離[cm]にこの時間いたら停止
      f32 runToWalkDistance;              // 走り→歩き開始距離[cm]
      f32 walkToRunDistance;              // 歩き→走り開始距離[cm]
      f32 endResetDistance;               // 停止終了距離[cm]
      b32 runEnable;                      // 走り有効化フラグ
    };

    struct CAME_NEAR
    {
      f32 baseDistance;                   // 基準距離 
      f32 baseBuffetDistance;             // 基準距離(+バッファ) 
      f32 baseMagnetDistance;             // 吸着を発動 
      f32 baseWalkDistance;               // 歩くを発動 
      f32 baseDistanceForModorou;         // 基準距離(もどろう)
    };

    struct SOUND
    {
      f32 min_volume;
      f32 max_volume;
      f32 min_length;
      f32 max_length;
      s32 ease_func;
    };

    f32 degreeRangeForInterest;           // 注目発動範囲[角度]
    f32 degreeRangeForRotation;           // 回転発動範囲[角度]

    f32 distanceRangeForInterest;         // 注目発動範囲[距離]
    f32 distanceRangeForInterestWide;     // 注目発動範囲(ワイド版)[距離]
    f32 distanceRangeForInterestShort;    // 注目発動範囲(縮小版)[距離]
    f32 distanceRangeForRotation;         // 回転発動範囲[距離]

    // 内側の判定を別に持つ
    f32 inSideDegreeRangeForInterest;     // 注目発動範囲[角度][内側]
    f32 inSideDegreeRangeForRotation;     // 回転発動範囲[角度][内側]
    f32 inSideDistanceRangeForInterest;   // 注目発動範囲[距離][内側]
    f32 inSideDistanceRangeForRotation;   // 回転発動範囲[距離][内側]

    // 最近接の判定を別に持つ
    f32 nearestDegreeRangeForInterest;    // 注目発動範囲[角度][最近接]
    f32 nearestDegreeRangeForRotation;    // 回転発動範囲[角度][最近接]
    f32 nearestDistanceRangeForInterest;  // 注目発動範囲[距離][最近接]
    f32 nearestDistanceRangeForRotation;  // 回転発動範囲[距離][最近接]

    f32 talkEndResetDistanceRange;        // 会話終了をリセットする距離
    f32 dirResetDistanceRange;            // 向きをリセットする距離
    u32 dirResetWaitBase;                 // 向きをリセットする際の待ちフレーム(最低保障)
    u32 dirResetWaitRand;                 // 向きをリセットする際の待ちフレーム(最低保障に足しこむ乱数値)
    
    f32 talkEndResetDistanceRangeShort;   // 会話終了をリセットする距離(縮小版)
    f32 dirResetDistanceRangeShort;       // 向きをリセットする距離(縮小版)

    f32 degreeDiffForRotationChange;      // 回転を実行する角度の差分
    u32 enterWaitForInterestToRotation;   // 回転発動待ち時間、このフレーム間注目範囲にいたら回転を実行する


    u32 disableInterestFrame;             // 注目解除にかけるフレーム
    u32 enterWaitForReturn;               // 元の状態に戻す発動待ち時間、このフレーム間何も反応がなければ実行する


    u32 rotationFrame;                    // 回転にかけるフレーム数
    b32 isCullingReset;                   // true:カリング時に再度有効化される
    f32 hideRangeDeg;                     // 背後扱いの角度(片翼)
    u32 moveWaitCount;
    u32 moveKeepCount;
    f32 rotRange;
    f32 basePositionBuffetDistance;       // 基本位置に戻っているとみなすバッファ 
    f32 sleepIconDispDistance;            // ZZZアイコンを表示する距離

    WALK  walk;                           // 歩き関連設定
    CAME_NEAR cameNear[3];                // 近寄り関連設定
    SOUND sound;                          // サウンド関連設定
  };

#if PM_DEBUG
	// デバッグ用挙動設定
	struct DEBUG_SETTING
	{
		b32 isMoveAreaCollison;

		b32 isNadeNadeTest;
		f32 nadenade_distance;
		f32 nadenade_angle;
		f32 nadenade_distance_x;
		
		b32 isBaseStanceTest;
		b32 base_stance;
	};
#endif

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeFureaiTypeA(void)
  {

  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeFureaiTypeA()
  {

  }

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const;

  /**
  *  @brief  近づいた後の座標を取得
  *
  *  @param  rNowPositon 現在座標
  *  @param  pOut        座標
  */
  static b32 GetComeNearPosition( FieldMoveCodeWork* pMoveCodeWork, gfl2::math::Vector3* pOut, b32 isRunEnable );
  static b32 SetEndStatus( FieldMoveCodeWork* pMoveCodeWork, s32 endType );
  static b32 SetSleepStart( FieldMoveCodeWork* pMoveCodeWork );
  static b32 SetSleepEndStatus( FieldMoveCodeWork* pMoveCodeWork );
  static b32 EventPlayerActionReq( FieldMoveCodeWork* pMoveCodeWork, s32 type );

  static b32 IsPlayerInterest( FieldMoveCodeWork* pMoveCodeWork );
  static b32 IsMuurandHeadNear( FieldMoveCodeWork* pMoveCodeWork );
  static b32 IsPlayerUreshiiAction( FieldMoveCodeWork* pMoveCodeWork );
  static void GetInterestPositionForPokemon( FieldMoveCodeWork* pMoveCodeWork, FieldMoveModelPlayer* pMoveModel, gfl2::math::Vector3* pOut);
  static b32 IsSleep(FieldMoveCodeWork* pMoveCodeWork );
  static b32 CanNadeNadeDistance(FieldMoveCodeWork* pMoveCodeWork);
	static f32 GetBaseDistance(FieldMoveCodeWork* pMoveCodeWork);
	static f32 GetBaseDistanceForModorou(FieldMoveCodeWork* pMoveCodeWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
  static SETTING* GetSettingData( void ) { return &m_Setting; }
  
	//----------------------------------------------------------------------------
  /**
  *  @brief  設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
	static SETTING::CAME_NEAR* GetCameNearSettingData(FieldMoveCodeWork* pMoveCodeWork);

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief  デバッグ設定データへのポインタを取得
  *  @note   デバッグメニューで操作するため、それ以外では使用禁止
  */
  //-----------------------------------------------------------------------------
  static DEBUG_SETTING* GetDebugSettingData( void ) { return &m_DebugSetting; }
#endif


protected:

  //----------------------------------------------------------------------------
  /**
  *  @brief  初回更新処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubReadyProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  待機中処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubIdleProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  回転中処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  嬉しい処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubUreshiiProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  寝ている処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubSleepProcess(FieldMoveCodeWork* pMoveCodeWork) const;
	static b32 SubSleepIconVisibleProcess(FieldMoveCodeWork* pMoveCodeWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  一回転処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubOneRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const;

	//----------------------------------------------------------------------------
	/**
	*  @brief  アイコン処理　
	*
	*  @param  pMoveCodeWork  動作コードワーク
	*
	*  @retval true 今フレームにもう一度処理を実行する
	*/
	//-----------------------------------------------------------------------------
	virtual b32 SubIconProcess(FieldMoveCodeWork* pMoveCodeWork) const;

	//----------------------------------------------------------------------------
	/**
	*  @brief  プレイヤー反応開始処理　
	*
	*  @param  pMoveCodeWork        動作コードワーク
	*/
	//-----------------------------------------------------------------------------
	b32 CheckAndStartPlayerReaction(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  嬉しい開始処理　
  *
  *  @param  pMoveCodeWork        動作コードワーク
  */
  //-----------------------------------------------------------------------------
	static void StartUreshii(FieldMoveCodeWork* pMoveCodeWork, s32 reqType = PokemonFureai::FPF_YOROKOBI_REQ_RANK_A, b32 isIcon = true, b32 isRandReset = true );

  //----------------------------------------------------------------------------
  /**
  *  @brief  回転開始処理　
  *
  *  @param  pMoveCodeWork        動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void StartRotation(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY, u32 frame, b32 isReturn = false, b32 isCompressMotion = true, s32 nextState = State::IDLE ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  一回転開始処理　
  *
  *  @param  pMoveCodeWork        動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void StartOneRotation(FieldMoveCodeWork* pMoveCodeWork, b32 isLeft, b32 isPiyori = false ) const;

	//----------------------------------------------------------------------------
	/**
	*  @brief  アイコン開始処理　
	*
	*  @param  pMoveCodeWork        動作コードワーク
	*/
	//-----------------------------------------------------------------------------
	static void StartIcon(FieldMoveCodeWork* pMoveCodeWork, s32 iconType, s32 iconMode = ICON_MODE_NORMAL, b32 isRandReset = true);

  //----------------------------------------------------------------------------
  /**
  *  @brief  寝ている開始処理　
  *
  *  @param  pMoveCodeWork        動作コードワーク
  */
  //-----------------------------------------------------------------------------
	static void StartSleep(FieldMoveCodeWork* pMoveCodeWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  歩き/走り中処理　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *
  *  @retval true 今フレームにもう一度処理を実行する
  */
  //-----------------------------------------------------------------------------
  virtual b32 SubWalkToPlayerProcess(FieldMoveCodeWork* pMoveCodeWork) const;
	virtual b32 SubWalkToTargetProcess(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget, b32 isRunEnable = false ) const;
	virtual b32 SubWalkCoreProcess(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget, b32 isRunEnable = true ) const;
  static b32 SubWalkStopProcess(FieldMoveCodeWork* pMoveCodeWork);


  static b32 GetMoveTargetPosition(FieldMoveCodeWork* pMoveCodeWork, gfl2::math::Vector3* pOut );
  static b32 HitCheckReactionRangeToPlayer(FieldMoveCodeWork* pMoveCodeWork, f32 range );
  static f32 GetDirDiffToPlayer(FieldMoveCodeWork* pMoveCodeWork);
  static f32 GetDistanceToPlayer(FieldMoveCodeWork* pMoveCodeWork);
  static f32 GetDirToPlayer(FieldMoveCodeWork* pMoveCodeWork);
  static f32 GetDirDiffToTarget(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget );
  static f32 GetDirToTarget(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget );
  static void PositionClampMoveArea(FieldMoveCodeWork* pMoveCodeWork);
  static void PositionClampMoveAreaForComeNear(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rStart, const gfl2::math::Vector3& rEnd, gfl2::math::Vector3* pOut);
	static void PlayerPush(FieldMoveCodeWork* pMoveCodeWork);

  // 各種カウンター制御
	static void CountResetMove(FieldMoveCodeWork* pMoveCodeWork);
	static void CountResetRndIdle(FieldMoveCodeWork* pMoveCodeWork, b32 resetActionCount = true );


  static SETTING m_Setting;             // 挙動設定、デバッグメニューで操作します

#if PM_DEBUG
  static DEBUG_SETTING m_DebugSetting;  // デバッグ挙動設定、デバッグメニューで操作します
#endif

}; // class FieldMoveCodeNpcLookShort


}; //end of namespace mmodel
}; //end of namespace fld
