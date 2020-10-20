//============================================================================================
/**
 * @file    BattleRecorderSaveData.cpp
 * @brief   バトルレコーダーセーブデータ
 * @author  sogabe
 * @date    12.09.20
 */
//============================================================================================

//#include "battle/btl_net.h"
//#include "battle/battle_SetupParam.h"
#include "ExtSavedata/include/BattleRecorderSaveData.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>
//#include "message/message.gaix"
#include "GameSys/include/NijiTime.h"
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

namespace ExtSavedata {

//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
BattleRecorderSaveData::BattleRecorderSaveData( gfl2::heap::HeapBase* heap, GameSys::GameManager* gameMgr )
{
  //STATIC_ASSERT( sizeof(BATTLE_REC_DATA) == 11872 );

  mHeap              = heap;
  mBattleRecData     = ( BATTLE_REC_DATA* )GflHeapAllocMemory( heap, sizeof( BATTLE_REC_DATA ) );
  mGameManager       = gameMgr;
  mIsLoadData        = false;
  mFileIndex         = NONE_FILE_INDEX;
  mFileCount         = NONE_FILE_COUNT;
  mIsAsync           = false;
  mReadContinue      = false;   ///<妥当性チェック継続版かどうか
  mContinueCount     = 0;       ///<妥当性チェック継続版カウント数
  mContinueIndex     = 0;       ///<妥当性チェック継続版インデックス
  m_waitCount = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
BattleRecorderSaveData::~BattleRecorderSaveData()
{
  GflHeapFreeMemory( mBattleRecData );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   BattleSetupParamとRegulationからバトルレコーダーセーブデータを生成
 *
 * @param[in] bsp BattleSetupParam
 * @param[in] reg Regulation
 *
 * @retval  true:生成成功　false:生成失敗
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CreateSaveData( const BATTLE_SETUP_PARAM* bsp, const Regulation* reg )
{
  if( bsp->recBuffer == NULL ) return false;
  // 天球使用の戦闘は録画できない（本来この処理は必要ないはず）
  //if( bsp->fieldSituation.bgComponent.skyboxFlag != false ) return false;

  gfl2::str::StrBuf* name = GFL_NEW_LOW( mHeap ) gfl2::str::StrBuf( pml::PERSON_NAME_BUFFER_SIZE, mHeap );

  //録画セーブデータヘッダー生成
  //対戦施設種別reg
  mBattleRecData->upload.header.category     = reg->GetBattleCategory();
  //対戦モード
  mBattleRecData->upload.header.rule         = reg->GetBattleRule();
  mBattleRecData->upload.header.magic_key    = REC_MAGIC_KEY;
  //逆さバトル
  mBattleRecData->upload.header.sakasaBattle = reg->GetBoolParam( Regulation::REVERSE );

  //  勝敗結果を設定
  {
    mBattleRecData->upload.header.battle_result1  = bsp->recHeader.result1;
    mBattleRecData->upload.header.battle_result2  = bsp->recHeader.result2;
    mBattleRecData->upload.header.end_turn_num    = bsp->recHeader.turnNum;
  }

  //ロイヤル順位
  mBattleRecData->upload.header.royal_result = bsp->battleRoyalResult.clientRanking[ bsp->commPos ];

  gfl2::str::StrBuf buf( sizeof(mBattleRecData->upload.header.cupname1) , mHeap );

  reg->GetTextParam( Regulation::CUPNAME1 , &buf );
  ::std::memcpy( mBattleRecData->upload.header.cupname1 , buf.GetPtr() , sizeof(mBattleRecData->upload.header.cupname1) );

  if( reg->GetBattleCategory() == Regulation::BATTLE_CATEGORY_DELIVERY )
  {//配信レギュレーションの場合の名前取得
    reg->GetTextParam( Regulation::DELIVERY_NAME , &buf );
  }
  else
  {//配信レギュレーション以外はデフォルトで大会名を入れる
    reg->GetTextParam( Regulation::CUPNAME2 , &buf );
  }
  

  ::std::memcpy( mBattleRecData->upload.header.cupname2 , buf.GetPtr() , sizeof(mBattleRecData->upload.header.cupname2) );

  //PSSアイコン系
  for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
  {
    if( bsp->playerStatus[ i ] != NULL )
    {
      //アイコン兼着せ替えデータ
      mBattleRecData->upload.header.pssIcon[ i ]  = bsp->playerStatus[ i ]->GetIcon();

      bsp->playerStatus[ i ]->GetNameString( name );
      name->PutStr( mBattleRecData->upload.header.name[ i ], pml::PERSON_NAME_BUFFER_SIZE );
    }

    if( bsp->tr_data[ i ] != NULL )
    {
      mBattleRecData->upload.header.trainer_id[ i ] = bsp->tr_data[ i ]->GetTrainerType();
      GFL_PRINT("Set AI TrainerID : index[%d]id[%d]\n" , i , mBattleRecData->upload.header.trainer_id[ i ] );
      if( trainer::TRTYPE_HEROINE < mBattleRecData->upload.header.trainer_id[ i ] )
      {
        if( bsp->tr_data[ i ]->GetName() )
        {
          bsp->tr_data[ i ]->GetName()->PutStr( mBattleRecData->upload.header.name[ i ], pml::PERSON_NAME_BUFFER_SIZE );
        }
        else
        {
          GFL_ASSERT(0); //@fix
        }
      }
    }
    else
    {
      mBattleRecData->upload.header.trainer_id[ i ] = 0;
    }
    
  }

  //通常とマルチでポケパーティからの取り出し方が違う
  {
    if( bsp->multiMode )
    {
      for( s32 i = 0 ; i < pml::PokeParty::MAX_MEMBERS / 2; i++ )
      {
        const pml::pokepara::PokemonParam* pp_player1 = bsp->party[ BTL_CLIENT_PLAYER ]->GetMemberPointerConst( i );
        const pml::pokepara::PokemonParam* pp_enemy1  = bsp->party[ BTL_CLIENT_ENEMY1 ]->GetMemberPointerConst( i );
        const pml::pokepara::PokemonParam* pp_player2 = bsp->party[ BTL_CLIENT_PARTNER ]->GetMemberPointerConst( i );
        const pml::pokepara::PokemonParam* pp_enemy2  = bsp->party[ BTL_CLIENT_ENEMY2 ]->GetMemberPointerConst( i );

        mBattleRecData->upload.header.mons_no[ i ] = pp_player1->GetMonsNo();
        mBattleRecData->upload.header.form_no[ i ] = pp_player1->GetFormNo();
        mBattleRecData->upload.header.gender[ i ]  = pp_player1->GetSex();
        mBattleRecData->upload.header.rare[ i ]    = pp_player1->IsRare();

        mBattleRecData->upload.header.mons_no[ i + PP_HALF ] = pp_player2->GetMonsNo();
        mBattleRecData->upload.header.form_no[ i + PP_HALF ] = pp_player2->GetFormNo();
        mBattleRecData->upload.header.gender[ i + PP_HALF ]  = pp_player2->GetSex();
        mBattleRecData->upload.header.rare[ i + PP_HALF ]    = pp_player2->IsRare();

        mBattleRecData->upload.header.mons_no[ i + PP_FULL ] = pp_enemy2->GetMonsNo();
        mBattleRecData->upload.header.form_no[ i + PP_FULL ] = pp_enemy2->GetFormNo();
        mBattleRecData->upload.header.gender[ i + PP_FULL ]  = pp_enemy2->GetSex();
        mBattleRecData->upload.header.rare[ i + PP_FULL ]    = pp_enemy2->IsRare();
        
        mBattleRecData->upload.header.mons_no[ i + PP_FULL + PP_HALF ] = pp_enemy1->GetMonsNo();
        mBattleRecData->upload.header.form_no[ i + PP_FULL + PP_HALF ] = pp_enemy1->GetFormNo();
        mBattleRecData->upload.header.gender[ i + PP_FULL + PP_HALF ]  = pp_enemy1->GetSex();
        mBattleRecData->upload.header.rare[ i + PP_FULL + PP_HALF ]    = pp_enemy1->IsRare();
      }
    }
    else
    {
      if( bsp->rule == BTL_RULE_ROYAL )
      {
        for( s32 i = 0 ; i < pml::PokeParty::MAX_MEMBERS / 2; i++ )
        {
          const pml::pokepara::PokemonParam* pp_player1 = bsp->party[ BTL_CLIENT_PLAYER ]->GetMemberPointerConst( i );
          const pml::pokepara::PokemonParam* pp_enemy1  = bsp->party[ BTL_CLIENT_ENEMY1 ]->GetMemberPointerConst( i );
          const pml::pokepara::PokemonParam* pp_player2 = bsp->party[ BTL_CLIENT_PARTNER ]->GetMemberPointerConst( i );
          const pml::pokepara::PokemonParam* pp_enemy2  = bsp->party[ BTL_CLIENT_ENEMY2 ]->GetMemberPointerConst( i );

          mBattleRecData->upload.header.mons_no[ i ] = pp_player1->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i ] = pp_player1->GetFormNo();
          mBattleRecData->upload.header.gender[ i ]  = pp_player1->GetSex();
          mBattleRecData->upload.header.rare[ i ]    = pp_player1->IsRare();

          mBattleRecData->upload.header.mons_no[ i + PP_HALF ] = pp_player2->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i + PP_HALF ] = pp_player2->GetFormNo();
          mBattleRecData->upload.header.gender[ i + PP_HALF ]  = pp_player2->GetSex();
          mBattleRecData->upload.header.rare[ i + PP_HALF ]    = pp_player2->IsRare();

          mBattleRecData->upload.header.mons_no[ i + PP_FULL ] = pp_enemy2->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i + PP_FULL ] = pp_enemy2->GetFormNo();
          mBattleRecData->upload.header.gender[ i + PP_FULL ]  = pp_enemy2->GetSex();
          mBattleRecData->upload.header.rare[ i + PP_FULL ]    = pp_enemy2->IsRare();

          mBattleRecData->upload.header.mons_no[ i + PP_FULL + PP_HALF ] = pp_enemy1->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i + PP_FULL + PP_HALF ] = pp_enemy1->GetFormNo();
          mBattleRecData->upload.header.gender[ i + PP_FULL + PP_HALF ]  = pp_enemy1->GetSex();
          mBattleRecData->upload.header.rare[ i + PP_FULL + PP_HALF ]    = pp_enemy1->IsRare();
        }
      }
      else
      {
        for( s32 i = 0 ; i < pml::PokeParty::MAX_MEMBERS ; i++ )
        {
          const pml::pokepara::PokemonParam* pp_player = bsp->party[ BTL_CLIENT_PLAYER ]->GetMemberPointerConst( i );
          const pml::pokepara::PokemonParam* pp_enemy  = bsp->party[ BTL_CLIENT_ENEMY1 ]->GetMemberPointerConst( i );

          mBattleRecData->upload.header.mons_no[ i ] = pp_player->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i ] = pp_player->GetFormNo();
          mBattleRecData->upload.header.gender[ i ]  = pp_player->GetSex();
          mBattleRecData->upload.header.rare[ i ]    = pp_player->IsRare();

          mBattleRecData->upload.header.mons_no[ i + PP_FULL ] = pp_enemy->GetMonsNo();
          mBattleRecData->upload.header.form_no[ i + PP_FULL ] = pp_enemy->GetFormNo();
          mBattleRecData->upload.header.gender[ i + PP_FULL ]  = pp_enemy->GetSex();
          mBattleRecData->upload.header.rare[ i + PP_FULL ]    = pp_enemy->IsRare();
        }
      }
    }
  }

  mBattleRecData->upload.header.battle_counter = 0; //連勝数

  //通信時は接続マシン中の最新バージョンになるので、取得方法が違う
  if( bsp->commMode )
  {
    mBattleRecData->upload.header.server_version = bsp->commServerVer;
  }
  else
  {
    mBattleRecData->upload.header.server_version = BTL_NET_SERVER_VERSION;
  }

  // 戦闘背景
  mBattleRecData->upload.header.background = reg->GetBackGroundType();

