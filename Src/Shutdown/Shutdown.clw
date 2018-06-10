; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CShutdownApp
LastClass=CShutdownApp
NewFileInclude2=#include "Shutdown.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Class2=CShutdownConfigDlg
LastTemplate=CDialog
Resource1=IDD_CONFIGDLG
Class3=CShutdownCountdownDlg
Resource2=IDD_SHUTDOWNCOUNTDOWNDLG

[CLS:CShutdownApp]
Type=0
HeaderFile=shutdownapp.h
ImplementationFile=shutdownapp.cpp
BaseClass=CWinApp
Filter=N

[DLG:IDD_CONFIGDLG]
Type=1
Class=CShutdownConfigDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ENABLE,button,1342242819
Control4=IDC_DATE,SysDateTimePick32,1342242848
Control5=IDC_TIME,SysDateTimePick32,1342242857
Control6=IDC_TURNOFF,button,1342242819

[CLS:CShutdownConfigDlg]
Type=0
HeaderFile=ShutdownConfigDlg.h
ImplementationFile=ShutdownConfigDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CShutdownConfigDlg
VirtualFilter=dWC

[DLG:IDD_SHUTDOWNCOUNTDOWNDLG]
Type=1
Class=CShutdownCountdownDlg
ControlCount=4
Control1=IDCANCEL,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_COUNTDOWN,static,1342308865
Control4=IDOK,button,1342242816

[CLS:CShutdownCountdownDlg]
Type=0
HeaderFile=ShutdownCountdownDlg.h
ImplementationFile=ShutdownCountdownDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CShutdownCountdownDlg

