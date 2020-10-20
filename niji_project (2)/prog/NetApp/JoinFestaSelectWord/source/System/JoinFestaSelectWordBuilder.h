// ============================================================================
/*
 * @file JoinFestaSelectWordBuilder.h
 * @brief 挨拶選択アプリを構築するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORD_BUILDER_H_INCLUDE )
#define JOINFESTASELECTWORD_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaSelectWordViewSystemAccessor;


class JoinFestaSelectWordBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordBuilder );
public:
  JoinFestaSelectWordBuilder( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork );
  virtual~JoinFestaSelectWordBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor )
  {
    m_pJoinFestaSelectWordViewSystemAccessor = pJoinFestaSelectWordViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual bool IsCreateNetAppMessageMenu();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                m_pJoinFestaSelectWordWork;
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor*  m_pJoinFestaSelectWordViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
