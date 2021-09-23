#!/usr/bin/env bash
# Lab 1
set -e

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

# lsh clean code
TEMPLATE_URL="https://chalmersuniversity.box.com/shared/static/5fl1c8q4vulfcadr4kqqazq3qr27sn1t.zip"

SUBMISSION="lab1-$(whoami)"
TEMPLATE_NAME="lab1-template"
REQUIRED_PATH="code/lsh.c"

[[ $(hostname) =~ remote1[1-2].chalmers.se ]] ||  { echo "[ERROR] You need to compile and test your program at remote11/remote12.chalmers.se before submission!"; exit 1; }

cd "$script_dir"
[[ ! -e "$REQUIRED_PATH" ]] && { echo "[ERROR] Script needs to be placed in the directory that contains: $REQUIRED_PATH"; exit 1; }

rm -rf "$TEMPLATE_NAME"
rm -rf "$SUBMISSION"
wget -q -O "${TEMPLATE_NAME}.zip" "$TEMPLATE_URL" > /dev/null
unzip "${TEMPLATE_NAME}.zip"
mv "$TEMPLATE_NAME" "$SUBMISSION"
rm "${TEMPLATE_NAME}.zip"
cp code/lsh.c "$SUBMISSION"/
cd "$SUBMISSION"

make clean > /dev/null
make

./lsh << EOF
date
exit
EOF

make clean > /dev/null

cd ..
zip -FSr "${SUBMISSION}.zip" "$SUBMISSION/"
rm -r "$SUBMISSION"
echo
echo "===================================================="
echo ">> Submission prepared SUCCESSFULLY: $PWD/${SUBMISSION}.zip"
echo ">> Upload to Canvas together with your report!"
echo "===================================================="

