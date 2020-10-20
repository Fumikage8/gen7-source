//=============================================================================
/**
 * @file    ObataTestBase.cpp
 * @brief   ポケモンライブラリのテスト
 * @author  obata_toshihiro
 * @date    2015.05.15
 */
//=============================================================================
#if PM_DEBUG

#include <System/include/HeapDefine.h>
#include <Print/include/PrintSystem.h>
#include "../include/ObataTestBase.h"

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_obata.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
ObataTestBase::ObataTestBase( void ) :
  m_systemHeap( NULL ),
  m_deviceHeap( NULL ),
  m_messageData( NULL ),
  m_wordset( NULL )
{
  for( u32 i=0; i<PRIVATE_STRBUF_NUM; ++i ) {
    m_privateStrBuf[i] = NULL;
  }
  for( u32 i=0; i<STRBUF_NUM; ++i ) {
    m_publicStrBuf[i] = NULL;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
ObataTestBase::~ObataTestBase()
{
}





//---------------------------------------------------------------------------
/**
 * @brief 初期化
 */
//---------------------------------------------------------------------------
void ObataTestBase::SetupBase( void )
{
  this->CreateHeap();
  this->CreateMessageData();
  this->CreateStrBuf();
  this->CreateWordset();
}

//---------------------------------------------------------------------------
/**
 * @brief 後片付け
 */
//---------------------------------------------------------------------------
void ObataTestBase::CleanupBase( void )
{
  this->DeleteWordset();
  this->DeleteStrBuf();
  this->DeleteMessageData();
  this->DeleteHeap();
}

/**
 * @brief テストプロセスで使用するヒープを作成する
 */
void ObataTestBase::CreateHeap( void )
{
  gfl2::heap::HeapBase* appSystemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
  gfl2::heap::HeapBase* appDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  size_t systemHeapSize = 0xc00;
  size_t deviceHeapSize = 0x400000;

  m_systemHeap = GFL_CREATE_LOCAL_HEAP_NAME( appSystemHeap, systemHeapSize, gfl2::heap::HEAP_TYPE_EXP, false, "HEAPID_OBATATEST" );
  m_deviceHeap = GFL_CREATE_LOCAL_HEAP_NAME( appDeviceHeap, deviceHeapSize, gfl2::heap::HEAP_TYPE_EXP, false, "HEAPID_OBATATEST_DEVICE" );
}

/**
 * @brief テストプロセスで使用するヒープを削除する
 */
void ObataTestBase::DeleteHeap( void )
{
  gfl2::heap::Manager::DeleteHeap( m_deviceHeap );
  gfl2::heap::Manager::DeleteHeap( m_systemHeap );
}

/**
 * @brief テスト用メッセージデータを生成する
 */
void ObataTestBase::CreateMessageData( void )
{
  m_messageData = GFL_NEW( m_systemHeap ) gfl2::str::MsgData( print::GetMessageArcId_Debug( System::MSGLANGID_MAX ), GARC_debug_message_d_obata_DAT, m_systemHeap );
}

/**
 * @brief テスト用メッセージデータを削除する
 */
void ObataTestBase::DeleteMessageData( void )
{
  GFL_SAFE_DELETE( m_messageData );
}

/**
 * @brief 文字列バッファを生成する
 */
void ObataTestBase::CreateStrBuf( void )
{
  for( u32 i=0; i<PRIVATE_STRBUF_NUM; ++i ) {
    m_privateStrBuf[i] = GFL_NEW( m_systemHeap ) gfl2::str::StrBuf( 256, m_systemHeap );
  }

  for( u32 i=0; i<STRBUF_NUM; ++i ) {
    m_publicStrBuf[i] = GFL_NEW( m_systemHeap ) gfl2::str::StrBuf( 256, m_systemHeap );
  }
}

/**
 * @brief 文字列バッファを破棄する
 */
void ObataTestBase::DeleteStrBuf( void )
{
  for( u32 i=0; i<PRIVATE_STRBUF_NUM; ++i ) {
    GFL_SAFE_DELETE( m_privateStrBuf[i] );
  }

  for( u32 i=0; i<STRBUF_NUM; ++i ) {
    GFL_SAFE_DELETE( m_publicStrBuf[i] );
  }
}

/**
 * @brief ワードセットを生成する
 */
void ObataTestBase::CreateWordset( void )
{
  m_wordset = GFL_NEW( m_systemHeap ) print::WordSet( m_systemHeap );
}

/**
 * @brief ワードセットを破棄する
 */
void ObataTestBase::DeleteWordset( void )
{
  GFL_SAFE_DELETE( m_wordset );
}


//---------------------------------------------------------------------------
/**
 * @brief システムヒープを取得する
 */
//---------------------------------------------------------------------------
gfl2::heap::HeapBase* ObataTestBase::SystemHeap( void ) const
{
  return m_systemHeap;
}

//---------------------------------------------------------------------------
/**
 * @brief デバイスヒープを取得する
 */
//---------------------------------------------------------------------------
gfl2::heap::HeapBase* ObataTestBase::DeviceHeap( void ) const
{
  return m_deviceHeap;
}

//---------------------------------------------------------------------------
/**
 * @brief ボタンオブジェクトを取得する
 */
//---------------------------------------------------------------------------
const gfl2::ui::Button* ObataTestBase::UiButton( void ) const
{
  const GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  const gfl2::ui::DeviceManager* uiDeviceManager = gameManager->GetUiDeviceManager();
  return uiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字列バッファを取得する
 * @param bufIndex  バッファのインデックス
 */
//---------------------------------------------------------------------------
gfl2::str::StrBuf* ObataTestBase::GetStrBuf( u32 bufIndex ) const
{
  if( STRBUF_NUM <= bufIndex ) {
    return NULL;
  }
  return m_publicStrBuf[ bufIndex ];
}

//---------------------------------------------------------------------------
/**
 * @brief 種族名を取得する
 * @param pokeParam  種族名を取得するポケモンのパラメータ
 */
//---------------------------------------------------------------------------
void ObataTestBase::GetMonsName( gfl2::str::StrBuf* destBuffer, const pml::pokepara::PokemonParam* pokeParam ) const
{
  m_messageData->GetString( label_obata_test_monsname, *m_privateStrBuf[0] );
  m_wordset->RegisterPokeMonsName( 0, pokeParam );
  m_wordset->Expand( destBuffer, m_privateStrBuf[0] );
}

//---------------------------------------------------------------------------
/**
 * @brief 種族名を取得する
 * @param pokeParam  種族名を取得するポケモンのモンスターNo
 */
//---------------------------------------------------------------------------
void ObataTestBase::GetMonsName( gfl2::str::StrBuf* destBuffer, MonsNo monsno ) const
{
  m_messageData->GetString( label_obata_test_monsname, *m_privateStrBuf[0] );
  m_wordset->RegisterPokeMonsNameNo( 0, monsno );
  m_wordset->Expand( destBuffer, m_privateStrBuf[0] );
}

//---------------------------------------------------------------------------
/**
 * @brief 技名を取得する
 * @param wazano  技No.
 */
//---------------------------------------------------------------------------
void ObataTestBase::GetWazaName( gfl2::str::StrBuf* destBuffer, WazaNo wazano ) const
{
  m_messageData->GetString( label_obata_test_wazaname, *m_privateStrBuf[0] );
  m_wordset->RegisterWazaName( 0, wazano );
  m_wordset->Expand( destBuffer, m_privateStrBuf[0] );
}

//---------------------------------------------------------------------------
/**
 * @brief 特性名を取得する
 * @param tokuseiNo  特性No.
 */
//---------------------------------------------------------------------------
void ObataTestBase::GetTokuseiName( gfl2::str::StrBuf* destBuffer, TokuseiNo tokuseiNo ) const
{
  m_messageData->GetString( label_obata_test_tokusei, *m_privateStrBuf[0] );
  m_wordset->RegisterTokuseiName( 0, tokuseiNo );
  m_wordset->Expand( destBuffer, m_privateStrBuf[0] );
}

//---------------------------------------------------------------------------
/**
 * @brief 性格名を取得する
 * @param seikaku  性格
 */
//---------------------------------------------------------------------------
void ObataTestBase::GetSeikakuName( gfl2::str::StrBuf* destBuffer, pml::pokepara::Seikaku seikaku ) const
{
  m_messageData->GetString( label_obata_test_seikaku, *m_privateStrBuf[0] );
  m_wordset->RegisterSeikakuName( 0, seikaku );
  m_wordset->Expand( destBuffer, m_privateStrBuf[0] );
}



GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG
