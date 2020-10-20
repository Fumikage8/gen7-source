#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_BEACON_H__
#define __GFLNET_BEACON_H__
//=============================================================================
/**
 * @file	 gflnet_Beacon.h
 * @brief	 ビーコンのヘッダーの管理クラス
 * @author k.ohno
 * @date	 2012.5.7
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <nn/uds.h>
#include <pia/local/local_UdsNetwork.h>

#include <gflnet2/include/gflnet2_NetworkDefine.h>
#include <gflnet2/include/beacon/gflnet2_BeaconCommandIterator.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)

typedef u16 GFLNET_CRC_TYPE;


#define BEACONTYPE_MATCH (1)  //PIAがビーコンの形を変えているので識別子
#define BEACONTYPE_EACH (2)
#define BEACONTYPE_AUTOMATCH (3)



//この構造体はメンバーの位置を変更してはいけない k.ohno 2011.8.24
typedef struct{
  GFLNET_CRC_TYPE CRC;           ///< CRC  nn::util::Crc16
  u8 GameID;       ///< なんとかシリーズであるという識別子
  u8 commandnum;           ///< トータルコマンド数
  u8 PMVer;          ///< ＲＯＭバージョン番号
  u8 LangCode;       ///< 言語番号
  u8 connectNum;     ///< 接続人数
  u8 GFLNETVersionAndProductOrDevelopment;  ///< 通信バージョン  開発か本番か
  bit64 TransferedID;       ///< 発信者のTransferedID
} BeaconHead;  ///< ビーコンヘッダー 16byte

///< ビーコンの最大送信バイト  NET_DESC_APPDATA_SIZE_MAXは200なので 184使える計算
static const u8 BEACON_MAXBYTE = nn::uds::NET_DESC_APPDATA_SIZE_MAX - sizeof(BeaconHead);

///< 接続用ビーコンの最大送信バイト  184-4-16 = 164;
static const u8 BEACON_CONNECT_MAXBYTE = BEACON_MAXBYTE - sizeof(BeaconCommand) - nn::pia::local::UdsNode::BEACON_DATA_SYSTEM_RESERVED;

/// ビーコン生存時間  現在１分 2012.5.7
//                     nijiでは30FPSなので30に変更
static const u16 BEACON_DOWN_TIMER = 60*30;



typedef struct{
  BeaconHead header;   ///< ビーコンヘッダー
  u8 body[BEACON_MAXBYTE];   ///< ビーコン本体
} BeaconData;                ///< ビーコンデータ


typedef struct{
  bit64 lfc;           ///< ローカルフレンドコード
  u16 DownCount;       ///< ビーコン生存時間を数える
  u8 ConnectAsClient;  ///< 接続が許可されているかどうか検査 許可＝２ 不許可＝１ データ無し＝０
  u8 nowEntry;         ///< 接続人数
} BeaconRecvData;



typedef struct{
  BeaconData data;     ///< ビーコンデータ
  BeaconRecvData recv;  ///< レシーブデータ
  int NewFlg;   ///< 書き込みを行ったかどうかを判断するフラグ
} BeaconRecvCell;      ///< うけとるビーコンデータ



class Beacon{
  GFL_FORBID_COPY_AND_ASSIGN(Beacon);

private:
  BeaconData* mpBeaconData;

private:
  void SetCRC(void);
  u8 GetBeaconCommandSize(BeaconData* pBeaconData);

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @note    このクラスは実体をもちません。
              実体がほしい場合 BeaconSubstanceクラスを作ってください
              その時は200バイトほど使うことを意識してください。
   * @param   ビーコンデータ
   */
  //------------------------------------------------------------------
  Beacon(BeaconData* pBeacon);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンデータヘッダーを作る
   * @param   pmver         ポケモン？バージョン  GFLNET的にはゲームのバージョン
   * @param   LangCode      言語のバージョン
   * @param   transferedID  自分のtransferedID（識別ID）
   */
  //------------------------------------------------------------------
  void CreateBeaconHeader( u8 GameID, u8 pmver, u8 LangCode, const bit64 transferedID,int connectNum  );
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンコマンドを追加
   * @param   type          タイプ（アプリごとの番号）
   * @param   no            NO(アプリの中での使い分け)
   * @param   pData         データ
   * @param   size          サイズ
   */
  //------------------------------------------------------------------
  void AddBeaconCommand( u8 type, u8 no, const u8* pData, size_t size);



  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンコマンドを全部消す
   */
  //------------------------------------------------------------------
  void ResetCommand( void );
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンデータをえる
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  void* GetBeaconAddress(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコン生データヘッダー＋ボディをえる
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  void* GetBeaconBody(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ボディーをダイレクトに書く
   */
  //------------------------------------------------------------------
  void WriteBeaconBody(const u8* pData, const u32 size);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンデータを得る
   * @return    ビーコンデータ
   */
  //------------------------------------------------------------------
  inline BeaconData* GetBeaconData(void){ return mpBeaconData; };
  //------------------------------------------------------------------
  /**
   * @brief	  ポケモン？バージョンを得る
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  u8 GetPMVer(void);
  //------------------------------------------------------------------
  /**
   * @brief	  言語コード
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  u8 GetLangCode(void);
  //------------------------------------------------------------------
  /**
   * @brief	  接続人数を得る
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  u8 GetConnectNum(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ゲームフリークバージョンを得る
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  u8 GetGFLNETVersion(void);
  //------------------------------------------------------------------
  /**
   * @brief	  開発なのか本番なのかを得る
   * @return  製品ならtrue
   */
  //------------------------------------------------------------------
  bool IsProduct(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコン生データサイズをえる
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  size_t GetBeaconSize(void);
  //------------------------------------------------------------------
  /**
   * @brief	  GameIDをえる
   */
  //------------------------------------------------------------------
  u8 GetGameID(void);
  //------------------------------------------------------------------
  /**
   * @brief	  このビーコンのTransferedID
   * @return  上記のもの
   */
  //------------------------------------------------------------------
  bit64 GetTransferedID(void);       ///< 発信者のTransferedID
  //------------------------------------------------------------------
  /**
   * @brief	  呼び出しIDが引数のTransferedIDと一致するかどうか
   * @return  -1は一致していない その他は一致しておりインデックスを指す
   */
  //------------------------------------------------------------------
  int CheckCallTransferedID(bit64 transferedID);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンのデータをセットした
   * @return
   */
  //------------------------------------------------------------------
  void SetBeaconData(BeaconData* pBeaconData);
  //------------------------------------------------------------------
  /**
   * @brief	 データセット
   */
  //------------------------------------------------------------------
  void ChangeBeaconData(BeaconData* pBeaconData);
  //------------------------------------------------------------------
  /**
   * @brief	  CRCを得る
   * @return  CRC
   */
  //------------------------------------------------------------------
  GFLNET_CRC_TYPE GetCRC(void);
  //------------------------------------------------------------------
  /**
   * @brief	  ビーコンの中に特定のコマンドがあるかどうか検査
   * @param   type
   * @param   no
   * @return  あったらコマンドの先頭ポインタ  無ければNULL
   */
  //------------------------------------------------------------------
  void* CheckBeaconCommand( u8 type, u8 no );


};


GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)
#endif // __GFLNET_BEACON_H__






#endif
