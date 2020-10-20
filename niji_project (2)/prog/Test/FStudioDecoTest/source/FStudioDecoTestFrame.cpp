#if PM_DEBUG

//======================================================================
/**
 * @file    FStudioDecoTestFrame.cpp
 * @date    2016/10/26 20:14:55
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FStudioDecoTestFrame.h"

#include "draw/FStudioDecoTestUpperDraw.h"
#include "draw/FStudioDecoTestLowerDraw.h"
#include "draw/FStudioDecoTestLowerTabDraw.h"
#include "draw/FStudioDecoTestLowerDecoIconDraw.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <Test/ImageDBTest/source/pic/ImageDBTestSavePicture.h>
#include <Test/ImageDBTest/source/search/ImageDBTestSearch.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/FStudioDecoTest.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )


//----------------------------------------------------------------------
// @brief   コンストラクタ
//----------------------------------------------------------------------
Frame::Frame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pImgdbHeap(NULL)
  , m_pImgdbSave(NULL)
  , m_pImgdbLoad(NULL)
  , m_pUpperDraw( NULL )
  , m_pLowerDraw( NULL )
  , m_pLowerTabDraw( NULL )
  , m_pLowerDecoIconDraw( NULL )
  , m_pLytDataBuff( NULL )
  , m_InitSeq( 0 )
  , m_LoadSeq( 0 )
  , m_ImgLoadSeq( 0 )
{
  ;
}



//----------------------------------------------------------------------
// @brief   初期化・終了処理
//----------------------------------------------------------------------
applib::frame::Result Frame::InitFunc( void )
{
  if( m_InitSeq == 0 )
  {
    // ImageDB用ヒープを生成
    m_pImgdbHeap = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppHeap->GetDeviceHeap(), IMGDB_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "ImageDBHeap" );
#if defined(GF_PLATFORM_CTR)
    // Save
    m_pImgdbSave = GFL_NEW( m_pAppHeap->GetSystemHeap() ) ImageDB::ImageDBSave( m_pImgdbHeap );
    GFL_ASSERT( m_pImgdbSave );
    // Load
    m_pImgdbLoad = GFL_NEW( m_pAppHeap->GetSystemHeap() ) ImageDB::ImageDBLoad( m_pImgdbHeap );
    GFL_ASSERT( m_pImgdbLoad );
    m_pImgdbLoad->SetListener( this );
#endif // defined(GF_PLATFORM)
    m_InitSeq++;
  }
  if( m_InitSeq == 1 )
  {
    if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;
    if( !LoadPhotoData() )  return applib::frame::RES_CONTINUE;

    m_InitSeq++;
  }
  if( m_InitSeq == 2 )
  {
    gfl2::heap::HeapBase* heap = m_pAppHeap->GetSystemHeap();
    {
      // 上画面
      m_pUpperDraw = GFL_NEW( heap ) UpperDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pUpperDraw->SetDrawListener( this );
      m_pUpperDraw->InitializeSystem();
      m_pUpperDraw->Initialize2D( m_pLytDataBuff );
      m_pUpperDraw->InitializeMember();
#if defined(GF_PLATFORM_CTR)
      m_pUpperDraw->InitializePhoto( m_pImgdbLoad->GetColorBuffer() );
#endif

      // 下画面：キャンバス
      m_pLowerDraw = GFL_NEW( heap ) LowerDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pLowerDraw->SetDrawListener( this );
      m_pLowerDraw->InitializeSystem();
      m_pLowerDraw->Initialize2D( m_pLytDataBuff );
      m_pLowerDraw->InitializeMember();
      m_pLowerDraw->InitializeButtonManager();
#if defined(GF_PLATFORM_CTR)
      m_pLowerDraw->InitializePhoto( m_pImgdbLoad->GetColorBuffer() );
#endif
      m_pUpperDraw->AddSubView( m_pLowerDraw );   // Subビュー追加

      // 下画面：デコアイコン
      m_pLowerDecoIconDraw = GFL_NEW( heap ) LowerDecoIconDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pLowerDecoIconDraw->SetDrawListener( this );
      m_pLowerDecoIconDraw->InitializeSystem();
      m_pLowerDecoIconDraw->Initialize2D( m_pLytDataBuff );
      m_pLowerDecoIconDraw->InitializePane();
      m_pLowerDecoIconDraw->InitializeButtonManager();
      m_pUpperDraw->AddSubView( m_pLowerDecoIconDraw );   // Subビュー追加

      // 下画面：タブ
      m_pLowerTabDraw = GFL_NEW( heap ) LowerTabDraw( m_pAppHeap, m_pAppRenderingManager );
      m_pLowerTabDraw->SetDrawListener( this );
      m_pLowerTabDraw->InitializeSystem();
      m_pLowerTabDraw->Initialize2D( m_pLytDataBuff );
      m_pLowerTabDraw->InitializeButtonManager();
      m_pUpperDraw->AddSubView( m_pLowerTabDraw );   // Subビュー追加
    }
    m_InitSeq++;
  }
  else if( m_InitSeq == 3 )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

applib::frame::Result Frame::EndFunc( void )
{
  if( m_pLowerTabDraw )
  {
    if( !m_pLowerTabDraw->Terminate() ) return applib::frame::RES_CONTINUE;

    m_pLowerTabDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pLowerTabDraw );
  }
  if( m_pLowerDecoIconDraw )
  {
    if( !m_pLowerDecoIconDraw->Terminate() ) return applib::frame::RES_CONTINUE;

    m_pLowerDecoIconDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pLowerDecoIconDraw );
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
  GFL_SAFE_DELETE( m_pImgdbLoad );
  GFL_SAFE_DELETE( m_pImgdbSave );
#endif // defined(GF_PLATFORM)

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
#endif // defined(GF_PLATFORM_CTR)

  if( GetSubSeq() == MM_SEQ_END )
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
  u32 arcID = ARCID_TEST_FINDER_STUDIO_DECO;

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
      GARC_FStudioDecoTest_FStudioDecoTest_applyt_COMP,
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
// @brief   写真データの読込み
//----------------------------------------------------------------------
bool Frame::LoadPhotoData( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_ImgLoadSeq == 0 )
  {
    ImageDB::ImageDBSearch::ImageSearch( m_pImgdbHeap, m_pImgdbHeap );

    //m_pImgdbLoad->RequestLoadPhoto_Snap( L"$_sdmc:/DCIM/100NIN03/HNI_0000.JPG", NULL );
    m_pImgdbLoad->RequestLoadPhoto_Snap( ImageDB::ImageDBSearch::m_pPictureInfo[0].m_path, NULL );
    m_ImgLoadSeq++;
  }
  if( m_ImgLoadSeq == 1 )
  {
    m_pImgdbLoad->Update();
  }
  if( m_ImgLoadSeq == 2 )
  {
    ImageDB::ImageDBSearch::DeletePictureInfo();
    m_ImgLoadSeq++;
  }
  if( m_ImgLoadSeq == 3 )
  {
    return true;
  }

  return false;
#else
  return true;
#endif
}



//----------------------------------------------------------------------
// @brief   リスナー関数群
//----------------------------------------------------------------------
void Frame::OnEndDecoTest( void )
{
  this->SetSubSeq( MM_SEQ_END );
}

void Frame::OnRequestShootAndSave( void )
{
#if defined(GF_PLATFORM_CTR)
  m_pImgdbSave->RequestPhotoShoot_Save( m_pAppRenderingManager );
#endif
}

void Frame::OnOpenDecoWindow( DecoItemKind type )
{
  if( !m_pLowerDecoIconDraw->IsOpen() )
  {
    m_pLowerDecoIconDraw->Open( type );
  }
  else if( type == m_pLowerDecoIconDraw->GetDecoType() )
  {
    m_pLowerDecoIconDraw->Close();
  }
  else {
    m_pLowerDecoIconDraw->ChangeDecoType( type );
  }
}

void Frame::OnUpdateTouchPoint( gfl2::math::VEC3 vec )
{
  m_pUpperDraw->UpdateTouchPoint( vec );
}


void Frame::OnAddDecoItem( void )
{
  m_pUpperDraw->AddDecoItem();
  m_pLowerDraw->AddDecoItem();
}

void Frame::OnRemoveDecoItem( void )
{
  m_pUpperDraw->RemoveDecoItem();
  m_pLowerDraw->RemoveDecoItem();
}

void Frame::OnSetDecoItem( u32 index )
{
  m_pUpperDraw->SetDecoItem( index );
  m_pLowerDraw->SetDecoItem( index );
}


#if defined(GF_PLATFORM_CTR)
void Frame::OnEndLoadSnap( void )
{
  m_ImgLoadSeq++;
}
#endif


GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // #if PM_DEBUG
