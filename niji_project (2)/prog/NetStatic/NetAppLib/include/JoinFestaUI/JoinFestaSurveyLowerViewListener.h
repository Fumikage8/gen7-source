// ============================================================================
/*
 * @file JoinFestaSurveyLowerViewListener.h
 * @brief アンケート下画面のリスナー
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASURVERLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTASURVERLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBaseListener.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaSurveyLowerViewListener : public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener
{
public:

  JoinFestaSurveyLowerViewListener()
  {
  }
  ~JoinFestaSurveyLowerViewListener()
  {
  }

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTASURVERLOWERVIEWLISTENER_H_INCLUDE
