//================================================================================================
/**
 * @brief 鳴き声プレイヤー
 * @file VoicePlayer.cpp
 * @author obata_toshihiro
 * @date 2012.02.07
 */
//================================================================================================

#include "Sound/include/Sound.h"
#include "Sound/include/PMSoundSystem.h"
//#include "Sound/include/VoiceDataManager.h"
#include "Sound/include/VoicePlayer.h"
#include "Sound/include/SoundMiddleIDConverter.h"
#include <arc_def_index/arc_def.h>

#include <debug/include/gfl2_DebugPrint.h>
#include "util/include/gfl2_std_string.h"
#include "System/include/HeapDefine.h"

namespace Sound {

	#define BUFFER_ALIGN	(32)
	#define SEQ_BUFFER_SIZE	(1000)
	#define WARC_BUFFER_SIZE	(40000)


	//================================================================================================
  /**
   * @brief コンストラクタ
   * @param sound_system       再生に使用するサウンドシステム
   * @param sound_handle_index 再生に使用するサウンドハンドル
   * @param seq_itemID         再生するシーケンスのサウンドアイテムID
   * @param warc_ItemID        再生する波形アーカイブのサウンドアイテムID
   * @param actor_index_3D     ボイスプレイヤーに関連づける専用3Dアクターのindex
   */
	//================================================================================================
  VoicePlayer::VoicePlayer( 
      PMSoundSystem* sound_system,
      PMSoundSystem::HandleIndex sound_handle_index,
			gfl2::fs::AsyncFileManager* pFileReadManager,
      u32 seq_itemID,
      u32 warc_ItemID,
      u8  actor_index_3D ) :
    m_soundSystem( sound_system ),
    m_soundHandleIndex( sound_handle_index ),
    m_pFileReadManager( pFileReadManager ),
    m_seqItemID( seq_itemID ),
    m_warcItemID( warc_ItemID ),
    m_state( STATE_EMPTY ),
    m_playMode( PLAY_MODE_NORMAL ),
    m_effect(),
    m_3DActorIndex(actor_index_3D),
    m_userId( 0 ),
    m_req_play( false ),
	  m_regist_mode( WAVE_FREE )
  {
		this->InitSoundData(&m_sd_seq);
		this->InitSoundData(&m_sd_warc);
  }

	//================================================================================================
  /**
   * @brief デストラクタ
   */
	//================================================================================================
  VoicePlayer::~VoicePlayer()
  {
		this->Deactivate(); 
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief サウンドデータ初期化、作成。削除
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::InitSoundData(SoundData* pSd)
	{
		pSd->buffer = NULL;
		pSd->dataSize = 0,
		pSd->dataID = 0;
	}
	
  void VoicePlayer::CreateSoundData(SoundData* pSd, gfl2::heap::HeapBase* heap, u32 size)
	{
		this->DeleteSoundData(pSd);

		pSd->buffer = GflHeapAllocMemoryAlign(heap, size, BUFFER_ALIGN);
		pSd->dataSize = 0;
		pSd->dataID = 0;


    if( pSd->buffer == NULL ){
      SOUND_SYSTEM_INITIALIZE_ASSERT("VoicePlayer Initialize Failed\n"); //システム初期化失敗
    }
	}

  void VoicePlayer::DeleteSoundData(SoundData* pSd)
	{
		if(pSd->buffer){
			GflHeapFreeMemory(pSd->buffer);
			pSd->buffer = NULL;
		}
		pSd->dataSize = 0;
		pSd->dataID = 0;
	}

	//================================================================================================
  /**
   * @brief アクティベート
	 * @param heap バッファ確保対象ヒープ
	 * @retval true = 成功 : false = 失敗
   */
	//================================================================================================
  b32 VoicePlayer::Activate(gfl2::heap::HeapBase* heap) 
	{
		if(m_state != STATE_EMPTY){ return false; }

		this->CreateSoundData(&m_sd_seq, heap, SEQ_BUFFER_SIZE);
		this->CreateSoundData(&m_sd_warc, heap, WARC_BUFFER_SIZE);

		m_state = STATE_STANDBY;
    m_regist_mode = WAVE_FREE;
    m_req_play = false;

		return true;
	}
	
	//================================================================================================
  /**
   * @brief デアクティベート
	 * @retval true = 実行 : false = なにもしなかった
   */
	//================================================================================================
  b32 VoicePlayer::Deactivate(void) 
	{
		if(m_state == STATE_EMPTY){ return false; }

    /*
     *
     * GFBTS2488対策 ボイス強制停止
     * STATE_PLAYING||STATE_PLAYSTANDBY中にDeactivateされると、
     * nw::SoundDataManagerへのボイス用波形アーカイブのRegist状態が解除されなくなる。
     * この状態でバッファを破棄してしまうと、サウンドライブラリ側からアクセスされて問題を引き起こす
     *
     * KujiraではSTATE_LOADING対策はアッパーバージョン修正対象としていた。
     * ---------------------------------------------------------------------------
     *  本当はFileReadManagerが動いている状態のSTATE_LOADINGも待つべきなのだが、
     *  現状STATE_LOADING状態でプレイヤーが破棄されうるのは
     *  強制ゲーム終了時のみで、そこは別途対処をしているため、コード変更量を最小限に抑える為
     *  ストップのみにする。
     * ---------------------------------------------------------------------------
     * 本関数でLOADINGステータス時の呼び出しに対応しようとすると処理が煩雑になるため
     * SangoでLOADING対策は呼び出し元で行ってもらう仕様にすることで対応
     */
    if( m_state == STATE_LOADING ){
      GFL_ASSERT_MSG( 0, "LOADING中はDeactivate不可! 呼び出し側でLoad終了を待ってDeactivateしてください!\n");
      return false;
    }

    if( m_state == STATE_PLAYING || m_state == STATE_PLAYSTANDBY || m_state == STATE_STOPPING ){
      _stopVoice( WAVE_FREE );
    }
    this->_registerVoice( false );
		this->DeleteSoundData(&m_sd_seq);
		this->DeleteSoundData(&m_sd_warc);

		m_state = STATE_EMPTY;
    m_regist_mode = WAVE_FREE;
    m_req_play = false;
    m_userId = 0;

		return true;
	}
	
	//================================================================================================
  /**
   * @brief アクティベート判定
	 * @retval true = アクティベート済み : false = 未アクティベート
   */
	//================================================================================================
  b32 VoicePlayer::IsActivate( void )
	{
		if(m_state == STATE_EMPTY){ return false; }
		return true;
	}

  //================================================================================================
  /**
   * @brief  プレイヤーの状態リセットリクエスト(ゲームリセット用)
   *
   * @note    BTS6040,BTS6242,BTS6269,BTS6274対策
   * @note    既に読み込み中のスレッド終了を待ち、ボイス再生中の場合はStopさせます
   *          trueが返るまで呼び続けてください
   *
	 * @retval true   ストップリクエストが処理された
   * @retval false  まだ処理中
   *
   * これを呼び出した後のゲーム復帰はできないので注意！
   * 
   * 161622 追記 by iwasawa
   * 本関数を呼び出すときは、サウンドアップデートをスレッドではなくメインから呼び出しており、
   * 本関数の直後にアップデートを回すため、SoundHandleの停止を待たずに、波形登録解除しても大丈夫
   */
	//================================================================================================
  b32 VoicePlayer::ResetForceReq( void )
	{
		switch( m_state )
    {
    case STATE_LOADING:
      // 読み込み終了判定
      if(m_pFileReadManager->IsArcFileLoadDataBufFinished(m_sd_seq.buffer) == false){
        return false;
		  }
      if(m_pFileReadManager->IsArcFileLoadDataBufFinished(m_sd_warc.buffer) == false){
        return false;
      }
      break;
    case STATE_PLAYING:
    case STATE_STOPPING:
      //強制終了
      this->_stopVoice( WAVE_FREE ); //止める(NWのSoundDataManagerへの登録も解除)
      break;
    default:
      break;
    }
    this->_registerVoice( false );  //波形登録解除

    m_state = STATE_STANDBY;
    m_regist_mode = WAVE_FREE;

    return true;  //終了
	}
	
	//================================================================================================
  /**
   * @brief   ボイス波形登録(レジストモードへ移行) 160622 使用不可 by iwasawa
   * @param		seqID		再生するシーケンスID
   * @param		warcID  再生する波形アーカイブID
   * @param		heap		ファイル展開用ヒープ
	 * @retval	true = ロードスレッド設定終了 : false = ロード不可状態
	 * 
	 * @caution　VoicePlayer::RegisterVoice　160622 使用不可 by iwasawa
   * GFNMCat[5264] Sound::Updateのスレッド化対策が出来ておらず、どこも現状利用していないので、nijiではこのまま利用不可とする
   * アッパーで必要があればスレッド化対応して解放する予定
   * 
   * 再生中ステータスでコールされた場合に、SoundHandleの停止を待ってから波形登録解除する必要がある
   */
	//================================================================================================
  b32 VoicePlayer::RegisterVoice(u32 seqID, u32 warcID, gfl2::heap::HeapBase* heap)
	{
    GFL_ASSERT_STOP_MSG(0," VoicePlayer::RegisterVoiceはnijiでは利用できません\n");

		if(m_state == STATE_EMPTY){ return false; }				// 未アクティベート(空)
		if(m_state == STATE_LOADING){ return false; }			// データ読み込み待ち(中断不可)
    if( this->IsVoicePlayingState() ){ //現在再生途中だった場合は一度止める
			this->_stopVoice( WAVE_FREE );
		}
		_loadVoice(seqID, warcID, heap);
    m_regist_mode = WAVE_REGIST;

		m_state = STATE_LOADING;

		return true;
	}
	
  //================================================================================================
  /**
   * @brief   ボイス波形解放(レジストモード解除)　 160622 使用不可 by iwasawa
   * @param		seqID		再生するシーケンスID
   * @param		warcID  再生する波形アーカイブID
   * @param		heap		ファイル展開用ヒープ
	 * @retval	true = ロードスレッド設定終了 : false = ロード不可状態
   * 
   * @caution　VoicePlayer::FreeVoice　160622 使用不可 by iwasawa
   * GFNMCat[5264] Sound::Updateのスレッド化対策が出来ておらず、どこも現状利用していないので、nijiではこのまま利用不可とする
   * アッパーで必要があればスレッド化対応して解放する予定
   * 
   * 再生中ステータスでコールされた場合に、SoundHandleの停止を待ってから波形登録解除する必要がある
   */
	//================================================================================================
  b32 VoicePlayer::FreeVoice(u32 seqID, u32 warcID, gfl2::heap::HeapBase* heap)
	{
    GFL_ASSERT_STOP_MSG(0," VoicePlayer::FreeVoiceはnijiでは利用できません\n");

    GFL_UNUSED(seqID);
    GFL_UNUSED(warcID);
    GFL_UNUSED(heap);

		if(m_state == STATE_EMPTY){ return false; }				// 未アクティベート(空)
		
    if(m_state == STATE_LOADING){ // データ読み込み待ち(中断不可)
      GFL_ASSERT_MSG( 0, "ロード中は解放できません\n"); //@not care スレッドを途中で停止できないので運用で回避
      return false;
    }

    if( m_state != STATE_PLAYSTANDBY ){
      GFL_ASSERT_MSG(0,"再生中に解放リクエストされた state=%d\n", m_state );
    
      if( this->IsVoicePlayingState() ){ //現在再生途中だった場合は止める
			  this->_stopVoice( WAVE_REGIST );  //解放はこの後行う
		  }
    }

    //波形登録解放
    this->_registerVoice( false );
    m_regist_mode = WAVE_FREE;

		m_state = STATE_STANDBY;

		return true;
	}

	//================================================================================================
  /**
   * @brief   ロード済みボイス再生リクエスト
	 * @param		act3D	3Dモード
	 * @retval	true = 再生開始 : false = 再生ボイスなし
   *
   * @attention
	 * 130507現在sound_Voice.cpp層から呼ばれておらず、未使用の関数
	 * 
   * @caution　VoicePlayer::PlayVoiceRegister　160622 使用不可 by iwasawa
   * GFNMCat[5264] Sound::Updateのスレッド化対策が出来ておらず、どこも現状利用していないので、nijiではこのまま利用不可とする
   * アッパーで必要があればスレッド化対応して解放する予定
   * 
   * 再生中ステータスでコールされた場合に、SoundHandleの停止を待ってから波形登録解除する必要がある   */
	//================================================================================================
  b32 VoicePlayer::PlayVoiceRegister( b32 act3D )
	{
    GFL_ASSERT_STOP_MSG(0," VoicePlayer::PlayVoiceRegisterはnijiでは利用できません\n");

    if( m_state < STATE_LOADING || m_regist_mode == WAVE_FREE ){
      GFL_ASSERT_MSG(0,"RegistVoice処理終了後しか使えません\n");
      return false;
    }
    m_playMode = (act3D != false)? PLAY_MODE_3DACTOR : PLAY_MODE_NORMAL;

    if( m_state == STATE_LOADING ){
      //まだロード中のため再生リクエストを出しておいて、ここでは戻る
      m_req_play = true;
      return true;
    }
    if( this->IsVoicePlayingState() ){  //現在再生中だったら一度止める
			this->_stopVoice( WAVE_REGIST );
		}
		_playVoice();

		m_state = STATE_PLAYING;

		return true;
	}
	
	//================================================================================================
  /**
   * @brief   ボイスのロード〜再生までを一括で行う
   * @param		seqID		再生するシーケンスID
   * @param		warcID  再生する波形アーカイブID
   * @param		heap		ファイル展開用ヒープ
	 * @param		act3D		3Dモード
	 * @retval	true = ロードスレッド＆再生設定終了 : false = ロード不可状態
   */
	//================================================================================================
  b32 VoicePlayer::PlayVoice
    (u32 seqID, u32 warcID, gfl2::heap::HeapBase* heap, b32 act3D, u32 userId )
	{
		if(m_state == STATE_EMPTY){ return false; }				// 未アクティベート(空)
		if(m_state == STATE_LOADING){ return false; }			// データ読み込み待ち(中断不可)

    if( m_regist_mode == WAVE_REGIST ){
      GFL_ASSERT_MSG(0,"Registモード時はPlayVoiceRegister()を利用してください\n");
      return false;
    }

    /*
     *  GFNMCat[5264] Sound::Updateのスレッド化対策
     *  
     *  再生中の場合は停止してからロードを始めないと、参照中のサウンドメモリを破壊する事になる
     *  
     *  SoundDataManager::Updateをスレッド実行にした場合、m_soundSystem->StopVoiceコール後
     *  nw::snd::SoundDataManager::Updateが呼ばれて、実際に停止するまでにラグがある。
     *  この間にサウンドスレッド本体に割り込まれると、破壊したメモリを参照することになる。
     *  
     *  よって、このタイミングではWaveデータアドレスの解除や、バッファの上書きを始めてはいけないので
     *  Stopリクエストだして、読み込み情報記憶するだけにする
     */
    if( this->IsVoicePlayingState()){ //現在再生中の場合は止めてから処理しないといけないので、リクエストだけ行う
      _loadVoiceReq( seqID, warcID, heap );

    }else{
      //既に止まっていればロード開始をリクエストしていい
      _loadVoice( seqID, warcID, heap );

    }   

    m_playMode = (act3D != false)? PLAY_MODE_3DACTOR : PLAY_MODE_NORMAL;

		m_state = STATE_LOADING;
    m_req_play = true;
    m_userId = userId;

		return true;
	}
	
	//================================================================================================
  /**
   * @brief ボイスの停止
   *
	 * @param	fadeFrames フェードフレーム
	 * @retval true = 実行 : false = なにもしなかった
   */
	//================================================================================================
  b32 VoicePlayer::StopVoice(s32 fadeFrames)
	{
    if( m_state == STATE_EMPTY ){ return false; }

    //ロード中にストップリクエスト
    if( m_state == STATE_LOADING ){
      if( m_regist_mode == WAVE_FREE ){
        m_req_play = false; //再生をフック
      }
      return true;
    }
    //ボイス再生ステータスでない場合何もしない
    if( this->IsVoicePlayingState() == false ){ return false; }

		m_state = this->_stopVoiceReq( m_regist_mode, fadeFrames);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
  /**
   * @brief ボイスロード本体
   * @param seqID		再生するシーケンスID
   * @param warcID  再生する波形アーカイブID
	 * @retval true = ロードスレッド設定終了 : false = ロード不可状態
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::_loadVoice(u32 seqID, u32 warcID, gfl2::heap::HeapBase* heap)
	{
    m_sd_seq.dataID = seqID; 
    m_sd_warc.dataID = warcID; 

    // 読み込み開始
		gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.prio                = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    req.arcId               = ARCID_POKE_VOICE;
    req.heapForCompressed   = NULL; //圧縮解凍しないので利用しない
    req.heapForReq          = heap->GetLowerHandle(); //スレッド生成に利用するテンポラリヒープ

    req.datId               = seqID; 
    req.pBuf			      		= m_sd_seq.buffer;
    req.bufSize		        	= SEQ_BUFFER_SIZE;//m_sd_seq.dataSize;
//    m_pFileReadManager->SyncArcFileLoadDataBuf(req); 
    m_pFileReadManager->AddArcFileLoadDataBufReq(req); //GFNMCat[5264] Sound::Updateのスレッド化対策 同期だとロック時間長いので、ちゃんと非同期にする


    req.datId               = warcID; 
    req.pBuf				      	= m_sd_warc.buffer;
    req.bufSize 	      		= WARC_BUFFER_SIZE;//m_sd_warc.dataSize;
	
//    m_pFileReadManager->SyncArcFileLoadDataBuf(req); 
    m_pFileReadManager->AddArcFileLoadDataBufReq(req); //GFNMCat[5264] Sound::Updateのスレッド化対策 同期だとロック時間長いので、ちゃんと非同期にする 

    m_heapForLoad = NULL; //リクエストかけたので、ロードリクエスト用のヒープを忘れていい
	}

  void VoicePlayer::_loadVoiceReq(u32 seqID, u32 warcID, gfl2::heap::HeapBase* heap)
  {
    //後からロードするデータを覚えておく
    m_sd_seq.dataID = seqID; 
    m_sd_warc.dataID = warcID; 
    m_heapForLoad = heap; //ロードリクエスト用のヒープを覚えておく

    //再生中のボイスの停止をリクエスト(nw::snd::SoundDataManager::Updateを通るまで実際には停止しないので注意)
    if( m_playMode == PLAY_MODE_NORMAL ){
      m_soundSystem->StopVoice( m_soundHandleIndex, m_seqItemID, 0 /*fadeFrames*/ );  
    } else {
      m_soundSystem->Stop3DActor(m_3DActorIndex, 0 /*fadeFrames*/ );
    }
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief ボイス再生本体
	 * @retval true = 再生開始 : false = 再生ボイスなし
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::_playVoice( void )
	{
    if( m_playMode == PLAY_MODE_3DACTOR ){
			m_soundSystem->PlayVoice3DActor( m_3DActorIndex, m_soundHandleIndex, m_seqItemID );
    } else {
			m_soundSystem->PlayVoice( m_soundHandleIndex, m_seqItemID );
    }
		this->VoiceEffectCore();
	}
  
