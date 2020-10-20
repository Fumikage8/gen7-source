//======================================================================
/**
 * @file	gfl2_DebugwinSystem.cpp
 * @brief	デバッグウィンドウ
 * @author	ariizumi
 * @data	11/03/29
 */
//======================================================================
#include <ui/include/gfl2_UI.h>
#include <math/include/gfl2_math.h>
#include <debug/include/gfl2_DebugPrint.h>


#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinLocal.h>
#include <gfx/include/gfl2_DrawUtil.h>

#if defined(GF_PLATFORM_WIN32)
#define DRAW_TEXT_FUNC DrawTextW
#elif defined(GF_PLATFORM_CTR)
#define DRAW_TEXT_FUNC DrawText
#endif

#if defined(GF_PLATFORM_WIN32)
#include <system/include/InputDevice/gfl2_KeyBoard.h>
#include <system/include/InputDevice/gfl2_Mouse.h>
#endif

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_DEBUGWIN_USE
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gfl2::debug::DebugWin)
template class gfl2::base::SingletonAccessor<gfl2::debug::DebugWin>;
#endif
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
const static u32 DEBUG_WIN_LINE_MAX = 14;
#if GFL_DEBUGWIN_USE

void DebugWin_Initialize( gfl2::heap::HeapBase * heap , gfl2::heap::HeapBase * devHeap , gfl2::gfx::IGLAllocator* allocator , gfl2::str::Font *font , int groupNum )
{
  //warn対応。singletonなのでこれでOK。
  //gfl2::debug::DebugWin *debWin = GFL_NEW(heap) gfl2::debug::DebugWin(heap,devHeap,font,groupNum);
  GFL_NEW(heap) gfl2::debug::DebugWin(heap,devHeap,allocator,font,groupNum);
}

void DebugWin_Finalize( void )
{
  gfl2::debug::DebugWin * debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  GFL_DELETE debWin;
}

void DebugWin_Update(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->Update();
}

void DebugWin_Draw( gfl2::gfx::CtrDisplayNo dispNo , gfl2::gfx::DisplayDeviceHandle handle )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->Draw(dispNo  , handle);
}

bool DebugWin_IsOpen(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->IsOpen();
}

bool DebugWin_IsCloseTrigger(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->IsCloseTrigger();
}


void DebugWin_CloseWindow(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->CloseWindow();
}


void DebugWin_ReSetFont(gfl2::str::Font *font)
{
	GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetDrawUtilText()->SetFont( font );
}


//---------------------------------------------------------
//グループ作成
DebugWinGroup* DebugWin_AddGroup( heap::HeapBase *heap , const wchar_t *name , DebugWinGroup *parent , int itemNum )
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddGroup(heap,name,parent,itemNum );
}

DebugWinGroup* DebugWin_AddGroup( heap::HeapBase *heap , str::MsgData *msgData , u32 strId , DebugWinGroup *parent , int itemNum )
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->AddGroup(heap,msgData,strId,parent,itemNum);
}

//グループ削除
void DebugWin_RemoveGroup( DebugWinGroup *group )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->RemoveGroup(group);
}

//グループ削除(名前)
void DebugWin_RemoveGroupByName( const wchar_t *name )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->RemoveGroup(name);
}

//デバイスマネージャーの設定
void DebugWin_SetDeviceManager( gfl2::ui::DeviceManager *devMng , gfl2::ui::Button *btn , gfl2::ui::VectorDevice *key )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetDeviceManager(devMng,btn,key);
}

//コールバック受け取りクラスの設定
void DebugWin_SetCallbackClass( gfl2::debug::DebugWinSystemCallBack *cbClass )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetCallbackClass(cbClass);
}

//テンポラリな文字列領域を取得
wchar_t * DebugWin_GetWorkStr( void )
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStr();
}
str::StrBuf * DebugWin_GetWorkStrBuf(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
}
DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name )
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetGroupByName( name );
}
DebugWinGroup* DebugWin_GetTopGroup(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetTopGroup();
}
void DebugWin_SetCurrentItem( DebugWinItem *selItem )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetCurrentItem(selItem);
}
void DebugWin_SetCurrentItemByIndex( s32 index )
{
  GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetCurrentItemByIndex(index);
}

