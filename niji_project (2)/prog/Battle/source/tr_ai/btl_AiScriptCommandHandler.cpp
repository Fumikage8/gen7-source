/**
 * @file btl_AiScriptCommandHandler.cpp
 * @brief 戦闘AIスクリプトコマンド制御
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#include "../btl_common.h"
#include "../btl_mainmodule.h"
#include "../btl_ServerFlow.h"
#include "../btl_pokecon.h"
#include "../btl_pokeparam.h"
#include "./tr_ai_cmd.h"
#include "./btl_BattleAiSystem.h"
#include "./btl_AiScriptCommandHandler.h"


GFL_NAMESPACE_BEGIN( btl )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heapHandle  使用するヒープ
 * @param mainModule  参照する MainModule
 * @param serverFlow  参照する ServerFlow
 * @param pokecon     全ポケモンを保持するコンテナ
 * @param randContext 使用する乱数コンテキスト
 */
//---------------------------------------------------------------------------
AiScriptCommandHandler::AiScriptCommandHandler(
    HeapHandle heapHandle,
    const MainModule* mainModule,
    ServerFlow* serverFlow,
    const POKECON* pokecon,
    const u32 randSeed ) : 
  m_mainModule( mainModule ),
  m_serverFlow( serverFlow ),
  m_pokecon( pokecon ),
  m_commandParam(),
  m_isEscapeSelected( false ),
  m_randGenerator( NULL )
{
  m_randGenerator = GFL_NEW( heapHandle ) AIRandSys();
  m_randGenerator->Initialize( randSeed );

  for( u32 pokeId=0; pokeId<BTL_POKEID_MAX; ++pokeId ) {
    for( u32 wazaIndex=0; wazaIndex<PTL_WAZA_MAX; ++wazaIndex ) {
      m_usedWaza[ pokeId ][ wazaIndex ] = WAZANO_NULL;
    }
  }
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
AiScriptCommandHandler::~AiScriptCommandHandler()
{
  GFL_SAFE_DELETE( m_randGenerator );
}



//---------------------------------------------------------------------------
/**
 * @brief コマンドの挙動を決定するパラメータを設定する
 */
//---------------------------------------------------------------------------
void AiScriptCommandHandler::SetCommandParam( const CommandParam& commandParam )
{
  m_commandParam = commandParam;
}



//---------------------------------------------------------------------------
/**
 * @brief 乱数ジェネレータを取得する
 */
//---------------------------------------------------------------------------
AIRandSys* AiScriptCommandHandler::GetRandGenerator( void ) const
{
  return m_randGenerator;
}

//---------------------------------------------------------------------------
/**
 * @brief MainModule を取得する
 */
//---------------------------------------------------------------------------
const MainModule* AiScriptCommandHandler::GetMainModule( void ) const
{
  return m_mainModule;
}

//---------------------------------------------------------------------------
/**
 * @brief ServerFlow を取得する
 */
//---------------------------------------------------------------------------
ServerFlow* AiScriptCommandHandler::GetServerFlow( void ) const
{
  return m_serverFlow;
}

//---------------------------------------------------------------------------
/**
 * @brief POKECON を取得する
 */
//---------------------------------------------------------------------------
const POKECON* AiScriptCommandHandler::GetPokeCon( void ) const
{
  return m_pokecon;
}

//---------------------------------------------------------------------------
/**
 * @brief 攻撃側ポケモンのパラメータを取得する
 */
//---------------------------------------------------------------------------
const BTL_POKEPARAM* AiScriptCommandHandler::GetAttackPokeParam( void ) const
{
  return m_commandParam.attackPoke;
}

//---------------------------------------------------------------------------
/**
 * @brief 防御側ポケモンのパラメータを取得する
 * @retval NULL  防御側ポケモンが登録されていない場合
 */
//---------------------------------------------------------------------------
const BTL_POKEPARAM* AiScriptCommandHandler::GetDefensePokeParam( void ) const
{
  return m_commandParam.defensePoke;
}

//---------------------------------------------------------------------------
/**
 * @brief 攻撃側ポケモンの立ち位置を取得する
 * @retval BTL_POS_NULL  攻撃側ポケモンが登録されていない場合
 */
//---------------------------------------------------------------------------
BtlPokePos AiScriptCommandHandler::GetAttackPokePos( void ) const
{
  const BTL_POKEPARAM* attackPoke = this->GetAttackPokeParam();
  return this->GetPokePos( attackPoke );
}

//---------------------------------------------------------------------------
/**
 * @brief 防御側ポケモンの立ち位置を取得する
 */
//---------------------------------------------------------------------------
BtlPokePos AiScriptCommandHandler::GetDefensePokePos( void ) const
{
  const BTL_POKEPARAM* defensePoke = this->GetDefensePokeParam();
  return this->GetPokePos( defensePoke );
}

/**
 * @brief 指定したポケモンの立ち位置を取得する
 * @param pokeParam  立ち位置を取得するポケモン
 */
BtlPokePos AiScriptCommandHandler::GetPokePos( const BTL_POKEPARAM* pokeParam ) const
{
  if( pokeParam == NULL ) {
    GFL_ASSERT(0);
    return BTL_POS_NULL;
  }
  BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokecon, pokeParam->GetID() );
  return pos;
}

