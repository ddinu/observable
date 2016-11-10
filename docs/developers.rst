Developers
==========

Anyone is welcome to contribute code, bug reports, feature requests or just ask
questions.

What code should look like
--------------------------

Please try to follow the `C++ Core Guidelines
<http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>`_ as much as
possible.

There is no formal coding standard, but this being a library, my preference is
for class names to be lowercase and generally, names to be
separated_by_underscores. Try to match the code in the file that you're editing
and everything should be fine.

Also try to keep files under 200 lines and definetly keep files under 500 lines;
and within reason, limit yourself to 80 characters per line if possible.

Testing
-------

Everything should be tested (within reason). Before any commit, please run
the tests.

The testing framework used is Google Test.

All tests should be runnable by just starting the test binary and by `make tests`
(CMake enables this without any issues).

