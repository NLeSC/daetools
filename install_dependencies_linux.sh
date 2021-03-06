#!/bin/bash

set -e

usage()
{
cat << EOF
usage: $0 [options]

This script detects the system and installs packages required to compile 
daetools libraries and python extension modules. Currently supported:
    - Debian GNU/Linux
    - Ubuntu
    - Fedora
    - CentOS
    - Linux Mint
    - Suse Linux
    - Arch Linux

NOTE: If the script fails check whether 'lsb_release' package is installed
      and whether your GNU/Linux distribution is recognized.

Typical use:
    sh install_dependencies_linux.sh

OPTIONS:
   -h | --help  Show this message.
EOF
}

args=`getopt -a -o "h" -l "help" -n "install_dependencies_linux" -- $*`

# Process options
for i; do
  case "$i" in
    -h|--help)  usage
                exit 1
                ;;
                  
  esac
done

HOST_ARCH=`uname -m`
PLATFORM=`uname -s | tr "[:upper:]" "[:lower:]"`
DISTRIBUTOR_ID=`echo $(lsb_release -si) | tr "[:upper:]" "[:lower:]"`
CODENAME=`echo $(lsb_release -sc) | tr "[:upper:]" "[:lower:]"`

# Check the dependencies and install missing packages
# There are three group of packages:
# 1. DAE Tools related
# 2. Development related (compiler, tools, libraries, etc)
# 3. Utilities (wget, subversion, etc)

if [ ${DISTRIBUTOR_ID} = "debian" ]; then
  #sudo apt-get update
  sudo apt-get install qtbase5-dev qtcreator \
                       autotools-dev automake make pkg-config autoconf gcc g++ gfortran binutils cmake patch \
                       wget subversion fakeroot libfreetype6-dev swig python-dev libpng-dev libxext-dev libbz2-dev

elif [ ${DISTRIBUTOR_ID} = "ubuntu" ]; then
  #sudo apt-get update
  sudo apt-get install qtbase5-dev qtcreator \
                       autotools-dev automake make pkg-config autoconf gcc g++ gfortran binutils cmake patch \
                       wget subversion fakeroot libfreetype6-dev swig python-dev libpng-dev libxext-dev libbz2-dev

elif [ ${DISTRIBUTOR_ID} = "linuxmint" ]; then
  #sudo apt-get update
  sudo apt-get install qtbase5-dev qtcreator \
                       autotools-dev automake make pkg-config autoconf gcc g++ gfortran binutils cmake patch \
                       wget subversion fakeroot libfreetype6-dev swig python-dev libpng-dev libxext-dev libbz2-dev

elif [ ${DISTRIBUTOR_ID} = "fedora" ]; then
  #sudo yum check-update
  sudo yum install qt5 qt-creator qt5-devel \
                   automake make autoconf gcc gcc-c++ gcc-gfortran binutils cmake patch \
                   wget subversion fakeroot rpm-build libbz2-devel

elif [ ${DISTRIBUTOR_ID} = "centos" ]; then
  #sudo yum check-update
  # Missing: scipy, suitesparse-devel, qt-creator 
  # Should be manually installed, ie. from http://pkgs.org
  sudo yum install qt5 qt5-devel python-devel \
                   automake make autoconf gcc gcc-c++ gcc-gfortran binutils cmake patch \
                   wget subversion fakeroot rpm-build libbz2-devel

elif [ ${DISTRIBUTOR_ID} = "suse linux" ]; then
  # Missing: scipy, suitesparse-devel, mayavi
  # Should be manually installed, ie. from http://pkgs.org
  sudo zypper in libqt5 libqt5-devel qt-creator \
                 automake make autoconf gcc gcc-c++ gcc-fortran binutils cmake patch \
                 wget subversion devel_rpm_build libbz2-devel
                 
elif [ ${DISTRIBUTOR_ID} = "arch" ]; then
  sudo pacman -S qt5 qtcreator \
                 base-devel automake make pkg-config autoconf gcc gcc-fortran binutils cmake patch \
                 wget subversion fakeroot swig libpng libxext bzip2

else
  echo "ERROR: unsupported GNU/Linux distribution; please edit the script to add support for: ${DISTRIBUTOR_ID}/${CODENAME}"
  exit -1
fi

