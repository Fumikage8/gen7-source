//==============================================================================
/**
 @file    PokeToolPokeGet.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.07
 @brief   ポケモンをゲットしたときの処理
 */
//==============================================================================

// poke_libのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <PokeTool/include/PokeToolExtendData.h>
#include <PokeTool/include/PokeToolPokePlace.h>
#include <PokeTool/include/PokeMemo.h>

#include <Savedata/include/ZukanSave.h>
#include <Savedata/include/BoxSave.h>
#include <Savedata/include/BoxPokemonSave.h>
#include <Savedata/include/ConfigSave.h>
#include <Savedata/include/EventWork.h>

#include <Field/FieldStatic/include/Zone/ZoneDataAccessor.h>

#include <PokeTool/include/PokeToolPokeGet.h>




GFL_NAMESPACE_BEGIN(PokeTool)


PokeGetUtil::ReturnType::Tag PokeGetUtil::UpdatePokeAtPokeGet(
    gfl2::heap::HeapBase*         heap,
    pml::pokepara::PokemonParam*  pp,
    CallerType::Tag               callerType,
    ReturnParam*                  returnParam,
    ExData*                       exData
)
{
  ReturnType::Tag returnType = ReturnType::FINISHED;
  ReturnParam l_returnParam;

  // トレーナーメモ(PokeMemo)
  setPokeMemo(heap, pp, callerType, exData);

  if( Check(pp, callerType) )
  {
    returnType = ReturnType::CALL_ZUKAN_REGISTER;
  }
  else
  {
    if( DoesPut_NotSetFlag(pp, callerType) )
    {
      PutPlace::Tag putPlace = PutPlace::NONE;
      PutSpace::Tag putSpace = PutSpace::NONE;
      if( DoesDecidePlaceSpaceIfPut(pp, callerType, &putPlace, &putSpace) )
      {
        if( putSpace == PutSpace::FREE )
        {
          Put_NotSetFlag(
              heap,
              pp,
              putPlace,
              putSpace,
              0,  // 使わないのでてきとー
              0,  // 使わないのでてきとー
              &l_returnParam
          );
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesDecidePlaceSpaceIfPutはFREEしか返さないはず。
        }
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesPut_NotSetFlagがtrueでDoesGoToPokePutSelectがfalseなら、DoesDecidePlaceSpaceIfPutはtrueになるはず。
      }
    }
    if( DoesSetFlag(pp, callerType) )
    {
      SetFlag(pp);
    }
  }

  if(returnParam)
  {
    *returnParam = l_returnParam;
  }
  return returnType;
}

void PokeGetUtil::PutPoke(
    gfl2::heap::HeapBase*         heap,
    pml::pokepara::PokemonParam*  pp,
    PutPlace::Tag                 putPlace,
    PutSpace::Tag                 putSpace,
    u32                           pos0,
    u32                           pos1,
    ReturnParam*                  returnParam
)
{
  Put_NotSetFlag(
      heap,
      pp,
      putPlace,
      putSpace,
      pos0,
      pos1,
      returnParam
  );
  SetFlag(pp);
}


bool PokeGetUtil::Check(
    const pml::pokepara::PokemonParam*  pp,
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  if(
         DoesGoToInfoShow(pp, callerType)       // 図鑑詳細画面がある場合は、図鑑登録アプリを呼ぶ。
      || DoesGoToPokeNameInput(pp, callerType)  // 名前入力画面がある場合は、図鑑登録アプリを呼ぶ。
      || DoesGoToPokePutSelect(pp, callerType)  // 送り先選択画面がある場合は、図鑑登録アプリを呼ぶ。
  )
  {
    ret = true;
  }
  return ret;
}

