//==============================================================================
/**
 * @file    PokeListStaticWork.h
 * @author  fukushima_yuya
 * @date    2015.03.05
 * @brief   リスト画面データ
 */
//==============================================================================

// niji
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/AppToolTimeLimit.h>
#include <Battle/Regulation/include/PokeRegulation.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyItemSave.h>      // MYITEM
#include <Savedata/include/ZukanSave.h>       // Zukan

// arc
#include <arc_def_index/arc_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   変数
//------------------------------------------------------------------------------
//! @brief  カーソル位置ごとの初期位置
const StaticWork::CursorPosDesc StaticWork::INIT_CPOS_DESC[ CPOS_NUM ] = {
  { PLATE_00, },    //!< CPOS_PLATE_TL
  { PLATE_01, },    //!< CPOS_PLATE_TR
  { PLATE_02, },    //!< CPOS_PLATE_ML
  { PLATE_03, },    //!< CPOS_PLATE_MR
  { PLATE_04, },    //!< CPOS_PLATE_BL
  { PLATE_05, },    //!< CPOS_PLATE_BR
  { PLATE_NULL, },  //!< CPOS_DECIDE
};

//! @brief  プレートごとの設定
const StaticWork::PlateDesc StaticWork::PLATE_DESC[ PLATE_NUM ] = {
  { WBTN_00, 0, },  //!< PLATE_00
  { WBTN_01, 1, },  //!< PLATE_01
  { WBTN_02, 2, },  //!< PLATE_02
  { WBTN_03, 3, },  //!< PLATE_03
  { WBTN_04, 4, },  //!< PLATE_04
  { WBTN_05, 5, },  //!< PLATE_05
};

// @fix GFNMCat[1366]
struct MultiData
{
  PartyID   partyID;
  u8        pokeIndex;
};

// 通常
static const MultiData c_MultiData_Base[] = {
  { PARTY_SELF, 0 },  //!< 左上
  { PARTY_SELF, 1 },  //!< 右上
  { PARTY_SELF, 2 },  //!< 左中
  { PARTY_SELF, 3 },  //!< 右中
  { PARTY_SELF, 4 },  //!< 左下
  { PARTY_SELF, 5 },  //!< 右下
};

// マルチ時の自分左配置
static const MultiData c_MultiData_Left[] = {
  { PARTY_SELF,   0 },  //!< 左上
  { PARTY_FRIEND, 0 },  //!< 右上
  { PARTY_SELF,   1 },  //!< 左中
  { PARTY_FRIEND, 1 },  //!< 右中
  { PARTY_SELF,   2 },  //!< 左下
  { PARTY_FRIEND, 2 },  //!< 右下
};
// マルチ時の自分右配置
static const MultiData c_MultiData_Right[] = {
  { PARTY_FRIEND, 0 },  //!< 左上
  { PARTY_SELF,   0 },  //!< 右上
  { PARTY_FRIEND, 1 },  //!< 左中
  { PARTY_SELF,   1 },  //!< 右中
  { PARTY_FRIEND, 2 },  //!< 左下
  { PARTY_SELF,   2 },  //!< 右下
};



//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pGameMan      ゲームマネージャ
 * @param   pHeap         ヒープ
 * @param   procContext   プロセスのコンテキスト
 * @param   pResult       
 * @param   pTimeLimit    制限時間
 * @param   pCmnIcon      状態異常アイコン
 */
//------------------------------------------------------------------------------
StaticWork::StaticWork( GameSys::GameManager* pGameMan, gfl2::heap::HeapBase* pHeap,
  CONTEXT_PARAM* procContext, App::Tool::TimeLimit* pTimeLimit, app::tool::AppCommonGrpIconData* pCmnIcon )
  : m_game_man( pGameMan )
  , m_proc_mode( MODE_FIELD )
  , m_mode_context()
  , m_removed_wazano( WAZANO_NULL )
  , m_field_wazano( WAZANO_NULL )
  , m_time_limit( pTimeLimit )
  , m_saveZukanData( NULL )
  , m_pContext( procContext )
  , m_pCommonIcon( pCmnIcon )
  //, m_pokeRegulation( NULL )
  , m_NowCursorID( CID_POKE )
  , m_UseWazaPoke( CPOS_NULL )
{
  // 図鑑データの取得
  if( m_game_man )
  {
    GameSys::GameData* pGameData = m_game_man->GetGameData();
    m_saveZukanData = pGameData->GetZukanData();
  }

  for( u8 i=0; i<CID_NUM; ++i )
  {
    m_cursor_pos[i] = CPOS_NULL;
  }

  for( u8 i=0; i<PARTY_NUM; ++i )
  {
    m_poke_party[i] = NULL;
  }

  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_joinPokeIndex[i] = m_pContext->join_order[i];
    m_wazaSetParam[i] = m_pContext->waza_set_param[i];
    m_simple2SetParam[i] = m_pContext->poke_select_enable[i];
  }

  m_poke_party[ PARTY_SELF ]   = &m_pContext->my_party;
  m_poke_party[ PARTY_FRIEND ] = &m_pContext->friend_party;
  m_poke_party[ PARTY_ENEMY0 ] = &m_pContext->enemy_party_0;
  m_poke_party[ PARTY_ENEMY1 ] = &m_pContext->enemy_party_1;

  m_my_status[ PARTY_SELF ]   = m_pContext->my_status;
  m_my_status[ PARTY_FRIEND ] = m_pContext->friend_status;
  m_my_status[ PARTY_ENEMY0 ] = m_pContext->enemy_status_0;
  m_my_status[ PARTY_ENEMY1 ] = m_pContext->enemy_status_1;

  m_proc_mode = m_pContext->mode;
  this->CreateRegulation( m_pContext, pHeap );
  this->SetupModeContext( m_pContext );
  this->InitCposDesc();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
StaticWork::~StaticWork( void )
{
  this->DeleteRegulation();
}



//------------------------------------------------------------------------------
/**
 * @brief   動作モードの取得
 * @return  動作モード
 */
//------------------------------------------------------------------------------
Mode StaticWork::GetProcMode( void ) const
{
  return m_proc_mode;
}

//------------------------------------------------------------------------------
/**
 * @brief   モードコンテキスト共用体の取得
 * @return  モードコンテキスト共用体
 */
//------------------------------------------------------------------------------
const ModeContext& StaticWork::GetModeContext( void ) const
{
  return m_mode_context;
}

//------------------------------------------------------------------------------
/**
 * @brief   モードコンテキスト共用体のポインタの取得
 * @return  モードコンテキスト共用体ポインタ
 */
//------------------------------------------------------------------------------
ModeContext* StaticWork::GetModeContextPointer( void )
{
  return &m_mode_context;
}



//------------------------------------------------------------------------------
/**
 * @brief   パーティを取得
 * @param   party_id   パーティID
 * @return  ポケモンパーティ
 */
//------------------------------------------------------------------------------
IntermediateData* StaticWork::GetPokeParty( PartyID party_id ) const
{
  if( party_id >= PARTY_NUM )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  //return m_poke_party[party_id];
  return m_poke_party[party_id];
}


//------------------------------------------------------------------------------
/**
 * @brief   メンバー数を取得
 *
 * @param   partyID   パーティID
 *
 * @return  メンバー数
 */
//------------------------------------------------------------------------------
u32 StaticWork::GetMemberCount( PartyID partyID ) const
{
  if( partyID >= PARTY_NUM )
  {
    GFL_ASSERT(0);
    return 1;
  }

#ifdef DEBUG_ONLY_FOR_fukushima_yuya
  const char* partyNameTable[] = {
    "MYSELF",
    "FRIEND",
    "ENEMY0",
    "ENEMY1",
  };
  FUKUSHIMA_PRINT( "GetMemberCount : Name[%s] = [%d]\n",
    partyNameTable[partyID], this->GetPokeParty(partyID)->GetMemberCount() );
#endif

  return this->GetPokeParty(partyID)->GetMemberCount();
}

//------------------------------------------------------------------------------
/**
 * @brief   メンバー数を取得
 *
 * @return  メンバー数
 */
//------------------------------------------------------------------------------
u32 StaticWork::GetMemberCount( void ) const
{
  // マルチバトル時
  if( this->IsMultiBattle() )
  {
    // 自分＋味方
    u32 memberCount = this->GetMemberCount( PARTY_SELF ) + this->GetMemberCount( PARTY_FRIEND );
    {
      if( memberCount > pml::PokeParty::MAX_MEMBERS )
      {
        memberCount = pml::PokeParty::MAX_MEMBERS;
      }
    }
    return memberCount;
  }

  return this->GetMemberCount( PARTY_SELF );
}


