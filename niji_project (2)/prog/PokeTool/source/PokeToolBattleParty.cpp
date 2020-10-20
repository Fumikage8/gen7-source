//=============================================================================
/**
 * @file    PokeToolBattleParty.h
 * @author  k.ohno
 * @brief   ポケモンバトルパーティーにかかわるクラス
 * @date    2015.08.07
 */
//=============================================================================


#include "PokeTool/include/PokeToolBattleParty.h"

#include <base/include/gfl2_Singleton.h>
#include "GameSys/include/GameData.h"
#include "GameSys/include/GameManager.h"

// Wordset
#include <str/include/gfl2_Str.h>
#include "Print/include/WordSet.h"
#include "Print/include/PrintSystem.h"
#include "arc_index/message.gaix"

// バトルチーム選択イベント(チームIndexの定義がある）
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

// 通信常駐メッセージ（てもち のラベルがある）
#include <niji_conv_header/message/msg_network_common.h>

GFL_NAMESPACE_BEGIN(PokeTool)

//------------------------------------------------------------------------------------------
/**
 * @brief	コンストラクタ＆デストラクタ
 */
//------------------------------------------------------------------------------------------
BattleParty::BattleParty( gfl2::heap::HeapBase* pHeap) :
mIsLocked(false),
mIsQrTeam(false)
{
  mpName = GFL_NEW(pHeap) gfl2::str::StrBuf(Savedata::BOX::TEAM_NAME_BUFSIZE,pHeap);
  mpPokeParty = GFL_NEW(pHeap) pml::PokeParty(pHeap);

  GFL_ASSERT(mpName);
  GFL_ASSERT(mpPokeParty);
  
}

BattleParty::~BattleParty()
{
  GFL_SAFE_DELETE(mpName);
  GFL_SAFE_DELETE(mpPokeParty);
}

//------------------------------------------------------------------------------------------
/**
 * @brief	    引数に従ったチームのバトルパーティーを生成する
 * @param     BOX     ボックスのポインタ
 * @param     teamNo  バトルチーム番号 0 - 5
 * @param     pTempHeap   一時的に必要なヒープ
 * @return    ポケモン数
 * @attention 開放はデストラクタで行われる
 */
//------------------------------------------------------------------------------------------
int BattleParty::CreateBattleParty( const Savedata::BOX* pBox,  Savedata::BoxPokemon* pBoxPoke, int teamNo, gfl2::heap::HeapBase* pTempHeap)
{
  GFL_PRINT("BattleParty::CreateBattleParty teamNo=%d\n", teamNo);

  // バトルチーム番号のerror値
  if( teamNo == NetApp::TeamSelect::TEAM_SELECT_ERROR )
  {
    GFL_ASSERT(0);
    return 0;
  }

  // 手持ちだった場合
  if( teamNo == NetApp::TeamSelect::TEAM_SELECT_TEMOCHI )
  {
    pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
    mpPokeParty->CopyFrom( *party );

    // @fix GFNMCat[4124]：手持ちをかえす時に全回復をしないと受信先で全回復するのでシリアライズデータが一致しなくなる
    mpPokeParty->RecoverAll();

    // チーム名は「てもち」固定
    {
      gfl2::str::MsgData* msgData = GFL_NEW_LOW( pTempHeap ) gfl2::str::MsgData( print::GetMessageArcId(),
                                                                                 GARC_message_network_common_DAT,
                                                                                 pTempHeap->GetLowerHandle(), gfl2::str::MsgData::LOAD_FULL );
      msgData->GetString( network_common_cap_01, *mpName );
      GFL_DELETE msgData;
    }

    return mpPokeParty->GetMemberCount();
  }
  // QRバトルチームだった場合
  else if( teamNo == NetApp::TeamSelect::TEAM_SELECT_QR )
  {
    pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRParty();
    mpPokeParty->CopyFrom( *party );

    // チーム名
    {
      if( mpName )
      {
        mpName->SetStr( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRPartyName() );
      }
    }

    mIsQrTeam = true;

    return mpPokeParty->GetMemberCount();
  }
  // 通常バトルチームだった場合
  else
  {
    int num=0;
    mIsLocked = pBox->IsTeamLock(teamNo);

    mpName->SetStr( pBox->GetTeamName(teamNo) );

    for(int i=0;i<Savedata::BOX::TEAM_NUM_MAX;i++)
    {
      u16 pos = pBox->GetTeamPokePos(teamNo,i);
      ICHI_PRINT("pos %d \n", pos);
      //  posの変数型がu16なので型を合わせないと正しい条件チェックをしない
      if(pos != (u16)Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE )
      {
        u16 tray = (pos >> 8 )& 0xff;
        pos = pos & 0xff;
        pml::pokepara::CoreParam * pc = pBoxPoke->GetPokemon( tray, pos, pTempHeap );
        if( pBoxPoke->CheckPokemon( tray, pos, pc, false ) )
        {
          pml::pokepara::PokemonParam* pPokep = GFL_NEW(pTempHeap) pml::pokepara::PokemonParam(pTempHeap, *pc);
          mpPokeParty->AddMember( *pPokep );
          GFL_SAFE_DELETE(pPokep);
          num++;
        }
        GFL_SAFE_DELETE(pc);
      }
    }
    return num;
  }
}



GFL_NAMESPACE_END(PokeTool);


