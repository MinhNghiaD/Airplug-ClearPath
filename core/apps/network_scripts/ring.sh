#!/bin/bash

# Topology
#                      BAS3
#                       ^
#                       |
#                       v
# BAS1 <--> NET1 <--> NET3 <--> NET4 <--> BAS4
#             ^                  ^
#             |                  |
#             v                  v
# BAS2 <--> NET2 <------------> NET5 <--> BAS5
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
mkfifo in9 out9
mkfifo in10 out10

echo "init applications"
$APPDIR/BAS/bas -geometry -450+0  --whatwho --auto --autosend --dest=NET --ident=bas1 < in1 > out1 &
$APPDIR/NET/net -geometry -0+0    --whatwho --ident=net1 < in2 > out2 &

$APPDIR/BAS/bas -geometry -900+0  --whatwho --auto --autosend --dest=NET --ident=bas2 < in3 > out3 &
$APPDIR/NET/net --whatwho --ident=net2 --nogui < in4 > out4 &

$APPDIR/BAS/bas -geometry -1350+0 --whatwho --auto --autosend --dest=NET --ident=bas3 < in5 > out5 &
$APPDIR/NET/net --whatwho --ident=net3 --nogui < in6 > out6 &

$APPDIR/BAS/bas -geometry -0+350  --whatwho --auto --autosend --dest=NET --ident=bas4 < in7 > out7 &
$APPDIR/NET/net --whatwho --ident=net4 --nogui < in8 > out8 &

$APPDIR/BAS/bas -geometry -450+350 --whatwho --auto --autosend --dest=NET --ident=bas5 < in9 > out9 &
$APPDIR/NET/net --whatwho --ident=net5 --nogui < in10 > out10 &


# wait for the link creations
sleep 1

# links creations
echo "Link channels"

cat out1 > in2 &
cat out2 | tee in1 in4 in6 &

cat out3 > in4 &
cat out4 | tee in2 in3 in10 &

cat out5 > in6 &
cat out6 | tee in2 in5 in8 &

cat out7 > in8 &
cat out8 | tee in7 in6 in10 &

cat out9 > in10 &
cat out10 | tee in9 in8 in4 &
