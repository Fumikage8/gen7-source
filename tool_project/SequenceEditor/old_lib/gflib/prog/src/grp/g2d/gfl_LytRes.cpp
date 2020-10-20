//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_LytRes.cpp
 *	@brief  レイアウトリソースクラス
 *	@author	tomoya takahashi
 *	@date		2010.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/g2d/gfl_LytRes.h>

#include <gfl_Math.h>
#include <gfl_Std.h>


namespace gfl {
namespace grp {
namespace g2d {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytBinary構造
 */
//-----------------------------------------------------------------------------
static const u32 LYT_BINARY_PATH_MAX = 64;  //パス名テーブルのサイズ
struct _LytBinaryHeader {
  u32   filenum;
  char  nametbl[];
};

//-----------------------------------------------------------------------------
/**
 *          gfl::grp::g2d::LytPaneBinary構造
 */
//-----------------------------------------------------------------------------
static const u32 LYT_PANE_BINARY_NAME_SHIFT_BIT = 5;                                 // ペイン名算出用ビット数
static const u32 LYT_PANE_BINARY_NAME_STR_MAX = (1<<LYT_PANE_BINARY_NAME_SHIFT_BIT); // ペイン名文字列最大数


struct _LytPaneBinaryHeader 
{
  u32   panenum;
  char  nametbl[];
};



//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
LytRes::LytRes( void )
{
  m_memory        = NULL;
  m_buff          = NULL;
  m_resAccessor   = NULL;
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i ){
    m_useDefaultFont[i]= false;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
LytRes::~LytRes()
{
  if( m_buff ){
    Release();
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み
 *
 *	@param  allocator アロケータ
 *  @param  p_buff          バッファポインタ
 *	@param  addTextureNum   追加登録　テクスチャ数
 */
//-----------------------------------------------------------------------------
void LytRes::Load( heap::NwAllocator* allocator, const void * cp_buff, u32 addTextureNum )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  s32 result;
  uptr addr;
  
  // 使用するバッファを設定
  m_buff = reinterpret_cast<const LytBinaryHeader*>(cp_buff);

  // データ解析を行い、文字列部とアーカイブデータ部に分ける。
  {
    //　まずは、ファイル名リスト
    m_nameTbl = m_buff->nametbl;
    addr = 4 + (m_buff->filenum * LYT_BINARY_PATH_MAX);

    // その次にペイン名リスト
    m_paneData = reinterpret_cast<LytPaneBinaryHeader*>( reinterpret_cast<uptr>(m_buff) + addr );
    addr += (4 + (m_paneData->panenum * LYT_PANE_BINARY_NAME_STR_MAX));

    
    addr = math::RoundUp(addr, 128);  // データは128byteアライメントされている。

    GFL_PRINT( "panenum %d addr = 0x%x\n", m_paneData->panenum, addr );
    m_arcData = reinterpret_cast<void*>( reinterpret_cast<uptr>(m_buff) + addr );
  }
  

  // リソースアクセサー生成
  m_resAccessor = GFL_NEW(heap) nw::lyt::ArcResourceAccessor();
  result = m_resAccessor->Attach( m_arcData, "." );
  GFL_ASSERT_STOP( result );


  // 追加登録テクスチャキー
  m_textureKeyNum = addTextureNum;
  if( m_textureKeyNum ){
    m_textureKeyArray = reinterpret_cast<nw::lyt::TextureKey*>GflHeapAllocMemory( heap, sizeof(nw::lyt::TextureKey) * m_textureKeyNum );
    m_addTextureNum   = 0;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（圧縮データ対応版）
 *
 *	@param  allocator アロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param	comp			圧縮フラグ　true = 圧縮データ
 *	@param  addTextureNum   追加登録　テクスチャ数
 */
//-----------------------------------------------------------------------------
void LytRes::Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, bool comp, u32 addTextureNum )
{
	if( comp == false ){
		Load( allocator, arcFile, dataID, addTextureNum );
	}else{
		LoadComp( allocator, arcFile, dataID, addTextureNum );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（非圧縮データ）
 *
 *	@param  allocator アロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param  addTextureNum   追加登録　テクスチャ数
 */
//-----------------------------------------------------------------------------
void LytRes::Load( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum )
{
  u32 size;
  s32 result;
  heap::HeapBase* heap = allocator->GetHeapBase();

  size = arcFile->GetDataSize( dataID );

  m_memory = GflHeapAllocMemoryAlign( heap, size, 128 );

  result = arcFile->LoadData( dataID, m_memory );
  GFL_ASSERT_STOP( result >= 0 );

  // 使用するバッファを設定
  Load( allocator, m_memory, addTextureNum );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの読み込み（圧縮データ）
 *
 *	@param  allocator アロケータ
 *	@param  arcFile   アーカイブファイル
 *	@param  dataID    データID
 *	@param  addTextureNum   追加登録　テクスチャ数
 */
//-----------------------------------------------------------------------------
void LytRes::LoadComp( heap::NwAllocator* allocator, fs::ArcFile* arcFile, u32 dataID, u32 addTextureNum )
{
	m_memory = arcFile->LoadData( dataID, allocator->GetHeapBase(), 128 );

  // 使用するバッファを設定
  Load( allocator, m_memory, addTextureNum );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースの破棄
 */
//-----------------------------------------------------------------------------
void LytRes::Release( void )
{
  // 追加登録テクスチャキーを外す
  if( m_textureKeyNum ){
    for( int i=0; i<m_addTextureNum; i++ ){
      m_resAccessor->UnregistTexture( m_textureKeyArray[i] );
    }
    
    GflHeapFreeMemory(m_textureKeyArray);
    m_addTextureNum = 0;
    m_textureKeyNum = 0;
    m_textureKeyArray = NULL;
  }
  
  // Defaultフォントを外す
  for( u32 i=0; i<LYTRES_DEFAULT_FONT_MAX; ++i ){
    if( m_useDefaultFont[i] ){
      m_resAccessor->UnregistFont( m_fontKey[i]);
      m_useDefaultFont[i]= false;
    }
  }
  
  // リソースアクセサー破棄
  GFL_DELETE m_resAccessor;
  // バッファ破棄
  if(m_memory){
    GflHeapFreeMemory(m_memory);
  }
  
  m_resAccessor = NULL;
  m_buff = NULL;
  m_memory = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  レイアウトリソースが読み込み済みかチェック
 *
 *	@retval true    読み込み済み
 *	@retval false   読み込まれていない
 */
//-----------------------------------------------------------------------------
bool LytRes::IsDataIn(void) const
{
  return (m_buff != NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ファイル数取得
 *
 *	@return ファイル数
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetFileNum( void ) const
{
  return m_buff->filenum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ファイル名取得
 *
 *	@param  lytDataID データID
 *
 *	@return ファイル名
 */
//-----------------------------------------------------------------------------
const char* LytRes::GetFileName( u32 lytDataID ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  return &m_nameTbl[str_offset];
}


//-----------------------------------------------------------------------------
/**
 *  @brief  目的のリソースを取得
 *
 *  @retval リソースポインタ  ない場合製品版ではNULLが戻る。
 */
//-----------------------------------------------------------------------------
void* LytRes::GetResource( u32 lytDataID ) const
{
  void* ret = NULL;
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;

  //GFL_PRINT( "name %s\n", &m_nameTbl[str_offset] );

  ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_LAYOUT, &m_nameTbl[str_offset], NULL );
  GFL_ASSERT( ret ); // 目的のリソースがない。 @check
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *      リソースブロックの取得
 */
//-----------------------------------------------------------------------------
void* LytRes::GetAnimeResource( u32 lytDataID ) const
{
  void* ret = NULL;
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;

  //GFL_PRINT( "name %s\n", &m_nameTbl[str_offset] );

  ret = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_ANIMATION, &m_nameTbl[str_offset], NULL );
  GFL_ASSERT( ret ); // 目的のリソースがない。 @check
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャをすべてメモリに配置する
 *
 *  @param  area    転送先メモリ　（FCRAM/VRAMA/VRAMB）
 */
//-----------------------------------------------------------------------------
void LytRes::TransferAllTexture( gfl::grp::MemoryArea area )
{
  int i;
  void* texres;
  nw::lyt::TextureInfo texInfo;
  u32 size;

  for( i=0; i<m_buff->filenum; i++ ){

    texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[i * LYT_BINARY_PATH_MAX], &size );
    if( texres && (size > 0) ){

      texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );
      if( texInfo.IsValid() ){
        m_resAccessor->RegistTexture( &m_nameTbl[i * LYT_BINARY_PATH_MAX], texInfo );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャをメモリに配置する
 *
 *  @param  texArcIndex   テクスチャレイアウトデータID
 *  @param  area          転送先メモリ　（FCRAM/VRAMA/VRAMB）
 */
//-----------------------------------------------------------------------------
void LytRes::TransferIndexTexture( u32 texArcIndex, gfl::grp::MemoryArea area )
{
  void* texres;
  nw::lyt::TextureInfo texInfo;
  u32 size;


  texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[texArcIndex * LYT_BINARY_PATH_MAX], &size );
  if( texres && (size > 0) ){

    texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );
    if( texInfo.IsValid() ){
      m_resAccessor->RegistTexture( &m_nameTbl[texArcIndex * LYT_BINARY_PATH_MAX], texInfo );
    }
  }
}



//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャリソースを取得
 *
 *  @param  lytDataID   レイアウトデータID
 *
 */
//-----------------------------------------------------------------------------
const LytResTextureInfo LytRes::GetTexture( u32 lytDataID ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  
  return m_resAccessor->GetTexture( &m_nameTbl[str_offset] );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  テクスチャリソースを取得
 *
 *  @param  size リソースのサイズを受け取る変数へのポインタです。
 *               サイズを受け取る必要がない場合は NULL を渡してください。
 *
 *  @retval テクスチャリソースの先頭アドレスへのポインタ
 *
 *  @note   使用例
 *          u32 size;
 *          void* buffer = LytRes::GetTextureRes( lytDataID, &size );
 *          nw::lyt::TexResource tex_resource(buffer, *size);
 */
//-----------------------------------------------------------------------------
void* LytRes::GetTextureRes( u32 lytDataID, u32* size ) const
{
  u32 str_offset = lytDataID * LYT_BINARY_PATH_MAX;
  void* texres;

  texres = m_resAccessor->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, &m_nameTbl[str_offset], size );
  if(texres && (*size > 0)){
    return texres;
  }

  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  リソースアクセサーの取得
 */
//-----------------------------------------------------------------------------
LytResourceAccessor* LytRes::GetResourceAccessor(void) const
{
  return m_resAccessor;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ペイン名の取得
 */
//-----------------------------------------------------------------------------
const char * LytRes::GetPaneName( u32 paneEnum ) const
{
  if( paneEnum >= m_paneData->panenum ){
    GFL_ASSERT( paneEnum < m_paneData->panenum ); //@check
    return NULL;
  }

  return &m_paneData->nametbl[paneEnum<<LYT_PANE_BINARY_NAME_SHIFT_BIT];  // @suzuki
}

//----------------------------------------------------------------------------
/**
 *	@brief  ペイン名インデックス数を取得
 *
 *	@return ペイン名インデックス数
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNum( void ) const
{
  return m_paneData->panenum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  名前のインデックスを取得
 *
 *	@retval インデックス
 *	@retval LYTRES_PANENAME_ERROR そんな名前ない
 */
//-----------------------------------------------------------------------------
u32 LytRes::GetPaneNameIndex( const char* name ) const
{ 
  int i;
  u32 str_offset = 0;
  int result;

  if( name ){
  
    for( i=0; i<m_paneData->panenum; i++ ){
      
      result = gfl::std::StringCmp( &m_paneData->nametbl[ str_offset ], name );
      if( result == 0 ){  //等しいならこのIndexを返す
        return i;
      }
      str_offset += LYT_PANE_BINARY_NAME_STR_MAX;
    }
  }

  return LYTRES_PANENAME_ERROR;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Defaultフォントを設定
 *
 *	@param	font    フォント
 *	@param  name    名称
 *	@param  index   インデックス(0～LYTRES_DEFAULT_FONT_MAX)
 */
//-----------------------------------------------------------------------------
void LytRes::SetDefaultFont( gfl::str::Font* font, const char* name, u32 index )
{
  if( index < LYTRES_DEFAULT_FONT_MAX ){
    if( m_useDefaultFont[index] == false ){

      //GFL_PRINT( "registFont1 [%x][%x][%x]\n", m_resAccessor, font, name );
      //GFL_PRINT( "registFont2 [%s]\n", name );

      m_fontKey[index] = m_resAccessor->RegistFont( name, font );
      m_useDefaultFont[index] = true;
      return ;
    }
    // 設定済み
    GFL_ASSERT(0); //@check
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  テクスチャを追加登録
 *
 *	@param	res     リソース
 */
//-----------------------------------------------------------------------------
void LytRes::RegistTexture( const LytRes* res, gfl::grp::MemoryArea area )
{
  u32 filenum = res->GetFileNum();
  nw::lyt::TextureInfo texInfo;
  const char* name;
  void* texres;
  u32 size;

  LytResourceAccessor * arc_resource = res->GetResourceAccessor();
  
  for( int i=0; i<filenum; i++ ){

    // 登録可能かチェック
    if( m_textureKeyNum > m_addTextureNum ){
      name    = res->GetFileName( i );

      texres = arc_resource->GetResource( nw::lyt::res::RESOURCETYPE_TEXTURE, name, &size );

      if( texres ){
        texInfo = nw::lyt::LoadTexture( texres, size, (area | GL_NO_COPY_FCRAM_DMP) );

        //GFL_PRINT( "registTexture %s\n", name );
        m_textureKeyArray[m_addTextureNum] = m_resAccessor->RegistTexture( name, texInfo );
        m_addTextureNum ++;
      }
    }else{
      // 登録バッファオーバー
      GFL_ASSERT(0); //@check
    }
  }
}



} // g2d
} // grp
} // gfl
