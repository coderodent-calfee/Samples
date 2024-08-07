#pragma once

#include <list>
#include <memory>

#include "Observer.h"

class FiniteStateMachine;

class FSMStateInterface
{
public:
	virtual ~FSMStateInterface() {};
	virtual void entry(FiniteStateMachine*) = 0;
   virtual void exit(FiniteStateMachine*) = 0;
   virtual void update(FiniteStateMachine*) = 0;
};

typedef std::shared_ptr<FSMStateInterface> FSMStateInterfacePtr;

class FSMEmptyState : public FSMStateInterface {
public:
	void entry(FiniteStateMachine*) override {};
	void exit(FiniteStateMachine*) override {};
	void update(FiniteStateMachine*) override {};
};

class FSMStateImplementation;
typedef std::shared_ptr<FSMStateImplementation> FSMStatePtr;

class FSMStateImplementation {
public:
	FSMStateImplementation(const std::string& stateName, const FSMStateInterfacePtr& state) : stateName_(stateName), state_(state) {};

	virtual ~FSMStateImplementation() {};
	
	void entry(FiniteStateMachine* ctx)  {
		state_->entry(ctx);
	};
	
	void exit(FiniteStateMachine* ctx) {
		for (auto o : inputObservers_) {
			o->stop();
		}
		inputObservers_.clear();
		state_->exit(ctx);
	};
	
	void update(FiniteStateMachine* ctx){
		state_->update(ctx);
	};

	const std::string& getName() const {
		return stateName_;
	};

	void setPropertyUpdate(FiniteStateMachine* ctx, std::string property, std::function<void(const MessageType&)> updateFn);

private:
	std::list<ObserverPtr> inputObservers_;
	const std::string stateName_;
	FSMStateInterfacePtr state_;
};

class FSMStateTransition;

typedef std::shared_ptr<FSMStateTransition> FSMTransitionPtr;
typedef std::function<bool(FiniteStateMachine* ctx, const MessageType& message)> FSMTransitionFn;


class FSMStateTransition {
public:
	// gotoStateIfMessageMatch watches properties for updates
	// how to handle if property is already set to the transition state?
	// queue a transition?
   FSMStateTransition(const std::string& fromStateName, const std::string& toStateName, const std::string& propertyName, const MessageType& message) : 
		fromStateName_(fromStateName), 
		toStateName_(toStateName), 
		propertyName_(propertyName), 
		message_(message)
	{
		name_ = fromStateName_ + " to " + toStateName_ + " when " + propertyName_ + " is " + message_.toString();
	};

	// change state on any message change
	FSMStateTransition(const std::string& fromStateName, const std::string& toStateName, const std::string& propertyName) :
		fromStateName_(fromStateName),
		toStateName_(toStateName),
		propertyName_(propertyName)
	{
		name_ = fromStateName_ + " to " + toStateName_ + " when " + propertyName_ + " changes";
	};

	// given a test function to control moving to new state (not just message match)
	FSMStateTransition(const std::string& fromStateName, const std::string& toStateName, const std::string& propertyName, FSMTransitionFn conditionalFn) :
		fromStateName_(fromStateName),
		toStateName_(toStateName),
		propertyName_(propertyName),
		conditionalFn_(conditionalFn)
	{
		name_ = fromStateName_ + " to " + toStateName_ + " when the conditional is true and " + propertyName_ + " is " + message_.toString();
	};


	virtual ~FSMStateTransition() {};

	const std::string& getName() const {
		return name_;
	};

	const std::string& getFrom() const {
		return fromStateName_;
	};

	const std::string& getTo() const {
		return toStateName_;
	};

	const std::string& getProperty() const {
		return propertyName_;
	};

	const bool getConditional(FiniteStateMachine* ctx, const MessageType& message) const {
		return conditionalFn_(ctx, message);
	};

	const bool hasMessage()  {
		auto any = message_.getAny();
		return any.has_value();
	};

	const MessageType& getMessage() const {
		return message_;
	};

private:
	std::string name_;
	const std::string fromStateName_;
	const std::string toStateName_;
	const std::string propertyName_;
	const MessageType message_;
	FSMTransitionFn conditionalFn_ = [](FiniteStateMachine*, const MessageType&) { return true; };
};
