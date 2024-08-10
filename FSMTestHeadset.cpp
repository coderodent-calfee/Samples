#include "FiniteStateMachine.h"
#include <iostream>
#include <cassert>

using namespace std;


class FSMTestHeadset : public FiniteStateMachine {
public:
	FSMTestHeadset(Properties properties, States states, std::vector<FSMTransitionPtr> transitions, std::string startState) : FiniteStateMachine(properties, states, transitions, startState) {}
};

class SwitchInput : public SubjectImplementation {};


class FSMTestHeadsetStateOff : public FSMEmptyState {
public:
	void entry(FiniteStateMachine* ctx) override {
		std::cout << "Off entry:" << std::endl;
	};
};

class FSMTestHeadsetStatePairing : public FSMEmptyState {
public:
	void entry(FiniteStateMachine* ctx) override {
		std::cout << "Pairing entry:" << std::endl;
		ctx->setProperty("First Time On", false);
	};
};

class FSMTestHeadsetStateConnected : public FSMEmptyState {
public:
	void entry(FiniteStateMachine* ctx) override {
		std::cout << "Connected entry:" << std::endl;
	};
};

class FSMTestHeadsetStateSearching : public FSMEmptyState {
public:
	void entry(FiniteStateMachine* ctx) override {
		std::cout << "Searching entry:" << std::endl;
	};
};

void testFiniteStateMachine() {
	static MessageType firstTimeOn(true);

	//auto firstTimeProp = Property::create("First Time On");
	//firstTimeProp.second->setValue(firstTimeOn);

	FiniteStateMachine::Properties props = {
		{ Property::create("On/Off") },
		{ Property::create("Pairing Button") },
		{ Property::create("Paired With") },
		{ Property::create("Volume") },
		{ Property::create("Phone Volume") },
		{ Property::create("Timer") },
		{ Property::create("Connection") },
		{ Property::create("First Time On", true) }
	};

	FiniteStateMachine::States states = {
		FiniteStateMachine::create<FSMTestHeadsetStateOff>("Off"),
		FiniteStateMachine::create<FSMTestHeadsetStatePairing>("Pairing"),
		FiniteStateMachine::create<FSMTestHeadsetStateConnected>("Connected"),
		FiniteStateMachine::create<FSMTestHeadsetStateSearching>("Searching")
	};

	FSMTransitionFn conditionalFn = [](FiniteStateMachine* ctx, const MessageType& message) {

		bool firstTimeOn = false;
		auto firstTimeOnProp = ctx->getProperty("First Time On");

		firstTimeOnProp->getValue(firstTimeOn, firstTimeOn);

		if (firstTimeOn == true) {
			std::cout << "First Time On edge conditional" << std::endl;
			return true;
		}
		return false;
	};

	FiniteStateMachine::Transitions transitions = {
		std::make_shared<FSMStateTransition>("Off", "Pairing", "On/Off", conditionalFn), // except it needs to check "first time on"
		std::make_shared<FSMStateTransition>("Off", "Searching", "On/Off", "on"),
		std::make_shared<FSMStateTransition>("Pairing", "Off", "On/Off", "off"),
		std::make_shared<FSMStateTransition>("Pairing", "Searching", "Timer", "timeout"),
		std::make_shared<FSMStateTransition>("Pairing", "Searching", "Paired With"), // transition on any update of the property
		std::make_shared<FSMStateTransition>("Searching", "Off", "Timer", "timeout"),
		std::make_shared<FSMStateTransition>("Searching", "Connected", "Connection", "connected"),
		std::make_shared<FSMStateTransition>("Searching", "Off", "On/Off", "off"),
		std::make_shared<FSMStateTransition>("Searching", "Pairing", "Pairing Button"), // on any update (entry into Pairing sets button to false)
		std::make_shared<FSMStateTransition>("Connected", "Off", "On/Off", "off"),
		std::make_shared<FSMStateTransition>("Connected", "Pairing", "Pairing Button"),
		std::make_shared<FSMStateTransition>("Connected", "Searching", "Connection", "disconnected"),
	};
	
	auto fsm = std::make_shared<FSMTestHeadset>(props, states, transitions, "Off");
	auto graph = std::make_shared<FSMGraphAdapter>(fsm);

	cout << "init" << endl;
	assert(fsm->getState() == "Off");

	fsm->setProperty("On/Off", "on");
	assert(fsm->getState() == "Pairing");

	fsm->setProperty("Paired With", "Mobile Phone");
	assert(fsm->getState() == "Searching");

	fsm->setProperty("On/Off", "off");
	assert(fsm->getState() == "Off");

	fsm->setProperty("On/Off", "on");
	assert(fsm->getState() == "Searching");

	fsm->setProperty("Connection", "connected");
	assert(fsm->getState() == "Connected");

	fsm->setProperty("Connection", "disconnected");
	assert(fsm->getState() == "Searching");

	fsm->setProperty("On/Off", "off");
	assert(fsm->getState() == "Off");


	cout << endl << "OK! **done**" << endl;

	graph->show();

	//auto onOffSwitch = std::make_shared<SwitchInput>();
	//fsm.addInput(onOffSwitch);
	//onOffSwitch->CreateMessage(" Switch On ");
	//onOffSwitch->CreateMessage(" Switch Off ");
}
