/**
* @brief  攻略本用、マップ撮影イベント
* @fix CONNMCat[582]:攻略本用マップ撮影機能、
* @file   FieldDebugEventMapCapture.h
* @author ikuta_junya
* @data   2016.06.10
*/


#if PM_DEBUG

#if !defined( __FIELDDEBUGEVENTMAPCAPTURE_H_INCLUDED__ )
#define __FIELDDEBUGEVENTMAPCAPTURE_H_INCLUDED__
#pragma once


// niji
#include "GameSys/include/GameEvent.h"
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


//
// 前方宣言
//

GFL_NAMESPACE_BEGIN( Field );
class Fieldmap;
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
GFL_NAMESPACE_BEGIN( Event );

class ControllerTypeMapCapture;

class EventMapCapture : public GameSys::GameEvent
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EventMapCapture );

public:

  struct SETTING
  {
    b32 is_event_call;                      // trueならイベントコール済み
    b32 is_end_req;                         // trueならイベント終了
    b32 is_ortho;                           // trueなら正射影描画
    f32 step_move_value;                    // 固定移動量
    f32 step_move_value_for_control;        // 固定移動量
    f32 ortho_draw_range;                   // 正投影幅(縦)
    f32 animation_stop_frame;               // アニメーション停止させた際のフレーム
    gfl2::math::Vector3 camera_position;    // カメラ座標
    gfl2::math::Vector3 camera_rot_deg;     // カメラ回転
    b32 is_draw_control_positon;            // 地形制御座標
    gfl2::math::Vector3 control_position;   // 地形制御座標
  };

    /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    u32 dummy;
  };


  /**
  * @brief コンストラクタ
  *
  * @param  pSystemHeap GameEventに渡すヒープ
  *
  * @return 無し
  */ 
  EventMapCapture( gfl2::heap::HeapBase* pSystemHeap );
  
  /**
  * @brief デストラクタ
  *
  * @return 無し
  */ 
  virtual ~EventMapCapture();

  /**
  * @brief 初期化処理(インスタンス生成直後に実行する)
  *
  * @param  rDesc   初期化設定
  *
  * @return 無し
  */ 
  void Initialize( const InitializeDescription& rDesc );

  //--------------------------------------------------------------
  /**
  * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  * @retval  true  起動してよい
  * @retval  false 起動しない
  */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
  * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
  */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  /**
  * @brief   設定変数のポインタを取得する(デバッグメニュー用)
  *
  * @return  設定変数のポインタ
  */
  static SETTING* GetSettingPtr( void ) { return &s_Setting; }


private:

  /**
  * @brief   設定を各モジュールに反映
  *
  * @return  無し
  */
  void ApplySetting( void );
 
  /**
  * @brief   レイキャストの結果の位置に制御座標を移動する
  *
  * @return  無し
  */
  void SetCotrolPointByRaycast( void );

  // 
  // 内部制御
  // 
  u32 m_PrevUnitNo;
  static SETTING  s_Setting;        // 各種設定。デバッグでメニューで操作した結果を覚えておくため、static


  //
  // 内部オブジェクト
  //

  //
  // 内部システム
  // 
  ControllerTypeMapCapture* m_pCameraController;

  //
  // 外部オブジェクト
  //

  //
  // 外部システム
  //
  Fieldmap*             m_pFieldMap;
  gfl2::heap::HeapBase* m_pSystemHeap;

}; // class EventMapCapture

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif // __FIELDDEBUGEVENTMAPCAPTURE_H_INCLUDED__
#endif // PM_DEBUG