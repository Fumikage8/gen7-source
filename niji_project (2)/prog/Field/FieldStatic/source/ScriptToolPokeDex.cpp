//=============================================================================
/**
 *
 *  @file   ScriptToolPokeDex.cpp
 *  @brief  図鑑データアクセサ
 *          パソコン画面とスクリプトからの呼出しを共有化する
 *  @author masanori funakoshi
 *  @data   2012.09.25
 *
 */
//=============================================================================

#if 0 //nijiで未使用になった by iwasawa 160524

#include "Field/FieldStatic/include/ScriptToolPokeDex.h"

#include "GameSys/include/GameManager.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "PokeTool/include/PokeToolPokeMemory.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

/**
 * @brief フィールド
 */
GFL_NAMESPACE_BEGIN( Field )
/**
 * @brief スクリプト
 */
GFL_NAMESPACE_BEGIN( Script )


//--------------------------------------------------------------
//! コンストラクタ
//--------------------------------------------------------------
ScriptToolPokeDex::ScriptToolPokeDex( gfl2::heap::HeapBase* heap, GameSys::GameManager *gameManager, bool is_pc_mode ) :
  m_isPcMode( is_pc_mode ),
  m_isEvaluted( false )
{
  m_gameManager = gameManager;
  m_zukanData   = gameManager->GetGameData()->GetZukanData();
  m_eventWork   = gameManager->GetGameData()->GetEventWork();

  m_extend = GFL_NEW( heap ) PokeTool::ExtendData( heap );
  m_extend->Load( heap );

  ClearCommands();
}

//--------------------------------------------------------------
//! デストラクタ
//--------------------------------------------------------------
ScriptToolPokeDex::~ScriptToolPokeDex()
{
  GFL_SAFE_DELETE( m_extend );
}

//--------------------------------------------------------------
/**
 *  @brief   図鑑評価の生成
 *
 *  - プロパティにアクセスする前に実行する
 *  - スクリプト、アプリ両方に対応するため、固定長のコマンドリストを生成する
 *  - 実行側が簡易スクリプトエンジンを実装する必要はあるが同一の流れを実現することができる
 *
 *  @param   [in]  mode  図鑑モード
 *  @return  積んだコマンド数を返す
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::EvaluteWithMode( Savedata::ZukanData::Mode mode /* = MODE_ZENKOKU */ )
{
#if 0 //@attention DEPEND_ON_OLD_MODULE
  static const struct
  {
    // messageIdは実用上16bitあれば充分
    u16  evalId;   // 評価用     messageId
    u16  adviceId; // アドバイス messageId
    u16  compId;   // コンプした messageId
    u16  compedId; // コンプ済み messageId
    u16  compFlag; // コンプ済みフラグ
    PDID pdid;     // ポケモンダイアリーId
  }
  table_[] =
  {
    {
      msg_hyouka_hakase1_houen_eval,
      msg_hyouka_houen01,
      msg_hyouka_houen08,
      msg_hyouka_hakase1_houen_comp,
      SYS_FLAG_ZUKAN_CHI_COMP,
      PDID_PERSONAL_COMP_ZUKAN_LOCAL_ALL
    },
    {
      msg_hyouka_hakase1_zen_eval00,
      msg_hyouka_zen01,
      msg_hyouka_zen20,
      msg_hyouka_hakase1_zen_comp,
      SYS_FLAG_ZUKAN_ALL_COMP,
      PDID_PERSONAL_COMP_ZUKAN_ZENKOKU_ALL
    }
  };

  bool lastCommand = false;

  int  index = 0;    // modeから算出した内部インデックス // Coverity[10902]

  u16  count;    // 『みた』もしくは『捕獲』した数
  int  rank;     // countから算出される図鑑ランク

  int  meNo;

  // コマンドのクリア
  ClearCommands();

  // エリアとtable_[]を結びつける
  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : index = 0; break; // 地方
  case Savedata::ZukanData::MODE_ZENKOKU : index = 1; break; // 全国
  default                                : GFL_ASSERT( false );
  }

  if ( flagGet( table_[ index ].compFlag ) == true )
  {
    AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].compedId );   // 完成済み
    AppendCommand( COMMAND_MESSAGE_WAIT );                           // メッセージ終端待ち
    AppendCommand( COMMAND_WAIT_MARK );                              // ▼
  }
  else
  {
    if ( ( index >= 0 ) && ( index <= 0 ) )
    {
      count = GetPokeSeeCount( mode );
      rank  = localCountToRank( count, mode );
      meNo  = evalToMeNo( rank );

      AppendCommand( COMMAND_WORD_SET_NUM, 0, count );               // みた数設定
      AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].evalId );   // カロスの ××××で…
      AppendCommand( COMMAND_MESSAGE_WAIT );                         // メッセージ終端待ち
      AppendCommand( COMMAND_ME_PLAY, 0, meNo );                     // ME再生
      AppendCommand( COMMAND_ME_WAIT, 0, meNo );                     // ME待ち
      AppendCommand( COMMAND_WAIT_MARK );                            // ▼

      if ( rank == 7 ) // 最大？
      {
        // コンプ
        AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].compId ); // すばらしい
        AppendCommand( COMMAND_MESSAGE_WAIT );                       // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                          // ▼

        pokediary::Set( table_[ index ].pdid, 0, 0, 0 );             // ポケモンダイアリーの設定
        flagSet( table_[ index ].compFlag );                         // 地方図鑑 部分コンプフラグセット
      }
      else
      {
        // 『みた』数で評価
        AppendCommand( COMMAND_MESSAGE, 1, table_[index].adviceId + rank );   // 評価メッセージ
        AppendCommand( COMMAND_MESSAGE_WAIT );                                // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                   // ▼
      }
    }
    else
    {
      // 全国図鑑評価
      count = GetPokeGetCount();
      rank  = worldCountToRank( count );
      meNo  = evalToMeNo( worldRankToMeOffset( rank ) );

      AppendCommand( COMMAND_WORD_SET_NUM, 0, count );                        // 全国図鑑の捕獲数設定
      AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].evalId );            // つかまえた ポケモンの…
      AppendCommand( COMMAND_MESSAGE_WAIT );                                  // メッセージ終端待ち
      AppendCommand( COMMAND_ME_PLAY, 0, meNo );                              // ME再生
      AppendCommand( COMMAND_ME_WAIT, 0, meNo );                              // ME待ち
      AppendCommand( COMMAND_WAIT_MARK );                                     // ▼

      // 地方図鑑の捕獲数評価
      AppendCommand(                                                          // 地方図鑑の捕獲数設定
        COMMAND_WORD_SET_NUM,
        1,
        GetPokeGetCount( Savedata::ZukanData::MODE_LOCAL_A )
      );
      AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].evalId + 1 );        // カロスちほうで きみが つかまえた…
      AppendCommand( COMMAND_MESSAGE_WAIT );                                  // メッセージ終端待ち
      AppendCommand( COMMAND_WAIT_MARK );                                     // ▼

      if ( IsGetCompleteLocal() == true )
      {
        // 地方図鑑コンプリート
        AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].evalId + 2 );      // すばらしい！
        AppendCommand( COMMAND_MESSAGE_WAIT );                                // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                   // ▼
      }
      else
      {
        // コンプリートした地方を列挙
        if ( ( IsGetComplete( Savedata::ZukanData::MODE_LOCAL_A ) == true  )
         &&  ( flagGet( SYS_FLAG_ZUKAN_CHI_CAP_COMP )             == false ) )
        {
          AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_zen_eval06 );
          AppendCommand( COMMAND_MESSAGE_WAIT );                                // メッセージ終端待ち
          AppendCommand( COMMAND_WAIT_MARK );                                   // ▼

          flagSet( SYS_FLAG_ZUKAN_CHI_CAP_COMP );
        }
      }

      if ( rank == 19 ) // 最大？
      {
        // 全国図鑑コンプリート
        AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].compId );            // こりゃ たまげたな！
        AppendCommand( COMMAND_MESSAGE_WAIT );                                  // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                     // ▼

        pokediary::Set( table_[ index ].pdid, 0, 0, 0 );                        // ポケモンダイアリーを設定する
        flagSet( table_[ index ].compFlag );                                    // 全国図鑑コンプフラグセット

        PokeTool::PokeMemory::Set(PokeTool::PokeMemory::EVENT_COMPLETE_PICTUREBOOK, 0, m_gameManager->GetGameData()->GetPlayerParty(), m_gameManager->GetGameData() );  // sango ポケモンの思い出
      }
      else
      {
        // 『捕獲』数で評価
        AppendCommand( COMMAND_MESSAGE, 1, table_[ index ].adviceId + rank );   // 評価メッセージ
        AppendCommand( COMMAND_MESSAGE_WAIT );                                  // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                     // ▼
      }
    }
  }

  // おまもり入手チェック
  if ( ( index >= 0 ) && ( index <= 0 ) )
  {
    // GFBTS75：【図鑑評価仕様で各図鑑の完成メッセージを見ていなくてもご褒美がもらえてしまいます。】
    if ( flagGet( table_[0].compFlag ) &&
         /*( IsCompleteLocal() == true ) && */
       ( flagGet( SYS_FLAG_MARUIOMAMORI ) == false ) )
    {
      // 地方図鑑完成、かつ、まるいおまもり入手前
      if ( m_isPcMode == true )
      {
        // PCの場合は誘導
        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item01 );   // けんきゅうじょへ きてね
        AppendCommand( COMMAND_MESSAGE_WAIT );                            // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                               // ▼
      }
      else
      {
        // 話しかけの場合はプレゼント
        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item02 );   // これを プレゼントするわ
        AppendCommand( COMMAND_MESSAGE_WAIT );                            // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                               // ▼

        AppendCommand( COMMAND_ITEM_ADD, 0, ITEM_MARUIOMAMORI );          // まるいおまもり入手

        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item03 );   // ひかるおまもりを…
        AppendCommand( COMMAND_MESSAGE_WAIT );                            // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_LAST );                               // キーウェイト

        lastCommand = true;                                               // アイテム入手したら終了

        flagSet( SYS_FLAG_MARUIOMAMORI );                                 // フラグはドキュメント側で立てておく
      }
    }
  }
  else
  {
    if ( ( IsCompleteWorld() == true ) && ( flagGet( SYS_FLAG_HIKARUOMAMORI ) == false ) )
    {
      // 全国図鑑完成、かつ、ひかるおまもり入手前
      if ( m_isPcMode == true )
      {
        // PCの場合は誘導
        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item01 );     // けんきゅうじょへ きてね
        AppendCommand( COMMAND_MESSAGE_WAIT );                              // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                 // ▼
      }
      else
      {
        // 話しかけの場合はプレゼント
        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item02 );     // これを プレゼントするわ
        AppendCommand( COMMAND_MESSAGE_WAIT );                              // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_MARK );                                 // ▼

        AppendCommand( COMMAND_ITEM_ADD, 0, ITEM_HIKARUOMAMORI );           // ひかるおまもり入手
        AppendCommand( COMMAND_MEMORY );                                    // 思い出を設定する

        AppendCommand( COMMAND_MESSAGE, 1, msg_hyouka_hakase1_item04 );     // ひかるおまもりを…
        AppendCommand( COMMAND_MESSAGE_WAIT );                              // メッセージ終端待ち
        AppendCommand( COMMAND_WAIT_LAST );                                 // キーウェイト

        lastCommand = true;                                                 // アイテム入手したら終了

        flagSet( SYS_FLAG_HIKARUOMAMORI );                                  // フラグはドキュメント側で立てておく
      }
    }
  }

  if ( lastCommand == true )
  {
    AppendCommand( COMMAND_CLOSE );    // ウィンドウクローズ
    AppendCommand( COMMAND_BREAK );    // 終了
  }
  else
  {
    AppendCommand( COMMAND_CONTINUE ); // 選択肢へループ
  }
#endif

  m_isEvaluted = true; // 評価完了

  return m_commandCount;
}

/**
 *  @brief  図鑑評価の生成
 *  - PokeTool::ExtendData::LOCAL_AREA_[ABC] から Savedata::ZukanData::Mode に内部コードを統一する際、互換のためのに残す
 */
int ScriptToolPokeDex::Evalute( PokeTool::ExtendData::LocalArea area /* = PokeTool::ExtendData::LOCAL_AREA_NONE */ )
{
  return EvaluteWithMode( areaToMode( area ) );
}

//--------------------------------------------------------------
/**
 *  @brief   初回図鑑評価の生成
 *
 *  - プラターヌ博士の初回会話に挟み込むための専用関数
 *  - 他はEvalute*+()を参照
 *
 *  @return  積んだコマンド数を返す
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::Evalute1st()
{
#if 0 //@attention DEPEND_ON_OLD_MODULE
  u16 count;    // 『みた』もしくは『捕獲』した数
  int rank;     // countから算出される図鑑ランク
  int meNo;

  // ！以下３つ同期必須
  const Savedata::ZukanData::Mode MODE = Savedata::ZukanData::MODE_LOCAL_A;
  const PDID pdid                      = PDID_PERSONAL_COMP_ZUKAN_LOCAL_ALL;
  const u16 compFlag                   = SYS_FLAG_ZUKAN_CHI_COMP;

  // コマンドのクリア
  ClearCommands();

  /*
   *  地方図鑑 平原の評価を作成する
   *  初回はシチュエーションが限られるため、決め打ちのコードとなる
   */
  count = GetPokeSeeCount( MODE );
  rank  = localCountToRank( count, MODE );
  meNo  = evalToMeNo( rank );

  AppendCommand( COMMAND_WORD_SET_NUM, 0, count );                  // みた数設定
  AppendCommand( COMMAND_MESSAGE, 0, msg_hyouka_hakase1_houen_eval ); // カロスの へいげんで…
  AppendCommand( COMMAND_MESSAGE_WAIT );                            // メッセージ終端待ち
  AppendCommand( COMMAND_ME_PLAY, 0, meNo );                        // ME再生
  AppendCommand( COMMAND_ME_WAIT, 0, meNo );                        // ME待ち
  AppendCommand( COMMAND_WAIT_MARK );                               // ▼

  // 『みた』数で評価
  AppendCommand( COMMAND_MESSAGE, 0, msg_hyouka_houen01 + rank );     // 評価メッセージ
  AppendCommand( COMMAND_MESSAGE_WAIT );                            // メッセージ終端待ち
  AppendCommand( COMMAND_WAIT_MARK );                               // ▼

  if ( rank == 7 ) // 最大？
  {
    pokediary::Set( pdid, 0, 0, 0 );             // ポケモンダイアリーの設定
    flagSet( compFlag );                         // 地方図鑑 部分コンプフラグセット
    // ローカル図鑑完成はありえないのでチェックしない  // このコメントはScriptToolPokeDex::EvaluteWithMode関数中のif ( IsCompleteLocal() == true )のことですが、sangoではローカル図鑑は1個しかありません。川田
  }
#endif

  m_isEvaluted = true; // 評価完了

  return m_commandCount;
}

//--------------------------------------------------------------
/**
 *  @brief   指定図鑑が完成済みか？
 *  @param   [in]  mode  図鑑モード
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 *
 *  @note   地方は『みた』、全国は『捕獲』で判断
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsComplete( Savedata::ZukanData::Mode mode /* = MODE_ZENKOKU */, bool is_self /* = false */ )
{
  PokeTool::ExtendData::LocalArea area = modeToArea( mode );

  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : return m_zukanData->CheckLocalSeeComp( area, m_extend );
  case Savedata::ZukanData::MODE_ZENKOKU : return m_zukanData->CheckZenkokuComp();
  default                                : GFL_ASSERT( false );
  }

  return false;
}

//--------------------------------------------------------------
/**
 *  @brief   地方図鑑の『みた』コンプリートチェック
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 *
 *  @note    地方のため『みた』で判断
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsCompleteLocal()
{
  return
     ( ( IsComplete( Savedata::ZukanData::MODE_LOCAL_A ) == true )
       );
}

//--------------------------------------------------------------
/**
 *  @brief   地方図鑑の『捕獲』コンプリートチェック
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 *
 *  @note 後付のため名前付けがシックリこない
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsGetComplete(  Savedata::ZukanData::Mode mode, bool is_self )
{
  PokeTool::ExtendData::LocalArea area = modeToArea( mode );

  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : return m_zukanData->CheckLocalGetComp( area, m_extend );
  case Savedata::ZukanData::MODE_ZENKOKU : return m_zukanData->CheckZenkokuComp();
  default                                : GFL_ASSERT( false );
  }

  return m_zukanData->CheckLocalGetComp( area, m_extend );
}

//--------------------------------------------------------------
/**
 *  @brief   全地方図鑑の『捕獲』コンプリートチェック
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsGetCompleteLocal()
{
  return
     ( ( IsGetComplete( Savedata::ZukanData::MODE_LOCAL_A ) == true )
       );
}

//--------------------------------------------------------------
/**
 *  @brief   全国図鑑のコンプリートチェック
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsCompleteWorld()
{
  return IsComplete();
}

//--------------------------------------------------------------
/**
 *  @brief   図鑑のオールコンプリートチェック
 *  @param   [in]  makeCommands  コマンドリストを生成するか？(default : false)
 *  @return
 *  - true  コンプリートした
 *  - false コンプリートしていない
 */
