//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_DrawLytTexture.cpp
 *	@brief  テクスチャへの書き込み
 *	@author	Toru=Nagihashi
 *	@date		2012.10.24
 *
 *  @caution niji プロジェクトでは未使用（デバッグされていない） 内部で nngx 使う箇所もあるので注意
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "AppLib/include/Tool/app_tool_DrawLytTexture.h"

#include <Layout/include/gfl2_Layout.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

static  inline  s32 calc_ver_index( s32 now_index, s32 value, s32 index_max );

//-----------------------------------------------------------------------
// 初期化・破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
DrawLytTexture::DrawLytTexture( gfl2::heap::HeapBase* device_heap, u32 buffer_size, u32 buffer_align )
{
  m_heap  = device_heap;
  m_tex_data  = GFL_NEW( m_heap ) LytTexData( device_heap, buffer_size, buffer_align );
}
/**
 * @brief デストラクタ
 */
DrawLytTexture::~DrawLytTexture()
{
  if( m_write_buffer )
  {
    GflHeapFreeMemory( m_write_buffer );
    m_write_buffer = NULL;
  }
  if( m_src_buffer )
  {
    GflHeapFreeMemory( m_src_buffer );
    m_src_buffer = NULL;
  }
  GFL_SAFE_DELETE( m_tex_data );
}
//-----------------------------------------------------------------------
// 操作
//-----------------------------------------------------------------------
/**
 * @brief テクスチャデータを読み込む
 * @param heap_for_work      読み込みの作業用ワーク領域に使用するヒープ
 * @param file_read_manager  ファイル読み込み制御クラス
 * @param data_id            読み込むテクスチャデータを指定
 * @param compressed         true: 圧縮されている, false: 圧縮されていない
 */
void DrawLytTexture::LoadTexture(
    gfl2::heap::HeapBase* heap_for_work,
    gfl2::fs::AsyncFileManager *file_read_manager,
    gfl2::fs::ArcFile::ARCDATID data_id,
    bool compressed )
{
  m_tex_data->LoadTexture( heap_for_work, data_id, compressed );
  
  if( m_write_buffer )
  {
    GflHeapFreeMemory( m_write_buffer );
    m_write_buffer = NULL;
  }
  if( m_src_buffer )
  {
    GflHeapFreeMemory( m_src_buffer );
    m_src_buffer = NULL;
  }
  
  m_write_buffer  = GflHeapAllocMemoryAlign( m_heap, m_tex_data->GetTexSize(), 16 );
  m_src_buffer = GflHeapAllocMemoryAlign( m_heap, m_tex_data->GetTexSize(), 16 );
  

#if PM_DEBUG
#if GFL_DEBUG_PRINT
  {
    const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();
    nw::lyt::TexFormat format = m_tex_data->GetTexInfo().GetFormat();
    
    GFL_PRINT( "     w %d h %d\n", size.width, size.height );
//    GFL_PRINT( "real w %d h %d\n", r_size.width, r_size.height );
    GFL_PRINT( "format %d\n", format );
  }
#endif
#endif

  
  //テクスチャの内容を保存しておく
  {
#if defined( GF_PLATFORM_CTR )
    const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();
    nngxAddB2LTransferCommand( m_tex_data->GetTexData(), size.width, size.height, GL_RGBA4, m_src_buffer,     size.width,     size.height,     GL_RGBA4,  NN_GX_ANTIALIASE_NOT_USED,  GL_FALSE,  8 );
#endif
  }
  
  this->Clear();
}


/**
 * @brief 書いたデータをテクスチャに転送
 * 
 * @param target_pane 転送先のピクチャペイン
 * @param material_index  差し替えるマテリアルのインデックス(通常は0。複数存在するなら使用）
 * @param texture_index   差し替えるテクスチャのインデックス(通常は0。複数存在するなら使用）
 * 
 * @note  転送後２F後に反映します。１F目コマンドでメモリに配置、２F目ダブルバッファにて反映
 */
