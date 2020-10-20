//======================================================================
/**
 * @file FieldDebugCharaLayoutEditor.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：CharaLayoutEditor
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

#if !defined __FIELDDEBUG_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__
#define __FIELDDEBUG_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldRo/include/Event/FieldEventDebugCharaLayoutEditor.h"

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
 * @class DebugCharaLayoutEditor
 * @brief クラスの説明
 */
class DebugCharaLayoutEditor
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugCharaLayoutEditor);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugCharaLayoutEditor( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugCharaLayoutEditor( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase*             pHeap,
    gfl2::str::MsgData*               pMsgData,
    Fieldmap*                         pFieldmap,
    FieldScript::FieldScriptSystem*   pScriptSystem,
    GameSys::GameManager*             pGameManager,
    gfl2::fs::AsyncFileManager*       pAsyncFileReadManager
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
  DebugCharaLayoutEditorModeManager* GetModeManager() const { return m_pModeManager; }

  //! GameManagerの取得
  GameSys::GameManager* GetGameManager() const { return m_pGameManager; }

public:

  gfl2::heap::HeapBase*                 m_pHeap;
  Fieldmap*                             m_pFieldmap;
  gfl2::str::MsgData*                   m_pMsgData;
  FieldScript::FieldScriptSystem*       m_pScriptSystem;
  GameSys::GameManager*                 m_pGameManager;
  gfl2::fs::AsyncFileManager*           m_pAsyncFileReadManager;

  DebugCharaLayoutEditorModeManager*    m_pModeManager;

  enum DebugGroupIndex
  {
    DEBUG_GROUP_INDEX_ROOT,
    DEBUG_GROUP_INDEX_DISP_MODEL_LIST,
    DEBUG_GROUP_INDEX_EDIT_PLACEMENT,
    DEBUG_GROUP_INDEX_PLAY_MOTION,
    DEBUG_GROUP_INDEX_PLAY_DYNAMIC_MOTION,

    DEBUG_GROUP_MAX,
  };
  gfl2::debug::DebugWinGroup*           m_pDebugGroup[ DEBUG_GROUP_MAX ];


  gfl2::str::MsgData*                   m_pMsgDataMotion;
  gfl2::str::MsgData*                   m_pMsgDataDynamicMotion;
  void*                                 m_pDynamicMotionMsgIDData;


private:

  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize);

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // __FIELDDEBUG_DEBUG_CHARA_LAYOUT_EDITOR_H_INCLUDED__
#endif // PM_DEBUG

