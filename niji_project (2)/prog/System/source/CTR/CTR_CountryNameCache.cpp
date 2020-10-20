//===================================================================
/**
 * @file    CTR_CountryNameCache.cpp
 * @brief   国名をキャッシュ管理するためのクラス
*/
//===================================================================
#if defined( GF_PLATFORM_CTR )
#include "System/include/CTR/CTR_CountryNameCache.h"
#include "Print/include/PrintSystem.h"
#include "Print/include/wordset.h"

#include <nn/cfg/CTR/cfg_SimpleAddressPrivate.h>

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_illegalname.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )



//--------------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//--------------------------------------------------------------------------------------------
CountryNameCache::CountryNameCache( void ) :
  m_countryNameList(),
  m_unknownCountryName()
{
  initialize();
}
    
//--------------------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------------------
CountryNameCache::~CountryNameCache( void )
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 国名のリストを初期化します
 */
//--------------------------------------------------------------------------------------------
void  CountryNameCache::initialize( void )
{
  ::std::memset( m_countryNameList, 0, sizeof(m_countryNameList) );
  ::std::memset( m_unknownCountryName, 0, sizeof(m_unknownCountryName) );
}

//--------------------------------------------------------------------------------------------
/**
* @brief 言語コードを指定して、国名をキャッシュする
*
* @param devHeap     一時的に使用するデバイスヒープ
* @param langCode    ロード対象となる言語コード
*
*/
//--------------------------------------------------------------------------------------------
void CountryNameCache::LoadCountryName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode )
{
  gfl2::str::MsgData* msgData = GFL_NEW( devHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_illegalname_DAT, devHeap, gfl2::str::MsgData::LOAD_FULL );

  LoadCountryName( devHeap, langCode, msgData );

  GFL_SAFE_DELETE( msgData );

}


void CountryNameCache::LoadCountryName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode, gfl2::str::MsgData* pIllegalNameMessageData )
{
  // 複数のスレッドからの同期読み込み対策
  // illegalname の MsgData の作成を外で行って渡してもらいます
  // (この関数内では同期読み込みを行わないのでスレッドで呼び出せる)

  initialize();

  // 国名一覧を取得
  uptr       workBuf = (uptr)GflHeapAllocMemory( devHeap, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE);
  nn::Result result  = nn::cfg::CTR::GetCountryNameList( m_countryNameList, langCode, workBuf, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE );
  GflHeapFreeMemory( (void*)workBuf );
  GFL_ASSERT( result.IsSuccess() );

  print::WordSet*     wordSet = GFL_NEW( devHeap ) print::WordSet( devHeap );
  gfl2::str::StrBuf   strBuf( 128, devHeap );

  pIllegalNameMessageData->GetString( illegalname_01, strBuf );

  // 不明な国名を保存しておく
  ::std::wcsncpy( m_unknownCountryName, strBuf.GetPtr(), nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );

  // 取得できなかった国名に文字をセット
  for( u32 i = 0; i < nn::cfg::CTR::CFG_COUNTRY_MAX; i++ )
  {
    if( m_countryNameList[i][0] == '\0' ){
      ::std::wcsncpy( m_countryNameList[i], m_unknownCountryName, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
    }
  }

  GFL_SAFE_DELETE( wordSet );

}


//--------------------------------------------------------------------------------------------
/**
* @brief キャッシュされた国名を取得する
*
* @param countryCode 取得対象となる国コード
*
* @return キャッシュされた国名。
*/
//--------------------------------------------------------------------------------------------
const gfl2::str::STRCODE* CountryNameCache::GetCountryName( nn::cfg::CTR::CfgCountryCode countryCode ) const
{
  if( countryCode < nn::cfg::CTR::CFG_COUNTRY_MAX ){
    return m_countryNameList[countryCode];
  }
  return m_unknownCountryName;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 不明な国名に表示するための文字列「？？？」を取得する
 *
 * @return 不明な国名に表示するための文字列
 */
//--------------------------------------------------------------------------------------------
const gfl2::str::STRCODE* CountryNameCache::GetUnkownCountryName( void ) const
{
  return m_unknownCountryName;
}




GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )

#endif // defined( GF_PLATFORM_CTR )

