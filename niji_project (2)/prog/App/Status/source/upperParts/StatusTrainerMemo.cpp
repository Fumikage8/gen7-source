//======================================================================
/**
 * @file    StatusTrainerMemo.cpp
 * @date    2015/12/02 14:58:56
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：トレーナーメモ(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusTrainerMemo.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <print/include/WordSet.h>
#include <PokeTool/include/PokeToolPokePlace.h>
#include <Savedata/include/MyStatus.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_trainermemo.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
TrainerMemo::TrainerMemo( DrawerBase* base, gfl2::str::MsgData* msgData )
  : m_pBase(base)
  , m_pMsgData(msgData)
  , m_pMyStatus(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus())
  , m_pPokeParam(NULL)
  , m_pTrainerMemo(NULL)
  , m_pTextTrainerMemo(NULL)
  , m_pEggMemo(NULL)
  , m_pTextEggMemo(NULL)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
TrainerMemo::~TrainerMemo( void )
{
  for( u32 i=0; i<STR_MEMO_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pStrBuf[i] );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void TrainerMemo::Initialize( void )
{
  // メッセージ
  {
    gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

    for( u32 i=0; i<STR_MEMO_NUM; ++i )
    {
      m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( 512, devHeap );
      GFL_ASSERT( m_pStrBuf[i] );
    }
  }

  // ペインの設定
  {
    // トレーナーメモ表示切替ペイン
    m_pTrainerMemo     = m_pBase->GetPane( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_ALL_NORMAL );
    // トレーナーメモテキスト
    m_pTextTrainerMemo = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTBOX_00 );
    // タマゴメモ表示切替ペイン
    m_pEggMemo         = m_pBase->GetPane( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_ALL_EGG );
    // タマゴメモテキスト
    m_pTextEggMemo     = m_pBase->GetTextBox( PANENAME_FIELDSTATUS_SUB_UPP_000_PANE_TEXTEGG_00 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::Setup( const pml::pokepara::PokemonParam* pp )
{
  //if( m_pPokeParam != pp )
  {
    bool fastmode_flag = pp->StartFastMode();
    {
      // タマゴじゃない時
      if( !pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        // トレーナーメモのセットアップ
        Setup_TrainerMemo( pp );
      }
      // タマゴの時
      else {
        // タマゴメモのセットアップ
        Setup_EggMemo( pp );
      }
    }
    pp->EndFastMode( fastmode_flag );

    m_pPokeParam = pp;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   トレーナーメモのセットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::Setup_TrainerMemo( const pml::pokepara::PokemonParam* pp )
{
  // 第１性格のセット
  SetTrainerMemo_Seikaku( pp );
  // メインメモのセット
  SetTrainerMemo_Main( pp );
  // 第２性格のセット
  SetTrainerMemo_Seikaku2( pp );

  // 先頭の文字列に中間の文字列を連結
  m_pStrBuf[STR_MEMO_TOP]->AddCode( gfl2::str::CR_CODE );
  m_pStrBuf[STR_MEMO_TOP]->AddStr( *m_pStrBuf[STR_MEMO_MIDDLE] );

  // 先頭の文字列に終端の文字列を連結
  m_pStrBuf[STR_MEMO_TOP]->AddCode( gfl2::str::CR_CODE );
  m_pStrBuf[STR_MEMO_TOP]->AddStr( *m_pStrBuf[STR_MEMO_BOTTOM] );

  // テキストボックスに反映
  app::ui::UIView* view = m_pBase->GetUIView();
  view->SetTextboxPaneString( m_pTextTrainerMemo, m_pStrBuf[STR_MEMO_TOP] );

  // トレーナーメモの表示切替
  m_pBase->SetVisiblePane( m_pTrainerMemo, true );
  m_pBase->SetVisiblePane( m_pEggMemo, false );
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴメモのセットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::Setup_EggMemo( const pml::pokepara::PokemonParam* pp )
{
  // タマゴのメイン情報のセット
  SetEggMemo_Main( pp );
  // タマゴの様子をセット
  SetEggMemo_State( pp );

  // 中間の文字列に終端の文字列を連結
  m_pStrBuf[STR_MEMO_MIDDLE]->AddCode( gfl2::str::CR_CODE );
  m_pStrBuf[STR_MEMO_MIDDLE]->AddStr( *m_pStrBuf[STR_MEMO_BOTTOM] );

  // テキストボックスに反映
  app::ui::UIView* view = m_pBase->GetUIView();
  view->SetTextboxPaneString( m_pTextEggMemo, m_pStrBuf[STR_MEMO_MIDDLE] );

  // トレーナーメモの表示切替
  m_pBase->SetVisiblePane( m_pTrainerMemo, false );
  m_pBase->SetVisiblePane( m_pEggMemo, true );
}


//------------------------------------------------------------------------------
/**
 * @brief   第１性格のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetTrainerMemo_Seikaku( const pml::pokepara::PokemonParam* pp )
{
  pml::pokepara::Seikaku seikaku = pp->GetSeikaku();

  u32 msgID = trmemo_01_01 + seikaku;

  m_pMsgData->GetString( msgID, *m_pStrBuf[STR_MEMO_TOP] );
}

//------------------------------------------------------------------------------
/**
 * @brief   メインメモのセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetTrainerMemo_Main( const pml::pokepara::PokemonParam* pp )
{
  // メインメモのメッセージIDの取得
  u32 msgID = GetMainMemoIndex( pp );

  // メインメモのセット
  SetMainMemo( pp, msgID );
}

//------------------------------------------------------------------------------
/**
 * @brief   第２性格のセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetTrainerMemo_Seikaku2( const pml::pokepara::PokemonParam* pp )
{
  pml::pokepara::PowerID power_priority[]  =
  {
    pml::pokepara::POWER_HP,     // 最大HP
    pml::pokepara::POWER_ATK,    // こうげき
    pml::pokepara::POWER_DEF,    // ぼうぎょ
    pml::pokepara::POWER_AGI,    // すばやさ
    pml::pokepara::POWER_SPATK,  // とくこう
    pml::pokepara::POWER_SPDEF,  // とくぼう
  };

  u32 priority_index = pp->GetPersonalRnd() % GFL_NELEMS( power_priority );
  u32 priority_array[6][6]  =
  {
    { 0,1,2,3,4,5 },
    { 1,2,3,4,5,0 },
    { 2,3,4,5,0,1 },
    { 3,4,5,0,1,2 },
    { 4,5,0,1,2,3 },
    { 5,0,1,2,3,4 }, 
  };

  u32 max_index = 0;
  u32 max_power = 0;

  GFL_ASSERT( priority_index < 6 );
  for( int i=0;i<6;i++ )
  {
    u32 check_index = priority_array[priority_index][i];

    GFL_ASSERT( check_index < 6 );
    pml::pokepara::PowerID check_power = power_priority[ check_index ];
    //u32 power = pp->GetTalentPower( check_power );
    u32 power = pp->GetNativeTalentPower( check_power );  // @fix NMCat[885]

    if( max_power < power )
    {
      max_power = power;
      max_index = check_index;
    }
  }

  u32 msgID = trmemo_03_01_00 + max_index*5 + max_power%5;

  m_pMsgData->GetString( msgID, *m_pStrBuf[STR_MEMO_BOTTOM] );
}

//------------------------------------------------------------------------------
/**
 * @brief   メインメモのメッセージIDの取得
 *
 * @param   pp    ポケモンパラメータ
 *
 * @return  条件にあったメッセージID
 */
