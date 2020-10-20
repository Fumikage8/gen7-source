//=============================================================================
/**
 * @file    TrainerTypeData.cpp
 * @brief   トレーナータイプ毎のデータ
 * @date    2012.08.29
 * @author  tamada
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <arc_def_index/arc_def.h>

#include "niji_conv_header/poke_lib/item/itemsym.h"
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include "niji_conv_header/sound/SoundMiddleID.h"

GFL_NAMESPACE_BEGIN( trainer )


namespace
{
  #include <niji_conv_header/trainer/trtype_icon_table.cdat> // for TRAINER_TYPE_ICON_TABLE
}


//-----------------------------------------------------------------------------
/**
 * @brief トレーナーアイコンのIDを取得する
 *
 * @param trtype  トレーナータイプ
 *
 * @note <arc/arc_index/trainer_icon.gaix> に定義されているＩＤを返します
 */
//-----------------------------------------------------------------------------
u8 TrainerTypeData::GetTrainerIconID( TrType trtype )
{
  if( GFL_NELEMS(TRAINER_TYPE_ICON_TABLE) <= trtype )
  {
    GFL_ASSERT(0);
    return 0;
  }
  return TRAINER_TYPE_ICON_TABLE[ trtype ];
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TrainerTypeData::TrainerTypeData():
  mTrType(-1)
{
  gfl2::std::MemClear( &mCoreData, sizeof(TRTYPE_DATA) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TrainerTypeData::LoadData( gfl2::heap::HeapBase* pHeap, TrType trtype )
{
  mTrType = trtype;
  GFL_ASSERT( trtype < TRTYPE_MAX );

  static const bool close_soon = true;
  app::util::FileAccessor::FileOpenSync( ARCID_TRTYPE_DATA, pHeap->GetLowerHandle(), close_soon );

  if( app::util::FileAccessor::IsFileOpen( ARCID_TRTYPE_DATA ) ) 
  {
    static const bool is_comp = false;
    app::util::FileAccessor::FileLoadBufSync( ARCID_TRTYPE_DATA, trtype, &mCoreData, sizeof(TRTYPE_DATA), pHeap->GetLowerHandle(), is_comp );
    app::util::FileAccessor::FileCloseSync( ARCID_TRTYPE_DATA );
  }
}

#if 0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TrainerTypeData::LoadData( gfl2::fs::ArcFile * arc, TrType trtype )
{
  mTrType = trtype;
  GFL_ASSERT( trtype < TRTYPE_MAX );
  arc->LoadData( trtype, &mCoreData );
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u8 TrainerTypeData::GetSex( void ) const
{
  return mCoreData.sex;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u8 TrainerTypeData::GetPlural( void ) const
{
  return mCoreData.plural;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u8 TrainerTypeData::GetGroup( void ) const
{
  return mCoreData.group;
}

//-----------------------------------------------------------------------------
/**
 * @brief 手持ちポケモンのボールIDを取得する
 */
//-----------------------------------------------------------------------------
item::BALL_ID TrainerTypeData::GetBallID( void ) const
{
  return static_cast<item::BALL_ID>( mCoreData.ball_id );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TrainerTypeData::IsBossGroup( TrTypeGroup trgroup )
{
  switch ( trgroup )
  {
  case TRTYPE_GRP_LEADER:
  case TRTYPE_GRP_BIGFOUR:
  case TRTYPE_GRP_CHAMPION:
    return true;
  default:
    return false;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u32 TrainerTypeData::GetEyeBGM( void ) const
{
  //nijiではトレーナータイプアーカイブからではなくトレーナータイプで直接決定する
  switch(mTrType){
  case TRTYPE_SKULLM:
  case TRTYPE_SKULLW:
    return STRM_BGM_EYE02;
  case TRTYPE_ETHERESTAFFM:
  case TRTYPE_ETHERESTAFFW:
    return STRM_BGM_EYE03;

    // @note momiji追加
  case TRTYPE_ROCKETM:
  case TRTYPE_ROCKETW:
    return STRM_BGM_MJ_VSDIVEZONE15;
  case TRTYPE_SURFER:
  case TRTYPE_KAMI:
    return STRM_BGM_EYE01;
  case TRTYPE_ETHERRESEARCHER:
    return STRM_BGM_EYE03;
  }
  return STRM_BGM_EYE01;
  //return mCoreData.eye_bgm; //sangoまでの実装
}

u32 TrainerTypeData::GetEyeSE( void ) const
{
  return mCoreData.eye_se;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BattleEffectId TrainerTypeData::GetBattleEffectID( void ) const
{
  return static_cast<BattleEffectId>(mCoreData.btl_eff_id);
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナーアイコンのIDを取得する
 */
//-----------------------------------------------------------------------------
u8 TrainerTypeData::GetTrainerIconID( void ) const
{
  return mCoreData.iconId;
}

//-----------------------------------------------------------------------------
/**
 * @brief フィールドで表示するメインモデルのIDを取得する
 */
//-----------------------------------------------------------------------------
u16 TrainerTypeData::GetFieldMainModelID( void ) const
{
  return mCoreData.fieldModelId_main;
}

//-----------------------------------------------------------------------------
/**
 * @brief フィールドで表示するサブモデルのIDを取得する
 */
//-----------------------------------------------------------------------------
u16 TrainerTypeData::GetFieldSubModelID( void ) const
{
  return mCoreData.fieldModelId_sub;
}

//-----------------------------------------------------------------------------
/**
 * @brief バトルで表示するメインモデルのIDを取得する
 */
//-----------------------------------------------------------------------------
u16 TrainerTypeData::GetBattleMainModelID( void ) const
{
  return mCoreData.battleModelId_main;
}

//-----------------------------------------------------------------------------
/**
 * @brief バトルで表示するサブモデルのIDを取得する
 */
//-----------------------------------------------------------------------------
u16 TrainerTypeData::GetBattleSubModelID( void ) const
{
  return mCoreData.battleModelId_sub;
}

//-----------------------------------------------------------------------------
/**
 * @brief   メガ進化アイテムID取得
 */
//-----------------------------------------------------------------------------
u16 TrainerTypeData::GetMegaItemId( TrType trtype )
{
  switch ( trtype )
  {
  //メガリング
  case TRTYPE_GINA:	//ジーナ（XY登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_GINA2:	//ジーナ（XY登場NPC）ジムリーダー就任後
  case TRTYPE_DEKUSIO:	//デクシオ（XY登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_DEKUSIO2:	//デクシオ（XY登場NPC）ジムリーダー就任後
    return ITEM_MEGARINGU;

  //メガネックレス
  case TRTYPE_MITSURU:	//ミツル（ORAS登場NPC）
    return ITEM_MEGANEKKURESU;

  //キーストーン
  default:
  case TRTYPE_HERO:	//主人公♂（MyStatus::HasZenryokuRingセット後・モデル差し替えなし）
  case TRTYPE_HEROINE:	//主人公♀（MyStatus::HasZenryokuRingセット後・モデル差し替えなし）
  case TRTYPE_GIMA:	//ギーマ（BW登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_LIRA:	//リラ（RS登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_AKUROMA:	//アクロマ（BW2登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_SIRONA:	//シロナ（BW登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_RED:	//レッド（RGB登場NPC）但しデータ上で持たない可能性もある。
  case TRTYPE_GREEN:	//グリーン（RGB登場NPC）但しデータ上で持たない可能性もある。
    return ITEM_KIISUTOON;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 見せ場に使用されるトレーナータイプか？の判定
 * @param[in]   trtype
 * @return      true  見せ場に使用される
 *
 * @note    メガ進化を常にロングバージョンで見せるか？の判定に使用。
 * @note    もっと細かく指定したい場合はスクリプトからフラグを渡すこと
 */
//-----------------------------------------------------------------------------
bool TrainerTypeData::IsHighLightSceneTrainerType( TrType trtype )
{
  return false;
}



GFL_NAMESPACE_END( trainer )
