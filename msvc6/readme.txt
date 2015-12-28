This is the MSVC6 variant of the Macho class library (based on Version 0.9.5).

To compile with other compilers, replace the line
    #define TYPENAME
with
    #define TYPENAME typename
in file Macho.hpp
    
--------------------------------------------------------------
There are some major differences to the original version:

- The method setState and setStateDirect aren't template methods anymore 
and have a different signature.

Original version:

    setState<StateA>(new StateA::Box());
    
MSVC6 variant:

    StateA::setState(machine(), new StateA::Box());

The new state is entered by calling the static method setState with the machine
instance as parameter and an optional box instance.

--------------------------------------------------------------
Furthermore this variant is not as optimized as the original version. 
I'm tired of messing around with this crappy broken compiler, so there.

Why not let it die already?
