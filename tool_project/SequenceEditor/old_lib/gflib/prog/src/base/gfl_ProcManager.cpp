//=============================================================================================
/**
 * @file	gfl_ProcManager.cpp
 * @brief	プロセス管理マネージャー
 * @date	2010.10.18
 */
//=============================================================================================


#include "gfl_base.h"
#include "gfl_debug.h"
#include <ro/gfl_RoManager.h>

namespace gfl {
namespace proc {




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
Manager::Manager(gfl::heap::HeapBase * pBase)
{
	pHeapBase = pBase;
	mainHandle = NULL;
	call_flag = false;
	jump_flag = false;
	draw_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	  プロセス管理システム終了
 */
//------------------------------------------------------------------

Manager::~Manager()
{
#if GFL_DEBUG //============== GFL_DEBUG
  if(mainHandle!=NULL){
    GFL_WARNING("mainHandleが解放されていない\n");
  }
#endif //GFL_DEBUG
}


//------------------------------------------------------------------
// 子プロックに切り替え
//------------------------------------------------------------------
void Manager::entryCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(pHeapBase) processHandle;

  if(mainHandle){
    //親子へ切り替えの場合、MAINでの切り替えというわかりやすい状態にする
    GFL_ASSERT(mainHandle->proc_seq == SEQ_MAIN);  //@fix格下げ

    //親子関係構築
    proc->parent = mainHandle;
    mainHandle->child = proc;
    mainHandle = proc;
  }
  else{
    mainHandle = proc;
  }
  mainHandle->my = next_data;
  //フラグを落す
  draw_flag = false;
  call_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
// プロック移動
//------------------------------------------------------------------
void Manager::jumpCallProc(void)
{
  processHandle* proc;
  proc = GFL_NEW(pHeapBase) processHandle;

  //移動の場合は全部終わった時に行われる
  GFL_ASSERT_STOP(mainHandle->proc_seq == SEQ_UNLOAD);  //@fix

  //親子関係構築
  if(mainHandle->parent){
    proc->parent = mainHandle->parent;
    proc->parent->child = proc;
  }

  deleteMainProc( mainHandle );

  mainHandle = proc;
  mainHandle->my = next_data;

  //フラグを落す
  jump_flag = false;
  next_data = NULL;
}

//------------------------------------------------------------------
// メインプロセスの削除
//------------------------------------------------------------------
void Manager::deleteMainProc( processHandle* proc )
{
#if GFL_ENABLE_RO
  nn::ro::Module* modules[16+1];
  u32             childNum = proc->my->GetChildModulePtrBufferNum();
  //
  gfl::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl::ro::RoManager );

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
void Manager::deleteAndJumpProc(void)
{
  processHandle* proc = mainHandle;

  if(jump_flag){  //ジャンプを行う
    jumpCallProc();
  }
  else if(proc->parent){   //親子関係を戻す
    proc->parent->child = NULL;
    mainHandle = proc->parent;
    deleteMainProc( proc );
  }
  else{  //動く物はなくなったので消す
    deleteMainProc( proc );
    mainHandle = NULL;
  }
  GFL_DELETE proc;
}

//------------------------------------------------------------------
/**
 * @brief	プロセス動作処理メイン
 * @param	proc	プロセスへのポインタ
 * @retval	true	プロセス動作終了
 * @retval	false	プロセス動作継続中
 */
//------------------------------------------------------------------
bool Manager::procMain(void)
{
	Result result;

	switch (mainHandle->proc_seq) {
	case SEQ_LOAD:
		mainHandle->proc_seq = SEQ_INIT;
		/* fall through */
	case SEQ_INIT:
		result = mainHandle->my->InitFunc(this);
		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_MAIN;
			mainHandle->my->ResetSubSeq();
		}
		break;
	case SEQ_MAIN:
		result = mainHandle->my->UpdateFunc(this);
    draw_flag = true;

		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_END;
			mainHandle->my->ResetSubSeq();
		}
		break;
	case SEQ_END:
    draw_flag = false;
		result = mainHandle->my->EndFunc(this);
		if (result == RES_FINISH) {
			mainHandle->proc_seq = SEQ_UNLOAD;
      return true;
		}
		break;
	}
	return false;
}

//------------------------------------------------------------------
/**
 * @brief	PROCの処理
 * 登録したPROCの処理を行います。 適切な位置で呼んでください。
 * 必要な場合は、前後でグラフィックの処理を呼んでください。
 */
//------------------------------------------------------------------
void Manager::Draw(void)
{
  if( draw_flag )
  {
    //draw_flag = false;
    mainHandle->my->DrawFunc(this);
  }
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
  if( mainHandle ){
    return mainHandle->proc_seq;
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
  if( mainHandle ){
    mainHandle->proc_seq = seq;
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
BaseProcess* Manager::GetMainHandleBaseProcess( void ) const
{
  if( mainHandle ){
    return mainHandle->my;
  }
  return NULL;
}

/**
 * @brief 描画フラグをセット
 *
 * @param draw  描画フラグ
 */
void Manager::SetDrawFlag( bool draw )
{
  draw_flag = draw;
}


//------------------------------------------------------------------
/**
 * brief	プロセス管理システムメイン
 * @param	psys
 */
//------------------------------------------------------------------
MainStatus Manager::Main(void)
{

	if( (mainHandle == NULL) && (next_data==NULL) ){
    return MAINSTATUS_NULL;   //動いている物はない
  }

  MainStatus status = MAINSTATUS_VALID;

  if(call_flag) {  // コールする
    entryCallProc();
    status = MAINSTATUS_CALL;
  }

  if ( procMain() ) {
    deleteAndJumpProc();  //親に戻るかジャンプを行う
    status = MAINSTATUS_CHANGE;
  }else{

    if((mainHandle == NULL) && (next_data==NULL) ){
      status = MAINSTATUS_NULL;
    }
  }

  return status;
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
BaseProcess* Manager::SetNextProc(BaseProcess * process)
{
  GFL_ASSERT( jump_flag == false );  //@fix
  GFL_ASSERT(mainHandle);  //@fix

  if(mainHandle==NULL){
    return CallProc(process);
  }
  jump_flag = true;
  next_data = process;
  draw_flag = false;
  return next_data;
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
BaseProcess* Manager::CallProc(BaseProcess * process)
{
  GFL_ASSERT( call_flag == false );  //@fix
  call_flag = true;
  next_data = process;
  draw_flag = false;
  return next_data;
}


//------------------------------------------------------------------
/**
 * @brief	 今動いているBaseProcessの親に当たるBaseProcessを返す
 * @return BaseProcessクラスポインタ 親がなければNULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetParentBaseProcess(void)
{
  if(mainHandle->parent){
    return mainHandle->parent->my;
  }
  return NULL;
}


//------------------------------------------------------------------
/**
 * @brief	 今動いているBaseProcessを返す
 * @return BaseProcessクラスポインタ なければNULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetBaseProcess(void)
{
  if(mainHandle){
    return mainHandle->my;
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	 次に設定されているBaseProcessを返す
 * @return BaseProcessクラスポインタ なければNULL
 */
//------------------------------------------------------------------

BaseProcess* Manager::GetNextBaseProcess(void)
{
  if( jump_flag ){
    return next_data;
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
  if( !jump_flag && !call_flag ){
    // MAINシーケンスになった。
    if( this->GetMainHandleProcSeq() == SEQ_MAIN ){
      return true;
    }
  }
  return false;
}



} //proc
} //gfl
