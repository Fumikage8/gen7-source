// ============================================================================
/*
 * @file JoinFestaAttractionResultBuilder.h
 * @brief アトラクション結果アプリを構築するクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_BUILDER_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaAttractionResultViewSystemAccessor;


class JoinFestaAttractionResultBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultBuilder );
public:
  JoinFestaAttractionResultBuilder( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork );
  virtual~JoinFestaAttractionResultBuilder();

  void SetViewSystemAccessor( NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor )
  {
    m_pJoinFestaAttractionResultViewSystemAccessor = pJoinFestaAttractionResultViewSystemAccessor;
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
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*                m_pJoinFestaAttractionResultWork;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor*  m_pJoinFestaAttractionResultViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
