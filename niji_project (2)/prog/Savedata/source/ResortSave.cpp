//===================================================================
/**
 * @file    ResortSave.cpp
 * @brief   ポケモンリゾートのセーブ
 * @author  k.ohno
 * @date    2015.8.29
 */
//===================================================================

#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/NijiTime.h"

#include "../include/ResortSave.h"


GFL_NAMESPACE_BEGIN(Savedata)


enum{
  _SECOND,
  _MINITS,
  _HOUR,
  _DAY,
  _MONTH,
  _YEARLOWER,
  _YEARUPPER,
  _TIME,
  _TIME_MAX,
};


ResortSave::ResortSave( void )
{
  OHNO_PRINT("ResortSave::ResortSave %d \n",sizeof(ResortSaveData));

  
#if defined(GF_PLATFORM_WIN32)
   _rtcoffset = 0;
#else
   _rtcoffset = nn::cfg::GetUserTimeOffset().GetNanoSeconds();//ラップされていなかったのでこのまま使用
#endif
}

ResortSave::~ResortSave()
{
}



#if 0
void ResortSave::Debug10YearTest(void)
{
//テスト  １０年分何も無いことを確認

  u8 timeab[_TIME_MAX];

  u64 oldtime = 0;
  u64 newtime = 0;
  
  for(int yearlow=16 ; yearlow < 26; yearlow++){
    for(int month = 1 ; month <= 12; month++){
      
      int daybuff[]={0,31,28,31,30,31,30,31,31,30,31,30,31};

			if(yearlow%4==0){
				daybuff[2]=29;
			}

      for(int day = 1 ; day <= daybuff[month]; day++){

        timeab[_YEARUPPER] = 20;
        timeab[_YEARLOWER] = yearlow;
        timeab[_MONTH] = month;
        timeab[_DAY] = day;

        timeab[_HOUR] = 0;
        timeab[_MINITS] = 0;
        timeab[_SECOND] = 1;
        if(newtime==0){
           newtime = getDateTimeToSecond(timeab);  //最初だけはここを通過

          GFL_RELEASE_PRINT( "start\n"  );
        }
        else{
          newtime = getDateTimeToSecond(timeab);

          GFL_RELEASE_PRINT( "diffT %llu %llu\n",  oldtime, newtime  );
          if(newtime > (oldtime + (u64)66)){
            GFL_ASSERT(0);
          }
        }

        timeab[_HOUR] = 23;
        timeab[_MINITS] = 59;
        timeab[_SECOND] = 1;
        oldtime = getDateTimeToSecond(timeab);
      
      }
    }
  }

}
#endif




//--------------------------------------------------------------------------------------------
/**
 * @brief セーブデータクリア処理
 * @param heap
 */
//--------------------------------------------------------------------------------------------
void ResortSave::Clear( gfl2::heap::HeapBase* heap)
{
  gfl2::std::MemClear( &mData, sizeof(ResortSaveData) );


  u32 x = gfl2::math::Random::CreateGeneralSeed();
  gfl2::math::Random rand;
  rand.Initialize( x );
  //  rand.GetContext(&mData.random);
  mData.random = rand.SaveState();

  ResetTime();	// ここで本体時間で初期化しています。
  

  for(int no=0;no<FRIENDSHIP_GROUND_BEANS_MAX;no++){
    mData.GroundBeans[no] = POKEBEANS_NONE;
  }
	for(int i=0;i<ADVENTURE_ITEM_MAX;i++){
		mData.mItemBuff[i] = ITEM_DUMMY_DATA;
	}

  pml::pokepara::CoreParam * cp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap, MONSNO_NULL, 0, 0 );

  for( int i=0; i<STOCKPOKEMON_MAX; i++ ){
    cp->Serialize_Core( mData.mAdventurePokeStock[i].pokemonCoreParam );
    cp->Serialize_Core( mData.mGimPokeStock[i].pokemonCoreParam );
    cp->Serialize_Core( mData.mFriendShipPokeStock[i].pokemonCoreParam );
    cp->Serialize_Core( mData.mFarmPokeStock[i].pokemonCoreParam );
    cp->Serialize_Core( mData.mHotSpaPokeStock[i].pokemonCoreParam );
    mData.mAdventurePokeStock[i].bnew=false;
    mData.mGimPokeStock[i].bnew=false;
    mData.mFriendShipPokeStock[i].bnew=false;
    mData.mFarmPokeStock[i].bnew=false;
    mData.mHotSpaPokeStock[i].bnew=false;
  }
  cp->Serialize_Core( mData.wildPoke[0].pokemonCoreParam );
  cp->Serialize_Core( mData.wildPoke[1].pokemonCoreParam );
  cp->Serialize_Core( mData.wildPoke[2].pokemonCoreParam );

  mData.mAdventureCoursePattern = RESORT_ADV_COURSE_NONE;

  SetResortLevel(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP,1);
  SetPokemonEventTime(0,1,0);
  SetWildPokemonTime(1,0,0);
  SetFriendShipTime(1,0,0);
  SetBeansIvyHiddenTime(24,0,0);

//  SetBeansGetCount(20);  //最初かならずまめが地面に落ちるように

  // Default PokeBeans:
  SetPokeBeansNum(POKEBEANS_NORMAL_01,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_02,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_03,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_04,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_05,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_06,7);
  SetPokeBeansNum(POKEBEANS_NORMAL_07,7);

  GFL_DELETE cp;

#if 0
  Debug10YearTest();
#endif
}


//=============================================================================================
/**
 * @brief 初期化処理
 *
 * @param heap ヒープ
 */
//=============================================================================================
//void ResortSave::Initialize( gfl2::heap::HeapBase*  )
//{
//Clear(NULL);
//}





/**
 *  SetData
 */
void ResortSave::SetData( void * data )
{
  gfl2::std::MemCopy( data, &mData, sizeof(ResortSaveData) );
}



//======================================================================================================================
//======================================================================================================================

/**
 * シリアライズデータのサイズを取得
 *
 * @retval       u32    シリアライズデータのバイトサイズ
 */
u32   ResortSave::GetSerializeDataSize( void )
{
  return sizeof(ResortSaveData);
}

/**
 *  シリアライズ
 */
u32  ResortSave::Serialize( void* dst ) const
{
  gfl2::std::MemCopy( reinterpret_cast<const void*>(&mData), dst, sizeof(mData) );
  return sizeof(mData);
}