//---------------------------------------------------------------------------
/**
 * @brief 採点対象の控えポケモンのパラメータを取得する
 */
//---------------------------------------------------------------------------
const BTL_POKEPARAM* AiScriptCommandHandler::GetBenchPokeParam( void ) const
{
  return m_commandParam.currentBenchPoke;
}

//---------------------------------------------------------------------------
/**
 * @brief 現在採点中の技のインデックスを取得する
 */
//---------------------------------------------------------------------------
u8 AiScriptCommandHandler::GetCurrentWazaIndex( void ) const
{
  return m_commandParam.currentWazaIndex;
}

//---------------------------------------------------------------------------
/**
 * @brief 現在採点中の技No.を取得する
 */
//---------------------------------------------------------------------------
WazaNo AiScriptCommandHandler::GetCurrentWazaNo( void ) const
{
  return m_commandParam.currentWazaNo;
}

//---------------------------------------------------------------------------
/**
 * @brief 現在採点中の道具No.を取得する
 */
//---------------------------------------------------------------------------
u16 AiScriptCommandHandler::GetCurrentItemNo( void ) const
{
  return m_commandParam.currentItemNo;
}




//---------------------------------------------------------------------------
/**
 * @brief ポケモン位置からポケモンパラメータを取得する
 * @param pos  ポケモンの位置
 * @return 指定した場所にいるポケモンのパラメータ
 */
//---------------------------------------------------------------------------
const BTL_POKEPARAM* AiScriptCommandHandler::GetBpp( BtlPokePos pos ) const
{
  return m_pokecon->GetFrontPokeDataConst( pos );
}

//---------------------------------------------------------------------------
/**
 * @brief AI指定サイドからポケモンパラメータする取得する
 * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
 */
//---------------------------------------------------------------------------
const BTL_POKEPARAM* AiScriptCommandHandler::GetBppByAISide( u32 ai_side ) const
{
  if( ai_side == CHECK_BENCH ) {
    return this->GetBenchPokeParam();
  }

  BtlPokePos pos = AISideToPokePos( ai_side );
  return GetBpp( pos );
} 

//---------------------------------------------------------------------------
/**
 *  AI指定サイドからクライアントID取得
 * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
 */
//---------------------------------------------------------------------------
u8  AiScriptCommandHandler::AISideToClientID( u32 ai_side ) const
{
  u32 fixed_side = ai_side;
  if( fixed_side == CHECK_BENCH ) {
    fixed_side = CHECK_ATTACK; // 控えポケモンを指定されたら、攻撃側ポケモンのクライアントを取得する
  }
  BtlPokePos pos = AISideToPokePos( fixed_side );
  return m_mainModule->BtlPosToClientID( pos );
}

//---------------------------------------------------------------------------
/**
 * @brief AI指定サイド -> ポケモン位置へ変換
 * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
 */
//---------------------------------------------------------------------------
BtlPokePos AiScriptCommandHandler::AISideToPokePos( u32 ai_side ) const
{
  BtlPokePos result;

  switch( ai_side )
  {
  case CHECK_ATTACK:
    result = this->GetAttackPokePos();
    break;

  case CHECK_DEFENCE:
    result = this->GetDefensePokePos();
    break;

  case CHECK_ATTACK_FRIEND:
  case CHECK_DEFENCE_FRIEND:
    {
      BtlPokePos basePos = ( ai_side == CHECK_ATTACK_FRIEND ) ? ( this->GetAttackPokePos() ) : ( this->GetDefensePokePos() );

      switch( m_mainModule->GetRule() ){
      case BTL_RULE_SINGLE:
      case BTL_RULE_ROYAL:
      default:
        result = basePos;
        break;
      case BTL_RULE_DOUBLE:
      case BTL_RULE_INTRUDE:
        result = m_mainModule->GetFriendPokePos( basePos, 1 );
        break;
      }
    }
    break;

  default:
    GFL_ASSERT_MSG(0, "ai_side=%d", ai_side);
    result = this->GetAttackPokePos();
    break;
  }

  return result;
} 

//---------------------------------------------------------------------------
/**
 * @brief ポケモンの持っている特性をチェック
 *        ( 相手側なら、すでに見えているとくせいか否かで処理を別ける )
 *
 * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
 */
