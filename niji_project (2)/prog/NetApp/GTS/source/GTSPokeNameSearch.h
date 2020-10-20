//============================================================================================
/**
 * @file		GTSPokeNameSearch.h
 * @brief   文字列からポケモン開発NOを取得(みつからなかったら-1
 * @author	Akito Mori / 移植 endo_akira
 * @date		12.08.17 / 15.09.03
 *
 * @note 日本語・英語は頭文字から対象を絞り込んでから探索するが、韓国語は\n
 * @note 子音指定ができないため全探索する
 *
 */
//============================================================================================
#if !defined( GTS_POKENAME_SEARCH_H_INCLUDE )
#define GTS_POKENAME_SEARCH_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_StrBuf.h>
#include <str/include/gfl2_MsgData.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "Savedata/include/ZukanSave.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
  
  /// 文字列からポケモンNOを取得するクラス
  class GtsPokeNameSearch
  {
    ///コピーコンストラクタ＋代入禁止
    GFL_FORBID_COPY_AND_ASSIGN(GtsPokeNameSearch); 


  public:
    //=========================================================================================
    /**
     * @brief コンストラクタ
     *
     * @param   heap       ヒープ
     * @param   pZukanData 図鑑セーブデータへのポインタ
     *
     * @retval  int 無かったら0,  1-750:ポケモンNO
     */
    //=========================================================================================
    GtsPokeNameSearch( app::util::Heap *heap, Savedata::ZukanData *pZukanData );

    //=========================================================================================
    /**
     * @brief デストラクタ
     */
    //=========================================================================================
    ~GtsPokeNameSearch( void );
    
    //=========================================================================================
    /**
     * @brief 文字列を渡すとポケモンNOを探索して返す
     *
     * @param   name [in] ポケモンの名前として探索する文字列
     *
     * @return  int 無かったら0,  1-750:ポケモンNO
     */
    //=========================================================================================
    int CheckString2PokeNo( gfl2::str::StrBuf *name, NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );

  private:

    int getMonsNo( gfl2::str::StrBuf *pMonsName, NetAppLib::Message::MessageUtility* pMessageUtility );

    //=========================================================================================
    /**
     * @brief 文字列から濁点・半濁点を削除する（「パ・バ」だったら「ハ」にする）
     *
     * @param   array [in/out] 文字列
     */
    //=========================================================================================
    void DeleteDakutenHandakuten( gfl2::str::STRCODE *array );

    //=========================================================================================
    /**
     * @brief 文字列を渡すとポケモンNOを探索して返す
     *
     * @param   name [in] ポケモンの名前として探索する文字列
     *
     * @return  int 無かったら0,  1-750:ポケモンNO
     */
    //=========================================================================================
    int  GetString2SearchKey( gfl2::str::StrBuf *name );

    //=========================================================================================
    /**
     * @brief ひらがなをカタカナに変換する
     *
     * @param   name [in]  入力文字列
     * @param   name [out] カタカナに変換が終了した文字列
     */
    //=========================================================================================
    void ChangeHiraToKatakana( gfl2::str::StrBuf *src, gfl2::str::StrBuf *des );
    
  private:
    app::util::Heap      *m_heap;         ///< ヒープ
    gfl2::str::StrBuf    *m_search_str;   ///< 頭文字変換後用文字列格納バッファ
    gfl2::str::StrBuf    *m_kata_str;     ///< ひらがな→カタカナ変換後文字列格納バッファ
    Savedata::ZukanData  *m_pZukanData;   ///< 図鑑フラグセーブポインタ

    static const int TRANS_TOPWORD_TABLE_NUM_JP  = 31;  ///< 濁音・半濁音の数
    static const int SEARCH_TOPWORD_TABLE_NUM_JP = 89;  ///< 頭文字の数
  };
  
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTS_POKENAME_SEARCH_H_INCLUDE