bool PokeGetUtil::DoesGoToInfoShow(
    const pml::pokepara::PokemonParam*  pp,
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  switch(callerType)
  {
  case CallerType::HOKAKU:
  case CallerType::HOKAKU_RESORT:
  case CallerType::KOUKAN:
  case CallerType::KOUKAN_NET:
  case CallerType::SHINKA:
  case CallerType::SHINKA_NET:
  case CallerType::TAMAGO_FUKA:
  case CallerType::TAMAGO_FUKA_RESORT:
  case CallerType::MORAU:
  case CallerType::MORAU_NANASHI:
  case CallerType::MORAU_NANASHI_PARTY:
  case CallerType::HAIFU:
  case CallerType::HAIFU_NANASHI:
    {
      if( PokeTool::ExtendData::GetLocalNumberStatic(pp->GetMonsNo(), PokeTool::ExtendData::LOCAL_AREA_A) != PokeTool::ExtendData::LOCAL_NUMBER_NONE )  // アローラ図鑑に載っているポケモン
      {
        const Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
        if( svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_GET) )  // 図鑑を入手しているか
        {
          const Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
          bool seeFlag = false;
          if( !( svZukanData->GetPokeRegisterShowFlag(pp->GetMonsNo(), &seeFlag) ) )  // 初ゲットポケモン
          {
            ret = true;
          }
        }
      }
      break;
    }
  }
  return ret;
}
bool PokeGetUtil::DoesGoToPokeNameInput(
    const pml::pokepara::PokemonParam*  /*pp*/, 
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  switch(callerType)
  {
  case CallerType::HOKAKU:
  case CallerType::HOKAKU_RESORT:
  case CallerType::TAMAGO_FUKA:
  case CallerType::TAMAGO_FUKA_RESORT:
  case CallerType::MORAU_NANASHI:
  case CallerType::MORAU_NANASHI_PARTY:
  case CallerType::HAIFU_NANASHI:
    {
      ret = true;
      break;
    }
  }
  return ret;
}
bool PokeGetUtil::DoesGoToPokePutSelect(
    const pml::pokepara::PokemonParam*  /*pp*/,
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  switch(callerType)
  {
  case CallerType::HOKAKU:
  case CallerType::HOKAKU_RESORT:
  case CallerType::MORAU:
  case CallerType::MORAU_TAMAGO:
  case CallerType::MORAU_NANASHI:
  case CallerType::HAIFU:
  case CallerType::HAIFU_NANASHI:
  case CallerType::HAIFU_TAMAGO:
    {
      pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      if( party->IsFull() )  // 手持ちがいっぱい
      {
        const Savedata::ConfigSave* svConfig = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetConfig();
        if( svConfig->GetBoxStatus() == Savedata::ConfigSave::BOX_STATUS_MANUAL_SELECT )  // 手持ちかボックスかは自分で決める
        {
          ret = true;
        }
      }
      break;
    }
  }
  return ret;
}
bool PokeGetUtil::DoesPut_NotSetFlag(
    const pml::pokepara::PokemonParam*  /*pp*/,
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  switch(callerType)
  {
  case CallerType::HOKAKU:
  case CallerType::HOKAKU_RESORT:
  case CallerType::MORAU:
  case CallerType::MORAU_TAMAGO:
  case CallerType::MORAU_NANASHI:
  case CallerType::MORAU_NANASHI_PARTY:
  case CallerType::HAIFU:
  case CallerType::HAIFU_NANASHI:
  case CallerType::HAIFU_TAMAGO:
    {
      ret = true;
      break;
    }
  }
  return ret;
}

bool PokeGetUtil::DoesDecidePlaceSpaceIfPut(
    const pml::pokepara::PokemonParam*  pp,
    CallerType::Tag                     callerType,
    PutPlace::Tag*                      putPlace,
    PutSpace::Tag*                      putSpace
)
{
  bool ret = false;
  *putPlace = PutPlace::NONE;
  *putSpace = PutSpace::NONE;

  if( DoesPut_NotSetFlag(pp, callerType) )
  {
    if( !DoesGoToPokePutSelect(pp, callerType) )  // 送り先選択画面がある場合は、配置区分、配置場所を決められない
    {
      switch(callerType)
      {
      case CallerType::HOKAKU:
      case CallerType::HOKAKU_RESORT:
      case CallerType::MORAU:
      case CallerType::MORAU_TAMAGO:
      case CallerType::MORAU_NANASHI:
      case CallerType::HAIFU:
      case CallerType::HAIFU_NANASHI:
      case CallerType::HAIFU_TAMAGO:
        {
          pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
          if( !party->IsFull() )  // 手持ちに空きがある
          {
            ret = true;
            *putPlace = PutPlace::PARTY;
            *putSpace = PutSpace::FREE;
          }
          else
          {
            const Savedata::ConfigSave* svConfig = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetConfig();
            if( svConfig->GetBoxStatus() == Savedata::ConfigSave::BOX_STATUS_AUTO_SELECT )  // 自動でボックスに送る
            {
              ret = true;
              *putPlace = PutPlace::BOX;
              *putSpace = PutSpace::FREE;
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesGoToPokePutSelectの結果と異なっている。
            }
          }
          break;
        }
      case CallerType::MORAU_NANASHI_PARTY:
        {
          pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
          if( !party->IsFull() )  // 手持ちに空きがある
          {
            ret = true;
            *putPlace = PutPlace::PARTY;
            *putSpace = PutSpace::FREE;
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。手持ちに空きがないのでポケモンを配置できない。
          }
          break;
        }
      }
    }
  }

  return ret;
}

bool PokeGetUtil::DoesSetFlag(
    const pml::pokepara::PokemonParam*  /*pp*/,
    CallerType::Tag                     callerType
)
{
  bool ret = false;
  switch(callerType)
  {
  case CallerType::HOKAKU:
  case CallerType::HOKAKU_RESORT:
  case CallerType::KOUKAN:
  case CallerType::SHINKA:
  case CallerType::TAMAGO_FUKA:
  case CallerType::TAMAGO_FUKA_RESORT:
  case CallerType::MORAU:
  case CallerType::MORAU_TAMAGO:
  case CallerType::MORAU_NANASHI:
  case CallerType::MORAU_NANASHI_PARTY:
  case CallerType::HAIFU:
  case CallerType::HAIFU_NANASHI:
  case CallerType::HAIFU_TAMAGO:
  case CallerType::KOUKAN_TAMAGO:
    {
      ret = true;
      break;
    }
  }
  return ret;

}


void PokeGetUtil::Put_NotSetFlag(
    gfl2::heap::HeapBase*               heap,
    const pml::pokepara::PokemonParam*  pp,
    PutPlace::Tag                       putPlace,
    PutSpace::Tag                       putSpace,
    u32                                 pos0,
    u32                                 pos1,
    ReturnParam*                        returnParam
)
{
  *returnParam = ReturnParam();
  
  switch(putPlace)
  {
  case PutPlace::PARTY:
    {
      pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      switch(putSpace)
      {
      case PutSpace::FIX:
        {
          if( (static_cast<s32>(pos0)) < (static_cast<s32>(pml::PokeParty::MAX_MEMBERS)) )
          {
            party->ReplaceMember(pos0, *pp);
            setPutPlacePartyInReturnParam(returnParam, pos0);
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。手持ちの最大数より大きい。ポケモンを配置しない。
          }
          break;
        }
      case PutSpace::FREE:
        {
          if( !party->IsFull() )
          {
            u32 count = party->GetMemberCount();
            bool ret = party->AddMember(*pp);
            if(ret)
            {
              setPutPlacePartyInReturnParam(returnParam, count);
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。ポケモンを配置しない。
            }
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。空きがない。ポケモンを配置しない。
          }
          break;
        }
      default:
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。putSpaceが不適。ポケモンを配置しない。
          break;
        }
      }
      break;
    }
  case PutPlace::BOX:
    {
      Savedata::BOX* svBox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
      Savedata::BoxPokemon* svBoxPoke = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
      switch(putSpace)
      {
      case PutSpace::FIX:
        {
          if( pos0 < Savedata::BoxPokemon::TRAY_MAX && pos1 < Savedata::BoxPokemon::TRAY_POKE_MAX )
          {
            // クリアしてそこに配置する
            svBoxPoke->ClearPokemon( pos0, pos1, heap->GetLowerHandle() );  // heapは一時的に使用するだけなので後方確保にしておく。
            bool ret = svBoxPoke->PutPokemon( pp, pos0, pos1, heap->GetLowerHandle() );  // heapは一時的に使用するだけなので後方確保にしておく。
            if(ret)
            {
              setPutPlaceBoxInReturnParam(returnParam, pos0, pos1);
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。ポケモンを配置しない。
            }
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ボックスのトレイ数、ポケモン数の最大より大きい。ポケモンを配置しない。
          }
          break;
        }
      case PutSpace::FREE:
        {
          if( svBoxPoke->GetSpaceCountAll(heap->GetLowerHandle()) > 0 )  // heapは一時的に使用するだけなので後方確保にしておく。
          {
            u32 tray = svBox->GetCurrentTray();
            if( tray >= Savedata::BoxPokemon::TRAY_MAX )
            {
              tray = 0;
            }
            u16 posInTray = 0;
            tray = svBoxPoke->PutPokemonEmptyTrayAll( pp, tray, heap->GetLowerHandle(), &posInTray );  // heapは一時的に使用するだけなので後方確保にしておく。
            if( tray < Savedata::BoxPokemon::TRAY_MAX )
            {
              setPutPlaceBoxInReturnParam(returnParam, tray, static_cast<u16>(posInTray));
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。ポケモンを配置しない。
            }
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。空きがない。ポケモンを配置しない。
          }
          break;
        }
      default:
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。putSpaceが不適。ポケモンを配置しない。
          break;
        }
      }
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。putPlaceが不適。ポケモンを配置しない。
      break;
    }
  }
}


void PokeGetUtil::SetFlag(const pml::pokepara::PokemonParam*  pp)
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  //svZukanData->SetPokeSee(*pp);  SetPokeGet関数の中でseeフラグは立つので、外部からは立てないほうがいいのでコメントアウト。
  svZukanData->SetPokeGet(*pp);
  // svZukanData->IncrementPokeSeeAmountは今回は呼んでいない。呼ぶなら外でか？
}


void PokeGetUtil::setPutPlacePartyInReturnParam(ReturnParam* returnParam, u32 pos0)
{
  returnParam->isValid  = true;
  returnParam->putPlace = PutPlace::PARTY;
  returnParam->pos0     = pos0;
}
void PokeGetUtil::setPutPlaceBoxInReturnParam(ReturnParam* returnParam, u32 pos0, u32 pos1)
{
  returnParam->isValid  = true;
  returnParam->putPlace = PutPlace::BOX;
  returnParam->pos0     = pos0;
  returnParam->pos1     = pos1;
}

void PokeGetUtil::setPokeMemo(
    gfl2::heap::HeapBase*         heap,
    pml::pokepara::PokemonParam*  pp,
    CallerType::Tag               callerType,
    ExData*                       exData
)
{
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  const Savedata::MyStatus* myStatus = gameData->GetPlayerStatus();
  Field::ZoneDataAccessor accessor( gameData->GetFieldZoneDataLoader()->GetZoneData(gameData->GetNowZoneID()) );
  u32 placeLabelID = static_cast<u32>(accessor.GetPlaceLabelID());  // u16->u32

  gfl2::heap::HeapBase* tempHeap = heap->GetLowerHandle();
  
  switch(callerType)
  {
  case CallerType::HOKAKU:
    {
      PokeTool::PokeMemo::SetFromCapture(pp, myStatus, placeLabelID, tempHeap);
      break;
    }
  case CallerType::HOKAKU_RESORT:
    {
      PokeTool::PokeMemo::SetFromCapture(pp, myStatus, PokeTool::PokePlace::PLACE_NO_SPECIAL_POKERESORT, tempHeap);
      break;
    }
  case CallerType::KOUKAN:
    {
      PokeTool::PokeMemo::SetFromInnerTrade(pp);
      break;
    }
  case CallerType::KOUKAN_NET:
    {
      PokeTool::PokeMemo::SetFromOuterTrade(pp);
      break;
    }
  case CallerType::SHINKA:
    {
      // 何もしない
      break;
    }
  case CallerType::SHINKA_NET:
    {
      // 何もしない
      break;
    }
  case CallerType::TAMAGO_FUKA:
    {
      PokeTool::PokeMemo::SetFromEggBirth(pp, myStatus, placeLabelID, tempHeap);
      break;
    }
  case CallerType::TAMAGO_FUKA_RESORT:
    {
      PokeTool::PokeMemo::SetFromEggBirth(pp, myStatus, PokeTool::PokePlace::PLACE_NO_SPECIAL_POKERESORT, tempHeap);
      break;
    }
  case CallerType::MORAU:
    {
      PokeTool::PokeMemo::SetFromCapture(pp, myStatus, placeLabelID, tempHeap);
      break;
    }
  case CallerType::MORAU_TAMAGO:
    {
      u32 placeNo = placeLabelID;
      if(exData != NULL)
      {
        placeNo = exData->exDataUnion.exDataForMorauTamago.placeNo;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }
      PokeTool::PokeMemo::SetFromEggTaken(pp, myStatus, placeNo, tempHeap);
      break;
    }
  case CallerType::MORAU_NANASHI:
    {
      PokeTool::PokeMemo::SetFromCapture(pp, myStatus, placeLabelID, tempHeap);
      break;
    }
  case CallerType::MORAU_NANASHI_PARTY:
    {
      PokeTool::PokeMemo::SetFromCapture(pp, myStatus, placeLabelID, tempHeap);
      break;
    }
  case CallerType::HAIFU:
    {
      u32 placeNo = placeLabelID;
      u32 year    = 0;
      u32 month   = 0;
      u32 day     = 0;
      if(exData != NULL)
      {
        placeNo = exData->exDataUnion.exDataForHaifu.placeNo;
        year    = exData->exDataUnion.exDataForHaifu.year;
        month   = exData->exDataUnion.exDataForHaifu.month;
        day     = exData->exDataUnion.exDataForHaifu.day;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }
      PokeTool::PokeMemo::SetFromDistribution(pp, placeNo, year, month, day);
      break;
    }
  case CallerType::HAIFU_NANASHI:
    {
      u32 placeNo = placeLabelID;
      u32 year    = 0;
      u32 month   = 0;
      u32 day     = 0;
      if(exData != NULL)
      {
        placeNo = exData->exDataUnion.exDataForHaifu.placeNo;
        year    = exData->exDataUnion.exDataForHaifu.year;
        month   = exData->exDataUnion.exDataForHaifu.month;
        day     = exData->exDataUnion.exDataForHaifu.day;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }
      PokeTool::PokeMemo::SetFromDistribution(pp, placeNo, year, month, day);
      break;
    }
  case CallerType::HAIFU_TAMAGO:
    {
      u32 placeNo = placeLabelID;
      if(exData != NULL)
      {
        placeNo = exData->exDataUnion.exDataForHaifu.placeNo;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      }
      PokeTool::PokeMemo::SetFromEggTaken(pp, myStatus, placeNo, tempHeap);
      break;
    }
  case CallerType::KOUKAN_TAMAGO:
    {
      PokeTool::PokeMemo::SetFromInnerTrade(pp);
      break;
    }
  case CallerType::KOUKAN_TAMAGO_NET:
    {
      PokeTool::PokeMemo::SetFromOuterTrade(pp);
      break;
    }
  }
}




GFL_NAMESPACE_END(PokeTool)