/**
 *  デシリアライズ
 */
void ResortSave::Deserialize( const void* data )
{
  gfl2::std::MemCopy( data, reinterpret_cast<void*>(&mData), sizeof(mData) );
}


//--------------------------------------------------------------
/**
 * @brief    散らばっている豆を得る
 * @param    index  順番
 * @param    まめのしゅるい
 */
//--------------------------------------------------------------
void ResortSave::SetGroundBeans(int no, u8 beans)
{
  GFL_ASSERT( no < FRIENDSHIP_GROUND_BEANS_MAX );
  mData.GroundBeans[no] = beans;
}

//--------------------------------------------------------------
/**
 * @brief   散らばっている豆のランクを取得
 * @param   no  順番
 * @return  ランク
 */
//--------------------------------------------------------------
ResortSave::PokeBeansRank ResortSave::GetGroundBeansRank( int no )
{
  GFL_ASSERT( no < FRIENDSHIP_GROUND_BEANS_MAX );
  return GetBeansRank( static_cast<PokeBeansEnum>(mData.GroundBeans[no]) );
}

//--------------------------------------------------------------
/**
 * @brief   豆のランクを取得
 * @param   beans   種類
 * @return  ランク
 */
//--------------------------------------------------------------
ResortSave::PokeBeansRank ResortSave::GetBeansRank( PokeBeansEnum beans )
{
  if( beans == POKEBEANS_NONE )
  {
    return POKEBEANS_RANK_NONE;
  }
  else if( beans == POKEBEANS_NIJI )
  {
    return POKEBEANS_RANK_S_RARE;
  }
  else if( beans >= POKEBEANS_HEART_01 && beans <= POKEBEANS_HEART_07 )
  {
    return POKEBEANS_RANK_RARE;
  }
  return POKEBEANS_RANK_NORMAL;
}

//--------------------------------------------------------------
/**
 * @brief 種類に応じた持っている数を得る
 * @param PokeBeansEnum
 */
//--------------------------------------------------------------
u8 ResortSave::GetPokeBeansNum(PokeBeansEnum kind)
{
  return mData.mPokeBeansNum[kind];
}

//--------------------------------------------------------------
/**
 * @brief 種類に応じた持っている数を増減させる
 * @param PokeBeansEnum
 * @param add ＋ならふやし－ならへらす
 */
//--------------------------------------------------------------
void ResortSave::SetPokeBeansNum(PokeBeansEnum kind,int add)
{
  int x = (int)mData.mPokeBeansNum[kind];
  x+=add;
  if(x<0){
    x=0;
  }
  else if(x>255){
    x=255;
  }
  mData.mPokeBeansNum[kind] = (u8)x;
}

//--------------------------------------------------------------
/**
 * @brief 種類に応じた持っているまめ数を得る
 * @param beans ノーマルかハートか
 */
//--------------------------------------------------------------
int ResortSave::GetBigBeansKind(int beans )
{
  int x = 0;
  int ret = 0;

  for(int k = 0; k < 7 ;k++){
    if(mData.mPokeBeansNum[beans+k] > x ){
      x = mData.mPokeBeansNum[beans+k];
      ret = beans+k;
    }
  }
  return ret;
}



//--------------------------------------------------------------
/**
 * @brief 種類に応じた持っているまめ数をへらす
 * @param normal  ノーマルの数
 * @param rare  レアの数
 * @param niji  にじまめのかず
 */
//--------------------------------------------------------------
void ResortSave::SubPokeBeansNum(int normal, int rare, int niji )
{

  mData.mPokeBeansNum[POKEBEANS_NIJI] -= niji;

  for(int k = normal; k > 0 ;k--){
    int kind = GetBigBeansKind(POKEBEANS_NORMAL_01);
    mData.mPokeBeansNum[kind]--;
  }
  for(int k = rare; k > 0 ;k--){
    int kind = GetBigBeansKind(POKEBEANS_HEART_01);
    mData.mPokeBeansNum[kind]--;
  }
}



//--------------------------------------------------------------
/**
 * @brief      現在の時間との差分を得る
 * @attention  スタック消費します
 * @return      秒単位の時間の差  マイナスは返さない
 */
//--------------------------------------------------------------
u64 ResortSave::getDateTimeToSecond(u8* time8)
{
  u32 year = time8[_YEARUPPER];
  year = year * 100 + time8[_YEARLOWER];

  mTempDate.GetDate()->SetDateTimeToSecond(0);  //SystemMCAT 4301   初期化

  mTempDate.GetDate()->SetYear(  year );
	mTempDate.GetDate()->SetMonth(  time8[_MONTH] );
  mTempDate.GetDate()->SetDay( time8[_DAY]);
  mTempDate.GetDate()->SetHour( time8[_HOUR] );
  mTempDate.GetDate()->SetMinute( time8[_MINITS] );
  mTempDate.GetDate()->SetSecond(time8[_SECOND]);
  mTempDate.GetDate()->SetMilliSecond( 0);
  
  return mTempDate.GetDate()->GetDateTimeToSecond();
}


//--------------------------------------------------------------
/**
 * @brief      現在の時間との差分を得る
 * @attention  スタック消費します
 * @return     秒単位の時間の差
 */
//--------------------------------------------------------------

u64 ResortSave::GetDiffTimeDate(GameSys::DeviceDate* pdate)
{
  u8 time8[_TIME_MAX];

  time8[_YEARUPPER] = pdate->GetYear() / 100;
  time8[_YEARLOWER] = pdate->GetYear() % 100;
  time8[_MONTH] = pdate->GetMonth();
  time8[_DAY] = pdate->GetDay();
  time8[_HOUR] = pdate->GetHour();
  time8[_MINITS] = pdate->GetMinute();
  time8[_SECOND] = pdate->GetSecond();


  return GetDiffTime(time8);
}


//--------------------------------------------------------------
/**
 * @brief      現在の時間との差分を得る
 * @attention  スタック消費します
 * @return     秒単位の時間の差
 */
//--------------------------------------------------------------
u64 ResortSave::GetDiffTime(u8* time8)
{
  u64 x = getDateTimeToSecond(time8);
  u64 y = getDateTimeToSecond(mData.mLastAccessTime);

  if( y > x ){
    return 0;
  }
  return x - y;
}

//--------------------------------------------------------------
/**
 * @brief   終了時に時間を設定する
 * @return
 */
