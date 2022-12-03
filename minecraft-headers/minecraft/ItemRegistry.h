#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "HashedString.h"
#include "Memory.h"

struct ItemRegistry{
	static std::vector<SharedPtr<Item>> mItemRegistry;

	struct ItemAlias{
		HashedString alias;
		char padding[120];
	};

	static std::unordered_map<HashedString, ItemAlias> mItemAliasLookupMap;
	static std::unordered_map<HashedString, std::function<HashedString(short)>> mComplexAliasLookupMap;

    static std::unordered_map<short, HashedString> mLegacyIDToNameMap;

	static std::pair<HashedString, int> getNameFromAlias(HashedString const & name, int metadata);

};
