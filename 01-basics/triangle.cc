//-----------------------------------------------------------------------------
//
// Triangles demo: grouping together data and processing
//
//-----------------------------------------------------------------------------

#include <iostream>

struct Point 
{
    double x, y;
};

struct Triangle {
    Point pts[3];

    double square () const;
};

double Triangle::square () const 
{
    double sq = pts[0].x * (pts[1].y + pts[2].y) + 
                pts[1].x * (pts[0].y + pts[2].y) +
                pts[2].x * (pts[0].y + pts[1].y);
    return abs(sq) / 2.0;
}

int main ()
{
    struct Triangle t;
    t.pts[0] = Point{1.0, 1.0};
    t.pts[1] = Point{3.0, 3.0};
    t.pts[2] = Point{1.0, 2.0};
    double square = t.square();
    std::cout << square << std::endl;
}