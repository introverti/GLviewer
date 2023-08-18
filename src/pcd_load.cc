#include <FIleLoader/PcdLoader.h>

int main() {
  const std::string pcd_file_path =
      "/home/xavier/Codes/glviewer/data/pointcloud/frame.pcd";
  viewer::PcdLoader(pcd_file_path);
  return 0;
}