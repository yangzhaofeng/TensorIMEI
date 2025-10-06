MODDIR=${0%/*}

mkdir -p /data/adb/TensorIMEI
cd /data/adb/TensorIMEI
[ -f devinfo.log ] && mv devinfo.log devinfo.log.old
[ -f devinfo.err ] && mv devinfo.err devinfo.err.old
set -a
[ -f env ] && source ./env
set +a
#{
#    echo "Running post-fs-data.sh"
#    echo "listing /dev/block"
#    ls -lah /dev/block
#    echo "listing /dev/block/by-name"
#    ls -lah /dev/block/by-name
#} > devinfo.log

{
    imgfile=""
    chmod 700 "$MODDIR/chgdevinfo"
    if [ -n "$IMGOVERRIDE" ]; then
        echo "using overrided $IMGOVERRIDE"
        ls -lah "${IMGOVERRIDE}"
        imgfile="${IMGOVERRIDE}"
    else
        echo "dumping original devinfo"
        ls -lah /dev/block/by-name/devinfo
        dd if=/dev/block/by-name/devinfo of=devinfo.img
        imgfile="devinfo.img"
        echo "changing IMEI if needed"
	$MODDIR/chgdevinfo
        #echo "not touching"
    fi
    newlo="$(losetup -f)"
    echo "New loopback device $newlo"
    losetup "$newlo" "${imgfile}"
    echo "Trying to replace symbolic link"
    ln -sfn "$newlo" /dev/block/by-name/devinfo
    ls -lah /dev/block/by-name/devinfo
} > devinfo.log 2>devinfo.err


