/**
* @brief  カメラアニメーションを1セットだけ管理するクラス
* @file   FieldCameraAnimationLoader.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

// field
#include "Field/FieldRo/include/Camera/FieldCameraAnimationLoader.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraAnimationLoader::CameraAnimationLoader( void ) :
  m_State( State::NONE )
  ,m_ArcNo( -1 )
  ,m_ArcDataNo( -1 )
  ,m_ppResourceNodeTable( NULL )
  ,m_DataNum( 0 )
  ,m_pBinary( NULL )
  ,m_pFileManager( NULL )
  ,m_pHeap( NULL )
  ,m_pAllocator( NULL )
{

}


/**
* @brief デストラクタ
*
* @return 無し
*/
CameraAnimationLoader::~CameraAnimationLoader( void )
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
void CameraAnimationLoader::Initialize( const InitializeDescription& rInitDesc )
{

}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraAnimationLoader::Terminate( void )
{
  UnLoad();
}

/**
* @brief ロード処理
*
* @return 無し
*/
void CameraAnimationLoader::Load( const LoadDescription& rLoadDesc )
{
  if( m_State != State::NONE )
  {
    GFL_ASSERT( 0 ); // 多重ロード不可
    return;
  }

  m_ArcNo = rLoadDesc.m_ArcNo;
  m_ArcDataNo = rLoadDesc.m_ArcDataNo;
  m_pFileManager = rLoadDesc.m_pFileManager;
  m_pHeap = rLoadDesc.m_pHeap;
  m_pAllocator = rLoadDesc.m_pAllocator;

  // Garcを開く
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= m_ArcNo;
    req.heapForFile	= m_pHeap;
    req.heapForReq	= m_pHeap->GetLowerHandle();
    m_pFileManager->SyncArcFileOpen( req );
  }

  {
    // リクエスト生成
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = m_ArcNo;
    req.datId      = m_ArcDataNo;
    req.ppBuf      = &m_pBinary;
    req.heapForBuf = m_pHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pHeap->GetLowerHandle();
    req.heapForCompressed = NULL;

    m_pFileManager->AddArcFileLoadDataReq( req );
  }

  m_State = State::LOADING;

}

/**
* @brief ロード破棄処理
*
* @return 無し
*/
void CameraAnimationLoader::UnLoad( void )
{
  switch( m_State )
  {
  case State::LOADING:
    {
      GFL_ASSERT_STOP( 0 );
      break;
    } // case State::LOADING:

  case State::COMPLETE:
    {
      for( u32 i = 0; i < m_DataNum; i++ )
      {
        GFL_SAFE_DELETE( m_ppResourceNodeTable[i] );
      }
      GFL_SAFE_DELETE( m_ppResourceNodeTable );
      m_DataNum = 0;

      GflHeapSafeFreeMemory( m_pBinary );

      // Garcを閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
        req.arcId				= m_ArcNo;
        m_pFileManager->SyncArcFileClose( req );
      }

      m_ArcNo = -1;
      m_pFileManager = NULL;

      break;
    } //  case State::COMPLETE:
  } //  switch( m_State )

  m_State = State::NONE;
}

/**
* @brief 更新処理
*
* @return 無し
*/
void CameraAnimationLoader::Update( void )
{
  switch( m_State )
  {
  case State::LOADING:
    {
      if( m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pBinary) )
      {
        // リソース作成
        gfl2::fs::BinLinkerAccessor dataAccessor( m_pBinary );

        m_DataNum = dataAccessor.GetDataMax();
        if( m_DataNum != 0 )
        {
          m_ppResourceNodeTable = GFL_NEW_ARRAY( m_pHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_DataNum ];

          for( u32 i = 0; i < m_DataNum; i++ )
          {
            if( dataAccessor.GetDataSize( i ) != 0 ) 
            {
              gfl2::renderingengine::scenegraph::resource::GfBinaryMotData data;
              data.SetMotionData( static_cast<c8*>( dataAccessor.GetData( i ) ) );
              m_ppResourceNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &data );
            }
            else
            {
              m_ppResourceNodeTable[ i ] = NULL;

            }
          }
        }

        m_State = State::COMPLETE;
      }

      break;
    }
  } // case State::LOADING:
}

/**
* @brief ロード中か取得
*
* @return ロード中ならtrue、ロードしていない、ロード完了ならfalseを返す
*/
b32 CameraAnimationLoader::IsLoading( void ) const
{
  return ( m_State == State::LOADING );
}

/**
* @brief アニメーションデータ取得
*
* @param  dataIndex データ番号
*
* @return アニメーションデータ( NULL なら引数不正 )
*/
gfl2::renderingengine::scenegraph::resource::ResourceNode* CameraAnimationLoader::GetData( u32 dataIndex )
{
  if( dataIndex >= m_DataNum )
  {
    return NULL;
  }

  return m_ppResourceNodeTable[ dataIndex ];
}



GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
