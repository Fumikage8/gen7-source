
���t�@�C��

	pawn\project\
		Pawn.vcxproj		PAWN�v���W�F�N�g

	pawn\include\			PAWN�C���N���[�h�t�@�C��
		amx.h
		amxcons.h
		amxdbg.h
		bugfix_pawn.h
		gfl2_Amx.h
		gfl2_Pawn.h			PAWN���b�v�N���X
		gfl2_PawnBase.h		PAWN���N���X
		gfl2_PawnDebug.h	PAWN�f�o�b�O�N���X
		osdefs.h
		include\
			args.inc
			console.inc
			core.inc
			datagram.inc
			default.inc
			file.inc
			fixed.inc
			float.inc
			gfl_func.inc
			process.inc
			rational.inc
			string.inc
			time.inc

	pawn\source\
		amx.c				gfl2_UnityAmx.c ��include����Ă���PAWN��{�\�[�X
		amxcore.c			gfl2_UnityAmx.c ��include����Ă���PAWN��{�\�[�X
		amxdbg.c			gfl2_UnityAmx.c ��include����Ă���PAWN��{�\�[�X
		amxprint.c			gfl2_UnityAmx.c ��include����Ă���PAWN��{�\�[�X
		float.c				gfl2_UnityAmx.c ��include����Ă���PAWN��{�\�[�X
		gfl2_UnityAmx.c		PAWN��{�\�[�X�����킹������
		gfl2_PawnBase.cpp	PAWN���N���X
		gfl2_PawnDebug.cpp	PAWN�f�o�b�O�N���X

	pawn\Sample\			�ȉ��̓T���v�����s�ɕK�v�ȃt�@�C��
		Sample.sln
		project\
			Sample.vcxproj
			test.amx
		source\
			main.cpp
			PawnTask.cpp
			PawnTask.h
		script\				�X�N���v�g
			console.inc
			convert.bat
			pawncompiler.exe
			test.p



���N���X�\��

	PawnTask(�T���v���\�[�X)
			�J�X�^���R�}���h��ǉ������N���X
		��	---------------------------------------------------------------------------------
	Pawn
			�v���b�g�t�H�[���A�n�[�h�E�F�A�Ɉˑ����鏈���̃J�X�^�}�C�Y
		��	---------------------------------------------------------------------------------
	PawnDebug(�f�o�b�O�^�[�Q�b�g�̂�)
			�f�o�b�O�@�\. PawnIDE�Ƃ̘A�g��hio���̒�
		��
	PawnBase
			PAWN�N���X���. PAWN�̊�{�@�\


	PawnDebug �ȉ��͊��Ɉˑ����Ȃ��A��{�I�ɏC������K�v�̂Ȃ��\�[�X
		���\��A14/10/28���݂ł�RTTI�Ahash�Ahio�Aheap�Ɉˑ����Ă���

	���Ɉˑ�����C���� Pawn �ŋz��(�������֘A)�B


�����p���@

	Pawn�N���X���p���������̂��쐬
		class PawnTask : public gfl::pawn::Pawn

	�X�N���v�g�R�}���h�֐����쐬
	cell AMX_NATIVE_CALL PawnTask::ExtScript( AMX*, const cell* ){
		return 0;
	}

	�R�}���h�֐��e�[�u�����쐬
	static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
	{
		GFL_PAWN_FUNC( ExtScript )
		GFL_PAWN_FUNC_END,
	};

	�e�[�u����o�^���鏈��
	RegisterPawnFunction( s_FuncTable );

	�X�N���v�g�o�C�i���[�f�[�^��ǂݍ��� Load �ɓn��
	size = FileLoad( "test.amx", pBuff );
	cPawn.Load( pBuff, iSize, "PawnTest" );

	GFL_PAWN_FINISHED ���Ԃ�܂� Updata �����s
	do{	iRes = cPawn.Update();
	}while( iRes != gfl::pawn::PawnBase::GFL_PAWN_FINISHED );




�����ˑ�

�@���ˑ��Ή��̂��ߊ֐��Q

	�������֘A���.
	virtual u8*	PawnNew( const s32 iSize, const s32 iCategory )
	virtual void	PawnDel( u8 *pPtr )
	
	����������֘A.
	virtual void	PawnMemCpy( void *pD, const void *pS, const s32 iLength )
	virtual void	PawnMemFill( void *pPtr, const u8 _uVal, const s32 iLength )

	PawnBase.h �Œ�`�B���ł̓X�^���_�[�h�ȏ������L�q���Ă���܂��B




���X�N���v�g�e�X�g

	�X�N���v�g�t�@�C���� test.p �ł��B
	convert.bat
	�����s���鎖�ŃR���p�C���A��ƃt�H���_�ɃR�s�[����܂��B(�f�o�b�O���s���邱�Ƃ��O��̃p�X)
	
	�X�N���v�g�t�@�C���� 10000byte �Ɏ��܂��Ă��邱�Ƃ��O��ł�
		pawn/Sample/source/main.cpp(26): 	u8			Mem[ 10000 ];




������t���[

	pawn/Sample/script/convert.bat
	�����s����� test.amx ���쐬�����
	
	pawn/Sample/source/PawnTask.cpp(44): cell AMX_NATIVE_CALL PawnTask::ExtScript( AMX*, const cell* )
	�Ƀu���[�N�𒣂��Ă����ƃX�N���v�g	pawn/Sample/script/test.p(12): 	ExtScript();
	�� ExtScript �R�}���h�����s����Ă��鎖���m�F�ł���
	
	pawn/Sample/source/main.cpp(94):     // ���͂�҂��ďI��
	��L���ɂ���ƁA�R�}���h���C�����s�ł��o�͌��ʂ��m�F�ł���B


