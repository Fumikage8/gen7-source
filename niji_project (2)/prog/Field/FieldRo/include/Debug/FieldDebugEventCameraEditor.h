//======================================================================
/**
 * @file FieldDebugEventCameraEditor.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：EventCameraEditor
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUG_DEBUG_EVENT_CAMERA_EDITOR_H_INCLUDED__
#define __FIELDDEBUG_DEBUG_EVENT_CAMERA_EDITOR_H_INCLUDED__
#pragma once

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldRo/include/Event/FieldEventDebugEventCameraEditor.h"

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
}

namespace Field {
  class Fieldmap;
  namespace FieldScript {
    class FieldScriptSystem;
  }
}

namespace GameSys {
  class GameManager;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugEventCameraEditor
 * @brief クラスの説明
 */
class DebugEventCameraEditor
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugEventCameraEditor);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugEventCameraEditor( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugEventCameraEditor( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase*             pHeap,
    gfl2::str::MsgData*               pMsgData,
    Fieldmap*                         pFieldmap,
    FieldScript::FieldScriptSystem*   pScriptSystem,
    GameSys::GameManager*             pGameManager
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  セットアップ
   */
  void Setup( void );

  /**
   *  @brief  クリーン
   */
  void Clean( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  /**
  *  @brief  メニューの破棄
  */
  void DeleteDebugMenu( void );

  //! ヒープの取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  //! Fieldmapの取得
  Fieldmap* GetFieldmap() const { return m_pFieldmap; };

  //! DebugWinGroupのセット・取得
  void SetDebugWinGroup( gfl2::debug::DebugWinGroup* pGroup, u32 index) { m_pDebugGroup[index] = pGroup; }
  gfl2::debug::DebugWinGroup* GetDebugWinGroup( u32 index) const { return m_pDebugGroup[index]; }

  //! スクリプトシステムの取得
  FieldScript::FieldScriptSystem* GetFieldScriptSystem() const { return m_pScriptSystem; }

  //! モード管理インスタンスの取得
  DebugEventCameraEditorModeManager* GetModeManager() const { return m_pModeManager; }

  //! GameManagerの取得
  GameSys::GameManager* GetGameManager() const { return m_pGameManager; }

public:

  gfl2::heap::HeapBase*                 m_pHeap;
  Fieldmap*                             m_pFieldmap;
  gfl2::str::MsgData*                   m_pMsgData;
  FieldScript::FieldScriptSystem*       m_pScriptSystem;
  GameSys::GameManager*                 m_pGameManager;

  DebugEventCameraEditorModeManager*    m_pModeManager;

  enum DebugGroupIndex
  {
    DEBUG_GROUP_INDEX_ROOT,
    DEBUG_GROUP_INDEX_EDIT,
    DEBUG_GROUP_INDEX_FRAMING,

    DEBUG_GROUP_MAX,
  };
  gfl2::debug::DebugWinGroup*           m_pDebugGroup[ DEBUG_GROUP_MAX ];


};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)


#endif // GF_ENABLE_DEBUG_EDITOR
#endif // __FIELDDEBUG_DEBUG_EVENT_CAMERA_EDITOR_H_INCLUDED__

#endif // PM_DEBUG

