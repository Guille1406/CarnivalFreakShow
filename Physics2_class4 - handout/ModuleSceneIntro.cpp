#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "p2SString.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;

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
	StartTextures();
	//
	//AudioFx
	StartAudioFx();
	

	//kickers & Spring
	Startkickers();
	
	//Map
	Createmap();
	
	Sensors.add(App->physics->CreateRectangleSensor(120,75,18,15, SENSOR));
	Sensors.add(App->physics->CreateRectangleSensor(150, 75, 18, 15, SENSOR));
	Sensors.add(App->physics->CreateRectangleSensor(180, 75, 18, 15, SENSOR));
	Sensors.add(App->physics->CreateRectangleSensor(210, 75, 18, 15, SENSOR));
	Sensors.add(App->physics->CreateRectangleSensor(210, 75, 18, 15, SENSOR)); 
	Sensors.add(App->physics->CreateRectangleSensor(30, 220, 20, 20, RAIL_SENSOR_UP));
	Sensors.add(App->physics->CreateRectangleSensor(35, 250, 20, 20, RAIL_SENSOR_DOWN));
	Sensors.add(App->physics->CreateRectangleSensor(160,500, 120, 20, BALLOUT));
	spawner = true;
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		App->audio->PlayFx(launcher);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		Racket_left->body->ApplyForceToCenter(b2Vec2(0.0f, -35.0f), true);

	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		Racket_left->body->ApplyForceToCenter(b2Vec2(0.0f, -35.0f), true);
		App->audio->PlayFx(kickerleft_fx);
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
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		Racket_Right->body->ApplyForceToCenter(b2Vec2(0.0f, -25.0f), true);
		Racket_Rightop->body->ApplyForceToCenter(b2Vec2(0.0f, -25.0f), true);
		App->audio->PlayFx(kickerleft_fx);
	}
	else
	{
		Racket_Right->body->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
		Racket_Rightop->body->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
	}

	if(spawner==true)
	{
		spawner_timer = SDL_GetTicks();
		spawner = false;
		spawn = true;
	}
	if (SDL_GetTicks() > spawner_timer + 1000 && spawn==true) {
		b2BodyType Dyn = b2_dynamicBody;
		circles.add(App->physics->CreateCircle(340, 430, 7.5f, Dyn, NULL, BALL));
		circles.getLast()->data->listener = this;
		spawn = false;
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
		App->renderer->Blit(sprites, 108, 221, &(Centercircle.GetCurrentFrame()));
		App->renderer->Blit(sprites, 539, 352, &(chewbacca.GetCurrentFrame()));
		App->renderer->Blit(sprites, 409, 387, &(eyes.GetCurrentFrame()));

		


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
		score += 100* multiplier;
		hitbandL = false;
	}
	if (SDL_GetTicks() <= (timerHBL +300)) {
		App->renderer->Blit(sprites, 63, 364, &(hitbandL_Tex.GetCurrentFrame()));
	}
	if (twoHun1 == true) {
	
		App->renderer->Blit(twohundred_Tex, 110, 37, NULL, 1.0f, 0);
	}
	if (twoHun2 == true) {

		App->renderer->Blit(twohundred_Tex, 137, 37, NULL, 1.0f, 0);
	}
	if (twoHun3 == true) {

		App->renderer->Blit(twohundred_Tex, 167, 37, NULL, 1.0f, 0);
	}
	if (twoHun4 == true) {

		App->renderer->Blit(twohundred_Tex, 195, 37, NULL, 1.0f, 0);
	}
	if (twoHun1 == true && twoHun2 == true && twoHun3 == true && twoHun4 == true) {
		fiveXbool = true;
		timefiveX = SDL_GetTicks();
		twoHun1 = twoHun2 = twoHun3 = twoHun4 = false;
	}
	if (fiveXbool == true && SDL_GetTicks() <= (timefiveX + 15000)) {

		App->renderer->Blit(fivex_Tex, 150, 105, NULL, 1.0f, 0);
	}
	if (SDL_GetTicks() > multi_timer + 8000) {
		multiplier = 1;
	}
	if (fiveXbool == true && SDL_GetTicks() <= (timefiveX +2000)) {
		App->renderer->Blit(sprites,150, 105, &(fiveX.GetCurrentFrame()));
		App->audio->PlayFx(fiveX_fx);
		multi_timer = SDL_GetTicks();
		multiplier = 5;
	}
	
	if (hitbandR == true) {
		timerHBR = SDL_GetTicks();
		hitbandR = false;
		score += 100 * multiplier;
	}
	if (SDL_GetTicks() <= (timerHBR + 300)) {
		App->renderer->Blit(sprites, 217, 364, &(hitbandR_Tex.GetCurrentFrame()));
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
	if (score > highscore) {
		highscore = score;
	}

	return UPDATE_CONTINUE;
}



