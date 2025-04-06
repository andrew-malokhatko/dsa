@echo off
rem Simple loop from 1 to 1000

for /L %%i in (1,1,1000) do (
    echo Iteration number %%i
	python testgenerator.py
	StuTests.exe
    if errorlevel 1 (
        echo StuTests.exe failed on iteration %%i, stopping batch.
        exit /b 1
    )
	rem pause
)
