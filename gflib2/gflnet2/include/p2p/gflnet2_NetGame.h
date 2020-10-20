﻿
#ifndef __GFLNET_NETGAME_H__
#define __GFLNET_NETGAME_H__
#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_NetGame.h
 * @brief	 送信受信を管理するクラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================
#pragma once


#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include "gflnet2/include/p2p/gflnet2_RecvBuffer.h"
#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_RecvDataListener.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  
/**
* @namespace gflnet2::p2p
* @brief     Peer to Peer通信を行う
*/
GFL_NAMESPACE_BEGIN(p2p)


class NetGameBase;
class P2pManager;

class NetGame {
  GFL_FORBID_COPY_AND_ASSIGN(NetGame);

  friend class NetGameBase;


private:
  static const int RECV_HEAD_MAX = 32; //  一度に受け取るコマンド数 一台で３２個
  static const int FINALSYNCNO = 123;  // 終了に使う同期のユニーク番号
  static const int TIMING_MAX = 255;
  static const int TIMING_MIN = 128;

  RecvHead mRecvHead[GFLNET_P2PCONNECT_MAXNUM][RECV_HEAD_MAX];  //受信をヘッダーで管理
  int nowTimingMatchNo;  //全員が送ってきたタイミング番号
  u16 timingNoBackup[GFLNET_P2PCONNECT_MAXNUM]; //タイミング番号Backup
  u16 mTimingNoEnd; //終わりがきたタイミング番号  ここは
  u16 mTimingNo; //タイミング番号
  u16 mTimingNoReady; //タイミング番号準備
  u16 mdmy16; //タイミング番号準備

  u16 mBuffTimingNo;  //便利に一コマンドで終わるための取っておく数

  //  u8 connectFlg[GFLNET_P2PCONNECT_MAXNUM];   //接続をメモリに残しておく(システムの接続に左右されないように自己管理)
  u8 mMutualFlg; //相互通信用のフラグ
  u8 mTimingInterval; // タイミングコマンドを連続させない
  u8 mDmy2; //相互通信用のフラグ
  u8 mDmy3; //相互通信用のフラグ

