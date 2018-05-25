#!/bin/bash

echo "Patching MD_MAX72xx..."
patch -p1 < patches/MD_MAX72xx.patch
echo "Patching MD_Parola..."
patch -p1 < patches/MD_Parola.patch
echo "All patching done"
