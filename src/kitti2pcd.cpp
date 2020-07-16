/*-------------------------------------------------------------------*\

  NAME
    kitti2pcd


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


typedef struct {
    float x;
    float y;
    float z;
    float r;
} PointT;


int main(int argc, char* argv[])
{
    /*---------------------------------------------------------------*\
       Check command line parameters
    \*---------------------------------------------------------------*/
    if (argc < 3) {
        std::cout << argv[0] << ": Convert KITTI LiDAR binary files to ASCII PCD format" << std::endl;
        std::cout << "Usage: " << argv[0] << " input_KITTI_filename output_PCD_filename" << std::endl;
        return 1;
    }

    std::string inFileName = argv[1];
    std::string outFileName = argv[2];

    /*---------------------------------------------------------------*\
       Read KITTI file
    \*---------------------------------------------------------------*/
    std::ifstream pcInFile(inFileName, std::ios::binary);
    if (!pcInFile) {
        std::cerr << "*** Error: Point Cloud file'" << inFileName << "' not found." << std::endl;
        return 1;
    }

    // get length of file:
    pcInFile.seekg(0, pcInFile.end);
    int length = (int)pcInFile.tellg();
    pcInFile.seekg(0, pcInFile.beg);

    int numValues = length / sizeof(float);
 
    std::vector<float> buffer(numValues);
    pcInFile.read(reinterpret_cast<char *>(&buffer[0]), numValues*sizeof(float));
    pcInFile.close();

    PointT point;
    std::vector<PointT> points;

    for (uint32_t i = 0; i < buffer.size(); i+=4) {
        point.x = buffer[i];
        point.y = buffer[i+1];
        point.z = buffer[i+2];
        point.r = buffer[i+3];
        points.push_back(point);
    }
    std::cout << "File '" << inFileName << "' contains " << length << " bytes and " << points.size() << " points" << std::endl;

    /*---------------------------------------------------------------*\
       Write PCD file
    \*---------------------------------------------------------------*/
    std::ofstream pcOutFile(outFileName);
    if (!pcOutFile) {
        std::cerr << "*** Error: Could not open output file '" << outFileName << "'" << std::endl;
        return 1;
    }

    // Write the PCD Header
    pcOutFile << "# .PCD v.7 - Point Cloud Data file format" << std::endl;
    pcOutFile << "VERSION .7" << std::endl;
    pcOutFile << "FIELDS x y z intensity" << std::endl;
    pcOutFile << "SIZE 4 4 4 4" << std::endl;
    pcOutFile << "TYPE F F F F" << std::endl;
    pcOutFile << "COUNT 1 1 1 1" << std::endl;
    pcOutFile << "WIDTH " << points.size() << std::endl;
    pcOutFile << "HEIGHT 1" << std::endl;
    pcOutFile << "POINTS " << points.size() << std::endl;
    pcOutFile << "DATA ASCII" << std::endl;
    pcOutFile << std::setprecision(6);
    // Write points
    for (uint32_t i = 0; i < points.size(); i++) {
        pcOutFile << points[i].x << " " << points[i].y << " " << points[i].z << " " << points[i].r << std::endl;
    }
    std::cout << "Wrote " << points.size() << " points to '" << outFileName << "'" << std::endl;

    pcOutFile.close();

    return 0;
}

