//============================================================================================
/**
 * @file    WordsetLoader.h
 * @brief   wordsetでの事前バッファ読み込み管理
 * @author  tamada
 * @date  2012.08.23
 *
 * 3DSでは同期読み込みが問題となるため、事前に名称一覧のMsgDataを読み込んでおく必要がある。
 * WordSetに対して読み込み処理と読み込みデータを提供するためのクラス
 *
 * ポケモン名は常駐しているため指定する必要はない
 */
//============================================================================================
#if !defined( WORDSETLOADER_H_INCLUDED )
#define WORDSETLOADER_H_INCLUDED

#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/// 読み込み管理ID（WordsetLoader内部でのみ使用）
//------------------------------------------------------------------
enum {
  PRELOAD_NO_ITEMNAME = 0,
  PRELOAD_NO_WAZANAME,
  PRELOAD_NO_TYPENAME,
  PRELOAD_NO_TOKUSEI,
  PRELOAD_NO_SEIKAKU,
  PRELOAD_NO_PLACENAME,
  PRELOAD_NO_TRNAME,
/* @todo 未実装
  PRELOAD_NO_TRTYPE,
  PRELOAD_NO_ASSIST_POWER,
  PRELOAD_NO_KAWAIGARI_GOODS,
  PRELOAD_NO_POFURENAME,
  PRELOAD_NO_CONTEST,   //!< コンテスト関連　@note 単語以外のメッセージが含まれているため、サイズが大きい
  PRELOAD_NO_TREND,     //!< 流行語関連　@note 単語以外のメッセージが含まれているため、サイズが大きい
*/

  PRELOAD_NO_MAX,
};

//------------------------------------------------------------------
/**
 * @brief 読み込み指定用ビット定義
 * @note  src/print/WordSetLoader.cppのgarc_id_tableと並びをそろえること
 */
//------------------------------------------------------------------
enum {
  PRELOAD_ITEMNAME        = 1 << PRELOAD_NO_ITEMNAME,
  PRELOAD_WAZANAME        = 1 << PRELOAD_NO_WAZANAME,
  PRELOAD_TYPENAME        = 1 << PRELOAD_NO_TYPENAME,
  PRELOAD_TOKUSEI         = 1 << PRELOAD_NO_TOKUSEI,
  PRELOAD_SEIKAKU         = 1 << PRELOAD_NO_SEIKAKU,
  PRELOAD_PLACENAME       = 1 << PRELOAD_NO_PLACENAME,
  PRELOAD_TRNAME          = 1 << PRELOAD_NO_TRNAME,
/* @todo 未実装
  PRELOAD_TRTYPE          = 1 << PRELOAD_NO_TRTYPE,
  PRELOAD_ASSIST_POWER    = 1 << PRELOAD_NO_ASSIST_POWER,
  PRELOAD_KAWAIGARI_GOODS = 1 << PRELOAD_NO_KAWAIGARI_GOODS,
  PRELOAD_POFURENAME      = 1 << PRELOAD_NO_POFURENAME,
  PRELOAD_CONTEST         = 1 << PRELOAD_NO_CONTEST,    //!< コンテスト関連　@note 単語以外のメッセージが含まれているため、サイズが大きい
  PRELOAD_TREND           = 1 << PRELOAD_NO_TREND,      //!< 流行語関連　@note 単語以外のメッセージが含まれているため、サイズが大きい
*/
};

//============================================================================================
/**
 * @class WordSetLoader
 * @brief WordSetでの事前読み込み管理クラス
 */
//============================================================================================
class WordSetLoader
{

public:
  //------------------------------------------------------------------
  /**
   * @brief constructor
   * @param[in]   pHeap         読み込みデータ保持に使用するヒープ
   * @param[in]   read_id_bits  読み込むメッセージデータの指定(PRELOAD_～をorしたもの）
   */
  //------------------------------------------------------------------
  WordSetLoader( gfl2::heap::HeapBase *pHeap, u32 read_id_bits );

  //------------------------------------------------------------------
  /**
   * @brief  destructor
   */
  //------------------------------------------------------------------
  ~WordSetLoader();

  //------------------------------------------------------------------
  /**
   * @brief   読み込み開始
   * @param[in]   pTempHeap   読み込みに使用するテンポラリヒープ
   */
  //------------------------------------------------------------------
  void StartLoad( gfl2::heap::HeapBase *pTempHeap = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   読み込み待ち処理
   * @return  bool  trueのときセットアップ完了
   *
   * @note    全データ読み込み時に初めてMsgDataの生成を行っている
   */
  //------------------------------------------------------------------
  bool WaitLoad( void );

  //------------------------------------------------------------------
  /**
   * @brief   読み込み済みデータの解放
   */
  //------------------------------------------------------------------
  void Release( void );

  //------------------------------------------------------------------
  /**
   * @brief 指定IDのMsgDataを取得する
   * @param[in]   dat_id
   * @return      MsgDataへのポインタ、存在しない場合はNULLを返す
   *
   * @note    WordSet内部で使用される。
   */
  //------------------------------------------------------------------
  gfl2::str::MsgData * GetMsgData( u32 dat_id );


  //------------------------------------------------------------------
  /**
   * @brief   常駐バッファ読み込み処理
   * @param[in]   pHeap   読み込みデータ保持に使用するヒープ
   * @param[in]   bits    読み込むメッセージデータの指定(PRELOAD_～をorしたもの）
   *
   * @note  ゲーム全体を通して常駐させるバッファを読込
   */
  //------------------------------------------------------------------
  static void LoadGlobal( gfl2::heap::HeapBase * pHeap, u32 bits );

  //------------------------------------------------------------------
  /**
   * @brief   常駐バッファ再読み込み処理
   *
   * @note  LoadGlobalで設定、読み込んだバッファを解放、再読み込みする
   * @note  言語切り替えへの対応を行う
   */
  //------------------------------------------------------------------
  static void ReloadGlobal( void );

  static gfl2::str::MsgData * GetGlobalMsgData( u32 dat_id );

private:
  enum STATE {
    STATE_INIT,
    STATE_LOADING,
    STATE_LOADED,
  };

  //------------------------------------------------------------------
  /**
   * @brief MSGバッファ管理構造体
   */
  //------------------------------------------------------------------
  struct Buffer
  {
    void * mAddress;
    size_t mSize;
    u16     mGarcId;
    gfl2::str::MsgData * mMsgData;
  };

  gfl2::heap::HeapBase *      m_pHeap;
  const u32                   m_ReadBits;
  const u32                   m_ReadCount;
  STATE                       m_State;
  Buffer *                    m_BufferArray;
  bool                        m_bAppended;
  s32                         m_MessageArcId;

  /// グローバル
  static Buffer               s_StaticBuffer[ PRELOAD_NO_MAX ];
  static u32                  s_ReadBits;
  static gfl2::heap::HeapBase * s_BufferHeap;
};

#endif  // WORDSETLOADER_H_INCLUDED
