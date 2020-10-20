//==============================================================================
/**
 * @file   FieldEffectFesLevelUp.cpp
 * @date   2016/02/09 Tue. 18:16:37
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include  "../../../include/Effect/content/FieldEffectFesLevelUp.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const gfl2::math::Vector3              CAM_COORD_POS(  0.0f,   0.0f, -200.0f);
  static const Field::Effect::EffectResourceID  EFFECT_RESOURCE_ID  = Field::Effect::EFFECT_RESOURCE_JF_LEVELUP;
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
EffectFesLevelUp::EffectFesLevelUp(void)
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
EffectFesLevelUp::~EffectFesLevelUp()
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
void  EffectFesLevelUp::OnSetup(void)
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
void  EffectFesLevelUp::OnUpdate(void)
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
bool  EffectFesLevelUp::OnTerminate(void)
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
void  EffectFesLevelUp::Start(void)
{
  gfl2::math::Matrix34  mtx;

  GetCameraCoordMatrix(mtx, CAM_COORD_POS);
  StartCore(&m_Handle, EFFECT_RESOURCE_ID, &mtx);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesLevelUp::Stop(void)
{
  StopCore(&m_Handle);
}

} /*  namespace Effect  */
} /*  namespace Field   */
