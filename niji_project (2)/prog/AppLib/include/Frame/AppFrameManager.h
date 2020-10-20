#ifndef __APP_FRAME_MANAGER_H__
#define __APP_FRAME_MANAGER_H__
//=============================================================================
/**
 * @file    AppFrameManager.h
 * @brief   メニューフレームのマネージャー   AppFrameCellSuperを継承したクラスを管理する
 * @author  ohno_katsumi
 * @date    2014.12.17
 */
//=============================================================================
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include "gfx/include/gfl2_types.h"
#include "AppLib/include/Util/app_util_heap.h"

//#include "AppFrameCellSuper.h"

//===========================================================================================
// 静的モジュール
//===========================================================================================
/**
  呼び出しにはこちらを使ってください。
  NEWは外部 DELETEは内部で行う設計となってしまったので
  template定義で隠す事で違和感を緩和するねらいです

  JUMPは作りません。次に何処に行くかの制御はmenu_event側で行ってください。

  呼び出し元画面は描画はするがキー操作やUpdateを行わない場合     MENU_FRAME_CALL_MODALを使ってください。 infomation画面呼び出しとか

  呼び出し元の画面は描画もキーも拾う場合 説明がでているが制御は親が持ち続ける場合  MENU_FRAME_CALL_MODELESSを使ってください。

  呼び出し元の画面も呼び出し先も描画もキーも拾う場合
  連動もしてないときは   完全な並列なので
  マネージャーを別に作る必要があります。

  注意：このヒープは使用量は小さいですが穴が開きます  渡すメモリーは専用にしてください。
  注意：CALLが終わった際にはメモリーが元の状態に戻っているように設計してください。



 */
//PROCの呼び出し   MENU_FRAME_CALL_MODAL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* MENU_FRAME_CALL_MODAL(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->CallProc( pCC );
  pCC->SetMyManager(pManager);
  return pCC;
}

//PROCの並列実行   MENU_FRAME_CALL_MODELESS<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* MENU_FRAME_CALL_MODELESS(ManagerClass* pManager )
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pManager->ParallelProc( pCC );
  pCC->SetMyManager(pManager);
  return pCC;
}



GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class UtilTextRenderingPipeLine;
class DrawUtilText;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )


//===========================================================================================

GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )

class CellSuper;
class AppFrameListener;

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
  SEQ_UNLOAD_NEXT, ///< 終了済み 次をよぶ
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
    processHandle * parallel;			///< 並列状態を示すポインタ
    CellSuper * my;		    ///<現在のメインプロセスのポインタ
  };

private:

  //------------------------------------------------------------------
  /**
   * @brief	プロセスシステムメンバ変数
   */
  //------------------------------------------------------------------
  gfl2::heap::HeapBase * mpHeapBase; ///< PROCを作る時に使うHeapBase

  bool mParallel_flag;				///<並列プロセス呼び出しのリクエストフラグ
  bool mCall_flag;				///<子プロセス呼び出しのリクエストフラグ
  bool mJump_flag;				///<次のメインプロセスが存在するかどうかのフラグ
  CellSuper * mpNextData;	///<次のメインプロセスの生成データ

  processHandle * mpMainHandle;  // 進行中のハンドル

  AppFrameListener* mpAppFrameListener; //状態通知


  //------------------------------------------------------------------
  /**
   * @brief	プロセスシステムメンバ関数
   */
  //------------------------------------------------------------------
  void entryCallProc(void);
	void parallelCallProc(void);
  void jumpCallProc(void);
  bool deleteAndJumpProc(void);
  void deleteMainProc( processHandle* proc );
  bool procMain(processHandle* mainH );
  bool procUpdateMain( processHandle* mainH  );


protected:
  //------------------------------------------------------------------
  /**
   * @brief	進行中ハンドルへのアクセサ
   */
  //------------------------------------------------------------------
  ProcSeq GetMainHandleProcSeq( void ) const;
  void SetMainHandleProcSeq( ProcSeq seq );
  CellSuper* GetMainHandleBaseProcess( void ) const;
  void SetDrawFlag( bool draw );

