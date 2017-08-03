//======================================================================
/*! \file IbeoSdkFileDemo.cpp
 *
 * \copydoc Copyright
 * \author Mario Brumm (mb)
 * \date Jun 1, 2012
 *
 * Demo project for reading IDC files and process the data blocks.
 *///-------------------------------------------------------------------

#include <ibeosdk/lux.hpp>
#include <ibeosdk/ecu.hpp>
#include <ibeosdk/minilux.hpp>
#include <ibeosdk/scala.hpp>

#include <ibeosdk/devices/IdcFile.hpp>
#include <ibeosdk/datablocks/PointCloudPlane7510.hpp>
#include <ibeosdk/datablocks/commands/CommandEcuAppBaseStatus.hpp>
#include <ibeosdk/datablocks/commands/ReplyEcuAppBaseStatus.hpp>
#include <ibeosdk/datablocks/commands/CommandEcuAppBaseCtrl.hpp>
#include <ibeosdk/datablocks/commands/EmptyCommandReply.hpp>

#include "DataInterface/BufferResaved.hpp" // add
#include "Tools/TransNum.hpp"



#ifdef WITHJPEGSUPPORT
#  include <ibeosdk/jpegsupport/jmemio.h>
#endif // WITHJPEGSUPPORT

#ifdef WITHJPEGSUPPORTDEF
#  include <ibeosdk/jpegsupport/jmemio.h>
#endif

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

//======================================================================

using namespace ibeosdk;
using namespace oubaituosdk;

//======================================================================

const ibeosdk::Version::MajorVersion majorVersion(5);
const ibeosdk::Version::MinorVersion minorVersion(0);
const ibeosdk::Version::Revision revision(4);
const ibeosdk::Version::PatchLevel patchLevel;
const ibeosdk::Version::Build build;
const std::string info = "IbeoSdkFileDemo";

ibeosdk::Version appVersion(majorVersion, minorVersion, revision, patchLevel, build, info);

IbeoSDK ibeoSDK;
oubaituosdk::Buffer<ObjectEcuEt> objBuffer;
oubaituosdk::Buffer<ScanPointEcu> ptrBuffer;
oubaituosdk::Buffer<VehicleStateBasicEcu> egoBuffer;
oubaituosdk::TransNum transNum;

typedef std::fstream f_strm;

//======================================================================

void file_demo(const std::string& filename);
void file_create(const std::string& filename);

//======================================================================

TimeConversion tc;
VehicleStateBasicEcu current_vehiclestate;
VehicleStateBasicEcu last_vehiclestate;

//======================================================================

class AllListener : public ibeosdk::DataListener<FrameEndSeparator>,
                    public ibeosdk::DataListener<ScanLux>,
                    public ibeosdk::DataListener<ScanEcu>,
                    public ibeosdk::DataListener<Scan2208>,
                    public ibeosdk::DataListener<ObjectListLux>,
                    public ibeosdk::DataListener<ObjectListEcu>,
                    public ibeosdk::DataListener<ObjectListScala>,
                    public ibeosdk::DataListener<ObjectListScala2271>,
                    public ibeosdk::DataListener<ObjectListEcuEt>,
                    public ibeosdk::DataListener<ObjectListEcuEtDyn>,
                    public ibeosdk::DataListener<RefObjectListEcuEtDyn>,
                    public ibeosdk::DataListener<Image>,
                    public ibeosdk::DataListener<PositionWgs84_2604>,
                    public ibeosdk::DataListener<MeasurementList2821>,
                    public ibeosdk::DataListener<VehicleStateBasicLux>,
                    public ibeosdk::DataListener<VehicleStateBasicEcu2806>,
                    public ibeosdk::DataListener<VehicleStateBasicEcu>,
                    public ibeosdk::DataListener<ObjectAssociationList4001>,
                    public ibeosdk::DataListener<DeviceStatus>,
                    public ibeosdk::DataListener<DeviceStatus6303>,
                    public ibeosdk::DataListener<LogMessageError>,
                    public ibeosdk::DataListener<LogMessageWarning>,
                    public ibeosdk::DataListener<LogMessageNote>,
                    public ibeosdk::DataListener<LogMessageDebug>,
                    public ibeosdk::DataListener<PointCloudPlane7510> {
public:
	virtual ~AllListener() {}

public:
	//========================================
	void onData(const FrameEndSeparator* const fes)
	{
		logInfo << std::setw(5) << fes->getSerializedSize() << " Bytes  "
				<< "Frame received: # " << fes->getFrameId()
				<< "  Frame time: " << tc.toString(fes->getHeaderNtpTime().toPtime())
				<< std::endl;
	}

	//========================================
	void onData(const ScanLux* const scan)
	{
		logInfo << std::setw(5) << scan->getSerializedSize() << " Bytes  "
				<< "ScanLux received: # " << scan->getScanNumber()
				<< "  ScanStart: " << tc.toString(scan->getStartTimestamp().toPtime())
				<< std::endl;
	}

	//========================================
	void onData(const ScanEcu* const scan)
	{
		logInfo << std::setw(5) << scan->getSerializedSize() << " Bytes  "
				<< "ScanEcu received: # " << scan->getScanNumber()
				<< "  #Pts: " << scan->getNumberOfScanPoints()
				<< "  ScanStart: " << tc.toString(scan->getStartTimestamp().toPtime(), 3)
				<< std::endl;

//导出csv
		//get all scanpoints

		std::ofstream outfile;

		outfile.open("/home/oubaituo/scanPoint.csv", std::ios::out | std::ios::app);//收集所有的扫描数据

		std::vector<ScanPointEcu> PointVector;

		// datablock head title
		outfile << "DataType" << ",";
		outfile << "SerializedSize" << ",";
		outfile << "StartTimeStamp" << ",";
		outfile << "EndTimeOffset" << ",";
		outfile << "Flags" << ",";
		outfile << "ScanNumber" << ",";
		outfile << "NumberOfScanPoints" << ",";
		outfile << std::endl;

		// datablock head
		unsigned int s1 = scan->getDataType();
		outfile << transNum.transItoH(s1) << ",";
		outfile << scan->getSerializedSize() << ",";
		outfile << scan->getStartTimestamp() << ",";
		outfile << scan->getEndTimeOffset() << ",";
		std::bitset<32> b2(scan->getFlags());
		outfile << b2 << ",";
		outfile << scan->getScanNumber() << ",";
		outfile << scan->getNumberOfScanPoints() << ",";
//		outfile << scan->getScanPoints();
		outfile << std::endl;
		outfile << std::endl;

		// points title
		outfile << "PositionX" << ",";
		outfile << "PositionY" << ",";
		outfile << "PositionZ" << ",";
		outfile << "EchoPulseWidth" << ",";
		outfile << "TimeOffset" << ",";
		outfile << "Flags" << ",";
		outfile << "SegmentId()" << ",";
		outfile << "SerializedSize" << ",";
		outfile << "DeviceId" << ",";
		outfile << "Layer" << ",";
		outfile << "Echo";
//		outfile << "Reserved" << ",";
		outfile << std::endl;

		for (long unsigned int i = 0; i < scan->getNumberOfScanPoints(); i++)
		{


			ScanPointEcu point = scan->getScanPoints()[i];

			//just show LMS pts
//			if (point.getDeviceId() == 90)
//			{

			//ego relative movement
			double deltaX = current_vehiclestate.getXPos() - last_vehiclestate.getXPos();
			double deltaY = current_vehiclestate.getYPos() - last_vehiclestate.getYPos();
			double deltaRotation = current_vehiclestate.getCourseAngle() - last_vehiclestate.getCourseAngle();
			//compensation the egomotion

			double rotatedX = std::cos(deltaRotation) * point.getPositionX() - std::sin(deltaRotation) * point.getPositionY();
			double rotatedY = std::sin(deltaRotation) * point.getPositionX() + std::cos(deltaRotation) * point.getPositionY();

			double compensatedX = rotatedX + deltaX;
			double compensatedY = rotatedY + deltaY;



			//write the pts to csv file

			outfile << compensatedX << ",";
			outfile << compensatedY << ",";
			outfile << point.getPositionZ() << ",";
			outfile << point.getEchoPulseWidth() << ",";
			outfile << point.getTimeOffset() << ",";
			std::bitset<16> p1(point.getFlags());
			outfile << p1 << ",";
			outfile << point.getSegmentId() << ",";
			outfile << point.getSerializedSize() << ",";
			unsigned long p2 = point.getDeviceId();          // change it
			outfile << p2 << ",";
			unsigned long p3 = point.getLayer();
			outfile << p3 << ",";
			unsigned long p4 = point.getEcho();
			outfile << p4 << "," ;
			outfile << std::endl;

			ptrBuffer.push_back(point);


//			}
		}		outfile << std::endl;

		boost::asio::io_service io_service;
		boost::asio::ip::udp::socket tcp_socket(io_service);
		boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("192.168.0.100"), 12002);

		tcp_socket.open(local_add.protocol());



													    while (1)
													    {
													    	tcp_socket.connect(local_add);


													        boost::asio::ip::udp::endpoint  sendpoint;//请求的IP以及端口


													        tcp_socket.send_to(boost::asio::buffer(PointVector,PointVector.size()), local_add);

										//			        tcp_socket.receive_from(boost::asio::buffer(receive_str, 1024), local_add);//收取

													        std::cout << "收到" << PointVector.size() << "points" << std::endl;

													        break;

													    }

		outfile.close();

	}

	//========================================
	void onData(const Scan2208* const scan)
	{
		logInfo << std::setw(5) << scan->getSerializedSize() << " Bytes  "
				<< "Scan2208 received: # " << scan->getScanNumber()
				<< "  #Pts: " << scan->getSubScans().at(0).getNbOfPoints()
				<< "  ScanStart: " << tc.toString(scan->getSubScans().at(0).getStartScanTimestamp().toPtime(), 3)
				<< std::endl;
	}

	//========================================
	void onData(const ObjectListLux* const objList)
	{
		logInfo << std::setw(5) << objList->getSerializedSize() << " Bytes  " << "ObjectListLux received: # " << objList->getNumberOfObjects() << std::endl;
	}

	//========================================
	void onData(const ObjectListEcu* const objList)
	{
		logInfo << std::setw(5) << objList->getSerializedSize() << " Bytes  " << "ObjectListEcu received: # " << objList->getNumberOfObjects() << std::endl;

		//导出csv
				//get all scanpoints

				std::ofstream outfile;

				outfile.open("/home/oubaituo/test2.csv", std::ios::out | std::ios::app);//收集所有的扫描数据
				for (long unsigned int i = 0; i < objList->getNumberOfObjects(); i++)
				{


					ObjectEcu object = objList->getObjects()[i];

					//write the pts to csv file

					outfile << object.getObjectId() << ",";
					outfile << object.getClassification() << ",";
					outfile << std::endl;
		//			}
				}


				outfile.close();
	}

	//========================================
	void onData(const ObjectListScala* const objList)
	{
		logInfo << std::setw(5) << objList->getSerializedSize() << " Bytes  " << "ObjectListScala received: # " << objList->getNumberOfObjects() << std::endl;
	}

	//========================================
	void onData(const ObjectListScala2271* const objs)
	{
		logInfo << std::setw(5) << objs->getSerializedSize() << " Bytes  "
				<< "ObjectList 2271 received. Scan: " << objs->getScanNumber()
				<< "  ObjLstId: " << int(objs->getObjectListId())
				<< "  #Obj:" << objs->getNumberOfObjects()
				<< std::endl;
	}

	//========================================
	void onData(const ObjectListEcuEt* const objList)
	{
		logInfo << std::setw(5) << objList->getSerializedSize() << " Bytes  " << "ObjectListEcUEts received: # " << objList->getNbOfObjects() << std::endl;

		//导出csv
				//get all objects

				std::ofstream outfile;

				outfile.open("/home/oubaituo/Object.csv", std::ios::out | std::ios::app);//收集所有的扫描数据
//				ObjectListEcuEt* objListModi;
				std::vector<ObjectEcuEt> ObjectVector;

				// datablock head title
				outfile << "DataType" << ",";
				outfile << "SerializedSize" << ",";
				outfile << "ScanStartTimeStamp" << ",";
				outfile << "HeaderDeviceId" << ",";
				outfile << "NbOfObjects" << ",";
				outfile << "Timestamp" << ",";
				outfile << std::endl;

				// datablock head
				unsigned int ol1 = objList->getDataType();
				outfile << transNum.transItoH(ol1) << ",";
				outfile << objList->getSerializedSize() << ",";
				outfile << objList->getScanStartTimestamp() << ",";

				unsigned long ol2 = objList->getHeaderDeviceId();
				outfile << ol2 << ",";

				outfile << objList->getNbOfObjects() << ",";
				outfile << objList->getTimestamp() << ",";
		//		outfile << scan->getScanPoints();
				outfile << std::endl;
				outfile << std::endl;

				// points title
					outfile << "ObjectId" << ",";
					outfile << "Flags" << ",";
					outfile << "Classification" << ",";
					outfile << "trackedByStationaryModel" << ",";
					outfile << "mobile" << ",";
					outfile << "motionModelValidated" << ",";
					outfile << "ObjectAge" << ",";
					outfile << "Timestamp" << ",";
					outfile << "ObjectPredAge" << ",";
					outfile << "ClassCertainty" << ",";
					outfile << "ClassAge" << ",";
					outfile << "ObjBoxCenter" << ","<< ",";
//					outfile << "ObjBoxCenterSigma" << ",";
					outfile << "ObjBoxSize" << "," << ",";
//					outfile << "ObjCourseAngle" << ",";
//							outfile << "ObjCourseAngleSigma" << ",";
							outfile << "ObjBoxOrientation" << ",";
//							outfile << "ObjBoxOrientationSigma" << ",";
							outfile << "RelVelocity" << "," << ",";
//							outfile << "RelVelocitySigma" << ",";
							outfile << "AbsVelocity" << "," << ",";
//							outfile << "AbsVelocitySigma" << ",";
							outfile << "NbOfContourPoints" << ",";
							outfile << "ObjPriority" << ",";
							outfile << "RefPointLocation" ;

//							outfile << "ObjBoxCenterSigma" << ",";
					outfile << std::endl;
					outfile << std::endl;


				for (int i = 0; i < objList->getNbOfObjects(); i++)

				{


					ObjectEcuEt object = objList->getObjects()[i];

					//write the pts to csv file

					outfile << object.getObjectId() << ",";
					std::bitset<16> t2(object.getFlags());
					outfile << t2 << ",";
					outfile << object.getClassification() << ",";
					outfile << object.trackedByStationaryModel() << ",";
					outfile << object.mobile() << ",";
					outfile << object.motionModelValidated() << ",";
					outfile << object.getObjectAge() << ",";
					outfile << object.getTimestamp() << ",";
					outfile << object.getObjectPredAge() << ",";
					unsigned long t1 = object.getClassCertainty();
					outfile << t1 << ",";

					outfile << object.getClassAge() << ",";
					outfile << object.getObjBoxCenter() << "," ;
					outfile << object.getObjBoxSize() << ",";
//					outfile << object.getObjCourseAngle() << ",";
					outfile << object.getObjBoxOrientation() << ",";
					outfile << object.getRelVelocity() << ",";
					outfile << object.getAbsVelocity() << ",";
					unsigned long t3 = object.getNbOfContourPoints();
					outfile << t3 << ",";
					unsigned long t5 = object.getObjPriority();
					outfile << t5 << ",";
					unsigned long t4 = object.getRefPointLocation();
					outfile << t4;
					outfile << std::endl;
					outfile << std::endl;


//					objList->setObjects(object);
					ObjectVector.push_back(object);

					objBuffer.push_back(object);



				}

				    boost::asio::io_service io_service;
					boost::asio::ip::udp::socket tcp_socket(io_service);
					boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("192.168.0.141"), 2000);

							tcp_socket.open(local_add.protocol());


															    while (1)
															    {
															    	tcp_socket.connect(local_add);


															        boost::asio::ip::udp::endpoint  sendpoint;//请求的IP以及端口


															        tcp_socket.send_to(boost::asio::buffer(ObjectVector,ObjectVector.size()), local_add);

												//			        tcp_socket.receive_from(boost::asio::buffer(receive_str, 1024), local_add);//收取

															        std::cout << "收到" << ObjectVector.size() << "objects" << std::endl;


															        break;
															    }

				outfile.close();
	}

	//========================================
	void onData(const ObjectListEcuEtDyn* const objList)
	{
		logInfo << std::setw(5) << objList->getSerializedSize() << " Bytes  " << "ObjectListEcuEtDyn received: # " << objList->getNbOfObjects()
				<< std::endl
				<< "  ObjListId: " << toHex(objList->getObjectListId())
				<< "  DevIntfVr: " << toHex(objList->getDeviceInterfaceVersion())
				<< "  ObjListId: " << toHex(int(objList->getDeviceType()))
				<< std::endl;
	}

	//========================================
	void onData(const RefObjectListEcuEtDyn* const refObjList)
	{
		logInfo << std::setw(5) << refObjList->getSerializedSize() << " Bytes  " << "RefObjectListEcuEtDyn received: # " << refObjList->getNbOfObjects()
				<< std::endl
				<< "  ObjListId: " << toHex(refObjList->getObjectListId())
				<< "  DevIntfVr: " << toHex(refObjList->getDeviceInterfaceVersion())
				<< "  ObjListId: " << toHex(refObjList->getDeviceType())
				<< std::endl;
	}


	//========================================
	void onData(const Image* const image)
	{
		logInfo << std::setw(5) << image->getSerializedSize() << " Bytes  " << "Image received: time: " << tc.toString(image->getTimestamp().toPtime()) << std::endl;

#	  ifdef WITHJPEGSUPPORT
		if ((image->getFormat() == Image::JPEG)
		  || (image->getFormat() == Image::MJPEG)) {

			const char* rawBuffer;
			unsigned int compressedSize;

			const bool ok = image->getImageBuffer(rawBuffer, compressedSize);

			if (!ok) {
				return;
			}

			const unsigned int rgbBufferSize = (unsigned int)(image->getWidth() * image->getHeight() * 3);
			unsigned char* rgbBuffer = new unsigned char[size_t(rgbBufferSize)];

			unsigned int width = 0;
			unsigned int height = 0;
			// fill rgbBuffer, width and height
			const int retCode = readJpegFromMemory(rgbBuffer, &width, &height,
			                                     reinterpret_cast<const unsigned char*>(rawBuffer),
			                                     compressedSize);

			logDebug << "ok: " << ok
					<< " retCode: " << retCode
					<< " Size: " << width << " x " << height
					<< std::endl;

			{
				unsigned int cprSize;
				unsigned char* cprBuffer = new unsigned char[rgbBufferSize];
				writeJpegToMemory(rgbBuffer, width, height,
					cprBuffer, rgbBufferSize, 100, &cprSize);

				FILE* f = fopen("jpeg.jpg", "wb");
				fwrite(cprBuffer, cprSize, 1, f);
				delete[] cprBuffer;

			}

			delete[] rgbBuffer;
		} // if image is JPEG or MJPEG
#	  endif // WITHJPEGSUPPORT
	}

	//========================================
	void onData(const PositionWgs84_2604* const wgs84)
	{
		logInfo << std::setw(5) << wgs84->getSerializedSize() << " Bytes  "
				<< "PositionWGS84 received: time: " << tc.toString(wgs84->getPosition().getTimestamp().toPtime())
				<< std::endl;
	}

	//========================================
	void onData(const VehicleStateBasicLux* const vsb)
	{
		logInfo << std::setw(5) << vsb->getSerializedSize() << " Bytes  " << "VSB (LUX) received: time: " << tc.toString(vsb->getTimestamp().toPtime()) << std::endl;
	}

	//========================================
	void onData(const VehicleStateBasicEcu2806* const vsb)
	{
		logInfo << std::setw(5) << vsb->getSerializedSize() << " Bytes  "
				<< "VSB (ECU;old) received: time: " << tc.toString(vsb->getTimestamp().toPtime())
				<< std::endl;
	}

	//========================================
	void onData(const VehicleStateBasicEcu* const vsb)
	{
		logInfo << std::setw(5) << vsb->getSerializedSize() << " Bytes  "
				<< "VSB (ECU) received: time: " << tc.toString(vsb->getTimestamp().toPtime())
				<< std::endl;
	}
	//========================================
	void onData(const MeasurementList2821* const ml)
	{
		logInfo << std::setw(5) << ml->getSerializedSize() << " Bytes  "
				<< "MeasurementList received: time: " << tc.toString(ml->getTimestamp().toPtime())
				<< " LN: '" << ml->getListName() << "' GN: '" << ml->getGroupName() << "'" << "Num: " << ml->getMeasList().getSize()
				<< std::endl;

		typedef std::vector<Measurement> MLVector;

		MLVector::const_iterator itMl = ml->getMeasList().getMeasurements().begin();
		int ctr = 0;
		for (; itMl != ml->getMeasList().getMeasurements().end(); ++itMl, ++ctr)
		{
			logInfo << " M" << ctr << ":" << (*itMl) << std::endl;
		}
	}

	//========================================
	void onData(const ObjectAssociationList4001* const oaList)
	{
		logInfo << std::setw(5) << oaList->getSerializedSize() << " Bytes  "
				<< "ObjectAssociationList4001 received"
				<< std::endl
				<< "  RObjListId: " << toHex(oaList->getRefObjListId())
				<< "  RDevIntfVr: " << toHex(oaList->getRefDevInterfaceVersion())
				<< "  DevType: " << toHex(oaList->getRefDevType())
				<< std::endl
				<< "  DObjListId: " << toHex(oaList->getDutObjListId())
				<< "  DDevIntfVr: " << toHex(oaList->getDutDevInterfaceVersion())
				<< "  DevType: " << toHex(oaList->getDutDevType())
				<< "  # of associations: " << oaList->getObjectAssociations().size()
				<< std::endl;
	}


	//========================================
	void onData(const DeviceStatus* const devStat)
	{
		logInfo << std::setw(5) << devStat->getSerializedSize() << " Bytes  "
				<< "DevStat received"
				<< std::endl;
	}

	//========================================
	void onData(const DeviceStatus6303* const devStat)
	{
		logInfo << std::setw(5) << devStat->getSerializedSize() << " Bytes  "
				<< "DevStat 0x6303 received"
				<< std::endl;
	}

	//========================================
	void onData(const LogMessageError* const logMsg)
	{
		logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
				<< "LogMessage (Error) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
	}

	//========================================
	void onData(const LogMessageWarning* const logMsg)
	{
		logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
				<< "LogMessage (Warning) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
	}

	//========================================
	void onData(const LogMessageNote* const logMsg)
	{
		logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
				<< "LogMessage (Note) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
	}

	//========================================
	void onData(const LogMessageDebug* const logMsg)
	{
		logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
				<< "LogMessage (Debug) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
	}

	//========================================
	void onData(const PointCloudPlane7510* const pcl)
	{
		logInfo << std::setw(5) << pcl->getSerializedSize()  << " Bytes  "
				<< "PointCloudPlane7510 received. Is empty: " << pcl->empty()
				<< "  ReferencePlane at position: " << pcl->getReferencePlane().getGpsPoint().getLatitudeInDeg()
				<< "  " << pcl->getReferencePlane().getGpsPoint().getLongitudeInDeg() << std::endl;
	}

	//========================================

}; // AllListener

