#if PM_DEBUG

//======================================================================
/**
 * @file   ncl_wordset_test.cpp
 * @brief	 WordSetのテスト（特に文法系）
 * @author ichibayashi_hironori@Nintendo
 * @date   2013/01/08
 */
//======================================================================
#include "Debug/DebugWordSet/include/ncl_wordset_test.h"

#include <macro/include/gfl2_macros.h>

// nijiのインクルード
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include "AppLib/include/Util/AppRenderingManager.h"
#include "System/include/ArcIdGetter.h"
#include "System/include/HeapDefine.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "AppLib/include/Util/app_util_heap.h"

#include "GameSys/include/GameData.h"

#include "niji_conv_header/poke_lib/monsno_def.h"
#include "niji_conv_header/message/msg_typename.h" 
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/message/debug/msg_d_ncl.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <arc_index/debug_message.gaix>
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_word_set_input.gaix>

#include "niji_conv_header/app/debug/debug_word_set/res2d/debug_word_set_input.h"
#include "niji_conv_header/app/debug/debug_word_set/res2d/debug_word_set_input_anim_list.h"
#include "niji_conv_header/app/debug/debug_word_set/res2d/debug_word_set_input_pane.h"


GFL_NAMESPACE_BEGIN( test )
GFL_NAMESPACE_BEGIN( ncl )

/** テスト用UIView */
class WordsetTestView : public app::ui::UIView, public app::ui::UIInputListener
{
public:
  WordsetTestView( app::util::Heap * heap, app::util::AppRenderingManager* pRenderingManager, void* pLytBuff ) :
    UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  {
    GFL_ASSERT( pLytBuff );
    GFL_ASSERT( heap );

    m_pHeap = heap;

    //  レイアウト作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[] =
      {
          //  選択画面
          {
            0,
            LYTRES_DEBUG_WORD_SET_INPUT_WORDSET_TEST_BFLYT,
            LA_DEBUG_WORD_SET_INPUT_WORDSET_TEST___NUM,
            pLytBuff,
            true,
            app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      //  レイアウト作成
      UIView::Create2D( heap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), UIView::UNUSE_MESSAGE_DATA );
    }

    //  入力レスポンス受け取る設定
    UIResponder::SetInputListener( this );

    GFL_ASSERT( pRenderingManager );
    m_pAppRenderingManager  = pRenderingManager;

    m_bProcEnd  = false;
  }

  virtual ~WordsetTestView()
  {
    UIView::Delete2D();
  }

  virtual void Update(void)
  {
    UIView::Update();
  }

  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    // ここですべて描画
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, 0 );
  }

  gfl2::lyt::LytWk* GetLayoutWork()
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    return  pG2DUtil->GetLayoutWork( 0 );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
  {
    GFL_UNUSED( pTouchPanel );

    GFL_PRINT( "OnTouchEvent\n" );
    return ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   タッチパネルのダブルクリックイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnDoubleClickEvent( gfl2::ui::TouchPanel * pTouchPanel )
  {
    GFL_UNUSED( pTouchPanel );

    GFL_PRINT( "OnDoubleClickEvent\n" );
    return ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId )
  {
    GFL_PRINT( "OnLayoutPaneEvent : painId = %d\n", painId );
    return ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  キー入力の検知
  *         ※タッチパネルの入力が優先される
  *
  * @param  pKey    十字キー
  * @param  pStick  アナログスティック
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( pKey->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      m_bProcEnd  = true;
    }
    else
    if( pKey->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
    }

    return ENABLE_ACCESS;
  }

  bool IsProcEnd() const { return m_bProcEnd; }

private:
  bool  m_bProcEnd;

  app::util::Heap*  m_pHeap;
  app::util::AppRenderingManager* m_pAppRenderingManager;
};

