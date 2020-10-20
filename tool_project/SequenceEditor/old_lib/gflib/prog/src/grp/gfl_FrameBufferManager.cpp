//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FrameBufferManager.cpp
 *	@brief  フレームバッファマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>

#include <grp/gfl_FrameBufferManager.h>
#include <grp/gfl_RenderTarget.h>

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

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief    コンストラクタ
 */
//-----------------------------------------------------------------------------
FrameBufferManager::FrameBufferManager( heap::NwAllocator* allocator, u32 arrayMax )
{
  this->Initialize( allocator, arrayMax );
}


//----------------------------------------------------------------------------
/**
 *	@brief    デストラクタ
 */
//-----------------------------------------------------------------------------
FrameBufferManager::~FrameBufferManager()
{
  this->Finalize();
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファ　生成
 *
 *  @param    allocator アロケーター
 *	@param    setup	　  セットアップ情報
 *
 *	@return フレームバッファID
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::CreateBuffer( heap::NwAllocator* allocator, const RenderBufferSetUp& setup )
{
  RenderTarget::Description l_description;

  GFL_ASSERT( m_array != NULL );

  // 空いているIDを取得
  FrameBufferID id = this->GetClearBuffer();
  if( id == FRAMEBUFFER_ID_ERROR ){
    return id;
  }

  l_description.type    = RenderTarget::TYPE_ON_SCREEN_BUFFER;
  l_description.width   = setup.width;
  l_description.height  = setup.height;
  l_description.color_format = setup.colorFormat;
  l_description.depth_format = setup.depthFormat;
  l_description.color_area = setup.colorArea;
  l_description.depth_area = setup.depthArea;
  l_description.display_type = setup.targetDisplay;

  // shareフレームバッファのアドレスを指定
  if( setup.shareBuffer != DISPLAY_NONE ){

    // その描画タイプのフレームバッファがあるか？
    FrameBufferID display_frameID = this->GetDisplayTypeBuffer(setup.shareBuffer);
    GFL_ASSERT_STOP(display_frameID != FRAMEBUFFER_ID_ERROR);

    // そのアドレスで初期化する。
    l_description.color_address = m_array[display_frameID].GetColorAddress();
    l_description.depth_address = m_array[display_frameID].GetDepthAddress();
    l_description.fob_id        = m_array[display_frameID].GetFboID();
  }

  // 初期化
  m_array[id].Create( allocator, allocator, &l_description );

  // ViewPortを設定。(値があったら。）
  if( (setup.left != setup.right) && (setup.top != setup.bottom) ){
    m_array[id].SetViewport( setup.left, setup.top, setup.right, setup.bottom );
  }

  return id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファ　破棄
 *
 *	@param	id    フレームバッファID
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::DeleteBuffer( FrameBufferID id )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].Destroy();
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファをカレントに設定
 *
 *	@param	  id  フレームバッファＩＤ
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Bind( FrameBufferID id )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].ActivateBuffer();
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファをクリアする
 *
 *	@param	  id  フレームバッファＩＤ
 *	@param    clearColoer クリアカラー設定
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::BindAndClear( FrameBufferID id, const FrameBufferClearColor& clearColor, const FrameBufferClearFlag& clearFlag )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].ActivateBuffer();
      m_array[id].ClearBuffer(
          clearFlag.f_color,
          gfl::grp::ColorF32(clearColor.red, clearColor.green, clearColor.blue, clearColor.alpha),
          clearFlag.f_depth,
          clearColor.depth,
          clearFlag.f_stencil,
          clearColor.stencil);
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの情報をメモリにコピー
 *
 *	@param	id      フレームバッファID
 *	@param  type    バッファタイプ
 *	@param  memory  コピー先アドレス
 *
 *	*内部でカレントバッファを変更します。
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::OutputImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].OutputImage( type, memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  メモリからフレームバッファへ情報をコピー
 *
 *  @param  id      フレームバッファID
 *  @param  type    バッファタイプ
 *  @param  memory  コピー元アドレス
 *
 *  *内部でカレントバッファを変更します。
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::InputImage( FrameBufferID id, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].InputImage( memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  フレームバッファの情報をメモリにコピー (Linerフォーマット）
 *
 *	@param	id      フレームバッファID
 *	@param  type    バッファタイプ
 *	@param  memory  コピー先アドレス
 *
 *	*内部でカレントバッファを変更します。
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::OutputB2LImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory )
{
  if( id < m_arrayMax )
  {
    if( m_array[id].IsCreated() ){
      m_array[id].OutputB2LImage( type, memory );
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  レンダーターゲットの取得
 *
 *	@param	id      フレームバッファID
 *
 *	@return IDに対応したレンダーターゲット取得
 */
//-----------------------------------------------------------------------------
gfl::grp::RenderTarget* FrameBufferManager::GetRenderTarget( FrameBufferID id ) const
{
  if(id < m_arrayMax){
    if( m_array[id].IsCreated() ){
      return &m_array[id];
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	allocator アロケータ
 *	@param	arrayMax  配列最大数
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Initialize( heap::NwAllocator* allocator, u32 arrayMax )
{
  heap::HeapBase* heap = allocator->GetHeapBase();
  GFL_ASSERT( m_array == NULL ); //@check

  m_array = GFL_NEW_ARRAY(heap) RenderTarget[arrayMax];
  m_arrayMax = arrayMax;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 */
//-----------------------------------------------------------------------------
void FrameBufferManager::Finalize( void )
{
  if(m_array){

    GFL_DELETE_ARRAY m_array;
    m_array = NULL;
    m_arrayMax = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  空いているフレームバッファIDを取得
 *
 *	@retval フレームバッファID
 *	@retval FRAMEBUFFER_ID_ERROR 確保できない。
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::GetClearBuffer( void ) const
{
  FrameBufferID ret = FRAMEBUFFER_ID_ERROR;

  for( int i=0; i<m_arrayMax; ++i ){
    if( !m_array[i].IsCreated() ){
      ret = i;
      break;
    }
  }

  GFL_ASSERT( ret != FRAMEBUFFER_ID_ERROR ); //@check

  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ディスプレイタイプのフレームバッファIDを取得
 *
 *	@param	displayType   ディスプレイタイプ
 *
 *	@return フレームバッファID
 */
//-----------------------------------------------------------------------------
FrameBufferID FrameBufferManager::GetDisplayTypeBuffer( DisplayType displayType ) const
{
  FrameBufferID ret = FRAMEBUFFER_ID_ERROR;
  grp::DisplayType buf_disp_type;

  for( u32 i=0; i<m_arrayMax; ++i ){
    if( m_array[i].IsCreated() ){
      buf_disp_type = m_array[i].GetDisplayType();
      if( buf_disp_type == displayType ){
        ret = i;
        break;
      }
    }
  }

  return ret;
}


} /* namespace end grp */
} /* namespace end gfl */