  //------------------------------------------------------------------------------------------------
  /**
   * @brief ボイス停止リクエスト
	 * @param	regist_mode   WAVE_FREEだとシーケンスおよび波形アーカイブの置き換えを破棄, WAVE_REGISTだと維持
   * @param fadeFrames    フェードアウトタイム指定
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::_stopVoiceReq( RegistMode regist_mode, s32 fadeFrames )
	{
    if( m_playMode == PLAY_MODE_NORMAL ){
			m_soundSystem->StopVoice( m_soundHandleIndex, m_seqItemID, fadeFrames );  
		} else {
			m_soundSystem->Stop3DActor(m_3DActorIndex, fadeFrames);
		}
    /*
     *  GFNMCat[5264] Sound::Updateのスレッド化対策
     *  VoicePlayer.UpdateとSoundDataManager.Updateの間に再生スレッドが割り込んだとき対策用に
     *  fadeFramesに関わらず、ボイスの停止は確実に待ってから波形登録を解除せねばならない
     */
    return STATE_STOPPING;

#if 0
    if( fadeFrames > 0 ){
      return STATE_STOPPING; //フェードタイムが指定されている場合は、まだ登録解除しない
    }
		if( regist_mode == WAVE_REGIST ){ //波形維持モードの場合は再生待機に移行
      return STATE_PLAYSTANDBY;
    }
    //維持モードでない場合は、波形登録を破棄して待機に移行
    this->_registerVoice( false );
    return STATE_STANDBY;
#endif
	}
	
	//------------------------------------------------------------------------------------------------
  /**
   * @brief ボイス停止本体
	 * @param	regist_mode   WAVE_FREEだとシーケンスおよび波形アーカイブの置き換えを破棄, WAVE_REGISTだと維持
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::_stopVoice( RegistMode regist_mode )
	{
    (void)this->_stopVoiceReq( regist_mode, 0 );
	}
	

	//------------------------------------------------------------------------------------------------
  /**
   * @brief シーケンスおよび波形アーカイブのNW4Cライブラリへの登録と解除
   *
   * @param set_f  trueで登録、falseで解除
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::_registerVoice( b32 set_f )
	{
    if( set_f ){
      // シーケンスを置き換える
      this->RegisterVoiceDataWithSoundDataManager( m_seqItemID, m_sd_seq.buffer );
      // 波形アーカイブを置き換える
      this->RegisterVoiceDataWithSoundDataManager( m_warcItemID, m_sd_warc.buffer );
    }else{
      // シーケンスを置き換える
  	  this->RegisterVoiceDataWithSoundDataManager(m_seqItemID, NULL);
      // 波形アーカイブを置き換える
	  	this->RegisterVoiceDataWithSoundDataManager(m_warcItemID, NULL);
    }
	}
	
	//================================================================================================
  /**
   * @brief 更新処理
   */
	//================================================================================================
  void VoicePlayer::Update( void )
  {
    State ( VoicePlayer::*UPDATE_STATE[ STATE_NUM ] )( void ) = {
      &VoicePlayer::UpdateState_EMPTY,
      &VoicePlayer::UpdateState_STANDBY,
      &VoicePlayer::UpdateState_LOADING,
      &VoicePlayer::UpdateState_PLAYSTANDBY,
      &VoicePlayer::UpdateState_PLAYING,
      &VoicePlayer::UpdateState_STOPPING,
    };
    m_state = ( this->*UPDATE_STATE[ m_state ] )();
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 未アクティベート
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_EMPTY( void )
  {
    return STATE_EMPTY;
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 待機状態
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_STANDBY( void )
  {
    return STATE_STANDBY;
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 読み込み待ち状態
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_LOADING( void )
  {
    //GFNMCat[5264] Sound::Updateのスレッド化対策
    if( m_heapForLoad != NULL )
    {
      if(this->IsVoicePlaying() != false){
        //まだ止まってない！
        return STATE_LOADING;
      }
      //止まった時点で再生リクエストがキャンセルされていたら、波形登録解除して待機に戻る
      if( m_req_play == false )
      {
        if( m_regist_mode == WAVE_REGIST ){
          //REGISTモードの場合、波形登録残して待機に遷移
          return STATE_PLAYSTANDBY;
        }
        //通常モードの場合は、波形登録解除して待機に遷移
        this->_registerVoice( false );
        return STATE_STANDBY;
      }
      //ロードリクエストをかけて一回リターン
      _loadVoice( m_sd_seq.dataID, m_sd_warc.dataID, m_heapForLoad );
      return STATE_LOADING;
    }

    // 読み込み終了判定
    if(m_pFileReadManager->IsArcFileLoadDataBufFinished(m_sd_seq.buffer) == false){
      return STATE_LOADING;
		}
    if(m_pFileReadManager->IsArcFileLoadDataBufFinished(m_sd_warc.buffer) == false){
      return STATE_LOADING;
    }
    
    if( m_req_play ){
      //再生リクエストがかかっている場合は、レジストモードに関わらず再生に移行する

      this->_registerVoice( true );	// シーケンスと波形アーカイブを置き換える
		  this->_playVoice();	// 準備出来次第鳴らす

      m_req_play = false;
      return STATE_PLAYING;
    }
    else if( m_regist_mode == WAVE_REGIST ){
      //REGISTモードの場合、波形登録だけして待機
      this->_registerVoice( true );
			return STATE_PLAYSTANDBY;
    }
    //ロード途中でキャンセルされたので待機に戻る
    this->_registerVoice( false );
    return STATE_STANDBY;
  }
  
	//------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 再生待ち状態
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_PLAYSTANDBY( void )
  {
    return STATE_PLAYSTANDBY;
  }

	//------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 再生中
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_PLAYING( void )
  {
    if(this->IsVoicePlaying() != false){
			this->VoiceEffectCore();    
			return STATE_PLAYING;
    }

		if(m_regist_mode == WAVE_REGIST ){
			return STATE_PLAYSTANDBY;	// 連続再生対応(読み込み後状態を保持)
		}
    this->_registerVoice(false);  //読み込み状態解除
    return STATE_STANDBY;
  }
	
  //------------------------------------------------------------------------------------------------
  /**
   * @brief 状態遷移処理: 再生停止をリクエストした際のフェードアウト待ち
   * @return 次の状態
   */
	//------------------------------------------------------------------------------------------------
  VoicePlayer::State VoicePlayer::UpdateState_STOPPING( void )
  {
    if(this->IsVoicePlaying() != false){
			return STATE_STOPPING;
    }
		if( m_regist_mode == WAVE_REGIST ){
			return STATE_PLAYSTANDBY;	// 連続再生対応(読み込み後状態を保持)
		}
	  
    this->_registerVoice( false );
    return STATE_STANDBY;
  }
  
	//------------------------------------------------------------------------------------------------
  /**
   * @brief サウンドデータマネージャにボイスデータを登録する
   * @param middleID  置換対象の中間ID
   * @param data      登録するデータ
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::RegisterVoiceDataWithSoundDataManager( u32 middleID, const void* data )
  {
    gfl2::sound::SoundSystem* sound_system = m_soundSystem->GetSoundSystem();
    nw::snd::SoundArchive* sound_archive = sound_system->GetSoundArchive();
    u32 itemID = MiddleIDConverter::CalcSoundItemID( middleID );
    u32 fileID = sound_archive->GetItemFileId( itemID );
    m_soundSystem->SetSoundFileAddress( fileID, data );
  }
  
	//================================================================================================
  /**
   * @brief 再生処理中判定(読み込み + ボイス再生+フェードアウト中はtrue)
   * @retval true = 再生中 : false 再生中でない
   */
	//================================================================================================
  b32 VoicePlayer::IsPlayingState( void ) const
  {
	  if( m_state == STATE_LOADING ||
        m_state == STATE_PLAYING ||
        m_state == STATE_STOPPING){ return true; }

    return false;
  }

	//================================================================================================
  /**
   * @brief ボイス再生中ステート判定(ボイス再生+フェードアウト中はtrue)
   * @retval true = 再生中 : false 再生中でない
   */
	//================================================================================================
  b32 VoicePlayer::IsVoicePlayingState( void ) const
  {
	  if( m_state == STATE_PLAYING ||
        m_state == STATE_STOPPING){ return true; }

    return false;
  }
	
  //================================================================================================
  /**
   * @brief ボイスエフェクトリクエスト有効ステータス判定(読み込み + ボイス再生中はtrue)
   * @retval true = エフェクト有効中 : false エフェクトリクエストを受け付けない
   *
   * ※Stopリクエスト〜フェードアウト終了までは、サウンドハンドル自体は有効だが
   *　 エフェクトリクエストは受け付けないので注意
   */
	//================================================================================================
  b32 VoicePlayer::IsEffectEnableState( void ) const
  {
	  if( m_state == STATE_LOADING ||
        m_state == STATE_PLAYING ){ return true; }

    return false;
  }

	//================================================================================================
  /**
   * @brief 再生処理中判定(ボイス再生部のみ:ステートでの判定ではない)
   * @retval true = 再生中 : false 再生中でない
   */
	//================================================================================================
  b32 VoicePlayer::IsVoicePlaying( void ) const
  {
    if(m_playMode == PLAY_MODE_NORMAL){
			return( m_soundSystem->IsVoicePlaying(m_soundHandleIndex, m_seqItemID));
		} else {
			return( m_soundSystem->IsPlaying3DActor(m_3DActorIndex));
		}
  }

	//================================================================================================
  /**
	 *
   * @brief 再生エフェクト
	 *
   */
	//================================================================================================
	//================================================================================================
  /*
   * @brief 再生エフェクト 複数種のエフェクトをまとめてリクエスト
   * 
   * @param vEffectInfo  エフェクトパラメータ指定構造体参照
   * 
   * @note
   * この関数を呼び終わったらeffectInfoは破棄してOK
   */
	//================================================================================================
  void VoicePlayer::ChangeEffect( const VoicePlayInfo& vEffectInfo )
  {
    if(vEffectInfo.effect_enable & VEFFECT_BIT_VOLUME){
			u32 frame = (vEffectInfo.effect_enable & VEFFECT_BIT_CHANGE_VOLUME_FRAME)?
        vEffectInfo.change_volume_frame : 0;
				 
      this->ChangeVolume(vEffectInfo.volume, frame);
    }
    if(vEffectInfo.effect_enable & VEFFECT_BIT_PITCH){
      this->ChangePitch(vEffectInfo.pitch);
    } 
    if(vEffectInfo.effect_enable & VEFFECT_BIT_PAN){
      this->ChangePan(vEffectInfo.pan);
    } 
    if(vEffectInfo.effect_enable & VEFFECT_BIT_SURROUND_PAN){
      this->ChangeSurroundPan(vEffectInfo.surround_pan);
    } 
    if(vEffectInfo.effect_enable & VEFFECT_BIT_TEMPO ){
      this->ChangeTempo(vEffectInfo.tempo);
    }
    if(vEffectInfo.effect_enable & VEFFECT_BIT_POS){
      this->Change3DPos(vEffectInfo.pos3D);
    } 
    if(vEffectInfo.effect_enable & VEFFECT_BIT_VELOCITY){
      this->Change3DVelocity(vEffectInfo.velocity3D);
    }
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief  エフェクトリクエスト コントロール
   */
	//------------------------------------------------------------------------------------------------
  // フラグOn
  inline void VoicePlayer::OnEffectBit(const VoiceEffectBit effectBit)
		{ m_effect.effect_enable |= effectBit; }

  // フラグOff
  inline void VoicePlayer::OffEffectBit(const VoiceEffectBit effectBit)
		{ m_effect.effect_enable &= ~effectBit; }
  
  // フラグ取得
  inline b32 VoicePlayer::GetEffectBit(const VoiceEffectBit effectBit) const
		{ return(m_effect.effect_enable & effectBit); }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 再生エフェクト実行処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::VoiceEffectCore( void )
  {
		this->ChangeVolumeCore();
		this->ChangePitchCore();
		this->ChangePanCore();
		this->ChangeSurroundPanCore();
		this->ChangeTempoCore();
		this->Change3DPosCore();
		this->Change3DVelocityCore();
  }

	//================================================================================================
  /*
   * @brief ボリューム変更リクエスト処理
   * @param value  音量の倍率( 1.0fで等倍 )
   * @param change_frame 音量変化にかけるフレーム数(省略可：省略した場合は即時変化=0)
   */
	//================================================================================================
  void VoicePlayer::ChangeVolume( const f32 value, const u32 change_frame )
  {
		m_effect.volume = value;
		m_effect.change_volume_frame = change_frame;

		this->OnEffectBit(VEFFECT_BIT_VOLUME);
		if(change_frame > 0){
			this->OnEffectBit(VEFFECT_BIT_CHANGE_VOLUME_FRAME);
		}
  } 

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::ChangeVolumeCore( void )
  {
		if(GetEffectBit(VEFFECT_BIT_VOLUME) == false){ return; }

    if(m_playMode == PLAY_MODE_NORMAL){
			u32 frame = 0;
			if(GetEffectBit(VEFFECT_BIT_CHANGE_VOLUME_FRAME) != false){
				frame = m_effect.change_volume_frame;    
				this->OffEffectBit(VEFFECT_BIT_CHANGE_VOLUME_FRAME);
			}
			m_soundSystem->ChangeVoiceVolume(m_soundHandleIndex, m_effect.volume, frame);
		} else {
			m_soundSystem->Set3DActorVolume(m_3DActorIndex, m_effect.volume);
			this->OffEffectBit(VEFFECT_BIT_VOLUME);
		}
		this->OffEffectBit(VEFFECT_BIT_VOLUME);
  }

	//================================================================================================
  /*
   * @brief ピッチ変更リクエスト処理
   * @param value   周波数比率
   * @note
   * 周波数比率 0.5 = 1 オクターブ 低
   * 周波数比率 1.0 = 等倍 ( デフォルト値 )
   * 周波数比率 2.0 = 1 オクターブ 高
   */
	//================================================================================================
  void VoicePlayer::ChangePitch( const f32 value )
  {
		m_effect.pitch = value;
		this->OnEffectBit(VEFFECT_BIT_PITCH);
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::ChangePitchCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_PITCH) == false){ return; }

    if(m_playMode == PLAY_MODE_NORMAL){
			m_soundSystem->ChangeVoicePitch(m_soundHandleIndex, m_effect.pitch);
		} else {
			m_soundSystem->Set3DActorPitch(m_3DActorIndex, m_effect.pitch);
		}
		this->OffEffectBit(VEFFECT_BIT_PITCH);
  }

	//================================================================================================
  /**
   * @brief パン変更リクエスト処理
   * 
   * @param value   パン
   * @note
   * value が -1.0 なら, 音が左から聞こえる.
   * value が  0.0 なら, 音が正面から聞こえる.
   * value が +1.0 なら, 音が右から聞こえる.
   */
	//================================================================================================
  void VoicePlayer::ChangePan( const f32 value )
  {
		m_effect.pan = value;
		this->OnEffectBit(VEFFECT_BIT_PAN);
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::ChangePanCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_PAN) == false){ return; }

    if(m_playMode == PLAY_MODE_NORMAL){
			m_soundSystem->ChangeVoicePan(m_soundHandleIndex, m_effect.pan);
		} else {
			m_soundSystem->Set3DActorPan(m_3DActorIndex, m_effect.pan);
		}
		this->OffEffectBit(VEFFECT_BIT_PAN);
  }
  
	//================================================================================================
  /**
   * @brief サラウンド(垂直パン)変更リクエスト処理(現状、通常モードのみ有効)
   * @param value  パン
   * @note
   * value が  0.0 なら, 音が最前方から聞こえる.
   * value が +1.0 なら, 音が中央から聞こえる.
   * value が +2.0 なら, 音が最後方から聞こえる.
   */
	//================================================================================================
  void VoicePlayer::ChangeSurroundPan( const f32 value )
  {
    if(m_playMode == PLAY_MODE_NORMAL){
			m_effect.surround_pan = value;
			this->OnEffectBit(VEFFECT_BIT_SURROUND_PAN);
		}
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::ChangeSurroundPanCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_SURROUND_PAN) == false){ return; }

    if(m_playMode == PLAY_MODE_NORMAL){
			m_soundSystem->ChangeVoiceSurroundPan(m_soundHandleIndex, m_effect.surround_pan);
		}
		this->OffEffectBit(VEFFECT_BIT_SURROUND_PAN);
  }

	//================================================================================================
  /**
   * @brief テンポ変更リクエスト処理(現状、通常モードのみ有効)
   * @param value   テンポを倍率で指定( 1.0fで元に戻る )
   */
	//================================================================================================
  void VoicePlayer::ChangeTempo( const f32 value )
  {
    if(m_playMode == PLAY_MODE_NORMAL){
			m_effect.tempo = value;
			this->OnEffectBit(VEFFECT_BIT_TEMPO);
		}
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::ChangeTempoCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_TEMPO) == false){ return; }

    if(m_playMode == PLAY_MODE_NORMAL){
			m_soundSystem->ChangeVoiceTempoRatio(m_soundHandleIndex,m_effect.tempo);
		}
		this->OffEffectBit(VEFFECT_BIT_TEMPO);
  }
  
	//================================================================================================
  /**
   * @brief ポジション コントロール(3Dモードのみ有効)
   * @brief リクエスト処理
   * @param pos ポジション
   */
	//================================================================================================
  void VoicePlayer::Change3DPos( const gfl2::math::VEC3& pos )
  {
    if(m_playMode == PLAY_MODE_3DACTOR){
			m_effect.pos3D = pos;
			this->OnEffectBit(VEFFECT_BIT_POS);
		}
  }
 
	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::Change3DPosCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_POS) == false){ return; }

    if(m_playMode == PLAY_MODE_3DACTOR){
			m_soundSystem->Set3DActorPos(m_3DActorIndex, m_effect.pos3D);
		}
		this->OffEffectBit(VEFFECT_BIT_POS);
  }

	//================================================================================================
  /**
   * @brief ベロシティ コントロール(3Dモードのみ有効)
   * @brief リクエスト処理
   * @param velocity  ベロシティを指定
   */
	//================================================================================================
  void VoicePlayer::Change3DVelocity( const gfl2::math::VEC3& velocity )
  {
    if(m_playMode == PLAY_MODE_3DACTOR){
			m_effect.velocity3D = velocity;
			this->OnEffectBit(VEFFECT_BIT_VELOCITY);
		}
  }
  

	//================================================================================================
  /**
   * @brief サウンドのエフェクトセンド量を変更する
   *
   * @param busID AUXのバス
   * @param send  センド量
   */
	//================================================================================================
  b32 VoicePlayer::SetFxSend( nw::snd::AuxBus busID, float send )
  {
    nw::snd::SoundHandle* handle = m_soundSystem->GetSoundSystem()->GetSoundHandle( m_soundHandleIndex );
    if( handle->IsPrepared() )
    {
      handle->SetFxSend( busID, send );
      return true;
    }
    return false;
  }

	//------------------------------------------------------------------------------------------------
  /*
   * @brief 変更処理
   */
	//------------------------------------------------------------------------------------------------
  void VoicePlayer::Change3DVelocityCore( void )
  {
		if(this->GetEffectBit(VEFFECT_BIT_VELOCITY) == false){ return; }

    if(m_playMode == PLAY_MODE_3DACTOR){
			m_soundSystem->Set3DActorVelocity(m_3DActorIndex, m_effect.velocity3D);
		}
		this->OffEffectBit(VEFFECT_BIT_VELOCITY);
  }
  
} //namespace Sound 
