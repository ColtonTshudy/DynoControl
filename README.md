<h1>Dyno Mapper Embedded C</h1>
This firmware is written for an Arduino Uno to control a potentiometer throttle for an electric bike. Communication is handled over UART, where a connected computer sends commands. The arduino will send measurement frames every 500ms, but will send immediate frames if a measurement changes before 500ms.
&nbsp
The code base was written using PlatformIO for VSCode.
