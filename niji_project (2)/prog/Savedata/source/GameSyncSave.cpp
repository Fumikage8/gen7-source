//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file    GameSyncSave.cpp
 *  @brief   ゲームシンク用セーブデータ
 *  @author  hisanso suzuki
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "Savedata/include/GameSyncSave.h"

#include "GameSys/include/NijiTime.h"
#include <pml/include/item/item.h>

#include <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>
GFL_NAMESPACE_BEGIN(Savedata)

//=============================================================================
//@brief コンストラクタ・デストラクタ
//=============================================================================
/**
 * @brief コンストラクタ
 */
GameSyncSave::GameSyncSave() : SaveDataInterface()
{
  this->Clear(NULL);
}

/**
 * @brief デストラクタ
 */
GameSyncSave::~GameSyncSave()
{

}

//=============================================================================
//@brief アイテムスロットの空き数を取得
//=============================================================================
u8 GameSyncSave::GetFreeItemSlotNum(void) const
{
  if( m_data.m_itemInf.m_itemNum > STOCK_ITEM_MAX ){
    return 0;
  }
  return STOCK_ITEM_MAX-m_data.m_itemInf.m_itemNum;
}

//=============================================================================
//@brief PGLから受信済みのアイテム情報数を取得
//=============================================================================
u8 GameSyncSave::GetItemNum(void) const
{
  return m_data.m_itemInf.m_itemNum;
}

//=============================================================================
//@brief PGLから受信済みのアイテム情報を取得
//=============================================================================
const GameSyncSave::StockItem& GameSyncSave::GetItem(u8 index) const
{
  GFL_ASSERT( m_data.m_itemInf.m_itemNum > index ); // @fix

  if( index >= m_data.m_itemInf.m_itemNum ){
    return m_data.m_itemInf.m_itemSlot[0];  // 不正時の措置
  }

  return m_data.m_itemInf.m_itemSlot[index];
}


//=============================================================================
//@brief PGLから受信済みのアイテム情報を削除
//=============================================================================
void GameSyncSave::DeleteItem(u8 index)
{
  GFL_ASSERT( m_data.m_itemInf.m_itemNum > index );

  for( u8 i = index; i < (m_data.m_itemInf.m_itemNum-1); i++ ){
    m_data.m_itemInf.m_itemSlot[i] = m_data.m_itemInf.m_itemSlot[i+1];
    //末尾のクリア
    m_data.m_itemInf.m_itemSlot[i+1].m_id  = ITEM_DUMMY_DATA;
    m_data.m_itemInf.m_itemSlot[i+1].m_num = 0;
  }
  m_data.m_itemInf.m_itemNum--;
}

//=============================================================================
//@brief オートゲームシンクが有効かどうか
//=============================================================================
bool GameSyncSave::IsAutoGameSyncSaveEnable(void) const
{
  return m_data.m_config.m_isEnableAutoSync;
}


//=============================================================================
//@brief マイレージ送るが有効かどうか
//=============================================================================
bool GameSyncSave::IsSendingMileageEnable(void) const
{
  return m_data.m_config.m_isSendingMileage;
}


//=============================================================================
//@brief PGL登録済みかどうか true:登録済み false:未登録
//=============================================================================
bool GameSyncSave::IsSignedPGL(void) const
{
  return m_data.m_registInf.m_isSigned;
}

//=============================================================================
/**
 * @brief PGL登録済みを変更
 * @param[in] true:登録済み false:未登録
 */
//=============================================================================
void GameSyncSave::SetSignedPGL( bool isSigned )
{
  m_data.m_registInf.m_isSigned = isSigned;
}

//=============================================================================
//@brief PGLから受け取ったアイテムを登録
//=============================================================================
bool GameSyncSave::PushItem( u16 itemId, u16 itemNum )
{
  if( m_data.m_itemInf.m_itemNum < STOCK_ITEM_MAX )
  {
    if( ( itemId > 0 && itemId <= ITEM_DATA_MAX ) && itemNum > 0 )
    {
      StockItem* stockInf = &m_data.m_itemInf.m_itemSlot[m_data.m_itemInf.m_itemNum];
      stockInf->m_id  = itemId;
      stockInf->m_num = itemNum;
      m_data.m_itemInf.m_itemNum++;

      return true;
    }
  }

  return false;
}

//=============================================================================
//@brief アイテムを削除（巻き戻し用）
//=============================================================================
void GameSyncSave::PopItem(void)
{
  if( m_data.m_itemInf.m_itemNum > 0 )
  {
    m_data.m_itemInf.m_itemNum--;
    StockItem* stockInf = &m_data.m_itemInf.m_itemSlot[m_data.m_itemInf.m_itemNum];
    stockInf->m_id  = 0;
    stockInf->m_num = 0;
  }
}

//=============================================================================
/**
 * @brief ゲームシンクした時間を更新
 */
