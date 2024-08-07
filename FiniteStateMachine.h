#pragma once
#include <list>
#include <string>
#include <functional>


#include "Observer.h"
#include "Subject.h"

#include <map>
#include <string>
#include <memory>

#include <iostream>
#include <algorithm>

#include "Property.h"
#include "FSMState.h"



//class FSMOutput;
//class FSMInput;
class FSMStateInterface;
class FSMState;
class FiniteStateMachine;

//typedef std::shared_ptr<FSMOutput > FSMOutputPtr;
//typedef std::shared_ptr<FSMInput  > FSMInputPtr;

// todo: remove things that are not needed in h files
// need edges defined in construction of FSM; once created we should be able to draw graph
// no transitions in entry/exit
// defer transitions to queue


// todo: add Property for the current state, and the transition that was used to go there

class FiniteStateMachine
{
private:

public:
   typedef std::map<const std::string, PropertyPtr > Properties;
   typedef std::map<const std::string, FSMStatePtr > States;
   typedef std::vector<FSMTransitionPtr> Transitions;

   FiniteStateMachine(Properties properties, States states, Transitions transitions, std::string startState);
   std::string getState() { return currentState_; }

   // not sure about this
   void MyUpdate(const MessageType& message_from_subject);
   void addInput(SubjectPtr input);

   PropertyPtr getProperty(std::string key) const;
   void setProperty(std::string key, MessageType value) const;

   template <typename T> static std::pair<std::string, FSMStatePtr> create(const std::string& name) {
      return std::make_pair<>(name, std::make_shared<FSMStateImplementation>(name, std::make_shared<T>()));
   }

private:
   void buildTransitionsForState(const std::string& state);
   void enterState(const std::string& state);
   void transitionState(const std::string& fromState, const std::string& toState);

   template <typename T> void validate(const T& map, std::string key) const;
   FSMStatePtr hiddenStateWithNoTransitions_;

   std::string  currentState_;
   std::list<ObserverPtr> inputObservers_;
   Properties  properties_;
   States      states_;
   Transitions transitions_;
};




