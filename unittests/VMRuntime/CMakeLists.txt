# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(HERMES_LINK_COMPONENTS LLVHSupport)

set(RTSources
  AlignedHeapSegmentTest.cpp
  AlignedStorageTest.cpp
  Array.cpp
  ArrayTest.cpp
  ArrayStorageTest.cpp
  CallResultTest.cpp
  CardObjectBoundaryNCTest.cpp
  CardTableNCTest.cpp
  CastingTest.cpp
  CodeCoverageProfilerTest.cpp
  CopyableVectorTest.cpp
  DateUtilTest.cpp
  DecoratedObjectTest.cpp
  DictPropertyMapTest.cpp
  ExtStringForTest.cpp
  ExternalMemAccountingTest.cpp
  Footprint.cpp
  GCBasicsTest.cpp
  GCFinalizerTest.cpp
  GCFragmentationNCTest.cpp
  GCGuardPageNCTest.cpp
  GCInitTest.cpp
  GCLazySegmentNCTest.cpp
  GCHeapExtentsInCrashManagerTest.cpp
  GCMarkWeakTest.cpp
  GCObjectIterationTest.cpp
  GCOOMNCTest.cpp
  GCReturnUnusedMemoryNCTest.cpp
  GCSanitizeHandlesTest.cpp
  GCSegmentAddressIndexTest.cpp
  GCSegmentRangeTest.cpp
  GCSizingTest.cpp
  HeapSnapshotTest.cpp
  HermesValueTest.cpp
  HiddenClassTest.cpp
  IdentifierTableTest.cpp
  InstrumentationAPITest.cpp
  InternalPropertiesTest.cpp
  InterpreterTest.cpp
  IRInstrumentationTest.cpp
  JSLibTest.cpp
  LogSuccessStorageProvider.cpp
  NativeFrameTest.cpp
  NativeFunctionNameTest.cpp
  MarkBitArrayNCTest.cpp
  ObjectBufferTest.cpp
  ObjectModelTest.cpp
  OperationsTest.cpp
  PredefinedStringsTest.cpp
  HandleTest.cpp
  RuntimeConfigTest.cpp
  SegmentedArrayTest.cpp
  SerializerTest.cpp
  SmallXStringTest.cpp
  StaticBuiltinsTest.cpp
  StorageProviderTest.cpp
  StringBuilderTest.cpp
  StringPrimitiveTest.cpp
  StringViewTest.cpp
  SymbolIDTest.cpp
  TestHelpers.h
  TestHelpers.cpp
  TwineChar16Test.cpp
  WeakValueMapTest.cpp
  MetadataTest.cpp
  )

add_hermes_unittest(HermesVMRuntimeTests
  ${RTSources}
  )

target_link_libraries(HermesVMRuntimeTests
  hermesVMRuntime
  hermesAST
  hermesHBCBackend
  hermesBackend
  hermesOptimizer
  hermesFrontend
  hermesParser
  hermesSupport
  dtoa
  ${CORE_FOUNDATION}
 )

hermes_link_icu(HermesVMRuntimeTests)

add_subdirectory(Instrumentation)

if(HERMESVM_JIT)
  add_subdirectory(JIT)
endif()
