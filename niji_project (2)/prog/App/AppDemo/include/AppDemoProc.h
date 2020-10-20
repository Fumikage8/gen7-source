#if !defined(APPDEMOPROC_H_INCLUDED)
#define APPDEMOPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   AppDemoProc.h
 * @date   2015/08/17 Mon. 17:23:18
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  "GameSys/include/GameProc.h"
#include  "AppLib/include/Frame/AppFrameManager.h"
#include  "AppLib/include/Util/app_util_heap.h"

#include  "./AppDemoUtility.h"
#include  "./AppDemoProcParam.h"


namespace App {
namespace AppDemo {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  AppDemoProc
  *  @brief  シーケンスデモ再生
  *  @date   2015/08/17 Mon. 17:38:55
  */
//==============================================================================
class AppDemoProc
  : public GameSys::GameProc
  , AppDemoUtility::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc );

public:
  AppDemoProc(const AppDemoProcParam* pParam);
  virtual ~AppDemoProc();

  /*  GameProc  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void               DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

protected:
  /*
    派生側でのカスタマイズポイント
  */
  virtual AppDemoUtility::HeapModel  GetSequenceDemoHeapModel(void) const  {return AppDemoUtility::HEAPMODEL_Full;}
  virtual u32                        AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void                       AppDemoUtility_OnSequenceEnd(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void                       AppDemoUtility_CancelQuery(void) { }

  AppDemoProcParam  m_Param;      /**<  起動パラメータ  */
  AppDemoUtility*   m_pDemoUtil;
};


//==============================================================================
/**
  *  @class  
  *  @brief  おくりもの受信デモ
  *  @date   2015/08/19 Wed. 16:00:01
  */
//==============================================================================
class AppDemoProc_Gift
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_Gift );
public:
  AppDemoProc_Gift(const AppDemoProcParam_Gift* pParam);
  virtual ~AppDemoProc_Gift();

protected:
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual AppDemoUtility::HeapModel  GetSequenceDemoHeapModel(void) const  {return AppDemoUtility::HEAPMODEL_Half;}
  AppDemoProcParam_Gift     m_ParamGift;
};


//==============================================================================
/**
  *  @class  
  *  @brief  進化デモ
  *  @date   2015/08/25 Tue. 18:24:44
  */
//==============================================================================
class AppDemoProc_Evolution
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_Evolution );
public:
  AppDemoProc_Evolution(const AppDemoProcParam_Evolution* pParam);
  virtual ~AppDemoProc_Evolution();


  enum  EventCode
  {
    EVENT_EvolutionCancelRequest,
    EVENT_SyncSignal,
    
    EVENT_
  };
  void  SendEvent(const EventCode eventCode);


protected:
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  AppDemoProcParam_Evolution  m_ParamEvolution;
};


//==============================================================================
/**
  *  @class  
  *  @brief  GTSデモ
  *  @date   2015/08/27 Tue. 18:24:44
  */
//==============================================================================
class AppDemoProc_GTS
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_GTS );
public:
  AppDemoProc_GTS(const AppDemoProcParam_GTS* pParam);
  virtual ~AppDemoProc_GTS();

protected:
  virtual AppDemoUtility::HeapModel  GetSequenceDemoHeapModel(void) const;
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  AppDemoProcParam_GTS  m_ParamGTS;
};


//==============================================================================
/**
  *  @class  
  *  @brief  ミラクル交換デモ
  *  @date   2015/08/27 Tue. 18:24:44
  */
//==============================================================================
class AppDemoProc_MiracleTrade
  : public AppDemoProc
  , DemoLib::Sequence::SequenceCallBack
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_MiracleTrade );
public:
  AppDemoProc_MiracleTrade(const AppDemoProcParam_MiracleTrade* pParam);
  virtual ~AppDemoProc_MiracleTrade();

  virtual void SequenceCallBackFunc(int command, int paramA, int paramB);

private:
  bool compare( pml::pokepara::PokemonParam* pPokemonParam, Savedata::MyStatus* pMyStatus );

protected:
  virtual AppDemoUtility::HeapModel  GetSequenceDemoHeapModel(void) const;
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  AppDemoProcParam_MiracleTrade             m_ParamMiracleTrade;
  DemoLib::Sequence::SequenceViewSystem*    m_pViewSystem;
};


//==============================================================================
/**
  *  @class  
  *  @brief  おめでとうデモ
  *  @date   2016/01/18
  */
//==============================================================================
class AppDemoProc_Congrats
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_Congrats );
public:
  AppDemoProc_Congrats(const AppDemoProcParam_Congrats* pParam);
  virtual ~AppDemoProc_Congrats();

protected:
  virtual AppDemoUtility::HeapModel  GetSequenceDemoHeapModel(void) const  {return AppDemoUtility::HEAPMODEL_Dendou;}
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);
  virtual void  AppDemoUtility_CancelQuery(void);

  AppDemoProcParam_Congrats  m_ParamCongras;
};


//==============================================================================
/**
  *  @brief  VSデモ
  *  @date   2016/01/21
  *  @auther  ichiraku
  */
//==============================================================================
class AppDemoProc_VS
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_VS );
public:
  AppDemoProc_VS(const AppDemoProcParam_VS* pParam);
  virtual ~AppDemoProc_VS();

protected:
  virtual u32   AppDemoUtility_StartQuery(DemoLib::Sequence::SequenceViewSystem* pViewSystem);

  AppDemoProcParam_VS  m_ParamVS;
};


//=============================================================================
/**
 * @brief   イントロデモ後半
 * @date    2016/03/02
 * @auther  Hiroyuki Nakamura
 */
//=============================================================================
class AppDemoProc_Intro2nd
  : public AppDemoProc
{
  GFL_FORBID_COPY_AND_ASSIGN( AppDemoProc_Intro2nd );
public:
  AppDemoProc_Intro2nd( const AppDemoProcParam_Intro2nd * pParam );
  virtual ~AppDemoProc_Intro2nd();

protected:
  virtual u32 AppDemoUtility_StartQuery( DemoLib::Sequence::SequenceViewSystem * pViewSystem );

  AppDemoProcParam_Intro2nd m_paramIntro2nd;
};


} /*  namespace AppDemo */
} /*  namespace App     */
#endif  /*  #if !defined(APPDEMOPROC_H_INCLUDED)  */
