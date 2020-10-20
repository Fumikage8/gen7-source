//========================================================================================
/**
 * @brief スクロールバーの描画クラス
 * @file app_tool_ScrollBar.h
 * @author obata_toshihiro
 * @date 2011.05.20
 */
//========================================================================================
#ifndef __APP_TOOL_SCROLL_BAR_H__
#define __APP_TOOL_SCROLL_BAR_H__ 

#include <heap/include/gfl2_heap.h>
#include <Layout/include/gfl2_LytSys.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

// 前方宣言
class ScrollValue;


class ScrollBar
{
  GFL_FORBID_COPY_AND_ASSIGN( ScrollBar );

public:
  //! スクロールの向き
  enum ScrollDirection
  {
    SCROLL_DIRECTION_VERTICAL,   //!< 縦
    SCROLL_DIRECTION_HORIZONTAL, //!< 横
  }; 


private:
  static const u32 MARK_SIZE_MIN = 20;  //!< ツマミ部分のペインの最小サイズ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   parts   本体の部品ペイン
   * @param   mark    ツマミ部分のペイン
   * @param   top     スクロール最小値の位置ペイン
   * @param   bottom  スクロール最大値の位置ペイン
   * @param   dir     スクロールの向き
   */
  //-----------------------------------------------------------------------------
  ScrollBar( gfl2::lyt::LytPane * parts, gfl2::lyt::LytPane * mark, gfl2::lyt::LytPane * top, gfl2::lyt::LytPane * bottom, ScrollDirection dir );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ScrollBar();

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.03.09
   *
   * @param   scroll_value  スクロール値
   * @param   alpha         アルファ値
   */
  //-----------------------------------------------------------------------------
  void Draw( const ScrollValue & scroll_value, f32 alpha );

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.03.09
   *
   * @param   val     描画位置（現在値と最大値の割合）
   * @param   alpha   アルファ値
   */
  //-----------------------------------------------------------------------------
  void Draw( f32 val, f32 alpha );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMarkSize
   * @brief   ツマミ部分のペインのサイズを設定
   * @date    2015.03.09
   *
   * @param   scroll_max  スクロール最大値
   */
  //-----------------------------------------------------------------------------
  void SetMarkSize( f32 size );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisible
   * @brief   表示切り替え
   * @date    2015.03.09
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisible( bool flg );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CalcPos
   * @brief   ツマミの表示位置計算
   * @date    2015.03.09
   *
   * @param   mark_size   ツマミのサイズ
   * @param   val         描画位置（現在値と最大値の割合）
   */
  //-----------------------------------------------------------------------------
  f32 CalcPos( f32 mark_size, f32 val );


private:
  gfl2::lyt::LytPane * m_partsPane;   //!< 本体の部品ペイン
  gfl2::lyt::LytPane * m_markPane;    //!< ツマミ部分のペイン
  nw::lyt::Size m_markSize;           //!< ツマミ部分のペインサイズ
  gfl2::math::VEC3 m_topPos;          //!< スクロール値最小時の位置
  gfl2::math::VEC3 m_bottomPos;       //!< スクロール値最大時の位置
  f32 m_railLen;                      //!< レールの長さ
  ScrollDirection m_scrollDirection;  //!< スクロールの向き
  bool m_isMarkSizeUpdate;            //!< true = ツマミ部分のペインサイズが変更された
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_SCROLL_BAR_H__
