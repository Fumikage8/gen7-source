// ============================================================================
/*
 * @file KisekaeViewSystemAccessor.h
 * @brief きせかえアプリのViewシステム関連のクラスを取得するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define KISEKAE_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  View関連のインクルード
#include "App/Kisekae/source/System/KisekaeViewObjects.h"
#include "App/Kisekae/source/System/KisekaeViewList.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

class ViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( ViewSystemAccessor );

public:

  ViewSystemAccessor(
    App::Kisekae::System::ViewList* pViewList,
    App::Kisekae::System::ViewObjects* pViewObjects );

  virtual~ViewSystemAccessor();

  App::Kisekae::System::ViewList*  GetViewList() { return m_pViewList; }
  App::Kisekae::System::ViewObjects*  GetViewObjects() { return m_pViewObjects; }

private:

  App::Kisekae::System::ViewList*     m_pViewList;
  App::Kisekae::System::ViewObjects*  m_pViewObjects;
};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)


#endif // KISEKAE_VIEWSYSTEMACCESSOR_H_INCLUDE
