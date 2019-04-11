#************************************************************************************
#                 DAE Tools Project: www.daetools.com
#                 Copyright (C) Dragan Nikolic, 2010
#************************************************************************************
# DAE Tools is free software; you can redistribute it and/or modify it under the 
# terms of the GNU General Public License version 3 as published by the Free Software
# Foundation. DAE Tools is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with the
# DAE Tools software; if not, see <http://www.gnu.org/licenses/>.
#************************************************************************************
include(../dae.pri)
QT -= core gui
TEMPLATE = lib
CONFIG += shared plugin

#################################################
# Could be: SuperLU, SuperLU_MT, SuperLU_CUDA
#################################################
CONFIG += SuperLU

LIBS += $${SOLIBS_RPATH_SL}

#####################################################################
# Small hack used when compiling from compile_linux.sh shell script
#####################################################################
shellCompile {
shellSuperLU {
  CONFIG += SuperLU
}
shellSuperLU_MT {
  CONFIG += SuperLU_MT
}
shellSuperLU_CUDA {
  CONFIG += SuperLU_CUDA
}
}

INCLUDEPATH += $${BOOSTDIR} \
               $${SUNDIALS_INCLUDE}

SOURCES +=  stdafx.cpp \
            dllmain.cpp \
            superlu_la_solver.cpp \
            config_data.cpp \
            ../mmio.c

HEADERS +=  stdafx.h \
            superlu_solvers.h \
            superlu_la_solver.h \
            superlu_mt_gpu.h \
            config_data.h \
            ../mmio.h

######################################################################################
#                                   SuperLU
######################################################################################
CONFIG(SuperLU, SuperLU|SuperLU_MT|SuperLU_CUDA):message(SuperLU) { 

QMAKE_CXXFLAGS += -DdaeSuperLU
TARGET = cdaeSuperLU_LASolver
INCLUDEPATH += $${SUPERLU_INCLUDE}
LIBS += $${DAE_CONFIG_LIB} \
        $${SUPERLU_LIBS} \
        $${BLAS_LAPACK_LIBS} \
        $${BOOST_LIBS}
}

######################################################################################
#                                SuperLU_MT
######################################################################################
CONFIG(SuperLU_MT, SuperLU|SuperLU_MT|SuperLU_CUDA):message(SuperLU_MT) { 

QMAKE_CXXFLAGS += -DdaeSuperLU_MT
TARGET = cdaeSuperLU_MT_LASolver
INCLUDEPATH += $${SUPERLU_MT_INCLUDE}
LIBS += $${DAE_CONFIG_LIB} \
        $${SUPERLU_MT_LIBS} \
        $${BLAS_LAPACK_LIBS} \
        $${BOOST_LIBS}
}

######################################################################################
#                                SuperLU_CUDA
# compile it with: make --file=gpuMakefile
######################################################################################
CONFIG(SuperLU_CUDA, SuperLU|SuperLU_MT|SuperLU_CUDA):message(SuperLU_CUDA) { 

QMAKE_CXXFLAGS += -DdaeSuperLU_CUDA
TARGET = cdaeSuperLU_CUDA_LASolver
INCLUDEPATH += $${SUPERLU_CUDA_INCLUDE}
LIBS += $${SUPERLU_CUDA_LIBS} $${CUDA_LIBS}
}

OTHER_FILES += superlu_mt_gpu.cu gpuMakefile


#######################################################
#                Install files
#######################################################
QMAKE_POST_LINK = $${COPY_FILE} \
                  $${DAE_DEST_DIR}/$${SHARED_LIB_PREFIX}$${TARGET}$${SHARED_LIB_POSTFIX}.$${SHARED_LIB_EXT} \
                  $${SOLIBS_DIR}/$${SHARED_LIB_PREFIX}$${TARGET}$${SHARED_LIB_POSTFIX}.$${SHARED_LIB_EXT}

DAE_PROJECT_NAME = $$basename(PWD)

install_headers.path  = $${DAE_INSTALL_HEADERS_DIR}/$${DAE_PROJECT_NAME}
install_headers.files = *.h

install_libs.path  = $${DAE_INSTALL_LIBS_DIR}
install_libs.files = $${DAE_DEST_DIR}/$${SHARED_LIB_PREFIX}$${TARGET}$${SHARED_LIB_POSTFIX}.$${SHARED_LIB_EXT}

INSTALLS += install_headers install_libs
