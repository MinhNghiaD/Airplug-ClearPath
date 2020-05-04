#!/bin/bash

# Topology
# BAS1 <--> NET1 <--> NET3 <--> BAS3
#             ^        ^
#             |        |
#             v        v
# BAS2 <--> NET2      NET4 <--> BAS4
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
mkfifo in5 out5
mkfifo in6 out6
mkfifo in7 out7
mkfifo in8 out8

echo "init applications"
$APPDIR/BAS/bas --whatwho --auto --autosend --dest=NET --ident=bas1 < in1 > out1 &
$APPDIR/NET/net --whatwho --ident=net1 < in2 > out2 &
$APPDIR/BAS/bas --whatwho --auto --autosend --dest=NET --ident=bas2 < in3 > out3 &
$APPDIR/NET/net --whatwho --ident=net2 --nogui < in4 > out4 &

$APPDIR/BAS/bas --whatwho --auto --autosend --dest=NET --ident=bas3 < in5 > out5 &
$APPDIR/NET/net --whatwho --ident=net3 --nogui < in6 > out6 &
$APPDIR/BAS/bas --whatwho --auto --autosend --dest=NET --ident=bas4 < in7 > out7 &
$APPDIR/NET/net --whatwho --ident=net4 --nogui < in8 > out8 &

# wait for the link creations
sleep 1

# links creations
echo "Link channels"

cat out1 > in2 &
cat out2 | tee in1 in4 in6 &
cat out3 > in4 &
cat out4 | tee in2 in3 &

cat out5 > in6 &
cat out6 | tee in2 in5 in8 &
cat out7 > in8 &
cat out8 | tee in7 in6 &
