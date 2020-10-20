//============================================================================================
/**
 * @file    GTSSave.cpp
 * @brief   GTSセーブデータ
 * @author  Akito Mori
 * @date    12/07/10
 */
//============================================================================================
#include "Savedata/include/GTSSave.h"

GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
GtsData::GtsData(void)
{
  Clear( NULL );
  STATIC_ASSERT(sizeof(NetApp::GTS::GtsIndexData) == 168);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
GtsData::~GtsData(void)
{
}


//--------------------------------------------------------------------------------------------
/**
 * @brief セーブデータクリア処理
 * @param heap
 */
//--------------------------------------------------------------------------------------------
void GtsData::Clear( gfl2::heap::HeapBase * heap)
{
  gfl2::std::MemClear( &mGtsData, sizeof(Data) );
  mGtsData.SinglePhrase[0] = gfl2::str::EOM_CODE;
  mGtsData.DepositFlag = false;
  mGtsData.toBoxTray   = RETURN_POSITION_TEMOTI;
  mGtsData.toBoxPos    = 0;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief  退避ポケモンデータ取得
 * @param  [out] セーブデータからポケモンデータを受け取るバッファ
 */
//--------------------------------------------------------------------------------------------
void GtsData::GetBackupPokemonCoreParam( pml::pokepara::CoreParam *coreParam )
{
  coreParam->Deserialize_Core( &mGtsData.backupPokemon.data[0] );


}

//--------------------------------------------------------------------------------------------
/**
 * @brief  退避ポケモンデータセット
 * @param  [in]coreParam セーブデータに入れるポケモンデータ
 * @param  [in]tray      ポケモンがいたトレイNO（RETURN_POSITION_TEMOTI=0, trayNoは1オリジン)
 * @param  [in]pos       ポケモンがいたトレイ内の位置
 * @param  [in]poke      欲しいポケモンの種族NO
 * @param  [in]sex       欲しいポケモンの性別
 * @param  [in]level     欲しいポケモンのレベル
 */
//--------------------------------------------------------------------------------------------
void GtsData::SetBackupPokemonCoreParam( const pml::pokepara::CoreParam *coreParam, u8 tray, u8 pos, MonsNo poke, int sex, int level )
{
  coreParam->Serialize_Core( &mGtsData.backupPokemon.data[0] );
  mGtsData.toBoxTray  = tray;
  mGtsData.toBoxPos   = pos;
  mGtsData.WantPoke   = poke;
  mGtsData.WantSex    = sex;
  mGtsData.WantLevel  = level;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  GTSに預けているか？取得
 * @param  none
 * @retval GTSに預けているか？  true:預けている, false:預けていない
 */
//--------------------------------------------------------------------------------------------
bool GtsData::GetDepositFlag( void )
{
  return GFL_BOOL_CAST( mGtsData.DepositFlag );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief  GTSに預けているか？セット
 * @param  [in] true or false
 */
//--------------------------------------------------------------------------------------------
void GtsData::SetDepositFlag( bool flag )
{
  mGtsData.DepositFlag = flag;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  ポケモンを戻す場所の取得
 * @retval RETURN_POSITION_TEMOTI = てもち、  以降1オリジンでBOXトレイのNO
 */
//--------------------------------------------------------------------------------------------
int GtsData::GetReturnTrayNo( void )
{
  return mGtsData.toBoxTray;
}

int GtsData::GetReturnPosNo( void )
{
  return mGtsData.toBoxPos;
}

MonsNo GtsData::GetWantPoke( void )
{
  return static_cast<MonsNo>(mGtsData.WantPoke);
}
int GtsData::GetWantSex( void )
{
  return mGtsData.WantSex;
}
int GtsData::GetWantLevel( void )
{
  return mGtsData.WantLevel;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  一言メッセージを入力している？
 * @param  none
 * @retval 一言メッセージを入力している？  true:入力している, false:していない
 */
//--------------------------------------------------------------------------------------------
bool GtsData::GetSinglePhraseExist( void )
{
  int count = 0;
  for(int i=0;i<SINGLE_PHRASE_DATA_SIZE;i++){
    if(mGtsData.SinglePhrase[i]==gfl2::str::EOM_CODE){
      break;
    }
    count++;
  }
  
  if(count==0){
    return false;
  }
  
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  一言メッセージ取得
 * @param  [out] セーブデータから一言メッセージをコピーするSTRBUF
 */
//--------------------------------------------------------------------------------------------
void GtsData::GetSinglePhrase( gfl2::str::StrBuf *des )
{
  des->SetStr( mGtsData.SinglePhrase );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  一言メッセージをセーブデータにセット
 * @param  [in] 一言メッセージ
 */
//--------------------------------------------------------------------------------------------
void GtsData::SetSinglePhrase( const gfl2::str::StrBuf *src )
{
  src->PutStr( mGtsData.SinglePhrase, SINGLE_PHRASE_DATA_SIZE );

}

//--------------------------------------------------------------------------------------------
/**
 * @brief  欲しいポケモン名取得
 * @param  [out] セーブデータからポケモン名をコピーするSTRBUF
 */
//--------------------------------------------------------------------------------------------
void GtsData::GetWantPokeName( gfl2::str::StrBuf *des )
{
  des->SetStr( mGtsData.WantPokeName );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  欲しいポケモン名をセーブデータにセット
 * @param  [in] ポケモン名
 */
//--------------------------------------------------------------------------------------------
void GtsData::SetWantPokeName( const gfl2::str::StrBuf *src )
{
  src->PutStr( mGtsData.WantPokeName, pml::MONS_NAME_BUFFER_SIZE );
}


u64 GtsData::GetUploadResultId(void)
{
  return mGtsData.uploadId;
}

void GtsData::SetUploadResultId( u64 dataId )
{
  mGtsData.uploadId = dataId;
}


const NetApp::GTS::GtsSerializeRecordKey& GtsData::GetUploadKey() const
{
  return mGtsData.UploadKey;
}

void GtsData::SetUploadKey( const NetApp::GTS::GtsSerializeRecordKey& uploadKey )
{
  mGtsData.UploadKey = uploadKey;
}


const NetApp::GTS::GtsIndexData& GtsData::GetUploadGtsIndexData() const
{
  return mGtsData.uploadGtsIndexData;
}


void GtsData::SetUploadGtsIndexData( const NetApp::GTS::GtsIndexData& uploadGtsIndexData )
{
  mGtsData.uploadGtsIndexData = uploadGtsIndexData;
}


GFL_NAMESPACE_END(Savedata)
