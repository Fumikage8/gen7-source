/**
* @brief  フィールドカメラのシステム常駐アニメのインスタンスを管理するクラス
* @file   FieldCameraSystemAnimationResource.cpp
* @author ikuta_junya
* @data   2015.10.01
*/

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>


// field
#include "Field/FieldRo/include/Camera/FieldCameraSystemAnimationResource.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraSystemAnimationResource::CameraSystemAnimationResource( void ) :
  m_ppResourceNodeTable( NULL )
  ,m_ResourceNodeNum( 0 )
{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
CameraSystemAnimationResource::~CameraSystemAnimationResource( void )
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
void CameraSystemAnimationResource::Initialize( const InitializeDescription& rInitDesc )
{
  gfl2::fs::BinLinkerAccessor dataAccessor( rInitDesc.m_pData );

  m_ResourceNodeNum = dataAccessor.GetDataMax();
  m_ppResourceNodeTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [m_ResourceNodeNum];
  
  for( u32 i = 0; i < m_ResourceNodeNum; i++ )
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
    data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
    m_ppResourceNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( rInitDesc.m_pDeviceAllocator, &data );
  }
}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraSystemAnimationResource::Terminate( void )
{
  for( u32 i = 0; i < m_ResourceNodeNum; i++ )
  {
    GFL_SAFE_DELETE( m_ppResourceNodeTable[i] );
  }

  GFL_SAFE_DELETE_ARRAY( m_ppResourceNodeTable );
  m_ResourceNodeNum = 0;
}

/**
* @brief リソース取得
*
* @param  index データ番号
*
* @return リソース(　不正な場合はNULL )
*/
gfl2::renderingengine::scenegraph::resource::ResourceNode* CameraSystemAnimationResource::GetResource( u32 index )
{
  if( index >= m_ResourceNodeNum )
  {
    return NULL;
  }

  return m_ppResourceNodeTable[ index ];
}


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );