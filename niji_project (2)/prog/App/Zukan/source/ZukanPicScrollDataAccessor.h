#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDATAACCESSOR_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDATAACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPicScrollDataAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の絵巻データのアクセッサー
 @note    zukan_pic_scroll_data_converter.rbが生成したzukan_pic_scroll_data.binの中身にアクセスする
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// pmlibのインクルード
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPicScrollDataAccessor
 @brief     図鑑の絵巻データのアクセッサー
 */
//==============================================================================
class ZukanPicScrollDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPicScrollDataAccessor);

public:
  //! @brief  配置タイプ
  //! @note  zukan_pic_scroll_data_converter.rbと同じ値
  enum PlaceType
  {
    PLACE_TYPE_GRP000_EVO3       =  0,
    PLACE_TYPE_GRP001_EVO2       =  1,
    PLACE_TYPE_GRP002_EVO1       =  2,
    PLACE_TYPE_GRP003_PAIR2      =  3,
    PLACE_TYPE_GRP004_COMP_EBO3  =  4,
    PLACE_TYPE_GRP005_COMP_EBO4  =  5,
    PLACE_TYPE_GRP006_COMP_EBO9  =  6,
    PLACE_TYPE_GRP007_EVO4       =  7,
    PLACE_TYPE_GRP008_LGD2       =  8,
    PLACE_TYPE_GRP009_LGD_PAIR2  =  9,
    PLACE_TYPE_GRP010_LGD1       = 10,
    PLACE_TYPE_GRP010_EVO1       = 11,
    PLACE_TYPE_NUM               = 12,
  };
  //! @brief  配置タイプごとの配置数
  //! @note  zukan_pic_scroll_data_converter.rbと同じ値
  static const u32 PLACE_POS_NUM[PLACE_TYPE_NUM];

  //! @brief 指定なしのときの値
  //! @note  zukan_pic_scroll_data_converter.rbと同じ値
  static const u32 FORM_NO_NOT_SET          = 0x3F;  //!< フォルム番号指定なし
  static const u32 GENDER_PROG_VAL_NOT_SET  =  0x3;  //!< 性別指定なし
  //! @note  性別はzukan_pic_scroll_data_converter.rbとpml::Sexで値を合わせてある
 
  //! @brief  FormDataのビットシフト
  //! @note  zukan_pic_scroll_data_converter.rbと同じ値
  static const u32 FORM_DATA_BIT_FIELD_SHIFT_FORM_NO = 0;
  static const u32 FORM_DATA_BIT_FIELD_SHIFT_GENDER  = 6;
  static const u8  FORM_DATA_BIT_FIELD_MASK_FORM_NO  = 0x3F;  // これをSHIFT_FORM_NOだけシフト>>して使う
  static const u8  FORM_DATA_BIT_FIELD_MASK_GENDER   = 0xC0;  // これをSHIFT_GENDERだけシフト>>して使う

  //! @brief  階級
  //! @note  zukan_pic_scroll_data_converter.rbと同じ値にしたかったが、UBとティキはQUASI_LEGENDと同じ演出なのでUBとティキは設けないことにした。
  //! @note  ZukanType::REGISTER_LEGEND_LEVEL_...はzukan_pic_scroll_data_converter.rbと同じ値にしてある。
  enum GradeVal
  {
    // GRADE_VAL_NUMを使っているところがあるので、値を変更しないようにして下さい。
    GRADE_VAL_NORMAL        = 0,  //!< 普通
    GRADE_VAL_QUASI_LEGEND  = 1,  //!< 準伝説
    GRADE_VAL_LEGEND        = 2,  //!< 伝説

    GRADE_VAL_NUM           = 3,

    GRADE_VAL_UB            = 3,  //!< UB(演出は準伝説)
    GRADE_VAL_TIKI          = 4,  //!< ティキ(演出は準伝説)  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
    GRADE_VAL_MABOROSHI     = 5,  //!< 幻(演出は伝説)  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。
  };

  //! @brief  なしのときの値
  //! @note  このプログラムでしか使わない値
  static const u32 PLACE_GROUP_NO_NONE  = 0xFFFFFFFF;  //!< 配置グループの番号がない
 
  //! @brief  ポケモンのデータ
  struct PokeData
  {
    u32       placeGroupTopAlolaNo;  //!< 配置グループ先頭のアローラ図鑑番号(1始まり)
    PlaceType placeType;             //!< 配置タイプ
    GradeVal  gradeVal;              //!< 階級
    u8       ubFlag      :1;         //!< UBか
    u8       tikiFlag    :1;         //!< ティキか  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
    u8       maboroshiFlag :1;       //!< 幻か  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。
    u8       placePos    :5;         //!< 配置場所  // 0<=placePos<=8なので4ビットあればいい
  };
  //! @brief  座標情報
  struct TransData
  {
    f32 pokeX;
    f32 pokeY;
    f32 ballX;
    f32 ballY;
    f32 nameX;
    f32 nameY;
  };

