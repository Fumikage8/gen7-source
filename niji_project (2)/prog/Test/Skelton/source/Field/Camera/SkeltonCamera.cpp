#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCamera.cpp
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

// niji
#include "GameSys/include/GameManager.h"

#include "Test/Skelton/include/Field/Camera/SkeltonCamera.h"
#include "Test/Skelton/include/Field/Camera/SkeltonCameraUtility.h"
#include "Test/Skelton/include/SkeltonDebugInfo.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"
#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"
#include <system/include/InputDevice/gfl2_Pad.h>
// #include <math/include/gfl2_Quaternion.h>

#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

static const f32 ROTATE_LIMMIT_MIX_X( gfl2::math::ConvDegToRad( -80 ) );
static const f32 ROTATE_LIMMIT_MAX_X( gfl2::math::ConvDegToRad( 80 ) );

void GetRotate( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector4* pOut );


void RotateQuaternion( const gfl2::math::Vector4& rSrc, const LIMIT_DATA& rRimitData, const gfl2::math::Vector4& rCenter, f32 distance, const LIMIT_DATA& rPositionRimitData, gfl2::math::Quaternion* pOut )
{
  gfl2::math::Vector4 rot;
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

void RotateQuaternion( const gfl2::math::Vector4& rSrc, gfl2::math::Quaternion* pOut )
{
  gfl2::math::Vector4 rot;
  GetRotate( *pOut, &rot );

  rot.x += rSrc.x;
  rot.y += rSrc.y;

  rot.x = gfl2::math::Clamp( rot.x, ROTATE_LIMMIT_MIX_X, ROTATE_LIMMIT_MAX_X );
  pOut->RadianToQuaternion( rot.z, rot.y, rot.x );
}

void GetRotate( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector4* pOut )
{
  // クォータニオンからオイラー角を求める
  gfl2::math::Vector4 nowRot( 0.0f, 0.0f, 0.0f );
  {
    gfl2::math::Vector4 vec1( 0.0f, 0.0f, 1.0f );
    gfl2::math::Matrix rotMat;
    rSrc.QuaternionToMatrix( rotMat );
    vec1 = rotMat * vec1;

    gfl2::math::Vector4 vec2( vec1.x, 0.0f, vec1.z );
    vec2 = vec2.Normalize3();

    nowRot.y = atan2( vec2.x, vec2.z );

    // Y回転を打ち消す
    rotMat = gfl2::math::Matrix::GetRotationY( - nowRot.y ) * rotMat;
    vec1 = rotMat * gfl2::math::Vector4( 0.0f, 0.0f, 1.0f );

    gfl2::math::Vector4 vec3( 0.0f, vec1.y, vec1.z );
    vec3 = vec3.Normalize3();
    nowRot.x = atan2( vec3.y * -1.0f, vec3.z );
  }

  *pOut = nowRot;
}


struct MAYA_PARAM
{
  gfl2::math::Vector3 offset;
  gfl2::math::Vector3 rot;
  f32 distance;
  u32 interpolationFrame;
  f32 fovY;
};

// Maya上でコピペできる型式で出力
void OutputMayaData( const MAYA_PARAM& rMaya )
{
  char buffer[2048];
  u32 start = 0;

#if defined(GF_PLATFORM_WIN32)
#define CAMERA_PUTS_TEXT( text, ... ) start += sprintf_s( &buffer[ start ], sizeof( buffer ) - start, text, __VA_ARGS__ )
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_PUTS_TEXT( ... ) start += sprintf( &buffer[ start ], __VA_ARGS__ )
#endif

  // Mayaインポート用情報出力
  CAMERA_PUTS_TEXT( "<Root>\r\n");
  CAMERA_PUTS_TEXT( "  <Data>\r\n");
  CAMERA_PUTS_TEXT( "    <MainParam>\r\n");
  CAMERA_PUTS_TEXT( "      <Type>Static</Type>\r\n");
  CAMERA_PUTS_TEXT( "      <CompFrame>%d</CompFrame>\r\n", rMaya.interpolationFrame );
  CAMERA_PUTS_TEXT( "      <PointAParam>\r\n");

  CAMERA_PUTS_TEXT( "        <Setting>UserSet</Setting>\r\n");
  CAMERA_PUTS_TEXT( "        <Ofs>%f %f %f</Ofs>\r\n", rMaya.offset.x, rMaya.offset.y, rMaya.offset.z );
  CAMERA_PUTS_TEXT( "        <Axis>%f %f %f</Axis>\r\n", 
    rMaya.rot.x,
    rMaya.rot.y,
    rMaya.rot.z );
  CAMERA_PUTS_TEXT( "        <Fov>%f</Fov>\r\n", rMaya.fovY );
  CAMERA_PUTS_TEXT( "        <Distance>%f</Distance>\r\n", rMaya.distance );

  CAMERA_PUTS_TEXT( "      </PointAParam>\r\n");
  CAMERA_PUTS_TEXT( "    </MainParam>\r\n");
  CAMERA_PUTS_TEXT( "  </Data>\r\n");
  CAMERA_PUTS_TEXT( "</Root>\r\n");

  buffer[ start ] = '\0';
  gfl2::fs::PcUtil::WriteFile( "R:/home/momiji/tool/FieldTools/Maya/Resource/camera_adjustment.txt", buffer, start );

}


class RotationLengthData
{
  void Set( const gfl2::math::Vector4& rRot, f32 distance )
  {
    m_Rotation.RadianToQuaternion( rRot.z, rRot.y, rRot.x );
    m_Distance = distance;
  }

  void SetByOffsetData( const gfl2::math::Vector4& rOffset )
  {
  }

  void GetRotate( gfl2::math::Vector4* pOut ) const
  {

  }

// private:
  f32                     m_Distance;
  gfl2::math::Quaternion  m_Rotation;
};

class SkeltonCameraActionBase
{
public:

  struct CAMERA_DATA
  {
    gfl2::math::Vector4      targetPos;
    gfl2::math::Vector4      cameraPos;
  };

  SkeltonCameraActionBase( void ) : m_DebugModeEnable( false )
  {
    m_Name[0] = '\0'; 
  } 

  virtual ~SkeltonCameraActionBase( void )
  {
  }

  virtual void Update( void ) = 0;
  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const = 0;
  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const = 0;
  virtual u32 GetDataID( void ) const = 0;
  virtual b32 IsManual( void ) const = 0;
  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const{};

  virtual b32 IsDebugMode( void ) const { return m_DebugModeEnable; }
  virtual void SetDebugMode( b32 enable ) { m_DebugModeEnable = enable; }


  c8   m_Name[512];                  // 識別名(今はmesh名)
  b32  m_DebugModeEnable;
};

// 
class SkeltonCameraActionRelativeRotate : public SkeltonCameraActionBase
{
public:

  SkeltonCameraActionRelativeRotate( u32 id, const CAMERA_DATA_RELATIVE_ROTATE* pData, SkeltonModel* pTarget, u32 interpolationFrame, const SkeltonCameraActionBase* pPrev = NULL ) 
    :
    m_ManualResetFlag( false ),
    m_InterpolationFlag( false ), 
    m_DataID( 0 ), 
    m_StartRot( -0.604547f, -0.0223443f, 0.0f ),
    m_NowTargetPos( 0.0f, 0.0f, 0.0f ), 
    m_NowCameraPos( 0.0f, 500.0f, 500.0f ), 
    m_Distance( 514.0f ),
    m_Rot( -0.604547f, -0.0223443f, 0.0f ),
    m_TargetOffsetType( COORDINATE_SYSTEM_TYPE_WORLD ),
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
    m_StartTargetOffset = m_TargetOffset = pData->targetOffset;

    // 引継ぎ
    if( pPrev )
    {
      CAMERA_DATA prevCameraData;
      pPrev->GetCameraData( &prevCameraData );
      m_PrevCameraPos = prevCameraData.cameraPos;
      m_PrevTargetPos = prevCameraData.targetPos;

      // @todo 1フレーム遅延している
      gfl2::math::Vector4 targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;

      m_PrevTargetOffset = m_PrevTargetPos - targetPos;
      m_PrevCameraOffset = m_PrevCameraPos - m_PrevTargetPos;

      m_InterpolationCount = m_InterpolationTime;
    }

    m_StartRotation.RadianToQuaternion( m_StartRot.z, m_StartRot.y, m_StartRot.x );
    m_Rotation = m_StartRotation;
  }

  virtual ~SkeltonCameraActionRelativeRotate( void )
  {
  }

  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };

    m_ManualFlag = false;

    gfl2::math::Quaternion qt;

    // 入力情報格納
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );


    if( SkeltonDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
      m_TargetPos = m_pTarget->GetTransform()->GetTranslation();
      m_TargetPos.y += 40.0 * 10.0f / 3.0f;
    }

    Clamp( m_TargetPos, m_PositionLimitData, &m_TargetPos );

    if( m_DebugModeEnable )
    {
      SkeltonDebugInfo::m_CameraTargetDraw = true;
    }
    else
    {
      SkeltonDebugInfo::m_CameraTargetDraw = false;
    }

    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    const gfl2::system::InputDevice::StickPos& stickPos_r = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Right );

    gfl2::math::Vector3 pos( pVecDevice->GetX(), pVecDevice->GetY(), 0.0f );
    gfl2::math::Vector3 pos_r( stickPos_r.x, stickPos_r.y * -1.0f, 0.0f );

    //  bool pad_u = false;
    //  bool pad_d = false;
    //  bool pad_l = false;
    //  bool pad_r = false;
    //  pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
    //  pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
    //  pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
    //  pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );

    f32 stickValueX = pos_r.x;
    f32 stickValueY = pos_r.y;

    b32 debugUpdate = false;

    // 単軸モード
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      if( gfl2::math::Abs( stickValueX ) > gfl2::math::Abs( stickValueY ) )
      {
        stickValueY = 0.0f;
      }
      else
      {
        stickValueX = 0.0f;
      }
    }

    // 上下左右平行移動
    if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
    {
      if( stickValueX != 0.0f || stickValueY != 0.0f )
      {
        f32 moveValue( 2.0f );
        gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

        // 加速
        {
          if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
          {
            moveValue = 10.0f;
          }
        }

        // 入力値からローカル空間での移動量を算出
        {
          move.x = stickValueX * moveValue;
          move.y = stickValueY * moveValue;
        }

        // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
        {
          gfl2::math::Matrix34 rotMat;
          m_Rotation.QuaternionToMatrix( rotMat );
          move = rotMat.TransformCoord( move );
          m_TargetOffset += move;
        }

        debugUpdate = true;
      }
    }
    // 奥行左右平行移動
    else if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
    {
      f32 moveValue( 2.0f );
      gfl2::math::Vector3 offsetMove( 0.0f, 0.0f, 0.0f );
      f32 distanceMove( 0.0f );

      // 加速
      {
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          moveValue = 10.0f;
        }
      }

      // 入力値からローカル空間での移動量を算出
      {
        offsetMove.x = stickValueX * moveValue;
        distanceMove = stickValueY * moveValue * -1.0f;
      }

      // 距離
      {
        if( distanceMove != 0.0f )
        {
          m_Distance += distanceMove;
          m_Distance = gfl2::math::Clamp( m_Distance, 1.0f, 100000.0f );
          debugUpdate = true;
        }
      }

      // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
      {
        gfl2::math::Matrix34 rotMat;
        m_Rotation.QuaternionToMatrix( rotMat );
        offsetMove = rotMat.TransformCoord( offsetMove );
        if( offsetMove != gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ) )
        {
          m_TargetOffset += offsetMove;
          debugUpdate = true;
        }
      }
    }
    // カメラポジション操作
    else
    {
      SkeltonDebugInfo::m_InfoCameraManualMode = 0; 
      SkeltonDebugInfo::m_CameraTargetDrawColor.x = 0.0;

      // 回転
      if( stickValueY || stickValueX )
      {
        // 回転速度計算
        float rate = 0.05f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // 回転実行
        gfl2::math::Vector4 rot( stickValueY * rate * 1.0f, stickValueX * rate * -1.0f, 0.0f );
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
        float diff = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { m_Distance -= diff; }
        if ( pad_d ) { m_Distance += diff; }

        m_Distance = gfl2::math::Clamp( m_Distance, 0.0f, 100000.0f );
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

    gfl2::math::Vector4 targetOffset( m_TargetOffset );

    switch( m_TargetOffsetType )
    {
      // カメラの回転に合わせて、ターゲット座標オフセット値を回転
    case COORDINATE_SYSTEM_TYPE_LOCAL:
      {
        gfl2::math::Vector4 rot;
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
    gfl2::math::Vector4 vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;
#endif
   // m_NowCameraPos = m_StartRotation * ;
   // m_NowCameraPos = m_NowTargetPos + vecEye;
    
    gfl2::math::Vector4 vecEye( 0.0f, 0.0f, m_Distance );
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

      gfl2::math::Vector4 targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;
      gfl2::math::Vector4 prevTargetPos( targetPos + m_PrevTargetOffset );
      gfl2::math::Vector4 prevCameraPos( prevTargetPos + m_PrevCameraOffset );

      m_NowTargetPos = m_NowTargetPos + ( prevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( prevCameraPos - m_NowCameraPos ) * t;
#endif
    }
  }

  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix::GetIdentity();
    *pMtx = gfl2::math::Matrix::GetLookAt( m_NowCameraPos, m_NowTargetPos, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f )  );
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

  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const
  {
    // クォータニオンからオイラー角を求める
    gfl2::math::Vector4 nowRot( 0.0f, 0.0f, 0.0f );
    GetRotate( m_Rotation, &nowRot );
    
    // 度に変換
    ConvertRadianToDegree( &nowRot );

    // 出力
    *pOfstream << "  <CameraData MeshName=\"" << m_Name << "\" Type =\"TYPE_RELATIVE_ROTATE\" >" << std::endl; 
    *pOfstream << "    <Rotate x=\"" <<  nowRot.x << "\" " << "y=\"" << nowRot.y << "\" " << "z=\"" << nowRot.z << "\" />" << std::endl;
    *pOfstream << "    <Distance>" << m_Distance << "</Distance>" << std::endl; 
    *pOfstream << "    <TargetOffset Type=\"" << GetLabel( m_TargetOffsetType ) << "\" x=\"" <<  m_TargetOffset.x << "\" " << "y=\"" << m_TargetOffset.y << "\" " << "z=\"" << m_TargetOffset.z << "\" />" << std::endl;
    *pOfstream << "  </CameraData>" << std::endl; 
    

    // nijiカメラエリアのMayaバイナリにコピペできる型式で出力
    gfl2::math::Vector4 targetOffset( m_TargetOffset );

    switch( m_TargetOffsetType )
    {
      // カメラの回転に合わせて、ターゲット座標オフセット値を回転
    case COORDINATE_SYSTEM_TYPE_LOCAL:
      {
        gfl2::math::Vector4 rot;
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

    MAYA_PARAM mayaData;
    mayaData.distance = m_Distance;
    mayaData.fovY = rSetting.aov;
    mayaData.interpolationFrame = m_InterpolationTime;
    mayaData.offset.x = targetOffset.x;
    mayaData.offset.y = targetOffset.y;
    mayaData.offset.z = targetOffset.z;
    mayaData.rot.x = nowRot.x;
    mayaData.rot.y = nowRot.y;
    mayaData.rot.z = nowRot.z;
    OutputMayaData( mayaData );

  };


private:

  b32 m_ManualResetFlag;
  b32 m_InterpolationFlag;
  u32 m_DataID;
  
  LIMIT_DATA     m_PositionLimitData;
  LIMIT_DATA     m_PositionLimitDataEx;
  LIMIT_DATA     m_RotateLimitData;

  // 初期状態
  gfl2::math::Vector4      m_StartRot;
  gfl2::math::Quaternion  m_StartRotation;
  gfl2::math::Quaternion  m_PrevRotation;

  // ターゲット座標のオフセット値

  // 最新状態
  gfl2::math::Vector4      m_NowTargetPos;
  gfl2::math::Vector4      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector4      m_Rot;
  gfl2::math::Quaternion  m_Rotation;
  CoordinateSystemType    m_TargetOffsetType;
  gfl2::math::Vector4      m_TargetOffset;
  gfl2::math::Vector4      m_StartTargetOffset;
  

  gfl2::math::Vector4      m_TargetPos;
  SkeltonModel* m_pTarget;

  u32                     m_StickInterpolationCount;
  u32                     m_StickInterpolationTime;

  // 前回情報
  u32                     m_InterpolationCount;
  u32                     m_InterpolationTime;
  gfl2::math::Vector4      m_PrevTargetPos;
  gfl2::math::Vector4      m_PrevCameraPos;
  gfl2::math::Vector4      m_PrevTargetOffset;
  gfl2::math::Vector4      m_PrevCameraOffset;

  // 
  b32                     m_ManualFlag;

};

class SkeltonCameraActionStop : public SkeltonCameraActionBase
{
public:

  SkeltonCameraActionStop( u32 id, const CAMERA_DATA_STOP* pData, SkeltonModel* pTarget, u32 interpolationFrame, const SkeltonCameraActionBase* pPrev = NULL ) 
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

  virtual ~SkeltonCameraActionStop( void )
  {
  }

  virtual void Update( void )
  {
  }

  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix::GetIdentity();
    *pMtx = gfl2::math::Matrix::GetLookAt( m_NowCameraPos, m_NowTargetPos, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f )  );
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

  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const
  {
  };


private:

  u32 m_DataID;
  
  // 最新状態
  gfl2::math::Vector4      m_NowTargetPos;
  gfl2::math::Vector4      m_NowCameraPos;
  SkeltonModel* m_pTarget;

};

class SkeltonCameraActionRelativeRotateRatio : public SkeltonCameraActionBase
{
public:

  SkeltonCameraActionRelativeRotateRatio( u32 id, const CAMERA_DATA_RELATIVE_ROTATE_RATIO* pData, SkeltonModel* pTarget, u32 interpolationFrame, const SkeltonCameraActionBase* pPrev = NULL ) 
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
      gfl2::math::Vector4 targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;

      m_PrevTargetOffset = m_PrevTargetPos - targetPos;
      m_PrevCameraOffset = m_PrevCameraPos - m_PrevTargetPos;

      m_InterpolationCount = m_InterpolationTime;
    }
  }

  virtual ~SkeltonCameraActionRelativeRotateRatio( void )
  {
  }

  virtual void Update( void )
  {
    gfl2::math::Vector4 targetPos  = m_pTarget->GetTransform()->GetTranslation();
    gfl2::math::Vector4 startToEndVec( m_SettingData.endPointData.referencePoint - m_SettingData.startPointData.referencePoint );
    gfl2::math::Vector4 startToTargetVec( targetPos - m_SettingData.startPointData.referencePoint );
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

    gfl2::math::Vector4 vecEye( 0.0f, 0.0f, nowDistance );
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

      gfl2::math::Vector4 targetPos = m_pTarget->GetTransform()->GetTranslation();
      targetPos.y += 40.0 * 10.0f / 3.0f;
      gfl2::math::Vector4 prevTargetPos( targetPos + m_PrevTargetOffset );
      gfl2::math::Vector4 prevCameraPos( prevTargetPos + m_PrevCameraOffset );

      m_NowTargetPos = m_NowTargetPos + ( prevTargetPos - m_NowTargetPos ) * t;
      m_NowCameraPos = m_NowCameraPos + ( prevCameraPos - m_NowCameraPos ) * t;
#endif
    }


  }

  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix::GetIdentity();
    *pMtx = gfl2::math::Matrix::GetLookAt( m_NowCameraPos, m_NowTargetPos, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f )  );
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

  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const
  {
    // 出力
    *pOfstream << "  <CameraData MeshName=\"" << m_Name << "\" Type =\"TYPE_RELATIVE_ROTATE_RATIO\" >" << std::endl; 

    OutPutRationPointData( pOfstream, "StartPointData", m_SettingData.startPointData );
    OutPutRationPointData( pOfstream, "EndPointData", m_SettingData.endPointData );
    
    *pOfstream << "  </CameraData>" << std::endl; 

    // nijiカメラエリアのMayaバイナリにコピペできる型式で出力

    // Updateからコピペ
    gfl2::math::Vector4 targetPos  = m_pTarget->GetTransform()->GetTranslation();
    gfl2::math::Vector4 startToEndVec( m_SettingData.endPointData.referencePoint - m_SettingData.startPointData.referencePoint );
    gfl2::math::Vector4 startToTargetVec( targetPos - m_SettingData.startPointData.referencePoint );
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

    // クォータニオンからオイラー角を求める
    gfl2::math::Vector4 nowRot( 0.0f, 0.0f, 0.0f );
    GetRotate( nowRotate, &nowRot );

    // 度に変換
    ConvertRadianToDegree( &nowRot );

    // 距離を補間
    f32 nowDistance;
    nowDistance = m_SettingData.startPointData.distance + ( m_SettingData.endPointData.distance - m_SettingData.startPointData.distance ) * ratio;

    MAYA_PARAM mayaData;
    mayaData.distance = nowDistance;
    mayaData.fovY = rSetting.aov;
    mayaData.interpolationFrame = m_InterpolationTime;
    mayaData.offset.x = 0.0f;
    mayaData.offset.y = 0.0f;
    mayaData.offset.z = 0.0f;
    mayaData.rot.x = nowRot.x;
    mayaData.rot.y = nowRot.y;
    mayaData.rot.z = nowRot.z;
    OutputMayaData( mayaData );
  }


