# mapping
This is a BDS (Bedrock Dedicated Server) mod which allows generating mappings and tables of various kinds.

This includes (but is not limited to) the current block palette for a given version, blockstate upgrade tables from previous versions, item upgrade tables, and more besides.

## Usage
### Using bds-modding-devkit
[bds-modding-devkit](https://github.com/pmmp/bds-modding-devkit) is the simplest way to use this mod. See instructions on its README.

### Using the old way
If you don't like or don't want to use bds-modding-devkit, set up a BDS using [modloader-helper](https://github.com/Frago9876543210/modloader-helper).
Once you've done this, clone this repository into the `code` subdirectory, run `helper.sh build mapping`
Then, run the server using the `run.sh` script. The output files will be produced on server start.

## Input files
Input files should be placed in `input_files`. The following files are used by this mod:

| File name | Description |
|:----------|:------------|
| `r12_block_states.json` | JSON table of 1.12 block states as seen [here](https://github.com/pmmp/bds-modding-devkit/blob/122a2737663e2de6732de500586829ed378c0f85/input_files/r12_block_states.json). Used to generate `r12_to_current_block_map.bin`. |
| `old_palettes/*.nbt` | Old blockstate tables in the same format as seen [here](https://github.com/pmmp/BedrockData/blob/master/canonical_block_states.nbt). Used to generate blockstate mapping tables of old blockstates -> current version. |

## Output files
Output files are placed in the `mapping_files` directory.

| File name | Description | Used for | Sample |
|:----------|:------------|:---------|:-------|
| `biome_id_map.json` | Map of biome string ID -> legacy numeric ID | Validating and identifying biomes, network biome encoding | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/biome_id_map.json)
| `block_id_to_item_id_map.json` | Map of block IDs to their corresponding blockitem IDs (usually the same, but occasionally different) | Saving itemstacks in inventories on disk | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/block_id_to_item_id_map.json) |
| `canonical_block_states.nbt` | Ordered list of blockstates required by the chosen version, in varint NBT format. Read TAG_Compounds until the end of file. | Blockstate ID lookups for network, blockstate serializer testing, etc. | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/canonical_block_states.nbt) |
| `block_properties_table.json` | Map of block string ID -> various properties (hardness, light level etc.) | Finding properties for implementing new blocks - sometimes these differ from the Minecraft wiki | N/A |
| `item_tags.json` | Item tags | Crafting recipe validation and network serialization | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/item_tags.json)
| `level_sound_id_map.json` | Map of string LevelSoundEvent name -> numeric ID. **NOTE: This is not complete since the game itself doesn't map all of them to strings.** | Broadcasting network sounds | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/level_sound_id_map.json)
| `old_palette_mappings/*_to_current_block_map.bin` | Map of old version's blockstates -> current version.<br>The files are an array of TAG_Compound, where the 0th, 2nd, 4th etc. indexes are the old NBT, and the 1st, 3rd, 5th etc. indexes are the new NBT. Only the versions provided in `input_files/old_palettes` are generated.<br>NOTE: These files contain mappings even for states that haven't changed.<br>You may wish to post-process them to remove unchanged states and reduce their size. | Generating [schemas](https://github.com/pmmp/BedrockBlockUpgradeSchema) to upgrade old world saves | N/A |
| `particle_id_map.json` | Map of string particle name -> LevelEvent ID (without the 0x4000 flag). | Broadcasting network particles | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/particle_id_map.json) |
| `r12_to_current_block_map.bin` | Map of legacy 1.12 block ID+metadata -> blockstate NBT for the current version. | (PM4 only, non-essential) Mapping legacy internal ID/meta to current version | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/r12_to_current_block_map.bin)
| `r16_to_current_item_map.json` | Map of old item ID+meta -> current version IDs (where changed). | Generating [schemas](https://github.com/pmmp/BedrockItemUpgradeSchema) to upgrade old world saves | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/r16_to_current_item_map.json) |
| `block_state_meta_map.json` | List of meta values for each state ID in `canonical_block_states.nbt` | Serializing network crafting recipes, which still require legacy meta for some reason | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/block_state_meta_map.json) |
| `command_arg_types.json` | List of command symbol names, their descriptions, and network IDs | Serializing command arguments in `AvailableCommandsPacket` | [Go to file](https://github.com/pmmp/BedrockData/blob/33dd83601442b377af42ac91473278243cafd576/command_arg_types.json) |
