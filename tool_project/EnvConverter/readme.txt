gfenv��gfbenv�ɃR���o�[�g����c�[���uEnvConverter�v


////////////////////////////////////////////////////////////////////////////////
����

���ŋ�

2015.05.13  kawada_koji@gamefreak.co.jp
    �o�[�W����1.0�쐬

2015.04.30  kawada_koji@gamefreak.co.jp
    �o�[�W����0.1�쐬

2015.04.15  kawada_koji@gamefreak.co.jp
    �o�[�W����0.0�쐬

����


////////////////////////////////////////////////////////////////////////////////
�g����
EnvConverter.exe -i inFileName -o outFileName

��
EnvConverter.exe -i dirBbb/dirCcc/fileAaa.gfenv -o dirDdd/dirEee/fileAaa.gfbenv

Win32�ł�CTR�ł̋�ʂ͂���܂���B


////////////////////////////////////////////////////////////////////////////////
EnvConverter.sln�̊J����
EnvConverter.bat�ŊJ���B


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
gfbenv�o�[�W����1.0�̐���

gfbenv�̃o�[�W����1.0
gfenv�̃o�[�W����0.8��1.0
gfbenv�̓��t�o�[�W����0x15051320���\�L�Ɏg���o�[�W�����͂��ꁄ

���g
========================================
GFBENV00  �V�O�l�`��(8�o�C�g)
00  gfbenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfbenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
0000  gfbenv�̓��t�o�[�W����(u32, 4�o�C�g)

00  EnvTextureData�̌�o(u16, 2�o�C�g)
00  LightSet�̌�n(u16, 2�o�C�g)
00  Camera�̌�m(u16, 2�o�C�g)
00  �p�f�B���O(2�o�C�g)

DrawEnv�̊J�n�ʒu(���̃t�@�C���̐擪���牽�o�C�g�ڂ�)(u32, 4�o�C�g)
EnvTextureData�̊J�n�ʒu(u32, 4�o�C�g)          ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
LightSet[0]�̊J�n�ʒu(u32, 4�o�C�g)             ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
LightSet[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
LightSet[n-1]�̊J�n�ʒu(u32, 4�o�C�g)
Camera[0]�̊J�n�ʒu(u32, 4�o�C�g)               ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
Camera[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
Camera[m-1]�̊J�n�ʒu(u32, 4�o�C�g)

DrawEnv�̃o�C�g��(u32, 4�o�C�g)
EnvTextureData�̃o�C�g��(u32, 4�o�C�g)          ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
LightSet[0]�̃o�C�g��(u32, 4�o�C�g)             ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
LightSet[1]�̃o�C�g��(u32, 4�o�C�g)
...
LightSet[n-1]�̃o�C�g��(u32, 4�o�C�g)
Camera[0]�̃o�C�g��(u32, 4�o�C�g)               ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
Camera[1]�̃o�C�g��(u32, 4�o�C�g)
...
Camera[m-1]�̃o�C�g��(u32, 4�o�C�g)

DrawEnv�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
EnvTextureData�̃o�C�i���f�[�^                  ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[0]�̃o�C�i���f�[�^                     ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
LightSet[n-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[0]�̃o�C�i���f�[�^                       ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
Camera[m-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
========================================

4�o�C�g�A���C�������g�B
���`�敨������킯�ł͂Ȃ��̂ŁA�A���C�������g�͋C�ɂ��Ȃ��Ă������A4�o�C�g�ɂ��Ă������B��

���g���G���f�B�A�����ǂ����͎g�킹�Ă�����Ă���
R:\home\niji\git_program\gflib2\RenderingEngine\include\SceneGraph\Resource\gfl2_GfEnvData.h
����Ȃ̂ŁA���g���G���f�B�A���B
�������I�ɂ̓G���f�B�A���������ߍ��݁A�Ή��ł���悤�ɂ��邩���B
�@�������A���̏ꍇ�͎g�킹�Ă�����Ă���gfl2_GfEnvData.h�̃o�C�i�����R�[�h�̃G���f�B�A�����C�ɂ��Ȃ���΂Ȃ�Ȃ����A
�@���̃f�[�^�����߂���Ƃ�����e�G���f�B�A���ɑΉ����Ȃ���΂Ȃ�Ȃ��Ȃ�B��

EnvTextureData������Ƃ��́AWin32�ł�CTR�łŖ��ߍ��ރe�N�X�`�����قȂ�Ǝv����B
���̑Ή��́A�g�킹�Ă�����Ă���gfl2_GfEnvData.h���悾���A
�{�o�[�W�����������Ă���2015.05.13���݁Agfl2_GfEnvData.h��CTR�ł�EnvTextureData�����Ă�悤�ɂ͂Ȃ��Ă��Ȃ��B


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
gfbenv�o�[�W����0.1�̐���

gfbenv�̃o�[�W����0.1
gfenv�̃o�[�W����0.8
gfbenv�̓��t�o�[�W����0x15043011���\�L�Ɏg���o�[�W�����͂��ꁄ

���g
========================================
GFBENV00  �V�O�l�`��(8�o�C�g)
00  gfbenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfbenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
0000  gfbenv�̓��t�o�[�W����(u32, 4�o�C�g)

00  EnvTextureData�̌�o(u16, 2�o�C�g)
00  LightSet�̌�n(u16, 2�o�C�g)
00  Camera�̌�m(u16, 2�o�C�g)
00  �p�f�B���O(2�o�C�g)

DrawEnv�̊J�n�ʒu(���̃t�@�C���̐擪���牽�o�C�g�ڂ�)(u32, 4�o�C�g)
EnvTextureData�̊J�n�ʒu(u32, 4�o�C�g)          ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
LightSet[0]�̊J�n�ʒu(u32, 4�o�C�g)             ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
LightSet[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
LightSet[n-1]�̊J�n�ʒu(u32, 4�o�C�g)
Camera[0]�̊J�n�ʒu(u32, 4�o�C�g)               ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
Camera[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
Camera[m-1]�̊J�n�ʒu(u32, 4�o�C�g)

DrawEnv�̃o�C�g��(u32, 4�o�C�g)
EnvTextureData�̃o�C�g��(u32, 4�o�C�g)          ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
LightSet[0]�̃o�C�g��(u32, 4�o�C�g)             ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
LightSet[1]�̃o�C�g��(u32, 4�o�C�g)
...
LightSet[n-1]�̃o�C�g��(u32, 4�o�C�g)
Camera[0]�̃o�C�g��(u32, 4�o�C�g)               ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
Camera[1]�̃o�C�g��(u32, 4�o�C�g)
...
Camera[m-1]�̃o�C�g��(u32, 4�o�C�g)

DrawEnv�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
EnvTextureData�̃o�C�i���f�[�^                  ���yEnvTextureData�̌�o��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[0]�̃o�C�i���f�[�^                     ���yLightSet�̌�n��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
LightSet[n-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[0]�̃o�C�i���f�[�^                       ���yCamera�̌�m��0�̂Ƃ��͂Ȃ��z
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
Camera[m-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
========================================

4�o�C�g�A���C�������g�B
���`�敨������킯�ł͂Ȃ��̂ŁA�A���C�������g�͋C�ɂ��Ȃ��Ă������A4�o�C�g�ɂ��Ă������B��

���g���G���f�B�A�����ǂ����͎g�킹�Ă�����Ă���
R:\home\niji\git_program\gflib2\RenderingEngine\include\SceneGraph\Resource\gfl2_GfEnvData.h
����Ȃ̂ŁA���g���G���f�B�A���B
�������I�ɂ̓G���f�B�A���������ߍ��݁A�Ή��ł���悤�ɂ��邩���B
�@�������A���̏ꍇ�͎g�킹�Ă�����Ă���gfl2_GfEnvData.h�̃o�C�i�����R�[�h�̃G���f�B�A�����C�ɂ��Ȃ���΂Ȃ�Ȃ����A
�@���̃f�[�^�����߂���Ƃ�����e�G���f�B�A���ɑΉ����Ȃ���΂Ȃ�Ȃ��Ȃ�B��

EnvTextureData������Ƃ��́AWin32�ł�CTR�łŖ��ߍ��ރe�N�X�`�����قȂ�Ǝv����B
���̑Ή��́A�g�킹�Ă�����Ă���gfl2_GfEnvData.h���悾���A
�{�o�[�W�����������Ă���2015.04.30���݁Agfl2_GfEnvData.h��CTR�ł�EnvTextureData�����Ă�悤�ɂ͂Ȃ��Ă��Ȃ��B


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
gfbenv�o�[�W����0.0�̐���

gfbenv�̃o�[�W����0.0
gfenv�̃o�[�W����0.8
gfbenv�̓��t�o�[�W����0x15041520���\�L�Ɏg���o�[�W�����͂��ꁄ

���g
========================================
GFBENV00  �V�O�l�`��(8�o�C�g)
00  gfbenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfbenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃��W���[�o�[�W����(u16, 2�o�C�g)
00  gfenv�̃}�C�i�[�o�[�W����(u16, 2�o�C�g)
0000  gfbenv�̓��t�o�[�W����(u32, 4�o�C�g)

00  LightSet�̌�n(u16, 2�o�C�g)
00  Camera�̌�m(u16, 2�o�C�g)

DrawEnv�̊J�n�ʒu(���̃t�@�C���̐擪���牽�o�C�g�ڂ�)(u32, 4�o�C�g)
EnvTextureData�̊J�n�ʒu(u32, 4�o�C�g)
LightSet[0]�̊J�n�ʒu(u32, 4�o�C�g)
LightSet[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
LightSet[n-1]�̊J�n�ʒu(u32, 4�o�C�g)
Camera[0]�̊J�n�ʒu(u32, 4�o�C�g)
Camera[1]�̊J�n�ʒu(u32, 4�o�C�g)
...
Camera[m-1]�̊J�n�ʒu(u32, 4�o�C�g)

DrawEnv�̃o�C�g��(u32, 4�o�C�g)
EnvTextureData�̃o�C�g��(u32, 4�o�C�g)
LightSet[0]�̃o�C�g��(u32, 4�o�C�g)
LightSet[1]�̃o�C�g��(u32, 4�o�C�g)
...
LightSet[n-1]�̃o�C�g��(u32, 4�o�C�g)
Camera[0]�̃o�C�g��(u32, 4�o�C�g)
Camera[1]�̃o�C�g��(u32, 4�o�C�g)
...
Camera[m-1]�̃o�C�g��(u32, 4�o�C�g)

DrawEnv�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
EnvTextureData�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[0]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
LightSet[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
LightSet[n-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[0]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
Camera[1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
...
Camera[m-1]�̃o�C�i���f�[�^
�y4�o�C�g�̔{���ɂȂ��Ă��Ȃ�������p�f�B���O�z
========================================

4�o�C�g�A���C�������g�B
���`�敨������킯�ł͂Ȃ��̂ŁA�A���C�������g�͋C�ɂ��Ȃ��Ă������A4�o�C�g�ɂ��Ă������B��

���g���G���f�B�A�����ǂ����͎g�킹�Ă�����Ă���
R:\home\niji\git_program\gflib2\RenderingEngine\include\SceneGraph\Resource\gfl2_GfEnvData.h
����Ȃ̂ŁA���g���G���f�B�A���B
�������I�ɂ̓G���f�B�A���������ߍ��݁A�Ή��ł���悤�ɂ��邩���B
�@�������A���̏ꍇ�͎g�킹�Ă�����Ă���gfl2_GfEnvData.h�̃o�C�i�����R�[�h�̃G���f�B�A�����C�ɂ��Ȃ���΂Ȃ�Ȃ����A
�@���̃f�[�^�����߂���Ƃ�����e�G���f�B�A���ɑΉ����Ȃ���΂Ȃ�Ȃ��Ȃ�B��


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EnvConverter�\�����[�V�����̂����


1 �R�s�[
R:\home\niji\git_program\gflib2\Animation\Sample
���R�s�[���ASample.sln���J���B
�����I�񂾗��R�́Agfenv��ǂݍ���ł���Sample/DrawEnvAnimation�v���W�F�N�g�����邩��B
���Ȃ݂ɁASample.sln��Animation.bat�ŊJ���B


2 �폜
Visual Studio 2012����
Sample.sln��EnvConverter.sln�Ƀ��l�[���B

gflib2�\�����[�V�����t�H���_�ȊO�̑S�Ẵv���W�F�N�g�A�\�����[�V�����t�H���_���폜�B

R:\home\niji\git_program\gflib2\Animation\Sample���R�s�[���Ă������t�H���_����
���ۂ̃v���W�F�N�g�t�H���_���s�v�ɂȂ������͍̂폜�B


3 �������̒ǉ�
gflib2�\�����[�V�����t�H���_��
gflib2�̕K�v�ȃv���W�F�N�g��ǉ��B


4 �V�K�쐬 
EvnConverter�\�����[�V�����t�H���_������A������
Win32�R���\�[���A�v���P�[�V����
�Ƃ���EnvConverter�v���W�F�N�g������B
���ʂȃe���v���[�g�͎g���Ă��Ȃ��̂ŁAVisual Studio�W���̃v���W�F�N�g�ł��B


5 �K�v�Ȑݒ������
EnvConverter�v���W�F�N�g��gflib2�p�̃v���p�e�B�V�[�gprops��ǉ��B
����ŃC���N���[�h�p�X���ʂ�悤�ɂȂ�B

EnvConverter�v���W�F�N�g�̃v���p�e�B�ŕ����Z�b�g���}���`�o�C�g�����Z�b�g�ɕύX(Debug/Release�Ƃ���)�B
�����main�֐��̈������}���`�o�C�g�����ɂȂ�B

EnvConverter�v���W�F�N�g�̃v���p�e�B�ŉ��L�̂��̂���ς���Ă��܂��Ă���Ƃ����
���L�̒ʂ�ɂ��Ă���(Debug/Release�Ƃ���)�Bgflib2�p�̃v���p�e�B�V�[�g��
�n���Ă���̂ŕς���Ă���Ƃ��낪����B

�o�̓f�B���N�g��
$(SolutionDir)$(Configuration)\
���ԃf�B���N�g��
$(Configuration)\
�^�[�Q�b�g��
$(ProjectName)
�^�[�Q�b�g�̊g���q
.exe
�N���[�����ɍ폜����g���q
*.cdf;*.cache;*.obj;*.ilk;*.resources;*.tlb;*.tli;*.tlh;*.tmp;*.rsp;*.pgc;*.pgd;*.meta;*.tlog;*.manifest;*.res;*.pch;*.exp;*.idb;*.rep;*.xdc;*.pdb;*_manifest.rc;*.bsc;*.sbr;*.xml;*.metagen;*.bi
�r���h���O�t�@�C��
$(IntDir)\$(MSBuildProjectName).log

EnvConverter�v���W�F�N�g�̃v���p�e�B��gflib2�̃v���W�F�N�g���Q�Ƃɒǉ��B


6 ���s
EnvConverter�v���W�F�N�g���X�^�[�g�A�b�v�v���W�F�N�g�ɂ���B
Win32|Debug��Win32|Release�����g���Ă��܂���B


////////////////////////////////////////////////////////////////////////////////

�ȏ�B
