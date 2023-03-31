#!/bin/bash

name=imglabeler

cc=gcc
src=src/*.c

flags=(
    -Wall
    -Wextra
    -pedantic
    -O2
    -std=c99
)

sub=(
    imgtool
)

inc=(
    -Ispxe
)

lib=(
    -Llib
    -lz
    -lpng
    -ljpeg
    -lglfw
)

for mod in ${sub[*]}
do
    inc+=(-I$mod)
    lib+=(-l$mod)
done

if echo "$OSTYPE" | grep -q "darwin"; then
    lib+=(
        -framework OpenGL
    )
elif echo "$OSTYPE" | grep -q "linux"; then
    lib+=(
        -lGL
        -lGLEW
        -lm
    )
else
    echo "This OS is not supported by this shell script yet..." && exit
fi

cmd() {
    echo "$@" && $@
}

lib_build() {
    cmd pushd $1/ && ./build.sh $2 && cmd mv bin/*.a ../lib/ && cmd popd
}

build() {
    cmd mkdir -p lib/
    for mod in ${sub[*]} 
    do
        lib_build $mod static
    done
}

objs() {
    [ ! -d lib/ ] && build
    cmd mkdir -p tmp/
    cmd $cc -c $src ${flags[*]} ${inc[*]} && cmd mv *.o tmp/
}

comp() {
    objs && cmd $cc tmp/*.o -o $name ${flags[*]} ${lib[*]} ${inc[*]}
}

cleanf() {
    [ -f $1 ] && cmd rm $1
}

cleand() {
    [ -d $1 ] && cmd rm -r $1
}

cleanr() {
    cleand $1/tmp/
    cleand $1/bin/
}

clean() {
    for mod in ${sub[*]}
    do
        cleanr $mod
    done

    cleand lib
    cleand tmp
    cleanf $name
    return 0
}

case "$1" in
    "build")
        build;;
    "comp")
        comp;;
    "clean")
        clean;;
    *)
        echo "Run with 'cli' or 'rt' to compile runtime or client executables"
        echo "Use 'clean' to remove local builds.";;
esac
