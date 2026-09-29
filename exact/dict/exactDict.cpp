// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dictdIexactDict
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
#include "BiFocal.h"
#include "CEvent.h"
#include "Event.h"
#include "ExtractedData.h"
#include "GoldPlated.h"
#include "ICalibration.h"
#include "IEvent.h"
#include "IFile.h"
#include "IHealthTools.h"
#include "IPlotTools.h"
#include "ISiPM.h"
#include "ISims.h"
#include "IUtilities.h"
#include "Pulse.h"
#include "ReadConfiguration.h"
#include "SEvent.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *ReadConfiguration_Dictionary();
   static void ReadConfiguration_TClassManip(TClass*);
   static void delete_ReadConfiguration(void *p);
   static void deleteArray_ReadConfiguration(void *p);
   static void destruct_ReadConfiguration(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ReadConfiguration*)
   {
      ::ReadConfiguration *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ReadConfiguration));
      static ::ROOT::TGenericClassInfo 
         instance("ReadConfiguration", "ReadConfiguration.h", 15,
                  typeid(::ReadConfiguration), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ReadConfiguration_Dictionary, isa_proxy, 4,
                  sizeof(::ReadConfiguration) );
      instance.SetDelete(&delete_ReadConfiguration);
      instance.SetDeleteArray(&deleteArray_ReadConfiguration);
      instance.SetDestructor(&destruct_ReadConfiguration);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ReadConfiguration*)
   {
      return GenerateInitInstanceLocal(static_cast<::ReadConfiguration*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ReadConfiguration*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ReadConfiguration_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ReadConfiguration*>(nullptr))->GetClass();
      ReadConfiguration_TClassManip(theClass);
   return theClass;
   }

   static void ReadConfiguration_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ExtractedData_Dictionary();
   static void ExtractedData_TClassManip(TClass*);
   static void *new_ExtractedData(void *p = nullptr);
   static void *newArray_ExtractedData(Long_t size, void *p);
   static void delete_ExtractedData(void *p);
   static void deleteArray_ExtractedData(void *p);
   static void destruct_ExtractedData(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ExtractedData*)
   {
      ::ExtractedData *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ExtractedData));
      static ::ROOT::TGenericClassInfo 
         instance("ExtractedData", "ExtractedData.h", 9,
                  typeid(::ExtractedData), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ExtractedData_Dictionary, isa_proxy, 4,
                  sizeof(::ExtractedData) );
      instance.SetNew(&new_ExtractedData);
      instance.SetNewArray(&newArray_ExtractedData);
      instance.SetDelete(&delete_ExtractedData);
      instance.SetDeleteArray(&deleteArray_ExtractedData);
      instance.SetDestructor(&destruct_ExtractedData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ExtractedData*)
   {
      return GenerateInitInstanceLocal(static_cast<::ExtractedData*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ExtractedData*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ExtractedData_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ExtractedData*>(nullptr))->GetClass();
      ExtractedData_TClassManip(theClass);
   return theClass;
   }

   static void ExtractedData_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BiFocal_Dictionary();
   static void BiFocal_TClassManip(TClass*);
   static void *new_BiFocal(void *p = nullptr);
   static void *newArray_BiFocal(Long_t size, void *p);
   static void delete_BiFocal(void *p);
   static void deleteArray_BiFocal(void *p);
   static void destruct_BiFocal(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BiFocal*)
   {
      ::BiFocal *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BiFocal));
      static ::ROOT::TGenericClassInfo 
         instance("BiFocal", "BiFocal.h", 11,
                  typeid(::BiFocal), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &BiFocal_Dictionary, isa_proxy, 4,
                  sizeof(::BiFocal) );
      instance.SetNew(&new_BiFocal);
      instance.SetNewArray(&newArray_BiFocal);
      instance.SetDelete(&delete_BiFocal);
      instance.SetDeleteArray(&deleteArray_BiFocal);
      instance.SetDestructor(&destruct_BiFocal);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BiFocal*)
   {
      return GenerateInitInstanceLocal(static_cast<::BiFocal*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::BiFocal*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BiFocal_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::BiFocal*>(nullptr))->GetClass();
      BiFocal_TClassManip(theClass);
   return theClass;
   }

   static void BiFocal_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *IPlotTools_Dictionary();
   static void IPlotTools_TClassManip(TClass*);
   static void *new_IPlotTools(void *p = nullptr);
   static void *newArray_IPlotTools(Long_t size, void *p);
   static void delete_IPlotTools(void *p);
   static void deleteArray_IPlotTools(void *p);
   static void destruct_IPlotTools(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::IPlotTools*)
   {
      ::IPlotTools *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::IPlotTools));
      static ::ROOT::TGenericClassInfo 
         instance("IPlotTools", "IPlotTools.h", 7,
                  typeid(::IPlotTools), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &IPlotTools_Dictionary, isa_proxy, 4,
                  sizeof(::IPlotTools) );
      instance.SetNew(&new_IPlotTools);
      instance.SetNewArray(&newArray_IPlotTools);
      instance.SetDelete(&delete_IPlotTools);
      instance.SetDeleteArray(&deleteArray_IPlotTools);
      instance.SetDestructor(&destruct_IPlotTools);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::IPlotTools*)
   {
      return GenerateInitInstanceLocal(static_cast<::IPlotTools*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::IPlotTools*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *IPlotTools_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::IPlotTools*>(nullptr))->GetClass();
      IPlotTools_TClassManip(theClass);
   return theClass;
   }

   static void IPlotTools_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *IUtilities_Dictionary();
   static void IUtilities_TClassManip(TClass*);
   static void *new_IUtilities(void *p = nullptr);
   static void *newArray_IUtilities(Long_t size, void *p);
   static void delete_IUtilities(void *p);
   static void deleteArray_IUtilities(void *p);
   static void destruct_IUtilities(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::IUtilities*)
   {
      ::IUtilities *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::IUtilities));
      static ::ROOT::TGenericClassInfo 
         instance("IUtilities", "IUtilities.h", 18,
                  typeid(::IUtilities), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &IUtilities_Dictionary, isa_proxy, 4,
                  sizeof(::IUtilities) );
      instance.SetNew(&new_IUtilities);
      instance.SetNewArray(&newArray_IUtilities);
      instance.SetDelete(&delete_IUtilities);
      instance.SetDeleteArray(&deleteArray_IUtilities);
      instance.SetDestructor(&destruct_IUtilities);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::IUtilities*)
   {
      return GenerateInitInstanceLocal(static_cast<::IUtilities*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::IUtilities*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *IUtilities_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::IUtilities*>(nullptr))->GetClass();
      IUtilities_TClassManip(theClass);
   return theClass;
   }

   static void IUtilities_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Event_Dictionary();
   static void Event_TClassManip(TClass*);
   static void *new_Event(void *p = nullptr);
   static void *newArray_Event(Long_t size, void *p);
   static void delete_Event(void *p);
   static void deleteArray_Event(void *p);
   static void destruct_Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Event*)
   {
      ::Event *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Event));
      static ::ROOT::TGenericClassInfo 
         instance("Event", "Event.h", 16,
                  typeid(::Event), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Event_Dictionary, isa_proxy, 4,
                  sizeof(::Event) );
      instance.SetNew(&new_Event);
      instance.SetNewArray(&newArray_Event);
      instance.SetDelete(&delete_Event);
      instance.SetDeleteArray(&deleteArray_Event);
      instance.SetDestructor(&destruct_Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Event*)
   {
      return GenerateInitInstanceLocal(static_cast<::Event*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::Event*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Event_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::Event*>(nullptr))->GetClass();
      Event_TClassManip(theClass);
   return theClass;
   }

   static void Event_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *GoldPlated_Dictionary();
   static void GoldPlated_TClassManip(TClass*);
   static void *new_GoldPlated(void *p = nullptr);
   static void *newArray_GoldPlated(Long_t size, void *p);
   static void delete_GoldPlated(void *p);
   static void deleteArray_GoldPlated(void *p);
   static void destruct_GoldPlated(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GoldPlated*)
   {
      ::GoldPlated *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::GoldPlated));
      static ::ROOT::TGenericClassInfo 
         instance("GoldPlated", "GoldPlated.h", 11,
                  typeid(::GoldPlated), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &GoldPlated_Dictionary, isa_proxy, 4,
                  sizeof(::GoldPlated) );
      instance.SetNew(&new_GoldPlated);
      instance.SetNewArray(&newArray_GoldPlated);
      instance.SetDelete(&delete_GoldPlated);
      instance.SetDeleteArray(&deleteArray_GoldPlated);
      instance.SetDestructor(&destruct_GoldPlated);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GoldPlated*)
   {
      return GenerateInitInstanceLocal(static_cast<::GoldPlated*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::GoldPlated*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *GoldPlated_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::GoldPlated*>(nullptr))->GetClass();
      GoldPlated_TClassManip(theClass);
   return theClass;
   }

   static void GoldPlated_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ICalibration_Dictionary();
   static void ICalibration_TClassManip(TClass*);
   static void delete_ICalibration(void *p);
   static void deleteArray_ICalibration(void *p);
   static void destruct_ICalibration(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ICalibration*)
   {
      ::ICalibration *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ICalibration));
      static ::ROOT::TGenericClassInfo 
         instance("ICalibration", "ICalibration.h", 58,
                  typeid(::ICalibration), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ICalibration_Dictionary, isa_proxy, 4,
                  sizeof(::ICalibration) );
      instance.SetDelete(&delete_ICalibration);
      instance.SetDeleteArray(&deleteArray_ICalibration);
      instance.SetDestructor(&destruct_ICalibration);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ICalibration*)
   {
      return GenerateInitInstanceLocal(static_cast<::ICalibration*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ICalibration*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ICalibration_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ICalibration*>(nullptr))->GetClass();
      ICalibration_TClassManip(theClass);
   return theClass;
   }

   static void ICalibration_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *IEvent_Dictionary();
   static void IEvent_TClassManip(TClass*);
   static void *new_IEvent(void *p = nullptr);
   static void *newArray_IEvent(Long_t size, void *p);
   static void delete_IEvent(void *p);
   static void deleteArray_IEvent(void *p);
   static void destruct_IEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::IEvent*)
   {
      ::IEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::IEvent));
      static ::ROOT::TGenericClassInfo 
         instance("IEvent", "IEvent.h", 19,
                  typeid(::IEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &IEvent_Dictionary, isa_proxy, 4,
                  sizeof(::IEvent) );
      instance.SetNew(&new_IEvent);
      instance.SetNewArray(&newArray_IEvent);
      instance.SetDelete(&delete_IEvent);
      instance.SetDeleteArray(&deleteArray_IEvent);
      instance.SetDestructor(&destruct_IEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::IEvent*)
   {
      return GenerateInitInstanceLocal(static_cast<::IEvent*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::IEvent*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *IEvent_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::IEvent*>(nullptr))->GetClass();
      IEvent_TClassManip(theClass);
   return theClass;
   }

   static void IEvent_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *IFile_Dictionary();
   static void IFile_TClassManip(TClass*);
   static void delete_IFile(void *p);
   static void deleteArray_IFile(void *p);
   static void destruct_IFile(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::IFile*)
   {
      ::IFile *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::IFile));
      static ::ROOT::TGenericClassInfo 
         instance("IFile", "IFile.h", 25,
                  typeid(::IFile), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &IFile_Dictionary, isa_proxy, 4,
                  sizeof(::IFile) );
      instance.SetDelete(&delete_IFile);
      instance.SetDeleteArray(&deleteArray_IFile);
      instance.SetDestructor(&destruct_IFile);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::IFile*)
   {
      return GenerateInitInstanceLocal(static_cast<::IFile*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::IFile*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *IFile_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::IFile*>(nullptr))->GetClass();
      IFile_TClassManip(theClass);
   return theClass;
   }

   static void IFile_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Pulse_Dictionary();
   static void Pulse_TClassManip(TClass*);
   static void delete_Pulse(void *p);
   static void deleteArray_Pulse(void *p);
   static void destruct_Pulse(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Pulse*)
   {
      ::Pulse *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Pulse));
      static ::ROOT::TGenericClassInfo 
         instance("Pulse", "Pulse.h", 10,
                  typeid(::Pulse), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Pulse_Dictionary, isa_proxy, 4,
                  sizeof(::Pulse) );
      instance.SetDelete(&delete_Pulse);
      instance.SetDeleteArray(&deleteArray_Pulse);
      instance.SetDestructor(&destruct_Pulse);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Pulse*)
   {
      return GenerateInitInstanceLocal(static_cast<::Pulse*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::Pulse*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Pulse_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::Pulse*>(nullptr))->GetClass();
      Pulse_TClassManip(theClass);
   return theClass;
   }

   static void Pulse_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *IHealthTools_Dictionary();
   static void IHealthTools_TClassManip(TClass*);
   static void *new_IHealthTools(void *p = nullptr);
   static void *newArray_IHealthTools(Long_t size, void *p);
   static void delete_IHealthTools(void *p);
   static void deleteArray_IHealthTools(void *p);
   static void destruct_IHealthTools(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::IHealthTools*)
   {
      ::IHealthTools *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::IHealthTools));
      static ::ROOT::TGenericClassInfo 
         instance("IHealthTools", "IHealthTools.h", 11,
                  typeid(::IHealthTools), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &IHealthTools_Dictionary, isa_proxy, 4,
                  sizeof(::IHealthTools) );
      instance.SetNew(&new_IHealthTools);
      instance.SetNewArray(&newArray_IHealthTools);
      instance.SetDelete(&delete_IHealthTools);
      instance.SetDeleteArray(&deleteArray_IHealthTools);
      instance.SetDestructor(&destruct_IHealthTools);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::IHealthTools*)
   {
      return GenerateInitInstanceLocal(static_cast<::IHealthTools*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::IHealthTools*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *IHealthTools_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::IHealthTools*>(nullptr))->GetClass();
      IHealthTools_TClassManip(theClass);
   return theClass;
   }

   static void IHealthTools_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ISiPM_Dictionary();
   static void ISiPM_TClassManip(TClass*);
   static void *new_ISiPM(void *p = nullptr);
   static void *newArray_ISiPM(Long_t size, void *p);
   static void delete_ISiPM(void *p);
   static void deleteArray_ISiPM(void *p);
   static void destruct_ISiPM(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ISiPM*)
   {
      ::ISiPM *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ISiPM));
      static ::ROOT::TGenericClassInfo 
         instance("ISiPM", "ISiPM.h", 7,
                  typeid(::ISiPM), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ISiPM_Dictionary, isa_proxy, 4,
                  sizeof(::ISiPM) );
      instance.SetNew(&new_ISiPM);
      instance.SetNewArray(&newArray_ISiPM);
      instance.SetDelete(&delete_ISiPM);
      instance.SetDeleteArray(&deleteArray_ISiPM);
      instance.SetDestructor(&destruct_ISiPM);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ISiPM*)
   {
      return GenerateInitInstanceLocal(static_cast<::ISiPM*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ISiPM*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ISiPM_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ISiPM*>(nullptr))->GetClass();
      ISiPM_TClassManip(theClass);
   return theClass;
   }

   static void ISiPM_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ISims_Dictionary();
   static void ISims_TClassManip(TClass*);
   static void *new_ISims(void *p = nullptr);
   static void *newArray_ISims(Long_t size, void *p);
   static void delete_ISims(void *p);
   static void deleteArray_ISims(void *p);
   static void destruct_ISims(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ISims*)
   {
      ::ISims *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ISims));
      static ::ROOT::TGenericClassInfo 
         instance("ISims", "ISims.h", 11,
                  typeid(::ISims), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ISims_Dictionary, isa_proxy, 4,
                  sizeof(::ISims) );
      instance.SetNew(&new_ISims);
      instance.SetNewArray(&newArray_ISims);
      instance.SetDelete(&delete_ISims);
      instance.SetDeleteArray(&deleteArray_ISims);
      instance.SetDestructor(&destruct_ISims);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ISims*)
   {
      return GenerateInitInstanceLocal(static_cast<::ISims*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ISims*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ISims_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ISims*>(nullptr))->GetClass();
      ISims_TClassManip(theClass);
   return theClass;
   }

   static void ISims_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static void *new_SEvent(void *p = nullptr);
   static void *newArray_SEvent(Long_t size, void *p);
   static void delete_SEvent(void *p);
   static void deleteArray_SEvent(void *p);
   static void destruct_SEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SEvent*)
   {
      ::SEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SEvent >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("SEvent", ::SEvent::Class_Version(), "SEvent.h", 11,
                  typeid(::SEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::SEvent::Dictionary, isa_proxy, 4,
                  sizeof(::SEvent) );
      instance.SetNew(&new_SEvent);
      instance.SetNewArray(&newArray_SEvent);
      instance.SetDelete(&delete_SEvent);
      instance.SetDeleteArray(&deleteArray_SEvent);
      instance.SetDestructor(&destruct_SEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SEvent*)
   {
      return GenerateInitInstanceLocal(static_cast<::SEvent*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::SEvent*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr SEvent::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *SEvent::Class_Name()
{
   return "SEvent";
}

//______________________________________________________________________________
const char *SEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SEvent*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int SEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SEvent*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SEvent*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SEvent*)nullptr)->GetClass(); }
   return fgIsA;
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ReadConfiguration(void *p) {
      delete (static_cast<::ReadConfiguration*>(p));
   }
   static void deleteArray_ReadConfiguration(void *p) {
      delete [] (static_cast<::ReadConfiguration*>(p));
   }
   static void destruct_ReadConfiguration(void *p) {
      typedef ::ReadConfiguration current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ReadConfiguration

namespace ROOT {
   // Wrappers around operator new
   static void *new_ExtractedData(void *p) {
      return  p ? new(p) ::ExtractedData : new ::ExtractedData;
   }
   static void *newArray_ExtractedData(Long_t nElements, void *p) {
      return p ? new(p) ::ExtractedData[nElements] : new ::ExtractedData[nElements];
   }
   // Wrapper around operator delete
   static void delete_ExtractedData(void *p) {
      delete (static_cast<::ExtractedData*>(p));
   }
   static void deleteArray_ExtractedData(void *p) {
      delete [] (static_cast<::ExtractedData*>(p));
   }
   static void destruct_ExtractedData(void *p) {
      typedef ::ExtractedData current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ExtractedData

namespace ROOT {
   // Wrappers around operator new
   static void *new_BiFocal(void *p) {
      return  p ? new(p) ::BiFocal : new ::BiFocal;
   }
   static void *newArray_BiFocal(Long_t nElements, void *p) {
      return p ? new(p) ::BiFocal[nElements] : new ::BiFocal[nElements];
   }
   // Wrapper around operator delete
   static void delete_BiFocal(void *p) {
      delete (static_cast<::BiFocal*>(p));
   }
   static void deleteArray_BiFocal(void *p) {
      delete [] (static_cast<::BiFocal*>(p));
   }
   static void destruct_BiFocal(void *p) {
      typedef ::BiFocal current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::BiFocal

namespace ROOT {
   // Wrappers around operator new
   static void *new_IPlotTools(void *p) {
      return  p ? new(p) ::IPlotTools : new ::IPlotTools;
   }
   static void *newArray_IPlotTools(Long_t nElements, void *p) {
      return p ? new(p) ::IPlotTools[nElements] : new ::IPlotTools[nElements];
   }
   // Wrapper around operator delete
   static void delete_IPlotTools(void *p) {
      delete (static_cast<::IPlotTools*>(p));
   }
   static void deleteArray_IPlotTools(void *p) {
      delete [] (static_cast<::IPlotTools*>(p));
   }
   static void destruct_IPlotTools(void *p) {
      typedef ::IPlotTools current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::IPlotTools

namespace ROOT {
   // Wrappers around operator new
   static void *new_IUtilities(void *p) {
      return  p ? new(p) ::IUtilities : new ::IUtilities;
   }
   static void *newArray_IUtilities(Long_t nElements, void *p) {
      return p ? new(p) ::IUtilities[nElements] : new ::IUtilities[nElements];
   }
   // Wrapper around operator delete
   static void delete_IUtilities(void *p) {
      delete (static_cast<::IUtilities*>(p));
   }
   static void deleteArray_IUtilities(void *p) {
      delete [] (static_cast<::IUtilities*>(p));
   }
   static void destruct_IUtilities(void *p) {
      typedef ::IUtilities current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::IUtilities

namespace ROOT {
   // Wrappers around operator new
   static void *new_Event(void *p) {
      return  p ? new(p) ::Event : new ::Event;
   }
   static void *newArray_Event(Long_t nElements, void *p) {
      return p ? new(p) ::Event[nElements] : new ::Event[nElements];
   }
   // Wrapper around operator delete
   static void delete_Event(void *p) {
      delete (static_cast<::Event*>(p));
   }
   static void deleteArray_Event(void *p) {
      delete [] (static_cast<::Event*>(p));
   }
   static void destruct_Event(void *p) {
      typedef ::Event current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::Event

namespace ROOT {
   // Wrappers around operator new
   static void *new_GoldPlated(void *p) {
      return  p ? new(p) ::GoldPlated : new ::GoldPlated;
   }
   static void *newArray_GoldPlated(Long_t nElements, void *p) {
      return p ? new(p) ::GoldPlated[nElements] : new ::GoldPlated[nElements];
   }
   // Wrapper around operator delete
   static void delete_GoldPlated(void *p) {
      delete (static_cast<::GoldPlated*>(p));
   }
   static void deleteArray_GoldPlated(void *p) {
      delete [] (static_cast<::GoldPlated*>(p));
   }
   static void destruct_GoldPlated(void *p) {
      typedef ::GoldPlated current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::GoldPlated

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ICalibration(void *p) {
      delete (static_cast<::ICalibration*>(p));
   }
   static void deleteArray_ICalibration(void *p) {
      delete [] (static_cast<::ICalibration*>(p));
   }
   static void destruct_ICalibration(void *p) {
      typedef ::ICalibration current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ICalibration

namespace ROOT {
   // Wrappers around operator new
   static void *new_IEvent(void *p) {
      return  p ? new(p) ::IEvent : new ::IEvent;
   }
   static void *newArray_IEvent(Long_t nElements, void *p) {
      return p ? new(p) ::IEvent[nElements] : new ::IEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_IEvent(void *p) {
      delete (static_cast<::IEvent*>(p));
   }
   static void deleteArray_IEvent(void *p) {
      delete [] (static_cast<::IEvent*>(p));
   }
   static void destruct_IEvent(void *p) {
      typedef ::IEvent current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::IEvent

namespace ROOT {
   // Wrapper around operator delete
   static void delete_IFile(void *p) {
      delete (static_cast<::IFile*>(p));
   }
   static void deleteArray_IFile(void *p) {
      delete [] (static_cast<::IFile*>(p));
   }
   static void destruct_IFile(void *p) {
      typedef ::IFile current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::IFile

namespace ROOT {
   // Wrapper around operator delete
   static void delete_Pulse(void *p) {
      delete (static_cast<::Pulse*>(p));
   }
   static void deleteArray_Pulse(void *p) {
      delete [] (static_cast<::Pulse*>(p));
   }
   static void destruct_Pulse(void *p) {
      typedef ::Pulse current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::Pulse

namespace ROOT {
   // Wrappers around operator new
   static void *new_IHealthTools(void *p) {
      return  p ? new(p) ::IHealthTools : new ::IHealthTools;
   }
   static void *newArray_IHealthTools(Long_t nElements, void *p) {
      return p ? new(p) ::IHealthTools[nElements] : new ::IHealthTools[nElements];
   }
   // Wrapper around operator delete
   static void delete_IHealthTools(void *p) {
      delete (static_cast<::IHealthTools*>(p));
   }
   static void deleteArray_IHealthTools(void *p) {
      delete [] (static_cast<::IHealthTools*>(p));
   }
   static void destruct_IHealthTools(void *p) {
      typedef ::IHealthTools current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::IHealthTools

namespace ROOT {
   // Wrappers around operator new
   static void *new_ISiPM(void *p) {
      return  p ? new(p) ::ISiPM : new ::ISiPM;
   }
   static void *newArray_ISiPM(Long_t nElements, void *p) {
      return p ? new(p) ::ISiPM[nElements] : new ::ISiPM[nElements];
   }
   // Wrapper around operator delete
   static void delete_ISiPM(void *p) {
      delete (static_cast<::ISiPM*>(p));
   }
   static void deleteArray_ISiPM(void *p) {
      delete [] (static_cast<::ISiPM*>(p));
   }
   static void destruct_ISiPM(void *p) {
      typedef ::ISiPM current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ISiPM

namespace ROOT {
   // Wrappers around operator new
   static void *new_ISims(void *p) {
      return  p ? new(p) ::ISims : new ::ISims;
   }
   static void *newArray_ISims(Long_t nElements, void *p) {
      return p ? new(p) ::ISims[nElements] : new ::ISims[nElements];
   }
   // Wrapper around operator delete
   static void delete_ISims(void *p) {
      delete (static_cast<::ISims*>(p));
   }
   static void deleteArray_ISims(void *p) {
      delete [] (static_cast<::ISims*>(p));
   }
   static void destruct_ISims(void *p) {
      typedef ::ISims current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ISims

//______________________________________________________________________________
void SEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class SEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(SEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(SEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_SEvent(void *p) {
      return  p ? new(p) ::SEvent : new ::SEvent;
   }
   static void *newArray_SEvent(Long_t nElements, void *p) {
      return p ? new(p) ::SEvent[nElements] : new ::SEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_SEvent(void *p) {
      delete (static_cast<::SEvent*>(p));
   }
   static void deleteArray_SEvent(void *p) {
      delete [] (static_cast<::SEvent*>(p));
   }
   static void destruct_SEvent(void *p) {
      typedef ::SEvent current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::SEvent

namespace ROOT {
   static TClass *vectorlEvectorlEintgRsPgR_Dictionary();
   static void vectorlEvectorlEintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEintgRsPgR(void *p = nullptr);
   static void *newArray_vectorlEvectorlEintgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEintgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEintgRsPgR(void *p);
   static void destruct_vectorlEvectorlEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<int> >*)
   {
      vector<vector<int> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<int> >", -2, "vector", 389,
                  typeid(vector<vector<int> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<int> >) );
      instance.SetNew(&new_vectorlEvectorlEintgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEintgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEintgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<int> > >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<vector<int> >","std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<vector<int> >*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<vector<int> >*>(nullptr))->GetClass();
      vectorlEvectorlEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEintgRsPgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<int> > : new vector<vector<int> >;
   }
   static void *newArray_vectorlEvectorlEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<int> >[nElements] : new vector<vector<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEintgRsPgR(void *p) {
      delete (static_cast<vector<vector<int> >*>(p));
   }
   static void deleteArray_vectorlEvectorlEintgRsPgR(void *p) {
      delete [] (static_cast<vector<vector<int> >*>(p));
   }
   static void destruct_vectorlEvectorlEintgRsPgR(void *p) {
      typedef vector<vector<int> > current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<vector<int> >

namespace ROOT {
   static TClass *vectorlEunsignedsPshortgR_Dictionary();
   static void vectorlEunsignedsPshortgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPshortgR(void *p = nullptr);
   static void *newArray_vectorlEunsignedsPshortgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPshortgR(void *p);
   static void deleteArray_vectorlEunsignedsPshortgR(void *p);
   static void destruct_vectorlEunsignedsPshortgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned short>*)
   {
      vector<unsigned short> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned short>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned short>", -2, "vector", 389,
                  typeid(vector<unsigned short>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPshortgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned short>) );
      instance.SetNew(&new_vectorlEunsignedsPshortgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPshortgR);
      instance.SetDelete(&delete_vectorlEunsignedsPshortgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPshortgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPshortgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned short> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<unsigned short>","std::vector<unsigned short, std::allocator<unsigned short> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<unsigned short>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPshortgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<unsigned short>*>(nullptr))->GetClass();
      vectorlEunsignedsPshortgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPshortgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPshortgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned short> : new vector<unsigned short>;
   }
   static void *newArray_vectorlEunsignedsPshortgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned short>[nElements] : new vector<unsigned short>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPshortgR(void *p) {
      delete (static_cast<vector<unsigned short>*>(p));
   }
   static void deleteArray_vectorlEunsignedsPshortgR(void *p) {
      delete [] (static_cast<vector<unsigned short>*>(p));
   }
   static void destruct_vectorlEunsignedsPshortgR(void *p) {
      typedef vector<unsigned short> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned short>

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 389,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::vector<int, std::allocator<int> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr))->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete (static_cast<vector<int>*>(p));
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] (static_cast<vector<int>*>(p));
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = nullptr);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 389,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<float>","std::vector<float, std::allocator<float> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr))->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete (static_cast<vector<float>*>(p));
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] (static_cast<vector<float>*>(p));
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = nullptr);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 389,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<double>","std::vector<double, std::allocator<double> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr))->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete (static_cast<vector<double>*>(p));
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] (static_cast<vector<double>*>(p));
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace {
  void TriggerDictionaryInitialization_exactDict_Impl() {
    static const char* headers[] = {
"BiFocal.h",
"CEvent.h",
"Event.h",
"ExtractedData.h",
"GoldPlated.h",
"ICalibration.h",
"IEvent.h",
"IFile.h",
"IHealthTools.h",
"IPlotTools.h",
"ISiPM.h",
"ISims.h",
"IUtilities.h",
"Pulse.h",
"ReadConfiguration.h",
"SEvent.h",
nullptr
    };
    static const char* includePaths[] = {
"dict",
"./inc/",
"/data/TrinityLabComputer/Trinity/exact/dict/include",
"./dict/cling",
"./",
"/opt/root/include/",
"/data/TrinityLabComputer/Trinity/exact/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "exactDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$ReadConfiguration.h")))  __attribute__((annotate("$clingAutoload$BiFocal.h")))  ReadConfiguration;
class __attribute__((annotate("$clingAutoload$ExtractedData.h")))  __attribute__((annotate("$clingAutoload$BiFocal.h")))  ExtractedData;
class __attribute__((annotate("$clingAutoload$BiFocal.h")))  BiFocal;
class __attribute__((annotate("$clingAutoload$IPlotTools.h")))  __attribute__((annotate("$clingAutoload$CEvent.h")))  IPlotTools;
class __attribute__((annotate("$clingAutoload$IUtilities.h")))  __attribute__((annotate("$clingAutoload$CEvent.h")))  IUtilities;
class __attribute__((annotate("$clingAutoload$Event.h")))  Event;
class __attribute__((annotate("$clingAutoload$GoldPlated.h")))  GoldPlated;
class __attribute__((annotate("$clingAutoload$ICalibration.h")))  ICalibration;
class __attribute__((annotate("$clingAutoload$IEvent.h")))  IEvent;
class __attribute__((annotate("$clingAutoload$IFile.h")))  IFile;
class __attribute__((annotate("$clingAutoload$Pulse.h")))  __attribute__((annotate("$clingAutoload$IHealthTools.h")))  Pulse;
class __attribute__((annotate("$clingAutoload$IHealthTools.h")))  IHealthTools;
class __attribute__((annotate("$clingAutoload$ISiPM.h")))  ISiPM;
class __attribute__((annotate("$clingAutoload$ISims.h")))  ISims;
class __attribute__((annotate("$clingAutoload$SEvent.h")))  SEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "exactDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "BiFocal.h"
#include "CEvent.h"
#include "Event.h"
#include "ExtractedData.h"
#include "GoldPlated.h"
#include "ICalibration.h"
#include "IEvent.h"
#include "IFile.h"
#include "IHealthTools.h"
#include "IPlotTools.h"
#include "ISiPM.h"
#include "ISims.h"
#include "IUtilities.h"
#include "Pulse.h"
#include "ReadConfiguration.h"
#include "SEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"BiFocal", payloadCode, "@",
"Event", payloadCode, "@",
"ExtractedData", payloadCode, "@",
"GoldPlated", payloadCode, "@",
"ICalibration", payloadCode, "@",
"IEvent", payloadCode, "@",
"IFile", payloadCode, "@",
"IHealthTools", payloadCode, "@",
"IPlotTools", payloadCode, "@",
"ISiPM", payloadCode, "@",
"ISims", payloadCode, "@",
"IUtilities", payloadCode, "@",
"Pulse", payloadCode, "@",
"ReadConfiguration", payloadCode, "@",
"SEvent", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("exactDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_exactDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_exactDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_exactDict() {
  TriggerDictionaryInitialization_exactDict_Impl();
}
