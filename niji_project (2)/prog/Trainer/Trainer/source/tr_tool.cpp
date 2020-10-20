//=============================================================================
/**
 * @file   tr_tool.cpp
 * @brief  トレーナーデータ処理ツール群
 * @author obata_toshihiro
 */
//=============================================================================
#include <pml/include/pmlib.h>
#include <math/include/gfl2_math.h>

#include <Savedata/include/EventWork.h>
#include <Print/include/PrintSystem.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <Battle/include/battle_def.h>
#include <Battle/include/Battle_SetupTrainer.h>
#include <Trainer/Trainer/include/tr_tool.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/trainer/trid_def.h>



namespace trainer {




/**
 * @brief トレーナー手持ち持ちポケモンのパラメータ
 */
struct TRAINER_POKE_DATA
{
  u8    para;                  // 0x00 セットするポケモンのパラメータ（上位4bit：特性　下位4bit：性別）
  u8    seikaku;               // 0x01 性格
  u8    effort_hp;             // 0x02 努力値( HP )
  u8    effort_atk;            // 0x03 努力値( 攻撃 )
  u8    effort_def;            // 0x04 努力値( 防御 )
  u8    effort_spatk;          // 0x05 努力値( 特攻 )
  u8    effort_spdef;          // 0x06 努力値( 特防 )
  u8    effort_agi;            // 0x07 努力値( 素早さ )
  u32   talent_hp    :5;       // 0x08 個体値( HP )
  u32   talent_atk   :5;       // 0x08 個体値( 攻撃 )
  u32   talent_def   :5;       // 0x08 個体値( 防御 )
  u32   talent_spatk :5;       // 0x08 個体値( 特攻 )
  u32   talent_spdef :5;       // 0x08 個体値( 特防 )
  u32   talent_agi   :5;       // 0x08 個体値( 素早さ )
  u32   is_rare      :1;       // 0x08 色違いフラグ( 1なら色違い )
  u32                :1;       // 0x08 
  u16   decorationItem;        // 0x0c デコアイテムNo.
  u16   level;                 // 0x0e セットするポケモンのレベル
  u16   monsno;                // 0x10 セットするポケモン
  u16   form_no;               // 0x12 セットするフォルムナンバー
  u16   itemno;                // 0x14 セットするアイテム
  u16   padding;               // 0x16
  u16   waza[ PTL_WAZA_MAX ];  // 0x18 持ってる技
};


// 手持ちポケモンデータを読み込むためのバッファサイズ
static const u32 BUFFERSIZE_POKEDATA = sizeof( TRAINER_POKE_DATA ) * pml::PokeParty::MAX_MEMBERS;





TrainerID normalizeTrainerID( TrainerID trainerId );
void loadTrainerPokeData( gfl2::heap::HeapBase* tempHeap, TrainerID trainerId, void* dst );
void setupPokeParty( pml::PokeParty* pokeParty, const TRAINER_DATA* trainerData, const void* pokeDataBuffer, const TrainerTypeData* trainerTypeData, gfl2::heap::HeapBase* heap );
pml::pokepara::PokemonParam* create_pokemon( 
  MonsNo monsno, u8 form_no, u8 level, u8 talent_hp, 
  u8 talent_atk, u8 talent_def, u8 talent_spatk, u8 talent_spdef, u8 talent_agi,
  u8 effort_hp, u8 effort_atk, u8 effort_def, u8 effort_spatk, u8 effort_spdef, u8 effort_agi, 
  bool is_rare, u32 param, pml::pokepara::Seikaku seikaku, const TRAINER_DATA* trainerData, const TrainerTypeData* trainerTypeData, gfl2::heap::HeapBase* pHeap );
void setup_pokemon_waza( pml::pokepara::PokemonParam* pokeParam, const u16* wazaTbl );
void setup_pokemon_param( pml::pokepara::PokemonParam* pokeParam );
void setupSimpleParam( PokeTool::SimpleParam * simpleParam, MonsNo mons, FormNo form, pml::Sex sex );
pml::Sex getPokeSex( pml::personal::PersonalData * personalBuffer, MonsNo monsno, FormNo formno, u32 param, const TrainerTypeData * trainerTypeData );
void setupSimpleParam_byDataMulti( PokeTool::SimpleParam * simpleParam, const TRAINER_DATA * trainerData, const void * pokeData, const TrainerTypeData * trainerTypeData, pml::personal::PersonalData * personalBuffer );




//-----------------------------------------------------------------------------
/**
 * @briefトレーナーデータを読み込む
 *
 * @param[in]  tempHeap     アーカイブ一時オープン用ヒープハンドル
 * @param[in]  trainerId    取得したいトレーナーID
 * @param[out] trainerData  取得したトレーナーデータの格納先を指定
 */
//-----------------------------------------------------------------------------
void LoadTrainerData( gfl2::heap::HeapBase* tempHeap, TrainerID trainerId, TRAINER_DATA* trainerData )
{
  static const bool close_soon = true;
  static const bool is_comp = false;
  app::util::FileAccessor::FileOpenSync( ARCID_TR_DATA, tempHeap->GetLowerHandle(), close_soon );
  if( app::util::FileAccessor::IsFileOpen( ARCID_TR_DATA ) )
  {
    trainerId = normalizeTrainerID( trainerId );
    app::util::FileAccessor::FileLoadBufSync( ARCID_TR_DATA, trainerId, static_cast<void*>(trainerData), sizeof(TRAINER_DATA), tempHeap->GetLowerHandle(), is_comp );
    app::util::FileAccessor::FileCloseSync( ARCID_TR_DATA );
  }
}

/**
 * @brief トレーナーID のチェック
 * @param trainerId  チェック対象のトレーナーID
 * @return 有効なトレーナーID
 */
TrainerID normalizeTrainerID( TrainerID trainerId )
{
  if ( ( trainerId == TRID_NULL ) || 
       ( trainerId >= TRID_MAX  ) )
  {
    GFL_ASSERT_MSG( 0, "trainer ID error(%d)\n", trainerId );
    return static_cast<TrainerID>(1);
  }
  return trainerId;
}

//-----------------------------------------------------------------------------
/**
 * @brief 通常エンカウントトレーナーのバトルセットアップ用データを構築
 *
 * @param[in]  trainerId          トレーナーID
 * @param[out] battleTrainerData  セットアップ対象のトレーナーデータ
 * @param[in]  tempHeap           作業用ヒープ
 */
//-----------------------------------------------------------------------------
void EncountTrainerPersonalDataMake( TrainerID trainerId, BSP_TRAINER_DATA* battleTrainerData, gfl2::heap::HeapBase* tempHeap )
{
  trainerId = normalizeTrainerID( trainerId );
  TRAINER_DATA* trainerData = GFL_NEW_LOW( tempHeap ) TRAINER_DATA;

  if( trainerData == NULL ) {
    return;
  }

  LoadTrainerData( tempHeap, trainerId, trainerData );

  battleTrainerData->SetTrainerID( trainerId );
  battleTrainerData->SetAIBit( trainerData->aibit );
  battleTrainerData->LoadTrTypeData( tempHeap, static_cast<trainer::TrType>(trainerData->tr_type) );
  battleTrainerData->SetName( tempHeap, trainerId );
  battleTrainerData->SetGoldParam( trainerData->gold );
  battleTrainerData->SetUseItem( trainerData->use_item );

  GFL_DELETE trainerData;
}

//-----------------------------------------------------------------------------
/**
 * @brief 通常エンカウントトレーナーのポケモンパーティデータを構築
 *
 * @param[in]  trainerId  トレーナーID
 * @param[out] pokeParty  パーティ格納クラス
 * @param[in]  heap       ポケモンパラメータの生成に使用するヒープ
 */
//-----------------------------------------------------------------------------
void EncountTrainerPokeDataMake( TrainerID trainerId, pml::PokeParty* pokeParty, gfl2::heap::HeapBase* heap )
{
  trainerId = normalizeTrainerID( trainerId );

  TRAINER_DATA*    trainerData     = GFL_NEW_LOW( heap ) TRAINER_DATA;
  TrainerTypeData* trainerTypeData = GFL_NEW_LOW( heap ) TrainerTypeData();
  void*            pokeDataBuffer  = GflHeapAllocMemoryLow( heap->GetLowerHandle(), BUFFERSIZE_POKEDATA );

  LoadTrainerData( heap->GetLowerHandle(), trainerId, trainerData );
  trainerTypeData->LoadData( heap->GetLowerHandle(), static_cast<TrType>( trainerData->tr_type ) );
  loadTrainerPokeData( heap->GetLowerHandle(), trainerId, pokeDataBuffer );
  setupPokeParty( pokeParty, trainerData, pokeDataBuffer, trainerTypeData, heap );

  GflHeapFreeMemory( pokeDataBuffer );
  GFL_DELETE( trainerTypeData );
  GFL_DELETE( trainerData );
}

/**
 *  @briefトレーナーポケモンデータを取得
 *
 *  @param[in]   tempHeap   アーカイブ一時オープン用ヒープハンドル
 *  @param[in]   trainerId  取得したいトレーナーID
 *  @param[out]  destBuffer 取得したトレーナーポケモンデータの格納先を指定
 */
void loadTrainerPokeData( gfl2::heap::HeapBase* tempHeap, TrainerID trainerId, void* destBuffer )
{
  static const bool close_soon = true;
  static const bool is_comp = false;
  app::util::FileAccessor::FileOpenSync( ARCID_TR_POKE_DATA, tempHeap->GetLowerHandle(), close_soon );
  if( app::util::FileAccessor::IsFileOpen( ARCID_TR_POKE_DATA ) )
  {
    trainerId = normalizeTrainerID( trainerId );
    app::util::FileAccessor::FileLoadBufSync( ARCID_TR_POKE_DATA, trainerId, destBuffer, BUFFERSIZE_POKEDATA, tempHeap->GetLowerHandle(), is_comp );
    app::util::FileAccessor::FileCloseSync( ARCID_TR_POKE_DATA );
  }
}

/**
 * @brief トレーナーの手持ちポケモンデータを生成する
 * @param[out] pokeParty       生成したポケモンデータの格納先
 * @param[in]  trainerData     トレーナーデータ
 * @param[in]  pokeDataBuffer  手持ちポケモンデータ( の配列 )が格納されているバッファ
 * @param[in]  heap            ポケモンパラメータの生成に使用するヒープ     
 */
void setupPokeParty( pml::PokeParty* pokeParty, const TRAINER_DATA* trainerData, const void* pokeDataBuffer, const TrainerTypeData* trainerTypeData, gfl2::heap::HeapBase* heap )
{
  pokeParty->Clear();

  const TRAINER_POKE_DATA* pokeData = ( const TRAINER_POKE_DATA * )pokeDataBuffer;
  pml::pokepara::PokemonParam* pokeParam;

  for( u32 pokeIndex=0 ; pokeIndex<trainerData->poke_count ; pokeIndex++ )
  {
    pokeParam = create_pokemon(
      static_cast<MonsNo>( pokeData[ pokeIndex ].monsno ), 
      pokeData[ pokeIndex ].form_no,
      pokeData[ pokeIndex ].level, 
      pokeData[ pokeIndex ].talent_hp, 
      pokeData[ pokeIndex ].talent_atk,
      pokeData[ pokeIndex ].talent_def,
      pokeData[ pokeIndex ].talent_spatk,
      pokeData[ pokeIndex ].talent_spdef,
      pokeData[ pokeIndex ].talent_agi,
      pokeData[ pokeIndex ].effort_hp, 
      pokeData[ pokeIndex ].effort_atk,
      pokeData[ pokeIndex ].effort_def,
      pokeData[ pokeIndex ].effort_spatk,
      pokeData[ pokeIndex ].effort_spdef,
      pokeData[ pokeIndex ].effort_agi,
      pokeData[ pokeIndex ].is_rare,
      pokeData[ pokeIndex ].para,
      static_cast<pml::pokepara::Seikaku>( pokeData[ pokeIndex ].seikaku ),
      trainerData, 
      trainerTypeData, 
      heap->GetLowerHandle() );

    pokeParam->SetItem( pokeData[ pokeIndex ].itemno );
    setup_pokemon_waza( pokeParam, pokeData[ pokeIndex ].waza );
    setup_pokemon_param( pokeParam );
    pokeParty->AddMember( *pokeParam );
    GFL_DELETE( pokeParam );
  }
}

/**
 *  @brief ポケモンデータ生成
 *  @param[in]  mons_no       種族ナンバー
 *  @param[in]  form_no       フォルムナンバー
 *  @param[in]  level         レベル
 *  @param[in]  talent_hp     個体値( HP )
 *  @param[in]  talent_atk    個体値( 攻撃 ) 
 *  @param[in]  talent_def    個体値( 防御 ) 
 *  @param[in]  talent_spatk  個体値( 特攻 )   
 *  @param[in]  talent_spdef  個体値( 特防 )   
 *  @param[in]  talent_agi    個体値( 素早さ ) 
 *  @param[in]  effort_hp     努力値( HP )
 *  @param[in]  effort_atk    努力値( 攻撃 ) 
 *  @param[in]  effort_def    努力値( 防御 ) 
 *  @param[in]  effort_spatk  努力値( 特攻 )   
 *  @param[in]  effort_spdef  努力値( 特防 )   
 *  @param[in]  effort_agi    努力値( 素早さ ) 
 *  @param[in]  is_rare       色違いにするか？
 *  @param[in]  param         上位4ビットは性別指定（0=指定なし、1=♂、2=♀）
 *  @param[in]  seikaku       性格No.
 *  @param[in]  td            親トレーナーのトレーナーデータ
 *  @param[in]  trtype        親トレーナーのトレーナータイプデータ
 *  @param[in]  pHeap         使用するヒープ
 *  @return     PokemonParam  生成したポケモン
 *
 *  @note 戦う度に特性・性別・性格などが変動するとバランス調整できないので、固定にしている
 */
pml::pokepara::PokemonParam* create_pokemon( 
  MonsNo                 mons_no, 
  u8                     form_no, 
  u8                     level, 
  u8                     talent_hp,
  u8                     talent_atk,
  u8                     talent_def,
  u8                     talent_spatk,
  u8                     talent_spdef,
  u8                     talent_agi,
  u8                     effort_hp,
  u8                     effort_atk,
  u8                     effort_def,
  u8                     effort_spatk,
  u8                     effort_spdef,
  u8                     effort_agi,
  bool                   is_rare,
  u32                    param, 
  pml::pokepara::Seikaku seikaku, 
  const TRAINER_DATA*    td, 
  const TrainerTypeData* trtype, 
  gfl2::heap::HeapBase*  pHeap )
{
  gfl2::math::Random         randContext;
  pml::pokepara::InitialSpec initSpec;

  randContext.Initialize( (u32)( level + mons_no ) );
  for( u32 i=0; i<td->tr_type; ++i ) {
    randContext.Next();
  }

  u32 personal_rand = randContext.Next();

  initSpec.personal_rnd                              = personal_rand;
  initSpec.rare_rnd                                  = ( is_rare ) ? ( pml::pokepara::INIT_SPEC_RARE_TRUE ) : ( pml::pokepara::INIT_SPEC_RARE_FALSE );
  initSpec.id                                        = personal_rand;
  initSpec.monsno                                    = mons_no;
  initSpec.formno                                    = form_no;
  initSpec.level                                     = level;
  initSpec.seikaku                                   = seikaku;
  initSpec.talentPower[ pml::pokepara::POWER_HP ]    = talent_hp;
  initSpec.talentPower[ pml::pokepara::POWER_ATK ]   = talent_atk;
  initSpec.talentPower[ pml::pokepara::POWER_DEF ]   = talent_def;
  initSpec.talentPower[ pml::pokepara::POWER_SPATK ] = talent_spatk;
  initSpec.talentPower[ pml::pokepara::POWER_SPDEF ] = talent_spdef;
  initSpec.talentPower[ pml::pokepara::POWER_AGI ]   = talent_agi;

  // 性別
  { 
    pml::personal::PersonalData * per = GFL_NEW_LOW(pHeap) pml::personal::PersonalData(pHeap->GetLowerHandle());
    pml::personal::LoadPersonalData( mons_no, form_no, per );
    //pml::personal::SexVector vec = per->GetParam( pml::personal::PARAM_ID_sex );
    int vec = per->GetParam( pml::personal::PARAM_ID_sex );
    GFL_DELETE per;
    switch ( vec )
    {
    case pml::personal::SEX_VECTOR_ONLY_MALE:   initSpec.sex = pml::SEX_MALE;     break;
    case pml::personal::SEX_VECTOR_ONLY_FEMALE: initSpec.sex = pml::SEX_FEMALE;   break;
    case pml::personal::SEX_VECTOR_UNKNOWN:     initSpec.sex = pml::SEX_UNKNOWN;  break;
    default:
      if ( (param & 0xf) == 1 ) { initSpec.sex = pml::SEX_MALE;   break; }
      if ( (param & 0xf) == 2 ) { initSpec.sex = pml::SEX_FEMALE; break; }
      if ( vec <= 120 ) { initSpec.sex = pml::SEX_MALE;   break; }
      if ( vec >= 136 ) { initSpec.sex = pml::SEX_FEMALE; break; }
      initSpec.sex = trtype->GetSex();
    }
  }

  // 特性
  switch( (param >> 4) & 0x0f ){
  default: GFL_ASSERT( 0 );
    /* FALL THROUGH */
  case 0:   initSpec.tokusei_index = ((personal_rand >> 16 ) & 1 ); break;
  case 1:   initSpec.tokusei_index = 0; break;
  case 2:   initSpec.tokusei_index = 1; break;
  case 3:   initSpec.tokusei_index = 2; break;
  }

  pml::pokepara::PokemonParam* pokeParam = GFL_NEW( pHeap ) pml::pokepara::PokemonParam( pHeap, initSpec );

  // ボールID
  pokeParam->SetGetBall( trtype->GetBallID() );

  // 努力値
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_HP, effort_hp );
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_ATK, effort_atk );
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_DEF, effort_def );
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_SPATK, effort_spatk );
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_SPDEF, effort_spdef );
  pokeParam->ChangeEffortPower( pml::pokepara::POWER_AGI, effort_agi );

  return pokeParam;
}

