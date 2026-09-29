// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME DictGET
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "GDataSample.h"
#include "GDataChannel.h"
#include "GFrameHeader.h"
#include "GDataFrame.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_GETcLcLGDataSample(void *p = 0);
   static void *newArray_GETcLcLGDataSample(Long_t size, void *p);
   static void delete_GETcLcLGDataSample(void *p);
   static void deleteArray_GETcLcLGDataSample(void *p);
   static void destruct_GETcLcLGDataSample(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GET::GDataSample*)
   {
      ::GET::GDataSample *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::GET::GDataSample >(0);
      static ::ROOT::TGenericClassInfo 
         instance("GET::GDataSample", ::GET::GDataSample::Class_Version(), "GDataSample.h", 55,
                  typeid(::GET::GDataSample), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::GET::GDataSample::Dictionary, isa_proxy, 4,
                  sizeof(::GET::GDataSample) );
      instance.SetNew(&new_GETcLcLGDataSample);
      instance.SetNewArray(&newArray_GETcLcLGDataSample);
      instance.SetDelete(&delete_GETcLcLGDataSample);
      instance.SetDeleteArray(&deleteArray_GETcLcLGDataSample);
      instance.SetDestructor(&destruct_GETcLcLGDataSample);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GET::GDataSample*)
   {
      return GenerateInitInstanceLocal((::GET::GDataSample*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GET::GDataSample*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_GETcLcLGDataChannel(void *p = 0);
   static void *newArray_GETcLcLGDataChannel(Long_t size, void *p);
   static void delete_GETcLcLGDataChannel(void *p);
   static void deleteArray_GETcLcLGDataChannel(void *p);
   static void destruct_GETcLcLGDataChannel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GET::GDataChannel*)
   {
      ::GET::GDataChannel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::GET::GDataChannel >(0);
      static ::ROOT::TGenericClassInfo 
         instance("GET::GDataChannel", ::GET::GDataChannel::Class_Version(), "GDataChannel.h", 58,
                  typeid(::GET::GDataChannel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::GET::GDataChannel::Dictionary, isa_proxy, 4,
                  sizeof(::GET::GDataChannel) );
      instance.SetNew(&new_GETcLcLGDataChannel);
      instance.SetNewArray(&newArray_GETcLcLGDataChannel);
      instance.SetDelete(&delete_GETcLcLGDataChannel);
      instance.SetDeleteArray(&deleteArray_GETcLcLGDataChannel);
      instance.SetDestructor(&destruct_GETcLcLGDataChannel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GET::GDataChannel*)
   {
      return GenerateInitInstanceLocal((::GET::GDataChannel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GET::GDataChannel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_GETcLcLGFrameHeader(void *p = 0);
   static void *newArray_GETcLcLGFrameHeader(Long_t size, void *p);
   static void delete_GETcLcLGFrameHeader(void *p);
   static void deleteArray_GETcLcLGFrameHeader(void *p);
   static void destruct_GETcLcLGFrameHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GET::GFrameHeader*)
   {
      ::GET::GFrameHeader *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::GET::GFrameHeader >(0);
      static ::ROOT::TGenericClassInfo 
         instance("GET::GFrameHeader", ::GET::GFrameHeader::Class_Version(), "GFrameHeader.h", 56,
                  typeid(::GET::GFrameHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::GET::GFrameHeader::Dictionary, isa_proxy, 4,
                  sizeof(::GET::GFrameHeader) );
      instance.SetNew(&new_GETcLcLGFrameHeader);
      instance.SetNewArray(&newArray_GETcLcLGFrameHeader);
      instance.SetDelete(&delete_GETcLcLGFrameHeader);
      instance.SetDeleteArray(&deleteArray_GETcLcLGFrameHeader);
      instance.SetDestructor(&destruct_GETcLcLGFrameHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GET::GFrameHeader*)
   {
      return GenerateInitInstanceLocal((::GET::GFrameHeader*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GET::GFrameHeader*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_GETcLcLGDataFrame(void *p = 0);
   static void *newArray_GETcLcLGDataFrame(Long_t size, void *p);
   static void delete_GETcLcLGDataFrame(void *p);
   static void deleteArray_GETcLcLGDataFrame(void *p);
   static void destruct_GETcLcLGDataFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GET::GDataFrame*)
   {
      ::GET::GDataFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::GET::GDataFrame >(0);
      static ::ROOT::TGenericClassInfo 
         instance("GET::GDataFrame", ::GET::GDataFrame::Class_Version(), "GDataFrame.h", 61,
                  typeid(::GET::GDataFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::GET::GDataFrame::Dictionary, isa_proxy, 4,
                  sizeof(::GET::GDataFrame) );
      instance.SetNew(&new_GETcLcLGDataFrame);
      instance.SetNewArray(&newArray_GETcLcLGDataFrame);
      instance.SetDelete(&delete_GETcLcLGDataFrame);
      instance.SetDeleteArray(&deleteArray_GETcLcLGDataFrame);
      instance.SetDestructor(&destruct_GETcLcLGDataFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GET::GDataFrame*)
   {
      return GenerateInitInstanceLocal((::GET::GDataFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GET::GDataFrame*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace GET {
//______________________________________________________________________________
atomic_TClass_ptr GDataSample::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *GDataSample::Class_Name()
{
   return "GET::GDataSample";
}

//______________________________________________________________________________
const char *GDataSample::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataSample*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int GDataSample::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataSample*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *GDataSample::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataSample*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *GDataSample::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataSample*)0x0)->GetClass(); }
   return fgIsA;
}

} // namespace GET
namespace GET {
//______________________________________________________________________________
atomic_TClass_ptr GDataChannel::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *GDataChannel::Class_Name()
{
   return "GET::GDataChannel";
}

//______________________________________________________________________________
const char *GDataChannel::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataChannel*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int GDataChannel::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataChannel*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *GDataChannel::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataChannel*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *GDataChannel::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataChannel*)0x0)->GetClass(); }
   return fgIsA;
}

} // namespace GET
namespace GET {
//______________________________________________________________________________
atomic_TClass_ptr GFrameHeader::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *GFrameHeader::Class_Name()
{
   return "GET::GFrameHeader";
}

//______________________________________________________________________________
const char *GFrameHeader::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GFrameHeader*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int GFrameHeader::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GFrameHeader*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *GFrameHeader::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GFrameHeader*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *GFrameHeader::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GFrameHeader*)0x0)->GetClass(); }
   return fgIsA;
}

} // namespace GET
namespace GET {
//______________________________________________________________________________
atomic_TClass_ptr GDataFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *GDataFrame::Class_Name()
{
   return "GET::GDataFrame";
}

//______________________________________________________________________________
const char *GDataFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int GDataFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *GDataFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *GDataFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::GDataFrame*)0x0)->GetClass(); }
   return fgIsA;
}

} // namespace GET
namespace GET {
//______________________________________________________________________________
void GDataSample::Streamer(TBuffer &R__b)
{
   // Stream an object of class GET::GDataSample.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(GET::GDataSample::Class(),this);
   } else {
      R__b.WriteClassBuffer(GET::GDataSample::Class(),this);
   }
}

} // namespace GET
namespace ROOT {
   // Wrappers around operator new
   static void *new_GETcLcLGDataSample(void *p) {
      return  p ? new(p) ::GET::GDataSample : new ::GET::GDataSample;
   }
   static void *newArray_GETcLcLGDataSample(Long_t nElements, void *p) {
      return p ? new(p) ::GET::GDataSample[nElements] : new ::GET::GDataSample[nElements];
   }
   // Wrapper around operator delete
   static void delete_GETcLcLGDataSample(void *p) {
      delete ((::GET::GDataSample*)p);
   }
   static void deleteArray_GETcLcLGDataSample(void *p) {
      delete [] ((::GET::GDataSample*)p);
   }
   static void destruct_GETcLcLGDataSample(void *p) {
      typedef ::GET::GDataSample current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GET::GDataSample

namespace GET {
//______________________________________________________________________________
void GDataChannel::Streamer(TBuffer &R__b)
{
   // Stream an object of class GET::GDataChannel.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(GET::GDataChannel::Class(),this);
   } else {
      R__b.WriteClassBuffer(GET::GDataChannel::Class(),this);
   }
}

} // namespace GET
namespace ROOT {
   // Wrappers around operator new
   static void *new_GETcLcLGDataChannel(void *p) {
      return  p ? new(p) ::GET::GDataChannel : new ::GET::GDataChannel;
   }
   static void *newArray_GETcLcLGDataChannel(Long_t nElements, void *p) {
      return p ? new(p) ::GET::GDataChannel[nElements] : new ::GET::GDataChannel[nElements];
   }
   // Wrapper around operator delete
   static void delete_GETcLcLGDataChannel(void *p) {
      delete ((::GET::GDataChannel*)p);
   }
   static void deleteArray_GETcLcLGDataChannel(void *p) {
      delete [] ((::GET::GDataChannel*)p);
   }
   static void destruct_GETcLcLGDataChannel(void *p) {
      typedef ::GET::GDataChannel current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GET::GDataChannel

namespace GET {
//______________________________________________________________________________
void GFrameHeader::Streamer(TBuffer &R__b)
{
   // Stream an object of class GET::GFrameHeader.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(GET::GFrameHeader::Class(),this);
   } else {
      R__b.WriteClassBuffer(GET::GFrameHeader::Class(),this);
   }
}

} // namespace GET
namespace ROOT {
   // Wrappers around operator new
   static void *new_GETcLcLGFrameHeader(void *p) {
      return  p ? new(p) ::GET::GFrameHeader : new ::GET::GFrameHeader;
   }
   static void *newArray_GETcLcLGFrameHeader(Long_t nElements, void *p) {
      return p ? new(p) ::GET::GFrameHeader[nElements] : new ::GET::GFrameHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_GETcLcLGFrameHeader(void *p) {
      delete ((::GET::GFrameHeader*)p);
   }
   static void deleteArray_GETcLcLGFrameHeader(void *p) {
      delete [] ((::GET::GFrameHeader*)p);
   }
   static void destruct_GETcLcLGFrameHeader(void *p) {
      typedef ::GET::GFrameHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GET::GFrameHeader

namespace GET {
//______________________________________________________________________________
void GDataFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class GET::GDataFrame.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(GET::GDataFrame::Class(),this);
   } else {
      R__b.WriteClassBuffer(GET::GDataFrame::Class(),this);
   }
}

} // namespace GET
namespace ROOT {
   // Wrappers around operator new
   static void *new_GETcLcLGDataFrame(void *p) {
      return  p ? new(p) ::GET::GDataFrame : new ::GET::GDataFrame;
   }
   static void *newArray_GETcLcLGDataFrame(Long_t nElements, void *p) {
      return p ? new(p) ::GET::GDataFrame[nElements] : new ::GET::GDataFrame[nElements];
   }
   // Wrapper around operator delete
   static void delete_GETcLcLGDataFrame(void *p) {
      delete ((::GET::GDataFrame*)p);
   }
   static void deleteArray_GETcLcLGDataFrame(void *p) {
      delete [] ((::GET::GDataFrame*)p);
   }
   static void destruct_GETcLcLGDataFrame(void *p) {
      typedef ::GET::GDataFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GET::GDataFrame

namespace {
  void TriggerDictionaryInitialization_DictGET_Impl() {
    static const char* headers[] = {
"GDataSample.h",
"GDataChannel.h",
"GFrameHeader.h",
"GDataFrame.h",
0
    };
    static const char* includePaths[] = {
"/usr/lib/root/include/",
"/home/cherenkov/Desktop/CoBoFrameViewer/src/root/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "DictGET dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
namespace GET{class __attribute__((annotate("$clingAutoload$GDataSample.h")))  GDataSample;}
namespace GET{class __attribute__((annotate("$clingAutoload$GDataChannel.h")))  GDataChannel;}
namespace GET{class __attribute__((annotate("$clingAutoload$GFrameHeader.h")))  GFrameHeader;}
namespace GET{class __attribute__((annotate("$clingAutoload$GDataFrame.h")))  GDataFrame;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "DictGET dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "GDataSample.h"
#include "GDataChannel.h"
#include "GFrameHeader.h"
#include "GDataFrame.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"GET::GDataChannel", payloadCode, "@",
"GET::GDataFrame", payloadCode, "@",
"GET::GDataSample", payloadCode, "@",
"GET::GFrameHeader", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("DictGET",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_DictGET_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_DictGET_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_DictGET() {
  TriggerDictionaryInitialization_DictGET_Impl();
}
