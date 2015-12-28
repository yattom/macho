// Macho - C++ Machine Objects
//
// The Machine Objects class library (in short Macho) allows the creation of
// state machines based on the "State" design pattern in straight C++. It
// extends the pattern with the option to create hierarchical state machines,
// making it possible to convert the popular UML statechart notation to working
// code in a straightforward way. Other features are entry and exit actions,
// state histories and state variables.
//
// Copyright (c) 2005 by Eduard Hiti (feedback to macho@ehiti.de)
//
// Version 0.9.7 (released 2007-12-1)
//
// See Macho.hpp for more information.

#include "Macho.hpp"
using namespace Macho;

////////////////////////////////////////////////////////////////////////////////
// Helper functions for tracing.
#ifdef MACHO_TRACE
#	include <iostream>

	void MACHO_TRC1(const char * msg) { std::cout << msg << std::endl; }
	void MACHO_TRC2(const char * state, const char * msg)
	{ std::cout << "State " << state << ": " << msg << std::endl; }
	void MACHO_TRC3(const char * state, const char * msg1, const char * msg2)
	{ std::cout << "State " << state << ": " << msg1 << " " << msg2 << std::endl; }
#else
#	define MACHO_TRC1(MSG)
#	define MACHO_TRC2(STATE, MSG)
#	define MACHO_TRC3(STATE, MSG1, MSG2)
#endif

////////////////////////////////////////////////////////////////////////////////
// Implementation for State
_StateInfo & _StateBase::_getInfo(_MachineBase & machine) {
	// Look first in machine for existing StateInfo.
	_StateInfo * & info = machine.getInfo(key());
	if (!info)
		info = new _RootStateInfo(machine, 0);

	return *info;
}

void _StateBase::_shutdown() {
	_myStateInfo.machine().shutdown();
}

void _StateBase::_restore(_StateInfo & current) {
	_myStateInfo.machine().myCurrentState = &current;
}

////////////////////////////////////////////////////////////////////////////////
// StateInfo implementation
_StateInfo::_StateInfo(_MachineBase & machine, _StateInfo * parent)
	: myMachine(machine)
	, myInstance(0)
	, myHistory(0)
	, myParent(parent)
	, myBox(0)
	, myBoxPlace(0)
{}

_StateInfo::~_StateInfo() {
	if (myBoxPlace)
		::operator delete(myBoxPlace);

	delete myInstance;
}

void _StateInfo::entry(_StateInfo & previous, bool first) {
	// Only Root has no parent
	if (!myParent)
		return;

	// first entry or previous state is not substate -> perform entry
	if (first || !previous.isChild(*this)) {
		myParent->entry(previous, false);

		// Could be set from outside or persistent (or EmptyBox)
		if (!myBox)
			createBox();

		MACHO_TRC2(name(), "Entry");
		myInstance->entry();
	}
}

void _StateInfo::exit(_StateInfo & next) {
	// Only Root has no parent
	if (!myParent)
		return;

	// self transition or next state is not substate -> perform exit
	if (this == &next || !next.isChild(*this)) {
		MACHO_TRC2(name(), "Exit");
		myInstance->exit();

		myInstance->_deleteBox(*this);

		myParent->exit(next);
	}
}

void _StateInfo::init(bool history) {
	if (history && myHistory) {
		MACHO_TRC3(name(), "History transition to", myHistory->name());
		myMachine.setPendingState(*myHistory, true, 0);
	} else {
		MACHO_TRC2(name(), "Init");
		myInstance->init();
	}

	myHistory = 0;
}

#ifdef MACHO_SNAPSHOTS
void _StateInfo::copy(_StateInfo & original) {
	if (original.myHistory) {
		_StateInfo * history = myMachine.getInfo(original.myHistory->key());
		assert(history);
		setHistory(history);
	}

	if (original.myBox)
		cloneBox(original.myBox);
}

_StateInfo * _StateInfo::clone(_MachineBase & newMachine) {
	assert(!newMachine.getInfo(key()));

	_StateInfo * parent = 0;
	if (myParent)
		// Tell other machine to clone parent first.
		parent = newMachine.createClone(myParent->key(), myParent);

	_StateInfo * clone = create(newMachine, parent);
	return clone;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Base class for Machine objects.
_MachineBase::_MachineBase()
	: myCurrentState(0)
	, myPendingState(0)
	, myPendingBox(0)
	, myPendingHistory(false)
	, myPendingEvent(0)
{}

_MachineBase::~_MachineBase() {
	assert(!myPendingBox);

	delete[] myStates;
	delete myPendingEvent;
}

void _MachineBase::setState(_StateInfo & info, bool history, void * box) {
	setPendingState(info, history, box);
	rattleOn();
}

void _MachineBase::setState(const StateAlias & state, bool history) {
	setPendingState(state.getInfo(*this), history, state.takeBox());
	rattleOn();
}

void _MachineBase::start(_StateInfo & info, void * box) {
	MACHO_TRC1("Starting Machine");

	// Start with Root state
	myCurrentState = &_StateBase::_getInfo(*this);
	// Then go to state
	setState(info, false, box);
}

void _MachineBase::shutdown() {
	assert(!myPendingState);

	MACHO_TRC1("Shutting down Machine");

	// Performs exit actions by going to Root (=State) state.
	setState(_StateBase::_getInfo(*this), false, 0);

	myCurrentState = 0;
}

void _MachineBase::allocate(unsigned int count) {
	myStates = new _StateInfo *[count];
	for (unsigned int i = 0; i < count; ++i)
		myStates[i] = 0;
}

void _MachineBase::free(unsigned int count) {
	// Free from end of list, so that child states are freed first
	unsigned int i = count;
	while (i > 0) {
		--i;
		delete myStates[i];
		myStates[i] = 0;
	}
}

#ifdef MACHO_SNAPSHOTS
void _MachineBase::copy(_StateInfo ** other, unsigned int count) {
	Key i;

	// Create StateInfo objects
	for (i = 0; i < count; ++i)
		createClone(i, other[i]);

	// Copy StateInfo object's state
	for (i = 0; i < count; ++i) {
		_StateInfo * state = myStates[i];
		if (state) {
			assert(other[i]);
			state->copy(*other[i]);
		}
	}
}

_StateInfo * _MachineBase::createClone(Key key, _StateInfo * original) {
	_StateInfo * & clone = getInfo(key);

	// Object already created?
	if (!clone && original)
		clone = original->clone(*this);

	return clone;
}
#endif

void _MachineBase::rattleOn() {
	assert(myCurrentState);

	while (myPendingState || myPendingEvent) {
		// Loop here because init actions might change state again.
		while (myPendingState) {
			MACHO_TRC3(myCurrentState->name(), "Transition to", myPendingState->name());

#ifndef NDEBUG
			// Entry/Exit actions may not dispatch events: set dummy event.
			if (!myPendingEvent)
				myPendingEvent = (_IEventBase *) &myPendingEvent;
#endif

			// Perform exit actions (which exactly depends on new state).
			myCurrentState->exit(*myPendingState);

			// Store history information for previous state now.
			// Previous state will be used for deep history.
			myCurrentState->setHistorySuper(*myCurrentState);

			_StateInfo * previous = myCurrentState;
			myCurrentState = myPendingState;

			if (myPendingBox)
				myCurrentState->setBox(myPendingBox);

			// Perform entry actions (which exactly depends on previous state).
			myCurrentState->entry(*previous);

			// State transition complete.
			// Clear 'pending' information just now so that setState would assert in exits and entries, but not in init.
			myPendingState = 0;
			myPendingBox = 0;
			bool history = myPendingHistory;
			myPendingHistory = false;

			// "init" may change state again.
			myCurrentState->init(history);

			assert("Init may only transition to proper substates" &&
			       (!myPendingState ||
			        (myPendingState->isChild(*myCurrentState) && (myCurrentState != myPendingState)))
			);

#ifndef NDEBUG
			// Clear dummy event if need be
			if (myPendingEvent == (_IEventBase *) &myPendingEvent)
				myPendingEvent = 0;
#endif
		} // while (myPendingState)

		if (myPendingEvent) {
			_IEventBase * event = myPendingEvent;
			myPendingEvent = 0;
			event->dispatch(*myCurrentState);
			delete event;
		}
	} // while (myPendingState || myPendingEvent)
}
