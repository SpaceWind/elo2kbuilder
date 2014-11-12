xcopy sprites\*.* release\sprites\*.* /E /Y /C /H 
xcopy userdata\*.* release\userdata\*.* /E /Y /C /H 
xcopy maps\*.* release\maps\*.* /E /Y /C /H 
xcopy scenes\*.* release\scenes\*.* /E /Y /C /H 
xcopy sounds\*.* release\sounds\*.* /E /Y /C /H
xcopy config\*.* release\config\*.* /E /Y /C /H  
xcopy saves\*.* release\saves\*.* /E /Y /C /H  

copy elo2000.exe release\elo2000dc.exe

del release\elo2000.exe
upx -9 -orelease\elo2000.exe release\elo2000dc.exe
del release\elo2000dc.exe
del release\elo2000.rar

rar a -r release\elo2000.rar release\