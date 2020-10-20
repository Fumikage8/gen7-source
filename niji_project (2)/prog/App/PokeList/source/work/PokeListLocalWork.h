#ifndef NIJI_PROJECT_APP_POKELIST_LOCALWORK_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_LOCALWORK_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListLocalWork.h
 * @author  fukushima_yuya
 * @date    2015.03.06
 * @brief   ポケモンリスと画面のローカルワーク
 *          sango_project app_pokelist_LocalWork
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListConst.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
class WordSetLoader;


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! @brief  画面モード
enum DISP_MODE {
  DISP_MODE_NORMAL,
  DISP_MODE_POKE_EXCHANGE,
  DISP_MODE_ITEM_EXCHANGE,
  DISP_MODE_WINDOW,
  DISP_MODE_NUM
};

//------------------------------------------------------------------------------
// @brief   ローカルワーク
//------------------------------------------------------------------------------
class LocalWork
{
  GFL_FORBID_COPY_AND_ASSIGN( LocalWork );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeap     ヒープ
   */
  //------------------------------------------------------------------
  LocalWork( gfl2::heap::HeapBase* pHeap, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~LocalWork( void ) { ; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータを取得する
   * @return  メッセージデータ
   */
  //------------------------------------------------------------------
  gfl2::str::MsgData* GetMsgData( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   WORDSETを取得する
   * @return  WORDSET
   */
  //------------------------------------------------------------------
  print::WordSet* GetWordSet( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   預かった道具をセットする
   * @param   item_no  道具No.
   */
  //------------------------------------------------------------------
  void SetStoredItemNo( u16 item_no );

  //------------------------------------------------------------------
  /**
   * @brief   預かった道具を取得する
   * @return  預かった道具番号
   */
  //------------------------------------------------------------------
  u16 GetStoredItemNo( void ) const;


public:
  //------------------------------------------------------------------
  /**
   * @brief   画面モードの変更
   * @param   mode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispMode( DISP_MODE mode );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの取得
   * @return  画面モード
   */
  //------------------------------------------------------------------
  DISP_MODE GetDispMode( void ) const;


private:
  gfl2::str::MsgData*   m_pMsgData;   //!< メッセージデータ
  print::WordSet*       m_pWordSet;   //!< 単語セット
  WordSetLoader*        m_pWordSetLoader;   //!< 

  u16   m_StoredItemNo;   //!< 預かった道具

  DISP_MODE   m_DispMode;   //!< 画面モード
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_LOCALWORK_H_INCLUDED
