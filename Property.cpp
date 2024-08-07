#include "Property.h"



template <>
void Property::set(const std::string& m) {
   const MessageType msg(m);
   setValue(msg);
}