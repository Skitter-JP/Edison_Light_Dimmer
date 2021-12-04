# Edison Light Dimmer

In this project I have designed a dual trailing edge phase dimmer using a Triac as the main switching component.

Oriignally the device hosted a webpage which was accessable on the LAN that it was connected to, but this was a bit annoyting because I had to open a browers and wait for the web page to load everytime I wanted to chnanged the state of the device. I then found out about the MQTT protocol and Node-Red, check them out if you have not heared of them.

MQTT is basically a light weight communcation protocol which is perfect for home automation deivce.

Node-Red is a flow-based developement tool which is also great for home automation, you can interface almost anything to it, most notably MQTT and Homebridge.

Homebridge allows you to create custom devices that will show up in the iOS/Mac OS Homekit program, this is how I ended up controlling this device. In Homekit the device shows up as 2 dimable light bulbs.

Node-Red also allows you to interface a Telegram bot. So you can control almost anything in your home via the bot, for example if some over current/temperature is detected. Node-Red can send me a message via telegram notifying me.



This device has 3 microcontrollers.

1. An ESP8266 which handles the WiFi communication such as connecting to a MQTT server and sending/reciveing data to and from that server.
2. An Arduino Nano considered as the "Sensor" controller. It collects all the recorded temperatures in the Light Dimmer, it also captures the consumed current of the deivce and pushed these values to the ESP8266.
3.  Another Arduino Nano considered as the "Triac" controller. It recieves a Zero-Cross signal and then calcualtes the trigger times for each Triac based of the desiered power.

All 3 controllers are interconnected via I2C.

The block diagrma below desribes the main interconnections in the device.

<img src="/Images/Block-diagram.png" width=55%>




## uController PCB
Here we cna view the uController PCB that connects all 3 micro-controllers

This board has the following

- ESP8266
- 2x Arduino Nanos
- 4 LED connectors
- Connector to the Power Control PCB
- Connector to the Triac PCB
- 12[V] connector
- A buck PSU which supplys a 5[V] from 12[V], originally there was a 7805 but after a few months of running it I discovered that it got really hot. I had a choice between adding a headsink or put a buck converter in palce of the LDO. The buck converter seemed like a better idea since there would be not heat. Luckly there was enough space to mount it on the board.

<img src="/Images/voltage_regulator_mod.JPG" width=55%>

## Triac PCB

<img src="/Images/IMG_0307.JPG" width=55%>


<img src="/Images/Triac_Board_Unpopulated.JPG" width=55%>



## Power Control PCB
Here we can view the Power Control PCB. The Power control PCB hold the following

- Control Relays that will completlying disconnect the Live and Neutral when the Output is on the "off" state.
- an ACS712 for current sensing
- 

To the top left you can see the switch mode PSU that proivde the 12[V] rail for the system

To the top right you can see a EMI filter.

And lastly at the bottom of the enclouse you can see the Power Control Board.


<img src="/Images/Power_control_and_PSU_and_filter.JPG" width=55%>


## Notes

1. The Inductors L1, L2, L3 and L4 do not have enough indutance to fully supress the inrush current of the Bulbs. I did no maths when implementing these inductors. I simply bought the biggest inductors that were avaliable at my local electronics store. I would recommend finding an inductor with a value closer to 1[H] with a current rating of 6[A] minimum.

2. The Inductors L1 and L3, L2 and L4 are in parralle, which reduces the current between the inductors by half, this was done to reduce the amount of heat that each inductor would emit but, doing so reduces the indutance by half.

3. The temperatue sensors are interfaced with a One-Wire Interface. With 6 of these sensors alot of electrical noise is generated when communcating with them. This noise is somehow probgated to the cut sinusoid and an udable ringing noise is appartent, which emits from the bulbs. I have not investigated ways to emit or evening reduce this "electrical/audioable" noise. But maybe reducing the clock speed of the One-Wire bus could help. After speaking to a Professor about this issue, he mentioned that my track layout on the PCB is most likley the issue. If I create another revision of this project I would most likley change the tempeature sensors to some thermocouples or thermistors and have a mutliplex and opamp to measure the tempatures. 

4. Implememting 3 uControllers is diffenetly over kill but it makes things much easier when programing these devices. If I would put everything onto the ESP8266 it would be more difficuilt to code since I would have to think about all the different types of interupts that would occur when dealing with the Wifi, Triacs, Zero-Crossing, One-Wire ciruits, etc. The Wifi libary is a huge black box that just works. I did not want to spend time diving into the Wifi library to see how I could make things more efficant.

This project started when I was gifted 4 tungenten edison light bulbs, after thinking what I should do with them I came to the conclusion that I need 16 more of them and make a cendelaere like display of lights, but the true magic of these light bulbs are only apparent when they are running at around 20% of their rated power.

This project began before I started my education in electrical engineering and I did not know much about reducing/limiting the power to any type of load.

While browing a local elecrtronics store in their "kits" sections I found a "speed control" circuit for motors that used 220VAC as an input.


<img src="/Images/Control_via_Ipad.gif" align="right"
     alt="Dimmer GIF" width=30% height=10% >

<img src="/Images/Control_via_Ipad.gif" width=60%>



