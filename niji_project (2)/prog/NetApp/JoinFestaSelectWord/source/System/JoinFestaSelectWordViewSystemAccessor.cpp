// ============================================================================
/*
 * @file JoinFestaSelectWordViewSystemAccessor.cpp
 * @brief 挨拶選択アプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


JoinFestaSelectWordViewSystemAccessor::JoinFestaSelectWordViewSystemAccessor(
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewList* pJoinFestaSelectWordViewList,
  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewObjects* pJoinFestaSelectWordViewObjects ) :
  m_pJoinFestaSelectWordViewList( pJoinFestaSelectWordViewList ),
  m_pJoinFestaSelectWordViewObjects( pJoinFestaSelectWordViewObjects )
{
}


JoinFestaSelectWordViewSystemAccessor::~JoinFestaSelectWordViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
