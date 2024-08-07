#pragma once
#include "Subject.h"


class Property;
typedef std::shared_ptr<Property> PropertyPtr;

class Property : public SubjectImplementation {
public:
   Property(const std::string& name) : SubjectImplementation(name) {}
   virtual ~Property() {};

   template <typename T>
   void set(const T& m) {
      const MessageType msg(m);
      setValue(msg);
   }

   void set(const char* m) {
      set(std::string(m));
   }

   template <>
   void set(const std::string& m);


   template <typename T>
   T getValue(T& m, T& defaultValue) {
      if (get(m)) {
         return m;
      }
      return defaultValue;
   }

   const char* getTypeName() const {
      auto v = SubjectImplementation::getValue();
      std::any a = v.getAny();
      return a.type().name();
   }

   template <typename T>
   bool get( T& m) {
      auto v = SubjectImplementation::getValue();
      std::any a = v.getAny();
      if (a.has_value()) {
         T* ptr = std::any_cast<T>(&a);
         if (ptr) {
            m = *ptr;
            return true;
         }
      }
      return false;
   }

   static std::pair<std::string, PropertyPtr> create(const std::string& name)
   {
      PropertyPtr p = std::make_shared<Property>(name);
      return std::make_pair<>(name, p);
   }

   template <typename T>
   static std::pair<std::string, PropertyPtr> create(const std::string& name, T initialValue)
   {
      PropertyPtr p = std::make_shared<Property>(name);
      p->set(initialValue);
      return std::make_pair<>(name, p);
   }

};


