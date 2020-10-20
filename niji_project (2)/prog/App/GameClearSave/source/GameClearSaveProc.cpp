//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveProc.cpp
 *	@brief  ゲームクリアー時のセーブ画面プロセスクラス
 *	@author	Toru=Nagihashi
 *	@date		2013.01.29
 *	@author koji_kawada@gamefreak.co.jp
 *	@date   2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <fs/include/gfl2_Fs.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <App/GameClearSave/include/GameClearSaveProc.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyStatus.h>
#include "GameClearSaveView.h"
#include <System/include/HeapDefine.h>
#include <Sound/include/Sound.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)

//-----------------------------------------------------------------------
// コンストラクタ・デストラクタ
//-----------------------------------------------------------------------
/**
 *	@brief  コンストラクタ
 *	@param  param 引数
 */
Proc::Proc( APP_PARAM *param ) :
  GameSys::GameProc(),
  m_heap(),
  m_param( param ),
  m_seq( SEQ_START )
{
  //ヒープ作成
  gfl2::heap::HeapBase* parentDevMem  = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  m_heap.Create( parentDevMem, parentDevMem, HEAPID_GAME_CLEAR_SAVE_SYSTEM, SYSTEM_HEAP_SIZE, HEAPID_GAME_CLEAR_SAVE_DEVICE, DEVICE_HEAP_SIZE ); 
}

/**
 *	@brief  デストラクタ
 */
Proc::~Proc()
{
  //ヒープ破棄
  m_heap.Delete();
}



//-----------------------------------------------------------------------
// GameProcオーバーライト
//-----------------------------------------------------------------------
/**
 *	@brief  PROC初期化
 *	@param	Manager プロセスマネージャー
 *	@return Result 動作終了コード
 */
gfl2::proc::Result Proc::InitFunc(gfl2::proc::Manager * )
{
  enum
  {
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_EXIT,
  };

  switch( this->GetSubSeq() )
  {
  case SEQ_INIT:
    {
      // クリアカラーを覚えておき、変更する。
      for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
      {
        this->GetGameManager()->GetBufferClearSetting(static_cast<gfl2::gfx::CtrDisplayNo>(i), &(m_bufferClearSettingOriginal[i]));
      }
      for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
      {
        GameSys::GameManager::BUFFER_CLEAR_SETTING blackSetting = m_bufferClearSettingOriginal[i];
        blackSetting.color.SetR(0.0f);
        blackSetting.color.SetG(0.0f);
        blackSetting.color.SetB(0.0f);
        this->GetGameManager()->SetBufferClearSetting(static_cast<gfl2::gfx::CtrDisplayNo>(i), blackSetting);
      }

      gfl2::fs::AsyncFileManager *file_manager = this->GetGameManager()->GetAsyncFileManager();
      m_view  = GFL_NEW(m_heap.GetSystemHeap()) View(&m_heap,file_manager);
    
      this->AddSubSeq();
    }
    // fall
  case SEQ_WAIT:
    if( m_view->Initialize() )
    {
      this->AddSubSeq();
    }
    break;
  case SEQ_EXIT:
    return gfl2::proc::RES_FINISH; 
  }

  return gfl2::proc::RES_CONTINUE; 
}

/**
 *	@brief  PROC破棄
 *	@param	Manager プロセスマネージャー
 *	@return Result 動作終了コード
 */
gfl2::proc::Result Proc::EndFunc(gfl2::proc::Manager * )
{
  enum
  {
    SEQ_FINALIZE,
    SEQ_DELETE,
    SEQ_EXIT,
  };

  switch( this->GetSubSeq() )
  {
  case SEQ_FINALIZE:
    if( m_view->Finalize() )
    {
      this->AddSubSeq();
    }
    break;
  case SEQ_DELETE:
    GFL_SAFE_DELETE( m_view );
    this->AddSubSeq();
    // fall
  case SEQ_EXIT:
    /*
    // クリアカラーを元に戻す << momijiでは戻さず、黒のままにする
    for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
    {
      this->GetGameManager()->SetBufferClearSetting(static_cast<gfl2::gfx::CtrDisplayNo>(i), m_bufferClearSettingOriginal[i]);
    }
    */
    this->ResetSubSeq();
    return gfl2::proc::RES_FINISH;
  }
 
  return gfl2::proc::RES_CONTINUE; 
}

/**
 *	@brief  PROCメイン
 *	@param	Manager プロセスマネージャー
 *	@return Result 動作終了コード
 */
gfl2::proc::Result Proc::UpdateFunc(gfl2::proc::Manager * )
{
  enum
  {
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };
  
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  const gfl2::math::Vector4 white( 1.0f, 1.0f, 1.0f, 1.0f );
  const gfl2::math::Vector4 black( 0.0f, 0.0f, 0.0f, 1.0f );
  const gfl2::math::Vector4 black_transparency( 0.0f, 0.0f, 0.0f, 0.0f );

  switch( this->GetSubSeq() )
  { 
  case SEQ_FADEIN_START:
    {      
      pFadeManager->ForceOut( gfl2::Fade::DISP_UPPER, &black );  // sangoでは黒
      pFadeManager->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );  // sangoでは黒
      
      pFadeManager->ForceOut( gfl2::Fade::DISP_LOWER, &black );
    }
    this->AddSubSeq();
    //fall
  case SEQ_FADEIN_WAIT:
    if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
    {
      this->AddSubSeq();
    }
    break;
  case SEQ_MAIN:
    if( this->UpdateSequence() )
    {
      this->AddSubSeq();
    }
    break;
  case SEQ_FADEOUT_START:
    pFadeManager->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &black_transparency, &black );  // sangoでは黒
    this->AddSubSeq();
    //fall
  case SEQ_FADEOUT_WAIT:
    if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
    {
      this->ResetSubSeq();
      return gfl2::proc::RES_FINISH;
    }
  }
  
  m_view->UpdateTree();

  return gfl2::proc::RES_CONTINUE;
}

void Proc::PreDrawFunc(gfl2::proc::Manager * )
{}

/**
 *	@brief  PROC描画
 *	@param	Manager プロセスマネージャー
 *	@return Result 動作終了コード
 */
void Proc::DrawFunc(gfl2::proc::Manager * , gfl2::gfx::CtrDisplayNo displayNo)
{
  m_view->DrawTree(displayNo);
  if( m_view->GetAppRnderingManager() ) m_view->GetAppRnderingManager()->Draw(displayNo);
}



//-----------------------------------------------------------------------
// PRIVATE
//-----------------------------------------------------------------------
/**
 * @brief シーケンス更新
 *
 * @return  trueアプリ終了  false継続
 */
bool Proc::UpdateSequence( void )
{
  switch( m_seq )
  {
  case SEQ_START:
    m_seq = SEQ_START_SAVE;
    //fall
  case SEQ_START_SAVE:
    m_view->StartMessage( report_02_02, View::MESSAGE_TYPE_TIMER_ICON );
    this->StartSave();
    m_seq = SEQ_WAIT_SAVE;
    //fall
  case SEQ_WAIT_SAVE:
    {
      bool is_message_end = m_view->IsFinishMessage();
      SaveResult  save_result = this->UpdateSave();
      if( is_message_end 
          && save_result != SAVE_RESULT_PROCESSING )
      {
        if( save_result == SAVE_RESULT_SUCCESS )
        {
          Sound::PlaySE( SEQ_SE_SAVE );  //増田さん要望SEなし→nijiではSEありになった。
          m_seq = SEQ_START_SUCCESS_MSG;
        }
        else
        {
          m_seq = SEQ_START_FAILURE_MSG;
        }
      }
    }
    break;
  case SEQ_START_SUCCESS_MSG:
    {
      GameSys::GameData* gamedata = this->GetGameManager()->GetGameData();
      const Savedata::MyStatus* mystatus = gamedata->GetPlayerStatus();
      m_view->GetG2DUtil()->GetWordSet()->RegisterPlayerName( 0, mystatus );
      m_view->StartMessage( report_02_03, View::MESSAGE_TYPE_WAIT );
    }
    m_seq = SEQ_WAIT_MSG;
    break;
  case SEQ_START_FAILURE_MSG:
    m_view->StartMessage( report_02_05, View::MESSAGE_TYPE_WAIT );
    m_seq = SEQ_WAIT_MSG;
    break;
  case SEQ_WAIT_MSG:
    if( m_view->IsFinishMessage() )
    {
      m_seq = SEQ_END;
    }
    break;
  case SEQ_END:
    return true;
  }

  return false;
}

/**
 * @brief セーブ開始
 */
void Proc::StartSave( void )
{
  GameSys::GameData* gamedata = this->GetGameManager()->GetGameData();
  //gamedata->GetRecordSaveData()->Inc( savedata::Record::RECID_REPORT_COUNT );  // ←はnijiではないのでコメントアウト。

  gamedata->GetPlayTime()->PushSaveTime();

  //gamedata->GetPlayTime()->SetSaveTime( gamedata->GetGameTime() );
  // ↓のようにnijiでは変わった。
  Savedata::GameTime gameTime;
  GameSys::DeviceDate date;
  gameTime.SetDate( date.GetDateConst() );
  gamedata->GetPlayTime()->SetSaveTime( &gameTime );

  gamedata->SaveStart( m_heap.GetSystemHeap()->GetLowerHandle() );
  
  m_save_result = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;
}
/**
 * @brief セーブ更新
 *
 * @return セーブ結果
 */
Proc::SaveResult Proc::UpdateSave( void )
{
  GameSys::GameData* gamedata = this->GetGameManager()->GetGameData();
  if( m_save_result == gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE )
  {
    m_save_result = gamedata->SaveMain();
  }
  switch( m_save_result )
  {
  case gfl2::fs::FsCtrBackupSystem::RESULT_OK:
    return SAVE_RESULT_SUCCESS;
  case gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE:
    gamedata->GetPlayTime()->PopSaveTime();
    return SAVE_RESULT_PROCESSING;
  default:
    gamedata->GetPlayTime()->PopSaveTime();
    GFL_PRINT( "失敗理由:%d\n", m_save_result );
    return SAVE_RESULT_FAILURE;
  }
}
/**
 * @brief セーブロックフラグ
 *
 * @return trueロック中 falseロックしてない
 */
bool Proc::IsLockSave( void )
{
  //GameSys::GameData* gamedata = this->GetGameManager()->GetGameData();
  //return gamedata->IsSaveLock();
  
  // ↑はnijiではないのでfalseを返す。
  return false;
}

GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)

