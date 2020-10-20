//===================================================================
/**
 * @file    CTR_CountryNameCache.h
 * @brief   国名をキャッシュ管理するためのクラス
*/
//===================================================================
#if defined( GF_PLATFORM_CTR )
#if !defined( CTR_COUNTRY_NAME_CACHE_H_INCLUDE )
#define CTR_COUNTRY_NAME_CACHE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_MsgData.h>
#include <nn/cfg.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( CTR )


//--------------------------------------------------------------------------------------------
/**
* @brief 国名キャシュ管理用クラス
*/
//--------------------------------------------------------------------------------------------
class CountryNameCache
{
  GFL_FORBID_COPY_AND_ASSIGN( CountryNameCache );

public:

  //--------------------------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //--------------------------------------------------------------------------------------------
  CountryNameCache( void );
    
  //--------------------------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //--------------------------------------------------------------------------------------------
  virtual ~CountryNameCache( void );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief 言語コードを指定して、国名をキャッシュする
    *
    * @param devHeap     一時的に使用するデバイスヒープ
    * @param langCode    ロード対象となる言語コード
    *
    */
  //--------------------------------------------------------------------------------------------
  void LoadCountryName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode );
  void LoadCountryName( gfl2::heap::HeapBase* devHeap, nn::cfg::CTR::CfgLanguageCode langCode, gfl2::str::MsgData* pIllegalNameMessageData );

  //--------------------------------------------------------------------------------------------
  /**
    * @brief キャッシュされた国名を取得する
    *
    * @param countryCode 取得対象となる国コード
    *
    * @return キャッシュされた国名。
    */
  //--------------------------------------------------------------------------------------------
  const gfl2::str::STRCODE* GetCountryName( nn::cfg::CTR::CfgCountryCode countryCode ) const;

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
    * @brief 国名のリストを初期化します
    */
  //--------------------------------------------------------------------------------------------
  void initialize( void );


private:

  gfl2::str::STRCODE                m_countryNameList[nn::cfg::CTR::CFG_COUNTRY_MAX][nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH]; //!< 国名情報
  gfl2::str::STRCODE                m_unknownCountryName[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH];                             //!< 不明な国名

};



GFL_NAMESPACE_END( CTR )
GFL_NAMESPACE_END( System )


#endif // CTR_COUNTRY_NAME_CACHE_H_INCLUDE
#endif // defined( GF_PLATFORM_CTR )

