//=============================================================================
/**
 * @file	   server_main.h
 * @brief	   IPCサーバメイン

             必須
             １．接続名を送ってくれるコマンドをつくりリスト化する
             ２．送ってきたコマンドとリストの先に返信する

             オプション
             ４．出力ウインドウをつくる
             ５．ログをとる
             ６．ログをとるかとらないか選択可能にする
             ７．一週間たったログを消す
             ８．一週間たったログを消すかどうか選択可能にする

 * @author   k.ohno
 * @date	   2014.3.17
 */
//=============================================================================

#ifndef __SERVER_MAIN_H__
#define __SERVER_MAIN_H__

#include <network/include/gfl_network.h>
#include <network/include/gflnet_command_listener.h>

#include <ipc/include/gfl_ipc_network_def.h>

#include "output_listener.h"


GFL_NAMESPACE_BEGIN(gflnet)
class Manager;
GFL_NAMESPACE_BEGIN(p2p)
class NetSender;
GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet)


GFL_NAMESPACE_BEGIN(ipcnet)

class ServerMain : public gflnet::CommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ServerMain);

  gflnet::Manager* mpManager;
  gflnet::p2p::NetSender* mpSender;

  OutputListener* mpOutputListener;
  
  int mStep;
  int mTimer;

  static const int CONNECT_NUM = 16;  ///< ツールに接続できるアプリ数

  char targetName[CONNECT_NUM][gfl2::ipc::NAME_LENGTH+2];  ///< 接続中のアプリの名前を登録
  

public:
  ServerMain(OutputListener* pOutputListener);
  virtual ~ServerMain();

  void Update(void);

  void Start(void);

  gflnet::Manager* GetManager(void){ return mpManager; };

  bool out( const char *str, ...);
  
public:

  virtual bool ReceiveCallback( u16 SendClientId, u16 Command, u32 size, u8* pBuff,u32 ipno );


};

GFL_NAMESPACE_END(ipcnet)


#endif// __SERVER_MAIN_H__



