
#if !defined(GIFTWORK_H_INCLUDED)
#define GIFTWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftWork.h
 * @date   2015/07/31 Fri. 15:15:17
 * @author muto_yu
 * @brief  
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
#include  <NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h>
#include  <AppLib/include/Frame/AppFrameManager.h>

#include  "Savedata/include/MysteryGiftSave.h"
#include  "../Common/AuthenticationHelper.h"
#include  "../Views/GiftViewContainer.h"
#include  "../../include/GiftAppParam.h"

#include  "../Debug/GiftDebug.h"

namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  GiftWork
  *  @brief  Gift大会用work
   * @date   2015/07/09 Thu. 15:14:43
  */
//==============================================================================
class GiftWork
  : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GiftWork );

public:
  void                SetViewContainer(GiftViewContainer* pViewContainer) {m_pGiftViewContainer = pViewContainer;}
  GiftViewContainer*  GetViewContainer(void)                              {return m_pGiftViewContainer;}







  /*
    message
  */
  NetAppLib::Message::MessageUtility&  GetMessageUtility(void)   {return m_MessageUtility;}

  gfl2::str::MsgData*       GetMessageData(const gfl2::fs::ArcFile::ARCDATID garcID = GARC_message_mystery_DAT) {
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
                              return (messageID <= GetMessageData(garcID)->GetStrCount());
                            }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @return   社内fatalか（SDKfatalとは違い、タイトルアプリに制御が戻り、専用のFatalEventで停止させる。）
    */
  //------------------------------------------------------------------
  bool  NetworkErrorHandling(void);
  bool  IsInternalNetworkFatalErrorOccured(void) const  {return m_IsInternalFatalErrorOccured;}



  /*
    pokemodel
  */
  NetAppLib::Util::NetAppPokeModelUtility*    m_pPokeModelUtil;
  APP_PARAM*                                  m_pAppParam;

  /*  numberInput呼び出し用  */
  applib::frame::Manager*                     m_pAppFrameManager;

  AuthenticationHelper*                       m_pAuthHelper;
  bool                                        m_IsReceiveViaSerialAuth;

  bool                                        m_bCleanupMode;
  bool                                        m_bNoHandlingNetworkError;

#if PM_DEBUG
  Debug::GiftDebug*                           m_pGiftDebug;
#endif






  GiftWork(void);
  virtual ~GiftWork();
private:
  /*  message  */
  NetAppLib::Message::MessageUtility  m_MessageUtility;
  GiftViewContainer*                  m_pGiftViewContainer;
  bool                                m_IsInternalFatalErrorOccured;
};




} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTWORK_H_INCLUDED)  */

