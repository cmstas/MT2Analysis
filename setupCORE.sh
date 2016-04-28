#!/bin/bash

if [ -d /home/users/$USER/CORE ]
then
  echo "Creating symbolic link to ~/CORE , make sure that it's up to date."
  ln -s /home/users/$USER/CORE . 
else 
  git clone git@github.com:cmstas/CORE.git
  cd CORE
  git checkout master
  make -j 10
  cd ..
#  git clone git@github.com:cmstas/Tools.git
fi
