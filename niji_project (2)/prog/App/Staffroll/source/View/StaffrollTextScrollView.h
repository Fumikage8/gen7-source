// ============================================================================
/*
 * @file StaffrollTextScrollView.h
 * @brief スタッフロールアプリのテキストスクロール
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_TEXT_SCROLL_VIEW_H_INCLUDE )
#define STAFFROLL_TEXT_SCROLL_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/Staffroll/source/System/StaffrollWork.h"
#include "App/Staffroll/source/StaffrollResourceID.h"

//  リソースのインクルード
#include <niji_conv_header/app/staffroll/data/staffroll_data.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)

  class StaffrollTextBox;

GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(View)

  class StaffrollTextScrollView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollTextScrollView );
  public:
    StaffrollTextScrollView( App::Staffroll::System::StaffrollWork* pWork, const App::Staffroll::StaffrollResourceID resId, const app::util::G2DUtil::SETUP_DISPLAY_TYPE dispType, const f32 dispUpperLimit, const f32 dispLowLimit );
    virtual~StaffrollTextScrollView();

    //  移動
    void MoveYPos( const f32 speed );

    //  位置リセット
    void ResetPos();

    //  書き込み位置設定
    void SetWritePos( const f32 pos );
    void AddWritePos( const f32 num );

    //  すべてのテキストが流れているか
    bool IsTextAllOut();

    //  アルファ設定
    void SetAlpha( const u8 alpha );

    //  テキストボックス設定
    //  日本語と英語しか出さないがデータ９言語分に日本語と英語のみのテキストを入力するのでプログラム側で指定の必要はない
    bool SettingBoxText( 
    const u32 messageId, 
    const f32 xPos, 
    const f32 xOffsetPos, 
    const f32 yPos, 
    const u32 strPos, 
    const u32 font, 
    const u32 color );

    //  テキストボックスをすべてクリア
    void ClearAllTextBox();

    //  未使用テキストボックスを取得
    App::Staffroll::StaffrollTextBox* GetEmptyTextBox();

    //  使用してかつ画面一番下に表示しているテキストボックスを取得
    App::Staffroll::StaffrollTextBox* GetLastPosYDrawTextBox();

    //  下側の表示範囲
    f32 GetDispLowLimit() const { return m_dispLowLimit; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update(void);

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_UPPER = 0,
      LAYOUT_WORK_ID_LOWER,
      LAYOUT_WORK_ID_MAX
    };

    void _Clear()
    {
      m_pWork           = NULL;
      m_ppTextBoxArray  = NULL;
      for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
      {
        m_pPaneRoot[i]  = NULL;
      }
      m_textBoxLength   = 0;
      m_writePosY       = 0.0f;
      m_dispLowLimit    = 0.0f;
    }

    App::Staffroll::System::StaffrollWork*     m_pWork;

    gfl2::lyt::LytPane * m_pPaneRoot[LAYOUT_WORK_ID_MAX];

    u32 m_textBoxLength;
    f32 m_writePosY;
    f32 m_dispLowLimit;

    //  テキストオブジェクト
    StaffrollTextBox** m_ppTextBoxArray;


  private:
    void Initialize2D(
          App::Staffroll::System::StaffrollWork * work,
          const App::Staffroll::StaffrollResourceID res_id );
    void Terminate2D(void);
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLL_TEXT_SCROLL_VIEW_H_INCLUDE
