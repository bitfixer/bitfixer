$Id: readme.txt,v 1.2 2001/04/13 14:14:00 stefano Exp $

How pass the code to the VZ emulator.

- Get the VZ emulator from:
  http://www.powerup.com.au/~intertek/VZ200/vz.htm
- Get the utilities, also
- Set the environment variables correctly
- Compile your code (to a.bas)
- Use the rbinary utility (Rbinary.exe a.bas a.vz)
- Run the emulator
- Type: POKE 30862,0:POKE 30863,128
- Press F10, then 1 (Load Program)
- Chose a.vz
- Return to the emulator (press ENTER)
- Type: X=USR(0)

Enjoy it !
The most difficult thing is to "understand" the VZ keyboard.
It is also possible to create a .WAV file for passing the program on tape.

