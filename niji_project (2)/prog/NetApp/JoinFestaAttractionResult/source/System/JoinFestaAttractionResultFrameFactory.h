// ============================================================================
/*
 * @file JoinFestaAttractionResultFrameFactory.h
 * @brief アトラクション結果アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaAttractionResultViewSystemAccessor;


class JoinFestaAttractionResultFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultFrameFactory );
public:
  JoinFestaAttractionResultFrameFactory(
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor );
  virtual~JoinFestaAttractionResultFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*                m_pJoinFestaAttractionResultWork;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor*  m_pJoinFestaAttractionResultViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULT_FRAMEFACTORY_H_INCLUDE