WordsetTest::WordsetTest( void ) : GameProc()
{
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result WordsetTest::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );
  
  gfl2::heap::HeapBase* device_memory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  mDeviceMem = GFL_CREATE_LOCAL_HEAP( device_memory, 1024 * 1024 * 16, gfl2::heap::HEAP_TYPE_EXP, false );
  
  mDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  GFL_ASSERT( mDeviceManager );

  // ローカルヒープ生成
  {
    mUtilHeap = GFL_NEW( mDeviceMem ) app::util::Heap();
    mUtilHeap->LocalHeapCreate( mDeviceMem, mDeviceMem, 1024 * 1024, 1024 * 1024 * 14 );
  }

  {
    //自分のパイプラインを作成
    mpAppRenderingManager  = GFL_NEW(mUtilHeap->GetSystemHeap()) app::util::AppRenderingManager();

    {
      app::util::AppRenderingManager::Description appRenderingManagerDesc;

      //  下画面はレイアウトタイプ設定(派生先で内容を変えることができる)
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

      //  上画面はレイアウトタイプ設定(派生先で内容を変えることができる)
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;

      mpAppRenderingManager->SyncCreate(
          appRenderingManagerDesc,
          mUtilHeap,
          NULL,
          GameSys::GameProc::GetGameManager()->GetAsyncFileManager()
      );
    }
  }

  //  下画面View作成
  {
     // レイアウトファイル同期読み
     app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_WORD_SET, mDeviceMem );

     app::util::FileAccessor::FileLoadSync( ARCID_DEBUG_WORD_SET, GARC_debug_word_set_input_debug_word_set_input_APPLYT, &mLytBuf, mDeviceMem, false, 128 );

     app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_WORD_SET );

     // レイアウトView生成
     mUIView  = GFL_NEW( mDeviceMem ) WordsetTestView( mUtilHeap, mpAppRenderingManager, mLytBuf );
  }
    
  //  wordset作成
  {
    mWordSet = GFL_NEW(mDeviceMem) print::WordSet( mDeviceMem );

    mMsgDataItem  = GFL_NEW(mDeviceMem) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_itemname_wordset_DAT, mDeviceMem );
    mMsgDataBag   = GFL_NEW(mDeviceMem) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_bag_DAT, mDeviceMem );
    mMsgData      = GFL_NEW(mDeviceMem) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_ncl_DAT, mDeviceMem );

    mStrBuf = GFL_NEW(mDeviceMem) gfl2::str::StrBuf( 2048, mDeviceMem );
    mExpandBuf = GFL_NEW(mDeviceMem) gfl2::str::StrBuf( 2048, mDeviceMem );
    mLytTextWork = print::CreateLytTextExWork( mDeviceMem );
  }

  mIsUpdateMessage = false;
  mWordIndex = 1;
  mTestType = TESTTYPE_ITEMNAME;
  updateText();

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result WordsetTest::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );
 
  if( mUIView->IsDrawing() == false )
  {
    print::DeleteLytTextExWork( mLytTextWork );
    GFL_SAFE_DELETE( mStrBuf );
    GFL_SAFE_DELETE( mExpandBuf );
    GFL_SAFE_DELETE( mMsgData );
    GFL_SAFE_DELETE( mMsgDataItem );
    GFL_SAFE_DELETE( mMsgDataBag );
    GFL_SAFE_DELETE( mWordSet );
  
    GFL_SAFE_DELETE( mUIView );
    GFL_SAFE_DELETE( mpAppRenderingManager );
    GFL_SAFE_DELETE( mLytBuf );

    GFL_SAFE_DELETE( mUtilHeap );

    if( mDeviceMem )
    {
      GFL_DELETE_HEAP(mDeviceMem);
      mDeviceMem = NULL;
    }

    return gfl2::proc::RES_FINISH;
  }

  return  gfl2::proc::RES_CONTINUE;
}

int WordsetTest::between(int value, int minValue, int maxValue)
{
  return gfl2::math::Max(minValue, gfl2::math::Min(maxValue, value));
}

