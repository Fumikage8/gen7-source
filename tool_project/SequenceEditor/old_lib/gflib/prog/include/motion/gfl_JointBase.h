/* 
 * File:   gfl_JointBase.h
 * Author: kimura_shin
 *
 * Created on 2010/12/02, 16:52
 */

#ifndef GFL_JOINTBASE_H
#define	GFL_JOINTBASE_H
#pragma once

namespace gfl {
namespace motion {

class EffectorBase;
// =============================================================================
// JointBase
// =============================================================================

class JointBase {
public:

	enum {
		IK_NO, // なんでもない
		IK_EFFECTOR_CHILD, // エフェクターの子供
		IK_TIP, // エフェクターの先っちょ
		IK_CHAIN, // チェインルート
		IK_JOINT, // ジョイント

		IK_EFFECTOR = 0x08,
		IK_EFFECTOR_R,
		IK_EFFECTOR_2,
		IK_EFFECTOR_2_R,

		IK_SKIP_CALC = 0x10, // このオフセットを加算するとスキップ

		// ビット操作で種類を判断したいので
		IK_EFFECTOR_BIT = 0x08,
		IK_EFFECTOR_REVERSE_BIT = 0x01,
		IK_EFFECTOR_2_BIT = 0x02
	};

	JointBase( void ) :
#if MATCH_JOINT_BY_NAME
	_pName( 0 ),
#endif
	_IkType( IK_NO ) {
		_Matrix.SetUnit( );
		_StaticRot.SetZero( );
		_Scale.Set( 1.0f, 1.0f, 1.0f );
	}

	gfl::core::Matrix _MotionMatrix;
	gfl::core::Matrix _Matrix;
	// 16 バイトアラインメントが必要なプラットフォーム
#ifdef GFL_QUAT_NEEDS_16BYTE_ALIGN
	gfl::core::Quaternion _BasePoseQuat;
#endif
	gfl::core::Quaternion _Quat;
	gfl::core::Vector _StaticTrans;
	gfl::core::Vector _StaticRot;
	// ベースポーズで親ジョイントから相対Quaternion
	gfl::core::Vector _BasePoseLocalRot;
	// 親ジョイントからの相対角度
	gfl::core::Vector _Rot;
	// スケール
	gfl::core::Vector _Scale;

	// 親ジョイントからの相対座標ポインタ（モーションデータ内を指している）
	gfl::core::Vector* _pStaticTrans;
	// モーションデータエフェクター座標ポインタ（モーションデータ内を指している）
	gfl::core::Vector* _pTrans;

	JointBase* _pParent;
	// エフェクタークラス
	EffectorBase* _pEffectorBase;
#ifndef GFL_QUAT_NEEDS_16BYTE_ALIGN
	gfl::core::Quaternion* _pBasePoseQuat;
#endif
#if MATCH_JOINT_BY_NAME
	// 名前文字列
	c8* _pName;
#endif

#if GFL_DEBUG || defined(GFL_PLATFORM_PC)
	f32 _Length;
#endif
	u8 _Id;
	u8 _IkType;
	u8 _ChildCount;
	// スケールデータが存在するか？
	u8 _ScaleExist;

	/**
	 * @brief GetMotionMatrix
	 * @return モーションデータマトリックス
	 */
	gfl::core::Matrix* GetMotionMatrix( void ) {
		return &_MotionMatrix;
	}

	/**
	 * @brief GetMatrix
	 * @return　グローバルマトリックス
	 */
	gfl::core::Matrix* GetMatrix( void ) {
		return &_Matrix;
	}

	/**
	 * @brief GetMatrix
	 * @return グローバルマトリックス
	 */
	const gfl::core::Matrix* GetMatrix( void ) const {
		return &_Matrix;
	}

	/**
	 * @brief GetTrans
	 * @return 座標ポインタ
	 */
	gfl::core::Vector* GetTrans( void ) {
		return _pTrans;
	}

	/**
	 * @brief GetStaticTrans
	 * @return 静的相対座標
	 */
	const gfl::core::Vector* GetStaticTrans( void ) const {
		return _pStaticTrans;
	}

	/**
	 * @brief GetTrans
	 * @return 座標ポインタ
	 */
	const gfl::core::Vector* GetTrans( void ) const {
		return _pTrans;
	}

	/**
	 * @brief SetTrans
	 * @param ptr 座標ポインタ
	 */
	void SetTrans( gfl::core::Vector* ptr ) {
		_pTrans = ptr;
	}

	/**
	 * @brief GetRot
	 * @return ローカル角度
	 */
	gfl::core::Vector* GetRot( void ) {
		return &_Rot;
	}

	/**
	 * @brief GetRot
	 * @return ローカル角度
	 */
	const gfl::core::Vector* GetRot( void ) const {
		return &_Rot;
	}

	/**
	 * @brief GetBasePoseQuat
	 * @return ベースポーズのグローバルクォータニオン
	 */
	const gfl::core::Quaternion& GetBasePoseQuat( void ) const {
#ifdef GFL_QUAT_NEEDS_16BYTE_ALIGN
		return _BasePoseQuat;
#else
		return *_pBasePoseQuat;
#endif
	}

	/**
	 * @brief GetQuat
	 * @return ローカルクォータニオン
	 */
	gfl::core::Quaternion* GetQuat( void ) {
		return &_Quat;
	}

	/**
	 * @brief GetBasePoseLocalRot
	 * @return ベースポーズローカル角度
	 */
	const gfl::core::Vector& GetBasePoseLocalRot( void ) const {
		return _BasePoseLocalRot;
	}

	/**
	 * @brief GetIkType
	 * @return IK 型
	 */
	u8 GetIkType( void ) const {
		return _IkType;
	}

	/**
	 * @brief SetIkType
	 * @param val IK 型
	 */
	void SetIkType( const u8 val ) {
		_IkType = val;
	}

	/**
	 * @brief GetParent
	 * @return 親ジョイント
	 */
	JointBase* GetParent( void ) {
		return _pParent;
	}

	/**
	 * @brief GetParent
	 * @return 親ジョイント
	 */
	const JointBase* GetParent( void ) const {
		return _pParent;
	}

	/**
	 * @brief SetParent
	 * @param jnt 親ジョイント
	 */
	void SetParent( JointBase* jnt ) {
		_pParent = jnt;
	}

	/**
	 * @brief GetScale
	 * @return スケールポインタ
	 */
	gfl::core::Vector* GetScale( void ) {
		return &_Scale;
	}

	/**
	 * @brief GetScale
	 * @return スケールポインタ
	 */
	const gfl::core::Vector* GetScale( void ) const {
		return &_Scale;
	}

	/**
	 * @brief GetId
	 * @return ID
	 */
	u8 GetId( ) const {
		return _Id;
	}

	/**
	 * @brief GetName
	 * @return 名前文字列
	 */
	c8* GetName(){
		return _pName;
	}

	/**
	 * @brief GetName
	 * @return 名前文字列
	 */
	const c8* GetName() const {
		return _pName;
	}

	/**
	 * @brief GetChildCount
	 * @return 子供ジョイントの数
	 */
	u8 GetChildCount() const {
		return _ChildCount;
	}

#if GFL_DEBUG || defined( GFL_PLATFORM_PC )
	/**
	 * @brief GetLength
	 * @return ジョイントの長さ
	 */
	f32 GetLength() const {
		return _Length;
	}
	/**
	 * @brief SetLength
	 * @param len ジョイントの長さ
	 */
	void SetLength(const f32 len){
		_Length = len;
	}

#endif
	friend class MotionBase;
};

}
}

#endif	/* GFL_JOINTBASE_H */

