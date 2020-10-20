//=============================================================================
/**
 * @file   pml_EggWazaData.cpp
 * @date   2015/10/01 10:38:56
 * @author obata_toshihiro
 * @brief  タマゴ技データ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>

#include "pml/include/pokepara/pml_EggWazaData.h"
#include "pml/include/system/pml_Library.h"


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  バッファの確保に使用するヒープ
 */
 //-----------------------------------------------------------------------------
EggWazaData::EggWazaData( gfl2::heap::HeapBase* heap ) : 
  m_isLoaded( false ),
  m_buffer()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
EggWazaData::~EggWazaData()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief タマゴ技データを読み込む
 * @param arcFileName  アーカイブファイルの名前
 * @param monsno       読み込み対象モンスターの番号
 * @param formno       読み込み対象フォルムの番号
 */
//-----------------------------------------------------------------------------
void EggWazaData::Load( const char* arcFileName, MonsNo monsno, FormNo formno )
{
  u32 dataID = EggWazaData::GetDataID( monsno, formno );
  // @fix GFNMCat[1481] フォルムに対応したタマゴ技をロードしていなかった問題を修正
  EggWazaData::LoadData( &m_buffer, dataID );
  m_isLoaded = true;
}

/**
 * @brief タマゴ技データを読み込む
 * @param[out] buffer   読込先バッファ
 * @param      dataID   読み込むデータのID
 */
void EggWazaData::LoadData( TamagoWazaData* buffer, u32 dataID )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
  readReq.fileName = pml::Library::m_setting->garcPath_KowazaTable;
  readReq.datId    = dataID;
  readReq.pBuf     = buffer;
  readReq.bufSize  = sizeof(TamagoWazaData);
  pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );
}

/**
 * @brief タマゴ技データのIDを取得する
 * @param monsno  対象のモンスターナンバー
 * @param formno  対象のフォルムナンバー
 */
u32 EggWazaData::GetDataID( MonsNo monsno, FormNo formno )
{ 
  // タマゴにはパーソナルがないので, ダミーデータを読み込ませるようにする
  if( ( monsno == MONSNO_TAMAGO ) || 
      ( monsno == MONSNO_DAMETAMAGO ) ||
      ( MONSNO_END < monsno ) )
  {
    return 0;
  }

  // デフォルトフォルムは、モンスターナンバーの順に並んでいる
  if( formno == 0 )
  {
    return monsno;
  }
  
  // 別フォルムは、デフォルトフォルムのデータにインデックスが格納されている
  {
    TamagoWazaData data;
    EggWazaData::LoadData( &data, monsno );
    return ( data.anotherFormHeadDataID + formno - 1 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief タマゴ技の数を取得する
 */
//-----------------------------------------------------------------------------
u32 EggWazaData::GetEggWazaNum( void ) const
{
  if( !m_isLoaded )
  {
    return 0;
  }

  return m_buffer.wazaNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief タマゴ技を取得する
 * @param eggWazaIndex  タマゴ技のインデックス
 */
//-----------------------------------------------------------------------------
WazaNo EggWazaData::GetEggWazaNo( u32 eggWazaIndex ) const
{
  if( !m_isLoaded )
  {
    return WAZANO_NULL;
  }

  if( ( MAX_EGG_WAZA_NUM <= eggWazaIndex ) ||
      ( this->GetEggWazaNum() <= eggWazaIndex ) )
  {
    GFL_ASSERT(0);
    return WAZANO_NULL;
  }

  return static_cast<WazaNo>( m_buffer.wazano[ eggWazaIndex ] );
}



GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )