//
//  BaseTrack.cpp
//  tracksAndTimeTest
//
//  Created by Roy Macdonald on 3/15/20.
//

#include "LineaDeTiempo/View/TrackView.h"
#include "MUI/Utils.h"
#include "LineaDeTiempo/Controller/TrackController.h"

namespace ofx {
namespace LineaDeTiempo {



TrackView::TrackView(DOM::Element* parentView, TrackController* controller)
: BaseTrackView(controller->getId(), parentView)
, _unscaledHeight(BaseTrackView::initialHeight)
, BaseHasController<TrackController>(controller)
{
	_regionsStyle = make_shared<MUI::Styles>();
}

shared_ptr<MUI::Styles> TrackView::getRegionsStyle()
{
	return _regionsStyle;
}

void TrackView::setColor(const ofColor& color)
{
	BaseTrackView::setColor(color);
	
	_regionsStyle->setColor(_color, MUI::Styles::ROLE_BACKGROUND);
	
}


ofRectangle TrackView::timeRangeToRect(const ofRange64u& t) const
{
	ofRectangle r (timeToLocalPosition(t.min), 0,timeToLocalPosition(t.max), getHeight());
	r.width -= r.x;
	return r;
}

float TrackView::timeToLocalPosition(const uint64_t& t) const
{
	return MUI::Math::lerp(t, 0, getTimeControl().getTotalTime(), 0, getWidth());
}

uint64_t TrackView::localPositionToTime(float x) const
{
	return MUI::Math::lerp(x, 0, getWidth(), 0, getTimeControl().getTotalTime());
}

float TrackView::timeToScreenPosition(uint64_t time) const
{
	return DOM::Element::localToScreen({timeToLocalPosition(time),0}).x;
}

uint64_t  TrackView::screenPositionToTime(float x) const
{
	return localPositionToTime(DOM::Element::screenToLocal({x,0}).x);
}

float TrackView::getHeightFactor() const
{
	return _heightFactor;
}

void TrackView::setHeightFactor(float factor)
{
	_heightFactor = factor;
	_unscaledHeight = BaseTrackView::initialHeight * _heightFactor;
}



bool TrackView::removeRegion(RegionController * controller)
{
	if(controller == nullptr) return false;

	auto region = controller->getView();
	if(region){

		return (removeChild(region) != nullptr);
		
	}
	return false;
}

float TrackView::getUnscaledHeight()
{
	return _unscaledHeight;
}
float TrackView::updateScaledShape(float y, float yScale, float width)
{
	auto h = BaseTrackView::initialHeight * _heightFactor * yScale;
	setShape({0, y, width, h});
	updateLayout();
	return h;
}
void TrackView::updateLayout()
{
	for(auto c: children())
	{
		if(c) c->updateLayout();
	}
}

}} //ofx::LineaDeTiempo