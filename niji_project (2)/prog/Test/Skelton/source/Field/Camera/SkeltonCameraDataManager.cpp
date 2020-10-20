#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCameraDataManager.h
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

// #include <macro/include/gfl2_Macros.h>
// #include <heap/include/gfl2_heap_base.h>
// #include <heap/include/gfl2_HeapMacros.h>


#include "Test/Skelton/include/Field/Camera/SkeltonCameraDataManager.h"
#include "Test/Skelton/include/Field/Camera/SkeltonCameraUtility.h"

#include <xml/include/rapidxml.hpp> // no need to link
#include <Windows.h>

// ファイル操作系
#include <iostream>
#include <string>
#include <fstream>

#define SKELTON_DELETE delete
#define SKELTON_SAFE_DELETE( p )       { if( p ) { SKELTON_DELETE( p );		( p ) = 0; } }

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

static const u32 CAMERA_DATA_MAX(512);

#if 0
static const PROJECTION_DATA PROJECTION_DATA_DEFAULT_SETTING = 
{

};
  
  sc_ProjectionData;    // 射影情報
#endif

SkeltonCameraDataManager::SkeltonCameraDataManager( void ) : 
  m_pDefaultCameraData( NULL ),
  m_pCameraDataTable( NULL ),
  m_CameraDataUseSize( 0 )
{

}

SkeltonCameraDataManager::~SkeltonCameraDataManager( void )
{
  Terminate();
}

b32 GetVectorValue( const rapidxml::xml_node<>* pVecrotNode, gfl2::math::Vector4* pOut )
{
  pOut->x = static_cast<f32>( atof( pVecrotNode->first_attribute("x")->value() ) );
  pOut->y = static_cast<f32>( atof( pVecrotNode->first_attribute("y")->value() ) );
  pOut->z = static_cast<f32>( atof( pVecrotNode->first_attribute("z")->value() ) );
  return true;
}

void LoadPositionLimitData( const rapidxml::xml_node<>* pPositionLimitDataNode, LIMIT_DATA* pOut )
{
  if( pPositionLimitDataNode == NULL )
  {
    return;
  }

  // min
  const rapidxml::xml_node<>* pMinNode = pPositionLimitDataNode->first_node( "Min" );
  if( pMinNode )
  {
    const rapidxml::xml_attribute<>* pAttrWork = NULL;
    pAttrWork= pMinNode->first_attribute("x");
    if( pAttrWork )
    {
      pOut->enableMinX = 1;
      pOut->min.x = static_cast<f32>( atof( pAttrWork->value() ) );
    }
    pAttrWork= pMinNode->first_attribute("y");
    if( pAttrWork )
    {
      pOut->enableMinY = 1;
      pOut->min.y = static_cast<f32>( atof( pAttrWork->value() ) );
    }
    pAttrWork= pMinNode->first_attribute("z");
    if( pAttrWork )
    {
      pOut->enableMinZ = 1;
      pOut->min.z = static_cast<f32>( atof( pAttrWork->value() ) );
    }
  }

  // max
  const rapidxml::xml_node<>* pMaxNode = pPositionLimitDataNode->first_node( "Max" );
  if( pMaxNode )
  {
    const rapidxml::xml_attribute<>* pAttrWork = NULL;
    pAttrWork= pMaxNode->first_attribute("x");
    if( pAttrWork )
    {
      pOut->enableMaxX = 1;
      pOut->max.x = static_cast<f32>( atof( pAttrWork->value() ) );
    }
    pAttrWork= pMaxNode->first_attribute("y");
    if( pAttrWork )
    {
      pOut->enableMaxY = 1;
      pOut->max.y = static_cast<f32>( atof( pAttrWork->value() ) );
    }
    pAttrWork= pMaxNode->first_attribute("z");
    if( pAttrWork )
    {
      pOut->enableMaxZ = 1;
      pOut->max.z = static_cast<f32>( atof( pAttrWork->value() ) );
    }
  }
}

