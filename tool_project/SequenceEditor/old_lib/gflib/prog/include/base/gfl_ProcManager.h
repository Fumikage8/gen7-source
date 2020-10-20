#ifndef __GFL_PROCMANAGER_H__
#define __GFL_PROCMANAGER_H__
//=============================================================================
/**
 * @file	  gfl_ProcManager.h
 * @brief	  プロセス管理マネージャー
 * @author  k.ohno
 * @date	  2010.10.18
 */
//=============================================================================

#pragma once


#include <nn.h>
#include <gfl_Macros.h>
#include <ro/gfl_RoManager.h>
#include <base/gfl_ProcBaseProcess.h>
#include <gfl_Heap.h>


/**
  呼び出しにはこちらを使ってください。
  NEWは外部 DELETEは内部で行う設計となってしまったので
  define定義で隠す事で違和感を緩和するねらいです
*/

//===========================================================================================
// 静的モジュール用PROC
//===========================================================================================
//PROCの呼び出し（下位コール）
#define GFL_PROC_CALL( pManager, CallClass ) \
  pManager->CallProc( GFL_NEW(pManager->GetHeap()) CallClass )

//PROCの切り替え
#define GFL_PROC_SETNEXT( pManager, NextClass) \
  pManager->SetNextProc( GFL_NEW(pManager->GetHeap()) NextClass )


//PROCの呼び出し   GFLIB_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* GFLIB_PROC_CALL(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->CallProc( pCC );
  return pCC;
}

//PROCの切り替え（下位コール）   GFLIB_PROC_SETNEXT<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ManagerClass , typename CallClass> CallClass* GFLIB_PROC_SETNEXT(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->SetNextProc( pCC );
  return pCC;
}

//===========================================================================================
// 動的モジュール用PROC
//===========================================================================================
#ifdef GFL_USE_RO

//PROCの呼び出し   GFLIB_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ProcManagerClass , class CallClass> CallClass* GFLIB_RO_PROC_CALL(ProcManagerClass* pManager, const char* croFileName )
{
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;

  pManager->CallProc( pCC );
  pCC->SetModulePtr( pModule );

  return pCC;
}

//PROCの切り替え（下位コール）   GFLIB_PROC_SETNEXT<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ProcManagerClass , typename CallClass> CallClass* GFLIB_RO_PROC_SETNEXT(ProcManagerClass* pManager, const char* croFileName )
{
  nn::ro::Module* pModule = gfl::ro::StartModuleFunc( croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;

  pManager->SetNextProc( pCC );
  pCC->SetModulePtr( pModule );

  return pCC;
}

#endif



//===========================================================================================

namespace gfl {
  namespace proc {

    class BaseProcess;

    //------------------------------------------------------------------
    /**
     * @brief	プロセスメイン関数の結果
     *
     * プロセスの初期化・メイン・終了関数を登録したデータ
     */
    //------------------------------------------------------------------
    typedef enum {
      MAINSTATUS_NULL,         ///<PROCが存在しない
      MAINSTATUS_VALID,        ///<有効なPROCがある
      MAINSTATUS_CHANGE,       ///<PROCが切り替わった
      MAINSTATUS_CALL,         ///<あらたなPROCがコール
    } MainStatus;

    //------------------------------------------------------------------
    /**
     * @brief	プロセス動作の過程
     */
    //------------------------------------------------------------------
    typedef enum {
      SEQ_LOAD,   ///< 初期化がまだ
      SEQ_INIT,   ///< 初期化中
      SEQ_MAIN,   ///< メイン動作中
      SEQ_END,    ///< 終了中
      SEQ_UNLOAD, ///< 終了済み
      SEQ_USER,   ///< 以下ユーザー定義 procMainをオーバーライドして使う場合
    } ProcSeq;

    class Manager {

      GFL_FORBID_COPY_AND_ASSIGN(Manager); //コピーコンストラクタ＋代入禁止

    private:

      //------------------------------------------------------------------
      /**
       * @brief	プロセス動作の管理クラス（内部）
       */
      //------------------------------------------------------------------

      class processHandle {
      public:
        ProcSeq proc_seq;				  ///<プロセス内部シーケンス
        processHandle * parent;			///<上位プロセスへのポインタ
        processHandle * child;			///<下位プロセスへのポインタ
        BaseProcess * my;		    ///<現在のメインプロセスのポインタ
      };

    private:

      //------------------------------------------------------------------
      /**
       * @brief	プロセスシステムメンバ変数
       */
      //------------------------------------------------------------------
      gfl::heap::HeapBase * pHeapBase; ///< PROCを作る時に使うHeapBase
      bool call_flag;				///<子プロセス呼び出しのリクエストフラグ
      bool jump_flag;				///<次のメインプロセスが存在するかどうかのフラグ
      bool draw_flag;       ///<Drawの処理を行うかのフラグ。MainでUpdateFuncを呼ぶと立つ
      BaseProcess * next_data;	///<次のメインプロセスの生成データ

      processHandle * mainHandle;  // 進行中のハンドル


      //------------------------------------------------------------------
      /**
       * @brief	プロセスシステムメンバ関数
       */
      //------------------------------------------------------------------
      void entryCallProc(void);
      void jumpCallProc(void);
      void deleteAndJumpProc(void);
      void deleteMainProc( processHandle* proc );
      virtual bool procMain(void);


    protected:
      //------------------------------------------------------------------
      /**
       * @brief	進行中ハンドルへのアクセサ
       */
      //------------------------------------------------------------------
      ProcSeq GetMainHandleProcSeq( void ) const;
      void SetMainHandleProcSeq( ProcSeq seq );
      BaseProcess* GetMainHandleBaseProcess( void ) const;
      void SetDrawFlag( bool draw );

    public:
      Manager( gfl::heap::HeapBase* pBase );
      virtual ~Manager();


      //------------------------------------------------------------------
      /**
       * @brief	PROCの処理
       * 登録したPROCの処理を行います。 適切な位置で呼んでください。
       * @return MainStatus ステータスの動きが帰ってきます
       */
      //------------------------------------------------------------------
      MainStatus Main(void);
      //------------------------------------------------------------------
      /**
       * @brief	PROCの処理
       * 登録したPROCの処理を行います。 適切な位置で呼んでください。
       * 必要な場合は、前後でグラフィックの処理を呼んでください。
       */
      //------------------------------------------------------------------
      void Draw(void);


      //------------------------------------------------------------------
      /**
       * @brief	PROCの呼び出し（下位コール）
       * @param	procdata	新しいプロセスの定義データアドレス
       * @param	pwork		パラメータワークへのポインタ
       * @return 作ったBaseProcess
       * この関数を呼び出した次のフレームから指定したプロセスが呼び出される。
       * 呼び出されたプロセスが終了すると、現在のプロセスへと自動復帰する。
       */
      //------------------------------------------------------------------
      BaseProcess* CallProc(BaseProcess * proc);

      //------------------------------------------------------------------
      /**
       * @brief	PROCの切り替え
       * @param	procdata	新しいプロセスの定義データアドレス
       * @param	pwork		パラメータワークへのポインタ
       * @return 作ったBaseProcess
       * 現在のプロセスが終了した後に、この関数で指定したプロセスへと制御が切り替わる。
       */
      //------------------------------------------------------------------
      BaseProcess* SetNextProc(BaseProcess * proc);


      //------------------------------------------------------------------
      /**
       * @brief	PROCを作る時に確保する為のHeapBaseを返す
       * @return HeapBase ヒープベースクラスのポインタ
       */
      //------------------------------------------------------------------

      inline gfl::heap::HeapBase* GetHeap(void){ return pHeapBase; };

      //------------------------------------------------------------------
      /**
       * @brief	 今動いているBaseProcessの親に当たるBaseProcessを返す
       * @return BaseProcessクラスポインタ 親がなければNULL
       */
      //------------------------------------------------------------------

      BaseProcess* GetParentBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 今動いているBaseProcessを返す
       * @return BaseProcessクラスポインタ なければNULL
       */
      //------------------------------------------------------------------

      BaseProcess* GetBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 次に設定されているBaseProcessを返す
       * @return BaseProcessクラスポインタ なければNULL
       */
      //------------------------------------------------------------------
      BaseProcess* GetNextBaseProcess(void);

      //------------------------------------------------------------------
      /**
       * @brief	 ジャンププロセスへのフラグがたっているかどうか確認する
       * @return jump_flag
       */
      //------------------------------------------------------------------
      inline bool GetJumpFlag(void){ return jump_flag; };

      //------------------------------------------------------------------
      /**
       * @brief	 mainHandleに実体があるかどうか
       * @return あったらtrue
       */
      //------------------------------------------------------------------
      bool IsMainHandle(void){ return (mainHandle!=NULL); };
      //------------------------------------------------------------------
      /**
       * @brief	 描画フラグ
       * @return MainでUpdateFuncを呼ぶと立つ
       */
      //------------------------------------------------------------------
      bool GetDrawFlag(void){return draw_flag;}
      
      // メイン動作中のプロセスの初期化完了チェック
      //------------------------------------------------------------------
      /**
       * @brief	 メイン動作中のプロセスの初期化完了チェック
       * @return 初期化完了でtrue
       */
      //------------------------------------------------------------------
      bool GetBaseProcessInitialized( void ) const;

    };

  } // namespace proc
} // namespace gfl


#endif // __GFL_PROCMANAGER_H__
