#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_BossManager.h
 * @brief  BOSS管理マネージャクラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET_BOSS_MANAGER_H__
#define __GFLNET_BOSS_MANAGER_H__
#pragma once

#include <nn.h>
#include <nn/boss.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <base/include/gfl2_Singleton.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::boss
* @brief     BOSS関連
*/
GFL_NAMESPACE_BEGIN(boss)

//--------------------------------------------------------------------------
class BossFilelistDownloadThread;
class BossFileReadThread;
class BossCheckNewArrivalDataThread;
class BossImmediateTaskThread;
class BossTaskRegisterThread;
//--------------------------------------------------------------------------

/***************************************************************************!
 * @brief  BOSS管理マネージャクラス
 ***************************************************************************/
class BossManager : public gfl2::base::Singleton<BossManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(BossManager); //コピーコンストラクタ＋代入禁止

public:
  //マネージャ初期化パラメータ
   struct BOSS_INIT_PARAM
  {
    BOSS_INIT_PARAM()
    : storageId(INT_MAX)
    , storageSize(ULONG_MAX)
    , entryCount(USHRT_MAX)
    , pBossCode(NULL)
    , pMountName(NULL)
    , checkStorageInterval(UINT_MAX)
    {};//ユーザがパラメータを設定しなかった場合エラーになるように構造体を初期化

    s32 		storageId; 		//!<BOSSで使用する拡張セーブデータ領域のID
    size_t		storageSize;	//!<使用するサイズ（データ受信時に累計がこのサイズ以上になったら、古いものから削除される）
    u16			entryCount;		//!<最大ファイル数(1~2000)（データ受信時にファイル数がこれ以上になったら、古いものから削除される）
    const char*	pBossCode;		//!<BOSSコード（\0で終端されている文字列）
    const char* pMountName;		//!<拡張セーブデータマウント名
    u32			checkStorageInterval;//!<新着データの確認間隔（ミリ秒）0の場合はCheckNewArrivalDataが呼ばれるまで新着データをチェックしません
  };

  /*
    ファイルへのアクセスは↓のようなURLになります。
    https://npdl.cdn.nintendowifi.net/p01/nsa/[BOSSコード]/[タスクID]/[リージョンURL]/[ファイル名]
     [BOSSコード]はNADLタスク利用を申請した際にそのアプリケーション向けに振られるランダムな文字列です。
     [タスクID]は該当するタスクのIDです。タスク登録時に設定します。
    [ファイル名]はダウンロードするファイルの名称になります。
  */

  //タスク登録時パラメータ
  struct TASK_INIT_PARAM
  {
    TASK_INIT_PARAM()
    : pTaskName(NULL)
    , interval(UINT_MAX)
    , count(UINT_MAX)
    , pFileName(NULL)
    {};//ユーザがパラメータを設定しなかった場合エラーになるように構造体を初期化

    const char* pTaskName;		//!<タスク名(char[TASK_NAME_LENGTH(8)])
    u32			interval;		//!<タスク実行間隔（単位はhour）
    u32			count;			//!<タスク実行回数（1~100）
    const char* pFileName;		//!<ダウンロードするファイル名
  };

  //検索条件パラメータ
  /*
   * NSAリストのダウンロード(NsaList::Download)時、NSAリストに含ませるNSAを指定するための検索要素。

    属性１〜３において、NULLでないすべての属性の文字列が、
    サーバ側のNSAの属性の文字列と完全に一致するもののみ、
    ダウンロードされるNSAリストに含まれるようになります。
    例えばattribute1が"A"、attribute2とattribute3がNULLの場合、
    このSearchAttributesを指定してダウンロードしたNSAリストには、
    サーバのNSAで属性１が"A"となっているもののみ含まれるようになります。
    (サーバのNSAの属性２、３の文字列は影響しません。) またattribute1が"A"、attribute2が"B"、attribute3が"C"の場合、
    サーバのNSAで属性１が"A"、属性２が"B"、属性３が"C"となっているNSAのみ、NSAリストに含まれるようになります。
   *
   */
  struct BOSS_SEARCH_ATTRIBUTES
  {
    BOSS_SEARCH_ATTRIBUTES()
    : pAttribute1(NULL)
    , pAttribute2(NULL)
    , pAttribute3(NULL)
    {};//ユーザがパラメータを設定しなかった場合エラーになるように構造体を初期化
    const char* pAttribute1;
    const char* pAttribute2;
    const char* pAttribute3;
  };


  //定数
  enum
  {
    MAX_LISTENER_NUM = 16,									//!<最大登録リスナー数
    TASK_NAME_LENGTH = nn::boss::TASK_ID_LENGTH,			//!<ゼロ終端文字を含むタスク名の最大文字数（8）
    TASK_COUNT_MAX = 100,									//!<設定可能な最大タスク実行回数
    MOUNT_NAME_LENGTH = 32,									//!<拡張セーブデータマウント名サイズ
    THREAD_STACK_SIZE = 8192,								//!<スレッドスタックサイズ
    NSA_LIST_INFO_NUM = 64,									//!<ダウンロード可能なファイルリスト数(220byte * 64)
    TEMP_BUFFER_SIZE = 1000 * 32,							//!<テンポラリバッファサイズ
    SELIAL_ID_LIST_SIZE = TEMP_BUFFER_SIZE/4,				//!<シリアルIDリストサイズ(u8 -> u32なので4分の1)
  };

  /*  [nn::boss内の定数]
    MAX_FILEPATH_LENGTH =  256; 	//ファイルパス最大長
    MAX_URL_LENGTH =  512;			//URL最大長
    TASK_ID_LENGTH =  8;			//ゼロ終端文字を含むタスク名の最大文字数
    MAX_TASK_ID =  128;				//登録可能なタスクIDの最大個数
    MAX_STORAGE_ENTRY_COUNT =  2000;//BOSSストレージ内ファイル数の最大値
    FG_ONLY_TASK_ID[] = "FGONLYT";	//即時実行タスク名
  */

