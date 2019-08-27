@echo off
:: Minimal build script

SET INCLUDE=%INCLUDE%;%WDDK_INC_PATH%
SET LIB=%LIB%;%WDDK_LIB_PATH%

%PYTHON%\python.exe -m pip wheel . -w wheelhouse
