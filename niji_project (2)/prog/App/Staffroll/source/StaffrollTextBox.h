//======================================================================
/**
 * @file StarffrollTextBox.h
 * @date 2016/01/06 13:35:30
 * @author uchida_yuto
 * @brief スタッフロールで表示するテキストオブジェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __STAFF_ROLL_TEXT_BOX_HEADER_H_INCLUDED__
#define __STAFF_ROLL_TEXT_BOX_HEADER_H_INCLUDED__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_StrBuf.h>

//  リソースのインクルード
#include <niji_conv_header/app/staffroll/data/staffroll_data.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)

  /**
   * @class StaffrollTextBox
   * @brief スタッフロールで使うテキストボックスオブジェクト
   */
  class StaffrollTextBox
  {
      GFL_FORBID_COPY_AND_ASSIGN( StaffrollTextBox );
  public:
    StaffrollTextBox(
      gfl2::lyt::LytTextBox * upper_pane,
      gfl2::lyt::LytTextBox * lower_pane,
      const f32 dispUpperLimit, const f32 dispLowLimit, const StaffrollParamBinaryFormat& rParamData );
    virtual ~StaffrollTextBox();
    
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
    void Start( gfl2::str::StrBuf * pStrbuf, s32 x, s32 xofs, s32 y, u32 strpos, u32 font, u32 color );

    /**
     * @brief 未使用にする
     */
    void Clear( void );

    /**
     * @brief 使用中かどうか
     *
     * @return true使用中 false未使用
     */
    bool IsUsed( void ) const { return m_bUsed; }

    bool InDisplay(void) const;
    bool IsScrollOver( void ) const;

    //  画面範囲取得
    f32 GetDispUpperLimit() const { return m_dispUpperLimit; }
    f32 GetDispLowLimit() const { return m_dispLowLimit; }

    //  座標取得
    float GetPosY();

  private:
    enum LytID
    {
      LYTID_UPPER = 0,
      LYTID_LOWER,
      LYTID_MAX
    };

    gfl2::math::VEC3 _GetPaneGlobalPos( gfl2::lyt::LytPane* pane ) const;

    void _Clear()
    {
      for( u32 i=0; i<LYTID_MAX; i++ )
      {
        m_pTextPane[i] = NULL;
      }
      m_bUsed = false;
      m_dispUpperLimit  = 0.0f;
      m_dispLowLimit    = 0.0f;
    }

    gfl2::lyt::LytTextBox* m_pTextPane[LYTID_MAX];
    f32   m_dispUpperLimit;
    f32   m_dispLowLimit;

    StaffrollParamBinaryFormat  m_paramData;

    bool  m_bUsed;
  };

GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // __STAFF_ROLL_TEXT_BOX_HEADER_H_INCLUDED__