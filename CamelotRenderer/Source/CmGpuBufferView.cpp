#include "CmGpuBufferView.h"
#include "CmUtil.h"

namespace CamelotEngine
{
	size_t GpuBufferView::HashFunction::operator()(const GPU_BUFFER_DESC& key) const
	{
		size_t seed = 0;
		hash_combine(seed, key.elementWidth);
		hash_combine(seed, key.firstElement);
		hash_combine(seed, key.numElements);
		hash_combine(seed, key.useCounter);
		hash_combine(seed, key.usage);

		return seed;
	}

	bool GpuBufferView::EqualFunction::operator()
		(const GPU_BUFFER_DESC& a, const GPU_BUFFER_DESC& b) const
	{
		return a.elementWidth == b.elementWidth && a.firstElement == b.firstElement 
			&& a.numElements == b.numElements && a.useCounter == b.useCounter && a.usage == b.usage;
	}

	GpuBufferView::GpuBufferView(GPU_BUFFER_DESC& desc)
		:mDesc(desc)
	{

	}

	GpuBufferView::~GpuBufferView()
	{

	}
}