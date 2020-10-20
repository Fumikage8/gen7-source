/*
 * File:   GFL_MOTIONBASE.h
 * Author: s-kimura
 *
 * Created on 2010/07/06, 18:12
 */

#ifndef GFL_MOTIONBASE_H
#define	GFL_MOTIONBASE_H
#pragma once

#define IK_BEND_Z (0)
#define IK_BEND_Y (!IK_BEND_Z)
#define USE_LOCATOR_Y
#define GFL_CALC_SLOW_MOTION 1

namespace gfl {
namespace motion {

/**
 *  モーション基底クラス
 */

class MotionBase {
	GFL_FORBID_COPY_AND_ASSIGN( MotionBase );

public:

	enum {
		MOTION_SUM, /* モーションの総数 GetData()[Ex::MotionBase::MOTION_SUM] で取得可能 */
		MOTION_BASEPOSE,
		MOTION_BEGIN,

		JOINT_NO_USE = -1,
#ifdef USE_LOCATOR_Y
		LOCATOR_ROT_Y = 0, /**< モーションデータのはじめはロケーターＹ角度 */
		LOCATOR_TRANS_Y, /**< ロケーターＹ座標 */
		LOCATOR_DATA, /**< ロケーターデータ数 */

		ROOT_DATA_BEGIN = LOCATOR_DATA, /**< ルートデータの始まり */
#endif
	};

private:
	// FunctionCurve のインスタンス
	Fcurve _Fcurve;
	// Fcurve を解凍したモーションデータポインタ
	f32* _pValue;
#if GFL_CALC_SLOW_MOTION
	f32* _pNextValue;
#endif
	u32* _pData;
	// 絶対ＩＤのテーブルポインタ
#if MATCH_JOINT_BY_ID
	u8* _pIdTable;
#elif MATCH_JOINT_BY_HASH
	u32* _pHashTable;
#elif MATCH_JOINT_BY_NAME
	c8* _pNameTable;
	u8* m_pTextureTypeTable;
#endif
protected:
	gfl::heap::HeapBase* m_pHeapBase;
	// JointBaseのポインタ
	JointBase* _pJointBase;
	EffectorBase* _pEffectorBase;

private:
	// ジョイント数
	u8 _JointCount;
	// モーション番号
	// エフェクター数
	u8 _EffectorCount;
	u16 _MotionNumber;

	// ジョイントサイズ
	u16 _JointSize;
	// エフェクターサイズ
	u16 _EffectorSize;

	u8 m_ScaleNoInherit;

public:

	MotionBase( void ) : _pValue( 0 )
#if GFL_CALC_SLOW_MOTION
	, _pNextValue( 0 )
#endif
	, _pData( 0 ), _pJointBase( 0 )	, _pEffectorBase( 0 ), _MotionNumber( 0 ), m_ScaleNoInherit( 0 ) {
	}

	virtual ~MotionBase( void );

	/**
	 * @brief Initialize
	 * @param data_void モーションデータポインタ
	 * @param heap ヒープ
	 * @param model　モデルポインタ
	 * @return 真　成功
	 */
	bool Initialize( void* data_void, gfl::heap::HeapBase* heap = 0 );

	/**
	 * @brief SetTransRot
	 * @param jnt　ジョイントポインタ
	 * @param vp　ファンクションカーブ計算結果ポインタ
	 * @param count　セットする数
	 * @return 進めたファンクションカーブ計算結果ポインタ
	 */
	f32* SetTransRot( JointBase* jnt, f32* vp, const s32 count );
	/**
	 * @brief SetTransRot
	 * @param jnt ジョイントポインタ
	 * @param vp ファンクションカーブ計算結果ポインタ
	 * @param count　セットする数
	 * @param jnt_begin 開始ジョイントポインタ
	 * @param jnt_end　終了ジョイントポインタ
	 * @return 進めたファンクションカーブ計算結果ポインタ
	 */
	f32* SetTransRot( JointBase* jnt, f32* vp, const s32 count, JointBase* jnt_begin, JointBase* jnt_end );

