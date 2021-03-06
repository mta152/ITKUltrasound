/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkAnalyticSignalImageFilter_h
#define itkAnalyticSignalImageFilter_h

#include <complex>

#include "itkComplexToComplex1DFFTImageFilter.h"
#include "itkForward1DFFTImageFilter.h"
#include "itkImageRegionSplitterDirection.h"
#include "itkFrequencyDomain1DImageFilter.h"

namespace itk
{
/** \class AnalyticSignalImageFilter
 * \brief Generates the analytic signal from one direction of an image.
 *
 * This filter generates the complex valued analytic signal along one direction
 * of an image.  This input is a real valued image, and the output is a complex
 * image.
 *
 * The analytic signal is given by
 *
 * f_a(x) = f(x) - i f_H(x)
 *
 * Where i is the square root of one and f_H(x) is the Hibert transform of f(x).
 *
 * Since the Hilbert transform in the Fourier domain is
 *
 * F_H(k) = F(k) i sign(k),
 *
 * f_a(x) is calculated by
 *
 * f_a(x) = F^{-1}( F(k) 2 U(k) )
 *
 * where U(k) is the unit step function.
 *
 * \ingroup FourierTransform
 * \ingroup Ultrasound
 */
template< typename TInputImage, typename TOutputImage >
class ITK_TEMPLATE_EXPORT AnalyticSignalImageFilter:
  public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(AnalyticSignalImageFilter);

  /** Standard class typedefs. */
  typedef TInputImage                                         InputImageType;
  typedef TOutputImage                                        OutputImageType;
  typedef typename OutputImageType::RegionType                OutputImageRegionType;

  itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

  typedef AnalyticSignalImageFilter                             Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType > Superclass;
  typedef SmartPointer< Self >                                  Pointer;
  typedef SmartPointer< const Self >                            ConstPointer;

  itkTypeMacro( AnalyticSignalImageFilter, ImageToImageFilter );
  itkNewMacro( Self );

  typedef FrequencyDomain1DImageFilter< OutputImageType, OutputImageType > FrequencyFilterType;

  /** Get the direction in which the filter is to be applied. */
  virtual unsigned int GetDirection() const
    {
    return this->m_FFTRealToComplexFilter->GetDirection();
    }

  /** Set the direction in which the filter is to be applied. */
  virtual void SetDirection( const unsigned int direction )
    {
    if( this->m_FFTRealToComplexFilter->GetDirection() != direction )
      {
      this->m_FFTRealToComplexFilter->SetDirection( direction );
      this->m_FFTComplexToComplexFilter->SetDirection( direction );
      if( this->m_FrequencyFilter.IsNotNull() )
        {
        this->m_FrequencyFilter->SetDirection( direction );
        }
      this->Modified();
      }
    }

  virtual void SetFrequencyFilter(FrequencyFilterType *filter)
   {
   if( filter != this->m_FrequencyFilter.GetPointer() )
     {
     this->m_FrequencyFilter = filter;
     this->m_FrequencyFilter->SetDirection( this->GetDirection() );
     this->Modified();
     }
   }

protected:
  AnalyticSignalImageFilter();
  virtual ~AnalyticSignalImageFilter() {}

  void PrintSelf(std::ostream& os, Indent indent) const override;

  // These behave like their analogs in Forward1DFFTImageFilter.
  virtual void GenerateInputRequestedRegion() override;
  virtual void EnlargeOutputRequestedRegion(DataObject *output) override;

  void BeforeThreadedGenerateData() override;
  void ThreadedGenerateData( const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId ) override;
  void AfterThreadedGenerateData() override;

  typedef Forward1DFFTImageFilter< InputImageType, OutputImageType > FFTRealToComplexType;
  typedef ComplexToComplex1DFFTImageFilter< OutputImageType, OutputImageType >      FFTComplexToComplexType;

  typename FFTRealToComplexType::Pointer    m_FFTRealToComplexFilter;
  typename FFTComplexToComplexType::Pointer m_FFTComplexToComplexFilter;


  /** Override to return a splitter that does not split along the direction we
   * are performing the transform. */
  virtual const ImageRegionSplitterBase* GetImageRegionSplitter() const override;

private:
  typename FrequencyFilterType::Pointer m_FrequencyFilter;
  ImageRegionSplitterDirection::Pointer m_ImageRegionSplitter;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAnalyticSignalImageFilter.hxx"
#endif

#endif // itkAnalyticSignalImageFilter_h
