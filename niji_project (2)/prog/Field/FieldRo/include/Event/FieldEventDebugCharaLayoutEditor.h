﻿//======================================================================
/**
* @file FieldEventDebugCharaLayoutEditor.h
* @brief スクリプト班向けデバッグ機能イベント : キャラレイアウトエディター
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#if !defined( __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__)
#define __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__
#pragma once

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gamesys
#include "GameSys/include/GameEvent.h"

// field
#include "Field/FieldRo/include/Event/FieldEventDebugCharaLayoutEditorMode.h"

// 前方参照
namespace Field {
  class Fieldmap;
  namespace MoveModel {
    FieldMoveModel;
  }
  namespace FieldScript {
    class FieldScriptSystem;
  }
}

GFL_NAMESPACE_BEGIN( Field )

/**
 * @class EventDebugCharaLayoutEditor
 * @brief スクリプト班向けデバッグ機能イベント : キャラレイアウトエディター
 */
class EventDebugCharaLayoutEditor : public GameSys::GameEvent
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugCharaLayoutEditor );

public:

  /**
   * @brief 初期化設定
   */
  struct InitializeDescription
  {
    gfl2::heap::HeapBase*               pHeap;
    gfl2::str::MsgData*                 pMsgData;
    gfl2::debug::DebugWinGroup*         pDebugGroup;
    Fieldmap*                           pFieldmap;
    FieldScript::FieldScriptSystem*     pScriptSystem;
    DebugCharaLayoutEditorModeManager*  pModeManager;
  };

public:

  /**
   * @brief コンストラクタ
   * @param pHeap GameEventに渡すヒープ
   */ 
  EventDebugCharaLayoutEditor( gfl2::heap::HeapBase* pHeap);
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~EventDebugCharaLayoutEditor();

  /**
   * @brief 初期化処理(インスタンス生成直後に実行する)
   * @param desc 初期化設定
   */ 
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   * @retval true 起動してよい
   * @retval false 起動しない
   */
  virtual bool BootChk(GameSys::GameManager* gmgr);

  /**
   * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   */
  virtual void InitFunc(GameSys::GameManager* gmgr);

  /**
   * @brief 呼び出される関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   * @return GameSys::GMEVENT_RESULT イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  /**
   * @brief 呼び出される関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   */
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:

  gfl2::heap::HeapBase*                 m_pHeap;
  Fieldmap*                             m_pFieldmap;
  gfl2::str::MsgData*                   m_pMsgData;
  gfl2::debug::DebugWinGroup*           m_pDebugGroup;
  FieldScript::FieldScriptSystem*       m_pScriptSystem;
  DebugCharaLayoutEditorModeManager*    m_pModeManager;
  bool                                  m_isBoot;

}; // class EventDebugCharaLayoutEditor

GFL_NAMESPACE_END( Field )

#endif // __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG
