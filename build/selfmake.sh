#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo -e "BUILDING \e[1;32mselfmake\e[0m"
    echo
    CC=gcc
    LIBS=-ldl -lX11 -lX11-xcb -lXcursor -lxkbcommon -lxkbcommon-x11
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "BUILDING selfmake"
    CC="clang -D_POSIX_C_SOURCE -D_C99_SOURCE"
    LIBS=""
fi

DIR="$(pwd)"

cp $DIR/src/lib/Common/Types.h $DIR/include/selfmake/Types.h
cp $DIR/src/lib/Common/Result.h $DIR/include/selfmake/Result.h
cp $DIR/src/lib/Common/Functions.h $DIR/include/selfmake/selfmake.h

$CC -std=gnu11 -shared -o$DIR/lib/libselfmake.so -lpthread $LIBS -fPIC \
$DIR/src/lib/Common/Result.c \
$DIR/src/lib/Core/Runtime.c \
$DIR/src/lib/Core/Utils.c \
$DIR/src/lib/Core/Thread.c \
$DIR/src/lib/Core/File.c \
$DIR/src/lib/Core/Configure.c \
$DIR/src/lib/Core/Build.c \
$DIR/src/lib/Core/Options.c \
$DIR/src/lib/Parser/Tokenizer.c \
$DIR/src/lib/Parser/Parser.c \
$DIR/src/lib/Parser/Variables.c \
$DIR/src/lib/Parser/Sources.c \
$DIR/src/lib/Parser/SourceContexts.c \
$DIR/src/lib/Parser/Functions.c \
$DIR/src/lib/Test/Channel.c \
$DIR/src/lib/Test/Process.c \
$DIR/src/lib/Test/Library.c \
$DIR/src/lib/Test/Test.c \
$DIR/src/lib/UI/Message.c \

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    $CC -std=gnu11 -no-pie -Wl,-rpath=$DIR/lib/ -o$DIR/build/smmake -L$DIR/lib/ -lselfmake \
    $DIR/src/bin/smmake/Main.c \
    $DIR/src/bin/smmake/Version.c \
    $DIR/src/bin/smmake/Documents.c \

    $DIR/build/smmake -b ALL 
elif [[ "$OSTYPE" == "darwin"* ]]; then
    $CC -std=gnu11 -no-pie -Wl -o$DIR/build/smmake -L$DIR/lib/ -lselfmake \
    $DIR/src/bin/smmake/Main.c \
    $DIR/src/bin/smmake/Version.c \
    $DIR/src/bin/smmake/Documents.c \

    $DIR/build/smmake -b ALL 
fi


if [ $? = 0 ]; then
    echo
    echo -e "BUILD $DIR/lib/\e[1;32mselfmake\e[0m"
else
    echo
    echo -e "BUILD \e[1;31mFAILURE\e[0m"
fi