//--------------------------------------------------------------
bool ScriptToolPokeDex::IsCompleteAll( bool makeCommands /* = false */ )
{
  bool result = ( ( IsCompleteLocal() == true ) && ( IsCompleteWorld() == true ) );

  if ( makeCommands == true )
  {
    // コマンドのクリア
    ClearCommands();
    if ( ( result == true ) )
     // &&  ( flagGet( SYS_FLAG_MARUIOMAMORI ) == true )
     // &&  ( flagGet( SYS_FLAG_HIKARUOMAMORI) == true ) )
    {
      // 全図鑑コンプコマンドを発行
      AppendCommand( COMMAND_MESSAGE, 0, 
#if 0 //@attention DEPEND_ON_OLD_MODULE
        msg_hyouka_hakase1_all_comp
#else
        0
#endif
      ); // 全図鑑完成済みメッセージ
      AppendCommand( COMMAND_WAIT_LAST );                               // キーウェイト
      AppendCommand( COMMAND_CLOSE );                                   // ウィンドウクローズ
      AppendCommand( COMMAND_BREAK );                                   // 会話終了
    }
  }

  m_isEvaluted = true; // 評価完了

  return result;
}

//--------------------------------------------------------------
/**
 *  @brief   指定地方、もしくは全国の『捕獲』ポケモン数を取得 伝説も含める
 *  @param   [in]  mode  図鑑モード
 *  @return  『捕獲』ポケモン数
 */
//--------------------------------------------------------------
u16 ScriptToolPokeDex::GetPokeGetCount( Savedata::ZukanData::Mode mode /* = MODE_ZENKOKU */ )
{
  PokeTool::ExtendData::LocalArea area = modeToArea( mode );

  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : return m_zukanData->GetLocalPokeGetCount( area, m_extend );
  case Savedata::ZukanData::MODE_ZENKOKU : return m_zukanData->GetPokeGetCount();
  default                                : GFL_ASSERT( false );
  }

  return 0;
}

//--------------------------------------------------------------
/**
 *  @brief   指定地方、もしくは全国の『みた』ポケモン数を取得 伝説も含める
 *  @param   [in]  mode  図鑑モード
 *  @return  『みた』ポケモン数
 */
//--------------------------------------------------------------
u16 ScriptToolPokeDex::GetPokeSeeCount( Savedata::ZukanData::Mode mode /* = MODE_ZENKOKU */ )
{
  PokeTool::ExtendData::LocalArea area = modeToArea( mode );

  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : return m_zukanData->GetLocalPokeSeeCount( m_extend, area );
  case Savedata::ZukanData::MODE_ZENKOKU : return m_zukanData->GetPokeSeeCount();
  default                                : GFL_ASSERT( false );
  }

  return 0;
}

//! コマンドバッファのクリア
void ScriptToolPokeDex::ClearCommands()
{
  m_commandCount = 0;
  gfl2::std::MemClear( &m_commands[ 0 ], sizeof( m_commands ) );
}

//! コマンドの発行
void ScriptToolPokeDex::AppendCommand( u16 id, u16 param /* = 0 */, u32 value /* = 0 */)
{
  GFL_ASSERT( m_commandCount < CommandBufferMax );

  // 各モードで必要ないコマンドをはじく
  if ( m_isPcMode == true)
  {
    if ( id == COMMAND_ITEM_ADD     ) { return; }
    if ( id == COMMAND_MEMORY       ) { return; }
  }
  else
  {
    if ( id == COMMAND_MESSAGE_WAIT ) { return; }
  }

  m_commands[ m_commandCount ].id    = id;
  m_commands[ m_commandCount ].param = param;
  m_commands[ m_commandCount ].value = value;
  m_commandCount += 1;
}

//! Savedata::ZukanData::Mode から PokeTool::ExtendData::LocalArea への変換
PokeTool::ExtendData::LocalArea ScriptToolPokeDex::modeToArea( Savedata::ZukanData::Mode mode )
{
  switch ( mode )
  {
  case Savedata::ZukanData::MODE_LOCAL_A : return PokeTool::ExtendData::LOCAL_AREA_A;
  default                                : break;
  }

  return PokeTool::ExtendData::LOCAL_AREA_NONE;
}

//! PokeTool::ExtendData::LocalArea から Savedata::ZukanData::Mode への変換
Savedata::ZukanData::Mode ScriptToolPokeDex::areaToMode( PokeTool::ExtendData::LocalArea area )
{
  switch ( area )
  {
  case PokeTool::ExtendData::LOCAL_AREA_A : return Savedata::ZukanData::MODE_LOCAL_A;
  default                                  : break;
  }

  return Savedata::ZukanData::MODE_ZENKOKU;
}

