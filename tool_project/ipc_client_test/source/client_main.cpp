//=============================================================================
/**
 * @file	   server_main.cpp
 * @brief	   IPCサーバ管理
 * @author     k.ohno
 * @date	   2014.3.4
 */
//=============================================================================



#include <network/include/gfl_network.h>
#include <math/include/gfl2_Math.h>

#include "main.h"

#include <network/tcp/include/gflnet_sender.h>
#include "../include/client_main.h"
#include <network/include/gflnet_manager.h>
#include <network/include/gflnet_typedef.h>

#include <util/include/gfl_std_string.h>




GFL_NAMESPACE_BEGIN(ipcnet)

ClientMain::ClientMain()
{
  mpOutputListener = NULL;
  mStep=0;
  mpIpcNet=NULL;
}


ClientMain::~ClientMain()
{

}

enum{
  _READY,
  _INIT,
  _END,
};


bool ClientMain::out( const char *str, ...)
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


void ClientMain::Update(void)
{

  if(mpIpcNet){
    mpIpcNet->Update();
  }
  switch(mStep){
  case _READY:
    break;
  case _INIT:
    break;
  case _END:
    mpIpcNet->Finalize();
    GFL_DELETE(mpIpcNet);
    break;
  }
}



void ClientMain::Initialize(void)
{
  mStep = _INIT;
  if(mpIpcNet){
	  mpIpcNet->Initialize();
	}
}


void ClientMain::Finalize(void)
{
  if(mpIpcNet){
		mpIpcNet->Finalize();
	}
}



void ClientMain::RecvData(const void* pData, size_t dSize, const char* pName)
{
	GFL_PRINT("RecvData \n");
	out("recv\n");

}


void ClientMain::StartCallback( int no, const u16* pName )
{

  if(mpIpcNet){
		mpIpcNet->Finalize();
	}

  gfl2::ipc::structAppParam param;

	switch(no){
	case 0:
		gfl2::std::MemCopy( "A_AP", param.myName , gfl2::ipc::NAME_LENGTH);
		gfl2::std::MemCopy( "B_AP", param.targetName , gfl2::ipc::NAME_LENGTH);
		break;
	case 1:
		gfl2::std::MemCopy( "B_AP", param.myName , gfl2::ipc::NAME_LENGTH);
		gfl2::std::MemCopy( "A_AP", param.targetName , gfl2::ipc::NAME_LENGTH);
		break;
	}
	gfl2::std::MemCopy( pName, param.mAddress, sizeof(param.mAddress));
	param.clientno = no;
	param.mpListener=this;
	param.mpHeapBase = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );

	mpIpcNet = new gfl2::ipc::Network(&param);


}


	//テストコマンド送信


void ClientMain::TestCommand(int no)
{
	if(!mpIpcNet){
		return;
	}
	switch(no){
		case 0:
			{
				char buff[100]={1,2,3,4,5,6};
				mpIpcNet->SendData(buff,sizeof(buff));
			}
			break;
		case 1:
			{
				char buff[10]={1,2,3,4,5,6};
				mpIpcNet->SendData(buff,sizeof(buff));
			}
			break;
	}


}




GFL_NAMESPACE_END(ipcnet)


