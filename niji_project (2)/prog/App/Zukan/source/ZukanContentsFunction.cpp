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
#include <pml/include/personal/pml_PersonalUtil.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>

#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeToolExtendData.h>
#include <PokeTool/include/PokeToolPersonalSort.h>

#include <AppLib/include/Util/app_util_heap.h>

// 図鑑のインクルード
#include "ZukanContents.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////
//// ポケモンリスト
//// publicメンバ関数
//// 表示用ポケモンデータインデックス作成
////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void ZukanContents::MakeDispPokeDataIndexList(
    const ZukanType::PaneListPosInfo* subjectPaneListPosInfo,
    u8*                               drawFigureDataIndexList,
    u16*                              dispPokeDataIndexNum,
    u16*                              dispPokeDataIndexList   
)
{
  if(m_pokeDataListOrder != POKE_DATA_LIST_ORDER_ALOLA)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    return;
  }


  ////////////////
  // 特殊処理
  ////////////////
  // 検索設定が初期状態のときは未発見の番号も含むので、専用の作成関数に任せる。
  if( ZukanType::IsSubjectPaneListPosInfoReset(subjectPaneListPosInfo) )
  {
    return this->MakeDispPokeDataIndexListDefault(
        drawFigureDataIndexList,
        dispPokeDataIndexNum,
        dispPokeDataIndexList   
    );   
  }


  ////////////////
  // 通常処理
  ////////////////

  // 条件をつくる
  bool haveToGet = false;  // 捕まえていなければ分からないものがあるときtrue

  u32                                   initial = PokeTool::PokemonSort::CONDITION_INITIAL_NONE;
  pml::PokeType                         type1   = POKETYPE_NULL;                         // git_program/poke_lib/pml/include/poketype_def.h
  pml::PokeType                         type2   = POKETYPE_NULL;                        
  pml::personal::Color                  color   = pml::personal::COLOR_MAX;              // git_program/poke_lib/pml/include/personal/pml_PersonalConst.h
  PokeTool::ExtendData::StyleType       style   = PokeTool::ExtendData::STYLE_TYPE_MAX;  // git_program/niji_project/prog/PokeTool/include/PokeToolExtendData.h
  PokeTool::PokemonSort::ContidionZukan get     = PokeTool::PokemonSort::CONDITION_ZUKAN_SEE_OR_CAPTURE;  // 図鑑に表示されるポケモンは見たことのあるものだけなので、CONDITION_ZUKAN_NONEではない。

  ZukanType::OrderType    orderType     = ZukanType::GetOrderTypeFromPos   ( subjectPaneListPosInfo[ZukanType::SUBJECT_ORDER    ].pos );
  ZukanType::FigureType   figureType    = ZukanType::GetFigureTypeFromPos  ( subjectPaneListPosInfo[ZukanType::SUBJECT_FIGURE   ].pos );
  ZukanType::GetType      getType       = ZukanType::GetGetTypeFromPos     ( subjectPaneListPosInfo[ZukanType::SUBJECT_GET      ].pos );
  
  {
    ZukanType::InitialType  initialType   = ZukanType::GetInitialTypeFromPos ( subjectPaneListPosInfo[ZukanType::SUBJECT_INITIAL  ].pos, m_gameLang );
    if(initialType   != ZukanType::INITIAL_TYPE_NONE ) initial = static_cast<u32>(initialType);
    
    ZukanType::PoketypeType poketypeType1 = ZukanType::GetPoketypeTypeFromPos( subjectPaneListPosInfo[ZukanType::SUBJECT_POKETYPE1].pos );
    if(poketypeType1 != ZukanType::POKETYPE_TYPE_NONE) type1   = static_cast<pml::PokeType>(poketypeType1);
    
    ZukanType::PoketypeType poketypeType2 = ZukanType::GetPoketypeTypeFromPos( subjectPaneListPosInfo[ZukanType::SUBJECT_POKETYPE2].pos );
    if(poketypeType2 != ZukanType::POKETYPE_TYPE_NONE) type2   = static_cast<pml::PokeType>(poketypeType2);
    
    ZukanType::ColorType    colorType     = ZukanType::GetColorTypeFromPos   ( subjectPaneListPosInfo[ZukanType::SUBJECT_COLOR    ].pos );
    if(colorType     != ZukanType::COLOR_TYPE_NONE   ) color   = static_cast<pml::personal::Color>(colorType);
    
    ZukanType::StyleType    styleType     = ZukanType::GetStyleTypeFromPos   ( subjectPaneListPosInfo[ZukanType::SUBJECT_STYLE    ].pos );
    if(styleType     != ZukanType::STYLE_TYPE_NONE   ) style   = static_cast<PokeTool::ExtendData::StyleType>(styleType);
    
    if(getType       != ZukanType::GET_TYPE_NONE     )
    {
      switch(getType)
      {
      case ZukanType::GET_TYPE_GET: get = PokeTool::PokemonSort::CONDITION_ZUKAN_CAPTURE; break;
      case ZukanType::GET_TYPE_SEE: get = PokeTool::PokemonSort::CONDITION_ZUKAN_SEE;     break;
      }
    }

    // 条件を修正する
    // 捕まえていなければ分からないものがあるときは、捕まえていることを条件に追加する。
    if(
           orderType == ZukanType::ORDER_TYPE_HEAVY
        || orderType == ZukanType::ORDER_TYPE_LIGHT
        || orderType == ZukanType::ORDER_TYPE_TALL
        || orderType == ZukanType::ORDER_TYPE_SHORT
        //|| poketypeType1 != ZukanType::POKETYPE_TYPE_NONE  // nijiではタイプアイコンを伏せることはなくなったのでコメントアウト。
        //|| poketypeType2 != ZukanType::POKETYPE_TYPE_NONE  // nijiではタイプアイコンを伏せることはなくなったのでコメントアウト。
    )
    {
      get = PokeTool::PokemonSort::CONDITION_ZUKAN_CAPTURE;
      haveToGet = true;
    }
  }


  // 図鑑セーブデータを一時的に変更する
  {
    StoreSaveDrawData(m_saveDrawData);

    this->changeSaveDrawDataToFit(haveToGet, type1, type2, color, style, figureType);  // @caution haveToGetの使い方がMakeDispPokeDataIndexList内とchangeSaveDrawDataToFit内で違うので注意。
                                                                                       // MakeDispPokeDataIndexList内 = 捕まえていなければ分からないものがあるときtrue; 捕まえていなければ分からないものは特にないときfalse;
                                                                                       // changeSaveDrawDataToFit内 = trueのとき捕まえていなければならない; falseのとき見ていなければならない(捕まえていてもいい);
   
    // 図鑑セーブデータを変更したので、m_pokePersonalSortに記憶されているものを更新する。
    m_pokePersonalSort->ReloadFullData();
        // m_pokePersonalSortを生成した際のLoadTypeがLOAD_TYPE_FULLの場合、
        // m_pokePersonalSortが記憶しているものは以下の通り。
        //   ・図鑑セーブデータが記憶しているmonsNoごとの表示formNoをm_pokePersonalSortは記憶している。
        //   ・(monsNo,表示formNo)のときのpml::personal::PersonalDataをm_pokePersonalSortは記憶している。
        //
        // m_pokePersonalSortを生成した際のLoadTypeがLOAD_TYPE_PARTの場合、
        // m_pokePersonalSortは図鑑セーブデータ関連のものを何も記憶しておらず、
        // monsNoごとの情報を得るときに毎回図鑑セーブデータを見に行くので、
        // ReloadFullDataは必要ない。
  }


  // PokeTool::PersonalSort
  {   
    // 古い条件をクリア
    m_pokePersonalSort->ClearAllCondition();

    // 絞り込み
    m_pokePersonalSort->SetConditionLocalArea( PokeTool::ExtendData::LOCAL_AREA_A );  // アローラ地方
    //m_pokePersonalSort->SetConditionLocalArea( PokeTool::ExtendData::LOCAL_AREA_MAX );  // 全国

    m_pokePersonalSort->SetConditionInitial( initial );
    m_pokePersonalSort->SetConditionPokeType( type1, type2 );
    m_pokePersonalSort->SetConditionColor( color );
    m_pokePersonalSort->SetConditionStyle( style );
    m_pokePersonalSort->SetConditionZukan( get );
    
    // 並び替え
    switch( orderType )
    {
    case ZukanType::ORDER_TYPE_NAME:
      {
        m_pokePersonalSort->OrderByMonsName();
        break;
      }
    case ZukanType::ORDER_TYPE_HEAVY:
      {
        m_pokePersonalSort->OrderByHeavy();
        break;
      }
    case ZukanType::ORDER_TYPE_LIGHT:
      {
        m_pokePersonalSort->OrderByLight();
        break;
      }
    case ZukanType::ORDER_TYPE_TALL:
      {
        m_pokePersonalSort->OrderByHigh();
        break;
      }
    case ZukanType::ORDER_TYPE_SHORT:
      {
        m_pokePersonalSort->OrderByLow();
        break;
      }
    case ZukanType::ORDER_TYPE_NO:
    default:
      {
        m_pokePersonalSort->OrderByLocalNumber( PokeTool::ExtendData::LOCAL_AREA_A );  // アローラ地方  // 地方の場合はこちら
        //m_pokePersonalSort->OrderByZenkokuNumber();  // 全国  // 全国の場合はこちら
        break;
      }
    }
  }


  // リストの移し替え
  {
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    u32 sortedDataNum = m_pokePersonalSort->GetSortedDataMax();
  
    u32 matchDataNum = 0;  // sortedDataNumのうち実際にリストに移し替えられるデータの数
    for(u32 i=0; i<sortedDataNum; ++i)
    {
      PokeTool::SimpleParam sp;
      sp.monsNo = m_pokePersonalSort->GetSortedMonsNo(i);
      svZukanData->GetDrawData( sp.monsNo, &sp.sex, &sp.isRare, &sp.formNo );

      if( this->isGetTypeMatch(sp, getType) && this->isFigureTypeMatch(sp, figureType) )
      {
        u32 index = this->GetPokeDataIndex(sp.monsNo);
        drawFigureDataIndexList[index] = this->GetFigureDataIndex(sp);
        dispPokeDataIndexList[matchDataNum] = index;
        ++matchDataNum;
      }
    }
    *dispPokeDataIndexNum = matchDataNum;
        // 検索設定が初期状態のときは未発見の番号も含むので気を付けなければならないが、それは本関数前半で専用の関数に任せているのでここには到達しない。


    // 確認
    GFL_PRINT("sorted--------\n");
    for(u32 i=0; i<sortedDataNum; ++i)
    {
      PokeTool::SimpleParam sp;
      sp.monsNo = m_pokePersonalSort->GetSortedMonsNo(i);
      svZukanData->GetDrawData( sp.monsNo, &sp.sex, &sp.isRare, &sp.formNo );
      GFL_PRINT("%4d: monsNo=%d formNo=%d sex=%d rare=%d\n", i, sp.monsNo, sp.formNo, sp.sex, sp.isRare);
    }
  }


  // 図鑑セーブデータを一時的に変更していたのを戻す
  {
    RestoreSaveDrawData(m_saveDrawData);
  }
}


