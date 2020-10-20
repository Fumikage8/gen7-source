//=============================================================================
/**
 * @file    FieldMenuSave.cpp
 * @brief   フィールドメニューセーブデータ
 * @author	Hiroyuki Nakamura
 * @date		2015.05.20
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include "Savedata/include/FieldMenuSave.h"
#include "System/include/PokemonVersion.h"

GFL_NAMESPACE_BEGIN(Savedata)

//! ロトム親密度データ
const FieldMenu::ROTOM_FRIENDSHIP_DATA FieldMenu::RotomFriendshipData[FieldMenu::ROTOM_FRIENDSHIP_DATA_MAX] =
{
  { // 島１クリアまで
    600,
    50,
  },
  { // 島２クリアまで
    1000,
    40,
  },
  { // 島３クリアまで
    1000,
    30,
  },
  { // 島４以降
    1000,
    20,
  },
};


//-----------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenu::FieldMenu(void)
{
  // 型のサイズが変わってしまっていないかチェックする ( MyStatusのマネ )
  STATIC_ASSERT(sizeof(m_menuData.nickname)==26);

	Clear( NULL );
}

//-----------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//-----------------------------------------------------------------------------
FieldMenu::~FieldMenu()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Clear
 * @brief   セーブデータクリア処理
 * @date    2015.05.20
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void FieldMenu::Clear( gfl2::heap::HeapBase * heap )
{
	gfl2::std::MemClear( &m_menuData, sizeof(MenuData) );

  m_menuData.is_page = 1;   // ページ存在フラグ

  for( int i=0; i<PAGE_MAX; i++ )
  {
    for( int j=0; j<PAGE_ICON_MAX; j++ )
    {
      u32 id = i * PAGE_ICON_MAX + j;
      m_menuData.icon_id[i][j] = id;        // 配置アイコンID
      m_menuData.icon_new |= ( 1 << id );   // 配置アイコンのNewフラグ
    }
  }

  { // アイコンセット
    static const IconID tbl[] =
    {
//      ICON_ID_ALBUM,    // マイアルバム
      ICON_ID_CONFIG,   // コンフィグ
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      SetMenuIconID( 0, i, tbl[i] );
      ResetIconNew( tbl[i] );
    }
  }

  // ライドポケモンのショートカット登録
  for( int i=0; i<KEY_ID_MAX; i++ )
  {
    m_menuData.ride_short_cut[i] = RIDE_ID_NONE;
  }
  m_menuData.ride_new = 0xff;   // ライドポケモンのNewフラグ

  // ニックネーム
  ClearNickNameString();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuPage
 * @brief   表示中のページ番号をセット
 * @date    2015.05.20
 *
 * @param   page  ページ番号
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetMenuPage( u32 page )
{
  if( page >= PAGE_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_menuData.page = page;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMenuPage
 * @brief   表示中のページ番号を取得
 * @date    2015.05.20
 *
 * @return  ページ番号
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::GetMenuPage(void) const
{
  return m_menuData.page;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMoveMenuPage
 * @brief   切り替え先のページ番号を取得
 * @date    2015.05.20
 *
 * @param   mv  切り替え方向
 *
 * @return  ページ番号
 *
 * @note  ページ番号は変更されない
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::GetMoveMenuPage( int mv ) const
{
  int page = GetMenuPage();

  int new_page = page + mv;
  if( new_page < 0 || new_page >= PAGE_MAX )
  {
    return page;
  }
  if( IsMenuPageEnable( new_page ) == false )
  {
    return page;
  }

  return new_page;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuPageEnable
 * @brief   ページを有効にする
 * @date    2015.05.29
 *
 * @param   page  ページ番号
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetMenuPageEnable( u32 page )
{
  if( page >= PAGE_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_menuData.is_page |= ( 1 << page );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMenuPageEnable
 * @brief   ページが有効かを調べる
 * @date    2015.05.29
 *
 * @param   page  ページ番号
 *
 * @retval  true  = 有効
 * @retval  false = 無効
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsMenuPageEnable( u32 page ) const
{
  if( page >= PAGE_MAX )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( ( m_menuData.is_page & (1<<page) ) != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMenuIconPos
 * @brief   アイコンのページ内の位置を取得
 * @date    2015.06.03
 *
 * @param   id  アイコンID
 *
 * @return  位置
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::GetMenuIconPos( IconID id )
{
  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    for( u32 j=0; j<PAGE_ICON_MAX; j++ )
    {
      if( m_menuData.icon_id[i][j] == id )
      {
        return j;
      }
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMenuIconPos
 * @brief   アイコンの位置を初期化
 * @date    2015.08.21
 */