b32 LoadRelativeRotateRatioPointData( const rapidxml::xml_node<>* pDataNode, CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA* pOut )
{
  // 回転
  if( GetVectorValue( pDataNode->first_node("Rotate"), &pOut->rotate ) == false )
  {
    return false;
  }

  ConvertDegreeToRadian( &pOut->rotate );

  // 基準点
  if( GetVectorValue( pDataNode->first_node("ReferencePoint"), &pOut->referencePoint ) == false )
  {
    return false;
  }

  const rapidxml::xml_node<>* pDistanceNode = pDataNode->first_node( "Distance" );
  if( pDistanceNode == NULL )
  {
    return false;
  }

  pOut->distance = atof( pDistanceNode->value() );
  return true;
}


void* CreateCameraData( Type type, const rapidxml::xml_node<>* pCameraDataNode )
{
  switch( type )
  {
  case TYPE_RELATIVE_ROTATE:
    {
      CAMERA_DATA_RELATIVE_ROTATE* pData = new CAMERA_DATA_RELATIVE_ROTATE;

      InitializeLimitData( &pData->positionLimitData );
      InitializeLimitData( &pData->positionLimitDataEx );
      InitializeLimitData( &pData->rotateLimitData );
     
      pData->rotateLimitCoordinateSystemType = COORDINATE_SYSTEM_TYPE_LOCAL;

      if( GetVectorValue( pCameraDataNode->first_node("Rotate"), &pData->rotate ) == false )
      {
        delete pData;
        return NULL;
      }

      // ターゲットオフセット指定はないこともある
      pData->targetOffsetType = COORDINATE_SYSTEM_TYPE_LOCAL;
      pData->targetOffset = gfl2::math::Vector4( 0.0f, 0.0f, 0.0f );
      const rapidxml::xml_node<>* pTargetOffsetNode = pCameraDataNode->first_node("TargetOffset");

      if( pTargetOffsetNode )
      {
        b32 failedFlag( false );
        gfl2::math::Vector4 posWork;
        if( GetVectorValue( pTargetOffsetNode, &posWork ) == false )
        {
          failedFlag = true;
        }

        CoordinateSystemType type;
        const rapidxml::xml_attribute<>* pAttr = pTargetOffsetNode->first_attribute("Type");
        if( !failedFlag && pAttr && SkeltonCameraDataManager::GetCoordinateSystemType( pAttr->value(), &type ) == false )
        {
          failedFlag = true;
        }

        if( failedFlag == false )
        {
          pData->targetOffset = posWork;
          pData->targetOffsetType = type;
        }
      }


      ConvertDegreeToRadian( &pData->rotate );

      LoadPositionLimitData( pCameraDataNode->first_node("PositionLimitData"), &pData->positionLimitData );
      LoadPositionLimitData( pCameraDataNode->first_node("PositionLimitDataEx"), &pData->positionLimitDataEx );
      LoadPositionLimitData( pCameraDataNode->first_node("RotateLimitData"), &pData->rotateLimitData );

      ConvertDegreeToRadian( &pData->rotateLimitData.max );
      ConvertDegreeToRadian( &pData->rotateLimitData.min );

      // @todo rotateLimitCoordinateSystemType　読み込み
      pData->rotateLimitData.min += pData->rotate;
      pData->rotateLimitData.max += pData->rotate;

      pData->distance = static_cast<f32>( atof( pCameraDataNode->first_node("Distance")->value() ) );
      return static_cast<void*>( pData );
    }

  case TYPE_RELATIVE_ROTATE_RATIO:
    {
      CAMERA_DATA_RELATIVE_ROTATE_RATIO* pData = new CAMERA_DATA_RELATIVE_ROTATE_RATIO;
      b32 loadErrorFlag( false );
      if( LoadRelativeRotateRatioPointData( pCameraDataNode->first_node( "StartPointData" ), &pData->startPointData ) == false )
      {
        loadErrorFlag = true;
      }
      if( LoadRelativeRotateRatioPointData( pCameraDataNode->first_node( "EndPointData" ), &pData->endPointData ) == false )
      {
        loadErrorFlag = true;
      }

      if( loadErrorFlag )
      {
        delete pData;
        return NULL;
      }

      return static_cast<void*>( pData );
    }

  case TYPE_STOP:
    {
      CAMERA_DATA_STOP* pData = new CAMERA_DATA_STOP;
      return static_cast<void*>( pData );
    }
  }

  // @todo 無いものもあるんじゃ…
  return NULL;
}