void ZukanContents::MakeDispPokeDataIndexListDefault(
    u8*                               drawFigureDataIndexList,
    u16*                              dispPokeDataIndexNum,
    u16*                              dispPokeDataIndexList   
) const
{
  if(m_pokeDataListOrder != POKE_DATA_LIST_ORDER_ALOLA)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    return;
  }

  // BySaveData
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  // アローラ図鑑  // m_pokeDataListがアローラ図鑑になっているので、このままでは全国図鑑のリストはつくれないので注意すること！

  u32 seePokeNum = 0;
  u16 lastIndexOfSeePoke = 0;  // アローラ図鑑のポケモンで見たもののうち、最もアローラ図鑑番号の大きいもの
  for(u16 index=0; index<this->GetPokeDataNum(); ++index)
  {
    const PokeData* pokeData = this->GetPokeData(index);

    PokeTool::SimpleParam sp;
    sp.monsNo = static_cast<MonsNo>(pokeData->monsNo);
    svZukanData->GetDrawData( sp.monsNo, &sp.sex, &sp.isRare, &sp.formNo );
   
    drawFigureDataIndexList[index] = this->GetFigureDataIndex(sp);
    dispPokeDataIndexList[index]   = index;  // m_pokeDataListはアローラ図鑑番号順になっているからこのまま並べればいい。

    if(pokeData->figureSeeNum > 0)
    {
      ++seePokeNum;
      lastIndexOfSeePoke = index;
    }
  }

  *dispPokeDataIndexNum = 0;
  if(seePokeNum > 0)  // アローラ図鑑はストーリー展開上seePokeNumが1以上からしか始まらない。
  {
    *dispPokeDataIndexNum = lastIndexOfSeePoke +1;  // アローラ図鑑のポケモンの見たものまでになるよう*dispPokeDataIndexNumを設定する。アローラ図鑑番号順。
  }
}


