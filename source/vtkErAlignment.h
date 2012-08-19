/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "vtkErDll.h"
#include "vtkErBindable.h"

#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkCamera.h>

using namespace ExposureRender;

/*! 
	@class vtkErAlignment
	@brief Alignment class

	This class is used to specify transformations in Exposure Render.

	Currently there are four ways to specify the alignment: axis aligned(\a Axis, \a AutoFlip, \a Position), look-at (\a Position, \a Target, \a Up), spherical (\a Azimuth, \a Elevation, \a Offset) and manual (\a ManualTM)
 */
class VTK_ER_EXPORT vtkErAlignment : public vtkAlgorithm
{
public:
	static vtkErAlignment* New();
	vtkTypeRevisionMacro(vtkErAlignment, vtkAlgorithm);

	void RequestData(ExposureRender::Alignment& Alignment);

	vtkGetMacro(AlignmentType, int);
	vtkSetMacro(AlignmentType, int);

	vtkGetMacro(Axis, int);
	vtkSetMacro(Axis, int);

	vtkGetMacro(AutoFlip, bool);
	vtkSetMacro(AutoFlip, bool);

	vtkGetVector3Macro(Position, float);
	vtkSetVector3Macro(Position, float);

	vtkGetVector3Macro(Target, float);
	vtkSetVector3Macro(Target, float);

	vtkGetVector3Macro(Up, float);
	vtkSetVector3Macro(Up, float);

	vtkGetMacro(Elevation, float);
	vtkSetMacro(Elevation, float);

	vtkGetMacro(Azimuth, float);
	vtkSetMacro(Azimuth, float);

	vtkGetMacro(Offset, float);
	vtkSetMacro(Offset, float);

	void SetManualTM(vtkMatrix4x4* ManualTM) { this->ManualTM = ManualTM; };

	vtkGetMacro(RelativeToCamera, bool);
	vtkSetMacro(RelativeToCamera, bool);

	vtkGetMacro(UseCameraFocalPoint, bool);
	vtkSetMacro(UseCameraFocalPoint, bool);

	virtual void GetCameraOffset(vtkCamera* Camera, Matrix44& Offset);

protected:
	vtkErAlignment();
	virtual ~vtkErAlignment() {};

private:
	vtkErAlignment(const vtkErAlignment& Other);	// Not implemented
    void operator = (const vtkErAlignment& Other);	// Not implemented

	int								AlignmentType;				/*! Type of alignment e.g. axis aligned(\a Axis, \AutoFlip, \a Position), look-at, spherical and manual */
	int								Axis;						/*! Axis to align to */
	bool							AutoFlip;					/*! Whether auto flipping is enabled(Only valid when type is 'axis aligned') */
	float							Position[3];				/*! Position */
	float							Target[3];					/*! Target */
	float							Up[3];						/*! Up-vector (in case of look-at) */
	float							Elevation;					/*! Elevation or longitude (in case of spherical alignment) */
	float							Azimuth;					/*! Azimuth or latitude (in case of spherical alignment) */
	float							Offset;						/*! Offset from center (in case of spherical alignment) */
	vtkSmartPointer<vtkMatrix4x4>	ManualTM;					/*! Manual transformation matrix */
	bool							RelativeToCamera;			/*! Whether the alignment is relative to the scene camera */
	bool							UseCameraFocalPoint;		/*! If the alignment is relative to the camera's focal or position */
};
