// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME DictGuiGET
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
#include "CoBoFrameBrowser.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_GETcLcLCoBoFrameBrowser(void *p);
   static void deleteArray_GETcLcLCoBoFrameBrowser(void *p);
   static void destruct_GETcLcLCoBoFrameBrowser(void *p);
   static void streamer_GETcLcLCoBoFrameBrowser(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GET::CoBoFrameBrowser*)
   {
      ::GET::CoBoFrameBrowser *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::GET::CoBoFrameBrowser >(0);
      static ::ROOT::TGenericClassInfo 
         instance("GET::CoBoFrameBrowser", ::GET::CoBoFrameBrowser::Class_Version(), "CoBoFrameBrowser.h", 59,
                  typeid(::GET::CoBoFrameBrowser), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::GET::CoBoFrameBrowser::Dictionary, isa_proxy, 16,
                  sizeof(::GET::CoBoFrameBrowser) );
      instance.SetDelete(&delete_GETcLcLCoBoFrameBrowser);
      instance.SetDeleteArray(&deleteArray_GETcLcLCoBoFrameBrowser);
      instance.SetDestructor(&destruct_GETcLcLCoBoFrameBrowser);
      instance.SetStreamerFunc(&streamer_GETcLcLCoBoFrameBrowser);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GET::CoBoFrameBrowser*)
   {
      return GenerateInitInstanceLocal((::GET::CoBoFrameBrowser*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GET::CoBoFrameBrowser*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace GET {
//______________________________________________________________________________
atomic_TClass_ptr CoBoFrameBrowser::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *CoBoFrameBrowser::Class_Name()
{
   return "GET::CoBoFrameBrowser";
}

//______________________________________________________________________________
const char *CoBoFrameBrowser::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::CoBoFrameBrowser*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int CoBoFrameBrowser::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::GET::CoBoFrameBrowser*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *CoBoFrameBrowser::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::CoBoFrameBrowser*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *CoBoFrameBrowser::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::GET::CoBoFrameBrowser*)0x0)->GetClass(); }
   return fgIsA;
}

} // namespace GET
namespace GET {
//______________________________________________________________________________
void CoBoFrameBrowser::Streamer(TBuffer &R__b)
{
   // Stream an object of class GET::CoBoFrameBrowser.

   //This works around a msvc bug and should be harmless on other platforms
   typedef ::GET::CoBoFrameBrowser thisClass;
   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      fQObject.Streamer(R__b);
      R__b >> fMainFrame;
      R__b >> fCanvas;
      R__b >> fEcanvas;
      R__b >> fButPrev;
      R__b >> fButNext;
      R__b >> fButGoTo;
      R__b >> fBut3D;
      R__b >> fButExit;
      R__b >> fFile;
      R__b >> fTree;
      R__b >> fEntries;
      R__b >> fCurrentId;
      int R__i;
      for (R__i = 0; R__i < 4; R__i++)
         R__b >> multiGraph[R__i];
      for (R__i = 0; R__i < 4; R__i++)
         R__b >> histo[R__i];
      R__b >> fFrame;
      R__b >> isUpToDate;
      drawOption.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, thisClass::IsA());
   } else {
      R__c = R__b.WriteVersion(thisClass::IsA(), kTRUE);
      fQObject.Streamer(R__b);
      R__b << fMainFrame;
      R__b << fCanvas;
      R__b << fEcanvas;
      R__b << fButPrev;
      R__b << fButNext;
      R__b << fButGoTo;
      R__b << fBut3D;
      R__b << fButExit;
      R__b << fFile;
      R__b << fTree;
      R__b << fEntries;
      R__b << fCurrentId;
      int R__i;
      for (R__i = 0; R__i < 4; R__i++)
         R__b << multiGraph[R__i];
      for (R__i = 0; R__i < 4; R__i++)
         R__b << (TObject*)histo[R__i];
      R__b << fFrame;
      R__b << isUpToDate;
      drawOption.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

} // namespace GET
namespace ROOT {
   // Wrapper around operator delete
   static void delete_GETcLcLCoBoFrameBrowser(void *p) {
      delete ((::GET::CoBoFrameBrowser*)p);
   }
   static void deleteArray_GETcLcLCoBoFrameBrowser(void *p) {
      delete [] ((::GET::CoBoFrameBrowser*)p);
   }
   static void destruct_GETcLcLCoBoFrameBrowser(void *p) {
      typedef ::GET::CoBoFrameBrowser current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_GETcLcLCoBoFrameBrowser(TBuffer &buf, void *obj) {
      ((::GET::CoBoFrameBrowser*)obj)->::GET::CoBoFrameBrowser::Streamer(buf);
   }
} // end of namespace ROOT for class ::GET::CoBoFrameBrowser

namespace {
  void TriggerDictionaryInitialization_DictGuiGET_Impl() {
    static const char* headers[] = {
"CoBoFrameBrowser.h",
0
    };
    static const char* includePaths[] = {
"../../src/root",
"/usr/lib/root/include/",
"/home/cherenkov/Desktop/CoBoFrameViewer/src/root2disp/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "DictGuiGET dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
namespace GET{class __attribute__((annotate("$clingAutoload$CoBoFrameBrowser.h")))  CoBoFrameBrowser;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "DictGuiGET dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "CoBoFrameBrowser.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"GET::CoBoFrameBrowser", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("DictGuiGET",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_DictGuiGET_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_DictGuiGET_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_DictGuiGET() {
  TriggerDictionaryInitialization_DictGuiGET_Impl();
}
