@echo off
:: Minimal build script

SET INCLUDE=%WDDK_INC_PATH%;%INCLUDE%
SET LIB=%WDDK_LIB_PATH%;%LIB%

%PYTHON%\python.exe -m pip wheel .
