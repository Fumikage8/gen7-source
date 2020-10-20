//=============================================================================
/**
 * @file app_tool_ButtonManager.cpp
 * @brief ボタン管理
 * @author obata_toshihiro -> Toru=Nagihashi
 * @date 2011.07.06 -> 2012.05.01
 */
//=============================================================================
#include "AppLib/include/Tool/app_tool_ButtonManager.h"
#include "AppLib/include/Util/app_util_Common.h"

#include <Sound/include/Sound.h>

#include <math/include/gfl2_Rect.h>
#include <math/include/gfl2_math_convert.h>


// ビルドを通すため
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(snd)
GFL_NAMESPACE_BEGIN(SoundSystem)

static u32 INVALID_SOUND_ID = 0;

GFL_NAMESPACE_END(SoundSystem)
GFL_NAMESPACE_END(snd)
GFL_NAMESPACE_END(gfl2)

// ビルドを通すため

namespace app {
  namespace tool {

    const ButtonManager::ButtonId ButtonManager::BUTTON_ID_NULL = 0xffffffff;                   //!< ボタンの無効値
    const u32 ButtonManager::ANIMATION_NULL = 0xffffffff;                        //!< アニメーションの無効値
    const u32 ButtonManager::SE_NULL = gfl2::snd::SoundSystem::INVALID_SOUND_ID;  //!< 選択音の無効値
    const u32 ButtonManager::DEFAULT_LONG_PRESS_FRAME = app::util::DEFAULT_GET_COUNT_30F;   //!< 長押しと判定するフレーム数
    const f32 ButtonManager::MOVE_PANE_DISTANCE = 7.0f;                          //!< タッチ操作キャンセル判定距離

#define BUTTON_MANAGER_ACCELERATION //高速化


#if PM_DEBUG
#ifdef DEBUG_ONLY_FOR_nagihashi_toru
//#define DEBUG_BUTTON_MANAGER_TICK
#endif

    enum
    {
      BUTTON_MANAGER_TICK_SYS_UPDATE,
      BUTTON_MANAGER_TICK_MAX,
    };
#endif
    
#ifdef DEBUG_BUTTON_MANAGER_TICK  
    GFL_DEBUG_TICK_SETUP(BUTTON_MANAGER, BUTTON_MANAGER_TICK_MAX );
#endif

#if PM_DEBUG
    u8 ButtonManager::m_isVisibleBoundingPane = 0;
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param buttonID             自身のID
     * @param layout_work          自身が所属するレイアウトワーク
     * @param pane_index           自身のボタンを表すペイン
     * @param bound_pane_index     当たり判定を表すペイン
     * @param null_anime_index     自身のアニメ無効値
     * @param hold_anime_index     ホールドタッチされた際のアニメ
     * @param active_anime_index   有効状態にするアニメ
     * @param passive_anime_index  無効状態にするアニメ
     * @param selectSE             選択時のSE
     * @param callback_target      選択時のコールバック対象
     */
    //-------------------------------------------------------------------------
    ButtonManager::Button::Button(
        ButtonId buttonID,
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index,
        gfl2::lyt::LytPaneIndex bound_pane_index,
        u32 null_anime_index,
        u32 hold_anime_index,
        u32 release_anime_index,
        u32 cancel_anime_index,
        u32 select_anime_index,
        u32 active_anime_index,
        u32 passive_anime_index,
        u32 selectSE,
        IButtonCallback* callback_target ) :
      m_ID( buttonID ),
      m_pLayoutWork( layout_work ),
      m_paneIndex( layout_work->GetPane(pane_index) ),
      m_boundPaneIndex( layout_work->GetPane(bound_pane_index) ),
      m_nullAnimeIndex( null_anime_index ),
      m_holdAnimeIndex( hold_anime_index ),
      m_releaseAnimeIndex( release_anime_index ),
      m_cancelAnimeIndex( cancel_anime_index ),
      m_selectAnimeIndex( select_anime_index ),
      m_activeAnimeIndex( active_anime_index ),
      m_passiveAnimeIndex( passive_anime_index ),
      m_selectSE( selectSE ),
      m_pCallbackTarget( callback_target ),
      m_state( STATE_RELEASE ),
      m_isActive( true ),
      m_holdCount( 0 ),
      m_longPressFrame( 0 ),
      m_bindKey( gfl2::ui::BUTTON_INVALID ),
      m_bindKeyMode( app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER ),
      m_selectType( SELECT_TYPE_RELEASE ),
      m_notyfiTimming( NOTIFY_AFTER_ANIME ),
      m_seq( SEQ_INPUT_CHECK ),
      m_isSelected( false ),
      m_isHolding( false ),
      m_isCancel( false ),
      m_isLongHolding( false ),
      m_old_touch_x( 0 ),
      m_old_touch_y( 0 ),
      m_button_mode( BUTTON_MODE_FIX ),
      m_inputEnable( true ),
      m_use_visible_of_root(false),
      m_isKeySelected(false),
      m_linkButton(NULL),
      m_isPlayLinkAnime( false )
    {
#ifdef DEBUG_BUTTON_MANAGER_TICK  
      GFL_DEBUG_TICK_SET_FLAG(BUTTON_MANAGER, true );
#endif
    }