//------------------------------------------------------------------------------
/**
 * @brief   指定したカーソル位置にいるポケモンのインデックスを取得する
 * @param   cpos    カーソル位置
 * @param   ポケモンのインデックス
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetPokeIndex( CursorPos cpos ) const
{
  if( cpos >= CPOS_NUM )
  {
    GFL_ASSERT(0);
    cpos = CPOS_PLATE_TL;
  }

  PlateID plate_id = m_cpos_desc[cpos].plateID;

  return this->GetPokeIndex( plate_id );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したプレートにいるポケモンのインデックスを取得する
 * @param   plate_id   プレートID
 * @return  ポケモンのインデックス
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetPokeIndex( PlateID plate_id ) const
{
  if( plate_id >= PLATE_NUM )
  {
    GFL_ASSERT(0);
    plate_id = PLATE_00;
  }

  return PLATE_DESC[plate_id].poke_index;
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のカーソル位置にいるポケモンのインデックスを取得する
 * @param   cid   カーソルID
 * @return  ポケモンのインデックス
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetPokeIndexOnCursor( CursorID cid ) const
{
  CursorPos cpos = GetCursorPos( cid );
  
  return this->GetPokeIndex( cpos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンを取得する
 * @param   party_id    パーティID
 * @param   poke_index  ポケモンのインデックス
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* StaticWork::GetPoke( PartyID party_id, u8 poke_index ) const
{
  if( party_id >= PARTY_NUM )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  if( m_poke_party[party_id] == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  //return m_poke_party[party_id]->GetMemberPointer( poke_index );
  return m_poke_party[party_id]->GetPoke( poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のカーソル位置に対応するポケモンを取得する
 * @param   cid   カーソルID
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* StaticWork::GetPokeOnCursor( CursorID cid ) const
{
  u8 poke_index = this->GetPokeIndexOnCursor( cid );

  return this->GetPoke( PARTY_SELF, poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したのカーソル位置に対応するポケモンを取得する
 * @param   cpos    カーソル位置
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* StaticWork::GetPoke( CursorPos cpos ) const
{
  // マルチバトルなら専用関数で取得
  if( this->IsMultiBattle() )
  {
    return this->GetPokeMulti( cpos );
  }

  u8 poke_index = this->GetPokeIndex( cpos );
  return this->GetPoke( PARTY_SELF, poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したのプレートに対応するポケモンを取得する
 * @param   plate_id   プレートID
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* StaticWork::GetPoke( PlateID plate_id ) const
{
  CursorPos cpos = this->GetCursorPos( plate_id );

  return this->GetPoke( cpos );
}

// @fix GFNMCat[1366]
//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの取得：マルチバトル用
 *
 * @param   cpos    カーソル位置
 *
 * @return  ポケモンパラメータ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* StaticWork::GetPokeMulti( CursorPos cpos ) const
{
  u8 btlMode = GetModeContext().battle.btl_mode;

  if( btlMode == BTL_MODE_MULTI_LEFT )
  {
    return this->GetPoke(c_MultiData_Left[cpos].partyID, c_MultiData_Left[cpos].pokeIndex);
  }
  else if( btlMode == BTL_MODE_MULTI_RIGHT )
  {
    return this->GetPoke(c_MultiData_Right[cpos].partyID, c_MultiData_Right[cpos].pokeIndex);
  }

  // ここへはこない
  GFL_ASSERT(0);
  u8 pokeIndex = this->GetPokeIndex( cpos );
  {
    return this->GetPoke( PARTY_SELF, pokeIndex );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンを取得する
 * @param   party_id    パーティID
 * @param   poke_index  ポケモンのインデックス
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetPokeConst( PartyID party_id, u8 poke_index ) const
{
  if( party_id >= PARTY_NUM )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  if( m_poke_party[party_id] == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  //return m_poke_party[party_id]->GetMemberPointer( poke_index );
  return m_poke_party[party_id]->GetPokeConst( poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のカーソル位置に対応するポケモンを取得する
 * @param   cid   カーソルID
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetPokeConstOnCursor( CursorID cid ) const
{
  u8 poke_index = this->GetPokeIndexOnCursor( cid );

  return this->GetPokeConst( PARTY_SELF, poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したのカーソル位置に対応するポケモンを取得する
 * @param   cpos    カーソル位置
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetPokeConst( CursorPos cpos ) const
{
  // マルチバトルなら専用関数で取得
  if( this->IsMultiBattle() )
  {
    return this->GetPokeConstMulti( cpos );
  }

  u8 poke_index = this->GetPokeIndex( cpos );
  return this->GetPokeConst( PARTY_SELF, poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したのプレートに対応するポケモンを取得する
 * @param   plate_id   プレートID
 * @return  ポケモンのパラメータ
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetPokeConst( PlateID plate_id ) const
{
  CursorPos cpos = this->GetCursorPos( plate_id );

  return this->GetPokeConst( cpos );
}

// @fix GFNMCat[1366]
//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの取得：マルチバトル用
 *
 * @param   cpos    カーソル位置
 *
 * @return  ポケモンパラメータ：const
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetPokeConstMulti( CursorPos cpos ) const
{
  u8 btlMode = GetModeContext().battle.btl_mode;

  if( btlMode == BTL_MODE_MULTI_LEFT )
  {
    return this->GetPokeConst(c_MultiData_Left[cpos].partyID, c_MultiData_Left[cpos].pokeIndex);
  }
  else if( btlMode == BTL_MODE_MULTI_RIGHT )
  {
    return this->GetPokeConst(c_MultiData_Right[cpos].partyID, c_MultiData_Right[cpos].pokeIndex);
  }

  // ここへはこない
  GFL_ASSERT(0);
  u8 pokeIndex = this->GetPokeIndex( cpos );
  {
    return this->GetPokeConst( PARTY_SELF, pokeIndex );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   指定したプレートのポケモンがタマゴかどうかを調べる
 * @param   plate_id   プレートID
 * @return  trueならタマゴ
 */
