// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME T2KEventDict
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
#include "T2KEvent.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_T2KEvent(void *p = 0);
   static void *newArray_T2KEvent(Long_t size, void *p);
   static void delete_T2KEvent(void *p);
   static void deleteArray_T2KEvent(void *p);
   static void destruct_T2KEvent(void *p);
   static void streamer_T2KEvent(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::T2KEvent*)
   {
      ::T2KEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::T2KEvent >(0);
      static ::ROOT::TGenericClassInfo 
         instance("T2KEvent", ::T2KEvent::Class_Version(), "T2KEvent.h", 32,
                  typeid(::T2KEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::T2KEvent::Dictionary, isa_proxy, 16,
                  sizeof(::T2KEvent) );
      instance.SetNew(&new_T2KEvent);
      instance.SetNewArray(&newArray_T2KEvent);
      instance.SetDelete(&delete_T2KEvent);
      instance.SetDeleteArray(&deleteArray_T2KEvent);
      instance.SetDestructor(&destruct_T2KEvent);
      instance.SetStreamerFunc(&streamer_T2KEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::T2KEvent*)
   {
      return GenerateInitInstanceLocal((::T2KEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::T2KEvent*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr T2KEvent::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *T2KEvent::Class_Name()
{
   return "T2KEvent";
}

//______________________________________________________________________________
const char *T2KEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::T2KEvent*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int T2KEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::T2KEvent*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *T2KEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::T2KEvent*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *T2KEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::T2KEvent*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void T2KEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class T2KEvent.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fNChannels;
      R__b >> fNBins;
      R__b >> fNTBins;
      fData.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, T2KEvent::IsA());
   } else {
      R__c = R__b.WriteVersion(T2KEvent::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fNChannels;
      R__b << fNBins;
      R__b << fNTBins;
      fData.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_T2KEvent(void *p) {
      return  p ? new(p) ::T2KEvent : new ::T2KEvent;
   }
   static void *newArray_T2KEvent(Long_t nElements, void *p) {
      return p ? new(p) ::T2KEvent[nElements] : new ::T2KEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_T2KEvent(void *p) {
      delete ((::T2KEvent*)p);
   }
   static void deleteArray_T2KEvent(void *p) {
      delete [] ((::T2KEvent*)p);
   }
   static void destruct_T2KEvent(void *p) {
      typedef ::T2KEvent current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_T2KEvent(TBuffer &buf, void *obj) {
      ((::T2KEvent*)obj)->::T2KEvent::Streamer(buf);
   }
} // end of namespace ROOT for class ::T2KEvent

namespace {
  void TriggerDictionaryInitialization_T2KEventDict_Impl() {
    static const char* headers[] = {
"T2KEvent.h",
0
    };
    static const char* includePaths[] = {
"/usr/lib/root/include/",
"/home/cherenkov/Desktop/CoBoFrameViewer/src/graw2root/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "T2KEventDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$T2KEvent.h")))  T2KEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "T2KEventDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "T2KEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"T2KEvent", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("T2KEventDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_T2KEventDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_T2KEventDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_T2KEventDict() {
  TriggerDictionaryInitialization_T2KEventDict_Impl();
}
