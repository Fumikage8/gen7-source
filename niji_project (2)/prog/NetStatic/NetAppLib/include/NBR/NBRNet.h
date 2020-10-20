//======================================================================
/**
 * @file NBRNet.h
 * @date 2015/07/08 15:02:39
 * @author ichiraku_katsuhiko
 * @brief 通信対戦受付アプリの通信モジュール
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NBR_NET_H_INCLUDED__
#define __NBR_NET_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_List.h>
#include <pml/include/pmlib.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>

// NBRNetDefine
#include "NetStatic/NetAppLib/include/NBR/NBRNetDefine.h"
// Regulation
#include "Battle/Regulation/include/Regulation.h"

// パケット分割送信クラス
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransferListener.h"

// 前方宣言
namespace gflnet2{
  namespace p2p{
    class NetGame;
  }
}
class Regulation;

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

// 前方宣言
class NBRNetRecvListener;

/**
 * @class NBRNet
 * @brief NBR通信モジュール
 */
class NBRNet : public NetLib::P2P::P2pPacketTransferListener
{
  GFL_FORBID_COPY_AND_ASSIGN(NBRNet);

public:
  /**
   * @enum NetGameCommand
   * NetGameに渡す通信コマンド
   */
  enum NetGameCommand
  {
    NET_GAME_COMMAND_END              = 40,   //! 終了通知
    NET_GAME_COMMAND_PRIORITY         = 41,   //! ルール候補者の優先順位通知
    NET_GAME_COMMAND_RULE_SELECTOR    = 42,   //! ルール選択者通知
    NET_GAME_COMMAND_RULE_OTHER       = 43,   //! ルール選択を他の人に任せる通知
    NET_GAME_COMMAND_SELECTOR_SELF    = 44,   //! ルール選択者が自分になった通知
    NET_GAME_COMMAND_REGULATION_READY = 45,   //! レギュレーション送信準備
    NET_GAME_COMMAND_REGULATION_P     = 46,   //! レギュレーション送信（プリセット）
    NET_GAME_COMMAND_REGULATION_D     = 47,   //! レギュレーション送信（配信）
    NET_GAME_COMMAND_REGULATION_RET   = 48,   //! レギュレーション受信結果送信
    NET_GAME_COMMAND_BATTLE_START     = 49,   //! 勝負開始通知
    NET_GAME_COMMAND_CANT_PARTICIPATE = 50,   //! 参加出来ない通知
    NET_GAME_COMMAND_POKEPARTY_RESULT = 51,   //! ポケパーティ検査結果通知
    NET_GAME_COMMAND_MULTI_DECIDE     = 52,   //! マルチ振り分け決定通知
    NET_GAME_COMMAND_MULTI_SELECT     = 53,   //! マルチ振り分け通知
    NET_GAME_COMMAND_CONTINUE         = 54,   //! 続けて遊ぶ通知
  };

private:
  static const int COMMAND_MAX = 14;  //! 通信コマンド総数 

  /**
   * @struct PacketData
   * @brief  通信データ構造体
   */
  struct PacketData
  {
    union{
      //! NET_GAME_COMMAND_PRIORITY用
      u8 priority[ NBRNetDefine::NET_NUM_MAX ]; //! NetIdが要素で優先度が格納されている

      //! NET_GAME_COMMAND_RULE_SELECTOR用
      u8 rule_selector_command;

      //! NET_GAME_COMMAND_REGULATION_READY用
      b8 is_preset;

      //! NET_GAME_COMMAND_REGULATION_P用
      u8 regulation_preset;

      //! NET_GAME_COMMAND_REGULATION_RET用
      u8 regulation_ret_command;

      //! NET_GAME_COMMAND_POKEPARTY_RESULT
      u8 pokeparty_ret_command[ NBRNetDefine::NET_NUM_MAX ]; //! NetIdが要素

      //! NET_GAME_COMMAND_CONTINUE用
      u8 continue_command;

      //! NET_GAME_COMMAND_BATTLE_START用
      struct{
        u32 selectBgm;
        b8  is_qr_team; // QRバトルチーム用
      };

      //! NET_GAME_COMMAND_MULTI_DECIDE用
      u8 place[ NBRNetDefine::NET_NUM_MAX ]; //! NetIdが要素で戦う位置が入っている
      
      //! NET_GAME_COMMAND_MULTI_SELECT用
      u8 multiValue[2];  // [0]=決めた人のNetID [1]=決めた立ち位置
    };

  };

