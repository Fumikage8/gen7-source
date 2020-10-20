
#if !defined(PHOTOVIEWWORK_H_INCLUDED)
#define PHOTOVIEWWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewWork.h
 * @date   2017/02/07 Tue. 13:18:49
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
#include  <NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h>


#include  "../../include/PhotoViewAppParam.h"
#include  "../Views/PhotoViewViewContainer.h"
#include  "../PhotoViewResource.h"


namespace NetApp  {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/07 Tue. 13:19:25
  */
//==============================================================================
class PhotoViewWork
  : public  NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoViewWork );

public:
  //------------------------------------------------------------------
  /**
    *  @func     PhotoViewWork
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  PhotoViewWork(void);

  //------------------------------------------------------------------
  /**
    *  @func     PhotoViewWork
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  virtual ~PhotoViewWork();

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void                SetViewContainer(PhotoViewViewContainer* pViewContainer) {m_pPhotoViewViewContainer = pViewContainer;}
  PhotoViewViewContainer*  GetViewContainer(void)                              {return m_pPhotoViewViewContainer;}




  /*
    message
  */
  NetAppLib::Message::MessageUtility&  GetMessageUtility(void)   {return m_MessageUtility;}

  gfl2::str::MsgData*       GetMessageData(const gfl2::fs::ArcFile::ARCDATID garcID = MESSAGE_GRAC_ID) {
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


  /*
    members
  */
  APP_PARAM*                                  m_pAppParam;
  NetAppLib::Util::NetAppTrainerIconUtility*  m_pTrainerIconUtil;

private:
  /*  message  */
  NetAppLib::Message::MessageUtility          m_MessageUtility;
  PhotoViewViewContainer*                     m_pPhotoViewViewContainer;
};




} /*  namespace PhotoView */
} /*  namespace NetApp      */
#endif  /*  #if !defined(PHOTOVIEWWORK_H_INCLUDED)  */