private:

  virtual void OutPutRationPointData( std::ofstream* pOfstream, const c8* pName, const CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA& rData ) const
  {

    *pOfstream << "    <" << pName << ">" << std::endl; 
    *pOfstream << "      <Rotate x=\"" <<  rData.rotate.x << "\" " << "y=\"" << rData.rotate.y << "\" " << "z=\"" << rData.rotate.z << "\" />" << std::endl;
    *pOfstream << "      <Distance>" << rData.distance << "</Distance>" << std::endl; 
    *pOfstream << "    </" << pName << ">" << std::endl; 
  }


  // 設定情報
  u32 m_DataID;
  CAMERA_DATA_RELATIVE_ROTATE_RATIO m_SettingData;

  // 最新状態
  gfl2::math::Vector4      m_NowTargetPos;
  gfl2::math::Vector4      m_NowCameraPos;
  SkeltonModel* m_pTarget;

  // 前回情報
  u32                     m_InterpolationCount;
  u32                     m_InterpolationTime;
  gfl2::math::Vector4      m_PrevTargetPos;
  gfl2::math::Vector4      m_PrevCameraPos;
  gfl2::math::Vector4      m_PrevTargetOffset;
  gfl2::math::Vector4      m_PrevCameraOffset;
};

class SkeltonCameraActionBase_Debug : public SkeltonCameraActionBase
{
public:

  SkeltonCameraActionBase_Debug( SkeltonModel* pTarget, const SkeltonCameraActionBase* pPrev = NULL ) 
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

    strcpy_s( m_Name, 512, "DebugMode" );
  }

  virtual ~SkeltonCameraActionBase_Debug( void )
  {
  }

  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };

    // 入力情報格納
    const gfl2::system::InputDevice::StickPos& pos
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
    const gfl2::system::InputDevice::StickPos& pos_r
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Right );
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );

    if( SkeltonDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
      m_TargetPos = m_pTarget->GetTransform()->GetTranslation();
      m_TargetPos.y += 40.0 * 10.0f / 3.0f;
    }

    // 更新処理
    if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
    {
      SkeltonDebugInfo::m_CameraTargetDraw = true;

      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // オフセット移動
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        SkeltonDebugInfo::m_CameraTargetOffset.x -= lx;
        SkeltonDebugInfo::m_CameraTargetOffset.z -= ly;
      }

      if(pad_u || pad_d )
      {
        float diff = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { SkeltonDebugInfo::m_CameraTargetOffset.y += diff; }
        if ( pad_d ) { SkeltonDebugInfo::m_CameraTargetOffset.y -= diff; }
      }
    }
    else
    {
      SkeltonDebugInfo::m_CameraTargetDraw = false;

      // カメラ回転
      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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
        float diff = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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

    gfl2::math::Vector4 targetOffset;

    targetOffset.x = SkeltonDebugInfo::m_CameraTargetOffset.x * cosYaw - SkeltonDebugInfo::m_CameraTargetOffset.z * sinYaw;
    targetOffset.y = SkeltonDebugInfo::m_CameraTargetOffset.y;
    targetOffset.z = SkeltonDebugInfo::m_CameraTargetOffset.x * sinYaw + SkeltonDebugInfo::m_CameraTargetOffset.z * cosYaw;

    m_NowTargetPos = m_TargetPos + targetOffset;

    gfl2::math::Vector4 vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;

    m_NowCameraPos = m_NowTargetPos + vecEye;
  }

  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix::GetIdentity();
    *pMtx = gfl2::math::Matrix::GetLookAt( m_NowCameraPos, m_NowTargetPos, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f )  );
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

  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const
  {
    gfl2::math::Vector4 nowRot( m_Rot );

    // 度に変換
    ConvertRadianToDegree( &nowRot );

    *pOfstream << "<CameraData MeshName=\"\"> Type =\"TYPE_RELATIVE_ROTATE\" >" << std::endl; 
    *pOfstream << "  <Rotate x=\"" <<  nowRot.x << "\" " << "y=\"" << nowRot.y << "\" " << "z=\"" << nowRot.z << "\" />>" << std::endl;
    *pOfstream << "  <Distance>" << m_Distance << "</Distance>" << std::endl; 
    *pOfstream << "</CameraData>" << std::endl; 

    // nijiカメラエリアのMayaバイナリにコピペできる型式で出力
    gfl2::math::Vector4 targetOffset( SkeltonDebugInfo::m_CameraTargetOffset );
    gfl2::math::Vector4 rot;
    float sinYaw = sinf( m_Rot.y * -1.0f ) * -1.0f;
    float cosYaw = cosf( m_Rot.y * -1.0f ) * -1.0f;
    targetOffset.x = SkeltonDebugInfo::m_CameraTargetOffset.x * cosYaw - SkeltonDebugInfo::m_CameraTargetOffset.z * sinYaw;
    targetOffset.y = SkeltonDebugInfo::m_CameraTargetOffset.y;
    targetOffset.z = SkeltonDebugInfo::m_CameraTargetOffset.x * sinYaw + SkeltonDebugInfo::m_CameraTargetOffset.z * cosYaw;

    MAYA_PARAM mayaData;
    mayaData.distance = m_Distance;
    mayaData.fovY = rSetting.aov;
    mayaData.interpolationFrame = 0;
    mayaData.offset.x = targetOffset.x;
    mayaData.offset.y = targetOffset.y;
    mayaData.offset.z = targetOffset.z;
    mayaData.rot.x = nowRot.x;
    mayaData.rot.y = nowRot.y;
    mayaData.rot.z = nowRot.z;
    OutputMayaData( mayaData );

  };


