#ifndef BRASSTACKS_ECS_ENTITY_REGISTRAR_HPP
#define BRASSTACKS_ECS_ENTITY_REGISTRAR_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"

#include <array>
#include <queue>

namespace btx {

class EntityRegistrar final {
public:
    static Entity create_entity();
    static void destroy_entity(Entity entity);
    static void set_signature(Entity entity, Signature sig);
    static Signature get_signature(Entity entity);

    static void init();

    ~EntityRegistrar() = default;

    EntityRegistrar(const EntityRegistrar &&) = delete;
    EntityRegistrar(EntityRegistrar &)        = delete;

    EntityRegistrar & operator=(const EntityRegistrar &&) = delete;
    EntityRegistrar & operator=(EntityRegistrar &)        = delete;

private:
	static std::queue<Entity> _available_ids;
	static std::array<Signature, MAX_ENTITIES> _signatures;
	static std::uint32_t _active_entities;

    EntityRegistrar() = default;
};

} // namespace btx

#endif // BRASSTACKS_ECS_ENTITY_REGISTRAR_HPP