void CreateInterpolationData( const rapidxml::xml_node<>* pInterpolationDataNode, INTERPOLATION_DATA* pOut )
{
  if( pInterpolationDataNode == NULL )
  {
    // 指定されていない事もある
    return;
  }
    
  // デフォルト値が指定されていたら上書き
  const rapidxml::xml_node<>* defaultInterpolationFrameNode = pInterpolationDataNode->first_node( "DefaultInterpolationFrame" );
  if( defaultInterpolationFrameNode )
  {
    pOut->defaultFrame = atoi( defaultInterpolationFrameNode->value() );
  }

  // 接続情報
  const rapidxml::xml_node<>* pConnectDataNode = pInterpolationDataNode->first_node( "ConnectData" );
  if( pConnectDataNode == NULL )
  {
    // 指定されていない事もある
    return;
  }

  u32 connectDataNum = 0;
  for( const rapidxml::xml_node<>* connectInterpolationFrameNode = pConnectDataNode->first_node(); connectInterpolationFrameNode; connectInterpolationFrameNode = connectInterpolationFrameNode->next_sibling() )
  {
    connectDataNum++;
  }

  if( connectDataNum == 0 )
  {
    return;
  }

  pOut->pUniqueInterpolationDataTable = new UNIQUE_INTERPOLATION_DATA[ connectDataNum ];
  
  pOut->uniqueInterpolationDataSize = 0;
  for( const rapidxml::xml_node<>* connectInterpolationFrameNode = pConnectDataNode->first_node(); connectInterpolationFrameNode; connectInterpolationFrameNode = connectInterpolationFrameNode->next_sibling() )
  {
    UNIQUE_INTERPOLATION_DATA* pData = &pOut->pUniqueInterpolationDataTable[ pOut->uniqueInterpolationDataSize ];

    std::string meshName = connectInterpolationFrameNode->first_attribute("MeshName")->value();
    strcpy_s( pData->name, 512, meshName.c_str() );
    pData->frame = atoi( connectInterpolationFrameNode->value() );

    pOut->uniqueInterpolationDataSize++;
  }
}

void DeleteInterpolationData( INTERPOLATION_DATA* pOut )
{
  SKELTON_SAFE_DELETE( pOut->pUniqueInterpolationDataTable );
  pOut->uniqueInterpolationDataSize = 0;
}

b32 LoadCameraData( const rapidxml::xml_node<>* pCameraDataNode, CAMERA_DATA* pCameraData, u32 defaultInterpolationFrame, f32 defaultAngleOfView )
{
  std::string meshName = pCameraDataNode->first_attribute("MeshName")->value();
  std::string typeName = pCameraDataNode->first_attribute("Type")->value();
  const rapidxml::xml_node<>* pPriorityNode = pCameraDataNode->first_node("Priority");


  Type type;
  if( SkeltonCameraDataManager::GetCameraType( typeName.c_str(), &type ) == false )
  {
    return false;
  }

  // ヘッダー部分
  // pData->id = m_CameraDataUseSize;
  pCameraData->type = type;
  strcpy_s( pCameraData->name, 512, meshName.c_str() );

  pCameraData->priority = 0;

  // 補間情報
  pCameraData->interpolationData.defaultFrame = defaultInterpolationFrame;
  pCameraData->interpolationData.pUniqueInterpolationDataTable = NULL;
  pCameraData->interpolationData.uniqueInterpolationDataSize = 0;

  CreateInterpolationData( pCameraDataNode->first_node("InterpolationData"), &pCameraData->interpolationData );


  // 射影設定
  pCameraData->projectionData.aov = defaultAngleOfView;
  pCameraData->projectionData.nearZ = 1.0f;
  pCameraData->projectionData.farZ = 100000.0f;

  const rapidxml::xml_node<>* pAngleOfViewNode = pCameraDataNode->first_node( "AngleOfView" );
  if( pAngleOfViewNode )
  {
    pCameraData->projectionData.aov = atoi( pAngleOfViewNode->value() );
  }

  // プライオリティ設定
  if( pPriorityNode )
  {
    pCameraData->priority = atoi( pPriorityNode->value() );
  }

  // タイプ別データ部分
  pCameraData->pData = CreateCameraData(
    pCameraData->type,
    pCameraDataNode
    );


  if( pCameraData->pData )
  {
    return true;
  }
  else
  {
    // エラー
    DeleteInterpolationData( &pCameraData->interpolationData );
    return false;
  }
}

