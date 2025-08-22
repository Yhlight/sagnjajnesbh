#pragma once

#include <cstddef>
#include <string>
#include "SegmentType.h"

struct Segment {
	SegmentType type;
	std::string text;
	std::size_t startOffset;
	std::size_t endOffset;
	std::string originTypeName; // 对于OriginCustom存储@类型名

	Segment()
		: type(SegmentType::CHTL), startOffset(0), endOffset(0) {}

	Segment(SegmentType t, std::string s, std::size_t start, std::size_t end)
		: type(t), text(std::move(s)), startOffset(start), endOffset(end) {}
};