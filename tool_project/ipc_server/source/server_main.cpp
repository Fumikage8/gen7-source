//=============================================================================
/**
 * @file	   server_main.cpp
 * @brief	   IPCサーバ管理
 * @author     k.ohno
 * @date	   2014.3.4
 */
//=============================================================================



#include <network/include/gfl_network.h>
#include <math/include/gfl_math.h>

#include "main.h"

#include <network/tcp/include/gflnet_sender.h>
#include "../include/server_main.h"
#include <network/include/gflnet_manager.h>
#include <network/include/gflnet_typedef.h>

#include <util/include/gfl_std_string.h>




GFL_NAMESPACE_BEGIN(ipcnet)

ServerMain::ServerMain(OutputListener* pOutputListener)
{
  mpOutputListener = pOutputListener;
  mStep=0;
  mpManager=NULL;
}


ServerMain::~ServerMain()
{

  delete mpSender;
}

enum{
  _READY,
  _INIT,
  _END,
};


bool ServerMain::out( const char *str, ...)
{

    char debugOutBuff[ 1024 ];
 
    va_list ap;
    va_start( ap, str );
 
    if ( !vsprintf_s( debugOutBuff, 1024, str, ap ) ) {
        OutputDebugStringA( "dprintf error." );
        return false;
    }
    mpOutputListener->OutputCallback(debugOutBuff);
 
    return true;


}


void ServerMain::Update(void)
{
  if(mpManager){
    mpManager->Update();
  }
  switch(mStep){
  case _READY:
    break;
  case _INIT:
    break;
  case _END:
    if(mpManager){
      mpManager->Disconnect();
    }
    delete mpSender;
    delete mpManager;
    break;
  }
}

bool ServerMain::ReceiveCallback( u16 SendClientId, u16 Command, u32 size, u8* pBuff,u32 no )
{
	const gfl2::ipc::SendDataHead* pHead = reinterpret_cast<const gfl2::ipc::SendDataHead*>(pBuff);



	switch(Command){
  case gflnet::command::IPC_START:
    //SendClientIDに対応した 名前の受信
		gfl2::std::MemCopy(pHead->targetName, targetName[SendClientId], gfl2::ipc::NAME_LENGTH);
    GFL_PRINT("IPC_START %d \n", SendClientId);
    out("IPC_START %d %s\n", SendClientId,targetName[SendClientId]);
    break;
  case gflnet::command::IPC_COMMAND:
    GFL_PRINT("IPC_COMMAND %d \n", SendClientId);
    out("IPC_COMMAND %d \n", SendClientId);
    for(int i=0;i < CONNECT_NUM;i++){
      if(0==gfl2::std::MemComp(targetName[i], pHead->targetName, gfl2::ipc::NAME_LENGTH)){
        mpSender->SendDataCommand(pBuff,size, gflnet::command::IPC_COMMAND, i, 0);
		    out("IPC_COMMAND_RET %d \n", i);
      }
    }
    break;
  default:
   GFL_PRINT("non commnad %d \n", SendClientId);
    break;
  }
  return true;
}



void ServerMain::Start(void)
{
  mStep = _INIT;

  mpSender = new gflnet::p2p::NetSender(0, 2);
  mpSender->SetRecvDataListener(this);

  mpManager = new gflnet::Manager();
  mpManager->Initialize(true, true, true, false);
  GFL_PRINT("おやになった\n");
  mpManager->SetSender( mpSender );
  mpManager->Connect((u32)0);

}

GFL_NAMESPACE_END(ipcnet)


