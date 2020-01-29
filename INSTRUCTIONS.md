# Instructions

Hint: All keybindings can be seen by pressing `F1`.

---

The goal of the game is to get as high as possible.

You can move your character with `A / Left Arrow` and `D / Right Arrow` and jump with `W / Up Arrow` or `Space`.  
To jump higher you have to press the jump key longer.

## Characters

There are three different types of characters:
1. **Walkers**  
    Those are the red enemies that can walk.  
    The walker will walk around the map and deals damage on contact.
2. **Shooters**  
    Those are the black ones that shoot at you.  
    The shooter can't move but will shoot at players if they're near enough.
3. **Players**  
    All players. Simple. (Not like the other ones.)  
    Can't shoot or attack anything but can die.

## Objects

First, the basics:
- **Simple grounds**  
    Those are the ones with grass on top.  
    You can walk on them and they won't do anything.
- **Spikes**  
    The gray ones which look like... spikes. :-)
    They deal damage on contact so you should try to avoid them.
    (I am so helpful :-) )
- **Portals**  
    They are those weird looking *roundish* things.  
    If you get in contact with them, you will be teleported somewhere else.
- **Coins**  
    You can collect them and get... coins.  
    The current amount of owned coins can be seen at the upper border of the screen.
    You can use these to buy things.
- **Hearts**  
    Collect them to gain lifes back.  
    You can't get more the 5 lifes though.

Ok now more interactive stuff. There are different types of buttons which can be activated with `Enter` while standing "in" them.
- **Multiplayer**  
    Press to start mutiplayer. Nice.  
    The game window will close and you have to enter something in the terminal.  
    If you want to host a game, just press `Enter`.
    After that you can setup a "race".
    This will generate a red line in the world which can be seen by everyone and the first player who passes this line is the winner.  
    If you want to join a game, you have to enter the hosts ip.
    **Note:** The port used by this game is `2307`.
- **Unlock**  
    This is a open lock.
    It's purpose is mostly for multiplayer.  
    The small ground which blocks the way will be removed and the game is started, when you press the button.  
    After this is done, no more players can join the game.
- **Shopping cart**  
    Yeah, you can be things here:
    - **Star**  
        This will give you immortality for 30 seconds.
        The price is 15 coins.
    - **Up Arrow**  
        This will give you a jump boost for 15 seconds.
        The price is 3 coins.  
        By the way: You can stack this to jump higher.

Cool. That's it for the objects.

## Command line arguments

There are two possible command line arguments:
1. **ip**  
    You can give the program an ip at start. It will then directly try to onnect to this ip.  
    Example: `ip=192.168.178.123`, `ip=localhost`
2. **scenes**  
    You may notice that the world is built out of different scenes / segments which will replay at some point.  
    Those scenes will be randomly placed on each other while jumping higher.
    If all players are above a certain point, all stuff below will be removed.

    So if you want to play a special order or only one scene, you can tell the program to do so.  
    Example: `lucky_jump.xml`, `lucky_jump.xml,hello_world.xml`  
    Note: The file name is a relative path and `Ressources/Scenes/` will be set before each file name.

3. **editor**  
    It is possible to directly open the editor with a loaded file with the `editor` argument.
    The editor will behave exactly as if you loaded a file wit `L`.  
    The only exception is that there won't be the two extra grounds for the start of each scene.
    The two ones which are saved in the provided file (if so) will be there though.

    Example: `editor=lucky_jump.xml`


## Editor

As mentioned above, the world is built from multiple scenes.
There are multiple scenes available which are stored in xml files inside the `Ressources/Scenes/` directory.  
But it is very easy to build new scenes by yourself with the editor.

To open the editor, press `F6` while being ingame.
Note that this will end the current game as you are not able to return to the game once the editor is opened.

The editor itself is pretty simple.
You can place objects with the mouse and switch between different types of objects by pressing the keys `0 - 9`.  
The left mouse button will place grounds and the right one will place enemies.
An overview about the specific keys can be seen by pressing `F1` ingame or in the `Ressources/editor_help.txt` file.

You can select multiple objects by pressing `Ctrl` and clicking on them.
By pressing `Shift` and moving the mouse, all selected objects will be moved.
`C` will place a copy of the selected objects at the same position and `Del` will remove all selected objects.  
It is also possible to remove the last placed ground or enemie by pressing `Backspace`.  
**Note**: This won't replace objects and isn't working with some special ones.

**Note**: When opening the editor, there are two grounds already placed.
Since they act as a transition for the previous scene they shouldn't be (re-)moved.
You can see where and how the next scene will start by pressing `N`.

Press `P` to place the player at the mouse position and test the scene.  
Since there is no autosave you should save and reload your work every few minutes.
Saving is done by pressing `Enter`.