//--------------------------------------------------------------
void ResortSave::SetPlayTime(GameSys::DeviceDate* pdate)
{
  mData.mLastAccessTime[_YEARUPPER] = pdate->GetYear() / 100;
  mData.mLastAccessTime[_YEARLOWER] = pdate->GetYear() % 100;
  mData.mLastAccessTime[_MONTH] = pdate->GetMonth();
  mData.mLastAccessTime[_DAY] = pdate->GetDay();
  mData.mLastAccessTime[_HOUR] = pdate->GetHour();
  mData.mLastAccessTime[_MINITS] = pdate->GetMinute();
  mData.mLastAccessTime[_SECOND] = pdate->GetSecond();

}


//--------------------------------------------------------------
/**
 * @brief 野生のPokemonを得る３体
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetWildPokemon(int index, pml::pokepara::CoreParam * cp)
{
  GFL_ASSERT(index < RESORT_FRIENDSHIP_PLAYPOKE_MAX);
  GFL_ASSERT(index >= 0);
  cp->Deserialize_Core( mData.wildPoke[index].pokemonCoreParam );
}

//--------------------------------------------------------------
/**
 * @brief 野生のPokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetWildPokemon(int index, const pml::pokepara::CoreParam * cp)
{
  GFL_ASSERT(index < RESORT_FRIENDSHIP_PLAYPOKE_MAX);
  GFL_ASSERT(index >= 0);
  cp->Serialize_Core( mData.wildPoke[index].pokemonCoreParam );
  mData.updateFlg=true;
}


//--------------------------------------------------------------
/**
 * @brief    ポケモンイベント抽選の時間
 * @param    時間、分、秒
   @return   時間が０ならtrue
 */
//--------------------------------------------------------------

bool ResortSave::PokemonEventTimeDownCount(void)
{
  return downTimeCount( mData.mPokemonEventTime );
}


//--------------------------------------------------------------
/**
 * @brief    ポケモンイベント抽選の時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------

void ResortSave::SetPokemonEventTime(u8 hour,u8 min,u8 sec)
{
  mData.mPokemonEventTime[_HOUR]=hour;
  mData.mPokemonEventTime[_MINITS]=min;
  mData.mPokemonEventTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    ポケモンイベント抽選の時間を得る
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetPokemonEventTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mPokemonEventTime[_HOUR];
  *min = mData.mPokemonEventTime[_MINITS];
  *sec = mData.mPokemonEventTime[_SECOND];
}




//--------------------------------------------------------------
/**
 * @brief    収穫の時間
 * @param    index RESORT_FARM_MAX
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetFarmTime(int index,u8* hour,u8* min,u8* sec)
{
  *hour = mData.mFarmTime[index][_HOUR];
  *min = mData.mFarmTime[index][_MINITS];
  *sec = mData.mFarmTime[index][_SECOND];
}


//--------------------------------------------------------------
/**
 * @brief   収穫の時間
 * @param   index RESORT_FARM_MAX
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetFarmTime(int index,u8 hour,u8 min,u8 sec)
{
  mData.mFarmTime[index][_HOUR]=hour;
  mData.mFarmTime[index][_MINITS]=min;
  mData.mFarmTime[index][_SECOND]=0;//sec;
}


//--------------------------------------------------------------
/**
 * @brief   収穫の時間を減らしていく
 * @param   index RESORT_FARM_MAX
 * @param   時間になったらtrue
 */
//--------------------------------------------------------------
bool ResortSave::FarmTimeDownCount(int index)
{
  return downTimeCount( mData.mFarmTime[index] );
}


//--------------------------------------------------------------
/**
 * @brief    こやしの効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetFarmFertilizerTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mFarmFertilizerTime[_HOUR];
  *min = mData.mFarmFertilizerTime[_MINITS];
  *sec = mData.mFarmFertilizerTime[_SECOND];
}


//--------------------------------------------------------------
/**
 * @brief   こやしの効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetFarmFertilizerTime(u8 hour,u8 min,u8 sec)
{
  mData.mFarmFertilizerTime[_HOUR]=hour;
  mData.mFarmFertilizerTime[_MINITS]=min;
  mData.mFarmFertilizerTime[_SECOND]=0;//sec;
}


//--------------------------------------------------------------
/**
 * @brief    こやしの効果持続時間ダウンカウント
 * @return   ボーナス時間ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::FarmFertilizerTimeDownCount(void)
{
  return downTimeCountBonus( mData.mFarmFertilizerTime );
}



//--------------------------------------------------------------
/**
 * @brief    こやしの効果持続時間ダウンカウント
 * @return   ボーナス時間ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::IsFarmFertilizerTime(void)
{
  return downTimeCountchk( mData.mFarmFertilizerTime );
}






//--------------------------------------------------------------
/**
 * @brief    探検時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetAdventureTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mAdventureCount[_HOUR];
  *min = mData.mAdventureCount[_MINITS];
  *sec = mData.mAdventureCount[_SECOND];
}

//--------------------------------------------------------------
/**
 * @brief   探検時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetAdventureTime(u8 hour,u8 min,u8 sec)
{
  mData.mAdventureCount[_HOUR]=hour;
  mData.mAdventureCount[_MINITS]=min;
  mData.mAdventureCount[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    探検時間ダウンカウント
 * @return   探検終了でTRUE
 */
//--------------------------------------------------------------
bool ResortSave::AdventureTimeDownCount(void)
{
  return downTimeCount( mData.mAdventureCount );
}


//--------------------------------------------------------------
/**
 * @brief    トロッコの効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetAdventureBonusTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mAdventureBonusTime[_HOUR];
  *min = mData.mAdventureBonusTime[_MINITS];
  *sec = mData.mAdventureBonusTime[_SECOND];

}


//--------------------------------------------------------------
/**
 * @brief   トロッコの効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetAdventureBonusTime(u8 hour,u8 min,u8 sec)
{
  mData.mAdventureBonusTime[_HOUR]=hour;
  mData.mAdventureBonusTime[_MINITS]=min;
  mData.mAdventureBonusTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    トロッコの効果持続時間ダウンカウント
 * @return   ボーナス時間ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::AdventureBonusTimeDownCount(void)
{
  return downTimeCountBonus( mData.mAdventureBonusTime );
}


//--------------------------------------------------------------
/**
 * @brief    トロッコの効果持続時間ダウンカウント
 * @return   動いていたらtrue
 */
//--------------------------------------------------------------
bool ResortSave::IsAdventureBonusTime(void)
{
  return downTimeCountchk( mData.mAdventureBonusTime );
}

//--------------------------------------------------------------
/**
 * @brief    うんどう装置の効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetGimBonusTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mGimBonusTime[_HOUR];
  *min = mData.mGimBonusTime[_MINITS];
  *sec = mData.mGimBonusTime[_SECOND];

}

//--------------------------------------------------------------
/**
 * @brief   うんどう装置の効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetGimBonusTime(u8 hour,u8 min,u8 sec)
{
  mData.mGimBonusTime[_HOUR]=hour;
  mData.mGimBonusTime[_MINITS]=min;
  mData.mGimBonusTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    GIMのボーナス時間ダウンカウント
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::GimBonusTimeDownCount(void)
{
  return downTimeCountBonus( mData.mGimBonusTime );
}

//--------------------------------------------------------------
/**
 * @brief    GIMのボーナス時間ダウンカウント
 * @return   動いていたらtrue
 */
//--------------------------------------------------------------
bool ResortSave::IsGimBonusTime(void)
{
  return downTimeCountchk( mData.mGimBonusTime );
}


//--------------------------------------------------------------
/**
 * @brief    うんどう装置ののこり時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetGimEndTime(int kind,u8* hour,u8* min,u8* sec)
{
  *hour = mData.mGimEndTime[kind][_HOUR];
  *min = mData.mGimEndTime[kind][_MINITS];
  *sec = mData.mGimEndTime[kind][_SECOND];
}

//--------------------------------------------------------------
/**
 * @brief   うんどう装置ののこり時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetGimEndTime(int kind,u8 hour,u8 min,u8 sec)
{
  mData.mGimEndTime[kind][_HOUR]=hour;
  mData.mGimEndTime[kind][_MINITS]=min;
  mData.mGimEndTime[kind][_SECOND]=0;//sec;
}



//--------------------------------------------------------------
/**
 * @brief    GIMののこり時間ダウンカウント
 * @return   時間が一時間毎にtrueを帰す
 */
//--------------------------------------------------------------
bool ResortSave::GimEndTimeDownCount(int kind)
{
  return downTimeCount( mData.mGimEndTime[kind]);
}


//--------------------------------------------------------------
/**
 * @brief    入浴剤の効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetHotSpaBonusTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mHotSpaBonusTime[_HOUR];
  *min = mData.mHotSpaBonusTime[_MINITS];
  *sec = mData.mHotSpaBonusTime[_SECOND];
}
//--------------------------------------------------------------
/**
 * @brief   入浴剤の効果持続時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetHotSpaBonusTime(u8 hour,u8 min,u8 sec)
{
  mData.mHotSpaBonusTime[_HOUR]=hour;
  mData.mHotSpaBonusTime[_MINITS]=min;
  mData.mHotSpaBonusTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    入浴剤の効果持続ダウンカウント
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::HotSpaBonusTimeDownCount(void)
{
  return downTimeCountBonus( mData.mHotSpaBonusTime );
}


//--------------------------------------------------------------
/**
 * @brief    入浴剤の効果持続
 * @return   動いていたらtrue
 */
//--------------------------------------------------------------
bool ResortSave::IsHotSpaBonusTime(void)
{
  return downTimeCountchk( mData.mHotSpaBonusTime );
}



//--------------------------------------------------------------
/**
 * @brief    共通ダウンカウント
 * @param    時間、分、秒
 * @param    ０時間になったときだけtrueを帰す
 */
//--------------------------------------------------------------
bool ResortSave::downTimeCount(u8* pTimeBuf)
{

  if(pTimeBuf[_MINITS]==0){
    if(pTimeBuf[_HOUR]==0){
    }
    else{
      pTimeBuf[_HOUR]-=1;
      pTimeBuf[_MINITS]=59;
    }
  }
  else{
    pTimeBuf[_MINITS]-=1;
    if(pTimeBuf[_MINITS]==0){
      if(pTimeBuf[_HOUR]==0){
        return true;
      }
    }
  }
  return false;

}

//--------------------------------------------------------------
/**
 * @brief    共通タイマー検査
 * @param    時間、分、秒
 * @return   稼動してるとtrue
 */
//--------------------------------------------------------------
bool ResortSave::downTimeCountchk(u8* pTimeBuf)
{
  if(pTimeBuf[_SECOND]==0){
    if(pTimeBuf[_MINITS]==0){
      if(pTimeBuf[_HOUR]==0){
        return false;
      }
    }
  }
  return true;
}


//--------------------------------------------------------------
/**
 * @brief    共通ダウンカウント
 * @param    時間、分、秒
 * @return   カウントしてるとtrue
 */
//--------------------------------------------------------------
bool ResortSave::downTimeCountBonus(u8* pTimeBuf)
{
  if(pTimeBuf[_MINITS]==0){
    if(pTimeBuf[_HOUR]==0){
      return false;
    }
    else{
      pTimeBuf[_HOUR]-=1;
      pTimeBuf[_MINITS]=59;
    }
  }
  else{
    pTimeBuf[_MINITS]-=1;
  }
  return true;
}




//--------------------------------------------------------------
/**
 * @brief    ふれあいリゾートの時間
 * @param    時間、分、秒
   @return   時間が０ならtrue
 */
//--------------------------------------------------------------

bool ResortSave::FriendShipTimeDownCount(void)
{
  return downTimeCount( mData.mFriendShipTime );
}


