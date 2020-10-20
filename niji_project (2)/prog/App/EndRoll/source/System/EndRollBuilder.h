// ============================================================================
/*
 * @file EndRollBuilder.h
 * @brief エンドロールアプリを構築するクラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_BUILDER_H_INCLUDE )
#define APPENDROLL_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/EndRoll/source/System/EndRollWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


class EndRollBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollBuilder );
public:
  EndRollBuilder( App::EndRoll::System::EndRollWork* pEndRollWork );
  virtual~EndRollBuilder();

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

private:
  App::EndRoll::System::EndRollWork*                m_pEndRollWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
