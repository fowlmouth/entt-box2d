#pragma once

namespace Physics
{

struct Body
{
  b2Body* body = nullptr;
};

bool set_position(entt::registry& r, entt::entity entity, Body& body, b2Vec2 position)
{
  if(body.body)
  {
    body.body->SetTransform(position, body.body->GetAngle());
    return true;
  }
  return false;
}

bool set_position(entt::registry& r, entt::entity entity, b2Vec2 position)
{
  if(Body* body = r.try_get< Body >(entity))
    return set_position(r, entity, *body, position);
  return false;
}

bool get_position(entt::registry& r, entt::entity entity, b2Vec2& position)
{
  if(Body* body = r.try_get< Body >(entity))
  {
    position = body->body->GetPosition();
    return true;
  }
  return false;
}

bool get_transform(entt::registry& r, entt::entity entity, b2Transform& transform)
{
  if(Body* body = r.try_get< Body >(entity))
  {
    transform = body->body->GetTransform();
    return true;
  }
  return false;
}

bool set_linear_velocity(entt::registry& r, entt::entity entity, b2Vec2 velocity)
{
  if(auto body = r.try_get< Body >(entity))
  {
    body->body->SetLinearVelocity(velocity);
    return true;
  }
  return false;
}


void destroy_body(entt::registry& r, entt::entity id)
{
  Body& body = r.get< Body >(id);
  body.body->GetWorld()->DestroyBody( body.body );
  body.body = nullptr;
}


} // ::Physics


// namespace Data
// {

//   template<typename Manager>
//   struct Loader< json11::Json, Physics::Body, Manager >
//   {
//     static bool load(const json11::Json& json, Physics::Body& component, Manager& manager, entt::entity entity)
//     {
//       auto world = Relationships::search_parents< Physics::World >(manager, entity); // entity.search_parents< Components::PhysicsWorld >();
//       if(! world)
//         return false;

//       b2BodyDef body_def;
//       body_def.position = b2vec2(json["position"]);
//       body_def.angularDamping = json["angular-damping"].number_value();
//       body_def.linearDamping = json["linear-damping"].number_value();

//       const std::string& type = json["type"].string_value();
//       if(type == "static")
//         body_def.type = b2_staticBody;
//       else if(type == "kinematic")
//         body_def.type = b2_kinematicBody;
//       else
//         body_def.type = b2_dynamicBody;

//       b2Body* body = world->world.CreateBody(&body_def);
//       body->SetUserData(reinterpret_cast< void* >(entity));
      
//       component.body = body;
//       return true;

//     }
//   };

// } // ::Data
