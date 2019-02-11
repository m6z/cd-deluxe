# update the version

import os, sys, datetime

cdd_version = sys.argv[1]

dct_format = {
    'cdd_version': cdd_version,
    'cdd_version_commas': cdd_version.replace('.', ','),
    'date': datetime.datetime.now().strftime('%Y%m%d'),
}

lst_file = [
    ( 'cdd/cdd_version.h.template',    'utf-8'     ),
    ( 'main/main_vs2015.rc.template',  'utf-16-le' ),
    ( 'install/CdDeluxe.nsh.template', 'utf-8'     ),
]

for file_in, encoding in lst_file:
    file_out = os.path.splitext(file_in)[0]
    print('file_in:', file_in)
    print('file_out:', file_out)
    with open(file_in, encoding=encoding) as fp_in:
        text = fp_in.read()
    text = text.format(**dct_format)
    with open(file_out, 'w', encoding=encoding) as fp_out:
        fp_out.write(text)

