//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file FieldEncount.cpp
 *  @brief エンカウントチェック
 *  @author tomoya takahashi / hosaka genya
 *  @date 2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#include "Field/FieldRo/include/Encount/FieldEncount.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/PokemonSearch/FieldPokemonSearchDataAccessor.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/include/Event/EventSymbolEncount.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// gamesys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// system
#include "System/include/GflUse.h"

// savedata
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/EventWork.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

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

// コンストラクタ
Encount::Encount(Fieldmap* p_fieldmap, gfl2::fs::BinLinkerAccessor* pEncountDataPackAccessor) : 
  m_pFieldmap(p_fieldmap),
  m_pPlayer(p_fieldmap->GetPlayerCharacter()),
  m_pEncountDataPackAccessor(pEncountDataPackAccessor),
  m_pHeap( NULL),
  m_pSymbolEncountParam( NULL),
  m_symbolEncountDataSize( 0),
  m_symbolEncountParamNum( 0),
  m_seq( 0)
{
}

// デストラクタ
Encount::~Encount()
{
}

//----------------------------------------------------------------------------
/** 
 *  @brief  シンボルエンカウントコール<static>
 *
 *  @param  p_gman
 *  @param  symbol_id
 *  @param  call_option enum EventBattleCallOption
 *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
 *
 *  @retval true  バトルイベント起動
 *  @retval false バトルイベント起動なし
 */
