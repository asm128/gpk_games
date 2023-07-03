mkdir ..\..\Win32.Release
mkdir ..\..\Win32.Debug
mkdir ..\..\x64.Release
mkdir ..\..\x64.Debug

copy *.json ..\..\Win32.Release\*.*
copy *.json ..\..\Win32.Debug\*.*
copy *.json ..\..\x64.Release\*.*
copy *.json ..\..\x64.Debug\*.*