//------------------------------------------------------------------------------
u32 TrainerMemo::GetMainMemoIndex( const pml::pokepara::PokemonParam* pp )
{
  u32 place1 = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE );
  u32 place2 = pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE );
  const bool isEvent  = pp->GetEventPokeFlag();
  const bool isParent = m_pMyStatus->IsMyPokemon( *pp );
  u32 version = pp->GetCassetteVersion();

  u32 memoId  = 0;

  if( IsVirtualConsole( version ) )
  {
    // 外部配布ON
    if( isEvent )
    {
      memoId = trmemo_02_10_01;
    }
    else
    {
      memoId = trmemo_02_09_01;
    }
  }
  else if( !IsOldVersion( version ) )
  {
    if( isEvent == false )
    { 
      if( place1 == 0 )
      {
        //ポケモンとして捕獲した
        switch( place2 )
        {
        default:
          //野生ポケモン捕獲orNPCからポケモン入手
          memoId = trmemo_02_01_01;
          break;

        case PokeTool::PokePlace::PLACE_NO_SPECIAL_INNER_TRADE:
          //ゲーム内交換
          memoId = trmemo_02_02_01;
          break;

        case PokeTool::PokePlace::PLACE_NO_SPECIAL_3DSLINK:
          memoId  = trmemo_02_11_01;
          break;
        }
      }
      else
      {
        //タマゴからかえした
        if( place1 == PokeTool::PokePlace::PLACE_NO_SPECIAL_3DSLINK )
        {
          memoId  = trmemo_02_12_01;
        }
        else if( place1 < PokeTool::PokePlace::PLACE_NO_START_PERSON )
        {
          //通信交換タマゴ孵化
          memoId = trmemo_02_03_01;
        }
        else
        {
          //ゲーム内取得のタマゴ孵化
          memoId = trmemo_02_04_01;
        }
      }
    }
    else  //isEvent == true
    {
      if( place1 == 0 )
      {
        //外部配布で入手したポケモン
        memoId = trmemo_02_05_01;
      }
      else
      {
        //外部配布のタマゴ孵化
        memoId = trmemo_02_06_01;
      }
    }
  }
  else
  {
    //バンク経由
    //place1に地方名が入っていたときのみ、2_9, 2_10に分岐する。（地方名はタマゴ孵化時にセットされる。）
    // …はずでしたが、バンクからタマゴが来ないので、すべて07, 08だけに分岐
#if 0
    if( this->GetVersionByPlace1( NULL ) == false )
#endif
    {
      if( !isEvent )
      {
        //バンク経由
        memoId  = trmemo_02_07_01;
      }
      else
      {
        //外部配布のバンク経由
        memoId  = trmemo_02_08_01;
      }
    }
#if 0
    else
    {
      if( !isEvent )
      {
        //バンク経由のタマゴ孵化
        memoId  = trmemo_02_09_01;
      }
      else
      {
        //外部配布バンク経由のタマゴ孵化
        memoId  = trmemo_02_10_01; 
      }
    }
#endif
  }

  if( isParent == false )
  {
    //タマゴじゃなければ、他人のポケの時のMsgは+1の位置にある。
    ++memoId;
  }

  return memoId;
}


