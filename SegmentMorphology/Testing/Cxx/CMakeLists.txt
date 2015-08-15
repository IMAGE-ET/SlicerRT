set(KIT qSlicer${MODULE_NAME}Module)

set(KIT_TEST_SRCS
  vtkSlicerSegmentMorphologyModuleLogicTest1.cxx
  )

slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  TARGET_LIBRARIES vtkSlicerSegmentMorphologyModuleLogic
  WITH_VTK_DEBUG_LEAKS_CHECK
  )

#-----------------------------------------------------------------------------
set(TEMP "${CMAKE_BINARY_DIR}/Testing/Temporary")

macro(TEST_WITH_DATA TestName TestExecutableName
      DataDirectoryPath InputContourAFile
      InputContourBFile ReferenceVolumeFile BaselineContourFile 
      TemporarySceneFile MorphologicalOperation
      MorphologicalParameter ApplySimpleTransformToInput VolumeDifferenceToleranceVoxel)
  add_test(
    NAME ${TestName}
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CxxTests> ${TestExecutableName} ${ARGN}
    -DataDirectoryPath ${DataDirectoryPath}
    -InputContourAFile  ${InputContourAFile}
    -InputContourBFile  ${InputContourBFile}
    -ReferenceVolumeFile  ${ReferenceVolumeFile}
    -BaselineContourFile  ${BaselineContourFile}
    -TemporarySceneFile ${TemporarySceneFile}
    -MorphologicalOperation ${MorphologicalOperation}
    -MorphologicalParameter ${MorphologicalParameter}
    -ApplySimpleTransformToInput ${ApplySimpleTransformToInput}
    -VolumeDifferenceToleranceVoxel ${VolumeDifferenceToleranceVoxel}
  )
endmacro()

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Expand
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_Bladder.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Expanded_5_5_5_Bladder.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate.mrml
  Expand
  5.0
  0
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Expand PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Shrink
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_Bladder.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Shrunk_5_5_5_Bladder.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate_Shrink.mrml
  Shrink
  5.0
  0
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Shrink PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Union
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_PTV.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Bladder_Union_PTV.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate_Union.mrml
  Union
  0.0
  0
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Union PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Intersect
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_PTV.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Bladder_Intersect_PTV.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate_Intersect.mrml
  Intersect
  0.0
  0
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Intersect PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Subtract
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_PTV.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Bladder_Subtract_PTV.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate_Subtract.mrml
  Subtract
  0.0
  0
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Subtract PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Intersect_ApplyTransform
  vtkSlicerSegmentMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Testing/Data/
  EclipseProstate_Bladder.ctr
  EclipseProstate_PTV.ctr
  EclipseProstate_Dose.nrrd
  EclipseProstate_Bladder_Intersect_PTV_ApplyTransform.ctr
  ${TEMP}/TestScene_SegmentMorphology_EclipseProstate_Intersect_ApplyTransform.mrml
  Intersect
  0.0
  1
  100.0
)
set_tests_properties(vtkSlicerSegmentMorphologyModuleLogicTest_EclipseProstate_Intersect_ApplyTransform PROPERTIES FAIL_REGULAR_EXPRESSION "Error;ERROR;Warning;WARNING" )