//--------------------------------------------------------------
/**
 * @brief    ふれあいリゾートの時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------

void ResortSave::SetFriendShipTime(u8 hour,u8 min,u8 sec)
{
  mData.mFriendShipTime[_HOUR]=hour;
  mData.mFriendShipTime[_MINITS]=min;
  mData.mFriendShipTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    ふれあいリゾートの時間を得る
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetFriendShipTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mFriendShipTime[_HOUR];
  *min = mData.mFriendShipTime[_MINITS];
  *sec = mData.mFriendShipTime[_SECOND];
}

//--------------------------------------------------------------
/**
 * @brief    野生ポケモンカウントの時間
 * @param    時間、分、秒
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::WildPokemonTimeDownCount(void)
{
  return downTimeCount( mData.mWildPokemonTime );
}

//--------------------------------------------------------------
/**
 * @brief    野生ポケモンカウントの時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetWildPokemonTime(u8 hour,u8 min,u8 sec)
{
  mData.mWildPokemonTime[_HOUR] = hour;
  mData.mWildPokemonTime[_MINITS]=min;
  mData.mWildPokemonTime[_SECOND]=0;//sec;
}
#if PM_DEBUG
void ResortSave::GetWildPokemonTime( u8 * hour, u8 * min, u8 * sec )
{
  *hour = mData.mWildPokemonTime[_HOUR];
  *min  = mData.mWildPokemonTime[_MINITS];
  *sec  = mData.mWildPokemonTime[_SECOND];
}
#endif

//--------------------------------------------------------------
/**
 * @brief    温泉ポケモンカウントの時間
 * @param    index
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::HotSpaPokemonTimeDownCount(int index)
{
  GFL_ASSERT(index <RESORT_HOTSPA_KIND_MAX);

  return downTimeCount( mData.mHotSpaPokeTime[index] );
}

//--------------------------------------------------------------
/**
 * @brief    温泉ポケモンカウントの時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetHotSpaPokemonTime(int index, u8 hour,u8 min,u8 sec)
{
  mData.mHotSpaPokeTime[index][_HOUR] = hour;
  mData.mHotSpaPokeTime[index][_MINITS] = min;
  mData.mHotSpaPokeTime[index][_SECOND] = 0;//sec;
}
#if PM_DEBUG
void ResortSave::GetHotSpaPokemonTime( int index, u8 * hour, u8 * min, u8 * sec )
{
  *hour = mData.mHotSpaPokeTime[index][_HOUR];
  *min  = mData.mHotSpaPokeTime[index][_MINITS];
  *sec  = mData.mHotSpaPokeTime[index][_SECOND];
}
#endif


//--------------------------------------------------------------
/**
 * @brief    温泉のぼせカウントの時間
 * @param    index
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::HotSpaNoboseTimeDownCount(int index)
{
  GFL_ASSERT(index <RESORT_HOTSPA_KIND_MAX);

  if( mData.mHotSpaNoboseTime[index][_HOUR] == 0 &&
      mData.mHotSpaNoboseTime[index][_MINITS] == 0 &&
      mData.mHotSpaNoboseTime[index][_SECOND] == 0 )
  {
    return true;
  }
  return downTimeCount( mData.mHotSpaNoboseTime[index] );
}

//--------------------------------------------------------------
/**
 * @brief    温泉のぼせカウントの時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetSpaNoboseTime(int index, u8 hour,u8 min,u8 sec)
{
  mData.mHotSpaNoboseTime[index][_HOUR] = hour;
  mData.mHotSpaNoboseTime[index][_MINITS] = min;
  mData.mHotSpaNoboseTime[index][_SECOND] = 0;//sec;
}
#if PM_DEBUG
void ResortSave::GetSpaNoboseTime( int index, u8 * hour, u8 * min, u8 * sec )
{
  *hour = mData.mHotSpaNoboseTime[index][_HOUR];
  *min  = mData.mHotSpaNoboseTime[index][_MINITS];
  *sec  = mData.mHotSpaNoboseTime[index][_SECOND];
}
#endif


//--------------------------------------------------------------
/**
 * @brief Pokemonを得る
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetHotSpaPokemon(int index, pml::pokepara::CoreParam * cp)
{
  if(index < STOCKPOKEMON_MAX)
  {
    cp->Deserialize_Core( mData.mHotSpaPokeStock[index].pokemonCoreParam );
  }
  else
  {
    cp->Clear();
  }
}

//更新フラグを得る
bool ResortSave::IsNewHotSpaPokemon(int index)
{
  return mData.mHotSpaPokeStock[index].bnew;
}
void ResortSave::ResetNewHotSpaPokemon(int index)
{
  mData.mHotSpaPokeStock[index].bnew = false;;
}


//--------------------------------------------------------------
/**
 * @brief Pokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetHotSpaPokemon(int index, const pml::pokepara::CoreParam * cp, bool bNew)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  cp->Serialize_Core( mData.mHotSpaPokeStock[index].pokemonCoreParam );
  if( bNew ) mData.mHotSpaPokeStock[index].bnew=true;
  mData.updateFlg=true;
}

//--------------------------------------------------------------
/**
 * @brief GimのPokemonを得る
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetGimPokemon(int index, pml::pokepara::CoreParam * cp)
{
  if(index < STOCKPOKEMON_MAX)
  {
    cp->Deserialize_Core( mData.mGimPokeStock[index].pokemonCoreParam );
  }
  else
  {
    cp->Clear();
  }

}

//--------------------------------------------------------------
/**
 * @brief GimのPokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetGimPokemon(int index, const pml::pokepara::CoreParam * cp)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);

  cp->Serialize_Core( mData.mGimPokeStock[index].pokemonCoreParam );
  mData.updateFlg=true;
}



//--------------------------------------------------------------
/**
 * @brief FarmのPokemonを得る
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetFarmPokemon(int index, pml::pokepara::CoreParam * cp)
{
  if(index < STOCKPOKEMON_MAX)
  {
    cp->Deserialize_Core( mData.mFarmPokeStock[index].pokemonCoreParam );
  }
  else{
    cp->Clear();
  }
}

//--------------------------------------------------------------
/**
 * @brief FarmのPokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetFarmPokemon(int index, const pml::pokepara::CoreParam * cp,int tray,int pos)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  cp->Serialize_Core( mData.mFarmPokeStock[index].pokemonCoreParam );
  if(tray!=-1){
    mData.mFarmPokeStock[index].tray=tray;
    mData.mFarmPokeStock[index].pos=pos;
  }
  mData.updateFlg=true;
}


//--------------------------------------------------------------
/**
 * @brief FarmのPokemonのボックストレイ位置を帰す
 * @return 上位８トレイ 下位８場所
 */
//--------------------------------------------------------------
u16 ResortSave::GetFarmPokemonBoxPos(int index)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  return mData.mFarmPokeStock[index].tray << 8 | mData.mFarmPokeStock[index].pos;
}




//--------------------------------------------------------------
/**
 * @brief たんけんのPokemonを得る
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetAdventurePokemon(int index, pml::pokepara::CoreParam * cp)
{
  if(index < STOCKPOKEMON_MAX)
  {
    cp->Deserialize_Core( mData.mAdventurePokeStock[index].pokemonCoreParam );
  }
  else{
    cp->Clear();
  }
}

//--------------------------------------------------------------
/**
 * @brief たんけんのPokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetAdventurePokemon(int index, const pml::pokepara::CoreParam * cp,int tray,int pos)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  cp->Serialize_Core( mData.mAdventurePokeStock[index].pokemonCoreParam );
  if(tray!=-1){
    mData.mAdventurePokeStock[index].tray=tray;
    mData.mAdventurePokeStock[index].pos=pos;
  }
  mData.updateFlg=true;
}


//--------------------------------------------------------------
/**
 * @brief たんけんのPokemonのボックストレイ位置を帰す
 * @return 上位８トレイ 下位８場所
 */
//--------------------------------------------------------------
u16 ResortSave::GetAdventurePokemonBoxPos(int index)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  return mData.mAdventurePokeStock[index].tray << 8 | mData.mAdventurePokeStock[index].pos;
}






//--------------------------------------------------------------
/**
 * @brief FriendShipのPokemonを得る
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::GetFriendShipPokemon(int index, pml::pokepara::CoreParam * cp)
{
  if(index < STOCKPOKEMON_MAX)
  {
    cp->Deserialize_Core( mData.mFriendShipPokeStock[index].pokemonCoreParam );
  }
  else{
    cp->Clear();
  }

}

//--------------------------------------------------------------
/**
 * @brief FriendShipのPokemonをセットする
 * @param PokemonCoreParam
 */
//--------------------------------------------------------------
void ResortSave::SetFriendShipPokemon(int index, const pml::pokepara::CoreParam * cp, int tray, int pos)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  cp->Serialize_Core( mData.mFriendShipPokeStock[index].pokemonCoreParam );
  if(tray!=-1){
    mData.mFriendShipPokeStock[index].tray=tray;
    mData.mFriendShipPokeStock[index].pos=pos;
  }
  mData.updateFlg=true;
}


//--------------------------------------------------------------
/**
 * @brief FriendShipのPokemonのボックストレイ位置を帰す
 * @return 上位８トレイ 下位８場所
 */
//--------------------------------------------------------------
u16 ResortSave::GetFriendShipPokemonBoxPos(int index)
{
  GFL_ASSERT(index < STOCKPOKEMON_MAX);
  return mData.mFriendShipPokeStock[index].tray << 8 | mData.mFriendShipPokeStock[index].pos;
}


//--------------------------------------------------------------
/**
 * @brief    野生ポケモン抽選確率２倍の時間を得る
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetWildPokemonServiceTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mWildPokemonServiceTime[_HOUR];
  *min = mData.mWildPokemonServiceTime[_MINITS];
  *sec = mData.mWildPokemonServiceTime[_SECOND];
}

//--------------------------------------------------------------
/**
 * @brief    野生ポケモン抽選確率２倍の時間設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetWildPokemonServiceTime(u8 hour,u8 min,u8 sec)
{
  mData.mWildPokemonServiceTime[_HOUR]=hour;
  mData.mWildPokemonServiceTime[_MINITS]=min;
  mData.mWildPokemonServiceTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    野生ポケモン抽選確率２倍の時間
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::WildPokemonServiceTimeDownCount(void)
{
  return downTimeCountBonus( mData.mWildPokemonServiceTime );
}


//--------------------------------------------------------------
/**
 * @brief    野生ポケモン抽選確率２倍の時間
 * @return   時間が０ならfalse 動いてたらtrue
 */
//--------------------------------------------------------------
bool ResortSave::IsWildPokemonServiceTime(void)
{
  return downTimeCountchk( mData.mWildPokemonServiceTime );
}





//--------------------------------------------------------------
/**
 * @brief    まめびんの時間を得る
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::GetBeansBottleServiceTime(u8* hour,u8* min,u8* sec)
{
  *hour = mData.mBeansBottleTime[_HOUR];
  *min = mData.mBeansBottleTime[_MINITS];
  *sec = mData.mBeansBottleTime[_SECOND];
}

//--------------------------------------------------------------
/**
 * @brief    まめびんの時間設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortSave::SetBeansBottleServiceTime(u8 hour,u8 min,u8 sec)
{
  mData.mBeansBottleTime[_HOUR]=hour;
  mData.mBeansBottleTime[_MINITS]=min;
  mData.mBeansBottleTime[_SECOND]=0;//sec;
}

//--------------------------------------------------------------
/**
 * @brief    まめびんの時間ダウンカウント
 * @return   時間が０ならtrue
 */
//--------------------------------------------------------------
bool ResortSave::BeansBottleServiceTimeDownCount(void)
{
  return downTimeCount( mData.mBeansBottleTime );
}




//--------------------------------------------------------------
/**
 * @brief   リゾートからサークルに豆ビンが使えるかどうかを得る
 * @return  使える場合TRUE
 */
//--------------------------------------------------------------
bool ResortSave::GetUseBeansBottle(void)
{
  return mData.mbUseBeansBottle;
}

//--------------------------------------------------------------
/**
 * @brief   リゾートからサークルに豆ビンが使えるかどうか設定
 * @param  使える場合TRUE 使えなくなったらFALSE
 */
//--------------------------------------------------------------
void ResortSave::SetUseBeansBottle(bool onoff)
{
  mData.mbUseBeansBottle = onoff;
}



//--------------------------------------------------------------
/**
 * @brief
 * @return
 */
//--------------------------------------------------------------
bool ResortSave::IsBeansBottleGround(PokeResortKind type)
{
  if(GetResortLevel(type) == 0){
    return false;
  }
  if( mData.mBeansBottleState[type] ){
    return true;
  }
  return false;
}


//--------------------------------------------------------------
/**
 * @brief
 * @return
 */
//--------------------------------------------------------------
void ResortSave::SetBeansBottleGround(PokeResortKind type,  bool onoff)
{
  if(GetResortLevel(type) == 0){
    return;
  }
  mData.mBeansBottleState[type] = onoff;

}


//--------------------------------------------------------------
/**
 * @brief    まめびんをもう入手したかどうかを得る
 * @return   とられたらtrue とる前はfalse
 */
//--------------------------------------------------------------
bool ResortSave::IsBeansBottleGet(PokeResortKind type)
{
  if(GetResortLevel(type) == 0){
    return false;
  }
  return mData.mBeansBottleState[type];
}


