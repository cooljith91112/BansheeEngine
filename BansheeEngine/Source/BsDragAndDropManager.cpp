//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDragAndDropManager.h"
#include "BsPlatform.h"
#include "BsCoreApplication.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	DragAndDropManager::DragAndDropManager()
		:mIsDragInProgress(false), mDragTypeId(0), mData(nullptr), mCaptureChanged(false), mCaptureActive(0), mNeedsValidDropTarget(false)
	{
		Platform::onMouseCaptureChanged.connect(std::bind(&DragAndDropManager::mouseCaptureChanged, this));
		Input::instance().onPointerReleased.connect(std::bind(&DragAndDropManager::cursorReleased, this, _1));
	}

	void DragAndDropManager::addDropCallback(std::function<void(bool)> dropCallback)
	{
		mDropCallbacks.push_back(dropCallback);
	}

	void DragAndDropManager::startDrag(UINT32 typeId, void* data, std::function<void(bool)> dropCallback, bool needsValidDropTarget)
	{
		mDragTypeId = typeId;
		mData = data;
		mNeedsValidDropTarget = needsValidDropTarget;
		addDropCallback(dropCallback);
		mIsDragInProgress = true;

		mCaptureActive.store(false);
		mCaptureChanged.store(false);

		Platform::captureMouse(*gCoreApplication().getPrimaryWindow());
	}

	void DragAndDropManager::_update()
	{
		if(!mIsDragInProgress)
			return;

		// This generally happens when window loses focus and capture is lost (e.g. alt+tab)
		int captureActive = mCaptureActive.load();
		if(!captureActive && mCaptureChanged.load())
		{
			endDrag(false);
		}
	}

	void DragAndDropManager::endDrag(bool processed)
	{
		for(auto& callback : mDropCallbacks)
			callback(processed);

		mDragTypeId = 0;
		mData = nullptr;
		mDropCallbacks.clear();
		mIsDragInProgress = false;
	}

	void DragAndDropManager::mouseCaptureChanged()
	{
		mCaptureActive.fetch_xor(1); // mCaptureActive = !mCaptureActive;
		mCaptureChanged.store(true);
	}

	void DragAndDropManager::cursorReleased(const PointerEvent& event)
	{
		if(!mIsDragInProgress)
			return;

		Platform::releaseMouseCapture();

		if(!onDragEnded.empty())
		{
			DragCallbackInfo info;
			onDragEnded(event, info);

			endDrag(info.processed);
		}
		else
			endDrag(false);
	}
}