//======================================================================
//======================================================================
//======================================================================

class CustomLogStreamCallbackExample : public CustomLogStreamCallback {
public:
	virtual ~CustomLogStreamCallbackExample() {}
public:
	virtual void onLineEnd(const char* const s, const int)
	{
		std::cerr << s << std::endl;
	}
}; // CustomLogStreamCallback


//======================================================================
//======================================================================
//======================================================================

int checkArguments(const int argc, const char** argv, bool& hasLogFile)
{
	const int minNbOfNeededArguments = 2;
	const int maxNbOfNeededArguments = 3;

	bool wrongNbOfArguments = false;
	if (argc < minNbOfNeededArguments) {
		std::cerr << "Missing argument" << std::endl;
		wrongNbOfArguments = true;
	}
	else if (argc > maxNbOfNeededArguments) {
		std::cerr << "Too many argument" << std::endl;
		wrongNbOfArguments = true;
	}

	if (wrongNbOfArguments) {
		std::cerr << argv[0] << " " << " INPUTFILENAME [LOGFILE]" << std::endl;
		std::cerr << "\tINPUTFILENAME Name of the file to use as input instead of a sensor." << std::endl;
		std::cerr << "\tLOGFILE name of the log file. If ommitted, the log output will be performed to stderr." << std::endl;
		return 1;
	}

	hasLogFile = (argc == maxNbOfNeededArguments);
	return 0;
}

