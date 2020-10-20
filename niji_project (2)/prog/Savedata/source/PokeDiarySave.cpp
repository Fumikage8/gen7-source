//==============================================================================
/**
 * @file	  PokeDiarySave.cpp
 * @brief   ポケモンダイアリー用セーブデータ
 * @date    2012.09.28
 * @author  tamada
 */
// =============================================================================
#include "Savedata/include/PokeDiarySave.h"
#include "Savedata/include/MyStatus.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/PokemonVersion.h"

#include <debug/include/gfl2_DebugPrint.h>
#include  <debug/include/gfl2_Assert.h>

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

// niji互換対応ポケモンチェック
#include "Battle/Regulation/include/PokeRegulation.h"

//=============================================================================
//=============================================================================
#if defined(DEBUG_ONLY_FOR_tamada) || defined(DEBUG_ONLY_FOR_kawachimaru_takeshi)
#define POKEDIARY_PRINT( ... )  GFL_PRINT(__VA_ARGS__)
#else
#define POKEDIARY_PRINT( ... )  /* do nothing */
#endif

GFL_NAMESPACE_BEGIN(PokeDiary)

//static xy_system::dress_up::SaveFashionBuffer temp_buffer;

//--------------------------------------------------------------------------
/**
 * @brief   ポケモンダイアリーをセットする
 * @param[in] pdid  ポケモンダイアリーイベントID
 * @param[in] p1    パラメータ１
 * @param[in] p2    パラメータ２
 * @param[in] p3    パラメータ３
 */
//--------------------------------------------------------------------------
void Set( PDID pdid, u16 p1, u16 p2, u16 p3 )
{
/*NN_COMPILER_ASSERT(
    sizeof(Savedata::PokeDiarySave::DIARY_FASHION_DATA)
    == sizeof(xy_system::dress_up::SaveFashionBuffer) );*/

  //グローバルにGameDataを取得、操作する
  GameSys::GameData * p_gamedata = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  if ( p_gamedata == NULL ) return;
  Savedata::PokeDiarySave * p_dsv = p_gamedata->GetPokeDiarySave();
  if ( p_dsv == NULL ) return;

  GameSys::DeviceDate now;

  Savedata::PokeDiarySave::DIARY_DATE date;
  date.year     = now.GetYear() - 2000;
  date.month    = now.GetMonth();
  date.day      = now.GetDay();
  date.hour     = now.GetHour();
  date.minute   = now.GetMinute();
  date.half_sec = now.GetSecond() / 2;

  {
    u8 year = date.year;
    u8 month = date.month;
    u8 day = date.day;
    u8 hour = date.hour;
    u8 minute = date.minute;
    u8 half_sec = date.half_sec;
    GFL_PRINT( "%4d/%2d/%2d %2d:%2d:%2d \n",
        year, month, day, hour, minute, half_sec );
  }

  if ( pdid < PDID_PERSONAL_NUM )
  {
    //p_gamedata->GetPlayerStatus()->GetDressUpData( &temp_buffer );
    //p_dsv->SetPersonal( pdid, p1, p2, p3, date,
    //    reinterpret_cast<const void*>(&temp_buffer));
    return;
  }
  if ( pdid < PDID_LIVE_NUM )
  {
    p_dsv->SetLive( static_cast<PDID>(pdid - PDID_LIVE_NULL), p1, p2, p3, date );
    return;
  }
  if ( pdid < PDID_COUNT_NUM )
  {
    p_dsv->SetCount( static_cast<PDID>(pdid - PDID_COUNT_NULL), p1, p2, p3, date );
    return;
  }
  GFL_ASSERT( 0 );  //指定範囲外の値が来た
  return;
}



//--------------------------------------------------------------------------
/**
 * @brief   ポケモンダイアリーのパーソナルの状態を取得
 */
//--------------------------------------------------------------------------
bool GetPersonal( PDID pdid )
{
  if ( pdid < PDID_PERSONAL_NUM )
  {
    if ( pdid <= PDID_NULL || pdid >= PDID_PERSONAL_NUM )
    {
      GFL_ASSERT( 0 );
      return false;
    }

    //グローバルにGameDataを取得、操作する
    GameSys::GameData * p_gamedata = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
    if ( p_gamedata == NULL ) return false;
    Savedata::PokeDiarySave * p_dsv = p_gamedata->GetPokeDiarySave();
    if ( p_dsv == NULL ) return false;

    Savedata::PokeDiarySave::CoreData* pCore = (Savedata::PokeDiarySave::CoreData*)p_dsv->GetData();
    Savedata::PokeDiarySave::DIARY_DATA * dpd = &(pCore->DiaryPersonalData[ pdid - 1 ]);
    if ( dpd->diary_id == pdid )
    {
      return true;
    }
  }

  return false;
}