//--------------------------------------------------------------
/**
 * @brief    まめびんをもう入手したかどうかを設定する
 * @return   とられたらtrue とる前はfalse
 */
//--------------------------------------------------------------
void ResortSave::SetBeansBottleGet(PokeResortKind type,bool onoff)
{

  if(GetResortLevel(type) == 0){
    return;
  }
  mData.mBeansBottleState[type] = onoff;

}



//--------------------------------------------------------------
/**
 * @brief リゾートのレベルを設定する
 * @param PokeBeansEnum
 */
//--------------------------------------------------------------
void ResortSave::SetResortLevel(PokeResortKind kind,u8 level)
{
  GFL_ASSERT(level <= 3);
  mData.mResortLevel[kind] = level;
}


//--------------------------------------------------------------
/**
 * @brief    探検で得たアイテム
 * @return   アイテム番号  
 */
//--------------------------------------------------------------
u16 ResortSave::GetAdventureItem(int index)
{
  GFL_ASSERT(index < ADVENTURE_ITEM_MAX);
  return mData.mItemBuff[index];
}

//--------------------------------------------------------------
/**
 * @brief   探検で得たアイテム セット
 * @param    格納番号
 * @param    アイテム番号
 */
//--------------------------------------------------------------
void ResortSave::SetAdventureItem(int index,u16 item)
{
  GFL_ASSERT(index < ADVENTURE_ITEM_MAX);
  mData.mItemBuff[index]=item;
}

//--------------------------------------------------------------
/**
 * @brief    探検で得たアイテム 追加  入らないものは入らない
 * @param    アイテム番号
 */
