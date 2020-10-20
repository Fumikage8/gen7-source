// ============================================================================
/*
 * @file NBRLowerViewBase.cpp
 * @brief 通信対戦受付アプリ　上画面ビュー
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
// mine
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
#include "NetApp/NBR/source/NBRResourceID.h"

#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

// コンストラクタ
NBRLowerViewBase::NBRLowerViewBase( NetAppLib::System::ApplicationWorkBase* pWork ) :
  NetAppLib::UI::NetAppCursorView( pWork )
  ,m_pWork( pWork )
  ,m_pHeap( pWork->GetAppHeap() )
{
}

// デストラクタ
NBRLowerViewBase::~NBRLowerViewBase()
{
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
