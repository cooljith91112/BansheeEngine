#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	class Renderer
	{
	public:
		virtual const String& getName() const = 0;

		/**
		 * @brief	Renders all cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	 Renders the scene from the perspective of a single camera
		 * 			 // TODO - Temporarily disabled since I moved Camera out of Camelot
		 */
		//virtual void render(const HCamera& camera) = 0;
	};
}