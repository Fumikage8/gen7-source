/**
* @brief  フィールドカメラの操作、カメラエリア用
* @file   FieldCameraControllerTypeArea.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSystem/include/GameManager.h"

// camera
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaDataManager.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

GFL_NAMESPACE_BEGIN( Area );
class MeshHitData
{
public:

  MeshHitData( void ) : 
    intersection( 0.0f, 0.0f, 0.0f )
    ,pTriangle( NULL )
    ,priority( 0 )
  {

  }

  virtual ~MeshHitData( void )
  {
  }

  /**
  * @brief エリア番号を取得する
  *
  * @return エリア番号
  * @retval Area::AreaNo::INVALID 無効値
  */
  s32 GetAreaNo( void ) const
  {
    if( pTriangle->m_GroundAttribute == -1 )
    {
      return Area::AreaNo::INVALID;
    }
    else
    {
      return pTriangle->m_GroundAttribute;
    }
  }

  /**
  * @brief 各頂点の頂点カラーから、交点の値を取得する
  *
  * @return 値[0.0..1.0]
  */
  f32 GetRatio( void ) const
  {
    // @todo 
    gfl2::math::Vector3 positions[3];
    for( u32 i = 0; i < 3; i++ )
    {
      positions[i].x = pTriangle->m_Positions[i].x;
      positions[i].y = pTriangle->m_Positions[i].y;
      positions[i].z = pTriangle->m_Positions[i].z;
    }

    // 必要となるベクトル
    gfl2::math::Vector3 vec0to2( positions[2] - positions[0] );
    gfl2::math::Vector3 vec0to1( positions[1] - positions[0] );
    gfl2::math::Vector3 vec1to2( positions[2] - positions[1] );
    gfl2::math::Vector3 vec0toIntersection( intersection - positions[0] );
    gfl2::math::Vector3 vec1toIntersection( intersection - positions[1] );

    // 面積を計算
    f32 sizeFor01 = ( vec0to1.Cross( vec0toIntersection ) ).Length() * 0.5f;
    f32 sizeFor12 = ( vec1to2.Cross( vec1toIntersection ) ).Length() * 0.5f;
    f32 sizeFor02 = ( vec0to2.Cross( vec0toIntersection ) ).Length() * 0.5f;


    // 面積の比率から、交点のレートを計算
    f32 ratio = ( 
      pTriangle->m_vertexColorG[ 0 ] * sizeFor12 + 
      pTriangle->m_vertexColorG[ 1 ] * sizeFor02 + 
      pTriangle->m_vertexColorG[ 2 ] * sizeFor01 
      ) / ( sizeFor12 + sizeFor02 + sizeFor01 ) / 255;

    return gfl2::math::Clamp( ratio, 0.0f, 1.0f );
  }

  void Copy( const MeshHitData& rSrc )
  {
    intersection = rSrc.intersection;
    pTriangle = rSrc.pTriangle;
    priority = rSrc.priority;
  }

  gfl2::math::Vector3 intersection;
  const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  s32  priority;

};
GFL_NAMESPACE_END( Area );


GFL_NAMESPACE_BEGIN( Controller );

static const f32 X_LIMIT( gfl2::math::ConvDegToRad( 85.0f ) );

// カメラエリアコリジョンモデル用、レイキャストコールバック
class RaycastCustomCallback : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  /**
  * @brief コンストラクタ
  */
  RaycastCustomCallback( const Area::AreaDataManager* pAreaDataManager ) : m_IsHit( false ), m_pAreaDataManager( pAreaDataManager )
  {
  }

  /**
  * @brief デストラクタ
  */
  virtual ~RaycastCustomCallback()
  {

  }

  /**
  * @brief 当たり判定コールバック関数
  */
  virtual bool CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection)
  {
    s32 attribute = pTriangle->m_GroundAttribute;

    // 既にヒットしているならプライオリティをチェック
    s32 priority = m_pAreaDataManager->GetPriority( attribute ); // @todo 無かったとき対応
    if( m_IsHit )
    {
      // 高い方を優先
      if( priority < m_HitData.priority )
      {
        return true;
      }

    }
    else
    {
      m_IsHit = true;
    }

    // 更新
    m_HitData.intersection.x = intersection.x;
    m_HitData.intersection.y = intersection.y;
    m_HitData.intersection.z = intersection.z;
    m_HitData.priority = priority;
    m_HitData.pTriangle = pTriangle;


    // 継続　@note プライオリティチェックをするので打ち切れない
    return true;
  }

  /**
  * @brief レイの一番近い交点を取得する
  * @return 衝突データ
  */
  b32 GetHitData( Area::MeshHitData* pOut ) const
  {
    if( IsHit() == false )
    {
      return false;
    }

    pOut->Copy( m_HitData );
    return true;
  }

  /**
  * @brief 1度でも当たったかを返す
  * @retval true 当たった
  * @retval false 当たっていない
  */
  bool IsHit(void) const
  {
    return m_IsHit;
  }

