#pragma once

struct Point {
	float x;
	float y;
};

// ДЗ18: специфікація оперує типом Coord. Геометрія в проєкті вже описана
// структурою Point, тож Coord — це просто її псевдонім (та сама математика).
using Coord = Point;
