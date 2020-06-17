#pragma once
#include <string>
#include <vector>
#include "simple_types.h"
class Grid3d;

class Section {
public:
    enum Plane {
        nonePlane,
        XOY,
        XOZ,
        YOZ
    };

    enum LocationOfPlane {
        noneLocation,
        start,
        center,
        end
    };

private:
    int startX, startY, startZ, endX, endY, endZ;

    Dimension dim = Dimension::d0;
    Plane plane1, plane2, plane3;
    LocationOfPlane loc1, loc2, loc3;

    void setBorders(const Grid3d& gr);

public:
    Section() {}
    Section(Plane _plane1, LocationOfPlane _loc1,
        Plane _plane2 = Plane::nonePlane, LocationOfPlane _loc2 = LocationOfPlane::noneLocation,
        Plane _plane3 = Plane::nonePlane, LocationOfPlane _loc3 = LocationOfPlane::noneLocation) :
        plane1(_plane1), loc1(_loc1), plane2(_plane2), loc2(_loc2), plane3(_plane3), loc3(_loc3) {
        if (plane1 == Plane::nonePlane || loc1 == LocationOfPlane::noneLocation) {
            plane1 = plane2 = plane3 = Plane::nonePlane;
            loc1 = loc2 = loc3 = LocationOfPlane::noneLocation;
        }
        setDim();
    }

    void setDim() {
        int idim = 3;
        if (plane1 != Plane::nonePlane && loc1 != LocationOfPlane::noneLocation) idim--;
        if (plane2 != Plane::nonePlane && loc2 != LocationOfPlane::noneLocation) idim--;
        if (plane3 != Plane::nonePlane && loc3 != LocationOfPlane::noneLocation) idim--;
        dim = (Dimension)idim;
    }

    friend class FileWriter;
};

class FileWriter {
private:
    std::string dir = std::string(ROOT_DIR)+"/";
    std::vector<std::pair<Field, Coordinate>> fields;
    std::vector<Field> fieldNorms;
    Section section;
    State state = on;

public:

    FileWriter() {}
    FileWriter(std::string _dir, Section _section, State _state = on) {
        dir = _dir;
        section = _section;
        state = _state;
    }

    void pushField(Field f, Coordinate c) {
        fields.push_back({ f, c });
    }

    void pushFieldNorm(Field f) {
        fieldNorms.push_back(f);
    }

    void write(const Grid3d& gr, std::string name,
        Type t = Type::Double, std::string message = "") {
        if (state == off) return;
        if (message != "") std::cout << message << "\n";
        section.setBorders(gr);
        switch (section.dim) {
        case d0:
            write0d(gr, name, t);
            break;
        case d1:
            write1d(gr, name, t);
            break;
        default:
            write2d(gr, name, t);
            break;
        }
    }

    std::string getDirectory() const {
        return dir;
    }

    void setDirectory(std::string& dir) {
        this->dir = dir;
    }

    State getState() {
        return this->state;
    }

    void setState(State state) {
        this->state = state;
    }

protected:
    void write0d(const Grid3d& gr, std::string name, Type type) const;
    void write1d(const Grid3d& gr, std::string name, Type type) const;
    void write2d(const Grid3d& gr, std::string name, Type type) const;

    void writeFields(const Grid3d & gr, std::string name, Type type, std::string si,
        std::string sj, std::string sk) const;
    void writeFieldNorms(const Grid3d & gr, std::string name, Type type, std::string si,
        std::string sj, std::string sk) const;

    void writeArr(const std::function<double(int, int, int)>& value,
        std::ofstream& file, std::string si, std::string sj, std::string sk) const;

    void write(const Grid3d & gr, std::string name, Type type, std::string si,
        std::string sj, std::string sk) const {
        writeFields(gr, name, type, si, sj, sk);
        writeFieldNorms(gr, name, type, si, sj, sk);
    }

    void setSymbols(Section::Plane plane, std::string& si, std::string& sj, std::string& sk) const;
};