﻿#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_RECVBUFFER_H__
#define __GFLNET_RECVBUFFER_H__
//=============================================================================
/**
 * @file	 gflnet_RecvBuffer.h
 * @brief	 受信バッファクラス
 * @author k.ohno
 * @date	 2011.6.9
 */
//=============================================================================
#pragma once

#include <nex.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2) 
GFL_NAMESPACE_BEGIN(p2p)

#define GFLNET_TYPE_NORMAL  (0x02) // 通常
#define GFLNET_TYPE_VCT     (0x01) // VCTデータ


#define GFLNET_NO_DATA  (0x00)  // データが無い
#define GFLNET_NORMAL_DATA  (0x01)  // 一般のデータ
#define GFLNET_TIMEST_DATA  (0x02)  // TIMINGSYNC
#define GFLNET_TIMEED_DATA  (0x03)  // TIMINGEND
#define GFLNET_CONNECTION_DATA  (0x04)  // コネクションIDとその番号を親から子全員に通達
#define GFLNET_START_GAME_DATA  (0x05)  // ゲーム開始通達

//#define GFLNET_REP_DATA  (0x0400)  // データ到着の返事を返す

//#define GFLNET_ALL_DATA     (0xff000000)  // 全部のコマンドをさす
//#define GFLNET_MASK_COMMAND (0xff000000)  //コマンド部分のマスク
//#define GFLNET_MASK_NETGAMENO (0x00ff0000)  //ネットゲームINDEX部分のマスク
//#define GFLNET_MASK_USER_COMMAND (0x000000ff)  //ユーザーが指定するコマンド部分のマスク
//#define GFLNET_MASK_USER_COMMAND (0x0000ff00)  //ユーザーが指定するコマンド部分のマスク


struct SendHead{  //２０バイト
  u16 CRC;
  u16 size;
  u32 incno;
  u8 RecvConnectionID[GFLNET_P2PCONNECT_MAXNUM];  //受け取ったコネクションIDの下位１バイト
  u8 RecvInc[GFLNET_P2PCONNECT_MAXNUM];  //受け取ったincnoの下位1バイト
  u8 netver;  //通信バージョン
  u8 type;   //  0:通常   1:ボイスチャット
  u8 dummy1;
  u8 dummy2;
};

#define COMMAND_ALL_HEADER_SIZE (sizeof(SendHead))  // SendHead

struct SendCommand{  //送信時のコマンドサイズ
  u8 sysCommand;   // SYSTEMコマンド
  u8 gameNo;    // GAMEクラスの番号
  u8 targetBit; // 誰に送るか ８人まで指定可能 0は全員
  u8 userCommand;      // ユーザー指定のコマンド
  u16 size;  //サイズ
  u16 dummy;
};

#define COMMAND_MINI_HEADER_SIZE (sizeof(SendCommand))  //コマンド + サイズ


struct RecvHead{  ///< 受信した時のヘッダー
  u8 sysCommand;   // SYSTEMコマンド
  u8 gameNo;    // GAMEクラスの番号
  u8 targetBit; // 誰に送るか ８人まで指定可能 0は全員
  u8 userCommand;      // ユーザー指定のコマンド
  u16 size;         ///< サイズ
  u16 index;    ///< リングバッファ上のインデックス
};


class RecvBuffer {
  GFL_FORBID_COPY_AND_ASSIGN(RecvBuffer);
public:

  static const int RECV_RING_MAX = GFLNET_P2PCONNECT_MAXPACKET*10;  //１０個分のリングバッファ
  //  static const int RECV_SINGLE_MAX = 1024;  //一個のコマンドの最大

  u8 mRingBuffer[RECV_RING_MAX];
  int recvHeadIndex;
//  int buffsize;  //RECVにはいっているサイズ 0でデータ無し
  int ringNoSt;
  int ringNoStMax;

  int ringNoEd;
  int ringNoEdMax;

  nn::nex::ConnectionID sourceId;  //これで関連付けを行う

  RecvBuffer(void){};
  virtual ~RecvBuffer(){};


  void Clear(void)
    {
      recvHeadIndex=0;
      ringNoSt=0;
      ringNoStMax=0;
      ringNoEd=0;
      ringNoEdMax=0;
      sourceId=0;  //これで関連付けを行う
    }

  //リングバッファにデータを蓄える
  bool pushBuffer(char* pData, int size)
    {
      int i;
      for(i=0;i<size;i++){
        mRingBuffer[ringNoSt] = pData[i];
        ringNoSt++;
        ringNoSt = ringNoSt % RECV_RING_MAX;
      }
      ringNoStMax += size;
      ringNoEdMax += size;
      if(ringNoStMax >=RECV_RING_MAX){
        GFL_ASSERT(0);   //@fix
        return false;  //バッファオーバーエラー
      }
      return true;
    }

  //リングバッファからヘッダーを形成するので読み進める
  void incBuffer(int size)
    {
      ringNoEd += size;
      ringNoEd = ringNoEd % RECV_RING_MAX;
      ringNoEdMax -= size;
      GFL_ASSERT(ringNoEdMax>=0); //@fix
    }

  //データがあるかどうかを確認
  bool isBuffer(void)
    {
      if( ringNoEdMax == 0){
        return false;
      }
      return true;
    }
  //リングバッファからデータを消す
  bool refleshBuffer(int size)
    {
      ringNoStMax -= size;
      if(ringNoStMax < 0){
        return false;  //リング以上に読み込んだ場合
      }
      return true;
    }


  //u16にみたてたよみこみ
  u16 readBuffer16(int index)
    {
/*      int i = (ringNoEd + (index*2)) % RECV_RING_MAX;  //不具合調査の為コメント
      u16* pU61 = reinterpret_cast<u16*>(&mRingBuffer[i]);
      return pU61[0];*/

      u16 num=0;
      int start = index*2;
      num = readBuffer8(start);
      num += readBuffer8(start+1)*0x100;
      return num;
    }

  //u8にみたてたよみこみ
  u8 readBuffer8(int index)
    {
      int i = (ringNoEd + (index)) % RECV_RING_MAX;
      u8* pU8 = reinterpret_cast<u8*>(&mRingBuffer[i]);
      return pU8[0];
    }

  //u16にみたてたIndex
  u16 readBuffer16Index(int index)
    {
      int i = (ringNoEd + (index*2)) % RECV_RING_MAX;
      return i;
    }

  void dataCopy(u16 index, char* pRecvData, int size)
    {
      int i ;
      for(i=0;i<size;i++){
        pRecvData[i] = mRingBuffer[((index+i)%RECV_RING_MAX)];
      }
    }

  // ConnectionIDを得る
  inline nn::nex::ConnectionID GetConnectionID(void){ return sourceId; }
  // ConnectionIDを設定
  inline void SetConnectionID(nn::nex::ConnectionID id){ sourceId = id; }


  
};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_NETGAME_H__

#endif
