; Copyright 2010-2019 Michael Graz
; http://www.plan10.com/cdd
; 
; This file is part of Cd Deluxe.
; 
; Cd Deluxe is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; Cd Deluxe is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with Cd Deluxe.  If not, see <http://www.gnu.org/licenses/>.

!include MUI2.nsh
!include nsDialogs.nsh
; !include EnvVarUpdate.nsh
!include CdDeluxe.nsh  ; dynamically generated

;----------------------------------------------------------------------
; Attributes

Name "Cd Deluxe"

; The file to write
OutFile ${CDD_EXE}

InstallDir "$PROGRAMFILES\Cd Deluxe"

ShowInstDetails show

!define UNINST_ROOT "HKLM"
!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\CdDeluxe"

;--------------------------------
; Language

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Reserve Files

;--------------------------------
; Variables

Var AddToPath
Var hwndAddToPath

Var CreateStartMenuShortcuts
Var hwndCreateStartMenuShortcuts

Var CreateDesktopShortcut
Var hwndCreateDesktopShortcut

Var RunCddShell
Var hwndRunCddShell

;----------------------------------------------------------------------
; Funcions

Function .onInit
    IntOp $AddToPath 0 + 1
    IntOp $CreateStartMenuShortcuts 0 + 1
    IntOp $CreateDesktopShortcut 0 + 1
    IntOp $RunCddShell 0 + 1
FunctionEnd

;----------------------------------------------------------------------
; Pages

!insertmacro MUI_PAGE_DIRECTORY
Page custom custom_settings custom_settings_end
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;----------------------------------------------------------------------
; Sections

Function custom_settings
  !insertmacro MUI_HEADER_TEXT "Customize Cd Deluxe installation" "Choose options for installation and configuration"
  nsDialogs::Create /NOUNLOAD 1018
  Pop $0

  ${NSD_CreateCheckBox} 15u 10u 100% 15u "Add to PATH"
  Pop $hwndAddToPath
  SendMessage $hwndAddToPath ${BM_SETCHECK} $AddToPath 0

  ${NSD_CreateCheckBox} 15u 30u 100% 15u "Create Start Menu Shortcuts"
  Pop $hwndCreateStartMenuShortcuts
  SendMessage $hwndCreateStartMenuShortcuts ${BM_SETCHECK} $CreateStartMenuShortcuts 0

  ${NSD_CreateCheckBox} 15u 50u 100% 15u "Create Desktop Shortcut"
  Pop $hwndCreateDesktopShortcut
  SendMessage $hwndCreateDesktopShortcut ${BM_SETCHECK} $CreateDesktopShortcut 0

  ${NSD_CreateCheckBox} 15u 70u 100% 15u "Run sample cdd shell when installation is completed"
  Pop $hwndRunCddShell
  SendMessage $hwndRunCddShell ${BM_SETCHECK} $RunCddShell 0

  nsDialogs::Show
FunctionEnd

Function custom_settings_end
  SendMessage $hwndAddToPath ${BM_GETCHECK} 0 0 $AddToPath
  SendMessage $hwndCreateStartMenuShortcuts ${BM_GETCHECK} 0 0 $CreateStartMenuShortcuts
  SendMessage $hwndCreateDesktopShortcut ${BM_GETCHECK} 0 0 $CreateDesktopShortcut
  SendMessage $hwndRunCddShell ${BM_GETCHECK} 0 0 $RunCddShell

  ; MessageBox MB_OK 'xx AddToPath=$AddToPath CreateStartMenuShortcuts=$CreateStartMenuShortcuts CreateDesktopShortcut=$CreateDesktopShortcut RunCddShell=$RunCddShell'
FunctionEnd

; The stuff to install
Section "CddInstaller (required)"
  SectionIn RO
  ; Update the registry

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; These are the files to install
  SetOverwrite on
  File cdd_installer.nsi
  File ..\Release\_cdd.exe
  File cdd.cmd
  File cdd_shell.cmd
  File LICENSE.txt

  CreateShortCut "$INSTDIR\cdd shell.lnk" cmd.exe '/k "$INSTDIR\cdd_shell.cmd"'

  ; These reg settings change the appearance of the cdd shell
  WriteRegDWORD   HKCU   "Console\cdd shell"   "ColorTable01"       0x007d0000
  WriteRegDWORD   HKCU   "Console\cdd shell"   "ColorTable15"       0x00fefefe
  WriteRegDWORD   HKCU   "Console\cdd shell"   "ScreenColors"       0x0000001f
  WriteRegStr     HKCU   "Console\cdd shell"   "FaceName"           "Lucida Console"
  WriteRegDWORD   HKCU   "Console\cdd shell"   "FontFamily"         0x00000036
  WriteRegDWORD   HKCU   "Console\cdd shell"   "FontSize"           0x000e0008
  WriteRegDWORD   HKCU   "Console\cdd shell"   "FontWeight"         0x00000190
  WriteRegDWORD   HKCU   "Console\cdd shell"   "ScreenBufferSize"   0x07d00064
  WriteRegDWORD   HKCU   "Console\cdd shell"   "WindowSize"         0x001e0064
  WriteRegDWORD   HKCU   "Console\cdd shell"   "QuickEdit"          0x00000800
  WriteRegDWORD   HKCU   "Console\cdd shell"   "HistoryNoDup"       0x00100000

  ; Add to path
  IntCmp $AddToPath 0 add_to_path_end
  ; MessageBox MB_OK "xx adding to path"
  ; ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"
  EnVar::AddValue "PATH" "$INSTDIR"
  add_to_path_end:

  ; Start menu
  IntCmp $CreateStartMenuShortcuts 0 start_menu_end
  ; MessageBox MB_OK "xx creating start menu shortcuts"
  CreateDirectory "$SMPROGRAMS\Cd Deluxe"
  CreateShortCut "$SMPROGRAMS\Cd Deluxe\cdd shell.lnk" cmd.exe '/k "$INSTDIR\cdd_shell.cmd"'
  CreateShortCut "$SMPROGRAMS\Cd Deluxe\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  start_menu_end:

  ; Desktop shortcut
  IntCmp $CreateDesktopShortcut 0 desktop_end
  ; MessageBox MB_OK "xx creating desktop shortcut"
  CreateShortCut "$DESKTOP\cdd shell.lnk" cmd.exe '/k "$INSTDIR\cdd_shell.cmd"'
  desktop_end:

  ; Uninstall stuff
  WriteUninstaller $INSTDIR\Uninstall.exe

  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "DisplayName" "Cd Deluxe"
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "InstallLocation " '"$INSTDIR"'
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "Publisher" "http://www.plan10.com/cdd"
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "NoModify " 1
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "NoRepair" 1
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "DisplayVersion" "${CDD_VER}"
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "HelpLink " "http://www.plan10.com/cdd"
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "URLUpdateInfo " "http://www.plan10.com/cdd"
  WriteRegStr "${UNINST_ROOT}" "${UNINST_KEY}" "URLInfoAbout " "http://www.plan10.com/cdd"

  ; Sample shell
  IntCmp $RunCddShell 0 cdd_shell_end
  ExecShell "" "$INSTDIR\cdd shell.lnk"
  cdd_shell_end:

  ; MessageBox MB_OK "xx at the end"

SectionEnd

Section "Uninstall"
  ; ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"
  EnVar::DeleteValue "PATH" "$INSTDIR"
  
  Delete $INSTDIR\cdd_installer.nsi
  Delete $INSTDIR\_cdd.exe
  Delete $INSTDIR\cdd.cmd
  Delete $INSTDIR\cdd_shell.cmd
  Delete $INSTDIR\Uninstall.exe
  Delete $INSTDIR\LICENSE.txt
  Delete "$INSTDIR\cdd shell.lnk"
  RMDir $INSTDIR

  ; Start Menu
  Delete "$SMPROGRAMS\Cd Deluxe\cdd shell.lnk"
  Delete "$SMPROGRAMS\Cd Deluxe\Uninstall.lnk"
  RMDir "$SMPROGRAMS\Cd Deluxe"

  ; Desktop
  Delete "$DESKTOP\cdd shell.lnk"

  ; Registry
  DeleteRegKey ${UNINST_ROOT} "${UNINST_KEY}"
  DeleteRegKey HKCU "Console\cdd shell"

SectionEnd

; vim:sw=2 ts=2