/**
 * @brief ポケモンワザ設定
 * @param[out] pokeParam  設定対象のポケモン
 * @param[in]  wazaTbl    技配列( 全て WAZANO_NULL なら、無効データ )
 */
void setup_pokemon_waza( pml::pokepara::PokemonParam* pokeParam, const u16* wazaTbl )
{
  bool isWazaDataExist = false;
  for( u32 i=0; i<PTL_WAZA_MAX; ++i ) 
  {
    if( wazaTbl[i] != WAZANO_NULL ) {
      isWazaDataExist = true;
      break;
    }
  }
  if( !isWazaDataExist ) {
    return;
  }

  for(u32 i=0; i<PTL_WAZA_MAX; ++i)
  {
    pokeParam->SetWaza( i, (WazaNo)(wazaTbl[i]) );
  }
}

/**
 * @brief ポケモンデータ詳細設定
 * @param[out] pokeParam  設定対象のポケモン
 */
void setup_pokemon_param( pml::pokepara::PokemonParam* pokeParam )
{
  bool bFastMode = pokeParam->StartFastMode();

  u8 friend_pow = (pokeParam->HaveWaza(WAZANO_YATUATARI))? 0 : 255;
  pokeParam->SetFamiliarity( friend_pow );

  pokeParam->EndFastMode( bFastMode );
}


//-----------------------------------------------------------------------------
/**
 * @brief トレーナーフラグを取得
 *
 * @param trainerId      トレーナーID
 * @param fieldEventWork イベントワークへのポインタ
 *
 * @retval true   対戦済み
 * @retval false  対戦前
 */
//-----------------------------------------------------------------------------
bool GetTrainerFlag( TrainerID trainerId, const Field::EventWork* fieldEventWork )
{
  TrainerID normalizedTrainerId = normalizeTrainerID( trainerId );
  return fieldEventWork->CheckEventFlag( TR_FLAG_AREA_START + normalizedTrainerId );
}


//-----------------------------------------------------------------------------
/**
 * @brief 指定IDのトレーナーデータとポケモンデータを取得
 *
 * @param tr_id トレーナーIDのテーブル
 * @param dst   トレーナーデータ格納場所
 * @param psp   ポケモンデータ格納場所
 * @param pHeap ヒープ
 *
 * ・マップナビで使用
 * ・ファイルのオープン/クローズをなるべく１回で済むようにした
 */
//-----------------------------------------------------------------------------
void GetEncountTrainerData( TrainerID * tr_id, BSP_TRAINER_DATA ** dst, PokeTool::SimpleParam * psp, u32 max, gfl2::heap::HeapBase * pHeap )
{
	TRAINER_DATA * td = GFL_NEW_LOW_ARRAY( pHeap ) TRAINER_DATA[max];
	if( td == NULL ){
		GFL_ASSERT( 0 );
		return;
	}

	{	// ポケモン以外のデータ
		// メッセージデータ：トレーナー名
		gfl2::str::MsgData * md_trname = GFL_NEW_LOW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(System::MSGLANGID_MAX), GARC_message_trname_DAT, pHeap, gfl2::str::MsgData::LOAD_FULL );
		if( md_trname == NULL ){
			GFL_ASSERT( 0 );
			GFL_DELETE_ARRAY td;
			return;
		}
		// メッセージデータ：トレーナータイプ名
		gfl2::str::MsgData * md_trtype = GFL_NEW_LOW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(System::MSGLANGID_MAX), GARC_message_trtype_DAT, pHeap, gfl2::str::MsgData::LOAD_FULL );
		if( md_trtype == NULL ){
			GFL_ASSERT( 0 );
			GFL_DELETE md_trname;
			GFL_DELETE_ARRAY td;
			return;
		}
		// トレーナーデータ
    app::util::FileAccessor::FileOpenSync( ARCID_TR_DATA, pHeap->GetLowerHandle(), true );
    if( app::util::FileAccessor::IsFileOpen( ARCID_TR_DATA ) == false ){
			GFL_ASSERT( 0 );
			GFL_DELETE md_trtype;
			GFL_DELETE md_trname;
			GFL_DELETE_ARRAY td;
			return;
		}

		for( u32 i=0; i<max; i++ ){
			TrainerID id = normalizeTrainerID( tr_id[i] );
      app::util::FileAccessor::FileLoadBufSync( ARCID_TR_DATA, id, static_cast<void*>(&td[i]), sizeof(TRAINER_DATA), pHeap->GetLowerHandle(), false );

			dst[i]->SetTrainerID( id );
			dst[i]->SetAIBit( td[i].aibit );
			dst[i]->LoadTrTypeData( pHeap, static_cast<trainer::TrType>(td[i].tr_type), md_trtype );
			dst[i]->SetName( md_trname, id );
			dst[i]->SetGoldParam( td[i].gold );
			dst[i]->SetUseItem( td[i].use_item );
		}

    app::util::FileAccessor::FileCloseSync( ARCID_TR_DATA );
		GFL_DELETE md_trtype;
		GFL_DELETE md_trname;
	}

	{	// ポケモン
		// ポケモンデータ読み込み場所
		void * pokeDataBuf = GflHeapAllocMemoryLow( pHeap->GetLowerHandle(), sizeof(TRAINER_POKE_DATA)*pml::PokeParty::MAX_MEMBERS );
		if( pokeDataBuf == NULL ){
			GFL_ASSERT( 0 );
			GFL_DELETE_ARRAY td;
			return;
		}
		// パーソナル
		pml::personal::PersonalData * personal = GFL_NEW_LOW( pHeap ) pml::personal::PersonalData( pHeap->GetLowerHandle() );
		if( personal == NULL ){
			GFL_ASSERT( 0 );
			GflHeapFreeMemory( pokeDataBuf );
			GFL_DELETE_ARRAY td;
			return;
		}
		// ポケモンデータ
    app::util::FileAccessor::FileOpenSync( ARCID_TR_POKE_DATA, pHeap->GetLowerHandle(), true );
    if( app::util::FileAccessor::IsFileOpen( ARCID_TR_POKE_DATA ) == false ){
			GFL_ASSERT( 0 );
			GFL_DELETE personal;
			GflHeapFreeMemory( pokeDataBuf );
			GFL_DELETE_ARRAY td;
			return;
		}

		for( u32 i=0; i<max; i++ )
    {
			TrainerID id = normalizeTrainerID( tr_id[i] );
			const TrainerTypeData * trainerTypeData = dst[i]->GetTrTypeData();
      app::util::FileAccessor::FileLoadBufSync( ARCID_TR_POKE_DATA, id, pokeDataBuf, sizeof(TRAINER_POKE_DATA)*pml::PokeParty::MAX_MEMBERS, pHeap->GetLowerHandle(), false );
			setupSimpleParam_byDataMulti( &psp[i*pml::PokeParty::MAX_MEMBERS], &td[i], pokeDataBuf, trainerTypeData, personal );
		}

		app::util::FileAccessor::FileCloseSync( ARCID_TR_POKE_DATA );
		GFL_DELETE personal;
		GflHeapFreeMemory( pokeDataBuf );
	}

	GFL_DELETE_ARRAY td;
}

 /**
  * @brief トレーナーポケモンデータ生成
  *
  * @param[out] simpleParam      セットアップ対象のパラメータ
  * @param      trainerData      トレーナーデータ
  * @param      pokeData         ポケモンデータ
  * @param      trainerTypeData  トレーナータイプデータ
  * @param      personalBuffer   セットアップに使用するパーソナルデータの展開場所
  */
