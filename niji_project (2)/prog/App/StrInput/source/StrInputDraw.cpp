//==============================================================================
/**
 * @file    StrInputDraw.cpp
 * @brief   Frame関連
 * @author  fukushima_yuya
 * @date    2015.04.30
 * @note    sango_project app strinput_main.cppから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputDraw.h"
#include "App/StrInput/source/StrInputSndDef.h"
#include "App/StrInput/source/StrInputUi.h"

#include <AppLib/include/Util/app_util_Common.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <GameSys/include/GameData.h>
#include <Print/include/hangul.h>
#include <Print/include/SystemFont.h>
#include <System/include/GameStrLen.h>
#include <System/include/gflUse.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>
#include <ngc/include/gfl2_NGWordChecker.h>
#include <str/include/gfl2_Str.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/strinput.gaix>
#include <arc_index/StrInputGrpFont.gaix>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
* @namespace  App::StrInput
 * @brief     文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//==============================================================================
//  定数定義
//==============================================================================
//! @brief  メインシーケンス定義
enum {
  MAINSEQ_INIT,                   //!< 初期化
  MAINSEQ_RELEASE,                //!< 解放
  MAINSEQ_FADE,                   //!< フェード待ち
  MAINSEQ_START,                  //!< 開始待ち処理
  MAINSEQ_MAIN,                   //!< メイン処理
  MAINSEQ_WAIT_LOAD_PLAYERICON,   //!< プレイヤーアイコン読み込み待ち
  MAINSEQ_NG_WORD_CHECK,          //!< NGワードチェック
  MAINSEQ_TEXTURE_LOAD,           //!< テクスチャの読み込み
  MAINSEQ_LAYOUT_LOAD,            //!< レイアウトの読み込み
  MAINSEQ_FILE_LOAD,              //!< ファイルの読み込み

  MAINSEQ_END                     //!< 終了定義
};

typedef int (MainSystem::*pSeqFunc)( void );  //!< メインシーケンス型


//------------------------------------------------------------------------------
/**
 * @brief   入力モードデータ
 */
//------------------------------------------------------------------------------
typedef struct {
  u16 siz;    //!< 入力サイズ
  u16 type;   //!< 初期入力タイプ
}STR_MODE_DATA;

static const u32 STRINPUT_APP_HEAP_SIZE     = 768 * 1024;    //!< アプリ用ローカルヒープサイズ(通常)
//static const u32 STRINPUT_DEVICE_HEAP_SIZE  = 0x50000;    //!< アプリ用ローカルデバイスヒープサイズ(通常)
static const u32 STRINPUT_DEVICE_HEAP_SIZE  = 4 * 1024 * 1024;

//static const u32 STRINPUT_FADE_SUNC = app::util::DEFAULT_FADE_SYNC;     //!< フェード速度
static const u32 STRINPUT_FADE_SUNC = gfl2::Fade::FADE_DEFAULT_SYNC;    //!< フェード速度

static const gfl2::str::STRCODE STRCODE_DAKUTEN    = L'゛';    //!< 濁点のSTRCODE
static const gfl2::str::STRCODE STRCODE_HANDAKUTEN = L'゜';    //!< 半濁点のSTRCODE
static const gfl2::str::STRCODE STRCODE_SPACE      = L'　';    //!< 全角スペースのSTRCODE
static const gfl2::str::STRCODE STRCODE_SPACE_HALF = L' ';     //!< 半角スペースのSTRCODE

static const u32 INPUT_CODE_MAX_AIUEO    = 65;    //!< 入力コード数(50音モード)
static const u32 INPUT_CODE_MAX_KEYBOARD = 45;    //!< 入力コード数(キーボードモード)

static const u32 INPUTLEN_POKEMON_NAME    = System::STRLEN_POKEMON_NAME;                      //!< 入力サイズ：ポケモン名
static const u32 INPUTLEN_PLAYER_NAME     = System::STRLEN_PLAYER_NAME;                       //!< 入力サイズ：人物名
static const u32 INPUTLEN_BOX_NAME        = 16 / 2;//savedata::BOX::TRAY_NAME_LEN / 2;                 //!< 入力サイズ：ボックス名
static const u32 INPUTLEN_BOX_NAME_E      = 14;                                               //!< 入力サイズ：ボックス名(日本、韓国以外)
static const u32 INPUTLEN_FREE_WORD_PSS   = System::STRLEN_PSS_MESSAGE;                       //!< 入力サイズ：一言メッセージ(PSS)
static const u32 INPUTLEN_FREE_WORD_GTS   = System::STRLEN_PSS_MESSAGE;                       //!< 入力サイズ：一言メッセージ(GTS)
static const u32 INPUTLEN_HAPPY_SIGN      = System::STRLEN_PSS_MESSAGE;                       //!< 入力サイズ：ハッピーサイン
static const u32 INPUTLEN_INTERVIEW_LONG  = System::STRLEN_INTERVIEW_WORD_LONG;               //!< 入力サイズ：一言メッセージ(インタビュー：16文字)
static const u32 INPUTLEN_INTERVIEW_SHORT = System::STRLEN_INTERVIEW_WORD_SHORT;              //!< 入力サイズ：一言メッセージ(インタビュー：8文字)
static const u32 INPUTLEN_INTERVIEW_VERY_SHORT = System::STRLEN_INTERVIEW_WORD_VERY_SHORT;    //!< 入力サイズ：一言メッセージ(インタビュー：6文字)
static const u32 INPUTLEN_BASE_WORD       = 10;                                               //!< 入力サイズ：秘密基地・国名と合言葉
static const u32 INPUTLEN_BASE_WORD_E     = 16;                                               //!< 入力サイズ：秘密基地・国名と合言葉(日本と韓国以外)
static const u32 INPUTLEN_BASE_HAPPY      = System::STRLEN_SECRET_BASE_MESSAGE;               //!< 入力サイズ：秘密基地・嬉しいときの言葉
static const u32 INPUTLEN_BASE_CHEER      = System::STRLEN_SECRET_BASE_MESSAGE;               //!< 入力サイズ：秘密基地・励ますときの言葉
static const u32 INPUTLEN_BASE_GREETING   = System::STRLEN_SECRET_BASE_MESSAGE;               //!< 入力サイズ：秘密基地・挨拶
static const u32 INPUTLEN_BASE_GOODS      = System::STRLEN_SECRET_BASE_MESSAGE;               //!< 入力サイズ：秘密基地・グッズ
static const u32 INPUTLEN_TEAM_NAME       = 10;                                               //!< 入力サイズ：チーム名
static const u32 INPUTLEN_FESCIRCLE_NAME  = 14;                                               //!< 入力サイズ：フェスサークル名


//! @brief  メインシーケンス解放テーブル
static const pSeqFunc MainSeq[] = {
  &MainSystem::MainSeq_Init,
  &MainSystem::MainSeq_Release,
  &MainSystem::MainSeq_Fade,
  &MainSystem::MainSeq_Start,
  &MainSystem::MainSeq_Main,
  &MainSystem::MainSeq_WaitLoadPlayerIcon,
  &MainSystem::MainSeq_NgWordCheck,
  &MainSystem::MainSeq_TextureLoad,
  &MainSystem::MainSeq_LayoutLoad,
  &MainSystem::MainSeq_FileLoad,
};

//! @brief  入力モードデータテーブル
static const STR_MODE_DATA InitData[] = {
  { INPUTLEN_PLAYER_NAME,             INPUT_TYPE_HIRAGANA },    //!< 00：人物名
  { INPUTLEN_POKEMON_NAME,            INPUT_TYPE_KATAKANA },    //!< 01：ポケモン名
  { INPUTLEN_BOX_NAME,                INPUT_TYPE_HIRAGANA },    //!< 02：ボックス名
  { INPUTLEN_FREE_WORD_PSS,           INPUT_TYPE_HIRAGANA },    //!< 03：入力サイズ：一言メッセージ(PSS)
  { INPUTLEN_FREE_WORD_GTS,           INPUT_TYPE_HIRAGANA },    //!< 04：入力サイズ：一言メッセージ(GTS)
  { INPUTLEN_PLAYER_NAME,             INPUT_TYPE_HIRAGANA },    //!< 05：ニックネーム
  { INPUTLEN_POKEMON_NAME,            INPUT_TYPE_KATAKANA },    //!< 06：GTSポケモン検索
  { INPUTLEN_POKEMON_NAME,            INPUT_TYPE_KATAKANA },    //!< 07：GTSポケモン検索(欲しいポケモン)
  { INPUTLEN_HAPPY_SIGN,              INPUT_TYPE_HIRAGANA },    //!< 08：ハッピーサイン

  { INPUTLEN_INTERVIEW_LONG,          INPUT_TYPE_HIRAGANA },    //!< 09：一言メッセージ(インタビュー：16文字)
  { INPUTLEN_INTERVIEW_SHORT,         INPUT_TYPE_HIRAGANA },    //!< 10：一言メッセージ(インタビュー：8文字)
  { INPUTLEN_INTERVIEW_VERY_SHORT,    INPUT_TYPE_HIRAGANA },    //!< 11：一言メッセージ(インタビュー：6文字)

  { INPUTLEN_BASE_WORD,               INPUT_TYPE_HIRAGANA },    //!< 12：秘密基地：団名と合言葉
  { INPUTLEN_BASE_HAPPY,              INPUT_TYPE_HIRAGANA },    //!< 13：秘密基地：嬉しいときの言葉
  { INPUTLEN_BASE_CHEER,              INPUT_TYPE_HIRAGANA },    //!< 14：秘密基地：励ますときの言葉
  { INPUTLEN_BASE_GREETING,           INPUT_TYPE_HIRAGANA },    //!< 15：秘密基地：挨拶
  { INPUTLEN_BASE_GOODS,              INPUT_TYPE_HIRAGANA },    //!< 16：秘密基地：グッズ

  { INPUTLEN_TEAM_NAME,               INPUT_TYPE_HIRAGANA },    //!< 17：チーム名
  { INPUTLEN_FESCIRCLE_NAME,          INPUT_TYPE_HIRAGANA },    //!< 18：チーム名

#if PM_DEBUG
  { INPUTLEN_PLAYER_NAME,             INPUT_TYPE_HIRAGANA },    //!< ：人物名デバッグ
#endif
};



//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   prm   外部設定パラメータ
 */
//------------------------------------------------------------------------------
MainSystem::MainSystem( APP_PARAM * prm )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->prm = prm;
  //mwk->mainSeq = MAINSEQ_INIT;
  mwk->mainSeq = MAINSEQ_TEXTURE_LOAD;

  InitInputCode();
  // ヒープの生成
  CreateHeap();

  u8 lang = mwk->prm->language;
  if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_KOREA )
  {
    mwk->inputType = InitData[prm->type].type;  //!< 入力タイプ
    mwk->shift     = 0;                         //!< SHIFTキー
  }
  else if( lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
  {
    mwk->inputType = INPUT_TYPE_ABC;
    mwk->shift     = 0;                         //!< SHIFTキー
  }
  else {
    mwk->inputType = INPUT_TYPE_CAPITAL;
    mwk->shift     = 1;                         //!< SHIFTキー
  }

  mwk->inputPos = 0;
  mwk->inputSiz = 0;

  mwk->inputSizeMax = InitData[prm->type].siz;

  mwk->keybordMode = 0;

  m_DataIndex = 0;
  m_FileIndex = 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   ゲームマネージャ関連取得
 *
 * @param   man   GameProcManager
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetGameManager( GameSys::GameProcManager * man )
{
  MAIN_WORK * mwk = &m_MainWork;

  GameSys::GameManager * gm = man->GetGameManager();

  mwk->gameManager      = gm;
  mwk->pFileReadManager = gm->GetAsyncFileManager();
  mwk->pUiDevManager    = gm->GetUiDeviceManager();
  mwk->pFadeManager     = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  mwk->pNGWordChecker   = GFL_SINGLETON_INSTANCE( gfl2::ngc::NGWordChecker );

  GameSys::GameData * gd = mwk->gameManager->GetGameData();
  mwk->pMyStatus = gd->GetPlayerStatus();
}

//------------------------------------------------------------------------------
/**
 * @brief   アップデート処理
 *
 * @param   none
 *
 * @retval  "true = 処理中"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::Update( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->mainSeq = (this->*MainSeq[mwk->mainSeq])();

  if( mwk->mainSeq == MAINSEQ_END )
  {
    return false;
  }

  if( m_SubWork ) m_SubWork->pGraWork->Update();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   描画処理
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::Draw( gfl2::gfx::CtrDisplayNo no )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->mainSeq == MAINSEQ_END )
  {
    return;
  }

  if( mwk->mainSeq == MAINSEQ_RELEASE )
  {
    // 開放シーケンスでは描画はしない
    return;
  }

  if( (NULL != m_SubWork) && (NULL != m_SubWork->pGraWork) )
  {
    m_SubWork->pGraWork->Draw( no );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   サブワーク作成
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::CreateHeap( void )
{
  if( m_MainWork.prm->isFieldSubProc )
  {
    m_pNgcWorkHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_Heap.Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE),
                 gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE),
                 HEAPID_STRINPUT_APP, STRINPUT_APP_HEAP_SIZE, HEAPID_STRINPUT_DEVICE, STRINPUT_DEVICE_HEAP_SIZE );
  }
  else {
    gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    m_Heap.Create( heap, heap, HEAPID_STRINPUT_APP, STRINPUT_APP_HEAP_SIZE, HEAPID_STRINPUT_DEVICE, STRINPUT_DEVICE_HEAP_SIZE );
    m_pNgcWorkHeap = m_Heap.GetSystemHeap();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   サブワーク作成
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::CreateSubWork( void )
{
  // ヒープの生成はCreateHeapに分離
  //bool isInitApplicationMemory = !m_MainWork.prm->skipInitGraphicMemory;
  //
  //if( m_MainWork.prm->isFieldSubProc )
  //{
  //  m_pNgcWorkHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT );
  //  m_Heap.Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE),
  //               gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE),
  //               HEAPID_STRINPUT_APP, STRINPUT_APP_HEAP_SIZE, HEAPID_STRINPUT_DEVICE, STRINPUT_DEVICE_HEAP_SIZE );
  //  isInitApplicationMemory = false;
  //}
  //else {
  //  m_Heap.Create( HEAPID_STRINPUT_APP, STRINPUT_APP_HEAP_SIZE, HEAPID_STRINPUT_DEVICE, STRINPUT_DEVICE_HEAP_SIZE );
  //  m_pNgcWorkHeap = m_Heap.GetSystemHeap();
  //}
  bool isInitApplicationMemory = !m_MainWork.prm->skipInitGraphicMemory;
  bool isFieldSubProc = m_MainWork.prm->isFieldSubProc;

  if( isFieldSubProc ) isInitApplicationMemory = false;

  //gfl2::heap::HeapBase* pHeap = m_Heap.GetSystemHeap();   // system heap => device heap
  gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
  {
    m_SubWork = GFL_NEW(pHeap) SUB_WORK;

    m_SubWork->ui = GFL_NEW(pHeap) UI( m_MainWork.pUiDevManager, &m_Heap );

    m_SubWork->pGraWork = GFL_NEW(pHeap) GraphicWork(
                                            &m_Heap,
                                            m_MainWork.pFileReadManager,
                                            m_MainWork.prm->mpAppRenderingManager,
                                            m_pArcReadBuff,
                                            m_pTexReadBuff,
                                            isInitApplicationMemory,
                                            isFieldSubProc );

    // @fix NMCat[914]
    if( m_MainWork.prm->language == POKEMON_LANG_KOREA )
    {
      m_SubWork->pGraWork->SetTextureGraphicFont_KOR();
    }

    m_SubWork->keyMap = GFL_NEW(pHeap) KeyMap( &m_Heap );

    m_SubWork->errPutFlag = false;

    m_SubWork->pHangulComposer = GFL_NEW(pHeap) print::HangulComposer( print::SystemFont_GetFont() );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   サブワーク削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::DeleteSubWork( void )
{
  GFL_DELETE m_SubWork->pHangulComposer;

  GFL_DELETE m_SubWork->keyMap;
  
  GFL_DELETE m_SubWork->pGraWork;

  GFL_DELETE m_SubWork->ui;

  GFL_DELETE m_SubWork;

  GflHeapFreeMemory( m_pTexReadBuff );

  GflHeapFreeMemory( m_pArcReadBuff );

  m_Heap.Delete();
}


//------------------------------------------------------------------------------
/**
 * @brief   フェードインセット
 *
 * @param   next    フェード後のシーケンス
 *
 * @return  MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
int MainSystem::SetFadeIn( int next )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( !mwk->prm->isFieldSubProc )
  {
    gfl2::math::Vector4 col( 0.0f, 0.0f, 0.0f, 255.0f );
    mwk->pFadeManager->ForceOut( gfl2::Fade::DISP_DOUBLE, &col );
    mwk->pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, STRINPUT_FADE_SUNC );
    
    mwk->fadeNextSeq = next;
    return MAINSEQ_FADE;
  }

  return next;
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードアウトセット
 *
 * @param   next    フェード後のシーケンス
 *
 * @return  MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
int MainSystem::SetFadeOut( int next )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( !mwk->prm->isFieldSubProc )
  {
    gfl2::math::Vector4 startColor( 0.0f, 0.0f, 0.0f, 255.0f );
    gfl2::math::Vector4 endColor( 0.0f, 0.0f, 0.0f, 255.0f );
    mwk->pFadeManager->RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &startColor, &endColor, STRINPUT_FADE_SUNC, false );

    mwk->fadeNextSeq = next;
    return MAINSEQ_FADE;
  }

  return next;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理設定
 *
 * @param   none
 *
 * @return  MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
int MainSystem::SetEnd( void )
{
  SetEndParam();
  m_MainWork.nextSeq = MAINSEQ_END;
  return SetFadeOut( MAINSEQ_RELEASE );
}

//------------------------------------------------------------------------------
/**
 * @brief   NGワード検査の完了通知イベント
 * @param   result 検査結果
 */
//------------------------------------------------------------------------------
void MainSystem::OnNGWordCheckFinish( gfl2::ngc::Result result )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  switch( result )
  {
  case gfl2::ngc::E_SUCCESS:                    //!< 問題なし
    swk->ngWordResult = WORD_CHECK_TRUE;
    break;

  case gfl2::ngc::E_ERROR_NUMBER_COUNT:         //!< 数字文字数エラー
    if( mwk->prm->type == TYPE_BOX_NAME )
    {
      swk->ngWordResult = WORD_CHECK_TRUE;
      break;
    }
    swk->ngWordResult = WORD_CHECK_NUMBER_OVER;
    break;

  case gfl2::ngc::E_ERROR_FREEWORD_CHAR_COUNT:  //!< フリーワード文字数エラー
    GFL_ASSERT( 0 );    //!< 入力できないのでこないはず
    swk->ngWordResult = WORD_CHECK_NG;
    break;

  case gfl2::ngc::E_ERROR_WORD:                 //!< 単語エラー
    swk->ngWordResult = WORD_CHECK_NG;
    break;

  default:
    GFL_ASSERT( 0 );
    swk->ngWordResult = WORD_CHECK_NG;
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キーコード取得
 *
 * @param   mode  入力モード
 * @param   id    取得位置
 *
 * @return  キーコード
 */
//------------------------------------------------------------------------------
gfl2::str::STRCODE & MainSystem::GetKeyCode( u32 mode, u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;
  u32 type = 0;

  if( mode == INPUT_MODE_AIUEO )
  {
    type = mwk->inputType;
  }
  else if( mwk->keybordMode == 0 )
  {
    type = KeyMap::KEY_DATA_KEYBOARD_NORMAL;
  }
  else {
    type = KeyMap::KEY_DATA_KEYBOARD_HANGUL_N;
  }

  return swk->keyMap->GetKeyCode( mode, type, mwk->shift, id );
}

//------------------------------------------------------------------------------
/**
 * @brief   キーマップ配置
 *
 * @param   mode  入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutKeyMap( u32 mode )
{
  switch( m_MainWork.prm->language )
  {
  case POKEMON_LANG_NULL:    //!< 言語コード：無効な値
    GFL_ASSERT(0);
  case POKEMON_LANG_JAPAN:   //!< 言語コード：日本
  case POKEMON_LANG_CHINA:   //!< 言語コード：簡体字（中国）
  case POKEMON_LANG_TAIWAN:  //!< 言語コード：繁体字（台湾、香港）
    PutKeyMapJPN( mode );
    break;
  case POKEMON_LANG_KOREA:   //!< 言語コード：韓国
    PutKeyMapKOR( mode );
    break;
  default:
    PutKeyMapUSA( mode );
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キーマップ配置
 *
 * @param   lytid   レイアウトID
 * @param   mode    入力モード
 * @param   max     キーの数
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutKeyMapCore( u32 lytid, u32 mode, int max )
{
  SUB_WORK * swk = m_SubWork;

  for( int i=0; i<max; ++i )
  {
    gfl2::str::STRCODE & code = GetKeyCode( mode, swk->ui->GetCursorID(i) );
    swk->pGraWork->PutStrCode( lytid, &code, swk->pGraWork->GetKeyMapTextPane(mode, i), PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00, &m_Heap );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   日本語のキーマップ配置
 *
 * @param   mode    入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutKeyMapJPN( u32 mode )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  int max = 0;
  u32 lytID = 0;

  if( mode == INPUT_MODE_AIUEO )
  {
    max = 65;
    lytID = GraphicWork::LAYOUT_ID_LOWER;
  }
  else {
    max = 45;
    lytID = GraphicWork::LAYOUT_ID_LOWER_KEYBOARD;

    if( mwk->shift == 0 )
    {
      swk->pGraWork->PutShiftMark( false );
    }
    else {
      swk->pGraWork->PutShiftMark( true );
    }
  }

  PutKeyMapCore( lytID, mode, max );
  PutTypeMark( mode );
}

//------------------------------------------------------------------------------
/**
 * @brief   EFIGSのキーマップ配置
 *
 * @param   mode    入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutKeyMapUSA( u32 mode )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  int max = 0;
  u32 lytID = 0;

  if( mode == INPUT_MODE_AIUEO )
  {
    max = 65;
    lytID = GraphicWork::LAYOUT_ID_LOWER;
    PutTypeMark( mode );
  }
  else {
    max = 45;
    lytID = GraphicWork::LAYOUT_ID_LOWER_KEYBOARD;

    if( mwk->shift == 0 )
    {
      swk->pGraWork->PutShiftMark( false );
    }
    else {
      swk->pGraWork->PutShiftMark( true );
    }
  }

  PutKeyMapCore( lytID, mode, max );
}

//------------------------------------------------------------------------------
/**
 * @brief   韓国語のキーマップ配置
 *
 * @param   mode    入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutKeyMapKOR( u32 mode )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  int max = 0;
  u32 lytID = 0;

  if( mode == INPUT_MODE_AIUEO )
  {
    max = INPUT_CODE_MAX_AIUEO;
    lytID = GraphicWork::LAYOUT_ID_LOWER;
  }
  else {
    max = INPUT_CODE_MAX_KEYBOARD;
    lytID = GraphicWork::LAYOUT_ID_LOWER_KEYBOARD;

    if( mwk->shift == 0 )
    {
      swk->pGraWork->PutShiftMark( false );
    }
    else {
      swk->pGraWork->PutShiftMark( true );
    }
  }

  PutKeyMapCore( lytID, mode, max );
  PutTypeMark( mode );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力タイプ切り替え
 *
 * @param   none
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::IncInputType( void )
{
  switch( m_MainWork.prm->language )
  {
  case POKEMON_LANG_NULL:     //!< 言語コード：無効な値
    GFL_ASSERT( 0 );
  case POKEMON_LANG_JAPAN:    //!< 言語コード：日本
  case POKEMON_LANG_CHINA:   //!< 言語コード：簡体字（中国）
  case POKEMON_LANG_TAIWAN:  //!< 言語コード：繁体字（台湾、香港）
    return IncInputTypeJPN();
  case POKEMON_LANG_KOREA:    //!< 言語コード：韓国
    return IncInputTypeKOR();
  default:                    //!< 言語コード：その他
    return IncInputTypeUSA();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   日本語の入力タイプ切り替え
 *
 * @param   none
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::IncInputTypeJPN( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->inputType++;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    if( mwk->inputType == INPUT_TYPE_MAX )
    {
      mwk->inputType = INPUT_TYPE_HIRAGANA;
    }
    PutKeyMap( mwk->inputMode );
  }
  else {
    if( mwk->inputType == INPUT_TYPE_MARK )
    {
      mwk->inputType = INPUT_TYPE_HIRAGANA;
    }
    PutTypeMark( INPUT_MODE_KEYBOARD );
    FixInput();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   EFIGSの入力タイプ切り替え
 *
 * @param   none
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::IncInputTypeUSA( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputType++;

    if( mwk->inputType == INPUT_TYPE_ABC )
    {
      mwk->inputType++;
    }
    if( mwk->inputType == INPUT_TYPE_MAX )
    {
      mwk->inputType = INPUT_TYPE_CAPITAL;
    }

    PutKeyMap( mwk->inputMode );
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   韓国語の入力タイプ切り替え
 *
 * @param   none
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::IncInputTypeKOR( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputType++;

    if( mwk->inputType == INPUT_TYPE_KATAKANA )
    {
      mwk->inputType = INPUT_TYPE_ABC;
    }
    if( mwk->inputType == INPUT_TYPE_MAX )
    {
      mwk->inputType = INPUT_TYPE_HANGUL;
    }
  }
  else {
    if( mwk->inputType == INPUT_TYPE_HANGUL )
    {
      mwk->inputType = INPUT_TYPE_ABC;
      mwk->keybordMode = 0;
    }
    else if( mwk->inputType == INPUT_TYPE_ABC )
    {
      mwk->inputType = INPUT_TYPE_HANGUL;
      mwk->keybordMode = 1;
    }
    else {
      GFL_ASSERT( 0 );
      mwk->inputType = INPUT_TYPE_HANGUL;
      mwk->keybordMode = 0;
    }
  }

  FixInput();
  PutKeyMap( mwk->inputMode );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力タイプマーク表示
 *
 * @param   mode  入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutTypeMark( u32 mode )
{
#if 0
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  static const u32 buttonID[] = {
    UI::BTNID_HIRAGANA,   //!< かな
    UI::BTNID_KATAKANA,   //!< カナ
    UI::BTNID_ABC,        //!< ABC
    UI::BTNID_MARK,       //!< 記号
  };

  gfl2::lyt::LytPaneIndex partsIndex;
  gfl2::lyt::LytPaneIndex paneIndex;
  swk->ui->GetButtonIDBoundingPane( buttonID[mwk->inputType], &partsIndex, &paneIndex );

  swk->pGraWork->PutTypeMark( mode, partsIndex, paneIndex );
  //swk->pGraWork->PutTypeMark( mode, swk->ui->GetButtonIDBoundingPane( buttonID[mwk->inputType] ) );
#else
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  static const u32 buttonID[] = {
    UI::BTNID_HIRAGANA,   //!< かな
    UI::BTNID_KATAKANA,   //!< カナ
    UI::BTNID_ABC,        //!< ABC
    UI::BTNID_MARK,       //!< 記号
  };

  swk->pGraWork->PutTypeMark( mode, mwk->inputType );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   終了パラメータ設定
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetEndParam( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->prm->changeFlag = false;
  mwk->prm->isInputCancel = CheckInputAllSpace();   //!< スペースしか入力されていない = true;

  if( mwk->prm->isInputCancel == true )
  {
    return;
  }

  if( mwk->prm->strCode != NULL )
  {
    if( gfl2::str::StrComp( mwk->prm->strBuf->GetPtr(), mwk->str ) == false )
    {
      gfl2::str::StrCopy( mwk->prm->strCode, mwk->str );
      mwk->prm->changeFlag = true;
    }
  }
  if( mwk->prm->strBuf != NULL )
  {
    // @fix NMCat[2343]：元のニックネームと比較
    if( mwk->prm->oldNameBuf != NULL )
    {
      if( gfl2::str::StrComp( mwk->prm->oldNameBuf->GetPtr(), mwk->str ) == false )
      {
        mwk->prm->strBuf->SetStr( mwk->str );
        mwk->prm->changeFlag = true;
      }
    }
    else if( gfl2::str::StrComp( mwk->prm->strBuf->GetPtr(), mwk->str ) == false )
    {
      mwk->prm->strBuf->SetStr( mwk->str );
      mwk->prm->changeFlag = true;
    }
  }

#if PM_DEBUG
  GFL_PRINT( "■ input STRCODE\n  " );

  for( int i=0; i<INPUT_CODE_MAX; ++i )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }

    GFL_PRINT( "0x%04x, ", mwk->str[i] );
  }

  GFL_PRINT( "\n" );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コードが濁点・半濁点か
 *
 * @param   id  入力位置
 *
 * @retval  "KeyMap::CHG_DATA_DAKUTEN = 濁点"
 * @retval  "KeyMap::CHG_DATA_HANDAKUTEN = 半濁点"
 * @retval  "KeyMap::CHG_DATA_MAX = それ以外"
 */
//------------------------------------------------------------------------------
u32 MainSystem::CheckChangeCode( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  gfl2::str::STRCODE & code = GetKeyCode( mwk->inputMode, id );

  if( code == STRCODE_DAKUTEN )
  {
    return KeyMap::CHG_DATA_DAKUTEN;
  }
  if( code == STRCODE_HANDAKUTEN )
  {
    return KeyMap::CHG_DATA_HANDAKUTEN;
  }

  return KeyMap::CHG_DATA_MAX;
}

//------------------------------------------------------------------------------
/**
 * @brief   変換文字コードバッファを初期化
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::InitChangeCodeBuff( void )
{
  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; ++i )
  {
    m_SubWork->tmp[i] = gfl2::str::EOM_CODE;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字の終端を取得
 *
 * @param   none
 *
 * @return  終端位置
 */
//------------------------------------------------------------------------------
u32 MainSystem::GetEndInputPos( void )
{
  u32 i = 0;

  for( i=0; i<m_MainWork.inputSizeMax; ++i )
  {
    if( m_MainWork.str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
  }

  return i;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード設定
 *
 * @param   code  入力文字コード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetInputCode( gfl2::str::STRCODE code )
{
  MAIN_WORK * mwk = &m_MainWork;

  int start = static_cast<int>( mwk->inputSizeMax ) - 2;
  int end = static_cast<int>( mwk->inputPos );

  for( int i=start; i>=end; i-- )
  {
    mwk->str[i+1] = mwk->str[i];
  }

  mwk->str[mwk->inputPos] = code;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード設定
 *
 * @param   code  入力文字コード
 * @param   max    入力文字コード数
 *
 * @return  入力文字コード数
 */
//------------------------------------------------------------------------------
u32 MainSystem::SetInputCode( gfl2::str::STRCODE * code, u32 max )
{
  MAIN_WORK * mwk = &m_MainWork;

  for( u32 i=0; i<max; ++i )
  {
    if( code[i] == gfl2::str::EOM_CODE )
    {
      max = i;
      break;
    }
  }

  for( u32 i=0; i<max; ++i )
  {
    if( (mwk->inputPos + i) >= mwk->inputSizeMax )
    {
      max = i;
      break;
    }

    int start = static_cast<int>( mwk->inputSizeMax ) - 2;
    int end = static_cast<int>( mwk->inputPos + i );

    for( int j=start; j>=end; j-- )
    {
      mwk->str[j+1] = mwk->str[j];
    }

    mwk->str[mwk->inputPos + i] = code[i];
  }

  return max;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード範囲設定（１文字入力版）
 *
 * @param   code  入力文字コード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetInputAreaCode( gfl2::str::STRCODE code )
{
  MAIN_WORK * mwk = &m_MainWork;

  mwk->str[mwk->inputPos] = code;

  for( int i=mwk->inputPos+1; i<mwk->inputPos+mwk->inputSiz; i++ )
  {
    mwk->str[i] = gfl2::str::EOM_CODE;
  }

  for( int i=mwk->inputPos; i<INPUT_CODE_MAX-1; i++ )
  {
    for( int j=i+1; j<INPUT_CODE_MAX; j++ )
    {
      if( mwk->str[i] == gfl2::str::EOM_CODE )
      {
        mwk->str[i] = mwk->str[j];
        mwk->str[j] = gfl2::str::EOM_CODE;
      }
      else {
        break;
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード範囲設定
 *
 * @param   code  入力文字コード
 * @param   max    入力文字コード数
 *
 * @return  入力文字コード数
 */
//------------------------------------------------------------------------------
u32 MainSystem::SetInputAreaCode( gfl2::str::STRCODE * code, u32 max )
{
  MAIN_WORK * mwk = &m_MainWork;

  // 範囲を詰める
  for( int i=mwk->inputPos; i<INPUT_CODE_MAX-1; i++ )
  {
    if( (i+mwk->inputSiz) >= INPUT_CODE_MAX )
    {
      mwk->str[i] = gfl2::str::EOM_CODE;
    }
    else {
      mwk->str[i] = mwk->str[i+mwk->inputSiz];
    }
  }

  mwk->str[INPUT_CODE_MAX-1] = gfl2::str::EOM_CODE;

  return SetInputCode( code, max );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード設定
 *
 * @param   id  入力位置
 *
 * @retval  "true = 入力可"
 * @retval  "false = 入力不可"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetInputCode( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;
  u32 chg = CheckChangeCode( id );    //!< 濁点、半濁点チェック
  bool ret = false;

  // 通常
  if( chg == KeyMap::CHG_DATA_MAX )
  {
    if( !( mwk->inputPos >= mwk->inputSizeMax ) )
    {
      gfl2::str::STRCODE & code = GetKeyCode( mwk->inputMode, id );

      if( mwk->inputSiz == 0 )
      {
        SetInputCode( code );
      }
      else {
        SetInputAreaCode( code );
      }

      PutInputCode();
      mwk->inputPos++;
      swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
      ret = true;
    }
    else {
      // 入力不可
      ret = false;
    }
  }
  // 濁点/半濁点
  else {
    if( mwk->inputPos != 0 && mwk->inputSiz == 0 )
    {
      u32  sp = mwk->inputPos - 1;

      InitChangeCodeBuff();
      swk->tmp[0] = mwk->str[sp];
      swk->keyMap->MakeCheckCode( swk->tmp );
      
      if( swk->keyMap->GetChangeCode( chg, swk->tmp, swk->tmp ) == true )
      {
        mwk->str[sp] = swk->tmp[0];
        swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[sp], sp, &m_Heap );
        ret = true;
      }
      else {
        // 入力不可
        ret = false;
      }
    }
    else {
      // 入力不可
      ret = false;
    }
  }

  mwk->inputSiz = 0;
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );

  swk->pGraWork->SetOverwriteCursorVisible( false );

  if( mwk->inputPos == mwk->inputSizeMax )
  {
    MoveCursorEndPos();
  }

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   スペース入力
 *
 * @param   none
 *
 * @retval  "true = 入力可"
 * @retval  "false = 入力不可"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetInputSpace( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  bool ret = true;

  if( !( mwk->inputPos >= mwk->inputSizeMax ) )
  {
    if( mwk->inputSiz == 0 )
    {
      SetInputCode( mwk->codeSpace );
    }
    else {
      SetInputAreaCode( mwk->codeSpace );
    }

    PutInputCode();
    mwk->inputPos++;
    swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
  }
  else {
    // 入力不可
    ret = false;
  }

  mwk->inputSiz = 0;
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
  swk->pGraWork->SetOverwriteCursorVisible( false );

  if( mwk->inputPos == mwk->inputSizeMax )
  {
    MoveCursorEndPos();
  }

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード表示
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutInputCode( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  for( u32 i=0; i<mwk->inputSizeMax; i++ )
  {
    if( mwk->str[i] != gfl2::str::EOM_CODE )
    {
      swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
    }
    else {
      swk->pGraWork->ClearInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, i );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コードループ変換
 *
 * @param   none
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetLoopCode( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( mwk->inputPos == 0 || mwk->inputSiz != 0 )
  {
    return false;
  }

  u32 sp = mwk->inputPos - 1;

  if( swk->keyMap->GetLoopCode( mwk->str[sp], swk->tmp ) == true )
  {
    mwk->str[sp] = swk->tmp[0];
    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[sp], sp, &m_Heap );
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力モード切り替え
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeKeyBoardMode( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  switch( mwk->prm->language )
  {
  case POKEMON_LANG_NULL:   //!< 言語コード：無効な値
    GFL_ASSERT( 0 );
  case POKEMON_LANG_JAPAN:  //!< 言語コード：日本
    ChangeKeyBoardModeJPN();
    break;
  case POKEMON_LANG_CHINA:  //!< 言語コード：簡体字（中国）
  case POKEMON_LANG_TAIWAN: //!< 言語コード：繁体字（台湾、香港）
    ChangeKeyBoardModeCHN();
    break;
  case POKEMON_LANG_KOREA:  //!< 言語コード：韓国
    ChangeKeyBoardModeKOR();
    break;
  default:                  //!< その他
    ChangeKeyBoardModeUSA();
    break;
  }

  ChangeUiData( mwk->inputMode );
  swk->pGraWork->PutKeyBoard( mwk->inputMode );
  PutKeyMap( mwk->inputMode );
  MoveCursor( mwk->inputMode, 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   日本語の入力モード切り替え
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeKeyBoardModeJPN( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputMode = INPUT_MODE_KEYBOARD;

    if( mwk->inputType == INPUT_TYPE_MARK )
    {
      mwk->inputType = INPUT_TYPE_HIRAGANA;
    }

    InitRomajiBuff();
  }
  else {
    mwk->inputMode = INPUT_MODE_AIUEO;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   EFIGSの入力モード切り替え
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeKeyBoardModeUSA( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputMode = INPUT_MODE_KEYBOARD;

    if( mwk->inputType == INPUT_TYPE_SMALL )
    {
      mwk->shift = 0;
      swk->pGraWork->PutShiftMark( false );
    }
    else if( mwk->inputType == INPUT_TYPE_CAPITAL )
    {
      mwk->shift = 1;
      swk->pGraWork->PutShiftMark( true );
    }

    mwk->inputType = INPUT_TYPE_ABC;
  }
  else {
    mwk->inputMode = INPUT_MODE_AIUEO;
    if( mwk->shift == 0 )
    {
      mwk->inputType = INPUT_TYPE_SMALL;
    }
    else {
      mwk->inputType = INPUT_TYPE_CAPITAL;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   韓国語の入力モード切り替え
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeKeyBoardModeKOR( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputMode = INPUT_MODE_KEYBOARD;

    if( mwk->inputType == INPUT_TYPE_HANGUL )
    {
      mwk->keybordMode = 1;
    }
    else {
      mwk->keybordMode = 0;
    }

    if( mwk->inputType == INPUT_TYPE_MARK )
    {
      //mwk->inputType = INPUT_TYPE_ABC;
      // 「記号」->「かな」
      mwk->inputType = INPUT_TYPE_HIRAGANA;
      mwk->keybordMode = 1;
    }
  }
  else {
    mwk->inputMode = INPUT_MODE_AIUEO;
  }

  InitRomajiBuff();
}

//------------------------------------------------------------------------------
/**
 * @brief   簡体字・繁体字の入力モード切り替え
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeKeyBoardModeCHN( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->inputMode == INPUT_MODE_AIUEO )
  {
    mwk->inputMode = INPUT_MODE_KEYBOARD;

    if( mwk->inputType == INPUT_TYPE_MARK )
    {
      mwk->inputType = INPUT_TYPE_ABC;
    }
  }
  else {
    mwk->inputMode = INPUT_MODE_AIUEO;
  }

  InitRomajiBuff();
}


//------------------------------------------------------------------------------
/**
 * @brief   UI設定
 *
 * @param   mode  入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetUiData( u32 mode )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;
  gfl2::lyt::LytWk * lytwk;
  gfl2::lyt::LytMultiResID resID = swk->pGraWork->GetLayoutResourceID(0);

  u8 langID = mwk->prm->language;

  if( mode == INPUT_MODE_AIUEO )
  {
    lytwk = swk->pGraWork->GetLayoutWork(GraphicWork::LAYOUT_ID_LOWER);
    swk->ui->CreateUiData( lytwk, resID, INPUT_MODE_AIUEO, langID );
  }
  else {
    lytwk = swk->pGraWork->GetLayoutWork(GraphicWork::LAYOUT_ID_LOWER_KEYBOARD);
    swk->ui->CreateUiData( lytwk, resID, INPUT_MODE_KEYBOARD, langID );
  }

  lytwk = swk->pGraWork->GetLayoutWork(GraphicWork::LAYOUT_ID_LOWER_CHANGE);
  swk->ui->SetArrowButtonData( lytwk, resID );

  if( mwk->prm->type == TYPE_POKEMON_NAME )
  {
    lytwk = swk->pGraWork->GetLayoutWork(GraphicWork::LAYOUT_ID_LOWER_INPUT);
    swk->ui->SetIconButtonData( lytwk, resID );
  }

  swk->ui->SetButtonNotifyTimming();
}

//------------------------------------------------------------------------------
/**
 * @brief   UI変更
 *
 * @param   mode  入力モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::ChangeUiData( u32 mode )
{
  SUB_WORK * swk = m_SubWork;

  swk->ui->DeleteUiData();
  SetUiData( mode );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力カーソル移動
 *
 * @param   mode  入力モード
 * @param   pos    移動位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::MoveCursor( u32 mode, u32 pos )
{
  SUB_WORK * swk = m_SubWork;

  gfl2::lyt::LytPaneIndex partsIndex;
  gfl2::lyt::LytPaneIndex paneIndex;
  swk->ui->GetButtonBoundingPane(pos, &partsIndex, &paneIndex);

  swk->pGraWork->MoveCursor( mode, partsIndex, paneIndex );
  //swk->pGraWork->MoveCursor( mode, swk->ui->GetButtonBoundingPane(pos) );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了位置に入力カーソルを移動
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::MoveCursorEndPos( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( swk->ui->SetCursorPos( swk->ui->GetButtonPosID(UI::BTNID_END) ) == true )
  {
    MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力のバッファを初期化
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::InitRomajiBuff( void )
{
  SUB_WORK * swk = m_SubWork;

  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    swk->romaji_str[i] = gfl2::str::EOM_CODE;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力：１文字消去
 *
 * @param   none
 *
 * @retval  "true = 消去した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::ClearRomajiOne( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    if( swk->romaji_str[i] == gfl2::str::EOM_CODE )
    {
      if( i == 0 )
      {
        return false;
      }

      swk->romaji_str[i-1] = gfl2::str::EOM_CODE;
      break;
    }
  }
  swk->romaji_str[KeyMap::CHANGE_CODE_LEN-1] = gfl2::str::EOM_CODE;

  // フィールド再表示
  swk->pGraWork->InitInputField();
  swk->pGraWork->PutInputSize(
    GraphicWork::LAYOUT_ID_LOWER_INPUT, PANENAME_STRINPUT_INP_LOW_000_PANE_CURSOR_02, mwk->inputPos, mwk->inputSiz );

  // 文字色を戻しておく
  InitInputColor();
  for( int i=0; i<INPUT_CODE_MAX; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
  }

  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    if( swk->romaji_str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }

    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &swk->romaji_str[i], mwk->inputPos+i, &m_Heap );
    swk->pGraWork->SetInputStrColor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos+i, GraphicWork::INPUT_COLOR_MODE_ROMAJI );
  }

  return true;}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力：１文字入力
 *
 * @param   id  入力位置
 *
 * @retval  "true = 入力した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetRomajiOne( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    if( swk->romaji_str[i] == gfl2::str::EOM_CODE )
    {
      gfl2::str::STRCODE & code = GetKeyCode( mwk->inputMode, id );
      swk->romaji_str[i] = code;
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力
 *
 * @param   id  入力位置
 *
 * @retval  "true = 入力した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetRomaji( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( mwk->inputPos >= mwk->inputSizeMax )
  {
    MoveCursorEndPos();
    return false;
  }

  if( SetRomajiOne( id ) == false )
  {
    return false;
  }

  InitInputColor();

  gfl2::str::STRCODE rest[KeyMap::CHANGE_CODE_LEN];

  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    InitChangeCodeBuff();

    for( int j=0; j<KeyMap::CHANGE_CODE_LEN; j++ )
    {
      if( (j+i) >= KeyMap::CHANGE_CODE_LEN )
      {
        break;
      }
      
      swk->tmp[j] = swk->romaji_str[j+i];
    }

    if( swk->tmp[0] == gfl2::str::EOM_CODE ){
      break;
    }

    swk->keyMap->MakeCheckCode( swk->tmp );
    rest[0] = gfl2::str::EOM_CODE;
    
    if( swk->keyMap->GetChangeRomajiCode( mwk->inputType, swk->tmp, swk->tmp, rest ) == true )
    {
      // 未変換部分を結合
      for( int j=0; j<KeyMap::CHANGE_CODE_LEN; j++ )
      {
        if( (j+i) >= KeyMap::CHANGE_CODE_LEN )
        {
          break;
        }

        swk->romaji_str[j+i] = swk->tmp[j];
      }

      if( mwk->inputSiz == 0 )
      {
        mwk->inputPos += SetInputCode( swk->romaji_str, KeyMap::CHANGE_CODE_LEN );
      }
      else {
        mwk->inputPos += SetInputAreaCode( swk->romaji_str, KeyMap::CHANGE_CODE_LEN );
      }

      if( mwk->inputPos > mwk->inputSizeMax )
      {
        mwk->inputPos = mwk->inputSizeMax;
      }

      //      InitRomajiBuff();
      // 描画
      swk->pGraWork->InitInputField();
      for( int j=0; j<INPUT_CODE_MAX; j++ )
      {
        if( mwk->str[j] == gfl2::str::EOM_CODE )
        {
          break;
        }

        swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[j], j, &m_Heap );
      }

      swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
      mwk->inputSiz = 0;
      swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
      swk->pGraWork->SetOverwriteCursorVisible( false );

      if( mwk->inputPos == mwk->inputSizeMax )
      {
        MoveCursorEndPos();
        InitRomajiBuff();
        return true;
      }

      if( rest[0] != gfl2::str::EOM_CODE )
      {
        InitRomajiBuff();
        for( int j=0; j<KeyMap::CHANGE_CODE_LEN; j++ )
        {
          swk->romaji_str[j] = rest[j];

          if( rest[j] == gfl2::str::EOM_CODE )
          {
            break;
          }
        }
        break;
      }
      else {
        InitRomajiBuff();
        return true;
      }
    }
  }

  // 仮描画
  for( int i=0; i<KeyMap::CHANGE_CODE_LEN; i++ )
  {
    if( swk->romaji_str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }

    swk->pGraWork->PutInputStr(
      GraphicWork::LAYOUT_ID_LOWER_INPUT, &swk->romaji_str[i], mwk->inputPos+i, &m_Heap );
    swk->pGraWork->SetInputStrColor(
      GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos+i, GraphicWork::INPUT_COLOR_MODE_ROMAJI );
  }

  return true;}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力確定
 *
 * @param   none
 *
 * @retval  "true = 成功"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::FixInputRomaji( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( CheckFixRomaji() == false )
  {
    return false;
  }

  if( mwk->inputSiz == 0 )
  {
    mwk->inputPos += SetInputCode( swk->romaji_str, KeyMap::CHANGE_CODE_LEN );
  }
  else {
    mwk->inputPos += SetInputAreaCode( swk->romaji_str, KeyMap::CHANGE_CODE_LEN );
  }

  InitRomajiBuff();
  // 描画
  swk->pGraWork->InitInputField();
  for( int i=0; i<INPUT_CODE_MAX; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }

    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
  }

  // 文字列の色を戻す
  InitInputColor();

  swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
  mwk->inputSiz = 0;
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
  swk->pGraWork->SetOverwriteCursorVisible( false );
  
  if( mwk->inputPos == mwk->inputSizeMax )
  {
    MoveCursorEndPos();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ローマ時入力を確定できるか
 *
 * @param   none
 *
 * @retval  "true = 可"
 * @retval  "false = 不可"
 */
//------------------------------------------------------------------------------
bool MainSystem::CheckFixRomaji( void )
{
  SUB_WORK * swk = m_SubWork;

  if( swk->romaji_str[0] == gfl2::str::EOM_CODE )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   言語ごとの初期設定
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::InitLang( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  u32 lang = mwk->prm->language;

  // 無効言語
  if( lang == POKEMON_LANG_NULL )
  {
    GFL_ASSERT( 0 );
    lang = POKEMON_LANG_JAPAN;
  }

  //! 例外対応：フェスサークル名
  if( mwk->prm->type == TYPE_FESCIRCLE_NAME )
  {
    if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_KOREA || lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
    {
      mwk->inputSizeMax = 8;
    }

    if( lang == POKEMON_LANG_JAPAN )
    {
      mwk->codeSpace = STRCODE_SPACE;
    }
    else if( lang == POKEMON_LANG_CHINA ||
      lang == POKEMON_LANG_TAIWAN )
    {
      // @fix NMCat[119]
      //if( mwk->inputMode == INPUT_MODE_KEYBOARD )
      //{
      //  mwk->keybordMode = 1;
      //}

      if( mwk->inputMode == INPUT_MODE_KEYBOARD )
      {
        mwk->inputType = INPUT_TYPE_ABC;
      }

      mwk->codeSpace = STRCODE_SPACE;
    }
    else if( lang == POKEMON_LANG_KOREA )
    {
      if( mwk->inputMode == INPUT_MODE_KEYBOARD )
      {
        mwk->keybordMode = 1; //!< ハングル専用のキーボードにする
      }

      mwk->codeSpace = STRCODE_SPACE;

      if( mwk->inputType == INPUT_TYPE_KATAKANA )
      {
        mwk->inputType = INPUT_TYPE_HIRAGANA;
      }
    }
    else {
      mwk->codeSpace = STRCODE_SPACE_HALF;

      if( mwk->inputMode == INPUT_MODE_KEYBOARD )
      {
        mwk->inputType = INPUT_TYPE_ABC;
      }
    }
  }
  else if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
  {
    if( mwk->prm->type != TYPE_FREE_WORD_PSS &&
      mwk->prm->type != TYPE_FREE_WORD_GTS &&
      mwk->prm->type != TYPE_HAPPY_SIGN &&
      mwk->prm->type != TYPE_BOX_NAME &&
      mwk->prm->type != TYPE_INTERVIEW_LONG &&
      mwk->prm->type != TYPE_BASE_WORD &&
      mwk->prm->type != TYPE_BASE_GREETING &&
      mwk->prm->type != TYPE_BASE_GOODS )
    {
        mwk->inputSizeMax /= 2;
    }

    mwk->codeSpace = STRCODE_SPACE;

    // TPC要望：GTSのポケモン検索(簡体字、繁体字)はカタカナで開始
    if( lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
    {
      if( mwk->prm->type == TYPE_POKE_WANT )
      {
        mwk->inputType = INPUT_TYPE_KATAKANA;
        mwk->inputMode = INPUT_MODE_AIUEO;
        mwk->keybordMode = 0;
      }
    }
  }
  else if( lang == POKEMON_LANG_KOREA )
  {
    if( mwk->prm->type != TYPE_FREE_WORD_PSS &&
      mwk->prm->type != TYPE_FREE_WORD_GTS &&
      mwk->prm->type != TYPE_HAPPY_SIGN &&
      mwk->prm->type != TYPE_BOX_NAME &&
      mwk->prm->type != TYPE_INTERVIEW_LONG &&
      mwk->prm->type != TYPE_BASE_WORD &&
      mwk->prm->type != TYPE_BASE_GREETING &&
      mwk->prm->type != TYPE_BASE_GOODS )
    {
        mwk->inputSizeMax /= 2;
    }
    
    if( mwk->inputMode == INPUT_MODE_KEYBOARD )
    {
      mwk->keybordMode = 1;
    }

//    // BTS[6958] 韓国語のスペースが全角になっている不具合を修正
//#ifdef BUGFIX_BTS6958_20130528
//    mwk->codeSpace = STRCODE_SPACE_HALF;
//#else
//    mwk->codeSpace = STRCODE_SPACE;
//#endif
    mwk->codeSpace = STRCODE_SPACE_HALF;

    if( mwk->inputType == INPUT_TYPE_KATAKANA )
    {
      mwk->inputType = INPUT_TYPE_HIRAGANA;
    }
  }
  else {
    if( mwk->prm->type == TYPE_BOX_NAME )
    {
      mwk->inputSizeMax = INPUTLEN_BOX_NAME_E;
    }
    else if( mwk->prm->type == TYPE_BASE_WORD )
    {
      mwk->inputSizeMax = INPUTLEN_BASE_WORD_E;
    }
    mwk->codeSpace = STRCODE_SPACE_HALF;

    if( mwk->inputMode == INPUT_MODE_KEYBOARD )
    {
      mwk->inputType = INPUT_TYPE_ABC;
    }
  }

  if( mwk->inputSizeMax >= INPUT_CODE_MAX )
  {
    GFL_ASSERT( 0 );
    mwk->inputSizeMax = INPUT_CODE_MAX-1;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   シフトモード終了
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::EndShiftMode( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  u32 lang = mwk->prm->language;

  if( mwk->inputMode == INPUT_MODE_KEYBOARD &&
    mwk->shift == 1 &&
    ( lang == POKEMON_LANG_ENGLISH || lang == POKEMON_LANG_KOREA ) )
  {
    mwk->shift = 0;
    PutKeyMap( mwk->inputMode );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力処理
 *
 * @param   id  入力結果
 *
 * @return  次のシーケンス
 */
//------------------------------------------------------------------------------
int MainSystem::UpdateDecide( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  switch( id )
  {
  case UI::RET_CHG_HIRAGANA:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    if( mwk->inputType != INPUT_TYPE_HIRAGANA )
    {
      mwk->inputType = INPUT_TYPE_HIRAGANA;
      if( mwk->inputMode == INPUT_MODE_AIUEO )
      {
        PutKeyMap( mwk->inputMode );
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          FixInput();
        }
      }
      else {
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          mwk->keybordMode = 1;
          PutKeyMap( mwk->inputMode );
        }
        else {
          PutTypeMark( INPUT_MODE_KEYBOARD );
        }
        FixInput();
      }
      MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
    }
    break;

  case UI::RET_CHG_KATAKANA:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    if( mwk->inputType != INPUT_TYPE_KATAKANA )
    {
      mwk->inputType = INPUT_TYPE_KATAKANA;
      if( mwk->inputMode == INPUT_MODE_AIUEO )
      {
        PutKeyMap( mwk->inputMode );
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          FixInput();
        }
      }
      else {
        PutTypeMark( INPUT_MODE_KEYBOARD );
        FixInput();
      }
      MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
    }
    break;

  case UI::RET_CHG_ABC:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    if( mwk->inputType != INPUT_TYPE_ABC )
    {
      mwk->inputType = INPUT_TYPE_ABC;
      if( mwk->inputMode == INPUT_MODE_AIUEO )
      {
        PutKeyMap( mwk->inputMode );
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          FixInput();
        }
      }
      else {
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          mwk->keybordMode = 0;
          PutKeyMap( mwk->inputMode );
        }
        else {
          PutTypeMark( INPUT_MODE_KEYBOARD );
        }
        FixInput();
      }
      MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
    }
    break;

  case UI::RET_CHG_MARK:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    if( mwk->inputMode == INPUT_MODE_AIUEO )
    {
      if( mwk->inputType != INPUT_TYPE_MARK )
      {
        mwk->inputType = INPUT_TYPE_MARK;
        PutKeyMap( mwk->inputMode );
        if( mwk->prm->language == POKEMON_LANG_KOREA )
        {
          FixInput();
        }
      }
      MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
    }
    break;

  case UI::RET_DELETE_KEY:
    swk->pGraWork->StartKeySelectAnime( mwk->inputMode, swk->ui->GetButtonPosIDAnmOffset(UI::BTNID_DELETE) );
  case UI::RET_DELETE:
    if( mwk->inputMode == INPUT_MODE_KEYBOARD )
    {
      if( ClearRomajiOne() == true )
      {
        Sound::PlaySE( STR_INPUT_SE_CNACEL );
        EndShiftMode();
        break;
      }
    }

    {
      bool ret = false;
      if( mwk->prm->language == POKEMON_LANG_KOREA && mwk->inputType == INPUT_TYPE_HANGUL )
      {
        ret = DeleteCodeHangul();
      }
      else {
        ret = DeleteCode();
      }
      if( ret == true )
      {
        Sound::PlaySE( STR_INPUT_SE_CNACEL );
      }
      else {
        Sound::PlaySE( STR_INPUT_SE_ERROR );
      }
      EndShiftMode();
    }
    break;

  case UI::RET_END:          // 終了
    if( FixInput() == false )
    {
      u32 ngc = CheckInputWord();
      // ポケモン名の場合は終了
      if( ngc == WORD_CHECK_POKENAME )
      {
        Sound::PlaySE( STR_INPUT_SE_COMPLETE );
        return SetEnd();
        // エラーじゃない場合はNGワードチェックへ
      }
      else if( ngc == WORD_CHECK_DO_NOT_NGC )
      {
        Sound::PlaySE( STR_INPUT_SE_COMPLETE );
        return SetEnd();
      }
      else if( ngc == WORD_CHECK_TRUE )
      {
        Sound::PlaySE( STR_INPUT_SE_COMPLETE );
        mwk->subSeq = 0;
        return MAINSEQ_NG_WORD_CHECK;
        // エラー
      }
      else {
        Sound::PlaySE( STR_INPUT_SE_ERROR );
        swk->errPutFlag = true;
        // 連打入力エラー
        if( ngc == WORD_CHECK_FAST )
        {
          swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_FAST, &m_Heap );
          // 空欄（名前入力時）
        }
        else if( ngc == WORD_CHECK_CHARNAME_NULL )
        {
          swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_CHARNAME_NULL, &m_Heap );
          // 空欄（ニックネーム入力時）
        }
        else if( ngc == WORD_CHECK_NICKNAME_NULL )
        {
          swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_NICKNAME_NULL, &m_Heap );
          // 空欄（インタビュー入力時）
        }
        else if( ngc == WORD_CHECK_INTERVIEW_NULL )
        {
          swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_INTERVIEW_NULL, &m_Heap );
        }
        else {
          GFL_ASSERT( 0 );
        }
      }
    }
    else {
      Sound::PlaySE( STR_INPUT_SE_DECIDE );
    }
    break;

  case UI::RET_SPACE:
    FixInput();
    if( SetInputSpace() == true )
    {
      Sound::PlaySE( STR_INPUT_SE_DECIDE );
    }
    else {
      Sound::PlaySE( STR_INPUT_SE_ERROR );
    }
    EndShiftMode();
    break;

  case UI::RET_SHIFT:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    mwk->shift ^= 1;
    if( mwk->prm->language != POKEMON_LANG_KOREA )
    {
      FixInput();
    }
    PutKeyMap( mwk->inputMode );
    break;

  case UI::RET_NONE:
  case UI::RET_CURSOR_MOVE:
  case UI::RET_CURSOR_MOVE_TOUCH:
  case UI::RET_END_JUMP:
  case UI::RET_CHG_INCREMENT:
  case UI::RET_INPUT:
  case UI::RET_INPUT_TOUCH:
  case UI::RET_CHG_CODE:
  case UI::RET_DECIDE:
  case UI::RET_ICON:
  case UI::RET_SCROLL_TOUCH:
  case UI::RET_L_SCROLL_KEY:
  case UI::RET_L_SCROLL:
  case UI::RET_R_SCROLL_KEY:
  case UI::RET_R_SCROLL:
    GFL_ASSERT( 0 );
    break;

  default:
    {
      bool ret = true;
      if( mwk->prm->language == POKEMON_LANG_KOREA )
      {
        if( mwk->inputType == INPUT_TYPE_HANGUL )
        {
          ret = SetHangleInput( id-UI::RET_KEYBOARD );
          EndShiftMode();
        }
        else {
          ret = SetInputCode( id-UI::RET_KEYBOARD );
        }
      }
      else {
        // 50音入力
        if( mwk->inputMode == INPUT_MODE_AIUEO )
        {
          ret = SetInputCode( id-UI::RET_KEYBOARD );
          // キーボード：ABC
        }
        else if( mwk->inputType == INPUT_TYPE_ABC )
        {
          ret = SetInputCode( id-UI::RET_KEYBOARD );
          EndShiftMode();
          // キーボード：かな/カナ
        }
        else {
          ret = SetRomaji( id-UI::RET_KEYBOARD );
          EndShiftMode();
        }
      }

      if( ret == true )
      {
        Sound::PlaySE( STR_INPUT_SE_DECIDE );
      }
      else {
        Sound::PlaySE( STR_INPUT_SE_ERROR );
      }
    }
    break;
  }

  return MAINSEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力データ初期化
 *
 * @param   comp  初期文字列を設定するか
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::InitInputCode( bool comp )
{
  MAIN_WORK * mwk = &m_MainWork;

  for( int i=0; i<INPUT_CODE_MAX; i++ )
  {
    mwk->str[i] = gfl2::str::EOM_CODE;
  }

  // 起動パラメータに渡したバッファにセットされている文字を初期メッセージとして使用する
  if( comp == true ||
    mwk->prm->type == TYPE_BOX_NAME ||
    mwk->prm->type == TYPE_FREE_WORD_PSS ||
    mwk->prm->type == TYPE_FREE_WORD_GTS ||
    mwk->prm->type == TYPE_HAPPY_SIGN ||
    mwk->prm->type == TYPE_INTERVIEW_VERY_SHORT ||
    mwk->prm->type == TYPE_BASE_WORD ||
    mwk->prm->type == TYPE_BASE_HAPPY ||
    mwk->prm->type == TYPE_BASE_CHEER ||
    mwk->prm->type == TYPE_BASE_GREETING ||
    mwk->prm->type == TYPE_BASE_GOODS )
  {
    if( mwk->prm->strCode != NULL )
    {
      gfl2::str::StrCopy( mwk->str, mwk->prm->strCode );
    }
    else if( mwk->prm->strBuf != NULL )
    {
      gfl2::str::StrCopy( mwk->str, mwk->prm->strBuf->GetPtr() );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力データに初期文字列を設定
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetDefaultInput( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  FixInput();  // ローマ字入力を強制終了
  InitInputCode( true );
    CheckInitStrCode();   //!< エラーチェック
  PutInputCode();
  mwk->inputPos = GetEndInputPos();
  swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );

  mwk->inputSiz = 0;
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
  swk->pGraWork->SetOverwriteCursorVisible( false );

  if( mwk->inputPos == mwk->inputSizeMax )
  {
    MoveCursorEndPos();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字列をチェック
 *
 * @param   none
 *
 * @retval  "WORD_CHECK_TRUE = 正常"
 * @retval  "WORD_CHECK_TRUE != 入力エラーコード"
 */
//------------------------------------------------------------------------------
u32 MainSystem::CheckInputWord( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  // ポケモン名
  if( CheckPokemonName() == true )
  {
    // GFL_PRINT( "ポケモンの名前と一致しました！\n" );
    return WORD_CHECK_POKENAME;
  }
  // 連打入力
  if( CheckInputWordFast() == false )
  {
    return WORD_CHECK_FAST;
  }
  // 空欄チェック
  u32 ret = CheckBufferNull();
  if( ret != WORD_CHECK_TRUE )
  {
    return ret;
  }
  // スペースのみ
  if( CheckInputAllSpace() == true )
  {
#if PM_DEBUG
    if( mwk->prm->type == TYPE_CHARACTER_NAME ||
      mwk->prm->type == TYPE_CHARACTER_NAME_DEBUG )  // 人物名デバッグ
#else
    if( mwk->prm->type == TYPE_CHARACTER_NAME )
#endif  // PM_DEBUG
    {
      return WORD_CHECK_CHARNAME_NULL;
    }
    else if( mwk->prm->type == TYPE_NICKNAME )
    {
      return WORD_CHECK_NICKNAME_NULL;
    }
    else if( mwk->prm->type == TYPE_INTERVIEW_LONG ||
      mwk->prm->type == TYPE_INTERVIEW_SHORT ||
      mwk->prm->type == TYPE_INTERVIEW_VERY_SHORT )
    {
      return WORD_CHECK_INTERVIEW_NULL;
    }
  }

  // NGワードチェックを行わない
  if( mwk->prm->type == TYPE_BOX_NAME  ||       //!< ボックス名
      mwk->prm->type == TYPE_POKE_WANT ||       //!< GTS：ほしいポケモン
      mwk->prm->type == TYPE_FESCIRCLE_NAME )   //!< フェスサークル名
  {
    return WORD_CHECK_DO_NOT_NGC;
  }

  return WORD_CHECK_TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   連打入力チェック
 *
 * @param   none
 *
 * @retval  "true = 正常"
 * @retval  "false = 不正"
 */
//------------------------------------------------------------------------------
bool MainSystem::CheckInputWordFast( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

#if PM_DEBUG
  if( mwk->prm->type == TYPE_CHARACTER_NAME ||
    mwk->prm->type == TYPE_CHARACTER_NAME_DEBUG )  // 人物名デバッグ
#else
  if( mwk->prm->type == TYPE_CHARACTER_NAME )
#endif  // PM_DEBUG
  {
    gfl2::str::STRCODE code;
    u8  lang = mwk->prm->language;
    if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_KOREA || lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
    {
      code = swk->keyMap->GetKeyCode( INPUT_MODE_AIUEO, KeyMap::KEY_DATA_50K_HIRAGANA, 0, 0 );
    }
    else {
      // 英語はSHIFTが解除されるが、同じなので問題ない
      code = swk->keyMap->GetKeyCode( INPUT_MODE_KEYBOARD, KeyMap::KEY_DATA_KEYBOARD_NORMAL, 1, 0 );
    }

    for( u32 i=0; i<mwk->inputSizeMax; i++ )
    {
      if( mwk->str[i] != code )
      {
        return true;
      }
    }
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力バッファが空の場合を許可するか
 *
 * @param   none
 *
 * @retval  "WORD_CHECK_TRUE = 正常"
 * @retval  "WORD_CHECK_CHARNAME_NULL = エラー：プレイヤー名入力"
 * @retval  "WORD_CHECK_NICKNAME_NULL = エラー：プレイヤーニックネーム入力"
 */
//------------------------------------------------------------------------------
u32 MainSystem::CheckBufferNull( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  if( mwk->str[0] == gfl2::str::EOM_CODE )
  {
#if PM_DEBUG
    if( mwk->prm->type == TYPE_CHARACTER_NAME ||
      mwk->prm->type == TYPE_CHARACTER_NAME_DEBUG )  // 人物名デバッグ
#else
    if( mwk->prm->type == TYPE_CHARACTER_NAME )
#endif  // PM_DEBUG
    {
      return WORD_CHECK_CHARNAME_NULL;
    }

    if( mwk->prm->type == TYPE_NICKNAME )
    {
      return WORD_CHECK_NICKNAME_NULL;
    }

    if( mwk->prm->type == TYPE_INTERVIEW_LONG ||
      mwk->prm->type == TYPE_INTERVIEW_SHORT ||
      mwk->prm->type == TYPE_INTERVIEW_VERY_SHORT )
    {
      return WORD_CHECK_INTERVIEW_NULL;
    }
  }

  return WORD_CHECK_TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   NGワードチェック初期化
 *
 * @param   none
 *
 * @retval  "true = 成功"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::InitNgWordCheck( void )
{
  m_SubWork->ngWordResult = WORD_CHECK_THREAD_RESULT_NULL;

#ifdef GF_PLATFORM_CTR
  gfl2::ngc::NGWordChecker * ngc = m_MainWork.pNGWordChecker;

  return ngc->CheckFreewordWithRegulationASync(
    m_pNgcWorkHeap, gfl2::ngc::NGWordChecker::ALL_REGION,
    m_MainWork.str, this, (16 - 1) );
    //m_MainWork.str, this, STRINPUT_NG_WORD_CHECK_THREAD_PRIORITY );
#else
  m_SubWork->ngWordResult = WORD_CHECK_TRUE;

  return true;
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   NGワードチェック終了
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::EndNgWordCheck( void )
{
  gfl2::ngc::NGWordChecker * ngc = m_MainWork.pNGWordChecker;

  if( ngc != NULL )
  {
    ngc->DeleteAsync();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字列をポケモン名と比較
 *
 * @param   none
 *
 * @retval  "true = 同じポケモン名あり"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::CheckPokemonName( void )
{
  return m_SubWork->pGraWork->CheckPokemonName( m_MainWork.str );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字列が全てスペースかをチェック
 *
 * @param   none
 *
 * @retval  "true = 全てスペース"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::CheckInputAllSpace( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  bool ret = true;

  for( u32 i=0; i<mwk->inputSizeMax; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }

    if( mwk->str[i] != STRCODE_SPACE &&
      mwk->str[i] != STRCODE_SPACE_HALF )
    {
        ret = false;
        break;
    }
  }

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモン生成用の乱数調整
 *
 * @param   val    乱数発生回数
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::SetRand( u32 val )
{
#if PM_DEBUG
  if( m_MainWork.prm->type != TYPE_CHARACTER_NAME ||
    m_MainWork.prm->type == TYPE_CHARACTER_NAME_DEBUG )  // 人物名デバッグ
#else
  if( m_MainWork.prm->type != TYPE_CHARACTER_NAME )
#endif  // PM_DEBUG
  {
    return;
  }

  for( u32 i=0; i<val; i++ )
  {
    System::GflUse::GetPokemonCreateRand();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期設定文字列をチェック
 *
 * @param   none
 *
 * @return  none
 *
 * @li  不正な文字コードがある場合は入力バッファを初期化
 */
//------------------------------------------------------------------------------
void MainSystem::CheckInitStrCode( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  u32  str_flg = 0;
  u32  str_max = 0;

  // フラグON
  for( u32 i=0; i<mwk->inputSizeMax; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
    str_flg |= (1<<i);
    str_max++;
  }

  if( str_max == 0 )
  {
    return;
  }
  //  GFL_PRINT( "◆◆◆　max = %d\n", str_max );

  switch( mwk->prm->language )
  {
  case POKEMON_LANG_NULL:
    GFL_ASSERT( 0 );
  case POKEMON_LANG_JAPAN:
  case POKEMON_LANG_CHINA:   //!< 言語コード：簡体字（中国）
  case POKEMON_LANG_TAIWAN:  //!< 言語コード：繁体字（台湾、香港）
    {
      static const u32 tbl[] = {
        INPUT_TYPE_HIRAGANA,  // かな
        INPUT_TYPE_KATAKANA,  // カナ
        INPUT_TYPE_ABC,        // ABC
        INPUT_TYPE_MARK,      // 記号
      };
      CheckErrorStrCode( tbl, GFL_NELEMS(tbl), mwk->str, str_max, &str_flg );
    }
    break;

  case POKEMON_LANG_KOREA:
    {
      for( u32 i=0; i<str_max; i++ )
      {
        if( hangul::IsHangul(mwk->str[i]) == true )
        {
          str_flg ^= (1<<i);
        }
      }
      static const u32 tbl[] = {
        INPUT_TYPE_ABC,        // ABC
        INPUT_TYPE_MARK,      // 記号
      };
      CheckErrorStrCode( tbl, GFL_NELEMS(tbl), mwk->str, str_max, &str_flg );
    }
    break;

  default:
    {
      static const u32 tbl[] = {
        INPUT_TYPE_CAPITAL,  // 大文字
        INPUT_TYPE_SMALL,    // 小文字
        INPUT_TYPE_MARK,    // 記号
      };
      CheckErrorStrCode( tbl, GFL_NELEMS(tbl), mwk->str, str_max, &str_flg );
    }
    break;
  }

  //  GFL_PRINT( "◆◆◆　flg = %d\n", str_flg );

  if( str_flg != 0 )
  {
    for( u32 i=0; i<mwk->inputSizeMax; i++ )
    {
      mwk->str[i] = gfl2::str::EOM_CODE;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   文字列の不正チェック
 *
 * @param   type_tbl    入力タイプテーブル
 * @param   type_max    入力タイプテーブルデータ数
 * @param   str          文字列
 * @param   str_max      文字数
 * @param   flg          チェックする文字ビット
 *
 * @return  none
 *
 * @li  不正な文字がない場合は"*flg = 0"になる
 */
//------------------------------------------------------------------------------
void MainSystem::CheckErrorStrCode( const u32 * type_tbl, u32 type_max, const gfl2::str::STRCODE * str, u32 str_max, u32 * flg )
{
  SUB_WORK * swk = m_SubWork;

  if( *flg == 0 )
  {
    return;
  }

  for( u32 i=0; i<str_max; i++ )
  {
    if( ( (*flg) & (1<<i) ) == 0 )
    {
      continue;
    }

    for( u32 j=0; j<type_max; j++ )
    {
      for( u32 k=0; k<INPUT_CODE_MAX_AIUEO; k++ )
      {
        if( str[i] == swk->keyMap->GetKeyCode( INPUT_MODE_AIUEO, type_tbl[j], 0, k ) )
        {
          *flg ^= (1<<i);
          break;
        }
        else if( swk->keyMap->GetLoopCode( str[i], swk->tmp ) == true )
        {
          *flg ^= (1<<i);
          break;
        }
      }
      
      if( ( (*flg) & (1<<i) ) == 0 )
      {
        break;
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力確定
 *
 * @param   none
 *
 * @retval  "true = 確定した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::FixInput( void )
{
  u32  lang = m_MainWork.prm->language;

  if( lang == POKEMON_LANG_JAPAN || lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN )
  {
    return FixInputRomaji();
  }
  else if( lang == POKEMON_LANG_KOREA )
  {
    return FixInputHangul();
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力コード削除
 *
 * @param   none
 *
 * @retval  "true = 削除した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::DeleteCode( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  bool ret = false;

  if( mwk->inputSiz == 0 )
  {
    if( mwk->inputPos != 0 )
    {
      mwk->inputPos--;
      SetInputAreaCode( gfl2::str::EOM_CODE );
      swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
      ret = true;
    }
  }
  else {
    SetInputAreaCode( gfl2::str::EOM_CODE );
    mwk->inputSiz = 0;
    swk->pGraWork->SetOverwriteCursorVisible( false );
    ret = true;
  }

  if( ret == true )
  {
    swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
    PutInputCode();
  }

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力エリアの文字色を初期化
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::InitInputColor( void )
{
  SUB_WORK * swk = m_SubWork;

  for( int i=0; i<INPUT_CODE_MAX; i++ )
  {
    swk->pGraWork->SetInputStrColor( GraphicWork::LAYOUT_ID_LOWER_INPUT, i, GraphicWork::INPUT_COLOR_MODE_NORMAL );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ハングル入力
 *
 * @param   id  入力位置
 *
 * @retval  "true = 入力した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::SetHangleInput( u32 id )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( mwk->inputPos == mwk->inputSizeMax )
  {
    return false;
  }

  gfl2::str::STRCODE code = GetKeyCode( mwk->inputMode, id );

  // 字母が入力された
  if( hangul::IsKSJamo(code) == true )
  {
    gfl2::str::STRCODE tmp;
    bool is_last_code = false;
    if( mwk->inputPos == (mwk->inputSizeMax-1) )
    {
      is_last_code = true;
    }
    // 入力できた
    if( swk->pHangulComposer->InputCode(code,&tmp,is_last_code) == true )
    {
      // 確定
      if( tmp != 0 )
      {
        if( mwk->inputSiz == 0 )
        {
          SetInputCode( tmp );
        }
        else {
          SetInputAreaCode( tmp );
        }
        mwk->inputPos++;

        mwk->inputSiz = 0;
        swk->pGraWork->SetOverwriteCursorVisible( false );
      }
      // 入力できなかった
    }
    else {
      return false;
    }
    // 字母以外が入力された
  }
  else {
    // 未確定文字があるか
    if( swk->pHangulComposer->HasCompositionChar() == true )
    {
      gfl2::str::STRCODE tmp = swk->pHangulComposer->FixComposition();
      if( mwk->inputSiz == 0 )
      {
        SetInputCode( tmp );
      }
      else {
        SetInputAreaCode( tmp );
      }
      mwk->inputSiz = 0;
      mwk->inputPos++;
    }
    // 入力されたものを加える
    if( mwk->inputPos < mwk->inputSizeMax )
    {
      if( mwk->inputSiz == 0 )
      {
        SetInputCode( code );
      }
      else {
        SetInputAreaCode( code );
      }
      mwk->inputPos++;
    }
    mwk->inputSiz = 0;
    swk->pGraWork->SetOverwriteCursorVisible( false );
  }

  swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );

  // オーバーしたときのチェック
  if( mwk->inputPos >= mwk->inputSizeMax )
  {
    mwk->inputPos = mwk->inputSizeMax;
    if( swk->pHangulComposer->HasCompositionChar() == true )
    {
      swk->pHangulComposer->FixComposition();
    }
    MoveCursorEndPos();
  }

  PutHangul();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ハングル削除
 *
 * @param   none
 *
 * @retval  "true = 削除した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::DeleteCodeHangul( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  bool ret = false;

  // 未確定文字がある
  if( swk->pHangulComposer->HasCompositionChar() == true )
  {
    swk->pHangulComposer->BackSpace();
    InitInputColor();
    ret = true;
  }
  else {
    if( mwk->inputSiz == 0 )
    {
      if( mwk->inputPos != 0 )
      {
        mwk->inputPos--;
        SetInputAreaCode( gfl2::str::EOM_CODE );
        swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
        ret = true;
      }
    }
    else {
      SetInputAreaCode( gfl2::str::EOM_CODE );
      mwk->inputSiz = 0;
      swk->pGraWork->SetOverwriteCursorVisible( false );
      ret = true;
    }
  }

  if( ret == true )
  {
    swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
    PutHangul();
  }
  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力したハングルを表示
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void MainSystem::PutHangul( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  u32  i;

  // 確定文字表示
  for( i=0; i<mwk->inputSizeMax; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
    swk->pGraWork->SetInputStrColor( GraphicWork::LAYOUT_ID_LOWER_INPUT, i, GraphicWork::INPUT_COLOR_MODE_NORMAL );
    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
  }
  // 未確定文字表示
  if( swk->pHangulComposer->HasCompositionChar() == true )
  {
    gfl2::str::STRCODE tmp = swk->pHangulComposer->GetCompositionChar();
    u32  pos = mwk->inputPos;
    if( pos == i )
    {
      i++;
    }
    swk->pGraWork->SetInputStrColor( GraphicWork::LAYOUT_ID_LOWER_INPUT, pos, GraphicWork::INPUT_COLOR_MODE_ROMAJI );
    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &tmp, pos, &m_Heap );
  }
  // 空きをクリア
  for( i=i; i<mwk->inputSizeMax; i++ )
  {
    swk->pGraWork->ClearInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, i );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   結合中ハングル（未確定文字）があるか
 *
 * @param   none
 *
 * @retval  "true = ある"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::HasCompositionHangulChar( void )
{
  SUB_WORK * swk = m_SubWork;

  if( m_MainWork.prm->language != POKEMON_LANG_KOREA )
  {
    return false;
  }

  return swk->pHangulComposer->HasCompositionChar();
}

//------------------------------------------------------------------------------
/**
 * @brief   ハングル入力確定
 *
 * @param   none
 *
 * @retval  "true = 確定した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool MainSystem::FixInputHangul( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  if( swk->pHangulComposer->HasCompositionChar() == false )
  {
    return false;
  }

  gfl2::str::STRCODE code = swk->pHangulComposer->FixComposition();

  if( mwk->inputSiz == 0 )
  {
    mwk->inputPos += SetInputCode( &code, 1 );
  }
  else {
    mwk->inputPos += SetInputAreaCode( &code, 1 );
  }

  // 描画
  swk->pGraWork->InitInputField();
  for( int i=0; i<INPUT_CODE_MAX; i++ )
  {
    if( mwk->str[i] == gfl2::str::EOM_CODE )
    {
      break;
    }
    swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
  }
  // 文字列の色を戻す
  InitInputColor();

  swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, mwk->inputPos );
  mwk->inputSiz = 0;
  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
  swk->pGraWork->SetOverwriteCursorVisible( false );
  
  if( mwk->inputPos == mwk->inputSizeMax )
  {
    MoveCursorEndPos();
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：初期化
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_INIT
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_Init( void )
{
  //CreateSubWork();  // MainSeq_FileLoadに移動

  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  gfl2::lyt::LytWk * lytwk;
  gfl2::lyt::LytMultiResID resID = swk->pGraWork->GetLayoutResourceID(0);

  // セーブデータに記憶している入力モードの取得
  mwk->prm->pSaveMisc->GetStrInputParam( &mwk->inputMode );

  InitLang();   //!< 言語別設定

  CheckInitStrCode();

  SetUiData( mwk->inputMode );

  lytwk = swk->pGraWork->GetLayoutWork(GraphicWork::LAYOUT_ID_LOWER_INPUT);
  swk->ui->CreateInputArea( &m_Heap, lytwk, resID, mwk->str, mwk->inputSizeMax );

  swk->pGraWork->InitInputField();
  swk->pGraWork->PutInputBar( mwk->inputSizeMax );

  {
    u16  start, end;
    swk->ui->GetInputAreaPos( &start, &end );
    mwk->inputPos = end;
    swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, start );
    for( int i=0; i<end; i++ ){
      swk->pGraWork->PutInputStr( GraphicWork::LAYOUT_ID_LOWER_INPUT, &mwk->str[i], i, &m_Heap );
    }
  }

  swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
  swk->pGraWork->PutUpperMessage( mwk->prm->type, mwk->prm->param, &m_Heap );
  swk->pGraWork->PutKeyBoard( mwk->inputMode );
  PutKeyMap( mwk->inputMode );
  MoveCursor( mwk->inputMode, 0 );

  int next_seq = MAINSEQ_START;
  bool iconFlag = false;
  bool ugcFlag = false;

  switch( mwk->prm->type )
  {
  case TYPE_CHARACTER_NAME:  // 人物名
  case TYPE_NICKNAME:        // ニックネーム
#if PM_DEBUG
  case TYPE_CHARACTER_NAME_DEBUG:  // 人物名デバッグ
#endif
    swk->pGraWork->CreatePlayerIcon( mwk->pFileReadManager, &m_Heap );
    swk->pGraWork->CreatePlayerIconTexRequest( mwk->pMyStatus, &m_Heap );
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = false;
    next_seq = MAINSEQ_WAIT_LOAD_PLAYERICON;
    break;

  case TYPE_POKEMON_NAME:    // ポケモン名
    swk->pGraWork->CreatePokeIcon( &m_Heap );
    swk->pGraWork->PutPokeIcon( mwk->prm->cp );
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = true;
    break;

  case TYPE_BOX_NAME:        // ボックス名
    swk->pGraWork->PutBoxIcon();
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = true;
    break;

  case TYPE_FREE_WORD_PSS:  // 一言メッセージ ( PSS )
  case TYPE_FREE_WORD_GTS:  // 一言メッセージ ( GTS )
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_16;
    iconFlag = false;
    ugcFlag = true;
    break;

  case TYPE_POKE_SEARCH:    // GTSポケモン検索
  case TYPE_POKE_WANT:      // GTSポケモン検索（欲しいポケモン）
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = false;
    break;

  case TYPE_HAPPY_SIGN:      // ハッピーサイン
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_16;
    iconFlag = false;
    ugcFlag = true;
    break;

  case TYPE_INTERVIEW_LONG:        // 一言メッセージ ( インタビュー: 16文字 )
  case TYPE_INTERVIEW_SHORT:      // 一言メッセージ ( インタビュー: 8文字 )
  case TYPE_INTERVIEW_VERY_SHORT:  // 一言メッセージ ( インタビュー: 6文字 )
  case TYPE_BASE_WORD:            // 秘密基地：団名と合言葉
  case TYPE_BASE_HAPPY:            // 秘密基地：嬉しいときの言葉
  case TYPE_BASE_CHEER:            // 秘密基地：励ますときの言葉
  case TYPE_BASE_GREETING:        // 秘密基地：挨拶
  case TYPE_BASE_GOODS:            // 秘密基地：グッズ
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_16;
    iconFlag = false;
    ugcFlag = true;
    break;

  case TYPE_TEAM_NAME:      // チーム名
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = false;
    break;

  case TYPE_FESCIRCLE_NAME:   // フェスサークル名
    swk->startAnm = GraphicWork::ANMID_INPUT_TYPE_12;
    iconFlag = false;
    break;
  }

  swk->pGraWork->SetStartAnime( swk->startAnm, iconFlag );
  swk->pGraWork->PutUgcMessage( ugcFlag );

  return next_seq;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：解放
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_RELEASE
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_Release( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  swk->pGraWork->DeletePokeIcon();
  swk->pGraWork->DeletePlayerIcon();

  swk->ui->DeleteInputArea();
  swk->ui->DeleteUiData();

  DeleteSubWork();

  // セーブデータセット
  mwk->prm->pSaveMisc->SetStrInputParam( mwk->inputMode );

  return mwk->nextSeq;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：フェード待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_FADE
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_Fade( void )
{
  gfl2::Fade::FadeManager* pFadeMan = m_MainWork.pFadeManager;

  if( pFadeMan->IsEnd( gfl2::Fade::DISP_UPPER ) && pFadeMan->IsEnd( gfl2::Fade::DISP_LOWER ) )
  {
    return m_MainWork.fadeNextSeq;
  }

  return MAINSEQ_FADE;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：開始待ち処理
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_START
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_Start( void )
{
  SUB_WORK * swk = m_SubWork;

  if( swk->pGraWork->CheckStartAnime( swk->startAnm ) == false )
  {
    return SetFadeIn( MAINSEQ_MAIN );
  }

  return MAINSEQ_START;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：メイン処理
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_MAIN
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_Main( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  u32  ret = swk->ui->Main();

  if( swk->errPutFlag == true )
  {
    if( ret != UI::RET_NONE )
    {
      swk->pGraWork->PutUpperMessage( mwk->prm->type, mwk->prm->param, &m_Heap );
      swk->errPutFlag = false;
    }
  }

  switch( ret )
  {
  case UI::RET_NONE:
    break;

  case UI::RET_CURSOR_MOVE:
    Sound::PlaySE( STR_INPUT_SE_CURSOR_MOVE );
    MoveCursor( mwk->inputMode, swk->ui->GetCursorPos() );
    break;

  case UI::RET_CURSOR_MOVE_TOUCH:
    {
      u32  tp = swk->ui->GetButtonPosID( swk->ui->GetTouchID(UI::TP_GET_MODE_HOLD) );
      if( swk->ui->SetCursorPos( tp ) == true )
      {
        MoveCursor( mwk->inputMode, tp );
      }
    }
    break;

  case UI::RET_END_JUMP:
    Sound::PlaySE( STR_INPUT_SE_CURSOR_MOVE );
    MoveCursorEndPos();
    break;

  case UI::RET_CHG_INCREMENT:
    if( IncInputType() == true )
    {
      Sound::PlaySE( STR_INPUT_SE_DECIDE );
    }
    break;

  case UI::RET_INPUT:
    if( !( CheckFixRomaji() == true || HasCompositionHangulChar() == true ) )
    {
      u16  start, end;
      swk->ui->GetInputAreaPos( &start, &end );
      if( start > end )
      {
        u16 tmp = start;
        start = end;
        end = tmp;
      }

      if( start <= GetEndInputPos() )
      {
        mwk->inputPos = start;
        //mwk->inputSiz = swk->ui->GetInputAreaSize();
        mwk->inputSiz = static_cast<u16>(swk->ui->GetInputAreaSize());
        swk->pGraWork->SetInputBarAnime( mwk->inputPos, mwk->inputSiz );
        swk->pGraWork->PutInputCursor( GraphicWork::LAYOUT_ID_LOWER_INPUT, start );
        swk->pGraWork->PutInputSize( GraphicWork::LAYOUT_ID_LOWER_INPUT, PANENAME_STRINPUT_INP_LOW_000_PANE_CURSOR_02, start, mwk->inputSiz );
      }
    }
    break;

  case UI::RET_INPUT_TOUCH:
    break;

  case UI::RET_CHG_CODE:
    if( SetLoopCode() == true )
    {
      Sound::PlaySE( STR_INPUT_SE_DECIDE );
    }
    break;

  case UI::RET_SCROLL_TOUCH:
    //    swk->pGraWork->SetArrowAutoAnime( false );
    break;

  case UI::RET_L_SCROLL_KEY:
    if( mwk->inputMode == INPUT_MODE_AIUEO )
    {
      break;
    }
    swk->pGraWork->SetArrowSelectAnime( 0 );
  case UI::RET_L_SCROLL:
    Sound::PlaySE( STR_INPUT_SE_PAGE_CHANGE );
    FixInput();
    ChangeKeyBoardMode();
    //    swk->pGraWork->SetArrowAutoAnime( true );
    break;

  case UI::RET_R_SCROLL_KEY:
    if( mwk->inputMode == INPUT_MODE_KEYBOARD )
    {
      break;
    }
    swk->pGraWork->SetArrowSelectAnime( 1 );
  case UI::RET_R_SCROLL:
    Sound::PlaySE( STR_INPUT_SE_PAGE_CHANGE );
    FixInput();
    ChangeKeyBoardMode();
    //    swk->pGraWork->SetArrowAutoAnime( true );
    break;

  case UI::RET_DECIDE:
    swk->pGraWork->StartKeySelectAnime( mwk->inputMode, swk->ui->GetCursorPosAnmOffset() );
    return UpdateDecide( swk->ui->GetDecideID() );

  case UI::RET_ICON:
    Sound::PlaySE( STR_INPUT_SE_DECIDE );
    SetDefaultInput();
    break;

  default:
    if( ret > UI::RET_KEYBOARD )
    {
      //SetRand( ret - UI::RET_KEYBOARD + 1 );  // 0オリジンなので+1
    }
    return UpdateDecide( ret );
  }

  return MAINSEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：プレイヤーアイコン読み込み待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_WAIT_LOAD_PLAYERICON
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_WaitLoadPlayerIcon( void )
{
  SUB_WORK * swk = m_SubWork;

  if( swk->pGraWork->CheckCreatePlayerIconTex() == true )
  {
    return SetFadeIn( MAINSEQ_MAIN );
  }

  return MAINSEQ_WAIT_LOAD_PLAYERICON;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：NGワードチェック
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_NG_WORD_CHECK
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_NgWordCheck( void )
{
  MAIN_WORK * mwk = &m_MainWork;
  SUB_WORK * swk = m_SubWork;

  gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();

  switch( mwk->subSeq )
  {
  case 0:
    swk->ngWordLangCount = 0;
    mwk->subSeq = 1;

  case 1:    // 他言語ポケモン名のチェック開始
    {
      static const u32 tbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
      };
      if( swk->ngWordLangCount == GFL_NELEMS(tbl) )
      {
        mwk->subSeq = 5;
        break;
      }
      if( tbl[swk->ngWordLangCount] == mwk->prm->language )
      {
        swk->ngWordLangCount++;
        if( swk->ngWordLangCount == GFL_NELEMS(tbl) )
        {
          mwk->subSeq = 5;
          break;
        }
      }

      //swk->pGraWork->StartLoadPokeNameMsgData(
      //  tbl[swk->ngWordLangCount], mwk->pFileReadManager, pHeap );
      swk->pGraWork->StartOpenPokeNameMsgData( tbl[swk->ngWordLangCount], pHeap );
      mwk->subSeq = 2;
    }
    break;

  case 2:
    {
      if( !swk->pGraWork->WaitOpenPokeNameMsgData() ) break;

      swk->pGraWork->StartLoadPokeNameMsgData( pHeap );
      mwk->subSeq = 3;
    } break;

  case 3:
    {
      if( !swk->pGraWork->WaitLoadPokeNameMsgData( pHeap ) ) break;

      swk->pGraWork->StartClosePokeNameMsgData( pHeap );
      mwk->subSeq = 4;
    } break;

  case 4:    // 他言語ポケモン名のチェック
    if( swk->pGraWork->WaitClosePokeNameMsgData() )
    //if( swk->pGraWork->WaitLoadPokeNameMsgData( mwk->pFileReadManager, pHeap ) == true )
    {
      if( swk->pGraWork->CheckPokemonNameLocalize( mwk->str ) == true )
      {
        mwk->subSeq = 7;  // 終了へ
        GFL_PRINT( "他言語のポケモン名\n" );
      }
      else {
        swk->ngWordLangCount++;
        mwk->subSeq = 1;
      }
      swk->pGraWork->EndLoadPokeNameMsgData();
    }
    break;

  case 5:    // NGワードチェック開始
    GFL_PRINT( "NGワードチェック開始\n" );
    if( InitNgWordCheck() == false )
    {
      break;
    }
    swk->pGraWork->SetVisibleTimeIcon( true );
    mwk->subSeq = 6;

  case 6:    // NGワードチェック
    switch( swk->ngWordResult )
    {
    case WORD_CHECK_TRUE:          // ok
      mwk->subSeq = 7;
      break;

    case WORD_CHECK_NG:            // NGワード
      swk->pGraWork->SetVisibleTimeIcon( false );
      EndNgWordCheck();
      Sound::PlaySE( STR_INPUT_SE_ERROR );
      swk->errPutFlag = true;
      swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_NG_WORD, &m_Heap );
      mwk->subSeq = 0;
      return MAINSEQ_MAIN;

    case WORD_CHECK_NUMBER_OVER:  // 数値入力エラー
      swk->pGraWork->SetVisibleTimeIcon( false );
      EndNgWordCheck();
      Sound::PlaySE( STR_INPUT_SE_ERROR );
      swk->errPutFlag = true;
      swk->pGraWork->PutErrorMessage( GraphicWork::ERR_ID_NUMBER, &m_Heap );
      mwk->subSeq = 0;
      return MAINSEQ_MAIN;

    default:
      break;
    }
    break;

  case 7:    // 正常終了
    swk->pGraWork->SetVisibleTimeIcon( false );
    EndNgWordCheck();
    return SetEnd();
  }

  return MAINSEQ_NG_WORD_CHECK;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：レイアウトの読み込み待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_LAYOUT_LOAD
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_LayoutLoad( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  switch( mwk->subSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      app::util::FileAccessor::FileOpen( ARCID_STRINPUT, pHeap->GetLowerHandle() );
      mwk->subSeq++;
    }

  case 1:
    {
      if( !app::util::FileAccessor::IsFileOpen( ARCID_STRINPUT ) ) break;

      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      app::util::FileAccessor::FileLoad(
        ARCID_STRINPUT,
        GARC_strinput_strinput_applyt_COMP,
        &m_pArcReadBuff,
        pHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      mwk->subSeq++;
    }

  case 2:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      app::util::FileAccessor::FileClose( ARCID_STRINPUT, pHeap->GetLowerHandle() );
      mwk->subSeq++;
    }

  case 3:
    {
      if( !app::util::FileAccessor::IsFileClose( ARCID_STRINPUT ) ) break;

      CreateSubWork();

      mwk->subSeq++;
    } break;

  case 4:
    {
      mwk->subSeq = 0;
      return MAINSEQ_FILE_LOAD;
    }
  }

  return MAINSEQ_LAYOUT_LOAD;
}

//------------------------------------------------------------------------------
/**
* @brief   メインシーケンス：テクスチャの読み込み待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_TEXTURE_LOAD
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_TextureLoad( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  u32 arcID = ARCID_STRINPUT_GRPFONT;

  switch( mwk->subSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase* heap = m_Heap.GetDeviceHeap();
      {
        app::util::FileAccessor::FileOpen( arcID, heap->GetLowerHandle() );
      }
      mwk->subSeq++;
    }

  case 1:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      gfl2::heap::HeapBase* heap = m_Heap.GetDeviceHeap();
      {
        app::util::FileAccessor::FileLoad(
          arcID,
          GARC_StrInputGrpFont_StrInputGrpFont_applyt_COMP,
          &m_pTexReadBuff,
          heap,
          true,
          nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      }

      mwk->subSeq++;
    }

  case 2:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pTexReadBuff ) ) break;

      gfl2::heap::HeapBase* heap = m_Heap.GetDeviceHeap();
      {
        app::util::FileAccessor::FileClose( arcID, heap->GetLowerHandle() );
      }
      mwk->subSeq++;
    }

  case 3:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      GFL_ASSERT( m_pTexReadBuff != NULL );
      mwk->subSeq++;
    } break;

  case 4:
    {
      mwk->subSeq = 0;
      return MAINSEQ_LAYOUT_LOAD;
    }
  }

  return MAINSEQ_TEXTURE_LOAD;
}

//------------------------------------------------------------------------------
/**
 * @brief   メインシーケンス：ファイルの読み込み待ち
 *
 * @param   none
 *
 * @return  次のシーケンス
 *
 *  定義：MAINSEQ_FILE_LOAD
 */
//------------------------------------------------------------------------------
int MainSystem::MainSeq_FileLoad( void )
{
  MAIN_WORK * mwk = &m_MainWork;

  switch( mwk->subSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      app::util::FileAccessor::FileOpen( ARCID_STRINPUT_DATA, pHeap->GetLowerHandle() );
      mwk->subSeq++;
    }

  case 1:
    {
      if( !app::util::FileAccessor::IsFileOpen( ARCID_STRINPUT_DATA ) ) break;

      mwk->subSeq++;
    }

  case 2:
    {
      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      SUB_WORK * swk = m_SubWork;
      swk->keyMap->LoadData( ARCID_STRINPUT_DATA, pHeap, m_DataIndex, m_FileIndex );
      mwk->subSeq++;
    }

  case 3:
    {
      SUB_WORK * swk = m_SubWork;
      if( !swk->keyMap->IsLoadData( m_DataIndex, m_FileIndex ) ) break;

      // 全てのデータを読み込めてないなら読み込みに戻る
      if( !swk->keyMap->IsDataLoadFinished( &m_DataIndex, &m_FileIndex ) )
      {
        mwk->subSeq--;
        break;
      }

      gfl2::heap::HeapBase* pHeap = m_Heap.GetDeviceHeap();
      app::util::FileAccessor::FileClose( ARCID_STRINPUT_DATA, pHeap->GetLowerHandle() );
      mwk->subSeq++;
    }

  case 4:
    {
      if( !app::util::FileAccessor::IsFileClose( ARCID_STRINPUT_DATA ) ) break;
      mwk->subSeq++;
    }

  case 5:
    {
      return MAINSEQ_INIT;
    }
  }

  return MAINSEQ_FILE_LOAD;
}



GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )
