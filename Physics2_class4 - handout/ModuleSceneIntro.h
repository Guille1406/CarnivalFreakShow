#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animation.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	void StartTextures();
	void StartAudioFx();
	void Startkickers();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	bool Createmap();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> Carnival;
	
	p2List<PhysBody*> Sensors;
	

// hits
	uint timefiveX;
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
	bool twoHun1;
	bool twoHun2;
	bool twoHun3;
	bool twoHun4;
	bool fiveXbool;

	SDL_Texture* twohundred_Tex;
	SDL_Texture* sprites;
	SDL_Texture* circle;
	SDL_Texture* Carnival_EX;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	SDL_Texture* Screen;
	SDL_Texture* grid;
	SDL_Texture* RacketLTex;
	SDL_Texture* RacketRTex;
	SDL_Texture* Bumper_Tex;
	SDL_Texture* fivex_Tex;
	//
	Animation hitbandR_Tex;
	Animation hitbandL_Tex;
	Animation Centercircle; 
	Animation eyes;
	Animation chewbacca;
	Animation fiveX;

	PhysBody* Racket_left;
	PhysBody* Pivot_letf;

	PhysBody* Racket_Right;
	PhysBody* Pivot_Right;

	PhysBody* Racket_Rightop;
	PhysBody* Pivot_Rightop;

	PhysBody* Spring;
	PhysBody* Pivot_spring;
	//SCORE
	uint score = 10;
	uint lives=3;
	uint highscore=3000;
	uint multiplier = 1;
	uint multi_timer;
	bool spawner;
	uint spawner_timer;
	bool spawn;
	//Audio_FX
	uint fiveX_fx;
	uint kickerleft_fx;
	uint Ballhit_fx;
	uint CarnivalMusic_fx;
	uint bands_fx;
	uint Bumpers1;
	uint Bumpers2;
	uint fatboy;
	uint kickeright;
	uint launcher;
	p2Point<int> ray;
	bool ray_on;
};
