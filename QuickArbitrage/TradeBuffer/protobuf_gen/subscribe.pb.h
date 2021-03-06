// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: subscribe.proto

#ifndef PROTOBUF_subscribe_2eproto__INCLUDED
#define PROTOBUF_subscribe_2eproto__INCLUDED

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

namespace protoc {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_subscribe_2eproto();
void protobuf_AssignDesc_subscribe_2eproto();
void protobuf_ShutdownFile_subscribe_2eproto();

class ReqSubscribe;
class RspSubscribe;
class ReqUnsubscribe;
class RspUnsubscribe;

// ===================================================================

class ReqSubscribe : public ::google::protobuf::Message {
 public:
  ReqSubscribe();
  virtual ~ReqSubscribe();
  
  ReqSubscribe(const ReqSubscribe& from);
  
  inline ReqSubscribe& operator=(const ReqSubscribe& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqSubscribe& default_instance();
  
  void Swap(ReqSubscribe* other);
  
  // implements Message ----------------------------------------------
  
  ReqSubscribe* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ReqSubscribe& from);
  void MergeFrom(const ReqSubscribe& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // repeated string symbols = 1;
  inline int symbols_size() const;
  inline void clear_symbols();
  static const int kSymbolsFieldNumber = 1;
  inline const ::std::string& symbols(int index) const;
  inline ::std::string* mutable_symbols(int index);
  inline void set_symbols(int index, const ::std::string& value);
  inline void set_symbols(int index, const char* value);
  inline void set_symbols(int index, const char* value, size_t size);
  inline ::std::string* add_symbols();
  inline void add_symbols(const ::std::string& value);
  inline void add_symbols(const char* value);
  inline void add_symbols(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& symbols() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_symbols();
  
  // @@protoc_insertion_point(class_scope:protoc.ReqSubscribe)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> symbols_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_subscribe_2eproto();
  friend void protobuf_AssignDesc_subscribe_2eproto();
  friend void protobuf_ShutdownFile_subscribe_2eproto();
  
  void InitAsDefaultInstance();
  static ReqSubscribe* default_instance_;
};
// -------------------------------------------------------------------

class RspSubscribe : public ::google::protobuf::Message {
 public:
  RspSubscribe();
  virtual ~RspSubscribe();
  
  RspSubscribe(const RspSubscribe& from);
  
  inline RspSubscribe& operator=(const RspSubscribe& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RspSubscribe& default_instance();
  
  void Swap(RspSubscribe* other);
  
  // implements Message ----------------------------------------------
  
  RspSubscribe* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspSubscribe& from);
  void MergeFrom(const RspSubscribe& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required bool succ = 1;
  inline bool has_succ() const;
  inline void clear_succ();
  static const int kSuccFieldNumber = 1;
  inline bool succ() const;
  inline void set_succ(bool value);
  
  // @@protoc_insertion_point(class_scope:protoc.RspSubscribe)
 private:
  inline void set_has_succ();
  inline void clear_has_succ();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  bool succ_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_subscribe_2eproto();
  friend void protobuf_AssignDesc_subscribe_2eproto();
  friend void protobuf_ShutdownFile_subscribe_2eproto();
  
  void InitAsDefaultInstance();
  static RspSubscribe* default_instance_;
};
// -------------------------------------------------------------------

class ReqUnsubscribe : public ::google::protobuf::Message {
 public:
  ReqUnsubscribe();
  virtual ~ReqUnsubscribe();
  
  ReqUnsubscribe(const ReqUnsubscribe& from);
  
  inline ReqUnsubscribe& operator=(const ReqUnsubscribe& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqUnsubscribe& default_instance();
  
  void Swap(ReqUnsubscribe* other);
  
  // implements Message ----------------------------------------------
  
  ReqUnsubscribe* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ReqUnsubscribe& from);
  void MergeFrom(const ReqUnsubscribe& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // repeated string symbols = 1;
  inline int symbols_size() const;
  inline void clear_symbols();
  static const int kSymbolsFieldNumber = 1;
  inline const ::std::string& symbols(int index) const;
  inline ::std::string* mutable_symbols(int index);
  inline void set_symbols(int index, const ::std::string& value);
  inline void set_symbols(int index, const char* value);
  inline void set_symbols(int index, const char* value, size_t size);
  inline ::std::string* add_symbols();
  inline void add_symbols(const ::std::string& value);
  inline void add_symbols(const char* value);
  inline void add_symbols(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& symbols() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_symbols();
  
  // @@protoc_insertion_point(class_scope:protoc.ReqUnsubscribe)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> symbols_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_subscribe_2eproto();
  friend void protobuf_AssignDesc_subscribe_2eproto();
  friend void protobuf_ShutdownFile_subscribe_2eproto();
  
  void InitAsDefaultInstance();
  static ReqUnsubscribe* default_instance_;
};
// -------------------------------------------------------------------

class RspUnsubscribe : public ::google::protobuf::Message {
 public:
  RspUnsubscribe();
  virtual ~RspUnsubscribe();
  
  RspUnsubscribe(const RspUnsubscribe& from);
  
  inline RspUnsubscribe& operator=(const RspUnsubscribe& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const RspUnsubscribe& default_instance();
  
  void Swap(RspUnsubscribe* other);
  
  // implements Message ----------------------------------------------
  
  RspUnsubscribe* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RspUnsubscribe& from);
  void MergeFrom(const RspUnsubscribe& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required bool succ = 1;
  inline bool has_succ() const;
  inline void clear_succ();
  static const int kSuccFieldNumber = 1;
  inline bool succ() const;
  inline void set_succ(bool value);
  
  // @@protoc_insertion_point(class_scope:protoc.RspUnsubscribe)
 private:
  inline void set_has_succ();
  inline void clear_has_succ();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  bool succ_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_subscribe_2eproto();
  friend void protobuf_AssignDesc_subscribe_2eproto();
  friend void protobuf_ShutdownFile_subscribe_2eproto();
  
  void InitAsDefaultInstance();
  static RspUnsubscribe* default_instance_;
};
// ===================================================================


// ===================================================================

// ReqSubscribe

// repeated string symbols = 1;
inline int ReqSubscribe::symbols_size() const {
  return symbols_.size();
}
inline void ReqSubscribe::clear_symbols() {
  symbols_.Clear();
}
inline const ::std::string& ReqSubscribe::symbols(int index) const {
  return symbols_.Get(index);
}
inline ::std::string* ReqSubscribe::mutable_symbols(int index) {
  return symbols_.Mutable(index);
}
inline void ReqSubscribe::set_symbols(int index, const ::std::string& value) {
  symbols_.Mutable(index)->assign(value);
}
inline void ReqSubscribe::set_symbols(int index, const char* value) {
  symbols_.Mutable(index)->assign(value);
}
inline void ReqSubscribe::set_symbols(int index, const char* value, size_t size) {
  symbols_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReqSubscribe::add_symbols() {
  return symbols_.Add();
}
inline void ReqSubscribe::add_symbols(const ::std::string& value) {
  symbols_.Add()->assign(value);
}
inline void ReqSubscribe::add_symbols(const char* value) {
  symbols_.Add()->assign(value);
}
inline void ReqSubscribe::add_symbols(const char* value, size_t size) {
  symbols_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ReqSubscribe::symbols() const {
  return symbols_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ReqSubscribe::mutable_symbols() {
  return &symbols_;
}

// -------------------------------------------------------------------

// RspSubscribe

// required bool succ = 1;
inline bool RspSubscribe::has_succ() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RspSubscribe::set_has_succ() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RspSubscribe::clear_has_succ() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RspSubscribe::clear_succ() {
  succ_ = false;
  clear_has_succ();
}
inline bool RspSubscribe::succ() const {
  return succ_;
}
inline void RspSubscribe::set_succ(bool value) {
  set_has_succ();
  succ_ = value;
}

// -------------------------------------------------------------------

// ReqUnsubscribe

// repeated string symbols = 1;
inline int ReqUnsubscribe::symbols_size() const {
  return symbols_.size();
}
inline void ReqUnsubscribe::clear_symbols() {
  symbols_.Clear();
}
inline const ::std::string& ReqUnsubscribe::symbols(int index) const {
  return symbols_.Get(index);
}
inline ::std::string* ReqUnsubscribe::mutable_symbols(int index) {
  return symbols_.Mutable(index);
}
inline void ReqUnsubscribe::set_symbols(int index, const ::std::string& value) {
  symbols_.Mutable(index)->assign(value);
}
inline void ReqUnsubscribe::set_symbols(int index, const char* value) {
  symbols_.Mutable(index)->assign(value);
}
inline void ReqUnsubscribe::set_symbols(int index, const char* value, size_t size) {
  symbols_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReqUnsubscribe::add_symbols() {
  return symbols_.Add();
}
inline void ReqUnsubscribe::add_symbols(const ::std::string& value) {
  symbols_.Add()->assign(value);
}
inline void ReqUnsubscribe::add_symbols(const char* value) {
  symbols_.Add()->assign(value);
}
inline void ReqUnsubscribe::add_symbols(const char* value, size_t size) {
  symbols_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ReqUnsubscribe::symbols() const {
  return symbols_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ReqUnsubscribe::mutable_symbols() {
  return &symbols_;
}

// -------------------------------------------------------------------

// RspUnsubscribe

// required bool succ = 1;
inline bool RspUnsubscribe::has_succ() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RspUnsubscribe::set_has_succ() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RspUnsubscribe::clear_has_succ() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RspUnsubscribe::clear_succ() {
  succ_ = false;
  clear_has_succ();
}
inline bool RspUnsubscribe::succ() const {
  return succ_;
}
inline void RspUnsubscribe::set_succ(bool value) {
  set_has_succ();
  succ_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protoc

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_subscribe_2eproto__INCLUDED
