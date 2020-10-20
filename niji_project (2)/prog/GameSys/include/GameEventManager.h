#ifndef __GAMEEVENTMANAGER_H__
#define __GAMEEVENTMANAGER_H__
//===================================================================
/**
 * @file    GameEventManager.h
 * @brief   イベントの制御クラス
 * @author  k.ohno
 * @date    2011.3.6   Syachiから移植
 */
//===================================================================

#pragma once

#include <base/include/gfl2_Singleton.h>


namespace GameSys {

  class GameEvent;
  class GameManager;
}

//!PROCの呼び出し   GFLIB_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_CALL(ManagerClass* pManager)
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // 常駐ヒープ監視
#endif // PM_DEBUG
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pManager->CallEvent( pCC );
  return pCC;
}

//!PROCの切り替え（下位コール）   GAMESYSTEM_EVENT_CHANGE<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GAMESYSTEM_EVENT_CHANGE(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pManager->ChangeEvent( pCC );
  return pCC;
}

//! DLL使用　コール
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_RO_CALL(ManagerClass* pManager, const char * croFileName )
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // 常駐ヒープ監視
#endif // PM_DEBUG
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( croFileName );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->CallEvent( pCC );
  return pCC;
}

//!PROCの切り替え（下位コール）   GAMESYSTEM_EVENT_CHANGE<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GAMESYSTEM_EVENT_RO_CHANGE(ManagerClass* pManager, const char * croFileName )
{
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( croFileName );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->ChangeEvent( pCC );
  return pCC;
}

#if PM_DEBUG
//! DLL使用　コール DLLをヒープの下からとる
template <class ManagerClass , class CallClass> CallClass* GAMESYSTEM_EVENT_RO_LOWER_CALL(ManagerClass* pManager, const char * croFileName )
{
#if PM_DEBUG
  pManager->SetChkHeapData(); // 常駐ヒープ監視
#endif // PM_DEBUG
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( croFileName, true );
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pManager->GetHeap() );
  pCC->SetRoModule( pModule );
  pManager->CallEvent( pCC );
  return pCC;
}
#endif // PM_DEBUG

namespace GameSys {
  
  //-----------------------------------------------------------------------------
  /**
   * @brief	イベント制御関数の戻り値定義
   */
  //-----------------------------------------------------------------------------
  enum GMEVENT_RESULT{
    GMEVENT_RES_CONTINUE = 0, ///<イベント継続中
    GMEVENT_RES_FINISH,       ///<イベント終了
    GMEVENT_RES_OFF,       ///<イベント終了中
    GMEVENT_RES_CONTINUE_DIRECT = 33,  ///<イベント継続（もう一周する）
  };

  class GameEventManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(GameEventManager); //コピーコンストラクタ＋代入禁止

    friend class GameManager; // isExists を使うため

    private:
      GameEvent* mpEvent;     ///< 実行中のイベント
      GameManager* mpGameMgr;	///<ゲームの全体管理クラスのポインタ

      //! チェンジイベント
      GameEvent* mpChangeEvent;

      bool mDoEvent;  ///< イベント実行したフレーム中はtrue

#if PM_DEBUG
    //! 常駐ヒープの監視
      enum{
      CHK_SYSTEM,           ///< システムヒープ
      CHK_RESIDENT,         ///< Residentヒープ
      CHK_RESIDENT_DEVICE,  ///< Residentデバイスヒープ
      CHK_APP,              ///< アプリケーションヒープ
      CHK_APP_DEVICE,       ///< アプリケーションデバイスヒープ 
      CHK_EVENT_DEVICE,     ///< イベントデバイスヒープ
      CHK_APP_CONTROL,      ///< アプリケーションコントロールヒープ

      CHK_HEAP_MAX,
      };
      //! ヒープチェックデータ
      struct ChkHeapData
      {
        u32 free;         ///< 空き量
        u32 allocatable;  ///< 最大確保量
      };
      static const u32  m_scChkHeapTbl[CHK_HEAP_MAX];     ///< ヒープチェックテーブル
      static const char*m_scChkHeapNameTbl[CHK_HEAP_MAX]; ///< ヒープチェック名前テーブル
      ChkHeapData       m_ChkHeapData[CHK_HEAP_MAX];      ///< ヒープチェック情報テーブル
      b32               m_ChkHeapDataFlag;  ///< チェックフラグ
      bool              m_ChkHeapDataAssertFlag;  ///< アサートフラグ
      b32               m_ChkHeapDataError; ///< エラー状態
      u32               m_ChkHeapDataErrorHeapID; ///< エラーが起きたヒープID
      u8                m_ChkHeapDataId; ///< チェックIDのカウンター

      void ChkHeapDataFunc( void ); ///< 保持ヒープ情報と今のヒープ情報を比較
#endif

      //------------------------------------------------------------------
      /**
       * @brief	イベント切替
       *
       * @param	event	  	  次のイベント制御ワークへのポインタ
       */
      //------------------------------------------------------------------
      void Change(GameEvent * event);
    
      //------------------------------------------------------------------
      /**
       * @brief	イベント起動中チェック
       * @retval	true	イベント起動中
       * @retval	false	イベントなし
       */
      //------------------------------------------------------------------
      bool isExists(void) const;

    public:

      //コンストラクタ
      GameEventManager(GameManager* gmgr);
      // デストラクタ
      ~GameEventManager();

      //------------------------------------------------------------------
      /**
       * @brief	イベント切替
       * @param	event		次のイベント制御ワークへのポインタ
       */
      //------------------------------------------------------------------
      void ChangeEvent(GameEvent * next_event);

      //------------------------------------------------------------------
      /**
       * @brief	イベント呼び出し
       * @param	event		子（呼び出す）イベント制御ワークへのポインタ
       */
      //------------------------------------------------------------------
      void CallEvent(GameEvent * child);
    
      //------------------------------------------------------------------
      /**
       * @brief	イベント起動中チェック そのフレーム1度でもイベントを起動していたらTRUE
       * @retval	true	イベント起動中
       * @retval	false	イベントなし
       */
      //------------------------------------------------------------------
      bool IsExists(void) const;

      //------------------------------------------------------------------
      /**
       * @brief	  イベント制御メイン
       * @retval	GMEVENT_RESULT
       */
      //------------------------------------------------------------------
      GMEVENT_RESULT Main(void);

      //----------------------------------------------------------------------------
      /**
       *	@brief  イベント用のヒープの取得
       */
      //-----------------------------------------------------------------------------
      gfl2::heap::HeapBase* GetHeap(void);

      GameSys::GameEvent* GetGameEvent(){ return mpEvent; }

#if PM_DEBUG
      void SetChkHeapData( void );    ///< 今のヒープ情報を保持
      inline void SetDebugChkHeapData( bool flag ){ m_ChkHeapDataFlag = flag; } ///< ヒープチェックの有効無効設定
      inline b32 IsDebugChkHeapDataError( void ) const{ return m_ChkHeapDataError; }  ///< 常駐ヒープ　leakが発生しているか？
      inline u32 IsDebugChkHeapErrorHeapID( void ) const { return m_ChkHeapDataErrorHeapID; } ///< 引っかかった最後のHEAPID
      inline bool * GetDebugChkHeapDataAssertFlag() { return &m_ChkHeapDataAssertFlag; }
#endif // PM_DEBUG

  };

} //namespace GameSys


#endif //__GAMEEVENTMANAGER_H__

