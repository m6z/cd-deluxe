cdd_exe=/home/mike/base/development/cd-deluxe/Debug/main/_cdd
if [[ -x $cdd_exe ]]
then
    function cdd { while read x; do eval $x >/dev/null; done < <(dirs -l -p | "${cdd_exe}" "$@"); }
    alias cd=cdd
fi