  /**
   * @struct PacketDataForRegulation
   * @brief  通信データ構造体(レギュレーション専用)
   */
  struct PacketDataForRegulation
  {
    //! NET_GAME_COMMAND_REGULATION用
    u8  regulation_buffer[ Regulation::REGULATION_MAX_DATA_SIZE ];
    u32 regulation_size;
    // 署名
    // 2016/05/16追記　レギュレーションの署名はレギュレーションデータ内に含まれる事になった。
    //                 下記は、実質使用していない。今から送信データ変えるのも影響が大きいのでこのままでいく。
    u8  cert_data[ 1 ];
    u32 cert_data_size;
  };


  //! パケットデータの最大サイズ
  static const u32 PACKET_MAX_SIZE = sizeof(PacketData);


public:
  // コンストラクタ
  NBRNet( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~NBRNet();

  /**
   * @brief   初期化
   */
  void Initialize();

  /**
   * @brief   初期化終了待ち
   */
  bool IsInitialize();

  /**
   * @brief   終了
   */
  void Terminate();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   親かどうか
   * @return  trueで親
   */
  bool IsMaster();

  /**
   * @brief   接続順番 = NetIDをかえす
   * @return  NetID
   */
  int GetNetID();

  /**
   * @brief   Wifi接続かどうか
   * @return  trueでWifi接続
   */
  bool IsWifi();

  /**
   * @brief   接続人数を取得
   * @return  接続人数（自分含む）
   */
  u8 GetConnectionNum();

  /**
   * @brief   プールしている受信バッファをクリアする
   */
  void ClearRecvPoolBuffer();

public:

  /**
   * @brief  タイミングをそろえるコマンドを発行確認まで内部でやってくれる
   * @param  timingUniqueNo    タイミングをそろえるためのユニーク番号 ０以外
   * @return bool              到着したタイミングから次のスタートを行うまでずっとtrue
   */
  bool IsTimingLoopCheck( const NBRNetDefine::TimingCommand timing );

  //=============================================================================
  /**
   *  送信
   */
  //=============================================================================
  /**
   * @brief  通信終了通知送信
   * @return  送信準備完了したらtrue
   */
  bool SendEnd();

  /**
   * @brief     ルール選ぶ人の候補者優先順位通知送信（親のみ）
   * @param[in] ルール選ぶ人の候補者優先順位（NetIdが要素で優先度が格納されている）
   * @return  送信準備完了したらtrue
   */
  bool SendPriority( const u8 priority[ NBRNetDefine::NET_NUM_MAX ] );

  /**
   * @brief     ルール選択者通知送信
   * @param[in] ルール選択者通知コマンド
   * @return  送信準備完了したらtrue
   */
  bool SendRuleSelector( const NBRNetDefine::RuleSelectorCommand command );

  /**
   * @brief  ルール選択を他の人に任せる通知送信
   * @return  送信準備完了したらtrue
   */
  bool SendRuleOther();

  /**
   * @brief  ルール選択者が自分になった通知送信
   * @return  送信準備完了したらtrue
   */
  bool SendRuleSelectorIsMe();

  /**
   * @brief  ルール選択者が自分になった通知送信
   * @param[in] 送信するレギュレーションがプリセットならtrue。配信ならfalse。
   * @return  送信準備完了したらtrue
   */
  bool SendRegulationReady( const bool isPreset );

  /**
   * @brief  レギュレーション送信（プリセット版）
   * @param[in]   presetIndex          送信するプリセットレギュレーションの番号
   * @return  送信準備完了したらtrue
   */
  bool SendRegulation( Regulation::PRESET presetIndex );

  /**
   * @brief  レギュレーション送信（配信レギュレーション版）
   * @param[in]   regulation          送信するレギュレーション
   * @param[in]   pCerdData           署名データ
   * @param[in]   cerdDataSize        署名データサイズ
   * @return  送信準備完了したらtrue
   */
  bool SendRegulation( Regulation& regulation,
                       const void* pCerdData, const u32 cerdDataSize );

  /**
   * @brief     レギュレーション受信結果通知送信
   * @param[in] レギュレーション受信結果通知コマンド
   * @return  送信準備完了したらtrue
   */
  bool SendRegulationRet( const NBRNetDefine::RegulationRetCommand command );

  /**
   * @brief     勝負開始通知送信
   * @param[in] bgm     選択されたBGM（ローカルで親のみ設定）
   * @param[in] isQrTeam QRバトルチームかどうか
   * @return  送信準備完了したらtrue
   */
  bool SendBattleStart( const u32 bgm, bool isQrTeam );

  /**
   * @brief PokePartyデータ送受信開始
   * @param[in] pPokeParty    ポケパーティ
   * @param[in] pCerdData     署名データ
   * @return  送信準備完了したらtrue
   */
  bool SendPokeParty( const pml::PokeParty* pPokeParty, const void* pCerdData );

  /**
   * @brief PokeParty送受信終了したか
   * @return 終了していたらtrue
   */
  bool IsRecvPokePartyEnd();

  /**
   * @brief   ポケモン改ざんチェックの結果取得
   * @param[in] netId   通信ID（NetGame::GetNetID)
   * @return  改ざんが行われていないならtrueを返却する
   */
  bool IsValidationCheckOk( const int netId );


  /**
   * @brief  参加出来ない受信通知送信
   * @return  送信準備完了したらtrue
   */
  bool SendCantParticipate();

  /**
   * @brief     ポケパーティ検査結果通知コマンド送信
   * @param[in] ポケパーティ検査結果通知コマンド
   * @return  送信準備完了したらtrue
   */
  bool SendPokePartyRet( const NBRNetDefine::PokePartyRetCommand command[ NBRNetDefine::NET_NUM_MAX ] );

  /**
   * @brief  マルチ振り分け決定通知送信
   * @param[in] NetIdが要素で戦う位置が格納されている
   * @return  送信準備完了したらtrue
   */
  bool SendMultiDecide( const u8 place[ NBRNetDefine::NET_NUM_MAX ] );

  /**
   * @brief  マルチ振り分け通知送信
   * @param[in] netId   決めた人のNetId
   * @param[in] place   立ち位置（BTL_CLIETN_ID）
   * @return  送信準備完了したらtrue
   */
  bool SendMultiSelect( int netId, const u8 place );

  /**
   * @brief     続けて遊ぶ受信結果通知送信
   * @param[in] 続けて遊ぶ受信結果通知コマンド
   * @return  送信準備完了したらtrue
   */
  bool SendContinue( const NBRNetDefine::ContinueCommand command );


  //=============================================================================
  /**
   *  受信
   */
  //=============================================================================
  /**
   * @brief  受信結果通知リスナー設定
   */
  void RegistRecvListener( NBRNetRecvListener* pListener );

  /**
   * @brief  受信結果通知リスナー解除
   */
  void UnregistRecvListener();

  /**
   * @brief  受信パケットを処理可能なコマンドかどうか
   * @return 可能ならtrue
   */
  bool IsRecvPacketEnableCommand( const u8 command );

  /**
   * @brief  受信パケットを処理可能なコマンド追加
   *         コレに登録しないとそのコマンドは処理されない
   */
  void AddRecvPacketEnableCommand( const u8 command );

  /**
   * @brief  受信パケットを処理可能なコマンドをクリア
   */
  void ClearRecvPacketEnableCommand();

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
  void RecvDataUpdate();

  /**
   * @brief   プールしてあるデータ処理関数
   */
  void PoolBufferUpdate();

  /**
   * @brief  受信したパケットデータ処理
   * @param[in] myNetId 自分の通信ID
   * @param[in] recvNetId 受信相手の通信ID
   * @param[in] packet  受信したパケットデータ
   * @param[in] command ユーザーコマンド
   */
  void RecvPacket( const int myNetId, const int recvNetId, const void* recvBuf, const u8 command );

  /**
   * @brief   自分の受信パケットを無視するコマンドかどうか
   * @return  trueで無視する
   */
  bool IsIgnoreMyRecvData( const u8 command ) const;


  // P2pPacketTransferListener
  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

private:
  gfl2::heap::HeapBase*     m_pHeap;
  gflnet2::p2p::NetGame*    m_pNetGame;       //! 送受信管理クラス
  NBRNetRecvListener*       m_pRecvListener;  //! 受信結果通知リスナー
  u8                        m_enableCommand[ COMMAND_MAX ]; //! パケットデータ処理が可能なコマンド                        
  NetLib::P2P::P2pPacketTransfer* m_pP2pPacketTransfer; //! パケット分割送信
  PacketDataForRegulation   m_sendRegulationPacket;     //! 送信するレギュレーションバッファ

  // 受信結果プール用
  struct RecvBuffer{
    int  id;        //! 通信ID
    u8   recv_data[PACKET_MAX_SIZE];//! 受信データ
    u8   command;   //! 受信コマンド
    u16  size;      //! 受け取ったデータサイズ
  };
  // プール出来る個数 マルチを考えると8つぐらい
  static const int RECV_POOL_BUFFER_NUM = 8;
  gfl2::util::List<RecvBuffer>  m_recvPoolBufferList;

};

GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // __NBR_NET_H_INCLUDED__