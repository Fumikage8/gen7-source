//======================================================================
/**
 * @file FieldDebugMenuAttrDispEditor.h
 * @date 2015/09/16 13:16:27
 * @author ikuta_junya
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __FIELDDEBUG_FIELDDEBUGMENUATTREDITOR_H_INCLUDED__
#define __FIELDDEBUG_FIELDDEBUGMENUATTREDITOR_H_INCLUDED__
#pragma once

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// exec 
#include <niji_conv_header/field/attribute/attribute_id.h>

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
}

namespace Field {
  class Fieldmap;
}

namespace GameSys {
  class GameManager;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)
GFL_NAMESPACE_BEGIN(Menu)

/**
 * @class DebugMenuAttrDispEditor
 * @brief クラスの説明
 */
class DebugMenuAttrDispEditor
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMenuAttrDispEditor);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugMenuAttrDispEditor( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugMenuAttrDispEditor( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase*             pHeap,
    gfl2::str::MsgData*               pMsgData,
    gfl2::str::MsgData*               pMsgDataAttrName,
    Fieldmap*                         pFieldmap,
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

  /**
  * @brief   アトリビュートカラーを更新
  */
  static void UpdateColor( void );


  //
  // メニュー生成関数向け
  // 

  /**
  * @brief 今のエリアのアトリビュート情報をチェック済みか取得
  */
  b32 IsAttrCheck( void ) const { return m_IsAttrCheck; }

  /**
  * @brief 今のエリアのアトリビュート情報を取得
  * @note  全ブロックをロードし探索するため、かなり重い
  */
  void CheckAttr( void );

  /**
  * @brief 指定アトリビュートが存在するか取得
  */
  b32 IsAttrExist( u32 attrId );

  /**
  * @brief メッセージデータを取得
  */
  gfl2::str::MsgData* GetDebugMenuMagData( void ) { return m_pMsgData; }

  /**
  * @brief メッセージデータを取得
  */
  gfl2::str::MsgData* GetAttrNameMagData( void ) { return m_pMsgDataAttrName; }

  /**
  * @brief エラーアトリビュート数を取得
  */
  u32 GetAttrErrorCount( void ) { return m_AttrErrorCount; }

public:

  // 内部オブジェクト
  b32                                   m_IsAttrCheck;
  u32                                   m_AttrCountTable[ ATTRIBUTE_MAX ];
  u32                                   m_AttrErrorCount;

  // 外部オブジェクト
  gfl2::heap::HeapBase*                 m_pHeap;
  Fieldmap*                             m_pFieldmap;
  gfl2::str::MsgData*                   m_pMsgData;
  gfl2::str::MsgData*                   m_pMsgDataAttrName;
  GameSys::GameManager*                 m_pGameManager;
  gfl2::fs::AsyncFileManager*           m_pAsyncFileReadManager;

  enum DebugGroupIndex
  {
    DEBUG_GROUP_INDEX_ROOT,
    DEBUG_GROUP_MAX,
  };
  gfl2::debug::DebugWinGroup*           m_pDebugGroup[ DEBUG_GROUP_MAX ];


private:

  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize);

};

GFL_NAMESPACE_END(Menu)
GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_FIELDDEBUGMENUATTREDITOR_H_INCLUDED__
#endif // PM_DEBUG
