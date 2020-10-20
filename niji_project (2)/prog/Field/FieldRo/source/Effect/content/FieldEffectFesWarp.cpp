//==============================================================================
/**
 * @file   FieldEffectFesWarp.cpp
 * @date   2016/02/09 Tue. 19:00:29
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

#include  "niji_conv_header/field/effect/fes_warp.h"
#include  "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include  "../../../include/Effect/content/FieldEffectFesWarp.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  static const Field::Effect::EffectResourceID  EFFECT_RESOURCE_ID  = Field::Effect::EFFECT_RESOURCE_JF_WARP;
  static const u32                              APPEAR_FRAME        = 8;
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
EffectFesWarp::EffectFesWarp(void)
  : EffectFesEffectBase()
  , m_Handle()
  , m_pHandler(NULL)
  , m_FrameCount(0)
  , m_IsPlaying(false)
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
EffectFesWarp::~EffectFesWarp()
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
void  EffectFesWarp::OnSetup(void)
{
  m_ModelMotionResource[MODEL_MOTION_RESOURCE_Model].CreateModel(this, BL_IDX_FES_WARP_EF_JOINFESTA_WARP_CTR_GFBMDLP);
  m_ModelMotionResource[MODEL_MOTION_RESOURCE_Motion].CreateMotion(this, BL_IDX_FES_WARP_EF_JOINFESTA_WARP_GFBMOT);


  m_ModelInstance.Create(this, m_ModelMotionResource[MODEL_MOTION_RESOURCE_Model]);
  m_ModelInstance.SetMotion(&m_ModelMotionResource[MODEL_MOTION_RESOURCE_Motion]);
  m_ModelInstance.GetBaseModel().SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesWarp::OnUpdate(void)
{
  m_ModelInstance.Update();

  if(m_IsPlaying)
  {
    if((m_FrameCount++) == APPEAR_FRAME)
    {
      Event(IEventHandler::EVENT_AppearCharacter);
    }

    /*  終了判定  */
    {
      bool  isEmitterAlive  = false;
      
      if(m_Handle.IsValid())
      {
        if(m_Handle.GetEmitterSet()->IsAlive())
        {
          isEmitterAlive  = true;
        }
      }
      
      if((!isEmitterAlive) && (m_ModelInstance.GetBaseModel().GetAnimationEndFrame() <= m_ModelInstance.GetBaseModel().GetAnimationFrame()))
      {
        Event(IEventHandler::EVENT_End);
        m_IsPlaying = false;
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  EffectFesWarp::OnTerminate(void)
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
void  EffectFesWarp::Start(const gfl2::math::Vector3& rPos)
{
  StartCore(&m_Handle, EFFECT_RESOURCE_ID, NULL, &rPos);
  
  m_ModelInstance.GetBaseModel().SetPosition(rPos);
  m_ModelInstance.GetBaseModel().SetVisible(true);
  m_ModelInstance.GetBaseModel().SetAnimationFrame(0.0f);
  
  m_IsPlaying   = true;
  m_FrameCount  = 0;

  Sound::PlaySE( SEQ_SE_SYS_NJ_043 );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesWarp::Stop(void)
{
  StopCore(&m_Handle);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesWarp::Event(const IEventHandler::EventID eventID)
{
  if(m_pHandler)
  {
    m_pHandler->EffectFesWarp_OnEvent(this, eventID);
  }
}



} /*  namespace Effect  */
} /*  namespace Field   */
