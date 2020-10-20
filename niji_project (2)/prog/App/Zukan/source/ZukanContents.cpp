//==============================================================================
/**
 @file    ZukanContents.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.31
 @brief   図鑑のコンテンツ
 */
//==============================================================================


// pmlibのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>
#include <Savedata/include/EventWork.h>

#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeToolExtendData.h>
#include <PokeTool/include/PokeToolPersonalSort.h>

#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanFormOrder.h"
#include "ZukanContents.h"

#include "ZukanDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// PokeData
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
u32 ZukanContents::PokeData::GetFigureDataIndex(pml::FormNo formNo, pml::Sex sex, bool rareFlag) const
{
  bool isMatch = false;  // 完全一致
  u32  figureDataIndex = 0;

  bool isFormRareMatch = false;  // フォルム番号と色違いが一致、性別は不一致。
  u32  figureDataIndexFormRare = 0;

  bool isFormMatch = false;  // フォルム番号が一致。色違いと性別は不一致。
  u32  figureDataIndexForm = 0;

  // 引数修正
  if(formNo<0 || formNum<=formNo)
  {
    if( monsNo != MONSNO_KOHUKIMUSI && monsNo != MONSNO_KOHUURAI )  // @fix NMCat[416] コフキムシをタマゴ孵化するとはまる
    {                                                               // コフキムシ、コフーライはフォルム数以上のフォルム番号が来るので、ASSERTに掛からないようにする。
      //GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。フォルム番号が異常。フォルム番号0にすることでケア。
                        // ケアしてあるので、フォルム番号が異常なことを許すことにする。ASSERTをコメントアウトした。
    }
    formNo = 0;
  }
  u32 l_rareFlag = rareFlag?1:0;

  // 探す
  for(u32 i=0; i<figureDataNum; ++i)
  {
    const FigureData* l_figureData = &(figureData[i]);
   
    // フォルム番号が一致
    if(formNo == l_figureData->formNo)
    {
      // 最初に一致したフォルム番号のインデックスを覚えておく
      if(isFormMatch == false)
      {
        isFormMatch = true;
        figureDataIndexForm = i;
      }

      // 色違い
      bool l_isFormRareMatch = false;
      if(l_figureData->rareFlag == l_rareFlag)
      {
        l_isFormRareMatch = true;

        // 最初に一致した「フォルム番号と色違い」のインデックスを覚えておく
        if(isFormRareMatch == false)
        {
          isFormRareMatch = true;
          figureDataIndexFormRare = i;
        }
      }

      // 性別
      bool isGenderMatch = false;
      switch(l_figureData->figureGender)
      {
      case FIGURE_GENDER_MALE:
        {
          if(sex == pml::SEX_MALE) isGenderMatch = true;
          break;
        }
      case FIGURE_GENDER_FEMALE:
        {
          if(sex == pml::SEX_FEMALE) isGenderMatch = true;
          break;
        }
      case FIGURE_GENDER_UNKNOWN:
        {
          if(sex == pml::SEX_UNKNOWN) isGenderMatch = true;
          break;
        }
      case FIGURE_GENDER_MALE_FEMALE:
        {
          if(sex == pml::SEX_MALE || sex == pml::SEX_FEMALE) isGenderMatch = true;
          break;
        }
      }

      // 完全一致か？
      if(isGenderMatch && l_isFormRareMatch)
      {
        isMatch = true;
        figureDataIndex = i;
        break;
      }
    }
  }

  // 結果
  if(!isMatch)
  {
    // 完全一致がないときはフォルム番号と色違いが一致したときのものを使う。
    if(isFormRareMatch)
    {
      figureDataIndex = figureDataIndexFormRare;
    }
    else
    {
      // それすらないときはフォルム番号が一致したときのものを使う。
      if(isFormMatch)
      {
        figureDataIndex = figureDataIndexForm;
      }
      else
      {
        // それすらないときは初期化に使った0のまま。
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。一致するものがなく、近いものもない。
      }
    }
  }

  return figureDataIndex;
}
  
u32 ZukanContents::PokeData::GetGetPokemonLang(u32* getPokemonLangBitFlag, ZukanType::ZukanLang gameLang) const
{
  u32 langNum     = 0;
  u32 langBitFlag = 0;

  if(getFlag != 0)
  {
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
 
    static const u8 POKEMON_LANG_LIST[] =  // git_program/niji_project/prog/System/include/PokemonVersion.h  POKEMON_LANG_JAPANなど
    {
      POKEMON_LANG_NULL      ,
      POKEMON_LANG_JAPAN     ,
      POKEMON_LANG_ENGLISH   ,
      POKEMON_LANG_FRANCE    ,
      POKEMON_LANG_ITALY     ,
      POKEMON_LANG_GERMANY   ,
      POKEMON_LANG_SPAIN     ,
      POKEMON_LANG_KOREA     ,
      POKEMON_LANG_CHINA     ,
      POKEMON_LANG_TAIWAN    ,
    };
 
    for(s32 i=0; i<GFL_NELEMS(POKEMON_LANG_LIST); ++i)
    {
      if( svZukanData->GetTextVersionUpFlag(static_cast<MonsNo>(this->monsNo), POKEMON_LANG_LIST[i]) )
      {
        ++langNum;
        langBitFlag |= 1 << POKEMON_LANG_LIST[i];
      }
    }
  }

  // ゲーム開始時に選択されている言語のビットフラグは立てておく
      // 見ただけで捕まえていないときは、言語ビットフラグが1つも立っていないが、この処理で1つは立つようになる。
  if( !( langBitFlag & (1 << ZukanType::ConvertZukanLang2PokemonLang(gameLang)) ) )
  {
    ++langNum;
    langBitFlag |= 1 << ZukanType::ConvertZukanLang2PokemonLang(gameLang);  // ゲーム開始時に選択されている言語のビットフラグは立てておく
  }

  if(getPokemonLangBitFlag) *getPokemonLangBitFlag = langBitFlag;
  return langNum;
}

u32 ZukanContents::PokeData::GetPerRand(u32 figureDataIndex) const
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  if(figureDataIndex < this->figureDataNum)
  {
    const FigureData* l_figureData = &(figureData[figureDataIndex]);
    return svZukanData->GetPokeRandomFlag( static_cast<MonsNo>(this->monsNo), static_cast<pml::Sex>(l_figureData->sex), l_figureData->rareFlag, static_cast<pml::FormNo>(l_figureData->formNo) );
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。引数で指定した容姿の番号がオーバーしている。
    return 0;
  }
}

