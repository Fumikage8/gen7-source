//=============================================================================
/**
 * @file TitleUpperUIDraw.h
 * @brief タイトルの上UI画面
 * @author yuto_uchida
 * @date 2015.05.08
 */
//=============================================================================
#include "App/Title/source/parts/TitleUpperUIDraw.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  TitleUpperUIDraw::TitleUpperUIDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );

    //  初期化
    {
      m_state     = STATE_TYPE_NONE;
      m_initParam = param;
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  TitleUpperUIDraw::~TitleUpperUIDraw()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void TitleUpperUIDraw::Initialize()
  {
    _Initialize2D( m_initParam.pLytBuff );

    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void TitleUpperUIDraw::Terminate()
  {
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void TitleUpperUIDraw::Update()
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void TitleUpperUIDraw::Draw(  gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i );
    }
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void TitleUpperUIDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, (u32)app::util::G2DUtil::ATTACH_ROOT },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            m_initParam.lytId,
            m_initParam.lytAnimeMax,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_UPPER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), UNUSE_MESSAGE_DATA );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void TitleUpperUIDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, m_initParam.animeInId ) )
      {
        _SetState( STATE_TYPE_INPUT_IDLE );
      }
    }

    if( m_state == STATE_TYPE_INPUT_IDLE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void TitleUpperUIDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );

      //  開始アニメをする
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_initParam.animeInId );
    }
    else if( m_state == STATE_TYPE_INPUT_IDLE )
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_initParam.animeKeepId );
    }
    else if( m_state == STATE_TYPE_OUT )
    {
    }
    else if( m_state ==  STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )
