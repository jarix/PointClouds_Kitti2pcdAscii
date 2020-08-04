/*-------------------------------------------------------------------*\

  NAME
    kitti2pcd_asc


  DESCRIPTION
    Converts binary Point cloud files from the Kitti dataset 
    to a PCD ASCII format

    The KITTI dataset:
    http://www.cvlibs.net/datasets/kitti/

    PCD File format:
    http://pointclouds.org/documentation/tutorials/pcd_file_format.html

  AUTHOR
    Jari Honkanen

\*-------------------------------------------------------------------*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace boostfs = boost::filesystem;

typedef struct {
    float x;
    float y;
    float z;
    float i;
} PointT;


enum OperModeE
{
    CONVERT_SINGLE_FILE = 0,
    CONVERT_DIRECTORY = 1
};

static OperModeE operMode = CONVERT_SINGLE_FILE;


/*---------------------------------------------------------------*\
    Read KITTI file
\*---------------------------------------------------------------*/
int readKittiFile(std::string inFileName, std::vector<PointT> &pointCloudVector, bool bDebugPrint)
{
    std::ifstream pcInFile(inFileName, std::ios::binary);
    if (!pcInFile) {
        std::cerr << "*** Error: readKittiFile(): Point Cloud file'" << inFileName << "' not found." << std::endl;
        return 0;
    }

    // get length of input file:
    pcInFile.seekg(0, pcInFile.end);
    int length = (int)pcInFile.tellg();
    pcInFile.seekg(0, pcInFile.beg);

    int numValues = length / sizeof(float);

    // Read data into buffer vector of floats
    std::vector<float> buffer(numValues);
    pcInFile.read(reinterpret_cast<char *>(&buffer[0]), numValues*sizeof(float));
    pcInFile.close();

    PointT point;

    for (uint32_t i = 0; i < buffer.size(); i+=4) {
        point.x = buffer[i];
        point.y = buffer[i+1];
        point.z = buffer[i+2];
        point.i = buffer[i+3];
        pointCloudVector.push_back(point);
    }
    if (bDebugPrint) {
        std::cout << "File '" << inFileName << "' contains " << length << " bytes and " << pointCloudVector.size() << " points" << std::endl;
    }

    return 1;
}



/*---------------------------------------------------------------*\
    Write PCD file
\*---------------------------------------------------------------*/
int writePcdFile(std::string outFileName, std::vector<PointT> &pointCloudVector, bool bDebugPrint)
{
    std::ofstream pcOutFile(outFileName);
    if (!pcOutFile) {
        std::cerr << "*** Error: writePcdFile(): Could not open output file '" << outFileName << "'" << std::endl;
        return 0;
    }

    // Write the PCD Header
    pcOutFile << "# .PCD v.7 - Point Cloud Data file format" << std::endl;
    pcOutFile << "VERSION .7" << std::endl;
    pcOutFile << "FIELDS x y z intensity" << std::endl;
    pcOutFile << "SIZE 4 4 4 4" << std::endl;
    pcOutFile << "TYPE F F F F" << std::endl;
    pcOutFile << "COUNT 1 1 1 1" << std::endl;
    pcOutFile << "WIDTH " << pointCloudVector.size() << std::endl;
    pcOutFile << "HEIGHT 1" << std::endl;
    pcOutFile << "POINTS " << pointCloudVector.size() << std::endl;
    pcOutFile << "DATA ASCII" << std::endl;
    pcOutFile << std::setprecision(6);
    // Write points
    for (uint32_t i = 0; i < pointCloudVector.size(); i++) {
        pcOutFile << pointCloudVector[i].x << " " << pointCloudVector[i].y << " " << pointCloudVector[i].z << " " << pointCloudVector[i].i << std::endl;
    }

    if (bDebugPrint) {
        std::cout << "Wrote " << pointCloudVector.size() << " points to '" << outFileName << "'" << std::endl;
    }

    pcOutFile.close();

    return 1;
}


