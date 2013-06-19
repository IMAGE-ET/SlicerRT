// MRML includes
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkImageThreshold.h>
#include <vtkTransform.h>

//----------------------------------------------------------------------------
template<typename T> bool SlicerRtCommon::ConvertVolumeNodeToItkImage(vtkMRMLVolumeNode* inVolumeNode, typename itk::Image<T, 3>::Pointer outItkVolume, bool paintForegroundTo1/*=false*/)
{
  if ( inVolumeNode == NULL )
  {
    std::cerr << "SlicerRtCommon::ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - input MRML volume node is NULL!" << std::endl;
    return false; 
  }

  vtkImageData* inVolume = inVolumeNode->GetImageData();
  if ( inVolume == NULL )
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - image in input MRML volume node is NULL!");
    return false; 
  }

  if ( outItkVolume.IsNull() )
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - output image is NULL!");
    return false; 
  }

  // Paint the foreground with label 1 (so that Plastimatch::DiceStatistics can count them properly)
  vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();
  if (paintForegroundTo1)
  {
    threshold->SetInput(inVolume);
    threshold->SetInValue(1);
    threshold->SetOutValue(0);
    threshold->ThresholdByUpper(1);
    threshold->SetOutputScalarTypeToUnsignedChar(); //TODO
    threshold->Update();
  }

  // Convert vtkImageData to itkImage 
  vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
  if (paintForegroundTo1)
  {
    imageExport->SetInput(threshold->GetOutput());
  }
  else
  {
    imageExport->SetInput(inVolume);
  }
  imageExport->Update(); 

  // Determine input volume to world transform
  vtkSmartPointer<vtkMatrix4x4> rasToWorldTransformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMRMLTransformNode* inTransformNode=inVolumeNode->GetParentTransformNode();
  if (inTransformNode!=NULL)
  {
    if (inTransformNode->IsTransformToWorldLinear() == 0)
    {
      vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: There is a non-linear transform assigned to an input dose volume. Only linear transforms are supported!");
      return false;
    }
    inTransformNode->GetMatrixTransformToWorld(rasToWorldTransformMatrix);
  }

  vtkSmartPointer<vtkMatrix4x4> inVolumeToRasTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolumeNode->GetIJKToRASMatrix(inVolumeToRasTransformMatrix);

  vtkSmartPointer<vtkTransform> inVolumeToWorldTransform = vtkSmartPointer<vtkTransform>::New();
  inVolumeToWorldTransform->Identity();
  inVolumeToWorldTransform->PostMultiply();
  inVolumeToWorldTransform->Concatenate(inVolumeToRasTransformMatrix);
  inVolumeToWorldTransform->Concatenate(rasToWorldTransformMatrix);

  // Set ITK image properties
  double outputSpacing[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetScale(outputSpacing);
  outItkVolume->SetSpacing(outputSpacing);

  double outputOrigin[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetPosition(outputOrigin);
  outItkVolume->SetOrigin(outputOrigin);

  double outputOrienationAngles[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetOrientation(outputOrienationAngles);
  vtkSmartPointer<vtkTransform> inVolumeToWorldOrientationTransform = vtkSmartPointer<vtkTransform>::New();
  inVolumeToWorldOrientationTransform->Identity();
  inVolumeToWorldOrientationTransform->RotateX(outputOrienationAngles[0]);
  inVolumeToWorldOrientationTransform->RotateY(outputOrienationAngles[1]);
  inVolumeToWorldOrientationTransform->RotateZ(outputOrienationAngles[2]);
  vtkSmartPointer<vtkMatrix4x4> inVolumeToWorldOrientationTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolumeToWorldOrientationTransform->GetMatrix(inVolumeToWorldOrientationTransformMatrix);
  itk::Matrix<double,3,3> outputDirectionMatrix;
  for(int i=0; i<3; i++)
  {
    for(int j=0; j<3; j++)
    {
      outputDirectionMatrix[i][j] = inVolumeToWorldOrientationTransformMatrix->GetElement(i,j);
    }
  }
  outItkVolume->SetDirection(outputDirectionMatrix);

  int inputExtent[6]={0,0,0,0,0,0}; 
  inVolume->GetExtent(inputExtent); 
  itk::Image<float, 3>::SizeType inputSize;
  inputSize[0] = inputExtent[1] - inputExtent[0] + 1;
  inputSize[1] = inputExtent[3] - inputExtent[2] + 1;
  inputSize[2] = inputExtent[5] - inputExtent[4] + 1;

  itk::Image<float, 3>::IndexType start;
  start[0]=start[1]=start[2]=0.0;

  itk::Image<float, 3>::RegionType region;
  region.SetSize(inputSize);
  region.SetIndex(start);
  outItkVolume->SetRegions(region);

  try
  {
    outItkVolume->Allocate();
  }
  catch(itk::ExceptionObject & err)
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to allocate memory for the image conversion: " << err.GetDescription())
    return false;
  }

  imageExport->Export( outItkVolume->GetBufferPointer() );

  return true;
}

//----------------------------------------------------------------------------
template<typename T> bool SlicerRtCommon::ConvertVolumeNodeToItkImage2(vtkMRMLVolumeNode* inVolumeNode, typename itk::Image<T, 3>::Pointer outItkVolume, bool paintForegroundTo1/*=false*/)
{
  if ( inVolumeNode == NULL )
  {
    std::cerr << "SlicerRtCommon::ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - input MRML volume node is NULL!" << std::endl;
    return false; 
  }

  vtkImageData* inVolume = inVolumeNode->GetImageData();
  if ( inVolume == NULL )
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - image in input MRML volume node is NULL!");
    return false; 
  }

  if ( outItkVolume.IsNull() )
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to convert volume node to itk image - output image is NULL!");
    return false; 
  }

  // Paint the foreground with label 1 (so that Plastimatch::DiceStatistics can count them properly)
  vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();
  if (paintForegroundTo1)
  {
    threshold->SetInput(inVolume);
    threshold->SetInValue(1);
    threshold->SetOutValue(0);
    threshold->ThresholdByUpper(1);
    threshold->SetOutputScalarTypeToUnsignedChar(); //TODO
    threshold->Update();
  }

  // Convert vtkImageData to itkImage 
  vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
  if (paintForegroundTo1)
  {
    imageExport->SetInput(threshold->GetOutput());
  }
  else
  {
    imageExport->SetInput(inVolume);
  }
  imageExport->Update(); 

  // Determine input volume to world transform
  vtkSmartPointer<vtkMatrix4x4> rasToWorldTransformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMRMLTransformNode* inTransformNode=inVolumeNode->GetParentTransformNode();
  if (inTransformNode!=NULL)
  {
    if (inTransformNode->IsTransformToWorldLinear() == 0)
    {
      vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: There is a non-linear transform assigned to an input dose volume. Only linear transforms are supported!");
      return false;
    }
    inTransformNode->GetMatrixTransformToWorld(rasToWorldTransformMatrix);
  }

  vtkSmartPointer<vtkMatrix4x4> inVolumeToRasTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolumeNode->GetIJKToRASMatrix(inVolumeToRasTransformMatrix);

  vtkSmartPointer<vtkMatrix4x4> RAS2LPSMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  RAS2LPSMatrix->SetElement(0,0,-1.0);
  RAS2LPSMatrix->SetElement(1,1,-1.0);
  RAS2LPSMatrix->SetElement(2,2, 1.0);
  RAS2LPSMatrix->SetElement(3,3, 1.0);
  
  vtkSmartPointer<vtkTransform> inVolumeToWorldTransform = vtkSmartPointer<vtkTransform>::New();
  inVolumeToWorldTransform->Identity();
  inVolumeToWorldTransform->PostMultiply();
  inVolumeToWorldTransform->Concatenate(inVolumeToRasTransformMatrix);
  inVolumeToWorldTransform->Concatenate(rasToWorldTransformMatrix);
  inVolumeToWorldTransform->Concatenate(RAS2LPSMatrix);

  // Set ITK image properties
  double outputSpacing[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetScale(outputSpacing);
  outputSpacing[0] = outputSpacing[0] < 0 ? -outputSpacing[0] : outputSpacing[0];
  outputSpacing[1] = outputSpacing[1] < 0 ? -outputSpacing[1] : outputSpacing[1];
  outputSpacing[2] = outputSpacing[2] < 0 ? -outputSpacing[2] : outputSpacing[2];
  outItkVolume->SetSpacing(outputSpacing);

  double outputOrigin[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetPosition(outputOrigin);
  outItkVolume->SetOrigin(outputOrigin);

  /*
  double outputOrienationAngles[3] = {0.0, 0.0, 0.0};
  inVolumeToWorldTransform->GetOrientation(outputOrienationAngles);
  vtkSmartPointer<vtkTransform> inVolumeToWorldOrientationTransform = vtkSmartPointer<vtkTransform>::New();
  inVolumeToWorldOrientationTransform->Identity();
  inVolumeToWorldOrientationTransform->RotateX(outputOrienationAngles[0]);
  inVolumeToWorldOrientationTransform->RotateY(outputOrienationAngles[1]);
  inVolumeToWorldOrientationTransform->RotateZ(outputOrienationAngles[2]);
  */
  
  vtkSmartPointer<vtkMatrix4x4> inVolumeToWorldTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  inVolumeToWorldTransform->GetMatrix(inVolumeToWorldTransformMatrix);
  itk::Matrix<double,3,3> outputDirectionMatrix;
  for(int i=0; i<3; i++)
  {
    for(int j=0; j<3; j++)
    {
      outputDirectionMatrix[i][j] = inVolumeToWorldTransformMatrix->GetElement(i,j);
    }
  }
  outItkVolume->SetDirection(outputDirectionMatrix);

  int inputExtent[6]={0,0,0,0,0,0}; 
  inVolume->GetExtent(inputExtent); 
  itk::Image<T, 3>::SizeType inputSize;
  inputSize[0] = inputExtent[1] - inputExtent[0] + 1;
  inputSize[1] = inputExtent[3] - inputExtent[2] + 1;
  inputSize[2] = inputExtent[5] - inputExtent[4] + 1;

  itk::Image<T, 3>::IndexType start;
  start[0]=start[1]=start[2]=0.0;

  itk::Image<T, 3>::RegionType region;
  region.SetSize(inputSize);
  region.SetIndex(start);
  outItkVolume->SetRegions(region);

  try
  {
    outItkVolume->Allocate();
  }
  catch(itk::ExceptionObject & err)
  {
    vtkErrorWithObjectMacro(inVolumeNode, "ConvertVolumeNodeToItkImage: Failed to allocate memory for the image conversion: " << err.GetDescription())
    return false;
  }

  imageExport->Export( outItkVolume->GetBufferPointer() );

  return true;
}
