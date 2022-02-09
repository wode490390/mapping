#pragma once

struct BlockPalette;
struct BiomeRegistry;

struct Level {
	BlockPalette *getGlobalBlockPalette() const;

	BlockPalette *getBlockPalette() const {
		return getGlobalBlockPalette();
	}
	BiomeRegistry *getBiomeRegistry() const;
};