bool ZukanContents::PokeData::DoesQRCodeExist(bool* notExistBecauseOfNoQR) const
{
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  {
    s32 debugQRAllDispFlag = 0;
#if PM_DEBUG
    debugQRAllDispFlag = ZukanDebugWin::GetQRAllDispFlag();
#endif
    if(debugQRAllDispFlag != 0) return true;
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE

  // QRコードがそもそもないポケモンのリスト
  static const MonsNo NO_QR_MONSNO_LIST[] =
  {
    MONSNO_ZIGARUDE    ,  // #718：ジガルデ
    MONSNO_GURIPUSU1   ,  // #772：タイプ：ヌル
    MONSNO_GURIPUSU2   ,  // #773：シルヴァディ
    MONSNO_THIKITORI   ,  // #785：カプ・コケコ
    MONSNO_THIKITYOU   ,  // #786：カプ・テテフ
    MONSNO_THIKIUSI    ,  // #787：カプ・ブルル
    MONSNO_THIKISAKANA ,  // #788：カプ・レヒレ
    MONSNO_BEBII       ,  // #789：ベビー
    MONSNO_MAYU        ,  // #790：マユ
    MONSNO_NIKKOU      ,  // #791：ソルガレオ
    MONSNO_GEKKOU      ,  // #792：ルナアーラ
    MONSNO_BIISUTOr    ,  // #793：ウツロイド
    MONSNO_BIISUTOy    ,  // #794：マッシブーン
    MONSNO_BIISUTOb    ,  // #795：フェローチェ
    MONSNO_BIISUTOg    ,  // #796：デンジュモク
    MONSNO_BIISUTOp    ,  // #797：テッカグヤ
    MONSNO_BIISUTOi    ,  // #798：カミツルギ
    MONSNO_BIISUTOo    ,  // #799：アクジキング
    MONSNO_PURIZUMU    ,  // #800：ネクロズマ
    MONSNO_MABOROSI16  ,  // #801：マギアナ
    MONSNO_MABOROSI17  ,  // #802：マーシャドー
    MONSNO_BIISUTOd1   ,  // #803：ベベノム
    MONSNO_BIISUTOd2   ,  // #804：アーゴヨン
    MONSNO_BIISUTOs    ,  // #805：ツンデツンデ
    MONSNO_BIISUTOh    ,  // #806：ズガドーン
    MONSNO_MABOROSI18  ,  // #807：ゼラオラ
  };

  *notExistBecauseOfNoQR = false;

  for(u32 i=0; i<GFL_NELEMS(NO_QR_MONSNO_LIST); ++i)
  {
    if( NO_QR_MONSNO_LIST[i] == static_cast<MonsNo>(monsNo) )
    {
      *notExistBecauseOfNoQR = true;
      return false;
    }
  }

  return (getFlag != 0);  // 捕まえていればQRコードがある
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// publicコンストラクタ、デストラクタ
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
ZukanContents::ZukanContents(PokeTool::PokeModelSystem* pokeModelSys, const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort)
{
  this->zukanContentsConstructor(pokeModelSys, pokeExtendData, pokePersonalSort);
}
ZukanContents::ZukanContents(const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort)
{
  this->zukanContentsConstructor(NULL, pokeExtendData, pokePersonalSort);
}
ZukanContents::~ZukanContents()
{
  GFL_SAFE_DELETE_ARRAY(m_dispPokeDataIndexList);
  GFL_SAFE_DELETE_ARRAY(m_drawFigureDataIndexList);
  GFL_SAFE_DELETE_ARRAY(m_pokeDataList);
}
void ZukanContents::zukanContentsConstructor(PokeTool::PokeModelSystem* pokeModelSys, const PokeTool::ExtendData* pokeExtendData, PokeTool::PersonalSort* pokePersonalSort)
{
  m_pokeModelSys                     = pokeModelSys;
  m_pokeExtendData                   = pokeExtendData;
  m_pokePersonalSort                 = pokePersonalSort;
  m_gameLang                         = ZukanType::ZUKAN_LANG_JAPAN;
  m_infoType                         = ZukanType::INFO_TYPE_ALOLA;
  m_pokeDataListOrder                = POKE_DATA_LIST_ORDER_NONE;
  m_pokeDataNumMax                   = 0;
  m_pokeDataNum                      = 0;
  m_pokeDataList                     = NULL;
  m_drawFigureDataIndexList          = NULL;
  m_dispPokeDataIndexNum             = 0;
  m_dispPokeDataIndexList            = NULL;
  m_currIndexOfDispPokeDataIndexList = 0;

  ZukanType::ResetSubjectPaneListPosInfo(m_paneListPosInfoOfSearchSubject);

  for(u32 i=0; i<ZukanType::INFO_TYPE_NUM; ++i)
  {
    m_achievementRate[i] = 0;
    m_getNum[i] = 0;
    m_getIsCompleted[i] = false;
    m_seeNum[i] = 0;
    m_seeIsCompleted[i] = false;
  }
  m_realGetNumAlola = 0;
  m_realSeeNumAlola = 0;

  gfl2::std::MemClear(m_saveDrawData, sizeof(SaveDrawData)*MONSNO_END);

  m_gameLang = ZukanType::ConvertPokemonLang2ZukanLang(System::GetLang());

  m_isEgg = false;
  
  m_useLocalPerRand = false;
  m_localPerRand    = 0;
  
  m_createDataSeq = 0;
  m_createDataSeePokeNum = 0;
  m_createDataLastIndexOfSeePoke = 0;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// publicメンバ関数
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void ZukanContents::InitAchievementDataBySaveData(void)
{
  static const PokeTool::ExtendData::LocalArea POKE_EXTEND_DATA_LOCAL_AREA[ZukanType::INFO_TYPE_NUM] =
  {
    PokeTool::ExtendData::LOCAL_AREA_A,
    PokeTool::ExtendData::LOCAL_AREA_B,
    PokeTool::ExtendData::LOCAL_AREA_C,
    PokeTool::ExtendData::LOCAL_AREA_D,
    PokeTool::ExtendData::LOCAL_AREA_E,
  };

  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  for(u32 i=0; i<ZukanType::INFO_TYPE_NUM; ++i)
  {
    m_getNum[i]         = svZukanData->GetLocalGetCompCount(POKE_EXTEND_DATA_LOCAL_AREA[i], m_pokeExtendData);
    m_getIsCompleted[i] = svZukanData->CheckLocalGetComp(POKE_EXTEND_DATA_LOCAL_AREA[i], m_pokeExtendData);
    m_seeNum[i]         = svZukanData->GetLocalSeeCompCount(POKE_EXTEND_DATA_LOCAL_AREA[i], m_pokeExtendData);
    m_seeIsCompleted[i] = svZukanData->CheckLocalSeeComp(POKE_EXTEND_DATA_LOCAL_AREA[i], m_pokeExtendData);
   
    u32 getNumMax = svZukanData->GetLocalCompNum(m_pokeExtendData, POKE_EXTEND_DATA_LOCAL_AREA[i]);
    if(getNumMax > 0)
    {
      m_achievementRate[i] = m_getNum[i]*100 / getNumMax;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
      m_achievementRate[i] = 0;
    }
    // 補正
    if(m_getNum[i] >= getNumMax)
    {
      m_achievementRate[i] = 100;
    }
    else if(m_getNum[i] == 0)
    {
      m_achievementRate[i] = 0;
    }
    else if(m_achievementRate[i] == 100 && m_getNum[i] < getNumMax)
    {
      m_achievementRate[i] = 99;
    }
    else if(m_achievementRate[i] == 0 && m_getNum[i] > 0 )
    {
      m_achievementRate[i] = 1;
    }
  }
  m_realGetNumAlola = svZukanData->GetLocalPokeGetCount(PokeTool::ExtendData::LOCAL_AREA_A, m_pokeExtendData);  // @fix NMCat[1022] 完成していないのに王冠マークがついてしまう
  m_realSeeNumAlola = svZukanData->GetLocalPokeSeeCount(m_pokeExtendData, PokeTool::ExtendData::LOCAL_AREA_A);  //      → コンプリート判定と実際の匹数を分けるために用意したメンバ変数。
}

void ZukanContents::CreateDataForRegister(gfl2::heap::HeapBase* heap, const pml::pokepara::PokemonParam* pp)
{
  m_pokeDataListOrder = POKE_DATA_LIST_ORDER_ONE;
  m_pokeDataNumMax = 1;
  m_pokeDataList = GFL_NEW_ARRAY(heap) PokeData[m_pokeDataNumMax];
  m_drawFigureDataIndexList = GFL_NEW_ARRAY(heap) u8[m_pokeDataNumMax];
  m_dispPokeDataIndexList = GFL_NEW_ARRAY(heap) u16[m_pokeDataNumMax];

  m_pokeDataNum = 0;
  for(u16 i=0; i<m_pokeDataNumMax; ++i)
  {
    MonsNo monsNo = pp->GetMonsNo();
    this->initPokeData(&m_pokeDataList[i], monsNo);
    this->initPokeDataBySaveData(&m_pokeDataList[i], &m_drawFigureDataIndexList[i], pp);
    ++m_pokeDataNum;
  }
  
  m_dispPokeDataIndexNum = 0;
  for(u16 i=0; i<m_pokeDataNumMax; ++i)
  {
    m_dispPokeDataIndexList[i] = i;
    ++m_dispPokeDataIndexNum;
  }
 
  m_currIndexOfDispPokeDataIndexList = 0;

  m_isEgg = (pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG));
  
  m_useLocalPerRand = true;
  m_localPerRand    = pp->GetPersonalRnd();
}

bool ZukanContents::CreateData(gfl2::heap::HeapBase* heap)
{
  // @fix GFNMCat[1210] 図鑑起動時に目立つカクツキがある
  //                    重い処理であるCreateDataを数フレームに分けた
  static const s32 CREATE_DATA_SEQ_START        =   0;
  static const s32 CREATE_DATA_SEQ_DATA_SET_NUM =  24;  // データを24分割する
  static const s32 CREATE_DATA_SEQ_END          =  -1;

  if( m_createDataSeq == CREATE_DATA_SEQ_END )
  {
    return true;
  }

  // BySaveData

  // アローラ図鑑
  if(m_createDataSeq == CREATE_DATA_SEQ_START)  // 最初だけ
  {
    m_pokeDataListOrder = POKE_DATA_LIST_ORDER_ALOLA;
    m_pokeDataNumMax = m_pokeExtendData->GetLocalAreaMonsMax(PokeTool::ExtendData::LOCAL_AREA_A);
    m_pokeDataList = GFL_NEW_ARRAY(heap) PokeData[m_pokeDataNumMax];
    m_drawFigureDataIndexList = GFL_NEW_ARRAY(heap) u8[m_pokeDataNumMax];
    m_dispPokeDataIndexList = GFL_NEW_ARRAY(heap) u16[m_pokeDataNumMax];
 
    m_pokeDataNum = m_pokeDataNumMax;  // アローラ図鑑の全ポケモン数

    m_createDataSeePokeNum = 0;
    m_createDataLastIndexOfSeePoke = 0;  // アローラ図鑑のポケモンで見たもののうち、最もアローラ図鑑番号の大きいもの
  }

  u32 dataNumInSet   = m_pokeDataNum / CREATE_DATA_SEQ_DATA_SET_NUM;
  u16 currStartIndex = m_createDataSeq * dataNumInSet;
  u16 currEndIndex   = (m_createDataSeq+1) * dataNumInSet;
  if(currEndIndex > m_pokeDataNum)
  {
    currEndIndex = m_pokeDataNum;
    m_createDataSeq = CREATE_DATA_SEQ_END;
  }
  else
  {
    ++m_createDataSeq;
  }

  for(u16 i=currStartIndex; i<currEndIndex; ++i)  // アローラ図鑑のポケモン全部。アローラ図鑑番号順。
  {
    u32 alolaNo = i +1;
    MonsNo monsNo = static_cast<MonsNo>(m_pokeExtendData->GetZenkokuNumber(alolaNo, PokeTool::ExtendData::LOCAL_AREA_A));
    GFL_ASSERT(monsNo > 0);  // 開発中に気付かせるためのASSERT
    this->initPokeData(&m_pokeDataList[i], monsNo);
    this->initPokeDataBySaveData(&m_pokeDataList[i], &m_drawFigureDataIndexList[i]);
    if(m_pokeDataList[i].figureSeeNum > 0)
    {
      ++m_createDataSeePokeNum;
      m_createDataLastIndexOfSeePoke = i;  // i = alolaNo -1
    }
  }
  //GFL_ASSERT(m_createDataSeePokeNum > 0);  // 開発中に気付かせるためのASSERT  0匹があり得るらしいので、ASSERTはコメントアウト。

  if(m_createDataSeq == CREATE_DATA_SEQ_END)  // 最後だけ
  {
    m_dispPokeDataIndexNum = 0;
    if(m_createDataSeePokeNum > 0)  // アローラ図鑑はストーリー展開上m_createDataSeePokeNumが1以上からしか始まらない。
    {
      for(u16 i=0; i<m_pokeDataNum; ++i)  // アローラ図鑑のポケモンの見たものまで。アローラ図鑑番号順。
      {
        m_dispPokeDataIndexList[i] = i;  // i = alolaNo -1
        ++m_dispPokeDataIndexNum;
        if(i == m_createDataLastIndexOfSeePoke) break;
      }
    }
    
    m_currIndexOfDispPokeDataIndexList = 0;  // 閲覧中のポケモン番号を設定するのは、後でSetCurrIndexOfDispPokeDataIndexListで行う。
  
    return true;
  }

  return false;
}

void ZukanContents::SetupData(ZukanType::InfoType infoType)
{
  if(m_pokeDataListOrder != POKE_DATA_LIST_ORDER_ALOLA)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    return;
  }

  ZukanType::ResetSubjectPaneListPosInfo(m_paneListPosInfoOfSearchSubject);

  // アローラ図鑑、島１～４図鑑
  static const PokeTool::ExtendData::LocalArea ZUKAN_INFO_TYPE_2_POKE_EXTEND_DATA_LOCAL_AREA[ZukanType::INFO_TYPE_NUM] =
  {
    PokeTool::ExtendData::LOCAL_AREA_A,
    PokeTool::ExtendData::LOCAL_AREA_B,
    PokeTool::ExtendData::LOCAL_AREA_C,
    PokeTool::ExtendData::LOCAL_AREA_D,
    PokeTool::ExtendData::LOCAL_AREA_E,
  };

  // alolaNoのポケモンのデータがm_pokeDataList[alolaNo -1]に入っている状態で、このSetupData関数が呼ばれる。

  u16 pokeDataNum = m_pokeExtendData->GetLocalAreaMonsMax(ZUKAN_INFO_TYPE_2_POKE_EXTEND_DATA_LOCAL_AREA[infoType]);  // 対象図鑑の全ポケモン数
  
  u32 seePokeNum = 0;
  u16 lastIndexOfSeePoke = 0;  // 対象図鑑のポケモンで見たもののうち、最も対象図鑑番号の大きいもの
  for(u16 i=0; i<pokeDataNum; ++i)  // 対象図鑑のポケモン全部を取り敢えずm_dispPokeDataIndexListに入れる。対象図鑑番号順。
  {
    u32 chihouNo = i +1;
    MonsNo monsNo = static_cast<MonsNo>(m_pokeExtendData->GetZenkokuNumber(chihouNo, ZUKAN_INFO_TYPE_2_POKE_EXTEND_DATA_LOCAL_AREA[infoType]));
    GFL_ASSERT(monsNo > 0);  // 開発中に気付かせるためのASSERT
    u32 alolaNo =  m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_A);
    GFL_ASSERT(alolaNo > 0);  // 開発中に気付かせるためのASSERT
    if(m_pokeDataList[alolaNo -1].figureSeeNum > 0)
    {
      ++seePokeNum;
      lastIndexOfSeePoke = i;
    }

    m_dispPokeDataIndexList[i] = alolaNo -1;  // i = chihouNo -1
  }

  m_dispPokeDataIndexNum = 0;
  if(seePokeNum > 0)  // 島図鑑はseePokeNumが0のこともあり得る。
  {
    m_dispPokeDataIndexNum = lastIndexOfSeePoke +1;  // 対象図鑑のポケモンの見たものまでになるようm_dispPokeDataIndexNumを設定する。対象図鑑番号順。
  }
  
  m_currIndexOfDispPokeDataIndexList = 0;  // 閲覧中のポケモン番号を設定するのは、後でSetCurrIndexOfDispPokeDataIndexListで行う。
}

// @fix NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう
//      →本関数内でSavedata::ZukanData::GetDrawDataは使わず、外部から渡された情報を使って容姿を決める。
//        Savedata::ZukanData::GetDrawDataは表示したい姿になっていないこともあるので。
void ZukanContents::SetupPokeData(PokeData* pokeData, u32* figureDataIndex, MonsNo monsNo, pml::FormNo formNo, pml::Sex sex, bool isRare) const  // 引数で渡したものを変更するだけなのでconst
{
  this->initPokeData(pokeData, monsNo);
  u8 l_figureDataIndex = 0;
  this->initPokeDataBySaveData(pokeData, &l_figureDataIndex, formNo, sex, isRare);
  *figureDataIndex = l_figureDataIndex;
}

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL
void ZukanContents::DebugPrintPokeModelAll(void)
{
  static const char FIGURE_GENDER_NAME[] =
  {
    'm',  // male
    'f',  // female
    'u',  // unknown
    's',  // male&female
  };

  static const char RARE_NAME[] =
  {
    'n',  // normal
    'r',  // rare
  };

  for(u32 i=1; i<=MONSNO_END; ++i)
  {
    MonsNo monsNo = static_cast<MonsNo>(i);
    PokeData pokeData;
    this->initPokeData(&pokeData, monsNo);
    // 全国図鑑番号,フォルム番号,性別,レア,アローラ図鑑番号
    for(u32 j=0; j<pokeData.figureDataNum; ++j)
    {
      PokeData::FigureData* figureData = &(pokeData.figureData[j]);
      GFL_PRINT("%03d,%02d,%c,%c,%03d\n", pokeData.monsNo, figureData->formNo, FIGURE_GENDER_NAME[figureData->figureGender], RARE_NAME[figureData->rareFlag], pokeData.alolaNo);
    }
  }
}
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL

void ZukanContents::SetDispPokeDataIndexListMadeBySearch(
    const ZukanType::PaneListPosInfo* subjectPaneListPosInfo,
    const u8*                         drawFigureDataIndexList,
    u16                               dispPokeDataIndexNum,
    const u16*                        dispPokeDataIndexList   
)
{
  for(u32 i=0; i<ZukanType::SUBJECT_NUM; ++i)
  {
    m_paneListPosInfoOfSearchSubject[i] = subjectPaneListPosInfo[i];
  }

  m_dispPokeDataIndexNum = dispPokeDataIndexNum;
  for(u16 i=0; i<dispPokeDataIndexNum; ++i)
  {
    m_drawFigureDataIndexList[dispPokeDataIndexList[i]] = drawFigureDataIndexList[dispPokeDataIndexList[i]];
    m_dispPokeDataIndexList[i]                          = dispPokeDataIndexList[i];
  }
}

bool ZukanContents::IsOpened(ZukanType::InfoType infoType) const
{
  const Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  bool ret = false;
  switch(infoType)
  {
  case ZukanType::INFO_TYPE_ALOLA:   ret = true;  break;
  case ZukanType::INFO_TYPE_ISLAND1: ret = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN);  break;
  case ZukanType::INFO_TYPE_ISLAND2: ret = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN);  break;
  case ZukanType::INFO_TYPE_ISLAND3: ret = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN);  break;
  case ZukanType::INFO_TYPE_ISLAND4: ret = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN);  break;
  }
  return ret;
}

bool ZukanContents::DoesRotomAppear(void) const
{
  const Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  bool ret = svEventWork->CheckEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);
  return ret;
}




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// ポケモンリスト
//// publicメンバ関数
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
u32 ZukanContents::GetPokeDataIndex(MonsNo monsNo) const
{
  if(m_pokeDataListOrder != POKE_DATA_LIST_ORDER_ALOLA)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    return 0;
  }

  u32 index = 0;
  if(m_pokeDataListOrder == POKE_DATA_LIST_ORDER_ALOLA)
  {
    u32 alolaNo = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_A);
    if( 1<=alolaNo )
    {
      index = alolaNo -1;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。monsNoがアローラ地方のポケモンのものではない。
    }
  }
  GFL_ASSERT(0<=index && index<m_pokeDataNumMax);
  return index;

  /*
  u32 index = 0;
  if( 1<=monsNo && monsNo<=MONSNO_END )
  {
    index = monsNo -1;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。monsNoが異常。
  }
  GFL_ASSERT(0<=index && index<m_pokeDataNumMax);
  return index;
  */
}

PokeTool::SimpleParam ZukanContents::GetPokeSimpleParam(u32 index) const
{
  PokeTool::SimpleParam sp;
  const PokeData* pokeData = this->GetPokeData(index);
  u32 drawFigureDataIndex = this->GetDrawFigureDataIndex(index);
  this->getPokeSimpleParam(&sp, pokeData, drawFigureDataIndex);
  return sp;
}

PokeTool::SimpleParam ZukanContents::GetPokeSimpleParamByFigureDataIndex(u32 index, u32 figureDataIndex) const
{
  PokeTool::SimpleParam sp;
  const PokeData* pokeData = this->GetPokeData(index);
  this->getPokeSimpleParam(&sp, pokeData, figureDataIndex);
  return sp;
}

u32 ZukanContents::GetFigureDataIndex(const PokeTool::SimpleParam& sp) const
{
  u32 figureDataIndex = 0;
  u32 index = this->GetPokeDataIndex(sp.monsNo);
  if(index < m_pokeDataNum)
  {
    const PokeData* pokeData = this->GetPokeData(index);
    if(pokeData->monsNo == sp.monsNo)
    {
      figureDataIndex = pokeData->GetFigureDataIndex(sp.formNo, sp.sex, sp.isRare); 
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。monsNoが一致しない。
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。indexが(monsNoが)本インスタンスで確保しているものの範囲外。
  }
  return figureDataIndex;
}

void ZukanContents::SetDrawFigureDataIndexToSaveData(void) const  // 閲覧中の容姿データをセーブデータに設定する  // セーブデータしか変更していないのでconstを付けている
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  u32 pokeDataNum = this->GetPokeDataNum();
  for(u32 i=0; i<pokeDataNum; ++i)
  {
    PokeTool::SimpleParam sp = this->GetPokeSimpleParam(i);
    if( svZukanData->GetPokeSeeFlag( sp.monsNo ) )  // 見たことあるものだけ
    {
      svZukanData->SetDrawData(sp.monsNo, sp.sex, sp.isRare, sp.formNo);
    }
  }
}

u32 ZukanContents::GetDispPokeDataIndexNumLimitedSee(void) const  // 見たか捕まえたポケモンに限定
{
  u32 num = 0;
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    const PokeData* pokeData = this->GetDispPokeData(i);
    if( pokeData->figureSeeNum > 0 )
    {
      ++num;
    }
  }
  return num;
}
u32 ZukanContents::GetDispPokeDataIndexNumLimitedGet(void) const  // 捕まえたポケモンに限定
{
  u32 num = 0;
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    const PokeData* pokeData = this->GetDispPokeData(i);
    if( pokeData->getFlag != 0 )
    {
      ++num;
    }
  }
  return num;
}

u32 ZukanContents::GetIndexOfDispPokeDataIndexForAlolaDefault(u32 alolaNo) const  // アローラ図鑑デフォルト状態で並んでいるときの、アローラ図鑑番号のインデックスを得る。
{
  u32 index = 0;
  if(m_pokeDataListOrder == POKE_DATA_LIST_ORDER_ALOLA)
  {
    if( ZukanType::IsSubjectPaneListPosInfoReset(m_paneListPosInfoOfSearchSubject) )  // m_pokeDataListのインデックスではなく、m_dispPokeDataIndexListのインデックスなので、このチェックも必要。
    {
      if(alolaNo > 0)
      {
        index = alolaNo -1;
        return index;
      }
    }
  }
  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。アローラ図鑑番号からインデックスへの変換ができない並びのときにこの関数を呼んだ。
  return index;
}

void ZukanContents::SetCurrIndexOfDispPokeDataIndexList(u32 currIndex)
{
  if(0 <= currIndex && currIndex < m_dispPokeDataIndexNum)
  {
    m_currIndexOfDispPokeDataIndexList = static_cast<u16>(currIndex);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。範囲外の値を設定しようとしている。
    // このときは何も設定しないでおく。
  }
}
void ZukanContents::IncreaseCurrIndexOfDispPokeDataIndexList(void)
{
  if( m_currIndexOfDispPokeDataIndexList +1 < m_dispPokeDataIndexNum )
  {
    ++m_currIndexOfDispPokeDataIndexList;
  }
  else
  {
    m_currIndexOfDispPokeDataIndexList = 0;
  }
}
void ZukanContents::DecreaseCurrIndexOfDispPokeDataIndexList(void)
{
  if( m_currIndexOfDispPokeDataIndexList > 0 )
  {
    --m_currIndexOfDispPokeDataIndexList;
  }
  else
  {
    if(m_dispPokeDataIndexNum > 0)
    {
      m_currIndexOfDispPokeDataIndexList = m_dispPokeDataIndexNum -1;
    }
    else
    {
      m_currIndexOfDispPokeDataIndexList = 0;  // 1個もないときは0にしておく
    }
  }
}

void ZukanContents::IncreaseCurrIndexOfDispPokeDataIndexListLimitedSee(void)  // 見たか捕まえたポケモンに限定
{
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    m_currIndexOfDispPokeDataIndexList = (m_currIndexOfDispPokeDataIndexList +1) % m_dispPokeDataIndexNum;
    const PokeData* pokeData = this->GetCurrDispPokeData();
    if( pokeData->figureSeeNum > 0 )
    {
      break;
    }
  }
}
void ZukanContents::DecreaseCurrIndexOfDispPokeDataIndexListLimitedSee(void)
{
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    m_currIndexOfDispPokeDataIndexList = (m_dispPokeDataIndexNum+ m_currIndexOfDispPokeDataIndexList -1) % m_dispPokeDataIndexNum;
    const PokeData* pokeData = this->GetCurrDispPokeData();
    if( pokeData->figureSeeNum > 0 )
    {
      break;
    }
  }
}

void ZukanContents::IncreaseCurrIndexOfDispPokeDataIndexListLimitedGet(void)  // 捕まえたポケモンに限定
{
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    m_currIndexOfDispPokeDataIndexList = (m_currIndexOfDispPokeDataIndexList +1) % m_dispPokeDataIndexNum;
    const PokeData* pokeData = this->GetCurrDispPokeData();
    if( pokeData->getFlag != 0 )
    {
      break;
    }
  }
}
void ZukanContents::DecreaseCurrIndexOfDispPokeDataIndexListLimitedGet(void)
{
  for(u16 i=0; i<m_dispPokeDataIndexNum; ++i)
  {
    m_currIndexOfDispPokeDataIndexList = (m_dispPokeDataIndexNum+ m_currIndexOfDispPokeDataIndexList -1) % m_dispPokeDataIndexNum;
    const PokeData* pokeData = this->GetCurrDispPokeData();
    if( pokeData->getFlag != 0 )
    {
      break;
    }
  }
}

bool ZukanContents::IsCurrIndexOfDispPokeDataIndexListHeadLimitedSee(void) const  // 現在表示しているポケモンは先頭か  // 見たか捕まえたポケモンに限定
{
  s32 l_currIndexOfDispPokeDataIndexList = static_cast<s32>(m_currIndexOfDispPokeDataIndexList);
  s32 l_currIndexOfDispPokeDataIndexListHead = -1;

  while(l_currIndexOfDispPokeDataIndexList >= 0)
  {
    const PokeData* pokeData = this->GetDispPokeData(static_cast<u32>(l_currIndexOfDispPokeDataIndexList));
    if( pokeData->figureSeeNum > 0 )
    {
      l_currIndexOfDispPokeDataIndexListHead = l_currIndexOfDispPokeDataIndexList;
    }
    --l_currIndexOfDispPokeDataIndexList;
  }

  if(l_currIndexOfDispPokeDataIndexListHead >= 0)
  {
    if( static_cast<u16>(l_currIndexOfDispPokeDataIndexListHead) == m_currIndexOfDispPokeDataIndexList )
    {
      return true;
    }
  }

  return false;
}
bool ZukanContents::IsCurrIndexOfDispPokeDataIndexListTailLimitedSee(void) const  // 現在表示しているポケモンは末尾か  
{
  s32 l_currIndexOfDispPokeDataIndexList = static_cast<s32>(m_currIndexOfDispPokeDataIndexList);
  s32 l_currIndexOfDispPokeDataIndexListTail = -1;

  while(l_currIndexOfDispPokeDataIndexList < static_cast<s32>(this->GetDispPokeDataIndexNum()))
  {
    const PokeData* pokeData = this->GetDispPokeData(static_cast<u32>(l_currIndexOfDispPokeDataIndexList));
    if( pokeData->figureSeeNum > 0 )
    {
      l_currIndexOfDispPokeDataIndexListTail = l_currIndexOfDispPokeDataIndexList;
    }
    ++l_currIndexOfDispPokeDataIndexList;
  }

  if(l_currIndexOfDispPokeDataIndexListTail >= 0)
  {
    if( static_cast<u16>(l_currIndexOfDispPokeDataIndexListTail) == m_currIndexOfDispPokeDataIndexList )
    {
      return true;
    }
  }

  return false;
}
bool ZukanContents::IsCurrIndexOfDispPokeDataIndexListHeadLimitedGet(void) const  // 捕まえたポケモンに限定
{
  s32 l_currIndexOfDispPokeDataIndexList = static_cast<s32>(m_currIndexOfDispPokeDataIndexList);
  s32 l_currIndexOfDispPokeDataIndexListHead = -1;

  while(l_currIndexOfDispPokeDataIndexList >= 0)
  {
    const PokeData* pokeData = this->GetDispPokeData(static_cast<u32>(l_currIndexOfDispPokeDataIndexList));
    if( pokeData->getFlag != 0 )
    {
      l_currIndexOfDispPokeDataIndexListHead = l_currIndexOfDispPokeDataIndexList;
    }
    --l_currIndexOfDispPokeDataIndexList;
  }

  if(l_currIndexOfDispPokeDataIndexListHead >= 0)
  {
    if( static_cast<u16>(l_currIndexOfDispPokeDataIndexListHead) == m_currIndexOfDispPokeDataIndexList )
    {
      return true;
    }
  }

  return false;
}
bool ZukanContents::IsCurrIndexOfDispPokeDataIndexListTailLimitedGet(void) const
{
  s32 l_currIndexOfDispPokeDataIndexList = static_cast<s32>(m_currIndexOfDispPokeDataIndexList);
  s32 l_currIndexOfDispPokeDataIndexListTail = -1;

  while(l_currIndexOfDispPokeDataIndexList < static_cast<s32>(this->GetDispPokeDataIndexNum()))
  {
    const PokeData* pokeData = this->GetDispPokeData(static_cast<u32>(l_currIndexOfDispPokeDataIndexList));
    if( pokeData->getFlag != 0 )
    {
      l_currIndexOfDispPokeDataIndexListTail = l_currIndexOfDispPokeDataIndexList;
    }
    ++l_currIndexOfDispPokeDataIndexList;
  }

  if(l_currIndexOfDispPokeDataIndexListTail >= 0)
  {
    if( static_cast<u16>(l_currIndexOfDispPokeDataIndexListTail) == m_currIndexOfDispPokeDataIndexList )
    {
      return true;
    }
  }

  return false;
}

PokeTool::SimpleParam ZukanContents::GetDispPokeSimpleParam(u32 indexOfDispPokeDataIndexList) const
{
  PokeTool::SimpleParam sp;
  const PokeData* pokeData = this->GetDispPokeData(indexOfDispPokeDataIndexList);
  u32 drawFigureDataIndex = this->GetDispDrawFigureDataIndex(indexOfDispPokeDataIndexList);
  this->getPokeSimpleParam(&sp, pokeData, drawFigureDataIndex);
  return sp;
}

void ZukanContents::SetDispDrawFigureDataIndexSpecialFormNo(u32 indexOfDispPokeDataIndexList, const ZukanFormOrder::FormOrder& formOrder)  // 表示上i番目に来ているポケモンの「閲覧できる容姿データ」を「特別扱いするフォルム」にする
{
  u8* target = &(m_drawFigureDataIndexList[ m_dispPokeDataIndexList[indexOfDispPokeDataIndexList] ]);  // this->GetDispDrawFigureDataIndex(indexOfDispPokeDataIndexList)と同じ
  s32 i = *target;
  const PokeData* pokeData = this->GetDispPokeData(indexOfDispPokeDataIndexList);
  const PokeData::FigureData* figureData = &(pokeData->figureData[i]);
  
  for(u8 k=0; k<formOrder.specialFormNum; ++k)
  {
    if( formOrder.formNo[k] == figureData->formNo )
    {
      // 既に特別扱いするフォルムになっていたので、何もせずに戻る。
      return;
    }
  }

  for(u32 j=0; j<pokeData->figureDataNum; ++j)
  {
    figureData = &(pokeData->figureData[j]);
    for(u8 k=0; k<formOrder.specialFormNum; ++k)
    {
      if( formOrder.formNo[k] == figureData->formNo )
      {
        if(figureData->seeFlag != 0)
        {
          // 特別扱いするフォルムの容姿データに変更して戻る。
          *target = j;
          return;
        }
      }
    }
  }
}

void ZukanContents::ResetDispPokeDataOfSaveData(void)
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  svZukanData->SetZukanMode(Savedata::ZukanData::MODE_LOCAL_A);
  svZukanData->SetDefaultMons(MONSNO_NULL);
}
void ZukanContents::SetDispPokeDataToSaveData(u32 indexOfDispPokeDataIndexList)  // 表示上i番目に来ているポケモンをセーブデータに設定する
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  ZukanType::InfoType infoType = this->GetInfoType();
  PokeTool::SimpleParam sp = this->GetDispPokeSimpleParam(indexOfDispPokeDataIndexList);

  static const Savedata::ZukanData::Mode mode[] =
  {
    Savedata::ZukanData::MODE_LOCAL_A, ///<地方  アローラ全域エリア
    Savedata::ZukanData::MODE_LOCAL_B, ///<地方  島１エリア
    Savedata::ZukanData::MODE_LOCAL_C, ///<地方  島２エリア
    Savedata::ZukanData::MODE_LOCAL_D, ///<地方  島３エリア
    Savedata::ZukanData::MODE_LOCAL_E, ///<地方  島４エリア
  };

  svZukanData->SetZukanMode(mode[infoType]);
  svZukanData->SetDefaultMons(sp.monsNo);
}
const ZukanContents::PokeData* ZukanContents::GetDispPokeDataFromSaveData(u32* indexOfDispPokeDataIndexList) const  // セーブデータに設定されているポケモンデータを取得する。戻り値がNULLでないとき、表示上i番目に来ているポケモンという情報を引数に入れて返す。
{
  const ZukanContents::PokeData* pokeData = NULL;
  *indexOfDispPokeDataIndexList = 0;
  
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  ZukanType::InfoType infoType = this->GetInfoType();

  static const Savedata::ZukanData::Mode mode[] =
  {
    Savedata::ZukanData::MODE_LOCAL_A, ///<地方  アローラ全域エリア
    Savedata::ZukanData::MODE_LOCAL_B, ///<地方  島１エリア
    Savedata::ZukanData::MODE_LOCAL_C, ///<地方  島２エリア
    Savedata::ZukanData::MODE_LOCAL_D, ///<地方  島３エリア
    Savedata::ZukanData::MODE_LOCAL_E, ///<地方  島４エリア
  };

  Savedata::ZukanData::Mode svMode = svZukanData->GetZukanMode();
  if(svMode == mode[infoType])  // セーブデータの図鑑モードにNULLはないが閲覧中のポケモン番号にMONSNO_NULLがあるので、初めて図鑑を起動したときはpokeData=NULLを返す。
  {
    MonsNo svMonsNo = svZukanData->GetDefaultMons();
    if(svMonsNo != MONSNO_NULL)
    {
      u32 dispIndexNum = this->GetDispPokeDataIndexNum();
      for(u32 dispIndex=0; dispIndex<dispIndexNum; ++dispIndex)
      {
        const PokeData* dispPokeData = this->GetDispPokeData(dispIndex);
        if( static_cast<MonsNo>(dispPokeData->monsNo) == svMonsNo )
        {
          pokeData = dispPokeData;
          *indexOfDispPokeDataIndexList = dispIndex;
          break;
        }
      }
    }
  }

  return pokeData;
}

PokeTool::SimpleParam ZukanContents::GetCurrDispPokeSimpleParam(void) const
{
  PokeTool::SimpleParam sp;
  const PokeData* pokeData = this->GetCurrDispPokeData();
  u32 drawFigureDataIndex = this->GetCurrDispDrawFigureDataIndex();
  this->getPokeSimpleParam(&sp, pokeData, drawFigureDataIndex);
  return sp;
}

void ZukanContents::IncreaseCurrDispDrawFigureDataIndex(bool isRing)  // 現在表示しているポケモンの「閲覧できる容姿データ」のうちで次にくるものにする  // isRingがtrueのときは端までいったら反対側にいく。
{
  // @fix GFNMCat[1300] ポケモン図鑑：検索後、別姿ボタンを押して、戻るとアサート  →  インデックスの指定の仕方が間違っていたのを修正。
  u8* target = &(m_drawFigureDataIndexList[ m_dispPokeDataIndexList[m_currIndexOfDispPokeDataIndexList] ]);  // this->GetCurrDispDrawFigureDataIndex()と同じ  
  s32 i = *target;
  const PokeData* pokeData = this->GetCurrDispPokeData();
  for(u32 j=0; j<pokeData->figureDataNum; ++j)
  {
    if(isRing == false && i == static_cast<s32>(pokeData->figureDataNum) -1)  // ここに来るということはpokeData->figureDataNum>0になっている
    {
      break;
    }
    s32 i2 = (i+j+1)%(pokeData->figureDataNum);
    const PokeData::FigureData* figureData = &(pokeData->figureData[i2]);
    if(figureData->seeFlag != 0)
    {
      i = i2;
      break;
    }
    if(isRing == false && i2 == static_cast<s32>(pokeData->figureDataNum) -1)  // ここに来るということはpokeData->figureDataNum>0になっている
    {
      break;
    }
  }
  *target = i;
}
void ZukanContents::DecreaseCurrDispDrawFigureDataIndex(bool isRing)  // 現在表示しているポケモンの「閲覧できる容姿データ」のうちで前にくるものにする  // isRingがtrueのときは端までいったら反対側にいく。
{
  // @fix GFNMCat[1300] ポケモン図鑑：検索後、別姿ボタンを押して、戻るとアサート  →  インデックスの指定の仕方が間違っていたのを修正。
  u8* target = &(m_drawFigureDataIndexList[ m_dispPokeDataIndexList[m_currIndexOfDispPokeDataIndexList] ]);  // this->GetCurrDispDrawFigureDataIndex()と同じ
  s32 i = *target;
  const PokeData* pokeData = this->GetCurrDispPokeData();
  for(u32 j=0; j<pokeData->figureDataNum; ++j)
  {
    if(isRing == false && i == 0)
    {
      break;
    }
    s32 i2 = (pokeData->figureDataNum+i-j-1)%(pokeData->figureDataNum);
    const PokeData::FigureData* figureData = &(pokeData->figureData[i2]);
    if(figureData->seeFlag != 0)
    {
      i = i2;
      break;
    }
    if(isRing == false && i2 == 0)
    {
      break;
    }
  }
  *target = i;
}

bool ZukanContents::IsCurrDispDrawFigureDataIndexHead(void) const  // 現在表示しているポケモンの「閲覧できる容姿データ」は先頭か
{
  s32 i = this->GetCurrDispDrawFigureDataIndex();
  const PokeData* pokeData = this->GetCurrDispPokeData();
  const PokeData::FigureData* figureData = &(pokeData->figureData[i]);
  return (figureData->seeFlag!=0 && figureData->seeNo==0);
}
bool ZukanContents::IsCurrDispDrawFigureDataIndexTail(void) const  // 現在表示しているポケモンの「閲覧できる容姿データ」は末尾か
{
  s32 i = this->GetCurrDispDrawFigureDataIndex();
  const PokeData* pokeData = this->GetCurrDispPokeData();
  const PokeData::FigureData* figureData = &(pokeData->figureData[i]);
  return (pokeData->figureSeeNum>0 && figureData->seeFlag!=0 && figureData->seeNo==pokeData->figureSeeNum-1);
}

void ZukanContents::SetCurrDispDrawFigureDataIndexSpecialFormNo(const ZukanFormOrder::FormOrder& formOrder)  // 現在表示しているポケモンの「閲覧できる容姿データ」を「特別扱いするフォルム」にする
{
  u8* target = &(m_drawFigureDataIndexList[ m_dispPokeDataIndexList[m_currIndexOfDispPokeDataIndexList] ]);  // this->GetCurrDispDrawFigureDataIndex()と同じ  
  s32 i = *target;
  const PokeData* pokeData = this->GetCurrDispPokeData();
  const PokeData::FigureData* figureData = &(pokeData->figureData[i]);
  
  for(u8 k=0; k<formOrder.specialFormNum; ++k)
  {
    if( formOrder.formNo[k] == figureData->formNo )
    {
      // 既に特別扱いするフォルムになっていたので、何もせずに戻る。
      return;
    }
  }

  for(u32 j=0; j<pokeData->figureDataNum; ++j)
  {
    figureData = &(pokeData->figureData[j]);
    for(u8 k=0; k<formOrder.specialFormNum; ++k)
    {
      if( formOrder.formNo[k] == figureData->formNo )
      {
        if(figureData->seeFlag != 0)
        {
          // 特別扱いするフォルムの容姿データに変更して戻る。
          *target = j;
          return;
        }
      }
    }
  }
}

void ZukanContents::SetCurrDispDrawFigureDataIndexToSaveData(void)  // 現在表示しているポケモンの閲覧中の容姿データをセーブデータに設定する
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  PokeTool::SimpleParam sp = this->GetCurrDispPokeSimpleParam();
  svZukanData->SetDrawData(sp.monsNo, sp.sex, sp.isRare, sp.formNo);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// privateメンバ関数
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// ポケモンリスト
//// privateメンバ関数
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void ZukanContents::initPokeData(PokeData* pokeData, MonsNo monsNo) const  // 引数で渡したものを変更するだけなのでconst
{
  // フォルム数
  pml::personal::LoadPersonalData( monsNo, 0 );

  u32 formNum = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);  // 通常のフォルムしかないフシギダネは1が返ってくる。
                                                                                    // リザードンは3(通常、Xメガ、Yメガ)が返ってくる。
  if(formNum > PokeData::FORM_NUM_MAX)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。フォルム数が想定外の多さ。
    formNum = PokeData::FORM_NUM_MAX;
  }

  pml::personal::LoadMegaEvolveTable( monsNo );
  u32 megaFormNum = pml::personal::GetMegaEvolveRouteNum();  // 通常のフォルムしかないフシギダネは0が返ってくる。
                                                             // リザードンは2(Xメガ、Yメガ)が返ってくる。
  if(megaFormNum > PokeData::FORM_NUM_MAX)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。フォルム数が想定外の多さ。
    megaFormNum = PokeData::FORM_NUM_MAX;
  }

  u32 normalFormNum = formNum - megaFormNum;


  // 番号
  pokeData->monsNo    = monsNo;
  pokeData->alolaNo   = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_A);
  pokeData->island1No = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_B);
  pokeData->island2No = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_C);
  pokeData->island3No = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_D);
  pokeData->island4No = m_pokeExtendData->GetLocalNumber(monsNo, PokeTool::ExtendData::LOCAL_AREA_E);


  // フォルム
  pokeData->formNum = formNum;


  // フォルムの特別な並び順
  ZukanFormOrder::FormOrder formOrder;
  bool formOrderEnable = ZukanFormOrder::GetFormOrder(monsNo, &formOrder);
  if(formOrderEnable)
  {
    if(formNum != formOrder.formNum)
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。フォルムの個数が「パーソナル」と「ZukanFormOrderのテーブル」で一致していない。
      if(formNum > formOrder.formNum) formNum = formOrder.formNum;  // 少ないほうに合わせておく
    }
    pokeData->formOrderType = formOrder.formOrderType;
  }
  else
  {
    pokeData->formOrderType = ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO;
  }


  // 容姿データ
  u32 figureDataIndex = 0;
  for(u32 formCount=0; formCount<formNum; ++formCount)
  {
    // フォルム番号
    u32 formNo = formCount;
    if(formOrderEnable)
    {
      formNo = formOrder.formNo[formCount];
    }

    // 特別なフォルム
    u32 megaFlag = 0;
    u32 genshiFlag = 0;
    if(formNo >= normalFormNum)
    {
      megaFlag = 1;
    }
    if( this->isFormGenshi(monsNo, formNo) )
    {
      genshiFlag = 1;
    }

    // 性別
    pml::personal::LoadPersonalData( monsNo, formNo );
    pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );

    pml::Sex                        sex                   = pml::SEX_MALE;
    PokeData::FigureGender          figureGender          = PokeData::FIGURE_GENDER_MALE;
    PokeData::FigureGenderExistType figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_DIFFERENT;
    bool femaleExists = false;  // メスで別処理が必要か

    if( monsNo == MONSNO_PATTIIRU )
    {
      // パッチールだけ特別処理
      sex                   = pml::SEX_MALE;
      figureGender          = PokeData::FIGURE_GENDER_MALE;
      figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_DIFFERENT;
      femaleExists = true;
    }
    else
    {
      // パッチール以外は通常処理
      switch(sexVec)
      {
      case pml::personal::SEX_VECTOR_ONLY_MALE:
        {
          sex                   = pml::SEX_MALE;
          figureGender          = PokeData::FIGURE_GENDER_MALE;
          figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_ONLY_MALE;
          break;
        }
      case pml::personal::SEX_VECTOR_ONLY_FEMALE:
        {
          sex                   = pml::SEX_FEMALE;
          figureGender          = PokeData::FIGURE_GENDER_FEMALE;
          figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_ONLY_FEMALE;
          break;
        }
      case pml::personal::SEX_VECTOR_UNKNOWN:
        {
          sex                   = pml::SEX_UNKNOWN;
          figureGender          = PokeData::FIGURE_GENDER_UNKNOWN;
          figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_ONLY_UNKNOWN;
          break;
        }
      default:
        {
          if( this->isPokeModelDifferentByGender(monsNo, formNo) )
          {
            sex                   = pml::SEX_MALE;
            figureGender          = PokeData::FIGURE_GENDER_MALE;
            figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_DIFFERENT;
            femaleExists = true;
          }
          else
          {
            sex                   = pml::SEX_MALE;  // figureGenderがFIGURE_GENDER_MALE_FEMALEのときはpml::SEX_MALEを入れておく。
            figureGender          = PokeData::FIGURE_GENDER_MALE_FEMALE;
            figureGenderExistType = PokeData::FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_SAME;
          }
          break;
        }
      }
    }

    // 値を設定する
    for ever
    {
      for(u32 rareFlag=0; rareFlag<2; ++rareFlag)
      {
        if(figureDataIndex < PokeData::FIGURE_DATA_NUM_MAX)
        {
          PokeData::FigureData* figureData = &(pokeData->figureData[figureDataIndex]);
          
          figureData->formNo                = formNo;
          figureData->sex                   = sex;
          figureData->rareFlag              = rareFlag;
          figureData->figureGender          = figureGender;
          figureData->megaFlag              = megaFlag;
          figureData->genshiFlag            = genshiFlag;
          figureData->figureGenderExistType = figureGenderExistType;
          figureData->figureRareExistType   = PokeData::FIGURE_RARE_EXIST_TYPE_NORMAL_RARE_DIFFERENT;

          ++figureDataIndex;
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。容姿データ数が想定外の多さ。
        }
      }

      if(femaleExists)
      {
        sex          = pml::SEX_FEMALE;
        figureGender = PokeData::FIGURE_GENDER_FEMALE;
        femaleExists = false;  // フラグを落としておく
      }
      else
      {
        break;
      }
    }
  }

  pokeData->figureDataNum = figureDataIndex;
}

void ZukanContents::initPokeDataBySaveDataCore(PokeData* pokeData, const Savedata::ZukanData* svZukanData) const  // 引数で渡したものを変更するだけなのでconst
{ 
  // ポケモンごと
  pokeData->getFlag = svZukanData->GetPokeGetFlag(static_cast<MonsNo>(pokeData->monsNo));
 
  // 容姿データ
  u32 figureSeeNo = 0;
  for(u32 figureDataIndex=0; figureDataIndex<pokeData->figureDataNum; ++figureDataIndex)
  {
    PokeData::FigureData* figureData = &(pokeData->figureData[figureDataIndex]);
   
    bool seeFlag = svZukanData->CheckPokeSeeForm(static_cast<MonsNo>(pokeData->monsNo), static_cast<pml::Sex>(figureData->sex), static_cast<bool>(figureData->rareFlag), static_cast<pml::FormNo>(figureData->formNo));
    if(figureData->figureGender == PokeData::FIGURE_GENDER_MALE_FEMALE)
    {
      figureData->maleSeeFlag = seeFlag;
      bool femaleSeeFlag = svZukanData->CheckPokeSeeForm(static_cast<MonsNo>(pokeData->monsNo), pml::SEX_FEMALE, static_cast<bool>(figureData->rareFlag), static_cast<pml::FormNo>(figureData->formNo));  // figureGenderがFIGURE_GENDER_MALE_FEMALEのときはpml::SEX_MALEが入っているので、pml::SEX_FEMALEを試す。
      figureData->femaleSeeFlag = femaleSeeFlag;
      seeFlag = seeFlag | femaleSeeFlag;
    }
    figureData->seeFlag = seeFlag;
    if(seeFlag)
    {
      figureData->seeNo = figureSeeNo;
      ++figureSeeNo;
    }
  }

  pokeData->figureSeeNum = figureSeeNo;

  // 確認
  GFL_ASSERT(
         ( pokeData->figureSeeNum==0 && svZukanData->GetPokeSeeFlag(static_cast<MonsNo>(pokeData->monsNo))==false )
      || ( pokeData->figureSeeNum> 0 && svZukanData->GetPokeSeeFlag(static_cast<MonsNo>(pokeData->monsNo))!=false )
  );  // 開発中に気付かせるためのASSERT。( 見ているフォルムがないのに見たフラグが立っている ) || ( 見ているフォルムがあるのに見たフラグが立っていない )
}

void ZukanContents::initPokeDataBySaveData(PokeData* pokeData, u8* drawFigureDataIndex, const pml::pokepara::PokemonParam* pp) const  // 引数で渡したものを変更するだけなのでconst
{
  const Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  
  this->initPokeDataBySaveDataCore(pokeData, svZukanData);

  // 閲覧中の容姿データのインデックス
  *drawFigureDataIndex = 0;
  if(pokeData->figureSeeNum > 0)
  {
    pml::FormNo formNo   = 0;
    pml::Sex    sex      = pml::SEX_MALE;
    bool        rareFlag = false;
    if(pp)
    {
      formNo   = pp->GetFormNo();
      // @fix NMCat[1339] 巨大フォルムの図鑑登録についての質問
      // 図鑑に登録できるフォルムに変換する
      svZukanData->GetFormNoWhenSetDrawData( static_cast<MonsNo>(pokeData->monsNo), pp->GetFormNo(), &formNo );
      sex      = pp->GetSex();
      rareFlag = pp->IsRare();
    }
    else
    {
      svZukanData->GetDrawData( static_cast<MonsNo>(pokeData->monsNo), &sex, &rareFlag, &formNo );
    }
    *drawFigureDataIndex = pokeData->GetFigureDataIndex(formNo, sex, rareFlag);
  }
}

// @fix NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう
//      →本関数内でSavedata::ZukanData::GetDrawDataは使わず、外部から渡された情報を使って容姿を決める。
//        Savedata::ZukanData::GetDrawDataは表示したい姿になっていないこともあるので。
void ZukanContents::initPokeDataBySaveData(PokeData* pokeData, u8* drawFigureDataIndex, pml::FormNo a_formNo, pml::Sex a_sex, bool a_isRare) const  // 引数で渡したものを変更するだけなのでconst
{
  const Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  
  this->initPokeDataBySaveDataCore(pokeData, svZukanData);

  // 閲覧中の容姿データのインデックス
  *drawFigureDataIndex = 0;
  if(pokeData->figureSeeNum > 0)
  {
    pml::FormNo formNo   = a_formNo;
    pml::Sex    sex      = a_sex;
    bool        rareFlag = a_isRare;
      
    // @fix NMCat[1339] 巨大フォルムの図鑑登録についての質問
    // 図鑑に登録できるフォルムに変換する
    svZukanData->GetFormNoWhenSetDrawData( static_cast<MonsNo>(pokeData->monsNo), a_formNo, &formNo );
    
    *drawFigureDataIndex = pokeData->GetFigureDataIndex(formNo, sex, rareFlag);
  }
}

bool ZukanContents::isPokeModelDifferentByGender(MonsNo monsNo, pml::FormNo formNo) const
{
  s32 maleDataIdx   = m_pokeModelSys->GetDataIdx(monsNo, formNo, pml::SEX_MALE  );
  s32 femaleDataIdx = m_pokeModelSys->GetDataIdx(monsNo, formNo, pml::SEX_FEMALE);
  return (maleDataIdx != femaleDataIdx);
}

bool ZukanContents::isFormGenshi(MonsNo monsNo, pml::FormNo formNo) const
{
  bool ret = false;
  if(monsNo == MONSNO_KAIOOGA && formNo == FORMNO_KAIOOGA_ALPHA)
  {
    ret = true;
  }
  else if(monsNo == MONSNO_GURAADON && formNo == FORMNO_GURAADON_OMEGA)
  {
    ret = true;
  }
  // レックウザはメガシンカ(メガストーン(○○ナイト)なしでメガシンカできる)
  return ret; 
}

void ZukanContents::getPokeSimpleParam(PokeTool::SimpleParam* sp, const PokeData* pokeData, u32 figureDataIndex) const
{
  const PokeData::FigureData* figureData = &(pokeData->figureData[figureDataIndex]);

  *sp = PokeTool::SimpleParam();
  sp->monsNo  = static_cast<MonsNo>(pokeData->monsNo);
  sp->formNo  = static_cast<pml::FormNo>(figureData->formNo);
  sp->isEgg   = m_isEgg;
  sp->sex     = static_cast<pml::Sex>(figureData->sex);
  sp->isRare  = static_cast<bool>(figureData->rareFlag);
  if(m_useLocalPerRand)
  {
    sp->perRand = m_localPerRand;
  }
  else
  {
    sp->perRand = pokeData->GetPerRand(figureDataIndex);
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

