// ============================================================================
/*
 * @file AppCountryRegionNameUtility.h
 * @brief 国名と全世界の国の地域名を取得する為のユーティリティクラスです。
 * @note  HEAPID_APP_DEVICE から 2MB 使用します。
 *        内部でスレッドを作成して、そのスレッド内でロード処理を行います。
 * @date 2015.10.23
 * @author endo_akira
 */
// ============================================================================
#if !defined( APPCOUNTRYREGIONNAMEUTILITY_H_INCLUDE )
#define APPCOUNTRYREGIONNAMEUTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ScopedLock.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_StrBuf.h>

#if defined( GF_PLATFORM_CTR )
#include "System/include/CTR/CTR_CountryNameCache.h"
#include "System/include/CTR/CTR_RegionNameCache.h"
#endif // defined( GF_PLATFORM_CTR )


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


#if defined( GF_PLATFORM_CTR )
class CountryRegionNameLoadThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CountryRegionNameLoadThread );
public:
  CountryRegionNameLoadThread(
    gfl2::thread::CriticalSection&                     criticalSection,
    nn::cfg::CTR::CfgLanguageCode                      cfgLanguageCode,
    gfl2::heap::HeapBase*                              pHeap,
    System::CTR::CountryNameCache&                     countryNameCache,
    gfl2::util::List<System::CTR::RegionNameCache*>&   regionNameCacheList,
    u8*                                                pWorkBuffer,
    gfl2::str::MsgData*                                pIllegalNameMessageData );
  virtual~CountryRegionNameLoadThread();
  virtual void Function();
  void LoadEndRequest();
private:
  gfl2::thread::CriticalSection&                    m_criticalSection;
  nn::cfg::CTR::CfgLanguageCode                     m_CfgLanguageCode;
  gfl2::heap::HeapBase*                             m_pHeap;
  System::CTR::CountryNameCache&                    m_CountryNameCache;
  gfl2::util::List<System::CTR::RegionNameCache*>&  m_RegionNameCacheList;
  u8*                                               m_pWorkBuffer;
  gfl2::str::MsgData*                               m_pIllegalNameMessageData;
  bool                                              m_bLoadEndRequest;
  gfl2::str::STRCODE                                m_UnknownName[ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH ];

};
#endif // defined( GF_PLATFORM_CTR )


class AppCountryRegionNameUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( AppCountryRegionNameUtility );

public:

  AppCountryRegionNameUtility();
  virtual~AppCountryRegionNameUtility();

  //--------------------------------------------------------------------------------------------
  /**
    * @brief スレッドを作成してロードを開始します
    */
  //--------------------------------------------------------------------------------------------
  void LoadStart();

  //--------------------------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //--------------------------------------------------------------------------------------------
  void Update();

  //--------------------------------------------------------------------------------------------
  /**
    * @brief ロードスレッドの終了リクエストを投げます
    */
  //--------------------------------------------------------------------------------------------
  void LoadEndRequest();

  //--------------------------------------------------------------------------------------------
  /**
    * @brief ロード中か判定します
    *
    * @retval true ロードしています
    *
    * @retval false ロードしていません
    */
  //--------------------------------------------------------------------------------------------
  bool IsLoading();

  //--------------------------------------------------------------------------------------------
  /**
    * @brief キャッシュされた国名を取得する
    *
    * @param countryCode 取得対象となる国コード
    *
    * @note  u16 で受け取っていますが、nn::cfg::CTR::CfgCountryCode 型にキャストして使用します
    *
    * @return キャッシュされた国名。
    */
  //--------------------------------------------------------------------------------------------
  const gfl2::str::STRCODE* GetCountryName( u16 countryCode );
#if defined( GF_PLATFORM_CTR )
  const gfl2::str::STRCODE* GetCountryName( nn::cfg::SimpleAddressId simpleAddressID );
#endif // defined( GF_PLATFORM_CTR )

  //--------------------------------------------------------------------------------------------
  /**
    * @brief キャッシュされた地域名を取得する
    *
    * @param countryCode 取得対象となる国コード
    *
    * @param regionCode 取得対象となる地域コード
    *
    * @note  countryCode と regionCode から nn::cfg::CTR::SimpleAddressId を作って使用します
    *
    * @return キャッシュされた地域名。
    */
  //--------------------------------------------------------------------------------------------
  const gfl2::str::STRCODE* GetRegionName( u16 countryCode, u16 regionCode );
#if defined( GF_PLATFORM_CTR )
  const gfl2::str::STRCODE* GetRegionName( nn::cfg::SimpleAddressId simpleAddressID );
#endif // defined( GF_PLATFORM_CTR )

private:
  

#if defined( GF_PLATFORM_CTR )
  gfl2::thread::CriticalSection                                   m_criticalSection;
  gfl2::heap::HeapBase*                                           m_pNameHeap;
  u8*                                                             m_pNameWorkBuffer;
  System::CTR::CountryNameCache                                   m_CountryNameCache;
  gfl2::util::List<System::CTR::RegionNameCache*>                 m_RegionNameCacheList;
  gfl2::thread::Thread*                                           m_pThread;
  app::util::CountryRegionNameLoadThread*                         m_pCountryRegionNameLoadThread;
  gfl2::str::MsgData*                                             m_pIllegalNameMessageData;

  gfl2::str::STRCODE                                              m_CountryName[ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH ];
  gfl2::str::STRCODE                                              m_RegionName[ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH ];

#else
  gfl2::str::STRCODE                                              m_NullString[4];
#endif // defined( GF_PLATFORM_CTR )



};

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif // APPCOUNTRYREGIONNAMEUTILITY_H_INCLUDE

