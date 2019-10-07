#pragma once
#include <string>
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
    Field field;
    Coordinate coord;
    Section section;
    State state = on;

public:

    FileWriter() {}
    FileWriter(std::string _dir, Field _field, Coordinate _coord, 
        Section _section, State _state = on) {
        initialize(_dir, _field, _coord, _section, _state);
    }

    void initialize(std::string _dir, Field _field, Coordinate _coord, 
        const Section& _section, State _state = on) {
        dir = _dir;
        field = _field;
        coord = _coord;
        section = _section;
        state = _state;
    }

    void write(const Grid3d& gr, std::string name, Type t = Type::Double,
        std::string message = "") {
        write(gr, name, field, coord, t, message);
    }

    Coordinate getCoord() const {
        return coord;
    }

    Field getField() const {
        return field;
    }

    std::string getDirectory() const {
        return dir;
    }

    Section getSection() const {
        return section;
    }

    void setCoord(Coordinate _coord) {
        coord = _coord;
    }

    void setField(Field _field) {
        field = _field;
    }

    void setDirectory(std::string _directory) {
        dir = _directory;
    }

    void setSection(const Section& _section) {
        section = _section;
    }

    void turnOn() {
        state = on;
    }
    void turnOff() {
        state = off;
    }

protected:
    void write0d(const Grid3d& gr, std::string name, Type type) const;
    void write1d(const Grid3d& gr, std::string name, Type type) const;
    void write2d(const Grid3d& gr, std::string name, Type type) const;
    void write(const Grid3d & gr, std::string name, Type type, std::string si,
        std::string sj, std::string sk) const;

    void write(const Grid3d& gr, std::string name, Field _field, Coordinate _coord,
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
};