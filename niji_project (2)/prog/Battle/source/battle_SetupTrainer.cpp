//=============================================================================
/**
 * @file    battle_SetupTrainer.cpp
 * @brief
 * @date    2012.11.24
 * @author  tamada
 */
//=============================================================================
#include <util/include/gfl2_std_string.h>

#include <System/include/GameStrLen.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Print/include/GrammaticalAttributeProvider.h>
#include <Battle/include/Battle_SetupTrainer.h>
#include <Trainer/Trainer/include/TrainerName.h>
#include <Trainer/Trainer/include/TrainerTypeName.h>


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
BSP_TRAINER_DATA::BSP_TRAINER_DATA( gfl2::heap::HeapBase* heap )
{
  STATIC_ASSERT( sizeof(print::GrammaticalAttribute) == sizeof(u8) );

  this->mCore.name              = GFL_NEW( heap ) gfl2::str::StrBuf( System::STRLEN_TRAINER_NAME+1, heap );
  this->mCore.trtype_name       = GFL_NEW( heap ) gfl2::str::StrBuf( System::STRLEN_TRAINER_TYPE+1, heap );
  this->mCore.tr_type_data      = GFL_NEW( heap ) trainer::TrainerTypeData();
  this->mCore.msgArcID          = MSGARC_ID_NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
BSP_TRAINER_DATA::~BSP_TRAINER_DATA()
{
  GFL_SAFE_DELETE( this->mCore.name );
  GFL_SAFE_DELETE( this->mCore.trtype_name );
  GFL_SAFE_DELETE( this->mCore.tr_type_data );
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナータイプデータ読み込み
 * @param heap         使用するヒープ
 * @param trainerType  読み込むトレーナータイプ
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::LoadTrTypeData( gfl2::heap::HeapBase* heap, trainer::TrType trainerType )
{
  this->LoadTrTypeData( heap, trainerType, NULL );
  trainer::TrainerTypeName::GetTrainerTypeName( this->mCore.trtype_name, &( this->mCore.trtype_grammar ), heap, trainerType );
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナータイプデータ読み込み
 * @param heap                使用するヒープ
 * @param trainerType         読み込むトレーナータイプ
 * @param trainerTypeNameData トレーナータイプ名のメッセージデータ
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::LoadTrTypeData( 
  gfl2::heap::HeapBase* heap, 
  trainer::TrType       trainerType, 
  gfl2::str::MsgData*   trainerTypeNameData )
{
  this->mCore.tr_type_data->LoadData( heap, trainerType );

  this->mCore.tr_type      = trainerType;
  this->mCore.tr_group     = this->mCore.tr_type_data->GetGroup();
  this->mCore.tr_sex       = this->mCore.tr_type_data->GetSex();
  this->mCore.tr_plural    = this->mCore.tr_type_data->GetPlural();
  this->mCore.btl_eff_id   = this->mCore.tr_type_data->GetBattleEffectID();

  if( trainerTypeNameData != NULL ) {
    this->mCore.trtype_grammar = trainerTypeNameData->GetString( trainerType, *( this->mCore.trtype_name ) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナータイプデータの再読み込み
 * @param heap  使用するヒープ
 *
 * 現在保持しているトレーナータイプIDから再読み込みを行う。
 * ※別言語データの場合に自国語に書き戻すために使用する
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::ReloadTrTypeData( gfl2::heap::HeapBase* heap )
{
  trainer::TrType trainerType = static_cast<trainer::TrType>( this->mCore.tr_type );
  this->LoadTrTypeData( heap, trainerType );
}




//-----------------------------------------------------------------------------
/**
 * @brief   トレーナータイプデータ取得
 * @return  トレーナータイプデータ
 */
//-----------------------------------------------------------------------------
const trainer::TrainerTypeData * BSP_TRAINER_DATA::GetTrTypeData(void) const
{
	return this->mCore.tr_type_data;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u8 BSP_TRAINER_DATA::GetTrTypePlural( void ) const
{
  return mCore.tr_plural;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::SetUseItem( const u16 * items )
{
  for(int i = 0; i < USE_ITEM_MAX; ++ i )
  {
    mCore.use_item[i] = items[i];
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナー名を設定する
 * @param pTempHeap  使用する作業用ヒープ
 * @param trainerId  トレーナーID
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::SetName( gfl2::heap::HeapBase* pTempHeap, trainer::TrainerID trainerId )
{
  trainer::TrainerName::GetTrainerName( this->mCore.name, pTempHeap, trainerId );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::SetName( gfl2::str::MsgData * msg_data, u16 str_id )
{
	msg_data->GetString( str_id, *(this->mCore.name) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::SetName( const gfl2::str::StrBuf & name )
{
  mCore.name->SetStr( name );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::SetTrTypeName( const gfl2::str::StrBuf & trtype_name, u8 grammar_attr )
{
  mCore.trtype_name->SetStr( trtype_name );
  mCore.trtype_grammar = grammar_attr;
}


//-----------------------------------------------------------------------------
/**
 * @brief トレーナーデータをシリアライズする
 * @param[out]  serializedData  シリアライズしたデータの格納先
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::Serialize( BSP_TRAINER_DATA::SERIALIZE_DATA* serializedData ) const
{
  serializedData->is_exist        = true;
  serializedData->tr_id           = mCore.tr_id;
  serializedData->ai_bit          = mCore.ai_bit;
  serializedData->btl_eff_id      = mCore.btl_eff_id;
  serializedData->tr_type         = static_cast<u16>(mCore.tr_type);
  serializedData->tr_group        = mCore.tr_group;
  serializedData->tr_sex          = mCore.tr_sex;
  serializedData->tr_plural       = mCore.tr_plural;
  serializedData->trtype_grammar  = mCore.trtype_grammar;
  serializedData->gold            = mCore.gold;
  for( s32 j = 0 ; j < USE_ITEM_MAX ; j++ )
  {
    serializedData->use_item[ j ] = mCore.use_item[ j ];
  }
  mCore.name->PutStr( serializedData->name, pml::PERSON_NAME_BUFFER_SIZE );
  //GFBTS[2653](kujira)
  //SBTS[74] トレーナータイプのバッファにEOMが無いのでベタコピー対応
  for( int i=0;i< System::STRLEN_TRAINER_TYPE+1; i++)
  {
    if( mCore.trtype_name->GetPtr()[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
    serializedData->trtype_name[i] = mCore.trtype_name->GetPtr()[i];
  }

  serializedData->msgArcID   = mCore.msgArcID;
  serializedData->win_strID  = mCore.win_strID;
  serializedData->lose_strID = mCore.lose_strID;
  
}

//-----------------------------------------------------------------------------
/**
 * @brief トレーナーデータをデシリアライズする
 * @param[in]  serializedData   元となるシリアライズ済みデータのアドレス
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::Deserialize( const BSP_TRAINER_DATA::SERIALIZE_DATA* serializedData )
{
  if( serializedData->is_exist != false )
  {
    mCore.tr_id           = serializedData->tr_id;
    mCore.ai_bit          = serializedData->ai_bit;
    mCore.btl_eff_id      = static_cast<BattleEffectId>(serializedData->btl_eff_id);
    mCore.tr_type         = serializedData->tr_type;
    mCore.tr_group        = serializedData->tr_group;
    mCore.tr_sex          = serializedData->tr_sex;
    mCore.tr_plural       = serializedData->tr_plural;
    mCore.trtype_grammar  = serializedData->trtype_grammar;
    mCore.gold            = serializedData->gold;
    for( s32 j = 0 ; j < USE_ITEM_MAX ; j++ )
    {
      mCore.use_item[ j ] = serializedData->use_item[ j ];
    }
    mCore.name->SetStr( serializedData->name );
    
    //GFBTS[2653](kujira)
    //SBTS[74] トレーナータイプのバッファにEOMが無いのでベタコピー対応
    gfl2::str::STRCODE tempArr[System::STRLEN_TRAINER_TYPE+1];
    for( int i=0;i<System::STRLEN_TRAINER_TYPE+1; i++)
    {
      tempArr[i] = gfl2::str::EOM_CODE;
    }
    for( int i=0;i< System::STRLEN_TRAINER_TYPE+1; i++)
    {
      if( serializedData->trtype_name[i] == gfl2::str::EOM_CODE )
      {
        break;
      }
      tempArr[i] = serializedData->trtype_name[i];
    }
    
    mCore.trtype_name->SetStr( tempArr );

    mCore.msgArcID   = serializedData->msgArcID;
    mCore.win_strID  = serializedData->win_strID;
    mCore.lose_strID = serializedData->lose_strID;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   シリアライズデータのクリア
 * @param[out]  serializedData
 */
//-----------------------------------------------------------------------------
void BSP_TRAINER_DATA::ClearSerializeData( BSP_TRAINER_DATA::SERIALIZE_DATA* serializedData )
{
  gfl2::std::MemClear( serializedData, sizeof(BSP_TRAINER_DATA::SERIALIZE_DATA) );
  serializedData->is_exist = false;
}

