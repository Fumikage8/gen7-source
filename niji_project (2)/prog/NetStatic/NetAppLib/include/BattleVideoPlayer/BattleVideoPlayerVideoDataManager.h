#if !defined(NIJI_PROJECT_BATTLE_VIDEO_PLYAER_VIDEO_DATA_MANGER_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_PLYAER_VIDEO_DATA_MANGER_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoPlayerVideoDataManager.h
 @date    2015.09.24
 @note
          アプリで使うビデオデータの管理をする
 */
//==============================================================================
// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "PokeTool/include/PokeTool.h"

//  通信処理
#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoRequestClient.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

/**
  Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
    デバッグ機能の追加
*/
#if PM_DEBUG
  GFL_NAMESPACE_BEGIN(Debug)
    class VideoPlayerDataDebug;
  GFL_NAMESPACE_END(Debug)
#endif


  //  ビデオデータのプレイヤーデータ
  struct PLAYER_DATA
  {
    PLAYER_DATA()
    {
      trainerId     = trainer::TRTYPE_HERO;
      pName         = NULL;
      bUseIconData  = false;
    }

    u32                 trainerId;

    //  プレイヤー名
    gfl2::str::StrBuf*  pName;

    //  アイコンデータ
    Savedata::MyStatus::ICON_DATA  iconData;
    b8  bUseIconData;
  };

  //  ビデオデータ
  struct VIDEO_DATA
  {
    VIDEO_DATA()
    {
      pTitleName01  = NULL;
      pTitleName02  = NULL;
      selectMsgId   = 0;

      bDelFlg           = 0;
      bErrorVersionFlg  = 0;
      bServerDataFlg    = 0;
      saveDataListIdx   = 0;
    }

    //  模擬戦できるデータかどうか
    bool IsVsBtl()
    {
      //  バトルマルチとロイヤルでは模擬戦ができない
      const Regulation::BATTLE_RULE     rule      = (Regulation::BATTLE_RULE)sdCardData.upload.header.rule;
      const Regulation::BATTLE_CATEGORY category  = (Regulation::BATTLE_CATEGORY)sdCardData.upload.header.category;

      if( rule == Regulation::BATTLE_RULE_MULTI )
      {
        return  false;
      }
      else
      if( rule == Regulation::BATTLE_RULE_ROYAL )
      {
        return  false;
      }
      else
      if( category == Regulation::BATTLE_CATEGORY_BATTLE_FES )
      {
        return  false;
      }

      return  true;
    }

    //  ポケモン最大数
    u32 GetPokeMemberCount() const { return pml::PokeParty::MAX_MEMBERS; }

    //  ポケモンパラメータ出力
    //  左配置用
    bool  OutputLeftPosPokeParam( PokeTool::SimpleParam* pOutputParam, const u32 idx );
    //  右配置用
    bool  OutputRightPosPokeParam( PokeTool::SimpleParam* pOutputParam, const u32 idx );

    //  プレイヤーアイコンオブジェクトデータ出力
    bool  OutputIconObjectParam( app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA* pOutputParam, PLAYER_DATA& rPlayerData );

    //  主人公アイコンか?
    bool  IsPlayerTrainerIcon( PLAYER_DATA& rPlayerData );

    //  登録日時を更新
    void UpdateRegistDate()
    {
      if( sdCardData.registDate.year <= 0 )
      {
        return;
      }

      // @fix GFCHECK[2979]Dayの情報を初期化する。
      registDate.SetDay(1);

      registDate.SetYear( sdCardData.registDate.year );
      registDate.SetMonth( sdCardData.registDate.month );
      registDate.SetDay( sdCardData.registDate.day );
      registDate.SetHour( sdCardData.registDate.hour );
      registDate.SetMinute( sdCardData.registDate.minute );
      registDate.SetSecond( sdCardData.registDate.second );

      UCHIDA_PRINT( "登録日付 %d/%d/%d %d:%d:%d \n",
                    registDate.GetYear(), registDate.GetMonth(), registDate.GetDay(),
                    registDate.GetHour(), registDate.GetMinute(), registDate.GetSecond() );
    }

    //  作成日時を更新
    void UpdateCreateDate()
    {
      if( sdCardData.createDate.year <= 0 )
      {
        return;
      }

      // @fix GFCHECK[2979]Dayの情報を初期化する。
      createDate.SetDay(1);   /**<  @fix  MMcat[820]   バトルビデオの登録日時が正しく反映されない : registDateを変更していたのでcreateDateに修正  */

      createDate.SetYear( sdCardData.createDate.year );
      createDate.SetMonth( sdCardData.createDate.month );
      createDate.SetDay( sdCardData.createDate.day );
      createDate.SetHour( sdCardData.createDate.hour );
      createDate.SetMinute( sdCardData.createDate.minute );
      createDate.SetSecond( sdCardData.createDate.second );

      UCHIDA_PRINT( "作成日付 %d/%d/%d %d:%d:%d \n",
                    createDate.GetYear(), createDate.GetMonth(), createDate.GetDay(),
                    createDate.GetHour(), createDate.GetMinute(), createDate.GetSecond() );
    }

    //  タイトル名
    gfl2::str::StrBuf*  pTitleName01;
    gfl2::str::StrBuf*  pTitleName02;

    //  選択名のmsgId
    u8  selectMsgId;

    //  削除マークフラグ
    b8  bDelFlg;

    //  シリアル入力で取得したサーバーデータか
    b8  bServerDataFlg;

    // バージョン違いかどうか
    b8                    bErrorVersionFlg;

    //  アプリが参照するプレイヤーデータ
    PLAYER_DATA aPlayerData[ BTL_CLIENT_NUM ];

    ///<セーブデータのidx
    u32                 saveDataListIdx;

    //  SDカードのデータ
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA          sdCardData;
    gfl2::system::Date               createDate;   ///<作成日時
    gfl2::system::Date               registDate;   ///<登録日時
  };

  //==============================================================================
  /**
   @class     VideoDataManager
   */
  //==============================================================================
  class VideoDataManager : public NetApp::BattleVideo::BattleVideoResponseListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(VideoDataManager);

  public:
    enum
    {
      //  ビデオデータ最大数
      VIDEO_DATA_MAX  = ExtSavedata::BattleRecorderSaveData::MAX_SAVE_DATA,
    };

    //  オンラインかオフラインでデータを扱うか
    enum NetMode
    {
      NET_MODE_OFFLINE  = 0,
      NET_MODE_ONLINE,
    };

    //  SDデータロードの結果
    enum LoadResult
    {
      LOAD_RESULT_SUCCESS = 0,
      LOAD_RESULT_FATAL,
      LOAD_RESULT_ERROR,
      LOAD_RESULT_RUN,
    };

    //  アップロードの結果
    enum UploadResult
    {
      UPLOAD_RESULT_SUCCESS = 0,
      UPLOAD_RESULT_RUN,
      UPLOAD_RESULT_NG_DATA,
      UPLOAD_RESULT_ERROR,
      UPLOAD_RESULT_FATAL,
      //  アップロード個数ごえ
      UPLOAD_RESULT_OVERLIMIT,
    };

    //  アップロードキャンセルの結果
    enum UploadCancelResult
    {
      UPLOAD_CANCEL_RESULT_SUCCESS = 0,
      UPLOAD_CANCEL_RESULT_RUN,
      UPLOAD_CANCEL_RESULT_NOW_UPLOAD_DATA,
      UPLOAD_CANCEL_RESULT_ERROR,
      UPLOAD_CANCEL_RESULT_FATAL,
    };

    //  データ削除結果
    enum DeleteResult
    {
      DELETE_RESULT_RUN  = 0,
      DELETE_RESULT_SUCCESS,
      DELETE_RESULT_FATAL,
      DELETE_RESULT_ERROR,
    };

    //  シリアルNOのロード結果
    enum SerialNoLoadResult
    {
      SERIAL_NO_LOAD_RESULT_SUCCESS = 0,
      SERIAL_NO_LOAD_RESULT_RUN,
      SERIAL_NO_LOAD_RESULT_NOT_FOUND,
      SERIAL_NO_LOAD_RESULT_ERROR,
      SERIAL_NO_LOAD_RESULT_ERROR_BATCH_VERSION,
      SERIAL_NO_LOAD_RESULT_ERROR_SERVER_VERSION,
    };

    //  再生回数のアップ通知結果
    enum IncPlayCountResult
    {
      INC_PLAY_COUNT_RESULT_SUCCESS = 0,
      INC_PLAY_COUNT_RESULT_RUN,
      INC_PLAY_COUNT_RESULT_ERROR,
    };

    //  再生完走の通知結果
    enum SendPlayComplateResult
    {
      SEND_PLAY_COMPLATE_RESULT_SUCCESS = 0,
      SEND_PLAY_COMPLATE_RESULT_RUN,
      SEND_PLAY_COMPLATE_RESULT_ERROR
    };

    enum
    {
      LIST_IDX_ERROR  = 0xffffffff,
    };

    VideoDataManager( gfl2::heap::HeapBase* pHeap, const NetMode mode );
    virtual ~VideoDataManager();

    //  クラス破棄する前の終了処理(破棄する前に必ず呼ぶ)
    bool  EndFunc();

    //  ビデオデータ初期化
    void InitVideoData();
    //  ビデオデータロード
    LoadResult LoadVideoData();

    //  オンライン初期化
    //  オンライン状態で開始 true / オフライン状態で失敗 false
    bool InitOnline();
    //  オンラインデータロード
    LoadResult LoadOnline();

    //  シリアルNOからデータロードの準備
    bool InitSerialNoData();
    //  シリアルNOからデータロード
    SerialNoLoadResult LoadSerialNoData( const u64 no, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

    //  アップロード開始
    bool StartUpload( VIDEO_DATA* pUploadVideoData );

    //  アップロード中
    UploadResult WaitUpload();

    //  アップロードキャンセル開始
    bool StartUploadCancel( VIDEO_DATA* pUploadCancelVideoData );

    //  アップロードキャンセル中
    UploadCancelResult  WaitUploadCancel();

    //  破損ファイルを削除したかどうか
    bool IsDeleteBrokeData() const { return m_bDeleteBrokeData; }

    //  タイトル名更新
    void UpdateTitleName( gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

    //  削除フラグがついてるデータを削除開始
    //  削除開始成功 -> true / 失敗 -> false
    bool  StartDeleteVideoData();

    //  削除フラグがついているデータを削除更新してリストも更新
    DeleteResult  UpdateDeleteVideoData();

    //  再生回数のアップ開始
    bool  StartIncPlayCountVideoData( VIDEO_DATA* pVideoData );
    //  再生回数のアップ処理待ち
    IncPlayCountResult UpdateIncPlayCountVideoData();

    //  再生完走の通知
    bool  StartSendPlayComplete( VIDEO_DATA* pVideoData );
    //  再生完走の通知処理待ち
    SendPlayComplateResult  UpdateSendPlayComplate();

    //  データ取得
    VIDEO_DATA* GetListData( const u32 idx );

    //  シリアルNOで作成したデータ取得
    VIDEO_DATA* GetSerialData() const { return m_pResultSerialNoData; }

    //  指定シリアルNOのデータが公開データ取得
    VIDEO_DATA* GetSerialPublicData( const u64 no );

    //  指定したビデオデータのidxを取得
    u32 GetVideoDataListIdx( const VIDEO_DATA* pVideoData );

    //  削除フラグ取得・設定
    void SetDataDelFlg( const u32 idx, const bool bFlg );
    bool GetDataDelFlg( const u32 idx );
    //  削除フラグをクリア
    void ClearDataDelFlg();

    //  削除フラグがついている個数を取得
    u32 GetDataDelFlgCount();

    //  データ数取得
    u32 GetListDataNum() const { return m_listDataNum; }

    //  レコードセーブインスタンス取得
    ExtSavedata::BattleRecorderSaveData*  GetBtlRecorderSaveData() const { return m_pBtlRecorderSaveData; }

//  通信処理時のコールバック
    //--------------------------------------------------------------------------------------------
    /**
     * @brief   通信エラー
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnError( BattleVideoResponseListener::ERROR_TYPE errorType );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   同期成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnSyncSuccess( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   削除成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnDeleteSuccess( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   公開取り消し失敗（公開当日だったら取り消しに失敗する）
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnCancelUploadFailed( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   アップロード成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnUploadSuccess( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオが不正
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnUploadInvalid( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   ダウンロード成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnDownloadSuccess( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pDownloadData , u32 playCount , ExtSavedata::BattleRecorderSaveData::REC_DATE* pDate );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   ダウンロードしたいファイルが見つからない
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnDownloadNotFound( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   再生回数インクリメント成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnIncPlayCountSuccess( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   再生完走通知成功
     */
    //--------------------------------------------------------------------------------------------
    virtual void OnPlayCompleteSuccess( void );

  private:
    //  ビデオデータ削除シーケンス
    enum DeleteVideoDataSeq
    {
      //  サーバーデータ削除
      DELETE_VIDEO_DATA_SEQ_NET_START,
      DELETE_VIDEO_DATA_SEQ_NET_WAIT,
      DELETE_VIDEO_DATA_SEQ_END
    };

    //  ビデオデータシリアルNO作成シーケンス
    enum CreateDataSerialNoSeq
    {
      CREATE_DATA_SERIAL_NO_SEQ_CHK = 0,
      CREATE_DATA_SERIAL_NO_SEQ_DOWN_LOAD,
      CREATE_DATA_SERIAL_NO_SEQ_END,
    };

    //  ビデオデータアップロードシーケンス
    enum UploadVideoDataSeq
    {
      //  サーバー通知開始
      UPLOAD_VIDEO_DATA_SEQ_SERVER_START = 0,
      UPLOAD_VIDEO_DATA_SEQ_SERVER_WAIT,
      UPLOAD_VIDEO_DATA_SEQ_SERVER_END,

      UPLOAD_VIDEO_DATA_SEQ_END,
    };

    //  ビデオデータアップロードキャンセルシーケンス
    enum UploadCancelVideoDataSeq
    {
      //  サーバー通知開始
      UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_START = 0,
      UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_WAIT,
      UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_END,
    };

    //  ビデオデータの再生数アップシーケンス
    enum IncPlayCountVideoDataSeq
    {
      INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_START  = 0,
      INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_WAIT,
      INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_END,
    };

    //  ビデオデータの再生完走アップのシーケンスデータ
    enum SendPlayComplateVideoDataSeq
    {
      SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_START  = 0,
      SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_WAIT,
      SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_END,
    };

    //  ビデオデータを削除するシーケンスデータ
    struct DeleteVideoDataSeqData
    {
      DeleteVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        seq               = DELETE_VIDEO_DATA_SEQ_NET_START;
        delDataNum        = 0;
        nowDelDataIdx     = 0;
        netRequestResult  = DELETE_RESULT_RUN;
        gfl2::std::MemClear( pDeleteListData, sizeof( pDeleteListData ) );
      }

      DeleteVideoDataSeq                                seq;
      u32                                               delDataNum;
      u32                                               nowDelDataIdx;
      DeleteResult                                      netRequestResult;
      VIDEO_DATA*                                       pDeleteListData[ VIDEO_DATA_MAX ];
    };

    //  ビデオデータをシリアルno作成シーケンスデータ
    struct CreateSerialNoVideoDataSeqData
    {
      CreateSerialNoVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        seq = CREATE_DATA_SERIAL_NO_SEQ_CHK;
        result  = SERIAL_NO_LOAD_RESULT_RUN;
      }

      CreateDataSerialNoSeq seq;
      SerialNoLoadResult    result;
    };

    //  ビデオデータのアップロードシーケンスデータ
    struct UploadVideoDataSeqData
    {
      UploadVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        seq               = UPLOAD_VIDEO_DATA_SEQ_SERVER_START;
        result            = UPLOAD_RESULT_RUN;
        sdCardSaveResult  = ExtSavedata::BattleRecorderSaveData::RESULT_SUCCESS;
        pUploadData       = NULL;
      }

      VIDEO_DATA*         pUploadData;
      UploadVideoDataSeq  seq;
      UploadResult  result;

      ExtSavedata::BattleRecorderSaveData::Result sdCardSaveResult;
    };

    //  ビデオデータのアップロードキャンセルシーケンスデータ
    struct UploadCancelVideoDataSeqData
    {
      UploadCancelVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        pUploadCancelData = NULL;
        seq               = UPLOAD_CANCEL_VIDEO_DATA_SEQ_SERVER_START;
        sdCardSaveResult  = ExtSavedata::BattleRecorderSaveData::RESULT_SUCCESS;
        result            = UPLOAD_CANCEL_RESULT_RUN;
      }

      VIDEO_DATA*                                 pUploadCancelData;
      UploadCancelVideoDataSeq                    seq;
      ExtSavedata::BattleRecorderSaveData::Result sdCardSaveResult;
      UploadCancelResult                          result;
    };

    //  ビデオデータの再生回数加算のアップシーケンスデータ
    struct IncPlayCountVideoDataSeqData
    {
      IncPlayCountVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        seq     = INC_PLAY_COUNT_VIDEO_DATA_SEQ_SERVER_START;
        result  = INC_PLAY_COUNT_RESULT_RUN;
        pVideoData  = NULL;
      }

      VIDEO_DATA*               pVideoData;
      IncPlayCountVideoDataSeq  seq;
      IncPlayCountResult        result;
    };

    //  ビデオデータの再生完走アップのシーケンスデータ
    struct SendPlayComplateVideoDataSeqData
    {
      SendPlayComplateVideoDataSeqData()
      {
        Reset();
      }

      void Reset()
      {
        pVideoData  = NULL;
        seq         = SEND_PLAY_COMPLATE_VIDEO_DATA_SEQ_SERVER_START;
        result      = SEND_PLAY_COMPLATE_RESULT_RUN;
      }

      VIDEO_DATA*                   pVideoData;
      SendPlayComplateVideoDataSeq  seq;
      SendPlayComplateResult        result;
    };

