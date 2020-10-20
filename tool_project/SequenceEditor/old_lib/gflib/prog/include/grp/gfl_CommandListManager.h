#ifndef __GFL_COMMANDLISTMANAGER_H__
#define __GFL_COMMANDLISTMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_CommandListManager.h
 *	@brief  コマンドリストマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once


#include <nn.h>
#include <nw.h>

#include <grp/gfl_GraphicsType.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>


namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define GFL_COMMAND_LIST_DEBUG_USE (1)  // 製品版でも機能を使いたいシチュエーションのために別定義にする。

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	コマンドリストセットアップデータ
//=====================================
typedef struct {
  u32 bufferSize;     // バッファサイズ（コマンドバッファの総サイズ）
  u32 queueingCount;  // キューイングコマンド数（バッファの中に何個コマンドをいれるか）
} CommandListSetUp;



//-------------------------------------
/// コマンドリストID
//=====================================
typedef u32 CommandListID;
const CommandListID COMMANDLIST_ID_ERROR = 0xffffffff;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class CommandList;

//-------------------------------------
///	コマンドリストマネージャ
//=====================================
class CommandListManager{

  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(CommandListManager);

public:
  //-----------------------------------------------------------------------------
  /**
   *      コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  CommandListManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~CommandListManager();

  //----------------------------------------------------------------------------
  /**
   *	@brief    コマンドリスト生成
   *
   *	@param	セットアップデータ
   *
   *	@retval コマンドリストID
   *	@retval COMMANDLIST_ID_ERROR  コマンドリストを生成できなかった。
   */
  //-----------------------------------------------------------------------------
  CommandListID CreateBuffer( const CommandListSetUp& setup );

