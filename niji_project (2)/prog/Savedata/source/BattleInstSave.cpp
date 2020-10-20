//==============================================================================
/**
 * @file	  BattleInstSave.cpp
 * @brief		バトル施設セーブデータ（バトルツリー/ロイヤル）
 */
// =============================================================================
#include <Debug/include/gfl2_Assert.h>

#include "Savedata/include/BattleInstSave.h"

/**
 * @brief		バトルツリー/ロイヤルセーブデータ
 */
GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------
BattleInstSave::BattleInstSave( void )
{
	this->Clear( NULL );
}

//--------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------
BattleInstSave::~BattleInstSave( void )
{
}

//--------------------------------------------------------------------------
/**
 * @brief		読み込んだデータをセットする関数
 * @param		pData		先頭のポインタ
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetData(void* data)
{
	gfl2::std::MemCopy(data, &m_data, sizeof(m_data) );
}

//--------------------------------------------------------------------------
/**
 * @brief    データの先頭ポインタを得る関数
 * @return   先頭のポインタ
 */
//--------------------------------------------------------------------------
void* BattleInstSave::GetData(void)
{
	return reinterpret_cast<void*>(&m_data);
}
//--------------------------------------------------------------------------
/**
 * @brief    データサイズ
 * @retval   バイト
 */
//--------------------------------------------------------------------------
size_t BattleInstSave::GetDataSize(void)
{
  return sizeof( BATTLE_INST_DATA );
}

//--------------------------------------------------------------------------
/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ
 * @fix  BTS[1493]　内部実装が未完だったので追加
 */
//--------------------------------------------------------------------------
void BattleInstSave::Clear( gfl2::heap::HeapBase * heap )
{
	GFL_UNUSED(heap);
  gfl2::std::MemClear( &m_data, this->GetDataSize() );
  m_data.TreeBattleBgm = NO_BTL_BGM;

  for( u32 i = 0; i < SCOUT_TRAINER_MAX; ++i)
  {
    ClearScoutTrainerIndex(i);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数インクリメント
 * @param type
 */
//--------------------------------------------------------------------------
void BattleInstSave::AddWinCount( const BattleTreeType type, const BattleTreeRank rank )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    IncWin( &m_data.TreeSingleWin[rank], &m_data.TreeSingleWinMax[rank] );
    break;
  case BATTLE_TREE_TYPE_DOUBLE:
    IncWin( &m_data.TreeDoubleWin[rank], &m_data.TreeDoubleWinMax[rank] );
    break;
  case BATTLE_TREE_TYPE_MULTI:
    IncWin( &m_data.TreeMultiWin[rank], &m_data.TreeMultiWinMax[rank] );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数クリア
 * @param type
 */
//--------------------------------------------------------------------------
void BattleInstSave::ClearWinCount( const BattleTreeType type, const BattleTreeRank rank )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    m_data.TreeSingleWin[rank] = 0;
    break;
  case BATTLE_TREE_TYPE_DOUBLE:
    m_data.TreeDoubleWin[rank] = 0;
    break;
  case BATTLE_TREE_TYPE_MULTI:
    m_data.TreeMultiWin[rank] = 0;
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数セット
 * @param type
 * @param super
 * @param val
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetWinCount( const BattleTreeType type, const BattleTreeRank rank, u16 val )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    SetWin( &m_data.TreeSingleWin[rank], &m_data.TreeSingleWinMax[rank], val );
    break;
  case BATTLE_TREE_TYPE_DOUBLE:
    SetWin( &m_data.TreeDoubleWin[rank], &m_data.TreeDoubleWinMax[rank], val );
    break;
  case BATTLE_TREE_TYPE_MULTI:
    SetWin( &m_data.TreeMultiWin[rank], &m_data.TreeMultiWinMax[rank], val );
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数取得
 * @param type
 * @retval int 連勝数
 */
//--------------------------------------------------------------------------
u16 BattleInstSave::GetWinCount( const BattleTreeType type, const BattleTreeRank rank )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    return m_data.TreeSingleWin[rank];
  case BATTLE_TREE_TYPE_DOUBLE:
    return m_data.TreeDoubleWin[rank];
  case BATTLE_TREE_TYPE_MULTI:
    return m_data.TreeMultiWin[rank];
  }
  GFL_ASSERT(0);
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief 最大連勝数取得
 * @param type
 * @retval int 連勝数
 */
//--------------------------------------------------------------------------
u16 BattleInstSave::GetMaxWinCount( const BattleTreeType type, const BattleTreeRank rank )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    return m_data.TreeSingleWinMax[rank];
  case BATTLE_TREE_TYPE_DOUBLE:
    return m_data.TreeDoubleWinMax[rank];
  case BATTLE_TREE_TYPE_MULTI:
    return m_data.TreeMultiWinMax[rank];
  }
  GFL_ASSERT(0);
  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数インクリメント
 * @param now
 * @param max
 */