void setupSimpleParam_byDataMulti( 
  PokeTool::SimpleParam*       simpleParam, 
  const TRAINER_DATA*          trainerData, 
  const void*                  pokeDataBuffer, 
  const TrainerTypeData*       trainerTypeData, 
  pml::personal::PersonalData* personalBuffer )
{
  const TRAINER_POKE_DATA* pokeData = static_cast<const TRAINER_POKE_DATA *>( pokeDataBuffer );

	for( int i=0; i<trainerData->poke_count; i++ )
  {
    MonsNo monsno = static_cast<MonsNo>( pokeData[i].monsno );
    FormNo formno = static_cast<FormNo>( pokeData[i].form_no );
    pml::Sex sex = getPokeSex( personalBuffer, monsno, formno, pokeData[i].para, trainerTypeData );
		setupSimpleParam( &simpleParam[i], monsno, formno, sex );
	}
}

/**
 * @brief ポケモンの性別取得
 *
 * @param personalBuffer    パーソナル展開場所
 * @param monsno            ポケモン番号
 * @param formno            フォルム番号
 * @param param 
 * @param trainerTypeData   トレーナータイプデータ
 *
 * @return 性別
 */
pml::Sex getPokeSex( pml::personal::PersonalData* personalBuffer, MonsNo monsno, FormNo formno, u32 param, const TrainerTypeData* trainerTypeData )
{
	pml::personal::LoadPersonalData( monsno, formno, personalBuffer );
	int sexVector = personalBuffer->GetParam( pml::personal::PARAM_ID_sex );

	switch( sexVector )
  {
	case pml::personal::SEX_VECTOR_ONLY_MALE:
		return pml::SEX_MALE;

	case pml::personal::SEX_VECTOR_ONLY_FEMALE:
		return pml::SEX_FEMALE;

	case pml::personal::SEX_VECTOR_UNKNOWN:
		return pml::SEX_UNKNOWN;

	default:
		if( ( param & 0xf ) == 1 ) {
			return pml::SEX_MALE;
		}
		if( ( param & 0xf ) == 2 ) {
			return pml::SEX_FEMALE;
		}
		if( sexVector <= 120 ) {
			return pml::SEX_MALE;
		}
		if( sexVector >= 136 ) {
			return pml::SEX_FEMALE;
		}
	}

	return static_cast<pml::Sex>( trainerTypeData->GetSex() );
}

/**
 * @brief SimpleParam 作成
 *
 * @param simpleParam    作成場所
 * @param mons  ポケモン番号
 * @param form  フォルム番号
 * @param sex   性別
 */
void setupSimpleParam( PokeTool::SimpleParam * simpleParam, MonsNo mons, FormNo form, pml::Sex sex )
{
	simpleParam->monsNo  = mons;
	simpleParam->formNo  = form;
	simpleParam->sex     = sex;
	simpleParam->isRare  = false;	// 無視
	simpleParam->isEgg   = false;	// 無視
	simpleParam->perRand = 0;			// 無視
}

//-----------------------------------------------------------------------------
/**
 * @brief 2人分のトレーナータイプからタッグトレーナーかチェックし、タッグ用のトレーナータイプを返す。
 *        ふたごちゃん等をメッセージで分岐するため
 *
 * @param trtype1 １人目のトレーナータイプ
 * @param trtype2 ２人目のトレーナータイプ
 *
 * @return int -1の場合タッグトレーナーではない。それ以外はタイプが返ってくる
 */
//-----------------------------------------------------------------------------
int GetTagTrainerType( u16 trtype1 , u16 trtype2 )
{
  //momiji追加分
  //※niji分の書き方がまずくて修行仲間が空手親子に引っかかってしまう。なので上に持ってくる
  //OLｘえんじ♀＝ダンスおやこ
  if ((trtype1 == TRTYPE_WOL && trtype2 == TRTYPE_TWINS) ||
    (trtype1 == TRTYPE_TWINS && trtype2 == TRTYPE_WOL)){
    return TRTYPE_WDANCE;
  }

  //カラテおうｘダンサー＝カポエラカップル
  if ((trtype1 == TRTYPE_WKARATE &&  trtype2 == TRTYPE_WDENCER) ||
    (trtype1 == TRTYPE_WDENCER && trtype2 == TRTYPE_WKARATE)){
    return TRTYPE_WCAPOEIRA;
  }

  //かんこうきゃく♂ｘバッドガール及びベルボーイｘかんこうきゃく♀＝かんこうカップル
  if ((trtype1 == TRTYPE_HONEYMOON &&  trtype2 == TRTYPE_BADCOUPLE2) ||
    (trtype1 == TRTYPE_BADCOUPLE2 && trtype2 == TRTYPE_HONEYMOON) ||
    (trtype1 == TRTYPE_WBELLBOY &&  trtype2 == TRTYPE_HONEYMOON2) ||
    (trtype1 == TRTYPE_HONEYMOON2 && trtype2 == TRTYPE_WBELLBOY)){
    return TRTYPE_WTOURIST;
  }

  //ベテラントレーナー♂ｘエリートトレーナー♂＝バトルしてい
  if ((trtype1 == TRTYPE_WVETERAN && trtype2 == TRTYPE_WELITE) ||
    (trtype1 == TRTYPE_WELITE && trtype2 == TRTYPE_WVETERAN)){
    return TRTYPE_WBATTLE;
  }

  //カラテおうｘカラテおう＝しゅぎょうなかま
  if (trtype1 == TRTYPE_WKARATE && trtype2 == TRTYPE_WKARATE){
    return TRTYPE_KARATEKARATE;
  }

  //ふたごちゃん
  if( trtype1 == TRTYPE_TWINS &&
      trtype2 == TRTYPE_TWINS )
  {
    return TRTYPE_TWINSBASIS;
  }
  //ホープコンビ
  if( (trtype1 == TRTYPE_WHOPE || trtype1 == TRTYPE_WHOPE2) &&
      (trtype2 == TRTYPE_WHOPE || trtype2 == TRTYPE_WHOPE2) )
  {
    return TRTYPE_WHOPEBASIS;
  }
  //バッドカップル
  if( (trtype1 == TRTYPE_BADCOUPLE || trtype1 == TRTYPE_BADCOUPLE2) &&
    (trtype2 == TRTYPE_BADCOUPLE || trtype2 == TRTYPE_BADCOUPLE2) )
  {
    return TRTYPE_BADCOUPLEBASIS;
  }
  //カラテおやこ
  if( (trtype1 == TRTYPE_WKARATE || trtype1 == TRTYPE_WKARATE2) &&
    (trtype2 == TRTYPE_WKARATE || trtype2 == TRTYPE_WKARATE2) )
  {
    return TRTYPE_WKARATEBASIS;
  }
  //みずぎカップル
  if( (trtype1 == TRTYPE_SWIMCOUPLE || trtype1 == TRTYPE_SWIMCOUPLE2) &&
    (trtype2 == TRTYPE_SWIMCOUPLE || trtype2 == TRTYPE_SWIMCOUPLE2) )
  {
    return TRTYPE_SWIMCOUPLEBASIS;
  }
  //エリートコンビ
  if( (trtype1 == TRTYPE_WELITE || trtype1 == TRTYPE_WELITE2) &&
    (trtype2 == TRTYPE_WELITE || trtype2 == TRTYPE_WELITE2) )
  {
    return TRTYPE_WELITEBASIS;
  }
  //ハネムーン
  if( (trtype1 == TRTYPE_HONEYMOON || trtype1 == TRTYPE_HONEYMOON2) &&
    (trtype2 == TRTYPE_HONEYMOON || trtype2 == TRTYPE_HONEYMOON2) )
  {
    return TRTYPE_HONEYMOONBASIS;
  }
  //ベテランコンビ
  if( (trtype1 == TRTYPE_WVETERAN || trtype1 == TRTYPE_WVETERAN2) &&
    (trtype2 == TRTYPE_WVETERAN || trtype2 == TRTYPE_WVETERAN2) )
  {
    return TRTYPE_WVETERANBASIS;
  }
  //スポーツきょうだい
  if( (trtype1 == TRTYPE_WSPORTSM || trtype1 == TRTYPE_WSPORTSW) &&
    (trtype2 == TRTYPE_WSPORTSM || trtype2 == TRTYPE_WSPORTSW) )
  {
    return TRTYPE_WSPORTSBASIS;
  }
  //ビキニギャルズ
  if( (trtype1 == TRTYPE_WSWIMMERW1 || trtype1 == TRTYPE_WSWIMMERW2) &&
    (trtype2 == TRTYPE_WSWIMMERW1 || trtype2 == TRTYPE_WSWIMMERW2) )
  {
    return TRTYPE_WSWIMMERWBASIS;
  }
  //ゴルファーズ
  if( (trtype1 == TRTYPE_WGOLFERW ) &&
    (trtype2 == TRTYPE_WGOLFERW ) )
  {
    return TRTYPE_WGOLFERWBASIS;
  }
  return -1;
}



} // end of namespace 'trainer'

