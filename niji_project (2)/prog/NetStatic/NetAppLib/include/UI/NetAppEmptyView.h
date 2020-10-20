// ============================================================================
/*
 * @file NetAppEmptyView.h
 * @brief 
 * @date 2015.06.15
 */
// ============================================================================
#if !defined( NETAPPEMPTYVIEW_H_INCLUDE )
#define NETAPPEMPTYVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)


class NetAppEmptyView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppEmptyView );

public:

  NetAppEmptyView( app::util::Heap* pHeap ) : NetApplicationViewBase( pHeap ) {}
  NetAppEmptyView( NetAppLib::System::ApplicationWorkBase* pWorkBase) : NetApplicationViewBase( pWorkBase->GetAppHeap() ) {}
  virtual~NetAppEmptyView(){}

  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}

};


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPEMPTYVIEW_H_INCLUDE