//------------------------------------------------------------------------------
/**
 * @brief   タマゴのメイン情報をセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetEggMemo_Main( const pml::pokepara::PokemonParam* pp )
{
  // タマゴのメイン情報のメッセージIDの取得
  u32 msgID = GetMainMemoIndexEgg( pp );

  // メインメモのセット
  SetMainMemo( pp, msgID );
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴの様子をセット
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetEggMemo_State( const pml::pokepara::PokemonParam* pp )
{
  const u8 natuki = pp->GetOriginalFamiliarity();
  u32 msgID = 0;
  if( natuki <= 5 )
  {
    msgID = trmemo_01_T_02_01;
  }
  else if( natuki <= 10 )
  {
    msgID = trmemo_01_T_02_02;
  }
  else if( natuki <= 40 )
  {
    msgID = trmemo_01_T_02_03;
  }
  else
  {
    msgID = trmemo_01_T_02_04;
  }

  m_pMsgData->GetString( msgID, *m_pStrBuf[STR_MEMO_BOTTOM] );
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴのメイン情報のメッセージIDの取得
 *
 * @param   pp    ポケモンパラメータ
 *
 * @return  条件にあったメッセージID
 */
//------------------------------------------------------------------------------
u32 TrainerMemo::GetMainMemoIndexEgg( const pml::pokepara::PokemonParam* pp )
{
  u32 place1 = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE );
  const bool isEvent = pp->GetEventPokeFlag() ;
  bool isParent = m_pMyStatus->IsMyPokemon( *pp );

  u32 memoId  = 0;
#if 0 //卵がバンクから来ることがないので、フローを削除
  if( !this->IsOldVersion( pp ) )
