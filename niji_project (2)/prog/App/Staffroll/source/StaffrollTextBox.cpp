//======================================================================
/**
 * @file StarffrollTextBox.cpp
 * @date 2016/01/06 13:35:37
 * @author uchida_yuto
 * @brief スタッフロールで表示するテキストオブジェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "App/Staffroll/source/StaffrollTextBox.h"

//  nijiのインクルード
#include "Print/include/PrintSystem.h"
#include "Print/include/SystemFont.h"
#include "AppLib/include/Util/app_util_2d.h"

//  リソースのインクルード
//#include <niji_conv_header/app/staffroll/data/staffroll_data.h>
//#include <niji_conv_header/app/staffroll/data/staffroll_data.cdat>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)

  //
  //
  //スタッフロールの文字１つ
  //
  //
  /**
   * @brief テキストボックスコンストラクタ
   *
   * @param text_pane テキストペイン
   */
  StaffrollTextBox::StaffrollTextBox(
    gfl2::lyt::LytTextBox * upper_pane,
    gfl2::lyt::LytTextBox * lower_pane,
    const f32 dispUpperLimit, const f32 dispLowLimit, const StaffrollParamBinaryFormat& rParamData )
  {
    _Clear();

    m_pTextPane[LYTID_UPPER] = upper_pane;
    m_pTextPane[LYTID_LOWER] = lower_pane;
    m_dispUpperLimit  = dispUpperLimit;
    m_dispLowLimit    = dispLowLimit;

    gfl2::std::MemCopy( &rParamData, &m_paramData, sizeof( m_paramData ) );
    
    this->Clear();
  }

  /**
   * @brief テキストボックスデストラクタ
   */
  StaffrollTextBox::~StaffrollTextBox()
  {
  }

  /**
   * @brief 開始
   *
   * @param message 表示するメッセージ
   * @param x       表示する基準X座標
   * @param xofs    X座標オフセット
   * @param y       Y座標
   * @param strpos  表示幅寄せ
   * @param font    フォント
   * @param color   色
   */
  void StaffrollTextBox::Start( gfl2::str::StrBuf * pStrbuf, s32 x, s32 xofs, s32 y, u32 strpos, u32 font, u32 color )
  {
    if( pStrbuf == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_bUsed  = true;

    //表示ON
    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      m_pTextPane[i]->SetVisible( true );
    }

    //文字列設定
    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      print::SetStringForLytTextBox( m_pTextPane[i], pStrbuf );
    }

    //色設定
    {
      GFL_ASSERT( color < GFL_NELEMS(m_paramData.aColorFormatData) );
      gfl2::lyt::ColorU8 up_color( m_paramData.aColorFormatData[color].up_r, 
          m_paramData.aColorFormatData[color].up_g,
          m_paramData.aColorFormatData[color].up_b,
          255 );

      gfl2::lyt::ColorU8 dwn_color( m_paramData.aColorFormatData[color].dwn_r, 
          m_paramData.aColorFormatData[color].dwn_g,
          m_paramData.aColorFormatData[color].dwn_b,
          255 );
      gfl2::lyt::ColorU8 black_color( m_paramData.aColorFormatData[color].black_r, 
          m_paramData.aColorFormatData[color].black_g,
          m_paramData.aColorFormatData[color].black_b,
          0 );
      gfl2::lyt::ColorU8 white_color( m_paramData.aColorFormatData[color].white_r, 
          m_paramData.aColorFormatData[color].white_g,
          m_paramData.aColorFormatData[color].white_b,
          255 );

      for( u32 i=0; i<LYTID_MAX; i++ )
      {
        m_pTextPane[i]->SetTextColor( up_color, dwn_color );

        gfl2::lyt::LytMaterial* material = m_pTextPane[i]->GetMaterial();
        //!< 黒カラー補間
        material->SetColor( app::util::G2DUtil::COLOR_INDEX_BLACK, black_color );
        //!< 白カラー補間
        material->SetColor( app::util::G2DUtil::COLOR_INDEX_WHITE, white_color );
      }
    }

    //フォント
    gfl2::str::Font* cfont  = NULL;
    {
      switch( m_paramData.afontFormatData[font].font_type )
      {
      default:
        GFL_ASSERT(0);
      case STAFFROLL_FONTDEF_NORMAL:
        cfont = print::SystemFont_GetFont();
        break;
      case STAFFROLL_FONTDEF_BATTLE:
        cfont = print::SystemFont_GetBattleFont();
        break;
      }
      for( u32 i=0; i<LYTID_MAX; i++ )
      {
        m_pTextPane[i]->SetFont( cfont );
      }
    }

    //フォントサイズ
    nw::lyt::Size fontSize;
    {
      GFL_ASSERT_STOP( font < GFL_NELEMS( m_paramData.afontFormatData ) );
      
      fontSize.width  = m_paramData.afontFormatData[font].x_size;
      fontSize.height = m_paramData.afontFormatData[font].y_size;
      for( u32 i=0; i<LYTID_MAX; i++ )
      {
        m_pTextPane[i]->SetFontSize( fontSize );
      }
    }
  
    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      //バトルフォントが見づらいので間をあける
      if( m_paramData.afontFormatData[font].font_type == STAFFROLL_FONTDEF_BATTLE )
      {
        m_pTextPane[i]->SetCharSpace( 1.0f );
      }

      //垂直方向位置
      m_pTextPane[i]->SetTextPositionV( nw::lyt::VERTICALPOSITION_CENTER );

      //幅寄せ設定
      m_pTextPane[i]->SetTextPositionH( nw::lyt::HORIZONTALPOSITION_CENTER );
    }

    s32 px  = 0;
  
    //GetStringWidthは「コンバートされたサイズ」をもとに計算する
    //あとでここではSetFontSizeするため自前で計算する
    f32 fontWidth = fontSize.width;
  #if 0 //ここを変えるとフォントによって見え方の位置がかわってしまうのでスケール計算しない
    if( print::CheckEnableShortStr( text_box, print::GetMessageLangId() ) )
    {
       fontWidth  *= print::PRINT_EFIGS_SCALE;
    }
  #endif

    int str_width = print::GetStringWidth( pStrbuf, cfont, fontWidth );
    switch(strpos)
    {
    default:
      GFL_ASSERT(0);
    case STAFFROLL_STRING_CENTER: //中央
      px  = x + xofs;
      break;
    case STAFFROLL_STRING_LEFT:   //左寄せ
      px  = x + xofs + ( str_width/2 );
      break;
    case STAFFROLL_STRING_RIGHT:  //右寄せ
      px  = x + xofs - ( str_width/2 );
      break;
    }

    //位置
    gfl2::math::VEC3 pos( px, y, 0 );
    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      m_pTextPane[i]->SetTranslate( pos );
    }

    UCHIDA_PRINT( "文字描画成功 px=%d y=%d \n", px, y );
 //   UCHIDA_PRINT( "文字描画 str=%s x=%lf xofs=%lf w=%d pos=%d y=%lf\n", pStrbuf->GetPtr(), x, xofs, str_width, strpos, y );
  }

  /**
   * @brief 未使用にする
   */
  void StaffrollTextBox::Clear( void )
  {
    for( u32 i=0; i<LYTID_MAX; i++ )
    {
      m_pTextPane[i]->SetVisible( false );
    }
    m_bUsed  = false;
  }

  /**
   * @brief 画面内にいるかどうか
   *
   * @return trueいる falseいない
   */
  bool StaffrollTextBox::InDisplay( void ) const
  {
    // @note 上下の限界座標が下画面基準なので下画面をチェックする
    bool ret = false;
  
    gfl2::math::VEC3 pos = _GetPaneGlobalPos( m_pTextPane[LYTID_LOWER] );
    nw::lyt::Size size =  m_pTextPane[LYTID_LOWER]->GetSize();

    if( ( m_dispLowLimit <= pos.y + size.height / 2 )
     && ( pos.y - size.height / 2 <= m_dispUpperLimit ) )
    {
      ret = true;
    }

    return ret;
  }

  /**
   * @brief 画面上部にいるかどうか
   *
   * @return trueいる falseいない
   */
  bool StaffrollTextBox::IsScrollOver( void ) const
  {
    // @note 上下の限界座標が下画面基準なので下画面をチェックする
    bool ret = false;

    gfl2::math::VEC3 pos = _GetPaneGlobalPos(m_pTextPane[LYTID_UPPER]);
    nw::lyt::Size size =  m_pTextPane[LYTID_UPPER]->GetSize();

    if( m_dispUpperLimit <= pos.y - size.height/2 )
    {
      ret = true;
    }

    return ret;
  }

  //  座標取得
  float StaffrollTextBox::GetPosY()
  {
    // @note どちらでもよさげだが、下画面からインするので下画面を取得
    gfl2::math::VEC3 pos = _GetPaneGlobalPos(m_pTextPane[LYTID_LOWER]);
    return  pos.y;
  }

  gfl2::math::VEC3 StaffrollTextBox::_GetPaneGlobalPos( gfl2::lyt::LytPane* pane ) const
  {
    gfl2::math::VEC3 pos(0,0,0);

    const gfl2::math::MTX34 & global_matrix = pane->GetGlobalMtx();
    pos.x = global_matrix._03;
    pos.y = global_matrix._13;
    pos.z = global_matrix._23;

    return pos;
  }

GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
