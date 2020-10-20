#if !defined( __BTL_ADAPTER_H__ )
#define __BTL_ADAPTER_H__
//=============================================================================================
/**
 * @file  btl_adapter.h
 * @brief ポケモンXY バトルシステム サーバ-クライアント間アダプター
 * @author  taya
 *
 * @date  2010.12.22  作成
 */
//=============================================================================================
#pragma once


#include "Battle/include/battle_SetupParam.h"
#include "btl_common.h"
#include "btl_server_const.h"
#include "btl_SendData.h"

GFL_NAMESPACE_BEGIN(btl)

class AdapterFactory;

//----------------------------------------------------------------------
/**
 *  Adapter クラス本体
 */
//----------------------------------------------------------------------
class Adapter {
  GFL_FORBID_COPY_AND_ASSIGN( Adapter );

private:
  Adapter( u8 clientID, bool fCommType );
  ~Adapter();
  friend  class AdapterFactory;

public:

  void         Init( void );
  void         ChangeCommType( bool fCommType );
  bool         IsWaitingClientReply( void ) const;

  void         SetCmd( SendDataSerialNumber serialNumber, ServerSequence serverSeq, AdapterCommand cmd, const void* sendData, u32 sendDataSize );
  void         ResetCmd( void );
  bool         WaitCmd( void );
  const SendData& GetReturnData( void ) const;


  static BtlCommMode  s_commMode;
  static void BeginSetCmd( void );
  static void EndSetCmd( void );

  void   RecvCmd( AdapterCommand* command, SendDataSerialNumber* commandSerialNumber, ServerSequence* serverSeq );
  void   ResetRecvBuffer( void );
  u32    GetRecvData( const void** ppRecv ) const;
  bool   ReturnCmd( const SendData& returnData );



private:

  typedef enum {
    AS_FREE = 0,
    AS_CMD_RECEIVED,
    AS_WAIT_SEND_TO_CLIENT,
    AS_WAIT_RECV_FROM_CLIENT,
    AS_DONE,
  }State;  


  SendData m_sendData;   // サーバーからの送信データのバッファ
  SendData m_returnData; // クライアントからの返信データのバッファ

  AdapterCommand    m_processingCmd;
  u8       m_myID;
  u8       m_myState;
  u8       m_fRetDataPrepared : 4;
  u8       m_fCommType : 4;


  bool                  startToReception( void );
  bool                  receptionClient( void );



};  // end of class Adapter

//----------------------------------------------------------------------
/**
 *  Adapter クラスを作成・破棄する FactoryClass
 */
//----------------------------------------------------------------------
class AdapterFactory {
  GFL_FORBID_COPY_AND_ASSIGN( AdapterFactory );

private:
  AdapterFactory(){ };
  ~AdapterFactory( void ){ };

  static AdapterFactory*  s_pSingleton;

public:
  static void      InitSystem( BtlCommMode commMode, gfl2::heap::HeapBase* pHeap );
  static void      QuitSystem( void );
  static AdapterFactory*  GetInstance( void );

  Adapter*    Create( BtlCommMode commMode, u8 clientID, HeapHandle heapHandle );


private:
  Adapter*    m_set[ BTL_CLIENT_MAX ];

};  // end of class AdapterFactory




GFL_NAMESPACE_END(btl)


#endif // __BTL_ADAPTER_H__
