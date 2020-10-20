//======================================================================
/**
 * @file FieldAnimationSignal.h
 * @date 2015/07/27 17:36:18
 * @author miyachi_soichi
 * @brief アニメーション連動イベント発行
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_MOTION_SIGNAL_H_INCLUDED__
#define __FIELD_MOTION_SIGNAL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(Field)

GFL_NAMESPACE_BEGIN(MoveModel)
class FieldMoveModel;
GFL_NAMESPACE_END(MoveModel)

/**
 *  @brief  イベント最大数
 *    1キャラクタあたり16データまで
 */
static const u32 SIGNAL_EVENT_DATA = 16;
/**
 *  @brief  イベントの種別
 */
enum SignalEventType
{
  SIGNAL_TYPE_NONE,       //!< 無効データ
  SIGNAL_TYPE_SPD,        //!< 速度変更
  SIGNAL_TYPE_EYE_RATIO,  //!< 視線倍率

  SIGNAL_TYPE_MAX,        //!< 定義最大

  SIGNAL_TYPE_FREEZE,      //!< 停止：しばらく未公開化
};

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
/**
 *  @brief  イベントデータ
 */
struct SignalEventData
{
  u32 motion_id;      //!< ひもづくアニメーションID
  u32 signal_type;    //!< 起動するイベントの種類
  s32 value;          //!< イベントに必要なパラメータ
  f32 frame;          //!< いべんとが発行されるフレーム
}; // struct SignalEventData
struct SignalEventDataHeader
{
  u32 num;
  SignalEventData data[];
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

/**
  * @class MotionSignal
  * @brief モーション連動イベント発行器
  */
class MotionSignal
{
  GFL_FORBID_COPY_AND_ASSIGN(MotionSignal);

public:
  /**
   *  @brief  コンストラクタ
   */
  MotionSignal( void );

  /**
   *  @brief  デストラクタ
   */
  ~MotionSignal( void );

  /**
   *  @brief  初期化
   *  @param  pResource データ
   */
  void Initialize( const void *pResource );

  /**
   *  @brief  解放
   */
  void Terminate( void );

  /**
   *  @brief  更新
   *  @param  pParent   親
   */
  void Update( Field::MoveModel::FieldMoveModel *pParent );

  /**
   *  @brief  登録
   *  @param  idx     配列番号
   *  @param  animeID モーション番号
   *  @param  type    起動するシグナルの種別
   *  @param  value   シグナルに必要なパラメータ
   *  @param  frame   起動するフレーム
   */
  void RegistEvent( u32 idx, u32 animeID, u32 type, s32 value, f32 frame );

  /**
   *  @brief  削除
   *  @param  idx     配列番号
   */
  void UnRegistEvent( u32 idx );

  /**
   *  @brief  データ取得
   *  @param  idx     配列番号
   */
  const SignalEventData *GetData( u32 idx );

  /**
   *  @brief  停止設定
   *  こちらは外部からの設定
   */
  inline void SetPause( bool flag ){ m_bPause = flag; }

  /**
   *  @brief  停止判定
   */
  inline bool IsPause( void ){ return m_bPause; }

  /**
   *  @brief  停止判定
   */
  inline bool IsFreeze( void ){ return 0 < m_FreezeCnt; }

#if PM_DEBUG
  /**
   *  @brief  デバッグ用：クリア
   */
  void Debug_Clear( void );

  /**
   *  @brief  デバッグ用：再初期化
   */
  void Debug_ReInitialize( void *pResource );
#endif // PM_DEBUG

private:
  /**
   *  @brief  イベントの処理
   *  @param  pParent   対象者
   *  @param  type      行為
   *  @param  value     どのくらい
   */
  void raiseEvent( Field::MoveModel::FieldMoveModel *pParent, u32 type, s32 value );

  /**
   *  @brief  停止解除
   *  @param  pParent   対象者
   */
  void resetFreeze( Field::MoveModel::FieldMoveModel *pParent );

private:
  SignalEventData                   m_aData[ SIGNAL_EVENT_DATA ];

  s32                               m_FreezeCnt;  //!< 停止時間
  f32                               m_KeepStep;   //!< 保存用ステップフレーム

  bool                              m_bPause;     //!< 外部停止
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_MOTION_SIGNAL_H_INCLUDED__