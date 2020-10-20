#if !defined(LOWERVIEW_SELECTINSTRUCTION_H_INCLUDED)
#define LOWERVIEW_SELECTINSTRUCTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_SelectInstruction.h
 * @date   2016/04/04 Mon. 20:01:32
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

#include  <AppLib/include/Tool/GeneralView/GeneralLowerView.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppEmptyView.h>

namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/04/04 Mon. 20:29:36
  */
//==============================================================================
class LowerView_SelectInstruction
  : public    NetAppLib::UI::NetAppEmptyView
  , protected app::tool::GeneralLowerView::IEventHandler
  , protected app::tool::IResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerView_SelectInstruction);
public:

  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*  instruction  */
      EVENT_AboutGift,
      EVENT_AboutAlbum,
      EVENT_AboutAlbumFull,
      
      EVENT_Exit,
      
      EVENT_NONE
    };

    virtual void  LowerView_SelectInstruction_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  void  Reset(void);



  /*  ctor/dtor  */
  LowerView_SelectInstruction(GiftWork* pWork);
  virtual ~LowerView_SelectInstruction();

  virtual void OnSetInputEnabled( bool isEnable );


protected:
  virtual void    GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode);
  virtual void*   IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID);

  void  Event(const IEventHandler::EventCode eventCode);


  GiftWork*                     m_pWork;
  IEventHandler*                m_pHandler;
  
  app::tool::GeneralLowerView*  m_pGeneralLowerView;
};







} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_SELECTINSTRUCTION_H_INCLUDED)  */
