import os, re, sys, datetime, subprocess

def get_version(fn_hpp):
    _re_version = re.compile('^#define\s+CDD_VERSION\s+"([a-zA-Z0-9_.]+)"')
    for line in file(fn_hpp).readlines():
        m = _re_version.match(line)
        if m: return m.group(1)
    return None

version = get_version('../cdd/cdd.h')
date = datetime.datetime.now().strftime('%Y%m%d')
fn_exe = 'CddInstaller_%(version)s_%(date)s.exe' % locals()
print '-- Creating:', fn_exe
if os.path.exists(fn_exe):
    print '-- Removing old:', fn_exe
    os.remove(fn_exe)

fp_nsh = file('CdDeluxe.nsh', 'w')
print >>fp_nsh, '!define CDD_EXE "%s"' % fn_exe
print >>fp_nsh, '!define CDD_VER "%s"' % version
fp_nsh.close()

cmd = 'D:/App/NSIS/makensis.exe cdd_installer.nsi'
rc = subprocess.call(cmd)
if rc:
    print '** ERROR running:', cmd
else:
    subprocess.call(fn_exe)
