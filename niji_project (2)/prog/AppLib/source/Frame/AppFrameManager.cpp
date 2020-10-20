//=============================================================================================
/**
 * @file	AppFrameManager.cpp
 * @brief	管理マネージャー
 * @date	2015.02.08
 */
//=============================================================================================


#include <AppLib/include/Frame/AppFrameCellSuper.h>  // 基底クラス
#include <AppLib/include/Frame/AppFrameManager.h>  // マネージャー
#include <AppLib/include/Frame/AppFrameListener.h>  // Listener

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include "System/include/HeapDefine.h"

#include "util/include/gfl2_UtilTextRenderingPipeLine.h"

#include "Print/include/SystemFont.h"
#include <system/include/GflUse.h>


namespace applib {
namespace frame {




//=============================================================================================
//
//		外部公開関数
//
//=============================================================================================

//------------------------------------------------------------------
/**
 * @brief	  プロセス管理システム初期化
 * @param	  heap_id			使用するヒープのID
 */
//------------------------------------------------------------------
Manager::Manager(gfl2::heap::HeapBase * pBase)
{
	mpHeapBase = pBase;
	mpMainHandle = NULL;
	mCall_flag = false;
	mJump_flag = false;
  mParallel_flag = false;
  mpNextData = NULL;
  mpAppFrameListener=NULL;

//#if PM_DEBUG
#if 0
  DrawDebugPrintInit();
  m_debugIsEndCall = false;
  m_debugNoDisp = true;
#endif

}

//------------------------------------------------------------------
/**
 * @brief	  プロセス管理システム終了
 */
//------------------------------------------------------------------

Manager::~Manager()
{
//#if PM_DEBUG
#if 0
  GFL_ASSERT_MSG( m_debugIsEndCall, "Endがコールされていない\n" );
  if(mpMainHandle!=NULL){
    GFL_WARNING("mpMainHandleが解放されていない\n");
  }
  DrawDebugPrintEnd();
#endif //PM_DEBUG
}


//------------------------------------------------------------------
// 子プロックに切り替え
//------------------------------------------------------------------
void Manager::entryCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(mpHeapBase) processHandle;

  if(mpMainHandle){
    proc->parent = mpMainHandle;
    mpMainHandle->child = proc;
    mpMainHandle = proc;
  }
  else{
    mpMainHandle = proc;
  }
  mpMainHandle->my = mpNextData;
  //フラグを落す
  mCall_flag = false;
  mpNextData = NULL;
}



//------------------------------------------------------------------
//  並列になるものを生成
//------------------------------------------------------------------
void Manager::parallelCallProc(void)
{

  processHandle* proc;
  proc = GFL_NEW(mpHeapBase) processHandle;

  if(mpMainHandle){
    proc->parallel = mpMainHandle;
    proc->parent = mpMainHandle;
    mpMainHandle->child = proc;
    mpMainHandle = proc;
  }
  else{
    mpMainHandle = proc;
  }
  mpMainHandle->my = mpNextData;
  //フラグを落す
  mParallel_flag = false;
  mpNextData = NULL;
}


//------------------------------------------------------------------
// プロック移動
//------------------------------------------------------------------
void Manager::jumpCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(mpHeapBase) processHandle;

  //移動の場合は全部終わった時に行われる
  GFL_ASSERT_STOP(mpMainHandle->proc_seq == SEQ_UNLOAD);  //@fix

  //親子関係構築
  if(mpMainHandle->parent){
    proc->parent = mpMainHandle->parent;
    proc->parent->child = proc;
  }

  deleteMainProc( mpMainHandle );

  mpMainHandle = proc;
  mpMainHandle->my = mpNextData;

  //フラグを落す
  mJump_flag = false;
  mpNextData = NULL;
}

//------------------------------------------------------------------
// メインプロセスの削除
//------------------------------------------------------------------
void Manager::deleteMainProc( processHandle* proc )
{
#if GFL_RO_ENABLE & 0
  nn::ro::Module* modules[16+1];
  u32             childNum = proc->my->GetChildModulePtrBufferNum();
  //
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  // ローカルバッファは16しか確保していないので念のため
  GFL_ASSERT_STOP( (childNum<=16) );

  // 割り当てられたモジュールをコピー
  modules[0] = proc->my->GetModulePtr();
  for( u32 i = 0; i < childNum; i++ ){
    modules[1+i] = proc->my->GetChildModulePtr(i);
  }

  // デストラクタ実行の為、DLLモジュール解放の前にPROCを解放する
  GFL_SAFE_DELETE( proc->my );

  // DLLモジュールを解放
  u32 moduleNum = (childNum+1);
  for( u32 i = 0; i < moduleNum; i++ )
  {
    if( modules[i] != NULL ){
      roMgr->DisposeModule( modules[i] );
    }
  }
#else
  GFL_SAFE_DELETE( proc->my );
#endif
  proc->my = NULL;
}

//------------------------------------------------------------------
// プロックの消去とジャンプを行う
//------------------------------------------------------------------
bool Manager::deleteAndJumpProc(void)
{
  processHandle* proc = mpMainHandle;
	bool retflg = false;

	if(mpMainHandle->proc_seq == SEQ_UNLOAD_NEXT){  //同じフレームにて次の処理を呼ぶ
		retflg = true;
	}
  if(mJump_flag){  //ジャンプを行う
    jumpCallProc();
  }
  else if(proc->parent){   //親子関係を戻す
    proc->parent->child = NULL;
    mpMainHandle = proc->parent;
    deleteMainProc( proc );
		retflg = true;
  }
  else{  //動く物はなくなったので消す
    deleteMainProc( proc );
    mpMainHandle = NULL;
  }
  GFL_DELETE proc;
	return retflg;
}




//------------------------------------------------------------------
/**
 * @brief	プロセス動作処理メイン
 * @param	proc	プロセスへのポインタ
 * @retval	true	プロセス動作終了
 * @retval	false	プロセス動作継続中
 */
//------------------------------------------------------------------
bool Manager::procUpdateMain( processHandle* mainH  )
{
	Result result;
  processHandle* pNowHandle = mainH;

  if(pNowHandle){
    for ever {
      if( pNowHandle->parallel == NULL){
        break;
      }
      pNowHandle = pNowHandle->parallel;
    }
    for ever {
      result = pNowHandle->my->UpdateFunc();
      if(pNowHandle->child==NULL){
        if ((result == RES_FINISH) || (result == RES_NEXT)) {
          pNowHandle->proc_seq = SEQ_END;
          pNowHandle->my->ResetSubSeq();
        }

        if (result != RES_NEXT) {
          return true;
        }
        return false;
      }
      GFL_ASSERT(result == RES_CONTINUE);  //TOPでないフレームは状態変化をしてはいけない
      pNowHandle = pNowHandle->child;
    }
  }
  return true;
}



//------------------------------------------------------------------
/**
 * @brief	プロセス動作処理メイン
 * @param	proc	プロセスへのポインタ
 * @retval	true	プロセス動作終了
 * @retval	false	プロセス動作継続中
 */