#if defined(GF_PLATFORM_CTR)
  //CRCを生成
  mBattleRecData->upload.header.crc = nn::util::Crc32::Calculate( &mBattleRecData->upload.header,
                                                                  sizeof( BATTLE_REC_HEADER ) -
                                                                  sizeof( mBattleRecData->upload.header.crc ) );
#endif
  //録画セーブデータ本体生成
  //ランダムの種
  mBattleRecData->upload.body.setupSubset.recRandSeed = bsp->recRandSeed;

  //画面構築用情報
  mBattleRecData->upload.body.setupSubset.fieldSituation = bsp->fieldSituation;
  mBattleRecData->upload.body.setupSubset.fieldSituation.fix_time = true;//時間固定ON

  //バトル演出パラメータ
  mBattleRecData->upload.body.setupSubset.battleEffect   = bsp->btlEffData;

  //ユーザコンフィグ
  if( bsp->configData != NULL )
  {
    memcpy( &mBattleRecData->upload.body.setupSubset.config, bsp->configData->GetData(), bsp->configData->GetDataSize() );
  }

  //シューター使用制限パラメータ
  mBattleRecData->upload.body.setupSubset.shooterBitWork = bsp->shooterBitWork;

  //バトルルール
  mBattleRecData->upload.body.setupSubset.competitor   = bsp->competitor;
  mBattleRecData->upload.body.setupSubset.rule         = bsp->rule;
  mBattleRecData->upload.body.setupSubset.multiMode    = bsp->multiMode;
  mBattleRecData->upload.body.setupSubset.skyBattle    = bsp->bSkyBattle;
  mBattleRecData->upload.body.setupSubset.sakasaBattle = bsp->bSakasaBattle;

  //デバッグ用フラグ　なんで入れてるかわかんない（WB準拠）
  mBattleRecData->upload.body.setupSubset.debugFlag = bsp->DebugFlagBit;
  //通信対戦時の自分の立ち位置
  mBattleRecData->upload.body.setupSubset.commPos = bsp->commPos;
  //バトルフェス専用フィールド効果
  if (reg->GetBattleCategory() == Regulation::BATTLE_CATEGORY_BATTLE_FES)
  {
    mBattleRecData->upload.body.setupSubset.btlFesFieldType = bsp->btlFesFieldType;
  }
  else
  {
    mBattleRecData->upload.body.setupSubset.btlFesFieldType = 0;
  }

  //録画データ
  mBattleRecData->upload.body.opBuffer.size = bsp->recDataSize;
  memcpy( mBattleRecData->upload.body.opBuffer.buffer, bsp->recBuffer, BTLREC_OPERATION_BUFFER_SIZE );

  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      //手持ちポケモン
      if( bsp->party[ i ] != NULL )
      {
        mBattleRecData->upload.body.rec_party[ i ].is_exist = true;
        bsp->party[ i ]->SerializeCore( &mBattleRecData->upload.body.rec_party[ i ].members );
      }
      else
      {
        mBattleRecData->upload.body.rec_party[ i ].is_exist = false;
      }
      //MyStatus
      if( bsp->playerStatus[ i ] != NULL )
      {
        mBattleRecData->upload.body.myStatus[ i ].is_exist = true;
        bsp->playerStatus[ i ]->Serialize( mBattleRecData->upload.body.myStatus[ i ].status );
      }
      else
      {
        mBattleRecData->upload.body.myStatus[ i ].is_exist = false;
      }
    }
  }

  //トレーナーデータ
  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      if( bsp->tr_data[ i ] != NULL )
      {
        bsp->tr_data[ i ]->Serialize( &mBattleRecData->upload.body.tr_data[ i ] );
      }
      else
      {
        BSP_TRAINER_DATA::ClearSerializeData( &mBattleRecData->upload.body.tr_data[ i ] );
      }
    }
  }

  mBattleRecData->upload.body.magic_key = REC_MAGIC_KEY;
  mBattleRecData->upload.body.padding   = 0;
