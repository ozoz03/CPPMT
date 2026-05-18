#pragma once


struct AmmoParams {
	char name[32];
	float mass;
	float drag;
	float lift;
	unsigned int type;
};

struct Point {
	float x;
	float y;
};

struct BallisticParams {
	float xd;
	float yd;
	float zd;
	float targetX;
	float targetY;
	float attackSpeed;
	float accelerationPath;
	char ammo_name[32];
};