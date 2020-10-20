//======================================================================
/**
 * @file    FinderStudioCaptureUpperView.cpp
 * @date    2016/11/24 15:26:08
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：上画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)
#include <nn/jpeg.h>  // nn
#endif

#include "FinderStudioCaptureUpperView.h"
#include "FinderStudioCaptureViewListener.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 定数宣言 begin ]---
static const u32 ANIM_ID_CURTAIN_OPEN = LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_OPEN;
static const u32 ANIM_ID_CURTAIN_CLOSE = LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_CLOSE;
static const u32 ANIM_ID_SHUTTER_OPEN = LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_SHUTTER_00;
static const u32 ANIM_ID_SHUTTER_CLOSE = LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_SHUTTER_01;
/// ---[ 定数宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
UpperView::UpperView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_pPhotoTexture( NULL )
  , m_pPhotoPicture( NULL )
  , m_PhotoTexInfo()
  , m_CurtainSeq( CURTAIN_SEQ_INIT )
  , m_PhotoSeq( PHOTO_SEQ_IDLE )
  , m_ShutterSeq( SHUTTER_SEQ_OPEN_KEEP )
  , m_TexInfoFlg( 0 )
  , m_AutoFlg( true )
{
  ;
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
  // 写真データバッファの生成
  CreatePhotoTexture();
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

  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00___NUM,
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
 * @brief   レイアウトペインの初期設定：非公開
 */
void UpperView::InitPane( void )
{
  m_pPhotoPicture = m_pUtil->GetPicture( PANENAME_STUDIO_CURTAIN_UP_00_PANE_DUMMYPHOTO );
  GFL_ASSERT( m_pPhotoPicture );
  InvisiblePhotoImage();
}

/**
 * @brief   レイアウトアニメの初期設定：非公開
 */
void UpperView::InitAnim( void )
{
  // カーテン閉じた状態
  _StartCurtainAnim( CURTAIN_ANIM_CLOSE_KEEP );
  // 照準は非表示
  InvisibleSight();
}

/**
 * @brief   破棄：公開
 */
bool UpperView::Terminate( void )
{
  // ViewUtilの破棄
  this->DeleteViewUtil();
  // レイアウトシステムの破棄
  this->TerminateSystem();
  // 写真テクスチャの破棄
  DeletePhotoTexture();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：公開
 */
void UpperView::Update( void )
{
  app::ui::UIView::Update();

  _UpdateCurtainAnim();
  _UpdatePhotoImage();
  _UpdateShutterAnim();
}

/**
 * @brief   カーテンアニメの更新：非公開
 */
void UpperView::_UpdateCurtainAnim( void )
{
  if( m_CurtainSeq == CURTAIN_SEQ_OPEN )
  {
    if( m_pUtil->IsAnimEnd( ANIM_ID_CURTAIN_OPEN ) )
    {
      _StartCurtainAnim( CURTAIN_ANIM_OPEN_KEEP );
      m_AutoFlg = true;
    }
  }
  else if( m_CurtainSeq == CURTAIN_SEQ_CLOSE )
  {
    if( m_pUtil->IsAnimEnd( ANIM_ID_CURTAIN_CLOSE ) )
    {
      _StartCurtainAnim( CURTAIN_ANIM_CLOSE_KEEP );
    }
  }
}

/**
 * @brief   写真表示の更新：非公開
 */
void UpperView::_UpdatePhotoImage( void )
{
  switch( m_PhotoSeq )
  {
    case PHOTO_SEQ_INVISIBLE:
    {
      InvisiblePhotoImage();
      m_PhotoSeq = PHOTO_SEQ_MAKE_TEXTURE;
    } // no break;

    case PHOTO_SEQ_MAKE_TEXTURE:
    {
      _MakeTexture();
      m_PhotoSeq = PHOTO_SEQ_REPLACE_TEXTURE;
    } // no break;

    case PHOTO_SEQ_REPLACE_TEXTURE:
    {
      _ReplaceTexture();
      m_PhotoSeq = PHOTO_SEQ_VISIBLE;
    } // no break;

    case PHOTO_SEQ_VISIBLE:
    {
      _NextTexInfo();
      VisiblePhotoImage();
      m_PhotoSeq = PHOTO_SEQ_IDLE;
    } // no break;
  }
}

/**
 * @brief   更新：シャッターアニメ
 */
void UpperView::_UpdateShutterAnim( void )
{
  if( m_ShutterSeq == SHUTTER_SEQ_OPEN )
  {
    if( m_pUtil->IsAnimEnd( ANIM_ID_SHUTTER_OPEN ) )
    {
      m_ShutterSeq = SHUTTER_SEQ_OPEN_KEEP;
    }
  }
  else if( m_ShutterSeq == SHUTTER_SEQ_CLOSE )
  {
    if( m_pUtil->IsAnimEnd( ANIM_ID_SHUTTER_CLOSE ) )
    {
      m_ShutterSeq = SHUTTER_SEQ_CLOSE_KEEP;
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーテンアニメ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーテンを開く：公開
 */
void UpperView::CurtainOpen( void )
{
  _StartCurtainAnim( CURTAIN_ANIM_OPEN );
}

/**
 * @brief   カーテンを閉じる：公開
 */
void UpperView::CurtainClose( bool autoAnim/* = true*/ )
{
  _StartCurtainAnim( CURTAIN_ANIM_CLOSE );
  m_AutoFlg = autoAnim;
}

/**
 * @brief   カーテンを閉じた状態：公開
 */
void UpperView::CurtainCloseKeep( bool autoAnim/* = true*/ )
{
  _StartCurtainAnim( CURTAIN_ANIM_CLOSE_KEEP );
  m_AutoFlg = autoAnim;
}

/**
 * @brief   カーテンアニメの再生：非公開
 */
void UpperView::_StartCurtainAnim( CurtainAnim animID )
{
  const u32 c_CurtainAnim[CURTAIN_ANIM_NUM] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_OPEN,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_OPENKEEP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_CLOSE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_CURTAIN_CLOSEKEEP,
  };
  const CurtainSeq c_CurtainSeq[CURTAIN_ANIM_NUM] = {
    CURTAIN_SEQ_OPEN,
    CURTAIN_SEQ_OPEN_KEEP,
    CURTAIN_SEQ_CLOSE,
    CURTAIN_SEQ_CLOSE_KEEP,
  };
  m_pUtil->StartAnim( c_CurtainAnim[animID] );
  m_CurtainSeq = c_CurtainSeq[animID];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    照準
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   照準の表示
 */
void UpperView::VisibleSight( void )
{
  _SetSightAnim( SIGHT_TYPE_CROSS );
}

/**
 * @brief   照準の非表示
 */
void UpperView::InvisibleSight( void )
{
  _SetSightAnim( SIGHT_TYPE_NONE );
}

/**
 * @brief   照準アニメのセット
 */
void UpperView::_SetSightAnim( SightType type )
{
  const u32 c_SightAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_SIGHTPATERN_00,
    LA_FINDERSTUDIOCAPTURE_STUDIO_CURTAIN_UP_00_SIGHTPATERN_02,
  };

  // アニメの再生
  m_pUtil->StartAnim( c_SightAnim[type] );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    シャッター
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   シャッターを開く
 */
void UpperView::ShutterOpen( void )
{
  if( m_ShutterSeq == SHUTTER_SEQ_CLOSE_KEEP )
  {
    m_pUtil->StartAnim( ANIM_ID_SHUTTER_OPEN );
    m_ShutterSeq = SHUTTER_SEQ_OPEN;
  }
  else {
    GFL_ASSERT( 0 );
  }
}

/**
 * @brief   シャッターを閉じる
 */
void UpperView::ShutterClose( void )
{
  if( m_ShutterSeq == SHUTTER_SEQ_OPEN_KEEP )
  {
    m_pUtil->StartAnim( ANIM_ID_SHUTTER_CLOSE );
    m_ShutterSeq = SHUTTER_SEQ_CLOSE;
  }
  else {
    GFL_ASSERT( 0 );
  }
}

/**
 * @brief   シャッターを開くアニメが終了したか
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool UpperView::IsShutterOpen( void )
{
  return (m_ShutterSeq == SHUTTER_SEQ_OPEN_KEEP);
}

/**
 * @brief   シャッターを閉じるアニメが終了したか
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool UpperView::IsShutterClose( void )
{
  return ( m_ShutterSeq == SHUTTER_SEQ_CLOSE_KEEP );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真 表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示：公開
 */
void UpperView::VisiblePhotoImage( void )
{
  m_pUtil->SetVisiblePane( m_pPhotoPicture, true );
}

/**
 * @brief   表示：非公開
 */
void UpperView::InvisiblePhotoImage( void )
{
  m_pUtil->SetVisiblePane( m_pPhotoPicture, false );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リクエスト
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真表示リクエスト：公開
 */
void UpperView::RequestPhotoImage( void* texture )
{
  const u32 bufSize = sizeof( u32 ) * Static::PHOTO_BUFFER_WIDTH * Static::PHOTO_BUFFER_HEIGHT;
  gfl2::std::MemCopy( texture, m_pPhotoTexture, bufSize );

  m_PhotoSeq = PHOTO_SEQ_INVISIBLE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真描画処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   TexInfoの生成：非公開
 */
void UpperView::_MakeTexture( void )
{
  gfl2::lyt::LytResTextureInfo* texInfo = &m_PhotoTexInfo[m_TexInfoFlg];

  // TexInfoの生成
  _MakeTexInfo( texInfo, m_pPhotoTexture );
}

/**
 * @brief   テクスチャの差し替え：非公開
 */
void UpperView::_ReplaceTexture( void )
{
  gfl2::lyt::LytResTextureInfo* texInfo = &m_PhotoTexInfo[m_TexInfoFlg];

  // テクスチャの差し替え
  _SetTexture( texInfo );
}

/**
 * @brief   バッファフラグの更新：非公開
 */
void UpperView::_NextTexInfo( void )
{
  m_TexInfoFlg = ( m_TexInfoFlg + 1 ) % TEXTURE_INFO_NUM;
}

/**
 * @brief   TexInfoの生成：非公開
 */
void UpperView::_MakeTexInfo( gfl2::lyt::LytResTextureInfo* dstTexInfo, void* srcTexture )
{
#if defined(GF_PLATFORM_CTR)
#if 0
  // gfl2::gfx::Texture版。
  // 生成の際に確保できないなど問題があったため使用しなくなりました。
  m_pUtil->GetG2D()->MakeTextureInfo( dstTexInfo, srcTexture );
#else
  const nw::lyt::TexSize texSize( Static::PHOTO_BUFFER_WIDTH, Static::PHOTO_BUFFER_HEIGHT );
  const nw::lyt::TexSize realSize( Static::PHOTO_BUFFER_WIDTH, Static::PHOTO_BUFFER_HEIGHT );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  // 仮想アドレスから物理アドレスへの変換
  uptr physicalAddr = nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>( srcTexture ) );

  // テクスチャの生成
  dstTexInfo->Set( 0, physicalAddr, texSize, realSize, format );
#endif
#endif
}

/**
 * @brief   テクスチャの差し替え：非公開
 */
void UpperView::_SetTexture( const gfl2::lyt::LytResTextureInfo* texInfo )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::lyt::LytMaterial* material = m_pPhotoPicture->GetMaterial( 0 );
  material->SetTexMap( 0, texInfo );
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真データ 生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真データの生成：非公開
 */
void UpperView::CreatePhotoTexture( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap()->GetLowerHandle();

  // @note ピクチャペインに貼り付けるテクスチャバッファは[128]でアライメントする必要がある。
  //       Viewerから取得するテクスチャバッファは[16]でアライメントされている。
  m_pPhotoTexture = GflHeapAllocMemoryAlign( heap, Static::PHOTO_BUFFER_SIZE, Static::PHOTO_BUFFER_ALIGN );
}

/**
 * @brief   写真データの破棄：非公開
 */
void UpperView::DeletePhotoTexture( void )
{
  GflHeapSafeFreeMemory( m_pPhotoTexture );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
