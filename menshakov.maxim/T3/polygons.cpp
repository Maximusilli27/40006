#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <numeric>

struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

struct Polygon {
    std::vector<Point> points;

    double area() const {
        double a = 0.0;
        int n = points.size();
        for (int i = 0; i < n; ++i) {
            const Point& p1 = points[i];
            const Point& p2 = points[(i + 1) % n];
            a += (p1.x * p2.y - p2.x * p1.y);
        }
        return std::abs(a) / 2.0;
    }

    int vertexCount() const { return points.size(); }
};

bool parsePolygonLine(const std::string& line, Polygon& poly) {
    std::istringstream iss(line);
    int n;
    if (!(iss >> n)) return false;
    poly.points.clear();
    for (int i = 0; i < n; ++i) {
        char c1, c2, c3;
        int x, y;
        if (!(iss >> c1 >> x >> c2 >> y >> c3)) return false;
        if (c1 != '(' || c2 != ';' || c3 != ')') return false;
        poly.points.push_back({ x, y });
    }
    return poly.points.size() == size_t(n);
}

bool parsePolygonFromCommand(const std::string& cmd, Polygon& poly) {
    std::istringstream iss(cmd);
    std::string action;
    int n;
    if (!(iss >> action >> n)) return false;
    Polygon temp;
    for (int i = 0; i < n; ++i) {
        char c1, c2, c3;
        int x, y;
        if (!(iss >> c1 >> x >> c2 >> y >> c3)) return false;
        if (c1 != '(' || c2 != ';' || c3 != ')') return false;
        temp.points.push_back({ x, y });
    }
    poly = std::move(temp);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: filename is required\n";
        return 1;
    }

    std::ifstream fin(argv[1]);
    if (!fin) {
        std::cerr << "Error: cannot open file\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string line;

    while (std::getline(fin, line)) {
        Polygon p;
        if (parsePolygonLine(line, p))
            polygons.push_back(p);
    }

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string action, param;

        if (!(iss >> action)) {
            std::cout << "<INVALID COMMAND>\n";
            continue;
        }

        if (action == "RECTS") {
            if (iss >> param) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            int count = std::count_if(polygons.begin(), polygons.end(),
                [](const Polygon& p) {
                    if (p.points.size() != 4) return false;

                    auto& pts = p.points;

                    auto isRightAngle = [](const Point& a, const Point& b, const Point& c) {
                        int ba_x = a.x - b.x;
                        int ba_y = a.y - b.y;
                        int bc_x = c.x - b.x;
                        int bc_y = c.y - b.y;
                        return (ba_x * bc_x + ba_y * bc_y) == 0;
                        };

                    return isRightAngle(pts[0], pts[1], pts[2]) &&
                        isRightAngle(pts[1], pts[2], pts[3]) &&
                        isRightAngle(pts[2], pts[3], pts[0]) &&
                        isRightAngle(pts[3], pts[0], pts[1]);
                }
            );

            std::cout << count << "\n";
        }
        else {
            if (!(iss >> param)) {
                std::cout << "111 <INVALID COMMAND>\n";
                continue;
            }

            if (action == "AREA") {
                if (param == "MEAN") {
                    if (polygons.empty()) { std::cout << "0\n"; continue; }
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [](double acc, const Polygon& p) { return acc + p.area(); });
                    std::cout << std::fixed << std::setprecision(1) << sum / polygons.size() << "\n";
                }
                else if (param == "ODD" || param == "EVEN") {
                    auto pred = (param == "ODD")
                        ? [](const Polygon& p) { return p.vertexCount() % 2 == 1; }
                    : [](const Polygon& p) { return p.vertexCount() % 2 == 0; };
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [pred](double acc, const Polygon& p) { return acc + (pred(p) ? p.area() : 0.0); });
                    std::cout << std::fixed << std::setprecision(1) << sum << "\n";
                }
                else {
                    int n = std::stoi(param);
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [n](double acc, const Polygon& p) { return acc + (p.vertexCount() == n ? p.area() : 0.0); });
                    std::cout << std::fixed << std::setprecision(1) << sum << "\n";
                }
            }
            else if (action == "MAX") {
                if (param == "AREA") {
                    if (polygons.empty()) { std::cout << "0\n"; continue; }
                    auto maxIt = std::max_element(polygons.begin(), polygons.end(),
                        [](const Polygon& a, const Polygon& b) { return a.area() < b.area(); });
                    std::cout << std::fixed << std::setprecision(1) << maxIt->area() << "\n";
                }
                else if (param == "VERTEXES") {
                    if (polygons.empty()) { std::cout << "0\n"; continue; }
                    auto maxIt = std::max_element(polygons.begin(), polygons.end(),
                        [](const Polygon& a, const Polygon& b) { return a.vertexCount() < b.vertexCount(); });
                    std::cout << maxIt->vertexCount() << "\n";
                }
                else std::cout << "<INVALID COMMAND>\n";
            }
            else if (action == "MIN") {
                if (param == "AREA") {
                    if (polygons.empty()) { std::cout << "0\n"; continue; }
                    auto minIt = std::min_element(polygons.begin(), polygons.end(),
                        [](const Polygon& a, const Polygon& b) { return a.area() < b.area(); });
                    std::cout << std::fixed << std::setprecision(1) << minIt->area() << "\n";
                }
                else if (param == "VERTEXES") {
                    if (polygons.empty()) { std::cout << "0\n"; continue; }
                    auto minIt = std::min_element(polygons.begin(), polygons.end(),
                        [](const Polygon& a, const Polygon& b) { return a.vertexCount() < b.vertexCount(); });
                    std::cout << minIt->vertexCount() << "\n";
                }
                else std::cout << "<INVALID COMMAND>\n";
            }
            else if (action == "COUNT") {
                int count = 0;
                if (param == "ODD") count = std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& p) { return p.vertexCount() % 2 == 1; });
                else if (param == "EVEN") count = std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& p) { return p.vertexCount() % 2 == 0; });
                else {
                    int n = std::stoi(param);
                    count = std::count_if(polygons.begin(), polygons.end(),
                        [n](const Polygon& p) { return p.vertexCount() == n; });
                }
                std::cout << count << "\n";
            }
            else if (action == "SAME") {
                Polygon target;
                if (!parsePolygonFromCommand(line, target)) { std::cout << "<INVALID COMMAND>\n"; continue; }
                int count = std::count_if(polygons.begin(), polygons.end(),
                    [&target](const Polygon& p) {
                        return p.vertexCount() == target.vertexCount() &&
                            std::is_permutation(p.points.begin(), p.points.end(),
                                target.points.begin());
                    });
                std::cout << count << "\n";
            }
        }
    }

    return 0;
}
