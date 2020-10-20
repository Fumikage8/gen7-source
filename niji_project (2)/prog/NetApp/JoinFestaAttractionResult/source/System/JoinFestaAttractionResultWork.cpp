// ============================================================================
/*
 * @file JoinFestaAttractionResultWork.cpp
 * @brief アトラクション結果アプリのワーククラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionResultWork::JoinFestaAttractionResultWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_AttractionData(),
  m_JoinFestaRankingListManager( GetDeviceHeap() )
{
}


JoinFestaAttractionResultWork::~JoinFestaAttractionResultWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