  static NetGameBase* mpNetGameBase;
  static int mNetGameBaseCount;
  static int mNetGameTimingLoop;

#if GFL_DEBUG
  int debugTimingEnd;
  bool bDebugDelete;
#endif //GFL_DEBUG


private:
  //---------------------------------------------------------------------------------
  /**
  * @brief  ヘッダーをクリアする
  */
  //---------------------------------------------------------------------------------
  void ClearHeader(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  コマンドの解析処理
  * @param  pRecvBuff  受信バッファ管理構造体
  * @return 解析したときのコマンド数
  */
  //---------------------------------------------------------------------------------
  int queueSet(RecvBuffer* pRecvBuff);

  //---------------------------------------------------------------------------------
  /**
  * @brief  ヘッダーを登録する
  * @param  netid    通信ID
  * @param  sysCommand    SYSTEMコマンド
  * @param  gameNo        GAMEクラスの番号
  * @param  targetBit     誰に送るか ８人まで指定可能 0は全員
  * @param  userCommand   ユーザー指定のコマンド
  * @param  size     コマンドのサイズ
  * @param  index    受信バッファのindex
  * @return 登録できたらtrue
  */
  //---------------------------------------------------------------------------------
  bool PushHeader(int netid, u8 sysCommand, u8 gameNo,u8 targetBit,u8 userCommand, u16 size,u16 index);
  //---------------------------------------------------------------------------------
  /**
  * @brief  通信コマンドの引き出し処理（データを引き出し消す）
  * @param  netid   通信ID
  * @param  pHead   受信バッファヘッダ構造体
  * @param  コマンド
  * @return 引き出したら true
  */
  //---------------------------------------------------------------------------------
  bool queuePop(int netid, RecvHead* pHead,u8 sysCommand);
  //---------------------------------------------------------------------------------
  /**
  * @brief  タイミングコマンドの解析処理
  * @param  pRecvBuff  受け取ったデータの管理構造体
  */
  //---------------------------------------------------------------------------------
  RecvHead* commandCheck(int netid,u8 command);
  //---------------------------------------------------------------------------------
  /**
  * @brief  コマンドの消去処理
  * @param  netid     通信ID
  * @param  command   コマンド
  */
  //---------------------------------------------------------------------------------
  void commandDelete(int netid,u8 command);
  //---------------------------------------------------------------------------------
  /**
  * @brief  タイミングコマンドの解析処理
  * @param  netid   通信ID
  */
  //---------------------------------------------------------------------------------
  void timingCheck(int netid ,RecvBuffer* pRecv);

  //---------------------------------------------------------------------------------
  /**
  * @brief  親機でのタイミングコマンドの解析処理
  * @param  pRecvBuff  受け取ったデータの管理構造体
  * @return bool      タイミングがそろったらtrue
  */
  //---------------------------------------------------------------------------------
  bool parentTimingCheck(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  タイミング終了の到着チェック
  * @param  pRecvBuff  受け取ったデータの管理構造体
  * @return bool      タイミングがそろったらtrue
  */
  //---------------------------------------------------------------------------------
  bool endTimingCheck(int index , RecvBuffer* pRecv);

  //---------------------------------------------------------------------------------
  /**
  * @brief  データ送信 内部用
  * @param  command   ヘッダー
  * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
  * @param  size      送信サイズ
  * @return bool      送る準備ができたらtrue だめならfalse
  */
  //---------------------------------------------------------------------------------
  bool sendDataLocal(u8 sysCommand,u8 targetBit,u8 userCommand, char* pSendData, int size);

  //---------------------------------------------------------------------------------
  /**
  * @brief  動かす処理 1/30 か 1/60でまわす必要がある
  */
  //---------------------------------------------------------------------------------
  void Func(gflnet2::p2p::RecvBuffer* mRecv[GFLNET_P2PCONNECT_MAXNUM]);

public:
  //---------------------------------------------------------------------------------
  /**
  * @brief  コンストラクタ&デストラクタ
  */
  //---------------------------------------------------------------------------------
  NetGame( gflnet2::InitParam* pInit );
  NetGame();
  virtual ~NetGame();

  //---------------------------------------------------------------------------------
  /**
  * @brief  初期化
  */
  //---------------------------------------------------------------------------------
  void Initialize(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  初期化できたかどうか
  * @return 初期化できたらtrue
  */
  //---------------------------------------------------------------------------------
  bool IsInitialize(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  終了   必ず終了します
  */
  //---------------------------------------------------------------------------------
  void Finalize(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  終了同期開始
  */
  //---------------------------------------------------------------------------------
  void FinalizeSync(void);
  //---------------------------------------------------------------------------------
  /**
  * @brief  終了同期待ち
  */
  //---------------------------------------------------------------------------------
  bool IsFinalizeSync(void);

  //---------------------------------------------------------------------------------
  /**
  * @brief  電源OFF強制終了前準備（NetGameBaseをDeleteしないとNexメモリリークにひっかかる）
  */
  //---------------------------------------------------------------------------------
  static void PreparePowerOff(void);

  //---------------------------------------------------------------------------------
  /**
   * @brief  データ送信
   * @param  sendData  送るデータ
   * @param  size      送信サイズ
   * @return bool      送る準備ができたらtrue だめならfalse
   */
  //---------------------------------------------------------------------------------
  bool SendData(char* sendData, int size);

  //---------------------------------------------------------------------------------
  /**
   * @brief  コマンド付きデータ送信
   * @param  sendData  送るデータ
   * @param  size      送信サイズ
   * @param  command   コマンドNO
   * @return bool      送る準備ができたらtrue だめならfalse
   */
  //---------------------------------------------------------------------------------
  bool SendDataCommand( char* sendData,  int size,  u8 command);
  //---------------------------------------------------------------------------------
  /**
   * @brief  コマンド付きデータ送信 送信指定BITつき
   * @param  sendData  送るデータ
   * @param  size      送信サイズ
   * @param  command   コマンドNO
   * @param  playerBit   送る先のBIT
   * @return bool      送る準備ができたらtrue だめならfalse
   */
  //---------------------------------------------------------------------------------
  bool SendDataCommandSendBit( char* sendData,  int size,  u8 command, u8 playerBit);
  //---------------------------------------------------------------------------------
  /**
   * @brief  データ受信    ※必ずfalseが返ってくるまで読み込む事
   * @param  id        送ってきた人の通信ID
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   * @return bool      データがあればtrue なければfalse
   */
  //---------------------------------------------------------------------------------
  bool RecvData(int id, char* pRecvData, int size);
  //---------------------------------------------------------------------------------
  /**
   * @brief  データ受信    ※必ずfalseが返ってくるまで読み込む事
   * @param  id        送ってきた人の通信ID
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   * @param  pSize     受け取ったサイズを返す
   * @return bool      データがあればtrue なければfalse
   */
  //---------------------------------------------------------------------------------
  bool RecvData(int id, char* pRecvData, int size, u16* pSize);

  //---------------------------------------------------------------------------------
  /**
   * @brief  コマンド付きデータ受信    ※必ずfalseが返ってくるまで読み込む事
   * @param  id        送ってきた人の通信IDを受けとる
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   * @param  pCommand  コマンド番号を受け取る場所
   * @return bool      データがあればtrue なければfalse
   */
  //---------------------------------------------------------------------------------
  bool RecvDataCommand(int* pId, char* pRecvData, int size , u8* pCommand, u16* pSize = NULL);
  //---------------------------------------------------------------------------------
  /**
   * @brief  データ受信  コマンド指定版    ※必ずfalseが返ってくるまで読み込む事
   * @param  id        送ってきた人の通信ID
   * @param  RecvData  受け取るバッファ
   * @param  size      受信バッファサイズ これ以上は受け取りません
   * @param  getCommand  ほしいユーザーコマンド
   * @param  pSize     受け取ったサイズを返す
   * @return bool      データがあればtrue なければfalse
   */
  //---------------------------------------------------------------------------------
  bool RecvDataCommandLocalCommand(int* pId, char* pRecvData, int size , u8 getCommand, u16* pSize);

  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングをそろえるコマンドの送信
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * ※必ず送るので戻り値は無いですが 同じ番号を連続したりすると挙動がおかしくなります
   * 通信の遷移をしっかり使う側で把握してください
   */
  //---------------------------------------------------------------------------------
  void TimingStart(int timingUniqueNo);
  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングをそろえるコマンドが到着したかどうか
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
   */
  //---------------------------------------------------------------------------------
  bool IsTimingEnd(int timingUniqueNo);

  //---------------------------------------------------------------------------------
  /**
   * @brief  タイミングをそろえるコマンドを発行確認まで内部でやってくれる
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
   */
  //---------------------------------------------------------------------------------
  bool IsTimingLoopCheck(int timingUniqueNo);
  //---------------------------------------------------------------------------------
  /**
   * @brief   コネクションした人数を得る
   * @return  コネクション人数
   */
  //---------------------------------------------------------------------------------
  int GetConnectionCount(void);

  //---------------------------------------------------------------------------------
  /**
   * @brief   接続順番 = NetIDをかえす
   * @return  NetID
   */
  //---------------------------------------------------------------------------------
  int GetNetID(void);

  static inline NetGameBase* GetNetGameBase(void){ return mpNetGameBase; }


  static bool IsMaster(void);

  //--------------------------------------------------------------------------
  /**
   * @brief		同期NOのインクリメント
   */
  //--------------------------------------------------------------------------
  void IncTimingNo( void );


	//--------------------------------------------------------------------------
  /**
	 * @brief		同期NOのバックアップをクリア
   */
  //--------------------------------------------------------------------------
	void ClearTimingBackup( void );

	//--------------------------------------------------------------------------
  /**
	 * @brief		すでに壊れているかどうか
   */
  //--------------------------------------------------------------------------
  bool CheckBloken(void);


  void SetRecvDataListener(gflnet2::p2p::RecvDataListener* pR);


  bool IsDisconnectNetID( int netID );
};


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif // GF_PLATFORM_CTR

//↓WIN用ダミー定義
#if defined(GF_PLATFORM_WIN32)
#include <gflnet2/include/base/gflnet2_InitParam.h>
namespace gflnet2
{

  namespace p2p
  {
    typedef int GameInitialize;
    typedef int NetGameBase;
    typedef int RecvDataListener;
    class NetGame 
    {
      GFL_FORBID_COPY_AND_ASSIGN(NetGame);
    public:
      NetGame( gflnet2::InitParam* pInit){}
      NetGame(){};
      virtual ~NetGame(){}
      void Initialize(void){};
      bool IsInitialize(void){return true;}
      void Finalize(void){};
      void FinalizeSync(void){};
      bool IsFinalizeSync(void){return true;}
      bool SendData(char* sendData, int size){return true;}
      bool SendDataCommand( char* sendData,  int size,  u8 command){return true;}
      bool SendDataCommandSendBit( char* sendData,  int size,  u8 command, u8 playerBit){return true;}
      bool RecvData(int id, char* pRecvData, int size){return false;}
      bool RecvData(int id, char* pRecvData, int size, u16* pSize){return false;}
      bool RecvDataCommand(int* pId, char* pRecvData, int size , u8* pCommand, u16* pSize = NULL){return false;}
      bool RecvDataCommandLocalCommand(int* pId, char* pRecvData, int size , u8 getCommand, u16* pSize){return false;}
      void TimingStart(int timingUniqueNo){};
      bool IsTimingEnd(int timingUniqueNo){return true;}
      bool IsTimingLoopCheck(int timingUniqueNo){return true;}
      int GetConnectionCount(void){return 2;}
      int GetNetID(void){return 0;}
      static NetGameBase* GetNetGameBase(void){ return NULL; }
      static bool IsMaster(void){return true;}
      void IncTimingNo( void );
      void ClearTimingBackup( void ){}
      bool CheckBloken(void){return false;}
      void SetRecvDataListener(gflnet2::p2p::RecvDataListener* pR){}
      bool IsDisconnectNetID( int netID ){ return false; }
    };      
  }//namespace p2p
}//namespace gflnet
#endif

#endif // __GFLNET_NETGAME_H__