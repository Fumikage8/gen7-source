// ============================================================================
/*
 * @file EndRollBuilder.cpp
 * @brief エンドロールアプリを構築するクラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollBuilder.h"
#include "App/EndRoll/source/System/EndRollFrameSwitcher.h"
#include "App/EndRoll/source/System/EndRollFrameFactory.h"
#include "App/EndRoll/source/EndRollResourceID.h"
#include "App/EndRoll/source/EndRollFrameID.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


EndRollBuilder::EndRollBuilder( App::EndRoll::System::EndRollWork* pEndRollWork ) :
  m_pEndRollWork( pEndRollWork )
{
}


EndRollBuilder::~EndRollBuilder()
{
}


NetAppLib::System::IApplicationFrameSwitcher* EndRollBuilder::CreateApplicationFrameSwitcher()
{
  app::util::Heap* pAppHeap = m_pEndRollWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollFrameSwitcher();
}


NetAppLib::System::IApplicationFrameFactory* EndRollBuilder::CreateApplicationFrameFactory()
{
  app::util::Heap* pAppHeap = m_pEndRollWork->GetAppHeap();
  return GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::System::EndRollFrameFactory( m_pEndRollWork );
}


NetAppLib::System::ApplicationWorkBase* EndRollBuilder::GetApplicationWork()
{
  return m_pEndRollWork;
}


NetAppLib::System::FrameID EndRollBuilder::GetApplicationStartFrameID()
{
  return App::EndRoll::APPENDROLL_FRAME_ID_NORMAL;
}


u32 EndRollBuilder::GetApplicationResourceCount()
{
  return App::EndRoll::APPENDROLL_RESOURCE_MAX;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
