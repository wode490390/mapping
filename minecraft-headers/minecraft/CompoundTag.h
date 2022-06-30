#pragma once

#include <string>
#include <memory>

#include "Tag.h"

class CompoundTag : public Tag{
public:
	char filler[48];

	CompoundTag(void);
	CompoundTag(const CompoundTag&);
	CompoundTag(CompoundTag &&);

	CompoundTag* putCompound(std::string name, std::unique_ptr<CompoundTag> tag);
	short& putShort(std::string name, short value);

	std::unique_ptr<CompoundTag> clone(void) const;

	virtual ~CompoundTag(void);
};
