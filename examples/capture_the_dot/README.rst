Capture The Dot Example
=======================

*Capture The Dot* is a simple game built with Qt and the Observable library
to showcase how you can use observable objects to build the model part of
a MVC app.

The code is using a simple MVC architecture, consisting of a game controller,
model and view, and has just 3 classes in total.

Here's what these 3 classes do:

 - GameView -- Renders the game and captures user input. This is a QWidget.
 - GameModel -- Contains the game's business logic, like positioning computations,
                score and hit-test code. This class does not use Qt at all.
 - GameController -- Hooks up the view and the model so that the view is updated
                     when the model changes and the model's actions are called
                     when different events happen.

Building the example
--------------------

You will need Qt installed and discoverable by CMake.

You can manually tell CMake about Qt by either defining the QT_ROOT variable on
the command line or setting the QT_ROOT_32 or QT_ROOT_64 environment variables.

If Qt is not found, the target will not be generated.

Also, note that this target is compiled by default.
