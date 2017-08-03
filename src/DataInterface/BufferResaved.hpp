//======================================================================
/*! \file BufferResaved.hpp
 *
 *
 *
 *
 *///-------------------------------------------------------------------
#ifndef IBEOSDK_BUFFERRESAVED_HPP_SEEN
#define IBEOSDK_BUFFERRESAVED_HPP_SEEN

//======================================================================


#include <vector>
#include <deque>
#include <memory>
#include <thread>

//======================================================================

//using namespace ibeosdk;

//======================================================================

namespace oubaituosdk {

//======================================================================

template<typename T>
class Buffer  {

public:

    typedef std::deque<T> Data;

	Buffer():buffer_size_{1} {
	};
	Buffer(int buffer_size, T& t): buffer_size_{buffer_size} {
		buffer_.push_back(t);
	};

	void push_back(T& t){

		if (buffer_.size() >= buffer_size_) {
			buffer_.pop_front();
		}
		buffer_.push_back(t);
	};    // resave the data

	T& back(){
		return buffer_.back();
	};             // visit the elements

	Data getBuffer(){
		return buffer_;
	};         // get the data resaved

	size_t size() {return buffer_.size();};

	std::ofstream& write(std::ofstream& os) const
	    {
	        os << &buffer_;
	        return os;
	    }

protected:

	Data buffer_;
	int buffer_size_;
//	int buffer_size_max_;

}; // Buffer

//======================================================================


}// namespace oubaituosdk

//======================================================================

#endif // IBEOSDK_DBPROCESSING_HPP_SEEN

//======================================================================
