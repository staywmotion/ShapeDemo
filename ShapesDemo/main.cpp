/*!	\file	main.cpp
	\brief	demonstration of polymorphic class hierarchy with shared pointers.
	\author	Ethan Rivers
	\date	2023-12-12
	\copyright	Ethan Rivers
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>

using namespace std;

class Shape {
public:
	virtual double area() const = 0;
	virtual double perimeter() const = 0;
	~Shape() = default;
};

class Circle : public Shape {
public:
	double radius;
	Circle(double r) : radius(r) {}
	double area() const override { return 3.14159 * radius * radius; }
	double perimeter() const override { return 2 * 3.14159 * radius; }
};

class Polygon : public Shape {
public:
	int nSides{ 0 };
};

class Rectangle : public Polygon {
public:
	double length, width;
	Rectangle(double l, double w) : length(l), width(w) { nSides = 4; }
	double area() const override { return length * width; }
	double perimeter() const override { return 2 * (length + width); }
};

class Square : public Rectangle {
public:
	Square(double side) : Rectangle(side, side) {}
};

class Triangle : public Polygon {
public:
	double sides[3];
	Triangle(double a, double b, double c) {
		sides[0] = a;
		sides[1] = b;
		sides[2] = c;
		nSides = 3;
	}

	double area() const override {
		double a = sides[0], b = sides[1], c = sides[2];
		return 0.25 * sqrt((a + b + c) * (-a + b + c) * (a - b + c) * (a + b - c));
	}
	double perimeter() const override{ return sides[0] + sides[1] + sides[2]; }
};

using ShapePtr = shared_ptr<Shape>;

/*
	@name loadList
	@param string, vector<ShapePtr>
	@return void
	@brief parse file and determine what type of shape to make.
	@date 2023-12-12
*/
void loadList(string filename, vector<ShapePtr>& shapes) {
	ifstream infile(filename);
	if (!infile) { exit(EXIT_FAILURE); }

	string line;
	while (getline(infile, line)) {
		char shapeType;
		double a, b, c;
		stringstream ss(line);
		ss >> shapeType;

		switch (shapeType) {
		case 'C':
			ss >> a;
			shapes.push_back(make_shared<Circle>(a));
			break;
		case 'T':
			ss >> a >> b >> c;
			shapes.push_back(make_shared<Triangle>(a, b, c));
			break;
		case 'R':
			ss >> a >> b;
			shapes.push_back(make_shared<Rectangle>(a, b));
			break;
		case 'S':
			ss >> a;
			shapes.push_back(make_shared<Square>(a));
			break;
		default:
			cerr << "Unknown shape: " << shapeType << endl;
		}
	}

	infile.close();
}

// stream insertion operator overload
ostream& operator<<(ostream& os, const ShapePtr& shape) {
	os << typeid(*shape).name() + 6 << endl; // Run-Time Type Information (RTTI)
	return os;
}

int main(int argc, char* argv[]) {
	string filename = "shapes.txt";
	vector<ShapePtr> shapes;
	loadList(filename, shapes);

	unsigned totalPolygons{ 0 };
	double totalPolygonSides{ 0.0 };
	double totalPerimeter{ 0.0 };

	// sort based on area ascending 
	sort(shapes.begin(), shapes.end(), [](ShapePtr a, ShapePtr b) {
		return a->area() < b->area();
		});

	// get total perimeter of all shapes, total polygons, and total polygon number of sides (for average)
	for (auto& shape : shapes) {
		totalPerimeter += shape->perimeter();
		// dynamic pointer cast because we are dealing with shared pointers (raw will be just dynamic cast)
		if (auto polygon = dynamic_pointer_cast<Polygon>(shape)) {
			++totalPolygons;
			totalPolygonSides += polygon->nSides;
		}
	}
	
	// final outputs
	for (auto& shape : shapes)
		cout << shape;
	
	cout << "\nTotal Shapes: " << shapes.size() << endl;
	cout << "Total Perimeter of all shapes: " << totalPerimeter << endl;
	cout << "Total Polygons: " << totalPolygons << endl;
	cout << "Average Polygon Sides: " << totalPolygonSides / totalPolygons << endl;

	return 0;
}