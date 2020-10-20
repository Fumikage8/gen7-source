// ============================================================================
/*
 * @file KisekaeViewSystemAccessor.cpp
 * @brief きせかえアプリのViewシステム関連のクラスを取得するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeViewSystemAccessor.h"

//  gflib2のインクルード
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)


ViewSystemAccessor::ViewSystemAccessor(
  App::Kisekae::System::ViewList* pViewList,
  App::Kisekae::System::ViewObjects* pViewObjects )
{
  GFL_ASSERT( pViewList );
  GFL_ASSERT( pViewObjects );

  m_pViewList     = pViewList;
  m_pViewObjects  = pViewObjects;
}


ViewSystemAccessor::~ViewSystemAccessor()
{
}

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