//------------------------------------------------------------------
bool Manager::procMain( processHandle* mainH  )
{
	Result result;

	switch (mainH->proc_seq) {
	case SEQ_LOAD:
		mainH->proc_seq = SEQ_INIT;
		/* fall through */
	case SEQ_INIT:
		result = mainH->my->InitFunc();
		if ((result == RES_FINISH) || (result == RES_NEXT)) {
			mainH->proc_seq = SEQ_MAIN;
			mainH->my->ResetSubSeq();
		}
		if (result != RES_NEXT) {
		  break;
		}
	case SEQ_MAIN:   //※通信の処理入れるならMAINでないと、メモリー状況が不安定
# if 0
    result = mainH->my->UpdateFunc();
		if ((result == RES_FINISH) || (result == RES_NEXT)) {
			mainH->proc_seq = SEQ_END;
			mainH->my->ResetSubSeq();
		}
		if (result != RES_NEXT) {
		  break;
		}
#endif
    if(procUpdateMain(mainH)){
      break;
    }
  case SEQ_END:
		result = mainH->my->EndFunc();
		if (result == RES_FINISH) {
      if(mpAppFrameListener){
        mpAppFrameListener->EndFuncFinish( mainH->my );
      }
			mainH->proc_seq = SEQ_UNLOAD;
      return true;
		}
		else if (result == RES_NEXT) {
      if(mpAppFrameListener){
        mpAppFrameListener->EndFuncFinish( mainH->my );
      }
			mainH->proc_seq = SEQ_UNLOAD_NEXT;
      return true;
		}

		break;
	}
//#if PM_DEBUG
#if 0

//  DrawDebugPrintLog(mainH->my->GetSubSeq());
#endif
	return false;
}

//------------------------------------------------------------------
/**
 * @brief	PROCの処理
 * 登録したPROCの処理を行います。 適切な位置で呼んでください。
 * 必要な場合は、前後でグラフィックの処理を呼んでください。
 */
//------------------------------------------------------------------
void Manager::Draw(gfl2::gfx::CtrDisplayNo no)
{
  processHandle* pNowHandle = mpMainHandle;

  if(mpMainHandle){
    for ever{
      if( pNowHandle->parent == NULL){
        break;
      }
      pNowHandle = pNowHandle->parent;
    }
    for ever {
      if(pNowHandle->proc_seq == SEQ_MAIN){
        pNowHandle->my->DrawFunc(no);
      }
      pNowHandle = pNowHandle->child;
      if(pNowHandle==NULL){
        break;
      }
    }
  }


//#if PM_DEBUG
#if 0
//  DrawDebug(no);
#endif
}

//------------------------------------------------------------------
/**
 * @brief	進行中ハンドルへのアクセサ
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  進行中ハンドルのシーケンスを取得
 *
 *	@retval シーケンス
 */
//-----------------------------------------------------------------------------
ProcSeq Manager::GetMainHandleProcSeq( void ) const
{
  if( mpMainHandle ){
    return mpMainHandle->proc_seq;
  }
  return SEQ_LOAD;  // 進行中のものがない。
}

//----------------------------------------------------------------------------
/**
 *	@brief  進行中ハンドルのシーケンスを設定
 *
 *	@param	seq   シーケンス
 */
