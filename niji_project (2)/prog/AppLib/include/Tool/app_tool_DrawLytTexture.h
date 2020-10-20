//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_DrawLytTexture.h
 *	@brief  テクスチャへの書き込み
 *	@author	Toru=Nagihashi
 *	@date		2012.10.24
 *	
 *
 *	@note   ▼概要
 *	        レイアウトのテクスチャに対して、プログラムにて書き込みを行うクラスです。
 *	        テクスチャを読み込むと同じサイズ・フォーマットの
 *	        書き込みバッファ領域が作成されます。
 *	        ひと通り書き込んだあと、指定のペインに転送することで反映されます。
 *
 *
 *          ▼使い方
 *          １．生成
 *          hoge  = GFL_NEW(heap) app::tool::DrawLytTexture(…
 *
 *          ２．テクスチャ読み込み
 *          hoge->LoadTexture(…
 *          
 *          ３．色々書き込んで
 *          hoge->WritePoint(…
 *          
 *          ４．転送  画面への反映は２F後
 *          hoge->TransferTexture(…
 *          
 *          ５．破棄
 *          GFL_DELETE hoge
 *
 * 
 *          ▼制限
 *	        現在フォーマットはNW4CTGA、RGBA4固定です。
 *	        テクスチャサイズはwidth128,height128以上が必要。
 *	        書き込みテクスチャと転送先テクスチャは同じサイズ・フォーマットであること。
 *
 * 
 *          ▼サンプル
 *          src/test/nagihashi/test_nagi_DrawLytTexture.cpp
 *
 *
 *  @caution niji プロジェクトでは未使用（デバッグされていない） 内部で nngx 使う箇所もあるので注意
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_TOOL_DRAWLYTTEXTURE_H__)
#define __APP_TOOL_DRAWLYTTEXTURE_H__

#include "AppLib/include/Tool/app_tool_LytTexData.h"

#include <heap/include/gfl2_heap.h>
#include <Layout/include/gfl2_Layout.h>
#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  
class DrawLytTexture
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawLytTexture);
public:
  //-----------------------------------------------------------------------
  // 初期化・破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  DrawLytTexture( gfl2::heap::HeapBase* device_heap, u32 buffer_size, u32 buffer_align );
  /**
   * @brief デストラクタ
   */
  virtual ~DrawLytTexture();
  //-----------------------------------------------------------------------
  // 操作
  //-----------------------------------------------------------------------
  /**
   * @brief テクスチャデータ（NW4CTGA）の同期読み込み
   * @param heap_for_work         読み込みの作業用ワーク領域に使用するヒープ
   * @param file_read_manager     ファイル読み込み制御クラス
   * @param data_id               読み込むテクスチャデータを指定
   * @param compressed          true: 圧縮されている, false: 圧縮されていない
   * 
   * @note  アーカイブからではなくレイアウトから作成したいが…。方法模索中。
   *        あらかじめファイルを gfl2::fs::AsyncFileManager でファイルを開いておく必要がある
   */
  void LoadTexture(
      gfl2::heap::HeapBase* heap_for_work,
      gfl2::fs::AsyncFileManager *file_read_manager,
      gfl2::fs::ArcFile::ARCDATID data_id,
      bool compressed = false );

  /**
   * @brief 書いたデータをテクスチャに転送
   * 
   * @param target_pane 転送先のピクチャペイン
   * @param material_index  差し替えるマテリアルのインデックス(通常は0。複数存在するなら使用）
   * @param texture_index   差し替えるテクスチャのインデックス(通常は0。複数存在するなら使用）
   * 
   * @note  転送後２F後に反映します。１F目コマンドでメモリに配置、２F目ダブルバッファにて反映
   */
  void TransferTexture( gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index );
  //-----------------------------------------------------------------------
  // 書き込み
  //    ペインの左上が原点0,0で、右に向かってx+、下に向かってy+です。
  //-----------------------------------------------------------------------
  /**
   * @brief 点を書き込み
   *
   * @param vec 点座標
   * @param color 色  (0,0,0,0) 黒アルファあり 〜 (1.0,1.0,1.0,1.0) 白　アルファなし
   */
  void WritePoint( const gfl2::math::VEC2 & vec, const gfl2::math::VEC4 & color );
  /**
   * @brief 線を書き込み
   *
   * @param start 開始点
   * @param end   終了点
   */
  void WriteLine( const gfl2::math::VEC2 & start, const gfl2::math::VEC2 & end, const gfl2::math::VEC4 & color );

  /**
   * @brief 矩形塗りつぶし
   *
   * @param left_top      左上点
   * @param right_bottom  右下点
   * @param color         色
   */
  void FillRect( const gfl2::math::VEC2 & left_top, const gfl2::math::VEC2 & right_bottom, const gfl2::math::VEC4 & color );
  /**
   * @brief X方向にずらす
   *
   * @param start       座標X
   * @param width       座標Xからどのくらいの幅か
   * @param disntance   座標Xからどこに移動するか
   */
  void ShiftX( u32 start, u32 width, s32 disntance );

  /**
   * @brief 頂点テーブルから多角形を描画
   *
   * @param ver_tbl   頂点テーブル
   * @param ver_cnt   頂点数
   * @param color     色
   * @param is_paint  塗りつぶすかどうか（true:塗りつぶす）
   * @param is_clear  キャンバスをクリアしてから描画するか（true:クリアする）
   *
   */
  void WritePolygon( const gfl2::math::VEC2* ver_tbl, s32 ver_cnt, const gfl2::math::VEC4& color, bool is_paint, bool is_clear );

  /**
   * @brief 何も書かない状態にする
   */
  void Clear( void );
  //-----------------------------------------------------------------------
  // 取得
  //-----------------------------------------------------------------------
  /**
   * @brief レイアウトテクスチャデータ取得
   *
   * @return レイアウトテクスチャデータ
   */
  app::tool::LytTexData * GetLytTexData( void ) const { return m_tex_data; }
  
private:
  u32 Rgb( const gfl2::math::VEC4 & color );

  /**
   * @brief WritePolygon用ラインパラメータ
   */
  struct LineParam
  { 
    s32 x1;   ///<2点間線分のX座標1
    s32 y1;   ///<2点間線分のY座標1
    s32 x2;   ///<2点間線分のX座標2
    s32 y2;   ///<2点間線分のY座標2
    s32 dx;   ///<2点間のX距離
    s32 dy;   ///<2点間のY距離
    s32 sx;   ///<2点間のX方向
    s32 sy;   ///<2点間のY方向
    s32 dis;  ///<点を打った距離（これがdx以上になると描画終了）
    int E;    ///<この値が＋になったらYを操作するためのワーク（詳しくはプレゼンハムのアルゴリズム参照）
  };

  /**
   * @brief 多角形の頂点方向
   */
  enum
  { 
    BRANCH_L = 0, ///<左周り
    BRANCH_R,     ///<右周り
    BRANCH_MAX,   ///<多角形の頂点方向MAX
  };

  /**
   * @brief 多角形の頂点方向パラメータ
   */
  struct BranchParam
  { 
    bool            enable;   ///<有効かどうか
    bool            checked;  ///<チェックを開始したか
    LineParam       lp;       ///<ラインパラメータ
  };

  /**
   * @brief 多角形の頂点パラメータ
   */
  struct VertexParam
  { 
    bool        enable;           ///<有効かどうか
    s32         boot_line;        ///<描画を開始するライン
    BranchParam bp[ BRANCH_MAX ]; ///<頂点方向パラメータ
  };

  /**
   * @brief 多角形描画パラメータ
   */
  struct WritePolygonParam
  { 
    s32           search_line;  ///<現在描画しているライン
    VertexParam*  vertex;       ///<頂点パラメータ
  };

  /**
   * @brief LineParam初期化
   *
   * @param[out]  LineParam ラインパラメータ
   * @param[in]   start     線分の開始点
   * @param[in]   end       線分の終了点
   */
  void  InitLineParam( LineParam* line_param, const gfl2::math::VEC2* start_ver, const gfl2::math::VEC2* end_ver );

  /**
   * @brief LineParam計算をして線を描画
   *
   * @param[in,out] LineParam ラインパラメータ
   * @param[in]     color     描画線分のカラー
   *
   * @retval  true:描画中　false:描画終了
   */
  bool  CalcLineParam( LineParam* lp, const gfl2::math::VEC4& color );

private:
  gfl2::heap::HeapBase* m_heap;
  app::tool::LytTexData *m_tex_data;
  void *m_write_buffer;
  void *m_src_buffer;

};

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
#endif		// __APP_TOOL_DRAWLYTTEXTURE_H__