void WordsetTest::updateText()
{
  gfl2::lyt::LytWk* pLytWk  = mUIView->GetLayoutWork();

  print::SetStringForLytTextBoxEx(pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_GENDER)      , L"", mLytTextWork);
  print::SetStringForLytTextBoxEx(pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_COUNTABILITY), L"", mLytTextWork);
  print::SetStringForLytTextBoxEx(pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_INITIALSOUND), L"", mLytTextWork);

  // テスト番号
  {
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_TESTINDEX);
    print::StrNumber( mExpandBuf, mTestType, 2, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT);
    print::SetStringForLytTextBoxEx(textBox, mExpandBuf, mLytTextWork);
  }

  // 単語番号
  {
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_WORDINDEX);
    print::StrNumber( mExpandBuf, mWordIndex, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT);
    print::SetStringForLytTextBoxEx(textBox, mExpandBuf, mLytTextWork);
  }

  // expand
  {
    registerWords();
    mMsgData->GetString(WORDSETTEST_ITEMNAME + mTestType, *mStrBuf);
    mWordSet->Expand(mExpandBuf, mStrBuf);
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_EXPANDEDTEXT);
    print::SetStringForLytTextBoxEx(textBox, mExpandBuf, mLytTextWork);
  }

  //性
  {
    mMsgData->GetString(WORDSETTEST_GENDER, *mStrBuf);
    mWordSet->Expand(mExpandBuf, mStrBuf);
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_GENDER);
    print::SetStringForLytTextBoxEx(textBox, mExpandBuf, mLytTextWork);
  }
}

// 有効な単語番号の最大値
int WordsetTest::testWordMax[WordsetTest::TESTTYPE_COUNT] = 
{
  ITEM_DATA_MAX,          //TESTTYPE_ITEMNAME,
  trainer::TRTYPE_MAX-1,  //TESTTYPE_TRTYPEANDNAME,
  trainer::TRTYPE_MAX-1,  //TESTTYPE_TRTYPENAME,
  MONSNO_END,             //TESTTYPE_POKEMONNAME,

  msg_typename_max-1,                         //TESTTYPE_TYPENAME,
                            
  ITEM_DATA_MAX,          //TESTTYPE_ARTICLE_01,
  ITEM_DATA_MAX,          //TESTTYPE_ARTICLE_02,
  ITEM_DATA_MAX,          //TESTTYPE_ARTICLE_03,
  ITEM_DATA_MAX,          //TESTTYPE_ARTICLE_04,
  ITEM_DATA_MAX,          //TESTTYPE_ARTICLE_05,
  INT_MAX,                //TESTTYPE_RIVALNAME,
  INT_MAX,                //TESTTYPE_DIGITS,
};

void WordsetTest::registerWords()
{
  gfl2::lyt::LytWk* pLytWk  = mUIView->GetLayoutWork();

  switch (mTestType)
  {
  case TESTTYPE_ITEMNAME:
  case TESTTYPE_ARTICLE_01:
  case TESTTYPE_ARTICLE_02:
  case TESTTYPE_ARTICLE_03:
  case TESTTYPE_ARTICLE_04:
  case TESTTYPE_ARTICLE_05:
    registerWords_ItemName();
    break;
  case TESTTYPE_TRTYPEANDNAME:
    registerWords_TrTypeAndName();
    break;
  case TESTTYPE_TRTYPENAME:
    mWordSet->RegisterTrTypeName(0, mWordIndex);
    break;
  case TESTTYPE_POKEMONNAME:
    mWordSet->RegisterPokeMonsNameNo(0, static_cast<MonsNo>(mWordIndex));
    break;
  case TESTTYPE_TYPENAME:
    mWordSet->RegisterPokeTypeName(0, mWordIndex);
    break;
  case TESTTYPE_RIVALNAME:
  /*  WordSetクラスで未実装
    mWordSet->RegisterRivalName(0, GFL_SINGLETON_INSTANCE(gamesystem::GameManager)->GetGameData()->GetMisc());
    break;
  */
  case TESTTYPE_DIGITS:    // 桁区切りのテスト
    mWordSet->RegisterNumber(0, 9999999, 7, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT);
    break;
  default:
    GFL_ASSERT_MSG(false, "mTestType");
    break;
  }
}