void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(Ballhit_fx);

	App->physics->RailDetect(bodyA, bodyB);


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
			score += 50 * multiplier;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(Bumpers1);
			bumper2 = true;
			score += 50 * multiplier;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(Bumpers2);
			bumper3 = true;
			score += 50 * multiplier;
		}
		
		Sx = Sx->next;

		if (bodyB == Sx->data )
		{
			App->audio->PlayFx(bands_fx);
			hitbandL = true;
			score += 5 * multiplier;
		}

		Sx = Sx->next;

		if (bodyB == Sx->data)
		{
			App->audio->PlayFx(bands_fx);
			hitbandR = true;
			score += 5 * multiplier;
		}


		p2List_item<PhysBody*>* iterpoints = Sensors.getFirst();
		if (bodyB == iterpoints->data) {
			if (twoHun1 == false){
				twoHun1 = true;
				score += 200 * multiplier;
			}
			else
			{
				twoHun1 = false;
			}
		}
		iterpoints = iterpoints->next;
		if (bodyB == iterpoints->data) {
			if (twoHun2 == false) {
				twoHun2 = true;
				score += 200 * multiplier;
			}
			else
			{
				twoHun2 = false;
			}
		}
		iterpoints = iterpoints->next;
		if (bodyB == iterpoints->data) {
			if (twoHun3 == false) {
				twoHun3 = true;
				score += 200 * multiplier;
			}
			else
			{
				twoHun3 = false;
			}
		}
		iterpoints = iterpoints->next;
		if (bodyB == iterpoints->data) {
			if (twoHun4 == false) {
				twoHun4 = true;
				score += 200 * multiplier;
			}
			else
			{
				twoHun4 = false;
			}
		}
		//SCORE

		p2SString title("Score:%d Live:%d Highscore:%d  Multiplier%d", score, lives, highscore, multiplier);
		App->window->SetTitle(title.GetString());
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

	int grind[38] = {
		3, 148,
		5, 96,
		12, 60,
		22, 31,
		41, 4,
		54, -6,
		68, 7,
		54, 22,
		41, 44,
		34, 70,
		29, 101,
		28, 180,
		73, 1,
		48, -13,
		-3, 30,
		-2, 145,
		1, 193,
		5, 191,
		2, 151
	};

	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT;
	Carnival.add(App->physics->CreateChain(0,0, Carnival_outside, 114,false, WALLS));
	Carnival.add(App->physics->CreateCircle(115,148, 20, b2_staticBody,1.1f, WALLS));
	Carnival.add(App->physics->CreateCircle(167, 185, 20, b2_staticBody, 1.1f, WALLS));
	Carnival.add(App->physics->CreateCircle(218, 143, 20, b2_staticBody, 1.1f, WALLS));
	Carnival.add(App->physics->CreateChain(68,365, BouncerBandL,10,true, WALLS));
	Carnival.add(App->physics->CreateChain(66, 365, BouncerBandR, 10, true, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, Sticks, 12, false, WALLS));
	Carnival.add(App->physics->CreateChain(30, 0, Sticks, 12, false, WALLS));
	Carnival.add(App->physics->CreateChain(60, 0, Sticks, 12, false, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, RBand,14, false, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, LBand,12, false, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, Rarm, 16, false, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, Larm, 14, false, WALLS));
	Carnival.add(App->physics->CreateChain(242, 61, Rcorner, 20, false, WALLS));
	Carnival.add(App->physics->CreateChain(0, 0, Lcorner, 22, false, WALLS));
	Carnival.add(App->physics->CreateChain(15, 65, grind, 38, false, RAMP));

	

	return true;

}