	/**
	 * @brief SetValue
	 * @param vp ファンクションカーブ計算結果ポインタ
	 */
	void SetValue( f32* vp ) {
		_pValue = vp;
	}

	/**
	 * @brief SetJointSize
	 * @param size ジョイント数
	 */
	void SetJointSize( const s32 size ) {
		_JointSize = size;
	}

	/**
	 * @brief SetEffectorSize
	 * @param size エフェクター数
	 */
	void SetEffectorSize( const s32 size ) {
		_EffectorSize = size;
	}

	/**
	 * @brief SetBasePoseMatrix
	 * @param jnt ジョイントポインタ
	 * @param count　セットする数
	 */
	void SetBasePoseMatrix( JointBase* jnt, const s32 count );

	/**
	 * @brief SetBasePoseMatrix
	 */
	inline void SetBasePoseMatrix( void ) {
		SetBasePoseMatrix( GetJoint<JointBase*>(), GetJointCount( ) );
	}

	// =====================================================================
	// 仮想関数群 始まり
	// =====================================================================

	/**
	 *   JointBase インスタンスの作成
	 * @param  count インスタンスの数
	 * @return インスタンス
	 * @remark JointBase を継承したクラスの作成に必要です\n
	 *         LoadBasePose() からコールされます
	 *         そのまま使う場合があるので純粋仮想関数にしていません
	 */

	virtual JointBase* CreateJoint( const s32 count ) {
		// ここがミソ　JointBase を継承したクラスを設定してください
		SetJointSize( sizeof (JointBase) );
		return GFL_NEW_ARRAY( m_pHeapBase ) JointBase [count];
	}

	/**
	 *   JointBase インスタンスの破棄
	 * @param  なし
	 * @return なし
	 * @ramark 継承先のデストラクタで呼んでください。
	 *         そのまま使う場合があるので純粋仮想関数にしていません
	 */

	virtual void DestroyJoint( void ) {
		GFL_DELETE_ARRAY GetJoint<JointBase*>();
	}

	/**
	 *   EffectorBase インスタンスの作成
	 * @param  なし
	 * @return インスタンス
	 * @remark EffectorBase を継承したクラスの作成に必要です\n
	 *         LoadBasePose() からコールされます
	 */

	virtual EffectorBase* CreateEffector( const s32 count ) {
		SetEffectorSize( sizeof (EffectorBase) );
		return GFL_NEW_ARRAY( m_pHeapBase ) EffectorBase [count];
	}

	/**
	 * @brief DestroyEffector
	 */
	virtual void DestroyEffector( void ) {
		GFL_DELETE_ARRAY GetEffector<EffectorBase*>();
	}

	// =====================================================================
	// 仮想関数群 終わり
	// =====================================================================

	/**
	 *   更新
	 * @param  なし
	 * @return なし
	 */

	void UpdateMotionMatrix( void );

	/**
	 *   ジョイントのＩＤテーブルポインタの取得
	 * @param  なし
	 * @return ジョイントのＩＤテーブルポインタ
	 */

#if MATCH_JOINT_BY_ID

	u8* GetIdTable( void ) {
		return _pIdTable;
	}
#elif MATCH_JOINT_BY_HASH

	u32* GetHashTable( void ) {
		return _pHashTable;
	}
#elif MATCH_JOINT_BY_NAME

	c8* GetNameTable( void ) {
		return _pNameTable;
	}
#endif
#if GFL_DEBUG || defined(GFL_PLATFORM_PC)
	void SetBoneLength( void );
#endif

	/**
	 *   ジョイント数の取得
	 * @param  なし
	 * @return ジョイント数
	 */

	s32 GetJointCount( void ) const {
		return _JointCount;
	}

	/**
	 *   モーション番号の取得
	 * @param  なし
	 * @return モーション番号
	 */

	s32 GetMotionNumber( void ) const {
		return _MotionNumber;
	}

	/**
	 *   フレーム数の取得
	 * @param  なし
	 * @return フレーム数
	 */

	f32 GetFrameCount( void ) {
		return GetFcurve( )->GetFrameCount( );
	}

	/**
	 *   現在のフレームを取得
	 * @param  なし
	 * @return 現在のフレーム
	 */

	f32 GetFrame( void ) {
		return GetFcurve( )->GetFrame( );
	}

	/**
	 *   フレームを設定
	 * @param  frame フレーム
	 * @return なし
	 */
	void SetFrameAndClear( const f32 frame );

	/**
	 *   再生スピードを取得
	 * @param  なし
	 * @return 再生スピード
	 */

	f32 GetFrameSpeed( void ) const {
		return _Fcurve.GetFrameSpeed( );
	}

	/**
	 *   再生スピードを設定
	 * @param  spd 再生スピード
	 * @return なし
	 */

	void SetFrameSpeed( const f32 spd ) {
		_Fcurve.SetFrameSpeed( spd );
	}

	/**
	 *   モーションが終了したか？
	 * @param  なし
	 * @return 真　終了　偽　未終了
	 */

	bool QueryEnd( void ) {
		return GetFcurve( )->QueryEnd( );
	}

	/**
	 *   モーションが終了したか？
	 * @param  なし
	 * @return 真　終了　偽　未終了
	 * @remark QueryEnd はフレーム更新の後でチェックする場合
	 */

	bool QueryEndBefore( void ) {
		return GetFcurve( )->QueryEndBefore( );
	}

	/**
	 *   モーションデータのロード
	 * @param  data データポインタ
	 * @return 真　成功　偽　失敗
	 */
	bool Load( const void* data );

	/**
	 * モーションがあるのか？
	 * @param num モーション番号
	 * @return 　真　０でない　偽　０ 本当はオフセット値
	 */
	size_t IsAvailable( const s32 num );
	/**
	 *   モーション変更
	 * @param  num モーション番号
	 * @return 真　成功　偽　失敗
	 */
	virtual bool Change( const s32 num );

	/**
	 *   モーションを再スタート
	 * @param  なし
	 * @return なし
	 */

	inline void Repeat( void ) {
		Change( GetMotionNumber( ) );
		//SetSmoothFrameBefore(0.0f);
	}

	/**
	 *   モーション数の取得
	 * @param  なし
	 * @return モーション数
	 */

	f32 GetMotionCount( void ) {
		return GetData( )[MOTION_SUM];
	}

	/**
	 *   時間の更新
	 * @param  なし
	 * @return なし
	 */
	void UpdateFrame( void );

	/**
	 *   Fcurve を解凍したデータポインタの取得
	 * @param  なし
	 * @return データポインタ
	 */

	f32* GetValue( void ) {
		return _pValue;
	}

	/**
	 *   Fcurve を解凍したデータポインタの取得
	 * @param  なし
	 * @return データポインタ
	 */
	const f32* GetValue( void ) const {
		return _pValue;
	}

	f32 GetTextureValue( const s32 id ) {
		return _Fcurve.GetTextureValue( id );
	}

	/**
	 *   ロケータＹＲｏｔを取得
	 * @param  無し
	 * @return ロケータＹＲｏｔ
	 */

	f32 GetMotionRotY( void ) const {
		return 0.0f;
		//			return GetValue( )[LOCATOR_ROT_Y];
	}

	/**
	 *   ロケータＹtransを取得
	 * @param  無し
	 * @return ロケータＹtrans
	 */

	f32 GetMotionTransY( void ) const {
#if 0 //
		//return 0.0f;
		//	return GetValue( )[LOCATOR_TRANS_Y];
		if( GetValue( ) ){
			return GetMotionRootDataPointer( )[gfl::core::TY];
		}
		return 0.0f;
#else
		JointBase* jnt = _pJointBase;
		return (jnt) ? jnt->_pTrans->GetY( ) : 0.0f;
#endif
	}

	/**
	 *   ロケータtransを取得
	 * @param  無し
	 * @return ロケータtrans
	 */

	const gfl::core::Vector* GetMotionTrans( void ) const {
		JointBase* jnt = _pJointBase;
		return (jnt) ? jnt->_pTrans : (const gfl::core::Vector*) & gfl::core::Vector::GetZero( );
	}

	/**
	 *   ルートデータポインタを取得
	 * @param  無し
	 * @return ルートデータポインタ
	 */

	const f32* GetMotionRootDataPointer( void ) const {
#ifdef USE_LOCATOR_Y
		return &GetValue( )[ROOT_DATA_BEGIN];
#else
		return GetValue( );
#endif
	}

	/**
	 *   Fcurveポインタ の取得
	 * @param  なし
	 * @return Fcurveポインタ
	 */

	Fcurve* GetFcurve( void ) {
		return &_Fcurve;
	}

	/**
	 *   Fcurveポインタ の取得
	 * @param  なし
	 * @return Fcurveポインタ
	 */
	const Fcurve* GetFcurve( void ) const {
		return &_Fcurve;
	}

	/**
	 *   モーションアドレスを取得
	 * @param  なし
	 * @return モーションアドレス
	 */

	u32* GetData( void ) {
		return _pData;
	}

	/**
	 *   モーションアドレスを取得
	 * @param  idx インデックス
	 * @return モーションアドレス
	 */

	u32* GetData( const s32 idx ) {
		u32* data = GetData( );
		if( data ){
			size_t ofs = (size_t) data[idx];
			return (ofs) ? reinterpret_cast<u32*> ((size_t) data + ofs) : 0;
		}
		return 0;
	}

	/**
	 *   ジョイントポインタの先頭を取得
	 * @return ジョイントポインタ
	 * @remark ジョイントポインタの先頭
	 */

	template<typename T>
	T GetJoint( void ) {
		return static_cast<T> (_pJointBase);
	}
	
	/**
	 *   ID番目のジョイントポインタを取得
	 * @return ジョイントポインタ
	 */
	JointBase* GetJointAddress( const s32 id ){
		return gfl::core::Utility::AddAddress( _pJointBase, id * _JointSize );
	}

	/**
	 *   ジョイントポインタの取得
	 * @param  id 配列番号
	 * @return ジョイントポインタ
	 * @remark 絶対ＩＤではなく、単に配列番号で取得
	 */

	template<typename T>
	T GetJoint( const s32 id ) {
		return static_cast<T> (
			((id < GetJointCount( ))
			? GetJointAddress(id)
			: _pJointBase));
	}

	/**
	 * @brief GetEffectorCount
	 * @return エフェクター数
	 */
	s32 GetEffectorCount( void ) const {
		return _EffectorCount;
	}

	/**
	 * @brief GetEffector
	 * @return エフェクター
	 */
	template<typename T>
	T GetEffector( void ) {
		return static_cast<T> (_pEffectorBase);
	}

#if MATCH_JOINT_BY_HASH

	template<typename T>
	T GetJointByHash( const u32 id ) {
		for( s32 i = 0; i < GetJointCount( ); ++i ){
			if( id == _pHashTable[i] ){
				return GetJoint<T > (i);
			}
		}
		return 0;
	}
#elif MATCH_JOINT_BY_NAME
	/**
	 * @brief GetJointByName
	 * @param name ジョイント名文字列
	 * @return ジョイントポインタ
	 */
	template<typename T>
	T GetJointByName( const c8 * const name ) {
		for( s32 i = 1; i < GetJointCount( ); ++i ){
			T jnt = GetJoint<T > (i);
			if( 0 == strcmp( name, jnt->_pName ) ){
				return jnt;
			}
		}
		return 0;
	}
#endif

	/**
	 * セッティングデータ設定
	 * @param data　読み始めデータポインタ
	 * @return 読み終わりデータポインタ
	 */
	virtual u8* LoadSetting( u8* data, gfl::heap::HeapBase* heap );

	/**
	 *   絶対番号によるモーションジョイントの取得
	 * @param  abs_id 絶対番号
	 * @return モーションジョイント番号
	 */
	s32 GetMotionJointId( const u32 abs_id );