//取得系(DebugWinで操作できるようにポインタで返す)
gfl2::math::Vector4* DebugWin_GetItemColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetItemColor();
}
gfl2::math::Vector4* DebugWin_GetShadowColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetShadowColor();
}
gfl2::math::Vector4* DebugWin_GetCurrentColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetCurrentColor();
}
gfl2::math::Vector4* DebugWin_GetFillColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetFillColor();
}
f32* DebugWin_GetFontScale(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetFontScale();
}
f32* DebugWin_GetLineHeight(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetLineHeight();
}
int* DebugWin_GetHoldKey(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetHoldKey();
}
int* DebugWin_GetTriggerKey(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetTriggerKey();
}
gfl2::ui::Button* DebugWin_GetUiButton(void)
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetUiButton();
}

static DebugWinItemIt ItemItNull;

//コンストラクタ
DebugWin::DebugWin( heap::HeapBase *heap , gfl2::heap::HeapBase * devHeap , gfl2::gfx::IGLAllocator* allocator , gfl2::str::Font *font , int groupNum )
:mHeap(heap)
,mDevHeap(devHeap)
,mDeviceManager(NULL)
,mUiButton(NULL)
,mUiCrossKey(NULL)
,mHoldKey(gfl2::ui::BUTTON_SELECT)
,mTrgKey(gfl2::ui::BUTTON_SELECT)
,mLockTrgKey(gfl2::ui::BUTTON_START)
,mIsOpen(false)
,mIsCloseTrigger(false)
,mIsBanLock(false)
,mIsLock(false)
,mIsFoceStop(false)
,mGroupList(heap,groupNum)
,mCurrentIdx(0)
,mItemCol(1.0f,1.0f,1.0f,1.0f)
,mShadowCol(0.0f,0.0f,0.0f,1.0f)
,mCurrentCol(1.0f,0.0f,0.0f,1.0f)
,mFillCol(0.2f,0.2f,0.2f,0.8f)
,mFontScale(0.8f)
,mLineHeight(16.0f)
,mScrollOfs(0)
,mCallbackClass(NULL)
,m_ItemViewBufferUpper(3)
,m_ItemViewBufferBottom(3)

{
  //トップグループの作成
  mTopGroup = GFL_NEW(mHeap)DebugWinGroup(NULL,L"DebugWinTop",DEBUGWIN_GROUP_ITEM_NUM,heap); 
  mCurrentGroup = mTopGroup;
  mCurrentItem = ItemItNull;
  
  mWorkStrbuf = GFL_NEW(mHeap)str::StrBuf(DEBUGWIN_NAME_LEN,mHeap);

  //描画用
	// アロケーターの生成
  mHeapAllocator = GFL_NEW(heap) gfl2::heap::NwAllocator( heap );
  mDevAllocator  = GFL_NEW(heap) gfl2::heap::NwAllocator( devHeap );

  mRenderingPipeLine = GFL_NEW( heap ) gfl2::util::UtilTextRenderingPipeLine( allocator , 512, 256 );

  // DrawUtilTextの生成
  mpDrawUtilText = GFL_NEW( heap ) gfl2::util::DrawUtilText(mHeapAllocator, mDevAllocator, gfl2::util::DrawUtilText::StackMax, gfl2::util::DrawUtilText::LINE_CHAR_MAX * DEBUG_WIN_LINE_MAX );
	mpDrawUtilText->Initialize( mHeapAllocator, mDevAllocator, mRenderingPipeLine->GetUtilTextRenderPath() );
	mpDrawUtilText->SetFont( font );


    ItemViewBufferClear();


}

//デストラクタ
DebugWin::~DebugWin()
{
  // @attention 描画終了後1フレーム空けねばならない！
	if( mRenderingPipeLine != NULL )
	{
		mRenderingPipeLine->GetUtilTextRenderPath()->ClearDrawableText();
	}

	// DrawUtilTextの解放
	mpDrawUtilText->Terminate();

	GFL_SAFE_DELETE( mpDrawUtilText );
  GFL_SAFE_DELETE( mRenderingPipeLine );

  // アロケーターの解放
	GFL_SAFE_DELETE( mDevAllocator );
	GFL_SAFE_DELETE( mHeapAllocator );

  GFL_DELETE mWorkStrbuf;
  GFL_DELETE mTopGroup;
}

void DebugWin::Update()
{
  if( mIsFoceStop )
  {
    return;
  }

  if( mIsCloseTrigger )
  {
    mIsCloseTrigger = false;
    SoundCallBack(SE_WINDOW_CLOSE);
  }
  
  if( mDeviceManager != NULL )
  {
    DebugWinItemList *list = mCurrentGroup->GetItemList();

    gfl2::ui::Button *uiButton = GetUiButton();
    gfl2::ui::VectorDevice *uiKey = GetUiCrossKey();
    
    //開閉操作
    if( uiButton->IsHold(mHoldKey) &&
        uiButton->IsTrigger(mTrgKey) )
    {
      mIsOpen = !mIsOpen;
      if( mIsOpen )
      {
        SoundCallBack(SE_WINDOW_OPEN);
        GFL_PRINT("DebugWindow is open!\n");
        //開いたとき、デフォルト位置を設定
        if( mCurrentItem == ItemItNull &&
            list->Size() != 0 )
        {
          mCurrentItem = list->Begin();
          mCurrentIdx = 0;
        }

        if( mCurrentGroup )
        {
          mCurrentGroup->CallWindowOpenGroupFunc();
        }
      }
      else
      {
        GFL_PRINT("DebugWindow is close!\n");
        mIsCloseTrigger = true;
        //別個閉じる操作があるので、処理追加時には注意！
        if( mCurrentGroup )
        {
          mCurrentGroup->CallWindowCloseGroupFunc();
        }
      }
    }
    
    if( mIsOpen )
    {
      // ロック操作
      if( uiButton->IsTrigger(mLockTrgKey) )
      {
        mIsLock = !mIsLock;
      }

      if( mIsBanLock )
      {
        mIsLock = false;
      }

      //開いているとき更新
      if( mIsLock == false )
      {
        if( list->Size() != 0 && 
          mCurrentItem != ItemItNull )
        {
          //項目移動
          if( uiKey->IsRepeat(gfl2::ui::DIR_UP) )
          {
            int move = 1;
            if( uiButton->IsHold(gfl2::ui::BUTTON_R) )
            {
              move = 2;
            }

            DebugWinItemIt startItem = mCurrentItem;
            DebugWinItem *item;
            do
            {
              if( mCurrentItem == list->Begin() )
              {
                mCurrentItem = list->End();
                --mCurrentItem;
                mCurrentIdx = list->Size();
              }
              else
              {
                --mCurrentItem;
                --mCurrentIdx;
              }
              item = (*mCurrentItem);
              move--;
            }
            while( ( startItem != mCurrentItem && item->IsSelectSkip()==true) || move > 0 );
            SoundCallBack(SE_CURSOR_MOVE);
          }
          else
            if( uiKey->IsRepeat(gfl2::ui::DIR_DOWN) )
            {
              int move = 1;
              if( uiButton->IsHold(gfl2::ui::BUTTON_R) )
              {
                move = 2;
              }
              
              DebugWinItemIt startItem = mCurrentItem;
              DebugWinItem *item;
              do
              {
                ++mCurrentItem;
                ++mCurrentIdx;
                if( mCurrentItem == list->End() )
                {
                  mCurrentItem = list->Begin();
                  mCurrentIdx = 0;
                }
                item = (*mCurrentItem);
                move--;
              }
              while( ( startItem != mCurrentItem && item->IsSelectSkip()==true) || move > 0 );
              SoundCallBack(SE_CURSOR_MOVE);
            }

            //項目更新処理
            DebugWinItem *item = (*mCurrentItem);
            item->UpdateFunc(this);

            //スクロールの更新
            {
              //グループ名が入るから+1
              const int dispPos = (mCurrentIdx+1) * mLineHeight + mScrollOfs;

              //文字の下を見るからもう1個分＋

              // 下方向
              int bottomLimit( m_ItemViewBufferBottom );
              if( dispPos+mLineHeight > bottomLimit )
              {
                mScrollOfs = bottomLimit - ((mCurrentIdx+2) * mLineHeight );
              }

              // 上方向
              int upperLimit( m_ItemViewBufferUpper );
              if( dispPos < (upperLimit) )
              {
                mScrollOfs = (upperLimit) - ((mCurrentIdx+1) * mLineHeight);
              }

              if( mScrollOfs > 0 )
              {
                mScrollOfs = 0;
              }

              ItemViewBufferRecover();
            }
        }

        //親へ移動
        if( uiButton->IsTrigger(gfl2::ui::BUTTON_B) )
        {
          DebugWinGroup *parentGrp = mCurrentGroup->GetParentGroup();
          if( parentGrp != NULL )
          {
            mCurrentGroup->CallDestroyGroupFunc();
            SetCurrentGroup( parentGrp , mCurrentGroup );
            SoundCallBack(SE_GROUP_OUT);

            ItemViewBufferClear();
          }
        }

        // タッチ操作
        InputUpdateTouch();

        // キーボード操作
        InputUpdateKeyborad();

      } // if( mIsLock == false )
    }
    else
    {
      
    }
  }

  //描画用の積み込み
  if( mIsOpen )
  {
    //開いているとき更新
    int line = 0;
    int drawLineCount = 0;

    mpDrawUtilText->ClearDrawText();

    //@attention 背景の塗りつぶしはniji側でやっている
    if( mLineHeight*line+1 + mScrollOfs > 0 &&
        mLineHeight*line+1 + mScrollOfs < 240 - mLineHeight && 
        drawLineCount < DEBUG_WIN_LINE_MAX )
    {
      //グループ名の表示
      const wchar_t *dispStr = mCurrentGroup->GetDispName(this);
      mpDrawUtilText->SetTextScale( mFontScale,mFontScale );

      if( mIsLock )
      {
        //影
        mpDrawUtilText->SetTextColor( mShadowCol );
        mpDrawUtilText->DRAW_TEXT_FUNC( 0, mLineHeight*line+1 + mScrollOfs, L"[%ls][Lock]", dispStr );

        //メイン
        mpDrawUtilText->SetTextColor( mItemCol );
        mpDrawUtilText->DRAW_TEXT_FUNC( 0, mLineHeight*line + mScrollOfs, L"[%ls][Lock]", dispStr );
      }
      else
      {
        //影
        mpDrawUtilText->SetTextColor( mShadowCol );
        mpDrawUtilText->DRAW_TEXT_FUNC( 0, mLineHeight*line+1 + mScrollOfs, L"[%ls]", dispStr );

        //メイン
        mpDrawUtilText->SetTextColor( mItemCol );
        mpDrawUtilText->DRAW_TEXT_FUNC( 0, mLineHeight*line + mScrollOfs, L"[%ls]", dispStr );
      }

      drawLineCount++;
    }
    line++;

    //項目の表示
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    while( it != list->End() )
    {
      DebugWinItem *item = (*it);
      if( mLineHeight*line+1 + mScrollOfs > 0 &&
          mLineHeight*line+1 + mScrollOfs < 240 - mLineHeight &&
          drawLineCount < DEBUG_WIN_LINE_MAX )
      {
        const wchar_t *dispStr2 = item->GetDispName(this);
        mpDrawUtilText->SetTextScale( mFontScale,mFontScale );
        //影
  		  mpDrawUtilText->SetTextColor( mShadowCol );
  		  mpDrawUtilText->DRAW_TEXT_FUNC( 16, mLineHeight*line+1 + mScrollOfs, dispStr2 );

        //メイン
        mpDrawUtilText->SetTextColor( it==mCurrentItem?mCurrentCol:mItemCol );
  		  mpDrawUtilText->DRAW_TEXT_FUNC( 16, mLineHeight*line + mScrollOfs, dispStr2 );

        drawLineCount++;
      }
      line++;
      ++it;
    }
  }
}

void DebugWin::Draw( gfl2::gfx::CtrDisplayNo dispNo , gfl2::gfx::DisplayDeviceHandle handle )
{
  if( mIsFoceStop )
  {
    return;
  }

  if( dispNo == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    //m_pRenderingPipeLine->SetProjectionMatrix( m_ProjMatTable[ displayNo ] );
    //m_pRenderingPipeLine->SetViewMatrix( m_ViewMatTable[ displayNo ] );	

    mpDrawUtilText->SetRenderPath( mRenderingPipeLine->GetUtilTextRenderPath() );

    if( mIsOpen )
    {
      // 暗くする
      gfl2::gfx::GFGL::BeginScene();
      gfl2::gfx::DrawUtil::BeginRender();

      gfl2::gfx::DrawUtil::SetScreenSize( 320, 240);
      gfl2::gfx::GFGL::SetViewport( 0, 0, 320, 240);

      gfl2::gfx::DrawUtil::SetMaterialAlpha( 0.5f);
      gfl2::gfx::DepthStencilStateObject::StateData stateData;
      stateData.m_DepthTestEnable = false;
      gfl2::gfx::DrawUtil::SetDepthTestState( stateData);

      gfl2::gfx::DrawUtil::DrawRect( 0, 0, 320, 240, gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f) );

      gfl2::gfx::DrawUtil::EndRender();
      gfl2::gfx::GFGL::EndScene();

      mRenderingPipeLine->SetRenderTarget( pBackBuffer );
      mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      mRenderingPipeLine->Execute();
    }
  }
}


void DebugWin::SetCurrentGroup( DebugWinGroup *group , DebugWinGroup *selGroup )
{
  //現在グループの設定
  mCurrentGroup = group;
  
  mCurrentItem = mCurrentGroup->GetItemList()->Begin();
  mCurrentIdx = 0;
  if( selGroup != NULL )
  {
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    int tempIdx = 0;
    while( it != list->End() )
    {
      DebugWinItem *item = (*it);
      if( item == selGroup )
      {
        mCurrentItem = it;
        mCurrentIdx = tempIdx;
        break;
      }
      ++it;
      tempIdx++;
    }
    
  }
  
  //項目が無ければ現在項目をNULLへ
  if( mCurrentItem == mCurrentGroup->GetItemList()->End() )
  {
    mCurrentItem = ItemItNull;
  }

  ItemViewBufferClear();
}

void DebugWin::SetCurrentItem( DebugWinItem *selItem )
{
  if( selItem != NULL )
  {
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    int tempIdx = 0;
    while( it != list->End() )
    {
      DebugWinItem *item = (*it);
      if( item == selItem )
      {
        mCurrentItem = it;
        mCurrentIdx = tempIdx;
        return;
      }
      ++it;
      tempIdx++;
    }
  }
}
void DebugWin::SetCurrentItemByIndex( s32 index )
{
  DebugWinItemList *list = mCurrentGroup->GetItemList();
  DebugWinItemIt it = list->Begin();
  int tempIdx = 0;
  while (it != list->End())
  {
    DebugWinItem *item = (*it);
    if (tempIdx == index)
    {
      mCurrentItem = it;
      mCurrentIdx = tempIdx;
      return;
    }
    ++it;
    tempIdx++;
  }
}



