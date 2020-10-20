//=============================================================================
/**
 * @file	   start_listener.h
 * @brief	   テスト通信とつなぐためのListener
 * @author   k.ohno
 * @date	   2014.6.03
 */
//=============================================================================

#ifndef __START_LISTENER_H__
#define __START_LISTENER_H__



GFL_NAMESPACE_BEGIN(ipcnet)

class StartListener
{

public:
  virtual void StartCallback( int no,const u16* pname )=0;

	virtual void TestCommand(int no) = 0;




};

GFL_NAMESPACE_END(ipcnet)


#endif// __START_LISTENER_H__



