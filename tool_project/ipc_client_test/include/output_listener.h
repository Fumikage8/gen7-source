//=============================================================================
/**
 * @file	   output_listener.h
 * @brief	   �o�͉�ʂւ̃A�N�Z�X�̂��߂�Listener
 * @author   k.ohno
 * @date	   2014.5.30
 */
//=============================================================================

#ifndef __OUTPUT_LISTENER_H__
#define __OUTPUT_LISTENER_H__



GFL_NAMESPACE_BEGIN(ipcnet)

class OutputListener
{

public:
  virtual bool OutputCallback( char* pOut )=0;


};

GFL_NAMESPACE_END(ipcnet)


#endif// __OUTPUT_LISTENER_H__



