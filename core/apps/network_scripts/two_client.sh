#!/bin/bash

# Topology
# BAS1 <--> NET1
#             ^
#             |
#             v
# BAS2 <--> NET2
#

APPDIR="$(dirname "$PWD")"

if test -f "in*"; then
    rm in*
fi

if test -f "out*"; then
    rm out*
fi

mkfifo in1 out1
mkfifo in2 out2
mkfifo in3 out3
mkfifo in4 out4

echo "init applications"
$APPDIR/BAS/bas -geometry -0+0 --whatwho --auto --dest=NET --ident=bas1 < in1 > out1 &
$APPDIR/NET/net -geometry -450+0 --whatwho --ident=net1 < in2 > out2 &
$APPDIR/BAS/bas -geometry -900+0 --whatwho --auto --dest=NET --ident=bas2 < in3 > out3 &
$APPDIR/NET/net --whatwho --ident=net2 --nogui < in4 > out4 &

# wait for the link creations
sleep 1

# links creations
echo "Link channels"

cat out1 > in2 &
cat out2 | tee in1 in4 &
cat out3 > in4 &
cat out4 | tee in2 in3 &

