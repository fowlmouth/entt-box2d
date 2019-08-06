#pragma once

namespace Physics
{

struct Fixture
{
	b2Fixture* fixture = nullptr;
};


bool set_sensor(entt::registry& r, entt::entity entity, bool is_sensor)
{
	if(auto fixture = r.try_get< Fixture >(entity))
	{
		if(auto b2fixture = fixture->fixture)
		{
			b2fixture->SetSensor(is_sensor);
			return true;
		}
	}
	return false;
}

void destroy_fixture(entt::registry& r, entt::entity id)
{
	Fixture& fixture = r.get< Fixture >(id);
	if(fixture.fixture)
		fixture.fixture->GetBody()->DestroyFixture(fixture.fixture);
	fixture.fixture = nullptr;
};


} // ::Physics


// namespace Data
// {

// 	template<typename Manager>
// 	struct Loader< json11::Json, Physics::Fixture, Manager >
// 	{
// 		static bool load(const json11::Json& json, Physics::Fixture& component, Manager& manager, entt::entity entity)
// 		{
// 			Physics::Body* body = Relationships::search_parents< Physics::Body >(manager, entity);
// 			if(!body || !body->body)
// 				return false;

// 			b2Body* b2body = body->body;
// 			b2Fixture* fixture = nullptr;

// 			b2FixtureDef fixture_def;
// 			fixture_def.density = json["density"].number_value();
// 			fixture_def.friction = json["friction"].number_value();
// 			fixture_def.isSensor = ! json["sensor"].is_null();

// 			b2Vec2 offset = b2vec2(json["offset"]);
// 			float scale = json["scale"].is_number()
// 				? json["scale"].number_value()
// 				: 1.f;

// 			const std::string& type = json["shape"].string_value();
// 			if(type == "circle")
// 			{
// 				b2CircleShape circle_shape;
// 				circle_shape.m_radius = json["radius"].number_value() * scale;
// 				circle_shape.m_p = offset;

// 				fixture_def.shape = &circle_shape;
// 				fixture = b2body->CreateFixture(&fixture_def);
// 			}
// 			else if(type == "poly")
// 			{
// 				b2PolygonShape polygon_shape;
// 				std::vector< b2Vec2 > vertices;
// 				const auto& points = json["points"].array_items();
// 				vertices.resize(points.size());
// 				for(int i = 0; i < vertices.size(); ++i)
// 					vertices[i] = scale * b2vec2(points[i]) + offset;
// 				polygon_shape.Set(&vertices[0], vertices.size());

// 				fixture_def.shape = &polygon_shape;
// 				fixture = b2body->CreateFixture(&fixture_def);
// 			}
// 			else if(type == "box")
// 			{
// 				float width = json["width"].number_value() * scale;
// 				float height = json["height"].number_value() * scale;
// 				float angle = json["angle"].number_value();

// 				b2PolygonShape box_shape;
// 				box_shape.SetAsBox(width, height, offset, angle);

// 				fixture_def.shape = &box_shape;
// 				fixture = b2body->CreateFixture(&fixture_def);
// 			}
// 			else if(type == "chain")
// 			{
// 				const auto& points = json["points"].array_items();
// 				std::vector< b2Vec2 > vertices;
// 				vertices.resize(points.size());
// 				for(int i = 0; i < vertices.size(); ++i)
// 					vertices[i] = scale * b2vec2(points[i]) + offset;

// 				b2ChainShape chain_shape;
// 				if(json["loop"].is_null())
// 					chain_shape.CreateChain(&vertices[0], vertices.size());
// 				else
// 					chain_shape.CreateLoop(&vertices[0], vertices.size());

// 				fixture_def.shape = &chain_shape;
// 				fixture = b2body->CreateFixture(&fixture_def);
// 				LogDebug("Chain shape: " << fixture);
// 			}

// 			if(!fixture)
// 				return false;
			
// 			fixture->SetUserData(reinterpret_cast< void* >( entity ));
// 			component.fixture = fixture;
// 			return true;
// 		}
// 	};

// } // ::Data
