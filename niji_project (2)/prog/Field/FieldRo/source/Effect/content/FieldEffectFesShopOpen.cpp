//==============================================================================
/**
 * @file   FieldEffectFesShopOpen.cpp
 * @date   2016/02/09 Tue. 18:56:30
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
#include  "../../../include/Effect/content/FieldEffectFesShopOpen.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const gfl2::math::Vector3              CAM_COORD_POS( 0.0f,  60.0f, -200.0f);
  static const Field::Effect::EffectResourceID  EFFECT_RESOURCE_ID  = Field::Effect::EFFECT_RESOURCE_JF_SHOP_OPEN;
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
EffectFesShopOpen::EffectFesShopOpen(void)
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
EffectFesShopOpen::~EffectFesShopOpen()
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
void  EffectFesShopOpen::OnSetup(void)
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
void  EffectFesShopOpen::OnUpdate(void)
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
bool  EffectFesShopOpen::OnTerminate(void)
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
void  EffectFesShopOpen::Start(void)
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
void  EffectFesShopOpen::Stop(void)
{
  StopCore(&m_Handle);
}

} /*  namespace Effect  */
} /*  namespace Field   */
