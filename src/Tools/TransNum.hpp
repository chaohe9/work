//======================================================================
/*! \file BufferResaved.hpp
 *
 *
 *
 *
 *///-------------------------------------------------------------------
#ifndef IBEOSDK_TRANSNUM_HPP_SEEN
#define IBEOSDK_TRANSNUM_HPP_SEEN

//======================================================================


#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>


//======================================================================

//using namespace ibeosdk;

//======================================================================

namespace oubaituosdk {

//======================================================================
class TransNum {

public:
	TransNum () = default;
	TransNum (unsigned int i) : ni (i) {}
	TransNum (unsigned short s) : ni (s) {}
	TransNum (unsigned char c) : ni (c) {}

	std::string transItoH (unsigned int );

private:

	unsigned int ni = 0;
	unsigned short ns = 0;
	unsigned char nc = 0;
};

//======================================================================


}// namespace oubaituosdk

//======================================================================

#endif // IBEOSDK_DBPROCESSING_HPP_SEEN

//======================================================================
