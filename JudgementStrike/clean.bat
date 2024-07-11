@echo off

rmdir /s /q .vs
rmdir /s /q Debug
rmdir /s /q Release
rmdir /s /q Game\Release
rmdir /s /q Game\Debug
rmdir /s /q libogg\win32\VS2015\Win32
rmdir /s /q libvorbis\win32\VS2010\libvorbis\Win32
rmdir /s /q libvorbis\win32\VS2010\libvorbisfile\Win32
rmdir /s /q Game\libs\ogg\Win32
rmdir /s /q libogg\win32\VS2015\Win32

@pause