//------------------------------------------------------------------------------
bool StaticWork::IsEggOnPlate( PlateID plate_id ) const
{
  const pml::pokepara::PokemonParam* pPokeParam = this->GetPokeConst( plate_id );

  return pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のカーソル位置にいるポケモンがタマゴかどうかを調べる
 * @param   cid   カーソルID
 * @return  trueならタマゴ
 */
//------------------------------------------------------------------------------
bool StaticWork::IsEggOnCursor( CursorID cid ) const
{
  CursorPos cpos = this->GetCursorPos( cid );
  PlateID plate_id = this->GetPlateID( cpos );

  return this->IsEggOnPlate( plate_id );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したカーソル位置にいるポケモンがタマゴかどうかを調べる
 * @param   cpos    カーソル位置
 * @return  trueならタマゴ
 */
//------------------------------------------------------------------------------
bool StaticWork::IsEgg( CursorPos cpos ) const
{
  const pml::pokepara::PokemonParam* pPokeParam = this->GetPokeConst( cpos );

  return pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
}

//------------------------------------------------------------------------------
/**
 * @brief   猛毒かどうか
 *
 * @param   cpos    カーソル位置
 *
 * @return  "true  == 猛毒"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool StaticWork::IsMoudoku( CursorPos cpos )
{
  // マルチバトル専用
  if( IsMultiBattle() )
  {
    IntermediateData* data = NULL;
    u8 pokeIndex = 0;

    if( this->GetModeContext().battle.btl_mode == BTL_MODE_MULTI_LEFT )
    {
      data = this->GetPokeParty( c_MultiData_Left[cpos].partyID );
      pokeIndex = c_MultiData_Left[cpos].pokeIndex;
    }
    else {
      data = this->GetPokeParty( c_MultiData_Right[cpos].partyID );
      pokeIndex = c_MultiData_Right[cpos].pokeIndex;
    }

    return data->IsSickMoudoku( pokeIndex );
  }

  // マルチバトル以外
  IntermediateData* data = this->GetPokeParty( PARTY_SELF );
  {
    u8 pokeIndex = this->GetPokeIndex( cpos );
    return data->IsSickMoudoku( pokeIndex );
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置を変更する
 * @param   cid   カーソルID
 * @param   cpos  カーソル位置
 */
//------------------------------------------------------------------------------
void StaticWork::SetCursorPos( CursorID cid, CursorPos cpos )
{
  m_cursor_pos[cid] = cpos;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置を変更する
 * @param   cid   カーソルID
 * @param   waza_button_id    技ボタンID
 */
//------------------------------------------------------------------------------
void StaticWork::SetCursorPos( CursorID cid, WazaButtonID waza_button_id )
{
  PlateID plate_id = this->GetPlateID( waza_button_id );

  this->SetCursorPos( cid, plate_id );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置を変更する
 * @param   cid       カーソルID
 * @param   plate_id  プレートID
 */
//------------------------------------------------------------------------------
void StaticWork::SetCursorPos( CursorID cid, PlateID plate_id )
{
  CursorPos cpos = this->GetCursorPos( plate_id );

  this->SetCursorPos( cid, cpos );
}

//------------------------------------------------------------------------------
/**
 * @brief   現在のカーソル位置を取得する
 * @param   cid   カーソルID
 */
//------------------------------------------------------------------------------
CursorPos StaticWork::GetCursorPos( CursorID cid ) const
{
  return m_cursor_pos[cid];
}

//------------------------------------------------------------------------------
/**
 * @brief   指定したプレートが属するカーソル位置を取得する
 * @param   plate_id  プレートID
 */
//------------------------------------------------------------------------------
CursorPos StaticWork::GetCursorPos( PlateID plate_id ) const
{
  for( u8 i=0; i<CPOS_NUM; ++i )
  {
    if( m_cpos_desc[i].plateID == plate_id )
    {
      return static_cast<CursorPos>( i );
    }
  }

  GFL_ASSERT(0);  // 指定されたプレートはどのカーソル位置にも属さない

  return CPOS_NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   指定した技ボタンが属するカーソル位置を取得する
 * @param   waza_button_id    技ボタンID
 */
//------------------------------------------------------------------------------
CursorPos StaticWork::GetCursorPos( WazaButtonID waza_button_id ) const
{
  PlateID plateID = GetPlateID( waza_button_id );

  return this->GetCursorPos( plateID );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの初期位置の取得
 * return   初期位置
 */
//------------------------------------------------------------------------------
CursorPos StaticWork::GetInitCursorPos( void )
{
  if( this->GetProcMode() == MODE_BATTLE )
  {
    if( m_pContext->select_poke_addr != NULL )
    {
      for( u8 i=0; i<CPOS_PLATE_NUM; ++i )   // @fix cov_ctr[11844]
      {
        if( GetSelectCursorPos( i ) == m_pContext->select_poke_addr )
        {
          return static_cast<CursorPos>(i);
        }
      }
    }
  }

  return static_cast<CursorPos>( m_pContext->poke_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   現在動かしているカーソルIDの設定
 * @param   cid   カーソルID
 */
//------------------------------------------------------------------------------
void StaticWork::SetCursorID( CursorID cid )
{
  m_NowCursorID = cid;
}

//------------------------------------------------------------------------------
/**
 * @brief   現在動かしているカーソルIDの取得
 * @return  カーソルID
 */
//------------------------------------------------------------------------------
CursorID StaticWork::GetCursorID( void )
{
  return m_NowCursorID;
}


//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置判定に必要なポケモンパラメータのアドレスを取得
 *
 * @param   index   カーソル位置
 *
 * @return  ポケモンパラメータのアドレス
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* StaticWork::GetSelectCursorPos( u8 index )
{
  const MultiData* dataTable = c_MultiData_Base;

  ModeContext context = this->GetModeContext();
  {
    if( context.battle.btl_mode == BTL_MODE_MULTI_LEFT )
    {
      dataTable = c_MultiData_Left;
      GFL_PRINT("c_MultiData_Left\n");
    }
    else if( context.battle.btl_mode == BTL_MODE_MULTI_RIGHT )
    {
      dataTable = c_MultiData_Right;
      GFL_PRINT("c_MultiData_Right\n");
    }
    else {
      GFL_PRINT("c_MultiData_Base\n");
    }
  }

  // パーティデータの取得
  IntermediateData* data = this->GetPokeParty( dataTable[index].partyID );
  GFL_PRINT( "index[%d], partyID=[%d], addr[%x]\n"
    , dataTable[index].pokeIndex
    , dataTable[index].partyID
    , data->GetPoke_Battle( dataTable[index].pokeIndex )
  );
  // 
  return data->GetPoke_Battle( dataTable[index].pokeIndex );
}




//------------------------------------------------------------------------------
/**
 * @brief   指定したカーソル位置にあるプレートのIDを取得する
 * @param   cpos    カーソル位置
 */
//------------------------------------------------------------------------------
PlateID StaticWork::GetPlateID( CursorPos cpos ) const
{
  if( cpos >= CPOS_NUM )
  {
    GFL_ASSERT(0);
    cpos = CPOS_PLATE_TL;
  }

  return m_cpos_desc[cpos].plateID;
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートの並び順を入れ替える
 * @param   cpos0   カーソル位置(移動元)
 * @param   cpos1   カーソル位置(移動先)
 */
//------------------------------------------------------------------------------
void StaticWork::ExchangePlateOrder( CursorPos cpos0, CursorPos cpos1 )
{
  GFL_ASSERT( cpos0 < CPOS_NUM );
  GFL_ASSERT( cpos1 < CPOS_NUM );

  PlateID temp = m_cpos_desc[cpos0].plateID;

  m_cpos_desc[cpos0].plateID = m_cpos_desc[cpos1].plateID;
  m_cpos_desc[cpos1].plateID = temp;
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートの並び順を入れ替える
 * @param   plate0   プレート位置(移動元)
 * @param   plate1   プレート位置(移動先)
 */
//------------------------------------------------------------------------------
void StaticWork::ExchangePlateOrder( PlateID plate0, PlateID plate1 )
{
  CursorPos cpos0 = this->GetCursorPos( plate0 );
  CursorPos cpos1 = this->GetCursorPos( plate1 );

  this->ExchangePlateOrder( cpos0, cpos1 );
}




//------------------------------------------------------------------------------
/**
 * @brief   忘れさせる技の番号をセットする
 * @param   wazano    技番号
 */
//------------------------------------------------------------------------------
void StaticWork::SetRemovedWazaNo( WazaNo wazano )
{
  m_removed_wazano = wazano;
}

//------------------------------------------------------------------------------
/**
 * @brief   忘れさせる技を取得
 * @return  忘れさせる技の番号
 */
//------------------------------------------------------------------------------
WazaNo StaticWork::GetRemovedWazaNo( void ) const
{
  return m_removed_wazano;
}

//------------------------------------------------------------------------------
/**
 * @brief   使用するフィールド技の技番号をセットする
 * @param   wazano    技番号
 */
//------------------------------------------------------------------------------
void StaticWork::SetFieldWazaNo( WazaNo wazano )
{
  m_field_wazano = wazano;
}

//------------------------------------------------------------------------------
/**
 * @brief   使用するフィールド技の技番号を取得する
 * @return  フィールド技の技番号
 */
//------------------------------------------------------------------------------
WazaNo StaticWork::GetFieldWazaNo( void ) const
{
  return m_field_wazano;
}

//------------------------------------------------------------------------------
/**
 * @brief   先頭からのフィールド技の取得
 * @param   cpos    プレート位置
 * @param   index   先頭からのインデックス
 * @return  フィールド技の技番号
 */
//------------------------------------------------------------------------------
WazaNo StaticWork::GetFieldWazaNo( CursorPos cpos, u8 index ) const
{
  const pml::pokepara::PokemonParam* pp = GetPokeConst( cpos );
  WazaNo wazaNo = WAZANO_NULL;
  u8 count = 0;

  bool fastmode_flag = pp->StartFastMode();
  {
    u8 wazaCount = pp->GetWazaCount();  //!< 修得している技の個数

    for( u8 i=0; i<wazaCount; ++i )
    {
      wazaNo = pp->GetWazaNo( i );      //!< 技番号の取得

      if( IsFieldWaza( wazaNo ) )       //!< フィールド技かどうか
      {
        if( index == count ) break;     //!< 指定したインデックスと一致していれば現在の技番号を返す

        count++;
      }
    }
  }
  pp->EndFastMode( fastmode_flag );

  return wazaNo;
}

//------------------------------------------------------------------------------
/**
 * @brief   フィールド技かどうか
 * @param   wazaNo    技番号
 * @return  true = フィールド技、false = 通常技
 */
//------------------------------------------------------------------------------
bool StaticWork::IsFieldWaza( WazaNo wazaNo ) const
{
  if( wazaNo != WAZANO_MIRUKUNOMI &&
      wazaNo != WAZANO_TAMAGOUMI  &&
      wazaNo != WAZANO_TEREPOOTO  &&
      wazaNo != WAZANO_ANAWOHORU  &&
      wazaNo != WAZANO_AMAIKAORI)
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   技を使用するポケモンのセット
 * @param   cpos    プレート位置
 */
//------------------------------------------------------------------------------
void StaticWork::SetFieldWazaUsePoke( CursorPos cpos )
{
  m_UseWazaPoke = cpos;
}

//------------------------------------------------------------------------------
/**
 * @brief   技を使用するポケモンの取得
 * @return  プレート位置
 */
//------------------------------------------------------------------------------
CursorPos StaticWork::GetFieldWazaUsePoke( void )
{
  return m_UseWazaPoke;
}



//------------------------------------------------------------------------------
/**
 * @brief   所持道具データを取得する
 * @return  所持道具データ
 */
//------------------------------------------------------------------------------
Savedata::MyItem* StaticWork::GetMyItem( void ) const
{
  GameSys::GameData * gd = m_game_man->GetGameData();

  return gd->GetMyItem();
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの所持している道具Noの取得
 * @param   cpos    カーソル位置
 * @return  道具No
 */
//------------------------------------------------------------------------------
u16 StaticWork::GetPokeItem( CursorPos cpos )
{
  u16 item_no = ITEM_DUMMY_DATA;
  bool isEgg = false;

  pml::pokepara::PokemonParam* pPoke = GetPoke( cpos );
  bool fastmode_flag = pPoke->StartFastMode();
  {
    item_no = pPoke->GetItem();
    isEgg = pPoke->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }
  pPoke->EndFastMode( fastmode_flag );

  // タマゴの場合強制的にダミーIDにする
  if( isEgg ) item_no = ITEM_DUMMY_DATA;

  return item_no;
}




//------------------------------------------------------------------------------
/**
 * @brief   参加ポケモンを追加する
 * @param   poke_index    ポケモンのインデックス
 */
//------------------------------------------------------------------------------
void StaticWork::AddJoinPoke( u8 poke_index )
{
  u8 count = this->GetJoinPokeCount();

  GFL_ASSERT( count < pml::PokeParty::MAX_MEMBERS );
  GFL_ASSERT( this->IsJoinPoke( poke_index ) == false );

  if( count < pml::PokeParty::MAX_MEMBERS )
  {
    m_joinPokeIndex[count] = poke_index;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   参加ポケモンを除外する
 * @param   poke_index    ポケモンのインデックス
 */
//------------------------------------------------------------------------------
void StaticWork::RemoveJoinPoke( u8 poke_index )
{
  s8 index = this->GetJoinOrder( poke_index );
  u8 count = this->GetJoinPokeCount();

  if( 0 <= index )
  {
    for( u8 i=index; i<count-1; ++i )
    {
      m_joinPokeIndex[i] = m_joinPokeIndex[i+1];
    }

    for( u8 i=count-1; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      m_joinPokeIndex[i] = -1;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   追加済みの参加ポケモンの数を取得する
 * @return  参加済みのポケモンの数
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetJoinPokeCount( void ) const
{
  u8 count = 0;

  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    if( m_joinPokeIndex[i] < 0 )
    {
      break;
    }

    count++;
  }

  return count;
}

//------------------------------------------------------------------------------
/**
 * @brief   参加ポケのインデックスを取得する
 * @brief   order   参加番号
 * @return  参加済みのポケモンのインデックス
 */
//------------------------------------------------------------------------------
s8 StaticWork::GetJoinPokeIndex( u8 order ) const
{
  return m_joinPokeIndex[order];
}

//------------------------------------------------------------------------------
/**
 * @brief   参加ポケモンに含まれているか？
 * @param   poke_index    ポケモンのインデックス
 * @return  trueなら参加
 */
//------------------------------------------------------------------------------
bool StaticWork::IsJoinPoke( u8 poke_index ) const
{
  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    if( this->GetJoinPokeIndex(i) == poke_index )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   参加できるか？
 * @param   poke_index    ポケモンのインデックス
 * @return  trueなら参加可能
 */
//------------------------------------------------------------------------------
bool StaticWork::IsJoinEnable( u8 poke_index ) const
{
  GFL_ASSERT( poke_index < pml::PokeParty::MAX_MEMBERS );

  if( poke_index < pml::PokeParty::MAX_MEMBERS )
  {
    const ModeContext& context = this->GetModeContext();
    return context.join.join_enable[poke_index];
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   何番目の参加ポケか？
 * @param   poke_index    ポケモンのインデックス
 * @return  参加番号
 */
//------------------------------------------------------------------------------
s8 StaticWork::GetJoinOrder( u8 poke_index ) const
{
  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    if( this->GetJoinPokeIndex(i) == poke_index )
    {
      return i;
    }
  }

  return -1;
}

//------------------------------------------------------------------------------
/**
 * @brief   参加選択用：
 * @param   isDecidePokemon    ポケモン選択完了しているかどうか（ステータス画面に行くときはfalse）
 *          選択したポケモンをResultにセット
 *          ※終了時に再セットしてるけど、まぁ、いいか。
 */
//------------------------------------------------------------------------------
void StaticWork::SetResultJoinPokeIndex( bool isDecidePokemon )
{
  for( int i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_pContext->join_order[i] = this->GetJoinPokeIndex(i);
  }

  if( isDecidePokemon )
  {//選択決定してたら完了フラグを立てる（ステータス画面に行くときは立てない）
    m_pContext->pokelist_wait = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの習得状況を取得
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetWazaSetParam( u8 poke_index ) const
{
  return m_wazaSetParam[poke_index];
}

//------------------------------------------------------------------------------
/**
 * @brief   対象選択２の選択可否状況を取得
 */
//------------------------------------------------------------------------------
u8 StaticWork::GetSimple2SelectEnable( u8 poke_index ) const
{
  return m_simple2SetParam[poke_index];
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間ありかどうか
 */
//------------------------------------------------------------------------------
bool StaticWork::IsTimeLimitMode( void ) const
{
  if( m_mode_context.join.time_limit_s == 0 ) return false;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間超過かどうか
 * @return  true    超過
 * @return  false   それ以外
 */
//------------------------------------------------------------------------------
bool StaticWork::IsTimeLimit( void )
{
  // 参加選択モードじゃない
  if( m_proc_mode != MODE_JOIN ) return false;
  if( m_time_limit == NULL ) return false;

  return m_time_limit->Update();
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間(秒)の取得
 */
//------------------------------------------------------------------------------
u32 StaticWork::GetTimeLimit( void ) const
{
  if( m_time_limit == NULL ) return 0;

  return m_time_limit->GetLimitSecond();
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間チェッククラスのポインタの取得
 */
//------------------------------------------------------------------------------
App::Tool::TimeLimit* StaticWork::GetTimeLimitClass( void )
{
  return m_time_limit;
}

//------------------------------------------------------------------------------
/**
 * @brief MYSTATUSを取得する
 * @param partyID     取得対象が所属するパーティ
 */
//------------------------------------------------------------------------------
Savedata::MyStatus* StaticWork::GetMyStatus( PartyID party_id ) const
{
  GFL_ASSERT( party_id < PARTY_NUM );

  return m_my_status[party_id];
}

//------------------------------------------------------------------------------
/**
 * @brief   図鑑データの取得
 * @return  図鑑データ
 */
//------------------------------------------------------------------------------
Savedata::ZukanData* StaticWork::GetZukanData( void )
{
  return m_saveZukanData;
}

//------------------------------------------------------------------------------
/**
 * @brief   レギュレーションチェック
 * @return  REGULATION_OKなら正常終了
 * @return  それ以外ならエラー
 */
//------------------------------------------------------------------------------
int StaticWork::CheckRegulation( gfl2::heap::HeapBase* heap )
{
  // レギュレーションの生成チェック
  if( m_pokeRegulation == NULL )
  {
    return PokeRegulation::RET_OK;
  }

  pml::PokeParty* partyData = GFL_NEW_LOW(heap) pml::PokeParty( heap );
  {
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      s8 pokeIndex = m_joinPokeIndex[i];
      {
        if( pokeIndex == -1 ) continue;

        const pml::pokepara::PokemonParam* pp = GetPokeConst( PARTY_SELF, pokeIndex );
        {
          if( pp == NULL ) continue;

          partyData->AddMember( *pp );
        }
      }
    }
  }

  // レギュレーションチェック
  PokeRegulation::PokelistCheckReturn result =
    m_pokeRegulation->CheckPokePartyForPokelist( partyData );

  GFL_DELETE partyData;

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief   レギュレーションで設定された伝説ポケモンの数を取得
 *
 * @return  ポケモンの数
 */
//------------------------------------------------------------------------------
u32 StaticWork::GetRegulationLegendNum( void )
{
  Regulation* regulation = m_pContext->regulation;

  if( regulation )
  {
    // レギュレーションで設定された伝説ポケモンの上限数
    return regulation->GetValueParam( Regulation::LEGEND_NUM_HI );
  }

  return 0;
}

//------------------------------------------------------------------
/**
 * @brief   レギュレーションで設定された見せ合いフラグを取得
 *
 * @return  見せ合いフラグ(trueで見せる falseで見せない）
 */
//------------------------------------------------------------------
bool StaticWork::GetRegulationShowPoke( void )
{
  Regulation* regulation = m_pContext->regulation;

  if( regulation )
  {
    return regulation->GetBoolParam( Regulation::SHOW_POKE );
  }

//  GFL_ASSERT(0); //GFNMCat[1927]対応のためAssertを外す
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   参加選択ポケモンの自動セット
 */
//------------------------------------------------------------------------------
void StaticWork::SetJoinOrderTimeOut( void )
{
  if( m_pokeRegulation )
  {
    s8 destOrder[pml::PokeParty::MAX_MEMBERS];
    {
      // タイムアウト時の自動セット
      m_pokeRegulation->SetJoinOrderTimeOutForPokelist(
        GetPokeParty( PARTY_SELF )->GetPokeParty(), m_joinPokeIndex, destOrder );
    }

    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      FUKUSHIMA_PRINT( "Order[%d] : [%d] -> [%d]\n", i, m_joinPokeIndex[i], destOrder[i] );
      m_joinPokeIndex[i] = destOrder[i];
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   外部からのポケモンリスト画面終了要求チェック
 * @return  true = 終了要求あり、false = 終了要求なし
 */
//------------------------------------------------------------------------------
bool StaticWork::IsListBreak( void )
{
  if( m_proc_mode == MODE_BATTLE )
  {
    return GFL_BOOL_CAST( *m_mode_context.battle.list_break );
  }
  else if( m_proc_mode == MODE_JOIN )
  {
    return GFL_BOOL_CAST( *m_mode_context.join.list_break );
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了モードのセット
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void StaticWork::SetEndMode( EndMode endMode )
{
  m_pContext->end_mode = endMode;
}


//------------------------------------------------------------------------------
/**
 * @brief   ゲームマネージャの取得
 * @return  ゲームマネージャ
 */
//------------------------------------------------------------------------------
GameSys::GameManager* StaticWork::GetGameManager( void ) const
{
  return m_game_man;
}

//------------------------------------------------------------------------------
/**
 * @brief   状態異常アイコンの取得
 * @return  app::tool::AppCommonGrpIconDataポインタ
 */
//------------------------------------------------------------------------------
app::tool::AppCommonGrpIconData* StaticWork::GetCommonIcon( void ) const
{
  return m_pCommonIcon;
}


// @fix GFNMCat[1366]
//------------------------------------------------------------------------------
/**
 * @brief   マルチバトルかどうか
 *
 * @return  "true  == マルチバトル"
 * @return  "false == それ以外"
 */
//------------------------------------------------------------------------------
bool StaticWork::IsMultiBattle( void ) const
{
  // マルチバトルかどうか
  if( GetProcMode() == MODE_BATTLE )
  {
    u8 btlMode = GetModeContext().battle.btl_mode;

    if( btlMode == BTL_MODE_MULTI_LEFT || btlMode == BTL_MODE_MULTI_RIGHT )
    {
      // マルチバトル
      return true;
    }
  }

  // それ以外
  return false;
}


// @fix GFNMCat[1363]
//------------------------------------------------------------------------------
/**
 * @brief   サウンドを鳴らすかどうか
 *
 * @return  "true  == 鳴らす"
 * @return  "false == 鳴らさない"
 */
//------------------------------------------------------------------------------
bool StaticWork::IsPlaySound( void ) const
{
  if( m_pContext )
  {
    return m_pContext->play_sound;
  }

  return true;
}




//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext( CONTEXT_PARAM* proc_context )
{
  void ( StaticWork::*MODE_CONTEXT_SETUP_FUNC[MODE_NUM] )( CONTEXT_PARAM* ) = {
    &StaticWork::SetupModeContext_FIELD,        //!< MODE_FIELD
    &StaticWork::SetupModeContext_JOIN,         //!< MODE_JOIN
    &StaticWork::SetupModeContext_SODATEYA,     //!< MODE_SODATEYA
    &StaticWork::SetupModeContext_SIMPLE,       //!< MODE_SIMPLE
    &StaticWork::SetupModeContext_SIMPLE,       //!< MODE_SIMPLE2
    &StaticWork::SetupModeContext_WAZA,         //!< MODE_WAZAOSHIE
    &StaticWork::SetupModeContext_BATTLE,       //!< MODE_BATTLE
    &StaticWork::SetupModeContext_PROMOTION,    //!< MODE_PROMOTION
    &StaticWork::SetupModeContext_FIELD,        //!< MODE_COLOSSEUM
  };
  ( this->*MODE_CONTEXT_SETUP_FUNC[proc_context->mode] )( proc_context );
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(通常のリストモード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_FIELD( CONTEXT_PARAM* proc_context )
{
  GFL_UNUSED( proc_context );

  //m_mode_context.field.skillUseWork = proc_context->skillUseWork;
  m_mode_context.field.pokelist_wait = &proc_context->pokelist_wait;
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(参加ポケモン選択モード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_JOIN( CONTEXT_PARAM* proc_context )
{
  m_mode_context.join.cancel_enable = GFL_BOOL_CAST( proc_context->cancel_flag );
  m_mode_context.join.join_upp_mode = proc_context->join_upp_mode;
  m_mode_context.join.join_poke_count_min = proc_context->join_count_min;
  m_mode_context.join.join_poke_count_max = proc_context->join_count_max;

  m_mode_context.join.list_break    = &proc_context->list_break;    // 外部から終了を受け取るフラグ
  m_mode_context.join.pokelist_wait = &proc_context->pokelist_wait; // 外部に待機中を通知するフラグ

  m_mode_context.join.selected_wait = proc_context->selected_wait;
  m_mode_context.join.qrcode = proc_context->qrcord;

  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    const pml::pokepara::PokemonParam* poke = this->GetPokeConst( PARTY_SELF, i );
    m_mode_context.join.join_enable[i] = this->IsPokemonJoinEnable( poke );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(対象選択モード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_SIMPLE( CONTEXT_PARAM* proc_context )
{
  GFL_UNUSED( proc_context );

  m_mode_context.simple.cancel_enable = true;
  m_mode_context.simple.pokelist_wait = &proc_context->pokelist_wait;
  m_mode_context.simple.send_box_flag = proc_context->send_box_flag;

  for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_mode_context.simple.poke_select_enable[i] = GFL_BOOL_CAST( proc_context->poke_select_enable[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(育て屋モード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_SODATEYA( CONTEXT_PARAM* proc_context )
{
  GFL_UNUSED( proc_context );

  m_mode_context.sodateya.pokelist_wait = &proc_context->pokelist_wait;
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(技覚え選択モード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_WAZA( CONTEXT_PARAM* proc_context )
{
  GFL_UNUSED( proc_context );

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
  {
    m_mode_context.waza.waza_set_param[i] = proc_context->waza_set_param[i];
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(バトルモード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_BATTLE( CONTEXT_PARAM* proc_context )
{
  m_mode_context.battle.cancel_flag     = GFL_BOOL_CAST( proc_context->cancel_flag );
  m_mode_context.battle.pokelist_wait   = &proc_context->pokelist_wait;
  m_mode_context.battle.list_break      = &proc_context->list_break;
  m_mode_context.battle.btl_mode        = proc_context->btl_mode;
  m_mode_context.battle.btl_change_mode = reinterpret_cast<u8*>(&proc_context->btl_change_mode);
  m_mode_context.battle.from_top        = proc_context->from_top;
}

//------------------------------------------------------------------------------
/**
 * @brief   モード固有のコンテキストをセットアップする(プロモ選択モード)
 * @param   proc_context  プロセスのコンテキスト
 */
//------------------------------------------------------------------------------
void StaticWork::SetupModeContext_PROMOTION( CONTEXT_PARAM* proc_context )
{
  GFL_UNUSED( proc_context );
}


//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置の初期化
 */
//------------------------------------------------------------------------------
void StaticWork::InitCposDesc( void )
{
  for( u8 i=0; i<CPOS_NUM; ++i )
  {
    m_cpos_desc[i] = INIT_CPOS_DESC[i];
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   プレートIDの取得
 * @param   waza_button_id  技ボタンID
 * @return  プレートのID
 */
//------------------------------------------------------------------------------
PlateID StaticWork::GetPlateID( WazaButtonID waza_button_id ) const
{
  for( u8 i=0; i<PLATE_NUM; ++i )
  {
    if( PLATE_DESC[i].wazaButtonID == waza_button_id )
    {
      return static_cast<PlateID>(i);
    }
  }

  GFL_ASSERT(0);    //!< 指定された技ボタンがどのプレートにも属さない
  
  return PLATE_NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンが参加できるかどうかを調べる
 * @param   poke   チェック対象のポケモン
 * @retval  true   参加できる
 * @retval  false  参加できない
 */
//------------------------------------------------------------------------------
bool StaticWork::IsPokemonJoinEnable( const pml::pokepara::PokemonParam* pPokeParam )
{
  //if( m_pokeRegulation != NULL )
  //{
  //  return this->CheckRegulation( pPokeParam );
  //}

  if( pPokeParam == NULL || pPokeParam->IsNull() ) return false;

  return !pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
}


//------------------------------------------------------------------------------
/**
 * @brief   レギュレーション生成
 * @param   proc_context  プロセスコンテキスト
 * @param   pHeap         ヒープ
 */
//------------------------------------------------------------------------------
void StaticWork::CreateRegulation( CONTEXT_PARAM* proc_context, gfl2::heap::HeapBase* pHeap )
{
  // 参加選択モードじゃない or レギュレーションなし
  if( proc_context->mode != MODE_JOIN || proc_context->regulation == NULL ) return;

  m_pokeRegulation = GFL_NEW(pHeap) PokeRegulation( pHeap );
  m_pokeRegulation->SetRegulation( proc_context->regulation );
}

//------------------------------------------------------------------------------
/**
 * @brief   レギュレーションの破棄
 */
//------------------------------------------------------------------------------
void StaticWork::DeleteRegulation( void )
{
  if( m_pokeRegulation )
  {
    GFL_SAFE_DELETE( m_pokeRegulation );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   パーティ内の並びをプレートの並びに一致させる
 */
//------------------------------------------------------------------------------
void StaticWork::SyncPokeOrder( void )
{
  IntermediateData* pData = this->GetPokeParty( PARTY_SELF );
  pml::PokeParty* pParty = pData->GetPokeParty();
  u8 memberCount = this->GetMemberCount();

  for( u8 i=0; i<memberCount; ++i )
  {
    CursorPos cpos_i = static_cast<CursorPos>(i);
    u8 pokeIndex_i = GetPokeIndex( cpos_i );

    if( pokeIndex_i == i ) continue;

    for( u8 j=i+1; j<memberCount; ++j )
    {
      CursorPos cpos_j = static_cast<CursorPos>(j);
      u8 pokeIndex_j = GetPokeIndex( cpos_j );

      if( pokeIndex_j == i )
      {
        pParty->ExchangePosition( pokeIndex_i, pokeIndex_j );
        ExchangePlateOrder( cpos_i, cpos_j );
        break;
      }
    }
  }
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