//--------------------------------------------------------------
void ResortSave::AddAdventureItem(u16 item, int max)
{
  int i;

  for(i=0;((i<ADVENTURE_ITEM_MAX) && (i < max)) ;i++){
    if(mData.mItemBuff[i] == ITEM_DUMMY_DATA){
      mData.mItemBuff[i] = item;
      break;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief    探検で得たアイテムが何個入っているか得る
 * @return   数
 */
//--------------------------------------------------------------
int ResortSave::GetTreasureItemNum(void)
{
  int i,j=0;
  
  for(i=0;i<ADVENTURE_ITEM_MAX;i++){
    if(mData.mItemBuff[i] != ITEM_DUMMY_DATA){
      j++;
    }
  }
  return j;
}


//--------------------------------------------------------------
/**
 * @brief   ボーナスのレベル
 * @return  ボーナスタイムの場合TRUE
 */
//--------------------------------------------------------------
u8 ResortSave::GetBonusLevel(void)
{
  return mData.mBounusLevel;
}
//--------------------------------------------------------------
/**
 * @brief   ボーナスのレベル
 * @param   ボーナスタイムの場合TRUE
 */
//--------------------------------------------------------------
void ResortSave::SetBonusLevel(u8 level)
{
  mData.mBounusLevel = level;
}


//--------------------------------------------------------------
/**
 * @brief リゾートの進行具合を得る チュートリアル終わったかとか
 * @return 終わっていたらtrue 終わってないとfalse
 */
//--------------------------------------------------------------

bool ResortSave::GetResortProgress(ResortTutorialState kind)
{
  return (mData.mEventProgress & kind);
}


//--------------------------------------------------------------
/**
 * @brief リゾートの進行具合を設定する
 * @param kind  進行度
 */
//--------------------------------------------------------------
void ResortSave::SetResortProgress(ResortTutorialState kind)
{
  mData.mEventProgress = mData.mEventProgress | kind;
}


//--------------------------------------------------------------
/**
 * @brief あずけているポケモンの数を数える たまごかぞえない NMCat1711
 * @param あずけている数
 */
//--------------------------------------------------------------

int ResortSave::PokemonCountHotSpaAndGim(pml::pokepara::CoreParam * cp)
{
  int count=0,i;
  
  for(i=0;i<STOCKPOKEMON_MAX;i++){
    GetHotSpaPokemon(i,cp);
    if(!cp->IsNull()){
      if(!cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
        count++;
      }
    }
  }
  for(i=0;i<STOCKPOKEMON_MAX;i++){
    GetGimPokemon(i,cp);
    if(!cp->IsNull()){
      if(!cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
        count++;
      }
    }
  }
  return count;
}


bool ResortSave::CheckUpdateFlg(void)
{
  bool updatef = mData.updateFlg;
  mData.updateFlg=false;
  return updatef;
}

void ResortSave::SetUpdateFlg(void)
{
  mData.updateFlg = true;
}


//--------------------------------------------------------------
/**
 * @brief 温泉リゾートのPokemonをPokeParty形式でセットする
 * @param PokeParty
 */
//--------------------------------------------------------------
void ResortSave::SetPokePartyToHotSpaPokemon( u32 groupno, const pml::PokeParty * pParty,gfl2::heap::HeapBase* heap)
{
  u32 i=0;
  GFL_ASSERT(groupno<3);
  
  for( i=0; i<pParty->GetMemberCount(); i++ ){
    const pml::pokepara::PokemonParam * pp = pParty->GetMemberPointerConst( i );
    SetHotSpaPokemon( groupno*6+ i,pp);
  }
  pml::pokepara::CoreParam* cp = GFL_NEW( heap->GetLowerHandle() ) pml::pokepara::CoreParam( heap->GetLowerHandle() );
  cp->Clear();
  for( ; i < pml::PokeParty::MAX_MEMBERS; i++ ){
    SetHotSpaPokemon(groupno*6+i,cp);
  }
  GFL_DELETE cp;
}

//--------------------------------------------------------------
/**
 * @brief 運動リゾートのPokemonをPokeParty形式でセットする
 * @param PokeParty
 */
//--------------------------------------------------------------
void ResortSave::SetPokePartyToGimPokemon( u32 groupno,const pml::PokeParty * pParty,gfl2::heap::HeapBase* heap)
{
  u32 i=0;
  GFL_ASSERT(groupno<3);
  for( i=0; i<pParty->GetMemberCount(); i++ ){
    const pml::pokepara::PokemonParam * pp = pParty->GetMemberPointerConst( i );
		SetGimPokemon(groupno*6+i,pp);
  }
  pml::pokepara::CoreParam* cp = GFL_NEW( heap->GetLowerHandle() ) pml::pokepara::CoreParam( heap->GetLowerHandle() );
  cp->Clear();
  for( ; i < pml::PokeParty::MAX_MEMBERS; i++ ){
    SetGimPokemon(groupno*6+i,cp);
  }
  GFL_DELETE cp;
}


//--------------------------------------------------------------
/**
 * @brief 運動リゾート開始時の値をセット
 * @param index   インデックス
 * @param val     セットする値
 */
//--------------------------------------------------------------
void ResortSave::SetGimStartValue( u8 index, u8 val )
{
  mData.mGimAddNum[index] = val;
}

//--------------------------------------------------------------
/**
 * @brief 運動リゾート開始時の値を取得
 * @param index   インデックス
 * @return  セットされた値
 */
//--------------------------------------------------------------
u8 ResortSave::GetGimStartValue( u8 index )
{
  return mData.mGimAddNum[index];
}


//--------------------------------------------------------------
/**
 * @brief 運動リゾートで上昇する努力値のIDを取得
 * @param kind  種類
 * @return  努力値ID
 */
//--------------------------------------------------------------
pml::pokepara::PowerID ResortSave::GetGimKindPowerID( PokeResortGimKind kind )
{
  if( kind == RESORT_GIM_KIND_LV )
  {
    GFL_ASSERT( 0 );
    return pml::pokepara::POWER_HP;
  }

  static const pml::pokepara::PowerID eff[] =
  {
    pml::pokepara::POWER_HP,
    pml::pokepara::POWER_ATK,
    pml::pokepara::POWER_DEF,
    pml::pokepara::POWER_SPATK,
    pml::pokepara::POWER_SPDEF,
    pml::pokepara::POWER_AGI,
  };

  return eff[kind-RESORT_GIM_KIND_HP];
}


//--------------------------------------------------------------
/**
 * @brief まめのきを叩いた回数を得る
 */
//--------------------------------------------------------------
u8 ResortSave::GetBeansIvyHiddenCount(void)
{
  return mData.beansIvyHiddenCount;
}

//--------------------------------------------------------------
/**
 * @brief まめのきを叩いた回数をふやす
   */
  //--------------------------------------------------------------
void ResortSave::AddBeansIvyHiddenCount(u8 max)
{

  if(mData.beansIvyHiddenCount < max){
    mData.beansIvyHiddenCount++;
  }
}

//--------------------------------------------------------------
/**
 * @brief まめのきを叩いた回数を０にする
 */
//--------------------------------------------------------------
void ResortSave::ResetBeansIvyHiddenCount(void)
{
  mData.beansIvyHiddenCount=0;
}


//--------------------------------------------------------------
/**
 * @brief    まめのきかくしタイマーの時間を減算する
 * @param    時間、分、秒
   @return   時間が０ならtrue
 */
//--------------------------------------------------------------

bool ResortSave::SetBeansIvyHiddenTimeDownCount(void)
{
  return downTimeCount( mData.beansIvyHiddenTime );
}


//--------------------------------------------------------------
/**
 * @brief    まめのきかくしタイマーの時間を設定
 * @param    時間、分、秒
 */
//--------------------------------------------------------------

void ResortSave::SetBeansIvyHiddenTime(u8 hour,u8 min,u8 sec)
{
  mData.beansIvyHiddenTime[_HOUR]=hour;
  mData.beansIvyHiddenTime[_MINITS]=min;
  mData.beansIvyHiddenTime[_SECOND]=0;//sec;
}



//--------------------------------------------------------------
// 温泉ポケモン設定処理
//--------------------------------------------------------------
void ResortSave::ResultBoxHotSpa(gfl2::heap::HeapBase* heap , int groupno )
{
  // のぼせタイマー初期化
  static const u8 NOBOSE_START_TIME_H = 24;
  static const u8 NOBOSE_START_TIME_M = 0;
  static const u8 NOBOSE_START_TIME_S = 0;
  SetSpaNoboseTime( groupno, NOBOSE_START_TIME_H, NOBOSE_START_TIME_M, NOBOSE_START_TIME_S );

  // イベントを変更
  u32 start = pml::PokeParty::MAX_MEMBERS * groupno;

  pml::pokepara::CoreParam * cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    GetHotSpaPokemon( start+i, cp );
    if( cp->IsNull() == false && cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false )
    {
      cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_HOTSPA_STATE );
      SetHotSpaPokemon( start+i, cp, false );
    }
  }

  GFL_DELETE cp;
}


//--------------------------------------------------------------
// 運動ポケモン設定処理
//--------------------------------------------------------------
void ResortSave::ResultBoxGim(gfl2::heap::HeapBase* heap , int groupno, bool binbox )
{

  // イベントを変更
  u32 start = pml::PokeParty::MAX_MEMBERS * groupno;
  pml::pokepara::CoreParam * cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    GetGimPokemon( start+i, cp );
    if( cp->IsNull() == false && cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false )
    {
      if(binbox){
        cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_PLAY_STATE );
      }
      else{
        cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_NONE );
      }
      SetGimPokemon( start+i, cp );
    }
  }

  GFL_DELETE cp;
}



void ResortSave::ResetTime(void)
{
  GameSys::DeviceDate date;
  SetPlayTime(&date);  //時間の経過が無かった扱いになる
  mData.rtcoffset1 = static_cast<u32>(_rtcoffset); //先ほどの値を再度保存
	mData.rtcoffset2 = _rtcoffset >> 32; //先ほどの値を再度保存


  OHNO_PRINT("ResortSave::ResetTime %llx  : %x - %x\n", _rtcoffset, mData.rtcoffset1, mData.rtcoffset2);

}

bool ResortSave::isRTCOffset(void)
{
	u64 rtco = mData.rtcoffset2;
  bool retflg=false;

	rtco = (rtco << 32) & 0xffffffff00000000;
	rtco += mData.rtcoffset1;

  if( rtco == _rtcoffset){
    retflg = true;
  }
  OHNO_PRINT("ResortSave::isRTCOffset %llx = %llx : %x - %x >> %d\n", rtco, _rtcoffset, mData.rtcoffset1, mData.rtcoffset2, retflg);
  return retflg;
}


void ResortSave::ResortRTCCheckFunc(void)
{
  if(isRTCOffset()==false){
    ResetTime();
  }
}




#if PM_DEBUG
//デバッグ用 チュートリアルフラグ設定
void ResortSave::DebugSetResortProgress(u32 data)
{
  mData.mEventProgress = data;
}

#endif

GFL_NAMESPACE_END(Savedata)

