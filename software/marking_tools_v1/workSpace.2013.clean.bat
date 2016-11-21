::http://www.codeproject.com/Articles/10855/Clean-Visual-Studio-Workspaces

del /s /F        msvc.2013\*.pdb   
del /s /F        msvc.2013\*.sdf  
for /d /r  %%a in (msvc.2013\*) do rd /s /q "%%a" 
for /d /r  %%a in (msvc.2013\*) do rd /s /q "%%a" 
for /d /r  %%a in (msvc.2013\*) do rd /s /q "%%a" 
for /d /r  %%a in (msvc.2013\*) do rd /s /q "%%a" 

pause