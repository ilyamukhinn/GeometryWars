#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <unordered_map>
#include <type_traits>

template<typename Enum>
class EnumMask {
public:
    using Underlying = typename std::underlying_type<Enum>::type;

    EnumMask() : value(0) {}
    EnumMask(Enum e) : value(static_cast<Underlying>(e)) {}

    EnumMask& operator|=(EnumMask other) {
        value |= other.value;
        return *this;
    }

    EnumMask& operator&=(EnumMask other) {
        value &= other.value;
        return *this;
    }

    EnumMask& operator^=(EnumMask other) {
        value ^= other.value;
        return *this;
    }

    EnumMask operator|(EnumMask other) const {
        return EnumMask(static_cast<Enum>(value | other.value));
    }

    EnumMask operator&(EnumMask other) const {
        return EnumMask(static_cast<Enum>(value & other.value));
    }

    EnumMask operator^(EnumMask other) const {
        return EnumMask(static_cast<Enum>(value ^ other.value));
    }

    EnumMask operator~() const {
        return EnumMask(static_cast<Enum>(~value));
    }

    bool has(EnumMask mask) const {
        return (value & mask.value) == mask.value;
    }

private:
    Underlying value;
};

enum class EntityTag : uint32_t {
    UNDEFINED = 0,
    PLAYER = 1 << 0,
    BULLET = 1 << 1,
    ENEMY = 1 << 2,
    SMALL_ENEMY = 1 << 3
};

static std::unordered_map<EntityTag, std::string> const entityTagToStr = {
    { EntityTag::UNDEFINED, "Undefined" },
    { EntityTag::PLAYER, "Player" },
    { EntityTag::BULLET, "Bullet" },
    { EntityTag::ENEMY, "Enemy" },
    { EntityTag::SMALL_ENEMY, "Small enemy" }
};

using EntityTagMask = EnumMask<EntityTag>;

#endif // !ENUMS_H