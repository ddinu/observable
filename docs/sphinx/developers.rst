Developers
==========

You are welcome to contribute code, bug reports, feature requests or just ask
questions.

Coding standard
---------------

Please try to follow the `C++ Core Guidelines
<http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>`_ as much as
possible.

All code should be as generic as possible.

There is no big, formal coding standard, but you should follow the guidelines
below:

 - Class names must be lowercase and names to be separated_by_underscores.
 - Everything must be documented.
 - Everything must be tested.
 - Try to match the code style in the file that you're editing.
 - Keep files under a couple hundred lines. Definitely keep them under 500
   lines (comments, includes and all).
 - Limit yourself to 80 characters per line.

Testing
-------

Everything should be tested, including code from the examples. Before any
commit, please run the tests. Test failures **will** break the build.

Please think about testing before designing the code.

The testing framework used is Google Test.

All tests should be runnable by just starting the test binary and by `make tests`
(CMake easily enables this).
