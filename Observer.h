#pragma once

#include <string>
#include <functional>
#include <memory>
#include <any>
#include <variant>
#include <iostream>

// todo: need a toString
// 
typedef struct MessageType {
   MessageType() {}
   MessageType(const std::string& v) : value_(v) {}
   MessageType(const char * v) : value_(std::string(v)) {}

   MessageType(std::any v) : value_(v) {}
   MessageType(int v) : value_(v) {}
   MessageType(bool v) : value_(v) {}

   std::any value_;
   void set(std::any v) { value_ = v; }
   const std::any& getAny() const { return value_; }
   template <typename T> T get() const { 
      T retVal;
      if (auto ptr = std::any_cast<T>(&value_)) {
         retVal = *ptr;
      }
      return retVal;
   }
   //todo confusing, change to equals
   // as string compare, return false if they match
   bool compare(const MessageType& match) const {
      if (value_.type() == match.value_.type()) {
         bool boolish = true;
         bool intish = 1;
         if (value_.type() == typeid(boolish)) {
            return  std::any_cast<bool>(value_) != std::any_cast<bool>(match.value_);
         }
         else if (value_.type() == typeid(intish)) {
            return  std::any_cast<int>(value_) != std::any_cast<int>(match.value_);
         }
         else {
            return  std::any_cast<std::string>(value_) != std::any_cast<std::string>(match.value_);
         }
      }
      return false;
   }

   std::string toString() const {
      bool boolish = true;
      int intish = 1;
      if (!value_.has_value()) {
         return "<no value>";
      }
      if (value_.type() == typeid(boolish)) {
         return  std::any_cast<bool>(value_) ? "true" : "false";
      }
      else if (value_.type() == typeid(intish)) {
         return std::to_string(std::any_cast<int>(value_));
      }
      else {
         return  std::any_cast<std::string>(value_);
      }
   }
} MessageType;



class SubjectInterface;
typedef std::shared_ptr<SubjectInterface> SubjectPtr;


class ObserverInterface : public std::enable_shared_from_this<ObserverInterface> {
public:
   virtual ~ObserverInterface() {};
   virtual void update(const MessageType& message_from_subject) = 0;
   virtual void setUpdate(std::function<void(const MessageType&)> updateFn) = 0;
   virtual void stop() = 0;
};

typedef std::shared_ptr<ObserverInterface> ObserverPtr;

class Observer : public ObserverInterface {
public:
   Observer(SubjectPtr& subject) : subject_(subject) {}
   static ObserverPtr create(SubjectPtr subject);
   virtual ~Observer() {}

   void update(const MessageType& message_from_subject) override {
      updateFn_(message_from_subject);
   }
   void setUpdate(std::function<void(const MessageType&)> updateFn) {
      updateFn_ = updateFn;
   }
   virtual void stop() override;

private:
   SubjectPtr subject_;
   std::function<void(const MessageType&)> updateFn_ = [](const MessageType&) {};
};

