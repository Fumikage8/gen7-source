//===================================================================
/**
 * @file    CTR_RegionNameCache.cpp
 * @brief   地域名をキャッシュ管理するためのクラス
*/
//===================================================================
#if defined( GF_PLATFORM_CTR )
#include "System/include/CTR/CTR_RegionNameCache.h"
#include "Print/include/PrintSystem.h"
#include  "Print/include/wordset.h"

#include <nn/cfg/CTR/cfg_SimpleAddressPrivate.h>


GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )



//--------------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//--------------------------------------------------------------------------------------------
RegionNameCache::RegionNameCache( const gfl2::str::STRCODE* pUnknownCountryName, u8* pWorkBuffer ) :
  m_pWorkBuffer( (uptr)pWorkBuffer ),
  m_unknownCountryName(),
  m_pRegionNameList( NULL ),
  m_pSimpleAddressIdList( NULL ),
  m_regionCount( 0 )
{
  if( pUnknownCountryName )
  {
    ::std::wcsncpy( m_unknownCountryName, pUnknownCountryName, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
  }
  else
  {
    ::std::memset( m_unknownCountryName, 0, sizeof(m_unknownCountryName) );
  }
}
    
//--------------------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------------------
RegionNameCache::~RegionNameCache( void )
{
  terminate();
}

//--------------------------------------------------------------------------------------------
/**
* @brief 地域名のリストを破棄します
*/
//--------------------------------------------------------------------------------------------
void  RegionNameCache::terminate( void )
{
  GFL_SAFE_DELETE_ARRAY( m_pRegionNameList );
  GFL_SAFE_DELETE_ARRAY( m_pSimpleAddressIdList );
  m_regionCount = 0;
}

//--------------------------------------------------------------------------------------------
/**
* @brief 言語コードと国コードを指定して、地域名をキャッシュする
*
* @param devHeap     一時的に使用するデバイスヒープ
* @param langCode    ロード対象となる言語コード
* @param countryCode ロード対象となる国コード
*
*/
//--------------------------------------------------------------------------------------------
void RegionNameCache::LoadRegionName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode, nn::cfg::CTR::CfgCountryCode countryCode )
{
  terminate();

  // 指定した国の地域名一覧を取得
  
  nn::Result result  = nn::cfg::CTR::GetRegionCount( &m_regionCount, static_cast<u32>( countryCode ), m_pWorkBuffer, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE );
//  GFL_ASSERT( result.IsSuccess() );

  if( result.IsSuccess() )
  {
    if( m_regionCount > 0 )
    {
//      GFL_PRINT( "Country ID = %d, Region count = %d, Simple address ID list size = %d byte, Region name list size = %d byte \n", countryCode, m_regionCount, m_regionCount * sizeof( nn::cfg::CTR::SimpleAddressId ), m_regionCount * ( sizeof( gfl2::str::STRCODE ) * nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH ) );

      m_pSimpleAddressIdList = GFL_NEW_ARRAY( devHeap ) nn::cfg::CTR::SimpleAddressId[ m_regionCount ];
 
      m_pRegionNameList = GFL_NEW_ARRAY( devHeap ) gfl2::str::STRCODE[ m_regionCount ][ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH ];

      result = nn::cfg::CTR::GetRegionList( m_pRegionNameList, m_pSimpleAddressIdList, countryCode, langCode, m_pWorkBuffer, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE );
//      GFL_ASSERT( result.IsSuccess() );

      // 取得できなかった地域名に文字をセット
      for( u32 i = 0; i < m_regionCount; ++i )
      {
        if( m_pRegionNameList[i][0] == '\0' )
        {
          ::std::wcsncpy( m_pRegionNameList[i], m_unknownCountryName, nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
        }
      }
    }
  }

}


//--------------------------------------------------------------------------------------------
/**
* @brief キャッシュされた地域名を取得する
*
*  @param simpleAddressId 簡易アドレス情報のID
*
* @return キャッシュされた地域名。
*/
//--------------------------------------------------------------------------------------------
const gfl2::str::STRCODE* RegionNameCache::GetRegionName( nn::cfg::CTR::SimpleAddressId& simpleAddressId ) const
{
  for( u32 i = 0; i < m_regionCount; ++i )
  {
    if( m_pSimpleAddressIdList[i].id == simpleAddressId.id )
    {
      return m_pRegionNameList[i];
    }
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
const gfl2::str::STRCODE* RegionNameCache::GetUnkownCountryName( void ) const
{
  return m_unknownCountryName;
}




GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )

#endif // defined( GF_PLATFORM_CTR )

