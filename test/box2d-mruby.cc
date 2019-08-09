
#include "entt-mruby/entt-mruby.h"
#include "entt-box2d/entt-box2d.h"
#include <iostream>





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


template<>
struct MRuby::ComponentInterface< Physics::Fixture >
: MRuby::DefaultComponentInterface< Physics::Fixture >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto world = registry.try_get< Physics::Fixture >(entity))
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

    b2FixtureDef fixture_def;
    std::string shape_type;
    entt::entity body_id;

    MRuby::HashReader reader(state, arg[0]);
    reader("body", body_id)("shape", shape_type); //("position", body_def.position);

    if(!registry.valid(body_id) || !registry.has< Physics::Body >(body_id))
      return mrb_nil_value();

    b2Body* body = registry.get< Physics::Body >(body_id).body;

    b2Fixture* fixture = nullptr;
    if(shape_type == "circle")
    {
      b2CircleShape circle_shape;
      reader("radius", circle_shape.m_radius);
      fixture_def.shape = &circle_shape;
      fixture = body->CreateFixture(&fixture_def);
    }

    if(fixture)
    {
      registry.assign_or_replace< Physics::Fixture >(entity, fixture);
      return arg[0];
    }

    return mrb_nil_value();

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
    mrb_init< Physics::World, Physics::Body, Physics::Fixture >(mrb);
  }

};

const int TestRegistry::max_static_components = 8;

int main()
{
  TestRegistry registry;

  mrb_load_string(registry.mrb, R"MRUBY(
    world = $registry.create_entity
    world.set 'Physics::World', { gravity: [0, 10] }
    @world_id = world.id

    entity2 = $registry.create_entity
    entity2.set 'Physics::Body', { world: world.id, type: 'dynamic', position: [0,0] }
    entity2.set 'Physics::Fixture', { body: entity2.id, shape: 'circle', radius: 1.0 }
    @entity2_id = entity2.id
  )MRUBY");

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

  for(int i = 0; i < 10; ++i)
  {
    world.step(1);
    auto position = body.body->GetPosition();
    std::cout << "(" << position.x << ", " << position.y << ")" << std::endl;
  }

  return 0;
}