private:

  b32 m_InterpolationFlag;
  u32 m_DataID;

  // 最新状態
  gfl2::math::Vector4      m_NowTargetPos;
  gfl2::math::Vector4      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector4      m_Rot;
  

  gfl2::math::Vector4      m_TargetPos;
  SkeltonModel* m_pTarget;
};


class SkeltonCameraActionBase_Snap : public SkeltonCameraActionBase
{
public:

  enum SnapAction
  {
    SNAP_ACTION_MOVE = 0, // 通常移動
    SNAP_ACTION_UP,       // しゃがんだ状態から戻る
    SNAP_ACTION_DOWN,     // しゃがむ
  };

  SkeltonCameraActionBase_Snap( SkeltonModel* pTarget, SkeltonCamera::ProjectionSetting* pProjectionSetting, const SkeltonCameraActionBase* pPrev = NULL ) 
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

    m_Rot.y = pTarget->GetTransform()->GetAngle();
    strcpy_s( m_Name, 512, "Snap" );

    m_pProjectionSetting->aov = 35.0f;
  }

  virtual ~SkeltonCameraActionBase_Snap( void )
  {
  }

  gfl2::math::Vector4 GetEyeVector( float rot_y )
  {
    float sinYaw    = sinf(rot_y);
    float cosYaw    = cosf(rot_y);
    float sinPitch  = 0.0f; //sinf(rot_x);
    float cosPitch  = 1.0f; //cosf(rot_x);
    float sinRoll   = 0.0f; //sinf(rot_z);
    float cosRoll   = 1.0f; //cosf(rot_z);

    gfl2::math::Vector4 vec;
    //	vec = gfl2::math::Vector4(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
    vec = gfl2::math::Vector4(sinYaw * 1.0f, 0.0f, cosYaw * 1.0f);
    vec.Normalize3();
    return vec;
  }


  virtual void Update( void )
  {
    enum{
      PlayerPadNo = 0,
    };

    // 入力情報格納
    const gfl2::system::InputDevice::StickPos& pos
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Left );
    const gfl2::system::InputDevice::StickPos& pos_r
      = gfl2::system::InputDevice::Pad::GetStick( PlayerPadNo, gfl2::system::InputDevice::Stick::Right );
    bool pad_u = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Up );
    bool pad_d = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Down );
    bool pad_l = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Left );
    bool pad_r = gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::Right );

    gfl2::math::Vector4 eyePositon;
    gfl2::math::Vector4 eyeOffset( 0.0f, 130.0f, 25.0f );


    // 自機制御→目オフセットを回転
    gfl2::math::Matrix eyeOffsetRotMat( gfl2::math::Matrix::GetRotationY( m_pTarget->GetTransform()->GetAngle() ) );
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
            float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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
            float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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
          float diff = 0.5f * SkeltonDebugInfo::m_CameraSpeed;
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
    gfl2::math::Vector4 moveOffsetWork( m_MoveOffset );
    gfl2::math::Matrix eyeMoveOffsetRotMat( gfl2::math::Matrix::GetRotationY( m_pTarget->GetTransform()->GetAngle() ) );
    moveOffsetWork = eyeMoveOffsetRotMat * moveOffsetWork;

    // 目の位置を算出
    eyePositon += m_DownOffset;
    eyePositon += moveOffsetWork;

    // 視線ベクトルを作成
    gfl2::math::Vector4 eyeVector( 0.0f, 0.0f, 1.0f );
    gfl2::math::Matrix eyeRotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y, m_Rot.x ) );
    eyeVector = eyeRotMat * eyeVector;

    m_NowCameraPos = eyePositon;
    m_NowTargetPos = eyePositon + eyeVector;

    // @todo 自機の向きを調整、本来ならこれは上層システムが行うべき
    m_pTarget->GetTransform()->SetAngle( m_Rot.y );

#if 0
    if( SkeltonDebugInfo::m_DebugInfoCameraStopFlag == false )
    {
    }

    // 更新処理
    if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::R2 ) )
    {
      SkeltonDebugInfo::m_CameraTargetDraw = true;

      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          rate *= 2.0f;
        }

        // オフセット移動
        float lx = pos_r.x * rate;
        float ly = pos_r.y * rate;
        SkeltonDebugInfo::m_CameraTargetOffset.x -= lx;
        SkeltonDebugInfo::m_CameraTargetOffset.z -= ly;
      }

      if(pad_u || pad_d )
      {
        float diff = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
        if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::L1 ) )
        {
          diff *= 2.0f;
        }
        if ( pad_u ) { SkeltonDebugInfo::m_CameraTargetOffset.y += diff; }
        if ( pad_d ) { SkeltonDebugInfo::m_CameraTargetOffset.y -= diff; }
      }
    }
    else
    {
      SkeltonDebugInfo::m_CameraTargetDraw = false;

      // カメラ回転
      if(pos_r.x || pos_r.y)
      {
        float rate = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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
        float diff = 5.0f * SkeltonDebugInfo::m_CameraSpeed;
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

    gfl2::math::Vector4 targetOffset;

    targetOffset.x = SkeltonDebugInfo::m_CameraTargetOffset.x * cosYaw - SkeltonDebugInfo::m_CameraTargetOffset.z * sinYaw;
    targetOffset.y = SkeltonDebugInfo::m_CameraTargetOffset.y;
    targetOffset.z = SkeltonDebugInfo::m_CameraTargetOffset.x * sinYaw + SkeltonDebugInfo::m_CameraTargetOffset.z * cosYaw;

    m_NowTargetPos = m_TargetPos + targetOffset;

    gfl2::math::Vector4 vecEye( 0.0f, 0.0f, m_Distance );
    gfl2::math::Matrix rotMat( gfl2::math::Matrix::GetRotationZYX( m_Rot.z, m_Rot.y * -1.0f, m_Rot.x ) );
    vecEye = rotMat * vecEye;

    m_NowCameraPos = m_NowTargetPos + vecEye;
#endif
  }

  virtual void GetViewMatrix( gfl2::math::Matrix* pMtx ) const
  {
    *pMtx = gfl2::math::Matrix::GetIdentity();
    *pMtx = gfl2::math::Matrix::GetLookAt( m_NowCameraPos, m_NowTargetPos, gfl2::math::Vector4( 0.0f, 1.0f, 0.0f )  );
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

  virtual void OutPutData( std::ofstream* pOfstream, const SkeltonCamera::ProjectionSetting& rSetting ) const
  {
  }


private:

  b32 m_InterpolationFlag;
  u32 m_DataID;
  SnapAction              m_SnapAciton;
  b32                     m_IsDown;
  gfl2::math::Vector4      m_DownOffset;
  gfl2::math::Vector4      m_MoveOffset;
  f32                     m_ZoomOffset;

  SkeltonCamera::ProjectionSetting* m_pProjectionSetting;
  

  gfl2::math::Vector4      m_EyeVector;
  gfl2::math::Vector4      m_EyeOffset;

  // 最新状態
  gfl2::math::Vector4      m_NowTargetPos;
  gfl2::math::Vector4      m_NowCameraPos;

  f32                     m_Distance;
  gfl2::math::Vector4      m_Rot;
  

  gfl2::math::Vector4      m_TargetPos;
  SkeltonModel* m_pTarget;
};

/**
* @brief   コンストラクタ
*
* @return  無し 
*/
SkeltonCamera::SkeltonCamera( void )
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
SkeltonCamera::~SkeltonCamera( void )
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
void SkeltonCamera::Initialize( SkeltonModel* pTarget )
{
  m_pTarget = pTarget;
  m_pCamera = new SkeltonCameraActionBase_Debug( m_pTarget );
}

/**
* @brief   終了処理
*
* @return  無し 
*/
void SkeltonCamera::Terminate( void )
{
  GFL_SAFE_DELETE( m_pCamera );
}

/**
* @brief   更新処理
*
* @return  無し 
*/
void SkeltonCamera::Update( void )
{
  enum{
    PlayerPadNo = 0,
  };

#if 0
  // 更新処理
  if( gfl2::system::InputDevice::Pad::IsPress( PlayerPadNo, gfl2::system::InputDevice::Button::B ) )
  {
    SkeltonCameraActionRelativeRotate* pNext = new SkeltonCameraActionRelativeRotate( m_pTarget, m_pCamera );
    GFL_SAFE_DELETE( m_pCamera );
    m_pCamera = pNext;
  }
#endif 

  if( m_pCamera )
  {
    m_pCamera->Update();
  }

}

b32 SkeltonCamera::Change( const CAMERA_DATA* pCameraDara, b32 isForce )
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
      SkeltonCameraActionRelativeRotate* pNext = new SkeltonCameraActionRelativeRotate( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_RELATIVE_ROTATE*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo 
      strcpy_s( m_pCamera->m_Name, 512, pCameraDara->name );
      return true;
    }

  case TYPE_RELATIVE_ROTATE_RATIO:
    {
      SkeltonCameraActionRelativeRotateRatio* pNext = new SkeltonCameraActionRelativeRotateRatio( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_RELATIVE_ROTATE_RATIO*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo 
      strcpy_s( m_pCamera->m_Name, 512, pCameraDara->name );
      return true;
    }

  case TYPE_STOP:
    {
      SkeltonCameraActionStop* pNext = new SkeltonCameraActionStop( 
        pCameraDara->id, 
        static_cast<CAMERA_DATA_STOP*>( pCameraDara->pData ), 
        m_pTarget, 
        interpolationFrame,
        m_pCamera );
      GFL_SAFE_DELETE( m_pCamera );
      m_pCamera = pNext;

      // @todo 
      strcpy_s( m_pCamera->m_Name, 512, pCameraDara->name );
      return true;
    }


  }

  return false;

}

b32 SkeltonCamera::ChangeDebugCamera( void )
{
  SkeltonCameraActionBase_Debug* pNext = new SkeltonCameraActionBase_Debug( m_pTarget );
  GFL_SAFE_DELETE( m_pCamera );
  m_pCamera = pNext;
  return true;
}

b32 SkeltonCamera::ChangeSnapCamera( void )
{
  SkeltonCameraActionBase_Snap* pNext = new SkeltonCameraActionBase_Snap( m_pTarget, &m_ProjectionSetting );
  GFL_SAFE_DELETE( m_pCamera );
  m_pCamera = pNext;
  return true;
}

/**
* @brief   カメラターゲット座標を取得
*
* @param   pOut 格納先
*
* @return  無し 
*/
void SkeltonCamera::GetTargetPosition( gfl2::math::Vector4* pOut ) const
{
  if( m_pCamera )
  {
    SkeltonCameraActionBase::CAMERA_DATA cameraData;
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
void SkeltonCamera::GetCameraPosition( gfl2::math::Vector4* pOut ) const
{
  if( m_pCamera )
  {
    SkeltonCameraActionBase::CAMERA_DATA cameraData;
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

void SkeltonCamera::GetViewMatrix( gfl2::math::Matrix* pMtx ) const
{
  if( m_pCamera )
  {
    m_pCamera->GetViewMatrix( pMtx );
  }
  else
  {
  }
}

/**
* @brief   射影行列取得
*
* @param   pMtx 結果格納先
*
* @return  無し 
*/
void SkeltonCamera::GetProjectionMatrix( gfl2::math::Matrix* pMtx ) const
{
  *pMtx = gfl2::math::Matrix::GetPerspective( 
    gfl2::math::ConvDegToRad( m_ProjectionSetting.aov ), 
    m_ProjectionSetting.aspect, 
    m_ProjectionSetting.nearZ, 
    m_ProjectionSetting.farZ 
    );
}


/**
* @brief   射影行列関連パラメータを設定
*
* @param   rSetting  設定内容
*
* @return  無し 
*/
void SkeltonCamera::SetProjectionSetting( const ProjectionSetting& rSetting )
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
void SkeltonCamera::GetProjectionSetting( ProjectionSetting* pSetting ) const
{
  *pSetting = m_ProjectionSetting;
}

/**
* @brief   カメラ操作の入力を行っているか取得
*
* @return  行っているならtrue、行っていないならfalse
*/
b32 SkeltonCamera::IsManual( void ) const
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
void SkeltonCamera::SetDebug( b32 enable )
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
b32 SkeltonCamera::IsDebug( void ) const
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



void SkeltonCamera::OutPutData( std::ofstream* pOfstream ) const
{
  if( m_pCamera )
  {
    m_pCamera->OutPutData( pOfstream, m_ProjectionSetting );
  }
}

const c8* SkeltonCamera::GetDataName( void ) const
{
  if( m_pCamera )
  {
    return &m_pCamera->m_Name[0];
  }

  return NULL;
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
