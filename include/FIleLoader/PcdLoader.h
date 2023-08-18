#pragma once
// Load binary pcd file
// https://pointclouds.org/documentation/tutorials/pcd_file_format.html
// Path: include\FIleLoader\PcdLoader.h
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/ScienceEnum.h"
#include "logger/logger.h"

// #define STR2(x) #x
// #define STR(x) STR2(x)

using science::enum_from_name;
using science::get_enum_name;

namespace viewer {

enum class PcdHeader_KeyWords {
  VERSION,
  FIELDS,
  SIZE,
  TYPE,
  COUNT,
  WIDTH,
  HEIGHT,
  VIEWPOINT,
  POINTS,
  DATA
};

enum class PcdHeader_DataTypes {
  I,  // int
  U,  // unsigned int
  F,  // float
};

enum class PcdHeader_DataFormat { ASCII, BINARY, BINARY_COMPRESSED };

struct PcdHeader {
  /* VERSION - specifies the PCD file version s*/
  float _version;
  /* FIELDS - specifies the name of each dimension/field that a point can have
   */
  std::vector<std::string> _fields;
  /* SIZE - specifies the size of each dimension in bytes */
  std::vector<size_t> _size;
  /* TYPE - specifies the type of each dimension as a char*/
  std::vector<std::string> _type;
  /* COUNT - specifies how many elements does each dimension have.
  By default, if COUNT is not present, all dimensions’ count is set to 1.*/
  std::vector<size_t> _count;
  /* WIDTH - specifies the width of the point cloud dataset in the number of
   * points. */
  size_t _width;
  /* HEIGHT - specifies the height of the point cloud dataset in the number of
   * points. */
  size_t _height;
  /* VIEWPOINT - specifies an acquisition viewpoint for the points in the
  dataset. The viewpoint information is specified as a translation (tx ty tz) +
  quaternion (qw qx qy qz)*/
  struct {
    float tx = 0;
    float ty = 0;
    float tz = 0;
    float qw = 1;
    float qx = 0;
    float qy = 0;
    float qz = 0;
  } _viewpoint;
  /* POINTS - specifies the total number of points in the cloud.  */
  size_t _points;
  /* DATA - specifies the data type that the point cloud data is stored in.*/
  std::string _data;

  bool _valid = false;

  friend std::ostream& operator<<(std::ostream& os, const PcdHeader& header) {
    os << "VERSION " << header._version << '\n';
    os << "FIELDS ";
    for (const auto& field : header._fields) {
      os << field << ' ';
    }
    os << '\n';
    os << "SIZE ";
    for (const auto& size : header._size) {
      os << size << ' ';
    }
    os << '\n';
    os << "TYPE ";
    for (const auto& type : header._type) {
      os << type << ' ';
    }
    os << '\n';
    os << "COUNT ";
    for (const auto& count : header._count) {
      os << count << ' ';
    }
    os << '\n';
    os << "WIDTH " << header._width << '\n';
    os << "HEIGHT " << header._height << '\n';
    os << "VIEWPOINT " << header._viewpoint.tx << ' ' << header._viewpoint.ty
       << ' ' << header._viewpoint.tz << ' ' << header._viewpoint.qw << ' '
       << header._viewpoint.qx << ' ' << header._viewpoint.qy << ' '
       << header._viewpoint.qz << '\n';
    os << "POINTS " << header._points << '\n';
    os << "DATA " << header._data << '\n';
    return os;
  }

  std::string to_string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }
};

template <typename T>
struct Field {
  std::string _name;
  size_t _size;
  PcdHeader_DataTypes _type;
  size_t _count;

  std::vector<T> _data;
};

struct PointCloudFrame {
  PcdHeader _header;

  std::vector<std::shared_ptr<void>> fields;
};

