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

	circle = App->textures->Load("pinball/wheel.png"); 
	Carnival_EX = App->textures->Load("pinball/walls.png");
	box = App->textures->Load("pinball/bouncer.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/Vamos.wav");
	RacketLTex = App->textures->Load("pinball/Racket_left.png");
	RacketRTex = App->textures->Load("pinball/Racket_right.png");



	Racket_left = App->physics->CreateRacket(230, 700, 20, 10);
	Pivot_letf = App->physics->CreateCircle(166, 687, 8,b2_staticBody, 0.0f);
	Racket_Right = App->physics->CreateRectangle(230, 687, 70, 10);

     App->physics->CreateRevolutionJoint(Racket_left,Pivot_letf);

	
	
	Createmap();
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

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
	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		b2BodyType Dyn = b2_dynamicBody;
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 9,Dyn  ,0.0f));
		circles.getLast()->data->listener = this;
	}
	
	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(510, 690, 30,15));
	}

	/*
	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		b2BodyType Dyntype = b2_dynamicBody;
		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64, Dyntype ,0.0f));
	
	}
	*/
	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>*  c = Carnival.getFirst();

	while (c != NULL)
	{
		App->renderer->Blit(Carnival_EX, 0, 0, NULL, 1.0f,0);
		c = c->next;
	}


	int x ,y;
	Racket_left->GetPosition(x, y);
	App->renderer->Blit(RacketLTex, x, y, NULL, 1.0f, Racket_left->GetRotation(),x-50);
	
	c = circles.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		
			App->renderer->Blit(circle, x, y, NULL, 1.0f,0);
		c = c->next;
	}

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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);


	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
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
	int Sticks[16] = {
		285, 92,
		285, 142,
		290, 147,
		295, 147,
		298, 143,
		299, 92,
		295, 88,
		289, 88
	};

	int RBand[14] = {
		329, 641,
		398, 595,
		399, 553,
		396, 548,
		391, 551,
		325, 634,
		325, 640
	};

	int LBand[14] = {
		100, 547,
		106, 550,
		169, 629,
		170, 635,
		167, 639,
		97, 594,
		98, 551
	};
	
	int Rarm[14] = {
		438, 619,
		438, 559,
		431, 558,
		430, 609,
		338, 671,
		343, 689,
		432, 621
	};
	int Larm[14] = {
		56, 562,
		56, 616,
		153, 689,
		163, 676,
		66, 609,
		66, 561,
		60, 558
	};

	int Rcorner[24] = {
		366, 182,
		386, 115,
		396, 103,
		406, 98,
		421, 98,
		432, 103,
		442, 114,
		448, 127,
		445, 143,
		432, 158,
		413, 167,
		371, 184
	};

	int Lcorner[26] = {
		74, 173,
		166, 136,
		166, 67,
		160, 61,
		99, 61,
		80, 68,
		64, 77,
		53, 91,
		47, 108,
		47, 125,
		52, 146,
		61, 159,
		73, 172
	};

	b2BodyType StaticType = b2_staticBody;
	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT;
	Carnival.add(App->physics->CreateChain(0,0, Carnival_outside, 114, StaticType, 0.0f));
	Carnival.add(App->physics->CreateCircle(175,225, 30,StaticType ,1.1f) );
	
	Carnival.add(App->physics->CreateCircle(253, 280, 30, StaticType,1.1f));
	Carnival.add(App->physics->CreateCircle(328, 213, 30, StaticType, 1.1f));
	Carnival.add(App->physics->CreateChain(0, 0, Sticks, 16, StaticType,0.0f));
	Carnival.add(App->physics->CreateChain(-45, 0, Sticks, 16, StaticType,0.0));
	Carnival.add(App->physics->CreateChain(-90, 0, Sticks, 16, StaticType,0.0f));
	Carnival.add(App->physics->CreateChain(0, 0, RBand,14, StaticType,1.8f));
	Carnival.add(App->physics->CreateChain(0, 0, LBand,14 , StaticType,1.8f));
	Carnival.add(App->physics->CreateChain(0, 0, Rarm, 14, StaticType,0.0f));
	Carnival.add(App->physics->CreateChain(0, 0, Larm, 14, StaticType,0.0f));
	Carnival.add(App->physics->CreateChain(0, 0, Rcorner, 24, StaticType,0.0f));
	Carnival.add(App->physics->CreateChain(0, 0, Lcorner, 26, StaticType,0.0f));


	

	return true;

}