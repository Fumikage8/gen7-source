//=============================================================================================
/**
 * @file   AppUtilFileControlState.h
 * @brief  ファイル制御ステート
 * @author yuto.uchida
 * @date   2015.5.15
 * @note
 *         arcファイルを開いて、dataをロードして、arcファイルを閉じる制御する
 */
//=============================================================================================
#include "AppLib/include/Util/AppUtilFileControlState.h"

#include "AppLib/include/Util/app_util_FileAccessor.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  //  Start関数でファイルを開く、End関数でファイルを閉じる処理をする手動用
  AppUtilFileControlState::AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    _Clear();

    m_arcId       = arcId;
    m_dataId      = gfl2::fs::ArcFile::ARCDATID_NULL;
    m_pUseHeap    = pUseHaep;
    m_bFileComp   = bFileComp;
    m_fileAlign   = align;
    m_fileLang    = lang;
    m_bSeqAuto    = false;
  }

  //  ファイルの開く、ロード、閉じるの処理をする自動処理用
  AppUtilFileControlState::AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::fs::ArcFile::ARCDATID dataId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    _Clear();

    m_arcId                 = arcId;
    m_dataId                = dataId;
    m_pUseHeap              = pUseHaep;
    m_bFileComp             = bFileComp;
    m_fileAlign             = align;
    m_fileLang              = lang;
    m_bSeqAuto              = true;
    m_bOpenCloseHeapLowwer  = true;
  }

  AppUtilFileControlState::~AppUtilFileControlState()
  {
    GflHeapSafeFreeMemory( m_pLoadBuffer );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイル制御開始！！
    * @note
    *          すでに開始しているのであれば処理しない
    *          再び開始するにはEnd()関数を呼んできちんと終わらせないといけない
    *          きちんと終わっているいるかは IsEnd()関数で取得
    */
    //------------------------------------------------------------------
  bool AppUtilFileControlState::Start()
  {
    if( IsEnd() )
    {
      //  すでに開始しているのであれば処理しない
      //  再び開始するにはきちんと終わらせないといけない
      if( m_bStart == false )
      {
        m_bStart  = true;
        m_bEnd    = false;
        m_bOpen   = false;

        _SetState( FILE_STATE_TYPE_OPEN );

        return  true;
      }
    }
    else
    {
      //  すでに処理が走っている段階で呼ぶのはNG
      GFL_ASSERT( 0 );
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ファイルバイナリロード開始
    * @note
    *        下記のコンストラクタを呼び出した場合に有効
    *        AppUtilFileControlState( gfl2::fs::ArcFile::ARCID arcId, gfl2::heap::HeapBase* pUseHaep, bool bFileComp, u32 align, gfl2::fs::ToolDefine::ArcLangType::Tag lang );
    */
  //------------------------------------------------------------------
  void AppUtilFileControlState::Load( gfl2::fs::ArcFile::ARCDATID dataId, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    GFL_ASSERT( m_bSeqAuto == true );
    GFL_ASSERT( gfl2::fs::ArcFile::ARCDATID_NULL != dataId );

    m_dataId  = dataId;
    m_fileLang = lang;

    _StartFileLoad();
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理をきちんと終わらせる
    * @note
    *          ロードしたデータが削除されるので注意
    */
  //------------------------------------------------------------------
  void AppUtilFileControlState::End()
  {
    //  ファイルロード中で呼ばれる可能性があるので、ロードが終わったらフラグを見てロードデータを開放
    m_bEnd    = true;
    m_bStart  = false;

    if( m_bSeqAuto == true )
    {
      //  もうロードが終わっているなら開放
      if( m_pLoadBuffer != NULL )
      {
        GflHeapSafeFreeMemory( m_pLoadBuffer );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   処理がきちんと終わっているか
    * @return  true -> きちんと終わってインスタンスを破棄できる / false -> まだちゃんと終わっていないのでインスタンスの破棄もできない
    */
  //------------------------------------------------------------------
  bool  AppUtilFileControlState::IsEnd()
  {
    if( IsCtrlEnd() || ( m_state == FILE_STATE_TYPE_NONE ) )
    {
      return  true;
    }

    return  false;
  }

  //  ファイル制御が終了しているかどうか
  bool AppUtilFileControlState::IsCtrlEnd()
  {
    return ( m_state == FILE_STATE_TYPE_END );
  }

  void AppUtilFileControlState::Update()
  {
    if( m_state == FILE_STATE_TYPE_OPEN )
    {
      if( app::util::FileAccessor::IsFileOpen( m_arcId ) )
      {
        if( m_bEnd == true )
        {
          //  終了通知しているのでロードせずに開いたのを閉じる
          _SetState( FILE_STATE_TYPE_CLOSE );
        }
        else
        {
          m_bOpen = true;
          if( m_bSeqAuto == false )
          {
            _SetState( FILE_STATE_TYPE_MANUAL_PROC_IDLE );
          }
          else
          {
            _SetState( FILE_STATE_TYPE_LOAD );
          }
        }
      }
    }

    if( m_state == FILE_STATE_TYPE_MANUAL_PROC_IDLE )
    {
      if( m_bEnd )
      {
        if( m_bOpen == true )
        {
          _SetState( FILE_STATE_TYPE_CLOSE );
        }
        else
        {
          _SetState( FILE_STATE_TYPE_END );
        }
      }
    }

    if( m_state == FILE_STATE_TYPE_LOAD )
    {
      if( app::util::FileAccessor::IsFileLoad( &m_pLoadBuffer ) )
      {
        //  終了しているので削除
        if( m_bEnd == true )
        {
          GflHeapSafeFreeMemory( m_pLoadBuffer );
          _SetState( FILE_STATE_TYPE_CLOSE );
        }
        else
        if( m_bSeqAuto == false )
        {
          //  マニュアル処理はマニュアル待機ステートに遷移
          _SetState( FILE_STATE_TYPE_MANUAL_PROC_IDLE );
        }
        else
        {
          //  自動処理はすぐにファイルを閉じる
          _SetState( FILE_STATE_TYPE_CLOSE );
        }
      }
    }

    if( m_state == FILE_STATE_TYPE_CLOSE )
    {
      if( app::util::FileAccessor::IsFileClose( m_arcId ) )
      {
        _SetState( FILE_STATE_TYPE_END );
      }
    }

    if( m_state == FILE_STATE_TYPE_END )
    {
    }
  }

  bool AppUtilFileControlState::_StartFileOpen()
  {
    if( m_arcId == gfl2::fs::ArcFile::ARCID_NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    app::util::FileAccessor::FileOpen( m_arcId, m_pUseHeap, m_bOpenCloseHeapLowwer, m_fileLang );

    return  true;
  }

  bool AppUtilFileControlState::_StartFileLoad()
  {
    if( m_dataId == gfl2::fs::ArcFile::ARCDATID_NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_bOpen == false )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  すでにロードしているなら削除する
    if( m_pLoadBuffer != NULL )
    {
      GflHeapSafeFreeMemory( m_pLoadBuffer );
    }

    //  ファイルのロード
    app::util::FileAccessor::FileLoad(
                                        m_arcId,
                                        m_dataId,
                                        &m_pLoadBuffer,
                                        m_pUseHeap,
                                        m_bFileComp,
                                        m_fileAlign,
                                        m_fileLang );

    return  true;
  }

  bool AppUtilFileControlState::_StartFileClose()
  {
    if( m_arcId == gfl2::fs::ArcFile::ARCID_NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  ファイルを閉じる
    app::util::FileAccessor::FileClose( m_arcId, m_pUseHeap );

    return  true;
  }

  //  ステート変更
  void AppUtilFileControlState::_SetState( const FileStateType state )
  {
    m_state = state;

    if( m_state == FILE_STATE_TYPE_OPEN )
    {
      //  ファイル開くのに失敗したらロードに遷移
      if( _StartFileOpen() == false )
      {
        _SetState( FILE_STATE_TYPE_LOAD );
      }
    }

    if( m_state == FILE_STATE_TYPE_LOAD )
    {
      if( _StartFileLoad() == false )
      {
        _SetState( FILE_STATE_TYPE_CLOSE );
      }
    }

    if( m_state == FILE_STATE_TYPE_CLOSE )
    {
      if( _StartFileClose() == false )
      {
        _SetState( FILE_STATE_TYPE_END );
      }
    }

    if( m_state == FILE_STATE_TYPE_END )
    {
    }
  }

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
