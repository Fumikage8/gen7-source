//======================================================================
/**
 * @file    FinderStudioDecoObject.cpp
 * @date    2017/02/06 14:52:53
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：デコオブジェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioDecoObject.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコオブジェクトの生成：上画面
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   デコオブジェクトの生成：上画面
 *
 * @param   heap    ヒープ
 * @param   lytRes  レイアウトリソース
 * @param   texID   テクスチャID
 * @param   pos     配置座標
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID, gfl2::math::VEC3 pos )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( texID, 0 );
  return CreateDecoObject_Upper( heap, texInfo, pos );
}

/**
 * @brief   デコオブジェクトの生成：上画面
 *
 * @param   heap      ヒープ
 * @param   texInfo   テクスチャ情報
 * @param   pos       配置座標
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::math::VEC3 pos )
{
  gfl2::lyt::LytPicture* picture = CreateDecoObject( heap, texInfo );

  // スケールを設定
  gfl2::math::VEC2 scale( 1.0f, 1.0f );
  picture->SetScale( scale );

  // 位置をレイアウト座標系へ変換
  gfl2::math::VEC3 vec = Static::ConvertUpperVectorScreenToLayout( pos );

  // 位置を設定
  picture->SetTranslate( vec );

  return picture;
}

/**
 * @brief   デコオブジェクトの生成：上画面
 *
 * @param   heap        ヒープ
 * @param   lytRes      レイアウトリソース
 * @param   stampData   スタンプデータ
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_Upper( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, Static::StampData stampData )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( stampData.id, 0 );
  gfl2::lyt::LytPicture* picture = CreateDecoObject( heap, texInfo );

  // スケールを設定
  gfl2::math::VEC2 scale( 1.0f, 1.0f );
  picture->SetScale( scale );

  gfl2::math::VEC3 pos( 0, 0, 0 );
  pos.x = static_cast<f32>( stampData.x );
  pos.y = static_cast<f32>( stampData.y );

  // 位置をレイアウト座標系へ変換
  gfl2::math::VEC3 vec = Static::ConvertUpperVectorScreenToLayout( pos );

  // 位置を設定
  picture->SetTranslate( vec );

  return picture;
}

/**
 * @brief   デコオブジェクトの生成：上画面フレーム
 *
 * @param   heap      ヒープ
 * @param   lytRes    レイアウトリソース
 * @param   texID     テクスチャID
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( texID, 0 );
  return CreateDecoObject_UpperFrame( heap, texInfo );
}

/**
 * @brief   デコオブジェクトの生成：上画面フレーム
 *
 * @param   heap      ヒープ
 * @param   texInfo   テクスチャ情報
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo )
{
  gfl2::lyt::LytPicture* picture = CreateDecoObject( heap, texInfo );

  // スケールを設定
  gfl2::math::VEC2 scale( 1.0f, 1.0f );
  picture->SetScale( scale );

  // 位置を設定
  gfl2::math::VEC3 pos(0, 0, 0);
  picture->SetTranslate( pos );

  return picture;
}

/**
 * @brief   デコオブジェクトの生成：上画面フレーム
 *
 * @param   heap      ヒープ
 * @param   lytRes    レイアウトリソース
 * @param   data      デコレーションデータ
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_UpperFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, Static::DecorationData data )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( data.deco_param[0].frame_id, 0 );
  return CreateDecoObject_UpperFrame( heap, texInfo );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコオブジェクトの生成：下画面
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   デコオブジェクトの生成：下画面
 *
 * @param   heap    ヒープ
 * @param   lytRes  レイアウトリソース
 * @param   texID   テクスチャID
 * @param   pos     配置座標
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_Lower( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID, gfl2::math::VEC3 pos )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( texID, 0 );
  return CreateDecoObject_Lower( heap, texInfo, pos );
}

/**
 * @brief   デコオブジェクトの生成：下画面
 *
 * @param   heap      ヒープ
 * @param   texInfo   テクスチャ情報
 * @param   pos       配置座標
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_Lower( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::math::VEC3 pos )
{
  gfl2::lyt::LytPicture* picture = CreateDecoObject( heap, texInfo );

  // スケールを設定
  gfl2::math::VEC2 scale( Static::DECO_SCALE_RATE, Static::DECO_SCALE_RATE );
  picture->SetScale( scale );

  // 位置を設定
  picture->SetTranslate( pos );

  return picture;
}

/**
 * @brief   デコオブジェクトの生成
 *
 * @param   heap      ヒープ
 * @param   lytRes    レイアウトリソース
 * @param   texID     テクスチャID
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_LowerFrame( gfl2::heap::HeapBase* heap, gfl2::lyt::LytRes* lytRes, u32 texID )
{
  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes->GetTexture( texID, 0 );
  return CreateDecoObject_LowerFrame( heap, texInfo );
}

/**
 * @brief   デコオブジェクトの生成
 *
 * @param   heap      ヒープ
 * @param   texInfo   テクスチャ情報
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject_LowerFrame( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo )
{
  gfl2::lyt::LytPicture* picture = CreateDecoObject( heap, texInfo );

  // スケールを設定
  gfl2::math::VEC2 scale( Static::DECO_SCALE_RATE, Static::DECO_SCALE_RATE );
  picture->SetScale( scale );

  // 位置を設定
  gfl2::math::VEC3 pos(0, 0, 0);
  picture->SetTranslate( pos );

  return picture;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコオブジェクトの生成：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   デコオブジェクトの生成
 *
 * @param   heap      ヒープ
 * @param   texInfo   テクスチャ情報
 *
 * @retval  ピクチャペイン
 */
gfl2::lyt::LytPicture* DecoObject::CreateDecoObject( gfl2::heap::HeapBase* heap, const gfl2::lyt::LytResTextureInfo* texInfo )
{
  gfl2::lyt::LytPicture* picture = GFL_NEW( heap ) gfl2::lyt::LytPicture( *texInfo );
  picture->SetUserAllocated();
  //picture->SetVisible( false );

  return picture;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコオブジェクトの破棄：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   デコオブジェクトの破棄
 */
void DecoObject::DeleteDecoObject( gfl2::lyt::LytPicture* picture )
{
  GFL_SAFE_DELETE( picture );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    座標変換
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   座標変換：下画面の座標値から上画面の座標値へ変換
 *
 * @param   lowPos    下画面の座標値
 *
 * @retval  上画面の座標値
 */
gfl2::math::VEC3 DecoObject::ConvertDecoPositionToUpper( gfl2::math::VEC3 lowPos )
{
  gfl2::math::VEC3 pos = lowPos;
  pos.x /= Static::DECO_SCALE_RATE;
  pos.y /= Static::DECO_SCALE_RATE;
  return pos;
}

/**
 * @brief   座標変換：上画面の座標値から下画面の座標値へ変換
 *
 * @param   uppPos    上画面の座標値
 *
 * @retval  下画面の座標値
 */
gfl2::math::VEC3 DecoObject::ConvertDecoPositionToLower( gfl2::math::VEC3 uppPos )
{
  gfl2::math::VEC3 pos = uppPos;
  pos.x *= Static::DECO_SCALE_RATE;
  pos.y *= Static::DECO_SCALE_RATE;
  return pos;
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