public:
  ZukanPicScrollDataAccessor(void)
    : m_buf(NULL),
      m_alolaNo_end(0),
      m_startByte(NULL)
  {}
  virtual ~ZukanPicScrollDataAccessor()
  {}

  //! @brief  アクセッサーが必要とするデータを設定する
  //! @param[in]  buf  zukan_pic_scroll_data.binを読み込んだバッファ
  void SetupData(const void* buf);

  //! @brief  ポケモンのデータを取得する
  //! @param[out] pokeData  ここに渡された構造体に値を書き込んで返す
  void GetPokeData(u32 alolaNo, PokeData* pokeData) const;
  //! @brief  ポケモンの指定した容姿の座標情報を取得する
  //! @param[out] transData  ここに渡された構造体に値を書き込んで返す
  void GetPokeTransData(u32 alolaNo, pml::FormNo formNo, pml::Sex sex, TransData* transData) const;

  //! @brief  このデータで扱っているアローラ番号の最大値を取得する  // 1<=alolaNo<=m_alolaNo_end
  u32 GetAlolaNoEndInPicScrollData(void) const { return m_alolaNo_end; }

  //! @brief  配置グループ数を取得する
  //! @note  カウントして求めているので多少時間が掛かる。一度ゲットした値は覚えておくとよい。
  u32 GetPlaceGroupNum(void) const;
  //! @brief  指定したポケモンが所属する配置グループの番号を取得する(0<= <GetPlaceGroupNum())
  //! @note  カウントして求めているので多少時間が掛かる。一度ゲットした値は覚えておくとよい。
  u32 GetPlaceGroupNo(u32 alolaNo) const;  // なしのときはZukanPicScrollDataAccessor::PLACE_GROUP_NO_NONE

private:
  struct PokeDataHeader
  {
    u16 placeGroupTopAlolaNo;
    u8  formNum;
    u8  gradeVal;
    u8  placeType;
    u8  placePos;
    u8  formDataTop;
    u8  padding[1];
  };
  struct TransDataS32
  {
    s32 pokeX;
    s32 pokeY;
    s32 ballX;
    s32 ballY;
    s32 nameX;
    s32 nameY;
  };
  const PokeDataHeader* getPokeDataHeader(u32 alolaNo) const;
  const u8* getPokeFormData(u32 alolaNo) const;
  u32 getPokeFormDataIndex(u32 a_alolaNo, pml::FormNo a_formNo, pml::Sex a_sex) const;
  const TransDataS32* getPokeTransDataS32(u32 alolaNo) const;

private:
  // 外部から受け取るポインタ
  const void* m_buf;

  // アクセスしやすくするため
  u32 m_alolaNo_end;       // m_buf内のアローラ図鑑番号の最大値  // 1<=alolaNo<=m_alolaNo_end
  const u32* m_startByte;  // 開始バイト位置 m_startByte[alolaNo]  // alolaNoに0は入れちゃダメ 
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPICSCROLLDATAACCESSOR_H_INCLUDED

