#if PM_DEBUG
/**
* @brief  デバッグ用地形ブロックチェックイベント
* @file   FieldEventDebugTerrainBlockControl.h
* @author ikuta_junya
* @data   2015.05.23
*/

#if !defined( __FIELDEVENTDEBUGTERRAINCONTROL_H_INCLUDED__ )
#define __FIELDEVENTDEBUGTERRAINCONTROL_H_INCLUDED__
#pragma once

#define ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL ( 0 )

#if ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL

// niji
#include "GameSys/include/GameEvent.h"


//
// 前方宣言
//

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );

class FieldMoveModel;

GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
class Fieldmap;
GFL_NAMESPACE_END( Field );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Event );

class EventDebugTerrainBlockControl : public GameSys::GameEvent
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugTerrainBlockControl );

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
  EventDebugTerrainBlockControl( gfl2::heap::HeapBase* pSystemHeap );
  
  /**
  * @brief デストラクタ
  *
  * @return 無し
  */ 
  virtual ~EventDebugTerrainBlockControl();

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
 
  void SetBlockInvisibleAll( b32 flag );
  void ReqBlockLodAll( void );

  // 
  // 内部制御
  // 
  s32 m_X;
  s32 m_Z;
  s32 m_Lod;

  //
  // 内部オブジェクト
  //

  //
  // 内部システム
  // 

  //
  // 外部オブジェクト
  //
  gfl2::heap::HeapBase*       m_pHeap;
  gfl2::debug::DebugWinGroup* m_pBlockControlGrop;
  
  //
  // 外部システム
  //
  Fieldmap*             m_pFieldMap;
  gfl2::heap::HeapBase* m_pSystemHeap;

}; // class EventDebugTerrainBlockControl

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif // ENABLE_DEBUG_TERRAIN_BLOCK_CONTROL
#endif // __FIELDEVENTDEBUGTERRAINCONTROL_H_INCLUDED__
#endif