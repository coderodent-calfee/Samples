#include "FiniteStateMachine.h"



void FSMStateImplementation::setPropertyUpdate(FiniteStateMachine* ctx, std::string property, std::function<void(const MessageType&)> updateFn) {
	auto prop = ctx->getProperty(property);
	ObserverPtr o = prop->getObserver();
	o->setUpdate(updateFn);
	inputObservers_.push_back(o);
}