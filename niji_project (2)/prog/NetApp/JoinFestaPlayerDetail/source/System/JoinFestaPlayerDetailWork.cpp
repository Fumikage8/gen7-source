// ============================================================================
/*
 * @file JoinFestaPlayerDetailWork.cpp
 * @brief 詳細アプリのワーククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerDetailWork::JoinFestaPlayerDetailWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_SelectPlayer(),
  m_JoinFestaRecordListManager( GetDeviceHeap() ),
  m_SelectRecord(),
  m_JoinFestaSurveyListManager( GetDeviceHeap() )
{
}


JoinFestaPlayerDetailWork::~JoinFestaPlayerDetailWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
