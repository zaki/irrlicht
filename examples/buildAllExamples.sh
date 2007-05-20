#! /bin/sh
[ -z $1 ] || TARGET=$1
[ -z $TARGET ] && TARGET=all
for i in [01]* Demo; do
  echo "Building $i";
  cd $i;
  make clean $TARGET;
  cd ..;
done
