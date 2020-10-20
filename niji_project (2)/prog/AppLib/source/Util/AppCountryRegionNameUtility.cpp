// ============================================================================
/*
 * @file AppCountryRegionNameUtility.cpp
 * @brief 国名と全世界の国の地域名を取得する為のユーティリティクラスです。
 * @note  Heap が 2MB 必要です。
 *        内部でスレッドを作成して、そのスレッド内でロード処理を行います。
 * @date 2015.10.23
 * @author endo_akira
 */
// ============================================================================
#include "AppLib/include/Util/AppCountryRegionNameUtility.h"
#include "System/include/PokemonVersion.h"
#include "System/include/PokemonLanguage.h"
#include "System/include/HeapDefine.h"
#include "Print/include/PrintSystem.h"
#include <util/include/gfl2_std_string.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_illegalname.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

static const u32 STACK_SIZE = 4096;
static const int THREAD_PRIORITY = 18;

static const u32  NAME_HEAP_SIZE                = 1024 * 1024 *  2;   //   2 MB


#if defined( GF_PLATFORM_CTR )

CountryRegionNameLoadThread::CountryRegionNameLoadThread(
  gfl2::thread::CriticalSection&                     criticalSection,
  nn::cfg::CTR::CfgLanguageCode                      cfgLanguageCode,
  gfl2::heap::HeapBase*                              pHeap,
  System::CTR::CountryNameCache&                     countryNameCache,
  gfl2::util::List<System::CTR::RegionNameCache*>&   regionNameCacheList,
  u8*                                                pWorkBuffer,
  gfl2::str::MsgData*                                pIllegalNameMessageData ) :
  m_criticalSection( criticalSection ),
  m_CfgLanguageCode( cfgLanguageCode ),
  m_pHeap( pHeap ),
  m_CountryNameCache( countryNameCache ),
  m_RegionNameCacheList( regionNameCacheList ),
  m_pWorkBuffer( pWorkBuffer ),
  m_pIllegalNameMessageData( pIllegalNameMessageData ),
  m_bLoadEndRequest( false ),
  m_UnknownName()
{
}

CountryRegionNameLoadThread::~CountryRegionNameLoadThread()
{
}

