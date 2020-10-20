#if !defined(NUMBERCONTROLLER_H_INCLUDED)
#define NUMBERCONTROLLER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NumberController.h
 * @date   2016/11/16 Wed. 15:18:08
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
#include  <math/include/gfl2_MathCommon.h>

#include  <Layout/include/gfl2_LytSys.h>
#include  <AppLib/include/Util/app_util_2d.h>

#include  "./DigitSeparator.h"

namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/16 Wed. 15:18:48
  */
//==============================================================================
class NumberController
{
  GFL_FORBID_COPY_AND_ASSIGN(NumberController);
public:
  /**
    内部イベント取得ハンドラ
  */
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      _dummy_,
      
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  NumberController_OnEvent(NumberController* pSender, const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pEventHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  explicit NumberController(const u32 digits = 1);

  /**
    dtor
  */
  virtual ~NumberController();


  void          SetG2DUtil(app::util::G2DUtil* pG2DUtil, const u32 layoutID)  {m_pG2DUtil = pG2DUtil; m_LayoutID  = layoutID;}
  void          SetDigits(const u32 digits)
                {
                  m_Digits    = digits;
                  m_MaxValue  = 1;
                  for(u32 i = 0; i < digits; ++i){m_MaxValue *= 10;}
                  --m_MaxValue;
                }
  void          SetupDigit(const u32 digit, gfl2::lyt::LytPane* pDigitPane, const u32 animID);
  virtual void  Update(void);
  void          SetTargetValue(const u32 value, const bool isImmediate = false);
  u32           GetValue(void) const                      {return m_CurrentValue;}
  u32           GetTargetValue(void) const                {return m_TargetValue;}
  void          EnablePaddingZero(const bool isEnable)    {m_IsPaddingZero = isEnable;}
  void          SetInterpolateFrame(const u32 frame)      {m_InterpolateFrame = gfl2::math::max<u32>(1, frame);}
  void          ResetFrame(void)                          {m_CurrentFrame = 0;}
  void          SetStartValueToCurrentValue(void)         {m_StartValue = m_CurrentValue;}

  void          SetupSeparator(
                  const u32           languageID,
                  const u32           localeAnimID,
                  gfl2::lyt::LytPane* pSeparatorPane
                );

protected:
  static const u32  DIGITS_MAX  = 6;

  struct  DigitInfo
  {
    gfl2::lyt::LytPane* pDigitPane;
    u32                 animID;
    
    DigitInfo(void)
      : pDigitPane(NULL)
      , animID(UINT_MAX)
    {
    }
  };

  void  UpdateSeparator(void);


  app::util::G2DUtil*   m_pG2DUtil;
  u32                   m_LayoutID;
  IEventHandler*        m_pEventHandler;
  DigitInfo             m_DigitInfos[DIGITS_MAX];
  DigitSeparator        m_Separator;
  u32                   m_Digits;
  u32                   m_MaxValue;
  u32                   m_TargetValue;
  u32                   m_CurrentValue;
  u32                   m_StartValue;;
  u32                   m_InterpolateFrame;
  u32                   m_CurrentFrame;
  bool                  m_IsPaddingZero;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(NUMBERCONTROLLER_H_INCLUDED)  */
