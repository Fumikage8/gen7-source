// ============================================================================
/*
 * @file JoinFestaGlobalAttractionBuilder.h
 * @brief グローバルアトラクションアプリを構築するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_BUILDER_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaGlobalAttractionViewSystemAccessor;


class JoinFestaGlobalAttractionBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionBuilder );
public:
  JoinFestaGlobalAttractionBuilder( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor )
  {
    m_pJoinFestaGlobalAttractionViewSystemAccessor = pJoinFestaGlobalAttractionViewSystemAccessor;
  }

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility();
  virtual void GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen );
  virtual bool IsCreateNetAppMessageMenu();

  virtual bool IsEnableInputExclusiveKeyMode();

private:
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor*  m_pJoinFestaGlobalAttractionViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