	/**
	 * @brief NextJoint
	 * @param jnt ジョイントポインタ
	 * @return 次のジョイントポインタ
	 */
	template<typename T>
	inline T NextJoint( T jnt ) {
		return gfl::core::Utility::AddAddress( jnt, _JointSize );
	}

	/**
	 * @brief PreviousJoint
	 * @param jnt ジョイントポインタ
	 * @return 前のジョイントポインタ
	 */
	template<typename T>
	inline T PreviousJoint( T jnt ) {
		return gfl::core::Utility::SubAddress( jnt, _JointSize );
	}

	/**
	 * @brief NextEffector
	 * @param eff エフェクターポインタ
	 * @return 次のエフェクターポインタ
	 */
	template<typename T>
	inline T NextEffector( T eff ) {
		return gfl::core::Utility::AddAddress( eff, _EffectorSize );
	}

	/**
	 * @brief PreviousEffector
	 * @param eff エフェクターポインタ
	 * @return 前のエフェクターポインタ
	 */
	template<typename T>
	inline T PreviousEffector( T eff ) {
		return gfl::core::Utility::SubAddress( eff, _EffectorSize );
	}

	/**
	 * @brief CalcIK
	 * @param jnt ジョイントポインタ
	 * @param count 計算するジョイント数
	 * @param type　どちらのIK モードか
	 */
	virtual void CalcIK( JointBase* jnt, const s32 count, const size_t ofs );

	/**
	 * @brief GetHeapBase
	 * @return ヒープ
	 */
	gfl::heap::HeapBase* GetHeapBase( ) const {
		return m_pHeapBase;
	}

	/**
	 * @brief SetGlobalRotation
	 * @param jnt ジョイントポインタ
	 * @param global_mat　計算元ルートマトリックス
	 * @param count　計算するジョイント数
	 */
	void SetGlobalRotation( JointBase* jnt, const gfl::core::Matrix& global_mat, const s32 count );

	enum {
		TEX_OTHER,
		TEX_EYE,
		TEX_MOUTH,
	};
#ifdef USE_TEXTURE_ANIMATION

	f32 GetCurrentTextureFrame( const s32 id ) {
		return _pValue[GetJointCount( ) * gfl::core::TXYZ * 3 + id];
	}

	u8* GetTextureTypeTable( ) const {
		return m_pTextureTypeTable;
	}

	u8 GetTextureType( const s32 id ) const {
		return m_pTextureTypeTable[id];
	}
#endif
	/**
	 * @brief CalculateMatrix
	 * @param jnt ジョイントポインタ
	 * @param count　計算するジョイント数
	 */
	void CalculateMatrix( JointBase* jnt, const s32 count );

	/**
	 * @brief SetScaleNoInherit
	 * @param ScaleNoInherit スケールを親から継承するか
	 */
	void SetScaleNoInherit( u8 ScaleNoInherit ) {
		this->m_ScaleNoInherit = ScaleNoInherit;
	}

	/**
	 * @brief GetScaleNoInherit
	 * @return 真　スケールを継承する　偽　スケールを継承しない
	 */
	u8 GetScaleNoInherit( ) const {
		return m_ScaleNoInherit;
	}

	/**
	 * モーション番号のフレーム数を取得
	 * @param motnum モーション番号
	 * @return フレーム数
	 */
	u16 GetFrameCount( const s32 motnum );

#if GFL_CALC_SLOW_MOTION
	/**
	 * @brief SetNextValue
	 * @param _pNextValue 次のフレームの計算結果の収納場所
	 */
    void SetNextValue( f32* _pNextValue ) {
    	this->_pNextValue = _pNextValue;
    }

	/**
	 * @brief GetNextValue
	 * @return 次のフレームの計算結果の収納場所
	 */
    f32* GetNextValue( ) const {
    	return _pNextValue;
    }
#endif
};

}
}

#endif	/* GFL_MOTIONBASE_H */

