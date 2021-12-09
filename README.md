# Edison Light Dimmer


<img src="/Images/IMG_2473.JPG" align="right"
     alt="lights_intro" width=40% >
     

In this project I have designed a dual trailing edge phase dimmer using a Triac as the main switching component.

Orignally the device hosted a webpage which was accessable on the LAN that it was connected to, but this was a bit annoying because I had to open a browser and wait for the web page to load everytime I wanted to chnanged the state of the device. I then discovered the MQTT protocol and Node-Red. check them out if you have not heared of them.

MQTT is basically a light weight communcation protocol which is perfect for microcontrollers.

Node-Red is a flow-based developement tool which is also great for home automation, you can interface almost anything to it, most notably MQTT and Homebridge.

Homebridge allows you to create custom devices that will show up in the iOS/Mac OS Homekit program, this is how I ended up controlling this device. In Homekit the device shows up as 2 dimable light bulbs.

<img src="/Images/mqtt_logo.png" align="right"
     alt="mqtt" width=20%> 
     
<img src="/Images/home-kit.jpg" align="right"
     alt="mqtt" width=25%> 

Node-Red also allows you to interface a Telegram bot. So, you can control almost anything in your home via the bot, for example if some over current/temperature is detected in the light dimmer, Node-Red can send me a message via telegram notifying me.

This device has 3 microcontrollers.

1. An ESP8266 which handles the WiFi communication such as connecting to a Wifi Network, connecting MQTT server and sending/reciveing data to and from that server.
2. An Arduino Nano considered as the "Sensor" controller. It collects all the recorded temperatures in the Light Dimmer, it also captures the consumed current of the deivce and pushes these values to the ESP8266.
3.  Another Arduino Nano considered as the "Triac" controller. It recieves a Zero-Cross signal and then calcualtes the trigger times for each Triac based of the desiered power.

All 3 controllers are interconnected via I2C.

The block diagrma below desribes the main interconnections in the device.

<img src="/Images/Block-diagram.png" width=55%>

## uController PCB
Here we can view the uController PCB that connects all 3 micro-controllers

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

Here we can view the Triac PCB mounted to the enclouser, the uController PCB is also visable.

This board conisits of the High and Low Voltage side, where the high voltage side has 220VAC that will be cut by the Triacs and the low voltage side has optocouplers which trigger those Triacs.

The circuit is quite basic and a full discription can be found [here](http://www.soloelectronica.net/PDF/moc3020.pdf) and a super basic diagram can be found [here](http://www.soloelectronica.net/PDF/moc3020.pdf)

This board has the following

-2x Triac Circuits with RC snubers and MOVs for voltage spikes
- A low voltage connector suppliny a 5[V] rail, Ground, One-Wire and 2 Trigger lines for the optocouplers, this connector goes to the uController PCB.
- A high voltage connector that has 220[VAC] input and the reutrn chopped sinewave. This connector goes to the Power Control PCB.

These photos were taken while I was assembling the device, that is why some components are missing.

<img src="/Images/IMG_0307.JPG" width=55%>

<img src="/Images/Triac_Board_Unpopulated.JPG" width=55%>

## Power Control PCB
Here we can view the Power Control PCB.

To the top left you can see the switch mode PSU that proivde the 12[V] rail for the system

To the top right you can see a EMI filter.

And lastly at the bottom of the enclouser you can see the Power Control Board.

The Power control PCB holds the following

- Control Relays that will completly disconnect the Live and Neutral when the output is off.
- an ACS712 for current sensing
- 2 pairs of inductors for current limiting.
- Zero cross detection circuit
- Fuses on the Live and Neutral lines

<img src="/Images/Power_control_and_PSU_and_filter.JPG" width=55%>

Here I was fitting the heatsinks to the board, the two outer screws hold the heatsink to the board, the one inner screw has a threaded hole and couples the Triac to the heatsink.

<img src="/Images/IMG_0294.JPG" width=55%>

## Pictures and Discriptions

Here we can see the dimmer interface with a website hosted on the ESP.
The website is very responsive and the values were updated via a websocket, therfor the web page did not have to refesh inorder to get new data. But loading the website took a few seconds which was quite annouying

<img src="/Images/Control_via_Ipad.gif" width=55%>

Here I was testing the trigger singals for the Triac vs the Zero Cross Signal

<img src="/Images/Testing_with_Scope.JPG" width=55%>

Here is ther complete unit although the Temperature sensors have not yet been installed

<img src="/Images/Complete_Unit.JPG" width=55%>

Here is a basic deminstartion of the dimmer turning a single bulb on and off.
Note the "soft start" of the bulb, theroitcally this can dramatically increase the life span of the bulbs.

<img src="/Images/Testing.gif" width=55%>

## Notes

1. The Inductors L1, L2, L3 and L4 do not have enough indutance to fully supress the inrush current of the Bulbs. I did no maths when implementing these inductors. I simply bought the biggest inductors that were avaliable at my local electronics store. I would recommend finding an inductor with a value closer to 1[H] with a current rating of 6[A] minimum.

2. The Inductors L1 and L3, L2 and L4 are in parralle, which reduces the current between the inductors by half, this was done to reduce the amount of heat that each inductor would emit but, doing so reduces the indutance by half.

3. The temperatue sensors are interfaced with a One-Wire Interface. With 6 of these sensors alot of electrical noise is generated when communcating with them. This noise is somehow probgated to the cut sinusoid and an udable ringing noise is appartent, which emits from the bulbs. I have not investigated ways to emit or evening reduce this "electrical/audioable" noise. But maybe reducing the clock speed of the One-Wire bus could help. After speaking to a Professor about this issue, he mentioned that my track layout on the PCB is most likley the issue. If I create another revision of this project I would most likley change the tempeature sensors to some thermocouples or thermistors and have a mutliplex and opamp to measure the tempatures. 

4. Implememting 3 uControllers is diffenetly over kill but it makes things much easier when programing these devices. If I would put everything onto the ESP8266 it would be more challanging to code since I would have to think about all the different types of interupts that would occur when dealing with the Wifi, Triacs, Zero-Crossing, One-Wire, etc. The Wifi libary is a huge black box that just works. I did not want to spend time diving into the Wifi library to see how I could make things more efficant.

5. Almost all these compomenets (enclouser, heatsinks, relays, EMI filter, PSU) were purchased from gme.cz 

6. The ESP8266 comes in different sizes, if you do plan to frabicate the PCBs ensure you buy the correct ESP8266 board.

7. The Code provided requires a MQTT server on the LAN it connects to, in the WiFi_code you can find the topics which the board will be publishing and subscrided to.

## Orders

If you would like a complete unit built and delivered this can be arranged for the right price.

Please contact me if you are interested.
