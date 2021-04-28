# Joustus SFML c++ clone

This is simple clone of Joustus card game form "Shovel Knight: King of cards"

### How to run game

## Open server: 
- open src/server folder in cmd
- enter "make IPADRESS PORT" 

## Open client
- open src folder in cmd
- enter "make IPADRESS PORT KEY" for first client 
- enter "bin\main IPADRESS PORT KEY" for second client 

IPADRESS and PORT is ipadress and port you want to put the server on 
KEY is key to game. To connect with another player you must have the same keys.
 
### Game instruction

To move cards use WASD.
To select or place card use space.
To cancel selection use escape.
To see animation of how your move will look, just wait a moment with selected card over another card.

Offical Joustus rules: https://yachtclubgames.com/joustus-instruction-manual/

### Warning

To run it on your computer you need to have mingw64 and SFML-2.5.1 on the root of your C drive