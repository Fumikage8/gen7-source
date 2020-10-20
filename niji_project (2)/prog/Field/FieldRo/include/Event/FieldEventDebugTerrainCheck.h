/**
* @brief  デバッグ用地形ブロックチェックイベント
* @file   FieldEventDebugTerrainCheck.h
* @author ikuta_junya
* @data   2015.05.23
*/

#if !defined( __FIELDEVENTDEBUGTERRAINCHECK_H_INCLUDED__ )
#define __FIELDEVENTDEBUGTERRAINCHECK_H_INCLUDED__
#pragma once

#if PM_DEBUG

// niji
#include "GameSys/include/GameEvent.h"
#include "Field/FieldStatic/include/Debug/DebugSymbolNameLoader.h"

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
GFL_NAMESPACE_BEGIN( Event );

class EventDebugTerrainCheck : public GameSys::GameEvent
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugTerrainCheck );

public:

    /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase* m_pHeap;
    Field::Debug::SymbolNameLoader* m_pZoneNameLoader;
    Field::Debug::SymbolNameLoader* m_pAreaNameLoader;    // m_isHeapInfoSaveがtrueのとき必要
    Field::Debug::SymbolNameLoader* m_pWorldNameLoader;   // m_isHeapInfoSaveがtrueのとき必要

    u32 m_AreaId;
    u32 m_WorldId;

    u32 dummy;
    b32 m_isHeapInfoSave;
  };


  /**
  * @brief コンストラクタ
  *
  * @param  pSystemHeap GameEventに渡すヒープ
  *
  * @return 無し
  */ 
  EventDebugTerrainCheck( gfl2::heap::HeapBase* pSystemHeap );
  
  /**
  * @brief デストラクタ
  *
  * @return 無し
  */ 
  virtual ~EventDebugTerrainCheck();

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
  u32 m_X;
  u32 m_Z;

  u32 m_WorldID;
  u32 m_AreaID;
  b32 m_IsHeapInfoSave;

  //
  // 内部オブジェクト
  //
  class HeapInfoSaver;
  HeapInfoSaver* m_pHeapInfoSaver;

  //
  // 内部システム
  // 

  //
  // 外部オブジェクト
  //
  Field::Debug::SymbolNameLoader* m_pZoneNameLoader;
  Field::Debug::SymbolNameLoader* m_pAreaNameLoader;
  Field::Debug::SymbolNameLoader* m_pWorldNameLoader;


  //
  // 外部システム
  //
  Fieldmap*             m_pFieldMap;
  gfl2::heap::HeapBase* m_pSystemHeap;


}; // class EventDebugTerrainCheck

GFL_NAMESPACE_END( Event );
GFL_NAMESPACE_END( Field );

#endif
#endif // __FIELDEVENTDEBUGTERRAINCHECK_H_INCLUDED__