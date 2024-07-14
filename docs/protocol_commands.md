# Moving processor from waiter robot (command controll robot) or normal moving robot

## Those are also commands from droid controll center android app

### Get Values

I# - 

V# - get the maximum power

v# - get the minimum power

c# - get the current power

d# - get the low power distance

s# - get the stop distance

C# - get encoder values

## with no reply

b# - break all engines

R# - reset encoders

## set values

Vxxx# - set the maximum power

vxxx# - set the minimum power

cxxx# - set the current power

dxxx# - set the low power distance

sxxx# - set the stop distance

mxxx,yyy# - move or rotate with distance or rotate with degree (if it is interrupted return the distances with I in front)

Mxxx,yyy# - move or rotate until stop command

ru# - raise rfid

rl# - lower rfid

rp# - get rfid possition

# MEGA for waitter robot or command controll robot:

P# - get acc power level

Pxxxx# - play the xxxx file

l - lcd operation

lc# - clear lcd

ln,xx,message# - print message on lcd on next line with xx font

lf,xx,file# - print the file on lcd with xx font (only the number of messages that could be printed will be printed)

N# - clear command list (from memory)

Nfrxxx# - remove command file xxxx

Nfsxxx# - save the navigation data to file xxx from SD

Nflxxx# - load the navigation data from file xxx from SD

Nxxxx# - add the xxxx command to navigation data (this should not have D or B command)

D# - move direct (with navigation data) using navigation from memory

B# - move reverse (with navigation data) using navigation from memory

NDxxx# - move direct (with navigation data) using xxx file

NBxxx# - move reverse (with navigation data) using xxx file

NR# - move using RFID stickers 

# other robot commads:

Txvalue# tilt the camera on horizontaly with the value from the actual possition

Tyvalue# tilt the camera on vertically with the value from the actual possition