//======================================================================

int main(const int argc, const char** argv)
{
	std::cerr << argv[0] << " Version " << appVersion.toString();
	std::cerr << "  using IbeoSDK " << ibeoSDK.getVersion().toString() << std::endl;

	bool hasLogFile;
	const int checkResult = checkArguments(argc, argv, hasLogFile);
	if (checkResult != 0)
		exit(checkResult);
	int currArg = 1;

	std::string filename = argv[currArg++];

	const off_t maxLogFileSize = 1000000;

	LogFileManager logFileManager;
	ibeosdk::LogFile::setTargetFileSize(maxLogFileSize);

	if (hasLogFile) {
		ibeosdk::LogFile::setLogFileBaseName(argv[currArg++]);
	}
	const ibeosdk::LogLevel ll = ibeosdk::logLevelFromString("Debug");
	ibeosdk::LogFile::setLogLevel(ll);

	static CustomLogStreamCallbackExample clsce;

	if (!hasLogFile)
		LogFile::setCustomLogStreamCallback(&clsce);

	logFileManager.start();

	if (hasLogFile) {
		logInfo << argv[0] << " Version " << appVersion.toString()
		        << "  using IbeoSDK " << ibeoSDK.getVersion().toString() << std::endl;
	}

//	file_demo(filename);
	file_create(filename);

	exit(0);
}

//======================================================================

void file_demo(const std::string& filename)
{
	IdcFile file;
	file.open(filename);
	if (file.isOpen()) {
		AllListener allListener;

		file.registerListener(&allListener); //make listeners ready

		const DataBlock* db = NULL;

		unsigned short nbMessages = 0; // # of messages we parsed

		while (file.isGood()) {
			db = file.getNextDataBlock();   // get the datablock
			if (db == NULL) {
				continue; // might be eof or unknown file type
			}
			file.notifyListeners(db);  // registered listener receive the datablock
			++nbMessages;
		}

		logDebug << "EOF reached. " << nbMessages << " known blocks found." << std::endl;
	}
	else {
		logError << "File not readable." << std::endl;
	}
	logInfo << "file_demo done" << std::endl;
}

//======================================================================

void file_create(const std::string& filename)
{
	IdcFile file1, file2, file3;
	file1.open(filename);
	file2.create("/home/oubaituo/workspace/resaved.idc");
//	file3.create("/home/oubaituo/workspace/modified.idc");
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket tcp_socket(io_service);
	boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("192.168.0.100"), 12002);
//	boost::asio::ip::tcp::acceptor m_acceptor;

	tcp_socket.open(local_add.protocol());
//	tcp_socket.bind(local_add);

	std::deque<ObjectEcuEt> objTemp;
	std::deque<ScanPointEcu> ptrTemp;

	if (file1.isOpen()) {
		AllListener allListener;

		file1.registerListener(&allListener);

		const DataBlock* db = NULL;
		logInfo << "1" << std::endl;
		IbeoDataHeader dh ;


		unsigned short nbMessages = 0; // # of messages we parsed

		while (file1.isGood()) {
			db = file1.getNextDataBlock();

			if (db == NULL) {
				continue; // might be eof or unknown file type
			}
			file1.notifyListeners(db);  // process in on data
			++nbMessages;

			if (objBuffer.size() != 0) {

				objTemp = objBuffer.getBuffer();
		}
			if (ptrBuffer.size() != 0) {

				ptrTemp = ptrBuffer.getBuffer();
			}

			const DataBlock* db2 = NULL;
					db2 = file1.getNextDataBlock();
					if (db2 == NULL) {
									continue; // might be eof or unknown file type
								}

					dh.setDataType(db2->getDataType());
							    dh.setPreviousMessageSize(0);
								dh.setMessageSize(db2->getSerializedSize());
								dh.setDeviceId(db2->getHeaderDeviceId());
								dh.setTimestamp(db2->getHeaderNtpTime());
					file2.write(dh,db2);

					std::ofstream idcoutfile;
					idcoutfile.open("/home/oubaituo/workspace/modified.idc", std::ios::out | std::ios::app);
					idcoutfile << dh.getDataType() << dh.getPreviousMessageSize() << dh.getMessageSize() << dh.getDeviceId() << dh.getTimestamp();
					ptrBuffer.write(idcoutfile) ;
					idcoutfile << dh.getDataType() << dh.getPreviousMessageSize() << dh.getMessageSize() << dh.getDeviceId() << dh.getTimestamp();
					objBuffer.write(idcoutfile) ;

//					file3.(dh,ptrTemp,dh,objTemp);
//					const DataBlock* db3 = NULL;
//					std::ostream<IbeoDataHeader> = dh;
//					db3->serialize(dh);


					 char receive_str[1024] = { 0 };//字符串

									    while (1)
									    {
									    	tcp_socket.connect(local_add);

									        boost::asio::ip::udp::endpoint  sendpoint;//请求的IP以及端口

									        tcp_socket.send_to(boost::asio::buffer(db2,db2->getSerializedSize()), local_add);
						//			        tcp_socket.receive_from(boost::asio::buffer(receive_str, 1024), local_add);//收取
									        std::cout << "收到" << db2->getSerializedSize() << "db" << std::endl;

									        memset(receive_str, 0, 1024);//清空字符串

									        break;

									    }

						logInfo << "3" << std::endl;









		}

		logDebug << "EOF reached. " << nbMessages << " known blocks found." << std::endl;



	}
	else {
		logError << "File not readable." << std::endl;
	}

	logInfo << "file_create done" << std::endl;
}

