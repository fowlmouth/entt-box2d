#pragma once

namespace Physics
{

  template< typename Derived >
  struct RegistryMixin
  {
    inline Derived& derived() { return *static_cast< Derived* >(this); }

    void physics_init()
    {
      Derived& registry = derived();

      registry.template on_destroy< Fixture >().template connect< &destroy_fixture >();
      registry.template on_destroy< Body >().template connect< &destroy_body >();

      registry.template on_construct< World >().template connect< &construct_world >();
    }

  };

}
