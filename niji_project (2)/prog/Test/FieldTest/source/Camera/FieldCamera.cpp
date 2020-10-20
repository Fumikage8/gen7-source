/**
* @file    FieldCamera.cpp
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/


// c
// #include <string.h>

#include "test/FieldTest/include/Camera/FieldCamera.h"
#include "test/FieldTest/include/Camera/FieldCameraUtility.h"
// #include "field/FieldDebug.h"
#include "test/FieldTest/include/movemodel.h"
#include "test/FieldTest/include/field_transform.h"

#include <math/include/gfl2_Quaternion.h>
#include <system/include/InputDevice/gfl2_Pad.h>

#include "GameSys/include/GameManager.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

static const f32 ROTATE_LIMMIT_MIX_X( gfl2::math::ConvDegToRad( -80 ) );
static const f32 ROTATE_LIMMIT_MAX_X( gfl2::math::ConvDegToRad( 80 ) );

void GetRotate( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector* pOut );


void RotateQuaternion( const gfl2::math::Vector& rSrc, const LIMIT_DATA& rRimitData, const gfl2::math::Vector& rCenter, f32 distance, const LIMIT_DATA& rPositionRimitData, gfl2::math::Quaternion* pOut )
{
  gfl2::math::Vector rot;
  GetRotate( *pOut, &rot );

  rot.x += rSrc.x;
  rot.y += rSrc.y;

  Clamp( rot, rRimitData, &rot );

  rot.x = gfl2::math::Clamp( rot.x, ROTATE_LIMMIT_MIX_X, ROTATE_LIMMIT_MAX_X );


  // @todo 反対側対応
  // 位置制御によるY回転制御
  if( rPositionRimitData.enableMinX )
  {
    f32 result1, result2;
    if( GetLimitLineRotate( rCenter, distance, rPositionRimitData.min.x, &result1, &result2 ) )
    {
      if( rot.y < result1 )
      {
        rot.y = result1;
      }
    }
  }
  if( rPositionRimitData.enableMaxX )
  {
    f32 result1, result2;
    if( GetLimitLineRotate( rCenter, distance, rPositionRimitData.max.x, &result1, &result2 ) )
    {
      if( rot.y > result1 )
      {
        rot.y = result1;
      }
    }
  }

  pOut->RadianToQuaternion( rot.z, rot.y, rot.x );
}

void RotateQuaternion( const gfl2::math::Vector& rSrc, gfl2::math::Quaternion* pOut )
{
  gfl2::math::Vector rot;
  GetRotate( *pOut, &rot );

  rot.x += rSrc.x;
  rot.y += rSrc.y;

  rot.x = gfl2::math::Clamp( rot.x, ROTATE_LIMMIT_MIX_X, ROTATE_LIMMIT_MAX_X );
  pOut->RadianToQuaternion( rot.z, rot.y, rot.x );
}

void GetRotate( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector* pOut )
{
  // クォータニオンからオイラー角を求める
  gfl2::math::Vector nowRot( 0.0f, 0.0f, 0.0f );
  {
    gfl2::math::Vector vec1( 0.0f, 0.0f, 1.0f );
    gfl2::math::Matrix rotMat;
    rSrc.QuaternionToMatrix( rotMat );
    vec1 = rotMat * vec1;

    gfl2::math::Vector vec2( vec1.x, 0.0f, vec1.z );
    vec2 = vec2.Normalize3();

    nowRot.y = atan2( vec2.x, vec2.z );

    // Y回転を打ち消す
    rotMat = gfl2::math::Matrix::GetRotationY( - nowRot.y ) * rotMat;
    vec1 = rotMat * gfl2::math::Vector( 0.0f, 0.0f, 1.0f );

    gfl2::math::Vector vec3( 0.0f, vec1.y, vec1.z );
    vec3 = vec3.Normalize3();
    nowRot.x = atan2( vec3.y * -1.0f, vec3.z );
  }

  *pOut = nowRot;
}


class RotationLengthData
{
  void Set( const gfl2::math::Vector& rRot, f32 distance )
  {
    m_Rotation.RadianToQuaternion( rRot.z, rRot.y, rRot.x );
    m_Distance = distance;
  }

  void SetByOffsetData( const gfl2::math::Vector& rOffset )
  {
  }

  void GetRotate( gfl2::math::Vector* pOut ) const
  {

  }

// private:
  f32                     m_Distance;
  gfl2::math::Quaternion  m_Rotation;
};

static const f32 sc_OrthoHeightTable[] = 
{
  1600.0f,
  3200.0f,
  6400.0f,
  12800.0f,
  25600.0f,
};

class FieldCameraActionBase_Top : public FieldCameraActionBase
{
public:


  FieldCameraActionBase_Top( fld::mmodel::MoveModel* pTarget ) :
    m_DataID( 0xFFFF ),
    m_Scale( 0 ),
    m_pTarget( NULL ),
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ),
    m_NowCameraPos( 0.0f, 0.0f, 0.0f )
  {
    m_pTarget = pTarget;
    gfl2::math::Vector offset( 0.0f, 4000.0f, 5.0f );
    {
      m_NowTargetPos = m_pTarget->GetTransform()->GetTranslation();
      m_NowCameraPos = m_NowTargetPos + offset;
    }
  }

  void Update( void )
  {
    GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    // scale更新
    if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
    {
      if( m_Scale == 0 )
      {
        m_Scale = GFL_NELEMS( sc_OrthoHeightTable ) - 1;
      }
      else
      {
        m_Scale--;
      }
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
    {
      if( m_Scale >= GFL_NELEMS( sc_OrthoHeightTable ) - 1 )
      {
        m_Scale = 0;
      }
      else
      {
        m_Scale++;
      }
    }

    gfl2::math::Vector offset( 0.0f, 4000.0f, 5.0f );
    {
      m_NowTargetPos = m_pTarget->GetTransform()->GetTranslation();
      m_NowCameraPos = m_NowTargetPos + offset;
    }
  }
  void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );

  }

  void GetProjectionMatrix( gfl2::math::Matrix44* pMtx, b32 pivotFlag ) const
  {
    // f32 f = 400.0f / 240.0f;
    f32 f = m_pProjectionSetting->aspect;
    gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_NONE;
    if( pivotFlag )
    {
      pivot = gfl2::math::PIVOT_UPSIDE_TO_TOP;
    }


    nw::math::MTX44 proj; // = gfl2::math::ConvertGfMtx44ToNwMtx44( m_proj );
    gfl2::math::MTX44OrthoPivot(
      &proj,
      sc_OrthoHeightTable[ m_Scale ] * -0.5f * f,
      sc_OrthoHeightTable[ m_Scale ] * 0.5f * f,
      sc_OrthoHeightTable[ m_Scale ] * -0.5f,
      sc_OrthoHeightTable[ m_Scale ] * 0.5f,
      1.0f,
      10000.0f,
      pivot );
    *pMtx = gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
  }

  void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->targetPos = m_NowTargetPos;
    pCameraData->cameraPos = m_NowCameraPos;
  }

  u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  b32 IsManual( void ) const
  {
    return false;
  }

private:
  u32 m_DataID;
  u32 m_Scale;
  fld::mmodel::MoveModel* m_pTarget;
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;
};

// 
class FieldCameraActionRelativeRotate : public FieldCameraActionBase
{
public:

  FieldCameraActionRelativeRotate( u32 id, const CAMERA_DATA_RELATIVE_ROTATE* pData, fld::mmodel::MoveModel* pTarget, u32 interpolationFrame, const FieldCameraActionBase* pPrev = NULL ) 
    :
    m_ManualResetFlag( false ),
    m_InterpolationFlag( false ), 
    m_DataID( 0 ), 
    m_StartRot( -0.604547f, -0.0223443f, 0.0f ),
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f ), 
    m_Distance( 514.0f ),
    m_Rot( -0.604547f, -0.0223443f, 0.0f ),
    m_TargetOffsetType( COORDINATE_SYSTEM_TYPE_LOCAL ),
    m_TargetOffset( 0.0f, 0.0f, 0.0f ),
    m_TargetPos( 0.0f, 0.0f, 0.0f ),
    m_StickInterpolationCount( 0 ),
    m_StickInterpolationTime( 0 ),
    m_InterpolationCount( 0 ),
    m_InterpolationTime( 0 ),
    m_PrevTargetPos( 0.0f, 0.0f, 0.0f ),
    m_PrevCameraPos( 0.0f, 0.0f, 0.0f ),
    m_PrevTargetOffset( 0.0f, 0.0f, 0.0f ),
    m_PrevCameraOffset( 0.0f, 0.0f, 0.0f ),
    m_ManualFlag( false )
  {
    m_DataID = id;
    m_pTarget = pTarget;

    m_StartRot = pData->rotate;
    m_Distance = pData->distance;
    m_InterpolationTime = interpolationFrame;

    m_PositionLimitData = pData->positionLimitData;
    m_PositionLimitDataEx = pData->positionLimitDataEx;
    m_RotateLimitData = pData->rotateLimitData;

    m_TargetOffsetType = pData->targetOffsetType;
    m_TargetOffset = pData->targetOffset;

    // 引継ぎ
    if( pPrev )
    {
      CAMERA_DATA prevCameraData;
      pPrev->GetCameraData( &prevCameraData );
      m_PrevCameraPos = prevCameraData.cameraPos;
      m_PrevTargetPos = prevCameraData.targetPos;

      // @todo 1フレーム遅延している
      gfl2::math::Vector targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;

      m_PrevTargetOffset = m_PrevTargetPos - targetPos;
      m_PrevCameraOffset = m_PrevCameraPos - m_PrevTargetPos;

      m_InterpolationCount = m_InterpolationTime;
    }

    m_StartRotation.RadianToQuaternion( m_StartRot.z, m_StartRot.y, m_StartRot.x );
    m_Rotation = m_StartRotation;
  }

  virtual ~FieldCameraActionRelativeRotate( void )
  {
  }

  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };

    m_ManualFlag = false;

    gfl2::math::Quaternion qt;

    
    GameSys::GameManager*	pGameManager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager	= pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice*	pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button*	pButton	= pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);


    gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
    gfl2::math::Vector3 pos_r( 0.0f, 0.0f, 0.0f );
    bool pad_u = false;
    bool pad_d = false;
    bool pad_l = false;
    bool pad_r = false;
    
    // L
    if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      pos_r.x = pVecDevice->GetX();
      pos_r.y = pVecDevice->GetY();

      pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
      pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
      pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
      pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );
    }

    // 入力情報格納
#if 0
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );
#endif
#if 0
	if( field::FieldDebugInfo::m_DebugInfoDebugMode == field::FieldDebugInfo::DEBUG_MODE_BLOOM )
	{
		pad_u = false;
		pad_d = false;
		pad_l = false;
		pad_r = false;
	}
#endif

    // if( field::FieldDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
      m_TargetPos = m_pTarget->GetTransform()->GetTranslation() + m_pTarget->GetTransform()->GetEffectTranslation();
      m_TargetPos.y += 40.0 * 10.0f / 3.0f;
    }

    Clamp( m_TargetPos, m_PositionLimitData, &m_TargetPos );

#if 0
    if( m_DebugModeEnable )
    {
      field::FieldDebugInfo::m_CameraTargetDraw = true;
    }
    else
    {
      field::FieldDebugInfo::m_CameraTargetDraw = false;
    }

#endif
#if 0
    // 更新処理
    if( gfl2::system::InputDevice::Pad::IsPush( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
    {
      m_ManualResetFlag = true;
    }
    // カメラターゲット操作
    else if( m_DebugModeEnable && gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
    {
#if 0
      field::FieldDebugInfo::m_InfoCameraManualMode = 1; 
      field::FieldDebugInfo::m_CameraTargetDrawColor.x = 1.0;
#endif
      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // オフセット移動
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;

        switch( m_TargetOffsetType )
        {
          // カメラの回転に合わせて、ターゲット座標オフセット値を回転
        case COORDINATE_SYSTEM_TYPE_LOCAL:
          { 
            // そのまま加算
            m_TargetOffset.x -= lx;
            m_TargetOffset.z -= ly;
            break;
          }

          // そのまま
        case COORDINATE_SYSTEM_TYPE_WORLD:
          {
            // 回転して加算
            gfl2::math::Vector rot;
            GetRotate( m_Rotation, &rot );
            gfl2::math::Vector offset( 0.0f, 0.0f, 0.0f );
            float sinYaw = sinf( rot.y * -1.0f ) * -1.0f;
            float cosYaw = cosf( rot.y * -1.0f ) * -1.0f;
            m_TargetOffset.x -= lx * cosYaw - ly * sinYaw;
            m_TargetOffset.z -= lx * sinYaw + ly * cosYaw;
            break;
          }
        } // switch( m_TargetOffsetType )
      }

      if(pad_u || pad_d )
      {
        float diff = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { m_TargetOffset.y += diff; }
        if ( pad_d ) { m_TargetOffset.y -= diff; }
      }
    }
    // カメラポジション操作
    else
#endif
    {
#if 0
      field::FieldDebugInfo::m_InfoCameraManualMode = 0; 
      field::FieldDebugInfo::m_CameraTargetDrawColor.x = 0.0;
#endif

      // 回転
      if(pos_r.x || pos_r.y)
      {
        // 回転速度計算
        float rate = 0.05f; // * field::FieldDebugInfo::m_CameraSpeed;
#if 0
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }
#endif
        // 回転実行
        gfl2::math::Vector rot( pos_r.y * rate * -1.0f, pos_r.x * rate * -1.0f, 0.0f );
        RotateQuaternion( rot, m_RotateLimitData, m_TargetPos, m_Distance, m_PositionLimitData, &m_Rotation );

        // 補間情報更新
        f32 dot = m_StartRotation.Dot( m_Rotation );
        f32 angle = acos( dot );
        m_PrevRotation = m_Rotation;
        m_StickInterpolationCount = 100 * angle;
        m_StickInterpolationTime = 100 * angle;
        m_ManualFlag = true;
        m_ManualResetFlag = false;
      }

      // 距離
      if(pad_u || pad_d )
      {
        float diff = 5.0f;// * field::FieldDebugInfo::m_CameraSpeed;
#if 0
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
#endif
        if ( pad_u ) { m_Distance -= diff; }
        if ( pad_d ) { m_Distance += diff; }

        m_Distance = gfl2::math::Clamp( m_Distance, 0.0f, 2048.0f );
      }
      
      if( pad_r || pad_l )
      {
        float diff = 0.1f; // * field::FieldDebugInfo::m_CameraSpeed;
#if 0
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
#endif
        if ( pad_u ) { m_pProjectionSetting->aov -= diff; }
        if ( pad_d ) { m_pProjectionSetting->aov += diff; }

        // f32 diffMax = 20.0f;
        // m_pProjectionSetting->aov = gfl2::math::Clamp( m_pProjectionSetting->aov, 35.0f - diffMax, 35.0f + diffMax );
      }
    }

    if( m_DebugModeEnable == false )
    {
      if( m_ManualResetFlag )
      {
        if( m_StickInterpolationCount > 0 )
        {
          m_StickInterpolationCount--;
          f32 t = static_cast<f32>( m_StickInterpolationCount ) / m_StickInterpolationTime;
          t = t * t;
          m_Rotation.Slerp( m_StartRotation, m_PrevRotation, t );
        }
        else
        {
          m_ManualResetFlag = false;
        }
      }
      else if( m_ManualFlag == false )
      {
        if( m_pTarget->GetAnimeID() != chara::MOTION_ID_WAIT )
        {
          if( m_StickInterpolationCount > 0 )
          {
            m_StickInterpolationCount--;
// 自機が移動した際に、スティックによるカメラ回転を戻す処理、無効化
#if 0
            f32 t = static_cast<f32>( m_StickInterpolationCount ) / m_StickInterpolationTime;
            t = t * t;
            m_Rotation.Slerp( m_StartRotation, m_PrevRotation, t );
#endif
          }
        }
      }
    }

    gfl2::math::Vector targetOffset( m_TargetOffset );

    switch( m_TargetOffsetType )
    {
      // カメラの回転に合わせて、ターゲット座標オフセット値を回転
    case COORDINATE_SYSTEM_TYPE_LOCAL:
      {
        gfl2::math::Vector rot;
        GetRotate( m_Rotation, &rot );
        float sinYaw = sinf( rot.y * -1.0f ) * -1.0f;
        float cosYaw = cosf( rot.y * -1.0f ) * -1.0f;
        targetOffset.x = m_TargetOffset.x * cosYaw - m_TargetOffset.z * sinYaw;
        targetOffset.y = m_TargetOffset.y;
        targetOffset.z = m_TargetOffset.x * sinYaw + m_TargetOffset.z * cosYaw;
        break;
      }

      // そのまま
    case COORDINATE_SYSTEM_TYPE_WORLD:
      {
        break;
      }
    }

    m_NowTargetPos = m_TargetPos + targetOffset;

#if 0
    gfl2::math::Vector vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;
#endif
   // m_NowCameraPos = m_StartRotation * ;
   // m_NowCameraPos = m_NowTargetPos + vecEye;
    
    gfl2::math::Vector vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat;
    m_Rotation.QuaternionToMatrix( rotMat );
    vecEye = rotMat * vecEye;
    m_NowCameraPos = m_NowTargetPos + vecEye;
    //Clamp( m_NowCameraPos, m_PositionLimitData, &m_NowCameraPos );

    // 座標補間
    if( m_InterpolationCount > 0 )
    {
      m_InterpolationCount--;
      f32 t = static_cast<f32>( m_InterpolationCount ) / m_InterpolationTime;
      //t = gfl2::math::FSin( PI * 0.5f * t );
      t = t * t;

#if 0
      m_NowTargetPos = m_NowTargetPos + ( m_PrevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( m_PrevCameraPos - m_NowCameraPos ) * t;
#else

      gfl2::math::Vector targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;
      gfl2::math::Vector prevTargetPos( targetPos + m_PrevTargetOffset );
      gfl2::math::Vector prevCameraPos( prevTargetPos + m_PrevCameraOffset );

      m_NowTargetPos = m_NowTargetPos + ( prevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( prevCameraPos - m_NowCameraPos ) * t;
#endif
    }
  }

  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );
  }

  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->cameraPos = m_NowCameraPos;
    pCameraData->targetPos = m_NowTargetPos;
  }

  virtual u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  virtual b32 IsManual( void ) const
  {
    if( m_StickInterpolationCount > 0 )
    {
      return true;
    }

    return m_ManualFlag;
  }

#if PM_DEBUG
  virtual void Dump( void ) const
  {
    // クォータニオンからオイラー角を求める
    gfl2::math::Vector nowRot( 0.0f, 0.0f, 0.0f );
    GetRotate( m_Rotation, &nowRot );
    
    // 度に変換
    ConvertRadianToDegree( &nowRot );

    // 出力
    GFL_RELEASE_PRINT( "  <CameraData MeshName=\"%s\" Type =\"TYPE_RELATIVE_ROTATE\" >\n", m_Name );
    GFL_RELEASE_PRINT( "    <Rotate x=\"%f\" y=\"%f\" z=\"%f\" />\n", nowRot.x, nowRot.y, nowRot.z );
    GFL_RELEASE_PRINT( "    <Distance>%f</Distance>\n", m_Distance ); 
    GFL_RELEASE_PRINT( "    <TargetOffset Type=\"%s\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", GetLabel( m_TargetOffsetType ), m_TargetOffset.x, m_TargetOffset.y, m_TargetOffset.z );
    GFL_RELEASE_PRINT( "  </CameraData>\n" );
  }
#endif

#if 0
  virtual void OutPutData( std::ofstream* pOfstream ) const
  {
    // クォータニオンからオイラー角を求める
    gfl2::math::Vector nowRot( 0.0f, 0.0f, 0.0f );
    GetRotate( m_Rotation, &nowRot );
    
    // 度に変換
    ConvertRadianToDegree( &nowRot );

    // 出力
    *pOfstream << "  <CameraData MeshName=\"" << m_Name << "\" Type =\"TYPE_RELATIVE_ROTATE\" >" << std::endl; 
    *pOfstream << "    <Rotate x=\"" <<  nowRot.x << "\" " << "y=\"" << nowRot.y << "\" " << "z=\"" << nowRot.z << "\" />" << std::endl;
    *pOfstream << "    <Distance>" << m_Distance << "</Distance>" << std::endl; 
    *pOfstream << "    <TargetOffset Type=\"" << GetLabel( m_TargetOffsetType ) << "\" x=\"" <<  m_TargetOffset.x << "\" " << "y=\"" << m_TargetOffset.y << "\" " << "z=\"" << m_TargetOffset.z << "\" />" << std::endl;
    *pOfstream << "  </CameraData>" << std::endl; 
  };
#endif

private:

  b32 m_ManualResetFlag;
  b32 m_InterpolationFlag;
  u32 m_DataID;
  
  LIMIT_DATA     m_PositionLimitData;
  LIMIT_DATA     m_PositionLimitDataEx;
  LIMIT_DATA     m_RotateLimitData;

  // 初期状態
  gfl2::math::Vector      m_StartRot;
  gfl2::math::Quaternion  m_StartRotation;
  gfl2::math::Quaternion  m_PrevRotation;

  // ターゲット座標のオフセット値

  // 最新状態
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector      m_Rot;
  gfl2::math::Quaternion  m_Rotation;
  CoordinateSystemType    m_TargetOffsetType;
  gfl2::math::Vector      m_TargetOffset;
  

  gfl2::math::Vector      m_TargetPos;
  fld::mmodel::MoveModel* m_pTarget;

  u32                     m_StickInterpolationCount;
  u32                     m_StickInterpolationTime;

  // 前回情報
  u32                     m_InterpolationCount;
  u32                     m_InterpolationTime;
  gfl2::math::Vector      m_PrevTargetPos;
  gfl2::math::Vector      m_PrevCameraPos;
  gfl2::math::Vector      m_PrevTargetOffset;
  gfl2::math::Vector      m_PrevCameraOffset;

  // 
  b32                     m_ManualFlag;

};

class FieldCameraActionStop : public FieldCameraActionBase
{
public:

  FieldCameraActionStop( u32 id, const CAMERA_DATA_STOP* pData, fld::mmodel::MoveModel* pTarget, u32 interpolationFrame, const FieldCameraActionBase* pPrev = NULL ) 
    :
    m_DataID( 0 ), 
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f )
  {
    m_DataID = id;
    m_pTarget = pTarget;

    // 引継ぎ
    if( pPrev )
    {
      CAMERA_DATA prevCameraData;
      pPrev->GetCameraData( &prevCameraData );
      m_NowCameraPos = prevCameraData.cameraPos;
      m_NowTargetPos = prevCameraData.targetPos;
    }
    else
    {
      // エラー

    }
  }

  virtual ~FieldCameraActionStop( void )
  {
  }

  virtual void Update( void )
  {
  }

  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );
  }

  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->cameraPos = m_NowCameraPos;
    pCameraData->targetPos = m_NowTargetPos;
  }

  virtual u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  virtual b32 IsManual( void ) const
  {
    return false;
  }

  virtual void OutPutData( std::ofstream* pOfstream ) const
  {
  };


private:

  u32 m_DataID;
  
  // 最新状態
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;
  fld::mmodel::MoveModel* m_pTarget;

};

class FieldCameraActionRelativeRotateRatio : public FieldCameraActionBase
{
public:

  FieldCameraActionRelativeRotateRatio( u32 id, const CAMERA_DATA_RELATIVE_ROTATE_RATIO* pData, fld::mmodel::MoveModel* pTarget, u32 interpolationFrame, const FieldCameraActionBase* pPrev = NULL ) 
    :
    m_DataID( 0 ), 
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f ),

    m_InterpolationCount( 0 ),
    m_InterpolationTime( 0 ),
    m_PrevTargetPos( 0.0f, 0.0f, 0.0f ),
    m_PrevCameraPos( 0.0f, 0.0f, 0.0f ),
    m_PrevTargetOffset( 0.0f, 0.0f, 0.0f ),
    m_PrevCameraOffset( 0.0f, 0.0f, 0.0f )

  {
    m_DataID = id;
    m_pTarget = pTarget;
    m_SettingData = *pData;

    m_InterpolationTime = interpolationFrame;

    // 引継ぎ
    if( pPrev )
    {
      CAMERA_DATA prevCameraData;
      pPrev->GetCameraData( &prevCameraData );
      m_PrevCameraPos = prevCameraData.cameraPos;
      m_PrevTargetPos = prevCameraData.targetPos;

      // @todo 1フレーム遅延している
      gfl2::math::Vector targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;

      m_PrevTargetOffset = m_PrevTargetPos - targetPos;
      m_PrevCameraOffset = m_PrevCameraPos - m_PrevTargetPos;

      m_InterpolationCount = m_InterpolationTime;
    }
  }

  virtual ~FieldCameraActionRelativeRotateRatio( void )
  {
  }

  virtual void Update( void )
  {
    gfl2::math::Vector targetPos  = m_pTarget->GetTransform()->GetTranslation();
    gfl2::math::Vector startToEndVec( m_SettingData.endPointData.referencePoint - m_SettingData.startPointData.referencePoint );
    gfl2::math::Vector startToTargetVec( targetPos - m_SettingData.startPointData.referencePoint );
    f32 startToEndLength( startToEndVec.Length3() );
    f32 startToTargetLength( startToTargetVec.Length3() );
    startToEndVec = startToEndVec.Normalize3();
    startToTargetVec = startToTargetVec.Normalize3();
    f32 dot = startToEndVec.Dot3( startToTargetVec );

    f32 ratio(1.0f);
    ratio = startToTargetLength * dot / startToEndLength;
    ratio = gfl2::math::Clamp( ratio, 0.0f, 1.0f );

    // 回転値を補間
    gfl2::math::Quaternion nowRotate;
    gfl2::math::Quaternion startRotate;
    gfl2::math::Quaternion endRotate;
    
    startRotate.RadianToQuaternion( 
      m_SettingData.startPointData.rotate.z,
      m_SettingData.startPointData.rotate.y,
      m_SettingData.startPointData.rotate.x
      );
    endRotate.RadianToQuaternion( 
      m_SettingData.endPointData.rotate.z,
      m_SettingData.endPointData.rotate.y,
      m_SettingData.endPointData.rotate.x
      );

    nowRotate.Slerp( startRotate, endRotate, ratio );

    // 距離を補間
    f32 nowDistance;
    nowDistance = m_SettingData.startPointData.distance + ( m_SettingData.endPointData.distance - m_SettingData.startPointData.distance ) * ratio;


    // カメラ制御を更新
    targetPos.y += 40.0 * 10.0f / 3.0f;
    m_NowTargetPos = targetPos;

    gfl2::math::Vector vecEye( 0.0f, 0.0f, nowDistance );
    gfl2::math::Matrix rotMat;
    nowRotate.QuaternionToMatrix( rotMat );
    vecEye = rotMat * vecEye;
    m_NowCameraPos = m_NowTargetPos + vecEye;

        // 座標補間
    if( m_InterpolationCount > 0 )
    {
      m_InterpolationCount--;
      f32 t = static_cast<f32>( m_InterpolationCount ) / m_InterpolationTime;
      //t = gfl2::math::FSin( PI * 0.5f * t );
      t = t * t;

#if 0
      m_NowTargetPos = m_NowTargetPos + ( m_PrevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( m_PrevCameraPos - m_NowCameraPos ) * t;
#else

      gfl2::math::Vector targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;
      gfl2::math::Vector prevTargetPos( targetPos + m_PrevTargetOffset );
      gfl2::math::Vector prevCameraPos( prevTargetPos + m_PrevCameraOffset );

      m_NowTargetPos = m_NowTargetPos + ( prevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( prevCameraPos - m_NowCameraPos ) * t;
#endif
    }


  }

  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );
  }

  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->cameraPos = m_NowCameraPos;
    pCameraData->targetPos = m_NowTargetPos;
  }

  virtual u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  virtual b32 IsManual( void ) const
  {
    return false;
  }

#if 0
  virtual void OutPutData( std::ofstream* pOfstream ) const
  {
    // 出力
    *pOfstream << "  <CameraData MeshName=\"" << m_Name << "\" Type =\"TYPE_RELATIVE_ROTATE_RATIO\" >" << std::endl; 

    OutPutRationPointData( pOfstream, "StartPointData", m_SettingData.startPointData );
    OutPutRationPointData( pOfstream, "EndPointData", m_SettingData.endPointData );
    
    *pOfstream << "  </CameraData>" << std::endl; 
  }
#endif

private:
#if 0
  virtual void OutPutRationPointData( std::ofstream* pOfstream, const c8* pName, const CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA& rData ) const
  {

    *pOfstream << "    <" << pName << ">" << std::endl; 
    *pOfstream << "      <Rotate x=\"" <<  rData.rotate.x << "\" " << "y=\"" << rData.rotate.y << "\" " << "z=\"" << rData.rotate.z << "\" />" << std::endl;
    *pOfstream << "      <Distance>" << rData.distance << "</Distance>" << std::endl; 
    *pOfstream << "    </" << pName << ">" << std::endl; 
  }
#endif

  // 設定情報
  u32 m_DataID;
  CAMERA_DATA_RELATIVE_ROTATE_RATIO m_SettingData;

  // 最新状態
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;
  fld::mmodel::MoveModel* m_pTarget;

  // 前回情報
  u32                     m_InterpolationCount;
  u32                     m_InterpolationTime;
  gfl2::math::Vector      m_PrevTargetPos;
  gfl2::math::Vector      m_PrevCameraPos;
  gfl2::math::Vector      m_PrevTargetOffset;
  gfl2::math::Vector      m_PrevCameraOffset;
};

class FieldCameraActionBase_Debug : public FieldCameraActionBase
{
public:

  FieldCameraActionBase_Debug( fld::mmodel::MoveModel* pTarget, const FieldCameraActionBase* pPrev = NULL ) 
    : m_InterpolationFlag( false ), 
    m_DataID( 0xFFFF ), 
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f ), 
    m_Distance( 514.0f ),
    m_Rot( -0.604547f, -0.0223443f, 0.0f ),
    m_TargetPos( 0.0f, 0.0f, 0.0f )
  {
    m_pTarget = pTarget;

    // 引継ぎ
    if( pPrev )
    {

    }

    // strcpy_s( m_Name, GFL_NELEMS( m_Name ), "DebugMode" );
  }

  virtual ~FieldCameraActionBase_Debug( void )
  {
  }

  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };


#if 0
    // 入力情報格納
    const gfl2::system::InputDevice::StickPos& pos
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
    const gfl2::system::InputDevice::StickPos& pos_r
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Right );
#if 0
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );
#endif
    // if( field::FieldDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
      m_TargetPos = m_pTarget->GetTransform()->GetTranslation();
      m_TargetPos.y += 40.0 * 10.0f / 3.0f;
    }

    // 更新処理
    if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
    {
      // field::FieldDebugInfo::m_CameraTargetDraw = true;

      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // オフセット移動
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        // field::FieldDebugInfo::m_CameraTargetOffset.x -= lx;
        // field::FieldDebugInfo::m_CameraTargetOffset.z -= ly;
      }

      if(pad_u || pad_d )
      {
        float diff = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
#if 0
        if ( pad_u ) { field::FieldDebugInfo::m_CameraTargetOffset.y += diff; }
        if ( pad_d ) { field::FieldDebugInfo::m_CameraTargetOffset.y -= diff; }
#endif
      }
    }
    else
    {
      // field::FieldDebugInfo::m_CameraTargetDraw = false;

      // カメラ回転
      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        m_Rot.x -= ly * 0.01f;
        m_Rot.y += lx * 0.01f;
        if(m_Rot.x < -1.5f) { m_Rot.x = -1.5f; }
        if(m_Rot.x > 1.5f)  { m_Rot.x = 1.5f; }
      }
      // カメラズーム
      if(pad_u || pad_d )
      {
        float diff = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { m_Distance -= diff; }
        if ( pad_d ) { m_Distance += diff; }
        if( m_Distance > 2048.0f){ m_Distance = 2048.0f; }
        if( m_Distance < 0.0f)   { m_Distance = 0.0f; }
      }
    }

    // 状態更新

    // ターゲットずらすぞ!
    float sinYaw = sinf( m_Rot.y ) * -1.0f;
    float cosYaw = cosf( m_Rot.y ) * -1.0f;

    gfl2::math::Vector targetOffset( 0.0f, 0.0f, 0.0f );

#if 0
    targetOffset.x = field::FieldDebugInfo::m_CameraTargetOffset.x * cosYaw - field::FieldDebugInfo::m_CameraTargetOffset.z * sinYaw;
    targetOffset.y = field::FieldDebugInfo::m_CameraTargetOffset.y;
    targetOffset.z = field::FieldDebugInfo::m_CameraTargetOffset.x * sinYaw + field::FieldDebugInfo::m_CameraTargetOffset.z * cosYaw;
#endif
    m_NowTargetPos = m_TargetPos + targetOffset;

    gfl2::math::Vector vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;

    m_NowCameraPos = m_NowTargetPos + vecEye;
#endif
  }

  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );
  }

  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->cameraPos = m_NowCameraPos;
    pCameraData->targetPos = m_NowTargetPos;
  }

  virtual u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  virtual b32 IsManual( void ) const
  {
    return true;
  }

#if 0
  virtual void OutPutData( std::ofstream* pOfstream ) const
  {
    gfl2::math::Vector nowRot( m_Rot );

    *pOfstream << "<CameraData MeshName=\"\"> Type =\"TYPE_RELATIVE_ROTATE\" >" << std::endl; 
    *pOfstream << "  <Rotate x=\"" <<  nowRot.x << "\" " << "y=\"" << nowRot.y << "\" " << "z=\"" << nowRot.z << "\" />>" << std::endl;
    *pOfstream << "  <Distance>" << m_Distance << "</Distance>" << std::endl; 
    *pOfstream << "</CameraData>" << std::endl; 
  };
#endif

private:

  b32 m_InterpolationFlag;
  u32 m_DataID;

  // 最新状態
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector      m_Rot;
  

  gfl2::math::Vector      m_TargetPos;
  fld::mmodel::MoveModel* m_pTarget;
};


class FieldCameraActionBase_Snap : public FieldCameraActionBase
{
public:

  enum SnapAction
  {
    SNAP_ACTION_MOVE = 0, // 通常移動
    SNAP_ACTION_UP,       // しゃがんだ状態から戻る
    SNAP_ACTION_DOWN,     // しゃがむ
  };

  FieldCameraActionBase_Snap( fld::mmodel::MoveModel* pTarget, FieldCamera::ProjectionSetting* pProjectionSetting, const FieldCameraActionBase* pPrev = NULL ) 
    : m_InterpolationFlag( false ), 
    m_DataID( 0xFFFE ),
    m_SnapAciton( SNAP_ACTION_MOVE ),
    m_IsDown( false ),
    m_DownOffset( 0.0f, 0.0f, 0.0f ),
    m_MoveOffset( 0.0f, 0.0f, 0.0f ),
    m_EyeVector( 0.0f, 0.0f, 1.0f ),
    m_EyeOffset( 0.0f, 0.0f, 0.0f ),
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f ), 
    m_Distance( 514.0f ),
    m_Rot( 0.0f, 0.0f, 0.0f ),
    m_TargetPos( 0.0f, 0.0f, 0.0f )
  {
    m_pTarget = pTarget;
    m_pProjectionSetting = pProjectionSetting;

    // 引継ぎ
    if( pPrev )
    {

    }

    // m_Rot.y = pTarget->GetTransform()->GetAngle();
    // strcpy_s( m_Name, GFL_NELEMS( m_Name ),"Snap" );

    m_pProjectionSetting->aov = 35.0f;
  }

  virtual ~FieldCameraActionBase_Snap( void )
  {
  }

  gfl2::math::Vector GetEyeVector( float rot_y )
  {
    float sinYaw    = sinf(rot_y);
    float cosYaw    = cosf(rot_y);
    float sinPitch  = 0.0f; //sinf(rot_x);
    float cosPitch  = 1.0f; //cosf(rot_x);
    float sinRoll   = 0.0f; //sinf(rot_z);
    float cosRoll   = 1.0f; //cosf(rot_z);

    gfl2::math::Vector vec;
    //	vec = gfl2::math::Vector(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
    vec = gfl2::math::Vector(sinYaw * 1.0f, 0.0f, cosYaw * 1.0f);
    vec.Normalize3();
    return vec;
  }


  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };
#if 0
    // 入力情報格納
    const gfl2::system::InputDevice::StickPos& pos
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
    const gfl2::system::InputDevice::StickPos& pos_r
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Right );
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );

    gfl2::math::Vector eyePositon;
    gfl2::math::Vector eyeOffset( 0.0f, 130.0f, 25.0f );


    // 自機制御→目オフセットを回転
    // gfl2::math::Matrix eyeOffsetRotMat( gfl2::math::Matrix::GetRotationY( m_pTarget->GetTransform()->GetAngle() ) );
    gfl2::math::Matrix eyeOffsetRotMat( gfl2::math::Matrix::GetRotationY( 0.0f ) );
    eyeOffset = eyeOffsetRotMat * eyeOffset;

    eyePositon = m_pTarget->GetTransform()->GetTranslation();
    eyePositon += eyeOffset;
    
    switch( m_SnapAciton )
    {
    case SNAP_ACTION_MOVE:
      {
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
        {
          if( pos_r.x || pos_r.y )
          {
            // オフセット移動
            float rate = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
            if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
            {
              rate *= 2.0f;
            }

            float lx = pos_r.x * rate;
            float ly = pos_r.y * rate;
            m_MoveOffset.x -= lx;
            m_MoveOffset.y -= ly;

            f32 moveMax = 20.0f;
            m_MoveOffset.x = gfl2::math::Clamp( m_MoveOffset.x, -moveMax, moveMax );
            m_MoveOffset.y = gfl2::math::Clamp( m_MoveOffset.y, -moveMax, moveMax );
          }
        }
        else
        {
          // 回転
          if( pos_r.x || pos_r.y )
          {
            float rate = 5.0f; // * field::FieldDebugInfo::m_CameraSpeed;
            if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
            {
              rate *= 2.0f;
            }
            float lx = pos_r.x * rate;
            float ly = pos_r.y * rate;
            m_Rot.x += ly * 0.01f;
            m_Rot.y -= lx * 0.01f;
            if( m_Rot.x < -1.5f ) { m_Rot.x = -1.5f; }
            if( m_Rot.x > 1.5f )  { m_Rot.x = 1.5f; }
          }
        }

        // ズーム
        if(pad_u || pad_d )
        {
          float diff = 0.5f; // * field::FieldDebugInfo::m_CameraSpeed;
          if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
          {
            diff *= 2.0f;
          }
          if ( pad_u ) { m_pProjectionSetting->aov -= diff; }
          if ( pad_d ) { m_pProjectionSetting->aov += diff; }

          f32 diffMax = 20.0f;
          m_pProjectionSetting->aov = gfl2::math::Clamp( m_pProjectionSetting->aov, 35.0f - diffMax, 35.0f + diffMax );
        }

        // アクション遷移
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::B ) )
        {
          if( m_IsDown )
          {
            m_SnapAciton = SNAP_ACTION_UP;
          }
          else
          {
            m_SnapAciton = SNAP_ACTION_DOWN;
          }
        }

        break;
      }

     case SNAP_ACTION_DOWN:
      {
        m_DownOffset.y -= 3.0f;
        if( m_DownOffset.y < - 60.0f )
        {
          m_DownOffset.y = -60.0f;
          m_IsDown = true;
          m_SnapAciton = SNAP_ACTION_MOVE;
        }

        break;
      }

     case SNAP_ACTION_UP:
      {
        m_DownOffset.y += 3.0f;
        if( m_DownOffset.y >= 0.0f )
        {
          m_DownOffset.y = 0.0f;
          m_IsDown = false;
          m_SnapAciton = SNAP_ACTION_MOVE;
        }

        break;
      }
    }

    // 目移動オフセットを回転
    gfl2::math::Vector moveOffsetWork( m_MoveOffset );
    // gfl2::math::Matrix eyeMoveOffsetRotMat( gfl2::math::Matrix::GetRotationY( m_pTarget->GetTransform()->GetAngle() ) );
    gfl2::math::Matrix eyeMoveOffsetRotMat( gfl2::math::Matrix::GetRotationY( 0.0f ) );
    moveOffsetWork = eyeMoveOffsetRotMat * moveOffsetWork;

    // 目の位置を算出
    eyePositon += m_DownOffset;
    eyePositon += moveOffsetWork;

    // 視線ベクトルを作成
    gfl2::math::Vector eyeVector( 0.0f, 0.0f, 1.0f );
    gfl2::math::Matrix eyeRotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
    eyeVector = eyeRotMat * eyeVector;

    m_NowCameraPos = eyePositon;
    m_NowTargetPos = eyePositon + eyeVector;

    // @todo 自機の向きを調整、本来ならこれは上層システムが行うべき
    // m_pTarget->GetTransform()->SetAngle( m_Rot.y );
#endif
#if 0
    if( field::FieldDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
    }

    // 更新処理
    if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
    {
      field::FieldDebugInfo::m_CameraTargetDraw = true;

      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // オフセット移動
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        field::FieldDebugInfo::m_CameraTargetOffset.x -= lx;
        field::FieldDebugInfo::m_CameraTargetOffset.z -= ly;
      }

      if(pad_u || pad_d )
      {
        float diff = 5.0f * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { field::FieldDebugInfo::m_CameraTargetOffset.y += diff; }
        if ( pad_d ) { field::FieldDebugInfo::m_CameraTargetOffset.y -= diff; }
      }
    }
    else
    {
      field::FieldDebugInfo::m_CameraTargetDraw = false;

      // カメラ回転
      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        m_Rot.x -= ly * 0.01f;
        m_Rot.y += lx * 0.01f;
        if(m_Rot.x < -1.5f) { m_Rot.x = -1.5f; }
        if(m_Rot.x > 1.5f)  { m_Rot.x = 1.5f; }
      }
      // カメラズーム
      if(pad_u || pad_d )
      {
        float diff = 5.0f * field::FieldDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { m_Distance -= diff; }
        if ( pad_d ) { m_Distance += diff; }
        if( m_Distance > 2048.0f){ m_Distance = 2048.0f; }
        if( m_Distance < 0.0f)   { m_Distance = 0.0f; }
      }
    }

    // 状態更新

    // ターゲットずらすぞ!
    float sinYaw = sinf( m_Rot.y ) * -1.0f;
    float cosYaw = cosf( m_Rot.y ) * -1.0f;

    gfl2::math::Vector targetOffset;

    targetOffset.x = field::FieldDebugInfo::m_CameraTargetOffset.x * cosYaw - field::FieldDebugInfo::m_CameraTargetOffset.z * sinYaw;
    targetOffset.y = field::FieldDebugInfo::m_CameraTargetOffset.y;
    targetOffset.z = field::FieldDebugInfo::m_CameraTargetOffset.x * sinYaw + field::FieldDebugInfo::m_CameraTargetOffset.z * cosYaw;

    m_NowTargetPos = m_TargetPos + targetOffset;

    gfl2::math::Vector vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;

    m_NowCameraPos = m_NowTargetPos + vecEye;
#endif
  }

  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Vector3 pos( m_NowCameraPos.x, m_NowCameraPos.y, m_NowCameraPos.z );
    gfl2::math::Vector3 target( m_NowTargetPos.x, m_NowTargetPos.y, m_NowTargetPos.z );
    *pMtx = gfl2::math::Matrix34::GetLookAt( pos, target, gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )  );
  }

  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const
  {
    pCameraData->cameraPos = m_NowCameraPos;
    pCameraData->targetPos = m_NowTargetPos;
  }

  virtual u32 GetDataID( void ) const
  {
    return m_DataID;
  }

  virtual b32 IsManual( void ) const
  {
    return true;
  }

  virtual void OutPutData( std::ofstream* pOfstream ) const
  {
  }


private:

  b32 m_InterpolationFlag;
  u32 m_DataID;
  SnapAction              m_SnapAciton;
  b32                     m_IsDown;
  gfl2::math::Vector      m_DownOffset;
  gfl2::math::Vector      m_MoveOffset;
  f32                     m_ZoomOffset;

  FieldCamera::ProjectionSetting* m_pProjectionSetting;
  

  gfl2::math::Vector      m_EyeVector;
  gfl2::math::Vector      m_EyeOffset;

  // 最新状態
  gfl2::math::Vector      m_NowTargetPos;
  gfl2::math::Vector      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector      m_Rot;
  

  gfl2::math::Vector      m_TargetPos;
  fld::mmodel::MoveModel* m_pTarget;
};

/**
* @brief   コンストラクタ
*
* @return  無し 
*/
FieldCamera::FieldCamera( void ) : m_pCamera( NULL ), m_pDebugCamera( NULL )
{
  m_ProjectionSetting.aov = 0.0f;
  m_ProjectionSetting.aspect = 0.0f;
  m_ProjectionSetting.farZ = 0.0f;
  m_ProjectionSetting.nearZ = 0.0f;
}

