//============================================================================================
/**
 * @file		sv_myitem.cpp
 * @brief		セーブデータ：所持アイテム
 * @author	Hiroyuki Nakamura
 * @date		11.05.23
 */
//============================================================================================
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <pml/include/item/item.h>

#include "../include/MyItemSave.h"
#include "../include/MyItemSaveDef.h"

#include "AppLib/include/Sort/StringItem.h"
#include "AppLib/include/Sort/StringWaza.h"


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	savedata
 * @brief			セーブデータ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN(Savedata)

static const u32 ERROR_POS = 0xffffffff;  //!< 不正な位置


//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------------------
MyItem::MyItem(void)
{
  GFL_ASSERT( NORMAL_ITEM_MAX >= ITEMPOCKET_COUNT_NORMAL );
  GFL_ASSERT( EVENT_ITEM_MAX >= ITEMPOCKET_COUNT_EVENT );
  GFL_ASSERT( WAZA_ITEM_MAX >= ITEMPOCKET_COUNT_WAZA );
  GFL_ASSERT( DRUG_ITEM_MAX >= ITEMPOCKET_COUNT_DRUG );
  GFL_ASSERT( NUTS_ITEM_MAX >= ITEMPOCKET_COUNT_NUTS );
  GFL_ASSERT( BEADS_ITEM_MAX >= ITEMPOCKET_COUNT_BEADS );
  GFL_ASSERT( ROTOPON_ITEM_MAX >= ITEMPOCKET_COUNT_ROTOPON );

  this->Clear(NULL);
  normalItem  = &itemAll[0];
  eventItem   = &itemAll[NORMAL_ITEM_MAX];
  wazaItem    = &itemAll[NORMAL_ITEM_MAX+EVENT_ITEM_MAX];
  drugItem    = &itemAll[NORMAL_ITEM_MAX+EVENT_ITEM_MAX+WAZA_ITEM_MAX];
  nutsItem    = &itemAll[NORMAL_ITEM_MAX+EVENT_ITEM_MAX+WAZA_ITEM_MAX+DRUG_ITEM_MAX];
  beadsItem   = &itemAll[NORMAL_ITEM_MAX+EVENT_ITEM_MAX+WAZA_ITEM_MAX+DRUG_ITEM_MAX+NUTS_ITEM_MAX];
  rotoponItem = &itemAll[NORMAL_ITEM_MAX+EVENT_ITEM_MAX+WAZA_ITEM_MAX+DRUG_ITEM_MAX+NUTS_ITEM_MAX+BEADS_ITEM_MAX];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
MyItem::~MyItem()
{
}

//-----------------------------------------------------------------------------
/**
* @brief セーブデータのクリア
* @param heap　クリアに使用するテンポラリヒープ
*/
//-----------------------------------------------------------------------------
void MyItem::Clear( gfl2::heap::HeapBase * heap )
{
  gfl2::std::MemClear( itemAll, sizeof(ITEM_ST)*TOTAL_ITEM_MAX );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを保存する場所を取得（フィールドポケット）
 *
 * @param		pocket	ポケット番号
 * @param		max			保存できる数格納場所
 *
 * @return	アイテム保存場所
 */
//--------------------------------------------------------------------------------------------
MyItem::ITEM_ST * MyItem::GetPocketItem( const u32 pocket, u32 * max ) const
{
  ITEM_ST * st;

  switch( pocket ){
  case ITEMPOCKET_NORMAL:
    st = normalItem;
    *max = NORMAL_ITEM_MAX;
    break;
  case ITEMPOCKET_DRUG:
    st = drugItem;
    *max = DRUG_ITEM_MAX;
    break;
  case ITEMPOCKET_WAZA:
    st = wazaItem;
    *max = WAZA_ITEM_MAX;
    break;
  case ITEMPOCKET_NUTS:
    st = nutsItem;
    *max = NUTS_ITEM_MAX;
    break;
  case ITEMPOCKET_EVENT:
    st = eventItem;
    *max = EVENT_ITEM_MAX;
    break;
  case ITEMPOCKET_BEADS:
    st = beadsItem;
    *max = BEADS_ITEM_MAX;
    break;
  case ITEMPOCKET_ROTOPON:
    st = rotoponItem;
    *max = ROTOPON_ITEM_MAX;
    break;
  case ITEMPOCKET_NONE:
    st = NULL;
    *max = 0;
    break;
  default:
    GFL_ASSERT( 0 );
    st = NULL;
    *max = 0;
  }

  return st;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを保存する場所を取得（フィールドポケット）
 *
 * @param[in]		id		アイテム番号
 * @param[out]  max		保存できる数格納場所
 *
 * @return	対象ポケット先頭のアイテムへのポインタ
 */
//--------------------------------------------------------------------------------------------
MyItem::ITEM_ST * MyItem::GetPocketParam( const u16 item_id, u32 * max ) const
{
  ITEM_ST * st = GetPocketItem( item::ITEM_DATA::GetParam( item_id, item::ITEM_DATA::PRM_ID_F_POCKET ), max );
  return st;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定アイテムが入っている場所を取得
 *
 * @param		id		アイテム番号
 * @param		max		保存できる数格納場所
 *
 * @retval	"NULL != アイテム保存場所 ※アイテムがない場合は空の場所"
 * @retval	"NULL = 失敗
 */
//--------------------------------------------------------------------------------------------
MyItem::ITEM_ST * MyItem::GetItemPos( const u16 id, u32 * max ) const
{
  ITEM_ST * item = GetPocketParam( id, max );
  if( item == NULL )
  {
    return NULL;
  }

  u32 pos = ERROR_POS;

  for( u32 i=0; i<*max; i++ )
  {
    if( item[i].id == id )
    {
      return &item[i];
    }
    if( pos == ERROR_POS )
    {
      if( item[i].id == ITEM_DUMMY_DATA )
      {
        pos = i;
      }
    }
  }
  if( pos == ERROR_POS )
  {
    return NULL;
  }
  return &item[pos];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定アイテムが入っている場所を取得
 *
 * @param		id		アイテム番号
 *
 * @retval	"NULL != アイテム保存場所
 * @retval	"NULL = 失敗
 */
//--------------------------------------------------------------------------------------------
MyItem::ITEM_ST * MyItem::GetItem( const u16 id ) const
{
  u32 max;
  ITEM_ST * item = GetPocketParam( id, &max );
  if( item == NULL )
  {
    return NULL;
  }

  for( u32 i=0; i<max; i++ )
  {
    if( item[i].id == id )
    {
      return &item[i];
    }
  }
  return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケット内の空の場所を取得
 *
 * @param		item	ポケットデータ
 * @param		max		保存できる数
 *
 * @return	空の場所
 */
//--------------------------------------------------------------------------------------------
MyItem::ITEM_ST * MyItem::GetItemPosSpace( ITEM_ST * item, const u32 max ) const
{
  u32 i;

  for( i=0; i<max; i++ ){
    if( item[i].id == ITEM_DUMMY_DATA ){
      return &item[i];
    }
  }
  return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム番号０に個数が入ってたら消す
 *
 * @param		item	アイテムデータ
 * @param		max		最大値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MyItem::DeleteNullItem( MyItem::ITEM_ST * item, const u32 max )
{
  u32	i;

  for( i=0; i<max; i++ ){
    if( item[i].id == ITEM_DUMMY_DATA ){
      item[i].num = 0;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケットのアイテム数を取得
 *
 * @param		pocket         	ポケット番号
 * @param   is_free_space   true = フリースペースを有効にする
 *
 * @return	アイテム数
 *
 * @note  フリースペースを有効にした場合、指定ポケット外とみなす
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetItemCount( const u32 pocket, bool is_free_space )
{
  const ITEM_ST * item;
  u32	max;
  u32 cnt = 0;

  DeleteSpace( pocket );	// あらかじめスペースを消しておく

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return 0;
  }

  for( u32 i=0; i<max; i++ )
  {
    if( item[i].id == ITEM_DUMMY_DATA )
    {
      break;
    }
    if( item[i].num != 0 && ( is_free_space == false || item[i].free_space == 0 ) )
    {
      cnt++;
    }
  }
  return cnt;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケットのアイテム総所持数を取得
 *
 * @param		pocket	ポケット番号
 * @param   is_free_space   true = フリースペースを有効にする
 *
 * @return	アイテム総所持数
 *
 * @note  フリースペースを有効にした場合、指定ポケット外とみなす
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetItemTotalCount( const u32 pocket, bool is_free_space )
{
  const MyItem::ITEM_ST * item;
  u32	max;
  u32 total_count = 0;

  DeleteSpace( pocket );	// あらかじめスペースを消しておく

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return 0;
  }

  for( u32 i=0; i<max; i++ ){
    if( item[i].id == ITEM_DUMMY_DATA )
    {
      break;
    }
    if( is_free_space == false || item[i].free_space == 0 )
    {
      total_count += item[i].num;
    }
  }
  return total_count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定ポケットの指定位置のアイテムを取得
 *
 * @param		pocket	ポケット番号
 * @param		pos			位置
 *
 * @retval	"NULL != アイテム"
 * @retval	"NULL = 失敗"
 */
//--------------------------------------------------------------------------------------------
const MyItem::ITEM_ST * MyItem::GetItem( const u32 pocket, const u32 pos ) const
{
  ITEM_ST * item;
  u32	max;

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return NULL;
  }
  if( pos < max )
  {
    return &item[pos];
  }
  return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム番号から指定ポケットのアイテムを取得
 *
 * @param		pocket	ポケット番号
 * @param		id			アイテム番号
 *
 * @retval	"NULL != アイテム"
 * @retval	"NULL = 失敗"
 */
//--------------------------------------------------------------------------------------------
const MyItem::ITEM_ST * MyItem::GetItemID( u32 pocket, u16 id ) const
{
  ITEM_ST * item;
  u32	max;

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return NULL;
  }
  for( u32 i=0; i<max; i++ )
  {
    if( item[i].id == id )
    {
      return &item[i];
    }
  }
  return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定ポケットのアイテム位置を取得
 *
 * @param   pocket          ポケット番号
 * @param   id              アイテム番号
 * @param   is_free_space   true = フリースペースを含める
 *
 * @retval  "ITEM_NONE != アイテム位置"
 * @retval  "ITEM_NONE = アイテムなし"
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetItemPocketPos( u32 pocket, u16 id, bool is_free_space ) const
{
  ITEM_ST * item;
  u32	max;
  u32 cnt = 0;

  if( id == ITEM_DUMMY_DATA ){
    return ITEM_NONE;
  }

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return ITEM_NONE;
  }
  for( u32 i=0; i<max; i++ )
  {
    if( item[i].id == id )
    {
      return cnt;
    }
    if( !( is_free_space == false && item[i].free_space != 0 ) )
    {
      cnt++;
    }
  }
  return ITEM_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムを加える
 *
 * @param   id          アイテム番号
 * @param   num         加える数
 * @param   is_set_new  新規入手とするか
 *
 * @retval  "true = 成功"
 * @retval  "false = 失敗"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::Add( const u16 id, const u16 num, bool is_set_new )
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item != NULL )
  {
    if( item[0].id == id )
    {
      if( ( item[0].num + 1 ) <= ITEM_HAVE_MAX )
      {
        u32 tmp = item[0].num + num;
        if( tmp > ITEM_HAVE_MAX )
        {
          tmp = ITEM_HAVE_MAX;
        }
        item[0].num = tmp;
        return true;
      }
    }else{
      item[0].id  = id;
      if( num > ITEM_HAVE_MAX )
      {
        item[0].num = ITEM_HAVE_MAX;
      }
      else
      {
        item[0].num = num;
      }
      if( is_set_new  == true )
      {
        item[0].new_flag = 1;
      }
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを加えられるかチェック
 *
 * @param		id		アイテム番号
 * @param		num		加える数
 *
 * @retval	"true = 可"
 * @retval	"false = 不可"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::AddCheck( const u16 id, const u16 num ) const
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item != NULL )
  {
    if( item[0].id == id )
    {
      u32 tmp = item[0].num + num;
      if( tmp <= ITEM_HAVE_MAX )
      {
        return true;
      }
    }
    else
    {
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを減らす
 *
 * @param		id		アイテム番号
 * @param		num		減らす数
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::Sub( const u16 id, const u16 num )
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item != NULL )
  {
    if( item[0].id == id && item[0].num >= num )
    {
      item[0].num -= num;
      if( item[0].num == 0 )
      {
        item[0].new_flag = 0;
        // スペースを埋める
        item = GetPocketParam( id, &max );
        for( int i=0; i<static_cast<int>(max)-1; i++ )
        {
          for( int j=i+1; j<static_cast<int>(max); j++ )
          {
            if( item[i].num == 0 )
            {
              Change( &item[i], &item[j] );
            }
          }
        }
      }
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを減らせるかチェック
 *
 * @param		id		アイテム番号
 * @param		num		減らす数
 *
 * @retval	"true = 可"
 * @retval	"false = 不可"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::SubCheck( const u16 id, const u16 num ) const
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item != NULL ){
    if( item[0].id == id && item[0].num >= num ){
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定アイテムを持っているかチェック
 *
 * @param		id		アイテム番号
 * @param		num		数
 *
 * @retval	"true = 持っている"
 * @retval	"false = 持っていない"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::Check( const u16 id, const u16 num ) const
{
  return SubCheck( id, num );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定アイテムの所持数を取得
 *
 * @param		id		アイテム番号
 *
 * @return	所持数
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetItemNum( u16 id ) const
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item == NULL ){
    return 0;
  }
  return item->num;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムを入れ替える
 *
 * @param		item1		アイテム１
 * @param		item2		アイテム２
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MyItem::Change( MyItem::ITEM_ST * item1, MyItem::ITEM_ST * item2 )
{
  ITEM_ST	tmp = *item1;
  *item1 = *item2;
  *item2 = tmp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定ポケットのアイテムを入れ替える
 *
 * @param		pocket	ポケット番号
 * @param		pos1		位置１
 * @param		pos2		位置２
 *
 * @retval	"NULL != アイテム"
 * @retval	"NULL = 失敗"
 */
//--------------------------------------------------------------------------------------------
bool MyItem::Change( const u32 pocket, const u32 pos1, const u32 pos2 )
{
  if( pos1 == pos2 ){ return false; }		// 位置が同じ

  ITEM_ST * item;
  u32	max;

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return false;
  }
  if( pos1 >= max || pos2 >= max )
  {
    return false;
  }

  Change( &item[pos1], &item[pos2] );

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定ポケットのアイテムを指定位置に挿入
 *
 * @param   pocket          ポケット番号
 * @param   pos1            位置１
 * @param   pos2            位置２
 * @param   is_free_space   true = フリースペースを含める
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MyItem::Insert( u32 pocket, u32 pos1, u32 pos2, bool is_free_space )
{
  // 位置が同じ
  if( pos1 == pos2 )
  {
    return;
  }

  ITEM_ST * item;
  u32	max;

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return;
  }

  // フリースペース登録分の位置補正
  if( is_free_space == false )
  {
    u32 cnt = 0;
    u16 flg1 = false;
    u16 flg2 = false;
    for( u32 i=0; i<max; i++ )
    {
      if( item[i].id != ITEM_DUMMY_DATA && item[i].free_space == 0 )
      {
        if( flg1 == false && pos1 == cnt )
        {
          pos1 = i;
          flg1 = true;
        }
        if( flg2 == false && pos2 == cnt )
        {
          pos2 = i;
          flg2 = true;
        }
        if( flg1 != false && flg2 != false )
        {
          break;
        }
        cnt++;
      }
    }
  }

  if( pos1 >= max || pos2 >= max )
  {
    return;
  }

  ITEM_ST	tmp = item[pos1];
  if( pos1 < pos2 )
  {
    for( u32 i=pos1; i<pos2; i++ )
    {
      item[i] = item[i+1];
    }
  }
  else
  {
    for( int i=static_cast<int>(pos1); i>static_cast<int>(pos2); i-- )
    {
      item[i] = item[i-1];
    }
  }
  item[pos2] = tmp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スペースをつめる
 *
 * @param		item	アイテムデータ
 * @param		max		最大値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MyItem::DeleteSpace( const u32 pocket )
{
  ITEM_ST * item;
  u32	max;
  u16	i, j;

  item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return;
  }
  DeleteNullItem( item, max );

  for( i=0; i<max-1; i++ )
  {
    for( j=i+1; j<max; j++ )
    {
      if( item[i].num == 0 )
      {
        Change( &item[i], &item[j] );
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちアイテム全体のNULLアイテムを消して詰める
 *
 * @param		none
 *
 * @return	none
 *
 * @li	異常ロム対処用。バッグ画面の初期化時に入れておくと良い
 */
//--------------------------------------------------------------------------------------------
void MyItem::CheckSafety(void)
{
  DeleteSpace( ITEMPOCKET_NORMAL );
  DeleteSpace( ITEMPOCKET_DRUG );
  DeleteSpace( ITEMPOCKET_WAZA );
  DeleteSpace( ITEMPOCKET_NUTS );
  DeleteSpace( ITEMPOCKET_EVENT );
  DeleteSpace( ITEMPOCKET_BEADS );
  DeleteSpace( ITEMPOCKET_ROTOPON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定アイテムを置き換える
 *
 * @param		item	置き換え前のアイテム
 *
 * @return	置き換え後のアイテム
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::Swap( u32 item )
{
  static const u16 swapItem[][2] =
  {
    { ITEM_IDENSINOKUSABI, ITEM_IDENSINOKUSABI_1 },
    { ITEM_SORUPURASU,     ITEM_SORUPURASU_1 },
    { ITEM_RUNAPURASU,     ITEM_RUNAPURASU_1 },
  };
  u32	max;
  ITEM_ST * st = GetPocketItem( ITEMPOCKET_EVENT, &max );
  bool flg = false;
  u32	si = ITEM_DUMMY_DATA;

  for( u32 i=0; i<GFL_NELEMS(swapItem); i++ )
  {
    for( u32 j=0; j<2; j++ )
    {
      if( item == swapItem[i][j] )
      {
        si = swapItem[i][j^1];
      }
    }
    if( si != ITEM_DUMMY_DATA )
    {
      break;
    }
  }

  if( si == ITEM_DUMMY_DATA )
  {
    GFL_ASSERT( 0 );
    return ITEM_DUMMY_DATA;
  }

  for( u32 i=0; i<max; i++ ){
    if( st[i].id == item ){
      if( flg == false ){
        st[i].id = si;
        if( st[i].num == 0 ){
          GFL_ASSERT( 0 );
          st[i].num = 1;
        }
        flg = true;
      }else{
        GFL_ASSERT( 0 );
        st[i].id  = ITEM_DUMMY_DATA;
        st[i].num = 0;
        st[i].free_space = 0;
      }
    }
  }

  DeleteSpace( ITEMPOCKET_EVENT );

  return si;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 名前によってソート
 *
 * @param sort  ソート用バイナリ
 * @param item  GetPocketItemで取得したITEM_ST*
 * @param max   GetPocketItemで取得したmax
 */
//--------------------------------------------------------------------------------------------
void MyItem::SortByName( const app::sort::StringItem * sort, ITEM_ST ** item, u32 max )
{
  u32 cnt = 0;
  for( u32 i=0; i<sort->GetSortOrderMax(); i++ )
  {
    u32 sort_item = sort->GetSortOrder( i );
    for( u32 j=0; j<max; j++ )
    {
      if( item[j]->id != ITEM_DUMMY_DATA )
      {
        if( item[j]->id == sort_item )
        {
          Change( item[j], item[cnt] );
          cnt++;
          break;
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 技マシンを技名によってソート
 *
 * @param sort  ソート用バイナリ
 * @param item  GetPocketItemで取得したITEM_ST*
 * @param max   GetPocketItemで取得したmax
 */
//--------------------------------------------------------------------------------------------
void MyItem::SortByWazaName( const app::sort::StringWaza * sort, ITEM_ST ** item, u32 max )
{
  u32 cnt = 0;
  for( u32 i=0; i<sort->GetSortOrderMax(); i++ )
  {
    u32 sort_waza = sort->GetSortOrder( i );
    for( u32 j=0; j<max; j++ )
    {
      u32 waza_machine_no = item::ITEM_GetWazaMashineNo( item[j]->id );
      if( waza_machine_no != ITEM_WAZAMACHINE_ERROR )
      {
        if( item::ITEM_GetWazaMashineWaza(waza_machine_no) == sort_waza )
        {
          Change( item[j], item[cnt] );
          cnt++;
          break;
        }
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ソートを使うようにする
 *
 * @param heap  ヒープ
 */
//--------------------------------------------------------------------------------------------
/*
void MyItem::SetupSortByName( gfl2::heap::HeapBase * heap )
{
  sortTemporary = GFL_NEW_ARRAY( heap ) ITEM_ST[ SORT_TEMPORARY_MAX ];
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief ソートを破棄する
 */
//--------------------------------------------------------------------------------------------
/*
void MyItem::CleanSortByName( void )
{
  GFL_DELETE_ARRAY sortTemporary;
}
*/



#if 0
//--------------------------------------------------------------------------------------------
/**
 * @func    GetFreeSpacePos
 * @brief   フリースペースの位置を取得 ( ID指定版 )
 * @date    2015.03.16
 *
 * @param   id  アイテム番号
 *
 * @retval  ITEM_NONE  = フリースペースに入っていない
 * @retval  ITEM_NONE != フリースペース内の位置
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetFreeSpacePos( u16 id )
{
  const ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item == NULL )
  {
    return ITEM_NONE;
  }
  return GetFreeSpacePos( item );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    GetFreeSpacePos
 * @brief   フリースペースの位置を取得 ( データ指定版 )
 * @date    2015.03.16
 *
 * @param   item  アイテムデータ
 *
 * @retval  ITEM_NONE  = フリースペースに入っていない
 * @retval  ITEM_NONE != フリースペース内の位置
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::GetFreeSpacePos( const ITEM_ST * item )
{
  if( item->free_space == 0 )
  {
    return ITEM_NONE;
  }
  return item->free_space;
}
#endif


//--------------------------------------------------------------------------------------------
/**
 * @func    MakeFreeSpaceItemTable
 * @brief   フリースペースの道具テーブルを生成
 * @date    2015.03.16
 *
 * @param   tbl       生成場所
 * @param   tbl_size  生成場所のサイズ
 *
 * @return  テーブルに登録した個数
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::MakeFreeSpaceItemTable( ITEM_ST ** tbl, u32 tbl_size )
{
  u32 max;
  s32 cnt = 0;

  // 全体からフリースペースのアイテムを取得
  for( u32 i=0; i<FIELD_BAG_POCKET_MAX; i++ )
  {
    ITEM_ST * item = GetPocketItem( i, &max );
    for( u32 j=0; j<max; j++ )
    {
      if( item[j].free_space != 0 )
      {
        tbl[cnt] = &item[j];
        cnt++;
      }
    }
  }

  // ソート
  for( s32 i=0; i<cnt-1; i++ )
  {
    for( s32 j=i+1; j<cnt; j++ )
    {
      if( tbl[i]->num == 0 || ( tbl[j]->num != 0 && tbl[i]->free_space > tbl[j]->free_space ) )
      {
        ITEM_ST * item = tbl[i];
        tbl[i] = tbl[j];
        tbl[j] = item;
      }
    }
  }

  // 位置を再設定
  s32 num_cnt = cnt;
  for( s32 i=0; i<cnt; i++ )
  {
    tbl[i]->free_space = i+1;
    if( num_cnt == cnt && tbl[i]->num == 0 )
    {
      num_cnt = i;
    }
  }

  return num_cnt;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    MakePocketItemTable
 * @brief   指定ポケットのフリースペース未登録の道具テーブルを生成
 * @date    2015.03.18
 *
 * @param   pocket    ポケット
 * @param   tbl       生成場所
 * @param   tbl_size  生成場所のサイズ
 *
 * @return  テーブルに登録した個数
 */
//--------------------------------------------------------------------------------------------
u32 MyItem::MakePocketItemTable( u32 pocket, ITEM_ST ** tbl, u32 tbl_size )
{
  DeleteSpace( pocket );

  u32 max;
  u32 cnt = 0;
  ITEM_ST * item = GetPocketItem( pocket, &max );
  if( item == NULL )
  {
    return 0;
  }

  for( u32 i=0; i<max; i++ )
  {
    if( item[i].id == ITEM_DUMMY_DATA || item[i].num == 0 )
    {
      continue;
    }
    if( item[i].free_space == 0 )
    {
      tbl[cnt] = &item[i];
      cnt++;
    }
  }

  return cnt;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    AddFreeSpace
 * @brief   フリースペースに登録
 * @date    2015.03.18
 *
 * @param   tbl           フリースペースの道具テーブル
 * @param   item          登録する道具
 * @param   freespace_max フリースペースに登録された道具数
 * @param   is_clear      登録番号を上書きするか
 */
//--------------------------------------------------------------------------------------------
void MyItem::AddFreeSpace( ITEM_ST ** tbl, ITEM_ST * item, u32 * freespace_max, bool is_clear )
{
  if( item->free_space != 0 && is_clear == false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  tbl[*freespace_max] = item;
  *freespace_max += 1;
  item->free_space = *freespace_max;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    SubFreeSpace
 * @brief   フリースペースの登録を解除
 * @date    2015.03.18
 *
 * @param   item  解除する道具
 */
//--------------------------------------------------------------------------------------------
void MyItem::SubFreeSpace( ITEM_ST * item )
{
  if( item->free_space == 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }

  item->free_space = 0;

  // 本来の保存場所の最後尾に移動する
  u32 max;
  ITEM_ST * pocket = GetPocketParam( item->id, &max );
  if( pocket == NULL )
  {
    return;
  }
  for( u32 i=0; i<max; i++ )
  {
    if( pocket[i].id == item->id )
    {
      ITEM_ST tmp = pocket[i];
      u32 j;
      for( j=i; j<max-1; j++ )
      {
        if( pocket[j+1].id == ITEM_DUMMY_DATA )
        {
          break;
        }
        pocket[j] = pocket[j+1];
      }
      pocket[j] = tmp;
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @func    InsertFreeSpace
 * @brief   フリースペースの指定位置に挿入
 * @date    2015.05.08
 *
 * @param   tbl           フリースペースの道具テーブル
 * @param   item          挿入する道具
 * @param   freespace_max フリースペースに登録された道具数
 * @param   in_pos        挿入位置
 */
//--------------------------------------------------------------------------------------------
void MyItem::InsertFreeSpace( ITEM_ST ** tbl, ITEM_ST * item, u32 * freespace_max, u32 in_pos )
{
  if( item->free_space == 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }
  if( in_pos >= *freespace_max )
  {
    GFL_ASSERT( 0 );
    return;
  }

  int now_pos = item->free_space - 1;

  if( now_pos == static_cast<int>(in_pos) )
  {
    return;
  }

  if( now_pos < static_cast<int>(in_pos) )
  {
    for( int i=now_pos; i<static_cast<int>(in_pos); i++ )
    {
      tbl[i] = tbl[i+1];
      tbl[i]->free_space = i+1;
    }
  }
  else
  {
    for( int i=now_pos; i>static_cast<int>(in_pos); i-- )
    {
      tbl[i] = tbl[i-1];
      tbl[i]->free_space = i+1;
    }
  }
  tbl[in_pos] = item;
  tbl[in_pos]->free_space = in_pos+1;

/*
  // 登録解除
  for( u32 i=0; i<*freespace_max; i++ )
  {
    if( tbl[i] == item )
    {
      for( u32 j=i; j<*freespace_max-1; j++ )
      {
        tbl[j] = tbl[j+1];
        tbl[j]->free_space -= 1;
      }
      tbl[*freespace_max] = NULL;
      break;
    }
  }

  // 再登録
  for( u32 i=*freespace_max; i>in_pos; i-- )
  {
    tbl[i] = tbl[i-1];
    tbl[i]->free_space += 1;
  }
  tbl[in_pos] = item;
  item->free_space = in_pos + 1;
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsRegisterFreeSpace
 * @brief   フリースペースに登録されているか
 * @date    2015.03.18
 *
 * @param   id  アイテム番号
 *
 * @retval  true  = 登録されている
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------------------------
bool MyItem::IsRegisterFreeSpace( u32 id )
{
  return IsRegisterFreeSpace( GetItem(id) );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsRegisterFreeSpace
 * @brief   フリースペースに登録されているか
 * @date    2015.05.08
 *
 * @param   item  手持ち道具データ
 *
 * @retval  true  = 登録されている
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------------------------
bool MyItem::IsRegisterFreeSpace( const ITEM_ST * item )
{
  if( item == NULL )
  {
    return false;
  }
  if( item->free_space == 0 )
  {
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsNew
 * @brief   新規入手アイテムかをチェック ( 番号指定版 )
 * @date    2015.04.06
 *
 * @param   id  アイテム番号
 *
 * @retval  true  = 新規
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------------------------
bool MyItem::IsNew( u32 id ) const
{
  ITEM_ST * item = GetItem( id );

  if( item == NULL )
  {
    return false;
  }
  return IsNew( item );
}

//--------------------------------------------------------------------------------------------
/**
 * @func    IsNew
 * @brief   新規入手アイテムかをチェック ( データ指定版 )
 * @date    2015.04.06
 *
 * @param   item  アイテムデータ
 *
 * @retval  true  = 新規
 * @retval  false = それ以外
 */
//--------------------------------------------------------------------------------------------
bool MyItem::IsNew( const ITEM_ST * item ) const
{
  if( item->new_flag == 0 )
  {
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    ResetNew
 * @brief   新規入手フラグを下げる ( 番号指定版 )
 * @date    2015.04.06
 *
 * @param   id  アイテム番号
 */
//--------------------------------------------------------------------------------------------
void MyItem::ResetNew( u32 id )
{
  ITEM_ST * item = GetItem( id );

  if( item != NULL )
  {
    ResetNew( item );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @func    ResetNew
 * @brief   新規入手フラグを下げる ( データ指定版 )
 * @date    2015.04.06
 *
 * @param   id  アイテム番号
 */
//--------------------------------------------------------------------------------------------
void MyItem::ResetNew( ITEM_ST * item )
{
  item->new_flag = false;
}




//=============================================================================
//=============================================================================
#if PM_DEBUG
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief		アイテムを減らして格納場所を削除（デバッグ用）
 *
 * @param		id		アイテム番号
 * @param		num		減らす数
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 */
//-----------------------------------------------------------------------------
bool MyItem::Debug_Sub( const u16 id, const u16 num )
{
  ITEM_ST * item;
  u32	max;

  item = GetItemPos( id, &max );
  if( item != NULL )
  {
    if( item[0].id == id && item[0].num >= num )
    {
      if( item[0].free_space != 0 )
      {
        SubFreeSpace( item );
      }
      item[0].num -= num;
      if( item[0].num == 0 )
      {
        item[0].id = ITEM_DUMMY_DATA;
        item[0].new_flag = 0;
        // スペースを埋める
        item = GetPocketParam( id, &max );
        for( int i=0; i<static_cast<int>(max)-1; i++ )
        {
          for( int j=i+1; j<static_cast<int>(max); j++ )
          {
            if( item[i].num == 0 )
            {
              Change( &item[i], &item[j] );
            }
          }
        }
      }
      return true;
    }
  }
  return false;
}

#endif  // PM_DEBUG


GFL_NAMESPACE_END(Savedata)
