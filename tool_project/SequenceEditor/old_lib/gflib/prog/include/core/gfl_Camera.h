/* 
 * File:   gfl_Camera.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 16:45
 * カメラ
 */

#ifndef GFL_CAMERA_H
#define	GFL_CAMERA_H
#pragma once

#define GFL_DEFAULT_NEAR_CLIP METER( 0.01f )
#define GFL_DEFAULT_FAR_CLIP METER( 400.0f )
#define GFL_DEFAULT_PERSPECTIVE 20.0f // degree

namespace gfl {
namespace core {

/**
 *   カメラクラス
 * @remark カメラ基底クラス　手っ取り早くテストするための簡素化されたカメラクラス
 */
class Camera {
public:

	/**
	 * カメラモード
	 */
	enum CameraMode {
		MODE_NORMAL,
		MODE_ORBIT,
	};

	// ここは簡単に使用したいのでカプセル化していません
	gfl::core::Matrix44 _Matrix;
	gfl::core::Vector _Trans;
	gfl::core::Vector _Interest;
	gfl::core::Vector _Rotation;
	f32 _ControlDistance;
	f32 _ControlRotationY;
	f32 _ControlRotationX;
	f32 _NearClip;
	f32 _FarClip;
	f32 _Perspective;
	u8 _Mode;

	/**
	 *   コンストラクタ
	 * @param  無し
	 * @return 無し
	 */
	Camera( void );

	/**
	 *   注視点から座標を設定する
	 * @param  無し
	 * @return 無し
	 */
	void SetTransByInterest( void );

	/**
	 *   注視点から角度を設定する
	 * @param  無し
	 * @return 無し
	 */
	void SetRotationByInterest( void );

	/**
	 *   座標からマトリックスを設定する
	 * @param  無し
	 * @return 無し
	 */
	void SetMatrixByPosition( f32 scale = 1.0f ); // 11.11.22 funakoshi 平行投影時に奥行きを表現するためscaleを採用

	/**
	 *   注視点座標を設定する
	 * @param  pos 注視点座標
	 * @return 無し
	 * @remark カメラの座標と角度が設定される
	 */
	void SetInterest( const gfl::core::Vector& pos );
	void UpdateMatrixByInterest( f32 scale = 1.0f ); // 11.11.22 funakoshi 平行投影時に奥行きを表現するためscaleを採用

	/**
	 *   カメラの X 角度に加算する
	 * @param  ang 加算する X 角度
	 * @return 無し
	 * @remark 上限までいってもひっくり返らないように制限がかかっている
	 */
	void AddRotationX( const f32 ang );

	/**
	 *   カメラの Y角度に加算する
	 * @param  ang 加算する Y 角度
	 * @return 無し
	 */

	inline void AddRotationY( const f32 ang ) {
		_ControlRotationY = gfl::core::Math::AddRadian( _ControlRotationY, ang );
	}

	/**
	 *   カメラと注視点の距離に加算する
	 * @param  dist 加算する距離
	 * @return 無し
	 */
	void AddDistance( const f32 dist );
};

}
}

#endif	/* GFL_CAMERA_H */

