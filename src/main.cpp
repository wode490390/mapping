#include <minecraft/BinaryStream.h>
#include <minecraft/Block.h>
#include <minecraft/BlockActor.h>
#include <minecraft/BlockLegacy.h>
#include <minecraft/BlockPalette.h>
#include <minecraft/BlockSerializationUtils.h>
#include <minecraft/BlockTypeRegistry.h>
#include <minecraft/CompoundTag.h>
#include <minecraft/Enchant.h>
#include <minecraft/Item.h>
#include <minecraft/ItemDescriptor.h>
#include <minecraft/ItemRegistry.h>
#include <minecraft/Level.h>
#include <minecraft/LevelSoundEventMap.h>
#include <minecraft/Memory.h>
#include <minecraft/Minecraft.h>
#include <minecraft/MobEffect.h>
#include <minecraft/NoteBlock.h>
#include <minecraft/ParticleTypeMap.h>
#include <minecraft/Potion.h>
#include <minecraft/PrintStream.h>
#include <minecraft/ServerInstance.h>
#include <minecraft/VanillaBlockConversion.h>
#include <minecraft/Biome.h>
#include <minecraft/BiomeRegistry.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <json.hpp>

struct FilePrintStream : public PrintStream {

	std::ofstream &mOutput;

	FilePrintStream(std::ofstream &output) : mOutput(output) {}
	virtual void print(std::string const& string) const {
		mOutput << string;
	}
};

void generate_r12_to_current_block_map(ServerInstance *serverInstance) {
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();

	std::filesystem::path inputPath{"input_files/r12_block_states.json"};
	if (!std::filesystem::exists(inputPath)) {
		std::cerr << "Input file " << inputPath << " not found, r12_to_current_block_map won't be generated!" << std::endl;
		return;
	}
	std::ifstream input(inputPath);

	auto stream = new BinaryStream();

	auto json = nlohmann::json::object();
	input >> json;

	for (auto &object : json["minecraft"].items()) {
		const auto &name = "minecraft:" + object.key();

		for (auto &it : object.value()) {
			auto state = it.get<unsigned short>();
			if (name == "minecraft:cocoa" && state >= 12) {
				continue;
			}

			auto block = BlockTypeRegistry::lookupByName(name, state, false);
			if (block == nullptr){
				std::cerr << "No matching blockstate found for " << name << " (THIS IS A BUG)" << std::endl;
				continue;
			}

			stream->writeUnsignedVarInt(name.length());
			stream->write(name.c_str(), name.length());
			stream->writeUnsignedShort(state);
			stream->writeType(block->tag);
		}
	}

	std::ofstream output("mapping_files/r12_to_current_block_map.bin");
	output << stream->buffer;
	output.close();
	delete stream;
	std::cout << "Generated R12 block state mapping table" << std::endl;
}

