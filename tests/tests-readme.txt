
Welcome to the Irrlicht test suite.

This is composed of a series of tests which exercise basic Irrlicht functionality.  These are not
strictly unit tests, since there is no stub framework that isolates each method under test.  They
do however test small units of functionality and should help to isolate problems or spot regressions.

Each test resides in its own source file, and must have an entry point with the signature
bool testName(void); where testName should be the same as the source file name (without the suffix).

Each test runs independently, and is responsible for cleaning up after itself and restoring the
working directory to /tests.

testUtils.cpp provides some functions for creating screenshots and comparing files (including images).

Validation images should go im the /media subdirectory.  Since the tests rely on the presence of /media
and /empty/empty subdirectories, the working directory must be the /tests directory, not /bin/$PLATFORM.
This means that you cannot run /bin/$PLATFORM/texts.exe from there.  You can however cd to /tests and
run ../bin/$PLATFORM/tests.exe

The overall test application will return a count of the number of test that failed, i.e. 0 is success.