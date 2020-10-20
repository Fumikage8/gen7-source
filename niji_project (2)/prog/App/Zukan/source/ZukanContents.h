#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANCONTENTS_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANCONTENTS_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanContents.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.31
 @brief   図鑑のコンテンツ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanType.h"
#include "ZukanFormOrder.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
  class ExtendData;
  class PersonalSort;
GFL_NAMESPACE_END(PokeTool)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanContents
 @brief     図鑑のコンテンツ
 */
//==============================================================================
class ZukanContents
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanContents);


public:
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////
  //// ポケモンリスト
  ////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // 図鑑用のポケモンデータ
  class PokeData
  {
    // 見た目が違う際の呼び分けとして、容姿(figure)という言葉を用いることにする。

  public:
    // 容姿に関わる性別
    enum FigureGender
    {
      FIGURE_GENDER_MALE          = 0,  // オス。メスがいないかメスと容姿が違う。
      FIGURE_GENDER_FEMALE        = 1,  // メス。オスがいないかオスと容姿が違う。
      FIGURE_GENDER_UNKNOWN       = 2,  // 性別なし。
      FIGURE_GENDER_MALE_FEMALE   = 3,  // オスメスあるが、容姿の違いなし。
    };

    // 性別容姿存在タイプ
    enum FigureGenderExistType
    {
      FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_DIFFERENT  = 0,  // オスメスあり、オスメス容姿が別のものである
      FIGURE_GENDER_EXIST_TYPE_ONLY_MALE              = 1,  // オス容姿しかない
      FIGURE_GENDER_EXIST_TYPE_ONLY_FEMALE            = 2,  // メス容姿しかない
      FIGURE_GENDER_EXIST_TYPE_ONLY_UNKNOWN           = 3,  // 性別なし容姿しかない
      FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_SAME       = 4,  // オスメスあるが、オスメス容姿は共通のものしかない
    };
    // 色違い容姿存在タイプ
    enum FigureRareExistType
    {
      //FIGURE_RARE_EXIST_TYPE_ONLY_NORMAL            = 0,  // 色違い容姿なし  // 色違いなしはデータとしてはあり得ないのでコメントアウト。それ前提でプログラムは書かれている。ただし、表記上は色違いなしもあり得る。
      FIGURE_RARE_EXIST_TYPE_NORMAL_RARE_DIFFERENT  = 1,  // 色違い容姿あり
    };

    // 最大値
    enum
    {
      FORM_NUM_MAX        =  60,  // formNumがこの上限を越えないかチェックすること
      FIGURE_DATA_NUM_MAX = 120,  // figureDataNumがこの上限を越えないがチェックすること
    };

    // 容姿データ  // 0で初期化する
    struct FigureData
    {
    public:
      ////////////////
      // monsNoが決まれば決まるもの
      ////////////////
      // PokeTool::SimpleParamの情報
      u32 formNo                 :  6;  // 0-63    // pml::FormNo
      u32 sex                    :  2;  // 0-3     // pml::Sex  // figureGenderがFIGURE_GENDER_MALE_FEMALEのときはpml::SEX_MALEを入れておく。
      u32 rareFlag               :  1;  // 0-1     // 色違い
 
      // 容姿に関わる性別
      u32 figureGender           :  2;  // 0-3     // FigureGender

      // 事前に取得しておく
      u32 megaFlag               :  1;  // 0-1     // メガシンカ
      u32 genshiFlag             :  1;  // 0-1     // ゲンシカイキ

      // formNoが同じなら同じ値になるもの
      u32 figureGenderExistType  :  3;  // 0-7     // FigureGenderExistType
      u32 figureRareExistType    :  2;  // 0-3     // FigureRareExistType

      ////////////////
      // ゲームの進行によって変動するもの
      ////////////////
      // 状態
      u32 seeFlag                :  1;  // 0-1     // 見たフラグ
      u32 maleSeeFlag            :  1;  // 0-1     // オスを見たフラグ(figureGenderがFIGURE_GENDER_MALE_FEMALEでないときは無効)
      u32 femaleSeeFlag          :  1;  // 0-1     // メスを見たフラグ(figureGenderがFIGURE_GENDER_MALE_FEMALEでないときは無効)
      u32 seeNo                  :  7;  // 0-127   // 見たものだけに付ける通し番号(seeFlagがfalseのときは無効)(0<= <figureSeeNum)
                                 
      // あまり                  
      u32 dummy                  :  4;
    };

  public:
    ////////////////
    // monsNoが決まれば決まるもの
    ////////////////
    s16 monsNo;     // MonsNo
    s16 alolaNo;    // そのまま表示できる番号。0のときはアローラに出ないポケモン
    s16 island1No;
    s16 island2No;
    s16 island3No;
    s16 island4No;
    u32 formNum       :  6;  // 0-63   // pml::FormNoの数
    u32 figureDataNum :  7;  // 0-127  // 容姿が違うもの数
    u32 formOrderType :  3;  // 0-7    // ZukanFormOrder::FormOrderType フォルムオーダーのタイプ

    ////////////////
    // ゲームの進行によって変動するもの
    ////////////////
    u32 getFlag       :  1;  // 0-1    // 捕まえたフラグ
    u32 figureSeeNum  :  7;  // 0-127  // 容姿が違うものの中で見たものだけの数
    u32 dummy         :  8;
    
    ////////////////
    // 容姿データ
    ////////////////
    FigureData figureData[FIGURE_DATA_NUM_MAX];  // データが入っているのは0<= <figureDataNum

  public:
    PokeData(void)
      : monsNo(0),
        alolaNo(0),
        island1No(0),
        island2No(0),
        island3No(0),
        island4No(0),
        formNum(0),
        figureDataNum(0),
        formOrderType(0),
        getFlag(0),
        figureSeeNum(0)
    {
      gfl2::std::MemClear(figureData, sizeof(FigureData)*FIGURE_DATA_NUM_MAX);
    }

    //! @brief  引数と一致する容姿データのインデックスを返す。一致するものがない場合はできるだけ近いものを返す。
    u32 GetFigureDataIndex(pml::FormNo formNo, pml::Sex sex, bool rareFlag) const;

    //! @brief  このポケモンについて捕まえたことがあるPOKEMON_LANG_???の個数と捕まえたことがあるPOKEMON_LANG_???がどれかを返す。
    //! @param[out]  getPokemonLangBitFlag  捕まえたことがあるPOKEMON_LANG_???がどれかを示す。ビットが立っているところが捕まえた言語。
    //! @param[in]   gameLang    ZUKAN_LANG_を渡す  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。
    //! @return  捕まえたことがあるPOKEMON_LANG_???の個数(getPokemonLangBitFlagの立っているビットの個数)(getFlagが0のとき0)
    u32 GetGetPokemonLang(u32* getPokemonLangBitFlag, ZukanType::ZukanLang gameLang) const;

    //! @brief  個性乱数を取得する
    u32 GetPerRand(u32 figureDataIndex) const;

    //! @brief  QRコードがあるか
    //! param[out] notExistBecauseOfNoQR  戻り値がfalseのときのみ有効。QRコードがない理由が「そもそもないときtrue」か「捕まえていなくてないときfalse」かを返す。
    //! @return  trueある、falseない。
    bool DoesQRCodeExist(bool* notExistBecauseOfNoQR) const;
  };


