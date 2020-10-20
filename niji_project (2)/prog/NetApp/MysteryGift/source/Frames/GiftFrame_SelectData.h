#if !defined(GIFTFRAME_SELECTDATA_H_INCLUDED)
#define GIFTFRAME_SELECTDATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_SelectData.h
 * @date   2015/08/05 Wed. 16:36:19
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
#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../GiftFrameDef.h"

#include  "../Views/GiftViewContainer.h"

namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_SelectData
  *  @brief  Gift大会Frame:main
  */
//==============================================================================
class Frame_SelectData
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_SelectData::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_SelectData );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_SelectData
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_SelectData(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_SelectData
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_SelectData();



  //------------------------------------------------------------------
  /**
    *  @func     RefreshList
    *  @brief    workの内容でListを再構築
    */
  //------------------------------------------------------------------
  void  RefreshList(void);



private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  OnExit(void);
  void  OnStartSave(void);
  void  ExitFrame(const FrameResult = RESULT_SELECTDATA_Exit);
  void  ReceiveCheck(void);
  void  AuthHelperResultHandling(AuthenticationHelper::CommitSaveResult helperResult);

  /*  implement LowerView_SelectData::IEventHandler  */
  virtual void  LowerView_SelectData_OnEvent(const LowerView_SelectData::IEventHandler::EventCode );
  /*  endimplement LowerView_SelectData::IEventHandler  */



  /*
    members
  */
  GiftWork*               m_pWork;
  GiftViewContainer*      m_pViewContainer;
  u32                     m_State;
  FrameResult             m_Result;
  bool                    m_IsCommitSaveStarted;
  
  
  AuthenticationHelper::CommitSaveResult  m_CommitSaveResult;

  UpperView_Message*      m_pUpperView_Message;
  LowerView_SelectData*   m_pLowerView_SelectData;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_SELECTDATA_H_INCLUDED)  */