int main(int argc, char* argv[])
{

    /*------------------------------------------------------------------------*\
       Process Command Line Options
    \*------------------------------------------------------------------------*/

    // Optional options:
    po::options_description optsDesc("Optional Parameters");

    optsDesc.add_options()
        ("help,h","Print this help message");

    std::string srcPath;
    std::string destPath;

    // Required source directory or file
    po::options_description positional_params("Required Paramaters");
    positional_params.add_options()
        ("src", po::value<std::string>(&srcPath)->required(),"Source Directory with KITTI bin files or a single KITTI bin file")
        ("dest", po::value<std::string>(&destPath)->required(),"Destination Directory with PCD files or a single PCD file");
    
    optsDesc.add(positional_params);

    po::positional_options_description posOptsDesc;
    posOptsDesc.add("src",1);  // Expect one argument
    posOptsDesc.add("dest",1);  // Expect one argument

    int unix_style = boost::program_options::command_line_style::unix_style | 
                     boost::program_options::command_line_style::short_allow_next;

    po::variables_map vm;
    try {
        po::store(
            po::command_line_parser(argc, argv)
            .options(optsDesc)
            .positional(posOptsDesc)
            .style(unix_style)
            .run(), vm);
        
        po::notify(vm);

        if (argc < 3 || vm.count("help")) {
            std::cout << "USAGE: " << argv[0] << "\n" << optsDesc << std::endl;
        }

    } catch (po::error &poe) {
        std::cerr << poe.what() << "\n" << "USAGE: " << argv[0] << "\n" << optsDesc << std::endl;
        
        return EXIT_FAILURE;
    }

    // Check if 'srcPath' paramater is a file or a directory
	boostfs::path ps(srcPath);
	if (boostfs::exists(ps)) {
	
	    if (is_regular_file(ps)) {       // is source path just a single file?
            operMode = CONVERT_SINGLE_FILE;
		} else if (is_directory(ps)) {      // is source path a directory?
            operMode = CONVERT_DIRECTORY;
		} else {
      		std::cerr << "*** ERROR: '" << ps << "' is not a regular file or directory!" << std::endl;
            std::cerr << "USAGE: " << argv[0] << "\n" << optsDesc << std::endl;
        
            return EXIT_FAILURE;
		}
 	}
	else {
		std::cerr << "*** ERROR: Source path '" << ps << "' does not exist!" << std::endl;
        std::cerr << "USAGE: " << argv[0] << "\n" << optsDesc << std::endl;
        
        return EXIT_FAILURE;
	}


    // Check if destination directory exist
    if (operMode == CONVERT_DIRECTORY) {
        boostfs::path pd(destPath);
        if (is_regular_file(pd)) {  // Looking for a directory, put destPath is a file
      		std::cerr << "*** ERROR: File '" << pd << "' is not a directory!" << std::endl;
            return EXIT_FAILURE;
        }
        if (!is_directory(pd)) {  // if desth path does not exist, create it
            boostfs::create_directories(pd);
        }
    }

    /*------------------------------------------------------------------------*\
       Read and Write Files
    \*------------------------------------------------------------------------*/
    std::vector<PointT> pointCloudVector;
    
    if (operMode == CONVERT_SINGLE_FILE) {
        readKittiFile(srcPath, pointCloudVector, true);
        writePcdFile(destPath, pointCloudVector, true);

    } else if (operMode == CONVERT_DIRECTORY) {
        
        std::vector<boostfs::path> inFiles(boostfs::directory_iterator{srcPath}, boostfs::directory_iterator{});
        std::vector<boostfs::path>::iterator fileIterator; 

        // sort files in ascending order based on filename 
        sort(inFiles.begin(), inFiles.end());

        fileIterator = inFiles.begin();

        std::cout << "DestPath: " << destPath << std::endl;

        // Iterate over the files in source directory and convert one by one
        while (fileIterator != inFiles.end()) {
            pointCloudVector.clear();
            readKittiFile((*fileIterator).string(), pointCloudVector, true);

            boostfs::path p((*fileIterator).string());
            std::string destFileName = destPath + "/" + p.stem().string() + ".pcd";
            writePcdFile(destFileName, pointCloudVector, true);            
            fileIterator++;
        }

    }

    return EXIT_SUCCESS;

}

