//==============================================================================
/**
 * @file	JFDynamicLoadSystem.h
 * @brief	ジョインフェスタモデル動的読み込みシステム
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#ifndef __JF_DYNAMICLOADSYSTEM_H__
#define __JF_DYNAMICLOADSYSTEM_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
// DebugWin
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// JoinFestaDefine
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"

// 前方宣言
namespace NetApp{
  namespace JoinFesta{
    class JFModelManager; // JFモデルマネージャ
    class JFPoint;        // JFポイント
    class JFPointManager; // JFポイントマネージャ
    class JoinFestaEffectSystem; // エフェクトシステム
    struct JoinFestaDataTable;
  }
}
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}
namespace Field{
  class Fieldmap;
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JFDynamicLoadSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(JFDynamicLoadSystem);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-------------------------------------
  /// 状態定義
  //=====================================
  enum STATE{
    STATE_NONE,       ///< なにもしない
    STATE_COMMON,     ///< 通常
    STATE_DELETING,   ///< モデル削除処理中
    STATE_DISPLAYING, ///< モデル表示処理中
  };
  
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  JFDynamicLoadSystem( gfl2::heap::HeapBase* pHeap, Field::Fieldmap* pFieldmap );
  // デストラクタ
  virtual ~JFDynamicLoadSystem();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   * @param[in]   pJoinFestaEffectSystem  エフェクトシステム
   * @param[in]   pDataTable　データテーブルのポインタ
   */
  //------------------------------------------------------------------
  void Initialize( JoinFestaEffectSystem* pJoinFestaEffectSystem, const JoinFestaDataTable* pDataTable );

  //------------------------------------------------------------------
  /**
   * @brief   初期化完了待ち
   * @return  trueで完了
   */
  //------------------------------------------------------------------
  bool IsInitializeComplete();

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update();

  //------------------------------------------------------------------
  /**
   * @brief   トラバース直前の更新（処理負荷対策用）
   */
  //------------------------------------------------------------------
  void TraverseBefore();

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   * @return  RESULT  関数の結果
   */
  //------------------------------------------------------------------
  RESULT Terminate();

  //------------------------------------------------------------------
  /**
   * @brief   動的に主人公の周りに動作モデルを出現させる仕組みを無効化する
   */
  //------------------------------------------------------------------
  void DynamickLoadDisable();

  //------------------------------------------------------------------
  /**
   * @brief   動的に主人公の周りに動作モデルを出現させる仕組みを有効化する
   * @note    DynamickLoadDisableの解除用
   */
  //------------------------------------------------------------------
  void DynamickLoadEnable();

  //------------------------------------------------------------------
  /**
   * @brief   指定したパーソナルデータの出現リクエストを出す
   */
  //------------------------------------------------------------------
  void DynamickLoadOneRequest( const u32 idx );
  void DynamickLoadOneRequest( const NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData );

  //------------------------------------------------------------------
  /**
   * @brief   指定したパーソナルデータの出現リクエストが終了したか
   * @retval  trueで終了した
   */
  //------------------------------------------------------------------
  bool IsDynamickLoadOneRequestEnd();

  //------------------------------------------------------------------
  /**
   * @brief   指定したパーソナルデータの削除リクエストを出す
   */
  //------------------------------------------------------------------
  void DynamickUnloadOneRequest( const JoinFestaFriendKey& friendKey );

  //------------------------------------------------------------------
  /**
   * @brief   指定したパーソナルデータの削除リクエストが終了したか
   * @retval  trueで終了した
   */
  //------------------------------------------------------------------
  bool IsDynamickUnloadOneRequestEnd();

  //=============================================================================
  // 動作モデル管理マネージャ取得
  JFModelManager* GetJFModelManager() const { return m_JFModelManager; }
  // ポイント管理マネージャ取得
  JFPointManager* GetJFPointManager() const { return m_JFPointManager; }

#if PM_DEBUG
  // デバッグ用：削除リクエスト
  bool DEBUG_UnloadRequest( const u32 idx );
#endif
  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  // 状態別更新関数
  void State_Common();
  void State_Deleting();
  void State_Displaying();

  // JFモデル
  void JFModelRequest( JFPoint* pJFPoint );
  void JFModelDelete( JFPoint* pJFPoint );

  // 最も近いJFポイントを取得
  JFPoint* GetNearJFPoint();

private:
  //! JFモデル表示ポイントサーチで進行方向距離（歩き）
  const f32 JFMODEL_WALK_SEARCH_POINT_Z;  

  //! JFモデル表示ポイントサーチで進行方向距離（走り）
  const f32 JFMODEL_RUN_SEARCH_POINT_Z;

  //! JFモデル表示ポイントサーチで進行方向距離（X軸調整）
  const f32 JFMODEL_SEARCH_POINT_X;

private:
  gfl2::heap::HeapBase*   m_heap;           ///< ヒープ
  JFModelManager*         m_JFModelManager; ///< モデル管理マネージャ
  JFPointManager*         m_JFPointManager; ///< モデル位置管理マネージャ
  STATE                   m_state;          ///< 状態
  Field::Fieldmap*        m_pFieldmap;      ///< フィールド
  bool                    m_isInitialize;   ///< 初期化済みフラグ
  u8                      m_initializeSeq;  ///< 初期化シーケンス
  bool                    m_isTutorial;     ///< チュートリアル中かどうか
  bool                    m_isRREvent;      ///< RR団イベント中かどうか
  bool                    m_isRoom;         ///< 城内か

  bool                    m_isDynamicLoadDisable; //! 動的出現システムの無効化フラグ
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JF_DYNAMICLOADSYSTEM_H__
