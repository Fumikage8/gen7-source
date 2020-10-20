#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_BossEventListener.h
 * @brief  Boss通知イベントハンドラ用クラス
 * @author shin kosaka
 */
//=============================================================================

#ifndef __GFLNET_BOSS_EVENT_LISTENER_H__
#define __GFLNET_BOSS_EVENT_LISTENER_H__
#pragma once

#include <nn.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(boss)

//------------------------------------------------------------------
/**
 * @brief Bossイベント通知用クラス
 *
 * @note Boss関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class BossEventListener
{
  //GFL_FORBID_COPY_AND_ASSIGN(BossEventListener); //コピーコンストラクタ＋代入禁止
public:
  virtual ~BossEventListener(void){}
public:

  //------------------------------------------------------------------
  /**
  * @brief 新着データ通知イベント
  * @param[in] serialId			データのシリアルID
  *
  *		新着データがBOSSストレージに到着したことを通知するイベントです。
  */
  //------------------------------------------------------------------
  virtual void OnBossNewArrivalData( const u32 serialId , const bool isFinalData ) = 0;
  //------------------------------------------------------------------
  /**
  * @brief  新着確認をしたがデータが空の時に発行されるイベント
  */
  //------------------------------------------------------------------
  virtual void OnBossNewArrivalDataEmpty( void ) = 0;
  //------------------------------------------------------------------
  /**
  * @brief ヘッダ読み込み完了イベント
  *
  * @param[in] pHeader		データヘッダ(NsDataHeaderInfo)
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
  //virtual bool OnBossReadHeader( const nn::boss::NsDataHeaderInfo* pHeader ) = 0; 未実装

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
  virtual bool OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody ) = 0;

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
  virtual void OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief ファイルリストダウンロード失敗通知イベント
  *
  * @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
  * @param[in] result			実行結果
  */
  //------------------------------------------------------------------
  virtual void OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief エラー通知イベント
  *
  * @param[in] result		実行結果
  */
  //------------------------------------------------------------------
  virtual void OnBossError( const nn::Result& result ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief タスクエラー通知イベント
  *
  * @param[in] pTaskName		タスク名（ユーザがタスク登録時に設定したもの）
  * @param[in] errorCode		エラーコード(nn::boss::TaskResultCode)
  */
  //------------------------------------------------------------------
  virtual void OnBossTaskError( const char* pTaskName , const u32 errorCode ) = 0;
};


GFL_NAMESPACE_END(boss)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET_BOSS_EVENT_LISTENER_H__
#endif
