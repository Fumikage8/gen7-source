//======================================================================
/**
 * @file  BtlvUiMsgWin.cpp
 * @brief バトル描画 メッセージ
 * @author  ariizumi  -> k.ohno
 * @data  10/11/15      2015.05.02
 */
//======================================================================

#include "btlv_ui.h"

#include "BtlvUiMsgWin.h"

#include "System/include/HeapDefine.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/battleUpperLyt.gaix>

#include <niji_conv_header/app/battle/upper/minupp.h>
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>
#include <niji_conv_header/app/battle/upper/minupp_pane.h>

#include <Sound/include/Sound.h>

#include "Print/include/SystemFont.h"

#if PM_DEBUG
//文字変換用
#include "Test/AriizumiTest/include/AutoBattleProc.h"
#endif

#if KANTAIHANTAI_TEST
#include "arc_index/message_Simp_Chinese.gaix"
#include "arc_index/message_Trad_Chinese.gaix"
#endif

GFL_NAMESPACE_BEGIN(btl)
GFL_NAMESPACE_BEGIN(app)


enum{
  READ_NONE_,
  OPEN_ING_,
  READ_ING_,
  CLOSE_ING_,
  END_ING_,
};


enum{
  MAIN_LAYOUT_ = 0,
  MESSAGE_WINDOW_ = 0
};


  // メッセージウィンドウ用ペイン
static const gfl2::lyt::LytPaneIndex messageWindowPane[] =
{
  PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_01,
  PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_02,
};


BtlvUiMsgWin::BtlvUiMsgWin(BattleViewSystem* pcore, ::app::util::Heap *pheap) :
::app::ui::UIView( pheap, pheap->GetDeviceAllocator(), pheap->GetDeviceAllocator() )
// ,mMsgCursor(NULL)
,mIsUpdateMessage( false )
,mIsVisible( false )
,mIsVisibleMsgCursor( false )
,mMsgSpeed(Savedata::ConfigSave::MSG_NORMAL)
,mCallBackTarget(NULL)
{
  m_pAppHeap = pheap;
  m_pBtlvCore = pcore;
  m_FileMode = READ_NONE_;
  m_renderingManager = pcore->GetRenderSys()->GetAppRenderingManager();
}


BtlvUiMsgWin::~BtlvUiMsgWin()
{
  //このクラスはファイル読み込み中は削除してはいけない
  GFL_ASSERT( (( m_FileMode == READ_NONE_)||( m_FileMode == END_ING_)) );

  if(m_pArcReadBuff)
  {
    GflHeapFreeMemory(m_pArcReadBuff);
    m_pArcReadBuff=NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

void BtlvUiMsgWin::fileReadStructSetup(::app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, ::app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}




/**
 * @brief レイアウトワーク作成  レイアウトデータを外部から受け取ってセットアップします
 */
bool BtlvUiMsgWin::Setup( gfl2::fs::AsyncFileManager* pAsyncFileMan )
{
  ::app::util::FileAccessor::RequestData* pfrs = &m_frs;

  switch(m_FileMode){
  case READ_NONE_:
    fileReadStructSetup(&m_frs, ARCID_BATTLE_UPPER_LYT, GARC_battleUpperLyt_minupp_applyt_COMP, m_pAppHeap, &m_pArcReadBuff);
    m_FileMode = OPEN_ING_;
    pAsyncFileMan->AddArcFileOpenReq(pfrs->openr);
    break;
  case OPEN_ING_:
    if(  pAsyncFileMan->IsArcFileOpenFinished( pfrs->openr.arcId ))
    {
      pAsyncFileMan->AddArcFileLoadDataReq(pfrs->loadr);
      m_FileMode = READ_ING_;
    }
    break;
  case READ_ING_:
    if(pAsyncFileMan->IsArcFileLoadDataFinished( pfrs->loadr.ppBuf ) )
    {
      pAsyncFileMan->AddArcFileCloseReq(pfrs->closer);
      m_FileMode = CLOSE_ING_;
    }
    break;
  case CLOSE_ING_:
    if(pAsyncFileMan->IsArcFileCloseFinished( pfrs->openr.arcId ) )
    {
      m_FileMode = END_ING_;
      return true;
    }
    break;
  case END_ING_:
    return true;
  }
  return false;
}



//初期化
void BtlvUiMsgWin::Create( void )
{
 
  // アーカイブIDテーブル
  ::app::util::G2DUtil::LytResData resTbl[] = {
    {  m_pArcReadBuff, 32,  ::app::util::G2DUtil::ATTACH_ROOT, },
  };

  ::app::util::G2DUtil::LytwkData setupData[] = {
    {//上
      0,
      LYTRES_MINUPP_BATTLE_MIN_UPP_000_BFLYT,  //      LYTRES_MINUPP_COMMON_MSG_UPP_000_BFLYT,
      LA_MINUPP_BATTLE_MIN_UPP_000___NUM,
      m_pArcReadBuff,
      true,
      ::app::util::G2DUtil::SETUP_UPPER,
      true
      },
  };


  Create2D( m_pAppHeap, NULL, 32,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            UNUSE_MESSAGE_DATA );

  ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();


  gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( MAIN_LAYOUT_ );
  gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( MAIN_LAYOUT_ );
  gfl2::lyt::LytParts*  pMsgWndLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_MSG );

  gfl2::lyt::LytTextBox * text1 = pLayoutWork->GetTextBoxPane( pMsgWndLytPartPane, messageWindowPane[0], &rMultiResId );
  gfl2::lyt::LytTextBox * text2 = pLayoutWork->GetTextBoxPane( pMsgWndLytPartPane, messageWindowPane[1], &rMultiResId );

  
  pG2DUtil->CreateMessageWindow( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),MESSAGE_WINDOW_ ,text1,text2, true, m_pAppHeap  );
  pG2DUtil->GetMsgWin()->SetMessageCallback(this);

  CreateMsgCursor();
}

