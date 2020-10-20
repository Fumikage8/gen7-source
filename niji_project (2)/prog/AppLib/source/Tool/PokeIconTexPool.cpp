// ============================================================================
/*
 * @file PokeIconTexPool.h
 * @brief ポケモンアイコン全データを丸ごと作成するクラスです。
 * @date 2015.12.16
 * @author yuto_uchida
 * @note
 *      ポケモンアイコンデータを丸ごと全部作成して、テクスチャーとして貼り付けることができます。
 *      メモリに余裕がある場合は使えます。
 */
// ============================================================================
#include "AppLib/include/Tool/PokeIconTexPool.h"

//  gflib2のインクルード
#include <debug/include/gfl2_Assert.h>
#include <cx/include/gfl2_StreamingUncomp.h>

//  nijiのインクルード
#include <System/include/ArcIdGetter.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/PokeIcon.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

  //  ポケモンアイコンtexデータ
  //  テクスチャーロードに必要なバッファ確保とtexinfoの管理をしている
  PokeIconTexData::PokeIconTexData( gfl2::heap::HeapBase* pHeap )
  {
    _Clear();

    u32 loopNum = GFL_NELEMS( m_aTexData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_aTexData[ i ].pTexBuffer  = PokeIconTexPool::CreateTextureBuffer( pHeap );
    }
  }

  PokeIconTexData::~PokeIconTexData()
  {
    u32 loopNum = GFL_NELEMS( m_aTexData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      _DeleteTexture( i );
      PokeIconTexPool::DeleteTextureBuffer( m_aTexData[ i ].pTexBuffer );
    }
  }

  //  作成したテクスチャーデータからペインに貼り付け
  bool PokeIconTexData::ReplectPaneTexture(
      gfl2::lyt::LytPicture*        pPane,
      u32                           materialIndex,
      u32                           textureIndex )
  {
    _DeleteTexture( m_nowDataIdx );

    // テクスチャ情報
    gfl2::lyt::LytResTextureInfo textureInfo;
    bool loadTexRes = nw::lyt::LoadTexture(&textureInfo, m_aTexData[ m_nowDataIdx ].pTexBuffer, m_aTexData[ m_nowDataIdx ].texSize );
    if(loadTexRes == false)
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャをデバイスにロードするのに失敗。
      return  false;
    }

    //  テクスチャーのロードが完了したので貼り付け
    m_aTexData[ m_nowDataIdx ].texInfo.Set( textureInfo );
    m_aTexData[ m_nowDataIdx ].bTexInfoAlive = true;

    m_nowDataIdx  = ( m_nowDataIdx + 1 ) % GFL_NELEMS( m_aTexData );

    // ペインに貼る
    bool replaceTexRes = app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
        &textureInfo,
        pPane,
        materialIndex,
        textureIndex
    );
    if( replaceTexRes == false )
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャを置き換えるのに失敗。
      return  false;
    }

    return  true;
  }

  void PokeIconTexData::_DeleteTexture( const u32 dataIdx )
  {
    if( m_aTexData[ m_nowDataIdx ].bTexInfoAlive == true )
    {
      //  すでにロードしているのでいったん削除
#if defined(GF_PLATFORM_WIN32)
        // @note DeleteTextureに相当するものがない
#elif defined(GF_PLATFORM_CTR)
        nw::lyt::DeleteTexture( m_aTexData[ m_nowDataIdx ].texInfo );  // 2回目なので前の後処理
#endif
    }
    m_aTexData[ m_nowDataIdx ].bTexInfoAlive = false;
  }

  //  テクスチャーバッファを作成
  //  あらかじめテクスチャーバッファを確保するのに使える
  void* PokeIconTexPool::CreateTextureBuffer( gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( pHeap );

    return  GflHeapAllocMemoryAlign( pHeap, app::tool::PokeIcon::TEX_SIZE, 128 );
  }

  //  テクスチャーデータバッファを破棄
  void PokeIconTexPool::DeleteTextureBuffer( void* pTexData )
  {
    GFL_ASSERT( pTexData );
    GflHeapSafeFreeMemory( pTexData );
  }

  PokeIconTexPool::PokeIconTexPool( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pRomAsyncFileManager )
  {
    GFL_ASSERT( pHeap );
    GFL_ASSERT( pRomAsyncFileManager );

    _Clear();
    //  すでにロードしているのでいったん削除

    u32 heapSize  = 1024 * 700;
#if defined(GF_PLATFORM_WIN32)
      heapSize  = 1024 * 700;
#elif defined(GF_PLATFORM_CTR)
      heapSize  = 1024 * 650;
#endif

    m_pHeap = GFL_CREATE_LOCAL_HEAP( pHeap, heapSize, gfl2::heap::HEAP_TYPE_EXP, false );

    m_pRomAsyncFileManager  = pRomAsyncFileManager;

    m_arcId = NIJI_ARCID_NUM( ARCID_POKEICON );
  }

  PokeIconTexPool::~PokeIconTexPool()
  {
    if( m_pHeap != NULL )
    {
      GFL_DELETE_HEAP( m_pHeap );
    }
  }

  //  初回処理
  //  true が返ってくるまで呼び出し続ける
  bool  PokeIconTexPool::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      //  ファイルオープン
      //  arcFileからdatIDを使ってテクスチャーに貼り付けるのでオープンのままにする
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId       = m_arcId;
      req.heapForFile = m_pHeap;
      //  断片化対策
      req.heapForReq  = m_pHeap->GetLowerHandle();
      m_pRomAsyncFileManager->AddArcFileOpenReq(req);

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
    {
      //  ファイルオープン待ち
      if( m_pRomAsyncFileManager->IsArcFileOpenFinished( m_arcId ) == true )
      {
        ++m_initSeq;
      }
    }

    if( m_initSeq == 2 )
    {
      u32 endDataId = GARC_PokeIcon_DATA_NUM -1;

      //  ロード開始
      const gfl2::fs::ArcFile*  pArcFile  = m_pRomAsyncFileManager->GetArcFile( m_arcId );
      GFL_ASSERT( pArcFile );

      size_t startOffsetFromArchiveTop = 0;
      pArcFile->GetOffsetFromArchiveTop( &startOffsetFromArchiveTop, 0 );

      size_t endOffsetFromArchiveTop = 0;
      size_t endDataSize = 0;
      {
        pArcFile->GetOffsetFromArchiveTop( &endOffsetFromArchiveTop, endDataId );

        // @memo 一気読みしているので、ArcSrcを使いたくない。fs側で防げないか。
        pArcFile->GetDataSize(&endDataSize, endDataId, m_pHeap->GetLowerHandle() );
      }

      size_t readSize = endOffsetFromArchiveTop + endDataSize - startOffsetFromArchiveTop;
      m_pTexBuffer = GflHeapAllocMemoryAlign( m_pHeap, readSize, 128 );

      gfl2::fs::AsyncFileManager::ArcFileLoadDataPieceBufReq  req;
      req.arcId             = m_arcId;
      req.datId             = 0;
      req.readSize          = readSize;
      req.pBuf              = m_pTexBuffer;
      req.bufSize           = readSize;
      req.heapForReq        = m_pHeap->GetLowerHandle();
      m_pRomAsyncFileManager->AddArcFileLoadDataPieceBufReq(req);

      ++m_initSeq;
    }

    if( m_initSeq == 3 )
    {
      //  ロード待ち
      if( m_pRomAsyncFileManager->IsArcFileLoadDataPieceBufFinished( m_pTexBuffer ) == true )
      {
        ++m_initSeq;
      }
    }

    if( m_initSeq == 4 )
    {
      //  準備完了
      m_bInit = true;
      return  true;
    }

    return  false;
  }

  //  終了処理
  //  true が返ってくるまで呼び出し続ける
  bool  PokeIconTexPool::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      //  クローズ開始
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId      = m_arcId;
      req.heapForReq = m_pHeap->GetLowerHandle();
      m_pRomAsyncFileManager->AddArcFileCloseReq(req);

      ++m_endSeq;
    }

    if( m_endSeq == 1 )
    {
      //  クロース待ち
      if( m_pRomAsyncFileManager->IsArcFileCloseFinished( m_arcId ) == true )
      {
        ++m_endSeq;
      }
    }

    if( m_endSeq == 2 )
    {
      GflHeapSafeFreeMemory( m_pTexBuffer );
      ++m_endSeq;
    }

    if( m_endSeq == 3 )
    {
      m_bEnd  = true;
      m_bInit = false;

      return  true;
    }

    return  false;
  }

  //  テクスチャーデータを作成
  void* PokeIconTexPool::CreateTexture(
      gfl2::heap::HeapBase*         pDevMem,
      gfl2::heap::HeapBase*         pTempMem,
      const PokeTool::SimpleParam&  rSp,
      bool                          bFlip
  )
  {
    GFL_ASSERT( pDevMem );
    GFL_ASSERT( pTempMem );

    if( m_bInit == false )
    {
      //  InitFuncで返り値がtrueになっていないのに呼んだ
      GFL_ASSERT( 0 );
      return  NULL;
    }

    gfl2::fs::ArcFile::ARCDATID datId = app::tool::PokeIcon::GetResourceIndex( &rSp, bFlip );

    void*                    dataBuf  = m_pTexBuffer;
    const gfl2::fs::ArcFile* pArcFile = m_pRomAsyncFileManager->GetArcFile( m_arcId );

    // オフセット
    size_t bufOffset  = 0;
    {
      size_t arcOffsetHead = 0;
      size_t arcOffsetCurr = 0;

      pArcFile->GetOffsetFromArchiveTop( &arcOffsetHead, 0 );
      pArcFile->GetOffsetFromArchiveTop( &arcOffsetCurr, datId );

      bufOffset = arcOffsetCurr - arcOffsetHead;
    }

    // 展開
    void* pUncompBuf = NULL;
    {
      // サイズ
      size_t realCompSize = 0; 
      pArcFile->GetRealDataSize( &realCompSize, datId, pTempMem );  // @memo 一気読みしたものを使っているので、ArcSrcを使いたくない。fs側で防げないか。

      // 圧縮データの先頭アドレス
      void* compBuf = reinterpret_cast<void*>( (reinterpret_cast<u32>(dataBuf)) + (static_cast<u32>(bufOffset)) );

      u32 uncompSize = gfl2::cx::GetUncompressedSize( compBuf, realCompSize );
      pUncompBuf = GflHeapAllocMemoryAlign( pDevMem, uncompSize, 128 );
      u32 uncompRes = gfl2::cx::SecureUncompress( compBuf, realCompSize, pUncompBuf, pTempMem );
      if(uncompRes > 0)
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。解凍エラー。
        DeleteTextureBuffer( pUncompBuf );
        return NULL;
      }
    }

    return pUncompBuf;
  }

  //  データクラスにテクスチャーデータを作成(SimpleParam版)
  bool PokeIconTexPool::CreatePokeData(
    PokeIconTexData*              pPokeIconTexData,
    gfl2::heap::HeapBase*         pTempMem,
    const PokeTool::SimpleParam&  rSp,
    bool                          bFlip )
  {
    GFL_ASSERT( pPokeIconTexData );
    GFL_ASSERT( pPokeIconTexData->m_aTexData[ pPokeIconTexData->m_nowDataIdx ].pTexBuffer );
    GFL_ASSERT( pTempMem );

    if( m_bInit == false )
    {
      //  InitFuncで返り値がtrueになっていないのに呼んだ
      GFL_ASSERT( 0 );
      return  false;
    }

    gfl2::fs::ArcFile::ARCDATID datId = app::tool::PokeIcon::GetResourceIndex( &rSp, bFlip );

    return  _CreatePokeData( pPokeIconTexData, datId, pTempMem );
  }

  //  データクラスにテクスチャーデータを作成(CoreParam版)
  bool PokeIconTexPool::CreatePokeData(
    PokeIconTexData*              pPokeIconTexData,
    gfl2::heap::HeapBase*         pTempMem,
    const pml::pokepara::CoreParam& rPp,
    bool                          bFlip )
  {
    gfl2::fs::ArcFile::ARCDATID datId = app::tool::PokeIcon::GetResourceIndex( &rPp, bFlip );

    return  _CreatePokeData( pPokeIconTexData, datId, pTempMem );
  }

  //  作成したテクスチャーデータからペインに貼り付け
  bool PokeIconTexPool::ReplectPaneTexture(
      void*                         pTexBuffer,
      size_t                        texSize,
      gfl2::lyt::LytPicture*        pPane,
      u32                           materialIndex,
      u32                           textureIndex,
      gfl2::lyt::LytResTextureInfo* pOutTextureInfo )
  {
    GFL_ASSERT( pTexBuffer );
    GFL_ASSERT( 0 < texSize );

    if( m_bInit == false )
    {
      //  InitFuncで返り値がtrueになっていないのに呼んだ
      GFL_ASSERT( 0 );
      return  false;
    }

    // テクスチャ情報
    gfl2::lyt::LytResTextureInfo textureInfo;
    bool loadTexRes = nw::lyt::LoadTexture(&textureInfo, pTexBuffer, texSize );
    if(loadTexRes == false)
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャをデバイスにロードするのに失敗。
      return  false;
    }

    // ペインに貼る
    bool replaceTexRes = app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
        &textureInfo,
        pPane,
        materialIndex,
        textureIndex
    );
    if(replaceTexRes == false)
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャを置き換えるのに失敗。
      return  false;
    }

    if( pOutTextureInfo != NULL )
    {
      pOutTextureInfo->Set( textureInfo );
    }

    return  true;
  }

  //  データクラスにテクスチャーデータを作成
  bool PokeIconTexPool::_CreatePokeData( PokeIconTexData* pOutPokeIconTexData, const gfl2::fs::ArcFile::ARCDATID datId, gfl2::heap::HeapBase* pTempMem )
  {
    void*                    dataBuf  = m_pTexBuffer;
    const gfl2::fs::ArcFile* pArcFile = m_pRomAsyncFileManager->GetArcFile( m_arcId );

    // オフセット
    size_t bufOffset  = 0;
    {
      size_t arcOffsetHead = 0;
      size_t arcOffsetCurr = 0;

      pArcFile->GetOffsetFromArchiveTop( &arcOffsetHead, 0 );
      pArcFile->GetOffsetFromArchiveTop( &arcOffsetCurr, datId );

      bufOffset = arcOffsetCurr - arcOffsetHead;
    }

    // 展開
    u32 uncompSize  = 0;
    {
      // サイズ
      size_t realCompSize = 0; 
      pArcFile->GetRealDataSize( &realCompSize, datId, pTempMem );  // @memo 一気読みしたものを使っているので、ArcSrcを使いたくない。fs側で防げないか。

      // 圧縮データの先頭アドレス
      void* compBuf = reinterpret_cast<void*>( (reinterpret_cast<u32>(dataBuf)) + (static_cast<u32>(bufOffset)) );

      uncompSize  = gfl2::cx::GetUncompressedSize( compBuf, realCompSize );
      u32 uncompRes = gfl2::cx::SecureUncompress( compBuf, realCompSize, pOutPokeIconTexData->m_aTexData[ pOutPokeIconTexData->m_nowDataIdx ].pTexBuffer, pTempMem );
      if(uncompRes > 0)
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。解凍エラー。
        return false;
      }

      pOutPokeIconTexData->m_aTexData[ pOutPokeIconTexData->m_nowDataIdx ].texSize = uncompSize;
    }

    return  true;
  }

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