    ButtonManager::Button::Button(
        ButtonId buttonID,
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPane* pane_index,
        gfl2::lyt::LytPane* bound_pane_index,
        u32 null_anime_index,
        u32 hold_anime_index,
        u32 release_anime_index,
        u32 cancel_anime_index,
        u32 select_anime_index,
        u32 active_anime_index,
        u32 passive_anime_index,
        u32 selectSE,
        IButtonCallback* callback_target ) :
      m_ID( buttonID ),
      m_pLayoutWork( layout_work ),
      m_paneIndex( pane_index ),
      m_boundPaneIndex( bound_pane_index ),
      m_nullAnimeIndex( null_anime_index ),
      m_holdAnimeIndex( hold_anime_index ),
      m_releaseAnimeIndex( release_anime_index ),
      m_cancelAnimeIndex( cancel_anime_index ),
      m_selectAnimeIndex( select_anime_index ),
      m_activeAnimeIndex( active_anime_index ),
      m_passiveAnimeIndex( passive_anime_index ),
      m_selectSE( selectSE ),
      m_pCallbackTarget( callback_target ),
      m_state( STATE_RELEASE ),
      m_isActive( true ),
      m_holdCount( 0 ),
      m_longPressFrame( 0 ),
      m_bindKey( gfl2::ui::BUTTON_INVALID ),
      m_bindKeyMode( app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER ),
      m_selectType( SELECT_TYPE_RELEASE ),
      m_notyfiTimming( NOTIFY_AFTER_ANIME ),
      m_seq( SEQ_INPUT_CHECK ),
      m_isSelected( false ),
      m_isHolding( false ),
      m_isCancel( false ),
      m_isLongHolding( false ),
      m_old_touch_x( 0 ),
      m_old_touch_y( 0 ),
      m_button_mode( BUTTON_MODE_FIX ),
      m_inputEnable( true ),
      m_use_visible_of_root(false),
      m_isKeySelected(false),
      m_linkButton(NULL),
      m_isPlayLinkAnime( false )
    {
#ifdef DEBUG_BUTTON_MANAGER_TICK  
      GFL_DEBUG_TICK_SET_FLAG(BUTTON_MANAGER, true );
#endif
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ButtonManager::Button::~Button()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力検知
     * @param touchPanel  タッチパネル
     * @param button  ボタン
     * @retval true 入力を検知をした falseしていない（検知したら、下の動作関数を呼んでください）
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateInput( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input )
    {
      if( m_seq == SEQ_INPUT_CHECK )
      {
        m_isSelected = false;
        m_isHolding = false;
        m_isCancel  = false;
        m_isLongHolding = false;
      }

      switch( m_selectType )
      {
      default:
        GFL_ASSERT(0);
      case SELECT_TYPE_TRIGGER:
      case SELECT_TYPE_REPEATEDLY:
        return this->UpdateInput_TRIGGER( touchPanel, button, is_force_input );
      case SELECT_TYPE_RELEASE:
        return this->UpdateInput_RELEASE( touchPanel, button, is_force_input );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 動作(アニメ待ち等)
     * @param touchPanel タッチパネル（押し続け検知のため）
     * @retval true 動作完了（アニメ待ち等を終えた） false動作中
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateMove( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button )
    {
      m_isSelected = false;

      switch( m_selectType )
      {
      default:
        GFL_ASSERT(0);
      case SELECT_TYPE_TRIGGER:
        return this->UpdateMove_TRIGGER( touchPanel );
      case SELECT_TYPE_RELEASE:
        if( m_button_mode == BUTTON_MODE_FIX )
        {
          return this->UpdateMove_RELEASE_FIX( touchPanel );
        }
        else
        {
          return this->UpdateMove_RELEASE_SCROLL( touchPanel );
        }
    
      case SELECT_TYPE_REPEATEDLY:
        this->UpdateInput_TRIGGER( touchPanel, button, true );
        return this->UpdateMove_TRIGGER( touchPanel );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押された通知を取得
     * @retval true 押された false 押されていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsSelected( void ) const
    {
      return m_isSelected;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押された通知を取得
     * @retval true 押された false 押されていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsHolding( void ) const
    {
      return m_isHolding;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キャンセルされた通知を取得
     * @retval true 押された false 押されていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsCancel( void ) const
    {
      return m_isCancel;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタン識別子を取得する
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::Button::GetId( void ) const
    {
      return m_ID;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 見えるかどうかを設定する
     * @param visibility  true なら見える
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetVisibility( bool visibility )
    {
      gfl2::lyt::LytPane* pane = m_paneIndex;
      gfl2::lyt::LytPane* bound_pane = m_boundPaneIndex;
      pane->SetVisible( visibility );
      bound_pane->SetVisible( visibility );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの可視判定
     * @retval true   見える
     * @retval false  見えない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsVisible( void ) const
    {
      gfl2::lyt::LytPane* pane = m_paneIndex;
      gfl2::lyt::LytPane* bound_pane = m_boundPaneIndex;
      
      bool is_root_visible  = true;
      
      if( m_use_visible_of_root )
      { 
        gfl2::lyt::LytPane * basePane = m_paneIndex;
        gfl2::lyt::LytPane * rootPane = m_pLayoutWork->GetLayoutCore()->GetRootPane();
        for ever{
          gfl2::lyt::LytPane * parentPane = basePane->GetParent();
          // 親がいない or 親がルートペイン
          if( parentPane == NULL || parentPane == rootPane ){
            break;
          }
          if( parentPane->IsVisible() == false )
          {
            is_root_visible = false;
          }
          basePane = parentPane;
        }
      }

      return pane->IsVisible() && bound_pane->IsVisible() && is_root_visible;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの当たり判定
     * @param touch_x  タッチx座標
     * @param touch_y  タッチy座標
     * @retval true   HIT した
     * @retval false  HIT してない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsHit( u16 touch_x, u16 touch_y ) const
    {
      if( m_pLayoutWork->IsHitCheckOK() )
      {
        return m_pLayoutWork->GetHitPane( touch_x, touch_y, m_boundPaneIndex );
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択可能かどうかを調べる
     * @retval true   選択できる
     * @retval false  選択できない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsActive( void ) const
    {
      return m_isActive;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択可能かどうかを設定する
     * @param activity   true なら選択できる
     * @param isAnime    true ならアニメーションを行う
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetActivity( bool activity, bool isAnime )
    {
      if( m_isActive != activity )
      {
        this->SetActivityForce(activity, isAnime);
      }
    }
          
    void ButtonManager::Button::SetActivityForce( bool activity, bool isAnime )  // 内部状態がactivityフラグ状態であっても、指定したactivityフラグ状態の処理を行う
    {
      m_isActive = activity;
      if( isAnime )
      {
        u32 start_anime = activity ? m_activeAnimeIndex : m_passiveAnimeIndex;
        u32 stop_anime = activity ? m_passiveAnimeIndex : m_activeAnimeIndex;
        
        this->StartAnime( start_anime );
        if( start_anime != m_nullAnimeIndex ) 
        {
          m_pLayoutWork->SetAutoAnimeSpeed( start_anime, 1 );
        }
        this->StopAnime( stop_anime );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択可能かどうかを設定する
     *        ボタンを有効/無効を切り替え、アニメは最終フレームにする。
     * @param activity   true なら選択できる
     * @param isAnime    true ならアニメーションを行う
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetActivityAnimeFrameLast( bool activity )
    {
      this->SetActivity(activity, true);

      u32 start_anime = activity ? m_activeAnimeIndex : m_passiveAnimeIndex;
 
      if( start_anime != m_nullAnimeIndex ) 
      {
        f32 maxFrame = m_pLayoutWork->GetAnimeMaxFrame(start_anime);
        // @fix MMCat[654],MMCat[691],GFMMCat[685]: 最終フレームの指定を修正
//        maxFrame = maxFrame - 1.0f;
//        if(maxFrame < 0.0f) maxFrame = 0.0f;
        m_pLayoutWork->SetAutoAnime( start_anime, false );
        m_pLayoutWork->SetAnimeFrame( start_anime, maxFrame );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力検知  トリガー検知版
     * @param touchPanel  タッチパネル
     * @param button  ボタン
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateInput_TRIGGER( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input )
    {
      if( m_seq == SEQ_INPUT_CHECK || is_force_input )
      {
        if( this->IsVisible() && this->IsActive() && this->IsInputEnable() )
        {
          //タッチされた
          if( touchPanel && touchPanel->IsTouchTrigger()
              && this->IsHit( touchPanel->GetX(), touchPanel->GetY() ) )
          {

            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallBackFuncOnButtonTouchTrigger( m_ID );
            }

            m_seq = SEQ_INPUT_DECIDE;
            return true;
          }
          else if( (button && this->IsInputButton( button )) 
              && (touchPanel == NULL ||!touchPanel->IsTouch()) )
          {
            m_isKeySelected = true;
            m_seq = SEQ_INPUT_DECIDE;
            return true;
          }
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 動作(アニメ待ち等)  トリガー検知版
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateMove_TRIGGER( const gfl2::ui::TouchPanel* touchPanel )
    {
      GFL_UNUSED( touchPanel );

      switch( m_seq )
      {
      default:
        GFL_ASSERT(0);
      case SEQ_INPUT_CHECK:
        // UpdateInput_TRIGGER関数にてチェック中
        break;
      case SEQ_INPUT_DECIDE:
        m_seq = SEQ_ANIME_START;
        /*  fall */
      case SEQ_ANIME_START:
        this->Select();
        if( m_notyfiTimming == NOTIFY_START_ANIME ) {
          m_isSelected = true;  // 即座に通知
          if( m_pCallbackTarget ) {
            m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
          }
        }
        m_seq = SEQ_ANIME_WAIT;
        /*  fall */
      case SEQ_ANIME_WAIT:
        if( this->IsSelectAnimeEnd() ) {
          if( m_notyfiTimming == NOTIFY_AFTER_ANIME ) {
            m_isSelected = true;
            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
            }
          }
          m_seq = SEQ_INPUT_CHECK;
          return true;
        }
        break;
      }

      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力検知  リリース検知版
     * @param touchPanel  タッチパネル
     * @param button  ボタン
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateInput_RELEASE( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input )
    {
      if( m_seq == SEQ_INPUT_CHECK || is_force_input )
      {
        if( this->IsVisible() && this->IsActive() && this->IsInputEnable() )
        {
          //タッチされた
          if( touchPanel && touchPanel->IsTouchTrigger() && this->IsHit( touchPanel->GetX(), touchPanel->GetY() ) )
          {
            m_old_touch_x = touchPanel->GetX();
            m_old_touch_y = touchPanel->GetY();

            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallBackFuncOnButtonTouchTrigger( m_ID );
            }

            this->GetTranslate( &m_trigger_pane_pos );
            
//            NAGI_PRINT( "trg pane%d x%f y%f\n", m_paneIndex, m_trigger_pane_pos.x, m_trigger_pane_pos.y );

            m_seq = SEQ_INPUT_DECIDE;
            this->Hold();
            m_isHolding = true;
            return true;
          }
          else if( (button && this->IsInputButton( button )) 
              && (touchPanel == NULL || !touchPanel->IsTouch()) )
          {
            m_isKeySelected = true;
            m_seq = SEQ_ANIME_START;
            return true;
          }
        }
      }

      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief 動作(アニメ待ち等)  リリース検知FIX版
     * @param touchPanel  タッチパネル（Hold検知のため）
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateMove_RELEASE_FIX( const gfl2::ui::TouchPanel* touchPanel )
    {
      bool is_touch = false;
      u16 touch_x = 0;
      u16 touch_y = 0;
      bool enable = true;
      if( touchPanel )
      {
        is_touch  = touchPanel->IsTouch();
        touch_x = touchPanel->GetX();
        touch_y = touchPanel->GetY();
      }
      else
      {
        //入力がないときは押し続けも不許可
        enable = false;
      }

      switch( m_seq )
      {
      default:
        GFL_ASSERT(0);
      case SEQ_INPUT_CHECK:
        // UpdateInput_RELEASE関数にてチェック中
        break;
      case SEQ_INPUT_DECIDE:
        if( is_touch )
        {
          // タッチされている
          if( m_isHolding )
          {
            //ここでアニメーションにより
            //タッチ範囲がかわるため、Holdアニメ、Releaseアニメが繰り返される
            //そのためHold時の座標をとり２点間を一定距離はなれないとReleaseアニメに
            //行かないようにする

            if( this->IsActive() &&
                this->IsHit( touch_x, touch_y ) )
            {
              this->Hold();   // @note 下から移動

              if( m_state == STATE_HOLD )
              {
                if( m_holdCount < m_longPressFrame )
                {
                  m_holdCount++;
                  if( m_longPressFrame <= m_holdCount )
                  {
                    m_isLongHolding = true;
                    if( m_pCallbackTarget )
                    {
                      m_pCallbackTarget->CallbackFuncOnButtonLongPressed( m_ID );
                    }
                  }
                }
              }

              //if( this->IsLeaveOldDistance(touch_x,touch_y) )
              {
                m_old_touch_x = touch_x;
                m_old_touch_y = touch_y;
//                this->Hold();   // @note 長押しのコールバックでボタンをリセットする場合、邪魔になるので上に移動
              }
            }
            else if( this->IsLeaveOldDistance(touch_x,touch_y) )
            {
              m_old_touch_x = touch_x;
              m_old_touch_y = touch_y;
              this->Release();
            }
          }
          break; //fallさせない
        }
        else if( m_isHolding )
        {
          // タッチが離された
          m_isHolding = false;
          if( this->Release() && this->IsActive() && enable )
          {
            m_seq = SEQ_ANIME_START;  // アニメ終了後に通知

            //このときにしかfallしない
          }
          else
          {
            break;  //fallさせない
          }
        }
        else
        {
          m_isCancel  = true;
          //キャンセルされた
          if( m_pCallbackTarget ) {
            m_pCallbackTarget->CallbackFuncOnButtonCancel( m_ID );
          }
          m_seq = SEQ_INPUT_CHECK;
          return true; //fallさせない
        }
        /* fall */
      case SEQ_ANIME_START:
        this->Select();
        if( m_notyfiTimming == NOTIFY_START_ANIME ) {
          m_isSelected = true;  // 即座に通知
          if( m_pCallbackTarget ) {
            m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
          }
        }
        m_seq = SEQ_ANIME_WAIT;
        /* fall */
      case SEQ_ANIME_WAIT:
        if( this->IsSelectAnimeEnd() ) {
          if( m_notyfiTimming == NOTIFY_AFTER_ANIME ) {
            m_isSelected = true;
            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
            }
          }
          m_seq = SEQ_INPUT_CHECK;
          return true;
        }
        break;
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief 動作(アニメ待ち等)  リリース検知SCROLL版
     * @param touchPanel  タッチパネル（Hold検知のため）
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::UpdateMove_RELEASE_SCROLL( const gfl2::ui::TouchPanel* touchPanel )
    {
      bool is_touch = false;
      u16 touch_x = 0;
      u16 touch_y = 0;
      if( touchPanel )
      {
        is_touch  = touchPanel->IsTouch();
        touch_x = touchPanel->GetX();
        touch_y = touchPanel->GetY();
      }
      else
      {
        //入力がないときは押し続けも不許可
        m_isHolding = false;
      }

      switch( m_seq )
      {
      default:
        GFL_ASSERT(0);
      case SEQ_INPUT_CHECK:
        // UpdateInput_RELEASE関数にてチェック中
        break;
      case SEQ_INPUT_DECIDE:
        if( is_touch )
        {
          if( this->IsMovePaneDisitance() )
          {
            m_isHolding = false;
            this->Release();

            m_isCancel  = true;
            //キャンセルされた
            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallbackFuncOnButtonCancel( m_ID );
            }
            m_seq = SEQ_INPUT_CHECK;
            return true;
          }

          // タッチされている
          if( m_isHolding )
          {
            if( this->IsActive() &&
                this->IsHit( touch_x, touch_y ) )
            {
              this->Hold();   // @note 下から移動

              if( m_state == STATE_HOLD )
              {
                if( m_holdCount < m_longPressFrame )
                {
                  m_holdCount++;
                  if( ( m_longPressFrame <= m_holdCount ) &&
                      ( m_pCallbackTarget ) )
                  {
                    m_pCallbackTarget->CallbackFuncOnButtonLongPressed( m_ID );
                  }
                }
              }

//              this->Hold();   // @note 長押しのコールバックでボタンをリセットする場合、邪魔になるので上に移動
            }
            else
            {
              m_isHolding = false;
              this->Release();

              m_isCancel  = true;
              //キャンセルされた
              if( m_pCallbackTarget ) {
                m_pCallbackTarget->CallbackFuncOnButtonCancel( m_ID );
              }
              m_seq = SEQ_INPUT_CHECK;
              return true;
            }
          }
          break; //fallさせない
        }
        else if( m_isHolding )
        {
          if( this->IsMovePaneDisitance() )
          {
            m_isHolding = false;
            this->Release();

            m_isCancel  = true;
            //キャンセルされた
            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallbackFuncOnButtonCancel( m_ID );
            }
            m_seq = SEQ_INPUT_CHECK;
            return true;
          }

          // タッチが離された
          m_isHolding = false;
          if( this->Release() && this->IsActive() )
          {
            m_seq = SEQ_ANIME_START;  // アニメ終了後に通知

            //このときにしかfallしない
          }
          else
          {
            break;  //fallさせない
          }
        }
        else
        {
          m_isCancel  = true;
          //キャンセルされた
          if( m_pCallbackTarget ) {
            m_pCallbackTarget->CallbackFuncOnButtonCancel( m_ID );
          }
          m_seq = SEQ_INPUT_CHECK;
          return true; //fallさせない
        }
        /* fall */
      case SEQ_ANIME_START:
        this->Select();
        if( m_notyfiTimming == NOTIFY_START_ANIME ) {
          m_isSelected = true;  // 即座に通知
          if( m_pCallbackTarget ) {
            m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
          }
        }
        m_seq = SEQ_ANIME_WAIT;
        /* fall */
      case SEQ_ANIME_WAIT:
        if( this->IsSelectAnimeEnd() ) {
          if( m_notyfiTimming == NOTIFY_AFTER_ANIME ) {
            m_isSelected = true;
            if( m_pCallbackTarget ) {
              m_pCallbackTarget->CallbackFuncOnButtonSelected( m_ID );
            }
          }
          m_seq = SEQ_INPUT_CHECK;
          return true;
        }
        break;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アニメーションを開始する
     * @param anime_index  開始するアニメ
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::StartAnime( u32 anime_index )
    {
      // 指定されたアニメーションが無効
      if( anime_index == m_nullAnimeIndex ) {
        return;
      }

      if( m_pLayoutWork->IsBindAnime( anime_index ) == false ) {
        m_pLayoutWork->BindAnime( anime_index );
      }
      m_pLayoutWork->SetAutoAnime( anime_index, true );
      m_pLayoutWork->SetAnimeFrame( anime_index, 0 );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アニメーションを停止する
     * @param anime_index  停止するアニメ
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::StopAnime( u32 anime_index )
    {
      // 指定されたアニメーションが無効
      if( anime_index == m_nullAnimeIndex ) {
        return;
      }

      if( m_pLayoutWork->IsBindAnime( anime_index ) ) {
        m_pLayoutWork->UnBindAnime( anime_index );
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief アニメ終了検知
     * @param anime_index 検知するアニメ
     * @return trueアニメ終了 false終了してない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsEndAnime( u32 anime_index )
    {
      return m_pLayoutWork->IsAnimeEnd(anime_index);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択時のＳＥを設定する
     * @param SEID  選択時に再生するＳＥ
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetSelectSE( u32 SEID )
    {
      m_selectSE = SEID;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 長押しと判定するフレーム数を指定する
     * @param frame フレーム数
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetLongPressFrame( u32 frame )
    {
      m_longPressFrame = frame;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バインド対象のキーを取得する
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsInputButton( const gfl2::ui::Button* button ) const
    {
      switch( m_bindKeyMode )
      {
      case BIND_KEY_MODE_TRIGGER:
        return button->IsTrigger( m_bindKey );
      case BIND_KEY_MODE_HOLD:
        return button->IsHold( m_bindKey );
      case BIND_KEY_MODE_REPEAT:
        return button->IsRepeat( m_bindKey );
      case BIND_KEY_MODE_RELEASE:
        return button->IsRelease( m_bindKey );
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief バインド対象のキーを設定する
     * @param key バインドするキー入力( gfl::ui::BUTTON_XXXX )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetBindKey( u32 key, BindKeyMode mode )
    {
      m_bindKey = key;
      m_bindKeyMode = mode;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの選択方法を変更する
     * @param type  設定する選択方法
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetSelectType( SelectType type )
    {
      m_selectType  = type;
    }


    /**
     * @brief 入力を許可するかどうか
     *
     * @param is_enable true許可（デフォルト）  false不許可
     *
     * @note  falseの場合、アニメ等の更新は動くが、入力検知のみ動かなくなる
     */
    void ButtonManager::SetInputEnable( bool is_enable )
    {
      m_isInputEnable = is_enable;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択されたことを通知するタイミングを変更する
     * @param timming  通知するタイミング
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetNotifyTimming( NotifyTimming timming )
    {
      m_notyfiTimming = timming;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを離す状態へリクエストする
     * @retval  true リクエストを受け付けた false受け付けなかった
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::RequestRelease( void )
    {
      this->Release();
      m_seq = SEQ_INPUT_CHECK;
      return true;

    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを選択状態へリクエストする
     * @retval  true リクエストを受け付けた false受け付けなかった
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::RequestSelect( void )
    {

      if( this->IsVisible() &&
          this->IsActive() )
      {
        if( m_seq == SEQ_INPUT_CHECK )
        {
          m_isKeySelected = true;
          m_seq = SEQ_ANIME_START;
          return true;
        }
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの判定モードを設定する
     *
     * @param mode  ボタンモード
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetButtonMode( ButtonMode mode )
    {
      m_button_mode  = mode;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アニメーション中かどうか
     *
     * @return true アニメ中  falseアニメ
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsAnimation( void ) const
    {
      return m_seq == SEQ_ANIME_WAIT;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの入力有効性をセット
     *
     * @param is_enable true有効  false無効
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SetInputEnable( bool is_enable )
    {
      m_inputEnable = is_enable;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの入力有効性を取得
     *
     * @return 入力有効性
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsInputEnable( void ) const
    {
      return m_inputEnable;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief 長押しの取得
     *
     * @return 長押しをしている
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsLongHolding( void ) const
    {
      return m_isLongHolding && m_isHolding;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief アニメーション中に非表示にした場合等に、アニメを初期値に戻す
     *
     * @param buttonID  ボタンID
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::ResetAnime( void )
    {
      ResetAnime( m_releaseAnimeIndex );  // タッチ
      ResetAnime( m_selectAnimeIndex );   // キー
    }
    void ButtonManager::Button::ResetAnime( u32 anime )
    {
      if( anime == m_nullAnimeIndex ) {
        return;
      }
      if( m_pLayoutWork->IsBindAnime( anime ) == false ) {
        m_pLayoutWork->BindAnime( anime );
      }
      m_pLayoutWork->SetAutoAnime( anime, false );
      m_pLayoutWork->SetAnimeFrame( anime, 0 );
    }

#if PM_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief バウンディングペインの矩形描画情報を登録する (デバッグ機能)
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::Debug_RegisterBoundingPaneRect( u32 buttonIndex)
    {
      if( !this->IsVisible() || !this->IsActive() || !this->IsInputEnable())
      {
        return;
      }

      // バウンディングペインでなくても、
      // ボタンのバウンディングペインとして登録されていることがあるので、GetPaneで取る
      gfl2::lyt::LytPane* pane = m_boundPaneIndex;

      if( pane != NULL)
      {
        gfl2::math::Rect rect;

        // サイズ取得
        nw::lyt::Size size = pane->GetSize();
        gfl2::math::VEC2 scl = pane->GetScale();
        f32 width = static_cast<f32>( size.width) * scl.x;
        f32 height = static_cast<f32>( size.height) * scl.y;
        /* ビルドを通すため
        rect.SetSize( width, height);
        ビルドを通すため(↓に変換) */
        rect.SetWidth( width );
        rect.SetHeight( height );

        gfl2::math::VEC3 pos = pane->GetTranslate();
#if 0
        // 位置取得。非表示ペインの行列計算をしていない可能性もあるので、親子関係を辿る
        gfl::grp::g2d::LytPane* root = m_pLayoutWork->GetLayoutCore()->GetRootPane();
        gfl::grp::g2d::LytPane* base = pane;
        while( 1)
        {
          gfl::grp::g2d::LytPane* parent = base->GetParent();
          if( parent == NULL || root == parent)
          {
            break;
          }
          pos += parent->GetTranslate();
          base = parent;
        } 
#endif
        // タッチ判定は行列で行っているので、行列から取った位置で出す
        gfl2::math::MTX34 mtx = pane->GetGlobalMtx();
        gfl2::math::VEC3 mtxPos = mtx.GetColumn( 3);
        pos.Set( mtxPos.x, mtxPos.y, mtxPos.z);

        // レイアウトエディタでは画面中心が0,0、描画時は画面左上0,0なので位置を変換するための値を用意
        const f32 center_x = static_cast<f32>(DISPLAY_LOWER_WIDTH) * 0.5f;
        const f32 center_y = static_cast<f32>(DISPLAY_LOWER_HEIGHT) * 0.5f;
        // 矩形は描画位置からx右、y下に長さの分だけ描かれるので、描画位置になるよう変換するための値を用意
        const f32 half_len_x = static_cast<f32>(size.width) * 0.5f;
        const f32 half_len_y = static_cast<f32>(size.height) * 0.5f;
        // 水平方向のペインの中心位置を考慮してずらす値を決める
        f32 horizontalPos = 0.0f;
        u8  horizontalType = pane->GetBasePositionH();
        switch( horizontalType)
        {
        case nw::lyt::HORIZONTALPOSITION_LEFT:
          horizontalPos = static_cast<f32>(size.width) * 0.5f;
          break;
        case nw::lyt::HORIZONTALPOSITION_CENTER:
          horizontalPos = 0.0f;
          break;
        case nw::lyt::HORIZONTALPOSITION_RIGHT:
          horizontalPos = -static_cast<f32>(size.width) * 0.5f;
          break;
        }
        // 垂直方向のペインの中心位置を考慮してずらす値を決める
        f32 verticalPos = 0.0f;
        u8  verticalType = pane->GetBasePositionV();
        switch( verticalType)
        {
        case nw::lyt::VERTICALPOSITION_TOP:
          verticalPos = static_cast<f32>(size.height) * 0.5f;
          break;
        case nw::lyt::VERTICALPOSITION_CENTER:
          verticalPos = 0.0f;
          break;
        case nw::lyt::VERTICALPOSITION_BOTTOM:
          verticalPos = -static_cast<f32>(size.height) * 0.5f;
          break;
        }
        // 描画開始位置を決定
        pos.x = center_x + pos.x - half_len_x + horizontalPos;
        pos.y = center_y - pos.y - half_len_y + verticalPos;
        /* ビルドを通すため
        rect.SetPosition( pos.x, pos.y);
        ↓に差し替え */
        rect.SetOriginAndSize( pos.x, pos.y, width, height );

        // 色決め。ちょっとずつずらせればOK
        f32 tmpColor[3] = { 0.0f, 0.0f, 0.0f };
        const f32 offset = 0.20f;
        const f32 max = 0.80f;
        for( u32 j = 0; j < buttonIndex; ++j)
        {
          u32 index = j % 3;
          tmpColor[index] += offset;
          if( tmpColor[index] > max)
          {
            tmpColor[index] -= max;
          }
        }
        gfl2::math::VEC4 color( tmpColor[0], tmpColor[1], tmpColor[2], 0.70f);

        // 登録
        /** ビルドを通すため
        GFL_SINGLETON_INSTANCE(xy_debug::RectDrawManager)->RegisterRectInfo( rect, color);
        ビルドを通すため */
      }
    }
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされていることを通知する
     * @retval true   通知することで状態が変わった
     * @retval false  通知することで状態は変わらなかった
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::Hold( void )
    {
      if( m_state == STATE_RELEASE )
      {
        HoldAnime();
        ResetLinkAnime();
        m_isKeySelected = false;
        m_state = STATE_HOLD;
        m_holdCount = 0;
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @func  HoldAnime
     * @brief タッチ時のアニメを再生
     * @date  2015.04.06
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::HoldAnime(void)
    {
      if( m_isPlayLinkAnime == false )
      {
        this->StopAnime( m_selectAnimeIndex );
        this->StopAnime( m_releaseAnimeIndex );
        this->StopAnime( m_cancelAnimeIndex );
        this->StartAnime( m_holdAnimeIndex );
        m_isPlayLinkAnime = true;
        if( m_linkButton != NULL )
        {
          m_linkButton->HoldAnime();
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief タッチされていないことを通知する
     * @retval true   通知することで状態が変わった
     * @retval false  通知することで状態は変わらなかった
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::Release( void )
    {
      if( m_state == STATE_HOLD )
      {
        ReleaseAnime();
        ResetLinkAnime();
        m_state = STATE_RELEASE;
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @func  ReleaseAnime
     * @brief タッチ解除時のアニメを再生
     * @date  2015.04.06
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::ReleaseAnime( void )
    {
      if( m_isPlayLinkAnime == false )
      {
        this->StopAnime( m_holdAnimeIndex );
        this->StartAnime( m_cancelAnimeIndex );
        m_isPlayLinkAnime = true;
        if( m_linkButton != NULL )
        {
          m_linkButton->ReleaseAnime();
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択されたことを通知する
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::Select( void )
    {
      //アニメ開始時に呼ぶ
      if( m_pCallbackTarget ) {
        m_pCallbackTarget->CallbackFuncOnButtonSelectedBeforeAnime( m_ID );
      }

      SelectAnime( m_isKeySelected );
      ResetLinkAnime();
      m_isKeySelected = false;

      // SE 再生
      if( m_selectSE != ButtonManager::SE_NULL ) {
        Sound::PlaySE( m_selectSE );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @func  SelectAnime
     * @brief 選択アニメを再生
     * @date  2015.04.06
     *
     * @param is_key_select   true = キー選択
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::SelectAnime( bool is_key_select )
    {
      if( m_isPlayLinkAnime == false )
      {
        // キャンセルアニメ停止 ( タッチリリース時にRelease()が呼ばれるため )
        this->StopAnime( m_cancelAnimeIndex );

        // 選択アニメ開始
        if( is_key_select != false )
        {
          this->StopAnime( m_releaseAnimeIndex );
          this->StartAnime( m_selectAnimeIndex );
        }
        else
        {
          this->StopAnime( m_selectAnimeIndex );
          this->StartAnime( m_releaseAnimeIndex );
        }

        m_isPlayLinkAnime = true;

        if( m_linkButton != NULL )
        {
          m_linkButton->SelectAnime( is_key_select );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @func  ResetLinkAnime
     * @brief リンクで再生したアニメのフラグを下げる
     * @date  2015.04.06
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::ResetLinkAnime(void)
    {
      if( m_isPlayLinkAnime != false )
      {
        m_isPlayLinkAnime = false;
        if( m_linkButton != NULL )
        {
          m_linkButton->ResetLinkAnime();
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択アニメが終了したかどうかを調べる
     * @retval true   終了した
     * @retval false  終了していない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsSelectAnimeEnd( void ) const
    {
      if( IsSelectAnimeEnd( m_selectAnimeIndex ) &&
          IsSelectAnimeEnd( m_releaseAnimeIndex ) )
      {
        return true;
      }
      return false;
    }
    bool ButtonManager::Button::IsSelectAnimeEnd( u32 anime ) const
    {
      if( anime == m_nullAnimeIndex ) {
        return true;  // 選択アニメが無効
      }
      return !( m_pLayoutWork->IsAnime(anime) && m_pLayoutWork->IsBindAnime(anime) && !m_pLayoutWork->IsAnimeEnd(anime) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief タッチ位置から移動したかどうか調べる
     * @param x タッチ座標X
     * @param y タッチ座標Y
     * @retval true   終了した
     * @retval false  終了していない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsLeaveOldDistance( u16 x, u16 y )
    {
      static const int RELEASE_LEAVE_DISTANCE = 6;
      int x2 = x;
      int y2 = y;
      int x1 = m_old_touch_x;
      int y1 = m_old_touch_y;
      //√(x2-x1)^2 + (y2-y1)^2
      return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) > RELEASE_LEAVE_DISTANCE*RELEASE_LEAVE_DISTANCE;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ペインが一定距離移動したかを調べる
     *
     * @return true移動した false移動しなかった
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::Button::IsMovePaneDisitance( void )
    {
      gfl2::math::VEC3 now_pane_pos;
      this->GetTranslate( &now_pane_pos );
      
//      NAGI_PRINT( "now pane%d x%f y%f\n", m_paneIndex, now_pane_pos.x, now_pane_pos.y );

      return gfl2::math::VEC3Dist( &now_pane_pos, &m_trigger_pane_pos ) >= MOVE_PANE_DISTANCE;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief トランスレートを取得する
     * @param pos トランスレート受け取り
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::GetTranslate( gfl2::math::VEC3* pos ) const
    {
      
      gfl2::lyt::LytPane * basePane = m_paneIndex;
      gfl2::lyt::LytPane * rootPane = m_pLayoutWork->GetLayoutCore()->GetRootPane();
      *pos = basePane->GetTranslate();
      
      for ever{
        gfl2::lyt::LytPane * parentPane = basePane->GetParent();
        // 親がいない or 親がルートペイン
        if( parentPane == NULL || parentPane == rootPane ){
          break;
        }
        *pos += parentPane->GetTranslate();
        basePane = parentPane;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @func  Link
     * @brief リンクさせるボタンを設定
     * @date  2015.04.06
     *
     * @param button  リンクさせるボタン
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Button::Link( Button * button )
    {
      m_linkButton = button;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  コンストラクタ
     *
     * @param buttons ボタン配列
     * @param button_num  ボタンの登録数（AddButtonで変わるのでconst参照でもらってます）
     */
    //-------------------------------------------------------------------------
    ButtonManager::SingleProcessManager::SingleProcessManager( Button** buttons, const u32 & button_num ) :
      IProcessManager( buttons, button_num ),
      m_seq( SEQ_INPUT_WAIT ),
      m_selectedButtonIndex( m_buttonCount )
    {
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  デストラクタ
     */
    //-------------------------------------------------------------------------
    ButtonManager::SingleProcessManager::~SingleProcessManager()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  更新
     * @param pUiTouchPanel タッチパネル
     * @param pUiButton     ボタン
     * @param isInputEnable 入力許可
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SingleProcessManager::Update( const gfl2::ui::TouchPanel* pUiTouchPanel, const gfl2::ui::Button* pUiButton, bool isInputEnable )
    {
      
#ifdef DEBUG_BUTTON_MANAGER_TICK  
      GFL_DEBUG_TICK_START(BUTTON_MANAGER, BUTTON_MANAGER_TICK_SYS_UPDATE, "マネージャー更新" );
#endif
      switch( m_seq )
      {
      default:
        GFL_ASSERT(0);
      case SEQ_INPUT_WAIT:
        m_selectedButtonIndex = m_buttonCount;
        if( isInputEnable )
        {
#ifdef BUTTON_MANAGER_ACCELERATION
          if( (pUiTouchPanel && pUiTouchPanel->IsTouch()) 
              || (pUiButton && pUiButton->GetHold()) )
#endif
          {
            for( u32 i=0; i<m_buttonCount; ++i ) {
              if( m_buttons[i]->UpdateInput( pUiTouchPanel, pUiButton ) ) {
                m_selectedButtonIndex = i;
                m_seq = SEQ_ANIME_WAIT;
                break;
              }
            }
          }
        }
        break;

      case SEQ_ANIME_WAIT:
        {
          const gfl2::ui::TouchPanel* touchPanel = NULL;
          const gfl2::ui::Button* ui_button  = NULL;
          if( isInputEnable )
          {
            touchPanel  = pUiTouchPanel;
            ui_button   = pUiButton;
          }
          if( m_buttons[ m_selectedButtonIndex ]->UpdateMove( touchPanel, ui_button ) )
          {
            m_seq = SEQ_INPUT_WAIT;
          }
        }
        break;
      }
      
#ifdef DEBUG_BUTTON_MANAGER_TICK  
      GFL_DEBUG_TICK_END(BUTTON_MANAGER, BUTTON_MANAGER_TICK_SYS_UPDATE );
#endif
      
#ifdef DEBUG_BUTTON_MANAGER_TICK  
      {
        if( true )
        {
          GFL_DEBUG_TICK_DUMP_ALL(BUTTON_MANAGER);
        }
      }
#endif
      
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンがアニメ中かチェック
     *
     * @return true アニメ中  falseアニメしてない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::SingleProcessManager::IsButtonAnimation( void ) const
    {
      return m_seq == SEQ_ANIME_WAIT;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  押し続けボタンIDの取得
     *
     * @return 押し続けボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::SingleProcessManager::GetHoldingButtonId( void ) const
    {
      if( this->IsButtonHolding() ) {
        return m_buttons[ m_selectedButtonIndex ]->GetId();
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  選択ボタンIDの取得
     *
     * @return 選択ボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::SingleProcessManager::GetSelectedButtonId( void ) const
    {
      if( this->IsButtonSelected() ) {
        return m_buttons[ m_selectedButtonIndex ]->GetId();
      }
      return BUTTON_ID_NULL;  // 選択されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  キャンセルボタンIDの取得
     *
     * @return キャンセルボタンID
     * @note  キャンセルとは、押したあとボタン範囲から離して、タッチリリースすることです
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::SingleProcessManager::GetCancelButtonId( void ) const
    {
      if( this->IsButtonCancel() ) {
        return m_buttons[ m_selectedButtonIndex ]->GetId();
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  長押しボタンIDの取得
     *
     * @return 長押しボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::SingleProcessManager::GetLongHoldingButtonId( void ) const
    {
      if( this->IsButtonLongHolding() ) {
        return m_buttons[ m_selectedButtonIndex ]->GetId();
      }
      return BUTTON_ID_NULL;  // 押されていない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタン選択中かどうか
     *
     * @return true選択中 false選択していない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::SingleProcessManager::IsButtonSelected( void ) const
    {
      //↓をm_seq == SEQ_ANIME_WAITにすると、buttonUpdateMoveでreturnを返したフレームを検知しないため
      if( m_selectedButtonIndex < m_buttonCount )
      {
        return m_buttons[ m_selectedButtonIndex ]->IsSelected();
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタン押し続け中かどうか
     *
     * @return true押し続け中 false押し続けしていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::SingleProcessManager::IsButtonHolding( void ) const
    {
      //↓をm_seq == SEQ_ANIME_WAITにすると、buttonUpdateMoveでreturnを返したフレームを検知しないため
      if( m_selectedButtonIndex < m_buttonCount )
      {
        return m_buttons[ m_selectedButtonIndex ]->IsHolding();
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタンキャンセル中かどうか
     *
     * @return trueキャンセル中 falseキャンセルしていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::SingleProcessManager::IsButtonCancel( void ) const
    {
      if( m_selectedButtonIndex < m_buttonCount )
      {
        return m_buttons[ m_selectedButtonIndex ]->IsCancel();
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタン長押し中かどうか
     *
     * @return true長押し中 false長押ししていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::SingleProcessManager::IsButtonLongHolding( void ) const
    {
      if( m_selectedButtonIndex < m_buttonCount )
      {
        return m_buttons[ m_selectedButtonIndex ]->IsLongHolding();
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタンを選択状態にする
     *
     * @param buttonID  選択状態にするボタンID
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SingleProcessManager::StartSelectAct( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          if( m_seq == SEQ_INPUT_WAIT )
          {
            if( m_buttons[i]->RequestSelect() )
            {
              m_selectedButtonIndex = i;
              m_seq = SEQ_ANIME_WAIT;
              break;
            }
          }
        }
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタン押し続け状態をリセット
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SingleProcessManager::ResetHoldState( void )
    {
      if( m_seq == SEQ_ANIME_WAIT ) {
        m_buttons[ m_selectedButtonIndex ]->RequestRelease();
      }
      m_seq = SEQ_INPUT_WAIT;
      m_selectedButtonIndex = m_buttonCount;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief シングルプロセスマネージャー  ボタン押し続け状態をリセット
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SingleProcessManager::ResetHoldStateOne( ButtonId buttonID )
    {
      if( m_selectedButtonIndex < m_buttonCount  )
      {
        if( m_buttons[ m_selectedButtonIndex ]->GetId() == buttonID )
        {
          m_buttons[ m_selectedButtonIndex ]->RequestRelease();
          m_seq = SEQ_INPUT_WAIT;
          m_selectedButtonIndex = m_buttonCount;
        }
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  コンストラクタ
     *
     * @param buttons ボタン配列
     * @param button_num  ボタンの登録数（AddButtonで変わるのでconst参照でもらってます）
     */
    //-------------------------------------------------------------------------
    ButtonManager::MultipleProcessManager::MultipleProcessManager( Button** buttons, const u32 & button_num ) :
      IProcessManager( buttons, button_num )
    {
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  デストラクタ
     */
    //-------------------------------------------------------------------------
    ButtonManager::MultipleProcessManager::~MultipleProcessManager()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  更新
     */
    //-------------------------------------------------------------------------
    void ButtonManager::MultipleProcessManager::Update( const gfl2::ui::TouchPanel* pUiTouchPanel, const gfl2::ui::Button* pUiButton, bool isInputEnable )
    {
      
#ifdef BUTTON_MANAGER_ACCELERATION
      if( pUiTouchPanel->IsTouch() || pUiButton->GetHold() )
#endif
      {
        for( u32 i=0; i<m_buttonCount; ++i )
        {
          if( isInputEnable )
          {
            m_buttons[i]->UpdateInput( pUiTouchPanel, pUiButton, true );
          }
        }
      }
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        const gfl2::ui::TouchPanel* touchPanel = NULL;
        const gfl2::ui::Button* ui_button  = NULL;
        if( isInputEnable )
        {
          touchPanel  = pUiTouchPanel;
          ui_button   = pUiButton;
        }
        m_buttons[i]->UpdateMove( touchPanel, ui_button );
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンがアニメ中かチェック
     *
     * @return true アニメ中  falseアニメしてない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::MultipleProcessManager::IsButtonAnimation( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsAnimation() )
        {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  押し続けボタンIDの取得
     *
     * @return 押し続けボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::MultipleProcessManager::GetHoldingButtonId( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsHolding() )
        {
          return m_buttons[i]->GetId();
        }
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  選択ボタンIDの取得
     *
     * @return 選択ボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::MultipleProcessManager::GetSelectedButtonId( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsSelected() )
        {
          return m_buttons[i]->GetId();
        }
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  キャンセルボタンIDの取得
     *
     * @return キャンセルボタンID
     * @note  キャンセルとは、押したあとボタン範囲から離して、タッチリリースすることです
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::MultipleProcessManager::GetCancelButtonId( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsCancel() )
        {
          return m_buttons[i]->GetId();
        }
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  長押しボタンIDの取得
     *
     * @return 長押しボタンID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::MultipleProcessManager::GetLongHoldingButtonId( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsLongHolding() )
        {
          return m_buttons[i]->GetId();
        }
      }
      return BUTTON_ID_NULL;  // 押されていない
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタン選択中かどうか
     *
     * @return true選択中 false選択していない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::MultipleProcessManager::IsButtonSelected( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsSelected() )
        {
          return true;
        }
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタン押し続け中かどうか
     *
     * @return true押し続け中 false押し続けしていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::MultipleProcessManager::IsButtonHolding( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsHolding() )
        {
          return true;
        }
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタンキャンセル中かどうか
     *
     * @return trueキャンセル中 falseキャンセルしていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::MultipleProcessManager::IsButtonCancel( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsCancel() )
        {
          return true;
        }
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタン長押し中かどうか
     *
     * @return true長押し中 false長押ししていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::MultipleProcessManager::IsButtonLongHolding( void ) const
    {
      for( u32 i=0; i<m_buttonCount; ++i )
      {
        if( m_buttons[i]->IsLongHolding() )
        {
          return true;
        }
      }
      return false;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタンを選択状態にする
     *
     * @param buttonID  選択状態にするボタンID
     */
    //-------------------------------------------------------------------------
    void ButtonManager::MultipleProcessManager::StartSelectAct( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          if( m_buttons[i]->RequestSelect() )
          {
            break;
          }
        }
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタン押し続け状態をリセット
     */
    //-------------------------------------------------------------------------
    void ButtonManager::MultipleProcessManager::ResetHoldState( void )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[ i ]->RequestRelease();
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief マルチプロセスマネージャー  ボタン押し続け状態をリセット
     */
    //-------------------------------------------------------------------------
    void ButtonManager::MultipleProcessManager::ResetHoldStateOne( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[ i ]->RequestRelease();
        }
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap               インスタンスの生成に使用するヒープ
     * @param ui_device_manager  操作に使用するUIデバイス
     * @param max_button_num     管理可能なボタンの最大数
     * @param null_anime_index   アニメーションの無効値( 以降, この値のアニメーションは無効 )
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonManager(
        gfl2::heap::HeapBase* heap,
        const gfl2::ui::DeviceManager* ui_device_manager,
        u32 max_button_num,
        u32 null_anime_index ) :
      m_pUiDeviceManager( ui_device_manager ),
      m_pUiTouchPanel( NULL ),
      m_pUiButton( NULL ),
      m_buttons( NULL ),
      m_maxButtonCount( max_button_num ),
      m_buttonCount( 0 ),
      m_nullAnimeIndex( null_anime_index ),
      m_isLocked( false ),
      m_isInputEnable( true ),
      m_process_manager( NULL ),
      m_input_mode( INPUT_MODE_BOTH )
    {
      if( ui_device_manager != NULL )
      {
        m_pUiTouchPanel = ui_device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
        m_pUiButton = ui_device_manager->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
      }

      m_buttons = GFL_NEW_ARRAY( heap ) Button*[ max_button_num ];
      for( u32 i=0; i<max_button_num; i++ ) {
        m_buttons[i] = NULL;
      }

      this->ChangeButtonProcessMode( BUTTON_PROCESS_SINGLE, heap );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ButtonManager::~ButtonManager()
    {
      GFL_SAFE_DELETE( m_process_manager );
      for( u32 i=0; i<m_buttonCount; i++ )
      {
        GFL_ASSERT( m_buttons[i] );
        if( m_buttons[i] )
        {
          GFL_DELETE m_buttons[i];
          m_buttons[i] = NULL;
        }
      }
      GFL_DELETE_ARRAY m_buttons;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンが有効かどうかを取得する
     * @param buttonID  調べるボタン
     * @retval true   有効
     * @retval false  無効
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonActive( ButtonId buttonID ) const
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          return m_buttons[i]->IsActive();
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを有効な状態にする
     * @param buttonID  有効にするボタン
     * @param isAnime   true指定時にアニメーションを行う
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonActive( ButtonId buttonID, bool isAnime )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivity( true, isAnime );
        }
      }
    }

    void ButtonManager::SetButtonActiveForce( ButtonId buttonID, bool isAnime )  // 内部状態がアクティブであってもアクティブ処理を行う
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivityForce( true, isAnime );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを無効な状態にする
     * @param buttonID  無効にするボタン
     * @param isAnime   true指定時にアニメーションを行う
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonPassive( ButtonId buttonID, bool isAnime )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivity( false, isAnime );
        }
      }
    }

    void ButtonManager::SetButtonPassiveForce( ButtonId buttonID, bool isAnime )  // 内部状態がパッシブであってもパッシブ処理を行う
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivityForce( false, isAnime );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを有効な状態にし、アニメは最終フレームにする。
     * @param buttonID  有効にするボタン
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonActiveAnimeFrameLast( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivityAnimeFrameLast( true );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを無効な状態にし、アニメは最終フレームにする。
     * @param buttonID  無効にするボタン
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonPassiveAnimeFrameLast( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetActivityAnimeFrameLast( false );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを表示する
     * @param buttonID  表示(非表示)するボタン
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonVisible( ButtonId buttonID, bool isResetAnime )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          if( isResetAnime ) 
          {
            this->ResetButtonAnime( buttonID );
          }
          m_buttons[i]->SetVisibility( true );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを非表示にする
     * @param buttonID  表示(非表示)するボタン
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonInvisible( ButtonId buttonID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetVisibility( false );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief すべてのボタンを表示する
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonVisible( void )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetVisibility( true );
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief すべてのボタンを非表示にする
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonInvisible( void )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetVisibility( false );
      }
    }

    //-------------------------------------------------------------------------
    /**
      * @brief ボタンが表示かどうかを取得する
      * @param buttonID  調べるボタン
      * @retval true   表示
      * @retval false  非表示
      */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonVisible( ButtonId buttonID ) const
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          return m_buttons[i]->IsVisible();
        }
      }
      return false;
    }
    
    /**
     * @brief ボタンの動作をさせなくする
     *
     * @param is_enable trueで動作  falseで動作しない
     *        SetInputEnableのボタン指定版です
     */
    void ButtonManager::SetButtonInputEnable( ButtonId buttonID, bool is_enable )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          return m_buttons[i]->SetInputEnable(is_enable);
        }
      }
    }
    
    /**
     * @brief ボタン動作を取得
     *
     * @param buttonID  ボタンID
     *
     * @return true動作 false動作しない 
     */
    bool ButtonManager::IsButtonInputEnable( ButtonId buttonID ) const
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          return m_buttons[i]->IsInputEnable();
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの選択方法を変更する
     * @param buttonID  設定を変更するボタン
     * @param type  設定する選択方法
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetSelectType( ButtonId buttonID, SelectType type )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetSelectType( type );
        }
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief すべてのボタンの選択方法を変更する
     * @param type  設定する選択方法
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetSelectType( SelectType type )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetSelectType( type );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択されたことを通知するタイミングを変更する
     * @param buttonID  設定を変更するボタン
     * @param timming  通知するタイミング
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetNotifyTimming( ButtonId buttonID, NotifyTimming timming )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetNotifyTimming( timming );
        }
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタン同時動作数モード設定
     * @param processモード
     * @param heap ヒープ 内部でマネージャーを切り替えるためクラスを生成しなおしています
     * @note デフォルト動作では、１つのボタンが動作（タッチからアニメ終了まで）している間は他のボタンが押せないようになっています。これはバグを懸念してのことです。基本の動作はデフォルトのこのモードでよいですが、名前入力などボタンを連続で入力する箇所はMULTIPLEをつかってください。
     */
    //-------------------------------------------------------------------------
    void ButtonManager::ChangeButtonProcessMode( ButtonProcess process, gfl2::heap::HeapBase * heap )
    {
      GFL_SAFE_DELETE( m_process_manager );
      switch( process )
      {
      default:
        GFL_ASSERT( 0 );
      case BUTTON_PROCESS_SINGLE:
        m_process_manager = GFL_NEW( heap ) SingleProcessManager( m_buttons, m_buttonCount );
        break;
      case BUTTON_PROCESS_MULTIPLE:
        m_process_manager = GFL_NEW( heap ) MultipleProcessManager( m_buttons, m_buttonCount );
        this->SetNotifyTimming( NOTIFY_START_ANIME );
        break;
      }
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタン判定モードの設定
     *
     * @param mode 判定モード
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonMode( ButtonMode mode )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetButtonMode( mode );
      }
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタン判定モードの設定  ボタンごと版
     *
     * @param buttonID  判定モードを設定するボタン
     * @param mode 判定モード ButtonMode列挙参照
     *
     * @note SELECT_TYPE_RELEASEでないと上記設定は意味がありません。
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonMode( ButtonId buttonID, ButtonMode mode )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetButtonMode( mode );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief すべてのボタンの選択されたことを通知するタイミングを変更する
     * @param timming  通知するタイミング
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetNotifyTimming( NotifyTimming timming )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetNotifyTimming( timming );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを追加する
     * @param heap                    管理ワーク確保に使用するヒープ
     * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
     * @param layout_work             ボタンが所属するレイアウトワーク
     * @param pane_index              ボタンを表すペイン( 当たり判定に使用 )
     * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
     * @param release_anime_index     ボタンが離された際に再生するアニメーション
     * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
     * @param key_select_anime_index  キーで選択された際に再生するアニメーション
     * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::AddButton(
        gfl2::heap::HeapBase* heap,
        u32 buttonID,
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index,
        u32 touch_anime_index,
        u32 release_anime_index,
        u32 cancel_anime_index,
        u32 key_select_anime_index,
        IButtonCallback* callback_target )
    {
      this->AddButton(
          heap, buttonID,
          layout_work, pane_index,
          touch_anime_index,
          release_anime_index,
          cancel_anime_index,
          key_select_anime_index,
          m_nullAnimeIndex,
          m_nullAnimeIndex,
          callback_target );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを追加する( 拡張版 )
     * @param heap                    管理ワーク確保に使用するヒープ
     * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
     * @param layout_work             ボタンが所属するレイアウトワーク
     * @param pane_index              ボタンを表すペイン( 当たり判定に使用 )
     * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
     * @param release_anime_index     ボタンが離された際に再生するアニメーション
     * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
     * @param key_select_anime_index  キーで選択された際に再生するアニメーション
     * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
     * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
     * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::AddButton(
        gfl2::heap::HeapBase* heap,
        u32 buttonID,
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index,
        u32 touch_anime_index,
        u32 release_anime_index,
        u32 cancel_anime_index,
        u32 key_select_anime_index,
        u32 active_anime_index,
        u32 passive_anime_index,
        IButtonCallback* callback_target )
    {
      this->AddButton(
              heap, buttonID,
              layout_work,
              pane_index,
              pane_index,
              touch_anime_index,
              release_anime_index,
              cancel_anime_index,
              key_select_anime_index,
              active_anime_index,
              passive_anime_index,
              callback_target );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを追加する( 拡張版2 境界ペイン追加版 )
     * @param heap                    管理ワーク確保に使用するヒープ
     * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
     * @param layout_work             ボタンが所属するレイアウトワーク
     * @param picture_pane            ボタンの絵用ペイン( 非表示等使用 )
     * @param bound_pane              当たり判定用ペイン
     * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
     * @param release_anime_index     ボタンが離された際に再生するアニメーション
     * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
     * @param key_select_anime_index  キーで選択された際に再生するアニメーション
     * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
     * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
     * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::AddButton(
          gfl2::heap::HeapBase* heap,
          ButtonId buttonID,
          gfl2::lyt::LytWk* layout_work,
          gfl2::lyt::LytPaneIndex picture_pane,
          gfl2::lyt::LytPaneIndex bound_pane,
          u32 touch_anime_index,
          u32 release_anime_index,
          u32 cancel_anime_index,
          u32 key_select_anime_index,
          u32 active_anime_index,
          u32 passive_anime_index,
          IButtonCallback* callback_target )
    {
      if( m_buttonCount < m_maxButtonCount )
      {
        Button* new_button = GFL_NEW( heap )
          Button(
              buttonID,
              layout_work,
              picture_pane,
              bound_pane,
              m_nullAnimeIndex,
              touch_anime_index,
              release_anime_index,
              cancel_anime_index,
              key_select_anime_index,
              active_anime_index,
              passive_anime_index,
              SE_NULL,
              callback_target );
        if( new_button == NULL )
        {
          GFL_ASSERT( 0 );
          return;
        }
        new_button->SetLongPressFrame( DEFAULT_LONG_PRESS_FRAME );

        if( m_buttons[ m_buttonCount ] != NULL )
        {
          GFL_ASSERT( 0 );
          GFL_DELETE new_button;
          return;
        }
        m_buttons[ m_buttonCount ] = new_button;
        m_buttonCount++;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンを追加する( ペインのポインタ指定版 )
     * @param heap                    管理ワーク確保に使用するヒープ
     * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
     * @param layout_work             ボタンが所属するレイアウトワーク
     * @param picture_pane            ボタンの絵用ペイン( 非表示等使用 )
     * @param bound_pane              当たり判定用ペイン
     * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
     * @param release_anime_index     ボタンが離された際に再生するアニメーション
     * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
     * @param key_select_anime_index  キーで選択された際に再生するアニメーション
     * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
     * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
     * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::AddButton(
        gfl2::heap::HeapBase* heap,
        ButtonId buttonID,
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPane* picture_pane,
        gfl2::lyt::LytPane* bound_pane,
        u32 touch_anime_index,
        u32 release_anime_index,
        u32 cancel_anime_index,
        u32 key_select_anime_index,
        u32 active_anime_index,
        u32 passive_anime_index,
        IButtonCallback* callback_target )
    {
      if( m_buttonCount < m_maxButtonCount )
      {
        Button* new_button = GFL_NEW( heap )
          Button(
              buttonID,
              layout_work,
              picture_pane,
              bound_pane,
              m_nullAnimeIndex,
              touch_anime_index,
              release_anime_index,
              cancel_anime_index,
              key_select_anime_index,
              active_anime_index,
              passive_anime_index,
              SE_NULL,
              callback_target );
        if( new_button == NULL )
        {
          GFL_ASSERT( 0 );
          return;
        }
        new_button->SetLongPressFrame( DEFAULT_LONG_PRESS_FRAME );

        if( m_buttons[ m_buttonCount ] != NULL )
        {
          GFL_ASSERT( 0 );
          GFL_DELETE new_button;
          return;
        }
        m_buttons[ m_buttonCount ] = new_button;
        m_buttonCount++;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの選択音(SE)を設定する
     * @param buttonID  選択音を設定するボタン
     * @param SEID      選択時に再生するＳＥ( SE_NULLなら再生しない )
     *
     * @note デフォルトは SE_NULL( 音なし )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonSelectSE( ButtonId buttonID, u32 SEID )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetSelectSE( SEID );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 長押しと判定するフレーム数を設定する
     * @param buttonID 設定するボタン
     * @param frame    長押しと判定するフレーム数
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonLongPressFrame( ButtonId buttonID, u32 frame )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetLongPressFrame( frame );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キー入力とバインドする
     * @param buttonID 設定するボタン
     * @param key      バインドするキー入力( gfl::ui::BUTTON_xxxx )
     */
    //-------------------------------------------------------------------------
    void ButtonManager::SetButtonBindKey( ButtonId buttonID, u32 key, BindKeyMode mode )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->SetBindKey( key, mode );
        }
      }
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief アニメーション中に非表示にした場合等に、アニメを初期値に戻す
     *
     * @param buttonID  ボタンID
     */
    //-------------------------------------------------------------------------
    void ButtonManager::ResetButtonAnime( ButtonId buttonID )
    {
      m_process_manager->ResetHoldStateOne( buttonID );

      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->ResetAnime();
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのボタン操作をロックする
     */
    //-------------------------------------------------------------------------
    void ButtonManager::LockButtonControl( void )
    {
      m_isLocked = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 操作ロックを解除する
     */
    //-------------------------------------------------------------------------
    void ButtonManager::UnlockButtonControl( void )
    {
      m_isLocked = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタン選択時の処理を実行する
     * @param buttonID  ボタンＩＤ
     *
     * @note
     * 指定したボタンに関連付けられた
     * アニメ再生, ＳＥ再生, コールバック呼び出しを実行します.
     */
    //-------------------------------------------------------------------------
    void ButtonManager::StartSelectedAct( ButtonId buttonID )
    {
      m_process_manager->StartSelectAct( buttonID );
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの座標を取得する（グローバルなレイアウト座標で取得する）
     * @param buttonID  ボタンＩＤ
     * @return  ボタンの座標
     * @note  レイアウト座標とは、画面の中心が(0,0,0)、下画面なら右下が(160,-120,0)、となるもの
     */
    //-------------------------------------------------------------------------
    gfl2::math::VEC3    ButtonManager::GetButtonPos( ButtonId buttonID ) const
    {
      bool buttonExists = false;
      gfl2::math::VEC3 pos(0,0,0);
      for( u32 i=0; i<m_buttonCount; i++ ) {
        if( m_buttons[i]->GetId() == buttonID ) {
          m_buttons[i]->GetTranslate(&pos);
          buttonExists = true;
          break;
        }
      }
      GFL_ASSERT(buttonExists);  // 開発中に気付かせるASSERT。ケア済み。buttonIDのボタンが存在しない。
      return pos;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンの座標をGFベクトル3の形式で取得する（グローバルなレイアウト座標で取得する）
     * @param buttonID  ボタンＩＤ
     * @return  ボタンの座標
     * @note  レイアウト座標とは、画面の中心が(0,0,0)、下画面なら右下が(160,-120,0)、となるもの
     */
    //-------------------------------------------------------------------------
    gfl2::math::Vector3 ButtonManager::GetButtonPosByGfVec3( ButtonId buttonID ) const
    {
      gfl2::math::VEC3 pos = this->GetButtonPos(buttonID);
      gfl2::math::Vector3 posByGfVec3 = gfl2::math::ConvertNwVec3ToGfVec3(pos);
      return posByGfVec3;
    }

    /**
     * @brief ルートの表示設定を使用するかどうか
     *
     * @param flag  true使用する（ルートが非表示ならばボタンも動作しない）
     *              false使用しない（ルートに無関係に）
     */
    void ButtonManager::SetUseVisibleOfRoot( bool flag )
    {
      for( u32 i=0; i<m_buttonCount; i++ ) {
        m_buttons[i]->SetUseVisibleOfRoot( flag );
      }
    }

    /**
     * @brief 入力モードをセット
     *
     * @param input_mode  入力方法
     */
    void ButtonManager::SetInputMode( InputMode input_mode )
    {
      m_input_mode  = input_mode;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンが押されたかどうかを調べる
     * @retval true  いずれかのボタンが押された
     * @retval false どのボタンも押されていない
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonTrigger( void ) const
    {
      return ( m_pUiTouchPanel->IsTouchTrigger() && this->IsButtonHolding() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押されたボタンを取得する
     * @return 押されたボタンのID
     * @retval BUTTON_ID_NULL  どのボタンも押されていない場合
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::GetTriggerButtonId( void ) const
    {
      return this->GetHoldingButtonId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 押されているボタンを取得する
     * @retval 押されているボタンのID
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::GetHoldingButtonId( void ) const
    {
      return m_process_manager->GetHoldingButtonId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 選択されたボタンのIDを取得する
     * @return 選択されたボタンのID
     * @retval BUTTON_ID_NULL  どのボタンも選択されていない場合
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::GetSelectedButtonId( void ) const
    {
      return m_process_manager->GetSelectedButtonId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンが選択されたかどうかを調べる
     * @retval true   選択された
     * @retval false  選択されていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonSelected( void ) const
    {
      return m_process_manager->IsButtonSelected();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 更新処理( 毎フレーム呼んでください )
     * @return 選択されたボタンのID
     * @return 更新後の状態
     */
    //-------------------------------------------------------------------------
    ButtonManager::UpdateState ButtonManager::Update( void )
    {
      // 操作ロック中
      if( this->m_isLocked )
      {
        return UPDATE_STATE_IDLE;
      }

      // ボタン更新
      this->UpdateButtons();

      // 入力をクリア
      if( m_pUiDeviceManager == NULL )
      {
        m_pUiButton = NULL;
        m_pUiTouchPanel = NULL;
      }

#if PM_DEBUG
      if( m_isVisibleBoundingPane)
      {
        this->Debug_RegisterBoundingPaneRect();
      }
#endif

      // 更新結果を返す
      return this->GetUpdateState();
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief 入力処理
     *
     * @param pUiButton     ボタン
     * @param pUiTouchPanel タッチバー
     * 
     * @note  この入力処理はコンストラクタのui_device_managerにNULLをいれたときだけ稼働します。
     */
    //-------------------------------------------------------------------------
    void ButtonManager::UpdateInput( const gfl2::ui::Button* pUiButton, const gfl2::ui::TouchPanel* pUiTouchPanel )
    {
      if( m_pUiDeviceManager == NULL )
      {
        m_pUiButton = pUiButton;
        m_pUiTouchPanel = pUiTouchPanel;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボタンが押されているかどうかを調べる
     * @retval true   押されている
     * @retval false  押されていない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonHolding( void ) const
    {
      return m_process_manager->IsButtonHolding();
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ボタンがキャンセルされたかどうかを調べる
     * @retval true  いずれかのボタンがキャンセルされた
     * @retval false どのボタンもキャンセルされていない
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonCancel( void ) const
    {
      return m_process_manager->IsButtonCancel();
    }
    //-------------------------------------------------------------------------
    /**
     * @brief キャンセルされたボタンを取得する
     * @return キャンセルされているボタンのID
     * @retval BUTTON_ID_NULL  どのボタンもキャンセルされてない場合
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::GetCancelButtonId( void ) const
    {
      return m_process_manager->GetCancelButtonId();
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンが長押しされたかどうかを調べる
     * @retval true  いずれかのボタンが長押しされた
     * @retval false どのボタンも長押しされていない
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonLongHolding( void ) const
    {
      return m_process_manager->IsButtonLongHolding();
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief 長押しされたボタンを取得する
     * @return 長押しされているボタンのID
     * @retval BUTTON_ID_NULL  どのボタンも長押しされてない場合
     *
     * @note SELECT_TYPE_RELEASE の場合にのみ有効
     */
    //-------------------------------------------------------------------------
    ButtonManager::ButtonId ButtonManager::GetLongHoldingButtonId( void ) const
    {
      return m_process_manager->GetLongHoldingButtonId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トリガ選択時の更新処理
     */
    //-------------------------------------------------------------------------
    void ButtonManager::UpdateButtons( void )
    {
      const gfl2::ui::TouchPanel* pUiTouchPanel  = m_pUiTouchPanel;
      const gfl2::ui::Button* pUiButton  = m_pUiButton;
      
      switch( m_input_mode )
      {
      default:
        GFL_ASSERT(0);
      case INPUT_MODE_BOTH:        //タッチ、キー両方きく（デフォルト）
        //none
        break;
      case INPUT_MODE_KEY_ONLY:    //キーのみきく
        pUiTouchPanel = NULL;
        break;
      case INPUT_MODE_TOUCH_ONLY:  //タッチのみきく
        pUiButton = NULL;
        break;
      }

      m_process_manager->Update( pUiTouchPanel, pUiButton, m_isInputEnable );
    }
    //-------------------------------------------------------------------------
    /**
     * @brief ボタンのホールド状況をリセットする
     */
    //-------------------------------------------------------------------------
    void ButtonManager::ResetHoldState( void )
    {
      m_process_manager->ResetHoldState();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 更新状況を取得する
     * @note  他の操作と重複しないように使用してください。
     */
    //-------------------------------------------------------------------------
    ButtonManager::UpdateState ButtonManager::GetUpdateState( void ) const
    {
      if( this->IsButtonCancel() )
      {
        return UPDATE_STATE_CANCEL;
      }
      else if( this->IsButtonSelected() )
      {
        return UPDATE_STATE_SELECTED;
      }
      else if( this->IsButtonHolding() )
      {
        return UPDATE_STATE_HOLDING;
      }
      else if( m_process_manager->IsButtonAnimation() )
      {
        return UPDATE_STATE_SELECT_ANIME_PLAYING;
      }
      else
      {
        return UPDATE_STATE_IDLE;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @func  LinkButton
     * @brief ボタン同士をリンクさせる
     * @date  2015.04.06
     *
     * @param tbl   ボタンIDテーブル
     * @param len   テーブル長
     */
    //-------------------------------------------------------------------------
    void ButtonManager::LinkButton( const ButtonId * tbl, u32 len )
    {
      u32 c = 1;

      for( u32 i=0; i<len; i++ )
      {
        Button * parent = GetButton( tbl[i] );
        Button * child  = GetButton( tbl[c] );
        if( parent == NULL || child == NULL || parent == child )
        {
          GFL_ASSERT( 0 );
          return;
        }
        parent->Link( child );
        c++;
        if( c >= len )
        {
          c = 0;
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @func  GetButton
     * @brief 指定IDのボタンを取得
     * @date  2015.04.06
     *
     * @param id  ボタンID
     *
     * @return  ボタン
     */
    //-------------------------------------------------------------------------
    ButtonManager::Button * ButtonManager::GetButton( ButtonId id )
    {
      for( u32 i=0; i<m_buttonCount; i++ )
      {
        if( m_buttons[i]->GetId() == id )
        {
          return m_buttons[i];
        }
      }
      return NULL;
    }

    //-------------------------------------------------------------------------
    /**
     * @func    IsButtonHit
     * @brief   指定IDのボタンがタッチ座標にあるか
     * @date    2015.10.26
     *
     * @param   id        ボタンID
     * @param   touch_x   タッチx座標
     * @param   touch_y   タッチy座標
     *
     * @retval  true  = ある
     * @retval  false = ない
     */
    //-------------------------------------------------------------------------
    bool ButtonManager::IsButtonHit( ButtonId id, u16 touch_x, u16 touch_y ) const
    {
      for( u32 i=0; i<m_buttonCount; i++ )
      {
        if( m_buttons[i]->GetId() == id )
        {
          return m_buttons[i]->IsHit( touch_x, touch_y );
        }
      }
      return false;
    }



#if PM_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 登録ボタンのバウンディングペインの矩形描画情報を登録する (デバッグ機能)
     */
    //-------------------------------------------------------------------------
    void ButtonManager::Debug_RegisterBoundingPaneRect( void)
    {
      if( !m_isVisibleBoundingPane)
      {
        return;
      }

      for( u32 i = 0; i < m_buttonCount; ++i)
      {
        m_buttons[i]->Debug_RegisterBoundingPaneRect( i);
      }
    }
#endif

  } // namespace tool
} // namespace app