//=============================================================================
void GameSyncSave::UpdateDateTime( void )
{
  m_backupDateInf = m_data.m_dateInf;

  GameSys::DeviceDate date;

  m_data.m_dateInf.m_year = date.GetYear() % 100;
  m_data.m_dateInf.m_month = date.GetMonth();
  m_data.m_dateInf.m_day = date.GetDay();
  m_data.m_dateInf.m_hour = date.GetHour();
  m_data.m_dateInf.m_minute = date.GetMinute();
}

//=============================================================================
/**
 * @brief ゲームシンクした時間をコミット
 */
//=============================================================================
void GameSyncSave::CommitDateTime( void )
{
  //gfl::std::MemClear( &m_backupDateInf, sizeof(m_backupDateInf) );
  ::std::memset( &m_backupDateInf , 0 , sizeof(m_backupDateInf) );
}

//=============================================================================
/**
 * @brief ゲームシンクした時間を巻き戻し
 */
//=============================================================================
void GameSyncSave::RollbackDateTime( void )
{
  if( m_backupDateInf.m_year != 0 )
  {
    m_data.m_dateInf = m_backupDateInf;
    m_backupDateInf.m_year = 0;
  }
}


//! @brief 初回説明フラグの設定
void GameSyncSave::SetDispedFirstGuideFlag( bool bFlag )
{
  m_data.m_config.m_isDispedFirstGuide = ( bFlag ) ? 1 : 0;
}


//=============================================================================
//@brief PGL未登録でオートゲームシンクを試みた時のアラートフラグ取得
//=============================================================================
bool GameSyncSave::GetIsNoRegistPGL(void) const
{
  if( m_data.m_config.m_isNoRegistPGL == 0 )
  {//アラートなし
    return false;
  }
  else
  {//アラートあり
    return true;
  }
}

//=============================================================================
//@brief PGL未登録でオートゲームシンクを試みた時のアラートフラグ設定
//=============================================================================
void GameSyncSave::SetIsNoRegistPGL( bool flag )
{
  if( flag )
  {
    m_data.m_config.m_isNoRegistPGL = 1;
  }
  else
  {
    m_data.m_config.m_isNoRegistPGL = 0;
  }
}

//=============================================================================
//@brief ゲームシンクアプリでゲームシンクIDを作成したかどうか
//=============================================================================
bool GameSyncSave::GetIsCreateGameSyncSaveID(void) const
{
  if( m_data.m_config.m_isUploaded == 0 )
  {//未作成
    return false;
  }
  else
  {//作成済み
    return true;
  }
}

//=============================================================================
//@brief ゲームシンク初回接続フラグ取得
//=============================================================================
bool GameSyncSave::GetGameSyncFirstConnected(void) const
{
  return true;//初回強制ゲームシンクはなしになったのでかならずtrue
}

//=============================================================================
//@brief ゲームシンク初回接続フラグ設定
//=============================================================================
void GameSyncSave::SetGameSyncFirstConnected( bool flag )
{
  if( flag )
  {
    m_data.m_config.m_isGameSyncFirstConnected = 1;
  }
  else
  {
    m_data.m_config.m_isGameSyncFirstConnected = 0;
  }
}

//=============================================================================
//@brief 5xxエラーが発生したのでゲームシンクを1時間ロックする（大会時はロックしない）
//=============================================================================
void GameSyncSave::LockGameSyncSaveFor5xxError(void)
{
#if defined(GF_PLATFORM_CTR)
  m_lockStartMinutes = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMinutes();
#endif
}

//=============================================================================
//@brief 5xxエラーによるロック確認（1時間経過していたら解除） true : ロック中、false : ロックしてない
//=============================================================================
bool GameSyncSave::CheckLockGameSyncSaveFor5xxError(void)
{
#if defined(GF_PLATFORM_CTR)
  if( m_lockStartMinutes >= 0 )
  {
    s64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMinutes();

    if( currentTime - m_lockStartMinutes > LOCK_MINUTES )
    {//解除
      m_lockStartMinutes = -1;
    }
  }

  return m_lockStartMinutes != -1;
#else
  return false;
#endif
}


//=============================================================================
//@brief SaveDataInterface実装
//=============================================================================

/**
 *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
 */
void* GameSyncSave::GetData(void)
{
  return reinterpret_cast<void*>(&m_data);
}

/**
 *  @brief    データサイズ
 *  @retval   バイト
 */
size_t GameSyncSave::GetDataSize(void)
{
  return sizeof( GameSyncSaveData );
}

/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ
 */
void GameSyncSave::Clear( gfl2::heap::HeapBase * /*heap*/ )
{
  ::std::memset( &m_data , 0 , sizeof(m_data) );
  ::std::memset( &m_backupDateInf , 0 , sizeof(m_backupDateInf) );

  //初期値設定
  m_data.m_config.m_isEnableAutoSync = true;
  m_data.m_config.m_isSendingMileage = true;

  m_lockStartMinutes = -1;
}


GFL_NAMESPACE_END(savedata)
