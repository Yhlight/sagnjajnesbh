#!/bin/bash
sed -i "s/CodeFragmentType/FragmentType/g" include/dispatcher/CompilerDispatcher.h
echo "Fixed dispatcher"
