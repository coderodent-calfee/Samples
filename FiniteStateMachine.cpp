// https://github.com/NameOfTheDragon/Java-Finite-State-Machine/tree/master thread-safe/java: one notion is that transittions go through a disconnected intermediate state
// https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus-2
// https://www.codeproject.com/Articles/1156423/Cplusplus-State-Machine-with-Threads
// https://github.com/digint/tinyfsm/tree/01908cab0397fcdadb0a14e9a3187c308e2708ca

#include "FiniteStateMachine.h"
#include <ranges>

template <typename T> void FiniteStateMachine::validate(const T& map, std::string key) const {
   if (!map.count(key)) {
      std::cout << "no state '" << key << "' exists in states" << std::endl
         << "Did you mean: " << std::endl;
      for (auto pair : map) {
         std::cout << " - " << pair.first << std::endl;
      }
   }
}

// todo: also take edge/transitions list
// change to const ref
FiniteStateMachine::FiniteStateMachine( Properties properties, States states, Transitions transitions, std::string startState) :
   properties_(properties.begin(), properties.end()),
   states_(states.begin(), states.end()),
   transitions_(transitions.begin(), transitions.end()),
   currentState_(startState) 
{

   properties_.insert(Property::create("FiniteStateMachine::EnterState"));
   properties_.insert(Property::create("FiniteStateMachine::ExitState"));
   properties_.insert(Property::create("FiniteStateMachine::Transition"));

   // todo validate all edges against known states
   std::for_each(transitions_.begin(), transitions_.end(), [&](const FSMTransitionPtr& transition) {
      validate<>(states_, transition->getFrom());
      validate<>(states_, transition->getTo());
      });


   hiddenStateWithNoTransitions_ = std::make_shared<FSMStateImplementation>("hiddenStateWithNoTransitions", std::make_shared<FSMEmptyState>());
   enterState(currentState_);
}


// todo: maybe we should have a clear transitions in this class too, instead we rely on the implementations exit
void FiniteStateMachine::buildTransitionsForState(const std::string& stateName) {
   const auto state = states_.at(stateName);
   FiniteStateMachine* ctx = this;

   std::for_each(transitions_.begin(), transitions_.end(), [&](const FSMTransitionPtr& transition) {
      if ( stateName == transition->getFrom() ) {
         state->setPropertyUpdate(this, transition->getProperty(),
            [=](const MessageType& m) {
//               std::cout << stateName << ":" << transition->getProperty() << " changed to " << m.toString() << std::endl;
               if (transition->getConditional(ctx, m) ) {
                  if (!transition->hasMessage() || (m.compare(transition->getMessage()) == 0)) {
                     ctx->doTransition(transition);
                  }
               }
            });
      }
   });
}

void FiniteStateMachine::enterState(const std::string& state) {
   validate<>(states_, currentState_);
   buildTransitionsForState(state);
   auto startAt = states_.at(state);
   startAt->entry(this);
}

void FiniteStateMachine::doTransition(const FSMTransitionPtr& transition) {
   std::cout << "transitioning from " << currentState_ << std::endl;
   
   int transitionIndex = -1;
   auto i = std::find(transitions_.begin(), transitions_.end(), transition);
   if (i != transitions_.end()){
         transitionIndex = (int)(i - transitions_.begin());
   } else {
      std::cout << "ERROR!!! transition is not from this FSM" << std::endl;
      return;
   }

   auto fromState = transition->getFrom();
   auto toState = transition->getTo();

   if (fromState != currentState_) { 
      std::cout << "ERROR!!! " << fromState << " is not " << currentState_ << std::endl;
      return;
   }

   // todo: trouble with multithreading locks
   auto current = states_.at(currentState_);
   currentState_ = hiddenStateWithNoTransitions_->getName();
   /// todo: no way to handle exceptions?
   current->exit(this);
   // we let subscribers know after we exit
   setProperty("FiniteStateMachine::ExitState", fromState);

   // we let subscribers know which transition
   setProperty("FiniteStateMachine::Transition", transitionIndex);

   currentState_ = toState;
   enterState(currentState_);
   // we let subscribers know after we enter
   setProperty("FiniteStateMachine::EnterState", toState);
}

PropertyPtr FiniteStateMachine::getProperty(std::string key) const {
   validate<>(properties_, key);
   return properties_.at(key);
}

void FiniteStateMachine::setProperty(std::string key, MessageType value) const {
   std::cout << std::endl << "set " << key << " to " << value.toString() << std::endl;
   validate<>(properties_, key);
   auto& v = properties_.at(key);
   v->setValue(value);
}

// not sure about this
void FiniteStateMachine::MyUpdate(const MessageType& value) {
   std::cout << "My Update:" << value.toString() << std::endl;
}

void FiniteStateMachine::addInput(SubjectPtr input) {
   auto o = input->getObserver();
   o->setUpdate([&](const MessageType& m) { MyUpdate(m); });
   inputObservers_.push_back(o);
}

FiniteStateMachine::EdgesWithNames FiniteStateMachine::getEdgesWithNames() const {
   Edges edges;
   Names edgeNames;

   auto kv = std::views::keys(states_);
   Names stateNames{ kv.begin(), kv.end() };

   auto stateToIndex = [&stateNames](const std::string& stateName) { return std::find(stateNames.begin(), stateNames.end(), stateName) - stateNames.begin(); };

   std::transform(transitions_.begin(), transitions_.end(),
      std::inserter(edges, edges.end()),
      [&](const FSMTransitionPtr& edge) {
         return std::make_pair(stateToIndex(edge->getFrom()), stateToIndex(edge->getTo()));
      });
   std::transform(transitions_.begin(), transitions_.end(),
      std::inserter(edgeNames, edgeNames.end()), [&](const FSMTransitionPtr& edge) { return edge->getName(); });

   return std::make_tuple(edges, edgeNames, stateNames);
}
