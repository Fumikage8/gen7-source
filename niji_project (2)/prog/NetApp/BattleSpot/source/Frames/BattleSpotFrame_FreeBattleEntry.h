#if !defined(BATTLESPOTFRAME_FREEBATTLEENTRY_H_INCLUDED)
#define BATTLESPOTFRAME_FREEBATTLEENTRY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_FreeBattleEntry.h
 * @date   2015/10/02 Fri. 13:32:33
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
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"
#include  "ExtSavedata/include/RegulationExtSaveData.h"

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>




namespace NetApp    {
namespace BattleSpot {
  class BattleSpotWork;
  class BattleSpotViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief
  */
//==============================================================================
class Frame_FreeBattleEntry
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
#if defined( GF_PLATFORM_CTR )
  , public gflnet2::boss::BossEventListener
#endif // defined( GF_PLATFORM_CTR )
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_FreeBattleEntry );

private:

  enum
  {
    REGSEQ_INIT = 0,
    REGSEQ_WAIT,
    REGSEQ_DELETED,
    REGSEQ_UPDATE,
    REGSEQ_RETURN_WAIT,
    REGSEQ_ERROR,
    REGSEQ_SD_FULL,
    REGSEQ_OK
  };

  enum
  {
    REGULATION_SINGLE_L = 0,
    REGULATION_DOUBLE_L,
    REGULATION_ROYAL_L,
    REGULATION_SINGLE,
    REGULATION_DOUBLE,
    REGULATION_ROYAL,
    REGULATION_NUM,
  };

public:
  Frame_FreeBattleEntry(BattleSpotWork* pWork);
  virtual ~Frame_FreeBattleEntry();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);



  void  ExitSelf(const FrameResult frameResult = RESULT_FreeBattleEntry_Back);

  bool UpdateRegulation();

#ifdef  GF_PLATFORM_CTR
  // BossEventListener
  virtual void OnBossNewArrivalData( const u32 serialId , const bool isFinalData );
  virtual void OnBossNewArrivalDataEmpty( void );
  virtual bool OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody );
  virtual void OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );
  virtual void OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result );
  virtual void OnBossError( const nn::Result& result );
  virtual void OnBossTaskError( const char* pTaskName , const u32 errorCode );

#endif
  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
  UpperView_Information*           m_pUpperView;
  LowerView_Selection*      m_pLowerView;

  u32 m_regSeq;
  u32 m_SerialID;

  Regulation* m_pRegulation[REGULATION_NUM];
  Regulation* m_pTempRegulation;
  ExtSavedata::RegulationExtSaveData       m_RegulationExtSaveData;

  bool m_isDeleteComplete;
  bool m_isNewExists;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_FREEBATTLEENTRY_H_INCLUDED)  */
