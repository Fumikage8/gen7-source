// ============================================================================
/*
 * @file JoinFestaSelectWordFrameBase.h
 * @brief 挨拶選択アプリのフレームの基底クラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_FRAMEBASE_H_INCLUDE )
#define JOINFESTASELECTWORD_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewList.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaSelectWordViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSelectWordFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordFrameBase );
public:
  JoinFestaSelectWordFrameBase(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor );
  virtual~JoinFestaSelectWordFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*            m_pJoinFestaSelectWordWork;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList*        m_pJoinFestaSelectWordViewList;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects*     m_pJoinFestaSelectWordViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_FRAMEBASE_H_INCLUDE
