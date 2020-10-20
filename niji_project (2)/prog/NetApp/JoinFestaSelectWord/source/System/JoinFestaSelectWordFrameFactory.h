// ============================================================================
/*
 * @file JoinFestaSelectWordFrameFactory.h
 * @brief 挨拶選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTASELECTWORD_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaSelectWordViewSystemAccessor;


class JoinFestaSelectWordFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordFrameFactory );
public:
  JoinFestaSelectWordFrameFactory(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor );
  virtual~JoinFestaSelectWordFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                m_pJoinFestaSelectWordWork;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor*  m_pJoinFestaSelectWordViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORD_FRAMEFACTORY_H_INCLUDE
