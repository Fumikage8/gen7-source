//============================================================================================
/**
 * @file    UIResponder.cpp
 * @brief    UIコントロール関連
 * @author  suzuki_hisanao
 * @date    12.2.25
 */
//============================================================================================
#include "AppLib/include/Ui/UIResponder.h"
#include "AppLib/include/Ui/UIViewManager.h"
#include <Prog/GameSys/include/GameManager.h>
#include <base/include/gfl2_Singleton.h>

#include <debug/include/gfl2_DebugWinSystem.h>


namespace app {
namespace ui {

// abs() for float
#define FABS(a) ((a<0.0f)?(-a):(a))

const float UIResponder::STICK_INPUT_VALUE = (0.4f);

//--------------------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 *
 * @param    heap    ヒープ
 */
//--------------------------------------------------------------------------------------------
UIResponder::UIResponder( app::util::Heap * heap ) :
  m_button_manager( NULL ),
  m_uiListener(NULL),
  m_buttonLock(false),
  m_isInputEnable(true),
  m_isExclusive(false),
  m_isAnimeExclusive(true),
  m_isTouchEnable(true),
  m_isKeyExclusiveByButtonAnime(true)
{
  Create( heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//--------------------------------------------------------------------------------------------
UIResponder::~UIResponder(void)
{
  Delete();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパッド、ボタン、アナログキーの入力イベント取得用リスナの登録
 *
 * @param   listener イベントリスナへ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetInputListener( app::ui::UIInputListener* listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    UIResponder関連作成
 *
 * @param    heap    ヒープ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::Create( app::util::Heap * heap )
{
  GameSys::GameManager * pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  m_uiDeviceManager = pgm->GetUiDeviceManager();
  
  this->SetStickEmulationMode( true );

  m_stick           = m_uiDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  m_touchPanel      = m_uiDeviceManager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

#ifdef  UIRESPONDER_USE_UI_GYROSCOPE
  // ジャイロ
  m_uiDeviceManager->BootGyroscope( heap->pHeap, gfl::ui::CTR_DeviceManager::GYROSCOPE_STANDARD );
  m_gyroscope  = m_uiDeviceManager->GetGyroscope( gfl::ui::CTR_DeviceManager::GYROSCOPE_STANDARD );
  m_gyroscope->SetZeroDriftMode( gfl::ui::Gyroscope::ZERO_DRIFT_MODE_TIGHT );
  m_gyroscope->DisableRevisionByAcceleration();
  m_gyroscope->DisableZeroDrift();
#endif  // UIRESPONDER_USE_UI_GYROSCOPE

#ifdef  UIRESPONDER_USE_UI_ACCELEROMETER
  // 加速度センサー
  m_uiDeviceManager->BootAccelerometer( heap->pHeap, gfl::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );
  m_accelerometer = m_uiDeviceManager->GetAccelerometer( gfl::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );
#endif  // UIRESPONDER_USE_UI_ACCELEROMETER
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    UIResponder関連削除
 */
//--------------------------------------------------------------------------------------------
void UIResponder::Delete(void)
{
  ReleaseButtonManager();

#ifdef  UIRESPONDER_USE_UI_ACCELEROMETER
  if( m_uiDeviceManager != NULL ){
    m_uiDeviceManager->StopAccelerometer( gfl::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );
  }
#endif  // UIRESPONDER_USE_UI_ACCELEROMETER
#ifdef  UIRESPONDER_USE_UI_GYROSCOPE
  if( m_uiDeviceManager != NULL ){
    m_uiDeviceManager->StopGyroscope( gfl::ui::CTR_DeviceManager::GYROSCOPE_STANDARD );
  }
#endif  // UIRESPONDER_USE_UI_GYROSCOPE
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパッド、ボタン、アナログキーの入力有効設定
 *
 * @param   isEnable 入力有効ならtrue、無効ならfalseを指定
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetInputEnabled( bool isEnable )
{
  m_isInputEnable = isEnable;

  OnSetInputEnabled( isEnable );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   SetInputEnabled を呼び出した後にコールされます
 * @note    UIView側で、自身の子Viewのみの入力制御を行いたい場合に使用します
 *
 * @param   isEnable 入力有効ならtrue、無効ならfalseを指定
 */
//--------------------------------------------------------------------------------------------
void UIResponder::OnSetInputEnabled( bool isEnable )
{
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパッド、ボタン、アナログキーの入力有効判定
 *
 * @return  isEnable 入力有効ならtrue、無効ならfalseを返却
 */
//--------------------------------------------------------------------------------------------
bool UIResponder::IsInputEnabled( void ) const
{
#if GFL_DEBUGWIN_USE
  //このデバッグウインドウが動いている時はUIViewを停止
  if( gfl2::debug::DebugWin_IsOpen()  ){
    return false;
  }
#endif
  return m_isInputEnable;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパッドの入力検出有効設定
 *
 * @param   isEnable タッチパッド入力検出を有効化するならtrue、無効化するならfalseを指定
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetTouchEnabled( bool isEnable )
{
  m_isTouchEnable = isEnable;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパッドの入力検出有効判定
 *
 * @return  isEnable 入力検出が有効ならtrue、無効ならfalseを返却
 */
//--------------------------------------------------------------------------------------------
bool UIResponder::IsTouchEnabled( void ) const
{
  return m_isTouchEnable;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   タッチパネルのタッチ状態を判定
 *
 * @retval true  タッチパネルがタッチ状態
 * @retval false タッチパネルは未タッチ
 */
//--------------------------------------------------------------------------------------------
bool UIResponder::IsHoldTouchPanel( void ) const
{
 return m_touchPanel->IsTouch();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   定期処理
 * @param   isEnable 呼び出し側にて入力を禁止する場合はfalseを指定する
 */
//--------------------------------------------------------------------------------------------
void UIResponder::UpdateResponse( UIView* pThisUIView,bool isEnable )
{
  m_buttonLock = false;
  CleanupButtonManager();
  
  if( !IsInputEnabled() || !isEnable ){
    //アニメの更新のみ行う
    //  行わないと内部状態遷移が止まってしまう。
    if( m_button_manager != NULL ){
      m_button_manager->Update();
    }
    return;   //入力禁止
  }

  // ツリー上の全ボタン監視
  if(GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->GetInputExclusiveKeyByButtonAnimation())
  {
    if( GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->IsButtonAnimation(pThisUIView) )
    {
//      m_buttonLock = true;
      if( m_button_manager != NULL ){
        m_button_manager->Update();
      }
      return;
    }
  }

  //入力検知→更新の流れにしないと1フレ回ってくる
  if( m_button_manager != NULL ){
    m_button_manager->UpdateInput( m_button, m_touchPanel );
    m_button_manager->Update();
  }

  if( m_uiListener != NULL )
  {
    if( IsTouchEnabled() )
    {
      if( m_touchPanel->IsTouch() || m_touchPanel->IsTouchRelease() )
      {
        if( !m_buttonLock )
        {
          app::ui::UIInputListener::ListenerResult x = m_uiListener->OnTouchEvent( m_touchPanel, m_touchPanel->IsTouch() );
          if((app::ui::UIInputListener::DISABLE_ACCESS==x) || (app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x)){
            m_buttonLock = true;
          }
          if(app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x){
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(pThisUIView);
          }
          else{
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(NULL);
          }
        }
      }

      if( m_touchPanel->IsDoubleClick() )
      {
        if( !m_buttonLock )
        {
          app::ui::UIInputListener::ListenerResult x = m_uiListener->OnDoubleClickEvent( m_touchPanel ); //ListenerResult
          if((app::ui::UIInputListener::DISABLE_ACCESS==x) || (app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x)){
            m_buttonLock = true;
          }
          if(app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x){
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(pThisUIView);
          }
          else{
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(NULL);
          }
        }
      }
    }
    
    // キー入力
    if( (FABS(m_stick->GetX())>=STICK_INPUT_VALUE) ||
        (FABS(m_stick->GetY())>=STICK_INPUT_VALUE) ||
        (m_button->GetHold() != 0) ||
        (m_key->GetHold() != 0) )
    {
      if( !m_buttonLock )
      {
        bool enable = true;
        if( m_isKeyExclusiveByButtonAnime ) {
          if( this->IsButtonAnimation() ) {
            enable = false;
          }
        }
        if( enable ) {
          app::ui::UIInputListener::ListenerResult x = m_uiListener->OnKeyAction( m_button, m_key, m_stick ); //ListenerResult
          if((app::ui::UIInputListener::DISABLE_ACCESS==x) || (app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x)){
            m_buttonLock = true;
          }
          if(app::ui::UIInputListener::MYSUBVIEW_ONLY_ACCESS==x){
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(pThisUIView);
          }
          else{
            GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(NULL);
          }
        }
      }
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   UpdateResponse()実行時にて入力操作が行われたかを判定
 *
 * @return  入力操作が検知されたならtrueを返却する
 */
//--------------------------------------------------------------------------------------------
bool UIResponder::IsInputAction( void ) const
{
  return m_buttonLock;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  入力の排他設定を行う
*         排他処理を有効にした場合、該当ウインドウ以降の親ウインドウの入力を全て無効化する
*
* @param  isExclusive 排他制御するならtrue、排他制御を解除するならfalseを指定
*/
//--------------------------------------------------------------------------------------------
void UIResponder::SetInputExclusive( bool isExclusive )
{
  m_isExclusive = isExclusive;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  入力の排他設定を取得する
*
* @return isExclusive 自身の親ウインドウ以降の入力を排他制御するならtrueを返却する
*/
//--------------------------------------------------------------------------------------------
bool UIResponder::IsInputExclusive( void ) const
{
  return m_isExclusive;
}

//--------------------------------------------------------------------------------------------
/**
* @brief  アニメ中に操作を排他するかどうかを設定する。(デフォルトはtrue)
*
* @param  isAnimeExclusive  trueならばアニメ中操作排他　falseならばアニメ中でも排他処理しない
*/
//--------------------------------------------------------------------------------------------
void UIResponder::SetAnimationExclusive( bool isAnimeExclusive )
{
  m_isAnimeExclusive  = isAnimeExclusive;
}

//--------------------------------------------------------------------------------------------
/**
* @brief  アニメ中に操作を排他するかどうかを取得する
*
* @return m_isAnimeExclusive trueならば排他　falseならば排他しない
*/
//--------------------------------------------------------------------------------------------
bool UIResponder::IsAnimationExclusive( void ) const
{
  return m_isAnimeExclusive;
}

//--------------------------------------------------------------------------------------------
/**
* @brief  ボタンのアニメ状態を取得する
*
* @return trueアニメ中  falseアニメしていない
*/
//--------------------------------------------------------------------------------------------
bool UIResponder::IsButtonAnimation( void ) const
{
  if( m_button_manager != NULL )
  {
    return m_button_manager->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE;
  }
  return false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのリリースコールバック
 * @param   button_id タッチ＆リリース対象のペインＩＤ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CallbackFuncOnButtonSelected( u32 button_id )
{
  if( IsInputEnabled() )
  {
    if( m_uiListener != NULL )
    {
      if( !m_uiListener->OnLayoutPaneEvent( button_id ) ){
        m_buttonLock = true;
      }
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインの長押しコールバック
 * @param   button_id 長押し通知対象のペインＩＤ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CallbackFuncOnButtonLongPressed( u32 button_id )
{
  if( IsInputEnabled() )
  {
    if( m_uiListener != NULL )
    {
      if( !m_uiListener->OnLayoutPaneLongPressEvent( button_id ) ){
        m_buttonLock = true;
      }
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのアニメーション開始コールバック
 * @param   button_id アニメーションの開始を通知する対象ペインＩＤ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CallbackFuncOnButtonSelectedBeforeAnime( u32 buttonID )
{
  if( IsInputEnabled() )
  {
    if( m_uiListener != NULL )
    {
      m_uiListener->OnLayoutPaneStartingAnimeEvent( buttonID );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのタッチトリガー時コールバック
 * @param   button_id アニメーションの開始を通知する対象ペインＩＤ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CallBackFuncOnButtonTouchTrigger( u32 buttonID )
{
  if( IsInputEnabled() )
  {
    if( m_uiListener != NULL )
    {
      m_uiListener->OnLayoutPaneTouchTriggerEvent( buttonID );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * @param   heap        ヒープ
 * @param   layout      レイアウト
 * @param   paneListPtr 登録ペインテーブル
 * @param   paneNum     登録ペイン数
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CreateButtonManager( app::util::Heap * heap, gfl2::lyt::LytWk* layout, const u32* paneListPtr, u32 paneNum )
{
  m_button_manager = GFL_NEW( heap->GetSystemHeap() ) app::tool::ButtonManager( heap->GetSystemHeap(), NULL, paneNum );

  for( u32 i=0; i<paneNum; i++ )
  {
    m_button_manager->AddButton(
        heap->GetSystemHeap(), paneListPtr[i], layout,  paneListPtr[i],
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        this );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * @param   heap            ヒープクラス
 * @param   layout          レイアウトクラス
 * @param   paneInfoListPtr 登録するベースペイン情報リスト
 * @param   paneNum         登録するペイン数
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CreateButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const ButtonInfoEx* paneListPtr, u32 paneNum )
{
  m_button_manager = GFL_NEW( heap->GetSystemHeap() ) app::tool::ButtonManager( heap->GetSystemHeap(), NULL, paneNum );

  for( u32 i=0; i<paneNum; i++ )
  {
    m_button_manager->AddButton(
        heap->GetSystemHeap(),
        paneListPtr[i].button_id,
        layout,
        paneListPtr[i].picture_pane,
        paneListPtr[i].bound_pane,
        paneListPtr[i].touch_anime_index,
        paneListPtr[i].release_anime_index,
        paneListPtr[i].cancel_anime_index,
        paneListPtr[i].key_select_anime_index,
        paneListPtr[i].active_anime_index,
        paneListPtr[i].passive_anime_index,
        this );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * @param   heap            ヒープクラス
 * @param   paneInfoListPtr 登録するベースペイン情報リスト
 * @param   paneNum         登録するペイン数
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CreateButtonManager( app::util::Heap* heap, const ButtonInfoLytWk* paneListPtr, u32 paneNum )
{
  m_button_manager = GFL_NEW( heap->GetSystemHeap() ) app::tool::ButtonManager( heap->GetSystemHeap(), NULL, paneNum );

  for( u32 i=0; i<paneNum; i++ )
  {
    m_button_manager->AddButton(
        heap->GetSystemHeap(),
        paneListPtr[i].button_id,
        paneListPtr[i].lyt_wk,
        paneListPtr[i].picture_pane,
        paneListPtr[i].bound_pane,
        paneListPtr[i].touch_anime_index,
        paneListPtr[i].release_anime_index,
        paneListPtr[i].cancel_anime_index,
        paneListPtr[i].key_select_anime_index,
        paneListPtr[i].active_anime_index,
        paneListPtr[i].passive_anime_index,
        this );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの解放
 */
//--------------------------------------------------------------------------------------------
void UIResponder::ReleaseButtonManager( void )
{
  CleanupButtonManager();

  GFL_SAFE_DELETE( m_button_manager );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャのクリーンナップ
 */
//--------------------------------------------------------------------------------------------
void UIResponder::CleanupButtonManager( void )
{
  for( ButtonManageEraseList::iterator it = m_buttonEraseList.begin(); it != m_buttonEraseList.end(); )
  {
    ButtonManageEraseNode* eraseInfo = &(*it);
    it = m_buttonEraseList.erase(it);

    if( eraseInfo ){
      if( eraseInfo->m_button_manager ){
        GFL_DELETE eraseInfo->m_button_manager;
      }
      GFL_DELETE eraseInfo;
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの再生成
 *
 * @param   heap        ヒープ
 * @param   layout      レイアウト
 * @param   paneListPtr 登録ペインテーブル
 * @param   paneNum     登録ペイン数
 */
//--------------------------------------------------------------------------------------------
void UIResponder::ReloadButtonManager( app::util::Heap * heap, gfl2::lyt::LytWk* layout, const u32* paneListPtr, u32 paneNum )
{
  CleanupButtonManager();

  // 削除予約リストに登録する(即座にボタンマネージャを削除しては駄目！）
  if( m_button_manager != NULL )
  {
    ButtonManageEraseNode* eraseInfo = GFL_NEW( heap->GetSystemHeap() ) ButtonManageEraseNode();
    m_button_manager->LockButtonControl();
    eraseInfo->m_button_manager = m_button_manager;
    m_buttonEraseList.push_back( eraseInfo );
    m_button_manager = NULL;
  }

  CreateButtonManager( heap, layout, paneListPtr, paneNum );
}
void UIResponder::ReloadButtonManager( app::util::Heap* heap, gfl2::lyt::LytWk* layout, const ButtonInfoEx* paneListPtr, u32 paneNum )
{
  CleanupButtonManager();

  // 削除予約リストに登録する(即座にボタンマネージャを削除しては駄目！）
  if( m_button_manager != NULL )
  {
    ButtonManageEraseNode* eraseInfo = GFL_NEW( heap->GetSystemHeap() ) ButtonManageEraseNode();
    m_button_manager->LockButtonControl();
    eraseInfo->m_button_manager = m_button_manager;
    m_buttonEraseList.push_back( eraseInfo );
    m_button_manager = NULL;
  }

  CreateButtonManager( heap, layout, paneListPtr, paneNum );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの取得
 *
 * @return  ボタンマネージャのインスタンス
 */
//--------------------------------------------------------------------------------------------
app::tool::ButtonManager* UIResponder::GetButtonManager( void )
{
  return m_button_manager;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   カーソル
 *
 * @param   key   十字ボタン情報
 * @param   stick アナログスティック情報
 *
 * @return  移動方向をあらわすビットフラグ
 *          gfl::ui::BUTTON_UP, gfl::ui::BUTTON_DOWN, gfl::ui::BUTTON_LEFT, gfl::ui::BUTTON_RIGHT
 *          のビットフラグが返却される
 */
//--------------------------------------------------------------------------------------------
u32 UIResponder::GetDirectionFlag( const gfl2::ui::Button& key, const gfl2::ui::VectorDevice& stick )
{
  u32 cursorDir = 0;

  // キー入力
  if( (stick.GetY() > STICK_INPUT_VALUE) || key.IsHold(gfl2::ui::BUTTON_UP) ){
    cursorDir |= gfl2::ui::BUTTON_UP;
  }
  else if( (stick.GetY() < -STICK_INPUT_VALUE) || key.IsHold(gfl2::ui::BUTTON_DOWN) ){
    cursorDir |= gfl2::ui::BUTTON_DOWN;
  }
  if( (stick.GetX() < -STICK_INPUT_VALUE) || key.IsHold(gfl2::ui::BUTTON_LEFT) ){
    cursorDir |= gfl2::ui::BUTTON_LEFT;
  }
  else if( (stick.GetX() > STICK_INPUT_VALUE) || key.IsHold(gfl2::ui::BUTTON_RIGHT) ){
    cursorDir |= gfl2::ui::BUTTON_RIGHT;
  }

  return cursorDir;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 十字キー(CrossKey)、ボタン(Button)にスティック（Stick）の操作を加える
 *
 * @param is_on  加える（デフォルト） false 加えない
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetStickEmulationMode( bool is_on )
{
  if( is_on )
  {
    m_button  = m_uiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
    m_key     = m_uiDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  }
  else
  {
    m_button  = m_uiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
    m_key     = m_uiDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ボタンアニメ中にキー入力を効かなくするかどうか
 *
 * @param is_on true  許可  false不許可（デフォルト）
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetInputExclusiveKeyByButtonAnimation( bool is_on )
{
  m_isKeyExclusiveByButtonAnime = is_on;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンの有効設定
 *
 * @param   buttonID  ボタン番号
 * @param   isEnable  ボタン有効フラグ
 * @param   isAnime   trueの指定によりボタンアニメーションを再生
 */
//--------------------------------------------------------------------------------------------
void UIResponder::SetButtonEnable( u32 buttonID, bool isEnable, bool isAnime )
{
  if( m_button_manager != NULL )
  {
    if( isEnable ){
      m_button_manager->SetButtonActive( buttonID, isAnime );
    }
    else{
      m_button_manager->SetButtonPassive( buttonID, isAnime );
    }
  }
}



}    // ui
}    // app
