#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper

    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    echo "GOT HERE 2"

    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    echo "GOT HERE 3"

   # make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
   # echo "Done building modules"

    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs

fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir rootfs
cd rootfs
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log 

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH}CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
export SYSROOT=$(aarch64-none-linux-gnu-gcc -print-sysroot)

cd ${OUTDIR}/rootfs

cp -a  $SYSROOT/lib/ld-linux-aarch64.so.1 lib64
cp -a $SYSROOT/lib/ld-linux-aarch64.so.1 lib
echo "Dep 0"

cp -a $SYSROOT/lib64/ld-2.31.so lib64
cp -a $SYSROOT/lib64/ld-2.31.so lib
echo "Dep 1"

cp -a $SYSROOT/lib64/libc.so.6 lib64
cp -a $SYSROOT/lib64/libc.so.6 lib

cp -a $SYSROOT/lib64/libc-2.31.so lib64
cp -a $SYSROOT/lib64/libc-2.31.so lib

cp -a $SYSROOT/lib64/libm.so.6 lib64
cp -a $SYSROOT/lib64/libm.so.6 lib

cp -a $SYSROOT/lib64/libm-2.31.so lib64
cp -a $SYSROOT/lib64/libm-2.31.so lib

cp -a $SYSROOT/lib64/libresolv.so.2 lib64
cp -a $SYSROOT/lib64/libresolv.so.2 lib

cp -a $SYSROOT/lib64/libresolv-2.31.so lib64
cp -a $SYSROOT/lib64/libresolv-2.31.so lib
echo "Dep 7"

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
echo "Node 1"

sudo mknod -m 600 dev/console c 5 1
echo "Node 2"

# TODO: Clean and build the writer utility
cp /usr/local/share/assignment-2-Dmitrii-hubbba/writer ${OUTDIR}/rootfs/home
cp /usr/local/share/assignment-2-Dmitrii-hubbba/finder-app/finder.sh ${OUTDIR}/rootfs/home
echo "Copied writer utility"

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp /usr/local/share/assignments-3-and-later-Dmitrii-hubbba/finder-app/*.sh ${OUTDIR}/rootfs/home
cp /usr/local/share/assignments-3-and-later-Dmitrii-hubbba/finder-app/conf/*.txt ${OUTDIR}/rootfs/home
# TODO: Chown the root directory


# TODO: Create initramfs.cpio.gz
cd ${OUTDIR}/rootfs
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio
echo "Made gzip"