static std::string slurp(std::ifstream& in) {
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

static void generate_old_to_current_palette_map_single(BlockPalette* palette, std::filesystem::directory_entry file) {
	std::ifstream infile(file.path());
	auto versionName = file.path().stem().string();
	std::ofstream mapping_file("mapping_files/old_palette_mappings/" + versionName + "_to_current_block_map.bin");

	auto input = new ReadOnlyBinaryStream(slurp(infile));
	auto output = new BinaryStream();

	auto length = input->buffer.length();

	BlockSerializationUtils::clearNBTToBlockCache();
	while(input->offset < length){
		CompoundTag state = input->getType<CompoundTag>();

		const Block* block = palette->getBlock(state);

		//TODO: compare and don't write if the states are the same
		//right now it's easier to do this outside of the mod
		output->writeType(state);
		output->writeType(block->tag);
	}

	mapping_file << output->buffer;
	mapping_file.close();
	delete input;
	delete output;

	std::cout << "Generated mapping table for " << versionName << std::endl;
}
static void generate_old_to_current_palette_map(ServerInstance *serverInstance) {
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();

	unsigned int generated = 0;

	std::filesystem::path oldBlockPalettesPath{"input_files/old_block_palettes"};
	if (!std::filesystem::exists(oldBlockPalettesPath)) {
		std::cerr << "Input path " << oldBlockPalettesPath << " does not exist, no block maps will be generated!" << std::endl;
		return;
	}

	std::filesystem::create_directory("mapping_files/old_palette_mappings");

	for(auto const& file : std::filesystem::directory_iterator{oldBlockPalettesPath}){
		if (file.path().extension().string() != ".nbt") {
			continue;
		}
		generate_old_to_current_palette_map_single(palette, file);
		generated++;
	}

	std::cout << "Generated " << std::to_string(generated) << " block state mapping tables" << std::endl;
}

void generate_palette(ServerInstance *serverInstance) {
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();
	unsigned int numStates = palette->getNumBlockNetworkIds();
	std::cout << "Number of blockstates: " << numStates << std::endl;

	auto paletteStream = new BinaryStream();
	for (unsigned int i = 0; i < numStates; i++) {
		auto state = palette->getBlock(i);
		paletteStream->writeType(state->tag);
	}

	std::ofstream paletteOutput("mapping_files/canonical_block_states.nbt");
	paletteOutput << paletteStream->buffer;
	paletteOutput.close();
	delete paletteStream;
	std::cout << "Generated block palette" << std::endl;
}

static void generate_blockstate_meta_mapping(ServerInstance *serverInstance) {
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();
	unsigned int numStates = palette->getNumBlockNetworkIds();

	auto metaArray = nlohmann::json::array();

	for(auto i = 0; i < numStates; i++){
		auto state = palette->getBlock(i);
		metaArray[i] = state->data;
	}

	std::ofstream out("mapping_files/block_state_meta_map.json");
	out << std::setw(4) << metaArray << std::endl;
	out.close();
	std::cout << "Generated blockstate <-> meta mapping table" << std::endl;
}


static void generate_block_properties_table(ServerInstance *serverInstance) {
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();
	unsigned int numStates = palette->getNumBlockNetworkIds();

	auto table = nlohmann::json::object();

	BlockTypeRegistry::forEachBlock([&table] (const BlockLegacy & blockLegacy)->bool {
		auto name = blockLegacy.getFullName();
		auto data = nlohmann::json::object();
		data["hardness"] = blockLegacy.getDestroySpeed();
		data["mHardness"] = blockLegacy.hardness;
		data["blastResistance"] = blockLegacy.blastResistance;
		data["friction"] = blockLegacy.friction;
		data["flammability"] = blockLegacy.flammability;
//		data["flameEncouragement"] = blockLegacy.flameEncouragement;
//		data["opacity"] = blockLegacy.opacity;
//		data["brightness"] = blockLegacy.brightness;
		table[name] = data;
		return true;
	});

	std::ofstream output("mapping_files/block_properties_table.json");
	output << std::setw(4) << table << std::endl;
	output.close();
	std::cout << "Generated hardness table" << std::endl;
}

void generate_biome_mapping(ServerInstance *server) {
	auto registry = server->getMinecraft()->getLevel()->getBiomeRegistry();

	auto map = nlohmann::json::object();

	registry->forEachBiome([&map] (Biome &biome) {
		auto id = biome.biomeId;
		map[biome.name.str] = id;
	});

	std::ofstream result("mapping_files/biome_id_map.json");
	result << std::setw(4) << map << std::endl;
	result.close();
	std::cout << "Generated biome ID mapping table" << std::endl;
}

static void generate_level_sound_mapping() {
	auto map = nlohmann::json::object();

	bool found_113 = false;
	for(int i = 0; i < 1000; i++) {
		auto name = LevelSoundEventMap::getName((LevelSoundEvent) i);
		if (name != "undefined") {
			map[name] = i;
			if (i == 113) {
				found_113 = true;
				std::cout << "WARNING: STOP_RECORD(113) has changed, and it won't appear in the output!" << std::endl;
			}
		}
	}
	if (!found_113) {
		map["stop_record"] = 113;
	}

	std::ofstream result("mapping_files/level_sound_id_map.json");
	result << std::setw(4) << map << std::endl;
	result.close();
	std::cout << "Generated LevelSoundEvent mapping table" << std::endl;
}

static void generate_particle_mapping() {
	auto map = nlohmann::json::object();

	auto list = ParticleTypeMap::getParticleNameTypeList();
	for(auto pair : list) {
		map[pair.first] = (unsigned int) pair.second;
	}

	std::ofstream result("mapping_files/particle_id_map.json");
	result << std::setw(4) << map << std::endl;
	result.close();
	std::cout << "Generated Particle mapping table" << std::endl;
}

static std::string add_prefix_if_necessary(std::string input) {
	if (input.rfind("minecraft:", 0) == 0) {
		return input;
	}

	return "minecraft:" + input;
}

static void generate_item_alias_mapping(ServerInstance *serverInstance) {
	auto simple = nlohmann::json::object();

	auto itemRegistry = serverInstance->getMinecraft()->getLevel()->getItemRegistry().mWeakRegistry.lock();
	assert(itemRegistry != nullptr);
	for(auto pair : itemRegistry->mItemAliasLookupMap) {
		auto prefixed = add_prefix_if_necessary(pair.second.alias.str);
		if (prefixed != pair.first.str) {
			simple[pair.first.str] = prefixed;
		}
	}

	auto complex = nlohmann::json::object();

	for(auto pair : itemRegistry->mComplexAliasLookupMap) {
		auto metaMap = nlohmann::json::object();

		auto func = pair.second.mCallback;

		auto zero = func(0).str;
		for(short i = 0; i < 32767; i++){
			auto iStr = func(i).str;
			if (iStr != "" && (i == 0 || iStr != zero)) {
				auto prefixed = add_prefix_if_necessary(iStr);
				if (prefixed != pair.first.str) {
					metaMap[std::to_string(i)] = prefixed;
				}
			}
		}
		complex[pair.first.str] = metaMap;
	}

	auto map = nlohmann::json::object();
	map["simple"] = simple;
	map["complex"] = complex;

	std::ofstream result("mapping_files/r16_to_current_item_map.json");
	result << std::setw(4) << map << std::endl;
	result.close();

	std::cout << "Generated legacy item alias mapping table" << std::endl;
}

static void generate_block_id_to_item_id_map(ServerInstance *serverInstance) {
	auto map = nlohmann::json::object();
	auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();

	auto oldItemRegistryRef = ItemRegistryManager::getItemRegistry();
	auto itemRegistryRef = serverInstance->getMinecraft()->getLevel()->getItemRegistry();

	ItemRegistryManager::setItemRegistry(itemRegistryRef);

	unsigned int numStates = palette->getNumBlockNetworkIds();

	for (unsigned int i = 0; i < numStates; i++) {
		auto state = palette->getBlock(i);
		auto descriptor = new ItemDescriptor(*state);

		const Item *item = descriptor->getItem();
		if (descriptor->getBlock() == nullptr) {
			//Filter out blocks which save as non-blockitems, like doors, signs, etc.
			//we should never be encoding these as blockitems
			//TODO: this doesn't filter older stuff like minecraft:item.spruce_door, which should also be skipped
			delete descriptor;
			continue;
		}
		delete descriptor;
		if (item == nullptr) {
			std::cout << "null item ??? " << state->getLegacyBlock().getFullName() << std::endl;
			continue;
		}
		std::string blockName = state->getLegacyBlock().getFullName();
		std::string itemName = item->getFullItemName();
		map[blockName] = itemName;
	}

	std::ofstream result("mapping_files/block_id_to_item_id_map.json");
	result << std::setw(4) << map << std::endl;
	result.close();

	ItemRegistryManager::resetItemRegistry();
	ItemRegistryManager::setItemRegistry(oldItemRegistryRef);
	std::cout << "Generated BlockID to ItemID mapping table" << std::endl;
}

static void generate_command_arg_types_table(ServerInstance *serverInstance) {
	auto map = nlohmann::json::object();

	auto registry = serverInstance->getMinecraft()->getCommands().getRegistry();

	for (int i = 0; i < 1000; i++) { //TODO: we can probably use CommandRegistry::forEachSymbol() for this instead
		int symbol = i | 0x100000;

		if (registry.isValid(symbol)) {
			auto name = registry.symbolToString(symbol);
			auto description = registry.describe(symbol);

			auto object = nlohmann::json::object();
			object["id"] = i;
			object["description"] = description;

			map[name] = object;
		}
	}

	std::ofstream result("mapping_files/command_arg_types.json");
	result << std::setw(4) << map << std::endl;
	result.close();

	std::cout << "Generated command parameter ID mapping table" << std::endl;
}

static void generate_item_tags(ServerInstance *serverInstance) {
	std::map<std::string, std::set<std::string>> tags;
	auto itemRegistry = serverInstance->getMinecraft()->getLevel()->getItemRegistry().mWeakRegistry.lock();

	for (const auto &pair: itemRegistry->mTagToItemsMap) {
		std::set<std::string> items;
		for (const auto &item: pair.second) {
			items.insert(item->getFullItemName());
		}
		tags[pair.first.str] = items;
	}

	nlohmann::json json = tags;
	std::ofstream result("mapping_files/item_tags.json");
	result << std::setw(4) << json << std::endl;
	result.close();

	std::cout << "Generated item tags!" << std::endl;
}

static void generate_note_instruments(ServerInstance *serverInstance) {
	std::map<std::string, unsigned int> instruments;
	for (unsigned int i = 0; i < 256; i++) { //assume this is OK?
		std::string name = NoteBlock::getSoundName(i);
		if (instruments.count(name) > 0) {
			continue;
		}
		instruments[name] = i;
	}
	nlohmann::json json = instruments;
	std::ofstream result("mapping_files/note_block_instruments.json");
	result << std::setw(4) << json << std::endl;
	result.close();

	std::cout << "Generated note instrument list" << std::endl;
}

static void generate_item_legacy_id_to_name_mapping(ServerInstance *serverInstance) {
    auto map = nlohmann::json::object();

    auto itemRegistry = serverInstance->getMinecraft()->getLevel()->getItemRegistry().mWeakRegistry.lock();
    for (auto& pair : itemRegistry->mLegacyIDToNameMap) {
        map[std::to_string(pair.first)] = add_prefix_if_necessary(pair.second.str);
    }

    std::ofstream result("mapping_files/item_legacy_id_to_name.json");
    result << std::setw(4) << map << std::endl;
    result.close();

    std::cout << "Generated item legacy id to name mapping table" << std::endl;
}

static void generate_item_runtime_id_to_name_mapping(ServerInstance *serverInstance) {
    auto map = nlohmann::json::object();
    auto map2 = nlohmann::json::object();

    auto itemRegistry = serverInstance->getMinecraft()->getLevel()->getItemRegistry().mWeakRegistry.lock();
    for (auto& pair : itemRegistry->mIdToItemMap) {
        map[std::to_string(pair.first)] = add_prefix_if_necessary(pair.second->getFullItemName());
        map2[add_prefix_if_necessary(pair.second->getFullItemName())] = pair.first;
    }

    std::ofstream result("mapping_files/item_runtime_id_to_name.json");
    result << std::setw(4) << map << std::endl;
    result.close();

    std::ofstream result2("mapping_files/item_name_to_runtime_id.json");
    result2 << std::setw(4) << map2 << std::endl;
    result2.close();

    std::cout << "Generated item runtime id to name mapping table" << std::endl;
}

static void generate_item_properties_table(ServerInstance *serverInstance) {
    auto table = nlohmann::json::array();

    unsigned int i = 0;
    auto itemRegistry = serverInstance->getMinecraft()->getLevel()->getItemRegistry().mWeakRegistry.lock();
    for (auto& item : itemRegistry->mItemRegistry) {
        auto data = nlohmann::json::object();

        data["identifier"] = item->getFullItemName();
        data["canBeDepleted"] = item->canBeDepleted();
        data["isDamageable"] = item->isDamageable();
        data["isExplodable"] = item->isExplodable();
        data["isFireResistant"] = item->isFireResistant();
        data["isHandEquipped"] = item->isHandEquipped();
        data["isSeed"] = item->isSeed();
        data["isStackedByData"] = item->isStackedByData();
        data["cooldownTime"] = item->getCooldownTime();
        data["runtimeId"] = item->getId();
        data["maxDamage"] = item->getMaxDamage();
        data["creativeCategory"] = item->getCreativeCategory();

        table[i++] = data;
    }

    std::ofstream result("mapping_files/item_properties_table.json");
    result << std::setw(4) << table << std::endl;
    result.close();

    std::cout << "Generated item properties table" << std::endl;
}

static void generate_block_properties_table2(ServerInstance *serverInstance) {
    auto palette = serverInstance->getMinecraft()->getLevel()->getBlockPalette();
    unsigned int numStates = palette->getNumBlockNetworkIds();

    auto table = nlohmann::json::array();

    unsigned int i = 0;
    BlockTypeRegistry::forEachBlock([&table, &i] (const BlockLegacy & blockLegacy)->bool {
        auto data = nlohmann::json::object();

        data["identifier"] = blockLegacy.getFullName();
        data["hardness"] = blockLegacy.getDestroySpeed();
        data["_mDestroySpeed"] = blockLegacy.hardness;
        data["_mExplosionResistance"] = blockLegacy.blastResistance;
        data["_mFriction"] = blockLegacy.friction;
        data["_mFlameOdds"] = blockLegacy.flammability;
//        data["_flameEncouragement"] = blockLegacy.flameEncouragement;
        data["_isLavaFlammable"] = blockLegacy.lavaFlammable;
//        data["_opacity"] = blockLegacy.opacity;
//        data["_brightness"] = blockLegacy.brightness;

        data["canBeOriginalSurface"] = blockLegacy.canBeOriginalSurface();
        data["canContainLiquid"] = blockLegacy.canContainLiquid();
        data["canHurtAndBreakItem"] = blockLegacy.canHurtAndBreakItem();
        data["canInstatick"] = blockLegacy.canInstatick();
        data["isLavaFlammable"] = blockLegacy.isLavaFlammable();
        data["isMotionBlockingBlock"] = blockLegacy.isMotionBlockingBlock();
        data["isSolid"] = blockLegacy.isSolid();
        data["isSolidBlockingBlock"] = blockLegacy.isSolidBlockingBlock();
        data["isVanilla"] = blockLegacy.isVanilla();
        data["isWaterBlocking"] = blockLegacy.isWaterBlocking();
        data["hasBlockEntity"] = blockLegacy.hasBlockEntity();
        data["shouldRandomTick"] = blockLegacy.shouldRandomTick();
        data["creativeCategory"] = blockLegacy.getCreativeCategory();
        data["blockEntityType"] = blockLegacy.getBlockEntityType();
        data["blockItemId"] = blockLegacy.getBlockItemId();
        data["burnOdds"] = blockLegacy.getBurnOdds();
        data["flameOdds"] = blockLegacy.getFlameOdds();
        data["renderLayer"] = blockLegacy.getRenderLayer();
        data["thickness"] = blockLegacy.getThickness();
        data["explosionResistance"] = blockLegacy.getExplosionResistance();

        data["_unknown560_288"] = blockLegacy.unknown560_288;
        data["_isWater"] = blockLegacy.isWater;
        data["_isPumpkin"] = blockLegacy.isPumpkin;
        data["_notLava"] = blockLegacy.notLava;
        data["_unused560_292"] = blockLegacy.unused560_292;
        data["_mBurnOdds"] = blockLegacy.mBurnOdds;
        data["_mLightBlock"] = blockLegacy.mLightBlock;
        data["_mLightEmission"] = blockLegacy.mLightEmission;
        data["_mMapColor"] = blockLegacy.mMapColor.toHexString();
        data["_mMapColorARGB"] = blockLegacy.mMapColor.toARGB();
        data["_mMapColor0-R"] = blockLegacy.mMapColor.r;
        data["_mMapColor1-G"] = blockLegacy.mMapColor.g;
        data["_mMapColor2-B"] = blockLegacy.mMapColor.b;
        data["_mMapColor3-A"] = blockLegacy.mMapColor.a;
        data["_unknown560_336"] = blockLegacy.unknown560_336;
        data["_isUnknownBlock"] = blockLegacy.isUnknownBlock;
        data["_id"] = blockLegacy.id;
        data["_mMinRequiredBaseGameVersion"] = blockLegacy.mMinRequiredBaseGameVersion.mFullVersionString;
        data["_mIsVanilla"] = blockLegacy.mIsVanilla;

        auto renderBlock = blockLegacy.getRenderBlock();

        auto material = renderBlock->getMaterial();
        data["MaterialType"] = material->mType;

        table[i++] = data;
        return true;
    });

    std::ofstream output("mapping_files/block_properties_table2.json");
    output << std::setw(4) << table << std::endl;
    output.close();
    std::cout << "Generated block properties table II" << std::endl;
}

static void generate_block_materials(ServerInstance *serverInstance) {
    auto table = nlohmann::json::array();

    unsigned int i = 0;
    for (auto& material : Material::mMaterials) {
        auto data = nlohmann::json::object();

        data["MaterialType"] = material->mType;
        data["_mNeverBuildable"] = material->mNeverBuildable;
        data["_mAlwaysDestroyable"] = material->mAlwaysDestroyable;
        data["_mLiquid"] = material->mLiquid;
        data["_mTranslucency"] = material->mTranslucency;
        data["_mBlocksMotion"] = material->mBlocksMotion;
        data["_mBlocksPrecipitation"] = material->mBlocksPrecipitation;
        data["_mSolid"] = material->mSolid;
        data["_mSuperHot"] = material->mSuperHot;
        data["isSolidBlocking"] = material->isSolidBlocking();
        data["isSuperHot"] = material->isSuperHot();
        data["isTopSolid(false,false)"] = material->isTopSolid(false, false);

        table[i++] = data;
    }

    std::ofstream output("mapping_files/block_materials_table.json");
    output << std::setw(4) << table << std::endl;
    output.close();
    std::cout << "Generated block materials table" << std::endl;
}

static void generate_mob_effects(ServerInstance *serverInstance) {
    auto list = nlohmann::json::array();

    for (unsigned int id = 0; id < 128; id++) { //assume this is OK?
        auto effect = MobEffect::getById(id);
        if (effect == nullptr) {
            continue;
        }

        auto data = nlohmann::json::object();
        data["mId"] = effect->mId;
        data["mIsHarmful"] = effect->mIsHarmful;
        data["mColor"] = effect->mColor.toHexString();
        data["mColorARGB"] = effect->mColor.toARGB();
        data["mColor0-R"] = effect->mColor.r;
        data["mColor1-G"] = effect->mColor.g;
        data["mColor2-B"] = effect->mColor.b;
        data["mColor3-A"] = effect->mColor.a;
        data["mDescriptionId"] = effect->mDescriptionId;
        data["mIcon"] = effect->mIcon;
        data["mDurationModifier"] = effect->mDurationModifier;
        data["mIsDisabled"] = effect->mIsDisabled;
        data["mResourceName"] = effect->mResourceName;
        data["mIconName"] = effect->mIconName;
        data["mEffectVisible"] = effect->mEffectVisible;
        data["mComponentName"] = effect->mComponentName.str;
        list[id] = data;
    }

    std::ofstream output("mapping_files/mob_effects.json");
    output << std::setw(4) << list << std::endl;
    output.close();
    std::cout << "Generated mob effect list" << std::endl;
}

static void generate_potions(ServerInstance *serverInstance) {
    auto list = nlohmann::json::array();

    unsigned int count = Potion::getPotionCount();
    for (int id = 0; id < count; id++) {
        auto potion = Potion::getPotion(id);

        auto data = nlohmann::json::object();
        data["mId"] = potion->mId;
        data["mNameId"] = potion->mNameId;
        data["mPrefix"] = potion->mPrefix;
        data["mVar"] = potion->mVar;
        data["descriptionId"] = potion->getDescriptionId();
        data["mobEffectId"] = potion->getMobEffectId();

        auto descriptionIds = nlohmann::json::array();
        unsigned int k = 0;
        for (auto &descriptionId: potion->mDescriptionIds) {
            descriptionIds[k++] = descriptionId;
        }
        data["mDescriptionIds"] = descriptionIds;

        list[id] = data;
    }

    std::ofstream output("mapping_files/potions.json");
    output << std::setw(4) << list << std::endl;
    output.close();
    std::cout << "Generated potion list" << std::endl;
}

static void generate_enchants(ServerInstance *serverInstance) {
    auto table = nlohmann::json::array();

    unsigned int i = 0;
    for (auto& enchant : Enchant::mEnchants) {
        auto data = nlohmann::json::object();

        data["Type"] = enchant->mEnchantType;
        data["Frequency"] = enchant->mFrequency;
        data["mIsLootable"] = enchant->mIsLootable;
        data["mPrimarySlots"] = enchant->mPrimarySlots;
        data["mSecondarySlots"] = enchant->mSecondarySlots;
        data["mCompatibility"] = enchant->mCompatibility;
        data["mDescription"] = enchant->mDescription;
        data["mStringId"] = enchant->mStringId.str;
        data["mIsDisabled"] = enchant->mIsDisabled;
        data["isDisabled"] = enchant->isDisabled();
        data["isAvailable"] = enchant->isAvailable();

        table[i++] = data;
    }

    std::ofstream output("mapping_files/enchants.json");
    output << std::setw(4) << table << std::endl;
    output.close();
    std::cout << "Generated enchant list" << std::endl;
}

static void generate_block_actor_id_map(ServerInstance *serverInstance) {
    auto map = nlohmann::json::object();

    for (auto& pair : BlockActor::mClassIdMap) {
        map[std::to_string(pair.first)] = pair.second;
    }

    std::ofstream result("mapping_files/block_actor_id_map.json");
    result << std::setw(4) << map << std::endl;
    result.close();

    std::cout << "Generated block actor id table" << std::endl;
}

#define GENERATE_ALL_DATA 1

extern "C" void modloader_on_server_start(ServerInstance *serverInstance) {
	std::filesystem::create_directory("mapping_files");

#if GENERATE_ALL_DATA
	generate_r12_to_current_block_map(serverInstance);
	generate_palette(serverInstance);
	generate_blockstate_meta_mapping(serverInstance);
	generate_biome_mapping(serverInstance);
	generate_level_sound_mapping();
	generate_particle_mapping();
	generate_block_properties_table(serverInstance);

	generate_old_to_current_palette_map(serverInstance);

	generate_item_alias_mapping(serverInstance);
	generate_item_tags(serverInstance);

	generate_block_id_to_item_id_map(serverInstance);
	generate_command_arg_types_table(serverInstance);
	generate_note_instruments(serverInstance);
#endif

    generate_item_legacy_id_to_name_mapping(serverInstance);
    generate_item_runtime_id_to_name_mapping(serverInstance);
    generate_item_properties_table(serverInstance);
    generate_block_properties_table2(serverInstance);
    generate_block_materials(serverInstance);
    generate_mob_effects(serverInstance);
    generate_potions(serverInstance);
    generate_enchants(serverInstance);
    generate_block_actor_id_map(serverInstance);
}
