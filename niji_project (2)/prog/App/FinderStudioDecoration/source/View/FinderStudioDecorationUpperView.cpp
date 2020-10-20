//======================================================================
/**
 * @file FinderStudioDecorationUpperView.cpp
 * @date 2017/01/31 17:22:39
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationUpperView.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoObject.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
UpperView::UpperView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : DecoViewBase( heap, renderMan )
  , m_pBackGroundPane( NULL )
  , m_pDecoBasePane( NULL )
  , m_pFrameBasePane( NULL )
  , m_pStampBasePane( NULL )
  , m_pStampMovePane( NULL )
  , m_pFramePicture( NULL )
  , m_FrameIndex( Static::DECO_NULL )
  , m_StampCount(0)
{
  for( u8 i = 0; i < STAMP_MAX; ++i )
  {
    m_pStampPicture[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化：公開
 */
void UpperView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // アニメの初期化
  InitAnim();
}

/**
 * @brief   2Dの初期化：非公開
 */
void UpperView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  GFL_UNUSED( msgData );
  GFL_UNUSED( wordSet );

  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIODECO_STUDIO_DECO_UP_00_BFLYT,
      LA_FINDERSTUDIODECO_STUDIO_DECO_UP_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1,
    resTbl, GFL_NELEMS( resTbl ),
    setupData, GFL_NELEMS( setupData ),
    this->UNUSE_MESSAGE_DATA );

  // ViewUtilの生成
  this->CreateViewUtil();
  // 表示
  this->_Show();
}

/**
 * @brief   初期化：ペイン
 */
void UpperView::InitPane( void )
{
  // 背景ヌルペイン
  m_pBackGroundPane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_UP_00_PANE_NULL_BG );
  // デコ配置用ヌルペイン
  m_pDecoBasePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_UP_00_PANE_NULL_STAMPALL );
  // フレーム配置用ヌルペイン
  m_pFrameBasePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_UP_00_PANE_NULL_FRAME );
  // スタンプ配置用ヌルペイン
  m_pStampBasePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_UP_00_PANE_NULL_STAMP_00 );
  // スタンプ移動用ヌルペイン
  m_pStampMovePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_UP_00_PANE_NULL_STAMP_01 );
}

/**
 * @brief   初期化：アニメ
 */
void UpperView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_UP_00_BG_LOOP_01 );
}

/**
 * @brief   破棄：公開
 */
