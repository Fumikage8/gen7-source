//=============================================================================
/**
 * @file	   server_main.h
 * @brief	   IPC�T�[�o���C��

             �K�{
             �P�D�ڑ����𑗂��Ă����R�}���h�����胊�X�g������
             �Q�D�����Ă����R�}���h�ƃ��X�g�̐�ɕԐM����

             �I�v�V����
             �S�D�o�̓E�C���h�E������
             �T�D���O���Ƃ�
             �U�D���O���Ƃ邩�Ƃ�Ȃ����I���\�ɂ���
             �V�D��T�Ԃ��������O������
             �W�D��T�Ԃ��������O���������ǂ����I���\�ɂ���

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

  static const int CONNECT_NUM = 16;  ///< �c�[���ɐڑ��ł���A�v����

  char targetName[CONNECT_NUM][gfl2::ipc::NAME_LENGTH+2];  ///< �ڑ����̃A�v���̖��O��o�^
  

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