//解放
void BtlvUiMsgWin::Delete(void)
{
  this->DeleteMsgCursor();
  // GFL_DELETE mStrWin;
  // mLytWin.DeleteLayout(mBtlvCore);

  // GFL_DELETE m_g2dUtil;

  ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  if(pG2DUtil){
    pG2DUtil->DeleteMessageWindow();

    Delete2D();
    DeleteGraphicsSystem();
  }

}



//更新
void BtlvUiMsgWin::Update(void)
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  if(pG2dUtil==NULL)
  {
    return;
  }
  print::MessageWindow* pMsgWin = pG2dUtil->GetMsgWin();


  
  if( mIsUpdateMessage )
  {
    const gfl2::str::StrWin::Result ret = pG2dUtil->PrintMessage();
    {
      // フィニッシュ待ち
      if( ret == gfl2::str::StrWin::RES_FINISH )
      {
        // 手動の場合はSEは自分で鳴らさないとならない
        if( mIsKeyWait == true )
        {
          Sound::PlaySE( SEQ_SE_MESSAGE );
        }

        // メッセージ表示終了
        mIsUpdateMessage = false;
        mCallBackArg = BTL_CLIENT::PRINTCB_JUST_DONE;
      }
      // 行送り待ち
      else if( ret == gfl2::str::StrWin::RES_PAUSE )
      {
        // 行送りの場合は自動送り
        // かつ 自動送り設定（通信・再生）の場合は、クリア送りも自動送り
        gfl2::str::StrWin::PauseType pauseType = pMsgWin->GetPauseType();
        if( pauseType == gfl2::str::StrWin::PAUSETYPE_FEED ||
            ( IsMustAutoKey() && pauseType == gfl2::str::StrWin::PAUSETYPE_CLEAR )
            )
        {
          // 一定時間ウェイト
          if( ++mAutoSkipWait > AUTO_SKIP_WAIT_PAUSE )
          {
            mAutoSkipWait = 0; // 初期化
            pMsgWin->SetMessageAutoOnce();
            ICHI_PRINT("行送り待ち\n");
          }
        }
        mCallBackArg = BTL_CLIENT::PRINTCB_RUN;
      }
      else
      {
        mCallBackArg = BTL_CLIENT::PRINTCB_RUN;
      }
    }

    // メッセージカーソル表示・非表示

    {
      if( CheckLoadMsgCursor() == false )
      {
        if( ret == gfl2::str::StrWin::RES_PAUSE &&
            pMsgWin->GetPauseType() == gfl2::str::StrWin::PAUSETYPE_CLEAR )
        {
          pG2dUtil->SetMsgCursorUserVisible( true ,true );
        }else{
          pG2dUtil->SetMsgCursorUserVisible( true ,false );
        }

        // 手動でキー待ちをする場合は自分で送りカーソルをだす
        if( mIsKeyWait == true && ret == gfl2::str::StrWin::RES_DONE )
        {
          pG2dUtil->SetMsgCursorUserVisible( true ,true );
        }
        // 通信・再生の時は必ず非表示
        if( IsMustAutoKey() ){
          pG2dUtil->SetMsgCursorUserVisible( true ,false );
        }
      }
    }
  }
  else
  {
    mCallBackArg = BTL_CLIENT::PRINTCB_AFTER_DONE;
  }

  // メッセージが自動で消える
  if( mIsUpdateMessage == false && mAutoHideWait > 0 )
  {
    // 一定時間ウェイト
    if( --mAutoHideWait == 0 )
    {
      this->HideMessage();
      ICHI_PRINT("自動メッセージ消え\n");
    }
  }

  ::app::ui::UIView::Update();
  //非表示の時にUpdateが回らないのでアニメが進まないのでバトルが始まらない！
  if( IsVisible() == false )
  {
    pG2dUtil->Update2D();
  }
  
  // メッセージカーソル更新
  UpdateMsgCursor();
}

// 描画
void BtlvUiMsgWin::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  DrawMsgCursor();

  ::app::util::G2DUtil* pG2DUtil  = ::app::ui::UIView::GetG2DUtil();
  //生成済みかのチェック
  if( pG2DUtil )
  {
    pG2DUtil->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
  }
}

//Layout取得
/*
BtlvLayout* BtlvUiMsgWin::GetLayout( const int type )
{
  GFL_UNUSED(type);
  return &mLytWin;
}
 */

static const int TEMP_STRBUF_LENGTH = 128;
void BtlvUiMsgWin::DispMessage( const gfl2::str::StrBuf& str , bool isOneLine, bool isKeyWait, const int autoHide, bool isFinishNone )
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  print::MessageWindow* pMsgWin = pG2dUtil->GetMsgWin();

#if PM_DEBUG
  if( m_pBtlvCore->GetDebugParam() && m_pBtlvCore->GetDebugParam()->mIsAutoTestMode )
  {
    //メッセージをログに出す
#if defined(GF_PLATFORM_CTR)
    char sjisbuf[TEMP_STRBUF_LENGTH*9];
    Test::Ariizumi::DebugStrcodeToUtf8(str.GetPtr(),sjisbuf);
    GFL_PRINT("■Msg「%s」\n",sjisbuf);
#elif defined(GF_PLATFORM_WIN32)
    char sjisBuf[TEMP_STRBUF_LENGTH*3];
    const char* defStr = " ";
    ::WideCharToMultiByte(932,WC_NO_BEST_FIT_CHARS,str.GetPtr(),-1,sjisBuf,TEMP_STRBUF_LENGTH*3,defStr,NULL);
    GFL_PRINT("■Msg「%s」\n",sjisBuf);
#endif  //プラットフォーム分岐
  }
#endif

  //UIView::SetVisible( true );
  gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( MAIN_LAYOUT_ );
  gfl2::lyt::LytParts*  pMsgWndLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_MSG );
  pG2dUtil->SetPaneVisible(pMsgWndLytPartPane,false);

  //  gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( MAIN_LAYOUT_ );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2dUtil->GetLayoutResourceID( MAIN_LAYOUT_ );
  //  gfl2::lyt::LytParts*  pMsgWndLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_MSG );

  gfl2::lyt::LytPane * text3 = pLayoutWork->GetPane( pMsgWndLytPartPane, PANENAME_BATTLE_PLT_UPP_004_PANE_NULL_TEXT1, &rMultiResId );
  gfl2::lyt::LytPane * text4 = pLayoutWork->GetPane( pMsgWndLytPartPane, PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_00, &rMultiResId );  //まとめて表示用のtextpane
  
  /*
  if( isOneLine )
  {
    pG2dUtil->StartAnime(0, 1 );
    mIsOneLine = true;
  }
  else
  {
    pG2dUtil->StartAnime(0, 0 );
    mIsOneLine = false;
  }
*/

  // メッセージが２行のときは１行に連結したものがウィンドウに納まるなら連結
  bool inRange = false;
  gfl2::str::StrBuf connetct_str(TEMP_STRBUF_LENGTH, m_pAppHeap->GetDeviceHeap() ); 

  if (CheckLineConnectable( str ))
  {
    // 連結メッセージを作成
    /*
      @fix NMCat[4015]: 連結後の文字数がテキストボックスペインの文字数を超えた場合は連結しないようにした

      @note:
        連結しなかった場合、タグは残ったままになるので、その分のバッファは消費する
    */
    u32 len = ConnectLine( str, connetct_str );
    gfl2::lyt::LytTextBox * text1 = pLayoutWork->GetTextBoxPane( pMsgWndLytPartPane, messageWindowPane[0], &rMultiResId );
    if( len <= text1->GetStringBufferLength() )
    {
      // 納まるかどうかチェック
      inRange = pMsgWin->CheckStringWidth( (const gfl2::str::StrBuf&)connetct_str );
    }
  }

  pMsgWin->Clear();

  if ( inRange )
  {
    pMsgWin->SetString( (const gfl2::str::StrBuf&)connetct_str );
    KIJIMA_PRINT( ">>>>> Message Connected.\n" );
  }
  else
  {
    pMsgWin->SetString( str );
  } 
 
  //別の速さをセットすることがあるので、表示時にリセット。
  pMsgWin->SetMessageSpeed( mMsgSpeed );
  bool bAutoKeyMode = SetMsgWinBehavior( isKeyWait, isFinishNone );  // メッセージウィンドウ挙動設定
  mIsVisibleMsgCursor = !bAutoKeyMode ;

  mAutoHideWait = autoHide;
  ICHI_PRINT("BtlvUiMsgWin::DispMessage 自動で消える設定 %d \n", autoHide );

  mIsUpdateMessage = true;
  mIsVisible = true;
  mAutoSkipWait = 0;

  m_pBtlvCore->GetUiSys()->SetDispGauge_MsgWin(false);

  pG2dUtil->SetPaneVisible(pMsgWndLytPartPane,true);
  
  pG2dUtil->SetPaneVisible(text3,true);
  pG2dUtil->SetPaneVisible(text4,false);
  



#if KANTAIHANTAI_TEST
//  TestChainaMessage();
#endif
  
}

// 指定のタグが含まれているかのチェック
bool BtlvUiMsgWin::IsExistTagCode( const STRCODE *str_ptr, STRCODE tag_code )
{
  // タグ開始コードでなければタグは無い
  if( *str_ptr != gfl2::str::TAG_START_CODE )
    {
      return false;
    }

  // ２ワード目は続くコード数
  u16 code_num = (u16)(str_ptr[1]);

  for(u16 i=0; i < code_num; i++ )
    {
      if(str_ptr[i+2] == tag_code)
        {
          return true;
        }
    }

  return false;

}

// メッセージが連結対象かどうかのチェック
// 戻り値がfalseの場合は連結対象外
bool BtlvUiMsgWin::CheckLineConnectable( const gfl2::str::StrBuf& str )
{
  int line_count = 0;
  u32 wchar_num = str.GetLength ();

  // 特殊コード
  const STRCODE TAGCODE_LINEFEED  = (print::TAGGROUP_STREAM_CTRL << 8) + print::TAGIDX_STREAM_LINE_FEED;
  const STRCODE TAGCODE_PAGECLEAR = (print::TAGGROUP_STREAM_CTRL << 8) + print::TAGIDX_STREAM_PAGE_CLEAR;

  /// 汎用コントロール処理:バトル用２行を１行にまとめる
  const STRCODE TAGCODE_BATTLE_ONELINE = (print::TAGGROUP_GENERAL_CTRL << 8) + print::TAGIDX_GENERAL_BATTLE_ONELINE;


  if (0 == wchar_num)
    {
      return false;
    }

  line_count ++;

  const STRCODE *str_ptr = str.GetPtr();

  if( !(IsExistTagCode (str_ptr, TAGCODE_BATTLE_ONELINE)) )
    {
      return false;
    }

  for (u32 i =0; i<wchar_num ;i++)
    {

      // 1行目に
      // 該当の特殊コードが含まれていた場合は対象外
      if( line_count == 1 )
        {
          if( str_ptr[i] == TAGCODE_LINEFEED )
            {
              return false;
            }
          
          if( str_ptr[i] == TAGCODE_PAGECLEAR )
            {
              return false;
            }
        }
 
     // 改行コード
     if( str_ptr[i] == gfl2::str::CR_CODE )
        {
          line_count ++;
        }
    }

  // ２行のメッセージのみ対象
  if (line_count != 2)
    {
      return false;
    }

  
  return true;

}

// １行目と２行目を連結したメッセージを作成する
// （実際には１行目の改行をスペースに置き換える）
// @fix NMCat[4015]: EOMを含まない文字数を返すようにした
u32 BtlvUiMsgWin::ConnectLine ( const gfl2::str::StrBuf &src_str, gfl2::str::StrBuf &dest_str )
{
  const STRCODE ZEN_SP_CODE = 0x3000;  // 全角スペース
  const STRCODE HAN_SP_CODE = 0x0020;  // 半角スペース

  STRCODE SP_CODE = ZEN_SP_CODE;

  // メッセージ言語によるスペースの文字コード切り替え
  System::MSGLANGID lang_ID = print::GetMessageLangId();
  switch (lang_ID)
  {
//  case System::MSGLANGID_KANA:
//  case System::MSGLANGID_KANJI:
  case System::MSGLANGID_ENGLISH:
  case System::MSGLANGID_FRENCH:
  case System::MSGLANGID_ITALIAN:
  case System::MSGLANGID_GERMAN:
  case System::MSGLANGID_SPANISH:
//  case System::MSGLANGID_KOREAN:
//  case System::MSGLANGID_CHINA:   //簡体字
//  case System::MSGLANGID_TAIWAN:  //繁体字
    SP_CODE = HAN_SP_CODE;
    break;

  default:
    break;
  }

  STRCODE str_buf[TEMP_STRBUF_LENGTH];
  const STRCODE *str_ptr = src_str.GetPtr();
  u32 len = src_str.GetLength();

  if( len >= TEMP_STRBUF_LENGTH )
  {
    GFL_ASSERT( 0 );
    dest_str.SetStr( src_str );
    return len;
  }

  int replace_count = 1;
  for( u32 i=0; i<len; i++ )
  {
    if( str_ptr[i] == gfl2::str::CR_CODE && ( replace_count > 0 ) )
    {
      str_buf[i] = SP_CODE;
      replace_count--;
    }
    else
    {
      str_buf[i] = str_ptr[i];
    }
  }
  str_buf[len] = gfl2::str::EOM_CODE;   // 終端文字

  dest_str.SetStr( gfl2::str::SkipTagCode(str_buf) );   // @fix NMCat[4015]: 先頭の連結タグをスキップ

  return dest_str.GetLength();
}




#if KANTAIHANTAI_TEST  //中国語のテスト

void BtlvUiMsgWin::TestChainaMessage(void)
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  static int monsno=0;

  monsno++;

  {
    gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    print::SystemFont_CreateBattleFont(devHeap,devHeap);
    print::SystemFont_SetLytSystemBattleFont();
  }



  gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( MAIN_LAYOUT_ );
  gfl2::lyt::LytMultiResID& rMultiResId = pG2dUtil->GetLayoutResourceID( MAIN_LAYOUT_ );
  gfl2::lyt::LytParts*  pLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_LB );
  gfl2::lyt::LytTextBox * btlnamepane = pLayoutWork->GetTextBoxPane( pLytPartPane, PANENAME_BATTLE_PLT_UPP_000_PANE_TEXTBOX_00, &rMultiResId );

  gfl2::str::MsgData* pMsgData = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::MsgData( ARCID_MESSAGE_CHN,
                                                                                            GARC_message_Simp_Chinese_monsname_DAT,
                                                                                            m_pAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  gfl2::str::StrBuf* strbuf = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 200, m_pAppHeap->GetDeviceHeap() );

  pMsgData->GetString(monsno, *strbuf);


   
 // GFL_PRINT("%x %x %x \n",strbuf->getCode(0),strbuf->getCode(1),strbuf->getCode(2));

  
  print::SetStringForLytTextBox( btlnamepane, strbuf );
