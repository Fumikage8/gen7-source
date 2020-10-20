//=============================================================================
/**
 *
 *  @file   ScriptToolPokeDex.h
 *  @brief  図鑑データアクセサ
 *          パソコン画面とスクリプトからの呼出しを共有化する
 *  @author masanori funakoshi
 *  @data   2012.09.25
 *
 */
//=============================================================================

#if 0 //nijiで未使用になった by iwasawa 160524

#ifndef __SCRIPTTOOLPOKEDEX_H__
#define __SCRIPTTOOLPOKEDEX_H__

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "GameSys/include/GameData.h"
namespace GameSys{
  class GameManager;
}

/**
 * @brief フィールド
 */
GFL_NAMESPACE_BEGIN( Field )
/**
 * @brief スクリプト
 */
GFL_NAMESPACE_BEGIN( Script )


/* ------------------------------------------------------------------------- */
/**
 * @class ScriptToolPokeDex 
 * @brief ポケモン図鑑操作ツール
 */
/* ------------------------------------------------------------------------- */
class ScriptToolPokeDex
{
  GFL_FORBID_COPY_AND_ASSIGN( ScriptToolPokeDex ); ///< コピーコンストラクタ + 代入禁止

// defines
public :
  enum tagIdentifier
  {
    CommandBufferMax = 32 ///< 8 * 32 = 256 byte(s)
  };

  //! 簡易スクリプト風コマンド群
  enum CommandId
  {
    COMMAND_NULL,         ///< 終端コマンド

    COMMAND_NOP,          ///< no operation

    COMMAND_CONTINUE,     ///< スクリプトのループ
    COMMAND_BREAK,        ///< スクリプトの終了

    COMMAND_MESSAGE,      ///< メッセージ描画
    COMMAND_MESSAGE_WAIT, ///< メッセージ終了待ち
    COMMAND_WAIT_MARK,    ///< ＡＢキー待ち『▼』
    COMMAND_WAIT_LAST,    ///< 最終キー待ち
    COMMAND_CLOSE,        ///< メッセージ閉じ

    COMMAND_SELECT,       ///< 図鑑選択(dummy)

    COMMAND_WORD_SET_NUM, ///< 数値をWordSetする

    COMMAND_ITEM_ADD,     ///< アイテム追加(フィールドのみ有効)

    COMMAND_ME_PLAY,      ///< ME再生
    COMMAND_ME_WAIT,      ///< ME待ち

    COMMAND_MEMORY,       ///< 思い出(フィールドのみ有効)

    COMMAND_MAX
  };

  //! コマンド、バリューペア
  typedef struct tagCommandData
  {
    u16 id;    ///< see enum CommandId
    u16 param; ///< slotNo(WORD_SET_NUM)
    u32 value; ///< 0 or messageId(MESSAGE) or soundId(ME_PLAY or ME_WAIT) or number(WORD_SET_NUM)
  } CommandData;

// methods
public :
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   *
   * @param heap ヒープへのポインタ
   * @param gameManager ゲームマネージャへのポインタ
   * @param is_pc_mode PCモードフラグ
   */
  /* ------------------------------------------------------------------------- */
  explicit ScriptToolPokeDex( gfl2::heap::HeapBase* heap, GameSys::GameManager *gameManager, bool is_pc_mode = true );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~ScriptToolPokeDex();

  /**
   *  @brief  図鑑評価の生成 モード指定版
   *  - プロパティにアクセスする前に実行する
   */
  int  EvaluteWithMode( Savedata::ZukanData::Mode       mode = Savedata::ZukanData::MODE_ZENKOKU       );
  /**
   *  @brief  図鑑評価の生成
   *  - プロパティにアクセスする前に実行する
   */
  int  Evalute        ( PokeTool::ExtendData::LocalArea area = PokeTool::ExtendData::LOCAL_AREA_NONE );

  /**
   *  @brief  初回図鑑評価の生成
   *  - プラターヌ博士の初回会話に挟み込むための専用関数
   */
  int  Evalute1st     ();

  //! 全ての図鑑が完成済みか？
  bool IsCompleteAll  ( bool makeCommands = false );

  /*!
   *  @brief  指定図鑑が完成済みか？
   *  @note   地方図鑑は『みた』、全国図鑑は『捕獲』
   */
  bool IsComplete( Savedata::ZukanData::Mode mode = Savedata::ZukanData::MODE_ZENKOKU, bool is_self = false );

