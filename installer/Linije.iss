; Inno Setup script for Linije.
; Build: ISCC.exe /DAppVersion=1.0.0 /DSourceDir=..\dist\Linije installer\Linije.iss

#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif
#ifndef SourceDir
  #define SourceDir "..\dist\Linije"
#endif

[Setup]
AppId={{481B4723-7629-4F25-A515-87F5F49002CC}
AppName=Linije
AppVersion={#AppVersion}
AppPublisher=Milan Milanovic
AppPublisherURL=https://github.com/milanm/Linije
AppSupportURL=https://github.com/milanm/Linije/issues
; The game writes saves and scores next to the exe, so install per user
; into a writable folder (%LOCALAPPDATA%\Programs\Linije) without admin rights.
PrivilegesRequired=lowest
DefaultDirName={autopf}\Linije
DefaultGroupName=Linije
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\Linije.exe
SetupIconFile=..\res\Linije.ico
OutputBaseFilename=Linije-{#AppVersion}-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Linije"; Filename: "{app}\Linije.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,Linije}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Linije"; Filename: "{app}\Linije.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\Linije.exe"; WorkingDir: "{app}"; Description: "{cm:LaunchProgram,Linije}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\linije.sav"
Type: files; Name: "{app}\linije.rez"
