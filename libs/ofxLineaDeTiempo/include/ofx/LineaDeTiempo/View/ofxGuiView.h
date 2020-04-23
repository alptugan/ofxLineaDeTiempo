//
//  ofxGuiView.hpp
//  ofxGuiWidgetDOMintegration
//
//  Created by Roy Macdonald on 4/12/20.
//


#include "ofx/MUI/Widget.h"
#include "ofxGui.h"


namespace ofx {
namespace LineaDeTiempo {

class TrackHeader;

template <typename ParamType>
class ofxGuiView
: public MUI::Widget

{
public:
	ofxGuiView(ofParameter<ParamType>& param, float width, TrackHeader* trackHeader);
	
	virtual ~ofxGuiView() = default;

	
	virtual void onDraw() const override;

	virtual void onUpdate() override;
	
	ofxGuiGroup & getOfxGui();
	const ofxGuiGroup & getOfxGui() const;
		
protected:
	
	
    virtual void _onPointerEvent(DOM::PointerUIEventArgs& e) override;


	mutable ofxGuiGroup _gui;

	ofRectangle _guiShape;
	
	float _minimizedHeightFactor = 1;

	TrackHeader* _trackHeader = nullptr;
	
};


} } // ofx::LineaDeTiempo