//---------------------------------------------------------------------------
TokuseiNo AiScriptCommandHandler::CheckTokuseiByAISide( int ai_side )
{
  const BTL_POKEPARAM* bpp = GetBppByAISide( ai_side );

  // 自分と味方の分はそのまま返す
  if( (ai_side == CHECK_ATTACK) || (ai_side == CHECK_ATTACK_FRIEND) )
  {
    return (TokuseiNo)( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE ) );
  }
  // 相手の分はいろんな可能性を考慮する
  else
  {
    TokuseiNo  tok = (TokuseiNo)( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE ) );

    // かげふみ、じりょく、ありじごくは自分が逃げられないことで察知できているハズなのでそのまま返す
    if( (tok == TOKUSEI_KAGEHUMI)
    ||  (tok == TOKUSEI_ZIRYOKU)
    ||  (tok == TOKUSEI_ARIZIGOKU)
    ){
      return tok;
    }

    // とくせいの発現をすでに見ている場合はそのまま返す
    u8 pokeID = bpp->GetID();
    if( BattleAiSystem::IsTokuseiOpened( pokeID ) ){
      return tok;
    }

    // それ以外は、パーソナルを参照して持ちうるとくせいからランダムで取得
    const pml::personal::ParamID  PersonalID[] = {
      pml::personal::PARAM_ID_tokusei1,
      pml::personal::PARAM_ID_tokusei2,
      pml::personal::PARAM_ID_tokusei3,
    };

    int mons_no = bpp->GetMonsNo();
    int form_no = bpp->GetFormNo();

    TokuseiNo tok_ary[ GFL_NELEMS(PersonalID) ];
    int tok_cnt = 0;

    for(u32 i=0; i<GFL_NELEMS(PersonalID); ++i)
    {
      TokuseiNo tmp_tok = (TokuseiNo)( calc::PERSONAL_GetParam( mons_no, form_no, PersonalID[i] ) );
      if( tmp_tok ){
        tok_ary[ tok_cnt++ ] = tmp_tok;
      }
    }

    if( tok_cnt )
    {
      int rnd = m_randGenerator->Next( tok_cnt );
      tok = tok_ary[ rnd ];
    }

    return tok;
  }
} 

//---------------------------------------------------------------------------
/**
 * @brief 攻撃側の持っているワザを全てチェックし、防御側に与えられる最大ダメージを計算する
 * @param atkPoke    攻撃側ポケモン
 * @param defPoke    防御側ポケモン
 * @param loss_flag  ダメージ計算の乱数が有効か
 * @return  攻撃側ポケモンが与えることの出来る最大のダメージ量
 */
//---------------------------------------------------------------------------
u32 AiScriptCommandHandler::CalcMaxDamage( 
    const BTL_POKEPARAM* atkPoke, 
    const BTL_POKEPARAM* defPoke, 
    bool loss_flag ) const
{
  u8  atkPokeID = atkPoke->GetID();
  u8  defPokeID = defPoke->GetID();

  u32 max_dmg = 0;
  u32 waza_count = atkPoke->WAZA_GetCount();
  for(u32 i=0; i<waza_count; ++i)
  {
    WazaNo waza_no = atkPoke->WAZA_GetID( i );

    u32 dmg = m_serverFlow->Hnd_SimulationDamage( atkPokeID, defPokeID, waza_no, true, loss_flag );
    if( dmg > max_dmg )
    {
      max_dmg = dmg;
    }
  }
  return max_dmg;
} 

//---------------------------------------------------------------------------
/**
 * @brief 直前に使ったワザをワークに保存しておく
 * @param bpp  技の使用履歴を保存するポケモン
 */
//---------------------------------------------------------------------------
void AiScriptCommandHandler::StoreUsedWaza( const BTL_POKEPARAM* bpp )
{
  WazaNo waza = bpp->GetPrevWazaID();
  u8 pokeID = bpp->GetID();

  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    if( m_usedWaza[pokeID][i] == waza ){
      break;
    }
    if( m_usedWaza[pokeID][i] == WAZANO_NULL ){
      m_usedWaza[pokeID][i] = waza;
      break;
    }
  }
}

//---------------------------------------------------------------------------
/**
 * @brief 使用履歴保存ワークに指定された技があるか判定
 * @param bpp      チェック対象のポケモン
 * @param waza_no  チェック対象の技
 * @retval true   履歴にある
 * @retval false  履歴にない
 */
//---------------------------------------------------------------------------
bool AiScriptCommandHandler::CheckWazaStored( const BTL_POKEPARAM* bpp, WazaNo waza_no ) const
{
  u8 pokeID = bpp->GetID();

  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    if( m_usedWaza[pokeID][i] == WAZANO_NULL ){
      continue;
    }
    if( m_usedWaza[pokeID][i] == waza_no ){
      return true;
    }
  }
  return false;
}




//---------------------------------------------------------------------------
/**
 * @brief 「にげる」の選択をリセットする
 */
//---------------------------------------------------------------------------
void AiScriptCommandHandler::ResetEscape( void )
{
  m_isEscapeSelected = false;
}

//---------------------------------------------------------------------------
/**
 * @brief 「にげる」を選択したことを記録する
 */
//---------------------------------------------------------------------------
void AiScriptCommandHandler::NotifyEscapeByAI( void )
{
  m_isEscapeSelected = true;
}

//---------------------------------------------------------------------------
/**
 * @brief 「にげる」を選択したか？
 * @retval true  「にげる」を選択した
 * @retval false 「にげる」を選択していない
 */
//---------------------------------------------------------------------------
bool AiScriptCommandHandler::IsEscapeSelected( void ) const
{
  return m_isEscapeSelected;
}



GFL_NAMESPACE_END( btl )