void ZukanContents::changeSaveDrawDataToFit(
    bool                            haveToGet,
    pml::PokeType                   SEARCH_type1,
    pml::PokeType                   SEARCH_type2,
    pml::personal::Color            SEARCH_color,
    PokeTool::ExtendData::StyleType SEARCH_style,
    ZukanType::FigureType           SEARCH_figureType
) const
{
  // セーブデータが保持している閲覧中の容姿データを
  // 引数で渡した( SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, SEARCH_figureType )に適合するように
  // 書き換える。
  // セーブデータはSavedata::ZukanDataのSetDrawDataで書き換える。
  //
  // haveToGetがtrueのとき捕まえていなければならない、falseのとき見ていなければならない。見てもいないポケモンは対象にならない。
  //
  // この関数内でm_saveDrawDataを利用しているので、この関数を呼ぶ前にstoreSaveDataを呼んでおかなければならない。

  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  bool allPokeTypeOk = this->arrangePokeTypeAndIsAllPokeTypeOk(SEARCH_type1, SEARCH_type2);

  // 変数
  PokeTool::SimpleParam sp;
  sp.isEgg   = false;
  //sp.perRand = m_pattiiruPerRand;  // perRandを使うものはないので、デフォルト値のままで構わないから、コメントアウトしておく。

  // 姿で分岐（メガシンカ、色違い、姿は何でもOK）
  for(u32 i=0; i<MONSNO_END; ++i)
  {
    sp.monsNo = static_cast<MonsNo>(i +1);
    
    bool isOk = false;
    if(m_saveDrawData[i].seeFlag != 0)
    {
      if(haveToGet)
      {
        if(svZukanData->GetPokeGetFlag(sp.monsNo))
        {
          isOk = true;
        }
      }
      else
      {
        isOk = true;
      }
    }

    if(isOk)
    {
      sp.formNo = static_cast<pml::FormNo>(m_saveDrawData[i].formNo);
      sp.sex    = static_cast<pml::Sex>(m_saveDrawData[i].sex);
      sp.isRare = (m_saveDrawData[i].rareFlag != 0);
          
      pml::personal::LoadPersonalData( sp.monsNo, sp.formNo );

      // フォルム数
      u32 formNum = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);  // 通常のフォルムしかないフシギダネは1が返ってくる。
                                                                                        // リザードンは3(通常、Xメガ、Yメガ)が返ってくる。
      
      switch(SEARCH_figureType)
      {
      case ZukanType::FIGURE_TYPE_ALOLA:
        {
          //////// アローラ ////////
          
          // 今のセーブデータ記憶値の地方種
          u32 regionID = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_region);

          // アローラ種
          pml::FormNo regionFormArray[MAX_FORM_NUM];  // monsno_def.h
          u32         regionFormNum = 0;
          pml::personal::Util::GetRegionFormNo(regionFormArray, &regionFormNum, sp.monsNo, pml::personal::REGION_ALOLA);  // @caution 内部でpml::personal::LoadPersonalData(...)を呼んでいるので注意！
                                                                                                                          // 次回からは、グローバル領域を使用せず、
                                                                                                                          // m_personal_array  = static_cast<pml::personal::PersonalData**>(GflHeapAllocMemory( heap, sizeof(pml::personal::PersonalData*)*(this->GetOriginalDataMax()) ) );
                                                                                                                          // m_personal_array[ i ]->LoadData( monsno, m_draw_data[ i ].formno );
                                                                                                                          // のように専用領域を用意したほうがいい。

          bool isFit = false;
          // 優先1位  今のセーブデータ記憶値が、条件に一致するアローラ種のときそのまま
          if( regionID == pml::personal::REGION_ALOLA )
          {
            isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, false);
          }
          if(!isFit)
          {
            // 優先2位  レアでない、アローラ種フォルムの若い番号、オス
            // 優先3位  レアでない、アローラ種フォルムの若い番号、メス
            // 優先4位  レアでない、アローラ種フォルムの大きい番号、オス
            // 優先5位  レアでない、アローラ種フォルムの大きい番号、メス
            // 優先6位  レア　　　、アローラ種フォルムの若い番号、オス
            // ・・・
            for(u32 r=0; r<2; ++r)  // レアでない/レア
            {
              for(u32 fi=0; fi<regionFormNum; ++fi)
              {
                for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                {
                  sp.formNo  = regionFormArray[fi];
                  sp.sex     = static_cast<pml::Sex>(s);
                  sp.isRare  = (r != 0);

                  isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                  if(isFit) break;
                }
                if(isFit) break;
              }
              if(isFit) break;
            }
          }
          break;
        }
      case ZukanType::FIGURE_TYPE_MEGA:
        {
          //////// メガシンカ ////////
          
          // フォルム数
          pml::personal::LoadMegaEvolveTable( sp.monsNo );
          u32 megaFormNum = pml::personal::GetMegaEvolveRouteNum();  // 通常のフォルムしかないフシギダネは0が返ってくる。
                                                                     // リザードンは2(Xメガ、Yメガ)が返ってくる。
          u32 normalFormNum = formNum - megaFormNum;
       
          bool isFit = false;
          // 優先1位  今のセーブデータ記憶値が、条件に一致するメガシンカのときそのまま
          if(sp.formNo >= normalFormNum)
          {
            isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, false);
          }
          if(!isFit)
          {
            // 優先2位  レアでない、メガシンカの若い番号、オス
            // 優先3位  レアでない、メガシンカの若い番号、メス
            // 優先4位  レアでない、メガシンカの大きい番号、オス
            // 優先5位  レアでない、メガシンカの大きい番号、メス
            // 優先6位  レア　　　、メガシンカの若い番号、オス
            // ・・・
            for(u32 r=0; r<2; ++r)  // レアでない/レア
            {
              for(u32 f=normalFormNum; f<formNum; ++f)  // メガシンカのフォルム番号
              {
                for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                {
                  sp.formNo  = static_cast<pml::FormNo>(f);
                  sp.sex     = static_cast<pml::Sex>(s);
                  sp.isRare  = (r != 0);

                  isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                  if(isFit) break;
                }
                if(isFit) break;
              }
              if(isFit) break;
            }
          }
          break;
        }
      case ZukanType::FIGURE_TYPE_RARE:
        {
          //////// 色違い ////////
   
          bool isFit = false;
          // 優先1位  今のセーブデータ記憶値が、条件に一致するレアのときそのまま
          if(sp.isRare)
          {
            isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, false);
          }
          if(!isFit)
          {
            ZukanFormOrder::FormOrder formOrder;
            bool formOrderEnable = ZukanFormOrder::GetFormOrder(sp.monsNo, &formOrder);
            if( formOrderEnable && (formOrder.formOrderType != ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO) )  // @fix NMCat[3787] 図鑑で「いろちがい」検索をした際に表示されるフォルムについての質問  →あらゆる特別な並び順に対応させる
            {
              // 優先2位  フォルムオーダーの若い番号、オス
              // 優先3位  フォルムオーダーの若い番号、メス
              // 優先4位  フォルムオーダーの大きい番号、オス
              // ・・・
              for(u32 fo=0; fo<formOrder.formNum; ++fo)  // フォルムオーダー番号
              {
                u32 f = formOrder.formNo[fo];  // フォルム番号
                for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                {
                  sp.formNo  = static_cast<pml::FormNo>(f);
                  sp.sex     = static_cast<pml::Sex>(s);
                  sp.isRare  = true;
             
                  isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                  if(isFit) break;
                }
                if(isFit) break;
              }
            }
            else
            {
              // 優先2位  フォルムの若い番号、オス
              // 優先3位  フォルムの若い番号、メス
              // 優先4位  フォルムの大きい番号、オス
              // ・・・
              for(u32 f=0; f<formNum; ++f)  // フォルム番号
              {
                for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                {
                  sp.formNo  = static_cast<pml::FormNo>(f);
                  sp.sex     = static_cast<pml::Sex>(s);
                  sp.isRare  = true;
             
                  isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                  if(isFit) break;
                }
                if(isFit) break;
              }
            }
          }
          break;
        }
      case ZukanType::FIGURE_TYPE_NONE:
      default:
        {
          //////// 姿は何でもOK ////////

          bool isFit = false;
          // 優先1位  今のセーブデータ記憶値が、条件に一致するときそのまま
          {
            isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, false);
          }
          if(!isFit)
          {
            ZukanFormOrder::FormOrder formOrder;
            bool formOrderEnable = ZukanFormOrder::GetFormOrder(sp.monsNo, &formOrder);
            if( formOrderEnable && (formOrder.formOrderType != ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO) )  // @fix NMCat[3787] 図鑑で「いろちがい」検索をした際に表示されるフォルムについての質問  →あらゆる特別な並び順に対応させる
            {
              // 優先2位  レアでない、フォルムオーダーの若い番号、オス
              // 優先3位  レアでない、フォルムオーダーの若い番号、メス
              // 優先4位  レアでない、フォルムオーダーの大きい番号、オス
              // 優先5位  レアでない、フォルムオーダーの大きい番号、メス
              // 優先6位  レア　　　、フォルムオーダーの若い番号、オス
              // ・・・
              for(u32 r=0; r<2; ++r)  // レアでない/レア
              {
                for(u32 fo=0; fo<formOrder.formNum; ++fo)  // フォルムオーダー番号
                {
                  u32 f = formOrder.formNo[fo];  // フォルム番号
                  for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                  {
                    sp.formNo  = static_cast<pml::FormNo>(f);
                    sp.sex     = static_cast<pml::Sex>(s);
                    sp.isRare  = (r != 0);
             
                    isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                    if(isFit) break;
                  }
                  if(isFit) break;
                }
                if(isFit) break;
              }
            }
            else
            {
              // 優先2位  レアでない、フォルムの若い番号、オス
              // 優先3位  レアでない、フォルムの若い番号、メス
              // 優先4位  レアでない、フォルムの大きい番号、オス
              // 優先5位  レアでない、フォルムの大きい番号、メス
              // 優先6位  レア　　　、フォルムの若い番号、オス
              // ・・・
              for(u32 r=0; r<2; ++r)  // レアでない/レア
              {
                for(u32 f=0; f<formNum; ++f)  // フォルム番号
                {
                  for(u32 s=pml::SEX_MALE; s<=pml::SEX_UNKNOWN; ++s)  // オス/メス/不明
                  {
                    sp.formNo  = static_cast<pml::FormNo>(f);
                    sp.sex     = static_cast<pml::Sex>(s);
                    sp.isRare  = (r != 0);
             
                    isFit = this->setSaveDrawDataIfFit(sp, SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, allPokeTypeOk, true);
                    if(isFit) break;
                  }
                  if(isFit) break;
                }
                if(isFit) break;
              }
            }
          }
          break;
        }
      }  // switch
    }  // isOk
  }  // for
}

bool ZukanContents::setSaveDrawDataIfFit(
    const PokeTool::SimpleParam&    sp,
    pml::PokeType                   SEARCH_type1,
    pml::PokeType                   SEARCH_type2,
    pml::personal::Color            SEARCH_color,
    PokeTool::ExtendData::StyleType SEARCH_style,
    bool                            allPokeTypeOk,
    bool                            isNeedCheckLoadSet
) const
{
  // ポケモンが引数と適合するときtrueを返す。
  //  
  // isNeedCheckLoadSetがtrueのとき、次のことを行う。
  //     ・spで示すポケモンを見ているかチェックする。→見ていないときfalseを返す。
  //     ・(sp.monsNo, sp.formNo)で示すポケモンをpml::personal::LoadPersonalDataでロードする。
  //     ・そのポケモンが
  //       引数で渡した( SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, SEARCH_figureType )に適合するとき
  //       セーブデータが保持している閲覧中の容姿データをSavedata::ZukanDataのSetDrawDataで書き換える。
  //
  // isNeedCheckLoadSetがfalseのとき、
  // spで示す(sp.monsNo, sp.formNo)のポケモンが
  // 引数で渡した( SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, SEARCH_figureType )に適合しているか
  // 判定するだけ。

  bool isFit = false;
  
  if(isNeedCheckLoadSet)
  {
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    if(svZukanData->CheckPokeSeeForm(sp.monsNo, sp.sex, sp.isRare, sp.formNo))
    {
      pml::personal::LoadPersonalData( sp.monsNo, sp.formNo );
      isFit = this->isSaveDrawDataFit(
        sp,
        SEARCH_type1,
        SEARCH_type2,
        SEARCH_color,
        SEARCH_style,
        allPokeTypeOk
      );
      if(isFit)
      {
        svZukanData->SetDrawData(sp.monsNo, sp.sex, sp.isRare, sp.formNo);
      }
    }
  }
  else
  {
    isFit = this->isSaveDrawDataFit(
      sp,
      SEARCH_type1,
      SEARCH_type2,
      SEARCH_color,
      SEARCH_style,
      allPokeTypeOk
    );
  }

  return isFit;
} 

bool ZukanContents::isSaveDrawDataFit(
    const PokeTool::SimpleParam&    sp,
    pml::PokeType                   SEARCH_type1,
    pml::PokeType                   SEARCH_type2,
    pml::personal::Color            SEARCH_color,
    PokeTool::ExtendData::StyleType SEARCH_style,
    bool                            allPokeTypeOk
) const
{
  // spで示す(sp.monsNo, sp.formNo)のポケモンをpml::personal::LoadPersonalDataでロード済みにしておくこと。
  // そのポケモンが
  // 引数で渡した( SEARCH_type1, SEARCH_type2, SEARCH_color, SEARCH_style, SEARCH_figureType )に適合しているか
  // 判定する。適合するときtrueを返す。
  // SEARCH_type1, SEARCH_type2はarrangePokeTypeAndIsAllPokeTypeOkを通し済みであり、
  // その戻り値がallPokeTypeOkであること。

  bool isFit = false;

  pml::PokeType                    POKE_type1 = POKETYPE_NULL;
  pml::PokeType                    POKE_type2 = POKETYPE_NULL;
  pml::personal::Color             POKE_color = pml::personal::COLOR_MAX;
  PokeTool::ExtendData::StyleType  POKE_style = PokeTool::ExtendData::STYLE_TYPE_MAX;

  POKE_type1 = static_cast<pml::PokeType>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type1 ) );
  POKE_type2 = static_cast<pml::PokeType>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type2 ) );
  if( sp.monsNo == MONSNO_ARUSEUSU || sp.monsNo == MONSNO_GURIPUSU2 ) POKE_type1 = POKE_type2 = sp.formNo;  // タイプの番号とアルセウス、シルヴァディのフォルムの番号の並び順は同じ  // @fix GFNMCat[3114] シルヴァディのタイプアイコンが「ノーマル」のまま
  POKE_color = static_cast<pml::personal::Color>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_color ) );
  POKE_style = m_pokeExtendData->GetStyleType( sp );
  
  if(    ( allPokeTypeOk || this->isSamePokeType(POKE_type1, POKE_type2, SEARCH_type1, SEARCH_type2) )
      && ( this->isSameColor(POKE_color, SEARCH_color) )
      && ( this->isSameStyle(POKE_style, SEARCH_style) ) )
  {
    isFit = true;
  }

  return isFit;
} 

bool ZukanContents::arrangePokeTypeAndIsAllPokeTypeOk(pml::PokeType& SEARCH_type0, pml::PokeType& SEARCH_type1) const
{
  // SEARCH_typeは検索で指定したタイプ。POKETYPE_NULLは何でもOKを意味するものとする。だからSEARCH_type0とSEARCH_type1の両方ともPOKETYPE_NULLだったらそのときはtrueを返す。
  // 引数を次のように修正して返す。
  // 0タイプものなら何もせずtrueを返す。
  // 1タイプものなら、SEARCH_type0にPOKETYPE_NULLでない値を入れ、SEARCH_type1にPOKETYPE_NULLを入れ、falseを返す。
  // 2タイプものなら、SEARCH_type0のほうがSEARCH_type1より小さい値になるように値を入れ、falseを返す。

  if(SEARCH_type0==SEARCH_type1) SEARCH_type1 = POKETYPE_NULL;
  if(SEARCH_type0==POKETYPE_NULL){ SEARCH_type0 = SEARCH_type1; SEARCH_type1 = POKETYPE_NULL; }

  if(SEARCH_type0==POKETYPE_NULL) return true;  // SEARCH_type0とSEARCH_type1の両方ともPOKETYPE_NULL

  if(SEARCH_type1!=POKETYPE_NULL)
  {
    if(SEARCH_type0>SEARCH_type1)
    {
      pml::PokeType tmp = SEARCH_type0;
      SEARCH_type0 = SEARCH_type1;
      SEARCH_type1 = tmp;
    }
  }
  return false;
}

bool ZukanContents::isSamePokeType(pml::PokeType POKE_type0, pml::PokeType POKE_type1, pml::PokeType SEARCH_type0, pml::PokeType SEARCH_type1) const
{
  // この関数を呼ぶ前に必ずarrangePokeTypeAndIsAllPokeTypeOkを呼び、
  // SEARCH_type0とSEARCH_type1は異なる値である、
  // SEARCH_type0はPOKETYPE_NULLでない(SEARCH_type1はPOKETYPE_NULLのこともあり得る)、
  // SEARCH_type1もPOKETYPE_NULLでないならSEARCH_type0のほうがSEARCH_type1より小さい値になっている、
  // という状態にしておいて下さい。
  // SEARCH_type0もSEARCH_type1も両方ともPOKETYPE_NULLのときは、この関数を呼ばないで下さい。

  // POKE_typeとSEARCH_typeを比較する
  // POKE_typeはポケモンの持っているタイプ。だからPOKE_type0とPOKE_type1の両方ともがPOKETYPE_NULLはあり得ないのでそのときはfalseを返す。
  // SEARCH_typeは検索で指定したタイプ。POKETYPE_NULLは何でもOKを意味するものとする。

  // 引数修正
  if(POKE_type0==POKE_type1) POKE_type1 = POKETYPE_NULL;
  if(POKE_type0==POKETYPE_NULL){ POKE_type0 = POKE_type1; POKE_type1 = POKETYPE_NULL; }

  // 引数チェック
  if(POKE_type0==POKETYPE_NULL) return false;  // POKE_type0とPOKE_type1の両方ともPOKETYPE_NULL

  // 引数並び替え
  if(POKE_type1!=POKETYPE_NULL)
  {
    if(POKE_type0>POKE_type1)
    {
      pml::PokeType tmp = POKE_type0;
      POKE_type0 = POKE_type1;
      POKE_type1 = tmp;
    }
  }

  // この時点で引数は、POKE_type0には必ず値が入っている、POKE_type1は2タイプものなら値が入っている、2タイプものならPOKE_type0のほうがPOKE_type1より小さい値になっている、という状態になっている。

  if(SEARCH_type1!=POKETYPE_NULL)
  {
    if(POKE_type0==SEARCH_type0 && POKE_type1==SEARCH_type1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    if(POKE_type0==SEARCH_type0 || POKE_type1==SEARCH_type0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool ZukanContents::isSameColor(pml::personal::Color POKE_color, pml::personal::Color SEARCH_color) const
{
  // POKE_colorはポケモンの持っている値。pml::personal::COLOR_MAXのときfalseを返す。
  // SEARCH_colorは検索で指定した値。pml::personal::COLOR_MAXのときtrueを返す。

  if( POKE_color == pml::personal::COLOR_MAX ) return false;
  if( SEARCH_color == pml::personal::COLOR_MAX ) return true;

  if( POKE_color == SEARCH_color ) return true;
  
  return false;
}

bool ZukanContents::isSameStyle(PokeTool::ExtendData::StyleType POKE_style, PokeTool::ExtendData::StyleType SEARCH_style) const
{
  // POKE_styleはポケモンの持っている値。PokeTool::ExtendData::STYLE_TYPE_MAXのときfalseを返す。
  // SEARCH_styleは検索で指定した値。PokeTool::ExtendData::STYLE_TYPE_MAXのときtrueを返す。

  if( POKE_style == PokeTool::ExtendData::STYLE_TYPE_MAX ) return false;
  if( SEARCH_style == PokeTool::ExtendData::STYLE_TYPE_MAX ) return true;
  
  if( POKE_style == SEARCH_style ) return true;
  
  return false;
}


void ZukanContents::StoreSaveDrawData(SaveDrawData* saveDrawData)
{
  // Savedata -> this
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  for(u32 i=0; i<MONSNO_END; ++i)
  {
    MonsNo      monsNo = static_cast<MonsNo>(i +1);
    bool        see    = svZukanData->GetPokeSeeFlag( monsNo );
    saveDrawData[i].seeFlag = see?1:0;
    if(see)  // 見たことあるものだけ
    {
      pml::FormNo formNo = 0;
      pml::Sex    sex    = pml::SEX_MALE;
      bool        rare   = false;
      svZukanData->GetDrawData( monsNo, &sex, &rare, &formNo );
      saveDrawData[i].formNo   = formNo;
      saveDrawData[i].sex      = sex;
      saveDrawData[i].rareFlag = rare?1:0;
    }
  }
}

void ZukanContents::RestoreSaveDrawData(const SaveDrawData* saveDrawData)
{
  // this -> Savedata
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  for(u32 i=0; i<MONSNO_END; ++i)
  {
    if(saveDrawData[i].seeFlag != 0)  // 見たことあるものだけ
    {
      svZukanData->SetDrawData( static_cast<MonsNo>(i +1), static_cast<pml::Sex>(saveDrawData[i].sex), saveDrawData[i].rareFlag!=0, static_cast<pml::FormNo>(saveDrawData[i].formNo) );
    }
  }
}

// @fix NMCat[1795] 図鑑：「捕まえていない」と「並び順」の組み合わせで、捕まえたポケモンが検索される
bool ZukanContents::isGetTypeMatch(const PokeTool::SimpleParam& sp, ZukanType::GetType getType) const
{
  // この関数内でm_saveDrawDataを利用しているので、この関数を呼ぶ前にstoreSaveDataを呼んでおかなければならない。
  
  // spがgetTypeに一致しているときtrueを返す
  // 見てもいないときはfalseを返す
  
  bool ret = false;

  const Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
 
  u32 i = 0;
  if(MONSNO_NULL < sp.monsNo && sp.monsNo <= MONSNO_END)
  {
    i = static_cast<u32>(sp.monsNo) -1;

    //bool see = svZukanData->GetPokeSeeFlag(sp.monsNo);
    bool see = (m_saveDrawData[i].seeFlag != 0);
  
    switch(getType)
    {
    case ZukanType::GET_TYPE_GET:
      {
        bool get = svZukanData->GetPokeGetFlag(sp.monsNo);
        if(get) ret = true;  // 捕まえていれば見てるはずなのでseeは確認しない。
        break;
      }
    case ZukanType::GET_TYPE_SEE:
      {
        bool get = svZukanData->GetPokeGetFlag(sp.monsNo);
        if(see && (!get)) ret = true;
        break;
      }
    default:  // ZukanType::GET_TYPE_NONE
      {
        if(see) ret = true;  // 見ていなければ捕まえていないはずなのでgetは確認しない。
        break;
      }
    }
  }

  return ret;
}

bool ZukanContents::isFigureTypeMatch(const PokeTool::SimpleParam& sp, ZukanType::FigureType figureType) const
{
  // spがSEARCH_figureTypeに一致しているときtrueを返す

  bool ret = false;
      
  switch(figureType)
  {
  case ZukanType::FIGURE_TYPE_ALOLA:
    {
      //////// アローラ ////////
      pml::personal::LoadPersonalData( sp.monsNo, sp.formNo );
      u32 regionID = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_region);
      if( regionID == pml::personal::REGION_ALOLA )
      {
        ret = true;
      }
      break;
    }
  case ZukanType::FIGURE_TYPE_MEGA:
    {
      //////// メガシンカ ////////
      pml::personal::LoadPersonalData( sp.monsNo, sp.formNo );

      u32 formNum = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);  // 通常のフォルムしかないフシギダネは1が返ってくる。
                                                                                        // リザードンは3(通常、Xメガ、Yメガ)が返ってくる。
      pml::personal::LoadMegaEvolveTable( sp.monsNo );
      u32 megaFormNum = pml::personal::GetMegaEvolveRouteNum();  // 通常のフォルムしかないフシギダネは0が返ってくる。
                                                                 // リザードンは2(Xメガ、Yメガ)が返ってくる。
      u32 normalFormNum = formNum - megaFormNum;
 
      if(sp.formNo >= normalFormNum)
      {
        ret = true;
      }
      break;
    }
  case ZukanType::FIGURE_TYPE_RARE:
    {
      //////// 色違い ////////
      if(sp.isRare)
      {
        ret = true;
      }
      break;
    }
  default:
    {
      ret = true;
      break;
    }
  }

  return ret;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

