#include "Observer.h"
#include "Subject.h"

ObserverPtr Observer::create(SubjectPtr subject) {
   auto observer = std::make_shared<Observer>(subject);
   observer->subject_->attach(observer);
   return observer;
}

void Observer::stop() {
   subject_->detach(shared_from_this());
   updateFn_ = [](const MessageType&) {};
}