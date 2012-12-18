#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgramManager.h"

namespace CamelotEngine
{
	class D3D11GpuProgramManager : public GpuProgramManager
	{
	public:
		D3D11GpuProgramManager(D3D11Device& device);
		~D3D11GpuProgramManager();

	protected:
		D3D11Device& mDevice;
		
		GpuProgram* create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype, GpuProgramProfile profile);
	};
}