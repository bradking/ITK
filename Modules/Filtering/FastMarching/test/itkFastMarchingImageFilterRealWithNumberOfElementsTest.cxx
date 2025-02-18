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


#include "itkFastMarchingImageFilterBase.h"
#include "itkFastMarchingNumberOfElementsStoppingCriterion.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkTestingMacros.h"


int
itkFastMarchingImageFilterRealWithNumberOfElementsTest(int, char *[])
{
  // create a fastmarching object
  using PixelType = float;
  constexpr unsigned int Dimension = 2;

  using FloatImageType = itk::Image<PixelType, Dimension>;

  using CriterionType = itk::FastMarchingNumberOfElementsStoppingCriterion<FloatImageType, FloatImageType>;

  using FastMarchingType = itk::FastMarchingImageFilterBase<FloatImageType, FloatImageType>;

  auto criterion = CriterionType::New();
  criterion->SetTargetNumberOfElements(100);

  auto marcher = FastMarchingType::New();
  marcher->SetStoppingCriterion(criterion);

  using NodePairType = FastMarchingType::NodePairType;
  //  using NodeContainerType = FastMarchingType::NodeContainerType;
  using NodePairContainerType = FastMarchingType::NodePairContainerType;

  // setup alive points
  auto alive = NodePairContainerType::New();

  NodePairType node_pair;

  constexpr FloatImageType::OffsetType offset0 = { { 28, 35 } };

  itk::Index<2> index{};

  node_pair.SetValue(0.0);
  node_pair.SetNode(index + offset0);
  alive->push_back(node_pair);

  node_pair.SetValue(42.0);
  index.Fill(200);
  node_pair.SetNode(index); // this node is out of range

  alive->push_back(node_pair);

  marcher->SetAlivePoints(alive);

  // setup trial points
  auto trial = NodePairContainerType::New();
  node_pair.SetValue(1.0);

  index.Fill(0);
  index += offset0;

  index[0] += 1;
  node_pair.SetNode(index);
  trial->push_back(node_pair);

  index[0] -= 1;
  index[1] += 1;
  node_pair.SetNode(index);
  trial->push_back(node_pair);

  index[0] -= 1;
  index[1] -= 1;
  node_pair.SetNode(index);
  trial->push_back(node_pair);

  index[0] += 1;
  index[1] -= 1;
  node_pair.SetNode(index);
  trial->push_back(node_pair);

  node_pair.SetValue(42.0);
  index.Fill(300); // this node is out of range
  node_pair.SetNode(index);
  trial->push_back(node_pair);

  marcher->SetTrialPoints(trial);

  // specify the size of the output image
  constexpr FloatImageType::SizeType size = { { 64, 64 } };
  marcher->SetOutputSize(size);

  // setup a speed image of ones
  auto                       speedImage = FloatImageType::New();
  FloatImageType::RegionType region;
  region.SetSize(size);
  speedImage->SetLargestPossibleRegion(region);
  speedImage->SetBufferedRegion(region);
  speedImage->Allocate();

  itk::ImageRegionIterator<FloatImageType> speedIter(speedImage, speedImage->GetBufferedRegion());
  while (!speedIter.IsAtEnd())
  {
    speedIter.Set(1.0);
    ++speedIter;
  }

  marcher->SetInput(speedImage);

  ITK_TRY_EXPECT_NO_EXCEPTION(marcher->Update());


  using OutputPixelType = char;

  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  using ThresholdingFilterType = itk::BinaryThresholdImageFilter<FloatImageType, OutputImageType>;

  auto thresholder = ThresholdingFilterType::New();

  thresholder->SetLowerThreshold(0.0);
  thresholder->SetUpperThreshold(100.0);
  thresholder->SetOutsideValue(0);
  thresholder->SetInsideValue(1);
  thresholder->SetInput(marcher->GetOutput());

  ITK_TRY_EXPECT_NO_EXCEPTION(thresholder->Update());


  const OutputImageType::Pointer output = thresholder->GetOutput();

  using OutputIteratorType = itk::ImageRegionConstIterator<OutputImageType>;

  OutputIteratorType it(output, output->GetLargestPossibleRegion());
  it.GoToBegin();

  unsigned int counter = 0;

  while (!it.IsAtEnd())
  {
    if (it.Get() == 1)
    {
      ++counter;
    }
    ++it;
  }

  if (counter >= 100)
  {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
