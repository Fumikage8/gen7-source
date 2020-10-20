

#ifndef __REGULAROBJECT_H__
#define __REGULAROBJECT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "ScriptObject.h"
#include "Field/FieldRo/include/Terrain/FieldDynamicList.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

class FieldScriptSystem;
class SceneChangeControl;
struct DebugSceneEditParam;

//=============================================================================
/**
 * @class RegularObject
 * @brief 通常スクリプト用オブジェクト
 */
//=============================================================================
class RegularObject : public ScriptObject
{
  GFL_FORBID_COPY_AND_ASSIGN( RegularObject ); ///< コピーコンストラクタ + 代入禁止
public:

  typedef Field::DynamicList< RegularObject* > RegularObjectList;
  typedef Field::ListIterator< RegularObject* > RegularObjectListIterator;

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param p_system スクリプトシステムへのポインタ
   * @param p_heap ヒープへのポインタ
   * @param p_heap_debug デバッグ用ヒープへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  RegularObject( FieldScriptSystem* p_system, gfl2::heap::HeapBase* p_heap, gfl2::heap::HeapBase* p_heap_debug = NULL );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  virtual ~RegularObject( void );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 主処理
   *
   * @return pawn主処理の戻り値
   */
  /* ------------------------------------------------------------------------- */
  virtual s32 Update( void );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief インスタンスを生成
   *
   * @param buffer AMXバッファ
   * @param size AMXバッファサイズ
   * @param amxname AMX名
   */
  /* ------------------------------------------------------------------------- */
  virtual void Load( const void* buffer, const size_t size, const c8* const amxname );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 既にあるバッファからインスタンスを生成
   *
   * @param p_org オリジナルインスタンスへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  virtual void LoadShare( const gfl2::pawn::PawnBase* p_org );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 通常ロード
   *
   * @param buffer AMXバッファ
   * @param size AMXバッファサイズ
   * @param amxname AMX名
   */
  /* ------------------------------------------------------------------------- */
  void LoadNormal( const void* buffer, const size_t size, const char* amxname );

  /* ------------------------------------------------------------------------- */
  /**
  * @brief スクリプトが完了しているか？
  *
  * @return true:している, false:していない
  */
  /* ------------------------------------------------------------------------- */
  bool IsFinished( void );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ロードが完了しているか？
   *
   * @return true:している, false:していない
   */
  /* ------------------------------------------------------------------------- */
  bool IsLoaded( void ) const { return m_IsLoaded; }
  //-----------------------------------------------------------------------------
  /**
   * @brief レジスト用のデータを設定（Loadの事前に設定する）
   *
   * @param zone_id ゾーンID
   * @param amx_id  AMX ID
   */
  //-----------------------------------------------------------------------------
  void SetRegistData( ZoneID zone_id, u32 amx_id );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief レジストデータIDを設定
   *
   * @param reg_id レジストデータID
   */
  /* ------------------------------------------------------------------------- */
  void SetRegistData( u32 reg_id ) { m_RegID = reg_id; };
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 終了処理
   */
  /* ------------------------------------------------------------------------- */
  void Finalize( void );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief シーン遷移データを設定
   *
   * @param sceneControl シーン遷移コントローラへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void SetSceneChangeData( SceneChangeControl * sceneControl );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ウェイト関数
   *
   * @param p_sys スクリプトシステムへのポインタ
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   *
   * @return true:Wait完了, false:Wait中
   */
  /* ------------------------------------------------------------------------- */
  typedef bool (*WAIT_FUNC)( FieldScriptSystem* p_sys, u32 label, void* p_work );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief ウェイト関数を設定
   *
   * @param func ウェイト関数へのポインタ
   * @param label ユーザーラベル
   * @param p_work ユーザーワーク
   */
  /* ------------------------------------------------------------------------- */
  void SetWaitFunc( WAIT_FUNC func, u32 label, void* p_work );

  //-----------------------------------------------------------------------------
  /**
  * @brief 並列実行するスクリプトオブジェクトを追加
  *
  * @param pObject 追加対象
  * @param p_heap  管理領域用ヒープ
  *
  * @return 登録ハンドル( -1で失敗 )
  */
  //-----------------------------------------------------------------------------
  s32 RegistParallelObject( RegularObject* pObject, gfl2::heap::HeapBase* p_heap );

  //-----------------------------------------------------------------------------
  /**
  * @brief 指定ハンドルの並列スクリプトが存在するか取得
  *
  * @param handle  指定ハンドル
  * @return true 存在する
  */
  //-----------------------------------------------------------------------------
  b32 IsParallelObjectExists( s32 handle );

  //-----------------------------------------------------------------------------
  /**
  * @brief 現在存在する並列スクリプトの数を取得
  *
  * @return 並列スクリプト数
  */
  //-----------------------------------------------------------------------------
  u32 GetParallelObjectCount( void ) const;

#if PM_DEBUG
  //-----------------------------------------------------------------------------
  /**
   * @brief デバッグ用シーンエディットパラメータ取得
   */
  //-----------------------------------------------------------------------------
  int DebugGetSceneEditParam( DebugSceneEditParam * param );

#endif  //PM_DEBUG

private:

  //-----------------------------------------------------------------------------
  /**
  * @brief 使われていないハンドルを取得
  *
  * @return ハンドル(-1:全て使われている)
  */
  //-----------------------------------------------------------------------------
  s32 GetUnusedParallelHandle( void ) const;

  //-----------------------------------------------------------------------------
  /**
  * @brief ハンドルの使用フラグを操作
  *
  * @param handle 指定ハンドル
  * @param isUse　true 使用
  */
  //-----------------------------------------------------------------------------
  void SetParallelHandleUsageState( s32 handle, b32 isUse );

  bool m_IsLoaded;        ///< ロード完了フラグ
  bool m_IsFirstUpdated;  ///< 初回更新完了フラグ
  u32 m_RegID;      ///< レジストデータID
  FieldScriptSystem* m_pSystem;  ///< スクリプトシステムへのポインタ
  WAIT_FUNC m_WaitFunc; ///< ウェイト関数
  u32       m_WaitLabel; ///< ウェイト関数ユーザーラベル
  void*     m_pWaitWork; ///< ウェイト関数ユーザーワーク


  RegularObjectList m_ParallelObjectList;
  s32               m_ParallelHandle;       // 並列スクリプトID、このオブジェクトが並列実行されているときのみ有効値
  s32               m_ParallelHandleBit;
};

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


#endif // __REGULAROBJECT_H__