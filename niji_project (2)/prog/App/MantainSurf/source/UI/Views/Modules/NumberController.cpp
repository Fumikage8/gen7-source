//==============================================================================
/**
 * @file   NumberController.cpp
 * @date   2016/11/16 Wed. 16:16:54
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NumberController.h"


namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NumberController::NumberController(const u32 digits)
  : m_pG2DUtil(NULL)
  , m_LayoutID(0)
  , m_pEventHandler(NULL)
  , m_Separator()
  , m_Digits(digits)
  , m_MaxValue(0)
  , m_TargetValue(0)
  , m_CurrentValue(0)
  , m_StartValue(0)
  , m_InterpolateFrame(1)
  , m_CurrentFrame(0)
  , m_IsPaddingZero(false)
{
  SetDigits(m_Digits);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NumberController::~NumberController()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController::SetupDigit(const u32 digit, gfl2::lyt::LytPane* pDigitPane, const u32 animID)
{
  GFL_ASSERT(digit < m_Digits);

  m_DigitInfos[digit].pDigitPane  = pDigitPane;
  m_DigitInfos[digit].animID      = animID;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController::Update(void)
{
  /* value   */
  {
    if(m_CurrentFrame < m_InterpolateFrame)
    {
      ++m_CurrentFrame;
      GFL_ASSERT(m_InterpolateFrame);
      m_CurrentValue  = m_StartValue + (static_cast<s32>(m_TargetValue) - static_cast<s32>(m_StartValue)) * m_CurrentFrame / m_InterpolateFrame;
    }
    else
    {
      m_CurrentValue  = m_TargetValue;
    }
  }

  /* appearance   */
  {
    const u32 value   = GetValue();
    u32       divisor = 1;
    
    for(u32 digit = 0; digit < m_Digits; ++digit)
    {
      const DigitInfo&  rDigitInfo    = m_DigitInfos[digit];
      const u32         dividedValue  = value / divisor;    /*  目標の桁及びその上位の値  */
      const u32         digitNum      = dividedValue % 10;  /*  目標の桁のみの値          */
      
      if(m_pG2DUtil)
      {
        if(rDigitInfo.animID != UINT_MAX)
        {
          if(m_pG2DUtil->IsAnimeEnd(m_LayoutID, rDigitInfo.animID))
          {
            m_pG2DUtil->StartAnime(m_LayoutID, rDigitInfo.animID, false);
          }
          m_pG2DUtil->SetAnimeFrame(m_LayoutID, rDigitInfo.animID, digitNum);
        }

        /*  値が0の場合は桁数字を消す（一の位は表示する）  */
        if(digit)
        {
          if(rDigitInfo.pDigitPane)
          {
            rDigitInfo.pDigitPane->SetVisible(m_IsPaddingZero || dividedValue);
          }
        }
      }
      divisor *= 10;
    }
  }

  UpdateSeparator();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController::SetTargetValue(const u32 value, const bool isImmediate)
{
  bool      bChanged        = false;
  const u32 oldTargetValue  = m_TargetValue;

  m_TargetValue = gfl2::math::Min(value, m_MaxValue);
  bChanged      = (m_TargetValue != oldTargetValue);
  if(isImmediate)
  {
    m_CurrentValue = m_TargetValue;
    m_CurrentFrame = m_InterpolateFrame;
  }
  else if(bChanged)
  {
    m_CurrentFrame = 0;
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController::SetupSeparator(
  const u32           languageID,
  const u32           localeAnimID,
  gfl2::lyt::LytPane* pSeparatorPane
)
{
  GFL_ASSERT(m_pG2DUtil);

  DigitSeparator::Info  separatorInfo;
  
  separatorInfo.pG2DUtil        = m_pG2DUtil;
  separatorInfo.layoutID        = m_LayoutID;
  separatorInfo.animID          = localeAnimID;
  separatorInfo.languageID      = languageID;
  separatorInfo.threshold       = 1000;
  separatorInfo.pSeparatorPane  = pSeparatorPane;
  
  m_Separator.Setup(separatorInfo);
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NumberController::UpdateSeparator(void)
{
  m_Separator.UpdateByValue(GetValue());
}


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
