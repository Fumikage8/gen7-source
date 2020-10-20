//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapFileControl.cpp
 *	@brief  マップ管理　ファイルコントロール
 *	@author	tomoya takahashi
 *	@date		2011.01.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>
#include <grp/map/gfl_MapFileControl.h>

namespace gfl{
namespace grp{
namespace map{


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
FileControlArray::FileControlArray( void ) :
  m_ppFileControl(NULL),
  m_FileControlNum(0)
{
}

//----------------------------------------------------------------------------
/**
 *	@brief    デストラクタ
 */
//-----------------------------------------------------------------------------
FileControlArray::~FileControlArray()
{
  Finalize();
}


//-----------------------------------------------------------------------------
/**
 *  @brief  初期化
 *
 *  @param  arrayNum    配列数
 *  @param  p_heap      ヒープ
 */
//-----------------------------------------------------------------------------
void FileControlArray::Initialize( u32 arrayNum, gfl::heap::HeapBase* p_heap )
{
  if( m_ppFileControl ){
    GFL_ASSERT( m_ppFileControl == NULL );//@check
    Finalize();
  }
  
  m_ppFileControl   = GFL_NEW_ARRAY(p_heap) FileControl*[arrayNum];
  m_FileControlNum  = arrayNum;
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄処理
 */
//-----------------------------------------------------------------------------
void FileControlArray::Finalize( void )
{
  if( m_ppFileControl ){
    GFL_DELETE_ARRAY m_ppFileControl;
    m_ppFileControl   = NULL;
    m_FileControlNum  = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief ファイルコントロールの登録
 *  
 *  @param  p_control   コントロール 
 *
 *  @retval true  登録成功
 *  @retval false 登録失敗（登録数オーバー）
 */
//-----------------------------------------------------------------------------
b32 FileControlArray::SetFileControl( FileControl* p_control )
{
  u32 index = GetClearIndex();

  if( index >= m_FileControlNum ){
    return false;
  }

  // ポインタの登録
  m_ppFileControl[index] = p_control;

  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief シグネチャのファイルコントロール取得
 *
 *  @param  signature   シグネチャー 
 *
 *  @retval シグネチャのファイルコントロール
 *  @retval NULL  みつからない。
 */
//-----------------------------------------------------------------------------
FileControl* FileControlArray::GetFileControl( SignatureType signature ) const
{
  // シグネチャのファイルコントロールを取得
  for( u32 i=0; i<m_FileControlNum; i++ ){

    if( m_ppFileControl[i] ){

      if( m_ppFileControl[i]->GetSignature() == signature ){
        return m_ppFileControl[i];
      }
    }
  }

  // 見つからなかった。
  GFL_ASSERT( 0 );
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  あいているバッファインデックスを取得
 *
 *	@return あいているバッファインデックス
 */
//-----------------------------------------------------------------------------
u32 FileControlArray::GetClearIndex( void ) const
{
  for( u32 i=0; i<m_FileControlNum; i++ ){
    if( m_ppFileControl[i] == NULL )
    {
      return i;
    }
  }

  // 要素数が足りない
  GFL_ASSERT( 0 );
  return 0;
}





//-----------------------------------------------------------------------------
/**
 *      FileContorl
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  ブロック情報を設定  ブロックのファイル読み込みが完了すると
 *	　　　　この関数がコールされる。　この内部で、描画環境を整える必要がある。
 *
 *	@param  p_map   マップブロック情報
 *
 *	@retval true  完了
 *	@retval false 途中
 */
//-----------------------------------------------------------------------------
b32 FileControl::SetUp( gfl::grp::map::System* p_map )
{
  GFL_UNUSED(p_map);

  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief    アトリビュート情報を取得
 *
 *	@param	cp_map      マップブロック情報
 *	@param  p_attrInfo  アトリビュート情報格納先
 *	@param  cp_pos      取得する３D座標
 *	@param  cp_way      地面方向（Default　(0,-1,0)
 *	@param  map_width   マップブロックのサイズ
 */
//-----------------------------------------------------------------------------
void FileControl::GetAttr( const gfl::grp::map::System* cp_map, AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width )
{
  GFL_UNUSED(cp_map);
  GFL_UNUSED(p_attrInfo);
  GFL_UNUSED(cp_pos);
  GFL_UNUSED(cp_way);
  GFL_UNUSED(map_width);

  // 構造体をクリア
  ClearAttrInfo( p_attrInfo );
}



//----------------------------------------------------------------------------
/**
 *	@brief    アトリビュート情報の初期化処理
 *
 *	@param  p_attrInfo  アトリビュート情報格納先
 */
//-----------------------------------------------------------------------------
void FileControl::ClearAttrInfo( AttrInfo* p_attrInfo )
{
  gfl::std::MemClear( p_attrInfo, sizeof(AttrInfo) );
}


} // namespace map
} // namespace grp
} // namespace gfl

