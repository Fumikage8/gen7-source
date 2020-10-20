//=============================================================================================
/**
 * @file  btl_str.h
 * @brief ポケモンXY バトルシステム 文字列処理モジュール
 * @author  taya
 *
 * @date  2011.01.20  作成
 */
//=============================================================================================
#ifndef __BTL_STR_H__
#define __BTL_STR_H__

//#include  "message/msg_btl_set.h"
//#include  "message/msg_btl_std.h"
//#include  "message/msg_btl_ui.h"
//#include  "message/msg_btl_attack.h"
//#include  "message/msg_waza_oboe.h"
//#include  "message/msg_yesnomenu.h"

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <Print/include/WordSet.h>
#include <Trainer/Trainer/include/TrainerMessage.h>
#include  "./btl_common.h"
#include  "./btl_PokeCon.h"

namespace trainer
{
  enum trainerID;
  enum MsgID;
}   // end of namespace 'trainer'


GFL_NAMESPACE_BEGIN(btl)

/**
 *  文字列タイプ
 */
typedef enum {
  BTL_STRTYPE_NULL = 0,
  BTL_STRTYPE_STD,
  BTL_STRTYPE_SET,
  BTL_STRTYPE_UI,
  BTL_STRTYPE_WAZA,
  BTL_STRTYPE_YESNO,

  BTL_STRTYPE_MAX,
}BtlStrType;

// 前方参照
class MainModule;

namespace str {

/**
 *  定数群
 */
enum {
  MSG_ARG_MAX = 8,
};

//=============================================================================================
/**
 * システム初期化（バトル起動時に１回）
 *
 * @param   mainModule
 * @param   playerClientID
 * @param   pokeCon
 * @param   pHeap
 *
 * @retval  extern void
 */
//=============================================================================================
extern void InitSystem( const MainModule* mainModule, u8 playerClientID, const POKECON* pokeCon, gfl2::heap::HeapBase *pHeap , bool isAsync = false);


//=============================================================================================
/**
 * メッセージの非同期読みセット
 */
//=============================================================================================
extern void StartLoadMessage( gfl2::fs::AsyncFileManager *readMng, gfl2::heap::HeapBase *dataHeap , gfl2::heap::HeapBase *workHeap);
extern bool IsFinishLoadMessage(gfl2::fs::AsyncFileManager *readMng );
extern void CreateMessage( gfl2::fs::AsyncFileManager *readMng, gfl2::heap::HeapBase *pHeap );

//=============================================================================================
/**
 * システム終了（バトル終了時に１回）
 *
 * @param   none
 *
 * @retval  extern void
 */
//=============================================================================================
extern void QuitSystem( void );


//=============================================================================================
/**
 * 標準メッセージの生成
 * ※標準メッセージ：対象者なし、あるいは対象が誰であっても一定のフォーマットで生成される文字列
 *
 * @param   buf       [out] 生成文字列格納バッファ
 * @param   strID     文字列ID
 * @param   numArgs   可変個引数の数
 * @param   ...       引数（可変）
 *
 */
//=============================================================================================
extern void MakeStringStd(STRBUF* buf, u32 strID, u32 numArgs, ...);
//加工無しのStdを受け取る(WordSet展開もされていません
extern void MakeStringStdPlane(STRBUF* buf, u32 strID);


//=============================================================================================
/**
 * 標準メッセージの生成（引数配列渡し版）
 *
 * @param   buf
 * @param   strID
 * @param   args
 */
//=============================================================================================
extern void MakeStringStdWithArgArray( STRBUF* buf, u32 strID, const int* args );


//=============================================================================================
/**
 * セットメッセージの生成
 * ※セットメッセージ：自分側，敵側（やせいの），敵側（あいての）が一定の法則でセットになった文字列
 *
 * @param[out]   buf
 * @param[in]    strID
 * @param[in]    args
 *
 */
//=============================================================================================
extern void MakeStringSet( STRBUF* buf, u32 strID, const int* args );


//=============================================================================================
/**
 * ワザメッセージの生成
 * ※ワザメッセージ：○○の××こうげき！とか。セットメッセージと同様、必ず４つセット。
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 * @param[in     args
 *
 */
//=============================================================================================
extern void MakeStringWaza( STRBUF* dst, u8 pokeID, u16 waza );

//=============================================================================================
/**
 * 全力技のワザメッセージの生成
 * ※変化技を全力技として使用した場合の「○○の××ブースト！」
 * ※自分の、野生の、相手の、ぬしの　の４つセット
 * ※ローカライズ要望により、ワードセットで解決せずに、技ごとにテキストを用意
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 * @param[in     args
 *
 */
//=============================================================================================
extern void MakeStringZenryokuWaza( STRBUF* dst, u8 pokeID, u16 waza );

//=============================================================================================
/**
 * @brief 全力技の名前を取得する
 * @param[out] dst   全力技名の格納先
 * @param[in]  waza  全力技の技No.
 */
//=============================================================================================
extern void GetZenryokuWazaName( STRBUF* dst, u16 waza );

//=============================================================================================
/**
 * UI用文字列取得
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 *
 */
//=============================================================================================
extern void GetUIString( STRBUF* dst, u16 strID );

//=============================================================================================
/**
 * ワザ選択時UI用文字列生成
 *
 * @param[out]   dst
 * @param[in]    wazaID
 * @param[in]    wazaPP
 * @param[in]    wazaPPMax
 */
//=============================================================================================
extern void MakeWazaUIString( STRBUF* dst, u16 wazaID, u8 wazaPP, u8 wazaPPMax );

//=============================================================================================
/**
 * ２択メッセージの生成
 *
 * @param[out]   dst
 * @param[in]    strID
 * @param[in]    args
 *
 */
//=============================================================================================
extern void MakeStringYesNoWithArgArray( STRBUF* dst, u32 strID, const int* args );

//=============================================================================================
/**
 * レベルアップウィンドウテキスト（前レベルとの差分）生成
 *
 * @param[out]   dst    生成先バッファ
 * @param[in]    hp
 * @param[in]    atk
 * @param[in]    def
 * @param[in]    sp_atk
 * @param[in]    sp_def
 * @param[in]    agi
 */
//=============================================================================================
extern void MakeString_LvupInfo_Diff( print::WordSet* wset, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi );

//=============================================================================================
/**
 * レベルアップウィンドウテキスト（新レベルパラメータ）生成
 *
 * @param[out]   dst    生成先バッファ
 * @param[in]    hp
 * @param[in]    atk
 * @param[in]    def
 * @param[in]    sp_atk
 * @param[in]    sp_def
 * @param[in]    agi
 */
//=============================================================================================
extern void MakeString_LvupInfo_Param( print::WordSet* wset, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi );

//=============================================================================================
/**
 * トレーナーメッセージの生成
 *
 * @param[out]   buf        生成文字列格納バッファ
 * @param[in]    trainerID  トレーナーID
 * @param[in]    msgID      メッセージ種類
 *
 */
//=============================================================================================
extern void MakeStringTrainer( STRBUF* buf, trainer::TrainerID trainerID, trainer::MessageKind msgID);

//=============================================================================================
/**
 * タグの展開
 *
 * @param[out]   dst        生成文字列格納バッファ
 * @param[in]    src        展開前の文字列
 *
 */
//=============================================================================================
extern void SetStringExpanded( STRBUF* dst, const STRBUF* src);

//=============================================================================================
/**
 * @brief ニックネームを、未発見ウルトラビースト用の文字列に変更する
 *
 * @param[out] poke  ニックネーム変更対象のポケモン
 *
 */
//=============================================================================================
extern void SetUnknownUBNickName( HeapHandle heap, pml::pokepara::PokemonParam* poke );

}   // end of namespace 'str'

GFL_NAMESPACE_END(btl)


#endif  // #ifndef __BTL_STR_H__
