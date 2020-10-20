//==============================================================================
/**
 * @file	gfl_CsklaXml.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/27, 15:35
 */
// =============================================================================

#if !defined( __GFL_CSKLAXML_H__ )
#define	__GFL_CSKLAXML_H__

#include <gflib.h>
#include "gfl_Xml.h"

namespace gfl { namespace xml {

/**
 * Cskla Nintendo Animation Format
 */
class Cskla : public Document {
public:
	class Joint;

	/**
	 * アニメーションがあるジョイント
	 */
	class JointAnim {
	public:

		JointAnim( ) {
			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				m_pTranslate[i] = 0;
				m_pRotate[i] = 0;
				m_pScale[i] = 0;
			}
		}

		~JointAnim( ) {
			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				delete [] m_pTranslate[i];
				delete [] m_pRotate[i];
				delete [] m_pScale[i];
			}
		}

		f32* GetScale( const s32 id ) const {
			return m_pScale[id];
		}

		f32* GetRotate( const s32 id ) const {
			return m_pRotate[id];
		}

		f32* GetTranslate( const s32 id ) const {
			return m_pTranslate[id];
		}
		
		const QString& GetName( ) const {
			return m_Name;
		}
	private:
		QString m_Name;
		Joint* m_pJoint;
		f32 *m_pTranslate[gfl::core::TXYZ];
		f32 *m_pRotate[gfl::core::TXYZ];
		f32 *m_pScale[gfl::core::TXYZ];

		friend class Cskla;
	};

	/**
	 * Joint
	 */
	class Joint {
	public:
		Joint() : m_pJointAnim(0), m_pParentJoint(0) {
			m_Matrix.SetUnit();
		}
		
		Joint* GetParentJoint( ) const {
			return m_pParentJoint;
		}

		bool IsScaleCompensate( ) const {
			return m_ScaleCompensate;
		}

		JointAnim* GetJointAnim( ) const {
			return m_pJointAnim;
		}

		const gfl::core::Vector& GetScale( ) const {
			return m_Scale;
		}

		const gfl::core::Vector& GetRotate( ) const {
			return m_Rotate;
		}

		const gfl::core::Vector& GetTranslate( ) const {
			return m_Translate;
		}

        gfl::core::Matrix* GetMatrix( ){
        	return &m_Matrix;
        }

        QString GetName( ) const {
        	return m_Name;
        }
	private:
		QString m_Name;
		gfl::core::Matrix m_Matrix;
		gfl::core::Vector m_Translate;
		gfl::core::Vector m_Rotate;
		gfl::core::Vector m_Scale;
		JointAnim* m_pJointAnim;
		Joint* m_pParentJoint;
		bool m_ScaleCompensate;
		friend class Cskla;
	};

	Cskla( void ) {
	}

	virtual ~Cskla( void ) {
		while(!m_JointAnim.isEmpty( )) {
			delete m_JointAnim.takeFirst( );
		}
		while(!m_Joint.isEmpty( )) {
			delete m_Joint.takeFirst( );
		}
	}

	bool Load( const QString& fname );
//	void CalcMatrix( gfl::core::Matrix* matp, const f32 frame);
	s32* CreateModelIdTable( gfl::cmdl::Model* model );

	s32 GetFrameCount( ) const {
		return m_FrameCount;
	}

	QList<Joint*>* GetJoint( ) {
		return &m_Joint;
	}
private:
	QList<JointAnim*> m_JointAnim;
	QList<Joint*> m_Joint;
	s32 m_FrameCount;
};


} }

#endif	/* __GFL_CSKLAXML_H__ */