#if PM_DEBUG
  //  テスト用のダミーデータ作成
    void _CreateDummyData( gfl2::heap::HeapBase* pHeap );
#endif

    //  ビデオデータのバッファ作成
    void _CreateDataBuffer( VIDEO_DATA* pOutputVideoData, gfl2::heap::HeapBase* pHeap );
    //  ビデオデータのバッファ削除
    void _DeleteDataBuffer( VIDEO_DATA* pOutputVideoData );

    //  タイトル名作成
    void _CreateTitleName( VIDEO_DATA* pOutputVideoData, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet );

    //  ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATAデータからVIDEO_DATAの中身を出力
    void _CreateVideoDataFromUploadData( VIDEO_DATA* pOutputVideoData, gfl2::heap::HeapBase* pHeap, const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pUploadData );

    //  プレイヤーのデータ構築
    void _CreatePlayerData( VIDEO_DATA* pOutputVideoData, const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_HEADER* pUploadHeaderData );

    //  リスト整理(削除フラグがついてるのを削除)
    void _RegulationList();

    void _Clear()
    {
      m_listDataNum           = 0;
      m_serialNoNotFound      = 0;
      m_pTmpStrBuf            = NULL;
      m_pBtlRecorderSaveData  = NULL;
      m_pHeap                 = NULL;
      m_pNetRequestClient     = NULL;
      m_pResultSerialNoData   = NULL;
      m_tmpLoadChkIdx         = 0;
      m_bDeleteBrokeData      = false;
      m_bUploadSelfData       = false;
      m_netMode               = NET_MODE_OFFLINE;
      m_resultSyncOnlineData  = LOAD_RESULT_SUCCESS;

      gfl2::std::MemClear( m_aBufferData, sizeof( m_aBufferData ) );
      gfl2::std::MemClear( m_pListData, sizeof( m_pListData ) );
      gfl2::std::MemClear( &m_netDeleteReqeustParam, sizeof( m_netDeleteReqeustParam ) );
    }

    VIDEO_DATA                                        m_aBufferData[ VIDEO_DATA_MAX ];
    VIDEO_DATA                                        m_serialNoData;
    
    //  シリアルNOからの結果アドレス
    VIDEO_DATA*                                       m_pResultSerialNoData;
    //  リストデータのデータアドレス
    VIDEO_DATA*                                       m_pListData[ VIDEO_DATA_MAX ];

    u32                                               m_listDataNum;
    u32                                               m_tmpLoadChkIdx;

    gfl2::str::StrBuf*                                m_pTmpStrBuf;

    ExtSavedata::BattleRecorderSaveData*              m_pBtlRecorderSaveData;

    gfl2::heap::HeapBase*                             m_pHeap;
    bool                                              m_bDeleteBrokeData;
    LoadResult                                        m_resultSyncOnlineData;

    NetMode                                           m_netMode;

    bool                                              m_bUploadSelfData;

#if PM_DEBUG
    bool                                              m_bDummyData;
#endif

    //  ビデオデータ削除のシーケンスデータ
    DeleteVideoDataSeqData                            m_deleteVideoDataSeqData;

    //  ビデオデータをシリアルNO作成シーケンスデータ
    CreateSerialNoVideoDataSeqData                    m_createSerirlaNoVideoDataSeqData;

    //  ビデオデータのアップロードシーケンスデータ
    UploadVideoDataSeqData                            m_uploadVideoDataSeqData;

    //  ビデオデータのアップロードキャンセルシーケンスデータ
    UploadCancelVideoDataSeqData                      m_uploadCancelVideoDataSeqData;

    //  ビデオデータの再生回数加算のアップシーケンスデータ
    IncPlayCountVideoDataSeqData                      m_incPlayCountVideoDataSeqData;

    //  ビデオデータの再生完走アップのシーケンスデータ
    SendPlayComplateVideoDataSeqData                  m_sendPlayComplateVideoDataSeqData;

    //  検索して見つからなかったシリアルNO
    u64                                               m_serialNoNotFound;

    //  通信用のパラメータ
    NetApp::BattleVideo::BattleVideoRequestClient*    m_pNetRequestClient;
    NetApp::BattleVideo::BattleVideoRequestClient::DELETE_REQUEST_PARAM         m_netDeleteReqeustParam;


/**
  Concact[217]   バトルビデオのサーバー時間デバッグ機能に関して
    デバッグ機能の追加
*/
#if PM_DEBUG
  Debug::VideoPlayerDataDebug*  m_pDebug;
#endif
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetAppLib)

#endif  // NIJI_PROJECT_BATTLE_VIDEO_PLYAER_VIDEO_DATA_MANGER_H_INCLUDED