// バトルの、トレーナータイプ名とトレーナー名の結合
void WordsetTest::registerWords_TrTypeAndName()
{
  mWordSet->RegisterTrTypeName(0, mWordIndex);
  mWordSet->RegisterTrainerName(1, 1);
  u8 lang = System::GetLang();
  if (lang == POKEMON_LANG_FRANCE || lang == POKEMON_LANG_SPAIN)
  {
    mWordSet->RegisterTrTypeNameAndTrainerName(0, 0, 1);
  }
}

// 冠詞タグと強制複数のテスト
void WordsetTest::registerWords_ItemName()
{
  gfl2::lyt::LytWk* pLytWk  = mUIView->GetLayoutWork();

  mWordSet->RegisterItemName(0, mWordIndex, 1);
  mWordSet->RegisterItemName(1, mWordIndex, 2);

  print::GrammaticalAttribute attribute = print::GrammaticalAttribute::FromMsgDataUserParam(mMsgDataItem->GetUserParam(mWordIndex));

  //可算性
  {
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_COUNTABILITY);
    const gfl2::str::STRCODE* text;
    if (attribute.IsAlwaysPlural)
      text = L"Always Plural";
    else if (attribute.IsUncountable)
      text = L"Uncountable";
    else
      text =  L"";
    print::SetStringForLytTextBoxEx(textBox, text, mLytTextWork);
  }
  //最初の音
  {
    gfl2::lyt::LytTextBox* textBox = pLytWk->GetTextBoxPane(PANENAME_WORDSET_TEST_PANE_INITIALSOUND);
    print::StrNumber( mExpandBuf, attribute.InitialSound, 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT);
    print::SetStringForLytTextBoxEx(textBox, mExpandBuf, mLytTextWork);
  }
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメインを行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result WordsetTest::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::Button *uiButton = mDeviceManager->GetButton(0);

  bool isUpdateRequested = false;
  int diffValue = 1;
  //  データリストを進める移動量をボタンを押している間は変える
  if (uiButton->IsHold( gfl2::ui::BUTTON_X ))
  {
    diffValue = 100;
  }
  else if (uiButton->IsHold( gfl2::ui::BUTTON_Y ))
  {
    diffValue = 10;
  }

  //  現在参照しているテキストデータリストを進めまたは戻す
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    mWordIndex -= diffValue;
    isUpdateRequested = true;
  }
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    mWordIndex += diffValue;
    isUpdateRequested = true;
  }

  //  テキストデータを切り替え
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_L) )
  {
    if (mTestType == 0)
      mTestType = static_cast<TestType>(TESTTYPE_COUNT-1);
    else
      mTestType = static_cast<TestType>(static_cast<int>(mTestType) - 1);
    isUpdateRequested = true;
  }
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_R) )
  {
    if (mTestType == TESTTYPE_COUNT-1)
      mTestType = static_cast<TestType>(0);
    else
      mTestType = static_cast<TestType>(static_cast<int>(mTestType) + 1);
    isUpdateRequested = true;
  }

  if (isUpdateRequested)
  {
    mWordIndex = between(mWordIndex, 0, testWordMax[static_cast<int>(mTestType)]);
    
    updateText();
  }

  mUIView->UpdateTree();
  mpAppRenderingManager->Update();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

void WordsetTest::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
  mUIView->DrawTree( displayNo );
  mpAppRenderingManager->Draw( displayNo );
}

GFL_NAMESPACE_END( test )
GFL_NAMESPACE_END( ncl )

#endif // PM_DEBUG
