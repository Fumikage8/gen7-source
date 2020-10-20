//======================================================================
/**
 * @file FinderMenuUpperDraw.cpp
 * @date 2015/11/02 18:59:15
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuUpperDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <Field/FieldRo/include/TrialModel/FieldFinderUtil.h>
#include <GameSys/include/GameData.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuUpperDraw::FinderMenuUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pGameData( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData() )
  , m_pPhotoPicture( NULL )
  , m_PhotoNum(6)
  , m_SnapFlag(0)
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  for( u8 i=0; i<SNAP_BUF_NUM; ++i )
  {
    m_pPhotoSnap[i] = static_cast<Field::TrialModel::FinderPhotoSnap*>( GflHeapAllocMemoryAlign( devHeap, sizeof(Field::TrialModel::FinderPhotoSnap), 128 ) );
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pMsgData  = msgData;
  m_pWordSet  = wordSet;
  m_pAppParam = pAppParam;
  m_PhotoNum  = pAppParam->shutter_count;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuUpperDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

#if defined(GF_PLATFORM_CTR)
  for( u8 i=0; i<SNAP_BUF_NUM; ++i )
  {
    GflHeapFreeMemory( m_pPhotoSnap[i] );
  }
#endif

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      Open();
    } break;

  case SEQ_IN:
    {
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderMenuUpperDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Open( void )
{
  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Close( void )
{
  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool FinderMenuUpperDraw::IsEndInOutAnim( void )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER )
      && m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_PHOTO_UP_000_BFLYT,
      LA_FINDER_MENU_FINDER_PHOTO_UP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet);

  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  m_pPhotoPicture = m_pDrawerBase->GetPicture( PANENAME_FINDER_PHOTO_UP_000_PANE_PHOTO_DUMMY );

  // 写真データのセットアップ
  SetupPhotoData();

  SetSelectPhoto( 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   写真データのセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::SetupPhotoData( void )
{
  for( u8 i=0; i<PHOTO_DATA_MAX; ++i )
  {
    m_pPhotoData[i] = m_pAppParam->photo_data[i];
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   写真選択位置の設定
 * @param   index   選択位置
 */
//------------------------------------------------------------------------------
void FinderMenuUpperDraw::SetSelectPhoto( u32 index )
{
#if defined(GF_PLATFORM_CTR)
  m_pDrawerBase->SetVisiblePane( m_pPhotoPicture, false );

  if( index < m_PhotoNum )
  {
    gfl2::heap::HeapBase* workHeap = m_pHeap->GetDeviceHeap();

    Field::TrialModel::FinderPhotoSnap* photoSnap = m_pPhotoSnap[m_SnapFlag];

    // @fix cov_ctr[11920]
    Field::TrialModel::FinderPhotoJpeg* jpeg = &m_pPhotoData[index]->jpeg;
    Field::TrialModel::FinderUtil::DecodeColorBuffer( workHeap->GetLowerHandle(), jpeg, photoSnap->data, nn::jpeg::PIXEL_FORMAT_CTR_RGBA8_BLOCK8 );

    FUKUSHIMA_PRINT( "PhotoIndex[%d] PhotoSnap[%d]\n", index, m_SnapFlag );

    const nw::lyt::TexSize texSize(256, 512);
    const nw::lyt::TexSize realSize(256, 512);
    const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

    uptr physicalAddr =  nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>(photoSnap->data) );

    gfl2::lyt::LytResTextureInfo texInfo;
    texInfo.Set( 0, physicalAddr, texSize, realSize, format );

    gfl2::lyt::LytMaterial* material = m_pPhotoPicture->GetMaterial();
    GFL_ASSERT( material );
    material->SetTexMap( 0, &texInfo );
    
    m_pDrawerBase->SetVisiblePane( m_pPhotoPicture, true );

    m_SnapFlag = (m_SnapFlag + 1) % SNAP_BUF_NUM;
  }
#endif
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
