//
//  TracksPanel.hpp
//  tracksTest
//
//  Created by Roy Macdonald on 2/20/20.
//

#pragma once
#include "MUI/ScrollablePanel.h"


#include "LineaDeTiempo/View/Playhead.h"
#include "LineaDeTiempo/View/TrackGroupView.h"

namespace ofx {
namespace LineaDeTiempo {



class TracksPanelController;
class TrackController;

class TracksPanel
: public TrackGroupView



{
public:
	TracksPanel(const std::string& id, DOM::Element* parentView, const ofRectangle& rect, TracksPanelController* controller);
	
	virtual ~TracksPanel() = default;


	virtual void updateLayout() override;
	
	
	float timeToScreenPosition(uint64_t time) const;
	uint64_t  screenPositionToTime(float x) const;
	

	Playhead * _playhead = nullptr;
	
	TracksClippedView* getClippingView();
	const TracksClippedView* getClippingView()const;
	
	DOM::Element* getContainer();
	const DOM::Element* getContainer() const;
	
	void setup()
	{
		if(tracksView)
		{
			tracksView->setScrollH({0,1});
			tracksView->setScrollV({0,1});
		}
	}
protected:
	
	MUI::TracksScrollPanel* tracksView;
	MUI::ClippedView * headersView;
	
	virtual DOM::Element* _getTracksContainer() override;
	virtual DOM::Element* _getHeadersContainer() override;


	virtual void _updateContainers() override;


	ofEventListeners _tracksContainerListeners;
	
	
	void _updateHeadersFromTracks();

	
	
	void _tracksMoved(DOM::MoveEventArgs& e);
	void _tracksResized(DOM::ResizeEventArgs& e);
	
	
	
	
	ofRectangle _makeHeadersViewRect();
	ofRectangle _makeTracksViewRect();
	
private:
	
	
	
	
};
} } // ofx::LineaDeTiempo
