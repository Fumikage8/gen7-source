/**
* @brief  デバッグ用カメラ位置調整イベント
* @file   FieldEventDebugCameraAdjustPosition.h
* @author ikuta_junya
* @data   2015.05.23
*/

#if !defined( __FIELDEVENTDEBUGCAMERAADJUSTPOSITION_H_INCLUDED__ )
#define __FIELDEVENTDEBUGCAMERAADJUSTPOSITION_H_INCLUDED__
#pragma once


// niji
#include "GameSys/include/GameEvent.h"


//
// 前方宣言
//

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );

class FieldMoveModel;

GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
class Fieldmap;
GFL_NAMESPACE_END( Field );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

class ControllerTypeAdjustPosition;

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field )


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );

class EventDebugCameraAdjustPosition : public GameSys::GameEvent
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugCameraAdjustPosition );

public:

    /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase* m_pHeap;
    u32 dummy;
  };


  /**
  * @brief コンストラクタ
  *
  * @param  pSystemHeap GameEventに渡すヒープ
  *
  * @return 無し
  */ 
  EventDebugCameraAdjustPosition( gfl2::heap::HeapBase* pSystemHeap );
  
  /**
  * @brief デストラクタ
  *
  * @return 無し
  */ 
  virtual ~EventDebugCameraAdjustPosition();

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

private:
 
  // 
  // 内部制御
  // 
  u32 m_PrevUnitNo;
  b32 m_IsDebugDraw;

  //
  // 内部オブジェクト
  //

  //
  // 内部システム
  // 

  //
  // 外部オブジェクト
  //
  MoveModel::FieldMoveModel*                               m_pPlayerCharacter;
  Field::Camera::Controller::ControllerTypeAdjustPosition* m_pCameraController;  // カメラ操作、作るのはこのイベントだが、寿命管理をカメラシステムに任せるので外部扱い

  //
  // 外部システム
  //
  Fieldmap*             m_pFieldMap;
  gfl2::heap::HeapBase* m_pSystemHeap;
  gfl2::str::MsgData*   m_pMsgData;

}; // class EventDebugCameraAdjustPosition

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif // __FIELDEVENTDEBUGCAMERAADJUSTPOSITION_H_INCLUDED__