//-----------------------------------------------------------------------------
void Encount::CallSymbolEncount( GameSys::GameManager* p_gman, u32 symbol_id, u32 call_option, u32 ret_bgm_id /*=Sound::SOUND_ITEM_ID_NONE*/ )
{
  Fieldmap* pFieldmap = p_gman->GetFieldmap();
  Attr attr = ATTR_KUSAMURA;  //当たり障りのないアトリビュート
  Field::weather::WeatherKind wether = Field::weather::SUNNY;
  PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
  poke_set.Clear();
 
  if( pFieldmap != NULL ){
    //フィールドがあるので足元アトリビュートを取る
    attr = pFieldmap->GetPlayerCharacter()->GetGroundAttributeLastSuccesful();
    //現在の天候を取る
    wether = p_gman->GetFieldmap()->GetWeatherControl()->GetNowWeatherKind();
  }

  //エンカウントポケモンセット用パラメータ初期化
  poke_set.Initialize( p_gman, TYPE_FORCE, attr, wether, false );
  
  // エンカウント
  {
    EventSymbolEncount* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventSymbolEncount>( p_gman->GetGameEventManager() );

    // イベントにパラメータを通達
    p_event->SetParam( &poke_set, symbol_id, call_option, ret_bgm_id );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  初期化
 *  @param  pHeap 読み込みに使用するヒープ 
 *
 *  @fix GFNMCat[1222][1223] ポケモンサーチのエンカウント回避チェックにシンボルエンカウントデータが必要
 *  それを読み込み、保持しておくため、初期化、初期化待ち、破棄の処理を追加
 */
//-----------------------------------------------------------------------------
void Encount::Initialize( gfl2::heap::HeapBase* pHeap)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pFieldmap->GetGameManager()->GetAsyncFileManager();

  m_pHeap = pHeap;
  m_seq = 0;

  // 非同期arcオープンリクエスト
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId           = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile     = m_pHeap->GetLowerHandle();
    openReq.heapForArcSrc   = m_pHeap->GetLowerHandle();
    openReq.heapForReq      = m_pHeap->GetLowerHandle();

    pAsyncFileManager->AddArcFileOpenReq( openReq);
  }
  {
    // 非同期読み込みリクエスト
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId       = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId       = GARC_script_event_data_symbol_encount_BIN;
    loadReq.ppBuf       = reinterpret_cast<void**>( &m_pSymbolEncountParam);
    loadReq.pBufSize    = &m_symbolEncountDataSize;
    loadReq.heapForBuf  = m_pHeap;
    loadReq.align       = 4;
    loadReq.heapForReq  = m_pHeap->GetLowerHandle();
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq);
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化待ち
*  @retval true 完了
*  @retval false 実行中
 */
//-----------------------------------------------------------------------------
bool Encount::WaitInitialize()
{
  enum Sequence
  {
    WAIT,
    WAIT_CLOSE,
    END,
  };

  gfl2::fs::AsyncFileManager* pAsyncFileManager = m_pFieldmap->GetGameManager()->GetAsyncFileManager();

  switch( m_seq)
  {
  case WAIT:
    if( pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) )
    {
      if( pAsyncFileManager->IsArcFileLoadDataFinished( reinterpret_cast<void**>( &m_pSymbolEncountParam)) )
      {
        m_symbolEncountParamNum = m_symbolEncountDataSize / sizeof( SymbolEncountParam );

        {
          // 非同期クローズリクエスト
          gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
          closeReq.arcId      = ARCID_SCRIPT_EVENT_DATA;
          closeReq.heapForReq = m_pHeap->GetLowerHandle();
          pAsyncFileManager->AddArcFileCloseReq( closeReq );
        }
        m_seq++;
      }
    }
    break;

  case WAIT_CLOSE:
    if( pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA) )
    {
      m_seq++;
      return true;
    }
    break;

  case END:
    return true;
    //break;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  破棄
 */
//-----------------------------------------------------------------------------
void Encount::Terminate()
{
  m_seq = 0;
  m_symbolEncountParamNum = 0;
  m_symbolEncountDataSize = 0;
  m_pHeap = NULL;
  GflHeapSafeFreeMemory( m_pSymbolEncountParam );
}

//----------------------------------------------------------------------------
/**
 *  @brief  強制エンカウントバトルコール
 *  @param  p_gman GameManager
 *  @param  encountDataID エリアごとに固めたエンカウントテーブルパック内のインデックス
 *  @param  call_option enum EvBattleOptionFlag
 *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
 */
//-----------------------------------------------------------------------------
bool Encount::CallForceEncount( GameSys::GameManager* p_gman, u32 encountDataID, bool isFishing, u32 call_option, u32 ret_bgm_id)
{
  u32 monsNo = this->LotteryEncountPoke( p_gman, encountDataID, isFishing)->monsNo;
  if( monsNo == MONSNO_NULL)
  {
    return false;
  }

  PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
  GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();
  EventBattleCall::CallWildEx( p_eventman, p_gman, poke_set, call_option, ret_bgm_id );

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウントポケモンの抽選までを行う
 *  @param  p_gman GameManager
 *  @param  encountDataID エリアごとに固めたエンカウントテーブルパック内のインデックス
 *  @param  call_option enum EvBattleOptionFlag
 *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
 *  @return 抽選したポケモンのエンカウントパラメータ
 *  @note   抽選したポケモンNoはPokeSet::GetTopMonsNoからも取得できます
 */
//-----------------------------------------------------------------------------
const EncountPokeParam* Encount::LotteryEncountPoke( GameSys::GameManager* p_gman, u32 encountDataID, bool isFishing, BattleEffectId btl_eff)
{
  const ENCOUNT_DATA* cp_encdata = this->GetEncountData( encountDataID);
  PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
  bool res = this->InitializeForForceEncount(p_gman, cp_encdata, poke_set, isFishing);

  if (res == false)
  {
    return NULL;
  }

  res = false;
  if (isFishing)
  {
    res = poke_set.SetNormalEncountData(*cp_encdata, BATTLE_EFFECT_WILD_WATER);
  }
  else
  {
    res = poke_set.SetNormalEncountData(*cp_encdata, btl_eff);
  }
  if( res == false)
  {
    GFL_ASSERT( 0); // エンカウントデータが存在し、強制エンカウントならばここは通らないはず
    return NULL;
  }

  return poke_set.GetTopMonsEncountParam();
}

//----------------------------------------------------------------------------
/**
  *  @brief  指定したエンカウントデータの最初のポケモンと、指定レベルでエンカウントするようにセット
  *
  *  @note   現時点で「きのみエンカウント」のみに利用する予定
  */
//----------------------------------------------------------------------------
void Encount::SetEncountPokeFromFirstRecord( GameSys::GameManager* p_gman, u32 encountDataID, u32 level, bool isFishing)
{
  const ENCOUNT_DATA* cp_encdata = this->GetEncountData( encountDataID);
  PokeSet& poke_set = p_gman->GetGameData()->GetEncountWork()->GetPokeSet();
  bool res = this->InitializeForForceEncount(p_gman, cp_encdata, poke_set, isFishing);

  if(res == false)
  {
    GFL_ASSERT(0); // エンカウントデータが存在し、強制エンカウントならばここは通らないはず
    return;
  }

  // テーブル一番最初のポケモンと指定レベルでエンカウント
  MonsNo monsNo;
  FormNo form;
  poke_set.GetFirstMonsInfo(*cp_encdata, &monsNo, &form);

  // @fix 取得したモンスターNoが不正ではないかチェック
  GFL_ASSERT_STOP( monsNo != MONSNO_NULL );

  poke_set.SetEncountDataFromParam(monsNo, form, level, poke_set.GetBattleEffectId());

  // @fix GFNMCat[1268] エンカウントデータが未設定状態で乱入が呼ばれていたのでエンカウントデータをセットする
  poke_set.SetEncountData( *cp_encdata);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ギミック系強制エンカウントで共通初期化処理
 */
//-----------------------------------------------------------------------------
bool Encount::InitializeForForceEncount(GameSys::GameManager* p_gman, const ENCOUNT_DATA* cp_encdata, PokeSet& poke_set, bool isFishing)
{
  poke_set.Clear();

  // 自機の足元のアトリビュートチェック
  Attr attr = m_pPlayer->GetGroundAttribute();

  //エンカウントデータが必要
  if( cp_encdata == NULL)
  {
    GFL_ASSERT( 0); // 開発中に気付く不具合。配置情報に含まれたID(=エリアに必ず含まれているデータ)を受け取る
    return false;
  }

  //エンカウントパターン取得
  const ENCPROB_PATTERN* cp_encpat = GetEncProbPattern( cp_encdata );
  if( cp_encpat == NULL)
  {
    GFL_ASSERT( 0); // 開発中に気付く不具合
    return false;
  }

  poke_set.Initialize( p_gman, TYPE_FORCE, attr, p_gman->GetFieldmap()->GetWeatherControl()->GetNowWeatherKind(), isFishing);

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  通常野生ランダムエンカウントチェック
 *
 *  @param  type    エンカウントタイプ
 *
 *  @retval true  バトルイベント起動
 *  @retval false バトルイベント起動なし
 */
//-----------------------------------------------------------------------------
bool Encount::CheckEncount( Encount::Type enc_type, u32 encountDataID, Attr attr /* = ATTR_ERROR */ )
{
  GameSys::GameManager* p_gman = m_pFieldmap->GetGameManager();
  GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();
  GameSys::GameData* p_gamedata = p_gman->GetGameData();

  MoveModel::FieldMoveModel* p_model = m_pPlayer;

  const ENCOUNT_DATA* cp_encdata;

  PokeSet& poke_set = p_gamedata->GetEncountWork()->GetPokeSet();
  poke_set.Clear();
  
  // ランダムでエンカウント

  if (attr == ATTR_ERROR)
  {
    // 自機の足元のアトリビュートチェック
    attr = p_model->GetGroundAttribute();
  }

  //イベント状態によるエンカウント有効チェック
  if( this->IsEncountEnable( p_gman ) == false ) return false;

  //エンカウントデータの有無
  cp_encdata = GetEncountData( encountDataID);
  //エンカウントデータが必要
  if( cp_encdata == NULL ){ return false; }

  //エンカウントパターンとロケーション取得
  {
    u32 per = 0;
    const ENCPROB_PATTERN* cp_encpat;
    
    //エンカウントパターン取得
    cp_encpat = GetEncProbPattern( cp_encdata );

    //エンカウントパターンが無効(=ロケーションのエンカウントデータが無効)ならエンカウントしない
    if( cp_encpat == NULL ){ return false; }

    //デフォルトエンカウント率取得
    per = p_gamedata->GetEncountWork()->GetNowEncountProb( cp_encpat );
    if( (per == 0) && enc_type != TYPE_FORCE ){ return false; }

    //エンカウントポケモンセット用パラメータ初期化
    poke_set.Initialize( p_gman, enc_type, attr, p_gman->GetFieldmap()->GetWeatherControl()->GetNowWeatherKind(), false );

    //道具&特性によるエンカウント率変動計算
    per = poke_set.EncountProbCalc(per, m_pPlayer);
    IWASAWA_PRINT( "Encount Per = %d \n", per );
    SAITA_PRINT( "Encount Per = %d \n", per );
  
    //100基準でエンカウント
    if( enc_type != TYPE_FORCE )
    {
      if( System::GflUse::GetPublicRand( 100 ) >= per ){ return false; }
    }
  }

  // @fix GFNMCat[1222][1223] 必ずSetNormalEncountを行っていたので、ポケモンサーチか通常かで処理を分けました。
  // ポケモンサーチ処理はPokeSetに専用のエンカウント回避関数を作成してそれを使用しています。

  // ポケモンサーチエンカウントアクセサー作成
  FieldResident*    pResident         = p_gman->GetFieldResident();
  void*             pPokemonSearchBin = pResident->GetResource( BL_IDX_FIELD_RESIDENT__POKEMON_SEARCH_PACK);
  const EventWork*  pEventWork        = p_gamedata->GetEventWork();
  u32               areaID            = m_pFieldmap->GetAreaID();
  PokemonSearch::DataAccessor pokemonSearchDataAccessor( pPokemonSearchBin);

  // ポケモンサーチエンカウントかどうかを判定
  bool isPokemonSearchEncountCall = pokemonSearchDataAccessor.IsSearchBattleCall( areaID, encountDataID, pEventWork);

  // ポケモンサーチエンカウント
  if( isPokemonSearchEncountCall )
  {
    if( m_pSymbolEncountParam == NULL )
    {
      GFL_ASSERT( 0);
      return false;
    }

    u32 symbolEncountID = pokemonSearchDataAccessor.GetSymbolEncountDataID();
    if( symbolEncountID >= m_symbolEncountParamNum)
    {
      GFL_ASSERT_MSG( 0, "SymbolEncount table index is over. : %d >= %d", symbolEncountID, m_symbolEncountParamNum);
      return false;
    }

    u32 encPokeLevel = m_pSymbolEncountParam[ symbolEncountID ].level;
    if( poke_set.IsPokemonSearchEncountAvoid( encPokeLevel))
    {
      return false;
    }

    pokemonSearchDataAccessor.CallSearchBattle( p_gman);
  }
  // 通常エンカウント
  // ノーマルエンカウントデータ生成 + 抽選
  else
  {
    if( poke_set.SetNormalEncountData( *cp_encdata ) == false )
    {
      return false;
    }

    EventBattleCall::CallWild( p_eventman, p_gman, poke_set, EVBTL_CALLOPT_NOT_SCRIPT | EVBTL_CALLOPT_ATTR_POKESET );
  }

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  通常野生ランダムエンカウント率制御(振り向きを含む移動時に呼び出す)
 *
 *  @param  move_f     一歩移動フラグ（振り向きを含まない）
 *  @param  ex_move_f  波乗り開始時などの特殊イベント一歩フラグ
 */
//-----------------------------------------------------------------------------
void Encount::CalcEncountProb( Attr attr, bool move_f, bool ex_move_f, u32 encountDataID, u8 attrNearBGPattern )
{
  GameSys::GameManager* p_gman = m_pFieldmap->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  EncountWork* p_encwork = p_gdata->GetEncountWork();

  const ENCOUNT_DATA* cp_encdata = GetEncountData( encountDataID);

  if( move_f || ex_move_f)
  {
    // 移動したらエンカウント率リセットチェックをかける
    bool isEncount = ( encountDataID == ENCOUNT_DATA_ID_ERROR );
    p_encwork->CheckProbReset( attr, isEncount, attrNearBGPattern);
  }
  
  if( cp_encdata == NULL ) { return; }
  
  //イベント状態によるエンカウント有効チェック
  if( this->IsEncountEnable( p_gman ) == false ) { return; }

  //パターンデータ取得
  p_encwork->CalcEncountProb( move_f, ex_move_f, GetEncProbPattern( cp_encdata ) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  通常野生ランダムエンカウント率制御データ取得
 */
//-----------------------------------------------------------------------------
const ENCPROB_PATTERN* Encount::GetEncProbPattern( const ENCOUNT_DATA * cp_encdata ) const
{
#if PM_DEBUG
  if( Debug::DebugTypes::s_isEncountDebugEnable)
  {
    return &Debug::DebugTypes::s_encountProbPattern;
  }
#endif // PM_DEBUG

  static const ENCPROB_PATTERN DATA_EncProbPattern[] =
  {
    // up,inteval, max, safety
    {  2,  3, 13,  8 },  // 草
    {  2,  5, 13, 12 },  // 長草
    {  2, 13,  7, 30 },  // 洞窟
    {  2, 12,  7, 12 },  // 砂漠
    {  2, 20,  7, 135},  // 水上
    {  2, 12,  7,  8 },  // 雪上
    {  2, 12,  7,  8 },  // 室内
    {  2,  9, 13,  8 },  // 花園
    {  2, 10,  7, 90 },  // 洞窟２
  };

  GFL_ASSERT_STOP( cp_encdata->probPattern < GFL_NELEMS( DATA_EncProbPattern) );

  return &DATA_EncProbPattern[ cp_encdata->probPattern ];
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウント有効チェック
 */
//-----------------------------------------------------------------------------
bool Encount::IsEncountEnable( GameSys::GameManager* p_gman ) const
{
  const EventWork* pEventWork = p_gman->GetGameData()->GetEventWork();

  if( pEventWork->CheckEventFlag( SYS_FLAG_FIELD_ENCOUNT_OPEN))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  エンカウントデータポインタ取得
 *  @param  packIndex パック内の番号。各エンカウント範囲配置情報からエンカウントテーブルIDを取得して渡す
 */
//-----------------------------------------------------------------------------
const ENCOUNT_DATA* Encount::GetEncountData( u32 packIndex) const
{
  u16 dataMax = m_pEncountDataPackAccessor->GetDataMax();
  if( packIndex >= dataMax)
  {
    return NULL;
  }

  if( m_pEncountDataPackAccessor->GetDataSize( packIndex) <= 0)
  {
    return NULL;
  }

  return static_cast<const ENCOUNT_DATA*>( m_pEncountDataPackAccessor->GetData( packIndex));
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )
