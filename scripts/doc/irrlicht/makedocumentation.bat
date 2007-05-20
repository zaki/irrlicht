mkdir ..\..\..\doctemp
mkdir ..\..\..\doctemp\html
copy doxygen.css ..\..\..\doctemp\html
copy irrlicht.png ..\..\..\doctemp\html
copy logobig.png ..\..\..\doctemp\html

..\doxygen.exe doxygen.cfg

pause