void PcdLoader(std::string path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return;
  }

  // parse header
  // read header line by line and parse each line
  PcdHeader header;
  std::string line;
  bool header_end = false;
  while (!header_end) {
    if (!std::getline(file, line)) {
      break;
    }
    std::istringstream iss(line);
    std::string key;
    iss >> key;
    int key_enum = 0;

    try {
      key_enum = enum_from_name<PcdHeader_KeyWords>(key);
    } catch (...) {
      continue;
    }

    switch (key_enum) {
      case int(PcdHeader_KeyWords::VERSION): {
        iss >> header._version;
        break;
      }

      case int(PcdHeader_KeyWords::FIELDS): {
        std::string field;
        while (iss >> field) {
          header._fields.push_back(field);
        }
        break;
      }

      case int(PcdHeader_KeyWords::SIZE): {
        size_t size;
        while (iss >> size) {
          header._size.push_back(size);
        }
        break;
      }

      case int(PcdHeader_KeyWords::TYPE): {
        std::string type;
        while (iss >> type) {
          header._type.push_back(type);
        }
        break;
      }

      case int(PcdHeader_KeyWords::COUNT): {
        size_t count;
        while (iss >> count) {
          header._count.push_back(count);
        }
        break;
      }

      case int(PcdHeader_KeyWords::WIDTH): {
        iss >> header._width;
        break;
      }

      case int(PcdHeader_KeyWords::HEIGHT): {
        iss >> header._height;
        break;
      }

      case int(PcdHeader_KeyWords::VIEWPOINT): {
        iss >> header._viewpoint.tx >> header._viewpoint.ty >>
            header._viewpoint.tz >> header._viewpoint.qw >>
            header._viewpoint.qx >> header._viewpoint.qy >>
            header._viewpoint.qz;
        break;
      }

      case int(PcdHeader_KeyWords::POINTS): {
        iss >> header._points;
        break;
      }

      case int(PcdHeader_KeyWords::DATA): {
        iss >> header._data;
        header_end = true;
        break;
      }

      default:
        break;
    }
  }

  // check header
  if (header_end) {
    return;
  }

  log_info("header: {}", header.to_string());

  // parser binary data
  if (header._data == get_enum_name(PcdHeader_DataFormat::BINARY)) {
    if (header._fields.size() != header._size.size() ||
        header._fields.size() != header._type.size() ||
        header._fields.size() != header._count.size()) {
      return;
    }

    // // single point size
    // const size_t point_size =
    //     std::accumulate(header._size.begin(), header._size.end(), 0,
    //                     [](size_t sum, size_t size) { return sum + size; });

    // // read from file
    // std::vector<char> data(header._points * point_size);
    // file.read(data.data(), data.size());

    // // parse data
    // PointCloudFrame frame;
    // frame._header = std::move(header);

    // size_t offset = 0;
    // for (size_t i = 0; i < header._fields.size(); ++i) {
    //   const auto& field_name = header._fields[i];
    //   const auto& field_size = header._size[i];
    //   const auto& field_type = header._type[i];
    //   const auto& field_count = header._count[i];

    //   if (field_type == get_enum_name(PcdHeader_DataTypes::I)) {
    //     Field<int> field;
    //     field._name = field_name;
    //     field._size = field_size;
    //     field._type = PcdHeader_DataTypes::I;
    //     field._count = field_count;

    //     field._data.resize(header._points * field_count);
    //     memcpy(field._data.data(), data.data() + offset, field._data.size());

    //     frame.fields.push_back(std::make_shared<Field<int>>(std::move(field)));
    //   } else if (field_type == get_enum_name(PcdHeader_DataTypes::U)) {
    //     Field<unsigned int> field;
    //     field._name = field_name;
    //     field._size = field_size;
    //     field._type = PcdHeader_DataTypes::U;
    //     field._count = field_count;

    //     field._data.resize(header._points * field_count);
    //     memcpy(field._data.data(), data.data() + offset, field._data.size());

    //     frame.fields.push_back(
    //         std::make_shared<Field<unsigned int>>(std::move(field)));
    //   } else if (field_type == get_enum_name(PcdHeader_DataTypes::F)) {
    //     Field<float> field;
    //     field._name = field_name;
    //     field._size = field_size;
    //     field._type = PcdHeader_DataTypes::F;
    //     field._count = field_count;

    //     field._data.resize(header._points * field_count);
    //     memcpy(field._data.data(), data.data() + offset, field._data.size());

    //     frame.fields.push_back(
    //         std::make_shared<Field<float>>(std::move(field)));
    //   } else {
    //     std::cerr << "Failed to parse header: " << path << '\n';
    //     return;
    //   }

    //   offset += field_size;
    // }
  }
}

}  // namespace viewer