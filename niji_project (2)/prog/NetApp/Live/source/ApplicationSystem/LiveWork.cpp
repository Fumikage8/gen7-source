//==============================================================================
/**
 * @file   LiveWork.cpp
 * @date   2015/07/09 Thu. 15:01:15
 * @author muto_yu
 * @brief  Live大会Work
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <print/include/PrintSystem.h>
#include  <str/include/gfl2_MsgData.h>
#include  <arc_index/message.gaix>

#include  "Battle/Regulation/include/Regulation.h"
#include  "./LiveWork.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE  = 0x00400000;
  static const u32  DEVHEAP_SIZE  = 0x00a00000;

  static const u32  LOCALHEAP_APP_REGION_SIZE = 0x00010000;
  static const u32  LOCALHEAP_DEV_REGION_SIZE = 0x00080000;
} /*  namespace  */



/*
*/
namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     Live
  *  @brief    ctor
  */
//------------------------------------------------------------------
LiveWork::LiveWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_pTrainerIconUtil(NULL)
  , m_MessageUtility(GetAppHeap()->GetDeviceHeap(), 2)
  , m_pLiveViewContainer(NULL)
#if defined( GF_PLATFORM_CTR )
  , m_RegionHeap()
  , m_pCountryNameCache(NULL)
  , m_pRegionNameCache(NULL)
#endif
{
#if defined( GF_PLATFORM_CTR )
  m_RegionHeap.LocalHeapCreate( GetDeviceHeap(), GetDeviceHeap(), LOCALHEAP_APP_REGION_SIZE, LOCALHEAP_DEV_REGION_SIZE);
#endif

}


//------------------------------------------------------------------
/**
  *  @func     Live
  *  @brief    ctor
  */
//------------------------------------------------------------------
LiveWork::~LiveWork()
{
#if defined( GF_PLATFORM_CTR )
  GFL_SAFE_DELETE(m_pRegionNameCache);
  GFL_SAFE_DELETE(m_pCountryNameCache);
#endif
}





//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LiveWork::SetupCountryRegionNameCache(void)
{
#if defined( GF_PLATFORM_CTR )
  gfl2::heap::HeapBase* pHeapBase = GetDeviceHeap();

  m_pCountryNameCache = GFL_NEW(pHeapBase)  System::CTR::CountryNameCache();
  m_pCountryNameCache->LoadCountryName(m_RegionHeap.GetDeviceHeap(), static_cast<nn::cfg::CTR::CfgLanguageCode>( System::Language::ConvertPM2CTR(GetPokemonLanguageID())));

  m_pRegionNameCache  = GFL_NEW(pHeapBase)  System::CTR::RegionNameCache( m_pCountryNameCache->GetUnkownCountryName(), reinterpret_cast<u8*>(&m_RegionNameWork));
#endif
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u8    LiveWork::GetPokemonLanguageID(void) const
{
  u8 u8PokeLanguageID = System::GetLang();

  if( u8PokeLanguageID == POKEMON_LANG_CHINA
   || u8PokeLanguageID == POKEMON_LANG_TAIWAN )
  {
    u8PokeLanguageID = POKEMON_LANG_ENGLISH; // @fix NMCat[4117] 繁体字、簡体字だったら英語に置き換える
  }

  /*  Debugなどで切り替える場合はAppParamで受け取るなどする  */
  return u8PokeLanguageID;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
const gfl2::str::STRCODE* LiveWork::GetCountry(const Savedata::MyStatus* pTargetStatus)
{

#if defined( GF_PLATFORM_CTR )
  if(pTargetStatus)
  {
    return m_pCountryNameCache->GetCountryName(static_cast<nn::cfg::CfgCountryCode>(pTargetStatus->GetCountry()));
  }
#endif

  return L"";
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
const gfl2::str::STRCODE* LiveWork::GetRegion(const Savedata::MyStatus* pTargetStatus)
{

#if defined( GF_PLATFORM_CTR )
  if(pTargetStatus)
  {
    nn::cfg::CTR::SimpleAddressId saID;

    saID.id  = (static_cast<u32>(pTargetStatus->GetCountry()) << nn::cfg::CFG_SIMPLE_ADDRESS_ID_COUNTRY_SHIFT) | (static_cast<u32>(pTargetStatus->GetArea()) << nn::cfg::CFG_SIMPLE_ADDRESS_ID_REGION_SHIFT);
    m_pRegionNameCache->LoadRegionName( m_RegionHeap.GetDeviceHeap(), static_cast<nn::cfg::CTR::CfgLanguageCode>(System::Language::ConvertPM2CTR(GetPokemonLanguageID())), static_cast<nn::cfg::CfgCountryCode>(pTargetStatus->GetCountry()));

    return m_pRegionNameCache->GetRegionName(saID);
  }
#endif

  return L"";
}



} /*  namespace Live    */
} /*  namespace NetApp  */
