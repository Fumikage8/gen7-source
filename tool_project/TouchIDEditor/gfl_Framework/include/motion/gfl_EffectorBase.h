/* 
 * File:   gfl_Effector.h
 * Author: kimura_shin
 *
 * Created on 2010/12/02, 16:54
 */

#ifndef GFL_EFFECTOR_H
#define	GFL_EFFECTOR_H
#pragma once

namespace gfl {
namespace motion {

/**
 *  エフェクター基底クラス
 * @remark ＩＫの計算に必要
 */

class EffectorBase {
public:
	// グローバル座標（実体）
	gfl::core::Vector _Trans;
	// グローバル座標ポインタ
	gfl::core::Vector* _pTrans;

	// チェインのジョイント
	JointBase* _pJointBase_Chain;
	// 骨のジョイント
	JointBase* _pJointBase_Joint;
	// エフェクターのジョイント
	JointBase* _pJointBase_Effector;

	EffectorBase( void ) {
	}

	~EffectorBase( void ) {
	}

	/**
	 * @brief GetTrans
	 * @return 座標の実体のポインタ
	 */
	gfl::core::Vector* GetGlobalTrans( void ) {
		return _pTrans;
	}

	/**
	 * @brief GetJoint_Chain
	 * @return チェインのジョイントポインタ
	 */
	template<typename T>
	T GetJoint_Chain( void ) {
		return static_cast<T> (_pJointBase_Chain);
	}

	/**
	 * @brief GetJoint_Joint
	 * @return 骨のジョイントポインタ
	 */
	template<typename T>
	T GetJoint_Joint( void ) {
		return static_cast<T> (_pJointBase_Joint);
	}

	/**
	 * @brief GetJoint_Effector
	 * @return エフェクターのジョイントポインタ
	 */
	template<typename T>
	T GetJoint_Effector( void ) {
		return static_cast<T> (_pJointBase_Effector);
	}

	friend class JointBase;
	friend class MotionBase;
};

}
}

#endif	/* GFL_EFFECTOR_H */

