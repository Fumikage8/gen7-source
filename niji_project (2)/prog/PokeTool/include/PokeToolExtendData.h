//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_tool_ExtendData.h
 *	@brief  パーソナル外にあるポケモンごとのデータ
 *	@author	Toru=Nagihashi
 *	@date		2012.05.24
 *	@author	kawada_koji
 *	@date		2015.08.12
 *
 *  @note   現在のパーソナル外データ一覧
 *          ・１．地方図鑑
 *          ・２．形状
 *          ・３．頭文字
 *
 *          上記１．２．３．４．はresource/zukan_dataにてコンバートしています。
 *
 *  
 *  kujira地方図鑑番号について
 *    名木橋が西野さんに聞き取り
 *    ・地方図鑑にエリアが複数そんざいし、エリアごとに１～MAXまでの値が割り振られる。
 *    ・エリアに重複するポケモンはいない
 *    ・同じポケモンだが、フォルム別でエリアが違うというものは存在しない
 *      （普通に考えるとエリアによって動植物の形態は変わるので、
 *        アッパーとかでやりたいっていいそう…。カラナクシの前例もあるし。）
 *
 *
 *  niji地方図鑑番号について
 *    ・アローラ全域エリアの図鑑、島１エリアの図鑑、島２エリアの図鑑、島３エリアの図鑑、島４エリアの図鑑がある。
 *    ・同じポケモンが複数のエリアに出る。例：ピカチュウのアローラ図鑑番号が20で島１図間番号が5。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __POKETOOL_EXTENDDATA_H_INCLUDED__)
#define __POKETOOL_EXTENDDATA_H_INCLUDED__
#pragma once


// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>

// pmlib
#include <pml/include/pmlib.h>

// niji
#include <PokeTool/include/PokeTool.h>


GFL_NAMESPACE_BEGIN(PokeTool)

class ExtendData
{
  GFL_FORBID_COPY_AND_ASSIGN(ExtendData);
public:
  /**
   * @brief 形状の種類
   */
  enum StyleType
  {
    STYLE_TYPE_CIRCLE,         //"丸型"=>0,
    STYLE_TYPE_LEG,            //"脚型"=>1,
    STYLE_TYPE_FISH,           //"魚型"=>2,
    STYLE_TYPE_INSECT,         //"昆虫型"=>3,
    STYLE_TYPE_QUADRUPED,      //"四足型"=>4,
    STYLE_TYPE_FOURTHWINGS,    //"四枚羽型"=>5,
    STYLE_TYPE_SWARM,          //"集合型"=>6,
    STYLE_TYPE_POLYPODIA,      //"多足型"=>7,
    STYLE_TYPE_STRAIGHTTRUNK,  //"直立胴型"=>8, 
    STYLE_TYPE_DOUBLELEGSTAIL, //"二足尻尾型"=>9,
    STYLE_TYPE_DOUBLEHUMAN,    //"二足人型"=>10,
    STYLE_TYPE_DOUBLEWINGS,    //"二枚羽型"=>11,
    STYLE_TYPE_CREEPNOREGS,    //"腹這脚無型"=>12,
    STYLE_TYPE_ARM,            //"腕型"=>13,

    STYLE_TYPE_MAX,
    STYLE_TYPE_EMPTY  = STYLE_TYPE_MAX, //未入力
    STYLE_TYPE_UNKNOWN,                 //未知の方
  };
  
  /**
   * @brief 地方番号がないポケモン(＝カロス地方のポケモンではない）
   */
  static const u32 LOCAL_NUMBER_NONE = 0;
  
  /**
   * @brief 地方図鑑エリア
   * 
   * @note  今回地方図鑑は複数のエリアがあり、
   *        エリアごとに番号１～MAXまでの値が割り振られます。
   */
  enum LocalArea
  {
    LOCAL_AREA_NONE,  ///<エリアなし(＝この地方のポケモンではない）
    LOCAL_AREA_A,  ///<アローラ全域エリア
    LOCAL_AREA_B,  ///<島１エリア
    LOCAL_AREA_C,  ///<島２エリア
    LOCAL_AREA_D,  ///<島３エリア
    LOCAL_AREA_E,  ///<島４エリア
    LOCAL_AREA_MAX,

