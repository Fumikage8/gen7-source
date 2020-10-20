//==============================================================================
/**
 * @file   FieldEffectFesStartSplash.cpp
 * @date   2016/02/09 Tue. 18:58:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "Sound/include/Sound.h"

#include  "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include  "../../../include/Effect/content/FieldEffectFesStartSplash.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const gfl2::math::Vector3              CAM_COORD_POS(   0.0f, -60.0f, -200.0f);
  static const Field::Effect::EffectResourceID  EFFECT_RESOURCE_ID  = Field::Effect::EFFECT_RESOURCE_JF_START_SPLASH;
}



namespace Field   {
namespace Effect  {
// =============================================================================
/**/
//==============================================================================
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
EffectFesStartSplash::EffectFesStartSplash(void)
  : EffectFesEffectBase()
  , m_Handle()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EffectFesStartSplash::~EffectFesStartSplash()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesStartSplash::OnSetup(void)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesStartSplash::OnUpdate(void)
{
  UpdateToCamCoord(&m_Handle, CAM_COORD_POS);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  EffectFesStartSplash::OnTerminate(void)
{
  Stop();
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesStartSplash::Start(void)
{
  gfl2::math::Matrix34  mtx;


  GetCameraCoordMatrix(mtx, CAM_COORD_POS);
  StartCore(&m_Handle, EFFECT_RESOURCE_ID, &mtx);

  Sound::PlaySE( SEQ_SE_SYS_NJ_044 );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesStartSplash::Stop(void)
{
  StopCore(&m_Handle);
}

} /*  namespace Effect  */
} /*  namespace Field   */
