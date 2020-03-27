//
//  KeyframedData.cpp
//  tracksAndTimeTest
//
//  Created by Roy Macdonald on 3/6/20.
//

#include "KeyframedData.h"
#include "ofLog.h"
#include "LineaDeTiempo/Controller/Interpolator.h"

namespace ofx {
namespace LineaDeTiempo {

//---------------------------------------------------------------------------------
template<typename T>
KeyframedData_<T>::KeyframedData_()
{
	_lastUpdateTime = std::numeric_limits<uint64_t>::max();
}

//---------------------------------------------------------------------------------
template<typename T>
T KeyframedData_<T>::getValueAtTime(const uint64_t& time)
{

	T val;
	
	if(!_isTimeInDataBounds(val, time)) return val;
	
	if(_timedMap.count(time)) return _timedMap[time]->value;
	
	if(_data.size() < 2)
	{
		ofLogError("KeyframedData_<T>::getValueAtTime") << "it should never get to this point. _data.size is less than 2.";
	}
	
	auto i = _findNextIndex(time);
	if(i <  _data.size()){

		
		return _currentValue = Interpolator::interpolateTimedData(_data[i-1].get() , _data[i].get(), time);
		
	}
	
	ofLogError("KeyframedData_<T>::getValueAtTime") << "it should never get to this point";
	
	return val;
	
	
}
template<typename T>
size_t KeyframedData_<T>::size() const
{
	return _data.size();
}
//---------------------------------------------------------------------------------
template<typename T>
TimedData_<T>* KeyframedData_<T>::add(const T& value, const uint64_t& time)
{
	
	if(_timedMap.count(time))
	{
		ofLogNotice("KeyframeTrackDataManager_<T>::add") << "there is already data at this point. Updating to new value. Returning nullptr";
		_timedMap[time]->value = value;
		return nullptr;
	}
	_addToCollection(value, time);
	
	
	sortData();
	return _timedMap[time];
}
//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::_addToCollection(const T& value, const uint64_t& time)
{
	_data.emplace_back(std::make_unique<TimedData_<T>>(value, time));
	_timedMap[time] = _data.back().get();
}
//---------------------------------------------------------------------------------
template<typename T>
bool KeyframedData_<T>::remove(TimedData_<T>* d)
{
	auto s = _data.size();
	ofRemove(_data,
			 [&](std::unique_ptr<TimedData_<T>>& i)
			 {
		if(i.get() == d){
			_timedMap.erase(i->time);
			return true;
		}else{
			return false;
		}
	});
	
	return s > _data.size();
}
//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::clear()
{
	_timedMap.clear();
	_data.clear();
	
}


//---------------------------------------------------------------------------------
template<typename T>
bool KeyframedData_<T>::update(const uint64_t& time)
{
	auto prev = _currentValue;
	if(!_isTimeInDataBounds(_currentValue, time)){
		_lastUpdateTime = time;
//		return false;
		return (prev != _currentValue);
	}
	
	
	if(_lastUpdateTime == time)
	{
		ofLogError("KeyframedData_<T>::update") << "time reached the maximum time possible or something else went wrong";
		return false;
	}
	
	
	if(_data[_currentIndex]->time == time){// this block might be unnecesary. TODO: Double check
		
		_currentValue = _data[_currentIndex]->value;
		_currentIndex++;
		_lastUpdateTime = time;
		return (prev != _currentValue);
	}
	
	if(_data[_currentIndex]->time < time){
		_currentIndex = _findNextIndex(time);
	}
	
	
	if(_currentIndex == 0)
	{
		_lastUpdateTime = time;
		ofLogError("KeyframedData_<T>::update") << "_currentIndex should never be zero. ";
		
		return false;
	}
	
	if(_currentIndex >= _data.size())
	{
		_lastUpdateTime = time;
		ofLogError("KeyframedData_<T>::update") << "_currentIndex >= _data.size()). this should not happen";
		return false;
	}
	
	_currentValue = Interpolator::interpolateTimedData(_data[_currentIndex-1].get() , _data[_currentIndex].get(), time);
	
	return (prev != _currentValue);
	

	
	
	
}

//---------------------------------------------------------------------------------
template<typename T>
bool KeyframedData_<T>::_isTimeInDataBounds(T& d,  const uint64_t& time)
{
	if(_data.size() == 0)
	{
		ofLogVerbose("KeyframedData_<T>::_isTimeInDataBounds") << "No data available. set to default ";
		d = T();
		return false;
	}
	if(time <= _data[0]->time)
	{
		d = _data[0]->value;
		return false;
	}
	if(_data.back()->time <= time )
	{
		d = _data.back()->value;
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------
template<typename T>
size_t KeyframedData_<T>::_findNextIndex(const uint64_t& time)
{
	for(size_t i = 1; i < _data.size(); i++)
	{
		if(_data[i]->time > time)
		{
			return i;
		}
	}
	return _data.size();
}


//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::sortData()
{
	ofSort(this->_data,
				[](const std::unique_ptr<TimedData_<T>>& a, const std::unique_ptr<TimedData_<T>>& b)
				{
					return a->time < b->time;
				}
		   );
//		   timedDataSort);
}
//---------------------------------------------------------------------------------
template<typename T>
const T& KeyframedData_<T>::getCurrentValue() const
{
	return _currentValue;
}
//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::setValue(const T& value, const uint64_t& time, bool bAddKeyIfNotFound)
{
	if(_timedMap.count(time))
	{
		if(_timedMap[time])
		{
			_timedMap[time]->value = value;
		}
	}
	else if(bAddKeyIfNotFound)
	{
		add(value, time);
	}
}
//---------------------------------------------------------------------------------
template<typename T>
const std::vector< std::unique_ptr< TimedData_<T> > > & KeyframedData_<T>::getData() const
{
	return _data;
}



//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::fromJson(const ofJson& j)
{
	try
	{
		
		
		if(j.at("keyframe_type").get<std::string>() != std::string(typeid(T).name()) ||
		   !j.at("keyframes").is_array()
		   )
		{
			ofLogError("KeyframedData_<T>::fromJson") << "failed. probably json file is malformed or corrupt.";
			return;
		}
		
//		j.at("name").get_to(_name);
		j.at("currentValue").get_to(_currentValue);
		j.at("lastUpdateTime").get_to(_lastUpdateTime);
		j.at("currentIndex").get_to(_currentIndex);
		setEnableKeyframing(j.at("bKeyframingEnabled").get<bool>());


		for(auto& k: j.at("keyframes")){
			_addToCollection(k.at("value").get<T>(), k.at("time").get<uint64_t>());
		}
		if(j.at("num_keyframes") != j.at("keyframes").size())
		{
			ofLogError("KeyframedData_<T>::fromJson") << "key frame number differs from what is on json file. probably json file is malformed or corrupt.";
		}
	}
	catch (nlohmann::json::type_error& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (nlohmann::json::out_of_range& e)
	{
		std::cout << e.what() << '\n';
	}
}

//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::toJson(ofJson& j) const
{
//	j["name"] = _name;
	j["currentValue"] = _currentValue;
	j["lastUpdateTime"] = _lastUpdateTime;
	j["currentIndex"] = _currentIndex;
	j["bKeyframingEnabled"] = isKeyFramingEnabled();
	j["keyframe_type"] = std::string(typeid(T).name());
	j["keyframes"] = nlohmann::json::array();
	for(auto& d: _data){
		if(d) j["keyframes"].emplace_back(*(d.get())) ;
	}
	j["num_keyframes"] = j["keyframes"].size();
}


//---------------------------------------------------------------------------------
template<typename T>
void KeyframedData_<T>::moveAllByTime(const uint64_t& _timeOffset)
{
	for(auto& d: _data){
		d->time += _timeOffset;
	}
}

template<typename DataType>
std::ostream& operator<<(std::ostream& os, const KeyframedData_<DataType>& data)
{
	os << data._name << ", "
	<< data._currentValue << ", "
	<< data._lastUpdateTime << ", "
	<< data._currentIndex << ", "
	<< data._bKeyframingEnabled << ", "
	<< data._data.size();
	for(auto& d: data._data){
		os << ", " << d ;
	}
	return os;
}
template<typename DataType>
std::istream& operator>>(std::istream& is, KeyframedData_<DataType>& data)
{

	is >> data._name;
	is.ignore(2);
	is >> data._currentValue;
	is.ignore(2);
	is >> data._lastUpdateTime;
	is.ignore(2);
	is >> data._currentIndex;
	is.ignore(2);
	is >> data._bKeyframingEnabled;
	is.ignore(2);
	size_t size;
	is >> size;
	//	<< data._data.size();
	DataType d;

	data.clear();

	for(size_t i =0; i < size; i++)
	{
		is.ignore(2);
		is >> d;
		data._addToCollection(d.value, d.time);

	}
	data.sortData();
	return is;

}




} } // ofx::LineaDeTiempo

