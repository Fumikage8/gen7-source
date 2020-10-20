// ============================================================================
/*
 * @file JoinFestaOpenListWork.cpp
 * @brief 開催選択アプリのワーククラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


JoinFestaOpenListWork::JoinFestaOpenListWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_JoinFestaAttractionListManager( GetDeviceHeap() ),
  m_pSelectAttraction( NULL ),
  m_pDescriptionAttraction( NULL )
{
}


JoinFestaOpenListWork::~JoinFestaOpenListWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