public:
  ZukanContents(PokeTool::PokeModelSystem* pokeModelSys, const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort);
  ZukanContents(const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort);
  virtual ~ZukanContents();
  void SetPokeModelSys(PokeTool::PokeModelSystem* pokeModelSys) { m_pokeModelSys = pokeModelSys; }
private:
  void zukanContentsConstructor(PokeTool::PokeModelSystem* pokeModelSys, const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort);


public:
  // 現在の状態
  ZukanType::OrderType GetOrderType(void) const { return static_cast<ZukanType::OrderType>(m_paneListPosInfoOfSearchSubject[ZukanType::SUBJECT_ORDER].pos); }
  
  ZukanType::InfoType GetInfoType(void) const                   { return m_infoType; }
  void                SetInfoType(ZukanType::InfoType infoType) { m_infoType = infoType; }

  // 検索情報
  const ZukanType::PaneListPosInfo* GetPaneListPosInfoOfSearchSubject(void) { return m_paneListPosInfoOfSearchSubject; }

  //! @brief  検索設定subjectPaneListPosInfoと
  //!         検索結果drawFigureDataIndexList, dispPokeDataIndexNum, dispPokeDataIndexListを設定する
  //! @param[in]  subjectPaneListPosInfo   subjectPaneListPosInfo[ZukanType::SUBJECT_NUM]
  //! @param[in]  drawFigureDataIndexList  drawFigureDataIndexList[m_pokeDataNumMax]
  //! @param[in]  dispPokeDataIndexNum
  //! @param[in]  dispPokeDataIndexList    dispPokeDataIndexList[m_pokeDataNumMax]
  void SetDispPokeDataIndexListMadeBySearch(
      const ZukanType::PaneListPosInfo* subjectPaneListPosInfo,
      const u8*                         drawFigureDataIndexList,
      u16                               dispPokeDataIndexNum,
      const u16*                        dispPokeDataIndexList   
  );
 
  // 達成率
  u32  GetAchievementRate(ZukanType::InfoType infoType) const { return m_achievementRate[infoType]; }
  u32  GetGetNum(ZukanType::InfoType infoType)          const { return m_getNum[infoType];          } 
  bool IsGetCompleted(ZukanType::InfoType infoType)     const { return m_getIsCompleted[infoType];  } 
  u32  GetSeeNum(ZukanType::InfoType infoType)          const { return m_seeNum[infoType];          } 
  bool IsSeeCompleted(ZukanType::InfoType infoType)     const { return m_seeIsCompleted[infoType];  }
  bool DoesPokeExist(ZukanType::InfoType infoType)      const { return ( m_seeNum[infoType]>0 || m_getNum[infoType]>0 ); }  // 捕まえたものは必ず見ているはずだからm_seeNumだけ確認すればいいと思うが念のため。
                                                                // 「カウント外の幻アローラポケモンだけしか見ていない」なんてことは仕様としてないから、m_realSeeNumAlolaを使ったDoesPokeExistは用意していない。
  u32  GetRealGetNum(ZukanType::InfoType infoType)      const { if(infoType==ZukanType::INFO_TYPE_ALOLA) { return m_realGetNumAlola; } else { return m_getNum[infoType]; } }  // アローラ図鑑にしかカウント外ポケモンはいない
  u32  GetRealSeeNum(ZukanType::InfoType infoType)      const { if(infoType==ZukanType::INFO_TYPE_ALOLA) { return m_realSeeNumAlola; } else { return m_seeNum[infoType]; } }

  // オープン情報
  bool IsOpened(ZukanType::InfoType infoType) const;
  bool DoesRotomAppear(void) const;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////
  //// ポケモンリスト
  ////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // ポケモンデータ
  u32 GetPokeDataNumMax(void) const { return m_pokeDataNumMax; }
  u32 GetPokeDataNum(void) const { return m_pokeDataNum; }
  u32 GetPokeDataIndex(MonsNo monsNo) const;
  const PokeData* GetPokeDataList(void) const { return m_pokeDataList; }
  const PokeData* GetPokeData(u32 index) const { return &(m_pokeDataList[index]); }
  const u8* GetDrawFigureDataIndexList(void) const { return m_drawFigureDataIndexList; }
  u32 GetDrawFigureDataIndex(u32 index) const { return m_drawFigureDataIndexList[index]; }
  PokeTool::SimpleParam GetPokeSimpleParam(u32 index) const;  // figureDataIndexはGetDrawFigureDataIndex関数の戻り値を使う
  PokeTool::SimpleParam GetPokeSimpleParamByFigureDataIndex(u32 index, u32 figureDataIndex) const;
  u32 GetFigureDataIndex(const PokeTool::SimpleParam& sp) const;
  void SetDrawFigureDataIndexToSaveData(void) const;  // 閲覧中の容姿データをセーブデータに設定する  // セーブデータしか変更していないのでconstを付けている
  // 表示用ポケモンデータインデックス 
  u32 GetDispPokeDataIndexNum(void) const { return m_dispPokeDataIndexNum; }
  u32 GetDispPokeDataIndexNumLimitedSee(void) const;  // 見たか捕まえたポケモンに限定
  u32 GetDispPokeDataIndexNumLimitedGet(void) const;  // 捕まえたポケモンに限定
  u32 GetIndexOfDispPokeDataIndexForAlolaDefault(u32 alolaNo) const;  // アローラ図鑑デフォルト状態で並んでいるときの、アローラ図鑑番号のインデックスを得る。
  const u16* GetDispPokeDataIndexList(void) const { return m_dispPokeDataIndexList; }
  // 表示用ポケモンデータインデックスの現在のインデックス
  u32 GetCurrIndexOfDispPokeDataIndexList(void) const { return m_currIndexOfDispPokeDataIndexList; }
  void SetCurrIndexOfDispPokeDataIndexList(u32 currIndex);
  void IncreaseCurrIndexOfDispPokeDataIndexList(void);
  void DecreaseCurrIndexOfDispPokeDataIndexList(void);
  void IncreaseCurrIndexOfDispPokeDataIndexListLimitedSee(void);  // 見たか捕まえたポケモンに限定
  void DecreaseCurrIndexOfDispPokeDataIndexListLimitedSee(void);
  void IncreaseCurrIndexOfDispPokeDataIndexListLimitedGet(void);  // 捕まえたポケモンに限定
  void DecreaseCurrIndexOfDispPokeDataIndexListLimitedGet(void);
  bool IsCurrIndexOfDispPokeDataIndexListHeadLimitedSee(void) const;  // 現在表示しているポケモンは先頭か  // 見たか捕まえたポケモンに限定
  bool IsCurrIndexOfDispPokeDataIndexListTailLimitedSee(void) const;  // 現在表示しているポケモンは末尾か  
  bool IsCurrIndexOfDispPokeDataIndexListHeadLimitedGet(void) const;  // 捕まえたポケモンに限定
  bool IsCurrIndexOfDispPokeDataIndexListTailLimitedGet(void) const;
  // 表示上i番目に来ているポケモンデータ  // indexOfDispPokeDataIndexListは表示用ポケモンデータインデックスのインデックス
  const PokeData* GetDispPokeData(u32 indexOfDispPokeDataIndexList) const { return &(m_pokeDataList[ m_dispPokeDataIndexList[indexOfDispPokeDataIndexList] ]); }
  u32 GetDispDrawFigureDataIndex(u32 indexOfDispPokeDataIndexList) const { return m_drawFigureDataIndexList[ m_dispPokeDataIndexList[indexOfDispPokeDataIndexList] ]; }
  PokeTool::SimpleParam GetDispPokeSimpleParam(u32 indexOfDispPokeDataIndexList) const;
  void SetDispDrawFigureDataIndexSpecialFormNo(u32 indexOfDispPokeDataIndexList, const ZukanFormOrder::FormOrder& formOrder);  // 表示上i番目に来ているポケモンの「閲覧できる容姿データ」を「特別扱いするフォルム」にする
  void ResetDispPokeDataOfSaveData(void);  // セーブデータに設定しているポケモンデータをリセットする
  void SetDispPokeDataToSaveData(u32 indexOfDispPokeDataIndexList);  // 表示上i番目に来ているポケモンをセーブデータに設定する
  const ZukanContents::PokeData* GetDispPokeDataFromSaveData(u32* indexOfDispPokeDataIndexList) const;  // セーブデータに設定されているポケモンデータを取得する。戻り値がNULLでないとき、表示上i番目に来ているポケモンという情報を引数に入れて返す。
  // 現在の表示用ポケモンデータ
  const PokeData* GetCurrDispPokeData(void) const { return this->GetDispPokeData( this->GetCurrIndexOfDispPokeDataIndexList() ); }
  u32 GetCurrDispDrawFigureDataIndex(void) const { return this->GetDispDrawFigureDataIndex( this->GetCurrIndexOfDispPokeDataIndexList() ); }
  PokeTool::SimpleParam GetCurrDispPokeSimpleParam(void) const;
  void IncreaseCurrDispDrawFigureDataIndex(bool isRing=true);  // 現在表示しているポケモンの「閲覧できる容姿データ」のうちで次にくるものにする  // isRingがtrueのときは端までいったら反対側にいく。
  void DecreaseCurrDispDrawFigureDataIndex(bool isRing=true);  // 現在表示しているポケモンの「閲覧できる容姿データ」のうちで前にくるものにする  // isRingがtrueのときは端までいったら反対側にいく。
  bool IsCurrDispDrawFigureDataIndexHead(void) const;  // 現在表示しているポケモンの「閲覧できる容姿データ」は先頭か
  bool IsCurrDispDrawFigureDataIndexTail(void) const;  // 現在表示しているポケモンの「閲覧できる容姿データ」は末尾か
  void SetCurrDispDrawFigureDataIndexSpecialFormNo(const ZukanFormOrder::FormOrder& formOrder);  // 現在表示しているポケモンの「閲覧できる容姿データ」を「特別扱いするフォルム」にする
  void SetCurrDispDrawFigureDataIndexToSaveData(void);  // 現在表示しているポケモンの閲覧中の容姿データをセーブデータに設定する


public:
  //! @brief  検索設定subjectPaneListPosInfoから
  //!         検索結果drawFigureDataIndexList, dispPokeDataIndexNum, dispPokeDataIndexListを得る
  //! @param[in]   subjectPaneListPosInfo   subjectPaneListPosInfo[ZukanType::SUBJECT_NUM]
  //! @param[out]  drawFigureDataIndexList  drawFigureDataIndexList[m_pokeDataNumMax]
  //! @param[out]  dispPokeDataIndexNum
  //! @param[out]  dispPokeDataIndexList    dispPokeDataIndexList[m_pokeDataNumMax]
  void MakeDispPokeDataIndexList(
      const ZukanType::PaneListPosInfo* subjectPaneListPosInfo,
      u8*                               drawFigureDataIndexList,
      u16*                              dispPokeDataIndexNum,
      u16*                              dispPokeDataIndexList   
  );  // 一時的使用のためにメンバm_saveDrawDataを書き換えているのでconstは付かない。セーブデータを一時的に書き換えて元に戻している。

  //! @brief  検索設定が初期状態のとき(ZukanType::IsSubjectPaneListPosInfoResetが真を返すとき)の
  //!         検索結果drawFigureDataIndexList, dispPokeDataIndexNum, dispPokeDataIndexListを得る
  //! @param[out]  drawFigureDataIndexList  drawFigureDataIndexList[m_pokeDataNumMax]
  //! @param[out]  dispPokeDataIndexNum
  //! @param[out]  dispPokeDataIndexList    dispPokeDataIndexList[m_pokeDataNumMax]
  //! @note  未発見の番号も検索結果に含まれる。
  void MakeDispPokeDataIndexListDefault(
      u8*                               drawFigureDataIndexList,
      u16*                              dispPokeDataIndexNum,
      u16*                              dispPokeDataIndexList   
  ) const;


public:
  // 「ゲームの進行によって変動するもの」を設定する
  void InitAchievementDataBySaveData(void);

  void CreateDataForRegister(gfl2::heap::HeapBase* heap, const pml::pokepara::PokemonParam* pp);  // 図鑑登録のデータを生成し、図鑑登録としてセットアップする。
  bool CreateData(gfl2::heap::HeapBase* heap);  // アローラ地方のデータを生成し、アローラ図鑑としてセットアップする。生成が完了したらtrueを返す。
  void SetupData(ZukanType::InfoType infoType);  // CreateDataが済んでいる状態で、infoTypeの図鑑としてセットアップする。

  //! @brief  MonsNoからPokeDataをセットアップする。容姿は引数で渡されたmonsNo, formNo, sex, isRareから決める。
  //! @param[out] pokeData
  //! @param[out] figureDataIndex
  //! @param[in]  monsNo
  //! @param[in]  formNo
  //! @param[in]  sex
  //! @param[in]  isRare
  void SetupPokeData(PokeData* pokeData, u32* figureDataIndex, MonsNo monsNo, pml::FormNo formNo, pml::Sex sex, bool isRare) const;  // 引数で渡したものを変更するだけなのでconst

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL
  //! @brief ポケモンモデルのモデル違い一覧をデバッグ出力する
  void DebugPrintPokeModelAll(void);