  /*!
   *  @brief  地方図鑑の『みた』コンプリートチェック
   *  @note   デフォルトは『みた』
   */
  bool IsCompleteLocal();

  //! 全国図鑑が完成済みか？
  bool IsCompleteWorld();

  //! 地方図鑑の『捕獲』コンプリートチェック
  bool IsGetComplete(  Savedata::ZukanData::Mode mode, bool is_self = false );

  //! 全地方図鑑の『捕獲』コンプリートチェック
  bool IsGetCompleteLocal();

  //! 指定した図鑑の捕獲カウントを取得 伝説も含める
  u16  GetPokeGetCount( Savedata::ZukanData::Mode mode = Savedata::ZukanData::MODE_ZENKOKU );

  //! 指定した図鑑のみたカウントを取得 伝説も含める
  u16  GetPokeSeeCount( Savedata::ZukanData::Mode mode = Savedata::ZukanData::MODE_ZENKOKU );

  //! コマンドバッファのクリア
  void ClearCommands();

  //! コマンドの発行
  void AppendCommand( u16 id, u16 param = 0, u32 vallue = 0);

  //! Savedata::ZukanData::Mode から PokeTool::ExtendData::LocalArea への変換
  static PokeTool::ExtendData::LocalArea modeToArea( Savedata::ZukanData::Mode mode );

  //! PokeTool::ExtendData::LocalArea から Savedata::ZukanData::Mode への変換
  static Savedata::ZukanData::Mode areaToMode( PokeTool::ExtendData::LocalArea area );


private :
  bool flagGet( u16 flag_no );                    ///< ビットフラグを取得
  void flagSet( u16 flag_no, bool value = true ); ///< ビットフラグを設定
  void flagRes( u16 flag_no );                    ///< ビットフラグをオフ

  int  localCountToRank( u16 count, Savedata::ZukanData::Mode mode );             ///< 地方図鑑の『みた』数から図鑑ランクを取得
  int  worldCountToRank( u16 count );             ///< 全国図鑑の『捕獲』数から図鑑ランクを取得

  int  worldRankToMeOffset( int rank );           ///< 全国図鑑のランクからMEオフセットを算出する

  int  evalToMeNo( int eval );                    ///< 図鑑評価からME番号を算出する

// properties
private :
  //! 評価済み(有効なデータ)か？
  inline bool isEvaluted() const { return m_isEvaluted; }

public :
  //! 図鑑評価を続けるか？
  inline bool IsContinue() const
  {
    GFL_ASSERT( isEvaluted() == true );
    return  m_isContinue;
  }

  //! 図鑑評価を終了するか？
  inline bool IsBreak() const
  {
    GFL_ASSERT( isEvaluted() == true );
    return ( IsContinue() == false );
  }

  //! 評価ランクの取得
  inline int GetRank() const
  {
    GFL_ASSERT( isEvaluted() == true );
    return m_rank;
  }

  //! 発生するME番号の取得
  inline int GetMENo() const
  {
    GFL_ASSERT( isEvaluted() == true );
    return m_meNo;
  }

  ///! コマンド数を取得
  inline int GetCommandCount() const
  {
    GFL_ASSERT( isEvaluted() == true );
    return m_commandCount;
  }

  ///! インデックスからコマンドを取得
  inline const CommandData *GetCommandAt( int index ) const
  {
    GFL_ASSERT( isEvaluted() == true );
    if ( index >= m_commandCount )
    {
      return NULL;
    }
    if ( m_commands[ index ].id == COMMAND_NULL )
    {
      return NULL;
    }
    return &m_commands[ index ];
  }

// fields
private :
  GameSys::GameManager* m_gameManager; ///< ゲームマネージャへのポインタ
  Savedata::ZukanData*     m_zukanData;   ///< 図鑑データへのポインタ
  EventWork*               m_eventWork;   ///< イベントワークへのポインタ

  PokeTool::ExtendData*   m_extend;      ///< ポケモン拡張データへのポインタ

  bool m_isPcMode;   ///< 呼出し元

  bool m_isEvaluted; ///< 評価済みか？
  bool m_isContinue; ///< 処理を続けるか？

  int  m_rank;       ///< 評価ランク
  int  m_meNo;       ///< 鳴らすME

  int         m_commandCount;                 ///< 実行するコマンド数
  CommandData m_commands[ CommandBufferMax ]; ///< 実行するコマンド群
};


GFL_NAMESPACE_END( Script )
GFL_NAMESPACE_END( Field )

  
#endif // __SCRIPTTOOLPOKEDEX_H__

#endif
