// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: enum.proto

#ifndef PROTOBUF_enum_2eproto__INCLUDED
#define PROTOBUF_enum_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace entity {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_enum_2eproto();
void protobuf_AssignDesc_enum_2eproto();
void protobuf_ShutdownFile_enum_2eproto();


enum PosiDirectionType {
  NET = 49,
  LONG = 50,
  SHORT = 51
};
bool PosiDirectionType_IsValid(int value);
const PosiDirectionType PosiDirectionType_MIN = NET;
const PosiDirectionType PosiDirectionType_MAX = SHORT;
const int PosiDirectionType_ARRAYSIZE = PosiDirectionType_MAX + 1;

const ::google::protobuf::EnumDescriptor* PosiDirectionType_descriptor();
inline const ::std::string& PosiDirectionType_Name(PosiDirectionType value) {
  return ::google::protobuf::internal::NameOfEnum(
    PosiDirectionType_descriptor(), value);
}
inline bool PosiDirectionType_Parse(
    const ::std::string& name, PosiDirectionType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PosiDirectionType>(
    PosiDirectionType_descriptor(), name, value);
}
enum LegStatus {
  UNOPENED = 0,
  IS_OPENING = 1,
  OPENED = 2,
  IS_CLOSING = 3,
  CLOSED = 4
};
bool LegStatus_IsValid(int value);
const LegStatus LegStatus_MIN = UNOPENED;
const LegStatus LegStatus_MAX = CLOSED;
const int LegStatus_ARRAYSIZE = LegStatus_MAX + 1;

const ::google::protobuf::EnumDescriptor* LegStatus_descriptor();
inline const ::std::string& LegStatus_Name(LegStatus value) {
  return ::google::protobuf::internal::NameOfEnum(
    LegStatus_descriptor(), value);
}
inline bool LegStatus_Parse(
    const ::std::string& name, LegStatus* value) {
  return ::google::protobuf::internal::ParseNamedEnum<LegStatus>(
    LegStatus_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================


// @@protoc_insertion_point(namespace_scope)

}  // namespace entity

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< entity::PosiDirectionType>() {
  return entity::PosiDirectionType_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< entity::LegStatus>() {
  return entity::LegStatus_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_enum_2eproto__INCLUDED