bool UpperView::Terminate( void )
{
  // フレームの削除
  _DeleteFrameTexture();

  // スタンプの削除
  for( u8 i = 0; i < STAMP_MAX; ++i )
  {
    if( m_pStampPicture[i] )
    {
      m_pStampBasePane->RemoveChild( m_pStampPicture[i] );
      // 削除
      GFL_SAFE_DELETE( m_pStampPicture[i] );
    }
  }

  // ViewUtilの破棄
  this->DeleteViewUtil();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示：背景
 */
void UpperView::ShowBackground( void )
{
  m_pUtil->SetVisiblePane( m_pDecoBasePane, false );
  m_pUtil->SetVisiblePane( m_pBackGroundPane, true );
}

/**
 * @brief   非表示：背景
 */
void UpperView::HideBackground( void )
{
  m_pUtil->SetVisiblePane( m_pDecoBasePane, true );
  m_pUtil->SetVisiblePane( m_pBackGroundPane, false );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    スタンプ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   スタンプの生成
 */
void UpperView::CreateStamp( u8 decoCount, DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  FUKUSHIMA_PRINT( "UpperView::CreateStamp : type=[%d] decoIndex[%d] pos x=[%.1f] y=[&.1f]\n", type, decoIndex, pos.x, pos.y );

  // 整数部だけを取得
  pos = _GetIntegerPart_VEC3( pos );

  // スタンプテクスチャの生成
  _CreateStampTexture( decoCount, type, decoIndex, pos );
}

/**
 * @brief   スタンプテクスチャの生成
 */
void UpperView::_CreateStampTexture( u8 decoCount, DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  u32 texID = Static::DecoDataAccessor::GetDecoTexID( type, decoIndex );

  // フレーム用ピクチャペインを生成
  m_pStampPicture[decoCount] = Static::DecoObject::CreateDecoObject_Upper(
    m_pHeap->GetDeviceHeap(), m_pUtil->GetLytRes(), texID, pos );

  // 子に追加
  m_pStampBasePane->AppendChild( m_pStampPicture[decoCount] );
}


/**
 * @brief   スタンプの削除
 */
void UpperView::DeleteStamp( u8 index )
{
  _DeleteStampTexture( index );
}

/**
 * @brief   スタンプテクスチャの破棄
 */
void UpperView::_DeleteStampTexture( u8 index )
{
  if( !_IsStampNull( index ) )
  {
    FUKUSHIMA_PRINT( "UpperView::_DeleteStampTexture : index=[%d]\n", index );

    m_pStampBasePane->RemoveChild( m_pStampPicture[index] );
    // 削除
    GFL_SAFE_DELETE( m_pStampPicture[index] );

    // 空いた部分を詰める
    _StuffingStamp( index );
  }
}

/**
 * @brief   スタンプを詰める
 *
 * @param   startIndex    開始インデックス
 */
void UpperView::_StuffingStamp( u8 startIndex )
{
  for( u8 i = startIndex; i < STAMP_MAX - 1; ++i )
  {
    m_pStampPicture[i] = m_pStampPicture[i + 1];
  }
  m_pStampPicture[STAMP_MAX - 1] = NULL;
}

/**
 * @brief   スタンプの位置変更
 */
void UpperView::SetStampPos( u8 index, gfl2::math::VEC3 pos )
{
  if( !_IsStampNull( index ) )
  {
    gfl2::math::VEC3 vecLayout = Static::ConvertUpperVectorScreenToLayout( pos );

    // 整数部だけを取得
    vecLayout = _GetIntegerPart_VEC3( vecLayout );
    m_pUtil->SetPanePos( m_pStampPicture[index], vecLayout );
  }
}

/**
 * @brief   スタンプを表示
 */
void UpperView::VisibleStamp( u8 index )
{
  if( !_IsStampNull( index ) )
  {
    m_pUtil->SetVisiblePane( m_pStampPicture[index], true );
  }
}

/**
 * @brief   スタンプを非表示
 */
void UpperView::InvisibleStamp( u8 index )
{
  if( !_IsStampNull( index ) )
  {
    m_pUtil->SetVisiblePane( m_pStampPicture[index], false );
  }
}

/**
 * @brief   スタンプがNULLかどうか
 *
 * @param   index   スタンプのインデックス
 */
bool UpperView::_IsStampNull( u8 index )
{
  GFL_ASSERT( index < STAMP_MAX );

  return (m_pStampPicture[index] == NULL);
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレーム
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フレームの生成
 */
void UpperView::CreateFrame( u32 frameIndex )
{
  FUKUSHIMA_PRINT( "UpperView::CreateFrame : frameIndex=[%d]\n", frameIndex );

  if( m_FrameIndex != frameIndex )
  {
    // テクスチャの破棄
    _DeleteFrameTexture();
    // テクスチャの生成
    _CreateFrameTexture( frameIndex );

    m_pUtil->SetVisiblePane( m_pDecoBasePane, true );

    m_FrameIndex = frameIndex;
  }
}

/**
 * @brief   フレームテクスチャの生成
 */
void UpperView::_CreateFrameTexture( u32 frameIndex )
{
  if( frameIndex != Static::DECO_NULL )
  {
    u32 texID = Static::DecoDataAccessor::GetDecoTexID( DECO_TYPE_FRAME, frameIndex );

    // フレーム用ピクチャペインを生成
    m_pFramePicture = Static::DecoObject::CreateDecoObject_UpperFrame(
      m_pHeap->GetDeviceHeap(), m_pUtil->GetLytRes(), texID );

    // 子に追加
    m_pFrameBasePane->AppendChild( m_pFramePicture );
  }
}

/**
 * @brief   フレームテクスチャの破棄
 */
void UpperView::_DeleteFrameTexture( void )
{
  if( m_FrameIndex != Static::DECO_NULL )
  {
    m_pFrameBasePane->RemoveChild( m_pFramePicture );
    GFL_SAFE_DELETE( m_pFramePicture );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ユーティリティ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   浮動小数点の整数部を取得
 *
 * @param[in]   val   対象の値
 *
 * @retval  整数部
 */
s32 UpperView::_GetIntegerPart_Float( f32 val )
{
  return static_cast<s32>( val );
}

/**
 * @brief   浮動小数点の整数部を取得：VEC3
 *
 * @param[in]   vec   対象のベクトル
 *
 * @retval  整数部だけのベクトル
 */
gfl2::math::VEC3 UpperView::_GetIntegerPart_VEC3( gfl2::math::VEC3 vec )
{
  s32 x = _GetIntegerPart_Float( vec.x );
  s32 y = _GetIntegerPart_Float( vec.y );

  gfl2::math::VEC3 newVec( 0, 0, 0 );
  {
    newVec.x = static_cast<f32>( x );
    newVec.y = static_cast<f32>( y );
  }

  return newVec;
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