//    print::SetStringForLytTextBox( text2, strbuf );


  pLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_RT );
  btlnamepane = pLayoutWork->GetTextBoxPane( pLytPartPane, PANENAME_BATTLE_PLT_UPP_000_PANE_TEXTBOX_00, &rMultiResId );

  print::SetStringForLytTextBox( btlnamepane, strbuf );

  GFL_DELETE strbuf;
  GFL_DELETE pMsgData;
  
}
#endif



void BtlvUiMsgWin::HideMessage(void)
{
  //SetVisible( false );
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( MAIN_LAYOUT_ );
  gfl2::lyt::LytParts*  pMsgWndLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_MSG );
  pG2dUtil->SetPaneVisible(pMsgWndLytPartPane,false);

  mIsVisible = false;
  m_pBtlvCore->GetUiSys()->SetDispGauge_MsgWin(true);
}

bool BtlvUiMsgWin::IsUpdateMessage()
{
  if( mCallBackTarget )
  {
    if( mCallBackTarget->PrintCallback( mCallBackArg ) )
    {
      mCallBackTarget = NULL;
      return mIsUpdateMessage;
    }
    else
    {
      return true;
    }
  }
  else
  {
    return mIsUpdateMessage;
  }
}

//メッセージ処理速度を設定
void BtlvUiMsgWin::SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED msgSpeed )
{
  mMsgSpeed = msgSpeed;
}

//今表示しているメッセージだけに有効なメッセージ速度変更
//トレーナーの繰り出しメッセージの時に、遅いの時だけ普通にする。
void BtlvUiMsgWin::SetMessageSpeedForce_Normal(void)
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  print::MessageWindow* pMsgWin = pG2dUtil->GetMsgWin();


  if( mMsgSpeed == Savedata::ConfigSave::MSG_SLOW )
  {
    pMsgWin->SetMessageSpeed( Savedata::ConfigSave::MSG_NORMAL );
  }
}

void BtlvUiMsgWin::SetAutoStepMessage(bool flg )
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();
  print::MessageWindow* pMsgWin = pG2dUtil->GetMsgWin();
  if( IsMustAutoKey() )
  {
    pMsgWin->SetMessageAutoFlg( true );
    ICHI_PRINT("メッセージ自動送り設定！ 通信版\n");
  }
  else
  {
    pMsgWin->SetMessageAutoFlg( flg );
    ICHI_PRINT("メッセージ自動送り設定！ %d \n", flg);
  }
}

//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソルの表示・非表示
 *
 *  @param		isVisible		true = 表示, false = 非表示
 */
//--------------------------------------------------------------------------
void BtlvUiMsgWin::SetVisibleMsgCursor( bool isVisible )
{
  mIsVisibleMsgCursor = isVisible;
}

//==========================================================================
/**
 *					メッセージカーソル系
 */
//==========================================================================
//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソル作成
 */
//--------------------------------------------------------------------------
void BtlvUiMsgWin::CreateMsgCursor( void )
{
  /*
  ::app::tool::Cursor::CREATE_DATA dat;

  dat.graSys  = mBtlvCore->GetGraphicSys(); // グラフィックシステム
  dat.lytSys  = mBtlvCore->GetLayoutSys();  // レイアウトシステム

  dat.heap    = mBtlvCore->GetHeapMem();    // ヒープ
  dat.devHeap = mBtlvCore->GetDeviceMem();  // デバイスヒープ
  dat.devAllocator = mBtlvCore->GetNwAllocaterDevice(); // デバイスアロケータ

  dat.isAutoDraw  = false;
  dat.frameRate   = gamesystem::FRAMEMODE_30;
  dat.initVisible = false;

  dat.setup       = LytWkDefaultSetup;

  // 位置
  // 通常の場合 2013.10.22 コンテストが素材を差し替えてこのクラスを使うので修正 saita_kazuki
  if( mBtlvCore->GetBtlvMode() != BTLV_MODE_CONTEST_APPEAL)
  {
    gfl2::math::VEC3 pos = GetPanePos( PANENAME_MSG_WIN_PANE_MSGWINTEXT2 );

    gfl2::grp::g2d::LytPane * pane = mLytWin.GetLytWk()->GetPane( PANENAME_MSG_WIN_PANE_MSGWINTEXT2 );
    nw::lyt::Size	size = pane->GetSize();
    pos.x = pos.x + ( size.width + ::app::tool::MsgCursor::CURSOR_SIZE_X ) / 2;
    dat.pos = pos;

    pos = GetPanePos( PANENAME_MSG_WIN_PANE_MSGWINTEXT2 );
    dat.pos.y = pos.y - ( size.height - ::app::tool::MsgCursor::CURSOR_SIZE_Y ) / 2;
  }
  // コンテスト用の位置 (素材が違うだけ)
  else
  {
    gfl2::math::VEC3 pos = GetPanePos( PANENAME_CONTEST_BATTLE_UPPER_PANE_MSGWINTEXT2 );

    gfl2::grp::g2d::LytPane * pane = mLytWin.GetLytWk()->GetPane( PANENAME_CONTEST_BATTLE_UPPER_PANE_MSGWINTEXT2 );
    nw::lyt::Size	size = pane->GetSize();
    pos.x = pos.x + ( size.width + ::app::tool::MsgCursor::CURSOR_SIZE_X ) / 2;
    dat.pos = pos;

    pos = GetPanePos( PANENAME_CONTEST_BATTLE_UPPER_PANE_MSGWINTEXT2 );
    dat.pos.y = pos.y - ( size.height - ::app::tool::MsgCursor::CURSOR_SIZE_Y ) / 2;
  }

  mMsgCursor = GFL_NEW( mBtlvCore->GetHeapMem() ) ::app::tool::MsgCursor( &dat );
   */


 // GetG2DUtil()->StartAnime(0, tbl[window_type]);

  GetG2DUtil()->CreateMsgCursor(m_pAppHeap,  0, PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_MSG, PANENAME_BATTLE_PLT_UPP_004_PANE_CURSOR_00  );
  GetG2DUtil()->StartAnime(0, LA_MINUPP_BATTLE_MIN_UPP_000__PLT_MSG__CURSOR_00_KEEP);

  mIsVisibleMsgCursor = true;
  HideMessage();
}
//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソル削除
 *
 *  @retval	"true = 削除完了"
 *  @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------
bool BtlvUiMsgWin::DeleteMsgCursor( void )
{
  /*
  if( mMsgCursor != NULL ){
    if( mMsgCursor->End() == true ){
      // メッセージカーソルの読み込み途中破棄はないはず。
      GFL_ASSERT(0); // @check
      return false;
    }
    GFL_DELETE mMsgCursor;
    mMsgCursor = NULL;
  }
   */
  if( GetG2DUtil() )
  {
    return GetG2DUtil()->DeleteMsgCursor();
  }
  return true;
}
//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソルアップデート更新
 */
//--------------------------------------------------------------------------
void BtlvUiMsgWin::UpdateMsgCursor( void )
{
  /*
  if( mMsgCursor != NULL ){
    CheckLoadMsgCursor();
    mMsgCursor->Update();
  }
   */
}

//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソルの描画
 */
//--------------------------------------------------------------------------
void BtlvUiMsgWin::DrawMsgCursor( void )
{
  //  if( mMsgCursor != NULL ){
  //    if( mIsVisibleMsgCursor == true )
  //    {
  //      mMsgCursor->Draw( gfl2::lyt::DISPLAY_UPPER );
  //    }
  //  }
}

//--------------------------------------------------------------------------
/**
 *  @brief  メッセージカーソル読み込みチェック
 *
 *  @retval	"true = 読み込み中"
 *  @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------
bool BtlvUiMsgWin::CheckLoadMsgCursor( void )
{
  return false;
  // return mMsgCursor->Start();
}

//--------------------------------------------------------------------------
/**
 *  @brief  ペインの座標取得
 *
 *  @param  index ペインのインデックス
 *
 *  @return ペインの座標
 */