//--------------------------------------------------------------------------
void BattleInstSave::IncWin( u16 *now, u16 *max )
{
  if ( (*now) < MAX_COUNT )
  {
    (*now)++;
    if ( (*now) > (*max) )
    {
      (*max) = (*now);
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数セット
 * @param val
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetPrizeWinCount( u16 val )
{
  if( m_data.TreePrizeWinCount < val )
  {
    m_data.TreePrizeWinCount = val;
    if ( m_data.TreePrizeWinCount > MAX_COUNT ) m_data.TreePrizeWinCount = MAX_COUNT;
  }
}

//--------------------------------------------------------------------------
/**
* @brief 連勝数(ご褒美用)を取得
*/
//--------------------------------------------------------------------------
u16 BattleInstSave::GetPrizeWinCount()
{
  return m_data.TreePrizeWinCount;
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数クリア
 * @param val
 */
//--------------------------------------------------------------------------
void BattleInstSave::ResetPrizeWinCount()
{
  m_data.TreePrizeWinCount = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief 連勝数セット
 * @param now
 * @param max
 * @param val
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetWin( u16 *now, u16 *max, u16 val )
{
  (*now) = val;
  if ( (*now) > MAX_COUNT ) (*now) = MAX_COUNT;

  if ( (*now) > (*max) )
  {
    (*max) = (*now);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief 戦闘ＢＧＭのセット
 * @param bgm
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetBattleBgm( int bgm )
{
  m_data.TreeBattleBgm = bgm;
}

//--------------------------------------------------------------------------
/**
 * @brief 戦闘ＢＧＭを設定なしにする
 */
//--------------------------------------------------------------------------
void BattleInstSave::ResetBattleBgm( void )
{
  m_data.TreeBattleBgm = NO_BTL_BGM;
}

//--------------------------------------------------------------------------
/**
 * @brief 戦闘ＢＧＭの取得
 * @retval int
 */
//--------------------------------------------------------------------------
int BattleInstSave::GetBattleBgm( void )
{
  return m_data.TreeBattleBgm;
}

//--------------------------------------------------------------------------
/**
 * @brief 戦闘ＢＧＭが設定されているか？
 * @retval bool
 */
//--------------------------------------------------------------------------
bool BattleInstSave::IsSetBattleBgm( void )
{
  if ( m_data.TreeBattleBgm != NO_BTL_BGM ) return true;
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 挑戦ランクとバトルタイプのセット
 * @param type
 * @param rank
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetBattleTypeAndRank( const BattleTreeType type, const BattleTreeRank rank )
{
  m_data.TreeType = type;
  m_data.TreeRank = rank;
}

//--------------------------------------------------------------------------
/**
 * @brief 挑戦ランクとバトルタイプのリセット
 */
//--------------------------------------------------------------------------
void BattleInstSave::ResetBattleTypeAndRank( void )
{
  m_data.TreeType = BATTLE_TREE_TYPE_MAX;
  m_data.TreeRank = BATTLE_TREE_RANK_NORMAL;
}

//--------------------------------------------------------------------------
/**
 * @brief 挑戦ランク取得
 * @retval super
 */
//--------------------------------------------------------------------------
BattleTreeRank BattleInstSave::GetRank( void )
{
  return m_data.TreeRank;
}

//--------------------------------------------------------------------------
/**
 * @brief 挑戦バトルタイプ取得
 * @retval type
 */
//--------------------------------------------------------------------------
BattleTreeType BattleInstSave::GetBattleType( void )
{
  return m_data.TreeType;
}

//--------------------------------------------------------------------------
/**
 * @brief カウンター最大値取得
 * @retval int
 */
//--------------------------------------------------------------------------
int BattleInstSave::GetCountMax( void )
{
  return MAX_COUNT;
}

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナー情報が一杯か？
 */
//--------------------------------------------------------------------------
bool BattleInstSave::IsFullScoutTrainerData() const
{
  s16 index = this->GetEmptyScoutTrainerIndex();
  if( index == SCOUT_TRAINER_MAX)
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 登録されているスカウトトレーナー情報の数を取得
 */
//--------------------------------------------------------------------------
u32 BattleInstSave::GetRegistScoutTrainerDataNum() const
{
  return this->GetEmptyScoutTrainerIndex();
}

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナー情報の追加
 * @param trainerID トレーナーデータID
 * @param pokeID1 手持ちポケモン1匹目のデータID
 * @param pokeID2 手持ちポケモン2匹目のデータID
 * @param tokuseiIndex1 手持ちポケモン1匹目の特性インデックス
 * @param tokuseiIndex2 手持ちポケモン2匹目の特性インデックス
 * @note このメソッドで指しているIDはバトルタワーデータメーカーのIDです
 */
//--------------------------------------------------------------------------
void BattleInstSave::AddScoutTrainerData( u16 trainerID, u16 pokeID1, u16 pokeID2,u8 tokuseiIndex1, u8 tokuseiIndex2 )
{
  // NMCat[1048] バトルツリーで同じトレーナーをスカウトできる問題を修正（同一トレーナーの場合はデータを上書き）
  u16 index = SearchRegistTrainerIndex( trainerID );
  if( index == SCOUT_TRAINER_MAX )
  {
    index = this->GetEmptyScoutTrainerIndex();

    // 未登録のトレーナーを追加しようとしているが空きがない
    if( index == SCOUT_TRAINER_MAX )
    {
      GFL_ASSERT_MSG( 0,"BattleInstSave : ScoutTrainerData is Full\n");
      return;
    }
  }

  this->SetScoutTrainerData( index, trainerID, pokeID1, pokeID2, tokuseiIndex1, tokuseiIndex2 );

}

//--------------------------------------------------------------------------
/**
 * @brief 指定したトレーナーが登録されているか検索
 * @param trainerID トレーナーデータID
 * @return 登録されているトレーナーのインデックス(登録されていない場合はSCOUT_TRAINER_MAX)
 */
//--------------------------------------------------------------------------
u16 BattleInstSave::SearchRegistTrainerIndex( u16 trainerID ) const
{
  if( trainerID == SCOUT_TRAINER_ERROR )
  {
    GFL_ASSERT_MSG( 0, "trainerID = SCOUT_TRAINER_ERROR\n");
    return SCOUT_TRAINER_MAX;
  }

  for( u32 i = 0; i < SCOUT_TRAINER_MAX; ++i )
  {
    if( m_data.ScoutTrainerID[i] == trainerID )
    {
      return i;
    }
  }
  return SCOUT_TRAINER_MAX;
}

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナー情報のセット
 * @param index どこにセットするか
 * @param trainerID トレーナーデータID
 * @param pokeID1 手持ちポケモン1匹目のデータID
 * @param pokeID2 手持ちポケモン2匹目のデータID
 * @param tokuseiIndex1 手持ちポケモン1匹目の特性インデックス
 * @param tokuseiIndex2 手持ちポケモン2匹目の特性インデックス
 * @note このメソッドで指しているIDはバトルタワーデータメーカーのIDです
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetScoutTrainerData( u32 index, u16 trainerID, u16 pokeID1, u16 pokeID2, u8 tokuseiIndex1, u8 tokuseiIndex2 )
{
  if( index >= SCOUT_TRAINER_MAX)
  {
    GFL_ASSERT_MSG( 0, "Over index\n");
    return;
  }

  m_data.ScoutTrainerID[index]      = trainerID;
  m_data.ScoutTrainerPokeID1[index] = pokeID1;
  m_data.ScoutTrainerPokeID2[index] = pokeID2;
  m_data.ScoutTrainerTokuseiIndex1[index] = tokuseiIndex1;
  m_data.ScoutTrainerTokuseiIndex2[index] = tokuseiIndex2;
}

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナー情報の取得
 * @param index どこから取得するか
 * @param pOutTrainerID トレーナーデータID格納先
 * @param pOutPokeID1 手持ちポケモン1匹目のデータID格納先
 * @param pOutPokeID2 手持ちポケモン2匹目のデータID格納先
 * @note このメソッドで指しているIDはバトルタワーデータメーカーのIDです
 */
//--------------------------------------------------------------------------
void BattleInstSave::GetScoutTrainerData( u32 index, u16* pOutTrainerID, u16* pOutPokeID1, u16* pOutPokeID2, u8* pOutTokuseiIndex1, u8* pOutTokuseiIndex2 ) const
{
  if( index >= SCOUT_TRAINER_MAX)
  {
    GFL_ASSERT_MSG( 0, "Over index\n");
    return;
  }

  *pOutTrainerID  = m_data.ScoutTrainerID[index];
  *pOutPokeID1    = m_data.ScoutTrainerPokeID1[index];
  *pOutPokeID2    = m_data.ScoutTrainerPokeID2[index];
  *pOutTokuseiIndex1 = m_data.ScoutTrainerTokuseiIndex1[index];
  *pOutTokuseiIndex2 = m_data.ScoutTrainerTokuseiIndex2[index];
}

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナーが存在しているか
 * @param trainerID トレーナーID
 * @retval true 存在している
 * @retval true 存在していない
 */
//--------------------------------------------------------------------------
bool BattleInstSave::IsExistScoutTrainerData( u16 trainerID ) const
{
  return ( this->SearchRegistTrainerIndex( trainerID ) != SCOUT_TRAINER_MAX );
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数加算
 */
//--------------------------------------------------------------------------
void BattleInstSave::AddWinCountRoyal()
{
  if( m_data.RoyalWinCount < BATTLE_ROYAL_WIN_COUNT_MAX)
  {
    m_data.RoyalWinCount++;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数クリア
 */
//--------------------------------------------------------------------------
void BattleInstSave::ClearWinCountRoyal()
{
  m_data.RoyalWinCount = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数取得
 */
//--------------------------------------------------------------------------
u16 BattleInstSave::GetWinCountRoyal() const
{
  return m_data.RoyalWinCount;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルマスターランク連勝数が最大か
 * @retval true 最大(=ボス戦)
 * @retval false それ以外
 */
//--------------------------------------------------------------------------
bool BattleInstSave::IsMaxWinCountRoyal() const
{
  return ( m_data.RoyalWinCount >= BATTLE_ROYAL_WIN_COUNT_MAX );
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤルランク解放
 * @note マスターランクまで解放されていた場合は何もしない
 */
//--------------------------------------------------------------------------
void BattleInstSave::ReleaseNextRankRoyal()
{
  if( m_data.RoyalRank < BATTLE_ROYAL_RANK_MASTER)
  {
    m_data.RoyalRank++;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤル解放ランク取得
 * @return 解放されているランク。スーパーまで解放されていればスーパーが返る
 */
//--------------------------------------------------------------------------
BattleRoyalRank BattleInstSave::GetReleaseRankRoyal() const
{
  return static_cast<BattleRoyalRank>( m_data.RoyalRank);
}

  // デバッグ用途
#if PM_DEBUG

//--------------------------------------------------------------------------
/**
 * @brief 最大連勝数セット
 * @param type
 * @param super
 * @param val
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetWinCountMax( const BattleTreeType type, const BattleTreeRank rank, u16 val )
{
  switch(type)
  {
  case BATTLE_TREE_TYPE_SINGLE:
    m_data.TreeSingleWinMax[rank] = val;
    break;
  case BATTLE_TREE_TYPE_DOUBLE:
    m_data.TreeDoubleWinMax[rank] = val;
    break;
  case BATTLE_TREE_TYPE_MULTI:
    m_data.TreeMultiWinMax[rank] = val;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}


//--------------------------------------------------------------------------
/**
 * @brief ロイヤル連勝数セット
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetWinCountRoyal( u16 count)
{
  m_data.RoyalWinCount = count;
}

//--------------------------------------------------------------------------
/**
 * @brief ロイヤル解放ランクセット
 */
//--------------------------------------------------------------------------
void BattleInstSave::SetReleaseRankRoyal( BattleRoyalRank rank)
{
  m_data.RoyalRank = rank;
}

#endif

//--------------------------------------------------------------------------
/**
 * @brief スカウトトレーナーの空き番号取得
 * @return 空き番号
 * @note SCOUT_TRAINER_MAXが返ってきたら空きがない
 */
//--------------------------------------------------------------------------
u16 BattleInstSave::GetEmptyScoutTrainerIndex() const
{
  for( u32 i = 0; i < SCOUT_TRAINER_MAX; ++i)
  {
    if( m_data.ScoutTrainerID[i] == SCOUT_TRAINER_ERROR)
    {
      return i;
    }
  }
  return SCOUT_TRAINER_MAX;
}

//--------------------------------------------------------------------------
/**
 * @brief 特定のインデックスのスカウトトレーナーをクリア
 * @param index　スカウトトレーナーのインデックス
 */
//--------------------------------------------------------------------------
void BattleInstSave::ClearScoutTrainerIndex( int index )
{
  m_data.ScoutTrainerID[index]      = SCOUT_TRAINER_ERROR;
  m_data.ScoutTrainerPokeID1[index] = SCOUT_TRAINER_ERROR;
  m_data.ScoutTrainerPokeID2[index] = SCOUT_TRAINER_ERROR;
  m_data.ScoutTrainerTokuseiIndex1[index] = 0;
  m_data.ScoutTrainerTokuseiIndex2[index] = 0;
}

GFL_NAMESPACE_END(Savedata)

