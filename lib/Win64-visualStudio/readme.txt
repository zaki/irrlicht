We don't provide a procompiled dll for Win64.

Visual Studio Express does not come with a 64-bit compiler by default. 
You'll have to download the Windows Platform SDK-

http://msdn.microsoft.com/en-us/windows/bb980924.aspx

When installing the platform SDK, make sure you install the x64 and 
IA64 compilers from Developer Tools -> Visual C++ Compilers.
If you're using VC Express you will only be able to compile from the
command line.