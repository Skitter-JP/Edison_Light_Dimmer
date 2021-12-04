# Edison Light Dimmer

<img src="/Images/dimmer.gif" align="right"
     alt="Dimmer GIF" width=50% height=50% >

In this project I have designed a dual trailing edge phase dimmer using a Triac as the main switching component. 

This device has 3 microcontrollers.

1. An ESP8266 which handles the WiFi communication such as sending and recvied data to and from a MQTT server.
2. An Arduino Nano considered as the "Sensor" controller. It collects all the recorded temperatures in the Light Dimmer, it also captures the consumed current of the deivce and pushed these values to the ESP8266.
3.  Another Arduino Nano considered as the "Triac" controller. It recieves a Zero-Cross signal and then calcualtes the trigger times for each Triac based of the desiered power.

All 3 controllers are interconnected via I2C.

## The ESP8266

## The Sensor uController

## The Triac uController


Notes:

1. The Inductors L1, L2, L3 and L4 do not have enough indutance to fully supress the inrush current of the Bulbs. I did no maths when implementing these inductors. I simply bought the biggest inductors that were avaliable at my local electronics store. I would recommend finding an inductor with a value closer to 1[H] with a current rating of 6[A] minimum.

2. The Inductors L1 and L3, L2 and L4 are in series, which reduces the current between the inductors by half, this was done to reduce the amount of heat that each inductor would emit but, doing so reduces the indutance by half.

3. The temperatue sensors are interfaced with a One-Wire Interface. With 6 of these sensors alot of electrical noise is generated when communcating with them. This noise is somehow probgated to the cut sinusoid and an udable ringing noise is appartent, which emits from the bulbs. I have not investigated ways to emit or evening reduce this "electrical/audioable" noise. But maybe reducing the clock speed of the One-Wire bus could help. After speaking to a Professor about this issue, he mentioned that my track layout on the PCB is most likley the issue. If I create another revision of this project I would most likley change the tempeature sensors to some thermocouples or thermistors and have a mutliplex and opamp to measure the tempatures. 

4. Implememting 3 uControllers is diffenetly over kill but it makes things much easier when programing these devices. If I would put everything onto the ESP8266 it would be more difficuilt to code since I would have to think about all the different types of interupts that would occur when dealing with the Wifi, Triacs, Zero-Crossing, One-Wire ciruits, etc. The Wifi libary is a huge black box that just works. I did not want to spend time diving into the Wifi library to see how I could make things more efficant.

This project started when I was gifted 4 tungenten edison light bulbs, after thinking what I should do with them I came to the conclusion that I need 16 more of them and make a cendelaere like display of lights, but the true magic of these light bulbs are only apparent when they are running at around 20% of their rated power.

This project began before I started my education in electrical engineering and I did not know much about reducing/limiting the power to any type of load.

While browing a local elecrtronics store in their "kits" sections I found a "speed control" circuit for motors that used 220VAC as an input.



<img src="/Images/dimmer.gif" width=60%>

<img src="/Images/Block-diagram.png" width=30%>

<img src="/Images/voltage_regulator_mod.JPG" width=40%>
