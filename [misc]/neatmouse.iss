; Copyright © 2016 Neat Decisions.
; Copying and distribution of this file, with or without modification,
; are permitted in any medium without royalty provided the copyright
; notice and this notice are preserved.  This file is offered as-is,
; without any warranty.
;
; NeatMouse InnoSetup installation script

#define ApplicationName 'NeatMouse'
#define ApplicationFileNameWithoutExtension 'neatmouse'
#define ApplicationFileName ApplicationFileNameWithoutExtension +'.exe'

#define ExeName '..\NeatMouseWtl\Release\' + ApplicationFileName
#define AppVersionNo GetFileVersion(ExeName)
#define AppMajorVersionIdx Pos(".", AppVersionNo)

#define AppVersionNo1 Copy(AppVersionNo, 1, AppMajorVersionIdx -1)
#define AppMinorVersionTemp Copy(AppVersionNo, AppMajorVersionIdx +1)

#define AppMajorVersionIdx Pos(".", AppMinorVersionTemp)
#define AppVersionNo2 '0' + Copy(AppMinorVersionTemp, 1, AppMajorVersionIdx-1)
#define AppVersionNo2 Copy(AppVersionNo2, Len(AppVersionNo2) - 1, 2)                                                                   

#define ApplicationVersion AppVersionNo1 + '.' + AppVersionNo2



[Setup]
AppName={#ApplicationName}
AppVerName={#ApplicationName} {#ApplicationVersion}
VersionInfoVersion={#ApplicationVersion}
DefaultDirName= {code:GetDefaultInstallPath}
DefaultGroupName={#ApplicationName}
UninstallDisplayIcon={app}\{#ApplicationFileName}
MinVersion=5.1
OutputBaseFilename={#ApplicationFileNameWithoutExtension}-{#ApplicationVersion}-setup
PrivilegesRequired=lowest
AppPublisher=Neat Decisions
AppPublisherURL=http://www.neatdecisions.com/
Uninstallable=not IsComponentSelected('portable')
AlwaysShowComponentsList=no
LicenseFile=..\LICENSE
DisableReadyMemo=yes
AllowNoIcons=yes
DisableDirPage=no


[Types]
Name: "normal"; Description: "Normal installation"
Name: "portable"; Description: "Portable installation"

[Components]
Name: "normal"; Description: "Normal installation"; Types: normal; Flags: exclusive
Name: "portable"; Description: "Portable installation"; Types: portable; Flags: exclusive

[Files]
Source: {#ExeName}; DestDir: "{app}"; Flags: ignoreversion
Source: "nonportable"; DestDir: "{app}"; Flags: ignoreversion; Components: normal

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: normal
Name: desktopicon\common; Description: "For all users"; GroupDescription: "Additional icons:"; Flags: exclusive; Components: normal
Name: desktopicon\user; Description: "For the current user only"; GroupDescription: "Additional icons:"; Flags: exclusive unchecked; Components: normal


[Icons]
Name: "{group}\{#ApplicationName}"; Filename: "{app}\{#ApplicationFileName}"; Components: normal
Name: "{group}\Uninstall {#ApplicationName}"; Filename: "{uninstallexe}"; Components: normal
Name: "{commondesktop}\{#ApplicationName}"; Filename: "{app}\{#ApplicationFileName}"; Tasks: desktopicon\common; Components: normal
Name: "{userdesktop}\{#ApplicationName}"; Filename: "{app}\{#ApplicationFileName}"; Tasks: desktopicon\user; Components: normal

[Registry]                                                                                          
Root: HKCU; Subkey: "Software\NeatDecisions"; Flags: uninsdeletekeyifempty; Components: normal
Root: HKCU; Subkey: "Software\NeatDecisions\{#ApplicationName}"; Flags: uninsdeletekey; Components: normal
Root: HKLM; Subkey: "Software\NeatDecisions"; Flags: uninsdeletekeyifempty; Components: normal
Root: HKLM; Subkey: "Software\NeatDecisions\{#ApplicationName}"; Flags: uninsdeletekey; Components: normal
Root: HKLM; Subkey: "Software\NeatDecisions\{#ApplicationName}\Settings"; ValueType: string; ValueName: "Path"; ValueData: "{app}"; Components: normal

[Run]
Filename: "{app}\{#ApplicationFileName}"; Description: "Run {#ApplicationName} {#ApplicationVersion}"; Flags: postinstall nowait skipifsilent unchecked

[Code]
#ifdef UNICODE
  #define AW "W"
#else
  #define AW "A"
#endif
type
  HINSTANCE = THandle;

var
  InstallationTypePage: TInputOptionWizardPage;
  Elevated: Boolean;
  PagesSkipped: Boolean;

procedure ExitProcess(uExitCode: UINT);
  external 'ExitProcess@kernel32.dll stdcall';
function ShellExecute(hwnd: HWND; lpOperation: string; lpFile: string;
  lpParameters: string; lpDirectory: string; nShowCmd: Integer): HINSTANCE;
  external 'ShellExecute{#AW}@shell32.dll stdcall';

// skip 
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if PageID = wpSelectComponents then
  begin
    Result := True;
  end else
  begin
    if not PagesSkipped then
    begin
      Result := Elevated and (PageID <> wpLicense);
      if not Result then PagesSkipped := True;
    end else
    begin
      Result := (PageID = wpSelectProgramGroup) and IsComponentSelected('portable');
    end;
  end;
end;


function CmdLineParamExists(const Value: string): Boolean;
var
  I: Integer;  
begin
  Result := False;
  for I := 1 to ParamCount do
    if CompareText(ParamStr(I), Value) = 0 then
    begin
      Result := True;
      Break;
    end;
end;


function GetDefaultInstallPath(Param: String): String;
begin
  Result := ExpandConstant('{pf}') + '\' + '{#ApplicationName}';
end;


function NextButtonClick(CurPageID: Integer): Boolean;
var
  Params: string;
  RetVal: HINSTANCE;
begin
  Result := True;
  if CurPageID = InstallationTypePage.ID then
  begin
    if InstallationTypePage.SelectedValueIndex = 1 then
    begin
      WizardForm.DirEdit.Text := ExpandConstant('{userdesktop}') + '\{#ApplicationName}';
      WizardForm.ComponentsList.Checked[1] := True;
    end else
    begin
      if not Elevated then
      begin
        Params := ExpandConstant('/ELEVATE /COMPONENTS=normal');
        RetVal := ShellExecute(WizardForm.Handle, 'runas', ExpandConstant('{srcexe}'), Params, '', SW_SHOW);
        if RetVal > 32 then
        begin
          ExitProcess(0);
        end
        else
        begin
          Result := False;
          if RetVal = 3 then
            MsgBox('Could not obtain write permission for the installation. Please make sure that the setup file is not executed from the network drive.', mbError, MB_OK)
          else
            MsgBox(Format('Could not obtain write permission for the installation. Error code: %d', [RetVal]), mbError, MB_OK);
        end;
      end else
      begin
        WizardForm.DirEdit.Text := GetDefaultInstallPath('');
        WizardForm.ComponentsList.Checked[0] := True;  
      end;
    end;
  end;
end;

                 
procedure InitializeWizard;
begin
  if  GetWindowsVersion < $06000000 then
  begin
    Elevated := True;
    PagesSkipped := True; 
  end else
  begin
    Elevated := CmdLineParamExists('/ELEVATE');
    PagesSkipped := False;
  end;


  { Create the pages }
  
  InstallationTypePage := CreateInputOptionPage(wpWelcome,
    'Installation mode', 
    'Please select the type of installation you wish to peform.',
    'Most users should choose "Normal Installation". However, if you need to install NeatMouse to be portable, please select that option.',
    True, False);
  InstallationTypePage.Add('Normal installation (recommended)');
  InstallationTypePage.Add('Portable installation');
  InstallationTypePage.SelectedValueIndex := 0;
end;
