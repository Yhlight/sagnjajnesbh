#include "dispatcher/code_fragment.h"

namespace chtl {

std::string CodeFragment::fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
        case FragmentType::LOCAL_STYLE: return "LOCAL_STYLE";
        case FragmentType::ORIGIN_HTML: return "ORIGIN_HTML";
        case FragmentType::ORIGIN_CSS: return "ORIGIN_CSS";
        case FragmentType::ORIGIN_JS: return "ORIGIN_JS";
        default: return "UNKNOWN";
    }
}

} // namespace chtl