/**
* @brief   デストラクタ
*
* @return  無し 
*/
FieldCamera::~FieldCamera( void )
{
  Terminate();
}

/**
* @brief   初期化処理
*
* @param   pTarget ターゲット
*
* @return  無し 
*/
void FieldCamera::Initialize( fld::mmodel::MoveModel* pTarget )
{
  m_pTarget = pTarget;
  // m_pCamera = new FieldCameraActionBase_Debug( m_pTarget );
}

/**
* @brief   終了処理
*
* @return  無し 
*/
void FieldCamera::Terminate( void )
{
  GFL_SAFE_DELETE( m_pCamera );
  GFL_SAFE_DELETE( m_pDebugCamera );
  
}

/**
* @brief   更新処理
*
* @return  無し 
*/
void FieldCamera::Update( void )
{
  enum{
    PlayerPadNo = 0,
  };

#if 0
  // 更新処理
  if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::B ) )
  {
    FieldCameraActionRelativeRotate* pNext = new FieldCameraActionRelativeRotate( m_pTarget, m_pCamera );
    GFL_SAFE_DELETE( m_pCamera );
    m_pCamera = pNext;
  }
#endif 

  if( m_pCamera )
  {
    m_pCamera->Update();
  }

  if( m_pDebugCamera )
  {
    m_pDebugCamera->Update();
  }

}

