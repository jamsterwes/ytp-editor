#pragma once
#include "public/uilayer.h"

namespace glui {

	// ordered_map<int, IUILayer*> to sort layers
	class UILayer : public IUILayer {
	public:
		UILayer(int clientWidth, int clientHeight);
		~UILayer();
		class IQuadRenderer* getQuadRenderer() override;
		void draw() override;
		void enable() override;
		void disable() override;
	private:
		class IQuadRenderer* _quadRenderer;
		bool _enabled;
	};

}