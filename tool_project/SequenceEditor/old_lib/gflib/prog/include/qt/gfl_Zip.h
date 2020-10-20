/* 
 * File:   gfl_Zip.h
 * Author: s-kimura
 *
 * Created on 2010/06/14, 18:50
 * ���k
 */

#ifndef GFL_ZIP_H
#define	GFL_ZIP_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

/**
 *   zlib ���b�p�[�N���X
 */
class Zip {
public:
	/**
	 *   zlib �G���R�[�h�t�@�C�����f�R�[�h���ă������[�ɓW�J
	 * @param  dst �W�J���ꂽ�f�[�^
	 * @param  fname �t�@�C����
	 * @return �^�@�����@�U�@���s
	 */
	static bool DecodeFile( QByteArray* dst, const QString& fname );

	/**
	 *   �t�@�C�������k���ăt�@�C���Ƃ��ĕۑ�
	 * @param  rfname �ǂݍ��݃t�@�C����
	 * @return ����
	 */
	static void EncodeFile( const QString& rfname );
};

}
}

#endif	/* GFL_ZIP_H */