void SettingDefaultCamera( CAMERA_DATA* pCameraData, u32 defaultInterpolationFrame, f32 defaultAngleOfView )
{
  // ヘッダー部分
  // pData->id = m_CameraDataUseSize;
  pCameraData->type = TYPE_RELATIVE_ROTATE;
  strcpy_s( &pCameraData->name[ 0 ], 512, "Default" );

  // 補間情報
  pCameraData->interpolationData.defaultFrame = defaultInterpolationFrame;
  pCameraData->interpolationData.pUniqueInterpolationDataTable = NULL;
  pCameraData->interpolationData.uniqueInterpolationDataSize = 0;

  // 射影設定
  pCameraData->projectionData.aov = defaultAngleOfView;
  pCameraData->projectionData.nearZ = 1.0f;
  pCameraData->projectionData.farZ = 100000.0f;
  pCameraData->priority = 0;

  // タイプ別データ部分
  CAMERA_DATA_RELATIVE_ROTATE* pData = new CAMERA_DATA_RELATIVE_ROTATE;

  InitializeLimitData( &pData->positionLimitData );
  InitializeLimitData( &pData->positionLimitDataEx );
  InitializeLimitData( &pData->rotateLimitData );
  pData->rotateLimitCoordinateSystemType = COORDINATE_SYSTEM_TYPE_LOCAL;

  pData->rotate = gfl2::math::Vector4( -34.637991, 0.0f, 0.0f );
  ConvertDegreeToRadian( &pData->rotate );
  pData->distance = 579;

  pData->targetOffsetType = COORDINATE_SYSTEM_TYPE_LOCAL;
  pData->targetOffset = gfl2::math::Vector4( 0.0f, 0.0f, 0.0f );

  pCameraData->pData = static_cast<void*>( pData );
};

/**
* @brief   初期化処理
*
* @return  無し 
*/
void SkeltonCameraDataManager::Initialize( void )
{
  m_pCameraDataTable = new CAMERA_DATA[ CAMERA_DATA_MAX ];
  m_pDefaultCameraData = new CAMERA_DATA;

  SettingDefaultCamera( m_pDefaultCameraData, 0, 46.5f );
}

/**
* @brief   終了処理
*
* @return  無し 
*/
void SkeltonCameraDataManager::Terminate( void )
{
  for( u32 i = 0; i < m_CameraDataUseSize; i++ )
  {
    CAMERA_DATA* pCamera = &m_pCameraDataTable[i];

    DeleteInterpolationData( &pCamera->interpolationData );
    SKELTON_SAFE_DELETE( pCamera->pData );
  }

  if( m_pDefaultCameraData )
  {
    DeleteInterpolationData( &m_pDefaultCameraData->interpolationData );
    SKELTON_SAFE_DELETE( m_pDefaultCameraData->pData );
  }
  SKELTON_SAFE_DELETE( m_pDefaultCameraData );
  

  SKELTON_SAFE_DELETE( m_pCameraDataTable );
  m_CameraDataUseSize = 0;
}

const CAMERA_DATA* SkeltonCameraDataManager::GetDefaultCameraData( void ) const
{
  return m_pDefaultCameraData; 
}

const CAMERA_DATA* SkeltonCameraDataManager::GetCameraData( const c8* pName ) const
{
  std::string name( pName );

  // gfmdlのメッシュ名の変更により修正
  // gfmdl version 2.1以前 <Mesh Name="|camera|camera_01|camera_01Shape" ～
  // ↓
  // gfmdl version 2.1以降 <Mesh Name="|camera|camera_01" ～
#if 0
  std::string::size_type t = name.find_last_of( "|" );
  if( t != std::string::npos )
  {
    name.erase( t );
    t = name.find_last_of( "|" );
    if( t != std::string::npos )
    {
      name.erase( 0, t + 1 );
    }
  }
#else
  // gfmdl version 2.1以前のモデル用の処理
  std::string::size_type t = name.rfind( "Shape" );
  if( t != std::string::npos)
  {
    t = name.find_last_of( "|" );
    if( t != std::string::npos )
    {
      name.erase( t );
    }
  }

  t = name.find_last_of( "|" );
  if( t != std::string::npos )
  {
    name.erase( 0, t + 1 );
  }
#endif


  for( u32 i = 0; i < m_CameraDataUseSize; i++ )
  {
    const CAMERA_DATA* pCamera = &m_pCameraDataTable[i];

    if( strcmp( name.c_str(), pCamera->name ) == 0 )
    {
      return pCamera;
    }
  }

  return NULL;
}

const CAMERA_DATA* SkeltonCameraDataManager::GetCameraDataByIndex( s32 dataIndex ) const
{
  if( dataIndex < 0 || dataIndex >= m_CameraDataUseSize )
  {
    return NULL;
  }

  return &m_pCameraDataTable[ dataIndex ];
}




b32 SkeltonCameraDataManager::LoadData( const c8* pFilePath )
{
  s32 size;
  c8* pFileData = ReadFile( pFilePath, &size );

  if( pFileData )
  {
    b32 result = UpdateImpl( pFileData );
    delete pFileData;
    return result;
  }
  else
  {
    // ファイルのロードに失敗した
    return false;
  }

}

b32 SkeltonCameraDataManager::LoadData( const c16* pFilePath )
{
  s32 size;
  c8* pFileData = ReadFile( pFilePath, &size );

  if( pFileData )
  {
    b32 result = UpdateImpl( pFileData );
    delete pFileData;
    return result;
  }
  else
  {
    // ファイルのロードに失敗した
    return false;
  }

}