void DrawLytTexture::TransferTexture( gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index )
{
  GFL_ASSERT_STOP( material_index < target_pane->GetMaterialNum() );  // material_index が不正 @fix テクスチャ書き換えなので、
                                                                      //あらぬアドレスへのアクセスが怖いので、STOPに変更
  gfl2::lyt::LytMaterial* material = target_pane->GetMaterial( material_index );
  GFL_ASSERT_STOP( texture_index < material->GetTexMapNum() );  // texture_index が不正 @fix テクスチャ書き換えなので、
                                                                //あらぬアドレスへのアクセスが怖いので、STOPに変更

  material->SetTexMap( texture_index, &m_tex_data->GetTexInfo() );

#if defined( GF_PLATFORM_CTR )
  {
    const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();

    //flush
    nngxUpdateBuffer( m_write_buffer, m_tex_data->GetTexSize() );
  
    //Linerをblockにして転送
    nngxAddL2BTransferCommand( m_write_buffer,  m_tex_data->GetTexData(),     size.width,     size.height,     GL_RGBA4,     8 );
  }
#endif
}
//-----------------------------------------------------------------------
// 書き込み
//-----------------------------------------------------------------------
/**
 * @brief 点を書き込み
 *
 * @param vec 点座標
 */
void DrawLytTexture::WritePoint( const gfl2::math::VEC2 & vec, const gfl2::math::VEC4 & color )
{
  const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();

#if defined( GF_PLATFORM_WIN32 )
  const nw::lyt::TexSize & r_size = m_tex_data->GetTexInfo().GetSize();
#elif defined( GF_PLATFORM_CTR )
  const nw::lyt::TexSize & r_size = m_tex_data->GetTexInfo().GetRealSize();
#endif

  nw::lyt::TexFormat format = m_tex_data->GetTexInfo().GetFormat();

  u32 x = gfl2::math::Clamp<u16>( static_cast<u16>(vec.x), static_cast<u16>(0), static_cast<u16>(r_size.width-1) );
  u32 y = gfl2::math::Clamp<u16>( static_cast<u16>(vec.y), static_cast<u16>(0), static_cast<u16>(r_size.height-1) );

  u16 *src_data =  static_cast<u16*>(m_write_buffer);
  src_data  += static_cast<u32>( y * size.width );
  src_data  += static_cast<u32>( x );

  GFL_ASSERT_MSG( format == nw::lyt::TEXFORMAT_RGBA4, "format=%d\n", format );  //@check ここは使用方法の間違いアサートなのでケアしない

  *src_data = this->Rgb( color );
}
/**
 * @brief 線を書き込み
 *
 * @param start 開始点
 * @param end   終了点
 */
void DrawLytTexture::WriteLine( const gfl2::math::VEC2 & start, const gfl2::math::VEC2 & end, const gfl2::math::VEC4 & color )
{
  s32 x1  = static_cast<s32>( start.x );
  s32 y1  = static_cast<s32>( start.y );
  s32 x2  = static_cast<s32>( end.x );
  s32 y2  = static_cast<s32>( end.y );

  //線分を書く式(プレゼンハム)
   int i;

  //二点間の距離
  s32 dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
  s32 dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

  //二点間の方向
  s32 sx = ( x2 > x1 ) ? 1 : -1;
  s32 sy = ( y2 > y1 ) ? 1 : -1;

  // 傾きが1より小さい場合
  if ( dx > dy ) {
    int E = -dx;
    for ( i = 0 ; i <= dx ; i++ ) {
      gfl2::math::VEC2 vec;
      vec.x = x1;
      vec.y = y1;
      this->WritePoint( vec, color );
      x1 += sx;
      E += 2 * dy;
      if ( E >= 0 ) {
        y1 += sy;
        E -= 2 * dx;
      }
    }
  // 傾きが1以上の場合
  } else {
    int E = -dy;
    for ( i = 0 ; i <= dy ; i++ ) {
      gfl2::math::VEC2 vec;
      vec.x = x1;
      vec.y = y1;
      this->WritePoint( vec, color );
      y1 += sy;
      E += 2 * dx;
      if ( E >= 0 ) {
        x1 += sx;
        E -= 2 * dy;
      }
    }
  }

}


/**
 * @brief 矩形塗りつぶし
 *
 * @param left_top      左上点
 * @param right_bottom  右下点
 * @param color         色
 */
void DrawLytTexture::FillRect( const gfl2::math::VEC2 & left_top, const gfl2::math::VEC2 & right_bottom, const gfl2::math::VEC4 & color )
{
  const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();

  nw::lyt::TexFormat format = m_tex_data->GetTexInfo().GetFormat();
  GFL_ASSERT_MSG( format == nw::lyt::TEXFORMAT_RGBA4, "format=%d\n", format );  //@check 使用方法
 
  
  u16 height_start  = left_top.y < right_bottom.y ? left_top.y: right_bottom.y;
  u16 height_end    = left_top.y < right_bottom.y ? right_bottom.y: left_top.y;
  u16 x             = left_top.x < right_bottom.x ? left_top.x: right_bottom.x;
  u16 width         = gfl2::math::Abs(left_top.x - right_bottom.x );
  
  for( int h = height_start; h < height_end; ++h )
  { 
    for( int w = x; w < x + width; ++w )
    { 
      u16 *src_data =  static_cast<u16*>(m_write_buffer);
      src_data  += static_cast<u32>( h * size.width );
      src_data  += static_cast<u32>( w );
      
      *src_data = this->Rgb( color );

      //memfill16がない・・・
      u16 *dst_data = static_cast<u16*>(m_write_buffer);
      dst_data  += static_cast<u32>( h * size.width );
      dst_data  += static_cast<u32>( x + width );
    
      gfl2::std::MemFill( src_data, 0, sizeof(u16)*width );
    }
  } 
}

/**
 * @brief X方向にずらす
 *
 * @param start       座標
 * @param width       幅
 * @param disntance   距離
 */
void DrawLytTexture::ShiftX( u32 start, u32 width, s32 disntance )
{
  const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();
// ビルドを通すため
#if defined( GF_PLATFORM_WIN32 )
  const nw::lyt::TexSize & r_size = m_tex_data->GetTexInfo().GetSize();
#elif defined( GF_PLATFORM_CTR )
  const nw::lyt::TexSize & r_size = m_tex_data->GetTexInfo().GetRealSize();
#endif
// ビルドを通すため

  nw::lyt::TexFormat format = m_tex_data->GetTexInfo().GetFormat();
  
  
  s16 dst = static_cast<s16>(start) + static_cast<s16>(disntance);
  u32 dst_pos = gfl2::math::Clamp( dst, static_cast<s16>(0), static_cast<s16>(r_size.width-1) );
  
  if( dst < 0 )
  {
    width += dst;
    start -= dst;
  }
  
  u32 src_pos = gfl2::math::Clamp( static_cast<u16>(start), static_cast<u16>(0), static_cast<u16>(r_size.width-1) );

  GFL_ASSERT_MSG( format == nw::lyt::TEXFORMAT_RGBA4, "format=%d\n", format );  //@check 使用方法

  for( int i = 0; i < r_size.height; ++i )
  { 
    u16 *src_data =  static_cast<u16*>(m_write_buffer);
    src_data  += static_cast<u32>( i * size.width );
    src_data  += static_cast<u32>( src_pos );
    
    u16 *dst_data = static_cast<u16*>(m_write_buffer);
    dst_data  += static_cast<u32>( i * size.width );
    dst_data  += static_cast<u32>( dst_pos );
    
    gfl2::std::MemCopy( src_data, dst_data, sizeof(u16)*width );
  } 
}

/**
 * @brief 頂点テーブルから多角形を描画
 *
 * @param ver_tbl   頂点テーブル
 * @param ver_cnt   頂点数
 * @param color     色
 * @param is_paint  塗りつぶすかどうか（true:塗りつぶす）
 * @param is_clear  キャンバスをクリアしてから描画するか（true:クリアする）
 *
 */
void DrawLytTexture::WritePolygon( const gfl2::math::VEC2* ver_tbl, s32 ver_cnt, const gfl2::math::VEC4& color, bool is_paint, bool is_clear )
{
  if( ver_cnt < 3 )
  { 
    //三角形以上じゃないとアサート
    GFL_ASSERT( 0 );  //@fix  なにもしない
    return;
  }

  const nw::lyt::TexSize& size = m_tex_data->GetTexInfo().GetSize();
  if( is_clear != false )
  { 
    gfl2::std::MemCopy( m_src_buffer, m_write_buffer, size.width * size.height * sizeof(u16) );
  }

  //一番高い頂点を抽出
  s32 higher_ver = 0;
  f32 higher_y   = ver_tbl[ 0 ].y;
  for( s32 i = 0 ; i < ver_cnt ; i++ )
  { 
    if( higher_y > ver_tbl[ i ].y )
    { 
      higher_y   = ver_tbl[ i ].y;
      higher_ver = i;
    }
  }
  //分岐の左右を判別
  s32 left_root;
  s32 right_root;
  { 
    s32 ver_1 = calc_ver_index( higher_ver,  1, ver_cnt );
    s32 ver_2 = calc_ver_index( higher_ver, -1, ver_cnt );
    //線が重なっているならアサート（多角形にならない）
    GFL_ASSERT( ver_tbl[ ver_1 ].x != ver_tbl[ ver_2 ].x ); //@check  データのミスを見つけるためのアサートなので、
                                                            //挙動には問題なし
    if( ver_tbl[ ver_1 ].x > ver_tbl[ ver_2 ].x )
    { 
      left_root  = -1;
      right_root =  1;
    }
    else
    { 
      left_root  =  1;
      right_root = -1;
    }
  }

  WritePolygonParam* wpp = ( WritePolygonParam* )GflHeapAllocMemoryLow( m_heap, sizeof( WritePolygonParam ) );
  wpp->vertex = ( VertexParam* )GflHeapAllocMemoryLow( m_heap, sizeof( VertexParam ) * ver_cnt );

  wpp->search_line = higher_y;

  s32 left_ver  = higher_ver;
  s32 right_ver = higher_ver;
  //左右ルートを探索して線分描画パラメータを初期化
  for( s32 i = 0 ; i < ver_cnt ; i++ )
  { 
    s32 next_ver;
    //左ルート
    next_ver = calc_ver_index( left_ver, left_root, ver_cnt );
    //線分が水平か、水平から下に向いているものだけをイネーブルにする
    if( ver_tbl[ left_ver ].y <= ver_tbl[ next_ver ].y )
    { 
      wpp->vertex[ left_ver ].bp[ BRANCH_L ].enable = true;
      InitLineParam( &wpp->vertex[ left_ver ].bp[ BRANCH_L ].lp, &ver_tbl[ left_ver ], &ver_tbl[ next_ver ] );
      wpp->vertex[ left_ver ].boot_line = wpp->vertex[ left_ver ].bp[ BRANCH_L ].lp.y1;
    }
    else
    { 
      wpp->vertex[ left_ver ].bp[ BRANCH_L ].enable = false;
    }
    left_ver = next_ver;

    //右ルート
    next_ver = calc_ver_index( right_ver, right_root, ver_cnt );
    //線分が水平から下に向いているものだけをイネーブルにする（水平は左ルートでイネーブルにしているので除外）
    if( ver_tbl[ right_ver ].y < ver_tbl[ next_ver ].y )
    { 
      wpp->vertex[ right_ver ].bp[ BRANCH_R ].enable = true;
      InitLineParam( &wpp->vertex[ right_ver ].bp[ BRANCH_R ].lp, &ver_tbl[ right_ver ], &ver_tbl[ next_ver ] );
      wpp->vertex[ right_ver ].boot_line = wpp->vertex[ right_ver ].bp[ BRANCH_R ].lp.y1;
    }
    else
    { 
      wpp->vertex[ right_ver ].bp[ BRANCH_R ].enable = false;
    }
    right_ver = next_ver;
  }

  //線分描画
  bool result;
  do
  { 
    result = false;
    //頂点起動チェック
    for( s32 i = 0 ; i < ver_cnt ; i++ )
    { 
      if( wpp->vertex[ i ].enable == false )
      { 
        if( wpp->vertex[ i ].boot_line == wpp->search_line )
        { 
          wpp->vertex[ i ].enable = true;
        }
      }
      if( wpp->vertex[ i ].enable != false )
      { 
        bool enable_result = false;
        if( wpp->vertex[ i ].bp[ BRANCH_L ].enable != false )
        { 
          wpp->vertex[ i ].bp[ BRANCH_L ].checked = false;
          wpp->vertex[ i ].bp[ BRANCH_L ].enable = CalcLineParam( &wpp->vertex[ i ].bp[ BRANCH_L ].lp, color );
          enable_result |= wpp->vertex[ i ].bp[ BRANCH_L ].enable;
        }
        if( wpp->vertex[ i ].bp[ BRANCH_R ].enable != false )
        { 
          wpp->vertex[ i ].bp[ BRANCH_R ].checked = false;
          wpp->vertex[ i ].bp[ BRANCH_R ].enable = CalcLineParam( &wpp->vertex[ i ].bp[ BRANCH_R ].lp, color );
          enable_result |= wpp->vertex[ i ].bp[ BRANCH_R ].enable;
        }
        wpp->vertex[ i ].enable = enable_result;
      }
      result |= wpp->vertex[ i ].enable;
    }
    if( is_paint != false )
    { 
      bool  paint_result;
      do
      { 
        paint_result = false;
        s32 left_x     = size.width;
        s32 right_x    = size.width;
        s32 lefter     =  0;
        s32 right_near =  0;
        for( s32 i = 0 ; i < ver_cnt ; i++ )
        { 
          //一番左を抽出
          if( ( wpp->vertex[ i ].enable != false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_L ].enable  != false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_L ].checked == false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_L ].lp.x1 < left_x ) )
          { 
            left_x = wpp->vertex[ i ].bp[ BRANCH_L ].lp.x1;
            lefter = i;
          }
          //一番近い右を抽出
          if( ( wpp->vertex[ i ].enable != false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_R ].enable  != false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_R ].checked == false ) &&
              ( wpp->vertex[ i ].bp[ BRANCH_R ].lp.x1 < right_x ) )
          { 
            right_x = wpp->vertex[ i ].bp[ BRANCH_R ].lp.x1;
            right_near = i;
          }
        }
        if( ( left_x != size.width ) && ( right_x != size.width ) )
        { 
          wpp->vertex[ lefter ].bp[ BRANCH_L ].checked = true;
          wpp->vertex[ right_near ].bp[ BRANCH_R ].checked = true;
          WriteLine( gfl2::math::VEC2( wpp->vertex[ lefter ].bp[ BRANCH_L ].lp.x1,
                                      wpp->vertex[ lefter ].bp[ BRANCH_L ].lp.y1 ),
                    gfl2::math::VEC2( wpp->vertex[ right_near ].bp[ BRANCH_R ].lp.x1,
                                      wpp->vertex[ right_near ].bp[ BRANCH_R ].lp.y1 ),
                    color );
          paint_result = true;
        }
        else
        { 
          paint_result = false;
        }
      }while( paint_result != false );
    }
    wpp->search_line++;
  }while( result != false );

  GflHeapFreeMemory( wpp->vertex );
  GflHeapFreeMemory( wpp );

}

