//======================================================================
/**
* @file FieldEventDebugCharaLayoutEditor.cpp
* @brief スクリプト班向けデバッグ機能イベント : キャラレイアウトエディター
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Event/FieldEventDebugCharaLayoutEditor.h"
#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>

// field script
#include "FieldScript/include/FieldScriptSystem.h"

// conv_header
#include "niji_conv_header/message/debug/msg_debug_menu.h"


GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief コンストラクタ
 * @param pHeap GameEventに渡すヒープ
 */ 
EventDebugCharaLayoutEditor::EventDebugCharaLayoutEditor( gfl2::heap::HeapBase* pHeap)
  : GameSys::GameEvent( pHeap)
  , m_pHeap( NULL)
  , m_pMsgData( NULL)
  , m_pDebugGroup( NULL)
  , m_pFieldmap( NULL)
  , m_pScriptSystem( NULL)
  , m_pModeManager( NULL)
  , m_isBoot( false)
{
}

/**
 * @brief デストラクタ
 */ 
EventDebugCharaLayoutEditor::~EventDebugCharaLayoutEditor()
{
}

/**
 * @brief 初期化処理(インスタンス生成直後に実行する)
 * @param desc 初期化設定
 */ 
void EventDebugCharaLayoutEditor::Initialize( const InitializeDescription& desc)
{
  m_pHeap           = desc.pHeap;
  m_pMsgData        = desc.pMsgData;
  m_pDebugGroup     = desc.pDebugGroup;
  m_pFieldmap       = desc.pFieldmap;
  m_pScriptSystem   = desc.pScriptSystem;
  m_pModeManager    = desc.pModeManager;

  m_isBoot = true;
}

/**
 * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 * @retval true 起動してよい
 * @retval false 起動しない
 */
bool EventDebugCharaLayoutEditor::BootChk(GameSys::GameManager* gmgr)
{
  return m_isBoot;
}

/**
 * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 */
void EventDebugCharaLayoutEditor::InitFunc(GameSys::GameManager* gmgr)
{
  // 動作モデルの動きを停止
  m_pFieldmap->GetMoveModelManager()->AddEventRequestAll();
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 * @return GameSys::GMEVENT_RESULT イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventDebugCharaLayoutEditor::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    UPDATE,
    END,
  };

  switch( this->GetSeqNo())
  {
  case UPDATE:
    {
      DebugCharaLayoutEditorModeManager::State state;
      state = m_pModeManager->Update();

      if( state == DebugCharaLayoutEditorModeManager::STATE_END)
      {
        this->SetSeqNo( END);
      }
    }
    break;

  case END:
    return GameSys::GMEVENT_RES_FINISH;
    //break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 * @brief 呼び出される関数 純粋仮想関数
 * @param gmgr ゲームマネージャークラス
 */
void EventDebugCharaLayoutEditor::EndFunc(GameSys::GameManager* gmgr)
{
  // 動作モデルの動きを再開
  m_pFieldmap->GetMoveModelManager()->PullEventRequestAll();
}

GFL_NAMESPACE_END( Field )

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG
