/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkTestingMacros.h"

int
itkExtractSlice(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cout << " in.nii out.nrrd";
    return EXIT_FAILURE;
  }

  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;
  using SliceType = itk::Image<PixelType, 2>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using ExtractType = itk::ExtractImageFilter<ImageType, SliceType>;
  using WriterType = itk::ImageFileWriter<SliceType>;

  auto reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->UpdateOutputInformation();

  const ImageType::RegionType inRegion = reader->GetOutput()->GetLargestPossibleRegion();

  ImageType::SizeType outSize = inRegion.GetSize();
  outSize[1] = 0;
  ImageType::IndexType outIndex = inRegion.GetIndex();
  outIndex[1] = inRegion.GetSize()[1] / 2;
  ImageType::RegionType outRegion;
  outRegion.SetSize(outSize);
  outRegion.SetIndex(outIndex);

  auto extractFilter = ExtractType::New();
  extractFilter->SetDirectionCollapseToSubmatrix();
  extractFilter->InPlaceOn();
  extractFilter->SetInput(reader->GetOutput());
  extractFilter->SetExtractionRegion(outRegion);

  auto writer = WriterType::New();
  writer->SetInput(extractFilter->GetOutput());
  writer->SetFileName(argv[2]);

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());


  return EXIT_SUCCESS;
}