//-----------------------------------------------------------------------------
void Manager::SetMainHandleProcSeq( ProcSeq seq )
{
  if( mpMainHandle ){
    mpMainHandle->proc_seq = seq;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  進行中ハンドルのプロセスを取得
 *
 *	@retval NULL以外  プロセス
 *	@retval NULL      今進行中のプロセスはない
 */
//-----------------------------------------------------------------------------
CellSuper* Manager::GetMainHandleBaseProcess( void ) const
{
  if( mpMainHandle ){
    return mpMainHandle->my;
  }
  return NULL;
}



//------------------------------------------------------------------
/**
 * brief	プロセス管理システムメイン
 * @param	psys
 */
//------------------------------------------------------------------
MainStatus Manager::Main(void)
{
	MainStatus status = MAINSTATUS_VALID;

  for ever {
  	if( (mpMainHandle == NULL) && (mpNextData==NULL) ){
      return MAINSTATUS_NULL;   //動いている物はない
    }

    if(mCall_flag) {  // コールする
      entryCallProc();
      status = MAINSTATUS_CALL;
    }
    if(mParallel_flag){//平行呼び出しコール
      parallelCallProc();
      status = MAINSTATUS_CALL;
    }

    if ( procMain( mpMainHandle ) ) {  //メイン処理
      if( deleteAndJumpProc() ){  //親に戻るかジャンプを行う
        status = MAINSTATUS_CHANGE;
  		}
  		else{
        status = MAINSTATUS_NULL;
   		}
		  break;
    }else{
      if((mpMainHandle == NULL) && (mpNextData==NULL) ){
        status = MAINSTATUS_NULL;
      }
  		break;
    }
	}
  return status;
}


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
bool Manager::End()
{
//#if PM_DEBUG
#if 0
  m_debugIsEndCall = true;
#endif // PM_DEBUG

  // プロセスが残っている場合は、終了処理する
  if( mpMainHandle != NULL || mpNextData != NULL )
  {
    // UpdateFuncはSkipする
    if( mpMainHandle != NULL && mpMainHandle->proc_seq == SEQ_MAIN )
    {
			mpMainHandle->proc_seq = SEQ_END;
			mpMainHandle->my->ResetSubSeq();
    }

    //メイン処理終了するまでまわす
    if ( Main() != MAINSTATUS_NULL )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
 * @brief	プロセス切り替えリクエスト
 * @param	psys
 * @param	procdata
 * @param	pwork
 * @return  生成したProc
 */
//------------------------------------------------------------------
CellSuper* Manager::SetNextProc(CellSuper * process)
{
  GFL_ASSERT( mJump_flag == false );  //@fix
  GFL_ASSERT(mpMainHandle);  //@fix

  if(mpMainHandle==NULL){
    return CallProc(process);
  }
  mJump_flag = true;
  mpNextData = process;
  return mpNextData;
}

//------------------------------------------------------------------
/**
 * @brief	プロセス呼び出し
 * @param	psys
 * @param	procdata
 * @param	pwork
 * @return  生成したProc
 */
//------------------------------------------------------------------
CellSuper* Manager::CallProc(CellSuper * process)
{
  GFL_ASSERT( mCall_flag == false );  //@fix
  mCall_flag = true;
  mpNextData = process;
  return mpNextData;
}

//------------------------------------------------------------------
/**
 * @brief	プロセス呼び出し
 * @param	psys
 * @param	procdata
 * @param	pwork
 * @return  生成したProc
 */
//------------------------------------------------------------------
CellSuper* Manager::ParallelProc(CellSuper * process)
{
  GFL_ASSERT( mParallel_flag == false );  //@fix
  mParallel_flag = true;
  mpNextData = process;
  return mpNextData;
}


//------------------------------------------------------------------
/**
 * @brief	 今動いているBaseProcessの親に当たるBaseProcessを返す
 * @return BaseProcessクラスポインタ 親がなければNULL
 */
//------------------------------------------------------------------

CellSuper* Manager::GetParentBaseProcess(void)
{
  if(mpMainHandle->parent){
    return mpMainHandle->parent->my;
  }
  return NULL;
}


//------------------------------------------------------------------
/**
 * @brief	 今動いているBaseProcessを返す
 * @return BaseProcessクラスポインタ なければNULL
 */
//------------------------------------------------------------------

CellSuper* Manager::GetBaseProcess(void)
{
  if(mpMainHandle){
    return mpMainHandle->my;
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	 次に設定されているBaseProcessを返す
 * @return BaseProcessクラスポインタ なければNULL
 */
//------------------------------------------------------------------

CellSuper* Manager::GetNextBaseProcess(void)
{
  if( mJump_flag ){
    return mpNextData;
  }
  return NULL;
}

// 
//----------------------------------------------------------------------------
/**
 *	@brief  メイン動作中のプロセスの初期化完了チェック
 *
 *	@retval true  初期化完了
 */
//-----------------------------------------------------------------------------
bool Manager::GetBaseProcessInitialized( void ) const
{
  // JumpもCallも実行されている状態で、
  if( !mJump_flag && !mCall_flag ){
    // MAINシーケンスになった。
    if( this->GetMainHandleProcSeq() == SEQ_MAIN ){
      return true;
    }
  }
  return false;
}



} //frame
} //applib

