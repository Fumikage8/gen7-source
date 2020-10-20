/* 
 * File:   gfl_QtFile.h
 * Author: kimura_shin
 *
 * Created on 2010/12/03, 16:33
 */

#ifndef GFL_QTFILE_H
#define	GFL_QTFILE_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

#define LINK_KEYWORD "GFL_FILE_LINKED"

class File {
public:

	enum {
		MODE_LITTLE_ENDIAN = 0,
		//		MODE_BIG_ENDIAN, // ����͂���Ȃ��H
		MODE_ENDIAN_MAX,
	};

	typedef u8* (*MemCatByEndian)(u8* dst, const void* src, const s32 size);
	static u8* MemCatEndianSwap( u8* dst, const void* src, const s32 size );
	static u8* MemCatNoEndianSwap( u8* dst, const void* src, const s32 size );
	/**
	 * �t�@�C�������[�h
	 * @param fname �t�@�C����
	 * @param flag �t���O�@�e�L�X�g���o�C�i������
	 * @return�@�t�@�C���f�[�^
	 */
	static QByteArray Load( const QString& fname, const QIODevice::OpenModeFlag flag = QIODevice::Text );

	static const size_t LINE_BUFFER_SIZE = 0x200;
	/**
	 * �e�L�X�g�t�@�C���s�P�ʓǂݍ���
	 * @param fname�@�t�@�C����
	 * @param remove_crlf ���s�R�[�h���폜���邩�H �f�t�H���g���^
	 * @return �s�P�ʕ�����
	 * @remark ��s�̒����ɐ���(LINE_BUFFER_SIZE)������܂��B
	 */
	static QStringList LoadLines( const QString& fname, const bool remove_crlf = true );

	static QByteArray LoadBinary( const QString& fname, const QIODevice::OpenModeFlag flag = QIODevice::ReadOnly ) {
		return Load( fname, flag );
	}

	/**
	 * �t�@�C����ۑ�
	 * @param fname�@�t�@�C����
	 * @param dat�@�������ރf�[�^
	 * @param flag�@�t���O�@�e�L�X�g���o�C�i������
	 * @return �^�@�ۑ������@�U�@���s
	 */
	static bool Save( const QString& fname, const QByteArray& dat, const QIODevice::OpenModeFlag flag = QIODevice::Text );

	static bool SaveBinary( const QString& fname, const QByteArray& dat, const QIODevice::OpenModeFlag flag = QIODevice::ReadOnly ) {
		return Save( fname, dat, flag );
	}

	/**
	 * �o�C�i���[�t�@�C���������N����
	 * @param save_fname�@�����N���ꂽ�t�@�C����
	 * @param fnames�@�����N����t�@�C�������X�g
	 * @param endian�@�G���f�B�A��
	 * @param align_size�@�A���C�������g�T�C�Y
	 * @param header�@�w�b�_�t�@�C���������������H
	 * @return �^�@�����@�U�@���s
	 */
	static bool LinkBinaryFiles( const QString& save_fname, const QStringList& fnames, const s32 endian = MODE_LITTLE_ENDIAN
								 , const u32 align_size = sizeof (u32), bool header = true );

	/**
	 * �C���^�C���X�^���v�̔�r
	 * @param orgfname�@���t�@�C����
	 * @param tagfname�@��t�@�C����
	 * @return �^�@���t�@�C���̕��������炵�����A��t�@�C�����Ȃ�
	 */
	static bool CheckModified( const QString& orgfname, const QString& tagfname );

	/**
	 * �C���^�C���X�^���v�̔�r
	 * @param orgtime�@�t�@�C���̌��̏C������
	 * @param fname�@�t�@�C����
	 * @return �^�@�t�@�C�����X�V���ꂽ
	 */
	static bool CheckModified( const QDateTime& orgtime, const QString& fname );

	/**
	 * �C���^�C���X�^���v�̎擾
	 * @param fname �t�@�C����
	 * @return �C������
	 */
	static QDateTime GetModifiedTime( const QString& fname );
	
	/**
	 * ��΃p�X�֕ϊ�
     * @param fname ���΃p�X�t�@�C����
     * @return ��΃p�X�t�@�C����
     */
	static QString ToAbsoluteFilePath( const QString& fname );
};

}}

#endif	/* GFL_QTFILE_H */

