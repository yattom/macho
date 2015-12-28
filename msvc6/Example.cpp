// Just a small state machine (3 states) as demonstration.
//
// Compile like this:
// cl /EHsc Macho.cpp Example.cpp

#include "Macho.hpp"

#include <iostream>
using namespace std;


namespace Example {

	////////////////////////////////////////////////////////
	// State declarations

	// Machine's top state
	TOPSTATE(Top) {
		// Top state variables (visible to all substates)
		struct Box {
			Box() : myData(0) {}
			Box(long l) : myData(l) {}
			long myData;
		};

		STATE(Top)

		// Machine's event protocol
		virtual void event1(int i) {}
		virtual void event2(long l) {}

	private:
		// special actions
		void entry();
		void exit();
		void init();
	};

	// A superstate
	SUBSTATE(Super, Top) {
		STATE(Super)

		// This state has history
		HISTORY()

	private:
		// Entry and exit actions of state
		void entry();
		void exit();
	};

	// A substate
	SUBSTATE(StateA, Super) {
		// State variables
		struct Box {
			Box() : myData(0) {}
			Box(int i) : myData(i) {}
			int myData;
		};

		STATE(StateA)

		// Event handler
		void event1(int i);

	private:
		void entry();
		void exit();
	};

	// A substate
	SUBSTATE(StateB, Super) {
		STATE(StateB)

		void event2(long l);

	private:
		void entry();
		void exit();
	};

	////////////////////////////////////////////////////////
	// Event handler implementations

	// Top state
	void Top::entry() { cout << "Top::entry" << endl; }
	void Top::exit() { cout << "Top::exit" << endl; }
	void Top::init() {
		// Initialize state with box
		StateA::setState(machine(), new StateA::Box(44));
	}

	// Super state
	void Super::entry() { cout << "Super::entry" << endl; }
	void Super::exit() { cout << "Super::exit" << endl; }

	// StateA state
	void StateA::entry() { cout << "StateA::entry" << endl; }
	void StateA::exit() { cout << "StateA::exit" << endl; }
	void StateA::event1(int i) {
		box().myData = i;
		cout << "StateA::box().myData: " << box().myData << endl;
		StateB::setState(machine());
	}

	// StateB state
	void StateB::entry() { cout << "StateB::entry" << endl; }
	void StateB::exit() { cout << "StateB::exit" << endl; }
	void StateB::event2(long l) {
		Top::box().myData = l;
		cout << "Top::box().myData: " << Top::box().myData << endl;
		StateA::setState(machine());
	}

} // namespace Example


//////////////////////////////////////////////////////////////////////
// Test run
int main() {
	// Initialize state machine with some data
	Macho::Machine<Example::Top> m(new Example::Top::Box(11));

	// Dispatch some events
	m->event1(42);
	m->event2(43);

	// Inspect state machine
	cout << "m.box().myData: " << m.box().myData << endl;

	return 0;
}

/*
   Output is:

   Top::entry
   Super::entry
   StateA::entry
   StateA::box().myData: 42
   StateA::exit
   StateB::entry
   Top::box().myData: 43
   StateB::exit
   StateA::entry
   m.box().myData: 43
   StateA::exit
   Super::exit
   Top::exit
*/