#endif
  {
    //クジラのタマゴ
    if( isEvent == false )
    {
      if( isParent == true )
      {
        if( place1 == PokeTool::PokePlace::PLACE_NO_SPECIAL_3DSLINK )
        {
          //３DSDLAapp経由のタマゴ
          memoId  = trmemo_01_T_01_07;
        }
        else
        {
          //ゲーム内でタマゴ入手
          memoId = trmemo_01_T_01_01; //もらったふしぎな?
        }
      }
      else
      {
        //通信交換でタマゴ入手
        memoId = trmemo_01_T_01_02; //てにいれたふしぎな?
      }
    }
    else
    {
      if( isParent == true )
      {
        //外部配布で入手したタマゴ
        memoId = trmemo_01_T_01_03; //てにいれたとってもふしぎなふしぎな?
      }
      else
      {
        //外部配布で入手したタマゴを通信交換
        memoId = trmemo_01_T_01_04; //てにいれたふしぎなふしぎな?
      }
    }
  }
#if 0
  else
  {
    if( isEvent == false )
    {
      //バンク経由のタマゴ
      memoId  = trmemo_01_T_01_05;
    }
    else
    {
      //外部配布で入手したバンク経由のタマゴ
      memoId  = trmemo_01_T_01_06;
    }
  }
#endif
  return memoId;
}


//------------------------------------------------------------------------------
/**
 * @brief   メインメモのセット
 *
 * @param   pp      ポケモンパラメータ
 * @return  msgID   メッセージID
 */
//------------------------------------------------------------------------------
void TrainerMemo::SetMainMemo( const pml::pokepara::PokemonParam* pp, u32 msgID )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();
  print::WordSet* wordSet = m_pBase->GetWordSet();

  u32 msg_id;
  u32 place2 = pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE );

  u32 version = pp->GetCassetteVersion();

  gfl2::str::StrBuf *placeStr1;
  gfl2::str::StrBuf *placeStr2 = PokeTool::PokePlace::CreatePlaceName( place2, devHeap );

  if( !this->IsOldVersion( version ) && !IsVirtualConsole( version ) )
  {
    u32 place1 = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE );
    placeStr1 = PokeTool::PokePlace::CreatePlaceName( place1, devHeap );
  }
  else
  {
    placeStr1 = PokeTool::PokePlace::CreateVersionName( version, devHeap );
  }

  msg_id = msgID;

  wordSet->RegisterWord( 8, *placeStr1 );
  wordSet->RegisterWord( 4, *placeStr2 );

  GFL_SAFE_DELETE( placeStr2 );
  GFL_SAFE_DELETE( placeStr1 );

  //必要なモノを単語登録
  u32 year2  = pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR );
  u32 month2 = pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH );
  u32 day2   = pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY );
  u32 year1  = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR );
  u32 month1 = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH );
  u32 day1   = pp->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY );
  u32 level  = pp->GetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL );

  wordSet->RegisterNumber( 0, year2,  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 1, month2, 2, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 2, day2,   2, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 3, level,  3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 5, year1,  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 6, month1, 2, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wordSet->RegisterNumber( 7, day1,   2, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );

  gfl2::str::StrBuf *src_str = GFL_NEW( devHeap->GetLowerHandle() ) gfl2::str::StrBuf( 512, devHeap->GetLowerHandle() );

  m_pMsgData->GetString( msg_id, *src_str );
  wordSet->Expand( m_pStrBuf[STR_MEMO_MIDDLE], src_str );

  GFL_SAFE_DELETE(src_str);
}

//------------------------------------------------------------------------------
/**
 * @brief   昔のバージョンかどうか
 *
 * @param   version   ROMバージョン
 *
 * @return  "true  = 旧バージョン"
 * @return  "false = 新バージョン"
 */
//------------------------------------------------------------------------------
bool TrainerMemo::IsOldVersion( u32 version ) const
{
  GFL_ASSERT_STOP( VERSION_SUN < VERSION_MOON );  // 定義が入れ替わることがあれば。

  if( version < VERSION_SUN  )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   VCかどうか
 *
 * @param   version   ROMバージョン
 *
 * @return  "true  = VC"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool TrainerMemo::IsVirtualConsole( u32 version ) const
{
  if( version >= VERSION_VC_RED && version <= VERSION_VC_CRISTAL )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
