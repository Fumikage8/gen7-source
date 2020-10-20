//=============================================================================
/**
 * @file	   client_main.h
 * @brief	   IPCクライアントのテスト
             gfl2::ipcを用いてテストを行う
 * @author   k.ohno
 * @date	   2014.3.17
 */
//=============================================================================

#ifndef __CLIENT_MAIN_H__
#define __CLIENT_MAIN_H__


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <network/include/gfl_network.h>
#include <network/include/gflnet_command_listener.h>

#include <ipc/include/gfl_ipc_network.h>

#include "../include/start_listener.h"
#include "output_listener.h"



GFL_NAMESPACE_BEGIN(gflnet)
class Manager;
GFL_NAMESPACE_BEGIN(p2p)
class NetSender;
GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet)


GFL_NAMESPACE_BEGIN(ipcnet)

class ClientMain : public gfl2::ipc::NetworkListener, public ipcnet::StartListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ClientMain);

  gfl2::ipc::Network* mpIpcNet;
  OutputListener* mpOutputListener;

  int mStep;
  int mTimer;

public:
  ClientMain(void);
  virtual ~ClientMain();

  void Update(void);

  void Initialize(void);

  void Finalize(void);

	virtual void RecvData(const void* pData, size_t dSize, const char* pName);

	virtual void StartCallback( int no, const u16* pName );

	virtual void TestCommand(int no);

	void SetOutput(OutputListener* pOutputListener){mpOutputListener = pOutputListener;};


	bool out( const char *str, ...);


};

GFL_NAMESPACE_END(ipcnet)


#endif// __CLIENT_MAIN_H__



