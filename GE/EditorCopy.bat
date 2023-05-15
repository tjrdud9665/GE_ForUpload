cd..
xcopy .\Editor\GameObject\*.* .\Client\GameObject\ /d /s /y
xcopy .\Editor\Notifies\*.* .\Client\Notifies\ /d /s /y
xcopy .\Editor\Components\*.* .\Client\Components\ /d /s /y
xcopy .\Editor\SceneInfo\*.* .\Client\SceneInfo\ /d /s /y

xcopy .\Editor\AITask\*.* .\Client\AITask\ /d /s /y

xcopy .\Editor\UserWidget\*.* .\Client\UserWidget\ /d /s /y

xcopy .\Editor\Assets\*.* .\Client\Assets\ /d /s /y

copy .\Editor\PCH.h .\Client\PCH.h
copy .\Editor\PCH.cpp .\Client\PCH.cpp