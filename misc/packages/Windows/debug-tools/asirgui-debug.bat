rem $OpenXM: OpenXM/misc/packages/Windows/debug-tools/asirgui-debug.bat,v 1.2 2003/07/29 07:38:16 takayama Exp $
rem
rem
set OpenXM_HOME=/cygdrive/c/cygwin/home/%USERNAME%/OpenXM/misc/packages/Windows/OpenXM-win
set OpenXM_HOME_WIN=c:\cygwin\home\%USERNAME%\OpenXM\misc\packages\Windows\OpenXM-win
rem 
rem Do not touch below
rem 
set OpenXM_START_EXE=start
set WIN_ASIR_ROOT=%OpenXM_HOME_WIN%\asir
set OpenXM_WIN_ROOT=%WIN_ASIR_ROOT%
set PATH=%OpenXM_HOME_WIN%\bin;%PATH%
rem ASIR_LIBDIR will be obsolete.
set ASIR_LIBDIR=%OpenXM_HOME_WIN%\lib\asir;%WIN_ASIR_ROOT%\lib
set ASIRLOADPATH=%WIN_ASIR_ROOT%\lib;%OpenXM_HOME_WIN%\src\asir-contrib\packages\src;%OpenXM_HOME_WIN%\src\asir-contrib\packages\sample;%OpenXM_HOME_WIN%\lib\asir-contrib;.
set ASIR_CONFIG=%OpenXM_HOME_WIN%\rc\asirrc
set ASIR_RSH='ssh -f -X -A '
set LOAD_SM1_PATH=%OpenXM_HOME%/lib/sm1
set LOAD_K_PATH=%OpenXM_HOME%/lib/k097
rem
rem  start the asirgui.exe
rem
cd %WIN_ASIR_ROOT%\bin
asirgui


