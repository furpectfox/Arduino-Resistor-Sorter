# Resistor Sorter with ATtiny85
While resistors have color codes, is it not more convenient to just read the value directly? A multimeter provides highly accurate readings, but measuring each resistor takes time. This device offers a quick and relatively accurate way to identify resistor values without the extra effort.

[Watch the demo on YouTube](https://www.youtube.com/watch?v=EC18hliZ0wA)

## Theory
This project is based on the voltage divider equation, a common method used in other Arduino ohm meter projects. The principle is simple: by placing an unknown resistor in series with a known reference resistor and measuring the voltage drop, we can calculate its resistance. The accuracy improves when the two resistors have similar values. To achieve a balanced tradeoff between precision and simplicity, this circuit uses five different known resistor values for measurement.

## Immplimentation
The "brain" of this project is an ATtiny85. Since it has a limited number of I/O pins, a 74HC595 Serial-In-Parallel-Out shift register is used to expand the number of output pins needed to control the transistors that select different reference resistors.

For the display, a 4-digit 7-segment display is used instead of an LCD or OLED—simply because that was what I had on hand. Another 74HC595 shift register is used to control the display, as the ATtiny85 alone does not have enough pins.
