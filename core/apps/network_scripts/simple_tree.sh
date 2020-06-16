#!/bin/bash

# Topology
# BAS1 <--> NET1 <--> NET3 <--> BAS3
#             ^        ^
#             |        |
#             v        v
# BAS2 <--> NET2      NET4 <--> BAS4
#             ^
#             |
#             v
#           BAS5

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

echo "init applications"
$APPDIR/game/game  -geometry -0+0 --whatwho --auto --autosend --dest=NET --ident=agent1 --startpoint=55,55 --goals=-75,-75 < in1 > out1 &
$APPDIR/NET/net -geometry -0+1400   --whatwho --ident=net1 < in2 > out2 &

$APPDIR/game/game  -geometry -900+0  --whatwho --auto --autosend --dest=NET --ident=agent2 --nogui --startpoint=-55,55 --goals=75,-75 < in3 > out3 &
$APPDIR/game/game  -geometry -1350+0 --whatwho --auto --autosend --dest=NET --ident=agent5 --nogui --startpoint=0,65 --goals=0,-75 < in9 > out9 &
$APPDIR/NET/net -geometry -0+350  --whatwho --nogui --ident=net2 < in4 > out4 &

$APPDIR/game/game  -geometry -900+350   --whatwho --auto --autosend --dest=NET --ident=agent3 --nogui --startpoint=55,-55 --goals=-75,75 < in5 > out5 &
$APPDIR/NET/net --whatwho --ident=net3 --nogui < in6 > out6 &
$APPDIR/game/game  -geometry -900+0 --whatwho --auto --autosend --dest=NET --ident=agent4 --startpoint=-55,-55 --goals=75,75 < in7 > out7 &
$APPDIR/NET/net --whatwho --ident=net4 --nogui < in8 > out8 &


# wait for the link creations
sleep 1

# links creations
echo "Link channels"

cat out1 > in2 &
cat out2 | tee in1 in4 in6 &

cat out3 > in4 &
cat out9 > in4 &
cat out4 | tee in2 in3 in9 &

cat out5 > in6 &
cat out6 | tee in2 in5 in8 &

cat out7 > in8 &
cat out8 | tee in7 in6 &


