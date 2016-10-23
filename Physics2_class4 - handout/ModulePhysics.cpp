#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// big static circle as "ground" in the middle of the screen
	
/*
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* big_ball = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	big_ball->CreateFixture(&fixture);
	*/
	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}



void ModulePhysics::CreateRevolutionJoint(PhysBody* bodya, PhysBody* bodyb, PhysBody* bodyc, PhysBody* bodyd)

{
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = bodya->body;
	revoluteJointDef.bodyB = bodyb->body;
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0.09f,0.16);
	revoluteJointDef.localAnchorB.Set(-0.1f, 0.05f);
	revoluteJointDef.referenceAngle = 0;
	revoluteJointDef.enableLimit = true;
	revoluteJointDef.lowerAngle = 20* DEGTORAD ;
	revoluteJointDef.upperAngle  = 80 * DEGTORAD;
	(b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);

	b2RevoluteJointDef revoluteJointDefB;
	revoluteJointDefB.bodyA = bodyc->body;
	revoluteJointDefB.bodyB = bodyd->body;
	revoluteJointDefB.collideConnected = false;

	revoluteJointDefB.localAnchorA.Set(0.9f,0.2f);
	revoluteJointDefB.localAnchorB.Set(0.05f, 0);
	revoluteJointDefB.referenceAngle = 0;
	revoluteJointDefB.enableLimit = true;
	revoluteJointDefB.lowerAngle = 20 * DEGTORAD;
	revoluteJointDefB.upperAngle = 160* DEGTORAD;
	(b2RevoluteJoint*)world->CreateJoint(&revoluteJointDefB);
}


void ModulePhysics::CreateDistanceJoint(PhysBody* bodya, PhysBody* bodyb)

{
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.bodyA = bodya->body;
	prismaticJointDef.bodyB = bodyb->body;
	prismaticJointDef.collideConnected = true;


	prismaticJointDef.localAnchorA.Set(0, 0);
	prismaticJointDef.localAnchorB.Set(0, -1);
	prismaticJointDef.localAxisA.Set(0, -1);
	prismaticJointDef.enableLimit = true;
	prismaticJointDef.lowerTranslation = -0.02;
	prismaticJointDef.upperTranslation = 1.0;
	(b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);

}



PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius,b2BodyType type , float Rest)
{
	b2BodyDef body;
	if(type == b2_dynamicBody)
	{
		body.type = b2_dynamicBody;
	}
	else
	{
		body.type = b2_staticBody;
	}
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = Rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool dynamic)
{
	b2BodyDef body;
	if (dynamic == true) {
		body.type = b2_dynamicBody;
	}
	else {
		body.type = b2_staticBody;
	}
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);
	
	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;

	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRacket(int x, int y, int width, int height, bool side)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	
	b2Vec2 vertices[8];
	if (side) {
		
		vertices[0].Set(0.0625f,0.1000f);
		vertices[1].Set(0.0208f,0.266f);
		vertices[2].Set(0.16666f,0.466f);
		vertices[3].Set(0.6958f,0.933f);
		vertices[4].Set(0.779167f,0.9333f);
		vertices[5].Set(0.737500f,0.8333f);
		vertices[6].Set(0.270833f,0.10000f);
		vertices[7].Set(00.1458f,0.06667f);
		
	}
	else {

		vertices[0].Set(0.9375f,0.3333f);
		vertices[1].Set(0.10416f, 0.933f);
		vertices[2].Set(0.020833f,0.9000f);
		vertices[3].Set(0.1041f,0.7666f);
		vertices[4].Set(0.7916f,0.066f);
		vertices[5].Set(0.875f,0.066f);
		vertices[6].Set(0.9375f,0.1333f);
		vertices[7].Set(0.9583f,0.233f);



	}
	int count = 8;
	b2Body* b = world->CreateBody(&body);
	b2PolygonShape racket;
	racket.Set(vertices, count);

	b2FixtureDef fixture;
	fixture.shape = &racket;
	fixture.density = 1.0f;
	fixture.restitution = 0.2f;
	b->CreateFixture(&fixture);
	PhysBody* pbody = new PhysBody();
	pbody->body = b;

	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}



	



PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, bool rest)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	
		body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	
	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	if(rest)
	fixture.restitution = 1.5f;
	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}



// 
update_status ModulePhysics::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}
			b2Vec2 point(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));

			// TODO 1: If mouse button 1 is pressed ...
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				// test if the current body contains mouse position
				bool hit;
				b2Transform transform = b->GetTransform();
				b2Shape* shape = f->GetShape();
				hit = shape->TestPoint(transform, point);


				if (hit == true)
				{
					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = b;
					def.target = point;
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * b->GetMass();
					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
				}
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT  && mouse_joint)
			{
				mouse_joint->SetTarget(point);
				App->renderer->DrawLine(METERS_TO_PIXELS(point.x), METERS_TO_PIXELS(point.y), METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 255, 255);
			}
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP  && mouse_joint)
			{
				world->DestroyJoint(mouse_joint);
				mouse_joint = nullptr;
			}
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}