//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_TouchPanelOperator.h
 *	@brief  タッチパネル操作クラス
 *	@author	Toru=Nagihashi
 *	@date		2012.06.29
 *	
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_TOOL_TOUCHPANELOPERATOR_H__)
#define __APP_TOOL_TOUCHPANELOPERATOR_H__

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <ui/include/gfl2_UI.h>
#include <Prog/GameSys/include/GameFrameMode.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

/**
 * @brief タッチパネル操作クラス
 * @note  タッチパネル操作クラスはドラッグ・フリック・決定（各仕様は以下）をサポートします。
 *	      …本当はgfl_UI_TouchPanel.hにあった方がよいのかも。
 *	      
 *
 * @note  現在の仕様
 * 
 *        ・ドラッグ
 *            タッチトリガからドラッグモード
 *            タッチリリースでドラッグモード終了
 *        
 *        ・フリック
 *            タッチリリースから指定フレーム前のタッチ位置の距離で判定
 *
 *        ・決定
 *            タッチリリース時タッチトリガから３ドット以内であれば決定となる
 *            これはドラッグ操作時に決定とみなさないためです。
 *
 */
class TouchPanelOperator
{
  GFL_FORBID_COPY_AND_ASSIGN(TouchPanelOperator);
public:
  //-----------------------------------------------------------------------
  // 定数
  //-----------------------------------------------------------------------
  static const f32 FLICK_POWER_MAX;
  static const f32 FLICK_POWER_MIN;
public:
  //-----------------------------------------------------------------------
  // 生成
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param touch_panel タッチパネル
   * @param framerate フレームレート
   */
  TouchPanelOperator( const gfl2::ui::TouchPanel *touch_panel, GameSys::FrameMode framerate );

  /**
   * @brief デストラクタ
   */
  virtual ~TouchPanelOperator();
  //-----------------------------------------------------------------------
  // システム
  //-----------------------------------------------------------------------
  /**
   * @brief 更新  毎フレーム回してください
   * 
   * @note  override可  状態遷移を変えたい時などを想定
   */
  virtual void Update(void);
  //-----------------------------------------------------------------------
  // 取得
  //-----------------------------------------------------------------------
  /**
   * @brief ドラッグ開始かを取得
   *
   * @return trueドラッグ開始中 falseドラッグ開始していない
   */
  bool IsDragStart(void) const;
  /**
   * @brief ドラッグ中かを取得(IsDragStart==trueも含みます)
   *
   * @return trueドラッグ中 falseドラッグしていない
   */
  bool IsDrag(void) const;
  /**
   * @brief ドラッグ開始座標を取得
   *
   * @param start_value ドラッグ開始座標受け取り
   *
   * @return trueドラッグ中で取得OK falseドラッグ中ではないため取得できなかった
   */
  bool GetDragStartValue( gfl2::math::VEC2 *start_value ) const;
  /**
   * @brief ドラッグ現在座標を取得
   *
   * @param now_value ドラッグ現在座標受け取り
   *
   * @return trueドラッグ中で取得OK falseドラッグ中ではないため取得できなかった
   */
  bool GetDragNowValue( gfl2::math::VEC2 *now_value ) const;
  /**
   * @brief フリックをしたかを取得
   *
   * @return trueフリックした falseフリックしていない
   * 
   * @note  タイミング的にはIsTouchRelease == true時の１フレームのみ返ってきます。
   *        またかならずIsDrag() != IsFlick()が保証されています。
   */
  bool IsFlick(void) const;
  /**
   * @brief フリック現在座標を取得
   *
   * @param end_value フリック現在座標受け取り
   *
   * @return trueフリック中で取得OK falseフリック中ではないため取得できなかった
   */
  bool GetFlickEndValue( gfl2::math::VEC2 *end_value ) const;
  /**
   * @brief フリックの強さを取得
   *
   * @return フリックの強さ(0~1.0f)
   */
  f32 GetFlickPower( void ) const;
  /**
   * @brief フリックの強さベクトルを取得
   *
   * @param vectorベクトル（now_value-start_value を正規化してFlickPowerをかけたものです）
   */
  void GetFlickPowerVector( gfl2::math::VEC2 *vector ) const;
  /**
   * @brief 決定を取得
   *
   * @return true決定 fale決定してない
   */
  bool IsDecide(void) const;
  /**
   * @brief 決定したときの値を取得
   *
   * @param decide_value 決定したときの値
   * @return true決定中で取得OK false決定中ではないため取得できなかった
   */
  bool GetDecideValue( gfl2::math::VEC2 *decide_value );
  /**
   * @brief タッチパネルを取得
   *
   * @return タッチパネル
   */
  const gfl2::ui::TouchPanel *GetTouchPanel(void) const;
protected:
  enum State
  {
    STATE_NONE,     ///<何もしていない状態
    STATE_DRAG_START,     ///<ドラッグ中
    STATE_DRAG_MAIN,     ///<ドラッグ中
    STATE_DECIDE,   ///<決定      （1フレームのみ返ってくる）
    STATE_FLICK,    ///<フリック  （1フレームのみ返ってくる）
  };
  static const int DRAG_LOG_NUM = 10;
protected:
  /**
   * @brief フリック検知
   *
   * @return trueでフリック検知 falseで検知しなかった
   * 
   * @note override可 フリック検知の仕組みを変えたい時を想定
   *      タッチリリース時にstartとendから検知している
   */
  virtual bool CheckFlickEnable( void ) const;
  /**
   * @brief フリック強度の計算
   *
   * @return (0.0f〜1.0f)フリックの強度
   *
   * @note override可 フリック強度の仕組みを変えたい時を想定
   */      
  virtual f32 CalcFlickPower( void ) const;
  /**
   * @brief 決定の範囲をチェック
   * @param now現在の座標
   * @return trueで決定範囲内 falseで決定範囲外
   * 
   * @note override可 決定範囲の仕組みを変えたい時を想定
   */
  virtual bool CheckDecideRange( const gfl2::math::VEC2 & now ) const;

  const gfl2::math::VEC2 & GetDragLogStartVec( void ) const;
  const gfl2::math::VEC2 & GetDragLogEndVec( void ) const;
protected: 
  const gfl2::ui::TouchPanel *m_touch_panel; ///<タッチパネル
  GameSys::FrameMode m_framerate;  ///<ゲームフレームレート

  gfl2::math::VEC2 m_start_value;  ///<タッチトリガ時の座標
  gfl2::math::VEC2 m_end_value;  ///<タッチリリース時の座標
  State m_state;  ///<タッチ状態詳しくはenum State
  f32 m_flick_power;  ///<フリックの強さ 0.0f 〜 1.0f
  u32 m_drag_counter; ///<ドラッグ中のみカウントされる.FRAMDEMODE_30時は+2,60時は+1
  bool m_spaced_decide;  ///<決定から離れたフラグ

  gfl2::math::VEC2 m_drag_log_value[ DRAG_LOG_NUM ];  //リングバッファ
  u16 m_drag_log_start;
  u16 m_drag_log_end;
  bool m_is_drag_log_around;

private:
  static const f32 FLICK_DISTANCE_MIN;  ///<フリックするために最低限必要な長さ
  static const f32 FLICK_DISTANCE_MAX; ///<フリックする最大距離。
  static const f32 DECIDE_DISCANCE; ///<これ以上トリガとリリースが離れると決定とみなされなくなる距離
  
private:
  void UpdateDragLogs( u16 x, u16 y );
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif		// __APP_TOOL_TOUCHPANELOPERATOR_H__
