
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyCamera.cpp
 * @date   2016/09/09 Fri. 15:49:11
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Debug/DebugWin/include/DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

#include  "./EasyCamera.h"

namespace
{
  static const gfl2::math::Vector3    defaultPos  (0.0f,   0.0f, 0.0f);
  static const gfl2::math::Vector3    defaultAt   (0.0f,  80.0f, 0.0f);
  static const gfl2::math::Vector3    defaultRot  (8.2f,   0.0f, 0.0f);

  static const int                    defaultFOVy = 60;
  static const f32                    defaultNear = 1.0f;
  static const f32                    defaultFar  = 16383.0f;
  static const f32                    defaultDist = 300.0f;

  void Rot_Deg( f32 &x , f32 &z , const f32 deg )
  {
    f32 sin,cos;
    const f32 wx = x;
    const f32 wz = z;
    gfl2::math::SinCosDeg(&sin, &cos, deg);
    x = wx*cos - wz*sin;
    z = wx*sin + wz*cos;
  }
}



namespace Test      {
namespace MutoTest {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyCamera::EasyCamera(void)
  : m_pDrawEnvNode(NULL)
  , m_pBaseCamera(NULL)
  , m_pUtilHeap(NULL)
  , m_CamInfo()
  , m_RenderPlace(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY)
  , m_bEnable(false)
{
  Reset();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyCamera::~EasyCamera()
{
  Release();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyCamera::Create(app::util::Heap* pUtilHeap, const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace)
{
  Release();


  m_pUtilHeap   = pUtilHeap;
  m_RenderPlace = renderPlace;

  /*  cam  */
  {
    app::util::AppRenderingManager::CreateDefaultCamera(m_pUtilHeap, &m_pDrawEnvNode, &m_pBaseCamera, m_RenderPlace);

    Reset();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyCamera::Release(void)
{
  GFL_SAFE_DELETE(m_pBaseCamera);
  GFL_SAFE_DELETE(m_pDrawEnvNode);
  m_bEnable = false;
}





//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyCamera::Move_AroundGazePoint(const f32 hSpeed, const f32 vSpeed)
{
    static const f32 STIC_MIN_VAL = 0.3f;
    static const f32 STIC_MID_VAL = 0.8f;
    static const f32 STIC_ROT_SPD = 2.0f;
    static const f32 STIC_ROT_X_MAX = 80.0f;

    f32 rotX = 0;
    f32 rotY = 0;
    if (gfl2::math::Abs(hSpeed) > STIC_MIN_VAL)
    {
      if (gfl2::math::Abs(hSpeed) > STIC_MID_VAL)
      {
        rotY += hSpeed * 3;
      }
      else
      {
        rotY += hSpeed;
      }
    }
    if (gfl2::math::Abs(vSpeed) > STIC_MIN_VAL)
    {
      if (gfl2::math::Abs(vSpeed) > STIC_MID_VAL)
      {
        rotX += vSpeed * 3;
      }
      else
      {
        rotX += vSpeed;
      }
    }

    m_CamInfo.rot.x += rotX * STIC_ROT_SPD;
    m_CamInfo.rot.x =  gfl2::math::Clamp(m_CamInfo.rot.x, -STIC_ROT_X_MAX, STIC_ROT_X_MAX);
    m_CamInfo.rot.y += rotY * STIC_ROT_SPD;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyCamera::Move_EyeDirection(const f32 speed)
{
  const f32 moveVal = 5.0f * speed;
  const f32 distMin = 10.0f;

  m_CamInfo.dist  += moveVal;
  if (m_CamInfo.dist < distMin)
  {
    m_CamInfo.dist = distMin;
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
void  EasyCamera::Move_GazePoint(const f32 hSpeed, const f32 vSpeed)
{
  const f32 moveVal_H = 5.0f * hSpeed;
  const f32 moveVal_V = 5.0f * vSpeed;


  m_CamInfo.at.y += vSpeed;

  if(FLT_EPSILON < gfl2::math::Abs(hSpeed))
  {
    f32 slideValX = moveVal_H;
    f32 slideValZ = 0;
    
    Rot_Deg( slideValX , slideValZ , m_CamInfo.rot.y );

    m_CamInfo.at.x += slideValX;
    m_CamInfo.at.z += slideValZ;
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
void  EasyCamera::UpdateCore(gfl2::ui::DeviceManager* pDeviceManager)
{
  if(
    !m_bEnable
    || !pDeviceManager
    || !m_pBaseCamera
    || gfl2::debug::DebugWin_IsOpen()
  )
  {
    return;
  }


  gfl2::ui::VectorDevice* pStick  = pDeviceManager->GetStick(0);
  gfl2::ui::Button*       pButton = pDeviceManager->GetButton(0);
  

  {
    if(!pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      Move_AroundGazePoint(pStick->GetX(), pStick->GetY());
    }


    // 距離の操作
    if (pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      f32 speed = 0.0f;

      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        speed = 1.0f;
      }
      else if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        speed = -1.0f;
      }

      Move_EyeDirection(speed);
    }
    else
    {
      f32 moveVal_H = 0.0f;
      f32 moveVal_V = 0.0f;

      // 注視点の操作
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        moveVal_V = 1.0f;
      }
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        moveVal_V = -1.0f;
      }

      if (pButton->IsHold(gfl2::ui::DIR_LEFT))
      {
        moveVal_H = 1.0f;
      }
      else
      if (pButton->IsHold(gfl2::ui::DIR_RIGHT))
      {
        moveVal_H = -1.0f;
      }

      Move_GazePoint(moveVal_H, moveVal_V);

    }

    // リセット
    if (!pButton->IsHold(gfl2::ui::BUTTON_R) && pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      Reset();
    }

    {
      gfl2::math::Vector3 camPos(0, 0, m_CamInfo.dist);

      Rot_Deg( camPos.z , camPos.y , m_CamInfo.rot.x );
      Rot_Deg( camPos.x , camPos.z , m_CamInfo.rot.y );

      camPos += m_CamInfo.at;
      
      m_CamInfo.pos = camPos;
    }
  }
  
  m_pBaseCamera->SetupCameraLookAt(m_CamInfo.pos, m_CamInfo.at, gfl2::math::Vector3(0.0f, 1.0f, 0.0f));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyCamera::Reset(void)
{
  if(!m_pBaseCamera)  return;

  m_CamInfo.pos         = defaultPos;
  m_CamInfo.at          = defaultAt;
  m_CamInfo.rot         = defaultRot;
  m_CamInfo.dist        = defaultDist;

  m_pBaseCamera->SetupCameraLookAt(m_CamInfo.pos, m_CamInfo.at, gfl2::math::Vector3(0.0f, 1.0f, 0.0f));
}









} /*  namespace MutoTest */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
