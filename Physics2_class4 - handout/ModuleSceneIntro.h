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

// hits

	uint timerHBL;
	uint timerHBR;
	uint timerB1;
	uint timerB2;
	uint timerB3;
	bool hitbandL;
	bool hitbandR;
	bool bumper;
	bool bumper2;
	bool bumper3;
	uint sens2;
	bool sens;




	SDL_Texture* circle;
	SDL_Texture* Carnival_EX;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	SDL_Texture* Screen;
	SDL_Texture* grid;
	SDL_Texture* RacketLTex;
	SDL_Texture* RacketRTex;
	SDL_Texture* hitbandL_Tex;
	SDL_Texture* hitbandR_Tex;
	SDL_Texture* Bumper_Tex;

	SDL_Texture* senstest;

	PhysBody* Racket_left;
	PhysBody* Pivot_letf;

	PhysBody* Racket_Right;
	PhysBody* Pivot_Right;

	PhysBody* Racket_Rightop;
	PhysBody* Pivot_Rightop;

	PhysBody* Spring;
	PhysBody* Pivot_spring;
	
	uint kickerleft_fx;
	uint Ballhit_fx;
	uint CarnivalMusic_fx;
	uint bands_fx;
	uint Bumpers1;
	uint Bumpers2;
	uint fatboy;
	uint kickeright;
	
	p2Point<int> ray;
	bool ray_on;
};