GFL_NAMESPACE_END(PokeDiary)




GFL_NAMESPACE_BEGIN(Savedata)




//=============================================================================
//=============================================================================
//--------------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------------
PokeDiarySave::PokeDiarySave( void )
{
	this->Clear( NULL );
}

//--------------------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------------------
PokeDiarySave::~PokeDiarySave( void )
{
}

//--------------------------------------------------------------------------
/**
 * @brief		読み込んだデータをセットする関数
 * @param		pData		先頭のポインタ
 */
//--------------------------------------------------------------------------
void PokeDiarySave::SetData(void* data)
{
	gfl2::std::MemCopy(data, &m_data, sizeof(m_data) );
}
//--------------------------------------------------------------------------
/**
 * @brief    データの先頭ポインタを得る関数
 * @return   先頭のポインタ
 */
//--------------------------------------------------------------------------
void* PokeDiarySave::GetData(void)
{
	return reinterpret_cast<void*>(&m_data);
}
//--------------------------------------------------------------------------
/**
 * @brief    データサイズ
 * @retval   バイト
 */
//--------------------------------------------------------------------------
size_t PokeDiarySave::GetDataSize(void)
{
  return sizeof( CoreData );
}

//--------------------------------------------------------------------------
/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ
 */
//--------------------------------------------------------------------------
void PokeDiarySave::Clear( gfl2::heap::HeapBase * heap )
{
  gfl2::std::MemClear( &m_data, GetDataSize() );
	GFL_UNUSED(heap);
}

//--------------------------------------------------------------------------
/**
 * @brief   レイログ作成
 *          （最新のデータを入れる）
 * @param[out] outDiaryData    ダイアリーデータ
 * @return trueで取得成功
 */
