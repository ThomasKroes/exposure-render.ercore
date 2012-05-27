/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "exception.h"
#include "log.h"

#ifdef __CUDA_ARCH__

#ifdef _WIN32
#include <windows.h>
#endif

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <GL/glew.h>
#include <cuda_gl_interop.h>
#include <map>

using namespace std;

namespace ExposureRender
{

namespace Cuda
{

static inline void HandleCudaError(const cudaError_t& CudaError, const char* pTitle = "")
{
	char Message[256];

	sprintf_s(Message, 256, "%s (%s)", cudaGetErrorString(CudaError), pTitle);

	if (CudaError != cudaSuccess)
	{
		DEBUG_BREAK
		throw(Exception(Enums::Error, Message));
	}
}

static inline void ThreadSynchronize()
{
	Cuda::HandleCudaError(cudaThreadSynchronize(), __FUNCTION__);
}

template<class T> static inline void Allocate(T*& pDevicePointer, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMalloc((void**)&pDevicePointer, Num * sizeof(T)), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void AllocatePiched(T*& pDevicePointer, const int Pitch, const int Width, const int Height)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMallocPitch((void**)&pDevicePointer, (size_t*)&Pitch, Width * sizeof(T), Height), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemSet(T*& pDevicePointer, const int Value, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemset((void*)pDevicePointer, Value, (size_t)(Num * sizeof(T))), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void HostToConstantDevice(T* pHost, char* pDeviceSymbol, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpyToSymbol(pDeviceSymbol, pHost, Num * sizeof(T)), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemCopyHostToDeviceSymbol(T* pHost, const char* pDeviceSymbol, const int& Num = 1, const int& Offset = 0)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpyToSymbol(pDeviceSymbol, pHost, Num * sizeof(T), Offset, cudaMemcpyHostToDevice), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemCopyDeviceToDeviceSymbol(T* pDevice, const char* pDeviceSymbol, const int& Num = 1, const int& Offset = 0)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpyToSymbol(pDeviceSymbol, pDevice, Num * sizeof(T), 0, cudaMemcpyDeviceToDevice), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemCopyHostToDevice(T* pHost, T* pDevice, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpy(pDevice, pHost, Num * sizeof(T), cudaMemcpyHostToDevice), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemCopyDeviceToHost(T* pDevice, T* pHost, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpy(pHost, pDevice, Num * sizeof(T), cudaMemcpyDeviceToHost), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

template<class T> static inline void MemCopyDeviceToDevice(T* pDeviceSource, T* pDeviceDestination, int Num = 1)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaMemcpy(pDeviceDestination, pDeviceSource, Num * sizeof(T), cudaMemcpyDeviceToDevice), __FUNCTION__);
	Cuda::ThreadSynchronize();
}

static inline void FreeArray(cudaArray*& pCudaArray)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaFreeArray(pCudaArray), __FUNCTION__);
	pCudaArray = NULL;
	Cuda::ThreadSynchronize();
}

template<class T> static inline void Free(T*& pBuffer)
{
	if (pBuffer == NULL)
		return;

	Cuda::ThreadSynchronize();
	
	HandleCudaError(cudaFree(pBuffer), __FUNCTION__);
	pBuffer = NULL;

	Cuda::ThreadSynchronize();
}

static inline void GetSymbolAddress(void** pDevicePointer, char* pSymbol)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGetSymbolAddress(pDevicePointer, pSymbol), __FUNCTION__);
}

static inline void GraphicsUnregisterResource(cudaGraphicsResource_t GraphicsResource)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGraphicsUnregisterResource(GraphicsResource), __FUNCTION__);
}

static inline void GraphicsGLRegisterBuffer(struct cudaGraphicsResource** Resource, GLuint Buffer, unsigned int Flags)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGraphicsGLRegisterBuffer(Resource, Buffer, Flags), __FUNCTION__);
}

static inline void GraphicsMapResources(int Count, cudaGraphicsResource_t* Resources, cudaStream_t Stream)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGraphicsMapResources(Count, Resources, Stream), __FUNCTION__);
}

static inline void GraphicsUnmapResources(int Count, cudaGraphicsResource_t* Resources, cudaStream_t Stream)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGraphicsUnmapResources(Count, Resources, Stream), __FUNCTION__);
}


static inline void GLMapBufferObject(void** DevicePointer, const GLuint& Buffer)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGLMapBufferObject(DevicePointer, Buffer), __FUNCTION__);
}

static inline void GLUnmapBufferObject(const GLuint& Buffer)
{
	Cuda::ThreadSynchronize();
	HandleCudaError(cudaGLUnmapBufferObject(Buffer), __FUNCTION__);
}



}

}

#endif