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

#include <iostream>
#include "itkRandomImageSource.h"
#include "itkShrinkImageFilter.h"
#include "itkCommand.h"
#include "itkTextOutput.h"
#include "itkTestingMacros.h"

// The following three classes are used to support callbacks
// on the shrink filter in the pipeline that follows later
class ShowProgressObject
{
public:
  ShowProgressObject(itk::ProcessObject * o) { m_Process = o; }
  void
  ShowProgress()
  {
    std::cout << "Progress " << m_Process->GetProgress() << std::endl;
  }
  itk::ProcessObject::Pointer m_Process;
};

class StartEndEvent
{
public:
  void
  Start()
  {
    std::cout << "start event" << std::endl;
  }
  void
  End()
  {
    std::cout << "end event " << std::endl;
  }
};


class AllEvents
{
public:
  void
  WatchEvents(itk::Object * caller, const itk::EventObject & event)
  {
    const char * eventName = nullptr;
    if (typeid(event) == typeid(itk::DeleteEvent))
    {
      eventName = "DeleteEvent";
    }
    else if (typeid(event) == typeid(itk::StartEvent))
    {
      eventName = "StartEvent";
    }
    else if (typeid(event) == typeid(itk::EndEvent))
    {
      eventName = "EndEvent";
    }
    else if (typeid(event) == typeid(itk::ProgressEvent))
    {
      auto * obj = dynamic_cast<itk::ProcessObject *>(caller);
      std::cout << "AnyEvent Progress " << obj->GetProgress() << std::endl;
      eventName = "ProgressEvent";
    }
    else if (typeid(event) == typeid(itk::PickEvent))
    {
      eventName = "PickEvent";
    }
    else if (typeid(event) == typeid(itk::StartPickEvent))
    {
      eventName = "StartPickEvent";
    }
    else if (typeid(event) == typeid(itk::AbortCheckEvent))
    {
      eventName = "AbortCheckEvent";
    }
    else if (typeid(event) == typeid(itk::ExitEvent))
    {
      eventName = "ExitEvent";
    }
    else
    {
      eventName = "UserEvent";
    }
    std::cout << "Event name: " << eventName << " Id: " << event.GetEventName() << std::endl;
  }
};


int
itkBasicArchitectureTest(int, char *[])
{
  // Comment the following if you want to use the itk text output window
  itk::OutputWindow::SetInstance(itk::TextOutput::New());

  // Uncomment the following if you want to see each message independently
  // itk::OutputWindow::GetInstance()->PromptUserOn();

  std::cout << std::endl << "Image dimension is " << itk::Image<float, 5>::ImageDimension << std::endl;
  std::cout << "Image dimension is " << itk::Image<short, 1>::ImageDimension << std::endl;

  // Begin by creating a simple pipeline. Use a scalar ss a pixel.
  //
  // Create a type alias to make the code more digestable
  using FloatImage2DType = itk::Image<float, 2>;

  // Create a source object (in this case a random image generator).
  // The source object is templated on the output type.
  //
  const itk::RandomImageSource<FloatImage2DType>::Pointer random = itk::RandomImageSource<FloatImage2DType>::New();
  random->SetMin(0.0);
  random->SetMax(1.0);

  // Create a filter...shrink the image by an integral amount. We also
  // add some callbacks to the start, progress, and end filter execution
  // methods. The filter is templated on the input and output data types.
  //
  const itk::ShrinkImageFilter<FloatImage2DType, FloatImage2DType>::Pointer shrink =
    itk::ShrinkImageFilter<FloatImage2DType, FloatImage2DType>::New();
  shrink->SetInput(random->GetOutput());
  shrink->SetShrinkFactors(2);

  // Create a command to call ShowProgress when progress event is triggered
  ShowProgressObject                                          progressWatch(shrink);
  const itk::SimpleMemberCommand<ShowProgressObject>::Pointer command =
    itk::SimpleMemberCommand<ShowProgressObject>::New();
  command->SetCallbackFunction(&progressWatch, &ShowProgressObject::ShowProgress);
  shrink->AddObserver(itk::ProgressEvent(), command);
  ITK_TEST_EXPECT_TRUE(shrink->HasObserver(itk::ProgressEvent()));

  // Create a command to call StartEndEvent when start event is triggered
  StartEndEvent                                          startEndWatch;
  const itk::SimpleMemberCommand<StartEndEvent>::Pointer start = itk::SimpleMemberCommand<StartEndEvent>::New();
  start->SetCallbackFunction(&startEndWatch, &StartEndEvent::Start);
  shrink->AddObserver(itk::StartEvent(), start);
  ITK_TEST_EXPECT_TRUE(shrink->HasObserver(itk::StartEvent()));

  // Create a command to call StartEndEvent when end event is triggered
  const itk::SimpleMemberCommand<StartEndEvent>::Pointer end = itk::SimpleMemberCommand<StartEndEvent>::New();
  end->SetCallbackFunction(&startEndWatch, &StartEndEvent::End);
  const unsigned long endEventObserverTag = shrink->AddObserver(itk::EndEvent(), end);
  ITK_TEST_EXPECT_TRUE(shrink->HasObserver(itk::EndEvent()));

  // Create a command that to call AnyEvent when event is fired
  AllEvents                                    allWatch;
  const itk::MemberCommand<AllEvents>::Pointer allEvents = itk::MemberCommand<AllEvents>::New();
  allEvents->SetCallbackFunction(&allWatch, &AllEvents::WatchEvents);
  const unsigned long anyEventObserverTag = shrink->AddObserver(itk::AnyEvent(), allEvents);
  ITK_TEST_EXPECT_TRUE(shrink->HasObserver(itk::AnyEvent()));
  shrink->Update();

  // Test RemoveObserver code
  shrink->RemoveObserver(anyEventObserverTag);
  ITK_TEST_EXPECT_TRUE(!shrink->HasObserver(itk::AnyEvent()));

  // Test RemoveObserver const code
  const auto & constShrink = *shrink;
  constShrink.RemoveObserver(endEventObserverTag);
  ITK_TEST_EXPECT_TRUE(!constShrink.HasObserver(itk::EndEvent()));

  return EXIT_SUCCESS;
}
