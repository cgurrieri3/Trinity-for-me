// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME EventInfoDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
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
#include "EventInfo.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_EventInfo(void *p = nullptr);
   static void *newArray_EventInfo(Long_t size, void *p);
   static void delete_EventInfo(void *p);
   static void deleteArray_EventInfo(void *p);
   static void destruct_EventInfo(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventInfo*)
   {
      ::EventInfo *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::EventInfo >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("EventInfo", ::EventInfo::Class_Version(), "EventInfo.h", 12,
                  typeid(::EventInfo), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::EventInfo::Dictionary, isa_proxy, 4,
                  sizeof(::EventInfo) );
      instance.SetNew(&new_EventInfo);
      instance.SetNewArray(&newArray_EventInfo);
      instance.SetDelete(&delete_EventInfo);
      instance.SetDeleteArray(&deleteArray_EventInfo);
      instance.SetDestructor(&destruct_EventInfo);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventInfo*)
   {
      return GenerateInitInstanceLocal(static_cast<::EventInfo*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::EventInfo*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr EventInfo::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *EventInfo::Class_Name()
{
   return "EventInfo";
}

//______________________________________________________________________________
const char *EventInfo::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventInfo*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int EventInfo::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventInfo*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *EventInfo::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventInfo*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *EventInfo::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventInfo*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void EventInfo::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventInfo.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(EventInfo::Class(),this);
   } else {
      R__b.WriteClassBuffer(EventInfo::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_EventInfo(void *p) {
      return  p ? new(p) ::EventInfo : new ::EventInfo;
   }
   static void *newArray_EventInfo(Long_t nElements, void *p) {
      return p ? new(p) ::EventInfo[nElements] : new ::EventInfo[nElements];
   }
   // Wrapper around operator delete
   static void delete_EventInfo(void *p) {
      delete (static_cast<::EventInfo*>(p));
   }
   static void deleteArray_EventInfo(void *p) {
      delete [] (static_cast<::EventInfo*>(p));
   }
   static void destruct_EventInfo(void *p) {
      typedef ::EventInfo current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::EventInfo

namespace {
  void TriggerDictionaryInitialization_EventInfoDict_Impl() {
    static const char* headers[] = {
"EventInfo.h",
nullptr
    };
    static const char* includePaths[] = {
"/opt/root/include/",
"/projects/cos-lab-aotte6/cos-4a17118/Trinity/TrinityDemonstrator/DataAnalysis/event_cleaning/EventSelection/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "EventInfoDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$EventInfo.h")))  EventInfo;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "EventInfoDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "EventInfo.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"EventInfo", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("EventInfoDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_EventInfoDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_EventInfoDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_EventInfoDict() {
  TriggerDictionaryInitialization_EventInfoDict_Impl();
}
