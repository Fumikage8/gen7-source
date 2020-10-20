/**
* @brief  フィールドカメラ、レコード機能
* @file   FieldCameraRecorder.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "Field/FieldRo/include/Camera/FieldCameraRecorder.h"

#if defined(GF_PLATFORM_WIN32)
#include <fs/include/gfl2_FsPcUtil.h>
#include <fstream>
static const c8* const RECORD_FILE_ROOT("R:/home/momiji/exec/direct_sdmc/Field/Script/ScriptCameraToMaya"); ///< カメラ情報の出力先

#endif

static const u32 FRAME_RIMIT( 60 * 60 * 30 );    // 1時間越えたらとめる

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

//
// class CameraRecorder::Impl
//

#if defined(GF_PLATFORM_WIN32)
class CameraRecorder::Impl
{
public:

  Impl( gfl2::heap::HeapBase* pSystemHeap )
  {
    m_pSystemHeap = pSystemHeap;
  }

  ~Impl()
  {

  }

  void Start( const poke_3d::model::BaseCamera* pTarget, const c8* pFileName )
  {
    if( pTarget == NULL || pFileName == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    c8 pathWork[512];
    sprintf_s( pathWork, 512, "%s/%s.mel", RECORD_FILE_ROOT, pFileName );
    
    // ディレクトリ生成
    if( gfl2::fs::PcUtil::CreateDir( pathWork ).IsSuccess() == false )
    {
      return;
    }

    m_pOfs = GFL_NEW( m_pSystemHeap ) std::ofstream( pathWork );
    (*m_pOfs) << "// Setting" << std::endl;
    (*m_pOfs) << "int $currentTime = 0;" << std::endl;

    (*m_pOfs) << "// Create Camera" << std::endl;
    (*m_pOfs) << "string $cameraNameTable[] = `camera -name \"" << pFileName << "_camera" << "\"`;" << std::endl;
    (*m_pOfs) << "objectMoveCommand;" << std::endl;
    (*m_pOfs) << "cameraMakeNode 2 \"\";" << std::endl;
    (*m_pOfs) << "string $tranceNodeName = $cameraNameTable[0];" << std::endl;
    (*m_pOfs) << "string $shapeNodeName = $cameraNameTable[1];" << std::endl;
    (*m_pOfs) << "string $aimNodeName = $tranceNodeName + \"_aim\";" << std::endl;

    // FovYをmaya上の型式に変換
    f32 horizontalFilmAperture = 15.748f;
    f32 verticalFilmAperture = 9.449f;
    f32 fovY = pTarget->GetFovy();
    f32 focalLength = ( verticalFilmAperture * 0.5f ) / tan( fovY / 2 ) / 0.03937f;

    // アス比、画角設定など
    (*m_pOfs) << "// Camera Setting" << std::endl;
    (*m_pOfs) << "select $shapeNodeName;" << std::endl;
    (*m_pOfs) << "setAttr .horizontalFilmAperture 15.748;" << std::endl;
    (*m_pOfs) << "setAttr .verticalFilmAperture 9.449;" << std::endl;
    (*m_pOfs) << "setAttr .focalLength " << focalLength << ";" << std::endl;
    (*m_pOfs) << "setAttr .displayFilmGate 1;" << std::endl;

    (*m_pOfs) << std::endl;
  }

  void Update( const poke_3d::model::BaseCamera* pTarget, u32 currentFrame, f32 bank )
  {
    if( pTarget == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    // (*m_pOfs) << "// Update" << std::endl;
    (*m_pOfs) << "// " << currentFrame << std::endl;
    (*m_pOfs) << "currentTime $currentTime;" << std::endl;

    // カメラ座標
    (*m_pOfs) << "select $tranceNodeName;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetPosition().x << " -at translateX;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetPosition().y << " -at translateY;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetPosition().z << " -at translateZ;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << gfl2::math::ConvRadToDeg( bank ) << " -at rotateAxisZ;" << std::endl;

    // ターゲット座標
    (*m_pOfs) << "select $aimNodeName;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetTargetPosition().x << " -at translateX;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetTargetPosition().y << " -at translateY;" << std::endl;
    (*m_pOfs) << "setKeyframe -v " << pTarget->GetTargetPosition().z << " -at translateZ;" << std::endl;
    (*m_pOfs) << "$currentTime += 1;" << std::endl;

  }

  void End( const poke_3d::model::BaseCamera* pTarget )
  {
    (*m_pOfs) << "// End" << std::endl;
    GFL_SAFE_DELETE( m_pOfs );
  }

private:
  gfl2::heap::HeapBase*       m_pSystemHeap;

  std::ofstream* m_pOfs;
};
#else

// CTRは今の所不要なので空実装
class CameraRecorder::Impl
{
public:

  Impl( gfl2::heap::HeapBase* pSystemHeap )
  {
    m_pSystemHeap = pSystemHeap;
  }

  ~Impl()
  {

  }

  void Start( const poke_3d::model::BaseCamera* pTarget, const c8* pFileName )
  {
  }

  void Update( const poke_3d::model::BaseCamera* pTarget, u32 currentFrame, f32 bank )
  {
  }

  void End( const poke_3d::model::BaseCamera* pTarget )
  {
  }

private:
  gfl2::heap::HeapBase*       m_pSystemHeap;
};

#endif

//
// class CameraRecorder
//

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraRecorder::CameraRecorder( void ) :
  // 制御
  m_CurrentFrame( 0 )
  ,m_IsRecording( false )
  ,m_RecodingType( RecType::FOR_MAYA )

  // 内部オブジェクト
  ,m_pImpl( NULL )

  // 外部オブジェクト
  ,m_pTargetCamera( NULL )
  ,m_pSystemHeap( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
CameraRecorder::~CameraRecorder( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void CameraRecorder::Initialize( const InitializeDescription& rInitDesc )
{
  m_pSystemHeap = rInitDesc.m_pSystemHeap;
  m_pImpl = GFL_NEW( m_pSystemHeap ) CameraRecorder::Impl( m_pSystemHeap );
}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraRecorder::Terminate( void )
{
  GFL_SAFE_DELETE( m_pImpl );
  m_pSystemHeap = NULL;
  m_pTargetCamera = NULL;
  m_IsRecording = false;
}


/**
* @brief 記録対象を設定
*
* @param pTarget 記録対象
*
* @return 無し
*/
void CameraRecorder::SetTarget( const poke_3d::model::BaseCamera* pTarget )
{
  m_pTargetCamera = pTarget;
}


void CameraRecorder::ClearTarget( void )
{
  m_pTargetCamera = NULL;
}


/**
* @brief 更新処理
*
* @return 無し
*/
void CameraRecorder::Update( f32 bank )
{
  if( m_IsRecording == false )
  {
    return;
  }

  m_pImpl->Update( m_pTargetCamera, m_CurrentFrame, bank );
  m_CurrentFrame++;

  if( m_CurrentFrame >= FRAME_RIMIT )
  {
    End();
  }
}

/**
* @brief 記録開始
*
* @param  type        記録型式
* @param  pFileName   保存ファイル名(拡張子含まず)
*
* @return 無し
*/
void CameraRecorder::Start( RecType::Value type, const c8* pFileName )
{
  if( m_IsRecording )
  {
    End();
  }

  m_CurrentFrame = 0;
  m_IsRecording = true;
  m_RecodingType = type;
  m_pImpl->Start( m_pTargetCamera, pFileName );
}

/**
* @brief 記録停止
*
* @return 無し
*/
void CameraRecorder::End( void )
{
  if( m_IsRecording == false )
  {
    return;
  }

  m_IsRecording = false;
  m_pImpl->End( m_pTargetCamera );
}

/**
* @brief 記録中か取得
*
* @return true 記録中、 false 記録していない
*/
b32 CameraRecorder::IsRecording( void ) const
{
  return m_IsRecording;
}

GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );