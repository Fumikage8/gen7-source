#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestSearch.cpp
 * @date    2016/10/24 13:34:39
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)

#include "ImageDBTestSearch.h"

#include <Test/ImageDBTest/source/util/ImageDBTestUtility.h>
#include <Test/ImageDBTest/source/debug/ImageDBTestTick.h>

#include <AppLib/include/Util/app_util_heap.h>
// product
#include "GameSys/include/GameManager.h"

#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

ImageDBPictureInfo* ImageDBSearch::m_pPictureInfo = NULL;
u32 ImageDBSearch::m_PhotoNum = 0;

void ImageDBSearch::CreatePictureInfo( gfl2::heap::HeapBase* heap, u32 num )
{
  // 破棄していない場合は一旦破棄しておく
  DeletePictureInfo();

  m_pPictureInfo = GFL_NEW( heap ) ImageDBPictureInfo[num];
  GFL_ASSERT( m_pPictureInfo );
}

void ImageDBSearch::DeletePictureInfo( void )
{
  GFL_SAFE_DELETE_ARRAY( m_pPictureInfo );
}


void ImageDBSearch::ImageSearch( gfl2::heap::HeapBase* workHeap, gfl2::heap::HeapBase* infoHeap )
{
  DebugTick::StartTick( TICK_ID_IMGDB_SEARCH );

  gfl2::heap::HeapBase* sysHeap = workHeap->GetLowerHandle();
  {
    ImageDBAccessor* accessor = ImageDBUtil::CreateImageDBAccessor(sysHeap);
    accessor->Initialize();

    ImageDBSearcher* searcher = GFL_NEW( sysHeap ) ImageDBSearcher();
    searcher->Initialize();

    // SDカードの写真枚数を取得
    m_PhotoNum = searcher->GetResultNum( GameSys::GameManager::APLICATION_UNIQUE_ID );

    CreatePictureInfo( infoHeap, m_PhotoNum );

    ImageDBSearcher::ResultCode resCode = ImageDBSearcher::E_RET_SUCCESS;
    resCode = searcher->GetInfo( 0, m_PhotoNum, m_pPictureInfo, GameSys::GameManager::APLICATION_UNIQUE_ID );

    if( resCode != ImageDBSearcher::E_RET_SUCCESS )
    {
      GFL_PRINT( "ImageDB Search Error!! resCode[%d]\n", resCode );
      GFL_ASSERT(0);
    }
    else{
      GFL_PRINT( "Search Result\n" );

      for( int i = 0; i < m_PhotoNum; ++i )
      {
        GFL_PRINT( "%03d : path[%ls]\n", i+1, m_pPictureInfo[i].m_path );
      }

      GFL_PRINT( "PhotoMax=[%d]\n", m_PhotoNum );
    }

    searcher->Finalize();
    GFL_DELETE( searcher );

    accessor->Finalize();
    GFL_DELETE( accessor );
  }

  DebugTick::EndTick( TICK_ID_IMGDB_SEARCH );
  DebugTick::DumpTick( TICK_ID_IMGDB_SEARCH, "ImageDBSearch" );
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // PM_DEBUG
