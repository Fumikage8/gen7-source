// ============================================================================
/*
 * @file JoinFestaSelectWordWork.cpp
 * @brief 挨拶選択アプリのワーククラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordResourceID.h"
#include <niji_conv_header/message/msg_setphrase.h>
#include <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)

 
JoinFestaSelectWordWork::JoinFestaSelectWordWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_eEditMenuID( NetApp::JoinFestaSelectWord::EDIT_MENU_ID_MEETING_MESSAGE ),
  m_JoinFestaSelectWordTable(),
  m_nMainFrameSequenceNo( 0 ),
  m_unSelectCategoryIndex( 0 ),
  m_bDescriptionOpenAnimeFlag( true )
{
}


JoinFestaSelectWordWork::~JoinFestaSelectWordWork()
{
}


void JoinFestaSelectWordWork::SetupTable()
{
  NetAppLib::System::ResourceManager* pResourceManager = GetResourceManager();

  m_JoinFestaSelectWordTable.Setup(
    pResourceManager->GetResourceBuffer( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_INFO_TABLE ),
    pResourceManager->GetResourceBuffer( NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_RESOURCE_ID_CATEGORY_TABLE ) );

}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
