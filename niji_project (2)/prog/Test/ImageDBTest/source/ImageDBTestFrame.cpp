#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestFrame.cpp
 * @date    2016/10/21 15:58:33
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ImageDBTestFrame.h"
#include "ImageDBTestUpperDraw.h"
#include "ImageDBTestLowerDraw.h"
#include "ImageDBTestLowerAlbum.h"

#include "search/ImageDBTestSearch.h"
#include "pic/ImageDBTestSavePicture.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/FinderStudioUITest.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )


//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum {
    MM_SEQ_MAIN,
    MM_SEQ_END,
  };

  static u32 HEAP_SIZE_TOTAL = 0x480000;              //
  static u32 HEAP_SIZE_SAVE_IMG_BUF = 0x80000;        // sizeof(u32) * 512 * 256
  static u32 HEAP_SIZE_LOAD_IMG_BUF = 0x80000;        // sizeof(u32) * 512 * 256
  static u32 HEAP_SIZE_LOAD_THM_BUF = 0x80000;        // (sizeof(u32) * 128 * 64) * 16
  static u32 HEAP_SIZE_LOAD_TMP1_BUF = 0x80000;       // sizeof(u32) * 512 * 256
  static u32 HEAP_SIZE_LOAD_TMP2_BUF = 0x80000;       // sizeof(u32) * 512 * 256
  static u32 HEAP_SIZE_IMGDB_ALLOCATOR = 0x130000;    // ImageDBAccessor内で確保：ライブラリ側に渡している
  static u32 HEAP_SIZE_IMGDB_TMP1_BUF = 0x5DC00;      // ImageDBAccessor内で確保：テンポラリバッファ
  static u32 HEAP_SIZE_IMGDB_TMP2_BUF = 0x5DC00;      // ImageDBAccessor内で確保：テンポラリバッファ
  static u32 HEAP_SIZE_ALLOC = (HEAP_SIZE_SAVE_IMG_BUF
                              + HEAP_SIZE_LOAD_IMG_BUF
                              + HEAP_SIZE_LOAD_THM_BUF
                              + HEAP_SIZE_LOAD_TMP1_BUF
                              + HEAP_SIZE_LOAD_TMP2_BUF
                              + HEAP_SIZE_IMGDB_ALLOCATOR
                              + HEAP_SIZE_IMGDB_TMP1_BUF
                              + HEAP_SIZE_IMGDB_TMP2_BUF);
  static u32 HEAP_SIZE_PADDING = HEAP_SIZE_TOTAL - HEAP_SIZE_ALLOC;   // 0x14800
  static u32 IMGDB_HEAP_SIZE = HEAP_SIZE_ALLOC + HEAP_SIZE_PADDING;
}




//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
Frame::Frame( void )
  : m_pAppHeap(NULL)
  , m_pAppRenderingManager(NULL)
  , m_pImgdbHeap(NULL)
  , m_pLytDataBuff(NULL)
  , m_pImgdbSave(NULL)
  , m_pImgdbLoad(NULL)
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
  , m_pLowerAlbumDraw(NULL)
  , m_LoadSeq( 0 )
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
applib::frame::Result Frame::InitFunc( void )
{
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  // ImageDB用ヒープを生成
  m_pImgdbHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppHeap->GetDeviceHeap(), IMGDB_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "ImageDBHeap" );

  gfl2::heap::HeapBase* heap = m_pAppHeap->GetSystemHeap();
  {
#if defined(GF_PLATFORM_CTR)
    // ImageDB保存クラス
    m_pImgdbSave = GFL_NEW( heap ) ImageDBSave( m_pImgdbHeap );
    // ImageDB読込みクラス
    m_pImgdbLoad = GFL_NEW( heap ) ImageDBLoad( m_pImgdbHeap );
    m_pImgdbLoad->SetListener( this );
#endif // defined(GF_PLATFORM_CTR)

    // 上画面
    m_pUpperDraw = GFL_NEW( heap ) UpperDraw( m_pAppHeap, m_pAppRenderingManager );
    m_pUpperDraw->SetDrawListener( this );
    m_pUpperDraw->InitializeSystem();
    m_pUpperDraw->Initialize2D( m_pLytDataBuff );
    m_pUpperDraw->InitializePane();

    // 下画面
    m_pLowerDraw = GFL_NEW( heap ) LowerDraw( m_pAppHeap, m_pAppRenderingManager );
    m_pLowerDraw->SetDrawListener( this );
    m_pLowerDraw->InitializeSystem();
    m_pLowerDraw->Initialize2D( m_pLytDataBuff );
    m_pLowerDraw->InitializeButtonManager();

    // 下画面アルバム
    m_pLowerAlbumDraw = GFL_NEW( heap ) LowerAlbumDraw( m_pAppHeap, m_pAppRenderingManager );
    m_pLowerAlbumDraw->SetDrawListener( this );
    m_pLowerAlbumDraw->InitializeSystem();
    m_pLowerAlbumDraw->Initialize2D( m_pLytDataBuff );
    m_pLowerAlbumDraw->InitializeButtonManager();

    m_pUpperDraw->AddSubView( m_pLowerDraw );
    m_pUpperDraw->AddSubView( m_pLowerAlbumDraw );
  }

  return applib::frame::RES_FINISH;
}

applib::frame::Result Frame::EndFunc( void )
{
  if( m_pLowerAlbumDraw )
  {
    if( !m_pLowerAlbumDraw->Terminate() ) return applib::frame::RES_CONTINUE;

    m_pLowerAlbumDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pLowerAlbumDraw );
  }

  if( m_pLowerDraw )
  {
    if( !m_pLowerDraw->Terminate() ) return applib::frame::RES_CONTINUE;

    m_pLowerDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pLowerDraw );
  }

  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->Terminate() ) return applib::frame::RES_CONTINUE;

    GFL_SAFE_DELETE( m_pUpperDraw );
  }

#if defined(GF_PLATFORM_CTR)
  ImageDBSearch::DeletePictureInfo();
  GFL_SAFE_DELETE( m_pImgdbLoad );
  GFL_SAFE_DELETE( m_pImgdbSave );
#endif // defined(GF_PLATFORM_CTR)

  GflHeapFreeMemory( m_pLytDataBuff );

  GFL_DELETE_HEAP( m_pImgdbHeap );

  return applib::frame::RES_FINISH;
}



//----------------------------------------------------------------------
// @brief   更新・描画処理
//----------------------------------------------------------------------
applib::frame::Result Frame::UpdateFunc( void )
{
  m_pUpperDraw->UpdateTree();

#if defined(GF_PLATFORM_CTR)
  m_pImgdbSave->Update();
  m_pImgdbLoad->Update();
#endif // defined(GF_PLATFORM_CTR)

  if( this->GetSubSeq() == MM_SEQ_END )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

void Frame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pUpperDraw->DrawTree( no );
}



//----------------------------------------------------------------------
// @brief   レイアウトデータの読込み
//----------------------------------------------------------------------
bool Frame::LoadLayoutData( void )
{
  gfl2::heap::HeapBase* heap = m_pAppHeap->GetSystemHeap();
  u32 arcID = ARCID_TEST_UI_FINDER_STUDIO;

  if( m_LoadSeq == 0 )
  {
    app::util::FileAccessor::FileOpen( arcID, heap->GetLowerHandle() );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 1 )
  {
    if( !app::util::FileAccessor::IsFileOpen( arcID ) ) return false;

    app::util::FileAccessor::FileLoad(
      arcID,
      GARC_FinderStudioUITest_FinderStudioUITest_applyt_COMP,
      &m_pLytDataBuff,
      heap->GetLowerHandle(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 2 )
  {
    if( !app::util::FileAccessor::IsFileLoad( &m_pLytDataBuff ) ) return false;

    app::util::FileAccessor::FileClose( arcID, heap->GetLowerHandle() );
    m_LoadSeq++;
  }
  if( m_LoadSeq == 3 )
  {
    if( !app::util::FileAccessor::IsFileClose( arcID ) ) return false;

    m_LoadSeq++;
  }
  if( m_LoadSeq == 4 )
  {
    return true;
  }

  return false;
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
void Frame::OnEndImageDBTest( void )
{
  this->SetSubSeq( MM_SEQ_END );
}

void Frame::OnRequestPhotoShoot( void )
{
#if defined(GF_PLATFORM_CTR)
  m_pImgdbSave->RequestPhotoShoot_Only( m_pAppRenderingManager );
#endif // defined(GF_PLATFORM_CTR)
}

void Frame::OnRequestImgdbSave( void )
{
#if defined(GF_PLATFORM_CTR)
  m_pImgdbSave->RequestPhotoShoot_Save( m_pAppRenderingManager );
#endif // defined(GF_PLATFORM_CTR)
}

void Frame::OnRequestImgdbLoad_Snap( u32 idx )
{
#if defined(GF_PLATFORM_CTR)
  ImageDBPictureInfo* picInfo = ImageDBSearch::m_pPictureInfo;
  u32 photoNum = ImageDBSearch::m_PhotoNum;

  if( picInfo && idx < ImageDBSearch::m_PhotoNum )
  {
    GFL_PRINT( "LoadSnapImage Idx[%d]\n", idx );
    GFL_PRINT( " PATH[%ls]\n", picInfo->m_path );
    GFL_PRINT( " UNQID[%d]\n", picInfo->m_uniqueId );

    gfl2::lyt::LytPicture* picture = m_pUpperDraw->GetPicturePane_Sidewise();
    m_pImgdbLoad->RequestLoadPhoto_Snap( picInfo[idx].m_path, picture );
  }
#endif // defined(GF_PLATFORM_CTR)
}

void Frame::OnRequestImgdbLoad_Thumb( u32 thumbIdx, u32 paneIndex, gfl2::lyt::LytPicture* picture )
{
#if defined(GF_PLATFORM_CTR)
  ImageDBPictureInfo* picInfo = ImageDBSearch::m_pPictureInfo;

  if( picInfo && thumbIdx < ImageDBSearch::m_PhotoNum )
  {
    m_pImgdbLoad->RequestLoadPhoto_Thumb( picInfo[thumbIdx].m_path, picture, paneIndex );
  }
#endif // defined(GF_PLATFORM_CTR)
}

void Frame::OnRequestImgdbSearch( void )
{
#if defined(GF_PLATFORM_CTR)
  ImageDBSearch::ImageSearch( m_pImgdbHeap, m_pAppHeap->GetSystemHeap() );
#endif // defined(GF_PLATFORM_CTR)
}

void Frame::OnRequestOpenAlbum( void )
{
  m_pLowerAlbumDraw->Open();
}

void Frame::OnRequestCloseAlbum( void )
{
  m_pLowerAlbumDraw->Close();
}


void Frame::OnEndLoadThumbnail( void )
{
  m_pLowerAlbumDraw->NextThumbnailSeq();
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