#if defined(GF_PLATFORM_CTR)
  //CRCを生成
  mBattleRecData->upload.body.crc = nn::util::Crc32::Calculate( &mBattleRecData->upload.body,
                                                                sizeof( BATTLE_REC_BODY ) -
                                                                sizeof( mBattleRecData->upload.body.crc ) );
#endif
  //SDカードにセーブするフッタデータ生成
  mBattleRecData->upload_flag = false;
  mBattleRecData->dataID      = 0;
  mBattleRecData->playCount   = 0;
  mBattleRecData->registDate.year  = 2000;
  mBattleRecData->registDate.month = 1;
  mBattleRecData->registDate.day   = 1;
  mBattleRecData->registDate.hour  = 0;
  mBattleRecData->registDate.minute= 0;
  mBattleRecData->registDate.second= 0;

  // 保存日時
  GameSys::DeviceDate now;

  mBattleRecData->createDate.year  = now.GetYear();
  mBattleRecData->createDate.month = now.GetMonth();
  mBattleRecData->createDate.day   = now.GetDay();
  mBattleRecData->createDate.hour  = now.GetHour();
  mBattleRecData->createDate.minute= now.GetMinute();
  mBattleRecData->createDate.second= now.GetSecond();

  mIsLoadData = true;
  mFileIndex  = NONE_FILE_INDEX;
  GFL_DELETE name;

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードからバトルレコーダーセーブデータをロード
 *
 * @param[in] index  ファイルインデックス
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result  BattleRecorderSaveData::LoadData( int index )
{
  ExtSavedata::BattleRecorderSaveData::Result result = RESULT_FAILED_NO_EXIST;
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) != false )
  {
    result = GetSaveLoadResult( xess->ReadFile( EXTID_BATTLEVIDEO, index, mBattleRecData, sizeof( BATTLE_REC_DATA ) ) );
    if( result == RESULT_SUCCESS )
    {
      mIsLoadData = true;
      mFileIndex  = index;
    }
  }
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードからライブ大会用セーブデータをロード
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::LiveCupLoadData( void )
{
  // ライブ大会専用セーブインデックス
  int index = LIVE_SAVE_INDEX;

  ExtSavedata::BattleRecorderSaveData::Result result = RESULT_FAILED_NO_EXIST;
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) != false )
  {
    result = GetSaveLoadResult( xess->ReadFile( EXTID_BATTLEVIDEO, index, mBattleRecData, sizeof( BATTLE_REC_DATA ) ) );
    if( result == RESULT_SUCCESS )
    {
      mIsLoadData = true;
    }
  }
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードにバトルレコーダーセーブデータをセーブ
 *          空いているインデックスをサーチしてセーブ
 *          すでにセーブしたことのあるデータなら、上書きセーブ
 *
 * @param[in] is_async  非同期セーブにするかどうか(true:非同期 false:同期)　デフォルト：false
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::SaveData( bool is_async )
{
  // データが未初期化
  if( mIsLoadData == false )
    return RESULT_FAILED;

  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( mFileIndex == NONE_FILE_INDEX )
  {
    for( s32 index = 0 ; index < MAX_SAVE_DATA ; index++ )
    {
      if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) == false )
      {
        return SaveData( index, is_async );
      }
    }
    return RESULT_FAILED_FULL;
  }
  return SaveData( mFileIndex, is_async );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードにバトルレコーダーセーブデータをセーブ
 *
 * @param[in] index     ファイルインデックス
 * @param[in] is_async  非同期セーブにするかどうか(true:非同期 false:同期)　デフォルト：false
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::SaveData( int index, bool is_async )
{
  // データが未初期化
  if( mIsLoadData == false )
    return RESULT_FAILED;

  //まだ非同期待ち中なのに同期待ちが呼ばれた
  if( ( is_async == false ) && ( mIsAsync != false ) )
  { 
    return RESULT_FAILED_WAIT_ASYNC;
  }

  ExtSavedata::BattleRecorderSaveData::Result result = RESULT_SUCCESS;
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( index >= MAX_SAVE_DATA ) return RESULT_FAILED_FULL;

  if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) == false )
  {
    result = GetSaveLoadResult( xess->CreateExtFile( EXTID_BATTLEVIDEO, index, sizeof( BATTLE_REC_DATA ) ) );
  }
  //セーブする→Create/ファイル破壊の可能性があるので、再カウント
  mFileCount = NONE_FILE_COUNT;
  if( IsSuccess( result ) != false )
  {
    if( is_async != false )
    { 
      if( xess->StartWriteFile( mHeap, EXTID_BATTLEVIDEO, index, mBattleRecData, sizeof( BATTLE_REC_DATA ) ) == false )
      { 
        result = RESULT_FAILED;
      }
      else
      { 
        m_waitCount = 0;
        mIsAsync = true;
        result   = RESULT_WAIT_ASYNC;
      }
    }
    else
    { 
      result = GetSaveLoadResult( xess->WriteFile( EXTID_BATTLEVIDEO, index, mBattleRecData,
                                                   sizeof( BATTLE_REC_DATA ) ), true );
      mIsAsync = false;
    }
    if( IsSuccess( result ) != false )
    {
      mFileIndex = index;
    }
    else
    {
      mFileIndex = NONE_FILE_INDEX;
    }
  }
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   非同期セーブ時の処理待ち関数
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::WaitAsync( bool isDummyWaitSkip )
{ 
  //非同期開始していないのに処理待ちが呼ばれた
  if( !mIsAsync ) return RESULT_FAILED_NO_ASYNC;

  if( isDummyWaitSkip == false )
  {
    if( m_waitCount < SD_WAIT_COUNT )
    {
      m_waitCount++;
      return RESULT_WAIT_ASYNC;
    }
  }


  gfl2::fs::ExtSaveFile::Result result;
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( xess->WaitWriteFile( &result ) )
  { 
    mIsAsync = false;
    return  GetSaveLoadResult( result, true );
  }
  else
  { 
    return  RESULT_WAIT_ASYNC;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードにライブ大会専用セーブデータをセーブ
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::LiveCupSaveData( void )
{
  // ライブ大会専用セーブインデックス
  int index = LIVE_SAVE_INDEX;

  // データが未初期化
  if( mIsLoadData == false )
    return RESULT_FAILED;

  ExtSavedata::BattleRecorderSaveData::Result result = RESULT_SUCCESS;
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) == false )
  {
    result = GetSaveLoadResult( xess->CreateExtFile( EXTID_BATTLEVIDEO, index, sizeof( BATTLE_REC_DATA ) ) );
  }

  if( IsSuccess( result ) != false )
  {
    result = GetSaveLoadResult( xess->WriteFile( EXTID_BATTLEVIDEO, index, mBattleRecData, sizeof( BATTLE_REC_DATA ) ), true );
  }
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードからバトルレコーダーセーブデータを削除
 *
 * @param[in] index  ファイルインデックス
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::DeleteData( int index )
{
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  //Deleteする→再カウント
  mFileCount = NONE_FILE_COUNT;
  return GetSaveLoadResult( xess->DeleteExtFile( EXTID_BATTLEVIDEO, index ) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   SDカードからライブ大会専用セーブデータを削除
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 * 
 * @li  ライブ大会専用です
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result BattleRecorderSaveData::LiveCupDeleteData( void )
{
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();
  return GetSaveLoadResult( xess->DeleteExtFile( EXTID_BATTLEVIDEO, LIVE_SAVE_INDEX ) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   読み込んだバトルレコーダーデータからBattleSetupParamを生成
 *
 * @param[in] bsp   BattleSetupParam
 *
 * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CreateBattleSetupParam( BATTLE_SETUP_PARAM* bsp, const Savedata::ConfigSave* configData )
{
  if( mIsLoadData == false ) return false;

  //一応初期化しておく
  BATTLE_SETUP_Init( bsp );

  btl::setup::common_base( bsp, mGameManager, &mBattleRecData->upload.body.setupSubset.fieldSituation );

  ///< 録画データ再生モード
  bsp->recordDataMode = BTL_RECORDDATA_PLAY;

  //ランダムの種
  bsp->recRandSeed = mBattleRecData->upload.body.setupSubset.recRandSeed;

  //画面構築用情報
  bsp->fieldSituation = mBattleRecData->upload.body.setupSubset.fieldSituation;

  //バトル演出パラメータ
  bsp->btlEffData = mBattleRecData->upload.body.setupSubset.battleEffect;

  //ユーザコンフィグ 解放忘れに気を付ける
  {
    bsp->configData = GFL_NEW( mHeap ) Savedata::ConfigSave();
    bsp->configData->SetData( &mBattleRecData->upload.body.setupSubset.config );
    if( configData ){
      bsp->configData->SetBattleAnimation( configData->GetBattleAnimation() );
      bsp->configData->SetTalkingSpeed( configData->GetTalkingSpeed() );
    }
  }

  //シューター使用制限パラメータ
  bsp->shooterBitWork = mBattleRecData->upload.body.setupSubset.shooterBitWork;

  //バトルルール
  bsp->competitor    = mBattleRecData->upload.body.setupSubset.competitor;
  bsp->rule          = mBattleRecData->upload.body.setupSubset.rule;
  bsp->multiMode     = mBattleRecData->upload.body.setupSubset.multiMode;
  bsp->bSkyBattle    = mBattleRecData->upload.body.setupSubset.skyBattle;
  bsp->bSakasaBattle = mBattleRecData->upload.body.setupSubset.sakasaBattle;

  // 戦闘背景
  if( mBattleRecData->upload.header.background == Regulation::BACKGROUND_WCS1 )
  {//WCSバージョン1
    // battleEffectは保存されているものをそのまま使用でOK
    GFL_PRINT("WCS1設定\n");
  }
  else if( mBattleRecData->upload.header.background == Regulation::BACKGROUND_WCS2 )
  {//WCSバージョン2
    // battleEffectは保存されているものをそのまま使用でOK
    GFL_PRINT("WCS2設定\n");
  }

  // バトルフェス用フラグ
  if( mBattleRecData->upload.header.category == Regulation::BATTLE_CATEGORY_BATTLE_FES )
  {
    BATTLE_PARAM_SetBtlStatusFlag( bsp, BTL_STATUS_FLAG_BATTLE_FES );
    GFL_PRINT( "バトルフェス用フラグ設定\n" );
    bsp->btlFesFieldType = static_cast<BtlFesField>(mBattleRecData->upload.body.setupSubset.btlFesFieldType);
  }

  //デバッグ用フラグ　なんで入れてるかわかんない（WB準拠）
  bsp->DebugFlagBit = mBattleRecData->upload.body.setupSubset.debugFlag;
  //通信対戦時の自分の立ち位置
  bsp->commPos = mBattleRecData->upload.body.setupSubset.commPos;

  //録画データ
  bsp->recDataSize = mBattleRecData->upload.body.opBuffer.size;
  BATTLE_PARAM_AllocRecBuffer( bsp, mHeap );
  memcpy( bsp->recBuffer, mBattleRecData->upload.body.opBuffer.buffer, BTLREC_OPERATION_BUFFER_SIZE );

  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      if( mBattleRecData->upload.body.rec_party[ i ].is_exist != false )
      {
        //手持ちポケモン 解放忘れに気を付ける
        bsp->party[ i ] = GFL_NEW( mHeap ) pml::PokeParty( mHeap );
        bsp->party[ i ]->DeserializeCore( &mBattleRecData->upload.body.rec_party[ i ].members );

        //不正文字置換処理
        for( u32 replaceIndex = 0 ; replaceIndex < bsp->party[ i ]->GetMemberCount() ; replaceIndex++ )
        {
          #if defined(GF_PLATFORM_CTR)
          NetLib::Validation::PokemonValidation::ReplaceMonsName( bsp->party[i]->GetMemberPointer( replaceIndex ) );
          #endif
        }
      }
      if( mBattleRecData->upload.body.myStatus[ i ].is_exist != false )
      {
        //MyStatus 解放忘れに気を付ける
        bsp->playerStatus[ i ] = GFL_NEW( mHeap ) Savedata::MyStatus();
        bsp->playerStatus[ i ]->Deserialize( mBattleRecData->upload.body.myStatus[ i ].status );
      }
    }
  }

  //トレーナーデータ
  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      //解放忘れに気を付ける
      bsp->tr_data[ i ] = GFL_NEW( mHeap ) BSP_TRAINER_DATA( mHeap );
      bsp->tr_data[ i ]->Deserialize( &mBattleRecData->upload.body.tr_data[ i ] );
      bsp->tr_data[ i ]->ReloadTrTypeData( mHeap ); //トレーナータイプデータの再読み込み（名前などを自言語で上書き）
    }
  }

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ビデオデータ取得
 *
 * @retval  ビデオデータ
 */
//--------------------------------------------------------------------------------------------
const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* BattleRecorderSaveData::GetBattleRecData( void ) const
{
  GFL_ASSERT_STOP( mBattleRecData != NULL ); //@fix
  return mBattleRecData;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ビデオデータ取得
 *
 * @retval  ビデオデータ
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* BattleRecorderSaveData::GetBattleRecDataDirect( void )
{
  GFL_ASSERT_STOP( mBattleRecData != NULL ); //@fix
  return mBattleRecData;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アップロードデータ取得
 *
 * @retval  アップロードデータ
 */
//--------------------------------------------------------------------------------------------
const BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* BattleRecorderSaveData::GetBattleRecUploadData( void ) const
{
  GFL_ASSERT_STOP( mBattleRecData != NULL ); //@fix
  return &mBattleRecData->upload;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アップロードデータを設定
 *
 * @param   upload  アップロードデータ
 */
//--------------------------------------------------------------------------------------------
void BattleRecorderSaveData::SetBattleRecUploadData( BATTLE_REC_UPLOAD_DATA * upload )
{
  ::std::memcpy( &mBattleRecData->upload , upload , sizeof(BATTLE_REC_UPLOAD_DATA) );
  mIsLoadData = true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   連勝数セット
 *
 * @param[in] value セットする連勝数
 */
//--------------------------------------------------------------------------------------------
void  BattleRecorderSaveData::SetBattleCounter( s32 value )
{
  if( mBattleRecData == NULL )
  {
    GFL_ASSERT( 0 ); //@fix
    return;
  }
  mBattleRecData->upload.header.battle_counter = value;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルビデオIDセット
 */
//--------------------------------------------------------------------------------------------
void  BattleRecorderSaveData::SetBattleVideoID( u64 battle_video_id )
{
  if( mBattleRecData == NULL )
  {
    GFL_ASSERT(0);  //@fix
    return;
  }
  mBattleRecData->dataID = battle_video_id;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルビデオアップロードフラグセット
 */
//--------------------------------------------------------------------------------------------
void  BattleRecorderSaveData::SetBattleVideoUploadFlag( bool upload_flag )
{
  if( mBattleRecData == NULL )
  {
    GFL_ASSERT(0);  //@fix
    return;
  }
  mBattleRecData->upload_flag = upload_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief  バトルビデオの再生回数をセット
 * @param  count  再生回数
 */
//--------------------------------------------------------------------------
void BattleRecorderSaveData::SetBattleVideoPlayCount( const u32 count )
{
  if( mBattleRecData == NULL )
  {
    GFL_ASSERT(0);  //@fix
    return;
  }
  mBattleRecData->playCount = count;
}

//--------------------------------------------------------------------------
/**
 * @brief   バトルビデオアップロード登録日をセット
 * @param   date     日時
 */
//--------------------------------------------------------------------------
void BattleRecorderSaveData::SetBattleVideoRegistDate( const gfl2::system::Date* date )
{
  if( mBattleRecData == NULL )
  {
    GFL_ASSERT(0);  //@fix
    return;
  }

  mBattleRecData->registDate.year  = date->GetYear();
  mBattleRecData->registDate.month = date->GetMonth();
  mBattleRecData->registDate.day   = date->GetDay();
  mBattleRecData->registDate.hour  = date->GetHour();
  mBattleRecData->registDate.minute= date->GetMinute();
  mBattleRecData->registDate.second= date->GetSecond();
  ICHI_PRINT("登録日セット %d/%d/%d %d:%d:%d \n",
             mBattleRecData->registDate.year, mBattleRecData->registDate.month, mBattleRecData->registDate.day,
             mBattleRecData->registDate.hour, mBattleRecData->registDate.minute, mBattleRecData->registDate.second );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   有効データの中の位置から、SDカード上での位置を取得する
 *
 * @param   pos     有効データの中の位置  0 <= pos < GetBattleVideoCount()
 *
 * @return  SDカード上での位置
 */
//--------------------------------------------------------------------------------------------
s32  BattleRecorderSaveData::GetBattleVideoIndex( s32 pos )
{
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();
  s32 count = 0;
  for( s32 index = 0 ; index < MAX_SAVE_DATA ; index++ )
  {
    if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) != false )
    {
      if( pos == count )
        return index;
      count++;
    }
  }
  return 0;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルビデオセーブ件数取得
 */
//--------------------------------------------------------------------------------------------
s32  BattleRecorderSaveData::GetBattleVideoCount( void )
{
  if ( mFileCount == NONE_FILE_COUNT ) return CountBattleVideo();
  return mFileCount;
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
s32 BattleRecorderSaveData::CountBattleVideo( void )
{
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();
  s32 count = 0;
  for( s32 index = 0 ; index < MAX_SAVE_DATA ; index++ )
  {
    if( xess->IsFileExists( EXTID_BATTLEVIDEO, index ) != false )
    {
      count++;
    }
  }
  mFileCount = count;
  return count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   全セーブデータの妥当性チェック
 *
 * @param heap  妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
 *
 * @retval  true:正常 false:壊れたファイルが存在したので、消去した
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CheckAllSaveDataValidity( gfl2::heap::HeapBase* heap )
{
  bool  result = true;

  //録画バッファテンポラリを確保
  if( heap == NULL )
  { 
    heap = mHeap;
  }
  BATTLE_REC_DATA*  recDataTemp = ( BATTLE_REC_DATA* )GflHeapAllocMemory( heap, sizeof( BATTLE_REC_DATA ) );

  //ロードを介するので、録画バッファをテンポラリに退避
  ::std::memcpy( recDataTemp , mBattleRecData,  sizeof(BATTLE_REC_DATA) );

  for( s32 index = 0 ; index < MAX_SAVE_DATA ; index++ )
  {
    if( LoadData( index ) == RESULT_FAILED_DATA_BROKEN )
    {
      result = false;
      DeleteData( index );
    }
  }

  //退避していた録画バッファを戻す
  ::std::memcpy( mBattleRecData,recDataTemp, sizeof(BATTLE_REC_DATA) );

  //LoadDataで読み込み済みと書き換えられてしまうので、未読み込み状態に戻す
  mFileIndex = NONE_FILE_INDEX;

  //録画バッファテンポラリを解放
  GflHeapFreeMemory( recDataTemp );

  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   全セーブデータの妥当性チェック（指定した個数チェックしたらリターンする版）
 *
 * @param[in]   heap    妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
 * @param[out]  result  チェック結果を格納するワークのポインタ  true:正常 false:壊れたファイルが存在したので、消去した
 * @param[in]   count   チェックする個数  初回は個数を指定する  継続では無視されます
 * @param[in]   is_init 初回初期化はtrue  継続ではfalseを指定
 *
 * @retval  true:継続中 false:チェック終了
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CheckAllSaveDataValidityContinue( gfl2::heap::HeapBase* heap, bool* result, s32 count, bool is_init )
{ 
  if( is_init != false )
  { 
    mReadContinue   = true;
    mContinueCount  = ( count != 0 ) ? count : 1 ;
    mContinueIndex  = 0;
    mContinueResult = true;
  }

  //継続の初回初期化をしていないとあぶないので、チェック終了で返す
  if( ( mReadContinue == false ) && ( is_init == false ) )
  { 
    GFL_ASSERT( 0 );  //@check  継続の初回初期化をしていないとあぶないので、チェック終了で返す
    return false;
  }

  bool is_continue = true;

  //録画バッファテンポラリを確保
  if( heap == NULL )
  { 
    heap = mHeap;
  }
  BATTLE_REC_DATA*  recDataTemp = ( BATTLE_REC_DATA* )GflHeapAllocMemory( heap, sizeof( BATTLE_REC_DATA ) );

  //ロードを介するので、録画バッファをテンポラリに退避
  ::std::memcpy( recDataTemp , mBattleRecData,  sizeof(BATTLE_REC_DATA) );

  s32 index;

  for( index = mContinueIndex ; index < mContinueIndex + mContinueCount ; index++ )
  {
    if( LoadData( index ) == RESULT_FAILED_DATA_BROKEN )
    {
      mContinueResult = false;
      DeleteData( index );
    }
    if( index + 1 >= MAX_SAVE_DATA )
    { 
      is_continue   = false;
      mReadContinue = false;
      break;
    }
  }

  mContinueIndex = index;

  //退避していた録画バッファを戻す
  ::std::memcpy(  mBattleRecData,recDataTemp, sizeof(BATTLE_REC_DATA) );

  //LoadDataで読み込み済みと書き換えられてしまうので、未読み込み状態に戻す
  mFileIndex = NONE_FILE_INDEX;

  //録画バッファテンポラリを解放
  GflHeapFreeMemory( recDataTemp );

  if( result != NULL )
  { 
    *result = mContinueResult;
  }

  return is_continue;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ライブ大会のセーブデータの妥当性チェック
 *
 * @param heap  妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
 *
 * @retval  true:正常 false:壊れたファイルが存在したので、消去した
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CheckLiveCupSaveDataValidity( gfl2::heap::HeapBase* heap )
{
  bool  result = true;

  //録画バッファテンポラリを確保
  if( heap == NULL )
  { 
    heap = mHeap;
  }
  BATTLE_REC_DATA*  recDataTemp = ( BATTLE_REC_DATA* )GflHeapAllocMemory( heap, sizeof( BATTLE_REC_DATA ) );

  //ロードを介するので、録画バッファをテンポラリに退避
  ::std::memcpy(  recDataTemp,mBattleRecData, sizeof(BATTLE_REC_DATA) );

  if( LiveCupLoadData() == RESULT_FAILED_DATA_BROKEN )
  {
    result = false;
    LiveCupDeleteData();
  }

  //退避していた録画バッファを戻す
  ::std::memcpy(  mBattleRecData,recDataTemp, sizeof(BATTLE_REC_DATA) );

  //LoadDataで読み込み済みと書き換えられてしまうので、未読み込み状態に戻す
  mFileIndex = NONE_FILE_INDEX;

  //録画バッファテンポラリを解放
  GflHeapFreeMemory( recDataTemp );

  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   セーブロード結果からバトルビデオ独自の結果を返す
 *
 * @retval  ExtSavedata::BattleRecorderSaveData::Result
 */
//--------------------------------------------------------------------------------------------
ExtSavedata::BattleRecorderSaveData::Result  BattleRecorderSaveData::GetSaveLoadResult( gfl2::fs::ExtSaveFile::Result result,
                                                                                        bool is_save )
{
  ExtSavedata::BattleRecorderSaveData::Result save_load_result = RESULT_SUCCESS;

#if PM_DEBUG
#ifdef DEBUG_ONLY_FOR_ichiraku_katsuhiko
  // デバッグ：強制にする
  //{
  //  gfl::ui::DeviceManager*  pDeviceManager = GFL_SINGLETON_INSTANCE(gamesystem::GameManager)->GetUiDeviceManager();
  //  gfl::ui::Button *uiButton = pDeviceManager->GetButton(0);
  //  
  //  if( uiButton->IsHold( gfl::ui::BUTTON_UP ) )
  //  {
  //    ICHI_PRINT("デバッグでFATAL ERRORにした！\n");
  //    return RESULT_FAILED_FATAL_ERROR; //ゲーム復帰不可能なエラー
  //  }
  //  if( uiButton->IsHold( gfl::ui::BUTTON_DOWN ) )
  //  {
  //    ICHI_PRINT("デバッグでFAILED ERRORにした！\n");
  //    return RESULT_FAILED_SD_FULL; //ゲーム復帰不可能なエラー
  //  }
  //}
#endif
#endif

  if( ExtSavedata::NijiExtSaveSystem::IsFatalError( result ) != false )
  { 
    save_load_result = RESULT_FAILED_FATAL_ERROR; //ゲーム復帰不可能なエラー
  }
  else
  { 
    switch( result ){
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:      // 失敗：指定したパスが存在しない
      save_load_result = RESULT_FAILED_NO_EXIST;
      break;
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:  // 失敗：指定したパスが既に存在する
      save_load_result = RESULT_FAILED_ALREADY_EXIST;
      break;
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:         // 失敗：データが壊れているか, 改竄されている
      save_load_result = RESULT_FAILED_DATA_BROKEN;
      break;
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:       // 失敗：メディアの空き容量不足
      save_load_result = RESULT_FAILED_SD_FULL;
      break;
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                     // 成功
      save_load_result = RESULT_SUCCESS;
      if( is_save != false )
      { 
        if( GetBattleVideoCount() >= MAX_SAVE_DATA )
        {
          save_load_result = RESULT_SUCCESS_AND_FULL;
        }
      }
      break;
    default:  //ここにはこないはずなので、アサート
      GFL_ASSERT( 0 );
      save_load_result = RESULT_FAILED;
      break;
    }
  }
  return save_load_result;
}

#if PM_DEBUG
//--------------------------------------------------------------------------------------------
/**
 * @brief   読み込んだバトルレコーダーデータからBattleSetupParamを生成（デバッグ用）
 *
 * @param[in] bsp   BattleSetupParam
 * @param[in] heap  動的に確保するメモリの確保先ヒープ
 *
 * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CreateBattleSetupParamDebug( BATTLE_SETUP_PARAM* bsp, gfl2::heap::HeapBase* heap )
{
  if( mIsLoadData == false ) return false;

  //一応初期化しておく
  BATTLE_SETUP_Init( bsp );

  btl::setup::common_base( bsp, mGameManager, &mBattleRecData->upload.body.setupSubset.fieldSituation );

  ///< 録画データ再生モード
  bsp->recordDataMode = BTL_RECORDDATA_PLAY;

  //ランダムの種
  bsp->recRandSeed = mBattleRecData->upload.body.setupSubset.recRandSeed;

  //画面構築用情報
  bsp->fieldSituation = mBattleRecData->upload.body.setupSubset.fieldSituation;


  //バトル演出パラメータ
  bsp->btlEffData = mBattleRecData->upload.body.setupSubset.battleEffect;

  //ユーザコンフィグ 解放忘れに気を付ける
  {
    bsp->configData = GFL_NEW( heap ) Savedata::ConfigSave();
    bsp->configData->SetData( &mBattleRecData->upload.body.setupSubset.config );
  }

  //シューター使用制限パラメータ
  bsp->shooterBitWork = mBattleRecData->upload.body.setupSubset.shooterBitWork;

  //バトルルール
  bsp->competitor    = mBattleRecData->upload.body.setupSubset.competitor;
  bsp->rule          = mBattleRecData->upload.body.setupSubset.rule;
  bsp->multiMode     = mBattleRecData->upload.body.setupSubset.multiMode;
  bsp->bSkyBattle    = mBattleRecData->upload.body.setupSubset.skyBattle;
  bsp->bSakasaBattle = mBattleRecData->upload.body.setupSubset.sakasaBattle;

  // 戦闘背景
  if( mBattleRecData->upload.header.background == Regulation::BACKGROUND_WCS1 )
  {//WCSバージョン1
    btl::GetBgComponentData(btl::bg::FAR_TYPE_NONE,btl::bg::NEAR_TYPE_WCS1,&(bsp->fieldSituation.bgComponent));
    BATTLE_PARAM_SetBattleEffect( bsp, BATTLE_EFFECT_COMM_WCS );
  }
  else if( mBattleRecData->upload.header.background == Regulation::BACKGROUND_WCS2 )
  {//WCSバージョン2
    btl::GetBgComponentData(btl::bg::FAR_TYPE_NONE,btl::bg::NEAR_TYPE_WCS2,&(bsp->fieldSituation.bgComponent));
    BATTLE_PARAM_SetBattleEffect( bsp, BATTLE_EFFECT_COMM_WCSFINAL );

    // ステータスフラグをWCS決勝に設定
    BATTLE_PARAM_SetBtlStatusFlag( bsp, BTL_STATUS_FLAG_WCS_FINAL );
    GFL_PRINT("WCS2設定\n");
  }

  // バトルフェス用フラグ
  if (mBattleRecData->upload.header.category == Regulation::BATTLE_CATEGORY_BATTLE_FES)
  {
    BATTLE_PARAM_SetBtlStatusFlag(bsp, BTL_STATUS_FLAG_BATTLE_FES);
    GFL_PRINT("バトルフェス用フラグ設定\n");
    bsp->btlFesFieldType = static_cast<BtlFesField>(mBattleRecData->upload.body.setupSubset.btlFesFieldType);
  }

  //デバッグ用フラグ　なんで入れてるかわかんない（WB準拠）
  bsp->DebugFlagBit = mBattleRecData->upload.body.setupSubset.debugFlag;
  //通信対戦時の自分の立ち位置
  bsp->commPos = mBattleRecData->upload.body.setupSubset.commPos;

  //録画データ
  bsp->recDataSize = mBattleRecData->upload.body.opBuffer.size;
  BATTLE_PARAM_AllocRecBuffer( bsp, mHeap );
  memcpy( bsp->recBuffer, mBattleRecData->upload.body.opBuffer.buffer, BTLREC_OPERATION_BUFFER_SIZE );

  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      if( mBattleRecData->upload.body.rec_party[ i ].is_exist != false )
      {
        //手持ちポケモン 解放忘れに気を付ける
        bsp->party[ i ] = GFL_NEW( heap ) pml::PokeParty( heap );
        bsp->party[ i ]->DeserializeCore( &mBattleRecData->upload.body.rec_party[ i ].members );
      }
      if( mBattleRecData->upload.body.myStatus[ i ].is_exist != false )
      {
        //MyStatus 解放忘れに気を付ける
        bsp->playerStatus[ i ] = GFL_NEW( heap ) Savedata::MyStatus();
        bsp->playerStatus[ i ]->Deserialize( mBattleRecData->upload.body.myStatus[ i ].status );
      }
    }
  }

  //トレーナーデータ
  {
    for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
    {
      //解放忘れに気を付ける
      bsp->tr_data[ i ] = GFL_NEW( heap ) BSP_TRAINER_DATA( heap );
      bsp->tr_data[ i ]->Deserialize( &mBattleRecData->upload.body.tr_data[ i ] );
    }
  }

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルレコーダーデータを設定し、BattleSetupParamを生成（デバッグ用）
 *
 * @param[out] bsp     BattleSetupParam
 * @param[in] recData  使用するバトルレコーダーデータ
 * @param[in] heap     動的に確保するメモリの確保先ヒープ
 *
 * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::CreateBattleSetupParamDebug( BATTLE_SETUP_PARAM* bsp, const BATTLE_REC_DATA* recData, gfl2::heap::HeapBase* heap )
{
  ::std::memcpy( mBattleRecData, recData,  sizeof(BATTLE_REC_DATA) );
  mIsLoadData = true;
  return CreateBattleSetupParam( bsp, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルレコーダーデータが存在するかチェック
 *
 * @param[in] index     ファイルインデックス
 *
 * @retval  true:存在する　false：存在しない
 */
//--------------------------------------------------------------------------------------------
bool  BattleRecorderSaveData::IsExistData( s32  index )
{ 
  ExtSavedata::NijiExtSaveSystem* xess = mGameManager->GetNijiExtSaveSystem();

  return  xess->IsFileExists( EXTID_BATTLEVIDEO, index );
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   CreateBattleSetupParamで動的に確保したメモリを解放する
 *
 * @param[in] bsp   BattleSetupParam
 */
//--------------------------------------------------------------------------------------------
void  BattleRecorderSaveData::ClearBattleSetupParam( BATTLE_SETUP_PARAM* bsp )
{
  BATTLE_SETUP_Clear( bsp );

  GFL_SAFE_DELETE( bsp->configData );

  for( s32 i = 0 ; i < BTL_CLIENT_NUM ; i++ )
  {
    GFL_SAFE_DELETE( bsp->playerStatus[ i ] );
  }
}

//--------------------------------------------------------------------------
/**
 *  @brief  アップロードデータからMyStatusを取得
 *  @param  クライアント番号
 *  @param  ヒープ
 *
 *  @return MyStatus    ない場合はNULLが返る
 *
 *  @li       解放は各自で！
 */
//--------------------------------------------------------------------------
Savedata::MyStatus*  BattleRecorderSaveData::GetMyStatus( BTL_CLIENT_ID clientId, gfl2::heap::HeapBase* heap )
{
  if( mIsLoadData == false ) return NULL;

  if( mBattleRecData->upload.body.myStatus[ clientId ].is_exist != false )
  {
    //MyStatus 解放忘れに気を付ける
    Savedata::MyStatus *mystatus = GFL_NEW( heap ) Savedata::MyStatus();
    mystatus->Deserialize( mBattleRecData->upload.body.myStatus[ clientId ].status );
    return mystatus;
  }
  else
  {
    return NULL;
  }
}

//--------------------------------------------------------------------------
/**
 *  @brief  アップロードデータからPokePartyを取得
 *  @param  クライアント番号
 *  @return PokeParty    ない場合はNULLが返る
 *
 *  @li       解放は各自で！
 */
//--------------------------------------------------------------------------
pml::PokeParty*  BattleRecorderSaveData::GetPokeParty( BTL_CLIENT_ID clientId, gfl2::heap::HeapBase* heap )
{
  if( mIsLoadData == false ) return NULL;

  if( mBattleRecData->upload.body.rec_party[ clientId ].is_exist != false )
  {
    //手持ちポケモン 解放忘れに気を付ける
    pml::PokeParty *pokeparty = GFL_NEW( heap ) pml::PokeParty( heap );
    pokeparty->DeserializeCore( &mBattleRecData->upload.body.rec_party[ clientId ].members );
    return pokeparty;
  }
  else
  {
    return NULL;
  }
}


} //namespace extsavedata
