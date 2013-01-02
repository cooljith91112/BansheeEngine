#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT GpuParamBlock
	{
	public:
		GpuParamBlock(const GpuParamBlockDesc& desc);
		~GpuParamBlock();

		void write(UINT32 offset, const void* data, UINT32 size);
		void zeroOut(UINT32 offset, UINT32 size);

		const UINT8* getDataPtr(UINT32 offset) const;

		virtual void updateIfDirty();
		
		static GpuParamBlockPtr create(const GpuParamBlockDesc& desc);
	private:
		bool mDirty;
		UINT8* mData;
		UINT32 mSize;
	};
}