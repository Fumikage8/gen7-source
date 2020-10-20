//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl2_LytRes.cpp
 *  @brief  レイアウトリソースクラス
 *  @author tomoya takahashi
 *  @date   2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <layout/include/gfl2_LytRes.h>

#include <math/include/gfl2_math.h>
#include <util/include/gfl2_std_string.h>

GFL_WARNING_WIN32_IGNORE(4200)    //!< C4200を抑制

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//-----------------------------------------------------------------------------
/**
 *          定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          gfl::lyt::g2d::LytBinary構造
 */
//-----------------------------------------------------------------------------
static const u32 LYT_BINARY_PATH_MAX = 64;  //パス名テーブルのサイズ
struct _LytBinaryHeader {
  u32   filenum;
  char  nametbl[];
};

//-----------------------------------------------------------------------------
/**
 *          gfl::lyt::g2d::LytPaneBinary構造
 */
//-----------------------------------------------------------------------------
static const u32 LYT_PANE_BINARY_NAME_SHIFT_BIT = 5;                                 // ペイン名算出用ビット数
static const u32 LYT_PANE_BINARY_NAME_STR_MAX = (1<<LYT_PANE_BINARY_NAME_SHIFT_BIT); // ペイン名文字列最大数


struct _LytPaneBinaryHeader 
{
  u32   panenum;
  char  nametbl[];
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LytRes::LytRes(void)
  : m_resAccessor( NULL )
  , m_multiResource( NULL )
  , m_multiResMax( 0 )
  , m_multiResNum( 0 )
{
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i ){
    m_useDefaultFont[i]= false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LytRes::~LytRes()
{
  ReleaseMultiResource();
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースが読み込み済みかチェック
 *
 * @retval  true    読み込み済み
 * @retval  false   読み込まれていない
 */
//-----------------------------------------------------------------------------
bool LytRes::IsDataIn(void) const
{
  if( m_multiResource == NULL )
  {
    return false;
  }
  return ( m_multiResource[0].buff != NULL );
}

//-----------------------------------------------------------------------------
/**
 * @brief   リソースアクセサ初期設定
 *
 * @param   heap            ヒープ
 * @param   multi_res_max   アクセサが保持できるアーカイブの数
 */
//-----------------------------------------------------------------------------
void LytRes::InitMultiResource( gfl2::heap::HeapBase * heap, u32 multi_res_max )
{
  GFL_ASSERT_STOP( multi_res_max );

  if( m_multiResource != NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_multiResource = GFL_NEW_ARRAY(heap) MultiResource[multi_res_max];
  m_multiResMax = multi_res_max;
  m_multiResNum = 0;

  for( u32 i=0; i<m_multiResMax; i++ )
  {
    m_multiResource[i].memory   = NULL;
    m_multiResource[i].buff     = NULL;
    m_multiResource[i].paneData = NULL;
    m_multiResource[i].nameTbl  = NULL;
    m_multiResource[i].link     = GFL_NEW(heap) nw::lyt::ArcResourceLink();

    m_multiResource[i].textureInfoArray    = NULL;
    m_multiResource[i].addTextureInfoArray = NULL;
    m_multiResource[i].addTextureMax = 0;
    m_multiResource[i].addTextureNum = 0;
  }

  m_resAccessor = GFL_NEW(heap) LytResourceAccessor();
}

//-----------------------------------------------------------------------------
/**
 * @brief   リソースアクセサ削除
 */
//-----------------------------------------------------------------------------
void LytRes::ReleaseMultiResource(void)
{
  // Defaultフォントを外す
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i )
  {
    if( m_useDefaultFont[i] )
    {
      m_resAccessor->UnregistFont( m_fontKey[i] );
      m_useDefaultFont[i]= false;
    }
  }

  if( m_multiResource != NULL )
  {
    for( u32 i=0; i<m_multiResMax; i++ )
    {
      MultiResource * res = &m_multiResource[i];

      // 追加登録テクスチャの登録を解除
      if( res->addTextureInfoArray != NULL )
      {
        for( u32 j=0; j<res->addTextureNum; j++ )
        {
          m_resAccessor->UnregistTexture( res->addTextureInfoArray[j] );
        }
        GflHeapFreeMemory( res->addTextureInfoArray );
      }
      // 通常テクスチャの登録を解除
      if( res->textureInfoArray != NULL )
      {
        for( u32 j=0; j<res->buff->filenum; j++ )
        {
          if( res->textureInfoArray[j] != NULL )
          {
            m_resAccessor->UnregistTexture( res->textureInfoArray[j] );
          }
        }
        GflHeapFreeMemory( res->textureInfoArray );
      }

      m_resAccessor->Detach( res->link );
      GFL_DELETE res->link;

      if( res->memory != NULL )
      {
        GflHeapFreeMemory( res->memory );
      }
    }
    GFL_DELETE_ARRAY m_multiResource;
    m_multiResource = NULL;

    GFL_DELETE m_resAccessor;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   リソースアクセサ取得
 */
//-----------------------------------------------------------------------------
LytResourceAccessor * LytRes::GetResourceAccessor(void) const
{
  return m_resAccessor;
}

//-----------------------------------------------------------------------------
/**
 * @brief   リソースセット
 *
 * @param   allocator     ヒープアロケータ
 * @param   cp_buff       リソースデータ
 * @param   addTxtureNum  追加登録するテクスチャの数
 */
//-----------------------------------------------------------------------------
void LytRes::SetResource( gfl2::heap::NwAllocator * allocator, const void * cp_buff, u32 addTextureNum )
{
  gfl2::heap::HeapBase* heap = allocator->GetHeapBase();

  MultiResource * res = &m_multiResource[m_multiResNum];

  // 使用するバッファを設定
  res->buff = reinterpret_cast<const LytBinaryHeader *>(cp_buff);

  // データ解析を行い、文字列部とアーカイブデータ部に分ける。
  // まずは、ファイル名リスト
  res->nameTbl = res->buff->nametbl;
  uptr addr = 4 + res->buff->filenum * LYT_BINARY_PATH_MAX;
  // その次にペイン名リスト
  res->paneData = reinterpret_cast<LytPaneBinaryHeader *>( reinterpret_cast<uptr>(res->buff) + addr );
  addr += ( 4 + res->paneData->panenum * LYT_PANE_BINARY_NAME_STR_MAX );
    
  addr = math::RoundUp( addr, 128 );  // データは128byteアライメントされている。

  GFL_PRINT( "panenum %d addr = 0x%x\n", res->paneData->panenum, addr );
  void * arc = reinterpret_cast<void *>( reinterpret_cast<uptr>(res->buff) + addr );

  bool result = res->link->Set( arc, "." );
  GFL_ASSERT_STOP( result );
  m_resAccessor->Attach( res->link );

  // テクスチャ情報
  if( res->buff->filenum != 0 )
  {
    res->textureInfoArray = static_cast<LytResTextureInfo**>( GflHeapAllocMemoryAlign( heap, res->buff->filenum*4, 4 ) );
    for( u32 i=0; i<res->buff->filenum; i++ )
    {
      res->textureInfoArray[i] = NULL;
    }
  }
  // 追加登録テクスチャキー
  res->addTextureMax = addTextureNum;
  if( res->addTextureMax != 0 )
  {
    res->addTextureInfoArray = static_cast<LytResTextureInfo**>( GflHeapAllocMemoryAlign( heap, res->addTextureMax*4, 4 ) );
    res->addTextureNum = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースの読み込み（データ指定版）
 *
 * @param   allocator     ヒープアロケータ
 * @param   cp_buff       リソースデータ
 * @param   addTxtureNum  追加登録するテクスチャの数
 *
 * @return  リソース登録番号
 */
//-----------------------------------------------------------------------------
u32 LytRes::Load( gfl2::heap::NwAllocator * allocator, const void * cp_buff, u32 addTextureNum )
{
  if( m_multiResource == NULL )
  {
    InitMultiResource( allocator->GetHeapBase(), 1 );
  }

  GFL_ASSERT_STOP( m_multiResNum < m_multiResMax );

  SetResource( allocator, cp_buff, addTextureNum );

  u32 ret = m_multiResNum;

  m_multiResNum++;

  return ret;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースの読み込み
 *
 * @param    pAsyncFileMan  ファイルマネージャ
 * @param   allocator     ヒープアロケータ
 * @param   arcFile       アーカイブファイル
 * @param   arcID         アーカイブIDarc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*))
 * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
 * @param   comp          圧縮フラグ ( true = 圧縮データ )
 * @param   addTextureNum 追加登録するテクスチャの数
 *
 * @return  リソース登録番号
 */
//-----------------------------------------------------------------------------
u32 LytRes::Load( gfl2::fs::AsyncFileManager* pAsncFileManager, gfl2::heap::NwAllocator* allocator, gfl2::fs::ArcFile::ARCDATID arcID, gfl2::fs::ArcFile::ARCDATID dataID, bool comp, u32 addTextureNum )
{
  if( m_multiResource == NULL )
  {
    InitMultiResource( allocator->GetHeapBase(), 1 );
  }

  GFL_ASSERT_STOP( m_multiResNum < m_multiResMax );

  if( comp == false )
  {
    LoadNormal( pAsncFileManager, allocator, arcID, dataID, addTextureNum );
  }
  else
  {
    LoadComp( pAsncFileManager, allocator, arcID, dataID, addTextureNum );
  }

  u32 ret = m_multiResNum;

  m_multiResNum++;

  return ret;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースの読み込み（非圧縮データ）
 *
 * @param    pAsyncFileMan  ファイルマネージャ
 * @param   allocator     ヒープアロケータ
 * @param   arcFile       アーカイブファイル
 * @param   arcID         アーカイブIDarc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*))
 * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
 * @param   addTextureNum 追加登録するテクスチャの数
 *
 * @note    Open,Closeは使用する側で行ってください
 */
//-----------------------------------------------------------------------------
void LytRes::LoadNormal( gfl2::fs::AsyncFileManager* pAsyncFileMan, gfl2::heap::NwAllocator * allocator, gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID dataID, u32 addTextureNum )
{
  gfl2::heap::HeapBase* heap = allocator->GetHeapBase();
  gfl2::heap::HeapBase* pMemHeapForTemp = heap->GetLowerHandle();   // @fix GFCHECK[8373]

  MultiResource * res = &m_multiResource[m_multiResNum];

  gfl2::fs::Result result;
  u32 size = 0;
  //u32 data_size = 0;

  const gfl2::fs::ArcFile * arcFile = pAsyncFileMan->GetArcFile( arcID );
  result = arcFile->GetDataSize( &size, dataID, NULL );
  GFL_ASSERT_STOP( result.IsSuccess() );

  res->memory = GflHeapAllocMemoryAlign( heap, size, 128 );

  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
  req.arcId      = arcID;
  req.datId      = dataID;
  req.pBuf       = res->memory;
  req.heapForCompressed = heap->GetLowerHandle();   // @fix GFCHECK[8373]
  req.heapForReq = pMemHeapForTemp;

  pAsyncFileMan->SyncArcFileLoadDataBuf( req );

  SetResource( allocator, res->memory, addTextureNum );
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースの読み込み（圧縮データ）
 *
 * @param    pAsyncFileMan  ファイルマネージャ
 * @param   allocator     ヒープアロケータ
 * @param   arcFile       アーカイブファイル
 * @param   arcID         アーカイブIDarc_tool.lstファイル内に書かれているID(*.garcファイル自体のID(ARCID_*))
 * @param   dataID        アーカイブデータID(*.gaixファイル内に書かれているID(GARC_*))
 * @param   addTextureNum 追加登録するテクスチャの数
 *
 * @note    Open,Closeは使用する側で行ってください
 */
//-----------------------------------------------------------------------------
void LytRes::LoadComp( gfl2::fs::AsyncFileManager* pAsyncFileMan, gfl2::heap::NwAllocator * allocator, gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID dataID, u32 addTextureNum )
{
  gfl2::heap::HeapBase* heap = allocator->GetHeapBase();
  gfl2::heap::HeapBase* pMemHeapForTemp = heap;

  MultiResource * res = &m_multiResource[m_multiResNum];

  //u32 buf_size = 0;
  //u32 read_size = 0;

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
//  req.compressed = true;
  req.arcId      = arcID;
  req.datId      = dataID;
  req.ppBuf      = &res->memory;
  req.heapForBuf = heap;
  req.align      = 128;//4;
  req.heapForReq = pMemHeapForTemp;

  pAsyncFileMan->SyncArcFileLoadData( req );

  SetResource( allocator, res->memory, addTextureNum );
}

//-----------------------------------------------------------------------------
/**
 * @brief   リソース登録数を取得
 *
 * @return  登録数
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetMultiResNum(void) const
{
  return m_multiResNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャをすべてメモリに配置する
 *
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytRes::TransferAllTexture(void)
{
  for( u32 i=0; i<m_multiResNum; i++ )
  {
    MultiResource * res = &m_multiResource[i];
    for( u32 j=0; j<res->buff->filenum; j++ )
    {
      TransferIndexTexture( i, j );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャをメモリに配置する
 *
 * @param   multi_id    リソース登録ID
 * @param   texArcIndex テクスチャレイアウトデータID
 *
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytRes::TransferIndexTexture( u32 multi_id, u32 texArcIndex )
{
  MultiResource * res = &m_multiResource[multi_id];

  if( res->buff->filenum <= texArcIndex )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( res->textureInfoArray[texArcIndex] != NULL )
  {
    GFL_ASSERT( 0 );  // すでに読み込まれている
    return;
  }

  u32 size;
  void * texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &res->nameTbl[texArcIndex * LYT_BINARY_PATH_MAX], &size );

  if( texres && ( size > 0 ) )
  {
    res->textureInfoArray[texArcIndex] = m_resAccessor->RegistTexture( &res->nameTbl[texArcIndex * LYT_BINARY_PATH_MAX] );
    if( res->textureInfoArray[texArcIndex] == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }
    bool ret = nw::lyt::LoadTexture( res->textureInfoArray[texArcIndex], texres, size );
    if( ret == false )
    {
      GFL_ASSERT( 0 );
      m_resAccessor->UnregistTexture( res->textureInfoArray[texArcIndex] );
      res->textureInfoArray[texArcIndex] = NULL;
      return;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャを追加する
 *
 * @param   dst_multi_id  追加先のリソース登録ID
 * @param   src_res       追加元のリソースデータ
 * @param   src_multi_id  追加元のリソース登録ID
 *
 * @li  レイアウトに設定されたメモリを使用する
 */
//-----------------------------------------------------------------------------
void LytRes::RegistTexture( u32 dst_multi_id, const LytRes * src_res, u32 src_multi_id )
{
  MultiResource * dst_res = &m_multiResource[dst_multi_id];

  u32 filenum = src_res->GetFileNum( src_multi_id );

  LytResourceAccessor * src_accessor = src_res->GetResourceAccessor();
  
  for( u32 i=0; i<filenum; i++ )
  {
    // 登録可能かチェック
    if( dst_res->addTextureNum < dst_res->addTextureMax )
    {
      u32 size;
      const char * name = src_res->GetFileName( i, src_multi_id );
      void * texres = src_accessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, name, &size );

      if( texres )
      {
        dst_res->addTextureInfoArray[dst_res->addTextureNum] = m_resAccessor->RegistTexture( name );
        if( dst_res->addTextureInfoArray[dst_res->addTextureNum] == NULL )
        {
          GFL_ASSERT( 0 );
          return;
        }
        bool ret = nw::lyt::LoadTexture( dst_res->addTextureInfoArray[dst_res->addTextureNum], texres, size );
        if( ret == false )
        {
          GFL_ASSERT( 0 );
          m_resAccessor->UnregistTexture( dst_res->addTextureInfoArray[dst_res->addTextureNum] );
          return;
        }
        dst_res->addTextureNum++;
      }
    }
    else
    {
      // 登録バッファオーバー
      GFL_ASSERT( 0 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル数取得
 *
 * @param   multi_id  リソース登録ID
 *
 * @return  ファイル数
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetFileNum( u32 multi_id ) const
{
  if( multi_id >= m_multiResNum )
  {
    GFL_ASSERT( 0 );
    return 0;
  }
  return m_multiResource[multi_id].buff->filenum;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ファイル名取得
 *
 * @param   lytDataID   データID
 * @param   multi_id    リソース登録ID
 *
 * @return  ファイル名
 */
//-----------------------------------------------------------------------------
const char * LytRes::GetFileName( u32 lytDataID, u32 multi_id ) const
{
  if( multi_id >= m_multiResNum )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  return &m_multiResource[multi_id].nameTbl[str_offset];
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースを取得
 *
 * @param   lytDataID   データID
 * @param   multi_id    リソース登録ID
 *
 * @return  レイアウトリソース ( ない場合はNULL )
 */
//-----------------------------------------------------------------------------
void * LytRes::GetLayoutResource( u32 lytDataID, u32 multi_id ) const
{
  const char* pResFileName  = GetFileName(lytDataID,multi_id);
  void * ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_LAYOUT, pResFileName, NULL );
  GFL_ASSERT_MSG( ret, "ResourceFile find Not = %s \n", pResFileName ); // 目的のリソースがない。 @check

  return ret;
}

//-----------------------------------------------------------------------------
/**
 * @brief   アニメリソースを取得
 *
 * @param   lytDataID   データID
 * @param   multi_id    リソース登録ID
 *
 * @return  アニメリソース ( ない場合はNULL )
 */
//-----------------------------------------------------------------------------
void * LytRes::GetAnimeResource( u32 lytDataID, u32 multi_id ) const
{
  void * ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_ANIMATION, GetFileName(lytDataID,multi_id), NULL );
  GFL_ASSERT( ret ); // 目的のリソースがない。 @check
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャリソースを取得
 *
 * @param   lytDataID   データID
 * @param   size        テクスチャサイズ格納場所 ( 不要の場合はNULL )
 * @param   multi_id    リソース登録ID
 *
 * @return  テクスチャリソース ( ない場合はNULL )
 *
 * @note    使用例
 *          u32 size;
 *          void* buffer = LytRes::GetTextureRes( lytDataID, &size, multi_id );
 *          nw::lyt::TexResource tex_resource(buffer, *size);
 */
//-----------------------------------------------------------------------------
void * LytRes::GetTextureResource( u32 lytDataID, u32 * size, u32 multi_id ) const
{
  void * tex = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, GetFileName(lytDataID,multi_id), size );
  if( tex == NULL )
  {
    return NULL;
  }
  if( size != NULL )
  {
    if( *size == 0 )
    {
      return NULL;
    }
  }
  return tex;
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャ情報を取得
 *
 * @param   lytDataID   データID
 * @param   multi_id    リソース登録ID
 *
 * @return  テクスチャ情報
 */
//-----------------------------------------------------------------------------
const LytResTextureInfo * LytRes::GetTexture( u32 lytDataID, u32 multi_id ) const
{
  return m_resAccessor->GetTexture( GetFileName(lytDataID,multi_id) );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペイン名の取得
 *
 * @param   paneEnum    ペイン番号
 * @param   multi_id    リソース登録ID
 *
 * @return  ペイン名
 */
//-----------------------------------------------------------------------------
const char * LytRes::GetPaneName( u32 paneEnum, u32 multi_id ) const
{
  MultiResource * res = &m_multiResource[multi_id];

  if( paneEnum >= res->paneData->panenum )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }

  return &res->paneData->nametbl[paneEnum<<LYT_PANE_BINARY_NAME_SHIFT_BIT];
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペイン名インデックス数を取得
 *
 * @param   multi_id  リソース登録ID
 *
 * @return  ペイン名インデックス数
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNum( u32 multi_id ) const
{
  return m_multiResource[multi_id].paneData->panenum;
}

//-----------------------------------------------------------------------------
/**
 * @brief  名前のインデックスを取得
 *
 * @param   name      名前
 * @param   multi_id  リソース登録ID
 *
 * @retval  インデックス
 * @retval  LYTRES_PANENAME_ERROR そんな名前ない
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNameIndex( const char * name, u32 multi_id ) const
{ 
  MultiResource * res = &m_multiResource[multi_id];

  if( name )
  {
    u32 str_offset = 0;

    for( u32 i=0; i<res->paneData->panenum; i++ )
    {
      if( gfl2::std::StringCmp( &res->paneData->nametbl[str_offset], name, LYT_PANE_BINARY_NAME_STR_MAX ) == 0 )
      {
        return i;
      }
      str_offset += LYT_PANE_BINARY_NAME_STR_MAX;
    }
  }

  return LYTRES_PANENAME_ERROR;
}

//-----------------------------------------------------------------------------
/**
 * @brief   Defaultフォントを設定
 *
 * @param   font    フォント
 * @param   name    名称
 * @param   index   インデックス ( 0 〜 LYTRES_DEFAULT_FONT_MAX )
 */
//-----------------------------------------------------------------------------
void LytRes::SetDefaultFont( gfl2::str::Font * font, const char * name, u32 index )
{
  if( index < LYTRES_DEFAULT_FONT_MAX )
  {
    if( m_useDefaultFont[index] == false )
    {
      //GFL_PRINT( "registFont1 [%x][%x][%x]\n", m_resAccessor, font, name );
      //GFL_PRINT( "registFont2 [%s]\n", name );
      m_fontKey[index] = m_resAccessor->RegistFont( name, font );
      m_useDefaultFont[index] = true;
      return;
    }
    // 設定済み
    GFL_ASSERT_MSG( 0, "設定済みです。グループ内で１回やればokです。\n" );
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャーをアンロードする
 * @param   LytTextureInfo*    テクスチャー情報のポインタ
 */
//-----------------------------------------------------------------------------

void LytRes::UnLoad(LytResTextureInfo* pInfo)
{
#if defined(GF_PLATFORM_CTR)
  nw::lyt::DeleteTexture(*pInfo);
#endif
}



GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)
