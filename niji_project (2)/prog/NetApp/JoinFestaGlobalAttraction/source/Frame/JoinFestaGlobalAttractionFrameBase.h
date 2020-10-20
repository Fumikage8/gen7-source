// ============================================================================
/*
 * @file JoinFestaGlobalAttractionFrameBase.h
 * @brief グローバルアトラクションアプリのフレームの基底クラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_FRAMEBASE_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewList.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaGlobalAttractionViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaGlobalAttractionFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionFrameBase );
public:
  JoinFestaGlobalAttractionFrameBase(
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor );
  virtual~JoinFestaGlobalAttractionFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*            m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewList*        m_pJoinFestaGlobalAttractionViewList;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewObjects*     m_pJoinFestaGlobalAttractionViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTION_FRAMEBASE_H_INCLUDE
