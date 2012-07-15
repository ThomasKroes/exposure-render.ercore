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

namespace ExposureRender
{

namespace Enums
{
	//! Where the memory resides
	enum MemoryType
	{
		Host,		// Memory resides on the host
		Device		// Memory resides on the device
	};

	//! Memory unit
	enum MemoryUnit
	{
		KiloByte,	// KiloByte
		MegaByte,	// MegaByte
		GigaByte	// GigaByte
	};

	//! Type of procedural
	enum ProceduralType
	{
		Uniform = 0,		// Uniform color accross entire procedural domain
		Checker,			// Checkerboard
		Gradient			// Gradient
	};
	
	//! Type of texture
	enum TextureType
	{
		Procedural = 0,		// See also ::ProceduralType
		Bitmap				// Bitmap
	};
	
	//! Type of shape
	enum ShapeType
	{
		Plane = 0,		// Plane
		Disk,			// Disk
		Ring,			// Ring
		Box,			// Box
		Sphere,			// Sphere
		Cylinder,		// Cylinder
		Cone			// Cone
	};
	
	//! Type of shading
	enum ShadingMode
	{
		BrdfOnly = 0,			// Use BRDF everywhere in the volume
		PhaseFunctionOnly,		// Use phase function everywhere in the volume
		Hybrid,					// Hybrid scattering
		Modulation				// Modulation
	};

	//! Type of method for gradient computation
	enum GradientMode
	{
		ForwardDifferences = 0,		// Forward differences
		CentralDifferences,			// Central differences
		Filtered,					// Filtered
	};

	//! Exception level
	enum ExceptionLevel
	{
		Info = 0,		// Info
		Warning,		// Warning
		Error,			// Error
		Fatal			// Fatal
	};

	//! Emission unit, pertains to emitters
	enum EmissionUnit
	{
		Power = 0,		// Emission strength per emitter
		Lux,			// Emission strength per unit area
	};
	
	//! Type of scattering function
	enum ScatterFunction
	{
		Brdf,			// Didirectional reflectance distribution function
		PhaseFunction	// Phase function
	};
	
	//! Type of scattering
	enum ScatterType
	{
		Volume			= 0x001,	// Scattering within volume
		Light			= 0x002,	// Scattering event on light
		Object			= 0x004		// Scattering event on object
	};

	//! Type of alignment
	enum AlignmentType
	{
		AxisAlign = 0,	// Align to principal axis (x, y, z)
		LookAt,			// Using look at constraint
		Spherical,		// Using spherical coordinates
		Manual			// Manual alignment
	};

	//! Cartesian axes
	enum Axis
	{
		X = 0,		// X
		Y,			// Y
		Z			// Z
	};
	
	//! Type of filtering
	enum FilterMode
	{
		NearestNeighbour = 0,		// Nearest neighbor
		Linear,						// Linear
	};
	
	//! Type of focussing
	enum FocusMode
	{
		AutoFocus = 0,		// Auto-focus (focal distance is calculated semi-automatically)
		ManualFocus,		// Manual focus (user provides focal distance)
	};

	//! Type of computing device
	enum DeviceType
	{
		Cuda = 0,	// CUDA
		OpenCL,		// OpenCL
	};

	//! Type of texture addressing
	enum AddressMode
	{
		Wrap = 0,		// Wrap
		Clamp,			// Clamp
		Mirror,			// Mirror
		Border			// Border
	};
	
	//! Type of accelerator
	enum AcceleratorType
	{
		NoAcceleration = 0,		// No acceleration
		Octree					// Octree
	};

	//! Shape of the aperture
	enum ApertureShape
	{
		Circular = 0,		// Circular
		Polygon				// Polygonal
	};

	//! Type of gradient
	enum GradientType
	{
		Ramp = 0,	// Ramp
		Radial		// Radial
	};

	//! Type of rendering
	enum RenderMode
	{
		StandardRayCasting,			// Standard direct volume rendering
		StochasticRayCasting		// Stochastic direct volume rendering
	};
}

}