b32 FieldCamera::Change( const CAMERA_DATA* pCameraDara, b32 isForce )
{
  if( m_pCamera && isForce == false )
  {
    if( m_pCamera->GetDataID() == pCameraDara->id )
    {
      return false;
    }
  }

  // 補間フレーム
  u32 interpolationFrame = 0;

  if( isForce == false )
  {
    interpolationFrame = pCameraDara->interpolationData.defaultFrame;

    if( m_pCamera )
    {
      for( u32 i = 0; i < pCameraDara->interpolationData.uniqueInterpolationDataSize; i++ )
      {
        if( strcmp( pCameraDara->interpolationData.pUniqueInterpolationDataTable[i].name, m_pCamera->m_Name ) == 0 )
        {
          interpolationFrame = pCameraDara->interpolationData.pUniqueInterpolationDataTable[i].frame;
          break;
        }
      }
    }
  }

  // 射影設定
  // @todo 指定がなければ継承できるようにする？
  m_ProjectionSetting.aov = pCameraDara->projectionData.aov;

  switch( pCameraDara->type )
  {
  case TYPE_RELATIVE_ROTATE:
    {
      FieldCameraActionRelativeRotate* pNext = new FieldCameraActionRelativeRotate( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_RELATIVE_ROTATE*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      pNext->m_pProjectionSetting = &m_ProjectionSetting;
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo
      // strcpy_s( m_pCamera->m_Name, GFL_NELEMS( m_pCamera->m_Name ), pCameraDara->name );
      return true;
    }

  case TYPE_RELATIVE_ROTATE_RATIO:
    {
      FieldCameraActionRelativeRotateRatio* pNext = new FieldCameraActionRelativeRotateRatio( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_RELATIVE_ROTATE_RATIO*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      pNext->m_pProjectionSetting = &m_ProjectionSetting;
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo 
      // strcpy_s( m_pCamera->m_Name, GFL_NELEMS( m_pCamera->m_Name ), pCameraDara->name );
      return true;
    }

  case TYPE_STOP:
    {
      FieldCameraActionStop* pNext = new FieldCameraActionStop( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_STOP*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      pNext->m_pProjectionSetting = &m_ProjectionSetting;
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo 
      // strcpy_s( m_pCamera->m_Name, GFL_NELEMS( m_pCamera->m_Name ), pCameraDara->name );
      return true;
    }


  }

  return false;

}

b32 FieldCamera::ChangeDebugCamera( void )
{
  FieldCameraActionBase_Debug* pNext = new FieldCameraActionBase_Debug( m_pTarget );
  pNext->m_pProjectionSetting = &m_ProjectionSetting;
  GFL_SAFE_DELETE( m_pCamera );
  m_pCamera = pNext;
  return true;
}

b32 FieldCamera::ChangeSnapCamera( void )
{
  FieldCameraActionBase_Snap* pNext = new FieldCameraActionBase_Snap( m_pTarget, &m_ProjectionSetting );
  //pNext->m_pProjectionSetting = &m_ProjectionSetting;
  GFL_SAFE_DELETE( m_pCamera );
  m_pCamera = pNext;
  return true;
}


b32 FieldCamera::ChangeTopCamera( void )
{
  FieldCameraActionBase_Top* pNext = new FieldCameraActionBase_Top( m_pTarget );
  pNext->m_pProjectionSetting = &m_ProjectionSetting;
  GFL_SAFE_DELETE( m_pDebugCamera );
  m_pDebugCamera = pNext;
  return true;
}

void FieldCamera::EndDebugCamera( void )
{
  GFL_SAFE_DELETE( m_pDebugCamera );
}

/**
* @brief   カメラターゲット座標を取得
*
* @param   pOut 格納先
*
* @return  無し 
*/
void FieldCamera::GetTargetPosition( gfl2::math::Vector* pOut ) const
{
  if( m_pDebugCamera )
  {

  }


  if( m_pCamera )
  {
    FieldCameraActionBase::CAMERA_DATA cameraData;
    m_pCamera->GetCameraData( &cameraData );
    *pOut = cameraData.targetPos;
  }
}

/**
* @brief   カメラポジション座標を取得
*
* @param   pOut 格納先
*
* @return  無し 
*/
void FieldCamera::GetCameraPosition( gfl2::math::Vector* pOut ) const
{
  if( m_pCamera )
  {
    FieldCameraActionBase::CAMERA_DATA cameraData;
    m_pCamera->GetCameraData( &cameraData );
    *pOut = cameraData.cameraPos;
  }
}

/**
* @brief   ビュー行列取得
*
* @param   pMtx 結果格納先
*
* @return  無し 
*/

void FieldCamera::GetViewMatrix( gfl2::math::Matrix34* pMtx ) const
{
  GetViewCamera()->GetViewMatrix( pMtx );
}

/**
* @brief   射影行列取得
*
* @param   pMtx       結果格納先
* @param   pivotFlag  trueのとき、左回転済みの行列を取得する(CTRのみ使用する)
*
* @return  無し 
*/
void FieldCamera::GetProjectionMatrix( gfl2::math::Matrix44* pMtx, b32 pivotFlag ) const
{
  GetViewCamera()->GetProjectionMatrix( pMtx, pivotFlag );

#if 0
  *pMtx = gfl2::math::Matrix44::GetPerspective( 
    gfl2::math::ConvDegToRad( m_ProjectionSetting.aov ), 
    m_ProjectionSetting.aspect, 
    m_ProjectionSetting.nearZ, 
    m_ProjectionSetting.farZ,
    true
    );

#endif
#if 0
  nw::math::MTX44 proj;
  gfl2::math::MTX44PerspectivePivotRad(
    &proj,
    gfl2::math::ConvDegToRad( m_ProjectionSetting.aov ), 
    m_ProjectionSetting.aspect, 
    m_ProjectionSetting.nearZ, 
    m_ProjectionSetting.farZ,
    gfl2::math::PIVOT_UPSIDE_TO_TOP );
  *pMtx = gfl2::math::ConvertNwMtx44ToGfMtx44( proj );
#endif
}

/**
* @brief   カリング用行列取得
*
* @param   pMtx 結果格納先
*
* @return  無し 
*/
void FieldCamera::GetCullingMatrix( gfl2::math::Matrix44* pMtx ) const
{
  const FieldCameraActionBase* pCamera = GetGameCamera();

  gfl2::math::Matrix34 view;
  pCamera->GetProjectionMatrix( pMtx, false );
  pCamera->GetViewMatrix( &view );
  *pMtx = ( *pMtx ) * view;
}

/**
* @brief   射影行列関連パラメータを設定
*
* @param   rSetting  設定内容
*
* @return  無し 
*/
void FieldCamera::SetProjectionSetting( const ProjectionSetting& rSetting )
{
  m_ProjectionSetting = rSetting;
}

/**
* @brief   射影行列関連パラメータを取得
*
* @param   pSetting 結果格納先
*
* @return  無し 
*/
void FieldCamera::GetProjectionSetting( ProjectionSetting* pSetting ) const
{
  *pSetting = m_ProjectionSetting;
}

/**
* @brief   カメラ方向のY軸回転値を取得
*
* @return  Y軸回転値(ラジアン)
*/
f32 FieldCamera::GetYaw( void ) const
{
  // @note 一旦XZ平面のみで考えるよ！
  gfl2::math::Vector pos;
  gfl2::math::Vector target;
  gfl2::math::Vector vec;

  GetCameraPosition( &pos );
  GetTargetPosition( &target );
  vec = target - pos;
  vec = vec.Normalize3();

  return gfl2::math::NormalizeRadianAngle( atan2( vec.x, vec.z ) + PI ); 
}

/**
* @brief   カメラ操作の入力を行っているか取得
*
* @return  行っているならtrue、行っていないならfalse
*/
b32 FieldCamera::IsManual( void ) const
{

  if( m_pCamera )
  {
    return m_pCamera->IsManual();
  }
  else
  {
    return false;
  }
}

/**
* @brief   デバック機能を制御
*
* @param   enable デバック機能を有効化するならtrue、無効化するならfalse
*
* @return  無し
*/
void FieldCamera::SetDebug( b32 enable )
{
  if( m_pCamera )
  {
    m_pCamera->SetDebugMode( enable );
  }
}

/**
* @brief   デバックカメラになっているか取得
*
* @return  デバックカメラならtrue、通常カメラならfalse
*/
b32 FieldCamera::IsDebug( void ) const
{
  if( m_pCamera )
  {
    return ( m_pCamera->IsDebugMode());
  }
  else
  {
    return false;
  }
}

#if PM_DEBUG
void FieldCamera::Dump( void )
{
  if( m_pCamera )
  {
    m_pCamera->Dump();
  }
}
#endif // #if PM_DEBUG


void FieldCamera::OutPutData( std::ofstream* pOfstream ) const
{
  if( m_pCamera )
  {
    m_pCamera->OutPutData( pOfstream );
  }
}

const c8* FieldCamera::GetDataName( void ) const
{
  if( m_pCamera )
  {
    return &m_pCamera->m_Name[0];
  }

  return NULL;
}

// デバッグカメラが有効でも通常カメラが返る
const FieldCameraActionBase* FieldCamera::GetGameCamera( void ) const
{
  return m_pCamera;
}

// デバッグカメラが有効ならデバッグカメラが返る
const FieldCameraActionBase* FieldCamera::GetViewCamera( void ) const
{
  if( m_pDebugCamera )
  {
    return m_pDebugCamera;
  }

  return m_pCamera;
}


GFL_NAMESPACE_END( camera );
GFL_NAMESPACE_END( field );
