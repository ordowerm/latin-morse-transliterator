# "Latin" to Morse Transliterator
This was a rainy-day project I made in Arduino.

It streams input from the serial terminal in the Arduino IDE, and when hooked up to appropriate hardware (either a buzzer or an LED), it repeats the message, letter-by-letter, in Morse code.

## Design Considerations
I opted to control transliteration with a switch statement, rather than an array. This is _far_ less efficient from a performance standpoint, but I had ample memory on my Arduino Uno, and I opted for more readable code, rather than optimal performance.
