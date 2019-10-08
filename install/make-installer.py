from __future__ import print_function
import os, re, sys, datetime, subprocess

def get_version(fn_hpp):
    _re_version = re.compile('^#define\s+CDD_VERSION\s+"([a-zA-Z0-9_.]+)"')
    for line in open(fn_hpp).readlines():
        m = _re_version.match(line)
        if m: return m.group(1)
    return None

version = get_version('../cdd/cdd_version.h')
date = datetime.datetime.now().strftime('%Y%m%d')
fn_exe = 'CddInstaller_%(version)s_%(date)s.exe' % locals()
print('-- Creating:', fn_exe)
if os.path.exists(fn_exe):
    print('-- Removing old:', fn_exe)
    os.remove(fn_exe)

fp_nsh = open('CdDeluxe.nsh', 'w')
# print >>fp_nsh, '!define CDD_EXE "%s"' % fn_exe
# print >>fp_nsh, '!define CDD_VER "%s"' % version
print('!define CDD_EXE "%s"' % fn_exe, file=fp_nsh)
print('!define CDD_VER "%s"' % version, file=fp_nsh)
fp_nsh.close()

cmd = 'D:/App/NSIS/makensis.exe cdd_installer.nsi'
rc = subprocess.call(cmd)
if rc:
    print('** ERROR running:', cmd)
else:
    subprocess.call(fn_exe)
