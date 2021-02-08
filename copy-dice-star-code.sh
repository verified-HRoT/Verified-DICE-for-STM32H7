#!/bin/bash

# Globals
ROOT_DIR=$(pwd)

DICE_STAR=$DICESTAR_HOME
DICE_ENGINE=$ROOT_DIR/src/core/dice_engine
L0_ENGINE=$ROOT_DIR/src/core/l0_engine
COMMON_DIR=$ROOT_DIR/src/common

# Copy DICE code 

# HWAbstraction.c has to be implemented specific to the device
# cd $DICE_STAR/dice/src/c/
# cp -v HWAbstraction.c     $COMMON_DIR

cd $DICE_STAR/dist/dice_engine

cp -v HWAbstraction.h       $COMMON_DIR
cp -v HWState.h             $COMMON_DIR

cp -v DICE*.c               $DICE_ENGINE
cp -v DICE*.h               $DICE_ENGINE

# Copy HACL* library
# cp -v lib/Hacl_Lib.c      $DICE_ENGINE
# cp -v lib/Hacl_Lib.h      $DICE_ENGINE

# Copy L0 code

cd $DICE_STAR/src/c
cp -v Declassification.c    $L0_ENGINE
cp -v L0*.c                 $L0_ENGINE

cd $DICE_STAR/dist/l0

cp -v ASN1*.h               $L0_ENGINE
cp -v ASN1*.c               $L0_ENGINE
cp -v L0*.h                 $L0_ENGINE
cp -v L0*.c                 $L0_ENGINE

# Copy HACL* library
# cp -v lib/Hacl_Lib.c      $L0_ENGINE
# cp -v lib/Hacl_Lib.h      $L0_ENGINE

cd $ROOT_DIR

echo "Copied DICE* C code."
