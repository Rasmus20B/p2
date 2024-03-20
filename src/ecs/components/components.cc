module;

#include <type_traits>

export module ecs.components;

export import ecs.components.basic;
export import ecs.components.bullets;
export import ecs.components.script;

export namespace ecs {
  namespace component {
    enum class ComponentID {
      Transform2D,
      Velocity,
      Collider,
      BulletInfo,
      Health,
      Parent,
      Children,
      Script,
      Input,
      Sprite,
      Attractor,
      BulletManager,
      BulletPattern,
      Size,
    };

    template<typename T>
    consteval ComponentID get_component_id() {
      if constexpr(std::is_same_v<T, CTransform2D>) {
        return ComponentID::Transform2D;
      }
      else if constexpr(std::is_same_v<T, CVelocity>) {
        return ComponentID::Velocity;
      }
      else if constexpr(std::is_same_v<T, CCollider>) {
        return ComponentID::Collider;
      }
      else if constexpr(std::is_same_v<T, CBulletManager>) {
        return ComponentID::BulletInfo;
      }
      else if constexpr (std::is_same_v<T, CHealth>) {
        return ComponentID::Health;
      }
      else if constexpr (std::is_same_v<T, CInput>) {
        return ComponentID::Input;
      }
      else if constexpr (std::is_same_v<T, CSprite>) {
        return ComponentID::Sprite;
      }
      else if constexpr (std::is_same_v<T, CAttraction>) {
        return ComponentID::Attractor;
      }
      else if constexpr (std::is_same_v<T, CScript>) {
        return ComponentID::Script;
      }
      return ComponentID::Size;
    };

    template<ComponentID id>
    struct get_component_type_t;

    template<>
    struct get_component_type_t<ComponentID::Transform2D> {
      using type =  CTransform2D;
    };

    template<>
    struct get_component_type_t<ComponentID::Velocity> {
      using type =  CVelocity;
    };

    template<>
    struct get_component_type_t<ComponentID::Script> {
      using type = CScript;
    };

    template<>
    struct get_component_type_t<ComponentID::Health> {
      using type = CHealth;
    };

    template<>
    struct get_component_type_t<ComponentID::Collider> {
      using type =  CCollider;
    };

    template<ComponentID id>
    using get_component_type = get_component_type_t<id>::type;

  }
}