//--------------------------------------------------------------------------
gfl2::math::Vector3 BtlvUiMsgWin::GetPanePos( gfl2::lyt::LytPaneIndex index )
{
  ::app::util::G2DUtil* pG2dUtil = GetG2DUtil();


  gfl2::lyt::LytPane *basePane = pG2dUtil->GetLayoutWork(0)->GetPane( index );
  gfl2::lyt::LytPane *rootPane = pG2dUtil->GetLayoutWork(0)->GetLayoutCore()->GetRootPane();
  gfl2::math::VEC3 pos = basePane->GetTranslate();

  for ever{
    gfl2::lyt::LytPane * parentPane = basePane->GetParent();
    // 親がいない or 親がルートペイン
    if( parentPane == NULL || parentPane == rootPane ){
      break;
    }
    pos += parentPane->GetTranslate();
    basePane = parentPane;
  }
  gfl2::math::Vector3 ve3 = gfl2::math::ConvertNwVec3ToGfVec3(pos);

  return ve3;
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージウィンドウの挙動を設定する
 * @param   isKeyWait   キー待ちをこのクラスで手動で行う
 *
 * @retval  true:自動キー送りさせる
 * @retval  false:自動キー送りさせない
 */
//--------------------------------------------------------------------------
bool BtlvUiMsgWin::SetMsgWinBehavior( bool isKeyWait, bool isFinishNone )
{
  //	app::util::G2DUtil*
  print::MessageWindow* pMsgWin = GetG2DUtil()->GetMsgWin();

  if(pMsgWin==NULL){
    return false;
  }

  mIsKeyWait = isKeyWait;
#if PM_DEBUG
  if( m_pBtlvCore->GetDebugParam() && m_pBtlvCore->GetDebugParam()->mIsAutoMsg )
  {
    //デバッグ自動送り
    // 通信・再生の場合は、メッセージ自動送りかつカーソル非表示
    pMsgWin->SetMessageAutoFlg( true );
    pMsgWin->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, 0 );
    return true;
  }
  else
#endif
  if( IsMustAutoKey() )
  {
    GFL_PRINT("メッセージ送り設定 自動キー送り\n");
    // 通信・再生の場合は、メッセージ自動送りかつカーソル非表示
    pMsgWin->SetMessageAutoFlg( false );
    pMsgWin->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, 24 );
    return true;
  }
  else if( isKeyWait )
  {
    GFL_PRINT("メッセージ送り設定 手動キー待ち\n");
    pMsgWin->SetMessageAutoFlg( false );
    pMsgWin->SetFinishMode( gfl2::str::StrWin::FINISH_USER, 24 );
    return false;
  }
  else if( isFinishNone )
  {
    GFL_PRINT("メッセージ送り設定 終了待ちNone設定 \n");
    pMsgWin->SetMessageAutoFlg( false );
    pMsgWin->SetFinishMode( gfl2::str::StrWin::FINISH_TIMER, 0 );
    return false;
  }
  else
  {
    GFL_PRINT("メッセージ送り設定 キー送りなし\n");
    pMsgWin->SetMessageAutoFlg( false );
    pMsgWin->SetFinishMode( gfl2::str::StrWin::FINISH_HYB, 24 );
    return false;
  }
}

//--------------------------------------------------------------------------
/**
 *  @brief  入力チェック
 */
//--------------------------------------------------------------------------
bool BtlvUiMsgWin::CheckInput( void )
{
  const gfl2::ui::Button *uiButton = m_pBtlvCore->GetButton();
  if( uiButton->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) )
  {
    return true;
  }

  const gfl2::ui::TouchPanel *tp = m_pBtlvCore->GetTP();
  if( tp->IsTouchTrigger() ){
    return true;
  }

  return false;
}

//print::MsgCallback継承
/*
 *  タグのコールバックが呼ばれると呼ばれます。
 *  戻り値がtureの限りメッセージの更新を止めます。
 *
 *  @param[in]   arg    メッセージで指定された引数が入ります。
 *
 *  @retval   bool      trueの間メッセージの更新は止まります。
 */
bool BtlvUiMsgWin::UpdateMsgCallback( u16 arg )
{
  switch( arg ){
  case 3:
    Sound::PlaySE( SEQ_SE_BT_WAZAWASURE );
    break;

  case 5:
    if( Sound::IsSEPlaying( SEQ_SE_BT_WAZAWASURE ) == false )
    {
      return true;
    }
    break;
  }
  return false;

}

::app::ui::UIInputListener::ListenerResult BtlvUiMsgWin::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  return ::app::ui::UIInputListener::ENABLE_ACCESS;
}



GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)

