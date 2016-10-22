#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool Createmap();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> Carnival;
	

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* Carnival_EX;
	SDL_Texture* box;
	SDL_Texture* rick;


	SDL_Texture* RacketLTex;
	SDL_Texture* RacketRTex;

	PhysBody* Racket_left;
	PhysBody* Pivot_letf;

	PhysBody* Racket_Right;
	PhysBody* Pivot_Right;


	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
