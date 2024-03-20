# Untitled 2D Coop Platfdrmer Game
## COMMON DELIVERABLES
- [X] Functioning Entity System
- [X] Fully interact-able environment (Collides with every tile within the world)
- [X] User Interface (small main menu that works and a dash counter)
- [X] File read / write access (The information does save but the only problem is that it doesn't load accurately]
- [X] README for how to play and see all game features 

## PERSONAL DELIVERABLES
- [X] tile based platforming world (You'll see it almost immediately)
- [?] 5 world obstacles / traps (got like maybe 3 out of 5)
   - Pit of death (without the death)
   - Things to jump on
   - gaps and vertical platforming challenge
- [X] 5 player movement abilities (all of this needs further work but it does move the character)
   - Walk
   - Jump
   - Dash
   - Air Dash
   - Wall Jump
- [X] Couch co-op with controller support (try the game with an xbox controller)
- [] 5 types of monsters

# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal. 

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder anre simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.