//============================================================================//
//  private methods
//============================================================================//

//----------------------------------------//
//! フラグの取得
//----------------------------------------//
bool ScriptToolPokeDex::flagGet( u16 flag_no )
{
  return m_eventWork->CheckEventFlag( flag_no );
}

//----------------------------------------//
//! フラグの設定
//----------------------------------------//
void ScriptToolPokeDex::flagSet( u16 flag_no, bool value /* = true */ )
{
  if ( value == true )
  {
    m_eventWork->SetEventFlag( flag_no );
  }
  else
  {
    m_eventWork->ResetEventFlag( flag_no );
  }
}

//----------------------------------------//
//! フラグの初期化
//----------------------------------------//
void ScriptToolPokeDex::flagRes( u16 flag_no )
{
  flagSet( flag_no, false );
}

//--------------------------------------------------------------
/**
 *  @brief  地方図鑑の『みた(or 捕獲)』数から図鑑ランクを取得
 *  @param  [in]  count  『みた』数
 *  @return 図鑑ランク 0-7
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::localCountToRank( u16 count, Savedata::ZukanData::Mode mode )
{
  static const u16 table_[ 7 ] = // 一応容量をケチってu16
  {
       29, //  0
       59, //  1
       89, //  2
      119, //  3
      149, //  4
      179, //  5
    65535, //  6 ... これ以上増えるならそれも構わない // 207
  };

  int i;

  if ( IsComplete( mode ) == true )
  {
    return GFL_NELEMS( table_ );
  }

  for ( i = 0; i < GFL_NELEMS( table_ ); ++i )
  {
    if ( table_[i] >= count )
    {
      return i;
    }
  }

  return GFL_NELEMS( table_ ) - 1;
}

//--------------------------------------------------------------
/**
 *  @brief  全国図鑑の『捕獲(or みた)』数から図鑑ランクを取得
 *  @param  [in]  count  『捕獲』数
 *  @return 図鑑ランク 0-19
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::worldCountToRank( u16 count )
{
  static const u16 table_[ 19 ] = // 一応容量をケチってu16
  {
       79, //  0
      149, //  1
      199, //  2
      249, //  3
      299, //  4
      349, //  5
      399, //  6
      449, //  7
      499, //  8
      529, //  9
      559, // 10
      589, // 11
      614, // 12
      634, // 13
      654, // 14
      674, // 15
      689, // 16
      699, // 17
    65535  // 18 ... これ以上増えるならそれも構わない
  };

  int i;

  if  ( IsCompleteWorld() == true )
  {
    return 19; // 最大
  }

  for ( i = 0; i < GFL_NELEMS( table_ ); ++i )
  {
    if ( table_[i] >= count )
    {
      return i;
    }
  }

  return GFL_NELEMS( table_ ) - 1;
}

//--------------------------------------------------------------
/**
 *  @brief  全国図鑑のランクからMEを算出する
 *  @param  [in]  rank  全国図鑑のランク
 *  @return MEオフセット 0-7
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::worldRankToMeOffset( int rank )
{
  static const u8 offset_[ 20 ] =
  {
//- 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 -//
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7
  };

  GFL_ASSERT( ( rank >= 0 ) && ( rank < GFL_NELEMS( offset_ ) ) );

  return static_cast< int >( offset_[ rank ] );
}

//--------------------------------------------------------------
/**
 *  @brief  評価段階からMEを返す
 *  @param  [in]  eval  評価ランク
 *  @return ME No
 */
//--------------------------------------------------------------
int ScriptToolPokeDex::evalToMeNo( int eval )
{
  static const int me_[ 8 ] =
  {
    STRM_ME_HYOUKA01,
    STRM_ME_HYOUKA02,
    STRM_ME_HYOUKA03,
    STRM_ME_HYOUKA04,
    STRM_ME_HYOUKA05,
    STRM_ME_HYOUKA06,
    STRM_ME_HYOUKA07,
    STRM_ME_HYOUKA08
  };

  GFL_ASSERT( ( eval >= 0 ) && ( eval < GFL_NELEMS( me_ ) ) );

  return me_[ eval ];
}


GFL_NAMESPACE_END( Script )
GFL_NAMESPACE_END( Field )

#endif