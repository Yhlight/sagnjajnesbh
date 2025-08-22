#pragma once

#include <string>
#include <vector>
#include "Segment.h"

class CHTLUnifiedScanner {
public:
	std::vector<Segment> SliceSegments(const std::string& source) const;
};