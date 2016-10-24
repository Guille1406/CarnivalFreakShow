#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//textures
	circle = App->textures->Load("pinball/wheel.png"); 
	Carnival_EX = App->textures->Load("pinball/walls.png");
	box = App->textures->Load("pinball/bouncer.png");
	rick = App->textures->Load("pinball/rick_head.png");
	RacketLTex = App->textures->Load("pinball/kickerleft.png");
	RacketRTex = App->textures->Load("pinball/kickerRight.png");
	background = App->textures->Load("pinball/background.png");
	Screen = App->textures->Load("pinball/Screen.png");
	grid = App->textures->Load("pinball/grid.png");
	hitbandL_Tex = App->textures->Load("pinball/hitband.png");
	hitbandR_Tex = App->textures->Load("pinball/hitbandR.png");
	Bumper_Tex = App->textures->Load("pinball/button.png");
	senstest = App->textures->Load("pinball/Letters/P.png");
	//
	//AudioFx
	kickerleft_fx = App->audio->LoadFx("pinball/SoundFx/kickerleft.wav");
	Ballhit_fx = App->audio->LoadFx("pinball/SoundFx/ballhit.wav");
	CarnivalMusic_fx = App->audio->LoadFx("pinball/SoundFx/CircusSound.wav");
	bands_fx = App->audio->LoadFx("pinball/SoundFx/Bands.wav");
	Bumpers1 = App->audio->LoadFx("pinball/SoundFx/Bumpers.wav");
	Bumpers2 = App->audio->LoadFx("pinball/SoundFx/Bumpers_2.wav");
	App->audio->PlayFx(CarnivalMusic_fx);

	//kickers
	Racket_left = App->physics->CreateRacket(120, 462, 1, 1, true);
	Pivot_letf = App->physics->CreateCircle(115, 462, 8,b2_staticBody, 0.0f);
	Racket_Right = App->physics->CreateRacket(145, 460, 1, 1, false);
	Pivot_Right = App->physics->CreateCircle(215, 460, 8, b2_staticBody, 0.0f);
	Racket_Rightop = App->physics->CreateRacket(202, 285, 1, 1, false);
	Pivot_Rightop = App->physics->CreateCircle(272, 285, 8, b2_staticBody, 0.0f);
	App->physics->CreateRevolutionJoint(Racket_left, Pivot_letf, Racket_Right, Pivot_Right, Racket_Rightop, Pivot_Rightop);
 

	//Spring
	Spring = App->physics->CreateRectangle(338, 450, 18, 10, true);
	Pivot_spring = App->physics->CreateRectangle(338, 510, 18, 10, false);
	App->physics->CreatePrismaticJoint(Spring, Pivot_spring);
	
	
	Createmap();
	sensor = App->physics->CreateRectangleSensor(120,75,18,15);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	
	






	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		Spring->body->ApplyForceToCenter(b2Vec2(0.0f, 0.01f), true);
	}
	else
	{
		Spring->body->ApplyForceToCenter(b2Vec2(0.0f, -30.0f), true);
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		Racket_left->body->ApplyForceToCenter(b2Vec2(0.0f, -35.0f), true);
	}
	else
	{
		Racket_left->body->ApplyForceToCenter(b2Vec2(0.0f,10.0f), true);
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		Racket_Right->body->ApplyForceToCenter(b2Vec2(0.0f, -25.0f), true);
		Racket_Rightop->body->ApplyForceToCenter(b2Vec2(0.0f, -25.0f), true);
	}
	else
	{
		Racket_Right->body->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
		Racket_Rightop->body->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
	}
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		b2BodyType Dyn = b2_dynamicBody;
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 7.5f,Dyn  ,NULL));
		circles.getLast()->data->listener = this;
	}
	
	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(510, 690, 30,15, true));
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);
	
	//draw all background
	
	

		App->renderer->Blit(background, 0, 0, NULL, 0);
		App->renderer->Blit(Screen, 365, 0, NULL, 0);
		
		
		int x ,y;
		Racket_left->GetPosition(x, y);
		App->renderer->Blit(RacketLTex, x+4, y-4, NULL, 1.0f, Racket_left->GetRotation()+50,0,2);
		Racket_Right->GetPosition(x, y);
		App->renderer->Blit(RacketRTex, x-25 , y+33 , NULL, 1.0f, Racket_Right->GetRotation()-50,25,-38); //repassar
		Racket_Rightop->GetPosition(x, y);
		App->renderer->Blit(RacketRTex, x - 25, y + 33, NULL, 1.0f, Racket_Rightop->GetRotation() - 50, 25, -38); 
		Spring->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f);


		


	// All draw functions ------------------------------------------------------
	
		p2List_item<PhysBody*>*  c = Carnival.getFirst();

		//Carnival's walls
	while (c != NULL)
	{
		App->renderer->Blit(Carnival_EX, 0, 0, NULL, 1.0f,0);
		c = c->next;
	}


	//circles
	
	c = circles.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		
			App->renderer->Blit(circle, x, y, NULL, 1.0f,0);
		c = c->next;
	}


	//boxes
	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	App->renderer->Blit(grid, 12, 68, NULL, 0);



	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}


	if (hitbandL== true) {
		timerHBL = SDL_GetTicks();
		hitbandL = false;
	}
	if (SDL_GetTicks() <= (timerHBL + 100)) {
		App->renderer->Blit(hitbandL_Tex, 50, 360, NULL, 1.0f, 0);
	}
	if (sens == true) {
		sens2 = SDL_GetTicks();
		sens = false;
	}
	if (SDL_GetTicks() <= (sens2 + 1000)) {
		App->renderer->Blit(senstest, 110, 40, NULL, 1.0f, 0);
	}
	if (hitbandR == true) {
		timerHBR = SDL_GetTicks();
		hitbandR = false;
	}
	if (SDL_GetTicks() <= (timerHBR + 100)) {
		App->renderer->Blit(hitbandR_Tex, 205, 362, NULL, 1.0f, 0);
	}
	if (bumper == true) {
		timerB1 = SDL_GetTicks();
		bumper = false;
	}
	if (SDL_GetTicks() <= (timerB1 + 100)) {
		App->renderer->Blit(Bumper_Tex, 95, 125, NULL, 1.0f, 0);
	}

	if (bumper2 == true) {
		timerB2 = SDL_GetTicks();
		bumper2 = false;
	}
	if (SDL_GetTicks() <= (timerB2 + 100)) {
		App->renderer->Blit(Bumper_Tex, 148,162, NULL, 1.0f, 0);
	}


	if (bumper3 == true) {
		timerB3 = SDL_GetTicks();
		bumper3 = false;
	}
	if (SDL_GetTicks() <= (timerB3 + 100)) {
		App->renderer->Blit(Bumper_Tex, 200, 115, NULL, 1.0f, 0);
	}

	return UPDATE_CONTINUE;
}



void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(Ballhit_fx);
	

	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	p2List_item<PhysBody*>* Sx =  Carnival.getFirst()->next;

		if (bodyB == Sx->data  )
		{
			App->audio->PlayFx(Bumpers1);
			bumper = true;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(Bumpers1);
			bumper2 = true;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(Bumpers2);
			bumper3 = true;
		}
		
		Sx = Sx->next;

		if (bodyB == Sx->data )
		{
			App->audio->PlayFx(bands_fx);
			hitbandL = true;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(bands_fx);
			hitbandR = true;
		}
		if (bodyB == sensor) {
			sens = true;
		}
		
		

	}


bool ModuleSceneIntro::Createmap() 
{
		int Carnival_outside[114] = {
		216, 500,
		363, 498,
		363, 0,
		0, 0,
		0, 498,
		111, 498,
		10, 421,
		11, 313,
		32, 289,
		15, 246,
		14, 211,
		39, 215,
		40, 236,
		53, 260,
		62, 246,
		65, 232,
		63, 218,
		14, 102,
		10, 78,
		12, 56,
		22, 39,
		34, 28,
		57, 24,
		300, 24,
		322, 31,
		337, 44,
		345, 58,
		348, 72,
		348, 494,
		327, 494,
		328, 74,
		321, 58,
		309, 46,
		290, 41,
		224, 40,
		218, 44,
		218, 95,
		227, 95,
		228, 88,
		232, 74,
		240, 61,
		251, 52,
		265, 48,
		281, 48,
		291, 51,
		302, 56,
		309, 62,
		316, 73,
		318, 90,
		315, 110,
		306, 162,
		306, 176,
		317, 258,
		278, 277,
		318, 313,
		318, 425,
		221, 495
	};

		
	/*
	int Spheres[34] = {
		173, 199,
		162, 201,
		152, 209,
		148, 219,
		147, 230,
		151, 241,
		159, 246,
		168, 252,
		178, 253,
		188, 249,
		197, 242,
		201, 234,
		202, 225,
		200, 216,
		194, 207,
		185, 200,
		179, 199
	};
	*/
		int Sticks[12] = {
			130, 62,
			130, 96,
			135, 100,
			140, 95,
			140, 62,
			135, 57
		};

	int RBand[14] = {
		217, 420,
		260, 366,
		263, 364,
		265, 366,
		265, 395,
		221, 425,
		217, 424
	};

	int LBand[12] = {
		64, 367,
		63, 395,
		108, 426,
		114, 423,
		70, 368,
		67, 365
	};
	
	int Rarm[16] = {
		285, 408,
		285, 376,
		289, 371,
		293, 376,
		292, 411,
		227, 462,
		218, 451,
		280, 411
	};
	int Larm [16] = {
		45, 408,
		45, 376,
		41, 372,
		37, 376,
		37, 412,
		101, 460,
		110, 450,
		45, 408
	};


	int Rcorner[20] = {
		6, 60,
		47, 41,
		54, 32,
		56, 22,
		51, 12,
		41, 4,
		27, 4,
		17, 14,
		0, 55,
		2, 59
	};

	int Lcorner[22] = {
		49, 118,
		109, 91,
		110, 46,
		106, 41,
		66, 41,
		49, 45,
		36, 56,
		30, 72,
		31, 89,
		37, 104,
		42, 110
	};
	int BouncerBandL[10] = {
		4, 0,
		0, 4,
		42, 55,
		46, 52,
		5, 1

	};

	int BouncerBandR[10] = {
		149, 51,
		153, 55,
		194, 5,
		190, 2,
		151, 50

	};




	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT;
	Carnival.add(App->physics->CreateChain(0,0, Carnival_outside, 114,false));
	Carnival.add(App->physics->CreateCircle(115,148, 20, b2_staticBody,1.1f));
	Carnival.add(App->physics->CreateCircle(167, 185, 20, b2_staticBody, 1.1f));
	Carnival.add(App->physics->CreateCircle(218, 143, 20, b2_staticBody, 1.1f));
	Carnival.add(App->physics->CreateChain(68,365, BouncerBandL,10,true));
	Carnival.add(App->physics->CreateChain(66, 365, BouncerBandR, 10, true));
	Carnival.add(App->physics->CreateChain(0, 0, Sticks, 12, false));
	Carnival.add(App->physics->CreateChain(30, 0, Sticks, 12, false));
	Carnival.add(App->physics->CreateChain(60, 0, Sticks, 12, false));
	Carnival.add(App->physics->CreateChain(0, 0, RBand,14, false));
	Carnival.add(App->physics->CreateChain(0, 0, LBand,12, false));
	Carnival.add(App->physics->CreateChain(0, 0, Rarm, 16, false));
	Carnival.add(App->physics->CreateChain(0, 0, Larm, 14, false));
	Carnival.add(App->physics->CreateChain(242, 61, Rcorner, 20, false));
	Carnival.add(App->physics->CreateChain(0, 0, Lcorner, 22, false));


	

	return true;

}