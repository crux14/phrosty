#!/bin/bash

ORIG_VER_PATH=$1
VERSTR=$(cat $ORIG_VER_PATH)
IFS=. VERSIONS=(${VERSTR})

OUTDIR=$2
VERSION_FPATH=$OUTDIR/version.h
MAJOR_VER=${VERSIONS[0]}
MINOR_VER=${VERSIONS[1]}
PATCH_VER=${VERSIONS[2]}

GIT_REV=$(git rev-parse --short HEAD)
if [ $? -ne 0 ]; then
    echo "[phrosty-version] Git Error Occured. If you are installing this product, you can ignore this error."
    exit 0
fi

GIT_REV_LINE="#define PHROSTY_GIT_REV \"${GIT_REV}\""

# if duplicate found, skip generating the header
if [[ -f $VERSION_FPATH ]]; then
  if grep "$GIT_REV_LINE" "$VERSION_FPATH" > /dev/null; then
      if grep "versions: ${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}" "$VERSION_FPATH" > /dev/null; then
          echo '[phrosty-version] Found duplicate. Skipping.'
          exit 0
      fi
  fi
fi


if [[ ! -e $OUTDIR ]]; then
  mkdir $OUTDIR 
fi

echo '#pragma once' > "$VERSION_FPATH"
echo "// versions: ${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}" >  "$VERSION_FPATH"
echo "#define PHROSTY_VERSION_MAJOR ${MAJOR_VER}" >> "$VERSION_FPATH"
echo "#define PHROSTY_VERSION_MINOR ${MINOR_VER}" >> "$VERSION_FPATH"
echo "#define PHROSTY_VERSION_PATCH ${PATCH_VER}" >> "$VERSION_FPATH"
echo ${GIT_REV_LINE} >> "$VERSION_FPATH"

echo "[phrosty-version] Version header (${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}-${GIT_REV}) generated."
