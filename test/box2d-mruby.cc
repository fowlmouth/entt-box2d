
#include "entt-mruby/entt-mruby.h"
#include "entt-box2d/entt-box2d.h"
#include <iostream>

bool b2vec2(mrb_state* mrb, mrb_value value, b2Vec2& vec)
{
  if(mrb_array_p(value))
  {
    if(ARY_LEN(mrb_ary_ptr(value)) != 2)
      return false;

    vec.x = mrb_to_flo(mrb, mrb_ary_entry(value, 0));
    vec.y = mrb_to_flo(mrb, mrb_ary_entry(value, 1));

    return true;
  }
  return false;
}

namespace MRuby
{
  bool read_hash(HashReader& reader, const char* symbol, b2Vec2& value)
  {
    mrb_value val = mrb_hash_get(
      reader.state,
      reader.self,
      mrb_symbol_value(mrb_intern_cstr(reader.state, symbol))
    );
    return b2vec2(reader.state, val, value);
  }

  bool read_hash(HashReader& reader, const char* symbol, entt::entity& entity)
  {
    mrb_value val = mrb_hash_get(
      reader.state,
      reader.self,
      mrb_symbol_value(mrb_intern_cstr(reader.state, symbol))
    );
    if(mrb_fixnum_p(val))
    {
      entity = static_cast< entt::entity >(mrb_fixnum(val));
      return true;
    }
    return false;
  }
}

template<>
struct MRuby::ComponentInterface< Physics::World >
: MRuby::DefaultComponentInterface< Physics::World >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto world = registry.try_get< Physics::World >(entity))
    {
      MRuby::HashBuilder hash(state);

      // auto& sf_sprite = sprite->sprite;
      return hash.self;
    }
    return mrb_nil_value();
  }

  static mrb_value set(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type, mrb_int argc, mrb_value* arg)
  {
    if(!argc || ! mrb_hash_p(arg[0]))
      return mrb_nil_value();

    b2Vec2 gravity;

    MRuby::HashReader reader(state, arg[0]);
    reader("gravity", gravity);

    auto& world = registry.assign_or_replace< Physics::World >(entity);
    world.world.SetGravity(gravity);

    return arg[0]; // mrb_nil_value();

  }
};



template<>
struct MRuby::ComponentInterface< Physics::Body >
: MRuby::DefaultComponentInterface< Physics::Body >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto world = registry.try_get< Physics::Body >(entity))
    {
      MRuby::HashBuilder hash(state);

      // auto& sf_sprite = sprite->sprite;
      return hash.self;
    }
    return mrb_nil_value();
  }

  static mrb_value set(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type, mrb_int argc, mrb_value* arg)
  {
    if(!argc || ! mrb_hash_p(arg[0]))
      return mrb_nil_value();

    b2BodyDef body_def;
    std::string body_type;
    entt::entity world_id;

    MRuby::HashReader reader(state, arg[0]);
    reader("world", world_id)("type", body_type)("position", body_def.position);

    if(!registry.valid(world_id) || !registry.has< Physics::World >(world_id))
      return mrb_nil_value();

    b2BodyType b2_type = b2_dynamicBody;
    if(body_type == "static")
      b2_type = b2_staticBody;
    else if(body_type == "kinematic")
      b2_type = b2_kinematicBody;

    body_def.type = b2_type;

    Physics::World& world = registry.get< Physics::World >(world_id);
    b2Body* b2_body = world.world.CreateBody(&body_def);

    auto& body = registry.assign_or_replace< Physics::Body >(entity, b2_body);

    return arg[0]; // mrb_nil_value();

  }
};



struct TestRegistry
: entt::registry,
  MRuby::RegistryMixin< TestRegistry >,
  Physics::RegistryMixin< TestRegistry >
{
  static const int max_static_components;
  int next_dynamic_component_id;

  mrb_state* mrb;

  TestRegistry()
  {
    physics_init();

    mrb = mrb_open();
    mrb_init< Physics::World, Physics::Body >(mrb);
  }

};

const int TestRegistry::max_static_components = 8;

int main()
{
  TestRegistry registry;

  mrb_load_string(registry.mrb, R"MRUBY(
    entity = $registry.create_entity
    entity.set 'Physics::World', { gravity: [0, 10] }
    @world_id = entity.id

    entity2 = $registry.create_entity
    entity2.set 'Physics::Body', { world: @world_id, type: 'dynamic', position: [0,0] }
    @entity2_id = entity2.id
  )MRUBY");

  // auto entity1 = registry.create();
  // auto& world = registry.assign< Physics::World >(entity1);
  // world.world.SetGravity(b2Vec2(0,10));
  auto entity1 = static_cast< entt::entity >(
    mrb_fixnum(
      mrb_iv_get(
        registry.mrb,
        mrb_top_self(registry.mrb),
        mrb_intern_lit(registry.mrb, "@world_id")
      )
    )
  );
  auto& world = registry.get< Physics::World >(entity1);

  // auto entity2 = registry.create();
  // auto& body = registry.assign< Physics::Body >(entity2);
  auto entity2 = static_cast< entt::entity >(
    mrb_fixnum(
      mrb_iv_get(
        registry.mrb,
        mrb_top_self(registry.mrb),
        mrb_intern_lit(registry.mrb, "@entity2_id")
      )
    )
  );
  auto& body = registry.get< Physics::Body >(entity2);
  {
    auto& fixture = registry.assign< Physics::Fixture >(entity2);

    // b2BodyDef body_def;
    // body_def.type = b2_dynamicBody;
    // body_def.position = b2Vec2(0,0);

    b2CircleShape circle_shape;
    circle_shape.m_radius = 1.0;

    b2FixtureDef fixture_def;
    fixture_def.shape = &circle_shape;

    // body.body = world.world.CreateBody(&body_def);
    fixture.fixture = body.body->CreateFixture(&fixture_def);

  }

  for(int i = 0; i < 10; ++i)
  {
    world.step(1);
    auto position = body.body->GetPosition();
    std::cout << "(" << position.x << ", " << position.y << ")" << std::endl;
  }

  return 0;
}