void CountryRegionNameLoadThread::Function()
{
  GFL_PRINT( "Country and region name load start.\n" );

  GFL_PRINT( "m_CountryNameCache.LoadCountryName() m_CfgLanguageCode = %d\n", m_CfgLanguageCode );
  
  {
    gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

    m_CountryNameCache.LoadCountryName( m_pHeap, m_CfgLanguageCode, m_pIllegalNameMessageData );

    ::std::wcsncpy( m_UnknownName, m_CountryNameCache.GetUnkownCountryName(), nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
  }


  u32 listSize = 0;
  {
    gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

    listSize = m_RegionNameCacheList.MaxSize();
  }

  for( u32 i = 0; i < listSize; ++i )
  {
    nn::cfg::CTR::CfgCountryCode loadCountryCode = static_cast<nn::cfg::CTR::CfgCountryCode>(i);

    System::CTR::RegionNameCache* pRegionNameCach = GFL_NEW( m_pHeap ) System::CTR::RegionNameCache( m_UnknownName, m_pWorkBuffer );
    GFL_PRINT( "pRegionNameCach->LoadRegionName() m_CfgLanguageCode = %d, loadCountryCode = %d\n", m_CfgLanguageCode, loadCountryCode );
    pRegionNameCach->LoadRegionName( m_pHeap, m_CfgLanguageCode, loadCountryCode );
    {
      gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御
      m_RegionNameCacheList.PushBack( pRegionNameCach );
    }

    if( m_bLoadEndRequest )
    {
      break;
    }
  }

  GFL_PRINT( "Country and region name load end.\n" );
}

void CountryRegionNameLoadThread::LoadEndRequest()
{
  m_bLoadEndRequest = true;
}

#endif // defined( GF_PLATFORM_CTR )


AppCountryRegionNameUtility::AppCountryRegionNameUtility() :
#if defined( GF_PLATFORM_CTR )
  m_criticalSection(),
  m_pNameHeap( NULL ),
  m_pNameWorkBuffer( NULL ),
  m_CountryNameCache(),
  m_RegionNameCacheList(),
  m_pThread( NULL ),
  m_pCountryRegionNameLoadThread( NULL ),
  m_pIllegalNameMessageData( NULL ),
  m_CountryName(),
  m_RegionName()
#else
  m_NullString()
#endif // defined( GF_PLATFORM_CTR )
{
#if defined( GF_PLATFORM_CTR )

  m_criticalSection.Initialize();

  gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  m_pNameHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, NAME_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

  m_pNameWorkBuffer = GFL_NEW_ARRAY( m_pNameHeap ) u8[ nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE ];

  m_RegionNameCacheList.CreateBuffer( m_pNameHeap, static_cast<u32>( nn::cfg::CTR::CFG_COUNTRY_MAX ) );

#endif // defined( GF_PLATFORM_CTR )
}


AppCountryRegionNameUtility::~AppCountryRegionNameUtility()
{
#if defined( GF_PLATFORM_CTR )

  for( gfl2::util::List<System::CTR::RegionNameCache*>::Iterator it = m_RegionNameCacheList.Begin(); it != m_RegionNameCacheList.End(); ++it )
  {
    GFL_SAFE_DELETE( *it );
  }
  m_RegionNameCacheList.Clear();

  GFL_SAFE_DELETE_ARRAY( m_pNameWorkBuffer );

  gfl2::heap::Manager::DeleteHeap( m_pNameHeap );

  m_criticalSection.Finalize();

#endif // defined( GF_PLATFORM_CTR )
}


void AppCountryRegionNameUtility::LoadStart()
{
#if defined( GF_PLATFORM_CTR )
  if( m_pThread == NULL )
  {
    u8 u8PokeLanguageID = System::GetLang();

    if( u8PokeLanguageID == POKEMON_LANG_CHINA
     || u8PokeLanguageID == POKEMON_LANG_TAIWAN )
    {
      u8PokeLanguageID = POKEMON_LANG_ENGLISH; // @fix NMCat[4117] 繁体字、簡体字だったら英語に置き換える
    }

    nn::cfg::CTR::CfgLanguageCode cfgLanguageCode = static_cast<nn::cfg::CTR::CfgLanguageCode>( System::Language::ConvertPM2CTR( u8PokeLanguageID ) );

    m_pIllegalNameMessageData = GFL_NEW( m_pNameHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_illegalname_DAT, m_pNameHeap, gfl2::str::MsgData::LOAD_FULL );
    m_pCountryRegionNameLoadThread = GFL_NEW( m_pNameHeap ) app::util::CountryRegionNameLoadThread( m_criticalSection, cfgLanguageCode, m_pNameHeap, m_CountryNameCache, m_RegionNameCacheList, m_pNameWorkBuffer, m_pIllegalNameMessageData );
    m_pThread = GFL_NEW( m_pNameHeap ) gfl2::thread::Thread( m_pCountryRegionNameLoadThread, m_pNameHeap, false, STACK_SIZE );
    m_pThread->Start( THREAD_PRIORITY );
  }
#endif // defined( GF_PLATFORM_CTR )
}


void AppCountryRegionNameUtility::Update()
{
#if defined( GF_PLATFORM_CTR )
  if( m_pThread != NULL )
  {
    bool bIsAlive = m_pThread->IsAlive();

    if( !bIsAlive )
    {
      m_pThread->Kill();

      GFL_SAFE_DELETE( m_pThread );
      GFL_SAFE_DELETE( m_pCountryRegionNameLoadThread );
      GFL_SAFE_DELETE( m_pIllegalNameMessageData );

      GFL_PRINT( "----------------------------------------------\n" );
      {
        gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御
        GFL_PRINT( "Region name list size = %d \n", m_RegionNameCacheList.Size() );
      }

      GFL_PRINT( "----------------------------------------------\n" );
      GFL_PRINT( "----- Name Heap -----\n" );
      GFL_PRINT( "Total size                = %d\n", m_pNameHeap->GetTotalSize() );
      GFL_PRINT( "Total free size           = %d\n", m_pNameHeap->GetTotalFreeSize() );
      GFL_PRINT( "Total allocatable size    = %d\n", m_pNameHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "Memory fragmentation size = %d\n", m_pNameHeap->GetTotalFreeSize() - m_pNameHeap->GetTotalAllocatableSize() );
      GFL_PRINT( "Use memory size           = %d\n", m_pNameHeap->GetTotalSize() - m_pNameHeap->GetTotalFreeSize() );
      GFL_PRINT( "----------------------------------------------\n" );
    }
  }
#endif // defined( GF_PLATFORM_CTR )
}


void AppCountryRegionNameUtility::LoadEndRequest()
{
#if defined( GF_PLATFORM_CTR )
  if( m_pCountryRegionNameLoadThread )
  {
    m_pCountryRegionNameLoadThread->LoadEndRequest();
  }
#endif // defined( GF_PLATFORM_CTR )
}


bool AppCountryRegionNameUtility::IsLoading()
{
#if defined( GF_PLATFORM_CTR )
  if( m_pThread )
  {
    return true;
  }
#endif // defined( GF_PLATFORM_CTR )
  return false;
}


const gfl2::str::STRCODE* AppCountryRegionNameUtility::GetCountryName( u16 countryCode )
{
#if defined( GF_PLATFORM_CTR )

  nn::cfg::CTR::SimpleAddressId simpleAddressId;

  u16 regionCode = 0;

  simpleAddressId.id = ( ( countryCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_COUNTRY_SHIFT ) |
                          ( regionCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_REGION_SHIFT ) );

  return GetCountryName( simpleAddressId );

#else
  
  return m_NullString;

#endif
}

#if defined( GF_PLATFORM_CTR )
const gfl2::str::STRCODE* AppCountryRegionNameUtility::GetCountryName( nn::cfg::SimpleAddressId simpleAddressID )
{
  {
    gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

    ::std::wcsncpy( m_CountryName, m_CountryNameCache.GetCountryName( simpleAddressID.GetCountryCode() ), nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
  }

  return m_CountryName;
}
#endif // defined( GF_PLATFORM_CTR )


const gfl2::str::STRCODE* AppCountryRegionNameUtility::GetRegionName( u16 countryCode, u16 regionCode )
{
#if defined( GF_PLATFORM_CTR )

  nn::cfg::CTR::SimpleAddressId simpleAddressId;

  simpleAddressId.id = ( ( countryCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_COUNTRY_SHIFT ) |
                          ( regionCode << nn::cfg::CTR::CFG_SIMPLE_ADDRESS_ID_REGION_SHIFT ) );

  return GetRegionName( simpleAddressId );

#else

  return m_NullString;

#endif
}


#if defined( GF_PLATFORM_CTR )
const gfl2::str::STRCODE* AppCountryRegionNameUtility::GetRegionName( nn::cfg::SimpleAddressId simpleAddressID )
{
  if( simpleAddressID.GetCountryCode() < nn::cfg::CTR::CFG_COUNTRY_MAX )
  {
    u32 count = 0;
    gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御
    for( gfl2::util::List<System::CTR::RegionNameCache*>::Iterator it = m_RegionNameCacheList.Begin(); it != m_RegionNameCacheList.End(); ++it )
    {
      if( static_cast<nn::cfg::CTR::CfgCountryCode>( count ) == simpleAddressID.GetCountryCode() )
      {
        ::std::wcsncpy( m_RegionName, (*it)->GetRegionName( simpleAddressID ), nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
    
        return m_RegionName;
      }
      ++count;
    }
  }

  gfl2::std::MemClear( m_RegionName, sizeof( m_RegionName ) );

  return m_RegionName;
}
#endif // defined( GF_PLATFORM_CTR )


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


