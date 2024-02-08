module;

#include <cstdint>
#include <functional>
#include <algorithm>
#include <vector>

import ecs.component;

export module archetype;

using archetype_id = std::uint32_t;

// export get_function(std::vector<ComponentID> ids) {
//
//   std::sort(ids.begin(), ids.end());
// }