#endif


private:
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////
  //// ポケモンリスト
  ////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // 「monsNoが決まれば決まるもの」をpokeDataに設定する
  void initPokeData(PokeData* pokeData, MonsNo monsNo) const;  // 引数で渡したものを変更するだけなのでconst



  // 「ゲームの進行によって変動するもの」をpokeDataに設定する(これを呼び出す時点では「monsNoが決まれば決まるもの」はinitPokeDataを使って設定済みであること)
  // pp版、a_useParam版で共通の処理
  void initPokeDataBySaveDataCore(PokeData* pokeData, const Savedata::ZukanData* svZukanData) const;  // 引数で渡したものを変更するだけなのでconst

  // 「ゲームの進行によって変動するもの」をpokeDataに設定する(これを呼び出す時点では「monsNoが決まれば決まるもの」はinitPokeDataを使って設定済みであること)
  // ppがNULLでないとき、drawFigureDataIndexにはppを指すインデックスを入れて返す。
  // ppがNULLのときは、drawFigureDataIndexにはセーブデータのGetDrawDataで指すインデックスを入れて返す。
  void initPokeDataBySaveData(PokeData* pokeData, u8* drawFigureDataIndex, const pml::pokepara::PokemonParam* pp=NULL) const;  // 引数で渡したものを変更するだけなのでconst

  // 「ゲームの進行によって変動するもの」をpokeDataに設定する(これを呼び出す時点では「monsNoが決まれば決まるもの」はinitPokeDataを使って設定済みであること)
  // drawFigureDataIndexには「a_formNo, a_sex, a_isRare」で指すインデックスを入れて返す。
  void initPokeDataBySaveData(PokeData* pokeData, u8* drawFigureDataIndex, pml::FormNo a_formNo, pml::Sex a_sex, bool a_isRare) const;  // 引数で渡したものを変更するだけなのでconst



  // ポケモンモデルが性別で差があるか？
  // 性別があり、差があるときはtrueを返す。性別があり、差がないときはfalseを返す。
  // 性別がないときはfalseを返す。
  bool isPokeModelDifferentByGender(MonsNo monsNo, pml::FormNo formNo) const;
  // ゲンシカイキのフォルムか？
  bool isFormGenshi(MonsNo monsNo, pml::FormNo formNo) const;

  // ポケモンシンプルパラメータを得る
  void getPokeSimpleParam(PokeTool::SimpleParam* sp, const PokeData* pokeData, u32 figureDataIndex) const;


private:
  // セーブデータが保持している閲覧中の容姿データに対する操作
  void changeSaveDrawDataToFit(
      bool                            haveToGet,
      pml::PokeType                   SEARCH_type1,
      pml::PokeType                   SEARCH_type2,
      pml::personal::Color            SEARCH_color,
      PokeTool::ExtendData::StyleType SEARCH_style,
      ZukanType::FigureType           SEARCH_figureType
  ) const;  // セーブデータを書き換えている(他の関数で元に戻す)。
  bool setSaveDrawDataIfFit(
      const PokeTool::SimpleParam&    sp,
      pml::PokeType                   SEARCH_type1,
      pml::PokeType                   SEARCH_type2,
      pml::personal::Color            SEARCH_color,
      PokeTool::ExtendData::StyleType SEARCH_style,
      bool                            allPokeTypeOk,
      bool                            isNeedCheckLoadSet
  ) const;  // セーブデータを書き換えている(他の関数で元に戻す)。
  bool isSaveDrawDataFit(
      const PokeTool::SimpleParam&    sp,
      pml::PokeType                   SEARCH_type1,
      pml::PokeType                   SEARCH_type2,
      pml::personal::Color            SEARCH_color,
      PokeTool::ExtendData::StyleType SEARCH_style,
      bool                            allPokeTypeOk
  ) const;
  bool arrangePokeTypeAndIsAllPokeTypeOk(pml::PokeType& SEARCH_type0, pml::PokeType& SEARCH_type1) const;
  bool isSamePokeType(pml::PokeType POKE_type0, pml::PokeType POKE_type1, pml::PokeType SEARCH_type0, pml::PokeType SEARCH_type1) const;
  bool isSameColor(pml::personal::Color POKE_color, pml::personal::Color SEARCH_color) const;
  bool isSameStyle(PokeTool::ExtendData::StyleType POKE_style, PokeTool::ExtendData::StyleType SEARCH_style) const;

  bool isGetTypeMatch(const PokeTool::SimpleParam& sp, ZukanType::GetType getType) const;
  bool isFigureTypeMatch(const PokeTool::SimpleParam& sp, ZukanType::FigureType figureType) const;

public:
  // セーブデータが保持している閲覧中の容姿データ
  struct SaveDrawData
  {
    u32 formNo          :  6;  // 0-63    // pml::FormNo
    u32 sex             :  2;  // 0-3     // pml::Sex
    u32 rareFlag        :  1;  // 0-1     // 色違い
    u32 seeFlag         :  1;  // 0-1     // 見たフラグ
    u32 dummy           : 22;
  };

  static void StoreSaveDrawData(SaveDrawData* saveDrawData);          // セーブデータの内容をsaveDrawDataに反映する。saveDrawDataは要素数MONSNO_ENDの配列。
  static void RestoreSaveDrawData(const SaveDrawData* saveDrawData);  // saveDrawDataの内容をセーブデータに反映する。monsNoのポケモンのデータはsaveDrawData[monsNo-1]に入る。

private:
  // m_pokeDataListの並び順
  enum PokeDataListOrder
  {
    POKE_DATA_LIST_ORDER_NONE,     // m_pokeDataListをつくっていない
    POKE_DATA_LIST_ORDER_ZENKOKU,  // 全国図鑑全部  // [0]=フシギダネ; [1]=フシギソウ; ... [MONSNO_END-1]=全国図鑑最後のポケモン;  // 今回は未対応
    POKE_DATA_LIST_ORDER_ALOLA,    // アローラ図鑑だけ  // [0]=アローラ図鑑最初のポケモン; ... [アローラ図鑑最後のポケモンの表記番号-1]=アローラ図鑑最後のポケモン;
    POKE_DATA_LIST_ORDER_ONE,      // 1匹だけ  // 図鑑登録のときなど
  };

private:
  // システム
  PokeTool::PokeModelSystem*     m_pokeModelSys;
  const PokeTool::ExtendData*    m_pokeExtendData;
  PokeTool::PersonalSort*        m_pokePersonalSort;

  // 現在の状態
  ZukanType::ZukanLang        m_gameLang;  // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。
  ZukanType::InfoType         m_infoType;
  ZukanType::PaneListPosInfo  m_paneListPosInfoOfSearchSubject[ZukanType::SUBJECT_NUM];  // 検索設定

  // 達成率
  u8   m_achievementRate[ZukanType::INFO_TYPE_NUM];
  u16  m_getNum[ZukanType::INFO_TYPE_NUM];
  bool m_getIsCompleted[ZukanType::INFO_TYPE_NUM];
  u16  m_seeNum[ZukanType::INFO_TYPE_NUM];
  bool m_seeIsCompleted[ZukanType::INFO_TYPE_NUM];
  u16  m_realGetNumAlola;  // アローラ図鑑にしかカウント外ポケモンはいない
  u16  m_realSeeNumAlola;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////
  //// ポケモンリスト
  ////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // ポケモンデータ
  PokeDataListOrder m_pokeDataListOrder;
  u16        m_pokeDataNumMax;
  u16        m_pokeDataNum;
  PokeData*  m_pokeDataList;  // GFL_NEW_ARRAY(heap) PokeData[m_pokeDataNumMax]だけ確保しておく。
                              // 実際にデータが入っているのは[0]<= <=[m_pokeDataNum-1]まで。
                              // 必ず番号順に並べておく。見ていないポケモンも入れておく。
                              // つまり、例えば全国図鑑番号順のときは、m_pokeDataList[i]には全国図鑑番号(i+1)番のポケモンが入っている。
                              // 図鑑登録のときは1個だけ、アローラ図鑑のときはアローラ図鑑番号順になっている。
  u8*        m_drawFigureDataIndexList;  // GFL_NEW_ARRAY(heap) u8[m_pokeDataNumMax]だけ確保しておく。
                                         // m_pokeDataListと同じ並び順。
                                         // 閲覧中の容姿データのインデックス
                                         // m_pokeDataList[i]のポケモンの閲覧中の容姿データはm_pokeDataList[i].figureData[m_drawFigureDataIndexList[i]]となる。

  // 表示用ポケモンデータインデックス 
  u16        m_dispPokeDataIndexNum;
  u16*       m_dispPokeDataIndexList;  // GFL_NEW_ARRAY(heap) u16[m_pokeDataNumMax]だけ確保しておく。
                                       // 実際にデータが入っているのは[0]<= <=[m_dispPokeDataIndexNum-1]まで。
                                       // 表示上i番目に来ているポケモンデータは m_pokeDataList[ m_dispPokeDataIndexList[i] ] で取得できる。
  // 表示用ポケモンデータインデックスの現在のインデックス
  u16        m_currIndexOfDispPokeDataIndexList;
 
  // セーブデータが保持している閲覧中の容姿データ
  SaveDrawData  m_saveDrawData[MONSNO_END];  // monsNoのポケモンの閲覧中の容姿データはm_saveDrawData[monsNo-1]に入っている。
                                             // Savedata::ZukanDataのGetDrawDataで得られる値を入れておく。
  
  // タマゴ表示フラグ、個性乱数
  bool m_isEgg;  // PokeTool::SimpleParamを生成する際にこの値を使う。
                 // CreateDataForRegisterで渡したpml::pokepara::PokemonParamがタマゴのときだけtrueになる。
  bool m_useLocalPerRand;  // CreateDataForRegisterが呼ばれたときだけtrueになる。
  u32  m_localPerRand;     // m_useLocalPerRandがtrueのときだけPokeTool::SimpleParamを生成する際にこの値を使う。
  
  // CreateDataシーケンス
  s32  m_createDataSeq;
  u32  m_createDataSeePokeNum;
  u16  m_createDataLastIndexOfSeePoke;


  // 拡張用のメモ書き
  // もし「フォルム違いを別扱いで表示するモード」をつくるなら
  // m_dispPokeDataIndexList           = { 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 3, ... }
  // m_dispPokeDataFigureDataIndexList = { 0, 1, 0, 1, 0, 1, 2, 3, 4, 5, 0, ... }  表示用ポケモンデータの容姿データのインデックス
  // とするといいだろう。
  // この例は
  // m_dispPokeDataIndexList           = { フシギダネ, フシギダネ  , フシギソウ, フシギソウ  , フシギバナ, フシギバナ, フシギバナ, フシギバナ, フシギバナ  , フシギバナ      , ヒトカゲ, ... }
  // m_dispPokeDataFigureDataIndexList = { オスメス  , オスメスレア, オスメス  , オスメスレア, オス      , オスレア  , メス      , メスレア  , オスメスメガ, オスメスメガレア, オスメス, ... }
  // を表している。
  // m_dispPokeDataIndexNumがm_pokeDataNumMaxより大きくなるので注意して下さい！
  // m_pokeDataNumMaxをm_pokeDataFigureDataNumMaxと改名すると、「フォルム違いを別扱いで表示しないモード」にも「フォルム違いを別扱いで表示するモード」にも対応している名前になっていいかもしれない。
  // 「フォルム違いを別扱いで表示しないモード」でもm_dispPokeDataFigureDataIndexListを使うことは可能だろう。そのときはm_drawFigureDataIndexListと示しているものが同じになるので、両者に同じ値を入れておかなければならない。
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANCONTENTS_H_INCLUDED

