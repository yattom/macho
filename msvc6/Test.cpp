// Comprehensive tests of state machine features.
//
// Compile like this:
// (don't forget defining the MACHO_SNAPSHOTS symbol)
// cl /EHsc -D MACHO_SNAPSHOTS Macho.cpp Test.cpp

#include "Macho.hpp"

#include <map>
#include <vector>
#include <iostream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// These tests will test for proper entry/exit, box and history handling
enum {
	STATE_TOP = 1 << 0,
	STATE_A = 1 << 1,
	STATE_AA = 1 << 2,
	STATE_AB = 1 << 3,
	STATE_AAA = 1 << 4,
	STATE_AAB = 1 << 5,
	STATE_ABA = 1 << 6,
	STATE_ABB = 1 << 7,
	STATE_B = 1 << 8,
	STATE_BA = 1 << 9,
	STATE_BB = 1 << 10,
	STATE_BAA = 1 << 11,
	STATE_BAB = 1 << 12,
	STATE_BBA = 1 << 13,
	STATE_BBB = 1 << 14,
	STATE_C = 1 << 15,
	STATE_CA = 1 << 16,
	STATE_CB = 1 << 17,
	STATE_CAA = 1 << 18,
	STATE_CAB = 1 << 19,
	STATE_CBA = 1 << 20,
	STATE_CBB = 1 << 21,
	STATE_X = 1 << 22
};

namespace Transitions {

	long boxes;

	void box_created(long state) {
		boxes |= state;
	}
	void box_destroyed(long state) {
		boxes &= ~state;
	}
	bool has_box(long state) {
		return (boxes & state) == state;
	}

	bool at(unsigned pos, const map<long, size_t> & m, long state) {
		map<long, size_t>::const_iterator it = m.find(state);
		if (it == m.end())
			return false;

		return it->second == pos;
	}

	void add(long state, map<long, size_t> & m) {
		size_t c = m.size();
		m[state] = c;
	}

	TOPSTATE(Top) {
		struct Box {
			Box() : data(0) { clear(); box_created(STATE_TOP); }
			~Box() { box_destroyed(STATE_TOP); }

			// Registering all exits and entries
			void entry(long state) { add(state, entries); }
			void exit(long state) { add(state, exits); }
			void init(long state) { add(state, inits); }

			void clear() const { entries.clear(); exits.clear(); inits.clear(); }

			mutable map<long, size_t> entries;
			mutable map<long, size_t> exits;
			mutable map<long, size_t> inits;

			mutable int data;
		};

		STATE(Top)

		virtual void event() {}

	private:
		void entry() { box().entry(STATE_TOP); }
		void exit() { box().exit(STATE_TOP); }
		void init() { box().init(STATE_TOP); }
	};

	SUBSTATE(StateA, Top) {
		struct Box {
			Box() : data(0) { box_created(STATE_A); }
			~Box() { box_destroyed(STATE_A); }
			int data;
		};

		STATE(StateA)

	private:
		// Testing proper box handling here
		void entry() { TOP::box().entry(STATE_A); box().data = 42; }
		void exit() { TOP::box().exit(STATE_A); assert(box().data == 42); }
		void init() { TOP::box().init(STATE_A); }
	};

	SUBSTATE(StateAA, StateA) {
		STATE(StateAA)

	private:
		// Testing proper box handling here
		void entry() { TOP::box().entry(STATE_AA); assert(StateA::box().data == 42); StateA::box().data = 43; }
		void exit() { TOP::box().exit(STATE_AA); assert(StateA::box().data == 43); StateA::box().data = 42; }
		void init() { TOP::box().init(STATE_AA); }
	};

	SUBSTATE(StateAAA, StateAA) {
		struct Box {
			Box() : data(0) {}
			~Box() {}
			int data;
		};
		PERSISTENT()
		STATE(StateAAA)

	private:
		void entry() { TOP::box().entry(STATE_AAA); assert(StateA::box().data == 43); }
		void exit() { TOP::box().exit(STATE_AAA); assert(StateA::box().data == 43); }
		void init() { TOP::box().init(STATE_AAA); }
	};

	SUBSTATE(StateAAB, StateAA) {
		struct Box {
			Box() { box_created(STATE_AAB); }
			~Box() { box_destroyed(STATE_AAB); }
		};

		STATE(StateAAB)

	private:
		void entry() { TOP::box().entry(STATE_AAB); assert(StateA::box().data == 43); }
		void exit() { TOP::box().exit(STATE_AAB); assert(StateA::box().data == 43); }
		void init() { TOP::box().init(STATE_AAB); }
	};

	SUBSTATE(StateAB, StateA) {
		STATE(StateAB)

	private:
		void entry() { TOP::box().entry(STATE_AB); assert(StateA::box().data == 42); }
		void exit() { TOP::box().exit(STATE_AB); assert(StateA::box().data == 42); }
		void init() { TOP::box().init(STATE_AB); }
	};

	SUBSTATE(StateABA, StateAB) {
		STATE(StateABA)

	private:
		void entry() { TOP::box().entry(STATE_ABA); assert(StateA::box().data == 42); }
		void exit() { TOP::box().exit(STATE_ABA); assert(StateA::box().data == 42); }
		void init() { TOP::box().init(STATE_ABA); }
	};

	SUBSTATE(StateABB, StateAB) {
		STATE(StateABB)

	private:
		void entry() { TOP::box().entry(STATE_ABB); assert(StateA::box().data == 42); }
		void exit() { TOP::box().exit(STATE_ABB); assert(StateA::box().data == 42); }
		void init() { TOP::box().init(STATE_ABB); }
	};

	SUBSTATE(StateB, Top) {
		STATE(StateB)
		HISTORY()

	private:
		void entry() { TOP::box().entry(STATE_B); }
		void exit() { TOP::box().exit(STATE_B); }
		void init() { TOP::box().init(STATE_B); }
	};

	SUBSTATE(StateBA, StateB) {
		STATE(StateBA)

	private:
		void entry() { TOP::box().entry(STATE_BA); }
		void exit() { TOP::box().exit(STATE_BA); }
		void init() { TOP::box().init(STATE_BA); }
	};

	SUBSTATE(StateBAA, StateBA) {
		STATE(StateBAA)

	private:
		void entry() { TOP::box().entry(STATE_BAA); }
		void exit() { TOP::box().exit(STATE_BAA); }
		void init() { TOP::box().init(STATE_BAA); }
	};

	SUBSTATE(StateBAB, StateBA) {
		STATE(StateBAB)

	private:
		void entry() { TOP::box().entry(STATE_BAB); }
		void exit() { TOP::box().exit(STATE_BAB); }
		void init() { TOP::box().init(STATE_BAB); }
	};

	SUBSTATE(StateBB, StateB) {
		STATE(StateBB)

	private:
		void entry() { TOP::box().entry(STATE_BB); }
		void exit() { TOP::box().exit(STATE_BB); }
		void init() { TOP::box().init(STATE_BB); }
	};

	SUBSTATE(StateBBA, StateBB) {
		STATE(StateBBA)

	private:
		void entry() { TOP::box().entry(STATE_BBA); }
		void exit() { TOP::box().exit(STATE_BBA); }
		void init() { TOP::box().init(STATE_BBA); }
	};

	SUBSTATE(StateBBB, StateBB) {
		STATE(StateBBB)

	private:
		void entry() { TOP::box().entry(STATE_BBB); }
		void exit() { TOP::box().exit(STATE_BBB); }
		void init() { TOP::box().init(STATE_BBB); }
	};

	SUBSTATE(StateC, Top) {
		STATE(StateC)
		DEEPHISTORY()

	private:
		void init();
		void entry() { TOP::box().entry(STATE_C); }
		void exit() { TOP::box().exit(STATE_C); }
	};

	SUBSTATE(StateCA, StateC) {
		STATE(StateCA)
		DEEPHISTORY()

	private:
		void init();
		void entry() { TOP::box().entry(STATE_CA); }
		void exit() { TOP::box().exit(STATE_CA); }
	};

	SUBSTATE(StateCAA, StateCA) {
		struct Box {
			Box(int i = 0) : data(i) { box_created(STATE_CAA); }
			~Box() { box_destroyed(STATE_CAA); }
			int data;
		};

		STATE(StateCAA)

	private:
		void entry() { TOP::box().entry(STATE_CAA); }
		void exit() { TOP::box().exit(STATE_CAA); }
		void init() { TOP::box().init(STATE_CAA); }
	};

	SUBSTATE(StateCAB, StateCA) {
		STATE(StateCAB)

	private:
		void entry() { TOP::box().entry(STATE_CAB); }
		void exit() { TOP::box().exit(STATE_CAB); }
		void init() { TOP::box().init(STATE_CAB); }
	};

	SUBSTATE(StateCB, StateC) {
		STATE(StateCB)

	private:
		void entry() { TOP::box().entry(STATE_CB); }
		void exit() { TOP::box().exit(STATE_CB); }
		void init() { TOP::box().init(STATE_CB); }
	};

	SUBSTATE(StateCBA, StateCB) {
		STATE(StateCBA)

	private:
		void entry() { TOP::box().entry(STATE_CBA); }
		void exit() { TOP::box().exit(STATE_CBA); }
		void init() { TOP::box().init(STATE_CBA); }
	};

	SUBSTATE(StateCBB, StateCB) {
		STATE(StateCBB)

	private:
		void entry() { TOP::box().entry(STATE_CBB); }
		void exit() { TOP::box().exit(STATE_CBB); }
		void init() { TOP::box().init(STATE_CBB); }
	};

	SUBSTATE(StateX, Top) {
		struct Box {
			Box() : state(Macho::State<StateX>()) { box_created(STATE_X); }
			Box(Macho::StateAlias s) : state(s) { box_created(STATE_X); }
			~Box() { box_destroyed(STATE_X); }

			Macho::StateAlias state;
		};

		STATE(StateX)

		void event() { setState(box().state); }

	private:
		void entry() { TOP::box().entry(STATE_X); }
		void exit() { TOP::box().exit(STATE_X); }
		void init() { TOP::box().init(STATE_X); }
	};

	void StateC::init() { TOP::box().init(STATE_C); StateCA::setState(machine()); }
	void StateCA::init() { TOP::box().init(STATE_CA); StateCAA::setState(machine()); }
}

////////////////////////////////////////////////////////////////////////////////
// Testing dispatch mechanism
enum {
	EVENT1,
	EVENT2,
	EVENT3,
	STATEA_ENTRY,
	STATEB_ENTRY
};

namespace Dispatch {

	TOPSTATE(Top) {
		typedef std::vector<int> Box;

		STATE(Top)

		virtual void clear() {
			box().clear();
		}
		virtual void event1(int i) {
			assert(i == 1);
			box().push_back(EVENT1);
		}
		virtual void event2(int i, bool b) {
			assert(i == 2); assert(!b);
			box().push_back(EVENT2);
		}
		virtual void event3(int i, bool b) {
			assert(i == 3); assert(b);
			box().push_back(EVENT3);
		}
	};

	SUBSTATE(StateA, Top) {
		STATE(StateA)

		virtual void event3(int i, bool b);

	private:
		void entry() {
			Top::box().push_back(STATEA_ENTRY);
		}
	};

	SUBSTATE(StateB, Top) {
		STATE(StateB)

		virtual void event3(int i, bool b);

	private:
		void entry() {
			Top::box().push_back(STATEB_ENTRY);
		}
	};

	void StateA::event3(int i, bool b) {
		SUPER::event3(i, b);
		StateB::setState(machine());
		dispatch(Macho::Event(&Top::event1, 1));
	}

	void StateB::event3(int i, bool b) {
		SUPER::event3(i, b);
		dispatch(Macho::Event(&Top::event1, 1));
		StateA::setState(machine());
	}
}

////////////////////////////////////////////////////////////////////////////////
// These tests will test for proper entry/exit, box and history handling
void testTransitions() {
	using namespace Transitions;

	boxes = 0;

	Macho::Machine<Top> m;

	assert(at(0, m.box().entries, STATE_TOP));
	assert(m.box().exits.empty());
	m.box().clear();

	m.box().data = 42;

	// Parent -> Child
	StateB::setState(m);
	m.rattleOn();
	assert(StateB::isCurrent(m));
	assert(StateB::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_B));
	assert(m.box().exits.empty());
	assert(at(0, m.box().inits, STATE_B));
	assert(has_box(STATE_TOP));
	m.box().clear();

	// Self transition with History
	StateB::setState(m);
	m.rattleOn();
	assert(StateB::isCurrent(m));
	assert(StateB::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_B));
	assert(at(0, m.box().exits, STATE_B));
	assert(at(0, m.box().inits, STATE_B));
	assert(has_box(STATE_TOP));
	m.box().clear();

	// Sibling -> Sibling
	StateA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(StateA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(0, m.box().exits, STATE_B));
	assert(at(0, m.box().inits, STATE_A));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Self transition
	StateA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(StateA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(0, m.box().exits, STATE_A));
	assert(at(0, m.box().inits, STATE_A));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Parent -> Child
	StateAA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(!StateA::isCurrentDirect(m));
	assert(StateAA::isCurrent(m));
	assert(StateAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_AA));
	assert(m.box().exits.empty());
	assert(at(0, m.box().inits, STATE_AA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Child -> Parent
	StateA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(StateA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(0, m.box().exits, STATE_AA));
	assert(at(1, m.box().exits, STATE_A));
	assert(at(0, m.box().inits, STATE_A));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Parent -> Child*
	StateAAA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(!StateA::isCurrentDirect(m));
	assert(StateAA::isCurrent(m));
	assert(!StateAA::isCurrentDirect(m));
	assert(StateAAA::isCurrent(m));
	assert(StateAAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_AA));
	assert(at(1, m.box().entries, STATE_AAA));
	assert(m.box().exits.empty());
	assert(at(0, m.box().inits, STATE_AAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Check persistent box
	StateABB::setState(m);
	m.rattleOn();
	StateAAA::setState(m);
	m.rattleOn();
	m.box().clear();

	// Sibling* -> Sibling*
	StateABB::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(!StateA::isCurrentDirect(m));
	assert(StateAB::isCurrent(m));
	assert(!StateAB::isCurrentDirect(m));
	assert(StateABB::isCurrent(m));
	assert(StateABB::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_AB));
	assert(at(1, m.box().entries, STATE_ABB));
	assert(at(0, m.box().exits, STATE_AAA));
	assert(at(1, m.box().exits, STATE_AA));
	assert(at(0, m.box().inits, STATE_ABB));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Child -> Parent*
	StateA::setState(m);
	m.rattleOn();
	assert(StateA::isCurrent(m));
	assert(StateA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(0, m.box().exits, STATE_ABB));
	assert(at(1, m.box().exits, STATE_AB));
	assert(at(2, m.box().exits, STATE_A));
	assert(at(0, m.box().inits, STATE_A));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	m.box().clear();

	// Sibling -> Child* of Sibling
	StateBAA::setState(m);
	m.rattleOn();
	assert(StateB::isCurrent(m));
	assert(!StateB::isCurrentDirect(m));
	assert(StateBA::isCurrent(m));
	assert(!StateBA::isCurrentDirect(m));
	assert(StateBAA::isCurrent(m));
	assert(StateBAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_B));
	assert(at(1, m.box().entries, STATE_BA));
	assert(at(2, m.box().entries, STATE_BAA));
	assert(at(0, m.box().exits, STATE_A));
	assert(at(0, m.box().inits, STATE_BAA));
	assert(has_box(STATE_TOP));
	m.box().clear();

	// Child* of Sibling -> Sibling with Init
	StateC::setState(m);
	m.rattleOn();
	assert(StateC::isCurrent(m));
	assert(!StateC::isCurrentDirect(m));
	assert(StateCA::isCurrent(m));
	assert(!StateCA::isCurrentDirect(m));
	assert(StateCAA::isCurrent(m));
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_C));
	assert(at(1, m.box().entries, STATE_CA));
	assert(at(2, m.box().entries, STATE_CAA));
	assert(at(0, m.box().exits, STATE_BAA));
	assert(at(1, m.box().exits, STATE_BA));
	assert(at(2, m.box().exits, STATE_B));
	assert(at(0, m.box().inits, STATE_C));
	assert(at(1, m.box().inits, STATE_CA));
	assert(at(2, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_CAA));
	m.box().clear();

	// Child* of Sibling -> Sibling with Shallow History
	StateB::setState(m);
	m.rattleOn();
	assert(StateB::isCurrent(m));
	assert(!StateB::isCurrentDirect(m));
	assert(StateBA::isCurrent(m));
	assert(StateBA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_B));
	assert(at(1, m.box().entries, STATE_BA));
	assert(at(0, m.box().exits, STATE_CAA));
	assert(at(1, m.box().exits, STATE_CA));
	assert(at(2, m.box().exits, STATE_C));
	assert(at(0, m.box().inits, STATE_BA));
	assert(has_box(STATE_TOP));
	m.box().clear();

	// Child of Sibling -> Sibling with Deep History
	StateC::setState(m);
	m.rattleOn();
	assert(StateC::isCurrent(m));
	assert(!StateC::isCurrentDirect(m));
	assert(StateCA::isCurrent(m));
	assert(!StateCA::isCurrentDirect(m));
	assert(StateCAA::isCurrent(m));
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_C));
	assert(at(1, m.box().entries, STATE_CA));
	assert(at(2, m.box().entries, STATE_CAA));
	assert(at(0, m.box().exits, STATE_BA));
	assert(at(1, m.box().exits, STATE_B));
	assert(at(0, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_CAA));
	m.box().clear();

	long old_boxes = boxes;
	Macho::Snapshot<Top> s(m);
	cout << endl << "Machine copied..." << endl;

	for (int i = 0; i < 2; ++i) {
		// Child -> Parent with Deep History
		StateC::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCA::isCurrent(m));
		assert(!StateCA::isCurrentDirect(m));
		assert(StateCAA::isCurrent(m));
		assert(StateCAA::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_C));
		assert(at(1, m.box().entries, STATE_CA));
		assert(at(2, m.box().entries, STATE_CAA));
		assert(at(0, m.box().exits, STATE_CAA));
		assert(at(1, m.box().exits, STATE_CA));
		assert(at(2, m.box().exits, STATE_C));
		assert(at(0, m.box().inits, STATE_CAA));
		assert(has_box(STATE_TOP));
		assert(has_box(STATE_CAA));
		m.box().clear();

		// Sibling -> Sibling, then Child -> Parent with Deep History
		StateCAB::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCA::isCurrent(m));
		assert(!StateCA::isCurrentDirect(m));
		assert(StateCAB::isCurrent(m));
		assert(StateCAB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_CAB));
		assert(at(0, m.box().exits, STATE_CAA));
		assert(at(0, m.box().inits, STATE_CAB));
		assert(has_box(STATE_TOP));
		m.box().clear();

		StateC::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCA::isCurrent(m));
		assert(!StateCA::isCurrentDirect(m));
		assert(StateCAB::isCurrent(m));
		assert(StateCAB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_C));
		assert(at(1, m.box().entries, STATE_CA));
		assert(at(2, m.box().entries, STATE_CAB));
		assert(at(0, m.box().exits, STATE_CAB));
		assert(at(1, m.box().exits, STATE_CA));
		assert(at(2, m.box().exits, STATE_C));
		assert(at(0, m.box().inits, STATE_CAB));
		assert(has_box(STATE_TOP));
		m.box().clear();

		// Sibling* -> Sibling*
		StateCBB::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCB::isCurrent(m));
		assert(!StateCB::isCurrentDirect(m));
		assert(StateCBB::isCurrent(m));
		assert(StateCBB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_CB));
		assert(at(1, m.box().entries, STATE_CBB));
		assert(at(0, m.box().exits, STATE_CAB));
		assert(at(1, m.box().exits, STATE_CA));
		assert(at(0, m.box().inits, STATE_CBB));
		assert(has_box(STATE_TOP));
		m.box().clear();

		// Child* of Sibling -> Sibling
		StateA::setState(m);
		m.rattleOn();
		assert(StateA::isCurrent(m));
		assert(StateA::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_A));
		assert(at(0, m.box().exits, STATE_CBB));
		assert(at(1, m.box().exits, STATE_CB));
		assert(at(2, m.box().exits, STATE_C));
		assert(at(0, m.box().inits, STATE_A));
		assert(has_box(STATE_TOP));
		assert(has_box(STATE_A));
		m.box().clear();

		// Sibling -> Sibling with Deep History
		StateC::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCB::isCurrent(m));
		assert(!StateCB::isCurrentDirect(m));
		assert(StateCBB::isCurrent(m));
		assert(StateCBB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_C));
		assert(at(1, m.box().entries, STATE_CB));
		assert(at(2, m.box().entries, STATE_CBB));
		assert(at(0, m.box().exits, STATE_A));
		assert(at(0, m.box().inits, STATE_CBB));
		assert(has_box(STATE_TOP));
		m.box().clear();

		// Ignoring history
		StateC::setStateDirect(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCA::isCurrent(m));
		assert(!StateCA::isCurrentDirect(m));
		assert(StateCAB::isCurrent(m));
		assert(StateCAB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_C));
		assert(at(1, m.box().entries, STATE_CA));
		assert(at(2, m.box().entries, STATE_CAB));
		assert(at(0, m.box().exits, STATE_CBB));
		assert(at(1, m.box().exits, STATE_CB));
		assert(at(2, m.box().exits, STATE_C));
		assert(at(0, m.box().inits, STATE_C));
		assert(at(1, m.box().inits, STATE_CAB));
		assert(has_box(STATE_TOP));
		m.box().clear();

		// Clearing history
		StateB::clearHistory(m);
		StateB::setState(m);
		m.rattleOn();
		assert(StateB::isCurrent(m));
		assert(StateB::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_B));
		assert(at(0, m.box().exits, STATE_CAB));
		assert(at(1, m.box().exits, STATE_CA));
		assert(at(2, m.box().exits, STATE_C));
		assert(at(0, m.box().inits, STATE_B));
		assert(has_box(STATE_TOP));

		// Clearing History deep
		StateCAB::setState(m);
		m.rattleOn();
		StateB::setState(m);
		m.rattleOn();
		m.box().clear();

		StateC::clearHistoryDeep(m);
		StateC::setState(m);
		m.rattleOn();
		assert(StateC::isCurrent(m));
		assert(!StateC::isCurrentDirect(m));
		assert(StateCA::isCurrent(m));
		assert(!StateCA::isCurrentDirect(m));
		assert(StateCAA::isCurrent(m));
		assert(StateCAA::isCurrentDirect(m));
		assert(at(0, m.box().entries, STATE_C));
		assert(at(1, m.box().entries, STATE_CA));
		assert(at(2, m.box().entries, STATE_CAA));
		assert(at(0, m.box().exits, STATE_B));
		assert(at(0, m.box().inits, STATE_C));
		assert(at(1, m.box().inits, STATE_CA));
		assert(at(2, m.box().inits, STATE_CAA));
		assert(has_box(STATE_TOP));
		assert(has_box(STATE_CAA));
		m.box().clear();

		// Testing Persistent Boxes and Snapshots
		StateAAA::setState(m);
		m.rattleOn();
		m.box().clear();

		// Top box data
		assert(m.box().data == 42);

		if (i == 0) {
			m = s;

			assert(m.box().entries.empty());
			assert(m.box().exits.empty());
			assert(m.box().inits.empty());

			boxes = old_boxes;
			m.box().clear();
			cout << endl << "Machine restored..." << endl;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Testing dispatch mechanism
void testDispatch() {
	using namespace Dispatch;

	Macho::Machine<Top> m;

	// Test dispatching of events
	m.dispatch(Macho::Event(&Top::event1, 1));
	m.dispatch(Macho::Event(&Top::event2, 2, false));
	m.dispatch(Macho::Event(&Top::event3, 3, true));

	assert(m.box()[0] == EVENT1); assert(m.box()[1] == EVENT2); assert(m.box()[2] == EVENT3);
	m->clear();

	StateA::setState(m);
	m.rattleOn();
	m->clear();
	// Test if internal dispatch happens after switching to new state
	m.dispatch(Macho::Event(&Top::event3, 3, true));
	assert(m.box()[0] == EVENT3); assert(m.box()[1] == STATEB_ENTRY); assert(m.box()[2] == EVENT1);

	m->clear();
	// Test if internal dispatch happens after switching to new state
	m.dispatch(Macho::Event(&Top::event3, 3, true));
	assert(m.box()[0] == EVENT3); assert(m.box()[1] == STATEA_ENTRY); assert(m.box()[2] == EVENT1);
}

void testAliases() {
	using namespace Transitions;

	boxes = 0;

	Macho::StateAlias state1 = Macho::State<StateCAA>(new StateCAA::Box(42));
	Macho::StateAlias state2 = Macho::State<StateAAB>(new StateAAB::Box());;

	// Test machine initialization with aliases.
	Macho::Machine<Top> m(state1);
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().entries, STATE_TOP));
	assert(at(1, m.box().entries, STATE_C));
	assert(at(2, m.box().entries, STATE_CA));
	assert(at(3, m.box().entries, STATE_CAA));
	assert(at(0, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_CAA));
	m.box().clear();

	// Test state transitions with aliases.
	m.setState(state2, true);
	assert(StateAAB::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_CAA));
	assert(at(1, m.box().exits, STATE_CA));
	assert(at(2, m.box().exits, STATE_C));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(1, m.box().entries, STATE_AA));
	assert(at(2, m.box().entries, STATE_AAB));
	assert(at(0, m.box().inits, STATE_AAB));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	assert(has_box(STATE_AAB));
	m.box().clear();

	// Test destructive use of alias boxes.
	m.setState(state1, true);
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_AAB));
	assert(at(1, m.box().exits, STATE_AA));
	assert(at(2, m.box().exits, STATE_A));
	assert(at(0, m.box().entries, STATE_C));
	assert(at(1, m.box().entries, STATE_CA));
	assert(at(2, m.box().entries, STATE_CAA));
	assert(at(0, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_CAA));
	m.box().clear();

	// Test parametrized state transitions.
	StateX::setState(m, new StateX::Box(Macho::State<StateAAB>()));
	m.rattleOn();
	assert(StateX::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_CAA));
	assert(at(1, m.box().exits, STATE_CA));
	assert(at(2, m.box().exits, STATE_C));
	assert(at(0, m.box().entries, STATE_X));
	assert(at(0, m.box().inits, STATE_X));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_X));
	m.box().clear();

	m->event(); // Will go to StateAAB.

	assert(StateAAB::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_X));
	assert(at(0, m.box().entries, STATE_A));
	assert(at(1, m.box().entries, STATE_AA));
	assert(at(2, m.box().entries, STATE_AAB));
	assert(at(0, m.box().inits, STATE_AAB));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_A));
	assert(has_box(STATE_AAB));
	m.box().clear();

	// Test cloning of aliases.
	state1 = Macho::State<StateCAA>(new StateCAA::Box(42));
	m.setState(state1.clone(), true);
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_AAB));
	assert(at(1, m.box().exits, STATE_AA));
	assert(at(2, m.box().exits, STATE_A));
	assert(at(0, m.box().entries, STATE_C));
	assert(at(1, m.box().entries, STATE_CA));
	assert(at(2, m.box().entries, STATE_CAA));
	assert(at(0, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	assert(has_box(STATE_CAA));

	StateAAB::setState(m);
	m.rattleOn();
	m.box().clear();

	m.setState(state1, true);
	assert(StateCAA::isCurrentDirect(m));
	assert(at(0, m.box().exits, STATE_AAB));
	assert(at(1, m.box().exits, STATE_AA));
	assert(at(2, m.box().exits, STATE_A));
	assert(at(0, m.box().entries, STATE_C));
	assert(at(1, m.box().entries, STATE_CA));
	assert(at(2, m.box().entries, STATE_CAA));
	assert(at(0, m.box().inits, STATE_CAA));
	assert(has_box(STATE_TOP));
	//assert(has_box(STATE_CAA)); // Box was already created before.
	m.box().clear();
}

int main() {
	cout << "Testing transitions" << endl;
	testTransitions();

	cout << endl << "Testing dispatch mechanism" << endl;
	testDispatch();

	cout << endl << "Testing state aliases" << endl;
	testAliases();

	cout << "Test complete" << endl;
	return 0;
}
