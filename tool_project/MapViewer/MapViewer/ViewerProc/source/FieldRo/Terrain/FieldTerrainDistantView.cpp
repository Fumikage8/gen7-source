/**
* @brief  フィールドの遠景1つ分クラス
* @file   TerrainDistantView.h
* @author ikuta_junya
* @data   2015.04.19
*/

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainDistantView.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainDistantView::TerrainDistantView( void ) :
  // 制御
  m_DistantViewNo( 0 )
  // オブジェクト
  ,m_pModel( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainDistantView::~TerrainDistantView( void )
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
void TerrainDistantView::Initialize( const InitializeDescription& rInitDesc )
{
  m_DistantViewNo = rInitDesc.m_DistantViewNo;
  m_pModel = rInitDesc.m_pTerrainFactory->CreateDistantViewModel( m_DistantViewNo );
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainDistantView::Terminate( void )
{
  GFL_SAFE_DELETE( m_pModel );
  m_DistantViewNo = 0;
}

/**
* @brief 更新処理
*
* @return 無し
*/
void TerrainDistantView::Update( void )
{
}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );