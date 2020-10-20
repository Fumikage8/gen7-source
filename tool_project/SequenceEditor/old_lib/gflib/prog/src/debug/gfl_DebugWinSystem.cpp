//======================================================================
/**
 * @file	gfl_DebugwinSystem.cpp
 * @brief	デバッグウィンドウ
 * @author	ariizumi
 * @data	11/03/29
 */
//======================================================================

#include <gflib.h>
#include <grp/util/gfl_UtilDrawUtil.h>

#include <debug/gfl_DebugWinSystem.h>
#include <ro/gfl_RoManager.h>


#include <debug/gfl_DebugWinLocal.h>


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_DEBUGWIN_USE
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::debug::DebugWin);
template class gfl::base::SingletonAccessor<gfl::debug::DebugWin>;
#endif
#endif

namespace gfl
{
namespace debug
{
#if GFL_DEBUGWIN_USE

void DebugWin_Initialize( gfl::heap::HeapBase * heap)
{
  gfl::debug::DebugWin *debWin = GFL_NEW(heap) gfl::debug::DebugWin(heap);
}

void DebugWin_Finalize( void )
{
  gfl::debug::DebugWin * debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  GFL_DELETE debWin;
}

void DebugWin_Update(void)
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->Update();
}

void DebugWin_Draw( gfl::grp::GraphicsSystem *grpSys , gfl::grp::DisplayType dispType )
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->Draw(grpSys,dispType);
}

bool DebugWin_IsOpen(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->IsOpen();
}

bool DebugWin_IsCloseTrigger(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->IsCloseTrigger();
}


void DebugWin_CloseWindow(void)
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->CloseWindow();
}

//---------------------------------------------------------
//グループ作成
DebugWinGroup* DebugWin_AddGroup( const wchar_t *name , DebugWinGroup *parent , heap::HeapBase *heap )
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->AddGroup(name,parent, heap );
}

DebugWinGroup* DebugWin_AddGroup( str::MsgData *msgData , u32 strId , DebugWinGroup *parent , heap::HeapBase *heap )
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->AddGroup(msgData,strId,parent, heap);
}

//グループ削除
void DebugWin_RemoveGroup( DebugWinGroup *group )
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->RemoveGroup(group);
}

//グループ削除(名前)
void DebugWin_RemoveGroupByName( const wchar_t *name )
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->RemoveGroup(name);
}

//デバイスマネージャーの設定
void DebugWin_SetDeviceManager( gfl::ui::DeviceManager *devMng , gfl::ui::Button *btn , gfl::ui::VectorDevice *key )
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->SetDeviceManager(devMng,btn,key);
}

//コールバック受け取りクラスの設定
void DebugWin_SetCallbackClass( gfl::debug::DebugWinSystemCallBack *cbClass )
{
  GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->SetCallbackClass(cbClass);
}

//テンポラリな文字列領域を取得
wchar_t * DebugWin_GetWorkStr( void )
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetWorkStr();
}
DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name )
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetGroupByName( name );
}

//取得系(DebugWinで操作できるようにポインタで返す)
gfl::math::VEC4* DebugWin_GetItemColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetItemColor();
}
gfl::math::VEC4* DebugWin_GetShadowColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetShadowColor();
}
gfl::math::VEC4* DebugWin_GetCurrentColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetCurrentColor();
}
gfl::math::VEC4* DebugWin_GetFillColor(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetFillColor();
}
f32* DebugWin_GetFontScale(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetFontScale();
}
f32* DebugWin_GetLineHeight(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetLineHeight();
}
int* DebugWin_GetHoldKey(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetHoldKey();
}
int* DebugWin_GetTriggerKey(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetTriggerKey();
}
gfl::ui::Button* DebugWin_GetUiButton(void)
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetUiButton();
}

static DebugWinItemIt ItemItNull;

//コンストラクタ
DebugWin::DebugWin( heap::HeapBase *heap )
:mHeap(heap)
,mDeviceManager(NULL)
,mUiButton(NULL)
,mUiCrossKey(NULL)
,mHoldKey(gfl::ui::BUTTON_R)
,mTrgKey(gfl::ui::BUTTON_X)
,mIsOpen(false)
,mIsCloseTrigger(false)
,mCurrentIdx(0)
,mItemCol(1.0f,1.0f,1.0f,1.0f)
,mShadowCol(0.0f,0.0f,0.0f,1.0f)
,mCurrentCol(1.0f,0.0f,0.0f,1.0f)
,mFillCol(0.2f,0.2f,0.2f,0.8f)
,mFontScale(0.5f)
,mLineHeight(16.0f)
,mScrollOfs(0)
,mCallbackClass(NULL)
{
  //トップグループの作成
  mTopGroup = GFL_NEW(mHeap)DebugWinGroup(NULL,L"DebugWinTop"); 
  mCurrentGroup = mTopGroup;
  mCurrentItem = ItemItNull;
  
  mWorkStrbuf = GFL_NEW(mHeap)str::StrBuf(DEBUGWIN_NAME_LEN,mHeap);
}

//デストラクタ
DebugWin::~DebugWin()
{
  GFL_DELETE mWorkStrbuf;
  GFL_DELETE mTopGroup;
}

void DebugWin::Update()
{
  if( mIsCloseTrigger )
  {
    mIsCloseTrigger = false;
    SoundCallBack(SE_WINDOW_CLOSE);
  }
  
  if( mDeviceManager != NULL )
  {
    DebugWinItemList *list = mCurrentGroup->GetItemList();

    gfl::ui::Button *uiButton = GetUiButton();
    gfl::ui::VectorDevice *uiKey = GetUiCrossKey();
    
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
            !list->empty() )
        {
          mCurrentItem = list->begin();
          mCurrentIdx = 0;
        }
      }
      else
      {
        GFL_PRINT("DebugWindow is close!\n");
        mIsCloseTrigger = true;
        //別個閉じる操作があるので、処理追加時には注意！
      }
    }
    
    if( mIsOpen )
    {
      //開いているとき更新
      if( !list->empty() && 
          mCurrentItem != ItemItNull )
      {
        //項目移動
        if( uiKey->IsRepeat(gfl::ui::DIR_UP) )
        {
          if( mCurrentItem == list->begin() )
          {
            mCurrentItem = list->end();
            mCurrentItem--;
            mCurrentIdx = list->size();
          }
          else
          {
            mCurrentItem--;
            mCurrentIdx--;
          }
          SoundCallBack(SE_CURSOR_MOVE);
        }
        else
        if( uiKey->IsRepeat(gfl::ui::DIR_DOWN) )
        {
          mCurrentItem++;
          mCurrentIdx++;
          if( mCurrentItem == list->end() )
          {
            mCurrentItem = list->begin();
            mCurrentIdx = 0;
          }
          SoundCallBack(SE_CURSOR_MOVE);
        }

        //項目更新処理
        DebugWinItem *item = &(*mCurrentItem);
        item->UpdateFunc(this);
        
        //スクロールの更新
        {
          //グループ名が入るから+1
          const int dispPos = (mCurrentIdx+1) * mLineHeight + mScrollOfs;
          static const int LIMIT = 48;
          
          //文字の下を見るからもう1個分＋
          if( dispPos+mLineHeight > (240-LIMIT) )
          {
            mScrollOfs = (240-LIMIT) - ((mCurrentIdx+2) * mLineHeight );
          }
          if( dispPos < (LIMIT) )
          {
            mScrollOfs = (LIMIT) - ((mCurrentIdx+1) * mLineHeight);
          }

          if( mScrollOfs > 0 )
          {
            mScrollOfs = 0;
          }
        }
        
      }

      //親へ移動
      if( uiButton->IsTrigger(gfl::ui::BUTTON_B) )
      {
        DebugWinGroup *parentGrp = mCurrentGroup->GetParentGroup();
        if( parentGrp != NULL )
        {
          SetCurrentGroup( parentGrp , mCurrentGroup );
          SoundCallBack(SE_GROUP_OUT);
        }
      }
    }
    else
    {
      
    }
  }
}

void DebugWin::Draw( gfl::grp::GraphicsSystem *grpSys , gfl::grp::DisplayType dispType )
{
  if( mIsOpen )
  {
    //開いているとき更新
    gfl::grp::RenderTarget* renderTarget = grpSys->GetCurrentFrameBuffer(dispType);
    if( grpSys->BindCurrentCommandList( renderTarget ) )
    {

      gfl::grp::util::DrawUtil::BeginRender( renderTarget );
      int line = 0;

      //塗りつぶし
      gfl::grp::util::DrawUtil::MaterialInfo info;
      gfl::grp::util::DrawUtil::SetMaterial( info );
      gfl::grp::util::DrawUtil::DrawFillScreen( mFillCol );
      
      //grpSys
      
      {
        if( mLineHeight*line+1 + mScrollOfs > 0 &&
            mLineHeight*line+1 + mScrollOfs < 240 - mLineHeight )
        {
          //グループ名の表示
          const wchar_t *dispStr = mCurrentGroup->GetDispName(this);
          gfl::grp::util::DrawUtil::SetTextScale( mFontScale,mFontScale );
          //影
          gfl::grp::util::DrawUtil::SetTextColor( mShadowCol );
          gfl::grp::util::DrawUtil::DrawText( 0, mLineHeight*line+1 + mScrollOfs, L"[%ls]", dispStr );
          //メイン
          gfl::grp::util::DrawUtil::SetTextColor( mItemCol );
          gfl::grp::util::DrawUtil::DrawText( 0, mLineHeight*line + mScrollOfs, L"[%ls]", dispStr );
        }
        line++;

        //項目の表示
        DebugWinItemList *list = mCurrentGroup->GetItemList();
        DebugWinItemIt it = list->begin();
        while( it != list->end() )
        {
          DebugWinItem *item = &(*it);
          if( mLineHeight*line+1 + mScrollOfs > 0 &&
              mLineHeight*line+1 + mScrollOfs < 240 - mLineHeight )
          {
            const wchar_t *dispStr2 = item->GetDispName(this);
            gfl::grp::util::DrawUtil::SetTextScale( mFontScale,mFontScale );
            //影
            gfl::grp::util::DrawUtil::SetTextColor( mShadowCol );
            gfl::grp::util::DrawUtil::DrawText( 16+1, mLineHeight*line+1 + mScrollOfs, dispStr2 );
            //メイン
            if( it == mCurrentItem )
            {
              //選択項目
              gfl::grp::util::DrawUtil::SetTextColor( mCurrentCol );
            }
            else
            {
              //非選択項目
              gfl::grp::util::DrawUtil::SetTextColor( mItemCol );
            }
            gfl::grp::util::DrawUtil::DrawText( 16, mLineHeight*line + mScrollOfs, dispStr2 );
          }
          line++;
          it++;
        }
      }

      gfl::grp::util::DrawUtil::EndRender();
    }
  }
}

void DebugWin::SetCurrentGroup( DebugWinGroup *group , DebugWinGroup *selGroup )
{
  //現在グループの設定
  mCurrentGroup = group;
  
  mCurrentItem = mCurrentGroup->GetItemList()->begin();
  mCurrentIdx = 0;
  if( selGroup != NULL )
  {
    DebugWinItemList *list = mCurrentGroup->GetItemList();
    DebugWinItemIt it = list->begin();
    int tempIdx = 0;
    while( it != list->end() )
    {
      DebugWinItem *item = &(*it);
      if( item == selGroup )
      {
        mCurrentItem = it;
        mCurrentIdx = tempIdx;
        break;
      }
      it++;
      tempIdx++;
    }
    
  }
  
  //項目が無ければ現在項目をNULLへ
  if( mCurrentItem == mCurrentGroup->GetItemList()->end() )
  {
    mCurrentItem = ItemItNull;
  }
}


void DebugWin::SetDeviceManager( gfl::ui::DeviceManager *devMng , gfl::ui::Button *btn , gfl::ui::VectorDevice *key )
{
  mDeviceManager = devMng;
  mUiButton = btn;
  mUiCrossKey = key;
}

void DebugWin::SetCallbackClass( gfl::debug::DebugWinSystemCallBack *cbClass )
{
  mCallbackClass = cbClass;
}

gfl::ui::Button* DebugWin::GetUiButton(void)const
{
  if( mUiButton == NULL )
  {
    return mDeviceManager->GetButton(0);
  }
  return mUiButton;
}

gfl::ui::VectorDevice* DebugWin::GetUiCrossKey(void)const
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

DebugWinGroup* DebugWin::AddGroup( const wchar_t *name , DebugWinGroup *parent , heap::HeapBase *heap )
{
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
  
  DebugWinGroup *group = GFL_NEW( tempHeap )DebugWinGroup( tmpParent , name );
  
  //グループの追加、項目との違いはグループ管理リストへの登録
  AddItem(group,parent);
  //検索優先度とか考えて先頭追加
  mGroupList.push_front(group);
  
  return group;
}

DebugWinGroup* DebugWin::AddGroup( str::MsgData *msgData , u32 strId , DebugWinGroup *parent , heap::HeapBase *heap )
{
  msgData->GetString( strId , *mWorkStrbuf );
  return AddGroup( mWorkStrbuf->GetPtr() , parent , heap );
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
  }
}

void DebugWin::PlaySound( const DEBUGWIN_SOUND_TYPE type )
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
  mGroupList.erase( group );
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
      &(*debWin->mCurrentItem) == item )
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
  DebugWinGroupIt it = debWin->mGroupList.begin();
  while( it != debWin->mGroupList.end() )
  {
    DebugWinGroup *group = &(*it);
    
    const bool ret = gfl::str::StrComp( name , group->GetName() );
    if( ret )
    {
      return group;
    }
    it++;
  }
  //見つからない！
  GFL_PRINT("DebugWin:Group[%ls] not found!!\n",name);
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

  wchar_t * DebugWin_GetWorkStr( void ){return NULL;}
  DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name ){return NULL;}

  gfl::math::VEC4* DebugWin_GetItemColor(void){return NULL;}
  gfl::math::VEC4* DebugWin_GetShadowColor(void){return NULL;}
  gfl::math::VEC4* DebugWin_GetCurrentColor(void){return NULL;}
  gfl::math::VEC4* DebugWin_GetFillColor(void){return NULL;}
  f32* DebugWin_GetFontScale(void){return NULL;}
  f32* DebugWin_GetLineHeight(void){return NULL;}
  int* DebugWin_GetHoldKey(void){return NULL;}
  int* DebugWin_GetTriggerKey(void){return NULL;}
  gfl::ui::Button* DebugWin_GetUiButton(void){return NULL;}

#endif //GFL_DEBUGWIN_USE


}//namespace debug
}//namespace gfl