//-----------------------------------------------------------------------------
void FieldMenu::InitMenuIconPos(void)
{
  for( int i=0; i<PAGE_MAX; i++ )
  {
    // 有効なものを入れる
    u32 pos = 0;
    for( int j=0; j<PAGE_ICON_MAX; j++ )
    {
      u32 id = i * PAGE_ICON_MAX + j;
      if( IsMenuIconEnable( static_cast<IconID>(id) ) != false )
      {
        m_menuData.icon_id[i][pos] = id;
        pos++;
      }
    }
    // 無効なものを入れる
    for( int j=0; j<PAGE_ICON_MAX; j++ )
    {
      u32 id = i * PAGE_ICON_MAX + j;
      if( IsMenuIconEnable( static_cast<IconID>(id) ) == false )
      {
        m_menuData.icon_id[i][pos] = id;
        pos++;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIconPos
 * @brief   アイコンの位置を取得
 * @date    2015.09.30
 *
 * @param   page  ページ番号格納場所
 * @param   pos   配置位置格納場所
 * @param   id    アイコンID
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool FieldMenu::GetIconPos( u16 * page, u16 * pos, IconID id )
{
  *page = PAGE_MAX;
  *pos  = PAGE_ICON_MAX;

  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    for( u32 j=0; j<PAGE_ICON_MAX; j++ )
    {
      if( m_menuData.icon_id[i][j] == id )
      {
        *page = i;
        *pos  = j;
        return true;
      }
    }
  }

  GFL_ASSERT( 0 );
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuIconID
 * @brief   アイコンIDをセット
 * @date    2015.05.20
 *
 * @param   page  ページ番号
 * @param   pos   配置位置
 * @param   id    アイコンID
 *
 * @li  指定位置から空いている位置を検索してセットする
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetMenuIconID( u32 page, u32 pos, IconID id )
{
  if( page >= PAGE_MAX || pos >= PAGE_ICON_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( IsMenuIconEnable(id) != false )
  {
    GFL_ASSERT( 0 );
    return;
  }

  // 現在の配置位置を取得
  u16 now_page = 0;
  u16 now_pos  = 0;
  if( GetIconPos( &now_page, &now_pos, id ) == false )
  {
    return;   // @note エラー
  }

  // 指定位置から検索して空いてる箇所に入れる
  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    for( u32 j=0; j<PAGE_ICON_MAX; j++ )
    {
      if( IsMenuIconEnable(static_cast<IconID>(m_menuData.icon_id[page][pos])) == false )
      {
        ChangeMenuIconID( now_page, now_pos, page, pos );
        m_menuData.icon_enable |= ( 1 << id );
        SetMenuPageEnable( page );
        return;
      }
      pos++;
      if( pos >= PAGE_ICON_MAX )
      {
        pos = 0;
      }
    }
    page++;
    if( page >= PAGE_MAX )
    {
      page = 0;
    }
    pos = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMenuIconID
 * @brief   アイコンIDをセット
 * @date    2015.05.20
 *
 * @param   id    アイコンID
 *
 * @return  セットしたページ番号
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::SetMenuIconID( IconID id )
{
  m_menuData.icon_enable |= ( 1 << id );

  // 初期位置に並べる
  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    for( u32 j=0; j<PAGE_ICON_MAX; j++ )
    {
      m_menuData.icon_id[i][j] = i * PAGE_ICON_MAX + j;
    }
  }
  // 空きを詰める
  for( u32 i=0; i<MENU_ICON_MAX; i++ )
  {
    u16 page1 = i / PAGE_ICON_MAX;
    u16 pos1  = i % PAGE_ICON_MAX;
    if( IsMenuIconEnable( static_cast<IconID>(m_menuData.icon_id[page1][pos1]) ) == false )
    {
      for( u32 j=i+1; j<MENU_ICON_MAX; j++ )
      {
        u16 page2 = j / PAGE_ICON_MAX;
        u16 pos2  = j % PAGE_ICON_MAX;
        if( IsMenuIconEnable( static_cast<IconID>(m_menuData.icon_id[page2][pos2]) ) != false )
        {
          ChangeMenuIconID( page1, pos1, page2, pos2 );
          break;
        }
      }
    }
  }
  // ページを有効にする @note 空きを詰めたので位置0だけ確認すればよい
  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    if( IsMenuIconEnable( static_cast<IconID>(m_menuData.icon_id[i][0]) ) != false )
    {
      SetMenuPageEnable( i );
    }
  }

  {
    u16 page = 0;
    u16 pos  = 0;
    if( GetIconPos( &page, &pos, id ) != false )
    {
      return page;
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMenuIconID
 * @brief   アイコンIDを取得
 * @date    2015.05.20
 *
 * @param   page  ページ番号
 * @param   pos   配置位置
 *
 * @return  アイコンID
 */
//-----------------------------------------------------------------------------
FieldMenu::IconID FieldMenu::GetMenuIconID( u32 page, u32 pos ) const
{
  if( page >= PAGE_MAX || pos >= PAGE_ICON_MAX )
  {
    GFL_ASSERT( 0 );
    return ICON_ID_MAX;
  }
  return static_cast<IconID>( m_menuData.icon_id[page][pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsMenuIconEnable
 * @brief   メニューアイコンが存在するか
 * @date    2015.05.20
 *
 * @param   id  アイコンID
 *
 * @retval  true  = 存在する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsMenuIconEnable( IconID id )
{
  if( ( m_menuData.icon_enable & (1<<id) ) != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeMenuIconID
 * @brief   アイコン入れ替え
 * @date    2015.05.20
 *
 * @param   page1   ページ１
 * @param   pos1    ページ１内の位置
 * @param   page2   ページ２
 * @param   pos2    ページ２内の位置
 */
//-----------------------------------------------------------------------------
void FieldMenu::ChangeMenuIconID( u32 page1, u32 pos1, u32 page2, u32 pos2 )
{
  if( page1 >= PAGE_MAX || pos1 >= PAGE_ICON_MAX || page2 >= PAGE_MAX || pos2 >= PAGE_ICON_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }
  // 同じ位置なので何もしない
  if( page1 == page2 && pos1 == pos2 )
  {
    return;
  }
  IconID tmp = static_cast<IconID>( m_menuData.icon_id[page1][pos1] );
  m_menuData.icon_id[page1][pos1] = m_menuData.icon_id[page2][pos2];
  m_menuData.icon_id[page2][pos2] = tmp;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeMenuIconID
 * @brief   アイコン入れ替え
 * @date    2015.06.03
 *
 * @param   id1   アイコンID１
 * @param   id2   アイコンID１
 */
//-----------------------------------------------------------------------------
void FieldMenu::ChangeMenuIconID( IconID id1, IconID id2 )
{
  u8  page1=0, page2=0, pos1=0, pos2=0;
  for( u32 i=0; i<PAGE_MAX; i++ )
  {
    for( u32 j=0; j<PAGE_ICON_MAX; j++ )
    {
      if( m_menuData.icon_id[i][j] == id1 )
      {
        page1 = i;
        pos1  = j;
      }
      else if( m_menuData.icon_id[i][j] == id2 )
      {
        page2 = i;
        pos2  = j;
      }
    }
  }
  ChangeMenuIconID( page1, pos1, page2, pos2 );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsIconNew
 * @brief   新規追加されたアイコンか
 * @date    2015.05.28
 *
 * @param   id  アイコンID
 *
 * @retval  true  = 新規
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsIconNew( IconID id ) const
{
  if( ( m_menuData.icon_new & ( 1 << id ) ) != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetIconNew
 * @brief   アイコンを新規追加状態にする
 * @date    2015.05.28
 *
 * @param   id  アイコンID
 *
 * @note  初期状態がNewなので、いらないかも。
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetIconNew( IconID id )
{
  m_menuData.icon_new |= ( 1 << id );
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetIconNew
 * @brief   アイコンの新規追加状態をOFF
 * @date    2015.05.28
 *
 * @param   id  アイコンID
 */
//-----------------------------------------------------------------------------
void FieldMenu::ResetIconNew( IconID id )
{
  if( IsIconNew( id ) == false )
  {
    return;
  }
  m_menuData.icon_new ^= ( 1 << id );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRideShortcut
 * @brief   登録されたライドポケモンを取得
 * @date    2015.07.15
 *
 * @param   key   ショートカットキー
 *
 * @retval  RIDE_ID_NONE = ショートカット未登録
 * @retval  RIDE_ID_NONE != 登録されたライドポケモン
 */
//-----------------------------------------------------------------------------
FieldMenu::RideID FieldMenu::GetRideShortcut( RideKeyID key )
{
  return static_cast<FieldMenu::RideID>(m_menuData.ride_short_cut[key]);
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRideShortcut
 * @brief   ライドポケモンを登録
 * @date    2015.07.15
 *
 * @param   key ショートカットキー
 * @param   id  登録する項目ID
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetRideShortcut( RideKeyID key, RideID id )
{
  m_menuData.ride_short_cut[key] = id;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRideShortcut
 * @brief   登録されたショートカットキーを取得
 * @date    2015.08.05
 *
 * @param   id  ライドポケモンID
 *
 * @retval  KEY_ID_MAX = ショートカット未登録
 * @retval  KEY_ID_MAX != 登録されたショートカットキー
 */
//-----------------------------------------------------------------------------
FieldMenu::RideKeyID FieldMenu::GetRideShortcutKey( RideID id )
{
  for( u32 i=0; i<KEY_ID_MAX; i++ )
  {
    if( m_menuData.ride_short_cut[i] == id )
    {
      return static_cast<RideKeyID>( i );
    }
  }
  return KEY_ID_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetRideNewFlag
 * @brief   ライドポケモンのNewフラグをOFFにする
 * @date    2015.08.05
 *
 * @param   id  ライドポケモンID
 */
//-----------------------------------------------------------------------------
void FieldMenu::ResetRideNewFlag( RideID id )
{
  m_menuData.ride_new &= ( 0xff ^ (1<<id) );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRideNewFlag
 * @brief   ライドポケモンのNewフラグがONか
 * @date    2015.08.05
 *
 * @param   id  ライドポケモンID
 *
 * @retval  true  = ON
 * @retval  false = OFF
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsRideNewFlag( RideID id )
{
  if( ( m_menuData.ride_new & (1<<id) ) != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsVisibleRotom
 * @brief   ロトムメッセージを表示するか
 * @date    2015.09.03
 *
 * @retval  true  = 表示
 * @retval  false = 非表示
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsVisibleRotom(void) const
{
  if( m_menuData.is_visible_rotom == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleRotom
 * @brief   ロトムメッセージを表示するかを設定
 * @date    2015.09.03
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetVisibleRotom( bool flg )
{
  if( flg == false )
  {
    m_menuData.is_visible_rotom = 0;
  }
  else
  {
    m_menuData.is_visible_rotom = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMapPosition
 * @brief   屋内にいるときのマップ表示座標を設定
 * @date    2015.09.09
 *
 * @param   x   X座標
 * @param   y   Y座標
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetMapPosition( f32 x, f32 y )
{
  m_menuData.map_x = x;
  m_menuData.map_y = y;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMapPosition
 * @brief   屋内にいるときのマップ表示座標を取得
 * @date    2015.09.09
 *
 * @param   x   X座標取得場所
 * @param   y   Y座標取得場所
 */
//-----------------------------------------------------------------------------
void FieldMenu::GetMapPosition( f32 * x, f32 * y ) const
{
  *x = m_menuData.map_x;
  *y = m_menuData.map_y;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomRank
 * @brief   ロトムの親密度ランクを取得
 * @date    2016.11.25
 *
 * @param   index   データインデックス
 *
 * @return  親密度ランク
 */
//-----------------------------------------------------------------------------
FieldMenu::RotomRank FieldMenu::GetRotomRank( u32 index )
{
  if( index >= ROTOM_FRIENDSHIP_DATA_MAX )
  {
    GFL_ASSERT( 0 );
    return ROTOM_RANK_1;
  }

  static const u32 tbl[] = 
  {
    300,
    600,
    999,
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( m_menuData.rotom_friendship <= tbl[i] )
    {
      return static_cast<RotomRank>( i );
    }
  }

  return ROTOM_RANK_4;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomFriendship
 * @brief   ロトムの親密度を取得
 * @date    2016.12.07
 *
 * @return  親密度
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::GetRotomFriendship(void)
{
  return m_menuData.rotom_friendship;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddRotomFriendship
 * @brief   ロトムの親密度をアップ
 * @date    2016.11.25
 *
 * @param   index     データインデックス
 * @param   val       アップ値
 * @param   is_touch  true = 枠タッチで上昇した
 *
 * @retval  true  = 性格変化
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::AddRotomFriendship( u32 index, s32 val, bool is_touch )
{
  if( index >= ROTOM_FRIENDSHIP_DATA_MAX )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  // @note マイナスは無効
  if( val < 0 )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  bool result = false;
  u32 limit = RotomFriendshipData[index].rank_limit;

  // 親密度アップ
  m_menuData.rotom_friendship += val;
  if( m_menuData.rotom_friendship > limit )
  {
    m_menuData.rotom_friendship = limit;
  }

  // ロトポン
  if( m_menuData.is_rotopon_open == 1 )
  {
    if( m_menuData.rotom_friendship_rotopon_flag == 0 )
    {
      if( m_menuData.rotom_friendship_rotopon_count != RotomFriendshipData[index].rotopon_count )
      {
        m_menuData.rotom_friendship_rotopon_count += val;
        if( m_menuData.rotom_friendship_rotopon_count >= RotomFriendshipData[index].rotopon_count )
        {
          m_menuData.rotom_friendship_rotopon_count = 0;
          m_menuData.rotom_friendship_rotopon_flag = 1;
        }
      }
    }
  }
  else
  {
    m_menuData.rotom_friendship_rotopon_count = 0;
  }

  if( m_menuData.rotom_friendship == limit )
  {
    // 性格変更
    if( GetRotomRank(index) == ROTOM_RANK_4 )
    {
      if( m_menuData.rotom_char_change_count != ROTOM_FRIENDSHIP_CHAR_CHG_OFFSET )
      {
        m_menuData.rotom_char_change_count += val;
        if( m_menuData.rotom_char_change_count >= ROTOM_FRIENDSHIP_CHAR_CHG_OFFSET )
        {
          m_menuData.rotom_char_change_count = 0;
          result = true;
        }
      }
    }
  }
  else
  {
    m_menuData.rotom_char_change_count = 0;
  }

  // 会話開始カウント
  m_menuData.rotom_talk_start_count++;
  if( m_menuData.rotom_talk_start_count > ROTOM_TALK_START_COUNT )
  {
    m_menuData.rotom_talk_start_count = ROTOM_TALK_START_COUNT;
  }

  return result;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomCharacter
 * @brief   ロトムの性格を設定
 * @date    2016.11.28
 *
 * @param   character   性格
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetRotomCharacter( RotomCharacter character )
{
  m_menuData.rotom_character = static_cast<u8>( character );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomCharacter
 * @brief   ロトムの性格を取得
 * @date    2016.11.28
 *
 * @return  性格
 */
//-----------------------------------------------------------------------------
FieldMenu::RotomCharacter FieldMenu::GetRotomCharacter(void)
{
  return static_cast<RotomCharacter>( m_menuData.rotom_character );
}

//-----------------------------------------------------------------------------
/**
 * @func    AddRotomCharacterCounter
 * @brief   ロトムの性格変更用のカウンタをアップ
 * @date    2016.11.30
 *
 * @param   character   性格
 * @param   value       アップ値
 */
//-----------------------------------------------------------------------------
void FieldMenu::AddRotomCharacterCounter( RotomCharacter character, u32 value )
{
  if( character == ROTOM_CHAR_SHY )
  {
    GFL_ASSERT( 0 );
    return;
  }

  u32 index = static_cast<u32>(character) - 1;
  if( m_menuData.rotom_char_counter[index] < ROTOM_CHAR_COUNTER_MAX )
  {
    u32 tmp = m_menuData.rotom_char_counter[index];
    m_menuData.rotom_char_counter[index] += value;
    if( m_menuData.rotom_char_counter[index] > ROTOM_CHAR_COUNTER_MAX )
    {
      m_menuData.rotom_char_counter[index] = ROTOM_CHAR_COUNTER_MAX;
    }
    // オーバーフロー対策
    else if( m_menuData.rotom_char_counter[index] < tmp )
    {
      m_menuData.rotom_char_counter[index] = ROTOM_CHAR_COUNTER_MAX;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomCharacterCounter
 * @brief   ロトムの性格変更用のカウンタを取得
 * @date    2016.11.30
 *
 * @param   character   性格
 *
 * @return  カウンタ
 */
//-----------------------------------------------------------------------------
u32 FieldMenu::GetRotomCharacterCounter( RotomCharacter character )
{
  if( character == ROTOM_CHAR_SHY )
  {
    GFL_ASSERT( 0 );
    return 0;
  }
  return m_menuData.rotom_char_counter[static_cast<u32>(character)-1];
}

//-----------------------------------------------------------------------------
/**
 * @func    InitRotomCharacterCounter
 * @brief   ロトムの性格変更用のカウンタを初期化
 * @date    2016.11.30
 */
//-----------------------------------------------------------------------------
void FieldMenu::InitRotomCharacterCounter(void)
{
  for( u32 i=0; i<ROTOM_CHAR_COUNTER; i++ )
  {
    m_menuData.rotom_char_counter[i] = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFriendshipRotoponStart
 * @brief   ロトポンを開始できる親密度になったか
 * @date    2016.12.06
 *
 * @retval  true  = できる
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsFriendshipRotoponStart(void)
{
  if( m_menuData.rotom_friendship_rotopon_flag == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    ClearFriendshipRotopon
 * @brief   ロトポン開始の親密度をクリア
 * @date    2016.12.06
 */
//-----------------------------------------------------------------------------
void FieldMenu::ClearFriendshipRotopon(void)
{
  m_menuData.rotom_friendship_rotopon_flag = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomRequest
 * @brief   ロトポンのお願いを設定
 * @date    2016.12.06
 *
 * @param   req   お願い
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetRotomRequest( RotomRequest req )
{
  if( req >= ROTOM_REQ_MAX )
  {
    GFL_ASSERT( 0 );
    req = ROTOM_REQ_NONE;
  }
  m_menuData.rotom_request_id = static_cast<u8>( req );
  m_menuData.rotom_request_clear = 0;   // 達成フラグOFF
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRotomRequest
 * @brief   ロトポンのお願いを取得
 * @date    2016.12.06
 *
 * @return  お願い
 */
//-----------------------------------------------------------------------------
FieldMenu::RotomRequest FieldMenu::GetRotomRequest(void)
{
  return static_cast<RotomRequest>( m_menuData.rotom_request_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomRequestClearFlag
 * @brief   ロトポンのお願い達成フラグを設定
 * @date    2016.12.06
 *
 * @param   flg   1 = 達成
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetRotomRequestClearFlag( u8 flg )
{
  m_menuData.is_rotopon_open = 1;   // @note 最初に達成されるのはチュートリアルなので、ここで解放する
  m_menuData.rotom_request_clear = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomRequestClearFlag
 * @brief   ロトポンのお願い達成フラグを取得
 * @date    2016.12.06
 *
 * @retval  true  = 達成
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsRotomRequestClearFlag(void)
{
  if( m_menuData.rotom_request_clear == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomTalkStart
 * @brief   ロトムの会話が発生するか
 * @date    2016.12.07
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldMenu::IsRotomTalkStart(void)
{
  if( m_menuData.rotom_talk_start_count == ROTOM_TALK_START_COUNT )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitRotomTalkStartCount
 * @brief   ロトムの会話開始カウントをクリア
 * @date    2016.12.07
 */
//-----------------------------------------------------------------------------
void FieldMenu::InitRotomTalkStartCount(void)
{
  m_menuData.rotom_talk_start_count = 0;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetNickNameString
 * @brief   ニックネーム文字列設定
 * @date    2017.01.05
 *
 * @param   src   設定する文字列
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetNickNameString( const gfl2::str::StrBuf * src )
{
  ClearNickNameString();  // ゴミが入ると嫌なのでクリア
  src->PutStr( m_menuData.nickname, System::STRLEN_PLAYER_NAME+System::EOM_LEN );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetNickNameString
 * @brief   ニックネーム文字列取得
 * @date    2017.01.05
 *
 * @param   dst   文字列格納場所
 */
//-----------------------------------------------------------------------------
void FieldMenu::GetNickNameString( gfl2::str::StrBuf * dst ) const
{
  dst->SetStr( m_menuData.nickname );
}

//-----------------------------------------------------------------------------
/**
 * @func    ClearNickNameString
 * @brief   ニックネーム文字列クリア
 * @date    2017.01.05
 */
//-----------------------------------------------------------------------------
void FieldMenu::ClearNickNameString(void)
{
  for( u32 i=0; i<System::STRLEN_PLAYER_NAME+System::EOM_LEN; i++ )
  {
    m_menuData.nickname[i] = gfl2::str::EOM_CODE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNicknameType
 * @brief   ニックネームタイプ設定
 * @date    2017.02.10
 *
 * @param   type  ニックネームタイプ
 */
//-----------------------------------------------------------------------------
void FieldMenu::SetNicknameType( NicknameType type )
{
  m_menuData.nickname_type = type;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNicknameType
 * @brief   ニックネームタイプ取得
 * @date    2017.02.10
 *
 * @return  ニックネームタイプ
 */
//-----------------------------------------------------------------------------
FieldMenu::NicknameType FieldMenu::GetNicknameType(void) const
{
  return static_cast<NicknameType>( m_menuData.nickname_type );
}




#if PM_DEBUG
void FieldMenu::DebugInitMenuSort(void)
{
  for( int i=0; i<PAGE_MAX; i++ )
  {
    for( int j=0; j<PAGE_ICON_MAX; j++ )
    {
      m_menuData.icon_id[i][j] = i * PAGE_ICON_MAX + j;
    }
  }
}

void FieldMenu::DebugSetRotomFriendship( u16 val )
{
  m_menuData.rotom_friendship = val;
}

u32 FieldMenu::DebugGetRotomFriendshipMax( u32 index )
{
  if( index >= ROTOM_FRIENDSHIP_DATA_MAX )
  {
    GFL_ASSERT( 0 );
    return 0;
  }
  return RotomFriendshipData[index].rank_limit;
}

void FieldMenu::DebugSetRotomCharacterCounter( RotomCharacter character, u32 val )
{
  if( character == ROTOM_CHAR_SHY )
  {
    GFL_ASSERT( 0 );
    return;
  }

  u32 index = static_cast<u32>(character) - 1;
  m_menuData.rotom_char_counter[index] = val;
  if( m_menuData.rotom_char_counter[index] > ROTOM_CHAR_COUNTER_MAX )
  {
    m_menuData.rotom_char_counter[index] = ROTOM_CHAR_COUNTER_MAX;
  }
}

u32 FieldMenu::DebugGetRotomCharacterChangeOffsetCount(void)
{
  return m_menuData.rotom_char_change_count;
}

void FieldMenu::DebugSetRotomCharacterChangeOffsetCount( u32 count )
{
  m_menuData.rotom_char_change_count = count;
}

u32 FieldMenu::DebugGetRotomCharacterChangeOffset(void)
{
  return ROTOM_FRIENDSHIP_CHAR_CHG_OFFSET;
}

u32 FieldMenu::DebugGetRotomRotoponStartOffsetCount(void)
{
  return m_menuData.rotom_friendship_rotopon_count;
}

void FieldMenu::DebugSetRotomRotoponStartOffsetCount( u32 count )
{
  if( m_menuData.rotom_friendship_rotopon_count != count )
  {
    m_menuData.is_rotopon_open = 1;
  }
  m_menuData.rotom_friendship_rotopon_count = count;
}

u32 FieldMenu::DebugGetRotomRotoponStartOffset(void)
{
  return RotomFriendshipData[0].rotopon_count;  // @note 一番大きい値にしておく
}

#endif

GFL_NAMESPACE_END(Savedata)
