//======================================================================
/**
 * @file    ResortUtility.cpp
 * @date    2015/10/08 11:24:33
 * @author  fukushima_yuya
 * @brief   便利関数など
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortConst.h"
#include "ResortDrawerBase.h"

#include <AppLib/include/Ui/UIView.h>
#include <Print/include/WordSet.h>
#include <Savedata/include/BoxPokemonSave.h>

// arc
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
GFL_NAMESPACE_BEGIN( util )

//! @brief    リゾートの種類
static const Savedata::ResortSave::PokeResortKind c_ResortKind[] = {
  Savedata::ResortSave::RESORT_KIND_FRIENDSHIP,
  Savedata::ResortSave::RESORT_KIND_FARM,
  Savedata::ResortSave::RESORT_KIND_ADVENTURE,
  Savedata::ResortSave::RESORT_KIND_GIM,
  Savedata::ResortSave::RESORT_KIND_HOTSPA,
};


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンペインデータのセット
 *
 * @param   base      [in]描画共通処理
 * @param   paneData  [out]ペインデータ
 * @param   paneTbl   [in]ペインインデックス配列
 */
//------------------------------------------------------------------------------
void SetPokePaneData( DrawerBase* base, PokePaneData* paneData, const u32* paneTbl )
{
  const u32 end = LYTRES_PANENAME_RESORT_PANE_END;
  u32 id;
  
  if( (id = paneTbl[POKE_PANE_ID_NULL]) < end ) {
    paneData->null = base->GetPane( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_ICON_PARTS]) < end ) {
    paneData->iconParts = base->GetParts( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_ICON_PIC]) < end ) {
    paneData->iconPic = base->GetPicture( id, paneData->iconParts );
  }

  if( (id = paneTbl[POKE_PANE_ID_TEXT_LV]) < end ) {
    paneData->textLv = base->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_TEXT_LV_NUM]) < end ) {
    paneData->textLvNum = base->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_GENDER]) < end ) {
    paneData->gender = base->GetPane( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_NAME]) < end ) {
    paneData->name = base->GetTextBox( id );
  }

  if( (id = paneTbl[POKE_PANE_ID_SEIKAKU]) < end ) {
    paneData->seikaku = base->GetTextBox( id );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画用ポケモンパラメータのセット
 *
 * @param   sp  [out]PokeTool::SimpleParam
 * @param   lv  [out]レベル値
 * @param   cp  [in]pml::pokepara::CoreParam
 *
 * @return  "true" == 設定成功
 * @return  "false" == それ以外
 */
//------------------------------------------------------------------------------
bool SetSimpleParam( PokeTool::SimpleParam* sp, u32* lv, pml::pokepara::CoreParam* cp )
{
  if( cp->IsNull() ) return false;

  PokeTool::GetSimpleParam( sp, cp );
  {
    sp->sex = PokeTool::GetDrawSex( cp );
  }

  if( lv )
  {
    bool fastmode = cp->StartFastMode();
    {
      *lv = cp->GetLevel();
    }
    cp->EndFastMode( fastmode );
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ数の取得
 *
 * @param   resortSave    リゾートセーブデータ
 *
 * @return  ノーマルランクのポケマメに換算した数
 */
//------------------------------------------------------------------------------
u32 GetAllBeansCount( Savedata::ResortSave* resortSave )
{
  u32 count = 0;

  for( u32 i=0; i<Savedata::ResortSave::POKEBEANS_KIND_MAX; ++i )
  {
    u32 num = resortSave->GetPokeBeansNum( static_cast<Savedata::ResortSave::PokeBeansEnum>( i ) );

    if( i == Savedata::ResortSave::POKEBEANS_NIJI )
    {
      count += num;
    }
    else if( i <= Savedata::ResortSave::POKEBEANS_HEART_07 &&  i >= Savedata::ResortSave::POKEBEANS_HEART_01 )
    {
      count += num;
    }
    else {
      count += num;
    }
  }

  return count;
}

//------------------------------------------------------------------------------
/**
 * @brief   ランクごとのポケマメ数の取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   dest        [out]格納先の配列
 */
//------------------------------------------------------------------------------
void GetRankBeansCount( Savedata::ResortSave* resortSave, u32* dest )
{
  for( u8 i=0; i<3; ++i )
  {
    dest[i] = 0;
  }

  for( u32 i=0; i<Savedata::ResortSave::POKEBEANS_KIND_MAX; ++i )
  {
    u32 num = resortSave->GetPokeBeansNum( static_cast<Savedata::ResortSave::PokeBeansEnum>( i ) );

    if( i == Savedata::ResortSave::POKEBEANS_NIJI )
    {
      dest[2] += num;
    }
    else if( i <= Savedata::ResortSave::POKEBEANS_HEART_07 &&  i >= Savedata::ResortSave::POKEBEANS_HEART_01 )
    {
      dest[1] += num;
    }
    else {
      dest[0] += num;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   時間表示
 *
 * @param   view      UIViewクラス
 * @param   wordSet   WordSetクラス
 * @param   textBox   時間を表示するテキストボックス
 * @param   time      時間
 */
//------------------------------------------------------------------------------
void SetTextBoxTime( app::ui::UIView* view, print::WordSet* wordSet, gfl2::lyt::LytTextBox* textBox, TimeParam time )
{
  if( wordSet == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( textBox == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( time.GetTotalSec() == 0 )    //!< 値が０の時
  {
    // [ - ]表示
    view->SetTextboxPaneMessage( textBox, msg_prsys_common_07 );
  }
  else {
    wordSet->RegisterNumber( 0, time.hour, 2, print::NUM_DISP_ZERO, print::NUM_CODE_ZENKAKU );
    wordSet->RegisterNumber( 1, time.min, 2, print::NUM_DISP_ZERO, print::NUM_CODE_ZENKAKU );
    wordSet->RegisterNumber( 2, time.sec, 2, print::NUM_DISP_ZERO, print::NUM_CODE_ZENKAKU );

    view->SetTextboxPaneMessage( textBox, msg_prsys_common_06 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   Ａ／Ｂ表示
 *
 * @param   view          UIViewクラス
 * @param   wordSet       WordSetクラス
 * @param   textBox       時間を表示するテキストボックス
 * @param   numerator     分子(Ａ)
 * @param   denominator   分母(Ｂ)
 */
//------------------------------------------------------------------------------
void SetTextBoxFraction( app::ui::UIView* view, print::WordSet* wordSet, gfl2::lyt::LytTextBox* textBox, u8 numerator, u8 denominator )
{
  if( wordSet == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( textBox == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  wordSet->RegisterNumber( 0, numerator, 3, print::NUM_DISP_SPACE, print::NUM_CODE_ZENKAKU );
  wordSet->RegisterNumber( 1, denominator, 3, print::NUM_DISP_LEFT, print::NUM_CODE_ZENKAKU );

  view->SetTextboxPaneMessage( textBox, msg_prsys_common_13 );
}

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのレベルを取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   resortID    [in]リゾートID
 *
 * @return  リゾートのレベル
 */
//------------------------------------------------------------------------------
u8 GetResortLevel( Savedata::ResortSave* resortSave, u8 resortID )
{
  return resortSave->GetResortLevel( c_ResortKind[resortID] );
}

//------------------------------------------------------------------------------
/**
 * @brief   開放済みのリゾート数を取得
 *
 * @param   none
 *
 * @return  開放済みのリゾート数
 */
//------------------------------------------------------------------------------
u8 GetDevelopedResortCount( Savedata::ResortSave* resortSave )
{
  u8 resortCount = 0;

  for( u8 i=0; i<GFL_NELEMS(c_ResortKind); ++i )
  {
    u8 level = GetResortLevel(resortSave, i);

    if( level > 0 )
    {
      resortCount++;
    }
  }

  return resortCount;
}

//------------------------------------------------------------------------------
/**
 * @brief   タマゴかどうか
 *
 * @param   cp    CoreParam
 *
 * @return  "true  = タマゴ"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool IsEgg( const pml::pokepara::CoreParam* cp )
{
  bool isEgg;

  bool fastmode_flag = cp->StartFastMode();
  {
    isEgg = cp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }
  cp->EndFastMode( fastmode_flag );

  return isEgg;
}

//------------------------------------------------------------------------------
/**
 * @brief   表示する性別を取得
 *
 * @param   cp    CoreParam
 * 
 * @retval  "pml::SEX_MALE = ♂を表示"
 * @retval  "pml::SEX_FEMALE = ♀を表示"
 * @retval  "pml::SEX_UNNON = 性別を表示しない"
 */
//------------------------------------------------------------------------------
pml::Sex GetSex( const pml::pokepara::CoreParam* cp )
{
  return PokeTool::GetDrawSex( cp );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボーナスタイムの取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   resortID    [in]リゾートID
 * @param   time        [out]時間
 *
 * @return  ボーナスタイム
 */
//------------------------------------------------------------------------------
void GetBonusTime( Savedata::ResortSave* resortSave, u32 resortID, TimeParam* time )
{
  TimeParam tmpTime;

  switch( resortID )
  {
  case RESORT_ID_FRIENDSHIP:
    {
      resortSave->GetWildPokemonServiceTime( &tmpTime.hour, &tmpTime.min, &tmpTime.sec );
    } break;
  case RESORT_ID_FARM:
    {
      resortSave->GetFarmFertilizerTime( &tmpTime.hour, &tmpTime.min, &tmpTime.sec );
    } break;
  case RESORT_ID_ADVENTURE:
    {
      resortSave->GetAdventureBonusTime( &tmpTime.hour, &tmpTime.min, &tmpTime.sec );
    } break;
  case RESORT_ID_GIM:
    {
      resortSave->GetGimBonusTime( &tmpTime.hour, &tmpTime.min, &tmpTime.sec );
    } break;
  case RESORT_ID_HOTSPA:
    {
      resortSave->GetHotSpaBonusTime( &tmpTime.hour, &tmpTime.min, &tmpTime.sec );
    } break;
  }

  // 上限チェック @fix NMCat[1660]
  if( tmpTime.hour >= 100 )
  {
    tmpTime.hour = 99;
    tmpTime.min  = 59;
    tmpTime.sec  = 59;
  }

  *time = tmpTime;
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスのポケモンの数を取得
 *
 * @param   boxPokemon    ボックスセーブデータ
 * @param   appHeap       アプリケーションヒープ
 *
 * @return  ボックスのポケモンの数
 */
//------------------------------------------------------------------------------
u32 GetBoxPokeNum( Savedata::BoxPokemon* boxPoke, app::util::Heap* appHeap )
{
  return boxPoke->GetPokemonCountAll( appHeap->GetDeviceHeap(), false );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスのポケモンの数を取得
 *
 * @param   resortSave    リゾートセーブデータ
 * @param   boxPokemon    ボックスセーブデータ
 * @param   appHeap       アプリケーションヒープ
 *
 * @return  ボックスのポケモンの数
 */
//------------------------------------------------------------------------------
u32 GetBoxPokeAndResortPokeNum( Savedata::ResortSave* resortSave, Savedata::BoxPokemon* boxPoke, app::util::Heap* appHeap )
{
  // ボックスポケモンの数を取得
  u32 pokeNum = GetBoxPokeNum( boxPoke, appHeap );

  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(appHeap->GetDeviceHeap()) pml::pokepara::CoreParam(appHeap->GetDeviceHeap());
  {
    pokeNum += static_cast<u32>(resortSave->PokemonCountHotSpaAndGim(cp));
  }
  GFL_DELETE cp;
  
  return pokeNum;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメの取得反映
 *
 * @param   resortSave  リゾートセーブデータ
 * @param   beansRank   マメの種類
 * @param   addNum      追加数
 */
//------------------------------------------------------------------------------
void AddPokeMame( Savedata::ResortSave* resortSave, Savedata::ResortSave::PokeBeansRank beansRank, u32 addNum )
{
  static const Savedata::ResortSave::PokeBeansEnum c_PokeBeansIndex[] = {
    Savedata::ResortSave::POKEBEANS_NONE,
    Savedata::ResortSave::POKEBEANS_NORMAL_01,
    Savedata::ResortSave::POKEBEANS_HEART_01,
    Savedata::ResortSave::POKEBEANS_NIJI,
  };
  static const u32 c_PokeBeansKindNum[] = {
    0, 7, 7, 1,
  };

  const Savedata::ResortSave::PokeBeansEnum offset = c_PokeBeansIndex[beansRank];

  for( u32 i=addNum; i>0; --i )
  {
    // 最小値
    Savedata::ResortSave::PokeBeansEnum minKind = offset;
    {
      u32 minNum = resortSave->GetPokeBeansNum( minKind );

      // 最小値の検索
      for( u32 j=1; j<c_PokeBeansKindNum[beansRank]; j++ )
      {
        Savedata::ResortSave::PokeBeansEnum kind =
          static_cast<Savedata::ResortSave::PokeBeansEnum>( j + offset );

        // 現在の所持数を取得
        u32 num = resortSave->GetPokeBeansNum( kind );

        // 現在の最小値と比較
        if( minNum > num )
        {
          // 最小値を更新
          minKind = kind;
          minNum  = num;
        }
      }

      // 空きなし
      if( minNum == 255 ) break;
    }

    // マメを１つ増やしてセット
    resortSave->SetPokeBeansNum( minKind, 1 );
  }
}


GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