c8* SkeltonCameraDataManager::ReadFile( const char* pFileName, s32* pSize, s32 align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, pFileName, "rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*pSize = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new c8[ buf_size ];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}

c8* SkeltonCameraDataManager::ReadFile( const c16* pFileName, s32* pSize, s32 align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	_wfopen_s( &hFile, pFileName, L"rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*pSize = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	u32 buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new c8[ buf_size ];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}


void SkeltonCameraDataManager::CreateXml( const c16* pFilePath )
{
  std::ofstream ofs( pFilePath );
  ofs << "<?xml version=\"1.0\" encoding=\"Shift-JIS\"?>" << std::endl;
  ofs << "<ROOT>" << std::endl;

  for( u32 i = 0; i < m_CameraDataUseSize; i++ )
  {
    CAMERA_DATA* pCamera = &m_pCameraDataTable[i];
    ofs << "  <" << pCamera->name << "/>" << std::endl;
  }

  ofs << "</ROOT>" << std::endl;
}


b32 SkeltonCameraDataManager::UpdateImpl( c8* pData )
{
  if( m_pCameraDataTable == NULL )
  {
    return false;
  }

  rapidxml::xml_document<> doc;

  try
  {
    doc.parse<0>( pData );
  }
  catch( rapidxml::parse_error &e )
  {
    MessageBoxA( NULL, "XMLの構文が間違っています", "エラー", MB_ICONERROR );
    return false;
  }

  const rapidxml::xml_node<>* pDefaultCameraDataNode = doc.first_node("DefauleCameraData");
  const rapidxml::xml_node<>* cameraDataListNode = doc.first_node("CameraDataList");
  const rapidxml::xml_node<>* defaultInterpolationFrameNode = doc.first_node( "DefaultInterpolationFrame" );
  const rapidxml::xml_node<>* pDefautlAngleOfViewNode = doc.first_node( "DefaultAngleOfView" );

  // 補間フレームデフォルト値
  u32 defaultInterpolationFrame = 0;
  if( defaultInterpolationFrameNode )
  {
    defaultInterpolationFrame = atoi( defaultInterpolationFrameNode->value() );
  }

  // 射影設定デフォルト値
  f32 defaultAngleOfView = 46.25f;
  if( pDefautlAngleOfViewNode )
  {
    defaultAngleOfView = atoi( pDefautlAngleOfViewNode->value() );
  }

  // デフォルトカメラデータ
  // 一旦中身を削除する
  DeleteCameraData( m_pDefaultCameraData );
  b32 defaultCameraLoadSuccess( false );
  if( pDefaultCameraDataNode )
  {
    m_pDefaultCameraData->id = CAMERA_ID_DEFAULT;
    if( LoadCameraData( pDefaultCameraDataNode, m_pDefaultCameraData, defaultInterpolationFrame, defaultAngleOfView ) )
    {
      defaultCameraLoadSuccess = true;
    }
  }
  if( defaultCameraLoadSuccess == false )
  {
    SettingDefaultCamera( m_pDefaultCameraData, defaultInterpolationFrame, defaultAngleOfView );
  }


  for( const rapidxml::xml_node<>* cameraDataNode = cameraDataListNode->first_node(); cameraDataNode; cameraDataNode = cameraDataNode->next_sibling() )
  {
    if( m_CameraDataUseSize >= CAMERA_DATA_MAX )
    {
      break;
    }

    m_pCameraDataTable[ m_CameraDataUseSize ].id = m_CameraDataUseSize;
    if( LoadCameraData( cameraDataNode, &m_pCameraDataTable[ m_CameraDataUseSize ], defaultInterpolationFrame, defaultAngleOfView ) )
    {
      m_CameraDataUseSize++;
    }
  }

  return true;
}

void SkeltonCameraDataManager::DeleteCameraData( CAMERA_DATA* pData ) const
{
  DeleteInterpolationData( &pData->interpolationData );
  SKELTON_SAFE_DELETE( pData->pData );
}

b32 SkeltonCameraDataManager::GetCameraType( const c8* pText, Type* pOut )
{
  struct HASH
  {
    const c8* const pText;
    Type            type;
  };

  const HASH hashTable[] = 
  {
    {
      "TYPE_RELATIVE_ROTATE",
      TYPE_RELATIVE_ROTATE
    },
    {
      "TYPE_RELATIVE_ROTATE_RATIO",
      TYPE_RELATIVE_ROTATE_RATIO
    },
    {
      "TYPE_STOP",
      TYPE_STOP
    }
  };

  for( u32 i = 0; i < GFL_NELEMS( hashTable ); i++ )
  {
    const HASH* pHash = &hashTable[ i ];

    if( strcmp( pHash->pText, pText ) == 0 )
    {
      *pOut = pHash->type;
      return true;
    }
  }

  return false;
}

b32 SkeltonCameraDataManager::GetCoordinateSystemType( const c8* pText, CoordinateSystemType* pOut )
{
  struct HASH
  {
    const c8* const pText;
    CoordinateSystemType   type;
  };

  const HASH hashTable[] = 
  {
    {
      "WORLD",
      COORDINATE_SYSTEM_TYPE_WORLD,
    },
    {
      "LOCAL",
      COORDINATE_SYSTEM_TYPE_LOCAL,
    },
  };

  for( u32 i = 0; i < GFL_NELEMS( hashTable ); i++ )
  {
    const HASH* pHash = &hashTable[ i ];

    if( strcmp( pHash->pText, pText ) == 0 )
    {
      *pOut = pHash->type;
      return true;
    }
  }

  return false;
}


GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32