//--------------------------------------------------------------------------
void PokeDiarySave::CreatePlayLog( DIARY_DATA pOutDiaryData[] )
{
  u16 lastDiaryId[ LAST_DIARY_MAX ];  //! 最新のデータ3件
  u8  addDiaryCount = 0;

  for( int i=0; i<LAST_DIARY_MAX; i++ )
  {
    lastDiaryId[i] = PDID_NULL;
  }

  // 配列には古い順で入っているので、後ろから取れば最新
  for( int i=DIARYTYPE_COUNT_MAX-1 ; i>=0; i-- )
  {
    const u16 targetPdid = m_data.DiaryCountData[i].diary_id;
    ICHI_PRINT(">ICHI セーブDiary[%d] = %d \n", i, targetPdid );

    if( targetPdid != PDID_NULL )
    {
      bool isSame = false;

      // 重複チェック
      for( int j=0; j<LAST_DIARY_MAX; j++ )
      {
        if( lastDiaryId[ j ] ==  targetPdid )
        {
          ICHI_PRINT("> ダイアリー履歴：重複していたので保存しない\n");
          isSame = true;
          break;
        }
      }

      // 重複していなかったら登録
      if( isSame == false )
      {
        lastDiaryId[ addDiaryCount++ ] = targetPdid;

        // 3件登録した時点で終了
        if( addDiaryCount >= LAST_DIARY_MAX )
        {
          break;
        }
      }
    }
  }

  // 戻り値に結果格納
  for( int i=0; i<addDiaryCount; i++ )
  {
    // @fix GFNMCat[1917]：PDIDがPDID_COUNT_NULLからのIDになっていない為、PDID_LIVEと見なされていた
    PDID pdid = static_cast<PDID>( lastDiaryId[i] + PDID_COUNT_NULL );
    Get( pdid, true, &pOutDiaryData[i] );

    // @fix MMCat[223] レコードのパラメータの中にmomiji新ポケモンが含まれていないかチェック
    if( CheckDiaryParamMomijiPoke( &pOutDiaryData[i] ) )
    {
      ClearSingleData( &pOutDiaryData[i] );
      GFL_RELEASE_PRINT( "System233対処 Diary[%d] = %d DIARY削除 \n", i, pdid );
    }

    ICHI_PRINT(">ICHI 登録Diary[%d] = %d \n", i, pdid );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケモンダイアリーを取得する
 * @param[in] pdid  ポケモンダイアリーイベントID
 * @param[out] outDiaryData    ダイアリーデータ
 * @return trueで取得成功
 */
//--------------------------------------------------------------------------
bool PokeDiarySave::Get( PDID pdid, bool isReverse, DIARY_DATA* pOutDiaryData )
{
  if ( pdid < PDID_PERSONAL_NUM )
  {
    return false;
  }
  if ( pdid < PDID_LIVE_NUM )
  {
    return false;
  }
  if ( pdid < PDID_COUNT_NUM )
  {
    DIARY_DATA * dcd = NULL;
    if( isReverse == false )
    {
      // 通常(=古い方)からデータ取得
      dcd = GetPos(m_data.DiaryCountData, static_cast<PDID>(pdid - PDID_COUNT_NULL), DIARYTYPE_COUNT_MAX );
    }
    else
    {
      // 逆(=最新)からデータ取得
      dcd = GetPosReverse(m_data.DiaryCountData, static_cast<PDID>(pdid - PDID_COUNT_NULL), DIARYTYPE_COUNT_MAX );
    }

    if( dcd != NULL && pOutDiaryData != NULL )
    {
      *pOutDiaryData = *dcd;
      return true;
    }
  }
  return false;
}

#if PM_DEBUG
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void PokeDiarySave::Dump( PokeDiarySave::DiaryType type )
{
  const DIARY_DATA * dd;
  int num = 0;
  switch ( type )
  {
  case DIARYTYPE_PERSONAL:
    dd = m_data.DiaryPersonalData;
    num = DIARYTYPE_PERSONAL_MAX;
    break;
  case DIARYTYPE_LIVE:
    dd = m_data.DiaryLiveData;
    num = DIARYTYPE_LIVE_MAX;
    break;
  case DIARYTYPE_COUNT:
    dd = m_data.DiaryCountData;
    num = DIARYTYPE_COUNT_MAX;
    break;
  default:
    GFL_ASSERT(0);
    return;
  }
  for ( int i = 0; i < num; ++ i, ++ dd )
  {
    u8 year = dd->date.year;
    u8 month = dd->date.month;
    u8 day = dd->date.day;
    u8 hour = dd->date.hour;
    u8 minute = dd->date.minute;
    u8 half_sec = dd->date.half_sec;
    GFL_PRINT( "%4d/%2d/%2d %2d:%2d:%2d",
        year, month, day, hour, minute, half_sec );
    GFL_PRINT(
        " ID(%3d)TYPE(%d):ROM(%2d):DATA(%3d,%3d,%3d)\n",
        dd->diary_id, dd->diary_type, dd->rom_code,
        dd->data[0], dd->data[1], dd->data[2] );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief ポケダイアリーのデータを文字に格納する
 * @param outStr  格納先ポインタ
 * @param type    データタイプ
 * @param index   データのインデックス(0オリジン)
 */
//--------------------------------------------------------------------------
void PokeDiarySave::SetStr( wchar_t* outStr, DiaryType type, int index )
{
  const DIARY_DATA * dd;
  switch ( type )
  {
  case DIARYTYPE_PERSONAL:
    dd = m_data.DiaryPersonalData;
    if( index < DIARYTYPE_PERSONAL_MAX )
    {
      dd += index;
    }
    break;
  case DIARYTYPE_LIVE:
    dd = m_data.DiaryLiveData;
    if( index < DIARYTYPE_LIVE_MAX )
    {
      dd += index;
    }
    break;
  case DIARYTYPE_COUNT:
    dd = m_data.DiaryCountData;
    if( index < DIARYTYPE_COUNT_MAX )
    {
      dd += index;
    }
    break;
  default:
    GFL_ASSERT( 0 );
    return;
  }
  {
    u8 year = dd->date.year;
    u8 month = dd->date.month;
    u8 day = dd->date.day;
    u8 hour = dd->date.hour;
    u8 minute = dd->date.minute;
    u8 half_sec = dd->date.half_sec;

    swprintf( outStr, gfl2::debug::DEBUGWIN_NAME_LEN,
              L"%2d/%2d/%2d %2d:%2d:%2d ID%2d R%2d %d,%d,%d",
        year, month, day, hour, minute, half_sec,
        dd->diary_id, dd->rom_code,
        dd->data[0], dd->data[1], dd->data[2] );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief ポケダイアリーのデータを追加する
 * @param type  データタイプ 0オリジン 定義は1オリジンなので+1必要あり
 * @param id    ID
 * @param rom   ROMコード
 * @param data  データ3つ
 */
//--------------------------------------------------------------------------
void PokeDiarySave::SetDebugData( DiaryType type, PDID id, int rom, int* data )
{
  ICHI_PRINT("type:%d id:%d rom:%d ", type, id, rom);
  ICHI_PRINT("data:%d %d %d \n", data[0], data[1], data[2]);

  // ROMコードを書き込む瞬間だけ書き換える
  u8 version = GET_VERSION();
  System::SetVersion( static_cast<u8>(rom) );

  PokeDiary::Set( static_cast<PDID>(id + PDID_COUNT_NULL), data[0], data[1], data[2] );

  // ROMコードを戻す
  System::SetVersion( version );
}

#endif
//=============================================================================
//=============================================================================
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void PokeDiarySave::SetPersonal( PDID pdid, u16 p1, u16 p2, u16 p3, PokeDiarySave::DIARY_DATE date, const void * fashion )
{
  if ( pdid <= PDID_NULL || pdid >= PDID_PERSONAL_NUM )
  {
    GFL_ASSERT( 0 );
    return;
  }
  DIARY_DATA * dpd = &this->m_data.DiaryPersonalData[ pdid - 1 ]; //1 org -> 0 org
  //上書き禁止
  if ( dpd->diary_id == 0 )
  {
    dpd->rom_code   = GET_VERSION();
    dpd->diary_type = DIARYTYPE_PERSONAL;
    dpd->diary_id   = pdid;
    dpd->data[0]    = p1;
    dpd->data[1]    = p2;
    dpd->data[2]    = p3;
    dpd->date       = date;

    gfl2::std::MemCopy( fashion, &m_data.DiaryFashion[ pdid - 1 ], sizeof(DIARY_FASHION_DATA) );
    POKEDIARY_PRINT("PokeDiary::SetPersonal(%d,%d,%d,%d)\n", pdid, p1, p2, p3);
  }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void PokeDiarySave::SetLive( PDID pdid, u16 p1, u16 p2, u16 p3, PokeDiarySave::DIARY_DATE date )
{
  DIARY_DATA * dld = SortAndGetPos(m_data.DiaryLiveData, DIARYTYPE_LIVE_MAX );
  ClearSingleData( dld );

  dld->rom_code   = GET_VERSION();
  dld->diary_type = DIARYTYPE_LIVE;
  dld->diary_id   = pdid;
  dld->data[0]    = p1;
  dld->data[1]    = p2;
  dld->data[2]    = p3;
  dld->date       = date;
  POKEDIARY_PRINT("PokeDiary::SetLive(%d,%d,%d,%d)\n", pdid, p1, p2, p3);
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void PokeDiarySave::SetCount( PDID pdid, u16 p1, u16 p2, u16 p3, PokeDiarySave::DIARY_DATE date )
{
  DIARY_DATA * dcd = SortAndGetPos(m_data.DiaryCountData, DIARYTYPE_COUNT_MAX );
  ClearSingleData( dcd );

  dcd->rom_code   = GET_VERSION();
  dcd->diary_type = DIARYTYPE_COUNT;
  dcd->diary_id   = pdid;
  dcd->data[0]    = p1;
  dcd->data[1]    = p2;
  dcd->data[2]    = p3;
  dcd->date       = date;
  POKEDIARY_PRINT("PokeDiary::SetCount(%d,%d,%d,%d)\n", pdid, p1, p2, p3);
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PokeDiarySave::DIARY_DATA * PokeDiarySave::SortAndGetPos( DIARY_DATA * dd, int max )
{
  //空きを探す
  for ( int i = 0; i < max; ++ i )
  {
    if ( dd[i].diary_id == PDID_NULL ) return &dd[i];
  }
  //詰める
  for ( int i = 0; i < max - 1; ++ i )
  {
    dd[i] = dd[i+1];
  }
  POKEDIARY_PRINT("PokeDiary::Sorted(%p)\n", dd );
  return &dd[max - 1];
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PokeDiarySave::DIARY_DATA * PokeDiarySave::GetPos( DIARY_DATA * dd, PDID pdid, int max )
{
  //探す
  for ( int i = 0; i < max; ++ i )
  {
    if ( dd[i].diary_id == pdid ) return &dd[i];
  }
  return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
PokeDiarySave::DIARY_DATA * PokeDiarySave::GetPosReverse( DIARY_DATA * dd, PDID pdid, int max )
{
  //逆から探す
  for ( int i=max-1; i>=0; --i )
  {
    if ( dd[i].diary_id == pdid ) return &dd[i];
  }
  return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void PokeDiarySave::ClearSingleData( DIARY_DATA * dd )
{
  static const DIARY_DATA CLEAR_DATA = {
    0,              //u16 rom_code:6;     /**< 生成データのロムバージョン*/
    DIARYTYPE_NONE, //u16 diary_type:2;   /**< ダイアリー種別*/
    0,              //u16 diary_id:8;     /**< ダイアリーID（種別内分類）*/
    { 0, 0, 0 },    //u16 data[3];        /**< 固有データ */
    0,              //u32 date;           /**< 日付 */
  };
  *dd = CLEAR_DATA;
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//
// 
//--------------------------------------------------------------------------
/**
 * @brief   レコードのパラメータの中にmomiji新ポケモンが含まれていないかチェック
 * @param[in] dd    ダイアリーデータ
 * @note   MMCat[223] 対処用
 *
 * @return trueでmomiji新ポケモンが含まれていた
 */
//--------------------------------------------------------------------------
bool PokeDiarySave::CheckDiaryParamMomijiPoke( const DIARY_DATA * dd )
{
  const PDID pdid = static_cast<PDID>(dd->diary_id + PDID_COUNT_NULL);


  // ダイアリーパラメータのチェック種類
  enum CHECK_TYPE{
    CHECK_NONE,               //! チェックしなし
    CHECK_DATA1,              //! data1をチェックする
    CHECK_DATA2,              //! data2をチェックする
    CHECK_DATA1_AND_DATA2,    //! data1とdata2をチェックする
  };

  CHECK_TYPE checkType = CHECK_NONE;

  // パラメータにポケモン種族Noが入っているダイアリーIDをチェック
  switch( pdid ){
  case PDID_COUNT_GANBA_RIBBON:   checkType = CHECK_DATA1; break;
  case PDID_COUNT_NAME_CHANGE:    checkType = CHECK_DATA1; break;
  case PDID_COUNT_FOSSIL:         checkType = CHECK_DATA1; break;
  case PDID_COUNT_CHECK_NATSUKI:  checkType = CHECK_DATA1; break;
  case PDID_COUNT_GTS_POKEPUT:    checkType = CHECK_DATA1; break;
  case PDID_COUNT_GTS_POKETRADE:  checkType = CHECK_DATA1_AND_DATA2; break;
  case PDID_COUNT_MTRADE:         checkType = CHECK_DATA1_AND_DATA2; break;
  case PDID_COUNT_P2PTRADE:       checkType = CHECK_DATA1_AND_DATA2; break;
  case PDID_COUNT_EFFORT:         checkType = CHECK_DATA1; break;
  case PDID_COUNT_PARLE:          checkType = CHECK_DATA1; break;
  case PDID_COUNT_BATTLEROYAL:    checkType = CHECK_DATA1; break;
  case PDID_COUNT_LVUP:           checkType = CHECK_DATA1; break;
  case PDID_COUNT_MARASADA:       checkType = CHECK_DATA2; break;
  case PDID_COUNT_MONOHIROI:      checkType = CHECK_DATA1; break;
  case PDID_COUNT_NAKAYOSHI:      checkType = CHECK_DATA1; break;
  case PDID_COUNT_LOMILOMI:       checkType = CHECK_DATA1; break;
  case PDID_COUNT_TRAINING:       checkType = CHECK_DATA1; break;
  case PDID_COUNT_WAZAMASHIN:     checkType = CHECK_DATA1; break;
  case PDID_COUNT_FIELDSCAN:      checkType = CHECK_DATA1; break;
  }


  switch( checkType ){
  case CHECK_DATA1:              //! data1をチェックする
    {
      return PokeRegulation::CheckNijiCompatibleMonsNo( static_cast<MonsNo>(dd->data[0]), 0 ); // フォルムは設定出来ないので0
    }
  case CHECK_DATA2:              //! data2をチェックする
    {
      return PokeRegulation::CheckNijiCompatibleMonsNo( static_cast<MonsNo>(dd->data[1]), 0 ); // フォルムは設定出来ないので0
    }
  case CHECK_DATA1_AND_DATA2:    //! data1とdata2をチェックする
    {
      const bool isData1Ng = PokeRegulation::CheckNijiCompatibleMonsNo( static_cast<MonsNo>(dd->data[0]), 0 );
      const bool isData2Ng = PokeRegulation::CheckNijiCompatibleMonsNo( static_cast<MonsNo>(dd->data[1]), 0 );
      return (isData1Ng || isData2Ng);
    }
  }

  return false;
}

GFL_NAMESPACE_END(Savedata)