//----------------------------------------------------------------------------
/**
*  @brief  現在のカーソル位置を更新(先頭にする)
*  @note   開いているグループに項目を追加したときの対処
*/
//-----------------------------------------------------------------------------
void DebugWin::UpdateCurrent( void )
{
  if( mCurrentItem != ItemItNull )
  {
    // 選択中ならそのまま

  }
  else
  {
    // 項目なし→項目ありになった際は、先頭にカーソルを合わせる
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    if( it != list->End() )
    {
      mCurrentIdx = 1;
      mCurrentItem = it;
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  タッチ更新
*/
//-----------------------------------------------------------------------------
void DebugWin::InputUpdateTouch()
{
  gfl2::ui::TouchPanel *touchPanel = GetDeviceManager()->GetTouchPanel( 0 );
  
  if( touchPanel->IsTouchTrigger() )
  {
    //項目の表示
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    int line = 0;
    int drawLineCount = 0;

    while( it != list->End() )
    {
      DebugWinItem *item = (*it);
      if(
        (*it)->IsSelectSkip() == false )
      {
        // 
        f32 y = touchPanel->GetY();
        if( touchPanel->GetY() >= mLineHeight*(line+1)+1 + mScrollOfs && touchPanel->GetY() <= mLineHeight*(line+2) + 1 + mScrollOfs )
        {
          // 1タッチ目で項目を選択。2タッチ目で項目を実行
          if( mCurrentItem == it )
          {
            (*it)->UpdateTouchFunc( this );
          }
          else
          {
            mCurrentIdx = line;
            mCurrentItem = it;

            ItemViewBufferCalTouchiSelectAfter();
          }
          break;
        }

        drawLineCount++;
      }
      line++;
      ++it;
    }
  }
}

// 表示位置関連
void DebugWin::ItemViewBufferCalTouchiSelectAfter( void )
{
  //グループ名が入るから+1
  const int dispPos = (mCurrentIdx+1) * mLineHeight + mScrollOfs;

  if( dispPos + mLineHeight > m_ItemViewBufferBottom )
  {
    m_ItemViewBufferBottom = dispPos + mLineHeight;
  }

  if( dispPos < m_ItemViewBufferUpper )
  {
    m_ItemViewBufferUpper = dispPos;
  }
}

void DebugWin::ItemViewBufferRecover( void )
{
  int def_bottom = 240 - mLineHeight * 3;
  int def_upper = mLineHeight * 3;

  const int dispPos = (mCurrentIdx+1) * mLineHeight + mScrollOfs;

  if( def_bottom != m_ItemViewBufferBottom )
  {
    int newBottom = 0;
    newBottom = gfl2::math::Max( static_cast<int>( dispPos + mLineHeight ), def_bottom );
    if( m_ItemViewBufferBottom > newBottom )
    {
      m_ItemViewBufferBottom = newBottom;
    }
  }

  if( def_upper != m_ItemViewBufferUpper )
  {
    int newUpper = 0;
    newUpper = gfl2::math::Min( dispPos, def_upper );
    if( m_ItemViewBufferUpper < newUpper )
    {
      m_ItemViewBufferUpper = newUpper;
    }
  }
}

void DebugWin::ItemViewBufferClear( void )
{
  m_ItemViewBufferBottom = 240 - mLineHeight * 3;
  m_ItemViewBufferUpper = mLineHeight * 3;
}

//----------------------------------------------------------------------------
/**
*  @brief  キー入力更新
*/
//-----------------------------------------------------------------------------
void DebugWin::InputUpdateKeyborad()
{
#if 0
#if defined(GF_PLATFORM_WIN32)

  int selectIndex = -1;

  static gfl2::system::InputDevice::KeyBoardKey s_KeyList[] =
  {
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_1,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_2,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_3,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_4,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_5,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_6,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_7,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_8,
    gfl2::system::InputDevice::KeyBoardKey::KBKEY_9,
  };

  for( u32 i = 0; i < GFL_NELEMS( s_KeyList ); i++ )
  {
    if( gfl2::system::InputDevice::KeyBoard::IsTrigger( s_KeyList[i] ) )
    {
      selectIndex = i;
      break;
    }
  }

  if( selectIndex != -1 )
  {
    //項目の表示
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->Begin();
    int line = 0;

    while( it != list->End() )
    {
      if( selectIndex == line && (*it)->IsSelectSkip() == false )
      {
        mCurrentItem = it;
        (*it)->UpdateTouchFunc( this );
        break;
      }

      line++;
      ++it;
    }
  }
#endif
#endif
}

void DebugWin::SetDeviceManager( gfl2::ui::DeviceManager *devMng , gfl2::ui::Button *btn , gfl2::ui::VectorDevice *key )
{
  mDeviceManager = devMng;
  mUiButton = btn;
  mUiCrossKey = key;
}

void DebugWin::SetCallbackClass( gfl2::debug::DebugWinSystemCallBack *cbClass )
{
  mCallbackClass = cbClass;
}

gfl2::ui::Button* DebugWin::GetUiButton(void)const
{
  if( mUiButton == NULL )
  {
    return mDeviceManager->GetButton(0);
  }
  return mUiButton;
}

gfl2::ui::VectorDevice* DebugWin::GetUiCrossKey(void)const
{
  if( mUiCrossKey == NULL )
  {
    return mDeviceManager->GetCrossKey(0);
  }
  return mUiCrossKey;
}

void DebugWin::AddItem( DebugWinItem *item , DebugWinGroup *parent )
{
  //項目の追加、親がNULLならトップへ追加
  if( parent )
  { 
    parent->AddItem(item);
  }
  else
  {
    mTopGroup->AddItem(item);
  }

}

DebugWinGroup* DebugWin::AddGroup( heap::HeapBase *heap , const wchar_t *name , DebugWinGroup *parent , int itemNum )
{
  GFL_ASSERT_MSG(mGroupList.Size() < mGroupList.MaxSize() , "リストサイズがMAXです\n" );

  //外部用グループ追加
  DebugWinGroup *tmpParent = parent;
  if( tmpParent == NULL )
  {
    tmpParent = mTopGroup;
  }
  heap::HeapBase *tempHeap = heap;
  if( tempHeap == NULL )
  {
    tempHeap = mHeap;
  }
  
  DebugWinGroup *group = GFL_NEW( tempHeap )DebugWinGroup( tmpParent , name , itemNum , heap );
  
  //グループの追加、項目との違いはグループ管理リストへの登録
  AddItem(group,parent);
  //検索優先度とか考えて先頭追加
  mGroupList.PushFront(group);
  
  return group;
}

DebugWinGroup* DebugWin::AddGroup( heap::HeapBase *heap , str::MsgData *msgData , u32 strId , DebugWinGroup *parent , int itemNum )
{
  msgData->GetString( strId , *mWorkStrbuf );
  return AddGroup( heap , mWorkStrbuf->GetPtr() , parent, itemNum );
}


bool DebugWin::IsOpen(void)
{
  //開閉チェック
  return GFL_SINGLETON_INSTANCE(DebugWin)->mIsOpen;
}

bool DebugWin::IsCloseTrigger(void)
{
  return GFL_SINGLETON_INSTANCE(DebugWin)->mIsCloseTrigger;
}

//強制グループ閉じ
void DebugWin::CloseWindow(void)
{
  if( GFL_SINGLETON_INSTANCE(DebugWin)->mIsOpen )
  {
    GFL_PRINT("DebugWindow is force close!\n");
    GFL_SINGLETON_INSTANCE(DebugWin)->mIsOpen = false;
    mIsCloseTrigger = true;

    if( mCurrentGroup )
    {
      mCurrentGroup->CallWindowCloseGroupFunc();
    }
  }
}

void DebugWin::SetLockBanFlag( f32 flag )
{
  GFL_SINGLETON_INSTANCE(DebugWin)->mIsBanLock = flag;
}

bool DebugWin::IsLock(void) const
{
  return GFL_SINGLETON_INSTANCE(DebugWin)->mIsLock;
}

void DebugWin::PlaySystemSound( const DEBUGWIN_SOUND_TYPE type )
{
  if( mCallbackClass )
  {
    mCallbackClass->SoundCallBack(type);
  }
}

void DebugWin::RemoveGroup( DebugWinGroup *group )
{
  //グループの削除
  //stdからnwにしたら、二重removeは止まる。デストラクタ内でやる
  //RemoveGroupList( group );
  RemoveItemFunc(group);

  GFL_DELETE group;
  
}

void DebugWin::RemoveGroup( const wchar_t *name )
{
  //名前からグループの削除
  DebugWinGroup *grp = GetGroupByName(name);
  if( grp != mTopGroup )
  {
    RemoveGroup(grp);
  }
}


void DebugWin::RemoveGroupList( DebugWinGroup *group )
{
  //グループ用リストからグループを除外する
  mGroupList.Remove( group );
}

void DebugWin::RemoveItemFunc( DebugWinItem *item )
{
  //項目削除の機能(DebugWinItemのデストラクタから呼ばれます)
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  
  if( debWin->mCurrentGroup == item )
  {
    debWin->mCurrentGroup = debWin->mTopGroup;
    debWin->mCurrentItem = ItemItNull;
    mCurrentIdx = 0;
  }
  if( debWin->mCurrentItem != ItemItNull &&
      (*debWin->mCurrentItem) == item )
  {
    debWin->mCurrentItem = ItemItNull;
    mCurrentIdx = 0;
  }
}

void DebugWin::SoundCallBack( const DEBUGWIN_SOUND_TYPE type )
{
  //コールバッククラスにサウンド処理を投げる
  if( mCallbackClass )
  {
    mCallbackClass->SoundCallBack(type);
  }
}

DebugWinGroup* DebugWin::GetGroupByName( const wchar_t *name )
{
  //名前からグループの取得
  //
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  DebugWinGroupIt it = debWin->mGroupList.Begin();
  while( it != debWin->mGroupList.End() )
  {
    DebugWinGroup *group = (*it);
    
    const bool ret = gfl2::str::StrComp( name , group->GetName() );
    if( ret )
    {
      return group;
    }
    ++it;
  }
  //見つからない！
  GFL_PRINT("DebugWin:Group[%ls] not found!!\n",name);
  return debWin->mTopGroup;
}

DebugWinGroup* DebugWin::GetTopGroup(void)
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  return debWin->mTopGroup;
}

#else  //GFL_DEBUGWIN_USE

/*
  #define DebugWin_Update() ((void)0)
  #define DebugWin_Draw(grpSys,dispType) ((void)0)
  #define DebugWin_IsOpen() (false)
  #define DebugWin_AddGroup(name,parent) (NULL)
  #define DebugWin_RemoveGroup(parent) ((void)0)
  #define DebugWin_RemoveGroupByName(name) ((void)0)
*/

  void DebugWin_Update(void){}
  void DebugWin_Draw(...){}
  bool DebugWin_IsOpen(void){return false;}
  bool DebugWin_IsCloseTrigger(void){return false;}
  void DebugWin_CloseWindow(void){}
  DebugWinGroup* DebugWin_AddGroup(...){return NULL;}
  DebugWinGroup* DebugWin_AddGroupMsg(...){return NULL;}
  void DebugWin_RemoveGroup(...){}
  void DebugWin_RemoveGroupByName(...){}
  void DebugWin_SetDeviceManager(...){}
  void DebugWin_SetCallbackClass(...){}
  void DebugWin_SetCurrentItem(...){}

  wchar_t * DebugWin_GetWorkStr( void ){return NULL;}
  str::StrBuf * DebugWin_GetWorkStrBuf(void){return NULL;}
  DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name ){return NULL;}

  gfl2::math::Vector4* DebugWin_GetItemColor(void){return NULL;}
  gfl2::math::Vector4* DebugWin_GetShadowColor(void){return NULL;}
  gfl2::math::Vector4* DebugWin_GetCurrentColor(void){return NULL;}
  gfl2::math::Vector4* DebugWin_GetFillColor(void){return NULL;}
  f32* DebugWin_GetFontScale(void){return NULL;}
  f32* DebugWin_GetLineHeight(void){return NULL;}
  int* DebugWin_GetHoldKey(void){return NULL;}
  int* DebugWin_GetTriggerKey(void){return NULL;}
  gfl2::ui::Button* DebugWin_GetUiButton(void){return NULL;}

#endif //GFL_DEBUGWIN_USE


GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)