/**
 * @brief 何も書かない状態にする
 */
void DrawLytTexture::Clear( void )
{
  const nw::lyt::TexSize & size = m_tex_data->GetTexInfo().GetSize();
  gfl2::std::MemClear( m_src_buffer, size.width * size.height * sizeof(u16) );
}


/**
 * @brief 色情報をRGBに変換する
 *
 * @param color 色
 *
 * @return RGB
 */
u32 DrawLytTexture::Rgb( const gfl2::math::VEC4 & color )
{
  nw::lyt::TexFormat format = m_tex_data->GetTexInfo().GetFormat();
  GFL_ASSERT_MSG( format == nw::lyt::TEXFORMAT_RGBA4, "format=%d\n", format );  //@check 使用方法
  
  return ( (static_cast<u16>(color.x * 0xF) & 0xF) << 12 )
    | ( (static_cast<u16>(color.y*0xF) & 0xF) << 8 )
    | ( (static_cast<u16>(color.z*0xF) & 0xF) << 4 )
    | ( (static_cast<u16>(color.w*0xF) & 0xF) );
}

/**
 * @brief LineParam初期化
 *
 * @param[out]  LineParam
 * @param[in]   start   線分の開始点
 * @param[in]   end     線分の終了点
 */
void  DrawLytTexture::InitLineParam( LineParam* lp, const gfl2::math::VEC2* start, const gfl2::math::VEC2* end )
{ 
  lp->x1 = static_cast<s32>( start->x );
  lp->y1 = static_cast<s32>( start->y );
  lp->x2 = static_cast<s32>( end->x );
  lp->y2 = static_cast<s32>( end->y );

  //線分を書く式(プレゼンハム)
  lp->dis = 0;

  //二点間の距離
  lp->dx = ( lp->x2 > lp->x1 ) ? lp->x2 - lp->x1 : lp->x1 - lp->x2;
  lp->dy = ( lp->y2 > lp->y1 ) ? lp->y2 - lp->y1 : lp->y1 - lp->y2;

  //二点間の方向
  lp->sx = ( lp->x2 > lp->x1 ) ? 1 : -1;
  lp->sy = ( lp->y2 > lp->y1 ) ? 1 : -1;

  // 傾きが1より小さい場合
  if( lp->dx > lp->dy )
  {
    lp->E = -lp->dx;
  }
  // 傾きが1以上の場合
  else
  {
    lp->E = -lp->dy;
  }
}

