// ============================================================================
/*
 * @file JoinFestaRankingLowerViewListener.h
 * @brief ランキング下画面のリスナー
 * @date 2015.12.10
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARANKINGLOWERVIEWLISTENER_H_INCLUDE )
#define JOINFESTARANKINGLOWERVIEWLISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBaseListener.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRankingLowerViewListener : public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener
{
public:

  JoinFestaRankingLowerViewListener()
  {
  }
  ~JoinFestaRankingLowerViewListener()
  {
  }

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARANKINGLOWERVIEWLISTENER_H_INCLUDE
