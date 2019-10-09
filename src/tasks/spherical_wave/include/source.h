#pragma once
#include "vector3d.h"

class Source {
public:
    vec3<double> coord;
    double time;   // время работы источника
    double omega;    // частота источника
    double omegaEnv;    // частота огибающей (по времени)
    vec3<double> width;    // ширина источника
    double startTime;    // время начала работы источника

    Source() : coord(0, 0, 0), time(0), omega(1), omegaEnv(1),
        width(1), startTime(0) {}

    Source(vec3<double> _coord, double _startTime, double _time, double _omega, double _omegaEnv,
        vec3<double> _width) {
        coord = _coord;
        time = _time;
        omega = _omega;
        omegaEnv = _omegaEnv;
        width = _width;
        startTime = _startTime;
    }
    double getEndTime() { return startTime + time; }

    double getJ(vec3<double> coordinate, double t) {
        if (t > time + startTime || t < startTime) return 0;
        if (coordinate.x > coord.x + 0.5*width.x ||
            coordinate.x < coord.x - 0.5*width.x ||
            coordinate.y > coord.y + 0.5*width.y ||
            coordinate.y < coord.y - 0.5*width.y ||
            coordinate.z > coord.z + 0.5*width.z ||
            coordinate.z < coord.z - 0.5*width.z)
            return 0;
        return pow(cos(constants::pi*(coordinate.x - coord.x) / width.x), 2) *
            pow(cos(constants::pi*(coordinate.y - coord.y) / width.y), 2) *
            pow(cos(constants::pi*(coordinate.z - coord.z) / width.z), 2) *
            sin(omega*(t - startTime))*
            pow(sin(omegaEnv*(t - startTime)), 2);
    }
};
