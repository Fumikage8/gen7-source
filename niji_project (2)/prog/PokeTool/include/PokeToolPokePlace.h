//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   poke_place.h
 *  @brief  ポケモンの捕まえた場所のメッセージデータ取得
 *  @author Toru=Nagihashi
 *  @date   2012.05.08
 *  @note   静的クラスです。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __POKE_PLACE_H__
#define __POKE_PLACE_H__
#pragma once


#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/message/msg_place_name.h>
#include <niji_conv_header/message/msg_place_name_out.h>
#include <niji_conv_header/message/msg_place_name_spe.h>
#include <niji_conv_header/message/msg_place_name_per.h>

GFL_NAMESPACE_BEGIN(PokeTool)

class PokePlace
{
  GFL_FORBID_COPY_AND_ASSIGN(PokePlace);
  PokePlace(){} //静的クラスなので生成禁止
public:
  static const int STRBUF_PLACE_NAME_LEN  = 128;
public:
  
  enum LoadType
  {
    LOAD_TYPE_REALTIME,   //関数使用時にその場で読み込みを行う
    LOAD_TYPE_PART,       //事前にすべてのMSGDATAを呼んでおく（部分読み込み）
    LOAD_TYPE_FULL,       //事前にすべてのMSGDATAを呼んでおく（フル読み込み）
  };

  //! 産地マークID
  enum PlaceMarkID
  {
    PLACE_MARK_CALOS = 0,     //!< カロス地方
    PLACE_MARK_ALOLA,     //!< アローラ地方
    PLACE_MARK_NONE,  //!< マーク表示なし
    PLACE_MARK_VC,     //!< バーチャルコンソール地方
    PLACE_MARK_HOLOHOLO,     //!< ホロホロ地方
  };

  /**
   * @brief 初期化
   *
   * @param heap    ヒープ
   * @param type    読み込みタイプ
   */
  static void Initialize( gfl2::heap::HeapBase *heap, LoadType type );
  /**
   * @brief 破棄
   */
  static void Finalize( void );

  /**
   *  @bief ポケモン捕まえた場所のアーカイブ内データのID取得
   *  @retval ポケモン捕まえた場所のアーカイブ内データのID
   */
  static gfl2::str::MsgData * GetMessageDatID( u32 place_no, gfl2::fs::ArcFile::ARCDATID * datID );

  /**
   *  @bief 上記のアーカイブデータ内のメッセージ番号取得
   *  @retval 上記のアーカイブデータ内のメッセージ番号
   */
  static u32 GetMessageID( u32 place_no );

  /**
   *  @bief 地名の名前を生成（解放は各自行なってください）
   *  @retval 地名の名前
   */
  static gfl2::str::StrBuf * CreatePlaceName( u32 place_no, gfl2::heap::HeapBase *temporary_heap );

  /**
   *  @bief バージョンの名前を生成（解放は各自行なってください）
   *  @retval バージョンの名前
   */
  static gfl2::str::StrBuf * CreateVersionName( u32 version, gfl2::heap::HeapBase *temporary_heap);

  /**
   *  @bief    IDを六桁表示するバージョン
   *  @retval  ６けたならtrue
   */
  static bool GetVersionSixID( u32 version );

  /**
   * @brief   カロス地方かどうか
   * @param   version   ROMバージョン
   * @retval  true = カロス地方
   */
  static bool IsCalos( u8 version );

  /**
   * @brief   アローラ地方かどうか
   * @param   version   ROMバージョン
   * @retval  true = アローラ地方
   */
  static bool IsAlola( u8 version );


  /**
   * @brief   ホロホロ地方かどうか
   * @param   version   ROMバージョン
   * @retval  true = ホロホロ地方
   */
  static bool IsHoloholo( u8 version );

  /**
   * @brief   バーチャルコンソール地方かどうか
   * @param   version   ROMバージョン
   * @retval  true = バーチャルコンソール地方
   */
  static bool IsVc( u8 version );
  
  /**
   * @brief   産地マークIDを取得
   * @param   version   ROMバージョン
   * @return  産地マークID
   */
  static PlaceMarkID GetPlaceMarkID( u8 version );


  
  
public:
  /**
   *  @brief PokemonParamのplaceに入っている値
   *        PLACE_NO_START_のオフセットにより、どのメッセージデータを使うのかを判定し、
   *        残りの値により、メッセージデータの何番の文字列を使うのかが決まる。
   */ 
  enum PlaceNo
  {
    PLACE_NO_START_NORMAL   = 0,      ///<内部
    PLACE_NO_START_SPECIAL  = 30001,  ///<特殊
    PLACE_NO_START_OUTER    = 40001,  ///<外部
    PLACE_NO_START_PERSON   = 60001,  ///<人物用
    PLACE_NO_END            = 65535,
 
    //特殊
#if 0 //kujiraから持ってきたポケモンをplace登録しなくなった
    PLACE_NO_SPECIAL_PAST_TRANSLATE = PLACE_NO_START_SPECIAL+MAPNAME_POKESIHUTAH,
#endif
    PLACE_NO_SPECIAL_INNER_TRADE    = PLACE_NO_START_SPECIAL+MAPNAME_NAIBUTUUSINKOUKAN,  //つうしんこうかん(ゲーム内)
    PLACE_NO_SPECIAL_OUTER_TRADE    = PLACE_NO_START_SPECIAL+MAPNAME_TUUSINKOUKAN,  //つうしんこうかん(人と通信)

    PLACE_NO_SPECIAL_KANTOU         = PLACE_NO_START_SPECIAL+MAPNAME_KANTO,  //カントー
    PLACE_NO_SPECIAL_JYOUTO         = PLACE_NO_START_SPECIAL+MAPNAME_ZYOTO,  //ジョウト
    PLACE_NO_SPECIAL_HOUEN          = PLACE_NO_START_SPECIAL+MAPNAME_HOUEN,  //ホウエン
    PLACE_NO_SPECIAL_SHINOU         = PLACE_NO_START_SPECIAL+MAPNAME_SINOH,  //シンオウ
    
    PLACE_NO_SPECIAL_ISSHU          = PLACE_NO_START_SPECIAL+MAPNAME_IISHU, //イッシュ
    PLACE_NO_SPECIAL_3DSLINK        = PLACE_NO_START_SPECIAL+MAPNAME_3DSLINK, //3DSリンク

    PLACE_NO_SPECIAL_KAROSU         = PLACE_NO_START_SPECIAL+MAPNAME_KAROSU, //カロス
    
//    PLACE_NO_SPECIAL_HOLOHOLO       = PLACE_NO_START_SPECIAL+MAPNAME_HOLOHOLO, //ホロホロ   // @note place_name_out.mstxtを使用するので、この定義はいらない
    PLACE_NO_SPECIAL_VC             = PLACE_NO_START_SPECIAL+MAPNAME_VCKANTOH,   //バーチャルコンソール

    PLACE_NO_SPECIAL_POKERESORT     = PLACE_NO_START_SPECIAL+MAPNAME_POKERESORT, //ポケリゾート

    PLACE_NO_SPECIAL_FAR_PLACE      = PLACE_NO_START_SPECIAL+MAPNAME_TOOKUHANARETATOTI,  //とおいとち(XD・コロシアム
    PLACE_NO_SPECIAL_UNKNOWN        = PLACE_NO_START_SPECIAL+MAPNAME_HUSEINAKOHDO,  //---------- 不明

    //人物
    // ここに定義を追加した場合、
    // field_convert/script_event_data/add_poke/add_poke.rb(94): $def_egg_hash = {
    // にも反映する
    PLACE_NO_PERSON_SODATEYA      = PLACE_NO_START_PERSON+MAPNAME_SODATEYA,  //育て屋夫婦
    PLACE_NO_PERSON_TOREZYAHANTA  = PLACE_NO_START_PERSON+MAPNAME_TOREZYAHANTA,  //トレジャーハンター
    PLACE_NO_PERSON_ONSENBAASAN   = PLACE_NO_START_PERSON+MAPNAME_ONSENBAASAN,  // 温泉ばあさん

    //不正値用
    PLACE_NO_UNKNOWN_PLACE  = MAPNAME_TOOIBASYO,
    PLACE_NO_UNKNOWN_PERSON = PLACE_NO_START_PERSON+MAPNAME_TOOKUNIIRUHITO,
  };
  
private:
  static gfl2::str::MsgData *m_place_msg;
  static gfl2::str::MsgData *m_spe_msg;
  static gfl2::str::MsgData *m_out_msg;
  static gfl2::str::MsgData *m_per_msg;
};


GFL_NAMESPACE_END(PokeTool)

#endif    // __POKE_PLACE_H__
