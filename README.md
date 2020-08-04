# PointClouds_Kitti2PCD

Converts LiDAR Point Cloud files from the Kitti dataset binary format to PCD ASCII format

The KITTI dataset:
http://www.cvlibs.net/datasets/kitti/

PCD File format:
http://pointclouds.org/documentation/tutorials/pcd_file_format.html

## Usage

```
kitti2pcd <input_KITTI_filename> <output_PCD_filename>
```

## Help

```
USAGE: ./kitti2pcd
Optional Parameters:
  -h [ --help ]          Print this help message

Required Paramaters:
  --src arg              Source Directory with KITTI bin files or a single 
                         KITTI bin file
  --dest arg             Destination Directory with PCD files or a single PCD 
                         file
```


