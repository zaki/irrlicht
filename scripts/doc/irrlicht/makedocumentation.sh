rm tut.txt || true;

mkdir ../../../doctemp
mkdir ../../../doctemp/html
cp doxygen.css irrlicht.png logobig.png ../../../doctemp/html

for i in ../../../examples/[012]*/main.cpp; do
  sed -f tutorials.sed $i >>tut.txt;
done

doxygen doxygen.cfg