public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  * @param[in] pHeap ヒープ
  */
  //------------------------------------------------------------------
  BossManager();

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~BossManager();

  //------------------------------------------------------------------
  /**
  * @brief  BossManagerの初期化
  * @param[in] initParam 初期化パラメータ
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result Initialize( const BOSS_INIT_PARAM& initParam );

  //------------------------------------------------------------------
  /**
  * @brief  BossManagerの終了処理
  * @param[in] isUnregistStorage	ストレージの登録を解除するかどうか
                  解除すると以降はバックグラウンドでもダウンロードが行われなくなります。
                  通常は解除する必要はありません。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result Finalize( bool isUnregistStorage = false );

  //------------------------------------------------------------------
  /**
  * @brief  使用するヒープを設定（ファイル読み込み等でサイズが大きい為、使用するアプリケーションが設定）
  */
  //------------------------------------------------------------------
  void SetAppHeap( gfl2::heap::HeapBase* pHeapBase );

  //------------------------------------------------------------------
  /**
  * @brief  使用するヒープの設定を解除
  */
  //------------------------------------------------------------------
  void RemoveAppHeap();

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングすること
  *			BossEventListenerに通知されるイベントはすべてこの関数内で実行されます。
  */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
  * @brief  初期化状態を調査
  *
  * @retval true： 初期済み
  * @retval false: 未初期化・または初期化中
  */
  //------------------------------------------------------------------
  bool IsInitialized( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  イベントリスナーの追加（最大16個）
  * @param[in] pListener イベントリスナー
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool AddEventListener( BossEventListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  リスナーを解除
  * @param[in] pListener イベントリスナー
  */
  //------------------------------------------------------------------
  void RemoveEventListener( BossEventListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  新着ダウンロードデータがBOSSストレージに到着しているかチェック
  *
  *	新着データがあった場合OnBossNewArrivalDataイベントで通知されます。
  *
  */
  //------------------------------------------------------------------
  void CheckNewArrivalData();

  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージに存在する全新着ダウンロードデータのヘッダを要求する
  *
  *	結果はOnReadHeaderイベントで取得します。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  //bool RequestNewArrivalDataHeader(); 未実装

  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージに存在する全ダウンロードデータのヘッダを要求する
  *
  *	新着以外のヘッダも要求します。
  *	結果はOnReadHeaderイベントで取得します。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  //bool RequestAllDataHeader(); 未実装

  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージ内に存在するダウンロードデータの本体を要求する
  * @param[in] serialId	データのシリアルID
  * @param[in] threadPriority スレッド優先度
  *
  *	シリアルIDはOnRecvNewDataイベント又は、ヘッダ情報から取得できます。
  *	結果はOnReadDataイベントで取得します。
  *
  */
  //------------------------------------------------------------------
  bool RequestDataBody( const u32 serialId , int threadPriority );

  //------------------------------------------------------------------
  /**
  * @brief  タスクの登録（バックグラウンド実行）
  * @param[in] taskInitParam 登録パラメータ
  *
  *	取得したデータはリスナー経由で受け取ります。
  *	すでに同名のタスクが登録されていた場合、上書きします。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result RegisterTask( int threadPriority , const TASK_INIT_PARAM& taskInitParam );


  //------------------------------------------------------------------
  /**
  * @brief  タスクの登録解除
  * @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result UnregisterTask( const char* pTaskName );

  //------------------------------------------------------------------
  /**
  * @brief  サービスが有効かどうかチェック
  * @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool IsEnableService( const char* pTaskName );

  //------------------------------------------------------------------
  /**
  * @brief  即時実行タスクを開始（フォアグラウンド実行）
  * @param[in] pTaskName タスク名
  * @param[in] pFileName ファイル名
  *
  *	即時実行タスクは同時に１つしか実行できません。
  *	即時実行タスクは1回だけ実行されます。取得したデータはリスナー経由で受け取ります。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result StartImmidiateTask( int threadPriority , const char* pFileName , const char* pTaskName = NULL , const char* pRegionURL = NULL );

  //------------------------------------------------------------------
  /**
  * @brief  即時実行タスクをキャンセル
  *
  *	即時実行タスクをキャンセルし、タスクを破棄します。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result CancelImmidiateTask();

  //------------------------------------------------------------------
  /**
  * @brief  ファイルリストのダウンロードを開始（フォアグラウンド実行）
  * @param[in] threadPriority スレッド優先度
  * @param[in] pTaskName タスク名(省略した場合はFGONRYTが設定される)
  * @param[in] pAttributes 検索条件
  *
  *	ファイルリストダウンロードは1回だけ実行されます。取得したリストはリスナー経由で受け取ります。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool StartDownloadFileList( int threadPriority , const char* pTaskName = NULL , const BOSS_SEARCH_ATTRIBUTES* pAttributes = NULL );

  //------------------------------------------------------------------
  /**
  * @brief  ファイルリストのダウンロードをキャンセル
  *
  *	ファイルリストのダウンロードをキャンセルします。
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result CancelDownloadFileList();

  //------------------------------------------------------------------
  /**
  * @brief  スレッドが起動中かどうか
  *
  * @retval true スレッドが起動中 : false スレッドが起動中ではない
  */
  //------------------------------------------------------------------
  bool IsRunningThread() const;

  //------------------------------------------------------------------
  /**
  * @brief  タスクの動作設定変更
  * @param[in] pTaskName タスク名(char[TASK_NAME_LENGTH(8)])
  * @param[in] interval タスク実行間隔（単位はhour）
  * @param[in] count タスク実行回数（1~100)
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result ChangeTaskConfig( const char* pTaskName , const u32 interval , const u32 count );

  //------------------------------------------------------------------
  /**
  * @brief  登録中のタスク数を取得
  *
  * @retval タスク数
  */
  //------------------------------------------------------------------
  u16 GetRegisterTaskNum( void );

  //------------------------------------------------------------------
  /**
  * @brief  登録中のタスク名を取得
  * @param[in] index インデックス
  *
  * @retval タスク名(引数が無効な場合はNULL)
  */
  //------------------------------------------------------------------
  const char* GetRegisterTaskName( const u16 index );


  //------------------------------------------------------------------
  /**
  * @brief  同名のファイル名かどうかチェック
  * @param[in] index インデックス
  * @param[in] pFileName ファイル名
  *
  * @retval 同名ならtrueそうでないならfalse
  */
  //------------------------------------------------------------------
  bool IsEqualFileName( const u16 index , const char* pFileName );

  //------------------------------------------------------------------
  /**
  * @brief  登録されているストレージの最大サイズを取得
  *
  * @retval ストレージ領域の最大サイズ
  */
  //------------------------------------------------------------------
  size_t GetStorageSizeMax( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  登録されているストレージの最大ファイル数を取得
  *
  * @retval ストレージの最大ファイル数
  */
  //------------------------------------------------------------------
  u16 GetStorageFileNumMax( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  OptOutフラグを設定
  * @param[in] flag		trueを設定すると、NADL タスクの実行時に、NS アーカイブに含まれているおしらせデータは破棄されます。（新着が通知されない）
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result SetOptoutFlag( const bool flag );

  //------------------------------------------------------------------
  /**
  * @brief  設定されているOptOutフラグを取得
  * @param[out] pOutput 結果を受け取る変数
  * @return 実行結果
  */
  //------------------------------------------------------------------
  nn::Result GetOptoutFlag( bool* pOutput ) const;


  //------------------------------------------------------------------
  /**
  * @brief  BOSSストレージのマウント名を取得
  * @return マウント名
  */
  //------------------------------------------------------------------
  const char* GetMountName( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief  BOSSコードを取得
  * @return BOSSコード
  */
  //------------------------------------------------------------------
  const char* GetBossCode( void ) const;

public:

  //------------------------------------------------------------------
  /**
  * @brief  BOSSエラー表示
  */
  //------------------------------------------------------------------
  static bool PrintError( const nn::Result& result );

private://内部処理

  //------------------------------------------------------------------
  /**
  * @brief  新着データをチェック
  */
  //------------------------------------------------------------------
  void CheckNewArrivalDataInternal();

private://イベント通知関連

  //------------------------------------------------------------------
  /**
  * @brief  新着データ発見イベント
  */
  //------------------------------------------------------------------
  void NotifyOnBossNewArrivalData( const u32 serialId , const bool isFinalData );
  //------------------------------------------------------------------
  /**
  * @brief  新着確認をしたがデータが空の時に発行されるイベント
  */
  //------------------------------------------------------------------
  void NotifyOnBossNewArrivalDataEmpty();
  //------------------------------------------------------------------
  /**
  * @brief  データ読み込み完了イベント
  */
  //------------------------------------------------------------------
  bool NotifyOnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody );

  //------------------------------------------------------------------
  /**
  * @brief  ファイルリストダウンロード完了イベント発行
  */
  //------------------------------------------------------------------
  void NotifyBossRecvFileListEvent( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );

  //------------------------------------------------------------------
  /**
  * @brief  ファイルリストダウンロード失敗イベント発行
  */
  //------------------------------------------------------------------
  void NotifyBossRecvFileListEventFailed( const char* pTaskName , const nn::Result& result );

  //------------------------------------------------------------------
  /**
  * @brief  エラーイベント発行
  */
  //------------------------------------------------------------------
  void NotifyOnBossError( const nn::Result& result );

  //------------------------------------------------------------------
  /**
  * @brief  タスクエラーイベント発行
  */
  //------------------------------------------------------------------
  void NotifyOnBossTaskError( const char* pTaskName , const u32 errorCode );


private://スレッド関連

  //-------------------------------------------------------------------------------
  /**
  * @brief 非同期通信が実行中かどうかを調べる
  * @retval true   実行中
  * @retval false  実行していない
  */
  //-------------------------------------------------------------------------------
  bool CheckAsyncInternal( void ) const;

  //-------------------------------------------------------------------------------
  /**
  * @brief 実行中の非同期通信をキャンセルする
  */
  //-------------------------------------------------------------------------------
  void CancelAsyncInternal( void );

  //-------------------------------------------------------------------------------
  /**
  * @brief 非同期処理の終了待ちを行います。
  */
  //-------------------------------------------------------------------------------
  void WaitAsyncFinishedInternal( void );

  //-------------------------------------------------------------------------------
  /**
  * @brief 非同期処理システムを破棄する
  */
  //-------------------------------------------------------------------------------
  void DeleteAsyncInternal( void );

public://デバッグ用関数
  //-------------------------------------------------------------------------------
  /**
  * @brief デバッグ用DNSチェック
  *        開発中のBOSSサーバのテスト配信を受け取る為にはDNS[210.128.103.149]の設定が必要です。
  *        この関数では本体にテスト用DNSが設定されているかどうかチェックします。
  *        設定されていない場合エラー表示も行います。
  *		すでにSocketSystem初期化済みの場合はisInitializeSocketSystemにfalseを設定してください。
  *
  * @param[in] pHeap SocketSystem用ヒープ
  * @param[in] isInitializeSocketSystem SocketSystemの初期化終了を行うかどうか

  * @retval true   DNSに[210.128.103.149]が設定されている
  * @retval false  設定されていない
  */
  //-------------------------------------------------------------------------------
  static bool DebugCheckDNS( gfl2::heap::HeapBase* pHeap , bool isInitializeSocketSystem );

private:
   gfl2::heap::HeapBase* m_pHeapBase;     		//!< ヒープベース
  bool                 m_isInit;            	//!< 初期済みフラグ
  nn::fnd::TimeSpan    m_timeout;           	//!< 非同期処理タイムアウト

  BossFilelistDownloadThread* m_pListDLThread;//!<ダウンロードスレッド
  BossFileReadThread*	m_pFileReadThread;//!<ファイル読み込みスレッド
  BossCheckNewArrivalDataThread* m_pCheckNewArrivalDataThread;//!<新着ファイル取得スレッド
  BossImmediateTaskThread* m_pImmediateTaskThread;//!<即時実行タスクスレッド
  BossTaskRegisterThread* m_pTaskRegisterThread;//!<タスク登録スレッド

  s32					m_storageId;			//!<BOSSで使用する拡張セーブデータ領域のID
  size_t				m_storageSize;			//!<使用するサイズ（データ受信時に累計がこのサイズ以上になったら、古いものから削除される）
  u16					m_entryCount;			//!<最大ファイル数(1~2000)（データ受信時にファイル数がこれ以上になったら、古いものから削除される）
  u32					m_checkStorageInterval;	//!<新着データ確認間隔（ミリ秒）（0の場合は自動的に確認しない）
  s64					m_lastCheckStorageTime;	//!<最後に新着データ確認を実行した時間
  nn::boss::TaskIdList m_taskList;			//!<タスクIDリスト

  bool				m_isRunningImmediateTask;	//!<即時実行タスクが実行中かどうか

  int m_threadPriority;//!<スレッド優先度

  nn::boss::NsaList::NsaInformation* m_pNsaListInfo; //!<ダウンロードしたファイルリスト
  u32*				m_pSerialIDList;//!<新着ファイルシリアルIDリスト

  char				m_bossCode[MOUNT_NAME_LENGTH];//!<BOSSコード（\0で終端されている文字列）
  char				m_mountName[MOUNT_NAME_LENGTH];//!<拡張セーブデータマウント名
  BossEventListener*	m_apEventListener[MAX_LISTENER_NUM];	//!<イベントリスナー
};

GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_BOSS_MANAGER_H__

#endif
