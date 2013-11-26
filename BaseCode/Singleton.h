#pragma once
#include <boost/serialization/singleton.hpp>

template<class T>
class Singleton
{
public:
	static T &GetInstance()
	{
		return boost::serialization::singleton<T>::get_mutable_instance();
	}
};
