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

#include <cmath>
#include <matplot/matplot.h>
#include <type_traits>
#include <tuple>

//class FSMOutput;
//class FSMInput;
class FSMStateInterface;
class FSMState;
class FiniteStateMachine;

typedef std::shared_ptr<FiniteStateMachine> FSMContextPtr;

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

   typedef std::vector<std::pair<size_t, size_t>> Edges;
   typedef std::vector<std::string> Names;
   typedef std::tuple<FiniteStateMachine::Edges, FiniteStateMachine::Names, FiniteStateMachine::Names> EdgesWithNames;

   EdgesWithNames getEdgesWithNames() const;

private:
   void buildTransitionsForState(const std::string& state);
   void enterState(const std::string& state);
   void doTransition(const FSMTransitionPtr& transition);

   template <typename T> void validate(const T& map, std::string key) const;
   FSMStatePtr hiddenStateWithNoTransitions_;

   std::string  currentState_;
   std::list<ObserverPtr> inputObservers_;
   Properties  properties_;
   States      states_;
   Transitions transitions_;
};


class FSMGraphAdapter {
private:
   std::shared_ptr<class matplot::network> graph_;
   int  currentState_;
   int  previousState_;
   int  transition_;
   FiniteStateMachine::Names edgeNames_;
   FiniteStateMachine::Names nodeNames_;
   std::vector<double> colors_;
   std::vector<float> markerSizes_;

   ObserverPtr entryObserver_;
   ObserverPtr exitObserver_;
   ObserverPtr edgeObserver_;

   void setEnteredState(const std::string& state);
   void setExitedState(const std::string& state);
   void setTransitionEdge(int edgeIndex);
   const FSMContextPtr& ctx_;
public:
   FSMGraphAdapter(const FSMContextPtr& ctx);
   void show() {
      matplot::show();
   }
};