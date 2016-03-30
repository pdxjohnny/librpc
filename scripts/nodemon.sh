#!/bin/bash

ln -s makefile makefile.make
nodemon -e c,h,make --exec 'clear; make clean && make; exit 0'

