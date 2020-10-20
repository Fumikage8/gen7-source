//===================================================================
/**
 * @file    CTR_RegionNameCache.h
 * @brief   地域名をキャッシュ管理するためのクラス
*/
//===================================================================
#if defined( GF_PLATFORM_CTR )
#if !defined( CTR_REGION_NAME_CACHE_H_INCLUDE )
#define CTR_REGION_NAME_CACHE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrSys.h>
#include <nn/cfg.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )


//--------------------------------------------------------------------------------------------
/**
* @brief 地域名キャシュ管理用クラス
*/
//--------------------------------------------------------------------------------------------
class RegionNameCache
{
  GFL_FORBID_COPY_AND_ASSIGN( RegionNameCache );

public:

  //--------------------------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //--------------------------------------------------------------------------------------------
  RegionNameCache( const gfl2::str::STRCODE* pUnknownCountryName, u8* pWorkBuffer );
    
  //--------------------------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //--------------------------------------------------------------------------------------------
  virtual ~RegionNameCache( void );

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
  void LoadRegionName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode, nn::cfg::CTR::CfgCountryCode countryCode );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief キャッシュされた地域名を取得する
    *
    * @param simpleAddressId 簡易アドレス情報のID
    *
    * @return キャッシュされた地域名。
    */
  //--------------------------------------------------------------------------------------------
  const gfl2::str::STRCODE* GetRegionName( nn::cfg::CTR::SimpleAddressId& simpleAddressId ) const;

  //--------------------------------------------------------------------------------------------
  /**
    * @brief 不明な国名に表示するための文字列「？？？」を取得する
    *
    * @return 不明な国名に表示するための文字列
    */
  //--------------------------------------------------------------------------------------------
  const gfl2::str::STRCODE* GetUnkownCountryName( void ) const;

private:

  //--------------------------------------------------------------------------------------------
  /**
    * @brief 地域名のリストを破棄します
    */
  //--------------------------------------------------------------------------------------------
  void terminate( void );

private:

  uptr                              m_pWorkBuffer;
  gfl2::str::STRCODE                m_unknownCountryName[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH];                             //!< 不明な国名
  gfl2::str::STRCODE(*              m_pRegionNameList)[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH];                               //!< 指定した国の地域名
  nn::cfg::CTR::SimpleAddressId*    m_pSimpleAddressIdList;
  u32                               m_regionCount;                                                                                  //!< 地域の数

};



GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )


#endif // CTR_REGION_NAME_CACHE_H_INCLUDE
#endif // defined( GF_PLATFORM_CTR )

