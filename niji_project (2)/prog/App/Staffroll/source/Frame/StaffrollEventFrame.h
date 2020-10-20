// ============================================================================
/*
 * @file StaffrollEventFrame.h
 * @brief スタッフロールアプリのプロセスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_FRAME_H_INCLUDE )
#define STAFFROLL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "App/Staffroll/source/System/StaffrollWork.h"

//  リソースのインクルード
#include <niji_conv_header/app/staffroll/data/staffroll_data.h>
#include <niji_conv_header/app/staffroll/data/staffroll_data_img_evt.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)

  class StaffrollTextBox;

GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(Frame)

  class StaffrollEventFrame : public NetAppLib::System::NetAppFrameBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollEventFrame );
  public:
    StaffrollEventFrame(
      App::Staffroll::System::StaffrollWork* pStaffrollWork );
    virtual~StaffrollEventFrame();

    virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  private:
    // NetAppFrameBase
    virtual bool startup();
    virtual bool cleanup();
    virtual void setListener();
    virtual void removeListener();
    virtual void updateSequence();

  private:

    enum
    {
      REQ_NONE,
      REQ_CREATE,
      REQ_DESTROY,
    };

    enum
    {
      STAFFROLL_FAST_FORWRD_SPEED = 3,
    };

    enum
    {
      UPDATE_SEQ_FADE_IN_START  = 0,
      UPDATE_SEQ_FADE_IN_WAIT,
      UPDATE_SEQ_COMMAND_PROC,
      UPDATE_SEQ_FADE_OUT,
      UPDATE_SEQ_END,
    };

    //  テキストイベント
    class TextEvent
    {
      GFL_FORBID_COPY_AND_ASSIGN( TextEvent );
    public:
      //コマンド
      enum CommandResult
      {
        COMMAND_RESULT_CONTINUE,  //現在のデータ処理が終わっていない
        COMMAND_RESULT_FINISH,    //現在のデータ処理が終わった
        COMMAND_RESULT_NEXTDATA,  //現在のデータ処理が終わり、すぐに次のデータを連続読み込み
      };

      //コマンド状態
      enum CommandState
      {
        COMMAND_STATE_NORMAL,
        COMMAND_STATE_LUMPPUT,
        COMMAND_STATE_ALPHAPUT,
      };

      /**
       * @brief スタッフロールリストデータ
       */
      struct StaffRollData
      {
        u32 message;
        u32 label;
        u32 wait;
        s32 y;
        u32 strpos;
        s32 x;
        s32 xofs;
        u32 font;
        u32 x_color;
        u32 y_color;
      };

      TextEvent( App::Staffroll::System::StaffrollWork* pWork );
      virtual ~TextEvent();

      //シーケンス
      typedef bool (StaffrollEventFrame::TextEvent::*Sequence)( void );
      void UpdateSequence( void );
      void SetSequence( Sequence sequence );
  
      //  コマンド一覧
      typedef CommandResult (StaffrollEventFrame::TextEvent::*CommandFunction)( const StaffRollData & data, CommandState state );
      CommandFunction CommandFactory( u32 label );
      void UpdateCommand( void ); 
      void SetCommandState( CommandState state );

      //  クレジット表記終了か
      bool IsEndCredit() const { return m_bEndCredit; }

      //  コマンド終了か
      bool IsEndCommand() const { return m_bEndCommand; }

    private:
      bool SequenceWait( void );    //停止中
      bool SequenceScroll( void );  //スクロール

      CommandResult Command_StartPut( const StaffRollData & data, CommandState state );
      CommandResult Command_EndPut( const StaffRollData & data, CommandState state );
      CommandResult Command_StartScroll( const StaffRollData & data, CommandState state );
      CommandResult Command_EndScroll( const StaffRollData & data, CommandState state );
      CommandResult Command_Wait( const StaffRollData & data, CommandState state );
      CommandResult Command_Space( const StaffRollData & data, CommandState state );
      CommandResult Command_String( const StaffRollData & data, CommandState state );
      CommandResult Command_Clear( const StaffRollData & data, CommandState state );
      CommandResult Command_StringX( const StaffRollData & data, CommandState state );
      CommandResult Command_StringY( const StaffRollData & data, CommandState state );
      CommandResult Command_End( const StaffRollData & data, CommandState state );
      CommandResult Command_StartAlpha( const StaffRollData & data, CommandState state );
      CommandResult Command_EndAlpha( const StaffRollData & data, CommandState state );
      CommandResult Command_AlphaClear( const StaffRollData & data, CommandState state );
  
      CommandResult Command_StringCore( const StaffRollData & data, CommandState state );

      void _Clear()
      {
        m_eventDataMax  = 0;
        m_eventDataPos  = 0;
        m_counter     = 0;
        m_command_state = COMMAND_STATE_NORMAL;

        m_pEventData  = NULL;
        m_pCreditLastTextBox  = NULL;

        m_sequence    = NULL;
        m_pWork       = NULL;
        m_pParamData  = NULL;
        m_bEndCredit  = false;
        m_bEndCommand = false;
      }

      StaffRollData* m_pEventData;
      u32            m_eventDataMax;
      u32            m_eventDataPos;

      //シーケンス 
      Sequence      m_sequence;

      //コマンド
      CommandState  m_command_state;

      //  クレジットの最後に表記
      App::Staffroll::StaffrollTextBox* m_pCreditLastTextBox;

      StaffrollParamBinaryFormat*  m_pParamData;

      u32   m_counter;

      bool  m_bEndCredit;
      bool  m_bEndCommand;

      App::Staffroll::System::StaffrollWork*                    m_pWork;
    };

    //  １枚絵イベント
    class ImgEvent
    {
      GFL_FORBID_COPY_AND_ASSIGN( ImgEvent );
    public:
      //コマンド
      enum CommandResult
      {
        COMMAND_RESULT_CONTINUE,  //現在のデータ処理が終わっていない
        COMMAND_RESULT_FINISH,    //現在のデータ処理が終わった
        COMMAND_RESULT_NEXTDATA,  //現在のデータ処理が終わり、すぐに次のデータを連続読み込み
      };

      //コマンド状態
      enum CommandState
      {
        COMMAND_STATE_NORMAL  = 0,
      };

      ImgEvent( App::Staffroll::System::StaffrollWork* pWork );
      virtual ~ImgEvent();

      //シーケンス
      typedef bool (StaffrollEventFrame::ImgEvent::*Sequence)( void );
      void UpdateSequence( void );
      void SetSequence( Sequence sequence );
  
      //  コマンド一覧
      typedef CommandResult (StaffrollEventFrame::ImgEvent::*CommandFunction)( const StaffrollImgEventItemData & data, CommandState state );
      CommandFunction CommandFactory( u32 label );
      void UpdateCommand( void ); 
      void SetCommandState( CommandState state );

      //  コマンド終了か
      bool IsEndCommand() const { return m_bEndCommand; }

    private:
      bool SequenceWait( void );    //停止中

      //  コマンド一覧
     	//  STAFFROLL_IMG_EVENT_LABEL_WAIT, //待機
      //  指定したフレームまで次のコマンドへいかない
      CommandResult Command_Wait( const StaffrollImgEventItemData & data, CommandState state );

      //  STAFFROLL_IMG_EVENT_LABEL_FADE_IN, //フェードイン
      //  画面のフェードインが終了するまで次のコマンドへいかない
      CommandResult Command_FadeIn( const StaffrollImgEventItemData & data, CommandState state );

	    //  STAFFROLL_IMG_EVENT_LABEL_FADE_OUT, //フェードアウト
      //  画面のフェードアウトが終了するまで次のコマンドへいかない
      CommandResult Command_FadeOut( const StaffrollImgEventItemData & data, CommandState state );

	    //  STAFFROLL_IMG_EVENT_LABEL_SET_IMG, //画像設定
      //  画像ロード設定をする。非同期でロード終了するまで次のコマンドへいかない
      CommandResult Command_SetImg( const StaffrollImgEventItemData & data, CommandState state );

      void _Clear()
      {
        m_eventDataMax  = 0;
        m_eventDataPos  = 0;
        m_counter     = 0;
        m_cmdSeq      = 0;
        m_pEventData  = NULL;

        m_sequence    = NULL;
        m_pWork       = NULL;
        m_bEndCommand = false;
        m_nowTrainerSex = pml::SEX_MALE;
        m_command_state = COMMAND_STATE_NORMAL;
      }

      StaffrollImgEventItemData* m_pEventData;
      u32            m_eventDataMax;
      u32            m_eventDataPos;

      //シーケンス 
      Sequence      m_sequence;

      //コマンド
      CommandState  m_command_state;

      u32   m_cmdSeq;
      u32   m_counter;

      bool  m_bEndCommand;
      u8    m_nowTrainerSex;

      App::Staffroll::System::StaffrollWork*                    m_pWork;
    };

    //  イベントの更新とシーケンス更新
    void _UpdateEventSequence();
    void _UpdateEventCommand();

    //  更新処理中のシーケンス設定
    u32 _StartUpdateSeq( const u32 seq );

    void _EndBGM( const u32 fadeFrame );

    void _Clear()
    {
      m_initSeq = m_endSeq  = 0;
      m_bPlayFlg    = false;
      m_bForceEnd   = false;
      m_bBGMEnd     = false;
      m_req         = 0;
      m_updateFrameCount  = 0;

      m_updateSubSeq  = 0;
      m_foceEndSeq  = 0;
      m_pWork       = NULL;
      m_pTextEvent  = NULL;
      m_pImgEvent   = NULL;
    }
    // スタッフロールリストデータ
    s32   m_req;
    bool  m_bPlayFlg;
    bool  m_bForceEnd;
    bool  m_bBGMEnd;

    App::Staffroll::System::StaffrollWork*                    m_pWork;

    u32 m_initSeq, m_endSeq;
    u32 m_updateSubSeq;
    u32 m_foceEndSeq;
    //  スタッフロール開始から終了までの総フレーム数取得するため(BGMの再生との調整のため)
    u32 m_updateFrameCount;

    TextEvent*  m_pTextEvent;
    ImgEvent*   m_pImgEvent;

    static const u32 ms_fadeTime;
  };

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLL_FRAME_H_INCLUDE
