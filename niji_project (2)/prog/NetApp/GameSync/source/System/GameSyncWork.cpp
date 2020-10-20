// ============================================================================
/*
 * @file GameSyncWork.cpp
 * @brief ゲームシンクアプリのワーククラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncWork::GameSyncWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_DescriptionType( 0 ),
  m_eDescriptionMenuType( DESCRIPTION_MENU_TYPE_NULL ),
  m_bMainBootFlag( false ),
  m_pTrainerIconUtility( NULL )
{
}


GameSyncWork::~GameSyncWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
