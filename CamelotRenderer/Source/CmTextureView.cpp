#include "CmTextureView.h"
#include "CmUtil.h"

namespace CamelotEngine
{
	size_t TextureView::HashFunction::operator()(const TEXTURE_VIEW_DESC &key) const
	{
		size_t seed = 0;
		hash_combine(seed, key.mostDetailMip);
		hash_combine(seed, key.numMips);
		hash_combine(seed, key.firstArraySlice);
		hash_combine(seed, key.numArraySlices);
		hash_combine(seed, key.usage);

		return seed;
	}

	bool TextureView::EqualFunction::operator()
		(const TEXTURE_VIEW_DESC &a, const TEXTURE_VIEW_DESC &b) const
	{
		return a.mostDetailMip == b.mostDetailMip && a.numMips == b.numMips 
			&& a.firstArraySlice == b.firstArraySlice && a.numArraySlices == b.numArraySlices && a.usage == b.usage;
	}

	TextureView::TextureView(Texture* texture, TEXTURE_VIEW_DESC& _desc)
		: mDesc(_desc)
	{

	}

	TextureView::~TextureView()
	{

	}
}