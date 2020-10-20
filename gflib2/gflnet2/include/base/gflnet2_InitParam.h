#ifndef __GFLNET_INITPARAM_H__
#define __GFLNET_INITPARAM_H__
//=============================================================================
/**
 * @file	 gflnet_InitParam.h
 * @brief	 通信初期化構造体アクセサクラス
 * @author k.ohno
 * @date	 2012.3.21
 */
//=============================================================================
#pragma once

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/beacon/gflnet2_BeaconListener.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>
#include <gflnet2/include/p2p/gflnet2_RecvDataListener.h>

//class gflnet2::base::BeaconListener;


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)



                                                      //この文字列を使って通信経路用の暗号鍵を生成します。
typedef u8 TYPE_GAMEID;  ///< ゲームの種類の管理するID たとえば交換とかバトルとか



typedef enum{
  NET_THREAD_NONE,        ///< スレッド休止中
  NET_THREAD_END,         ///< 終了
  NET_THREAD_INITIALIZE,  ///< 通信初期化
  NET_THREAD_DOWNLOAD,    ///< データダウンロード
  NET_THREAD_SCAN,        ///< 親機を探す  単発
  NET_THREAD_SCAN_AND_DELIVER,  ///< 親機を探しつつビーコンの配信を行う
  NET_THREAD_PARENT,      ///< 親になる
  NET_THREAD_CHILD,       ///< 子になって接続を行う
} EN_NET_THREAD;   ///< 役割別の通信スレッド   通信デバイスに頼らない定義 そのかわりにデバイスによっては無いスレッドがある


//-------------------------------------
/// ネットワーク種別定義
//=====================================
typedef enum {
  TYPE_NONE,          ///< タイプなし
  TYPE_ALONE,         ///< 通信しない通信 ライブラリ機能が使えているが通信はしない
  TYPE_P2P_IR,        ///< P2P:赤外線
  TYPE_P2P_WIRELESS,  ///< P2P:ワイヤレス
  TYPE_P2P_WIFI,      ///< P2P:WIFI
} EN_NET_TYPE;


typedef struct{
  bit64 TransferedID;  //自分のID
  gfl2::heap::HeapBase * pHeapBase;  ///< ベースメモリクラス
  gfl2::heap::HeapBase * pHeapDev;   ///< デバイスメモリクラス
  EN_NET_TYPE type;  ///< ネットワーク種別
  TYPE_GAMEID GameID;     ///< 通信別のID ライブラリの外で定義されるべきもの 255まで設計
  u8 LimitNum;    ///< 最大何人接続
  b8 EnableHostMigration;///< マイグレーション有効かどうか
} InitParameter;  ///< 通信初期化パラメータ アプリケーションごとに異なる

typedef struct{
  u32 ProductID;  ///< 任天堂からソフトごとに割り当ててくれるID
  u8 PMVer;       ///< ＲＯＭバージョン番号
  u8 LangCode;    ///< 言語番号
  u8 Version;     ///< 通信バージョン
  u8 padding1;
} InitParameterFixed;  ///< 通信初期化パラメータ ソフトでほぼ共通


class InitParam {
  GFL_FORBID_COPY_AND_ASSIGN(InitParam);

private:
  InitParameter mParam;   ///< 通信初期化パラメータ アプリケーションごとに異なる
  InitParameterFixed mParamFixed;///< 通信初期化パラメータ ソフトでほぼ共通
  bit32 mLocalCommID;
	bool mIsMigration;
  bool mIsMigrationCallBackEnable;

#if GFL_DEBUG
  u8 mDebugMatchmakeNo;  ///< デバッグ用：通信混線防止ID
#endif

public:

  //------------------------------------------------------------------
  /**
   * @brief	  InitParamコンストラクタ＆デストラクタ
   * @return
   */
  //------------------------------------------------------------------
  InitParam(void){};
  virtual ~InitParam(){};

  void Initialize(const InitParameter* pIParam, const InitParameterFixed* pIParamFix);
  
public:

  inline gfl2::heap::HeapBase* GetHeapBase(void){ return mParam.pHeapBase; };
  inline gfl2::heap::HeapBase* GetHeapDev(void){ return mParam.pHeapDev; };
  inline EN_NET_TYPE GetType(void) const { return mParam.type; };
  inline TYPE_GAMEID GetGameID(void) const { return mParam.GameID; };
  inline u8 GetLimitNum(void) const { return mParam.LimitNum; };
  inline u32 GetProductID(void) const { return mParamFixed.ProductID; };
  inline u8 GetPMVer(void) const { return mParamFixed.PMVer; };
  inline u8 GetLangCode(void) const { return mParamFixed.LangCode; };
  inline u8 GetVersion(void) const { return mParamFixed.Version; };
  inline bit32 GetLocalCommunicationId(void) const { return mLocalCommID; };
  inline bit64 GetTransferedID(void) const { return mParam.TransferedID; };
  inline b8 GetEnableHostMigration(void) const { return mParam.EnableHostMigration; };
	inline bool IsMigrationPowerOff(void){ return mIsMigration; };
	inline void SetMigrationPowerOff(bool flg){ mIsMigration = flg; };
  
  //@fix #4127   インターネット通信を介したバトルロイヤル中に親機がLANケーブルの挿抜を行うと子機がはまる【ホストマイグレーション】
  inline bool IsMigrationCallBackEnable(void){ return mIsMigrationCallBackEnable; };
  inline void SetMigrationCallBackEnable(bool flg){ mIsMigrationCallBackEnable = flg; };

#if GFL_DEBUG
  inline void SetDebugMatchmakeNo( u8 matchmakeNo ){ mDebugMatchmakeNo = matchmakeNo; }
  inline u8 GetDebugMatchmakeNo(){ return mDebugMatchmakeNo; }
#endif

#if GFL_DEBUG
  inline void SetGameID( TYPE_GAMEID type ){ mParam.GameID = type; };
#endif

  inline void SetTypeDirect( u32 type ){ mParam.type = (EN_NET_TYPE)type; };

  inline InitParameter* GetInitParam(void){ return &mParam; };
  inline InitParameterFixed* GetInitParamFixed(void){ return &mParamFixed; };

  
  bool IsIr(void);
  bool IsWifi(void);

};

GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_INITPARAM_H__
