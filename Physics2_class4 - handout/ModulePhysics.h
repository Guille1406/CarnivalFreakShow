#pragma once
#include "Module.h"
#include "Globals.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -8.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum collision {
	BALL = 1,
	WALLS = 2,
	RAMP = 4,
	KICKER = 8,
	NONE = 16,
	BUMPER = 32,
	SENSOR = 64,
	RAIL_SENSOR_UP = 128,
	LETTER_SENSOR = 256,
	RAIL_SENSOR_DOWN = 512,
	BALLOUT = 1024
};


// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius,b2BodyType Type,float rest, collision type);
	PhysBody* CreateRectangle(int x, int y, int width, int height, bool dynamic, collision type);
	PhysBody* CreateRacket(int x, int y, int width, int height, bool side, collision type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, collision typeC);

	void RailDetect(PhysBody* bodyA, PhysBody* bodyB);
	void CreateRevolutionJoint(PhysBody* a, PhysBody* b, PhysBody* c, PhysBody* d, PhysBody* bodye, PhysBody* bodyf);
	void CreatePrismaticJoint(PhysBody* bodya, PhysBody* bodyb);
	PhysBody* CreateChain(int x, int y, int* points, int size, bool rest,collision typeC);
	
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:
	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
};