  //----------------------------------------------------------------------------
  /**
   *	@brief    コマンドリスト破棄
   *
   *	@param	id  コマンドリストID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドを格納していくCommandListを指定
   *
   *	@param	id  コマンドリストID
   *
   *	*ここでバインドしたコマンドリストがカレントコマンドリストとなります。
   */
  //-----------------------------------------------------------------------------
  void Bind( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドの内容をクリアする
   *
   *	@param	id  コマンドリストID
   *
   *	＊実行後カレントコマンドリストを再バインドします。
   */
  //-----------------------------------------------------------------------------
  void Clear( CommandListID id );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  実行するCommandListを指定
   *
   *	@param	id  コマンドリストID
   *
   *	＊実行後カレントコマンドリストを再バインドします。
   */
  //-----------------------------------------------------------------------------
  void RunAsync( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  実行が完了するまで待つ
   *
   *	＊実行後カレントコマンドリストを再バインドします。
   */
  //-----------------------------------------------------------------------------
  void WaitDone( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンド実行中かチェック
   *	@retval true    コマンド実行中
   *	@retval false   コマンド実行していない
   */
  //-----------------------------------------------------------------------------
  bool IsRuning( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    実行しているコマンドIDを取得
   *
   *	@return 実行しているコマンドID
   */
  //-----------------------------------------------------------------------------
  CommandListID GetRuningCommandID(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    実行したコマンドIDを取得
   *
   *	@retval 実行したコマンドID
   *  @retval COMMANDLIST_ID_ERROR 実行中
   */
  //-----------------------------------------------------------------------------
  CommandListID GetRanCommandID(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドリストへのコマンドセーブ　開始
   *
   *	@param	コマンドをセーブするコマンドリストID
   *
   *	*idのコマンドリストにたまっているコマンドをクリアするには、通常と同様Clear関数を使用してください。
   */
  //-----------------------------------------------------------------------------
  void StartCommandSave( CommandListID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドリストへのコマンドセーブ　終了
   *	*この関数内で、Save前にバインドされていたコマンドリストを再バインドします。
   */
  //-----------------------------------------------------------------------------
  void EndCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  コマンドリストへのコマンドセーブ　終了  セーブ情報取得バージョン
   *	*この関数内で、Save前にバインドされていたコマンドリストを再バインドします。
   *
   *	@param  p_buffoffset  セーブ開始バッファオフセット
   *	@param  p_buffsize    セーブバッファサイズ
   *	@param  p_reqid       セーブ開始リクエストID
   *	@param  p_reqsize     セーブリクエストサイズ
   *	@param  split_cmd     SplitCmd
   */
  //-----------------------------------------------------------------------------
  void EndCommandSaveEx( u32* p_buffoffset, s32* p_buffsize, u32* p_reqid, s32* p_reqsize );

  //----------------------------------------------------------------------------
  /**
   *	@brief  1つ前に保存したコマンドを今バインドされているコマンドリストに追加する。
   *
   *	@param	save_id    保存コマンドリストID
   *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
   *	*コピーすると、save_id側のコマンドをクリアすることが出来ますが、
   *	　コピーにCPUの負荷がかかります。
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSave( CommandListID save_id, bool is_copy );

  //----------------------------------------------------------------------------
  /**
   *	@brief  保存したコマンドを今バインドされているコマンドリストに追加する。
   *
   *	@param	save_id    保存コマンドリストID
   *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
   *	*コピーすると、save_id側のコマンドをクリアすることが出来ますが、
   *	　コピーにCPUの負荷がかかります。
   *
   *	@param  buffoffset  バッファオフセット
   *	@param  buffsize    バッファサイズ
   *	@param  reqid       リクエストID
   *	@param  reqsize     リクエストサイズ
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSaveEx( CommandListID save_id, bool is_copy, u32 buffoffset, s32 buffsize, u32 reqid, s32 reqsize );

  //----------------------------------------------------------------------------
  /**
   *	@brief  セーブ中かチェック
   *
   *	@retval true    セーブ中
   *	@retval false   していない
   */
  //-----------------------------------------------------------------------------
  bool IsSaving( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    セーブしたコマンドではなく、コマンドリストの内容を全て、カレントのコマンドの後ろに蓄積させる。
   *
   *	@param	add_id    蓄積するコマンドリストID
   *	@param  is_copy    コマンドリストをコピーするのか、それともリンクをはるのか
   *	*コピーすると、save_id側のコマンドをクリアすることが出来ますが、
   *	　コピーにCPUの負荷がかかります。
   */
  //-----------------------------------------------------------------------------
  void AddCommand( CommandListID add_id, bool is_copy );


  //----------------------------------------------------------------------------
  /**
   *	@briefバッファの状態を取得
   *
   *	@param	p_usebuff   バッファの使用量
   *	@param	p_usereq    リクエストカウントの使用量
   *	@param	p_restbuff  バッファ残りサイズ
   *	@param	p_restreq   リクエストカウントバッファ残り数
   */
  //-----------------------------------------------------------------------------
  void GetBufferStatus( CommandListID id, u32* p_usebuff, u32* p_usereq, u32* p_restbuff, u32* p_restreq );


  //----------------------------------------------------------------------------
  /**
   *	@brief    バインドされているバッファの状態をログに出力
   */
  //-----------------------------------------------------------------------------
  void Dump( void ) const;

#if GFL_COMMAND_LIST_DEBUG_USE
  //-----------------------------------------------------------------------------
  /**
   *      デバッグ機能　使用方法
   *      1.全デバッグ機能を有効にする。
   *        gfl::grp::CommandListManager::DebugSetFlag( true )
   *        
   *        この関数にtrueを渡すことで、以下の関数が使用できるようになります。
   *        DebugSetCommandEndCallback()
   *        DebugGetDoneMs()
   *        DebugGetWaitDoneMs()
   *        
   *      2.ユーザーコールバックを使用する。
   *        gfl::grp::CommandListManager::DebugSetCommandEndCallback()
   *        コールバック関数とユーザーワークを渡すことで、
   *        コマンドリストの実行が完了したタイミング発生するコールバックを設定できます。
   *
   *      3.処理時間をチェックする。
   *      　コマンドリストの処理時間
   *        gfl::grp::CommandListManager::DebugGetDoneMs()
   *
   *        CPUの処理が完了から、GPUの処理完了までにかかった時間
   *        （GPUの完了をCPUが待っている時間）
   *        gfl::grp::CommandListManager::DebugGetWaitDoneMs()
   *
   *
   * 　　　　＊処理時間の取得は、、
   * 　　　　　以下のタイミングで行ってください。
   *        　GraphicsSystem::WaitVSync();
   *        　->この間で、取得してください。
   *        　GraphicsSystem::VSyncAfter();
   *
   *      4.蓄積されているコマンド情報を取得する
   *        コマンドリクエスト数
   *        gfl::grp::CommandListManager::DebugGetUseReqCount()
   *
   *        コマンド蓄積情報の総バイト数
   *        gfl::grp::CommandListManager::DebugGetUseBuffSize()
   *
   *      5.GPUコマンドの特定のタイミングで文字列を描画する。
   *        登録できる文字列は8個まで。
   *        gfl::grp::CommandListManager::DebugPutGpuDebugPrint()
   *        
   */
  //-----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　デバッグ機能のON・OFF
   *
   *	@param	flag    true:ON　　false:OFF
   */
  //-----------------------------------------------------------------------------
  static void DebugSetFlag( bool flag );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　GPU側の処理完了コールバック　ON・OFF
   *
   *	@param	func  関数ポインタ     （OFF：NULL)
   *	@param  work  ユーザーワーク    （OFF：NULL）
   */
  //-----------------------------------------------------------------------------
  static void DebugSetCommandEndCallback(void(*func)( void* ) = NULL, void* work = NULL );

  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　１つ前のコマンド実行にかかった処理時間
   *
   *  @retval マイクロ秒
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetDoneMs(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　１つ前のコマンド実行でGPUの処理待ちをした時間
   *
   *  @retval WaitDone処理をまったマイクロ秒
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetWaitDoneMs(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　実行コマンドリストに蓄積されていコマンド数
   *
   *  @retval コマンド数
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetUseReqCount(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  デバッグ用　実行コマンドリストに蓄積されているコマンド総バイトサイズ
   *
   *  @retval WaitDone処理をまったマイクロ秒
   */
  //-----------------------------------------------------------------------------
  static s32 DebugGetUseBuffSize(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief  ここまでに格納されているコマンドが実行されたら表示されるデバッグプリント
   *	　　　　を登録
   *
   *	@param	str   文字列
   */
  //-----------------------------------------------------------------------------
  static void DebugPutGpuDebugPrint( const char* str );
#endif  // GFL_COMMAND_LIST_DEBUG_USE

private:

  CommandList*  m_array;
  u32           m_arrayMax;

  CommandListID m_saveCommandID;
  CommandListID m_runCommandID;
  CommandListID m_bindCommandID;

  bool          m_runing;   // 実行中
  bool          m_saving;   // セーブ中
  
#if GFL_COMMAND_LIST_DEBUG_USE
  static bool m_debug_flag;             // コマンドリストデバッグフラグ
  static s64  m_debug_commandwait_ms;   // コマンド実行待ち時間（マイクロ秒）
  static s64  m_debug_commandDoneMs;    // コマンド実行にかかった時間（マイクロ秒）
  static s64  m_debug_commandStartDoneMs;// コマンド実行を開始した時間（チック時間）
  static void (*m_debug_commandCallbackFunc)(void*);  // コマンド完了コールバック関数ポインタ
  static void* m_debug_commandCallbackWork;  // コマンド完了コールバックに渡すユーザーワーク
  static bool m_debug_commandCallbackFlag; // コールバックが呼ばれることを待つ為のフラグ
  static s32  m_debug_commandUseReqCount; // 実行中のコマンド蓄積数
  static s32  m_debug_commandUseBuffSize;  // 実行中のコマンドバッファ蓄積サイズ
  static const int DEBUG_GPU_PRINT_BUF_MAX = 8;
  static const char* m_debug_gpuPrintBuf[2][DEBUG_GPU_PRINT_BUF_MAX];  // GPUデバッグ出力　文字列バッファ
  static s32  m_debug_gpuPrintBuffIndex;  //GPUデバッグ出力　ダブルバッファ用インデックス
  static s32  m_debug_gpuPrintBuffCount;  //GPUデバッグ出力　バッファ格納カウンタ
  static s32  m_debug_gpuPrintBuffPrintCount;  //GPUデバッグ出力　バッファ出力カウンタ
  static s32  m_debug_gpuPrintBuffPrintCountMax;  //GPUデバッグ出力　バッファ出力カウンタ最大値

#endif  // GFL_COMMAND_LIST_DEBUG_USE

private:

  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize( void );
  CommandListID GetClearBuffer( void ) const;
  void BindCurrentCommandList( void );
  
#if GFL_COMMAND_LIST_DEBUG_USE
  static void DebugCommandEndCallback(GLint id);
#endif  // GFL_COMMAND_LIST_DEBUG_USE
} ;




} /* namespace gfl */
} /* namespace grp */
#endif // __GFL_COMMANDLISTMANAGER_H__
