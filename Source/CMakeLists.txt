#	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
#	All rights reserved.
#
#	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#	
#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8)

OPTION(ER_VTK "Exposure Render connection with VTK" OFF)
OPTION(ER_VTK_PYTHON "Python wrapping for Exposure Render VTK" OFF)
OPTION(ER_VTK_EXAMPLE "Example project which shows how to use Exposure Render in VTK" OFF)

PROJECT(ExposureRender)

# Use CUDA
FIND_PACKAGE(CUDA)

# Supported streaming architectures, uncomment the lines that pertain to the hardware your compiling for
# More on CUDA gpu's: http://developer.nvidia.com/cuda-gpus

# Tesla architecture: sm 1.0 and above
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=sm_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=compute_10;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=sm_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=compute_11;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=sm_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=compute_12;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=sm_13;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=compute_13;${CUDA_NVCC_FLAGS}")

# Fermi architecture: sm 2.0 and above
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=sm_20;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=compute_20;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_21,code=sm_21;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_21,code=compute_21;${CUDA_NVCC_FLAGS}")

# Kepler architecture: sm 3.0 and above
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_30,code=sm_30;${CUDA_NVCC_FLAGS}")
# SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_30,code=compute_30;${CUDA_NVCC_FLAGS}")

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CUDA_SDK_ROOT_DIR}/common/inc
	${CUDA_TOOLKIT_INCLUDE}
)

# Make sure classes are exported
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

SOURCE_GROUP("Core" FILES ${vtkErCoreSources})

SET(General
	utilities.h
	geometry.h
	raymarching.h
	rng.h
	montecarlo.h
	ray.h
	textures.h
	log.h
	defines.h
	camera.h
	procedural.h
	matrix.h
	boundingbox.h
	exception.h
	timestamp.h
	enums.h
	exposurerender.h
	exposurerender.cpp
	range.h
	tracer.h
	statistic.h
	statistics.h
	volume.h
	intersection.h
	object.h
	texture.h
	bitmap.h
	alignment.h
	transform.h
	octree.h
	transport.h
	volumeproperty.h
)

SOURCE_GROUP("General" FILES ${General})

SET(Buffer
	buffer.h
	buffer1d.h
	buffer2d.h
	buffer3d.h
	randomseedbuffer.h
	randomseedbuffer1d.h
	randomseedbuffer2d.h
	randomseedbuffer3d.h
	randomseedbuffers.h
	buffers.h
	cudatexture.h
	cudatexture1d.h
	cudatexture2d.h
	cudatexture3d.h
	cudatextures.h
	framebuffer.h
)

SOURCE_GROUP("Buffer" FILES ${Buffer})

SET(TransferFunction
	piecewisefunctionnode.h
	piecewisefunction.h
	piecewiselinearfunction.h
	transferfunction.h
	transferfunction1d.h
	transferfunction2d.h
	transferfunctions.h
)

SOURCE_GROUP("Transfer Function" FILES ${TransferFunction})

SET(Shapes
	shape.h
	shapes.h
	plane.h
	box.h
	disk.h
	ring.h
	sphere.h
	cylinder.h
)

SOURCE_GROUP("Shapes" FILES ${Shapes})

SET(Sample
	mutate.h
	rendersample.h
	surfacesample.h
	lightsample.h
	lightingsample.h
	camerasample.h
	metrosample.h
	shadersample.h
	samples.h
)

SOURCE_GROUP("Sample" FILES ${Sample})

SET(Shading
	shader.h
	phasefunction.h
	brdf.h
	fresnel.h
	lambert.h
	microfacet.h
	phasefunction.h
	blinn.h
)

SOURCE_GROUP("Shading" FILES ${Shading})

SET(API
	hostbase.h
	hosttracer.h
	hostvolume.h
	hostobject.h
	hosttexture.h
	hostbitmap.h
)

SOURCE_GROUP("API" FILES ${API})

SET(Filter
	filter.h
	mitchell.h
	gaussian.h
	sinc.h
	triangle.h
)

SOURCE_GROUP("Filter" FILES ${Filter})

SET(Color
	color.h
	colorxyzf.h
	colorxyzaf.h
	colorrgbf.h
	colorrgbaf.h
	colorrgbauc.h
	kelvin.h
)

SOURCE_GROUP("Color" FILES ${Color})

SET(Vector
	vec.h
	vec2i.h
	vec2f.h
	vec3i.h
	vec3f.h
	vec4i.h
	vec4f.h
	indices.h
	hysteresis.h
	vector.h
)

SOURCE_GROUP("Vector" FILES ${Vector})

