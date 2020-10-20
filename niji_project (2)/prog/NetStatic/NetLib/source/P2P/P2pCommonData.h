//======================================================================
/**
 * @file P2pCommonData.h
 * @date 2015/07/06 14:32:14
 * @author ichiraku_katsuhiko
 * @brief P2P共通データクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __P2P_COMMON_DATA_H_INCLUDED__
#define __P2P_COMMON_DATA_H_INCLUDED__
#pragma once

#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>
#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <pml/include/pml_PokeParty.h>
#include <util/include/gfl2_List.h>
#include "Savedata/include/MyStatus.h"
// 人物データ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

// ポケモン不正チェック
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

/**
 * @class P2pCommonData
 * @brief P2P共通データ
 */
class P2pCommonData
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pCommonData);
public:
  // コンストラクタ
  P2pCommonData( gfl2::heap::HeapBase* pHeap, gflnet2::p2p::NetGame* pNetGame, JoinFestaScript::CommRouteType commRouteType );
  // デストラクタ
  virtual ~P2pCommonData();

private:

  //! 署名サイズ
#if defined(GF_PLATFORM_CTR)
  static const int SIGNATURE_SIZE = NetLib::Validation::PokemonValidation::SIGNATURE_SIZE;
#else
  static const int SIGNATURE_SIZE = 256;
#endif // GF_PLATFORM_CTR


  /**
   * @struct CoreData
   * @brief  共通データ構造体
   */
  struct CoreData
  {
    Savedata::MyStatus*   m_pMyStatus;   //! プレイヤーデータ
    //
    pml::PokeParty*       m_pPokeParty;
  };

  /**
   * @struct CoreData
   * @brief  通信データ構造体
   */
  struct PacketData
  {
    union{

      //! プレイヤーデータ
      u8  my_status_core[ Savedata::MyStatus::SERIALIZE_DATA_SIZE ];

      //! ポケモンパーティ
      struct{
        pml::PokeParty::SERIALIZE_DATA poke_party_core; //! PokePartyシリアライズデータ

        //  署名
        u8  cert_data[ SIGNATURE_SIZE ];
        u32 cert_data_size;
        b8  cert_data_enable; // 署名ある時true
      };

      //! ジョインフェスタパーソナルデータ
      u8  join_festa_personal_data[ NetAppLib::JoinFesta::JoinFestaPersonalData::CORE_DATA_SIZE ];

    };
  };

  /**
   * @enum 送信コマンド
   */
  enum NetCommand
  {
    NET_COMMANDO_MYSTATUS   = 145,  //! MyStatus送信コマンド
    NET_COMMANDO_POKEPARTY  = 146,  //! PokeParty送信コマンド
    NET_COMMANDO_JF_PERSONAL= 147,  //! JoinFestaPersonalData送信コマンド
  };

  /**
   * @enum 同期コマンド
   */
  enum TimingCommand{
    FINISH_TIMING_NO_MYSTATUS     = 200,  //! MyStatus送受信
    FINISH_TIMING_NO_POKEPARTY    = 201,  //! PokeParty送受信
    FINISH_TIMING_NO_JF_PERSONAL  = 202,  //! JoinFestaPersonalData送受信
  };

  //! 接続相手最大人数
  static const int CONNET_TARGET_MAX = gflnet2::p2p::CONNECT_MAX_FOUR;

public:
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief MyStatusデータ送受信開始
   * @return  送信準備完了したらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool SendMyStatus();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief PokePartyデータ送受信開始
   * @param[in] pPokeParty    ポケパーティ
   * @param[in] pCerdData     署名データ（必要ないなら省略可）
   * @return  送信準備完了したらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief JoinFestaPersonalDataデータ送受信開始
   * @return  送信準備完了したらtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool SendJoinFestaPersonalData();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief データ送受信終了したか
   * @return 終了していたらtrue
   *
   * @li     Updateを呼ばないと受信しません
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsRecvEnd();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief 更新
   */
  /* ----------------------------------------------------------------------------------------- */
  void Update();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief     受信したマイステータス取得
   * @param[in] 通信ID（NetGameから取得する）
   * @return    MyStatus
   */
  /* ----------------------------------------------------------------------------------------- */
  Savedata::MyStatus* GetMyStatus( const int netId ) const { return m_recvData[netId].m_pMyStatus; }

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief     受信したPokeParty取得
   * @param[in] 通信ID（NetGameから取得する）
   * @return    PokeParty（なければNULL）
   */
  /* ----------------------------------------------------------------------------------------- */
  pml::PokeParty* GetPokeParty( const int netId ) const { return m_recvData[netId].m_pPokeParty; }

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   ポケモン改ざんチェックの結果取得
   * @return  改ざんが行われていないならtrueを返却する
   */
  /* ----------------------------------------------------------------------------------------- */
  bool IsValidationCheckOk( const int netId ){ return m_isValidationOk[netId]; }

private:
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  /**
   * @brief  パケットデータ送信処理
   * @param[in] packet      送信したいパケットデータ
   * @param[in] packetSize  送信したいパケットデータサイズ
   * @param[in] command     ユーザーコマンド
   * @return  送信準備完了したらtrue
   */
  bool SendPacket( void* packet, u32 packetSize, const u8 command );

  /**
   * @brief   受信更新関数
   */
  void RecvData();

  /**
   * @brief  受信したパケットデータ処理
   * @param[in] netId 受信相手の通信ID
   * @param[in] packet  受信したパケットデータ
   * @param[in] command ユーザーコマンド
   */
  void RecvPacket( const int netId, const PacketData& packet, const u8 command );

  /**
   * @brief   ポケモンの改ざんチェック
   * @param[in]   pCheckParty     チェックするポケパーティ
   * @param[in]   pCertData       証明書データ
   * @param[in]   pCertDataSize   証明書データサイズ
   * @return  改ざんが行われていないならtrueを返却する
   */
  bool CheckRsaValidation( const pml::PokeParty* pCheckParty, const u8* pCerdData, const u32 cerdDataSize );


private:
  gfl2::heap::HeapBase*     m_pHeap;        //! システムヒープ
  gflnet2::p2p::NetGame*    m_pNetGame;     //! 送受信管理クラス
  CoreData                  m_recvData[ CONNET_TARGET_MAX ];  //! 受信データ
  int                       m_recvCnt;      //! 受信データカウンタ
  bool                      m_isValidationOk[ CONNET_TARGET_MAX ]; //! ポケモンの改ざんチェック結果
  TimingCommand             m_timingCommand;  //! 同期コマンド

  JoinFestaScript::CommRouteType m_commRouteType; //! P2PゲームIDから算出した通信経路

  // 状態
  enum State{
    STATE_NULL,       // なし
    STATE_RECV_WAIT,  // 受信中
    STATE_TIMING,     // 同期
    STATE_RECV_END,   // 受信完了
  } m_state;
};

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // __P2P_COMMON_DATA_H_INCLUDED__