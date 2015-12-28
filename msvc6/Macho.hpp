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
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// You are encouraged to provide any changes, extensions and corrections for
// this software to the author at the above-mentioned email address for
// inclusion into future versions.
//
//
// Description:
//
// States are represented as C++ classes. The hierarchy of states follows the
// inheritance relation between these state classes. A set of state classes for
// a single state machine derives directly or indirectly from a top state class,
// making it the composite state holding all other states. Events are processed
// by calling virtual methods of the top state class. Substates redefine the
// behaviour of these event handler methods.
//
// Special methods "entry", "exit" and "init" are called on state entry, state
// exit and state initialization of super- and substates (in the order defined
// by statechart semantics and current machine state).
//
// An object of type "Machine" maintains the current state of a state machine
// and dispatches events to it. The "Machine" type is a template class
// parametrized with the top state class of the state machine to be run.
//
// State data is not kept in state classes (because state class instances are
// created just once and then reused, whereas state data should be instantiated
// or destroyed each time its state is entered or left). State data is put in
// "Box" types specific to each state class instead, which are managed by the
// Machine object. States may be parametrized with preinitialized boxes on
// Machine object creation or state transition. Boxes are retrieved by calling
// the "box" method. Superstate boxes are accessible by qualifiying the "box"
// method with the state class name (e.g. TOP::box()).
//
// A history of entered substates can be kept for superstates. Upon direct
// transition into the superstate the previous substate is then reentered. History
// can be shallow (only direct substates) or deep (any substate).
//
//
// Example:
//
// #include "Macho.hpp"
// #include <iostream>
// using namespace std;
//
// namespace Example {
//	TOPSTATE(Top) {
//		struct Box {
//			Box() : myData(0) {}
//			Box(long l) : myData(l) {}
//			long myData;
//		};
//
//		STATE(Top)
//
//		virtual void event1() {}
//		virtual void event2() {}
//
//	private:
//		void entry();
//		void exit();
//		void init();
//	};
//
//	SUBSTATE(Super, Top) {
//		STATE(Super)
//		HISTORY()
//
//	private:
//		void entry();
//		void exit();
//	};
//
//	SUBSTATE(StateA, Super) {
//		struct Box {
//			Box() : myData(0) {}
//			Box(int i) : myData(i) {}
//			int myData;
//		};
//
//		STATE(StateA)
//
//		void event1();
//
//	 private:
//		void entry();
//		void exit();
//
//	};
//
//	SUBSTATE(StateB, Super) {
//		STATE(StateB)
//
//		void event2();
//
//	private:
//		void entry();
//		void exit();
//	};
//
//	void Top::entry() { cout << "Top::entry" << endl; }
//	void Top::exit() { cout << "Top::exit" << endl; }
//	void Top::init() { setState<StateA>(new StateA::Box(42)); }
//
//	void Super::entry() { cout << "Super::entry" << endl; }
//	void Super::exit() { cout << "Super::exit" << endl; }
//
//	void StateA::entry() { cout << "StateA::entry" << endl; }
//	void StateA::exit() { cout << "StateA::exit" << endl; }
//	void StateA::event1() { setState<StateB>(); }
//
//	void StateB::entry() { cout << "StateB::entry" << endl; }
//	void StateB::exit() { cout << "StateB::exit" << endl; }
//	void StateB::event2() { setState<StateA>(); }
// }
//
// int main() {
//	Macho::Machine<Example::Top> m(new Example::Top::Box(11));
//	m->event1();
//	m->event2();
//
//	return 0;
// }
//
// Output is:
//
// Top::entry
// Super::entry
// StateA::entry
// StateA::exit
// StateB::entry
// StateB::exit
// StateA::entry
// StateA::exit
// Super::exit
// Top::exit
//
//
// Version History:
// 
//	  0.9.7 (released 2007-12-1):
//		 - Introduction of template states
//		 - fixed rare memory leak
// 
//	  0.9.6 (released 2007-09-01):
//		 - Changes to state transition semantics (see file "changes_0_9_6.txt")
//		 - New mechanism for state initialization
//		 - Runtime reflection on state relationships now possible
// 
//	  0.9.5 (released 2007-05-01):
//		 - Introduction of parametrized state transitions
// 
//	  0.9.4 (released 2006-06-01):
//		 - Snapshot functionality added
// 
//	  0.9.3 (released 2006-04-20):
//		 - Code reorganization (file Macho.cpp added)
// 
//	  0.9.2 (released 2006-04-10):
//		 - Memory leak plugged
//		 - MSVC6 version updated
// 
//	  0.9.1 (released 2006-03-30):
//		 - Introduction of persistent boxes
//		 - Speed and size optimizations
//		 - Machine instance can be accessed in event handlers with method "machine"
// 
//	  0.9 (released 2006-01-15):
//		 - Introduction of queuable event type
// 
//	  0.8.2 (released 2005-12-15):
//		 - Code size reduction by minimizing use of template classes
// 
//	  0.8.1 (released 2005-12-01):
//		 - Added MSVC6 variant (see directory "msvc6")
//		 - Added method "clearHistoryDeep"
// 
//	  0.8 (released 2005-11-01):
//		 - Initial release
// 

#ifndef __MACHO_HPP__
#define __MACHO_HPP__

// Snapshots imply copyable boxes
#ifdef MACHO_SNAPSHOTS
#  ifndef MACHO_BOX_COPY
#	 define MACHO_BOX_COPY
#  endif
#endif

#include <new>
#include <cassert>

#define TYPENAME

////////////////////////////////////////////////////////////////////////////////
// Various macros for state and history declaration

// Use this macro to define your top state class.
#define TOPSTATE(TOP) \
  struct TOP : public Macho::Link< TOP, Macho::TopState< TOP > >

// Use this macro for all your other state classes.
#define SUBSTATE(STATE, SUPERSTATE) \
  struct STATE : public Macho::Link< STATE, SUPERSTATE >

// Use this macro in your class definition to give it state functionality
// (mandatory). If you have a state box declare it BEFORE macro invocation!
#define STATE(S) \
public: \
	/* Constructor and destructor already defined: you can't (and shouldn't) have your own! */ \
	/* For the user a state class "constructor" and "destructor" are its entry and exit method! */ \
	S(Macho::_StateInfo & info) : Macho::Link<S, SUPER>(info) { \
		/* Compile time check: S must derive directly from Link<S, SUPER> */ \
		Macho::Link<S, SUPER> * p = 0; \
		_LINK * p2 = 0; \
		p = p2; \
		p2 = p; \
	} \
	~S() {} \
	static const char * _state_name() { return #S; } \
	/* Get to your Box with this method: */ \
	Box & box() { return *static_cast<Box *>(_box()); } \

// Use this macro to select deep history strategy.
#define DEEPHISTORY() \
private: \
	/* If no superstate has history, SUPER::_setHistorySuper is a NOOP */ \
	void _saveHistory(Macho::_StateInfo & self, Macho::_StateInfo & shallow, Macho::_StateInfo & deep) \
	{ self.setHistory(&deep); SUPER::_setHistorySuper(self, deep); } \
protected: \
	/* Substates may use _setHistorySuper to bubble up history */ \
	void _setHistorySuper(Macho::_StateInfo & self, Macho::_StateInfo & deep) \
	{ self.setHistorySuper(deep); } \
public:

// Use this macro to select shallow history strategy.
#define HISTORY() \
private: \
	/* If no superstate has history, SUPER::_setHistorySuper is a NOOP */ \
	void _saveHistory(Macho::_StateInfo & self, Macho::_StateInfo & shallow, Macho::_StateInfo & deep) \
	{ self.setHistory(&shallow); SUPER::_setHistorySuper(self, deep); } \
protected: \
	/* Substates may use _setHistorySuper to bubble up history */ \
	void _setHistorySuper(Macho::_StateInfo & self, Macho::_StateInfo & deep) \
	{ self.setHistorySuper(deep); } \
public:

// Use this macro to have boxes survive state transitions
#define PERSISTENT() \
private: \
	void _deleteBox(Macho::_StateInfo & info) {} \
public:

////////////////////////////////////////////////////////////////////////////////
// Everything else is put into namespace 'Macho'.
// Some identifiers are prefixed with an underscore to prevent name clashes with
// deriving classes or to mark things as library internal. Don't touch things
// with an underscore prefix!
namespace Macho {

	typedef unsigned int Key;

	class _MachineBase;

	template<class T>
	class Machine;

	template<class T>
	class IEvent;

	class _StateInfo;

	template<class T>
	class TopState;

	template<class S>
	class State;

	////////////////////////////////////////////////////////////////////////////////
	// Box for states which don't declare own Box class.
	class _EmptyBox {
	public:
		_EmptyBox() {}
	};

	////////////////////////////////////////////////////////////////////////////////
	// Each state machine has its own key generator. The generator returns unique
	// identifiers. The identifiers are consecutive integers starting from zero,
	// which allows use as index into a vector for fast access.
	// Root always has zero as key.
	template<class TOP>
	struct _KeyInit {
		_KeyInit() : key(Machine<TOP>::theStateCount++) {}
		Key key;
	};

	////////////////////////////////////////////////////////////////////////////////
	// Base class for all state classes.
	// Also serves as 'Root' state. By entering this state we trigger entry
	// and exit actions of user's top state.
	class _StateBase {
	public:
		virtual ~_StateBase() {}

		// Get unique name of state.
		static Key key() {
			return 0;
		}

	protected:
		_StateBase(_StateInfo & info)
			: _myStateInfo(info)
		{}

		// Restore from snapshot event: set current state.
		// Default implementation: Does not trigger entry actions!
		virtual void _restore(_StateInfo & current);

		// Shutdown machine event: exit all states.
		// Default implementation: Triggers exit actions!
		// Override empty to omit calling exit actions.
		virtual void _shutdown();

		// This is the method to bubble up history information
		// for states whose superstates have no history (so does nothing).
		virtual void _setHistorySuper(_StateInfo & self, _StateInfo & deep) {}

	protected:
		// State exit. Not allowed to initiate state change.
		virtual void exit() {}

		// State entry. Not allowed to initiate state change.
		virtual void entry() {}

		// Special kind of state entry: Upon transition to a new state,
		// entry methods of that state and its superstates are called;
		// 'init' however is called only on the one state the transition
		// actually goes to.
		// Is allowed to change state.
		virtual void init() {}

	protected:
		// C++ needs something like package visibility

		friend class _StateInfo;
		friend class _RootStateInfo;

		friend class _MachineBase;

		// Create StateInfo object of state.
		static _StateInfo & _getInfo(_MachineBase & machine);

		virtual void _deleteBox(_StateInfo & info) {}

		// Default history strategy (no history).
		virtual void _saveHistory(_StateInfo & self, _StateInfo & shallow, _StateInfo & deep) {}

	protected:
		_StateInfo & _myStateInfo;
	};

	////////////////////////////////////////////////////////////////////////////////
	// Base class for user defined top state (and indirectly all other states).
	template<class T>
	class TopState : public _StateBase {
	public:
		// This typedef is an alias for user defined top state in all (sub)states.
		typedef T TOP;

	protected:
		TopState(_StateInfo & info)
			: _StateBase(info)
		{}

		void dispatch(IEvent<TOP> * event);

		// Returns current state machine instance.
		Machine<TOP> & machine() {
			 return *static_cast<Machine<TOP> *>(&this->_myStateInfo.machine());
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	// This class links substates to superstates by deriving from the superstate
	// and being derived from by the substate.
	// Substates inherit event handlers from superstates for reuse or redefinition
	// this way.
	template<class C, class P>
	class Link : public P {
	public:
		// Alias for superstate.
		typedef P SUPER;

		// Alias for topstate.
		typedef TYPENAME P::TOP TOP;

		// Default box type.
		typedef _EmptyBox Box;

		// Get unique key of state.
		static Key key() {
			return _theKey.key;
		}

		// Is machine m in this state?
		static bool isCurrent(Machine<TOP> & m) {
			return m.isCurrent(_getInfo(m));
		}

		// Is machine m in exactly this state?
		static bool isCurrentDirect(Machine<TOP> & m) {
			return m.isCurrentDirect(_getInfo(m));
		}

		static void clearHistory(Machine<TOP> & m) { _getInfo(m).setHistory(0); }
		static void clearHistoryDeep(Machine<TOP> & m) { m.clearHistoryDeep(_getInfo(m)); }

		static void setState(Machine<TOP> & machine, void * box = 0) {
			_StateInfo & info = _getInfo(machine);
			machine.setPendingState(info, true, box);
		}

		static void setStateDirect(Machine<TOP> & machine, void * box = 0) {
			_StateInfo & info = _getInfo(machine);
			machine.setPendingState(info, false, box);
		}

		// Initiate transition to a new state.
		// Parameter state is the new state to enter.
		// See above and class State for more information.
		void setState(const class StateAlias & state) {
			_StateInfo & info = state.getInfo(machine());
			machine().setPendingState(info, true, state.takeBox());
		}

		void setStateDirect(const class StateAlias & state) {
			_StateInfo & info = state.getInfo(machine());
			machine().setPendingState(info, true, state._takeBox());
		}

		// to be used with _restore
		void setState(_StateInfo & current, void * box = 0) {
			this->_myStateInfo.machine().setPendingState(current, true, box);
		}

	protected:
		// Needed to perform compile time checks.
		typedef Link<C, P> _LINK;

		Link(_StateInfo & info);

		// These definitions seem redundant but they are not!
		// They override parent definitions so each substate gets either
		// this default or their own, but never its parents definitions.
		virtual void entry() {}
		virtual void init() {}
		virtual void exit() {}

		// This method keeps '_myStateInfo' attribute private.
		void * _box();

	protected:
		// for _getInfo
		friend class _StateBase;

		// for _getInfo
		friend class Machine<TOP>;

		friend class State<C>;

		// Create StateInfo object of state.
		static _StateInfo & _getInfo(_MachineBase & machine);

		// Box is by default not persistent. Not redundant!
		void _deleteBox(_StateInfo & info);

		// Default history strategy (no history). Not redundant!
		void _saveHistory(_StateInfo & self, _StateInfo & shallow, _StateInfo & deep) {
			// Bubble up history. If no superstate has history, _setHistorySuper will do nothing.
			this->_setHistorySuper(self, deep);
		}

	protected:
		_StateInfo & _myStateInfo;

		static _KeyInit<TOP> _theKey;
	};

	template<class C, class P> _KeyInit<TYPENAME P::TOP> Link<C, P>::_theKey;

	////////////////////////////////////////////////////////////////////////////////
	// StateInfo describes a state. Keeps history, box and state object for state.
	// StateInfo object is created the first time state is entered.
	// There is at most one StateInfo object per state per machine instance.
	class _StateInfo {
	public:
		_StateInfo(_MachineBase & machine, _StateInfo * parent);

		virtual ~_StateInfo();

		// Perform entry actions.
		// 'first' is true on very first call.
		void entry(_StateInfo & previous, bool first = true);

		// Perform exit actions.
		void exit(_StateInfo & next);

		// Perform init action.
		void init(bool history);

		void saveHistory(_StateInfo & shallow, _StateInfo & deep) {
			// Check state's history strategy.
			myInstance->_saveHistory(*this, shallow, deep);
		}

		// Update superstate's history information:
		void setHistorySuper(_StateInfo & deep) {
			if (myParent)
				// Let it choose between deep or shallow history.
				myParent->saveHistory(*this, deep);
		}

		// Box has been created explicitly.
		void setBox(void * box) {
			assert(!myBox);

			if (myBoxPlace) {
				// Free cached memory of previously used box.
				::operator delete(myBoxPlace);
				myBoxPlace = 0;
			}

			myBox = box;
		}

#ifdef MACHO_SNAPSHOTS
		// Copy state of another StateInfo object.
		void copy(_StateInfo & original);

		// Create a clone of StateInfo object for another machine.
		_StateInfo * clone(_MachineBase & newMachine);

		virtual void cloneBox(void * box) = 0;
#endif

		void shutdown() {
			myInstance->_shutdown();
		}

		void restore(_StateInfo & info) {
			myInstance->_restore(info);
		}

		virtual Key key() = 0;

		// 'Virtual constructor' needed for cloning.
		virtual _StateInfo * create(_MachineBase & machine, _StateInfo * parent) = 0;

		virtual void createBox() = 0;
		virtual void deleteBox() = 0;

		virtual const char * name() const = 0;

		// Is 'state' a superstate?
		bool isChild(_StateInfo & state) const {
			return this == &state || (myParent && myParent->isChild(state));
		}

		_StateBase & instance() {
			assert(myInstance);
			return *myInstance;
		}

		void * box() {
			assert(myBox);
			return myBox;
		}

		_MachineBase & machine() {
			return myMachine;
		}

		void setHistory(_StateInfo * history) {
			myHistory = history;
		}

	protected:
		_MachineBase & myMachine;
		_StateBase * myInstance;   // Instance of state class
		_StateInfo * myHistory;
		_StateInfo * myParent;
		void * myBox;
		void * myBoxPlace;	// Reused box memory
	};

	////////////////////////////////////////////////////////////////////////////////
	// StateInfo for Root state.
	class _RootStateInfo : public _StateInfo {
	public:
		_RootStateInfo(_MachineBase & machine, _StateInfo * parent)
			: _StateInfo(machine, parent)
		{
			myInstance = new _StateBase(*this);
		}

		virtual Key key() {
			return 0;
		}

		virtual void createBox() {}
		virtual void deleteBox() {}

#ifdef MACHO_SNAPSHOTS
		virtual void cloneBox(void * box) {}
#endif

		virtual const char * name() const { return "Root"; }

		// 'Virtual constructor' needed for cloning.
		virtual _StateInfo * create(_MachineBase & machine, _StateInfo * parent) {
			return new _RootStateInfo(machine, parent);
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	// StateInfo for substates (including Top ;-)
	// Has methods to create state specific objects.
	template<class S>
	class _SubStateInfo : public _StateInfo {
	public:
		typedef TYPENAME S::Box Box;

		_SubStateInfo(_MachineBase & machine, _StateInfo * parent)
			: _StateInfo(machine, parent)
		{
			assert(parent);
			this->myInstance = new S(*this);
		}

		~_SubStateInfo() {
			if (this->myBox)
				deleteBox();
		}

		virtual const char * name() const { return S::_state_name(); }

		virtual Key key() {
			return S::key();
		}

		// 'Virtual constructor' needed for cloning.
		virtual _StateInfo * create(_MachineBase & machine, _StateInfo * parent) {
			return new _SubStateInfo<S>(machine, parent);
		}

#ifdef MACHO_SNAPSHOTS
		virtual void cloneBox(void * box) {
			assert(!myBox);
			assert(!myBoxPlace);
			// Needs copy constructor in ALL box types.
			myBox = new Box(*static_cast<Box *>(box));
		}
#endif

		virtual void createBox() {
			this->myBox = new Box();
		}

		virtual void deleteBox() {
			delete static_cast<Box *>(this->myBox);
			myBox = 0;
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	// Definitions for queuable event types

	// Generic interface for event objects (available only to MachineBase)
	class _IEventBase {
	public:
		virtual ~_IEventBase() {}
		virtual void dispatch(_StateInfo &) = 0;
	};

	// Interface for event objects (bound to a top state)
	template<class TOP>
	class IEvent : protected _IEventBase {
		friend class Machine<TOP>;
		friend class TopState<TOP>;
	};

	// Event with four parameters
	template<class TOP, class R, class P1, class P2, class P3, class P4>
	class _Event4 : public IEvent<TOP> {
		typedef R (TOP::*Signature)(P1, P2, P3, P4);

	public:
		_Event4(Signature handler, P1 p1, P2 p2, P3 p3, P4 p4)
			: myHandler(handler)
			, myParam1(p1)
			, myParam2(p2)
			, myParam3(p3)
			, myParam4(p4)
		{
			assert(handler);
		}

	protected:
		void dispatch(_StateInfo & info) {
			TOP & state = static_cast<TOP &>(info.instance());
			(state.*myHandler)(myParam1, myParam2, myParam3, myParam4);
		}

		Signature myHandler;
		P1 myParam1;
		P2 myParam2;
		P3 myParam3;
		P4 myParam4;
	};

	// Event with three parameters
	template<class TOP, class R, class P1, class P2, class P3>
	class _Event3 : public IEvent<TOP> {
		typedef R (TOP::*Signature)(P1, P2, P3);

	public:
		_Event3(Signature handler, P1 p1, P2 p2, P3 p3)
			: myHandler(handler)
			, myParam1(p1)
			, myParam2(p2)
			, myParam3(p3)
		{
			assert(handler);
		}

	protected:
		void dispatch(_StateInfo & info) {
			TOP & state = static_cast<TOP &>(info.instance());
			(state.*myHandler)(myParam1, myParam2, myParam3);
		}

		Signature myHandler;
		P1 myParam1;
		P2 myParam2;
		P3 myParam3;
	};

	// Event with two parameters
	template<class TOP, class R, class P1, class P2>
	class _Event2 : public IEvent<TOP> {
		typedef R (TOP::*Signature)(P1, P2);

	public:
		_Event2(R (TOP::*handler)(P1, P2), P1 p1, P2 p2)
			: myHandler(handler)
			, myParam1(p1)
			, myParam2(p2)
		{
			assert(handler);
		}

	protected:
		void dispatch(_StateInfo & info) {
			TOP & state = static_cast<TOP &>(info.instance());
			(state.*myHandler)(myParam1, myParam2);
		}

		Signature myHandler;
		P1 myParam1;
		P2 myParam2;
	};

	// Event with one parameter
	template<class TOP, class R, class P1>
	class _Event1 : public IEvent<TOP> {
		typedef R (TOP::*Signature)(P1);

	public:
		_Event1(R (TOP::*handler)(P1), P1 p1)
			: myHandler(handler)
			, myParam1(p1)
		{
			assert(handler);
		}

	protected:
		void dispatch(_StateInfo & info) {
			TOP & state = static_cast<TOP &>(info.instance());
			(state.*myHandler)(myParam1);
		}

		Signature myHandler;
		P1 myParam1;
	};

	// Event with no parameters
	template<class TOP, class R>
	class _Event0 : public IEvent<TOP> {
		typedef R (TOP::*Signature)();

	public:
		_Event0(R (TOP::*handler)())
			: myHandler(handler)
		{
			assert(handler);
		}

	protected:
		void dispatch(_StateInfo & info) {
			TOP & state = static_cast<TOP &>(info.instance());
			(state.*myHandler)();
		}

		Signature myHandler;
	};

	// Event creating functions using type inference
	template<class TOP, class R, class P1, class P2, class P3, class P4>
	inline IEvent<TOP> * Event(R (TOP::*handler)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) {
		return new _Event4<TOP, R, P1, P2, P3, P4>(handler, p1, p2, p3, p4);
	}

	template<class TOP, class R, class P1, class P2, class P3>
	inline IEvent<TOP> * Event(R (TOP::*handler)(P1, P2, P3), P1 p1, P2 p2, P3 p3) {
		return new _Event3<TOP, R, P1, P2, P3>(handler, p1, p2, p3);
	}

	template<class TOP, class R, class P1, class P2>
	inline IEvent<TOP> * Event(R (TOP::*handler)(P1, P2), P1 p1, P2 p2) {
		return new _Event2<TOP, R, P1, P2>(handler, p1, p2);
	}

	template<class TOP, class R, class P1>
	inline IEvent<TOP> * Event(R (TOP::*handler)(P1), P1 p1) {
		return new _Event1<TOP, R, P1>(handler, p1);
	}

	template<class TOP, class R>
	inline IEvent<TOP> * Event(R (TOP::*handler)()) {
		return new _Event0<TOP, R>(handler);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Base class for Machine objects.
	class _MachineBase {
	public:
		// Transition to new state (with optional preinitialized box).
		void setState(_StateInfo & info, bool history, void * box);

		// Transition to new state specified by state alias.
		void setState(const StateAlias & state, bool history);

		// Prepare transition to new state (performed on call to "rattleOn").
		// There can be only one state transition pending (asserts otherwise)!
		// "box" is an optional preinitialized state box for the new state.
		void setPendingState(_StateInfo & info, bool history, void * box) {
			assert( (!myPendingState || myPendingState == &info) &&
			        "There is already a state transition pending!");

			myPendingState = &info;
			myPendingBox = box;
			myPendingHistory = history;
		}

		void setPendingEvent(_IEventBase * event) {
			assert(event);
			assert(!myPendingEvent && "There is already an event pending!");

			myPendingEvent = event;
		}

		// Performs pending state transition.
		void rattleOn();

		// Get StateInfo object for key.
		_StateInfo * & getInfo(Key name) {
			return myStates[name];
		}

		bool isCurrent(_StateInfo & info) {
			return myCurrentState->isChild(info);
		}

		bool isCurrentDirect(_StateInfo & info) {
			return myCurrentState == &info;
		}

	protected:
		_MachineBase();

		~_MachineBase();

		// Starts the machine. Will make it go into top state.
		// Optional parameter "box" is a preinitialized state box for the top
		// state.
		void start(_StateInfo & info, void * box);

		// Shuts machine down. Will exit any states and free all allocated
		// resources.
		void shutdown();

		// Allocate space for pointers to StateInfo objects.
		void allocate(unsigned int count);

		// Free all StateInfo objects.
		void free(unsigned int count);

#ifdef MACHO_SNAPSHOTS
		// Create a copy of another machines StateInfo objects (includes boxes).
		void copy(_StateInfo ** other, unsigned int count);

		// Create a copy of another machines StateInfo object.
		_StateInfo * createClone(Key key, _StateInfo * original);
#endif

	protected:
		// C++ needs something like package visibility

		// for setPendingState
		friend class _StateInfo;

		friend class _StateBase;

		// Current state of Machine object.
		_StateInfo * myCurrentState;

		// Information about pending state transition.
		_StateInfo * myPendingState;
		void * myPendingBox;
		bool myPendingHistory;

		_IEventBase * myPendingEvent;

		// Array of StateInfo objects.
		_StateInfo ** myStates;
	};

	////////////////////////////////////////////////////////////////////////////////
	// Implementation for TopState
	template<class T>
	inline void TopState<T>::dispatch(IEvent<TOP> * event) {
		assert(event);
		_myStateInfo.machine().setPendingEvent(event);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Implementation for Link
	template<class C, class P>
	inline Link<C, P>::Link(_StateInfo & info)
		: P(P::_getInfo(info.machine()))
		// Can't initialize _myStateInfo with _getInfo,
		// because this would result in an endless loop (at least for first call)
		, _myStateInfo(info)
	{}

	// Create StateInfo object of state.
	template<class C, class P>
	inline _StateInfo & Link<C, P>::_getInfo(_MachineBase & machine) {
		// Look first in machine for existing StateInfo.
		_StateInfo * & info = machine.getInfo(key());
		if (!info)
			// Will create parent StateInfo object if not already created.
			info = new _SubStateInfo<C>(machine, &P::_getInfo(machine));

		return *info;
	}

	// This method keeps '_myStateInfo' attribute private.
	template<class C, class P>
	inline void * Link<C, P>::_box() {
		return _myStateInfo.box();
	}

	// Default behaviour: free box on exit.
	template<class C, class P>
	inline void Link<C, P>::_deleteBox(_StateInfo & info) {
		info.deleteBox();
	}

	////////////////////////////////////////////////////////////////////////////////
	// Helper classes to allow parametrization of state transitions with 'setState'
	// (also for starting machines).

	// Members of do-it-yourself virtual function table.
	typedef _StateInfo & (*_GetInfoFn)(_MachineBase & machine);
	typedef void (*_DestroyFn)(void * box);
	typedef void * (*_CloneFn)(void * box);

	// DIY virtual function table.
	struct _StateCharacter {
		_GetInfoFn getInfo;
		_DestroyFn destroy;
#ifdef MACHO_BOX_COPY
		_CloneFn clone;
#endif
	};

	// This is the base class for state aliases. A state alias represents a
	// state of a machine. A transition to that state can be initiated by
	// giving the alias object to the 'setState' method.
	//
	// State aliases are created by the template class 'State' further below.
	//
	// Objects of this types are copyable; however, on copying the copy
	// actually takes over the box instance of the original object.
	// This also happens when the alias object is used for a state transition:
	// the alias' box instance is used up by the reached state; the next time
	// the same alias is used for a state transition no box is provided
	// anymore!
	//
	// To keep the boxes of alias objects you need to clone the object with the
	// 'clone' method: for this to work however boxes need to be copyable (have
	// a copy constructor).
	// Define MACRO_BOX_COPY when compiling if this is the case!
	//
	// This class has no virtual functions but something similar in form of
	// pointers to static functions (static functions suffice since the
	// operations depend only on template parameters).	This crazy stuff is
	// done here because this way allocating memory on the heap when cloning
	// state aliases can be avoided, which we would have to do with ordinary
	// virtual methods. SPEED!!!
	class StateAlias {
	public:
		// Takes over box from other object!
		// If you do not want this use 'clone'!
		StateAlias(const StateAlias & other)
			: myBox(other.takeBox())
			, myCharacter(other.myCharacter)
		{}

		// Takes over box from other object!
		// If you do not want this use 'clone'!
		StateAlias & operator=(const StateAlias & other) {
			(myCharacter->destroy)(myBox);

			myCharacter = other.myCharacter;
			myBox = other.takeBox();

			return *this;
		}

		~StateAlias() {
			(myCharacter->destroy)(myBox);
		}

#ifdef MACHO_BOX_COPY
		// Clones object.
		// Will call copy constructor of box.
		StateAlias clone() {
			return StateAlias(myCharacter, myBox ? (myCharacter->clone)(myBox) : 0);
		}
#endif

	protected:
		// Protected constructor: User creates alias with the 'State' class below.
		StateAlias(_StateCharacter * character, void * box)
			: myBox(box)
			, myCharacter(character)
		{}

	public:
		_StateInfo & getInfo(_MachineBase & machine) const {
			return (myCharacter->getInfo)(machine);
		}

		// Hand over box object (to state instances or other state aliases).
		void * takeBox() const {
			void * box = myBox;
			myBox = 0;
			return box;
		}

	private:
		mutable void * myBox;

		// DIY virtual function table.
		_StateCharacter * myCharacter;
	};

	// Objects of this class represent states in user code.  They are used to
	// create aliases to be used as parameters to the setState method to
	// initiate transitions to the specified state.
	// If the object is created with a box instance for the state, that box
	// instance is used to initialize the state on transition:
	//
	// Example:
	//		StateAlias a = State<StateA>(new StateA::Box(42));
	//		...
	//		setState(a);
	template<class S>
	class State : public StateAlias {
	public:
		// Can be given a box to be used when transitioning to template
		// parameter state.
		explicit State(typename S::Box * box = 0)
			: StateAlias(&theCharacter, box)
		{}

	private:
		static _StateInfo & getInfo(_MachineBase & machine) {
			return S::_getInfo(machine);
		}

		static void destroy(void * box) {
			delete static_cast<typename S::Box *>(box);
		}

#ifdef MACHO_BOX_COPY
		static void * clone(void * box) {
			typedef typename S::Box Box;
			return new Box(* static_cast<Box *>(box));
		}
#endif

	private:
		static _StateCharacter theCharacter;
	};

	// DIY virtual function table of class 'State'.
	template<class S>
	_StateCharacter State<S>::theCharacter = {
		State<S>::getInfo,
		State<S>::destroy,
#ifdef MACHO_BOX_COPY
		State<S>::clone
#endif
	};

	////////////////////////////////////////////////////////////////////////////////
	// Snapshot of a machine object.
	// Saves the state of a machine object at a specific point in time to be restored
	// later (can be used to achieve something like backtracking).
	// Includes boxes of current state and persistent boxes.
	// Assign a snapshot to a machine (operator=) to restore state.
	// Note that no exit/entry actions of the overwritten machine state are performed!
	// Box destructors however are executed!
#ifdef MACHO_SNAPSHOTS
	template<class TOP>
	class Snapshot : public _MachineBase {
	public:
		Snapshot(const Machine<TOP> & machine);

		~Snapshot() {
			free(Machine<TOP>::theStateCount);
		}

	private:
		friend class Machine<TOP>;

		Snapshot(const Snapshot<TOP> & other);
		Snapshot & operator=(const Snapshot<TOP> & other);
	};
#endif

	////////////////////////////////////////////////////////////////////////////////
	// A Machine object maintains a current state.
	// The state can be any substate of template parameter TOP.
	// TOP is the Machine's top most and inital state. TOP must be defined by
	// the macro TOPSTATE. Event processing is done by calling methods (event
	// handlers) on the current state.
	// This is realized by defining an arrow ("->") operator on Machine,
	// forwarding to the interface of TOP.
	// Every possible event handler to be called must therefore appear in the
	// interface of TOP. Events are dispatched by using this operator on a
	// Machine object (e.g. "machine->event()").
	template<class TOP>
	class Machine : public _MachineBase {
	public:
		////////////////////////////////////////////////////////////////////////////////
		// This class performs an action in its destructor after an event
		// handler has finished. Comparable to an After Advice in AOP.
		struct AfterAdvice {
			AfterAdvice(Machine<TOP> & m) : myMachine(m) {}

			// Event handler has finished execution. Execute pending transitions now.
			~AfterAdvice() { myMachine.rattleOn(); }

			// this arrow operator finally dispatches to TOP interface.
			TOP * operator->() {
				return static_cast<TOP *>(& (myMachine.myCurrentState->instance()) );
			}

		private:
			Machine<TOP> & myMachine;
		};
		friend struct AfterAdvice;

		// Constructor takes an optional preinitialized Box parameter for its
		// initial (top) state.
		Machine(TYPENAME TOP::Box * box = 0) {
			// Compile time check: TOP must directly derive from TopState<TOP>
			TopState<TOP> * p = 0;
			TYPENAME TOP::SUPER * p2 = 0;
			p2 = p;

			allocate(theStateCount);
			start(TOP::_getInfo(*this), box);
		}

		// Initialize with a state alias object to have machine go to a state
		// other than TOP on startup.
		Machine(const StateAlias & state) {
			// Compile time check: TOP must directly derive from TopState<TOP>
			TopState<TOP> * p = 0;
			typename TOP::SUPER * p2 = 0;
			p2 = p;

			allocate(theStateCount);
			start(state.getInfo(*this), state.takeBox());
		}

#ifdef MACHO_SNAPSHOTS
		// Create machine from a snapshot.
		Machine(const Snapshot<TOP> & snapshot) {
			allocate(theStateCount);
			copy(snapshot.myStates, theStateCount);
		}

		// Overwrite current machine state by snapshot.
		Machine<TOP> & operator=(const Snapshot<TOP> & snapshot) {
			assert(!myPendingState);
			assert(!myPendingEvent);

			myCurrentState->shutdown();

			free(theStateCount);
			copy(snapshot.myStates, theStateCount);

			// Go to Root state first
			myCurrentState = getInfo(0);

			// Then set previous current state
			_StateInfo * current = getInfo(snapshot.myCurrentState->key());
			current->restore(*current);
			rattleOn();

			return *this;
		}
#endif

		~Machine() {
			myCurrentState->shutdown();
			free(theStateCount);
		}

		// Don't return pointer to interface right now: we need to know when the
		// event handler has finished; return an AfterAdvice object instead:
		// it allows us to perform actions after access.
		AfterAdvice operator->() {
			assert(myCurrentState);
			assert(!myPendingState);

			// We need to know when the event handler has finished.
			return AfterAdvice(*this);
		}

		// Dispatch an event object to machine.
		void dispatch(IEvent<TOP> * event, bool destroy = true) {
			assert(event);

			event->dispatch(*myCurrentState);
			if (destroy) delete event;
			rattleOn();
		}

		// Allow (const) access to top state's box (for state data extraction).
		const TYPENAME TOP::Box & box() const {
			assert(myCurrentState);
			return static_cast<TOP &>(myCurrentState->instance()).TOP::box();
		}

		// Clear history of state and children.
		void clearHistoryDeep(_StateInfo & state) {
			for (unsigned int i = 0; i < theStateCount; ++i) {
				_StateInfo * s = myStates[i];
				if (s && s->isChild(state))
					s->setHistory(0);
			}
		}

		// Next free identifier for StateInfo objects.
		static Key theStateCount;

	private:
		Machine(const Machine<TOP> & other);
		Machine<TOP> & operator=(const Machine<TOP> & other);

#ifdef MACHO_SNAPSHOTS
		friend class Snapshot<TOP>;
#endif
	};

	// Root is always there and has key 0, so start from 1
	template<class TOP> Key Machine<TOP>::theStateCount = 1;

	////////////////////////////////////////////////////////////////////////////////
	// Implementation for Snapshot
#ifdef MACHO_SNAPSHOTS
	template<class TOP>
	Snapshot<TOP>::Snapshot(const Machine<TOP> & machine) {
		assert(!machine.myPendingState);
		assert(!machine.myPendingEvent);
		assert(machine.myCurrentState);

		allocate(Machine<TOP>::theStateCount);
		copy(machine.myStates, Machine<TOP>::theStateCount);

		myCurrentState = getInfo(machine.myCurrentState->key());
	}
#endif

} // namespace Macho

#endif // __MACHO_HPP__