    // 以下、制御用の値
    LOCAL_AREA_START = LOCAL_AREA_A,  // LOCAL_AREA_START<= <=LOCAL_AREA_END
    LOCAL_AREA_END   = LOCAL_AREA_E,
    LOCAL_AREA_NUM   = LOCAL_AREA_END - LOCAL_AREA_START +1,  // エリア数
  };

public:
  //--------------------------------------------------------------------
  // 生成・破棄
  //--------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap  ヒープ
   */
  ExtendData( gfl2::heap::HeapBase *heap );
  /**
   * @brief デストラクタ
   */
  virtual ~ExtendData();
  
  /**
   * @brief 同期読み込み
   *
   * @param heap  ヒープ
   */
  void Load( gfl2::heap::HeapBase *heap );

  //--------------------------------------------------------------------
  // 取得
  //--------------------------------------------------------------------
  /**
   * @brief 地方図鑑にいるかどうか
   *
   * @return true地方図鑑にいる falseいない
   */
  bool IsExistLocalZukan( u32 zenkoku_num, LocalArea local_area ) const;
  bool IsExistLocalZukan( const pml::pokepara::CoreParam *core_param, LocalArea local_area ) const;

  // ファイル読み込みなしで使えるstatic版。LOCAL_AREA_Aのみ対応。
  static bool IsExistLocalZukanStatic( u32 zenkoku_num, LocalArea local_area );

  /**
   * @brief 地方図鑑番号を取得
   *
   * @return 地方図鑑番号 いない場合はLOCAL_NUMBER_NONE
   * 
   * @note 今回は地方ごとにエリアがあります。
   */
  u32 GetLocalNumber( u32 zenkoku_num, LocalArea local_area ) const;
  u32 GetLocalNumber( const pml::pokepara::CoreParam *core_param, LocalArea local_area ) const;

  // ファイル読み込みなしで使えるstatic版。LOCAL_AREA_Aのみ対応。
  static u32 GetLocalNumberStatic( u32 zenkoku_num, LocalArea local_area );
  
  /**
   * @brief 地方図鑑のモンスター番号最大を取得
   *
   * @param local_area  ローカルエリア
   *
   * @return 最大数
   *
   * @note  例：ローカル図鑑に302匹おり、1～302のローカル図鑑番号が割り振られていたら、302を返す。
   * @note  sangoのときは上記例では303を返していた。
   */
  u32 GetLocalAreaMonsMax( LocalArea local_area ) const;

  /**
   * @brief 全国図鑑番号を取得
   *
   * @return 全国図鑑番号
   */
  u32 GetZenkokuNumber( u32 local_num, LocalArea local_area ) const;
  
  /**
   * @brief   形状取得
   *
   * @return  形状
   */
  StyleType GetStyleType( const SimpleParam & simple_param ) const;
  StyleType GetStyleType( const pml::pokepara::CoreParam *core_param ) const;
  
  /**
   * @brief 図鑑で使う
   *
   * @param monsno
   * @param formno
   *
   * @return 
   */
  u32 GetTableIndex( MonsNo monsno, FormNo formno ) const;
  
private:
  gfl2::fs::BinLinkerAccessor m_bin_linker;
  
  void * m_bin;
    
  u16 *m_next_form_table;
  size_t m_next_form_table_size;  // ((0<=monsno<=MONSNO_END,TAMAGO,別フォルム)の(MONSNO_END+1+1+別フォルム数)のデータ)
  
  u16 *m_local_number_table[LOCAL_AREA_NUM];  // 地方図鑑番号の列((0<=monsno<=MONSNO_END)の(MONSNO_END+1)個のデータ)
  size_t m_local_number_table_size[LOCAL_AREA_NUM];  // LocalArea local_area;とするとlocal_areaのテーブルはm_local_number_table[local_area-1]となる。
  u32 m_local_number_max[LOCAL_AREA_NUM];  // 地方図鑑番号の最大値。例：1番から120番までいるときは120を入れておく。

  u8 *m_style_table;              // 形番号の列((0<=monsno<=MONSNO_END,TAMAGO,別フォルム)の(MONSNO_END+1+1+別フォルム数)のデータ)
  size_t  m_style_table_size;
};


GFL_NAMESPACE_END(PokeTool)

#endif		// __POKETOOL_EXTENDDATA_H_INCLUDED__

