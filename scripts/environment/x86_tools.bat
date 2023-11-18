@echo off

@REM watcom variables
set path=%cd%\tools\watcom\BINW;%path%
set path=%cd%\tools\watcom\BINNT;%path%
set WATCOM=%cd%\tools\watcom
set EDPATH=%cd%\tools\watcom\EDDAT
set WHTMLHELP=%cd%\tools\watcom\BINNT\HELP
set WIPFC=%cd%\tools\watcom\WIPFC

@REM adding tools to path
set path=%cd%\tools\nasm;%path%
set path=%cd%\tools\qemu;%path%
set path=%cd%\tools\bochs;%path%