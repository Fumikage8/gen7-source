#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACONMANAGER_H__
#define __GFLNET_BEACONMANAGER_H__
//=============================================================================
/**
 * @file	 gflnet_BeaconManager.h
 * @brief	 ビーコンマネージャー管理クラス
 * @author k.ohno
 * @date	 2012.5.8
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include "gflnet2/include/beacon/gflnet2_Beacon.h"
#include "gflnet2/include/beacon/gflnet2_BeaconSender.h"
#include "gflnet2/include/beacon/gflnet2_BeaconListener.h"
#include "gflnet2/include/base/gflnet2_InitParam.h"


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

/// ビーコンの保存しておく最大数
#define BEACON_MAX_NUM  100


#define BEACON_TEMP_DOWNCOUNT  (300)

#define TEMP_BEACON_CONNECT_NUM (0xFF)//テンポラリビーコンのダミー接続数

class BeaconManager{
  GFL_FORBID_COPY_AND_ASSIGN(BeaconManager);

private:
  ///< 受信バッファ
  BeaconRecvCell mBeaconRecvData[BEACON_MAX_NUM];
  ///< 最大サイズのビーコン送信データのバックアップ
  BeaconData mBeaconSendData;

  BeaconData mBackupBeaconData;

  ///< 初期化クラスのポインタ
  InitParam* mpInitParam;
  ///< ビーコンリスナー
  BeaconListener* mpBeaconListener;
  ///< ビーコンSender
  BeaconSender* mpBeaconSender;

  int mTempDownCount;
  u64 mTempSetTime;
  u8 m_tempGameID;


private:
  bool SetRecvBeaconDataInternal(int i,const BeaconData* pBeaconData, BeaconRecvData* pRecv);


public:
  BeaconManager();
  ~BeaconManager();


public:
  //ライブラリ用
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンリスナーをセット
   * @param   pBeaconListener クラスのポインタ
   */
  //------------------------------------------------------------------
  void SetBeaconListener( BeaconListener* pBeaconListener);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンSenderをセット
   * @param   pBeaconSender クラスのポインタ
   */
  //------------------------------------------------------------------
  void SetBeaconSender( BeaconSender* pBeaconSender){ mpBeaconSender = pBeaconSender; };
  //------------------------------------------------------------------
  /**
   * @brief	  ライブラリが拾ったビーコンをここに入れる
   * @param   データ
   * @param   サイズ
   */
  //------------------------------------------------------------------
  void SetRecvBeaconData(const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンデータを再送信
   * @param   セットできたらtrue
   */
  //------------------------------------------------------------------
  bool ReloadBeaconData( void );
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンをセット
   * @param   sudId PIAライブラリの差異を今はここで吸収
   */
  //------------------------------------------------------------------
  void SetRecvBeaconData(u8 sudId,const void* pRecvBeacon, size_t size, u8 nowEntry, u8 ConnectAsClient, bit64 lfc);
  void SetBeaconData(BeaconData* pBeaconData, BeaconRecvData* pBeaconRecv);


  void Clear(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンの生存を検査
   */
  //------------------------------------------------------------------
  void MainCheckRecvBeacon(void);

  //------------------------------------------------------------------
  /**
   * @brief	  初期化クラスをセット
   * @param   InitParam
   */
  //------------------------------------------------------------------
  void SetInitParam( InitParam* pParam ){ mpInitParam = pParam; };


  //------------------------------------------------------------------
  /**
   * @brief	  送信するビーコンデータを取得
   * @param   送信ビーコン
   */
  //------------------------------------------------------------------
  void* GetSendBeaconData( void );
  //------------------------------------------------------------------
  /**
   * @brief	  送信するビーコンデータのサイズを取得
   * @param   送信ビーコンサイズ
   */
  //------------------------------------------------------------------
  size_t GetSendBeaconDataSize( void );

public:
  //アプリ用
  //------------------------------------------------------------------
  /**
   * @brief	  受信したビーコンをクラスとして得る そのポインタ
   * @param   ビーコンクラス IN
   * @param   TransferedID
   * @return  あったらtrue なかったらfalse
   */
  //------------------------------------------------------------------
  bool GetBeacon(Beacon* pBeacon,int index);
  //------------------------------------------------------------------
  /**
   * @brief	  受信したビーコンをクラスとして得る そのポインタ
   * @param   ビーコンクラス IN
   * @param   TransferedID
   * @return  あったらtrue なかったらfalse
   */
  //------------------------------------------------------------------
  bool GetBeacon(Beacon* pBeacon, bit64 TransferedID);

  BeaconData* GetBeaconAddress(int i);
  BeaconData* GetBeaconAddress(bit64 TransferedID);
  void ResetBeaconAddress(int i);


  //------------------------------------------------------------------
  /**
   * @brief	  呼び出しビーコンをセットする
   * @param   TransferedID
   * @param   接続人数
   */
  //------------------------------------------------------------------
  bool SetBeacon(const void* pData, int max);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンを追加する
   * @param   command  ビーコンコマンド
   * @param   id       ビーコンID番号
   * @param   pData    送信データ
   * @param   size     送信サイズ
   */
  //------------------------------------------------------------------
  void AddBeaconData(u8 command ,u8 id ,const u8* pData, size_t size);
  void CreateBeaconData(void);


  void BackupBeacon(BeaconData* pBeaconData);
  void RestoreBeacon(BeaconData* pBeaconData);

///  void SetTemporaryBeaconData(void* pBeaconData);
  void SetTemporaryBeaconData(int id,int no,const u8* pBeaconData,int size , int downMsec , u8 tempGameID );
  void TemporaryBeaconUpdate(void);

  void SetTemporaryGameID( u8 tempGameID ){ m_tempGameID = tempGameID; };

private:
  void onBeaconData(void);


};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_BEACONMANAGER_H__






#endif
