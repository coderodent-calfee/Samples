// https://github.com/NameOfTheDragon/Java-Finite-State-Machine/tree/master thread-safe/java: one notion is that transittions go through a disconnected intermediate state
// https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus-2
// https://www.codeproject.com/Articles/1156423/Cplusplus-State-Machine-with-Threads
// https://github.com/digint/tinyfsm/tree/01908cab0397fcdadb0a14e9a3187c308e2708ca

#include "FiniteStateMachine.h"

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
                     ctx->transitionState(transition->getFrom(), transition->getTo());
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

void FiniteStateMachine::transitionState(const std::string& fromState, const std::string& toState) {
   std::cout << "transitioning from " << currentState_ << std::endl;

   // todo: trouble with multithreading locks
   if (fromState != currentState_) { 
      std::cout << "ERROR!!! " << fromState << " is not " << currentState_ << std::endl;
      return;
   }
   auto current = states_.at(currentState_);
   currentState_ = hiddenStateWithNoTransitions_->getName();
   /// todo: no way to handle exceptions?
   current->exit(this);
   currentState_ = toState;
   enterState(currentState_);
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
