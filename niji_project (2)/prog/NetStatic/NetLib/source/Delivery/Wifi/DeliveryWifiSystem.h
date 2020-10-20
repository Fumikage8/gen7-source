#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWifiSystem.h
 * @brief  Wifi配信システムクラス
 * @author Shin Kosaka
 * @date   2012.8.07
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_WIFI_SYSTEM_H__
#define __GFLNET_DELIVERY_WIFI_SYSTEM_H__

#include <nn.h>

#include <NetStatic/NetLib/source/Delivery/DeliverySystem.h>
#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>
#include <gflnet2/include/boss/gflnet2_BossEventListener.h>
#include <gflnet2/include/wifi/gflnet2_WifiConnectListener.h>
#include <gflnet2/include/boss/gflnet2_BossManager.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
/**
* @namespace NetAppLib::delivery
* @brief     ビーコン配信関連
*/
GFL_NAMESPACE_BEGIN(Delivery)

//--------------------------------------------------------------------------
class DeliveryRecvListener;
//--------------------------------------------------------------------------


/***************************************************************************!
 * @brief  ビーコン配信システムクラス
 ***************************************************************************/
class DeliveryWifiSystem : public NetLib::Delivery::DeliverySystem , public gflnet2::boss::BossEventListener , public gflnet2::wifi::WifiConnectListener
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryWifiSystem); //コピーコンストラクタ＋代入禁止

private:
  enum
  {
    BOSS_FILE_NAME_SIZE = 32,
    PROGRESS_FILELIST_DOWNLOADED = 30,
    PROGRESS_NEW_ARRIVAL_DATA = 60,
    PROGRESS_COMPLETE = 100
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  DeliveryWifiSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam , gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes , DeliveryFileListFilter* pFileListFilter , bool isWifiConnected );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliveryWifiSystem();

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryWifiSystemの初期化
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool Initialize( DeliveryRecvListener* pRecvListener , int threadPriority );

  //------------------------------------------------------------------
  /**
  * @brief  DeliveryWifiSystemの終了処理
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  virtual void Update( DeliveryRecvListener* pRecvListener );


  //------------------------------------------------------------------
  /**
   * @brief  終了準備
   */
  //------------------------------------------------------------------
  virtual void PrepareFinalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  停止確認
   */
  //------------------------------------------------------------------
  virtual bool IsCompretePrepareFinalize( void );

public:

  //------------------------------------------------------------------
  /**
  * @brief  リスナーを削除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void ){ m_pRecvListener = NULL; };

public://Wifiイベント

  virtual void OnWifiConnectSuccess( void );
  virtual void OnWifiConnectFailed( const nn::Result& result, u32 errorCode );
  virtual void OnWifiDisconnectSuccess( void );
  virtual void OnWifiDisconnecFailed( const nn::Result& result, u32 errorCode );



public://BOSSイベント


  //------------------------------------------------------------------
  /**
  * @brief 新着データ通知イベント
  * @param[in] serialId			データのシリアルID
  *
  *		新着データがBOSSストレージに到着したことを通知するイベントです。
  */
  //------------------------------------------------------------------
  virtual void OnBossNewArrivalData( const u32 serialId , const bool isFinalData );
  //------------------------------------------------------------------
  /**
  * @brief  新着確認をしたがデータが空の時に発行されるイベント
  */
  //------------------------------------------------------------------
  virtual void OnBossNewArrivalDataEmpty( void );

  //------------------------------------------------------------------
  /**
  * @brief データ読み込み完了イベント
  *
  * @param[in] pHeader		データヘッダ(NsDataHeaderInfo)
  * @param[in] bodySize    	ボディサイズ
  * @param[in] pBody			データ本体
  * @return 	true : 獲得済みフラグをONにする（以降のリスナーにも通知されません）
  * 			false: 獲得済みフラグをONにしない（以降のリスナーにも通知されます）
  *
  * 			struct NsDataHeaderInfo
  * 			{
  * 			   s64 titleId;					//64ビットのタイトルID
  * 			   bit32 flags;					//NSDのフラグ
  * 	    	   bit32 dataType;				//NSDのデータタイプ
  * 	    	   s32 length;					//NSDの長さ
  * 	    	   u32 serialId;				//NSDのシリアルID
  * 	    	   u32 version;					//NSDのバージョン番号
  * 	    	};
  *
  */
  //------------------------------------------------------------------
  virtual bool OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody );

  //------------------------------------------------------------------
  /**
  * @brief ファイルリストダウンロード完了イベント
  *
  * @param[in] pTaskName		タスク名（ユーザがタスクダウンロード開始時に設定したもの）
  * @param[in] pFileList    	ファイルリスト(nn::boss::NsaList::NsaInformation)
  * @param[in] fileNum    		ファイル数
  *
  * @note pFileListは配列になっています。
  *       pFileList[fileNum-1]までアクセスできます。
  *
  * 		struct NsaInformation
  * 		{
  * 		   char fileName[32];      	//ファイル名(最大31文字)
  * 		   u32 fileSize;			//ファイルサイズ(単位はバイト)
  * 	       u32 updateEpochTime;		//ファイルの更新時間(unix epochであるため、1970-01-01 00:00:00からの経過秒数)。NSAの公開開始日を値を用いるため、ダウンロード時のHTTPレスポンスのLast-Modifiedと一致する値となる。
  * 	       char attribute1[10];		//属性1(最大9文字の文字列)。使用可能な文字は、数値(0-9)、アルファベット(a-z,A-Z)、アンダーバー(_)のみである。サーバ側UIからNSAごとに任意の値を設定できる。NSAリスト取得時にそれに含ませるNSAを指定するための検索要素となる。
  * 	       char attribute2[10];		//属性2(最大9文字の文字列)。使用可能な文字や位置づけは、attribute1 と同様
  * 	       char attribute3[10];		//属性3(最大9文字の文字列)。使用可能な文字や位置づけは、attribute1 と同様
  * 	       u8 caption[150];			//説明文。UTF-8で最長50文字の文字列。サーバ側UIからNSAごとに任意の値を設定できる。
  *		};
  */
  //------------------------------------------------------------------
  virtual void OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );

  //------------------------------------------------------------------
  /**
  * @brief ファイルリストダウンロード失敗通知イベント
  *
  * @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
  * @param[in] result			実行結果
  */
  //------------------------------------------------------------------
  virtual void OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result );

  //------------------------------------------------------------------
  /**
  * @brief エラー通知イベント
  *
  * @param[in] result		実行結果
  */
  //------------------------------------------------------------------
  virtual void OnBossError( const nn::Result& result );

  //------------------------------------------------------------------
  /**
  * @brief タスクエラー通知イベント
  *
  * @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
  * @param[in] errorCode		エラーコード(nn::boss::TaskResultCode)
  */
  //------------------------------------------------------------------
  virtual void OnBossTaskError( const char* pTaskName , const u32 errorCode );


private:
  DeliveryRecvListener* m_pRecvListener;
  int m_threadPriority;
  u32 m_serialId;
  bool m_isWifiConnected;
  bool m_isWifiFinish;
  bool m_isPrepareFinalize;
  bool m_isFileReading;
  nn::os::LightEvent m_wifiStartEvent;
  nn::os::LightEvent m_wifiEndEvent;
  nn::Result m_result;
  char m_fileName[BOSS_FILE_NAME_SIZE];
  u32 m_errorCode;
  gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* m_pAttributes;
  DeliveryFileListFilter* m_pFileListFilter;
  bool m_isAreadyWifiConnected;//すでにWifi接続中なので、接続処理と切断処理をスキップ
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_WIFI_SYSTEM_H__

#endif
