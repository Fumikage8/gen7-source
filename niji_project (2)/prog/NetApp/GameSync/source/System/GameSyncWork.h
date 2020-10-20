// ============================================================================
/*
 * @file GameSyncWork.h
 * @brief ゲームシンクアプリのワーククラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_WORK_H_INCLUDE )
#define GAMESYNC_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/GameSync/source/GameSyncDefine.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


class GameSyncWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::GameSync::GAMESYNC_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::GameSync::GAMESYNC_APP_DEVICE_HEAP_SIZE;

public:

  enum DescriptionMenuType
  {
    DESCRIPTION_MENU_TYPE_NULL,
    DESCRIPTION_MENU_TYPE_TWO,
    DESCRIPTION_MENU_TYPE_THREE
  };

public:
  GameSyncWork();
  virtual~GameSyncWork();

  void SetMainBootFlag( bool bFlag ){ m_bMainBootFlag = bFlag; }
  bool GetMainBootFlag(){ return m_bMainBootFlag; }

  void SetDescriptionType( u32 type ){ m_DescriptionType = type; }
  u32 GetDescriptionType(){ return m_DescriptionType; }

  NetAppLib::Util::NetAppTrainerIconUtility* GetTrainerIconUtility() { return m_pTrainerIconUtility; }
  void SetTrainerIconUtility( NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility ){ m_pTrainerIconUtility = pTrainerIconUtility; }

  void SetDescriptionMenuType( DescriptionMenuType eType ){ m_eDescriptionMenuType = eType; }
  DescriptionMenuType GetDescriptionMenuType(){ return m_eDescriptionMenuType; }

private:
  u32                                           m_DescriptionType;
  DescriptionMenuType                           m_eDescriptionMenuType;
  bool                                          m_bMainBootFlag;

  NetAppLib::Util::NetAppTrainerIconUtility*    m_pTrainerIconUtility;
};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCWORK_H_INCLUDE
