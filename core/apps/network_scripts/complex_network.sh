#!/bin/bash

# Topology
#                      BAS3     NET10 <-->BAS10
#                       ^        ^
#                       |        |
#                       v        v
# BAS1 <--> NET1 <--> NET3 <--> NET4 <--> BAS4
#             ^                  ^
#             |                  |
#             v                  v
# BAS2 <--> NET2 <------------> NET5 <--> BAS5
#             ^                  ^
#             |                  |
#             |              |-------|
#             v              v       v
# BAS6 <--> NET6 <-------> NET7     NET8 <--> BAS8
#                            ^       ^
#                            |       |
#                            v       v      -->BAS9
#                          BAS7     NET9 <--|
#                                           -->BAS11
#
#
#
#
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
mkfifo in11 out11
mkfifo in12 out12
mkfifo in13 out13
mkfifo in14 out14
mkfifo in15 out15
mkfifo in16 out16
mkfifo in17 out17
mkfifo in18 out18
mkfifo in19 out19
mkfifo in20 out20
mkfifo in21 out21

echo "init applications"
$APPDIR/BAS/bas -geometry -450+0 --whatwho --auto --autosend --dest=NET --ident=bas1 < in1 > out1 &
$APPDIR/NET/net -geometry -0+0 --whatwho --ident=net1 < in2 > out2 &

$APPDIR/BAS/bas -geometry -900+0 --whatwho --auto --autosend --dest=NET --ident=bas2 < in3 > out3 &
$APPDIR/NET/net --whatwho --ident=net2 --nogui < in4 > out4 &

$APPDIR/BAS/bas -geometry -1350+0 --whatwho --auto --autosend --dest=NET --ident=bas3 < in5 > out5 &
$APPDIR/NET/net --whatwho --ident=net3 --nogui < in6 > out6 &

$APPDIR/BAS/bas -geometry -0+350 --whatwho --auto --autosend --dest=NET --ident=bas4 < in7 > out7 &
$APPDIR/NET/net --whatwho --ident=net4 --nogui < in8 > out8 &

$APPDIR/BAS/bas -geometry -450+350 --whatwho --auto --autosend --dest=NET --ident=bas5 < in9 > out9 &
$APPDIR/NET/net --whatwho --ident=net5 --nogui < in10 > out10 &

$APPDIR/BAS/bas -geometry -900+350 --whatwho --auto --autosend --dest=NET --ident=bas6 < in11 > out11 &
$APPDIR/NET/net --whatwho --ident=net6 --nogui < in12 > out12 &

$APPDIR/BAS/bas -geometry -1350+350 --whatwho --auto --autosend --dest=NET --ident=bas7 < in13 > out13 &
$APPDIR/NET/net --whatwho --ident=net7 --nogui < in14 > out14 &

$APPDIR/BAS/bas -geometry -0+700 --whatwho --auto --autosend --dest=NET --ident=bas8 < in15 > out15 &
$APPDIR/NET/net --whatwho --ident=net8 --nogui < in16 > out16 &

$APPDIR/BAS/bas -geometry -450+700 --whatwho --auto --autosend --dest=NET --ident=bas9  < in17 > out17 &
$APPDIR/BAS/bas -geometry -900+700 --whatwho --auto --autosend --dest=NET --ident=bas11 < in18 > out18 &
$APPDIR/NET/net --whatwho --ident=net9 --nogui < in19 > out19 &

$APPDIR/BAS/bas -geometry -1350+700 --whatwho --auto --autosend --dest=NET --ident=bas10 < in20 > out20 &
$APPDIR/NET/net --whatwho --ident=net10 --nogui < in21 > out21 &

# wait for the link creations
sleep 1

# links creations
echo "Link channels"

cat out1 > in2 &
cat out2 | tee in1 in4 in6 &

cat out3 > in4 &
cat out4 | tee in2 in3 in10 in12 &

cat out5 > in6 &
cat out6 | tee in2 in5 in8 &

cat out7 > in8 &
cat out8 | tee in7 in6 in10 in21 &

cat out9 > in10 &
cat out10 | tee in9 in8 in4 in14 in16 &

cat out11 > in12 &
cat out12 | tee in11 in4 in14 &

cat out13 > in14 &
cat out14 | tee in13 in12 in10&

cat out15 > in16 &
cat out16 | tee in15 in10 in19 &

cat out17 > in19 &
cat out18 > in19 &
cat out19 | tee in16 in17 in18 &

cat out20 > in21 &
cat out21 | tee in20 in8 &