/**
 * @brief LineParam計算をして線を描画
 *
 * @param[out]  LineParam
 * @param[in]   color     描画線分のカラー
 *
 * @retval  true:描画中　false:描画終了
 */
bool  DrawLytTexture::CalcLineParam( LineParam* lp, const gfl2::math::VEC4& color )
{ 
  bool  result = true;

  // 傾きが1より小さい場合
  if( lp->dx > lp->dy )
  {
    while( lp->dis <= lp->dx )
    {
      lp->dis++;
      gfl2::math::VEC2 vec = gfl2::math::VEC2( lp->x1, lp->y1 );
      this->WritePoint( vec, color );
      lp->x1 += lp->sx;
      lp->E += 2 * lp->dy;
      if ( lp->E >= 0 ) {
        lp->y1 += lp->sy;
        lp->E -= 2 * lp->dx;
        break;
      }
    }
    if( lp->dis > lp->dx ) result = false;
  }
  // 傾きが1以上の場合
  else
  {
    lp->dis++;
    gfl2::math::VEC2 vec = gfl2::math::VEC2( lp->x1, lp->y1 );
    this->WritePoint( vec, color );
    lp->y1 += lp->sy;
    lp->E += 2 * lp->dx;
    if ( lp->E >= 0 ) {
      lp->x1 += lp->sx;
      lp->E -= 2 * lp->dy;
    }
    if( lp->dis > lp->dy ) result = false;
  }

  return result;
}

static  inline  s32 calc_ver_index( s32 now_index, s32 value, s32 index_max )
{ 
  now_index += value;

  if( now_index >= index_max ) now_index = 0;
  if( now_index < 0 )          now_index = index_max - 1;

  return now_index;
}

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
