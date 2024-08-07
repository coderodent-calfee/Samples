#pragma once

#include <list>
#include <memory>
#include <string>

#include "Observer.h"
// todo:  the lapsed listener problem, This can be prevented if the subject holds weak references to the observers.

class SubjectInterface : public std::enable_shared_from_this<SubjectInterface> {
public:
   virtual ~SubjectInterface() {};
   virtual void attach(const ObserverPtr& observer) = 0;
   virtual void detach(const ObserverPtr& observer) = 0;
   virtual void notify() = 0;  // todo: protected?
   virtual void setValue(const MessageType& message) = 0;
   virtual const MessageType& getValue() const = 0;
   virtual const std::string& getName() const = 0;
   virtual ObserverPtr getObserver() = 0;

};

typedef std::shared_ptr<SubjectInterface> SubjectPtr;
// TODO: not thread safe

class SubjectImplementation : public SubjectInterface {
public:
   SubjectImplementation(const std::string& name) : name_(name) {}
   virtual ~SubjectImplementation() {}

   static std::pair<const std::string, SubjectPtr> create(const std::string& name) {
      return std::make_pair<>(name, std::make_shared<SubjectImplementation>(name));
   }

   void attach(const ObserverPtr& observer) override {
      list_observer_.push_back(observer);
   }

   void detach(const ObserverPtr& observer) override {
      list_observer_.remove(observer);
   }
   // todo: should the notification include prior state as well as new state?
   void notify() override {
      std::list<ObserverPtr> observers(list_observer_);

      std::list<ObserverPtr>::iterator iterator = observers.begin();
      while (iterator != observers.end()) {
         (*iterator)->update(message_);
         ++iterator;
      }
   }
   // todo: change message first? or after notify?
   // remember Cary's clever update on collections with a list of added or removed
   //also if the property type changes, or the property is destroyed
   void setValue(const MessageType& message) override {
      this->message_ = message;
      notify();
   }
   
   const MessageType& getValue() const override {
      return message_;
   }
   
   const std::string& getName() const override {
      return name_;
   };

   ObserverPtr getObserver() {
      return Observer::create(shared_from_this());
   }

private:
   const std::string name_;
   std::list<ObserverPtr> list_observer_;
   MessageType message_;
};

