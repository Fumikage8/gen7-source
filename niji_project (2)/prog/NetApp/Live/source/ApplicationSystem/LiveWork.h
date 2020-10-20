#if !defined(LIVEWORK_H_INCLUDED)
#define LIVEWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveWork.h
 * @date   2015/07/09 Thu. 15:14:43
 * @author muto_yu
 * @brief  Live大会work
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <str/include/gfl2_Str.h>
#include  <arc_index/message.gaix>
#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h>
#include  <NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h>

/*  country/region  */
#include  "System/include/PokemonVersion.h"
#include  "System/include/CTR/CTR_CountryNameCache.h"
#include  "System/include/CTR/CTR_RegionNameCache.h"


#include  "../../include/LiveAppParam.h"
#include  "../Views/LiveViewContainer.h"
#include  "../LiveResource.h"


class Regulation;

namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  LiveWork
  *  @brief  Live大会用work
   * @date   2015/07/09 Thu. 15:14:43
  */
//==============================================================================
class LiveWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( LiveWork );

public:
  //------------------------------------------------------------------
  /**
    *  @func     LiveWork
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LiveWork(void);

  //------------------------------------------------------------------
  /**
    *  @func     LiveWork
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  virtual ~LiveWork();

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void                SetViewContainer(LiveViewContainer* pViewContainer) {m_pLiveViewContainer = pViewContainer;}
  LiveViewContainer*  GetViewContainer(void)                              {return m_pLiveViewContainer;}




  /*
    message
  */
  NetAppLib::Message::MessageUtility&  GetMessageUtility(void)   {return m_MessageUtility;}

  gfl2::str::MsgData*       GetMessageData(const gfl2::fs::ArcFile::ARCDATID garcID = LIVE_MESSAGE_GRAC_ID) {
                              return GetMessageUtility().GetData(garcID);
                            }

  gfl2::str::StrBuf*        GetString(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID) {
                              if(IsValidMessageID(garcID, messageID))
                              {
                                return &(m_MessageUtility.GetString(garcID, messageID));
                              }
                              return NULL;
                            }
  
  print::WordSet*           GetWordSet(void)  {
                              return GetMessageUtility().GetWordSet();
                            }

  bool                      IsValidMessageID(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID)
                            {
                              return (messageID <= GetMessageData()->GetStrCount());
                            }


  /*  country/region  */
  void  SetupCountryRegionNameCache(void);
  u8    GetPokemonLanguageID(void) const;
  const gfl2::str::STRCODE* GetCountry(const Savedata::MyStatus* pTargetStatus);
  const gfl2::str::STRCODE* GetRegion(const Savedata::MyStatus* pTargetStatus);



  /*
    members
  */
  APP_PARAM*                                  m_pAppParam;
  NetAppLib::Util::NetAppTrainerIconUtility*  m_pTrainerIconUtil;

private:
  /*  message  */
  NetAppLib::Message::MessageUtility          m_MessageUtility;
  LiveViewContainer*                          m_pLiveViewContainer;

#if defined( GF_PLATFORM_CTR )
  app::util::Heap                 m_RegionHeap;
  System::CTR::CountryNameCache*  m_pCountryNameCache;   /**<  全ての国名          */
  System::CTR::RegionNameCache*   m_pRegionNameCache;    /**<  ある国の全ての国名  */
  u32                             m_RegionNameWork[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE / sizeof(u32)];
#endif
};




} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVEWORK_H_INCLUDED)  */

