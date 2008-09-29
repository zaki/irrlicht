rm tut.txt || true;
for i in ../../../examples/[01]*/main.cpp; do
  sed -f tutorials.sed $i >>tut.txt;
done

doxygen doxygen-pdf.cfg

cp doxygen.css irrlicht.png logobig.png ../../../doctemp/html