//======================================================================

//void Connect( std::string * server, std::string *message ) {
//   try{
//
//      // Create a TcpClient.
//      // Note, for this client to work you need to have a TcpServer
//      // connected to the same address as specified by the server, port
//      // combination.
//      int port = 13000;
//      TcpClient* client = gcnew TcpClient( server,port );
//
//      // Translate the passed message into ASCII and store it as a Byte array.
//      array<Byte>^data = Text::Encoding::ASCII->GetBytes( message );
//
//      // Get a client stream for reading and writing.
//      //  Stream stream = client->GetStream();
//
//      NetworkStream^ stream = client->GetStream();
//
//      // Send the message to the connected TcpServer.
//      stream->Write( data, 0, data->Length );
//
//      Console::WriteLine( "Sent: {0}", message );
//
//      // Receive the TcpServer::response.
//
//      // Buffer to store the response bytes.
//      data = gcnew array<Byte>(256);
//
//      // String to store the response ASCII representation.
//      String^ responseData = String::Empty;
//
//      // Read the first batch of the TcpServer response bytes.
//      Int32 bytes = stream->Read( data, 0, data->Length );
//      responseData = Text::Encoding::ASCII->GetString( data, 0, bytes );
//      Console::WriteLine( "Received: {0}", responseData );
//
//      // Close everything.
//      client->Close();
//   }
//   catch ( ArgumentNullException^ e )
//   {
//      Console::WriteLine( "ArgumentNullException: {0}", e );
//   }
//   catch ( SocketException^ e )
//   {
//      Console::WriteLine( "SocketException: {0}", e );
//   }
//
//   Console::WriteLine( "\n Press Enter to continue..." );
//   Console::Read();
//}

//======================================================================
