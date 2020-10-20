// ============================================================================
/*
 * @file JoinFestaAttractionResultFrameBase.h
 * @brief アトラクション結果アプリのフレームの基底クラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULT_FRAMEBASE_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULT_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewList.h"
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaAttractionResultViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionResultFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultFrameBase );
public:
  JoinFestaAttractionResultFrameBase(
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork* pJoinFestaAttractionResultWork,
    NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewSystemAccessor* pJoinFestaAttractionResultViewSystemAccessor );
  virtual~JoinFestaAttractionResultFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultWork*            m_pJoinFestaAttractionResultWork;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewList*        m_pJoinFestaAttractionResultViewList;
  NetApp::JoinFestaAttractionResult::System::JoinFestaAttractionResultViewObjects*     m_pJoinFestaAttractionResultViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONRESULT_FRAMEBASE_H_INCLUDE
