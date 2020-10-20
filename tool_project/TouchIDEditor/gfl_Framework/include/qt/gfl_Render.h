/* 
 * File:   gfl_Render.h
 * Author: s-kimura
 *
 * Created on 2010/06/08, 14:07
 */

#ifndef GFL_RENDER_H
#define	GFL_RENDER_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

/**
 *   レンダークラス
 * @remark 簡単にプリミティブモデルをレンダリングするスタティック関数群
 */
class Render {
public:

	/**
	 * ポリゴン色
	 */
	enum PolygonColor {
		PCOL_WHITE, /**< 白 */
		PCOL_RED, /**< 赤 */
		PCOL_GREEN, /**< 緑 */
		PCOL_BLUE, /**< 青 */
		PCOL_YELLOW, /**< 黄色 */
		PCOL_GRAY, /**< 灰色 */

		PCOL_DARKRED, /**< 暗赤 */
		PCOL_DARKGREEN, /**< 暗緑 */
		PCOL_DARKBLUE, /**< 暗青 */
		PCOL_DARKYELLOW, /**< 暗黄色 */
		PCOL_DARKGRAY, /**< 暗灰色 */

		PCOL_SKYBLUE, /**< 空青色 */
		PCOL_MAX, /**< 色テーブル最大値 */
	};

	/**
	 * シェーディングのアサイン　SHD_
	 */
	enum ShadingType {
		SHD_NORMAL, /**< フラットシェーディング */
		SHD_WIRE, /**< ワイヤーフレーム */
		SHD_GOURAUD, /**< グーロー */
		SHD_TEXTURE, /**< テクスチャーあり */
		SHD_MAX, /**< シェーディング種類最大値 */
	};

	/**
	 * グラウンドタイプ
	 */
	enum GroundType {
		GROUND_NORMAL, /**< 原点に表示 */
		GROUND_CURRENT, /**< 基準点を中心に表示 */
		DEFAULT_GROUND_SIZE = 5,
	};

	enum Status {
		ST_NONE,
		ST_LOADING,
		ST_READY,
		ST_RENDERING
	};

	static const f32 _scColorTable[][4];
	static const u8 _scBoneEdge[][4];

	/**
	 * マテリアルカラーを設定
	 * @param col 色番号
	 */
	static void SetMaterialColor( const PolygonColor col );
	static void SetColor( const PolygonColor col );

	/**
	 *   三角形を表示
	 * @param  pos 表示する座標
	 * @param  trans_y pos の Y 座標に加算される値
	 * @param  rot_y 三角形の向き　Y軸角度
	 * @param  type ShadingType 表示方法
	 * @return 無し
	 */
	static void RotY( const gfl::core::Vector& pos, const f32 trans_y, const f32 rot_y, const ShadingType type );

	/**
	 *   四角形を表示
	 * @param  x X 座標
	 * @param  z Z 座標
	 * @param  type ShadingType 表示方法
	 * @return 無し
	 */
	static void Pivot( const f32 x, const f32 z, const ShadingType type );

	/**
	 *   三点から法線を計算して設定する
	 * @param  v0 座標
	 * @param  v1 座標
	 * @param  v2 座標
	 * @return 無し
	 */
	static void SetTriangleNormal( const gfl::core::Vector3& v0, const gfl::core::Vector3& v1, const gfl::core::Vector3 & v2 );

	/**
	 *   XYZ軸を表示
	 * @param  size 軸の長さ
	 * @param  line_size 軸の太さ
	 * @return 無し
	 */
	static void Axis( const f32 size, const f32 line_size = 1.0f );

	/**
	 *   立方体を表示
	 * @param  size 軸の長さ
	 * @param  line_size 軸の太さ
	 * @return 無し
	 */
	static void Cube( const f32 size, const f32 line_size = 1.0f );

	/**
	 *   骨モデルを表示
	 * @param  sx X 軸長さ
	 * @param  sy Y 軸長さ
	 * @param  sz Z 軸長さ
	 * @param  type 表示方法
	 * @param  cotex_no ポリゴン色
	 * @return 無し
	 */
	static void Bone( const f32 sx, const f32 sy, const f32 sz, const ShadingType type = SHD_NORMAL, const PolygonColor cotex_no = PCOL_WHITE );

	/**
	 *   地面をグリッドで表示
	 * @param  size 地面サイズ
	 * @param  kind 原点表示か追随表示か
	 * @param  ofs_x X 座標オフセット
	 * @param  ofs_z Z 座標オフセット
	 * @return 無し
	 */
	static void Ground( const s32 size = DEFAULT_GROUND_SIZE, const GroundType kind = GROUND_NORMAL, const f32 ofs_x = 0.0f, const f32 ofs_z = 0.0f );
};

}
}

#endif	/* GFL_RENDER_H */