private:

  b32                  m_IsHit;
  Area::MeshHitData    m_HitData;
  
  // 外部オブジェクト
  const Area::AreaDataManager* m_pAreaDataManager;

};



static f32 g_t(0.0f);

/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
ControllerTypeArea::ControllerTypeArea( const InitializeDescription& rInitDesc ) : 
  // 内部制御
  m_InterpolationNowFrame( 0 )
  ,m_NowAreaDataId( Area::AreaNo::INVALID )
  ,m_AreaDataType( Area::DataType::INVALID )
  ,m_PrevAreaCheckPosition( -9999.0f, -9999.0f, -9999.0f )
  ,m_PrevTargetPosition( -9999.0f, -9999.0f, -9999.0f )
  ,m_pData( NULL )
  ,m_pPrevData( NULL )
  ,m_pMeshHitData( NULL )
  ,m_InterpolationCameraPosition( 0.0f, 0.0f, 0.0f )
  ,m_InterpolationTargetPosition( 0.0f, 0.0f, 0.0f )
  ,m_CameraPosition( 0.0f, 0.0f, 0.0f )
  ,m_TargetPosition( 0.0f, 0.0f, 0.0f )

  ,m_PrevCameraTargetPosition( 0.0f, 0.0f, 0.0f )
  ,m_PrevCameraPosition( 0.0f, 0.0f, 0.0f )

#if PM_DEBUG
  ,m_DebugRotation( 0.0f, 0.0f, 0.0f )
  ,m_DebugDistance( 0.0f )
#endif //#if PM_DEBUG


  // 外部オブジェクト
  ,m_pBaseCamera( NULL )
  ,m_pAreaDataManager( rInitDesc.m_pAreaDataManager )
  ,m_pTarget( rInitDesc.m_pTarget )
  ,m_pCameraAreaClollisionModel( rInitDesc.m_pCameraAreaClollisionModel )
{
  m_pData = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::DATA;
  m_pPrevData = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::DATA;

  m_pMeshHitData = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::MeshHitData;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeArea::~ControllerTypeArea( void )
{
  GFL_SAFE_DELETE( m_pMeshHitData );
  GFL_SAFE_DELETE( m_pPrevData );
  GFL_SAFE_DELETE( m_pData );
}


/**
* @brief 初期化処理
* @note  ControllerがCameraUnitの設定された直後に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  m_pBaseCamera = pCamera;
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::Update( poke_3d::model::BaseCamera* pCamera )
{
  GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);


  gfl2::math::Vector3 targetPosition;

  //
  // 最新のエリアデータに更新する処理
  // 
  {
    m_pTarget->GetCameraTargetPosition( &targetPosition );
    s32 newAreaDataId = Area::AreaNo::INVALID;

    // コリジョンによるエリアデータ更新、座標が更新されたときのみ行う
    {
      if( targetPosition != m_PrevAreaCheckPosition )
      {
#if 0
        // コリジョン @todo 
        {
          // 無い事もある
          if( m_pCameraAreaClollisionModel )
          {
            RaycastCustomCallback callback( m_pAreaDataManager );
            gfl2::math::Vector4 posStart; // @todo
            gfl2::math::Vector4 posEnd; // @todo
            posStart.x = targetPosition.x;
            posStart.y = targetPosition.y;
            posStart.z = targetPosition.z;
            posEnd = posStart;

            posStart.y += 150.0f; // @todo
            posEnd.y -= 150.0f;   // @todo

            m_pCameraAreaClollisionModel->GetIntersection( posStart, posEnd, NULL, &callback );

            if( callback.IsHit() )
            {
              Area::MeshHitData hitData;
              callback.GetHitData( &hitData );

              if( hitData.GetAreaNo() != Area::AreaNo::INVALID  )
              {
                newAreaDataId = hitData.GetAreaNo();
                m_pMeshHitData->Copy( hitData ); // @todo 適切？
              }
            }
          }

          // @todo サークルver
          {


          }
        }
#endif //@saito_del
        // HITしなかったらDEFAULTに切り替える
        if( newAreaDataId == Area::AreaNo::INVALID )
        {
          newAreaDataId = Area::AreaNo::DEFAULT;
        }
      }

      // 記憶
      m_PrevAreaCheckPosition = targetPosition;

    } // コリジョンによるエリアデータ更新、座標が更新されたときのみ行う


#if defined DEBUG_ONLY_FOR_ikuta_junya
    {
      // 確認用コード
      if( pButton->IsHold( gfl2::ui::BUTTON_START ) )
      {
        if( pButton->IsHold( gfl2::ui::BUTTON_A ))
        {
          newAreaDataId = 1;
        }
        if( pButton->IsHold( gfl2::ui::BUTTON_B ))
        {
          newAreaDataId = 2;
        }
      }
    }
#endif // #if defined DEBUG_ONLY_FOR_ikuta_junya

    // 現在INVALIDで今回もINVALIDならデフォルトに切り替える
    if( newAreaDataId == Area::AreaNo::INVALID && m_NowAreaDataId == Area::AreaNo::INVALID )
    {
      newAreaDataId = Area::AreaNo::DEFAULT;
    }

    // エリア更新
    if( newAreaDataId != Area::AreaNo::INVALID && m_NowAreaDataId != newAreaDataId )
    {
      if( newAreaDataId == Area::AreaNo::DEFAULT )
      {
        m_pAreaDataManager->GetDefaultData( m_pData );
      }
      else
      {
        m_pAreaDataManager->GetData( m_pData, newAreaDataId, m_NowAreaDataId );
      }

      *m_pPrevData = *m_pData;

      // 始めてなら補間できない
      if( m_NowAreaDataId == Area::AreaNo::INVALID )
      {
        m_pData->interpolationFrame = 0;
      }
      else
      {
        m_InterpolationTargetPosition = m_TargetPosition;
        m_InterpolationCameraPosition = m_CameraPosition;
      }

      g_t = 0.0f;
      m_InterpolationNowFrame = 0;
      m_NowAreaDataId = newAreaDataId;

#if PM_DEBUG
      // エリア切り替えに合わせてデバッグ操作をリセット
      this->ClearDebugEffect();
#endif
    }

  }  // 最新のエリアデータに更新する処理

  //
  // エリアデータに合わせたカメラ更新処理
  //
  if( m_pData )
  {
    switch( m_pData->dataType )
    {
    case Area::DataType::FOLLOW:
      {
        UpdateTypeFull( pCamera );
        break;
      }

    case Area::DataType::FOLLOW_TRANS:
      {
        UpdateTypePointInterpolation( pCamera );
        break;
      }
    }
  }
}

/**
* @brief 更新処理( DataType::FULL )
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::UpdateTypeFull( poke_3d::model::BaseCamera* pCamera )
{
  if( m_pData )
  {
    gfl2::math::Vector3 targetPosition;
    m_pTarget->GetCameraTargetPosition( &targetPosition );

    // 最終位置を計算
    {
      f32 distance = m_pData->pointA.distance;
      gfl2::math::Vector3 rotation = m_pData->pointA.rotation;

#if PM_DEBUG
      // デバッグ操作
      {
        GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
        gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
        gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

        gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
        gfl2::math::Vector3 pos_r( 0.0f, 0.0f, 0.0f );
        bool pad_u = false;
        bool pad_d = false;
        bool pad_l = false;
        bool pad_r = false;

        pos_r.x = pVecDevice->GetX();
        pos_r.y = pVecDevice->GetY();
        pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
        pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
        pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
        pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );

        // L
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          // 回転
          if( pos_r.x || pos_r.y)
          {
            // 回転速度計算
            float rate = 0.05f; // * field::FieldDebugInfo::m_CameraSpeed;

            // 回転実行
            gfl2::math::Vector3 rot( pos_r.y * rate * -1.0f, pos_r.x * rate * -1.0f, 0.0f );
            m_DebugRotation += rot;
          }
          // 距離
          if( pad_u || pad_d )
          {
            float diff = 5.0f;// * field::FieldDebugInfo::m_CameraSpeed;
            if ( pad_u ) { m_DebugDistance -= diff; }
            if ( pad_d ) { m_DebugDistance += diff; }
          }
          // リセット
          if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
          {
            this->ClearDebugEffect();
          }
        }

        // デバッグ操作の影響を反映
        distance += m_DebugDistance;
        rotation += m_DebugRotation;
      }
#endif // #if PM_DEBUG

      // 距離制限
      distance = gfl2::math::Clamp( distance, 0.01f, 100000.0f );
      
      // X軸の回転には制限をつける
      rotation.x = gfl2::math::Clamp( rotation.x, X_LIMIT * -1.0f, X_LIMIT );


      gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
      gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( rotation.z, rotation.y, rotation.x ) );
      cameraOffset = rotMat.TransformCoord( cameraOffset );

      m_TargetPosition = targetPosition + m_pData->pointA.offset;
      m_CameraPosition = m_TargetPosition + cameraOffset;
    }

    // 補間
    if( m_pData->interpolationFrame > 0 && m_InterpolationNowFrame < m_pData->interpolationFrame )
    {
      if( m_InterpolationNowFrame == 0 )
      {
        m_InterpolationTargetPosition = m_PrevCameraTargetPosition - m_PrevTargetPosition;
        m_InterpolationCameraPosition = m_PrevCameraPosition - m_PrevTargetPosition;
      }

      gfl2::math::Vector3 workTarget( m_TargetPosition );
      gfl2::math::Vector3 workCamera( m_CameraPosition );

      m_CameraPosition = targetPosition + m_InterpolationCameraPosition;
      m_TargetPosition = targetPosition + m_InterpolationTargetPosition;

      f32 t;
      m_InterpolationNowFrame++;
      t = ( static_cast<f32>( m_InterpolationNowFrame ) / m_pData->interpolationFrame );

      m_CameraPosition.Lerp( workCamera, t );
      m_TargetPosition.Lerp( workTarget, t );
    }

    // カメラ実体に設定
    pCamera->SetupCameraLookAt(
      m_CameraPosition,
      m_TargetPosition,
      gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
      );


    m_PrevTargetPosition = targetPosition;
    m_PrevCameraTargetPosition = m_TargetPosition;
    m_PrevCameraPosition = m_CameraPosition;

  }
}

/**
* @brief 更新処理( DataType::POINT_INTERPOLATION )
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::UpdateTypePointInterpolation( poke_3d::model::BaseCamera* pCamera )
{
  if( m_pData )
  {
    gfl2::math::Vector3 targetPosition;
    m_pTarget->GetCameraTargetPosition( &targetPosition );

    // 最終位置を計算
    {
      g_t += 0.01f;
      if( g_t > 1.0f )
      {
        g_t = 1.0f;
      }

      f32 t;
      t = m_pMeshHitData->GetRatio();
      // t = g_t;
      t = gfl2::math::Easing::GetValue( m_pData->easeFunc, t );


      Area::POINT_DATA data;
      m_pAreaDataManager->Lerp( &data, m_pData->pointA, m_pData->pointB, t );

      gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, data.distance );
      gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( data.rotation.z, data.rotation.y, data.rotation.x ) );
      cameraOffset = rotMat.TransformCoord( cameraOffset );

      m_TargetPosition = targetPosition + data.offset;
      m_CameraPosition = m_TargetPosition + cameraOffset;
    }

    // 補間
    if( m_pData->interpolationFrame > 0 && m_InterpolationNowFrame < m_pData->interpolationFrame )
    {
      if( m_InterpolationNowFrame == 0 )
      {
        m_InterpolationTargetPosition = m_PrevCameraTargetPosition - m_PrevTargetPosition;
        m_InterpolationCameraPosition = m_PrevCameraPosition - m_PrevTargetPosition;
      }

      gfl2::math::Vector3 workTarget( m_TargetPosition );
      gfl2::math::Vector3 workCamera( m_CameraPosition );

      m_CameraPosition = targetPosition + m_InterpolationCameraPosition;
      m_TargetPosition = targetPosition + m_InterpolationTargetPosition;

      f32 t;
      m_InterpolationNowFrame++;
      t = ( static_cast<f32>( m_InterpolationNowFrame ) / m_pData->interpolationFrame );

      m_CameraPosition.Lerp( workCamera, t );
      m_TargetPosition.Lerp( workTarget, t );
    }

    // カメラ実体に設定
    pCamera->SetupCameraLookAt(
      m_CameraPosition,
      m_TargetPosition,
      gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
      );


    m_PrevTargetPosition = targetPosition;
    m_PrevCameraTargetPosition = m_TargetPosition;
    m_PrevCameraPosition = m_CameraPosition;

  }
}

#if PM_DEBUG
/**
* @brief デバッグ操作による影響を全てクリア
*
* @return 無し
*/
void ControllerTypeArea::ClearDebugEffect( void )
{
  m_DebugRotation = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
  m_DebugDistance = 0.0f;
}
#endif //#if PM_DEBUG

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );