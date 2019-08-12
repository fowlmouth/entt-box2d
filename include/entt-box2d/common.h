#pragma once

#ifdef FOWL_ENTT_MRUBY

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
  template<>
  bool convert<b2Vec2>(mrb_state* state, mrb_value input, b2Vec2& output)
  {
    return b2vec2(state, input, output);
  }

  template<>
  bool convert<entt::entity>(mrb_state* state, mrb_value input, entt::entity& output)
  {
    if(mrb_fixnum_p(input))
    {
      output = static_cast< entt::entity >(mrb_fixnum(input));
      return true;
    }
    return false;
  }
}

#endif