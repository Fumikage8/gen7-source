//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DisplayBufferManager.cpp
 *	@brief  ディスプレイバッファマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/gfl_DisplayBufferManager.h>
#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>
#include <gfl_Macros.h>

namespace gfl {
namespace grp {

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
//-------------------------------------
/// ディスプレイバッファークラス
//=====================================
class DisplayBuffer
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(DisplayBuffer);

public:
  
  // コンストラクタ/デストラクタ
  DisplayBuffer(void);
  ~DisplayBuffer();
  

  void Initialize( const DisplayBufferSetUp& setup );
  void Finalize( void );

  void ActivateBuffer(void);
  void ActivateBuffer(gfl::grp::DisplayType display_type);

  void TransferRenderImage( const RenderTarget& renderTarget );

  const DisplayBufferSetUp& GetSetupData(void) const;
   
  bool IsInitialized(void) const;

private:
  bool    m_initialized;
  GLuint  m_bufferID;
  DisplayBufferSetUp m_setup;
};



//=============================================================================
/**
 *					ディスプレイバッファマネージャ実装
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *
 *  @param  allocator アロケータ
 *	@param	arrayMax  配列最大数
 */
//-----------------------------------------------------------------------------
DisplayBufferManager::DisplayBufferManager( heap::NwAllocator* allocator, u32 arrayMax )
{
  Initialize( allocator, arrayMax );
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
DisplayBufferManager::~DisplayBufferManager()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief    初期化処理
 *
 *	@param	  allocator アロケータ
 *	@param    arrayMax  配列最大数
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::Initialize( heap::NwAllocator* allocator, u32 arrayMax )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  
  if( m_array == NULL ){
    GFL_ASSERT(m_array == NULL); //@check
    Finalize();
  }
  
  m_array = GFL_NEW_ARRAY(heap) DisplayBuffer[arrayMax];
  m_arrayMax = arrayMax;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄処理
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::Finalize( void )
{
  if( m_array != NULL ){
    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
    m_arrayMax = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイバッファの確保
 *
 *	@param	setup   登録情報
 *
 *	@retval ディスプレイバッファID
 *	@retval DISPLAYBUFFER_ID_ERROR    確保失敗
 */
//-----------------------------------------------------------------------------
DisplayBufferID DisplayBufferManager::CreateBuffer( const DisplayBufferSetUp& setup )
{
  DisplayBufferID id;

  // 空いているワーク取得
  id = GetClearBuffer();
  
  // 空いていない可能性がある。
  if( id == DISPLAYBUFFER_ID_ERROR ){
    return DISPLAYBUFFER_ID_ERROR;
  }

  // バッファの初期化
  m_array[id].Initialize( setup );
  
  return id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイバッファの破棄
 *  
 *	@param	displayBufferID  ディスプレイバッファID
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::DeleteBuffer( DisplayBufferID displayBufferID )
{
  if( ( displayBufferID < m_arrayMax ) )
  {
    m_array[displayBufferID].Finalize();
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    ディスプレイバッファをアクティブ状態にする
 *
 *	@param	  displayBufferID ディスプレイバッファ
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::ActivateBuffer( DisplayBufferID displayBufferID )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].ActivateBuffer();
  }
  else
  {
    GFL_ASSERT(0);
  }
}
void DisplayBufferManager::ActivateBuffer( DisplayBufferID displayBufferID, gfl::grp::DisplayType display_type )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].ActivateBuffer(display_type);
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファのカラー情報を転送
 *
 *  @param  graphics          グラフィックス
 *	@param  displayBufferID   ディスプレイバッファID  
 *	@param	renderTarget      カラーの転送元となるレンダーターゲット
 */
//-----------------------------------------------------------------------------
void DisplayBufferManager::TransferRenderImage( DisplayBufferID displayBufferID, const RenderTarget& renderTarget )
{
  if( displayBufferID < m_arrayMax )
  {
    m_array[displayBufferID].TransferRenderImage( renderTarget );
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  セットアップ情報の取得
 *
 *	@param	displayBufferID   ディスプレイバッファＩＤ
 *
 *	@return セットアップ情報
 */
//-----------------------------------------------------------------------------
const DisplayBufferSetUp& DisplayBufferManager::GetSetupData( DisplayBufferID displayBufferID ) const
{
  if( displayBufferID < m_arrayMax )
  {
    return m_array[displayBufferID].GetSetupData();
  }
  else
  {
    GFL_ASSERT(0);
    return m_array[0].GetSetupData();
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  空いているディスプレイバッファの取得
 *
 *	@retval 空いているバッファID
 *	@retval DISPLAYBUFFER_ID_ERROR    確保失敗
 */
//-----------------------------------------------------------------------------
DisplayBufferID DisplayBufferManager::GetClearBuffer( void ) const
{
  DisplayBufferID ret = DISPLAYBUFFER_ID_ERROR;

  // 空いているワーク取得
  for( int id=0; id<m_arrayMax; id++ ){
    if( m_array[id].IsInitialized() == false ){
      ret = id; 
      break;
    }
  }

  GFL_ASSERT( ret != DISPLAYBUFFER_ID_ERROR ); //@check

  return ret;
}


//=============================================================================
/**
 *					ディスプレイバッファ実装
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
DisplayBuffer::DisplayBuffer(void)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
DisplayBuffer::~DisplayBuffer()
{
  Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	  セットアップデータ
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::Initialize( const DisplayBufferSetUp& setup )
{
  GLuint display;
  
  GFL_ASSERT( m_initialized == false );

  // 初期化情報の保存
  m_setup = setup;
  
  // 初期化面決定
  if( setup.displayType == DISPLAY_UPPER ){
    display = NN_GX_DISPLAY0;
  }else if( setup.displayType == DISPLAY_UPPER_RIGHT ){
    display = NN_GX_DISPLAY0_EXT;
  }else if( setup.displayType == DISPLAY_LOWER ){
    display = NN_GX_DISPLAY1;
  }else{
    // おかしい
    GFL_ASSERT( 0 );
    display = NN_GX_DISPLAY0;
  }

  // カラーフォーマットがRGBA8以外である必要がある。
  GFL_ASSERT( setup.colorFormat != RENDER_COLOR_FORMAT_RGBA8 );

  // そのディスプレイをアクティブ状態にする。
  nngxActiveDisplay(display);

  // ディスプレイIDの取得
  nngxGenDisplaybuffers(1, &m_bufferID);

  // ディスプレイバッファの確保
  nngxBindDisplaybuffer(m_bufferID);
  nngxDisplaybufferStorage(
      setup.colorFormat,
      setup.height,
      setup.width,
      setup.memArea);
  
  nngxDisplayEnv( setup.displayOffsetY, setup.displayOffsetX );

  GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );


  nngxBindDisplaybuffer(0); // デフォに戻す
  
  m_initialized = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄処理
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::Finalize( void )
{
  if( m_initialized ){
    //
    nngxDeleteDisplaybuffers( 1, &m_bufferID ); 
    GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );

    m_initialized = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    ディスプレイバッファをアクティブ状態にする
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::ActivateBuffer(void)
{
  this->ActivateBuffer( m_setup.displayType );
}
void DisplayBuffer::ActivateBuffer(gfl::grp::DisplayType display_type)
{
  GLuint display;

  // 初期化面決定
  if( display_type == DISPLAY_UPPER ){
    display = NN_GX_DISPLAY0;
  }else if( display_type == DISPLAY_UPPER_RIGHT ){
    display = NN_GX_DISPLAY0_EXT;
  }else if( display_type == DISPLAY_LOWER ){
    display = NN_GX_DISPLAY1;
  }else{
    // おかしい
    GFL_ASSERT( 0 );
    display = NN_GX_DISPLAY0;
  }

  nngxActiveDisplay(display);
  nngxBindDisplaybuffer(m_bufferID);
  nngxDisplayEnv( m_setup.displayOffsetY, m_setup.displayOffsetX );
  GFL_ASSERT_MSG( glGetError() == GL_NO_ERROR, "code = %d", glGetError() );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カラーバッファの内容をディスプレイバッファに転送
 *
 *	@param	  フレームバッファインスタンス
 */
//-----------------------------------------------------------------------------
void DisplayBuffer::TransferRenderImage( const RenderTarget& renderTarget )
{
  GFL_ASSERT(m_initialized);

  GLuint mode = 0;
  GLint addr;
  GLint block_mode;
  int block;

  ActivateBuffer();

  if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_NOT_USED)
  {
    mode = NN_GX_ANTIALIASE_NOT_USED;
  }
  else if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_2x1)
  {
    mode = NN_GX_ANTIALIASE_2x1;
  }
  else if (m_setup.transMode == TRANSFER_MODE_ANTIALIASE_2x2)
  {
    mode = NN_GX_ANTIALIASE_2x2;
  }

  nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &addr);

  // ブロックモード取得
  glGetIntegerv( GL_RENDER_BLOCK_MODE_DMP, &block_mode );
  if( block_mode == GL_RENDER_BLOCK8_MODE_DMP ){
    block = 8;
  }else{
    block = 32;
  }

  u32 offset_size = 4;
  switch( renderTarget.GetRenderColorFormat() ){
  case RENDER_COLOR_FORMAT_RGBA8:
    offset_size = 4;
    break;

  case RENDER_COLOR_FORMAT_RGB8:
    offset_size = 3;
    break;

  case RENDER_COLOR_FORMAT_RGBA4:
  case RENDER_COLOR_FORMAT_RGB5_A1:
  case RENDER_COLOR_FORMAT_RGB565:
    offset_size = 2;
    break;

  default:
    GFL_ASSERT(0);
    break;
  }

  offset_size = offset_size * renderTarget.GetHeight() * (renderTarget.GetWidth() - m_setup.width);

  // ブロックイメージのレンダーバッファをリニアイメージのディスプレイバッファに転送
  nngxAddB2LTransferCommand( 
      reinterpret_cast<GLvoid*>(renderTarget.GetColorAddress() + (offset_size)),  // srcaddr
      renderTarget.GetHeight(),          // srcwidth
      renderTarget.GetWidth(),         // srcheight
      static_cast<GLenum>(renderTarget.GetRenderColorFormat()),    // srcカラーフォーマット
      reinterpret_cast<GLvoid*>(addr),    // dstaddr
      m_setup.height,                    // dstwidth
      m_setup.width,                   // dstheight
      m_setup.colorFormat,              // dstcolor_format
      mode,                             // アンチエイリアスモード
      m_setup.isTransferFlipX,          // Yフリップ
      block                             // ブロックモード
      );

  GFL_GRPGL_ERROR_CHECK();

  nngxSplitDrawCmdlist();
}

//----------------------------------------------------------------------------
/**
 *	@brief  設定データを取得
 */
//-----------------------------------------------------------------------------
const DisplayBufferSetUp& DisplayBuffer::GetSetupData(void) const
{
  return m_setup;
}


//----------------------------------------------------------------------------
/**
 *	@brief  初期化済みチェック
 *
 *	@retval true  初期化済み
 *	@retval false 初期化されていない
 */
//-----------------------------------------------------------------------------
bool DisplayBuffer::IsInitialized(void) const
{
  return m_initialized;
}


} /* end namespace grp */
} /* end namespace gfl */
