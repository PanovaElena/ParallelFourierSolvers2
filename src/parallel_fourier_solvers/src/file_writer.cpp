#include <fstream>
#include <iomanip>
#include <functional>
#include <cmath>
#include "grid3d.h"
#include "file_writer.h"
#include "simple_types.h"
#include "class_member_ptr.h"

void FileWriter::writeArr(const std::function<double(int, int, int)>& value, 
    std::ofstream& file, std::string si, std::string sj, std::string sk) const {
    for (int k = section.startZ; k <= section.endZ; k++) {
        for (int j = section.startY; j <= section.endY; j++) {
            for (int i = section.startX; i <= section.endX; i++)
                file << std::setprecision(15) << value(i, j, k) << si;
            file << sj;
        }
        file << sk;
    }
}

void FileWriter::writeFields(const Grid3d& gr, std::string name, Type type, std::string si,
    std::string sj, std::string sk) const {
    for (int fi = 0; fi < fields.size(); fi++) {
        Field field = fields[fi].first;
        Coordinate coord = fields[fi].second;

        std::string _name = "/" + name + "_" + to_string(field) + to_string(coord) + ".csv";
        std::ofstream file(dir + _name);

        if (type == Double) {
            const Array3d<double>& arr = gr.*getMemberPtrField<double>(field)
                .*getMemberPtrFieldCoord<double>(coord);
            writeArr([arr](int i, int j, int k) {
                return arr(i, j, k);
            }, file, si, sj, sk);
        }
        else if (type == Complex) {
            const Array3d<MyComplex>& arr = gr.*getMemberPtrField<MyComplex>(field)
                .*getMemberPtrFieldCoord<MyComplex>(coord);
            writeArr([arr](int i, int j, int k) {
                return arr(i, j, k).getAbs();
            }, file, si, sj, sk);
        }

        file.close();
    }
}

void FileWriter::writeFieldNorms(const Grid3d& gr, std::string name, Type type, std::string si,
    std::string sj, std::string sk) const {
    for (int fi = 0; fi < fieldNorms.size(); fi++) {
        Field field = fieldNorms[fi];

        std::string _name = "/" + name + "_" + to_string(field) + "_norm.csv";
        std::ofstream file(dir + _name);

        if (type == Double) {
            const Array3d<double>& arrx = gr.*getMemberPtrField<double>(field)
                .*getMemberPtrFieldCoord<double>(Coordinate::x);
            const Array3d<double>& arry = gr.*getMemberPtrField<double>(field)
                .*getMemberPtrFieldCoord<double>(Coordinate::y);
            const Array3d<double>& arrz = gr.*getMemberPtrField<double>(field)
                .*getMemberPtrFieldCoord<double>(Coordinate::z);
            writeArr([arrx, arry, arrz](int i, int j, int k) {
                return sqrt(arrx(i, j, k)*arrx(i, j, k) +
                    arry(i, j, k)*arry(i, j, k) +
                    arrz(i, j, k)*arrz(i, j, k));
            }, file, si, sj, sk);
        }

        file.close();
    }
}

void FileWriter::write0d(const Grid3d& gr, std::string name, Type type) const {
    write(gr, name, type, "\n", "", "");
}

void FileWriter::write1d(const Grid3d& gr, std::string name, Type type) const {
    write(gr, name, type, "\n", "", "");
}

void FileWriter::write2d(const Grid3d& gr, std::string name, Type type) const {
    std::string si, sj, sk;
    setSymbols(section.plane1, si, sj, sk);
    write(gr, name, type, si, sj, sk);
}

void setCoordUsingLocation(Section::LocationOfPlane loc, int& start, int& end, int n) {
    switch (loc) {
    case Section::start:
        start = end = 0;
        break;
    case Section::center:
        start = end = n / 2;
        break;
    case Section::end:
        start = end = n;
        break;
    default:
        break;
    }
}

void Section::setBorders(const Grid3d& gr) {
    startX = startY = startZ = 0;
    endX = gr.sizeReal().x - 1;
    endY = gr.sizeReal().y - 1;
    endZ = gr.sizeReal().z - 1;
    if (plane1 == Plane::XOY || plane2 == Plane::XOY || plane3 == Plane::XOY) {
        LocationOfPlane loc = plane1 == Plane::XOY ? loc1 : plane2 == Plane::XOY ?
            loc2 : plane3 == Plane::XOY ? loc3 : LocationOfPlane::noneLocation;
        setCoordUsingLocation(loc, startZ, endZ, gr.sizeReal().z);
    }
    if (plane1 == Plane::XOZ || plane2 == Plane::XOZ || plane3 == Plane::XOZ) {
        LocationOfPlane loc = plane1 == Plane::XOZ ? loc1 : plane2 == Plane::XOZ ?
            loc2 : plane3 == Plane::XOZ ? loc3 : LocationOfPlane::noneLocation;
        setCoordUsingLocation(loc, startY, endY, gr.sizeReal().y);
    }
    if (plane1 == Plane::YOZ || plane2 == Plane::YOZ || plane3 == Plane::YOZ) {
        LocationOfPlane loc = plane1 == Plane::YOZ ? loc1 : plane2 == Plane::YOZ ?
            loc2 : plane3 == Plane::YOZ ? loc3 : LocationOfPlane::noneLocation;
        setCoordUsingLocation(loc, startX, endX, gr.sizeReal().x);
    }
}

void FileWriter::setSymbols(Section::Plane plane, std::string& si, std::string& sj, std::string& sk) const {
    switch (plane) {
    case Section::XOY:
        si = ";"; sj = "\n"; sk = "";
        break;
    case Section::XOZ:
        si = ";"; sj = ""; sk = "\n";
        break;
    case Section::YOZ:
        si = ""; sj = ";"; sk = "\n";
        break;
    default:
        break;
    }
}