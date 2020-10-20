//=============================================================================
/**
 * @brief 
 * @file sound_VOICE.cpp
 * @author obata
 * @date 2011.10.12
 */
//=============================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/SoundLocal.h"
#include "Sound/include/PMSoundSystem.h"
#include "Sound/include/VoicePlayer.h"
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"
#include <arc_def_index/arc_def.h>

namespace Sound { 

  #include "niji_conv_header/sound/VoiceIDTable.hpp"  // for VOICE_ID_TABLE

  #if defined(GF_PLATFORM_CTR)
    #define GF_SOUND_VOICE_NUM  (NN_SND_VOICE_NUM)
  #elif(GF_PLATFORM_WIN32)
    #define GF_SOUND_VOICE_NUM  (24)  //NN_SND_VOICE_NUMが24なので同じ値にしている
  #endif

  enum VoicePlayMode
  {
    VOICE_PLAY_MODE_NORMAL,  //通常再生
    VOICE_PLAY_MODE_3DACTOR //3Dアクター再生
  };

  struct VoicePlayerDesc 
  {
    PMSoundSystem::HandleIndex soundHandle;
    u32 seqItemID;
    u32 warcItemID;
  };

  #define DEFINE_VOICE_PLAER_DESC( index ) \
  {\
    PMSoundSystem::SHANDLE_VOICE_##index,\
    SEQ_PV_SLOT_##index,\
    WARC_PV_SLOT_##index,\
  }

  const VoicePlayerDesc VOICE_PLAYER_DESC[ PMSoundSystem::VOICE_HANDLE_NUM ] = 
  {
    DEFINE_VOICE_PLAER_DESC( 00 ),
    DEFINE_VOICE_PLAER_DESC( 01 ),
    DEFINE_VOICE_PLAER_DESC( 02 ),
    DEFINE_VOICE_PLAER_DESC( 03 ),
    DEFINE_VOICE_PLAER_DESC( 04 ),
    DEFINE_VOICE_PLAER_DESC( 05 ),
    DEFINE_VOICE_PLAER_DESC( 06 ),
    DEFINE_VOICE_PLAER_DESC( 07 ),
    DEFINE_VOICE_PLAER_DESC( 08 ),
    DEFINE_VOICE_PLAER_DESC( 09 ),
    DEFINE_VOICE_PLAER_DESC( 10 ),
    DEFINE_VOICE_PLAER_DESC( 11 ),
  };


  gfl2::heap::HeapBase* s_DeviceHeapForVoiceBuffer = NULL;
  VoicePlayer* s_VoicePlayer[ PMSoundSystem::VOICE_HANDLE_NUM ];
  u32 s_VoicePlayerNum = 0;

  static VoiceID GetVoiceID(MonsNo monsno, FormNo formno, VoiceType voice_type);
  static void GetVoiceDataID(VoiceID voiceID, u32* seqID, u32* warcID);

  //================================================================================================
  /**
   * @brief 使用準備
   * @param heap         システムのセットアップに使用するヒープ
   * @param device_heap  音声データの管理に使用するヒープ
   */
  //================================================================================================
  void Initialize_VOICE( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* device_heap )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
#ifndef BUGFIX_BTS7030_20130607
    //VCT用に一つ減らす
    nw::snd::SoundSystem::SetMaxVoiceCount( GF_SOUND_VOICE_NUM );
#endif  //BUGFIX_BTS7030_20130607
    
    // テンポラリヒープの取得(ファイル展開用)
    GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::fs::AsyncFileManager* read_manager = game_manager->GetAsyncFileManager(); 
    gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );

    s_DeviceHeapForVoiceBuffer = device_heap;

    // 使用するアーカイブファイルを登録
//    if( !read_manager->IsFileManaged(ARCID_POKE_VOICE) )
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId = ARCID_POKE_VOICE;
      req.heapForFile = fileread_heap;
      req.heapForReq = fileread_heap->GetLowerHandle();
      read_manager->SyncArcFileOpen( req );
    }

    // ボイスプレーヤーのセットアップ
    for( u32 i=0; i<PMSoundSystem::VOICE_HANDLE_NUM; i++ ) 
    {
      s_VoicePlayer[i] = GFL_NEW( heap )
        VoicePlayer( 
            s_PMSoundSystem,
            VOICE_PLAYER_DESC[i].soundHandle,
            read_manager,
            VOICE_PLAYER_DESC[i].seqItemID,
            VOICE_PLAYER_DESC[i].warcItemID,
            PMSoundSystem::SOUND_3D_ACTOR_INDEX_VOICE_START+i);
    }
    // 2つだけ常駐させる
    s_VoicePlayerNum = PMSoundSystem::VOICE_HANDLE_COMMON_NUM;
    for( u32 i = 0; i < s_VoicePlayerNum; i++ ){
      s_VoicePlayer[i]->Activate(device_heap);
    }
  } 

  //================================================================================================
  /**
   * @brief 後片付け
   */
  //================================================================================================
  void Finalize_VOICE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    // テンポラリヒープから登録解除
    GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::fs::AsyncFileManager* read_manager = game_manager->GetAsyncFileManager();
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_POKE_VOICE;
      read_manager->SyncArcFileClose( closeReq );
    }

    for( u32 i=0; i<PMSoundSystem::VOICE_HANDLE_NUM; i++ ){ GFL_DELETE s_VoicePlayer[i]; }
  }

  //================================================================================================
  /**
   * @brief プレーヤーの追加と削除
   * 削除の際は追加分全て削除するので注意
   * (現状の用途として複雑な追加状態はないと考え、きちんと管理されたシステムにしていないため)
   */
  //================================================================================================
  void CreateAdditionalVoicePlayer( u32 num, gfl2::heap::HeapBase* device_heap)
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    u32 addNum = num;
    if((s_VoicePlayerNum + num) > PMSoundSystem::VOICE_HANDLE_NUM){
      addNum = PMSoundSystem::VOICE_HANDLE_NUM - s_VoicePlayerNum;
    }
    for(u32 i=0; i< addNum; i++){
      s_VoicePlayer[s_VoicePlayerNum + i]->Activate(device_heap);
    }
    s_VoicePlayerNum += addNum;
  }
  
  /*
   *  @brief  非常駐ボイスプレイヤーが再生中かどうか？
   *
   *  @note 再生中はDeleteAddtionalVoicePlayerを呼び出してはいけない
   *  　    主にDelete前にボイスプレイヤーの状態遷移を待つ目的で用いる
   */
  b32 IsPlayingAdditionalVoicePlayer( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET( false );
    for(u32 i=PMSoundSystem::VOICE_HANDLE_COMMON_NUM; i<s_VoicePlayerNum; i++){
      if( s_VoicePlayer[i]->IsPlayingState() != false ){
        return true;
      }
    }
    return false;
  }

  /*
   *  @brief  非常駐ボイスプレイヤーを削除する
   *
   *  @note 呼び出す前にIsPlayingAdditionalVoicePlayer()を実行し
   *  　    結果がfalseである事を確認してから実行する事
   */
  void DeleteAdditionalVoicePlayer( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    for(u32 i=PMSoundSystem::VOICE_HANDLE_COMMON_NUM; i<s_VoicePlayerNum; i++){
      s_VoicePlayer[i]->Deactivate();
    }
    s_VoicePlayerNum = PMSoundSystem::VOICE_HANDLE_COMMON_NUM;
  }

  //================================================================================================
  /**
   * @brief 更新処理
   */
  //================================================================================================
  void Update_VOICE( void )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    for(u32 i=0; i<PMSoundSystem::VOICE_HANDLE_NUM; i++){ s_VoicePlayer[i]->Update(); }
  }

  //================================================================================================
  /**
   * @brief 鳴き声を再生する
   * @param player_index  声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   * @param pokeparam     ポケモンのパラメータ
   * @param voice_type    鳴き声の種類
   * @param act3D        3Dモード再生
   */
  //================================================================================================
  void PlayVoice(
      u8 player_index,
      const pml::pokepara::CoreParam* param,
      VoiceType voice_type,
      b32 act3D,
      u32 userId
      )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    MonsNo monsno = param->GetMonsNo();
    FormNo formno = param->GetFormNo();
    PlayVoice(player_index, monsno, formno, voice_type, act3D, userId );
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声を再生する
   * @param player_index  声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   * @param monsno        鳴くポケモンのモンスターNo.
   * @param formno        ポケモンのフォルムNo.
   * @param voice_type    鳴き声の種類
   * @param act3D        3Dモード再生
   */
  //================================================================================================
  void PlayVoice(
      u8 player_index,
      MonsNo monsno,
      FormNo formno,
      VoiceType voice_type,
      b32 act3D,
      u32 userId
      )
  { 
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    VoiceID voiceID = GetVoiceID( monsno, formno, voice_type );
    PlayVoice(player_index, voiceID, act3D, userId );
  }

  //================================================================================================
  /**
   * @brief 鳴き声を再生する
   * @param player_index 声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   * @param voiceID      再生するボイス
   * @param act3D        3Dモード再生
   */
  //================================================================================================
  void PlayVoice(u8 player_index, VoiceID voiceID, b32 act3D, u32 userId )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    u32 seq_dataID = 0;
    u32 warc_dataID = 0;
    GetVoiceDataID(voiceID, &seq_dataID, &warc_dataID);

    //ファイルロードに用いるテンポラリヒープを指定
    gfl2::heap::HeapBase* lower_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD )->GetLowerHandle();

    s_VoicePlayer[player_index]->PlayVoice(seq_dataID, warc_dataID, lower_heap, act3D, userId );
  }

  //================================================================================================
  /**
   * @brief 鳴き声が終了したか？
   * @retval true  終了した
   * @retval false 再生中
   */
  //================================================================================================
  b32 IsVoiceFinished( u8 player_index )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return true; }

    return !s_VoicePlayer[player_index]->IsPlayingState();
  }

  u32 GetVoiceUserId(u8 player_index)
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(true);
    if (s_VoicePlayerNum <= player_index)
    {
      GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return true;
    }

    return s_VoicePlayer[player_index]->GetVoiceUserId();
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声を停止する
   * @param player_index  ボイスプレイヤーのIndex指定
   * @param fadeFrames フェードフレーム
   * @param player_index 声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   */
  //================================================================================================
  void StopVoice( u8 player_index, s32 fadeFrame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    s_VoicePlayer[player_index]->StopVoice(fadeFrame);
  }
  
  //================================================================================================
  /**
   * @brief 全ての鳴き声を停止させる(ゲームリセット用)
   *
   * @note    BTS6040,BTS6242,BTS6269,BTS6274対策
   * @note    既に読み込み中のスレッド終了を待ち、ボイス再生中の場合はStopさせます
   *          trueが返るまで呼び続けてください
   *
   * @retval  true  全ての鳴き声が問題なく停止した
   * @retval  false まだ停止していないボイスがあるので待つ
   *
   * これを呼び出した後のゲーム復帰はできないので注意！
   */
  //================================================================================================
  b32 ResetVoiceReq( gfl2::fs::AsyncFileManager* pFileReadManager )
  {
    SOUND_UPDATE_CRITICALSECTION;

    for( u32 i = 0;i < s_VoicePlayerNum; i++ ){
      if( s_VoicePlayer[i]->IsActivate() == false ){
        continue; //プレイヤーが非アクティブ
      }
      //リセットリクエスト
      if( s_VoicePlayer[i]->ResetForceReq() == false ){
        return false; //まだ処理中
      }
    }
    s_PMSoundSystem->StopAllVoice();  //ハンドルで止め損ねた場合の保険としてプレイヤー経由で全て止める
    
    //全ての後始末が終わった
    return true;
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声にエフェクトを適用する
   * @param player_index 声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   * @param vEffectInfo   エフェクトパラメータ
   */
  //================================================================================================
  void ChangeVoiceEffect( u8 player_index, const VoicePlayInfo& vEffectInfo )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->ChangeEffect(vEffectInfo);
    }
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声のボリュームを変化させる
   * @param player_index 声の管理番号( 0 〜 PMSoundSystem::VOICE_HANDLE_NUM-1 )
   * @param value  音量の倍率( 1.0fで等倍 )
   * @param change_frame 音量変化にかけるフレーム数(省略可：省略した場合は即時変化=0)
   */
  //================================================================================================
  void ChangeVoiceVolume( u8 player_index, const f32 value, const u32 change_frame )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->ChangeVolume(value, change_frame);
    }
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声のピッチを変化させる
   * @param value   周波数比率
   * 
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
  //================================================================================================
  void ChangeVoicePitch( u8 player_index, const f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->ChangePitch(value);
    }
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声の水平方向のパンを変化させる
   *  
   * @param value   パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
  //================================================================================================
  void ChangeVoicePan( u8 player_index, const f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false ){
      s_VoicePlayer[player_index]->ChangePan(value);
    }  
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声の垂直方向のパンを変化させる(現状、通常モードのみ有効)
   * @param value  パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
  //================================================================================================
  void ChangeVoiceSurroundPan( u8 player_index, const f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->ChangeSurroundPan(value);
    }  
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声のテンポを変化させる(現状、通常モードのみ有効)
   * @param value   テンポを倍率で指定( 1.0fで元に戻る )
   */
  //================================================================================================
  void ChangeVoiceTempo( u8 player_index, const f32 value )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if( s_VoicePlayer[player_index]->IsEffectEnableState() != false ){
      s_VoicePlayer[player_index]->ChangeTempo(value);
    }  
  }
  
  //================================================================================================
  /**
   * @brief 3D再生の鳴き声のポジションを変化させる(3D再生モードのみ有効)
   *  
   * @param pos  ポジション
   */
  //================================================================================================
  void ChangeVoice3DPos( u8 player_index, const gfl2::math::VEC3& pos )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->Change3DPos(pos);
    }  
  }
  
  //================================================================================================
  /**
   * @brief 3D再生の鳴き声のベロシティを変化させる(3D再生モードのみ有効)
   *  
   * @param velocity  ベロシティ
   */
  //================================================================================================
  void ChangeVoice3DVelocity( u8 player_index, const gfl2::math::VEC3& velocity )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RETNONE;
    if( s_VoicePlayerNum <= player_index )
      { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return; }

    if(s_VoicePlayer[player_index]->IsEffectEnableState() != false){
      s_VoicePlayer[player_index]->Change3DVelocity(velocity);
    }  
  }
  
  //================================================================================================
  /**
   * @brief センド量を鳴き声にセットする
   *
   * @param player_index  プレイヤーインデックス
   * @param busID         バスID
   * @param send          センド量
   */
  //================================================================================================
  b32 SetVoiceFxSend( u8 player_index, nw::snd::AuxBus busID, float send )
  {
    SOUND_UPDATE_CRITICALSECTION;

    DEBUG_SOUND_OFF_RET(false);
    if( s_VoicePlayerNum <= player_index )
    { GFL_ASSERT_MSG(0, "voicePlayer Index Error\n"); return false; }

    return s_VoicePlayer[ player_index ]->SetFxSend( busID, send );
  }
  
  //================================================================================================
  /**
   * @brief 鳴き声IDを取得する
   * @param monsno     モンスターNo.
   * @param formno     フォルムNo.
   * @param voice_type 声の種類
   */
  //================================================================================================
  static VoiceID GetVoiceID(MonsNo monsno, FormNo formno, VoiceType voice_type)
  {
    GFL_ASSERT( MONSNO_HUSIGIDANE <= monsno );
    GFL_ASSERT( monsno <= MONSNO_END );
    GFL_ASSERT( voice_type < VOICE_TYPE_NUM );
    GFL_ASSERT( voice_type < GFL_NELEMS(VOICE_ID_TABLE[0]) );

    u32 table_index = monsno;
    if( ( 0 < formno ) && ( VOICE_ID_TABLE[ monsno ][0] != 0 ) ) {
      // フォルムにより鳴き声が変わる場合の処理
      table_index = VOICE_ID_TABLE[ monsno ][0] + (formno - 1);
    }
    GFL_ASSERT( table_index < GFL_NELEMS(VOICE_ID_TABLE) );
    return static_cast<VoiceID>( VOICE_ID_TABLE[ table_index ][ 1 + voice_type ] );
  }

  //================================================================================================
  /**
   * @brief 鳴き声データのIDを取得する
   * @param[in]  voiceID 鳴き声ID
   * @param[out] seqID   対応するシーケンスデータのID
   * @param[out] warcID  対応する波形アーカイブデータのID
   */
  //================================================================================================
  static void GetVoiceDataID(VoiceID voiceID, u32* seqID, u32* warcID)
  {
    *seqID = voiceID & 0x0000ffff;
    *warcID = voiceID >> 16;
  }

} // namespace Sound 