SET(Cuda
	estimate.cuh
	filtering.cuh
	tonemap.cuh
	autofocus.cuh
	dvr.cuh
	render.cuh
	samplecamera.cuh
	samplelight.cuh
	sampleshader.cuh
	intersect.cuh
	gaussianfilterrgbauc.cuh
	gaussianfilterxyzaf.cuh
	blendrgbauc.cuh
	list.cuh
	wrapper.cuh
	macros.cuh
	core.cu
)

SOURCE_GROUP("Cuda" FILES ${Cuda})

# Create the ErCore project as a shared library (DLL)
CUDA_ADD_LIBRARY(ErCore ${General} ${Buffer} ${TransferFunction} ${Shading} ${Shapes} ${Sample} ${API} ${Filter} ${Color} ${Vector} ${Cuda} SHARED)

IF(ER_VTK)

	IF(NOT VTK_BINARY_DIR)
		FIND_PACKAGE(VTK)
		IF(NOT VTK_DIR)
		 MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
		ENDIF(NOT VTK_DIR)
		INCLUDE(${VTK_USE_FILE})
	ENDIF(NOT VTK_BINARY_DIR)

	INCLUDE(CMakeOptions.cmake)

	INCLUDE_DIRECTORIES(
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
	)

	# Make sure classes are exported
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

	SET(General
		vtkErObject.h
		vtkErObject.cpp
		vtkErVolume.h
		vtkErVolume.cpp
		vtkErTracer.h
		vtkErTracer.cpp
		vtkErTexture.h
		vtkErTexture.cpp
		vtkErBindable.h
		vtkErBindable.cpp
		vtkErCamera.h
		vtkErCamera.cpp
		vtkErAlignment.h
		vtkErAlignment.cpp
		vtkErBitmap.h
		vtkErBitmap.cpp
		vtkErShape.h
		vtkErShape.cpp
		vtkErTimerCallback.h
		vtkErTimerCallback.cpp
		vtkErVolumeProperty.h
		vtkErVolumeProperty.cpp
		vtkErInteractorStyleTrackballCamera.h
		vtkErInteractorStyleTrackballCamera.cpp
		vtkErStable.h
		vtkErStable.cpp
		vtkErConfigure.h
		vtkErDll.h
		vtkErUtilities.h
	)

	SOURCE_GROUP("General" FILES ${General})

	# Sources that will be wrapped by python
	SET(WrapPython
		vtkErShape.h
		vtkErShape.cpp
		vtkErAlignment.h
		vtkErAlignment.cpp
		vtkErLight.h
		vtkErLight.cpp
		vtkErObject.h
		vtkErObject.cpp
		vtkErVolume.h
		vtkErVolume.cpp
		vtkErTracer.h
		vtkErTracer.cpp
		vtkErTexture.h
		vtkErTexture.cpp
		vtkErCamera.h
		vtkErCamera.cpp
		vtkErBitmap.h
		vtkErBitmap.cpp
		vtkErInteractorStyleTrackballCamera.h
		vtkErInteractorStyleTrackballCamera.cpp
	)

	ADD_LIBRARY(vtkEr ${General})
	
	# Make use of precompiled headers to speed up compilation
	if (MSVC)
	   set_target_properties(vtkEr PROPERTIES COMPILE_FLAGS "/YuvtkErStable.h")
	   set_source_files_properties(vtkErStable.cpp PROPERTIES COMPILE_FLAGS "/YcvtkErStable.h")
	endif(MSVC)
	
	TARGET_LINK_LIBRARIES(vtkEr vtkCommon vtkRendering vtkGraphics vtkVolumeRendering ErCore OpenGL32.lib)
	
ENDIF(ER_VTK)

IF(ER_VTK_PYTHON)

	IF(VTK_WRAP_PYTHON)
		INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
		VTK_WRAP_PYTHON3(vtkErPython vtkErPython_SRCS "${WrapPython}")
		ADD_LIBRARY(vtkErPythonD ${vtkErPython_SRCS})
		PYTHON_ADD_MODULE(vtkErPython vtkErPythonInit.cxx)
		
		TARGET_LINK_LIBRARIES(vtkErPython vtkErPythonD)
		TARGET_LINK_LIBRARIES(vtkErPythonD vtkEr vtkCommonPythonD vtkRenderingPythonD vtkGraphicsPythonD vtkVolumeRenderingPythonD)
	ENDIF(VTK_WRAP_PYTHON)

ENDIF(ER_VTK_PYTHON)

IF(ER_VTK_EXAMPLE)

	ADD_EXECUTABLE(vtkErExample vtkErExample.cpp)
	TARGET_LINK_LIBRARIES(vtkErExample vtkEr)

ENDIF(ER_VTK_EXAMPLE)