void ModuleSceneIntro::StartTextures() 
{
	sprites = App->textures->Load("pinball/backgroundall.png");
	Centercircle.PushBack({ 27,154,112,111 });
	Centercircle.PushBack({ 144,156,112,105 });
	Centercircle.PushBack({ 250,158,112,111 });
	Centercircle.PushBack({ 359,154,114,111 });
	Centercircle.PushBack({600,154,114,111 });
	Centercircle.speed = 0.01f;
	//
	chewbacca.PushBack({0,0,118,148});
	chewbacca.PushBack({129 ,0,118,148 });
	chewbacca.PushBack({ 1000 ,0,118,148 });
	chewbacca.speed = 0.005f;
	//
	hitbandL_Tex.PushBack({ 283,18,51,63 });
	hitbandL_Tex.PushBack({ 338,18,51,63 });
	hitbandL_Tex.PushBack({ 391,18,51,63 });
	hitbandL_Tex.PushBack({ 443,18,51,63 });
	hitbandL_Tex.PushBack({ 497,18,51,63 });
	hitbandL_Tex.PushBack({ 551,18,51,63 });
	hitbandL_Tex.speed = 0.16f;
	//
	hitbandR_Tex.PushBack({ 554,89,51,63 });
	hitbandR_Tex.PushBack({ 498,89,51,63 });
	hitbandR_Tex.PushBack({ 446,89,51,63 });
	hitbandR_Tex.PushBack({ 394,89,51,63 });
	hitbandR_Tex.PushBack({ 340,89,51,63 });
	hitbandR_Tex.PushBack({ 285,89,51,63 });
	hitbandR_Tex.speed = 0.16f;
		//
	eyes.PushBack({200,256,47,13});
	eyes.PushBack({ 200,269,47,13 });
	eyes.PushBack({ 200,282,47,13 });
	eyes.PushBack({ 200,295,47,13 });
	eyes.PushBack({ 200,308,47,13 });
	eyes.PushBack({ 200,321,47,13 });
	eyes.PushBack({ 200,334,47,13 });
	eyes.PushBack({ 200,321,47,13 });
	eyes.PushBack({ 200,308,47,13 });
	eyes.PushBack({ 200,295,47,13 });
	eyes.PushBack({ 200,282,47,13 });
	eyes.PushBack({ 200,269,47,13 });
	eyes.PushBack({ 200,256,47,13 });
	eyes.speed = 0.03;
	
	fiveX.PushBack({ 14,371,31,31 });
	fiveX.PushBack({ 55,371,31,31 });
	fiveX.speed = 0.2f;
	//
	fivex_Tex = App->textures->Load("pinball/5x.png");
	circle = App->textures->Load("pinball/wheel.png");
	Carnival_EX = App->textures->Load("pinball/walls.png");
	box = App->textures->Load("pinball/bouncer.png");
	rick = App->textures->Load("pinball/rick_head.png");
	RacketLTex = App->textures->Load("pinball/kickerleft.png");
	RacketRTex = App->textures->Load("pinball/kickerRight.png");
	background = App->textures->Load("pinball/background.png");
	Screen = App->textures->Load("pinball/Screen.png");
	grid = App->textures->Load("pinball/grid.png");
	
	
	Bumper_Tex = App->textures->Load("pinball/button.png");
	twohundred_Tex = App->textures->Load("pinball/Letters/200Active.png");

}
void ModuleSceneIntro::StartAudioFx()
{
	fiveX_fx = App->audio->LoadFx("pinball/SoundFx/soundX5.wav");
	kickerleft_fx = App->audio->LoadFx("pinball/SoundFx/kickerleft.wav");
	Ballhit_fx = App->audio->LoadFx("pinball/SoundFx/ballhit.wav");
	CarnivalMusic_fx = App->audio->LoadFx("pinball/SoundFx/CircusSound.wav");
	bands_fx = App->audio->LoadFx("pinball/SoundFx/Bands.wav");
	Bumpers1 = App->audio->LoadFx("pinball/SoundFx/Bumpers.wav");
	Bumpers2 = App->audio->LoadFx("pinball/SoundFx/Bumpers_2.wav");
	launcher = App->audio->LoadFx("pinball/SoundFx/launcher.wav");
	App->audio->PlayFx(CarnivalMusic_fx);
}

void ModuleSceneIntro::Startkickers() 
{

	Racket_left = App->physics->CreateRacket(120, 462, 1, 1, true, KICKER);
	Pivot_letf = App->physics->CreateCircle(115, 462, 8, b2_staticBody, 0.0f,NONE );
	Racket_Right = App->physics->CreateRacket(145, 460, 1, 1, false, KICKER);
	Pivot_Right = App->physics->CreateCircle(215, 460, 8, b2_staticBody, 0.0f, NONE);
	Racket_Rightop = App->physics->CreateRacket(290, 245, 1, 1, false,KICKER);
	Pivot_Rightop = App->physics->CreateCircle(310, 255, 8, b2_staticBody, 0.0f, NONE );
	App->physics->CreateRevolutionJoint(Racket_left, Pivot_letf, Racket_Right, Pivot_Right, Racket_Rightop, Pivot_Rightop);
	Spring = App->physics->CreateRectangle(338, 450, 18, 10, true,WALLS);
	Pivot_spring = App->physics->CreateRectangle(338, 510, 18, 10, false, WALLS);
	App->physics->CreatePrismaticJoint(Spring, Pivot_spring);

}