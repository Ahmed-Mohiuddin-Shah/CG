#pragma once
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

struct Point
{
	int x, y;
};

struct Line
{
	int x1, y1, x2, y2;
};

struct PolygonShape
{
	std::vector<Point> points;
};

struct Vertex
{
	Point A;
	Point B;
};

struct ViewPort
{
	int x, y, width, height;
};

struct Params
{
	ViewPort viewPort;
	std::vector<Line> lines;
	std::vector<PolygonShape> polygons;
};

void drawViewPort(ViewPort *viewPort, int screenWidth, int screenHeight)
{
	// drawing viewport at the center of the screen
	int x = (screenWidth - viewPort->width) / 2;
	int y = (screenHeight - viewPort->height) / 2;
	int width = viewPort->width;
	int height = viewPort->height;

	viewPort->x = x;
	viewPort->y = y;

	// extend dotted lines to show the TOP_VIEWPORT, BOTTOM_VIEWPORT, left_VIEWPORT and right_VIEWPORT of the viewport
	glColor3f(1.0, 1.0, 1.0);
	glLineStipple(1, 0x00FF);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glVertex2i(0, y);
	glVertex2i(screenWidth, y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(0, y + height);
	glVertex2i(screenWidth, y + height);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(x, 0);
	glVertex2i(x, screenHeight);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(x + width, 0);
	glVertex2i(x + width, screenHeight);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glColor3f(1.0, 0.55, 0.0);

	glBegin(GL_LINE_LOOP);
	glVertex2i(x, y);
	glVertex2i(x + width, y);
	glVertex2i(x + width, y + height);
	glVertex2i(x, y + height);
	glEnd();

	// write the viewport coordinates at the corners of the viewport
	glColor3f(1.0, 1.0, 0.0);
	glRasterPos2i(x - 20, y - 20);
	std::string s_x_min = std::to_string(viewPort->x);
	std::string s_x_max = std::to_string(viewPort->x + viewPort->width);
	std::string s_y_min = std::to_string(viewPort->y);
	std::string s_y_max = std::to_string(viewPort->y + viewPort->height);
	std::string s = "(" + s_x_min + ", " + s_y_min + ")";
	for (int i = 0; i < s.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glRasterPos2i(x + width + 10, y - 20);
	s = "(" + s_x_max + ", " + s_y_min + ")";
	for (int i = 0; i < s.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glRasterPos2i(x - 20, y + height + 10);
	s = "(" + s_x_min + ", " + s_y_max + ")";
	for (int i = 0; i < s.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glRasterPos2i(x + width + 10, y + height + 10);
	s = "(" + s_x_max + ", " + s_y_max + ")";
	for (int i = 0; i < s.length(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
}

class CohenSutherLandAlgo
{
private:
	double x_max, y_max, x_min, y_min;
	const int INSIDE = 0; // 0000
	const int LEFT = 1;	  // 0001
	const int RIGHT = 2;  // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8;	  // 1000
public:
	double x1, y1, x2, y2;
	CohenSutherLandAlgo()
	{
		x1 = 0.0;
		x2 = 0.0;
		y1 = 0.0;
		y2 = 0.0;
	}
	void getCoordinates(
		double x1, double y1, double x2, double y2);
	void getClippingRectangle(
		int x_min, int y_min, int x_max, int y_max);
	int generateCode(double x, double y);
	void cohenSutherland();
};
void CohenSutherLandAlgo::getCoordinates(
	double x1, double y1, double x2, double y2)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}
void CohenSutherLandAlgo::getClippingRectangle(int x_min, int y_min, int x_max, int y_max)
{
	this->x_min = x_min;
	this->y_min = y_min;
	this->x_max = x_max;
	this->y_max = y_max;
}
int CohenSutherLandAlgo::generateCode(double x, double y)
{
	int code = INSIDE; // intially initializing as being inside
	if (x < x_min)	   // lies to the left of rectangle
		code |= LEFT;
	else if (x > x_max) // lies to the right of rectangle
		code |= RIGHT;
	if (y < y_min) // lies below the rectangle
		code |= BOTTOM;
	else if (y > y_max) // lies above the rectangle
		code |= TOP;
	return code;
}
void CohenSutherLandAlgo::cohenSutherland()
{
	int code1 = generateCode(x1, y1); // Compute region codes for P1.
	int code2 = generateCode(x2, y2); // Compute region codes for P2.
	bool accept = false;			  // Initialize line as outside the rectangular window.
	while (true)
	{
		if ((code1 == 0) && (code2 == 0))
		{
			// If both endpoints lie within rectangle.
			accept = true;
			break;
		}
		else if (code1 & code2)
		{
			// If both endpoints are outside rectangle,in same region.
			break;
		}
		else
		{
			// Some segment of line lies within the rectangle.
			int code_out;
			double x, y;
			// At least one endpoint lies outside the rectangle, pick it.
			if (code1 != 0)
				code_out = code1;
			else
				code_out = code2;
			/*
			 * Find intersection point by using formulae :
			 y = y1 + slope * (x - x1)
			 x = x1 + (1 / slope) * (y - y1)
			 */
			if (code_out & TOP)
			{
				// point is above the clip rectangle
				x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
				y = y_max;
			}
			else if (code_out & BOTTOM)
			{
				// point is below the rectangle
				x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
				y = y_min;
			}
			else if (code_out & RIGHT)
			{
				// point is to the right of rectangle
				y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
				x = x_max;
			}
			else if (code_out & LEFT)
			{
				// point is to the left of rectangle
				y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
				x = x_min;
			}
			// Intersection point x,y is found.
			// Replace point outside rectangle by intersection point.
			if (code_out == code1)
			{
				x1 = x;
				y1 = y;
				code1 = generateCode(x1, y1);
			}
			else
			{
				x2 = x;
				y2 = y;
				code2 = generateCode(x2, y2);
			}
		}
	}
	if (accept)
	{
		std::cout << "Line accepted from " << "(" << x1 << ", "
				  << y1 << ")" << " to " << "(" << x2 << ", " << y2 << ")" << std::endl;
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		glEnd();
		glRasterPos2i(x1 + 10, y1 - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
		glRasterPos2i(x2 + 10, y2 - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'B');
	}
	else
	{
		std::cout << "Line rejected" << std::endl;
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		glEnd();
		glRasterPos2i(x1 + 10, y1 - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
		glRasterPos2i(x2 + 10, y2 - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'B');
	}
}

void drawCohenSutherlandLineClip(Line line, ViewPort viewPort)
{
	CohenSutherLandAlgo c;
	c.getCoordinates(line.x1, line.y1, line.x2, line.y2);
	c.getClippingRectangle(viewPort.x, viewPort.y, viewPort.x + viewPort.width, viewPort.y + viewPort.height);
	c.cohenSutherland();
}

void drawSutherHodgmanPolygonClip(PolygonShape *polygon, ViewPort viewPort)
{

	// getting vertex list
	std::vector<Vertex> vertexList;
	for (int i = 0; i < polygon->points.size(); i++)
	{
		Vertex vertex;
		vertex.A = polygon->points[i];
		vertex.B = polygon->points[(i + 1) % polygon->points.size()];
		vertexList.push_back(vertex);
	}
	// clipping against TOP_VIEWPORT boundary
	std::vector<Vertex> outputList;
	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex vertex = vertexList[i];
		Vertex nextVertex = vertexList[(i + 1) % vertexList.size()];
		if (vertex.A.y >= viewPort.y && nextVertex.A.y >= viewPort.y)
		{
			outputList.push_back(nextVertex);
		}
		else if (vertex.A.y >= viewPort.y && nextVertex.A.y < viewPort.y)
		{
			Vertex newVertex;
			newVertex.A.x = vertex.A.x + (nextVertex.A.x - vertex.A.x) * (viewPort.y - vertex.A.y) / (nextVertex.A.y - vertex.A.y);
			newVertex.A.y = viewPort.y;
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
		}
		else if (vertex.A.y < viewPort.y && nextVertex.A.y >= viewPort.y)
		{
			Vertex newVertex;
			newVertex.A.x = vertex.A.x + (nextVertex.A.x - vertex.A.x) * (viewPort.y - vertex.A.y) / (nextVertex.A.y - vertex.A.y);
			newVertex.A.y = viewPort.y;
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
			outputList.push_back(nextVertex);
		}
	}

	// clipping against right_VIEWPORT boundary
	vertexList = outputList;
	outputList.clear();
	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex vertex = vertexList[i];
		Vertex nextVertex = vertexList[(i + 1) % vertexList.size()];
		if (vertex.A.x <= viewPort.x + viewPort.width && nextVertex.A.x <= viewPort.x + viewPort.width)
		{
			outputList.push_back(nextVertex);
		}
		else if (vertex.A.x <= viewPort.x + viewPort.width && nextVertex.A.x > viewPort.x + viewPort.width)
		{
			Vertex newVertex;
			newVertex.A.x = viewPort.x + viewPort.width;
			newVertex.A.y = vertex.A.y + (nextVertex.A.y - vertex.A.y) * (viewPort.x + viewPort.width - vertex.A.x) / (nextVertex.A.x - vertex.A.x);
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
		}
		else if (vertex.A.x > viewPort.x + viewPort.width && nextVertex.A.x <= viewPort.x + viewPort.width)
		{
			Vertex newVertex;
			newVertex.A.x = viewPort.x + viewPort.width;
			newVertex.A.y = vertex.A.y + (nextVertex.A.y - vertex.A.y) * (viewPort.x + viewPort.width - vertex.A.x) / (nextVertex.A.x - vertex.A.x);
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
			outputList.push_back(nextVertex);
		}
	}

	// clipping against BOTTOM_VIEWPORT boundary
	vertexList = outputList;
	outputList.clear();
	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex vertex = vertexList[i];
		Vertex nextVertex = vertexList[(i + 1) % vertexList.size()];
		if (vertex.A.y <= viewPort.y + viewPort.height && nextVertex.A.y <= viewPort.y + viewPort.height)
		{
			outputList.push_back(nextVertex);
		}
		else if (vertex.A.y <= viewPort.y + viewPort.height && nextVertex.A.y > viewPort.y + viewPort.height)
		{
			Vertex newVertex;
			newVertex.A.x = vertex.A.x + (nextVertex.A.x - vertex.A.x) * (viewPort.y + viewPort.height - vertex.A.y) / (nextVertex.A.y - vertex.A.y);
			newVertex.A.y = viewPort.y + viewPort.height;
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
		}
		else if (vertex.A.y > viewPort.y + viewPort.height && nextVertex.A.y <= viewPort.y + viewPort.height)
		{
			Vertex newVertex;
			newVertex.A.x = vertex.A.x + (nextVertex.A.x - vertex.A.x) * (viewPort.y + viewPort.height - vertex.A.y) / (nextVertex.A.y - vertex.A.y);
			newVertex.A.y = viewPort.y + viewPort.height;
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
			outputList.push_back(nextVertex);
		}
	}

	// clipping against left_VIEWPORT boundary
	vertexList = outputList;
	outputList.clear();

	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex vertex = vertexList[i];
		Vertex nextVertex = vertexList[(i + 1) % vertexList.size()];
		if (vertex.A.x >= viewPort.x && nextVertex.A.x >= viewPort.x)
		{
			outputList.push_back(nextVertex);
		}
		else if (vertex.A.x >= viewPort.x && nextVertex.A.x < viewPort.x)
		{
			Vertex newVertex;
			newVertex.A.x = viewPort.x;
			newVertex.A.y = vertex.A.y + (nextVertex.A.y - vertex.A.y) * (viewPort.x - vertex.A.x) / (nextVertex.A.x - vertex.A.x);
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
		}
		else if (vertex.A.x < viewPort.x && nextVertex.A.x >= viewPort.x)
		{
			Vertex newVertex;
			newVertex.A.x = viewPort.x;
			newVertex.A.y = vertex.A.y + (nextVertex.A.y - vertex.A.y) * (viewPort.x - vertex.A.x) / (nextVertex.A.x - vertex.A.x);
			newVertex.B.x = nextVertex.A.x;
			newVertex.B.y = nextVertex.A.y;
			outputList.push_back(newVertex);
			outputList.push_back(nextVertex);
		}
	}

	// drawing clipped polygon
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < outputList.size(); i++)
	{
		// displaying vertex name

		glVertex2i(outputList[i].A.x, outputList[i].A.y);
	}
	glEnd();
	for (int i = 0; i < outputList.size(); i++)
	{
		// displaying vertex name

		glRasterPos2i(outputList[i].A.x + 10, outputList[i].A.y - 20);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A' + i);
	}
}

std::vector<Line> getLines()
{
	std::vector<Line> lines;

	// getting input from user
	std::cout << "Enter number of lines: ";
	int n;
	std::cin >> n;
	for (int i = 0; i < n; i++)
	{
		Line line;
		std::cout << "Enter line " << i + 1 << " x1 y1 x2 y2: ";
		std::cin >> line.x1 >> line.y1 >> line.x2 >> line.y2;
		lines.push_back(line);
	}

	// lines.push_back({ 450, 300, 140, 260 });
	// lines.push_back({ 20, 20, 240, 370 });
	// lines.push_back({ 300, 300, 350, 350 });
	// lines.push_back({ 100, 100, 150, 150 });
	// lines.push_back({ 300, 300, 700, 700 });
	// lines.push_back({ 100, 300, 700, 300 });
	// lines.push_back({ 100, 400, 300, 400 });
	// lines.push_back({ 500, 400, 700, 400 });
	// lines.push_back({ 400, 100, 400, 300 });
	// lines.push_back({ 400, 500, 400, 700 });
	// lines.push_back({ 100, 100, 700, 700 });
	// lines.push_back({ 100, 700, 700, 100 });
	// lines.push_back({ 100, 100, 700, 100 });
	// lines.push_back({ 100, 100, 100, 700 });
	// lines.push_back({ 200, 200, 700, 700 });
	// lines.push_back({ 700, 700, 200, 200 });
	// lines.push_back({ 200, 300, 600, 300 });

	return lines;
}

std::vector<PolygonShape> getPolygons()
{

	// getting input from user
	std::vector<PolygonShape> polygons;
	std::cout << "Enter number of polygons: ";
	int n;
	std::cin >> n;
	for (int i = 0; i < n; i++)
	{
		PolygonShape polygon;
		std::cout << "Enter polygon " << i + 1 << " number of points: ";
		int m;
		std::cin >> m;
		for (int j = 0; j < m; j++)
		{
			Point point;
			std::cout << "Enter point " << j + 1 << " x y: ";
			std::cin >> point.x >> point.y;
			polygon.points.push_back(point);
		}
		polygons.push_back(polygon);
	}

	//  PolygonShape polygon;
	//  polygon.points.push_back({ 450, 700 });
	//  polygon.points.push_back({ 700, 400 });
	//  polygon.points.push_back({ 400, 400 });
	//  polygon.points.push_back({ 400, 100 });
	//  polygon.points.push_back({ 100, 400 });
	//  polygons.push_back(polygon);

	return polygons;
}

Params getParams(bool lines = false, bool polygons = false)
{

	Params params;
	ViewPort viewPort;

	std::cout << "Enter ViewPort: " << std::endl;

	std::cout << "Enter width height: ";
	std::cin >> viewPort.width >> viewPort.height;

	params.viewPort = viewPort;

	if (lines)
		params.lines = getLines();

	if (polygons)
		params.polygons = getPolygons();

	return params;
}