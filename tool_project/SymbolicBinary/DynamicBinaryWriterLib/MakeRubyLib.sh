####バイナリターゲット別処理

### cygwin
RUBY_EXE=/usr/bin/ruby.exe
CC="gcc"
CXX="g++"
STD_LIB="-lstdc++"

### i386-mingw32
#RUBY_EXE=/r/home/niji/niji_extlib/common/Ruby187/bin/ruby.exe
#CC="gcc-3 -mno-cygwin"
#CXX="g++-3 -mno-cygwin"
#STD_LIB="-lstdc++"

### x64-mingw32
#RUBY_EXE=/r/home/niji/niji_extlib/common/Ruby/bin/ruby.exe
#CC="x86_64-w64-mingw32-gcc"
#CXX="x86_64-w64-mingw32-g++"
#STD_LIB="-static-libgcc -static-libstdc++"


## ライブラリパスを追加しながらMakefileの生成
${RUBY_EXE} -r'mkmf' -e"\
	\$LDFLAGS += \" ${STD_LIB}\";\
	\$LDFLAGS += \" -L\$(prefix)/bin\";\
	create_makefile('${1}', '${2}')"

## コンパイラの置換
sed -i -e"s/gcc -shared/\$(CC) -shared/" Makefile
sed -i -e"/^CC/c\CC = ${CC}" Makefile
sed -i -e"/^CC/i CXX = ${CXX}" Makefile

## .oを依存関係に追加
sed -i -e"/^.SUFFIXES/i OBJS += ${3}" Makefile

## 以前のゴミを消す
rm -f *.o

## コンパイル
make
