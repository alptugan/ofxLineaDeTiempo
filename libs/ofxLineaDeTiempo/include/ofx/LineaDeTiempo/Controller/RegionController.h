////
////  RegionController.hpp
////  tracksAndTimeTest
////
////  Created by Roy Macdonald on 3/6/20.
////
//
#pragma once
#include "LineaDeTiempo/BaseTypes/BaseViewController.h"
//#include "LineaDeTiempo/BaseTypes/BaseController.h"
#include "LineaDeTiempo/BaseTypes/BaseHasTimeControl.h"
#include "LineaDeTiempo/View/RegionView.h"
#include "ofRange.h"
#include "DOM/Node.h"
#include <type_traits>

namespace ofx {
namespace LineaDeTiempo {


class TrackController;


class RegionController
: public DOM::Node
, public BaseHasTimeControl
, public BaseViewController<RegionView>
{
public:

	RegionController(const std::string& name, const ofRange64u& timeRange, TrackController* parentTrack, TimeControl* timeControl);
	
	virtual ~RegionController() = default;
	
	const ofRange64u& getTimeRange() const;
	
	void setTimeRange(const ofRange64u& t);
	
	TrackController * getParentTrack();
	
	const TrackController * getParentTrack() const;
	
	ofEvent<ofRange64u> timeRangeChangedEvent;

	using BaseHasTimeControl::getTimeControl;
	
	virtual void update(uint64_t& t) = 0;
	
protected:
	
	ofRange64u _timeRange;

	TrackController * _parentTrack;

private:
	
	
};


} } // ofx::LineaDeTiempo