public:
  Manager( gfl2::heap::HeapBase* pBase );
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
  void Draw(gfl2::gfx::CtrDisplayNo no);

  //------------------------------------------------------------------
  /**
   * @brief	  終了処理
   * @retval  true:終了処理中
   * @retval  false:終了処理完了
   *
   * フレームが残っている場合、フレームを終わらせます。
   * 外側から強制でMainが終了される場合は必ずEndFuncでコールして下さい。
   */
  //------------------------------------------------------------------
  bool End();

  //------------------------------------------------------------------
  /**
   * @brief	fameの呼び出し（下位コール）
   * @param	procdata	新しいプロセスの定義データアドレス
   * @param	pwork		パラメータワークへのポインタ
   * @return 作ったBaseProcess
   * この関数を呼び出した次のフレームから指定したプロセスが呼び出される。
   * 呼び出されたプロセスが終了すると、現在のプロセスへと自動復帰する。
   */
  //------------------------------------------------------------------
  CellSuper* CallProc(CellSuper * proc);

  //------------------------------------------------------------------
  /**
   * @brief	farmeの並列呼び出し
   * @param	procdata	新しいプロセスの定義データアドレス
   * @param	pwork		パラメータワークへのポインタ
   * @return 作ったBaseProcess
   * 並列といっても作った側は子供なので、子供から先に消さないといけない構造になっています。
   * ASSERTいれておきますので間違った場合は気づくようになっています。
   * 別の構造がうれしい場合にはご相談下さい。
   */
  //------------------------------------------------------------------
  CellSuper* ParallelProc(CellSuper * proc);

  
  //------------------------------------------------------------------
  /**
   * @brief	PROCの切り替え
   * @param	procdata	新しいプロセスの定義データアドレス
   * @param	pwork		パラメータワークへのポインタ
   * @return 作ったBaseProcess
   * 現在のプロセスが終了した後に、この関数で指定したプロセスへと制御が切り替わる。
   */
  //------------------------------------------------------------------
  CellSuper* SetNextProc(CellSuper * proc);


  //------------------------------------------------------------------
  /**
   * @brief	PROCを作る時に確保する為のHeapBaseを返す
   * @return HeapBase ヒープベースクラスのポインタ
   */
  //------------------------------------------------------------------

  inline gfl2::heap::HeapBase* GetHeap(void){ return mpHeapBase; };

  //------------------------------------------------------------------
  /**
   * @brief	 今動いているBaseProcessの親に当たるBaseProcessを返す
   * @return BaseProcessクラスポインタ 親がなければNULL
   */
  //------------------------------------------------------------------

  CellSuper* GetParentBaseProcess(void);

  //------------------------------------------------------------------
  /**
   * @brief	 今動いているBaseProcessを返す
   * @return BaseProcessクラスポインタ なければNULL
   */
  //------------------------------------------------------------------

  CellSuper* GetBaseProcess(void);

  //------------------------------------------------------------------
  /**
   * @brief	 次に設定されているBaseProcessを返す
   * @return BaseProcessクラスポインタ なければNULL
   */
  //------------------------------------------------------------------
  CellSuper* GetNextBaseProcess(void);

  //------------------------------------------------------------------
  /**
   * @brief	 ジャンププロセスへのフラグがたっているかどうか確認する
   * @return jump_flag
   */
  //------------------------------------------------------------------
  inline bool GetJumpFlag(void){ return mJump_flag; };

  //------------------------------------------------------------------
  /**
   * @brief	 mainHandleに実体があるかどうか
   * @return あったらtrue
   */
  //------------------------------------------------------------------
  bool IsMainHandle(void){ return (mpMainHandle!=NULL); };

  // メイン動作中のプロセスの初期化完了チェック
  //------------------------------------------------------------------
  /**
   * @brief	 メイン動作中のプロセスの初期化完了チェック
   * @return 初期化完了でtrue
   */
  //------------------------------------------------------------------
  bool GetBaseProcessInitialized( void ) const;


  //------------------------------------------------------------------
  /**
   * @brief	 Frameの状態を取得するリスナーをセットする
   * @param  リスナー
   */
  //------------------------------------------------------------------
  void SetAppFrameListener(AppFrameListener* pL){ mpAppFrameListener = pL; }

  
//#if PM_DEBUG
#if 0

public:
  void SetDispDebugNo(bool bEnable){ m_debugNoDisp = bEnable; };
  
private:

  static const int SIZE_DEBUG_APP = 0x40000;
  static const int SIZE_DEBUG_APPDEV = 0x80000;

  gfl2::util::UtilTextRenderingPipeLine* mpDebugPipeLine;

  app::util::Heap* mpDebugHeap;

	gfl2::util::DrawUtilText* mpDrawUtilText;

  void DrawDebugPrintInit( void );
  void DrawDebugPrintEnd( void );
  void DrawDebugPrintLog( int seq );
  void DrawDebug(gfl2::gfx::CtrDisplayNo no );

private:
  bool m_debugIsEndCall;
  bool m_debugNoDisp;
  

#endif


};


GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )


#endif// __APP_FRAME_MANAGER_H__
